/*
 * Moschip Semiconductor Technology Limited
 *
 */

#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code("codesec5")
#endif

#include "uncached_buf.h"
#include "arc_mem.h"
#include <stdio.h>
#include <stdlib.h>

void *baseVirtualAddress;
void *basePhysicalAddress = 0;
int baseVirtualSize = 1024*1024;

#if defined __ARC_LINUX__
#include <sys/mman.h>
#include <sys/errno.h>


char *uncached_bitstream_alloc(void)
{
	const char *dev = "/dev/mem_uncached";
	void *vaddr;
	int fd = open(dev, 0666);

	if (basePhysicalAddress)
	{
		printf("uncached_bitstream_alloc called twice!\n");
		abort();
	}

	if (fd < 0)
		goto error;

	if (ioctl(fd,  GET_LEN, &baseVirtualSize))
		goto error;

	if (ioctl(fd, GET_PHYS, &basePhysicalAddress))
		goto error;

	vaddr =  mmap(0, baseVirtualSize, PROT_READ|PROT_WRITE,  MAP_SHARED , fd, 0);
	close(fd);
	if ((int)vaddr == -1)
		goto error;

	baseVirtualAddress = vaddr;

	return vaddr;

error:
	perror(dev);
	fprintf(stderr, "Failed to map uncached input buffer\n");
	if (errno == ENOTTY)
		fprintf(stderr, "Please upgrade to a newer version of the linux Kernel\n");
	exit(1);
}

void uncached_bitstream_free(char *p)
{
	munmap(p, baseVirtualSize);
	baseVirtualAddress = NULL;
}

#else

char *uncached_bitstream_alloc(void)
{
	baseVirtualAddress = malloc(baseVirtualSize);
	if (!baseVirtualAddress)
	{
		perror("Failed to allocate bitstream input buffer.");
		exit(1);
	}
	return baseVirtualAddress;
}

void uncached_bitstream_free(char *p)
{
	free(p);
	baseVirtualAddress = NULL;
}

#endif
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code()
#endif
