
/*                                                         -*-C-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2007 ARC International (Unpublished)                  *
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
 * constants.h                                                     *
 *                                                                 *
 * ARC Video Encoder API                                           *
 *                                                                 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/*----------------------------------------------------------------------------------*/

#define API_VERSION_MAJOR 0
#define API_VERSION_MINOR 9

#define API_VERSION ((API_VERSION_MAJOR << 16) || API_VERSION_MINOR)

#define MAX_OUTPUT_PACKETS 512

#define MAX_BLOCKS 30 // Maximum number of memory blocks that can be requested/released at one time

#define ARC_700_CACHE_LINE_LENGTH 32

/*----------------------------------------------------------------------------------*/

/* Data cache defines */

#define DC_IVDC 0x47
#define DC_CTRL 0x48
#define DC_IVDL 0x4A
#define DC_FLSH 0x4B
#define DC_FLDL 0x4C

#define DC_CTRL_DC 1
#define DC_CTRL_EB 2
#define DC_CTRL_SB 4
#define DC_CTRL_IM 0x40
#define DC_CTRL_FS 0x100

/*----------------------------------------------------------------------------------*/

#endif
