
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
 * standalone_support.c                                            *
 *                                                                 *
 * ARC Video Encoder support routines for standalone use           *
 *                                                                 */

#include <standalone_support.h>
#include <arc_encoder.h>
#include <arc_encoder_malloc.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

encoder_import_interface standalone_impif;

static int allocated = 0;

#ifdef TRACK_ALLOCATIONS
static int allocations = 0;
static int releases = 0;
#endif

static MemBlkDesc *descptr;

#define ROUNDUP(x,y) ((x + (y - 1)) & ~(y - 1))

/*----------------------------------------------------------------------------------*/

/* Application-side handler for encoder-sourced allocate request */
s32 standalone_allocate (encoder_interface *this,
			 u32 size,
			 u32 flags,
			 u32 numblocks,
			 MemBlkDesc **dest)
{
  u32 tmpptr;
  u32 aligned;
  u32 newsize;
  u32 i;
  u16 alignment;
  MemBlkDesc *current;

  assert(numblocks <= MAX_BLOCKS);

  if ((alignment = get_alignment(flags)) == 0)
  {
      API_LOG_STDERR("arc_encoder_api_malloc -  unsupported alignment (flags = 0x%08x)\n",flags);
      return ENC_ERR_FAILED;
  }

  if ((flags & ENC_MEM_DC_PAD) && (alignment < ARC_700_CACHE_LINE_LENGTH))
      alignment = ARC_700_CACHE_LINE_LENGTH; // All higher valid alignments are multiples of cache line length

  memset(descptr, 0, (MAX_BLOCKS * sizeof(MemBlkDesc)));
  *dest = descptr;
  current = descptr;

  newsize = size + alignment + ARC_700_CACHE_LINE_LENGTH; // Worst case
  if (flags & ENC_MEM_DC_PAD)
  {
//      newsize = ROUNDUP(newsize, (3*ARC_700_CACHE_LINE_LENGTH));
      newsize += ARC_700_CACHE_LINE_LENGTH;

  }

  for (i = 0; i < numblocks; i++, current++)
  {
      if ((tmpptr = (u32) malloc(newsize)) == 0)
      {
	  API_LOG_STDERR("standalone_allocate - malloc failed, %d bytes flags 0x%08x\n",size,flags);
	  standalone_release(this,dest);
	  *dest = 0;
	  return ENC_ERR_OOM;
      }

      aligned = ROUNDUP(tmpptr,(alignment));
      if (flags & ENC_MEM_OPSTREAM)
      {
	  // Handle alignment and usage tracking by making space for a pointer to the allocated
	  // address and a single byte flag, and aligning up to the next boundary
	  if ((aligned - tmpptr) < 2*sizeof(unsigned int))
	      aligned += alignment;

	  assert(aligned - tmpptr >= 2*sizeof(u32)); // Try not to corrupt the heap
	  assert((aligned %  ARC_700_CACHE_LINE_LENGTH) == 0);
	  assert((tmpptr + newsize) >= (aligned + size));
	  assert(((aligned + size) / ARC_700_CACHE_LINE_LENGTH) != ((tmpptr + newsize) / ARC_700_CACHE_LINE_LENGTH));
      }
      else
      {
	  // handle alignment locally by adjusting pointer to next alignment boundary and storing the allocated address in the 4 bytes below that
	  if (aligned == tmpptr)
	  {
	      aligned += alignment;
	  }
	  assert(aligned - tmpptr >= sizeof(u32)); // Try not to corrupt the heap
	  assert((tmpptr + newsize) >= (aligned + size));
      }

#ifdef CACHE_CONTROL
      if (flags & ENC_MEM_DMA_TGT)
	  invalidate_dcache_lines((u32) aligned, size);
#endif

      SETUP_ENCARGPTR(current, ENC_ARGTYPE_MEMBLKDESC, sizeof(MemBlkDesc));
      current->virt = (void*)aligned;
      current->phys = (void*)aligned;
      ((u32*)aligned)[-1] = tmpptr;
      current->size = size;
      current->flags = flags;
      current->tag = 0;

#ifdef VERBOSE_DEBUG
       API_LOG_STDERR("standalone_allocate - block 0x%08x aligned 0x%08x flags 0x%08x dc offset %d size %d alignment %d allocated size %d actual top 0x%08x effective top 0x%08x desc at 0x%08x \n",
 	      tmpptr,aligned,flags,(aligned % ARC_700_CACHE_LINE_LENGTH), size,alignment,newsize,(tmpptr+newsize),(aligned+size),current);
#endif
#ifdef TRACK_ALLOCATIONS
       allocations++;
#endif
  }

#ifdef TRACK_ALLOCATIONS
       API_LOG_STDERR("standalone_allocate - %d allocations, %d releases\n",allocations,releases);
#endif

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Application-side handler for encoder-sourced release request */
s32 standalone_release (encoder_interface *this,
			MemBlkDesc **block)
{
    u32* tmpptr;

    while (block[0] && (block[0]->virt))
    {
#ifdef VERBOSE_DEBUG
	API_LOG_STDERR("standalone_release - desc at 0x%08x virt 0x%08x phys 0x%08x\n",
		block[0],block[0]->virt,block[0]->phys);
#endif
	tmpptr = (u32*) block[0]->virt;  // Aligned address
	free((void*) (tmpptr[-1]));      // Allocated address
	block++;

#ifdef TRACK_ALLOCATIONS
	releases++;
#endif
    }
#ifdef TRACK_ALLOCATIONS
    API_LOG_STDERR("standalone_release - %d allocations, %d releases\n",allocations,releases);
#endif
    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 standalone_service_attention(encoder_interface *this, u32 cause)
{
    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 standalone_invalidate_dcache_lines(u32 start, u32 length)
{
#ifdef CACHE_CONTROL
    u32 tmpreg, tmpreg2;
    u32 end = start + length;

    if (start % ARC_700_CACHE_LINE_LENGTH)
    {
	API_LOG_STDERR("standalone_invalidate_dcache_lines - start address not on a cache line boundary\n");
	return ENC_ERR_FAILED;
    }

    tmpreg = _lr(DC_CTRL);
    _sr((tmpreg & ~DC_CTRL_IM),DC_CTRL);

    while (start <= end)
    {
	_sr(start,DC_IVDL);
	start += ARC_700_CACHE_LINE_LENGTH;
    }

    _sr(tmpreg,DC_CTRL);

#endif
    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 standalone_flush_dcache_lines(u32 start, u32 length)
{
#ifdef CACHE_CONTROL
    u32 end = start + length;

    if (start % ARC_700_CACHE_LINE_LENGTH)
    {
	API_LOG_STDERR("standalone_flush_dcache_lines - start address not on a cache line boundary 0x%08x\n",start);
	return ENC_ERR_FAILED;
    }

    while (start <= end)
    {
	_sr(start,DC_IVDL);
	start += ARC_700_CACHE_LINE_LENGTH;
    }


#endif
    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/


s32 init_standalone_support()
{
    u32 tmpreg;

    tmpreg = _lr(DC_CTRL);
    API_LOG_STDERR("Data cache %s, DC_CTRL is 0x%08x\n",((tmpreg & DC_CTRL_DC) ? "disabled" : "enabled"),tmpreg);

#ifdef CACHE_CONTROL
//    standalone_flush_dcache();
    _sr((DC_CTRL_IM),DC_CTRL);
    tmpreg = _lr(DC_CTRL);
    API_LOG_STDERR("Data cache %s, DC_CTRL is 0x%08x\n",((tmpreg & DC_CTRL_DC) ? "disabled" : "enabled"),tmpreg);
#endif

  memset(&standalone_impif,0,sizeof(encoder_import_interface));

  standalone_impif.allocate = standalone_allocate;
  standalone_impif.release = standalone_release;
  standalone_impif.attention = standalone_service_attention;

  if ((descptr = (MemBlkDesc*) malloc(MAX_BLOCKS * sizeof(MemBlkDesc))) == (MemBlkDesc*)0)
      return ENC_ERR_FAILED;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 deinit_standalone_support()
{
    free(descptr);
    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/
