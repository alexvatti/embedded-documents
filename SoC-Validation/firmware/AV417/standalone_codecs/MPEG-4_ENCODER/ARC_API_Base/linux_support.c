
/*                                                         -*-C-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2006-2007 ARC International (Unpublished)             *
 * All Rights Reserved.                                            *
 *                                                                 *
 * This document, material and/or software contains confidential   *
 * and proprietary information of ARC International and is         *
 * protected by copyright, trade secret and other state, federal,  *
 * and international laws, and may be embodied in patents issued   *
 * or pending.  Its receipt or possession does not convey any      *
 * rights to use, reproduce, disclose its contents, or to          *
 * manufacture, or sell anything it may describe.  Reverse         *
 * engineering is prohibited, and reproduction, disclosure or use  *
 * without specific written authorization of ARC International is  *
 * strictly forbidden.  ARC and the ARC logotype are trademarks of *
 * ARC International.                                              *
 *                                                                 *
 *                                                                 *
 * linux_support.c                                                 *
 *                                                                 *
 * ARC Video Encoder support routines for ARC Linux                *
 *                                                                 */


/*----------------------------------------------------------------------------------*/

#include <sys/mman.h>
#include <sys/errno.h>
#include <stdio.h>
#include <assert.h>

#include <linux_support.h>
#include <salloc.h>
#include <arc_encoder_malloc.h>
#include <asm-arc/arc_mem.h>
#include <asm-arc/arc_pgu.h>

/*----------------------------------------------------------------------------------*/

encoder_import_interface linux_impif;

/* Uncached memory device */

static u32 baseVirtualAddress = 0;
static u32 basePhysicalAddress = 0;
static u32 baseVirtualSize = 0;

#define VIRT_TO_PHYS(x) ((x - baseVirtualAddress) + basePhysicalAddress)
#define PHYS_TO_VIRT(x) ((x - basePhysicalAddress) + baseVirtualAddress)

/* Framebuffer memory */

#define NUM_FRAMES 2
static MemBlkDesc frameDesc[NUM_FRAMES];
static int fbCount;
static char fbAllocated[NUM_FRAMES];

static MemBlkDesc *descptr;

/*----------------------------------------------------------------------------------*/

unsigned int num_framebufs_linux()
{
    return NUM_FRAMES;
}

/*----------------------------------------------------------------------------------*/

s32 addmemory_linux(MemBlkDesc* memblkdesc)
{
    if (fbCount >= NUM_FRAMES)
	return ENC_ERR_FAILED;


    API_LOG_STDERR("Adding block %d at 0x%08x\n",fbCount,memblkdesc->virt);
    memcpy(&frameDesc[fbCount],memblkdesc,sizeof(MemBlkDesc));
    fbCount++;

    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Currently (ARC Linux 2.4) we use the uncached memory device to access uncached memory.
   This creates a region of memory at boot time, which is marked as uncached in the MMU
   page descriptors when it's mmapped. The driver only holds one region and, although
   multiple clients may access its memory, no protocol for inter-client coordination is 
   provided. So what we get here is all we're going to get. */

static s32 init_uncached_memory()
{
  // FIXME uncached/DMA allocation only
  const char *dev = "/dev/mem_uncached";
  int fd = open(dev, 0666);
  MemBlkDesc *descptr;

  if (basePhysicalAddress)
  {
    API_LOG("%s called twice!\n",__FUNCTION__);
    return ENC_ERR_OOM;
  }

  if (fd < 0)
    goto error;

  if (ioctl(fd,  GET_LEN, &baseVirtualSize))
    goto error;
    
  if (ioctl(fd, GET_PHYS, &basePhysicalAddress))
    goto error;
  
  baseVirtualAddress =  (u32) mmap(0, baseVirtualSize, PROT_READ|PROT_WRITE,  MAP_SHARED, fd, 0);
  if ((int)baseVirtualAddress == -1)
    goto error;

  close(fd);

  salloc_init((void*)baseVirtualAddress,baseVirtualSize);

  return ENC_ERR_NONE;

error:
    baseVirtualAddress = 0;
    basePhysicalAddress = 0;
    close(fd);
    API_LOG_STDERR( "Failed to map uncached input buffer\n");
    if (errno == ENOTTY)
	API_LOG_STDERR( "Unsupported kernel operation\n");
    return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/

static s32 deinit_uncached_memory()
{
  munmap((void*) baseVirtualAddress, baseVirtualSize);
  baseVirtualAddress = 0;
  basePhysicalAddress = 0;
  salloc_uninit();
  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static s32 arc_linux_release_uncached  (encoder_interface *this,
					MemBlkDesc **block)
{
  u32* tmpptr;

  assert(baseVirtualAddress != 0);

  while (block[0] && (block[0]->virt))
  {
      tmpptr = (u32*) block[0]->virt; // This is the aligned address
      sfree((void*)tmpptr[-1]);       // Free the original pointer
      block++;
  }

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static s32 arc_linux_allocate_uncached (encoder_interface *this,
					u32 size,
					u32 flags,
					u32 numblocks,
					MemBlkDesc **dest)
{
  MemBlkDesc *current;
  u32 ptr;
  u32 aligned;
  u32 newsize;
  u16 alignment;
  u32 i;

  assert(baseVirtualAddress != 0);
  assert(numblocks <= MAX_BLOCKS);

  memset(descptr, 0, (MAX_BLOCKS * sizeof(MemBlkDesc)));
  *dest = descptr;
  current = descptr;

  API_LOG_STDERR("Allocating %d from uncached\n",size);
  if ((alignment = get_alignment(flags)) == 0)
  {
      API_LOG_STDERR("arc_encoder_api_malloc -  unsupported alignment (flags = 0x%08x)\n",flags);
      return ENC_ERR_FAILED;
  }

  if (flags & ENC_MEM_OPSTREAM)
      if (alignment < 2*sizeof(unsigned int))
	  alignment += alignment;

  newsize = size + alignment;
  alignment--;

  for (i = 0; i < numblocks; i++, current++)
  {
      if (ptr = (u32) salloc(newsize))
      {
	  SETUP_ENCARGPTR(current, ENC_ARGTYPE_MEMBLKDESC, sizeof(MemBlkDesc));
	  // handle alignment locally by adjusting pointer to next alignment boundary 
          // and storing the allocated address in the 4 bytes below that. So
          // have to overallocate even for 4 byte alignment.
	  aligned = ((ptr + alignment) & ~alignment);
	  if (flags & ENC_MEM_OPSTREAM)
	  {
	      while ((aligned - ptr) < 2*sizeof(unsigned int))
		  aligned += alignment + 1;
	      assert(aligned - ptr >= 2*sizeof(u32)); // Try not to corrupt the heap
	  }
	  else
	  {
	      if (aligned == ptr)
	      {
		  aligned += alignment + 1;
	      }
	      assert(aligned - ptr >= sizeof(u32)); // Try not to corrupt the heap
	  }

	  current->virt = (void*) aligned;
	  current->phys = (void*) VIRT_TO_PHYS(aligned);
	  *((u32*)(aligned - sizeof(u32))) = ptr; 
	  current->size = size;
	  current->flags = flags;
	  current->tag = 0;
      }
      else
      {
	  API_LOG_STDERR("arc_linux_allocate_uncached failed\n");
	  arc_linux_release_uncached(this,dest);
	  *dest = 0;
	  return ENC_ERR_OOM;
      }
  }

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static s32 arc_linux_release_framebuf  (encoder_interface *this,
					MemBlkDesc **block)
{
    int i;

    while (block[0] && (block[0]->virt))
    {
	for (i = 0; i < NUM_FRAMES; i++)
	{
	    if (frameDesc[i].virt == block[0]->virt)
	    {
		fbAllocated[i] = 0;
	    }
	}
	block++;
    }

    return ENC_ERR_NONE;    
}

/*----------------------------------------------------------------------------------*/

static s32 arc_linux_allocate_framebuf (encoder_interface *this,
					u32 size,
					u32 flags,
					u32 numblocks,
					MemBlkDesc **dest)
{
    int i,j;
    int res;

// FIXME assuming that the framebuffer alignment has already been correctly set,
// so not looking at alignment part of flags. Ought to at least check this.

    assert(numblocks <= NUM_FRAMES);
    assert(numblocks <= MAX_BLOCKS);

    *dest = descptr;
    
    for (j = 0; j < numblocks; j++, descptr++)
    {
	for (i = 0; (i < NUM_FRAMES) && (fbAllocated[i]); i++);
	if (i >= NUM_FRAMES)
	{
	    descptr = 0;
	    arc_linux_release_framebuf(this,dest);
	    *dest = 0;
	    return ENC_ERR_OOM;
	}
	
	SETUP_ENCARGPTR(descptr, ENC_ARGTYPE_MEMBLKDESC, sizeof(MemBlkDesc));

	memcpy(descptr,&frameDesc[i],sizeof(MemBlkDesc)); // since the returned descriptor is always freed
	
	fbAllocated[i] = 1;
    }

//    API_LOG_STDERR("arc_linux_allocate_framebuf 0x%08x\n",descptr->virt);
    return ENC_ERR_NONE;    
}

/*----------------------------------------------------------------------------------*/

/* Main application-side handler for encoder-sourced allocate request */

s32 arc_linux_allocate (encoder_interface *this,
			u32 size,
			u32 flags,
			u32 numblocks,
			MemBlkDesc **dest)
{
    u32 memtype = flags & ENC_MEM_TYPEMASK;
 
    switch(memtype)
    {
      case ENC_MEM_INPUTYUV:
      case ENC_MEM_OPSTREAM:
	  return arc_linux_allocate_uncached(this,size,flags,numblocks,dest);

      case ENC_MEM_RECONYUV:
	  return arc_linux_allocate_framebuf(this,size,flags,numblocks,dest);

      default:
	  break;
    }
    return ENC_ERR_NOFUNC;
}

/*----------------------------------------------------------------------------------*/

s32 arc_linux_release  (encoder_interface *this,
			MemBlkDesc **block)
{
    u32 memtype;

    memtype = block[0]->flags & ENC_MEM_TYPEMASK;

    switch(memtype)
    {
      case ENC_MEM_INPUTYUV:
      case ENC_MEM_OPSTREAM:
	  return arc_linux_release_uncached(this, block);

      case ENC_MEM_RECONYUV:
	  return arc_linux_release_framebuf(this, block);

      default:
	  break;
    }

    return ENC_ERR_NOFUNC;
}

/*----------------------------------------------------------------------------------*/

s32 arc_linux_service_attention(encoder_interface *this, u32 cause)
{
    switch(cause)
    {
	case ARC_ENC_ATTN_RECON:
	    // Get frame number
	    // Switch to it
	    break;

        default:
	    break;
    }

    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 init_linux_support()
{
  init_uncached_memory();

  memset(&frameDesc,0,NUM_FRAMES*sizeof(MemBlkDesc));
  memset(&fbAllocated,0,NUM_FRAMES*sizeof(char));
  memset(&linux_impif,0,sizeof(encoder_import_interface));
  fbCount = 0;

  linux_impif.allocate = arc_linux_allocate;
  linux_impif.release = arc_linux_release;
  linux_impif.attention = arc_linux_service_attention;

  if ((descptr = malloc(MAX_BLOCKS*sizeof(MemBlkDesc))) == (MemBlkDesc*)0)
    return ENC_ERR_FAILED;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 deinit_linux_support()
{
  deinit_uncached_memory();
 
  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

unsigned virt_to_phys(unsigned x)
{
    return VIRT_TO_PHYS(x);
}

/*----------------------------------------------------------------------------------*/
