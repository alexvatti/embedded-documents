#ifndef _ARC_MEM_H_
#define _ARC_MEM_H_

#define ARC_CACHED_MEMORY_MINOR 250
#define ARC_UNCACHED_MEMORY_MINOR 251

#define GET_PHYS 1
#define GET_LEN 2
#define INVALIDATE_REGION 3

typedef struct  {
    	void *start;
	int len;
} invalidate_region_t;

#endif

