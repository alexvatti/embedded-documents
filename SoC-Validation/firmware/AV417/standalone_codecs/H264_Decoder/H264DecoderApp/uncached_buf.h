#ifndef _UNCACHED_BUF_H_
#define _UNCACHED_BUF_H_
extern void *baseVirtualAddress;
extern void *basePhysicalAddress;
extern int baseVirtualSize;

char *uncached_bitstream_alloc(void);

void uncached_bitstream_free(char *p);

#endif
