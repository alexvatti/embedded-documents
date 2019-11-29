/* Simple malloc clone
 * amnon June-06
 * 
 * This allocator is used to manage the allocation of the uncached memory
 * region used by VLC DMA assist.
 */
#include "salloc.h"
#include <string.h>
#include <stdio.h>

static unsigned *free_list;
static unsigned total_size;

static void *trim(void *addr, int len);
static void _sfree(void *addr);

#ifndef API_LOG
#define API_LOG(...)
#endif

#undef NULL
#define NULL 0
#define LEN(p) ((long *)p)[-1]

#ifdef TRACE_SALLOC
/* define me if you want allocation tracing */

typedef struct tr_t {
    struct tr_t *next;
    void *addr;
    int len;
    char *file;
    int lineno;
    int alloced_size;
    int resize_size;
    int seq_no;
} tr_t;

static struct tr_t *active = NULL;
static int seq_counter = 0;

static void tr_add(void *addr, int len)
{
    tr_t *t = malloc(sizeof(tr_t));

    memset(t, 0, sizeof(tr_t));
    t->next = active;
    t->addr = addr;
    t->len = len;
    t->alloced_size = len;
    t->seq_no = seq_counter++;
    active = t;
}

static void tr_del(void *addr)
{
    tr_t **p, *tmp;

    for (p=&active; *p && (*p)->addr != addr; p = &((*p)->next));

    if (!*p)
    {
	API_LOG("attempt to free %p, which was never allocated!\n", addr);
	return;
    }

    tmp = *p;
    *p = tmp->next;
    free(tmp);
}

static void tr_change(void *addr, int new_len)
{
    tr_t *p;

    for (p = active; p && p->addr != addr; p=p->next);

    if (!p)
	return;
    p->len = new_len;
}

static void print_contents(unsigned char *p)
{
    int i;
    for (i=0; i<4; i++)
	API_LOG("%02x ", p[i]);
}

static void tr_print_free_list(void)
{
    unsigned *p;
    unsigned free_size = 0;

    API_LOG("Total size = %d\n", total_size);

    for (p = free_list; p; p = (void *)*p)
	API_LOG("%p len %ld\n", p, LEN(p));
    API_LOG("\n");
}

static void tr_print(void)
{
    tr_t *p;
    int s = 0;

    API_LOG("active\n");
    for (p = active; p; p=p->next)
    {
	API_LOG("%s:%d %p, len=%d A %d R %d seq %d", p->file, p->lineno, p->addr, p->len,
	p->alloced_size, p->resize_size, p->seq_no);
	print_contents(p->addr);
	API_LOG("\n");

	s += p->len;
    }
    API_LOG("Total active %d\n", s);
    tr_print_free_list();
}

#else

/* dummy stubs */
static void tr_add(void *addr, int len) {}
static void tr_del(void *addr) {}
static void tr_change(void *addr, int new_len) {}
static void tr_print(void) {}
#endif


void salloc_init(void *mem, int len)
{
    unsigned *p = mem;

    /* put all memory on free list */
    p++;
    LEN(p) = len;
    _sfree(p);

    total_size = len;
}

void salloc_uninit(void)
{
    free_list = NULL;
}

static void out_of_mem(int len)
{
    API_LOG("out uncached memory (tried to alloc %d bytes)\n", len);
    API_LOG("Re-compile Kernel with larger unchached memory.\n");
    API_LOG("Current uncached size = %d bytes\n", total_size);
    tr_print();
//    exit(1);
}

static int roundup(int n)
{
    return (n + 3) & ~3;
}

static unsigned *best_fit(int len)
{
    unsigned *p, *best_p = NULL;
    unsigned min_len = ~0;

    for (p = free_list; p; p = (void*)*p)
    {
	if (LEN(p) < len)
	    continue;
	if (LEN(p) >= min_len)
	    continue;
	best_p = p;
	min_len = LEN(p);
    }
    return best_p;
}

void *salloc(int len)
{
    unsigned **p;
    unsigned *ret;

    len += 4; // XXX workaround for buffer overrun somewhere in MPEG4 codec
    len = roundup(len);

    ret = best_fit(len);
    if (!ret) {
	out_of_mem(len);
	return 0;
    }

    for (p = &free_list; *p != ret; p = (void*)*p);
    *p = (void *)*ret; /* remove from free list */

    tr_add(ret, LEN(ret));
    trim(ret, len);
    return ret;
}

static int is_next_to(void *left, void *right)
{
    return (char *)left + LEN(left) + sizeof(unsigned) == right;
}

static void *join(void *left, void *right)
{
    LEN(left) += LEN(right) + sizeof(unsigned);
    return left;
}

static void _sfree(void *addr)
{
    unsigned **p, *next;

    /* try to merge with neigbouring blocks */
    for (p = &free_list; *p; p = (void *)next)
    {
	next = *p;
	if (is_next_to(*p, addr))
	{
	    *p = (void *)*next;
	    *next = NULL;
	    addr = join(next, addr);
	}
	else if (is_next_to(addr, *p))
	{
	    *p = (void *)*next;
	    *next = NULL;
	    addr = join(addr, next);
	}
    }

    /* insert head of list */
    *(unsigned *)addr = (unsigned)free_list;
    free_list = addr;
}

void sfree(void *addr)
{
    tr_del(addr);
    _sfree(addr);
}

static void *trim(void *addr, int len)
{
    int old_len = LEN(addr);
    unsigned *tail = (unsigned *)((char *)addr + len + sizeof(unsigned));

    if (old_len < len + 64)
	return addr;
    tr_change(addr, len);
    LEN(addr) = len;
    LEN(tail) = old_len - len - sizeof(unsigned);
    _sfree(tail);
    return addr;
}

void *srealloc(void *addr, int len)
{
    void *rc;
    unsigned **p;

    len = roundup(len);
    if (LEN(addr) > len)
	return trim(addr, len);

    /* find right neighbour */
    for (p = &free_list; *p && !is_next_to(addr, *p); p = (void *)*p);

    if (*p && LEN(*p) + LEN(addr) + sizeof(unsigned) >= len)
    {
	unsigned *next = *p;
	*p = (void *)*next;
	*next = NULL;
	rc = join(addr, next);
	return trim(rc, len);
    }

    /* no luck -- just free and re-allocate */
    rc = salloc(len);
    memcpy(rc, addr, LEN(addr));
    sfree(addr);
    return rc;
}

