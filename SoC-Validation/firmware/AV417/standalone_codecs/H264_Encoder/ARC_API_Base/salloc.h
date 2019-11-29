
#ifndef _SALLOC_H_
#define _SALLOC_H_

void salloc_init(void *mem, int len);
void salloc_uninit(void);

void *salloc(int len);

void sfree(void *mem);

void *srealloc(void *mem, int len);

#endif
