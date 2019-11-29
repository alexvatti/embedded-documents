/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
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

/* May contain code (c) British Telecommunications plc 2005        */

#include "ArcEEtables.h"
#include "EnumTypes.h"
#include "defines.h"
#include "stdlib.h"
#include <memory.h>


/* ------------------------------------------------------------------------ */
/* NAME:		h264_4x4_transmission_to_raster_table						*/
/* DESCRIPTION:	The table for block indices from transmission order into 
				raster order. Note this conversion is self-inverting and so 
				can be used to convert either way between transmission and 
				raster order.												*/
/* ------------------------------------------------------------------------ */

extern const UNSIGNED_SEPTET h264_4x4_transmission_to_raster_table[16] =
{
	 0,  1,  4,  5,
	 2,  3,  6,  7,
	 8,  9, 12, 13,
	10, 11, 14, 15
};


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
