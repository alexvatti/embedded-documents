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
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
	#include <string.h>  // For memset
#endif

#include "CMP4Encoder.h"
#include "MP4Warnings.h"
#include "MP4MemoryMonitor.h"
#include "MP4EncoderControl.h"
#include "ArcEEOffsetTables.h"
#include "MP4Tables.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::GetNeighbouringLocationNonMBAFF				*/
/* DESCRIPTION:	This function performs the function in Table 6-3 of the
				standard, and uses the terminology of that table.
				xN and yN are an offset to a pel relative to the top left
				of the current macroblock, given by current_control. The
				function returns the macroblock containing the pel, given
				by neighbouring_control and the vertical offset, yW, of the
				pel relative to the top left of the macroblock it is within.
				maxWH is the width/height of the macroblock, i.e. 16 for
				luma and 8 for chroma.										*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
