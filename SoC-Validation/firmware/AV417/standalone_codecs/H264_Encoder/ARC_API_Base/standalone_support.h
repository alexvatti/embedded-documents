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
 * standalone_support.h                                            *
 *                                                                 *
 * ARC Video Encoder support routines for standalone use           *
 *                                                                 */


#ifndef STANDALONE_SUPPORT
#define STANDALONE_SUPPORT

#include <arc_encoder.h>

#ifdef __cplusplus
extern "C" 
{
#endif

extern encoder_import_interface standalone_impif;

s32 standalone_allocate (encoder_interface *this,
			 u32 size,
			 u32 flags,
			 u32 numblocks,
			 MemBlkDesc **dest);

s32 standalone_release (encoder_interface *this,
			MemBlkDesc **block);


s32 init_standalone_support();
s32 deinit_standalone_support();

// Data cache definitions

s32 standalone_invalidate_dcache_lines(u32 start,u32 end);
s32 standalone_flush_dcache_lines(u32 start,u32 end);

#ifdef __cplusplus
}
#endif 

#endif
