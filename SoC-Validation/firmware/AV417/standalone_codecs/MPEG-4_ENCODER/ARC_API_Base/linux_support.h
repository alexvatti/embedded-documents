
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
 * linux_support.h                                                 *
 *                                                                 *
 * ARC Video Encoder support routines for ARC Linux                *
 *                                                                 */


#ifndef LINUX_SUPPORT_H
#define LINUX_SUPPORT_H

#include <arc_encoder.h>

extern encoder_import_interface linux_impif;

s32 arc_linux_allocate (encoder_interface *this,
			u32 size,
			u32 flags,
			u32 numblocks,
			MemBlkDesc **dest);

s32 arc_linux_release (encoder_interface *this,
		       MemBlkDesc **blocks);


s32 init_linux_support();
s32 deinit_linux_support();

unsigned int num_framebufs_linux();

#endif
