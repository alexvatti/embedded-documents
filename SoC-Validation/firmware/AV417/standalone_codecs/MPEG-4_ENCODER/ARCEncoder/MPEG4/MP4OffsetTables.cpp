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

#include "ArcEEOffsetTables.h"

// JRMM all these arrays are not relevant to mpeg4/h263, but are needed to compile the ArcEntropyEncoderModel.cpp
/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_luma_4x4_block_bitstream_order_table			*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the top left pel
				of the 4x4 block when blocks are numbered in the order of
				residual coding:

				 0  1  4  5
				 2  3  6  7
				 8  9 12 13
				10 11 14 15													*/
/* ------------------------------------------------------------------------ */

const unsigned char top_left_of_luma_4x4_block_bitstream_order_table[16] =
	{   0,   4,  64,  68, 
		8,  12,  72,  76,
	  128, 132, 192, 196,
	  136, 140, 200, 204};


/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_luma_4x4_block_raster_order_table				*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the top left
				pel of the 4x4 block.										*/
/* ------------------------------------------------------------------------ */

const unsigned char top_left_of_luma_4x4_block_raster_order_table[16] =
	{   0,   4,   8,  12, 
	   64,  68,  72,  76,
	  128, 132, 136, 140,
	  192, 196, 200, 204};

/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_chroma_4x4_block_table							*/
/* DESCRIPTION:	The table for converting a cheoma block number into an offset
				into a 8x8 block giving the position of the top left pel
				of the 4x4 block.											*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET top_left_of_chroma_4x4_block_table[4] =
	{   0,   4,  32,  36};

/* ------------------------------------------------------------------------ */
/* NAME:		frame_mb_zig_zag_luma_dc_table								*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within a 4x4 (dc luma) frame block.			*/
/*- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET frame_mb_zig_zag_luma_dc_table[16] =
{  
	 0 + 0,  0 + 1,  4 + 0,  8 + 0,  
	 4 + 1,  0 + 2,  0 + 3,  4 + 2,
	 8 + 1, 12 + 0, 12 + 1,  8 + 2,
	 4 + 3,  8 + 3, 12 + 2, 12 + 3
};


/* ------------------------------------------------------------------------ */
/* NAME:		frame_mb_zig_zag_luma_table									*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within a 16x16 (luma) field macroblock.		*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET frame_mb_zig_zag_luma_table[16] =
{  
	 0 + 0,  0 + 1, 16 + 0, 32 + 0,  
	16 + 1,  0 + 2,  0 + 3, 16 + 2,
	32 + 1, 48 + 0, 48 + 1, 32 + 2,
	16 + 3, 32 + 3, 48 + 2, 48 + 3
};


/* ------------------------------------------------------------------------ */
/* NAME:		frame_mb_zig_zag_chroma_table								*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within an 8x8 (chroma) frame macroblock.	*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET frame_mb_zig_zag_chroma_table[16] =
{  
	 0 + 0,  0 + 1,  8 + 0, 16 + 0,  
	 8 + 1,  0 + 2,  0 + 3,  8 + 2,
	16 + 1, 24 + 0, 24 + 1, 16 + 2,
	 8 + 3, 16 + 3, 24 + 2, 24 + 3
};







/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
