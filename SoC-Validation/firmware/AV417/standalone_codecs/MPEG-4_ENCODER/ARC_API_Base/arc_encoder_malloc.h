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
 * arc_encoder_malloc.h                                            *
 *                                                                 *
 * ARC Video Encoder API memory allocation wrapper.                *
 *                                                                 */

#ifndef ARC_ENCODER_MALLOC_H
#define ARC_ENCODER_MALLOC_H

#ifdef CACHE_CONTROL
#include <standalone_support.h>
#endif

/*----------------------------------------------------------------------------------*/

/* Memory block descriptor flags */


#define ENC_MEM_TYPEMASK  0x00000fff
#define ENC_MEM_OPSTREAM  1  /* Output stream destination */
#define ENC_MEM_RECONYUV  2  /* Recon YUV */
#define ENC_MEM_INPUTYUV  3  /* Input YUV */
#define ENC_MEM_SDM       4  /* SIMD data memory */

#define ENC_MEM_ALIGNMASK 0x0000f000
#define ENC_MEM_ALIGNSHIFT 12
#define ENC_MEM_ALIGN_4   0x00001000  // default - word aligned
#define ENC_MEM_ALIGN_8   0x00002000
#define ENC_MEM_ALIGN_16  0x00003000
#define ENC_MEM_ALIGN_32  0x00004000
#define ENC_MEM_ALIGN_64  0x00005000
#define ENC_MEM_ALIGN_128 0x00006000

#define ENC_MEM_INFOMASK  0xffff0000
#define ENC_MEM_FB_YUV    0x00010000
#define ENC_MEM_DC_PAD    0x00100000 // Pad the buffer at both ends to ensure it
                                     // has sole occupancy of all its cache lines
#define ENC_MEM_DMA_TGT   0x00200000 
#define ENC_MEM_DMA_SRC   0x00400000 


/*----------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif 

void* arc_encoder_api_malloc (unsigned long,           /* size */
			      unsigned long,           /* flags */
			      unsigned char**,         /* physical address */
			      unsigned long*);         /* tag */
void  arc_encoder_api_free   (void*,                   /* base of region */
			      unsigned long,           /* flags */
			      unsigned long*);         /* tag */

#ifdef CACHE_CONTROL
 #ifdef __ARC_LINUX__
 #error "Data cache management code enabled for Linux build - result will not work"
 #endif
 // Invalidate entire cache, flushing dirty lines
 #define flush_dcache() _sr(0x1, 0x47)
 // Invalidate region without flushing
 #define invalidate_dcache_lines(start,length) standalone_invalidate_dcache_lines(start,length)
 // Selectively flush
 #define flush_dcache_lines(start,length) standalone_flush_dcache_lines(start,length)
#else
 #define flush_dcache()
 #define invalidate_dcache_lines(start,length)
 #define flush_dcache_lines(start,length)
#endif

#ifdef __cplusplus
}
#endif 

/*----------------------------------------------------------------------------------*/

#endif
