#include <configs/anusoc.h>
void *my_memcpy(void *,const void *,unsigned int);

void *my_memcpy(void *s1, const void *s2, unsigned int n)
{
char *r1 = s1;
const char *r2 = s2;
__asm__ __volatile__("PUSH {r4-r10}\n"
"LDMloop:"
      "LDMIA r1!, {r3 - r10}\n"
      "STMIA r0!, {r3 - r10}\n"
      "SUBS r2, r2, #32\n"
      "BGE LDMloop\n"
      "POP {r4-r10}\n"
        :
        : "r"(r2),"r"(r1),"r"(n)
        : "memory");
}

