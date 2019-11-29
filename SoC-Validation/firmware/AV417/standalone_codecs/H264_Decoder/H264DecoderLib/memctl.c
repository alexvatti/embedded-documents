/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#ifdef __GNUC__
#include <sys/mman.h>
#endif

#include "arc_codecs.h"
#include "memctl.h"
#include "vbuf.h"

#ifdef __ARC_LINUX__
#include "arc_pgu.h"
static struct yuv_info yuv;
#endif

//local static data
static void *base_address = (void *)0;				//the (virtual) base address of the buffer
static void *current_address = (void *)0;			//the current end (+1) address allocated in the buffer
/*static*/ int numBuffers = 0;
static unsigned long *virtualBufferAddresses = NULL;
static unsigned long *physicalBufferAddresses = NULL;
static int nextFreeBuffer = 0;

extern arc_codec_functions h264_functions;

// If a shared buffer is implemented, then initialise the pointer tables to the buffers
// which are supplied by the host application
//
void h264_init_memctl (unsigned long *vAddresses,unsigned long *pAddresses)
{
  virtualBufferAddresses = vAddresses;
  physicalBufferAddresses = pAddresses;
}

// Called from the codec when it knows the size of the image being decoded
//
void h264_mem_setsize (unsigned long hsize, unsigned long vsize, int num_buffers)
{
    static unsigned long hSize = 0;
    static unsigned long vSize = 0;
    extern int n_yuv_buffers;
    numBuffers = n_yuv_buffers;
#if !defined __ARC_BTW__
    //callback to set the sizes in the owning application
    if ((h264_functions.set_sizes != (fp_set_sizes)0) && (hSize == 0))
	h264_functions.set_sizes(hsize,vsize,numBuffers);
#endif
    hSize = hsize;
    vSize = vsize;
}

// Allocate memory from the H264 buffer
// If not called before, then initialise the memory buffer
//
void *h264_mem_alloc (int size, int align)
{
  void *ret_address = (void *)0;
  int available_size = 0;
#if !defined __ARC_LINUX__ || defined __ARC_SHARED_BUFFER__
  int idx = vbuf_alloc();
  if (numBuffers == 0)
  {
    fprintf(stderr,"\nMem Controller not initialised");
    return (NULL);
  }
  ret_address = (void *)(virtualBufferAddresses[idx]);
  //memset(ret_address,0,size); //TODO - remove?
#elif defined __ARC_LINUX__
  //check to see if the memory allocation has been initialised
  if (base_address == NULL)
  {
    char *fb_name = "/dev/fb0";
    //initialise it by opening /dev/mem and mapping the H264 physical memory to it
    int fd = open(fb_name,O_RDWR|O_SYNC);
    if (fd < 0)
    {
      perror(fb_name);
      return (NULL);
    }
    ioctl(fd, ARCPGUFB_GETYUV, &yuv);
    fprintf(stderr,"\nADRS = 0x%.8X SIZE= 0x%.8X", yuv.phys_base, yuv.yuv_size);
    base_address = mmap(0, yuv.yuv_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    if (base_address == (void *)-1)
    {
      fprintf(stderr,"\nFailed on mmap [%s]",strerror(errno));
      return (NULL);
    }
    current_address = base_address;
  }
  //allocate space from the buffer, noting alignment
  ret_address = (void *)((((int)current_address + (align - 1)) / align) * align);
  available_size = (yuv.phys_base + yuv.yuv_size) - (int)ret_address;
  if ((available_size <= 0) || (available_size < size))
  {
    fprintf(stderr,"\nH264 Allocation failure, asked for %u bytes, only %u bytes available",size,available_size);
    return (NULL);
  }
  current_address = ret_address + size;
#endif
  return (ret_address);
}


//  Return the mapped virtual address to physical address translation
//
//  0xFFFFFFF is returned on an error
unsigned long h264_get_physical_adrs (unsigned long virtualAdd)
{
  unsigned long physical;
#if defined __ARC_SHARED_BUFFER__
  int i;
  //check for initialisation
  if (numBuffers == 0)
  {
    fprintf(stderr,"\nMem Controller not initialised");
    return (0xFFFFFFFF);
  }
  for (i=0; i<(numBuffers-1); i++)
  {
    //check the address is valid
     if (virtualAdd < virtualBufferAddresses[i])
     {
       fprintf(stderr,"\nInvalid virtual address for translation - 0x%x",virtualAdd);
       //fprintf(stderr,"\nInvalid virtual address for translation");
      return (0xFFFFFFFF);
     }
     if (virtualAdd < virtualBufferAddresses[i+1])
     {
       //fprintf(stderr,"\nV: 0x%.8X, P: 0x%.8X",virtual,physicalBufferAddresses[i] + (virtualAdd - virtualBufferAddresses[i]));
       return (physicalBufferAddresses[i] + (virtualAdd - virtualBufferAddresses[i]));
     }
  }
  //if we reach here the the virtual address is greater than the last entry
  //fprintf(stderr,"\nV: 0x%.8X, P: 0x%.8X",virtualAdd,physicalBufferAddresses[i] + (virtualAdd - virtualBufferAddresses[i]));
  physical = physicalBufferAddresses[i] + (virtualAdd - virtualBufferAddresses[i]);
#elif defined __ARC_LINUX__
  if ((virtualAdd < (unsigned long)base_address) || (virtualAdd >= ((unsigned long)base_address + yuv.yuv_size)))
  {
    fprintf(stderr,"\nRequest to translate memory outside H264 buffer");
    return (0xFFFFFFFF);
  }
  physical = (virtualAdd - (unsigned long)base_address) + yuv.phys_base;
#else
	physical = virtualAdd;
#endif
  return (physical);
}

void h264_get_frame_index (unsigned long baseAddress,int *index)
{
#if defined __ARC_SHARED_BUFFER__
  int i;
  for (i=0; i<numBuffers; i++)
  {
    if (virtualBufferAddresses[i] == baseAddress)
    {
      *index = i;
      //fprintf(stderr,"\nBaseAddress 0x%.8X, index %u",baseAddress,i);
      return;
    }
  }
  *index = -1;
#endif
  return;
}


//  Return the mapped physical address to virtual address translation
//
//  0xFFFFFFF is returned on an error
unsigned long h264_get_virtual_adrs (unsigned long physical)
{
  int i;
  unsigned long virt;
#if defined __ARC_SHARED_BUFFER__
  for (i=0; i<numBuffers;i++)
  {
    if ((physical >= physicalBufferAddresses[i]) && (physical < physicalBufferAddresses[i+1]))
    {
      virt = physical - physicalBufferAddresses[i] + virtualBufferAddresses[i];
      return (virt);
    }
  }
  //otherwise not found
  return (0xFFFFFFFF);
#elif defined __ARC_LINUX__
  if ((physical < (unsigned long)yuv.phys_base) || (physical >= ((unsigned long)yuv.phys_base + yuv.yuv_size)))
  {
    fprintf(stderr,"\nRequest to translate memory outside H264 buffer");
    return (0xFFFFFFFF);
  }
  virt = (physical - (unsigned long)yuv.phys_base) + (unsigned long)base_address;
  return (virt);
#else
  return physical;
#endif
}

// Clean up all allocations, if done
//
void h264_mem_freeall (void *data)
{
#if defined __ARC_SHARED_BUFFER__
  //nothing to do
#elif defined __ARC_LINUX__
  if (base_address != NULL)
  {
    munmap(base_address,yuv.yuv_size);
    base_address = NULL;
  }
#else
  //nothing to do
#endif
}

