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


/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderTables.cpp									*/
/* DESCRIPTION:		H.264 code tables for the encoder						*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */

#include "H264EncoderTables.h"
#include "H264Tables.h"
#include "defines.h"




/* ------------------------------------------------------------------------ */
/* NAME:		h264_maximum_mb_bits[52] 						*/
/* DESCRIPTION:	This table contains maximum number of bits expected per
				MB per quantiser, used for packet end point decision										*/
/* ------------------------------------------------------------------------ */

const unsigned short h264_maximum_mb_bits[52] = 
{ 
	(3200 >> 2), (3200 >> 2), (3200 >> 2), (3200 >> 2), (3200 >> 2), (3100 >> 2), (3000 >> 2), (2900 >> 2), (2800 >> 2), (2700 >> 2),
	(2600 >> 2), (2500 >> 2), (2431 >> 2), (2378 >> 2), (2271 >> 2), (2215 >> 2), (2102 >> 2), (2051 >> 2), (1950 >> 2), (1883 >> 2),
	(1750 >> 2), (1704 >> 2), (1611 >> 2), (1558 >> 2), (1452 >> 2), (1396 >> 2), (1284 >> 2), (1238 >> 2), (1147 >> 2), (1109 >> 2),
	(1032 >> 2), (991 >> 2),  (908 >> 2),  (869 >> 2),  (791 >> 2),  (743 >> 2),  (648 >> 2),  (614 >> 2),  (546 >> 2),  (524 >> 2),
	(480 >> 2),  (456 >> 2),  (408 >> 2),  (375 >> 2),  (360 >> 2),  (350 >> 2),  (340 >> 2),  (330 >> 2),  (320 >> 2),  (310 >> 2),
	(300 >> 2),  (290 >> 2)

};


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
