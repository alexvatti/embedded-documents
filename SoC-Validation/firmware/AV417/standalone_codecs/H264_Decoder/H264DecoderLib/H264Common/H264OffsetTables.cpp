/* ------------------------------------------------------------------------ */
/* FILE:			H264OffsetTables.cpp									*/
/* DESCRIPTION:		H.264 offset tables										*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
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

#include "H264EnumTypes.h"
#include "H264Warnings.h"

#include "H264OffsetTables.h"

/* ------------------------------------------------------------------------ */
/* NAME:		h264_4x4_transmission_to_raster_table						*/
/* DESCRIPTION:	The table for block indices from transmission order into 
				raster order. Note this conversion is self-inverting and so 
				can be used to convert either way between transmission and 
				raster order.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_4x4_transmission_to_raster_table[16] =
{
	 0,  1,  4,  5,
	 2,  3,  6,  7,
	 8,  9, 12, 13,
	10, 11, 14, 15
};


const unsigned short h264_4x4_transmission_to_raster_mask_table[16] =
{
	 1<<0,  1<<1,  1<<4,  1<<5,
	 1<<2,  1<<3,  1<<6,  1<<7,
	 1<<8,  1<<9, 1<<12, 1<<13,
	1<<10, 1<<11, 1<<14, 1<<15
};



/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_luma_4x4_block_bitstream_order_table			*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the top left pel
				of the 4x4 block when blocks are numbered in the order of
				residual coding:

				 0  1  4  5
				 2  3  6  7
				 8  9 12 13
				10 11 14 15
/- ------------------------------------------------------------------------ */

const unsigned char top_left_of_luma_4x4_block_bitstream_order_table[16] =
	{   0,   4,  64,  68, 
		8,  12,  72,  76,
	  128, 132, 192, 196,
	  136, 140, 200, 204};

/* ------------------------------------------------------------------------ */
/* NAME:		top_right_of_luma_4x4_block_raster_order_table				*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the top right
				pel of the 4x4 block.
/- ------------------------------------------------------------------------ */

const unsigned char top_right_of_luma_4x4_block_raster_order_table[16] =
	{   3,   7,  11,  15, 
	   67,  71,  75,  79,
	  131, 135, 139, 143,
	  195, 199, 203, 207};

/* ------------------------------------------------------------------------ */
/* NAME:		bottom_left_of_luma_4x4_block_raster_order_table			*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the bottom left
				pel of the 4x4 block.
/- ------------------------------------------------------------------------ */

const unsigned char bottom_left_of_luma_4x4_block_raster_order_table[16] =
	{  48,  52,  56,  60, 
	  112, 116, 120, 124,
	  176, 180, 184, 188,
	  240, 244, 248, 252};

/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_luma_4x4_block_raster_order_table				*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the top left
				pel of the 4x4 block.
/- ------------------------------------------------------------------------ */

const unsigned char top_left_of_luma_4x4_block_raster_order_table[16] =
	{   0,   4,   8,  12, 
	   64,  68,  72,  76,
	  128, 132, 136, 140,
	  192, 196, 200, 204};

/* ------------------------------------------------------------------------ */
/* NAME:		top_left_of_chroma_4x4_block_table							*/
/* DESCRIPTION:	The table for converting a cheoma block number into an offset
				into a 8x8 block giving the position of the top left pel
				of the 4x4 block.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET top_left_of_chroma_4x4_block_table[8] =
{
	   0,   4,  32,  36,	// U
	   64,  68, 96,  100	// V
};

/* ------------------------------------------------------------------------ */
/* NAME:		frame_mb_zig_zag_luma_dc_table								*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within a 4x4 (dc luma) frame block.
/- ------------------------------------------------------------------------ */

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
				for a 4x4 block within a 16x16 (luma) field macroblock.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET frame_mb_zig_zag_luma_table[16] =
{  
	 0 + 0,  0 + 1, 16 + 0, 32 + 0,  
	16 + 1,  0 + 2,  0 + 3, 16 + 2,
	32 + 1, 48 + 0, 48 + 1, 32 + 2,
	16 + 3, 32 + 3, 48 + 2, 48 + 3
};

/* ------------------------------------------------------------------------ */
/* NAME:		transposed_frame_mb_zig_zag_luma_table						*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within a 16x16 (luma) frame macroblock.
                This fills the 4x4 array in a transposed fashion.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET transposed_frame_mb_zig_zag_luma_table[16] =
{  
	 0 + 0,	16 + 0,  0 + 1,  0 + 2,
	16 + 1, 32 + 0, 48 + 0, 32 + 1,
	16 + 2,  0 + 3, 16 + 3, 32 + 2,
	48 + 1, 48 + 2, 32 + 3, 48 + 3
};



/* ------------------------------------------------------------------------ */
/* NAME:		frame_mb_zig_zag_chroma_table								*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within an 8x8 (chroma) frame macroblock.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET frame_mb_zig_zag_chroma_table[16] =
{  
	 0 + 0,  0 + 1,  8 + 0, 16 + 0,  
	 8 + 1,  0 + 2,  0 + 3,  8 + 2,
	16 + 1, 24 + 0, 24 + 1, 16 + 2,
	 8 + 3, 16 + 3, 24 + 2, 24 + 3
};

/* ------------------------------------------------------------------------ */
/* NAME:		transposed_frame_mb_zig_zag_chroma_table								*/
/* DESCRIPTION:	The scan table for converting total run into pel position
				for a 4x4 block within an 8x8 (chroma) frame macroblock.
/- ------------------------------------------------------------------------ */
const UNSIGNED_SEPTET transposed_frame_mb_zig_zag_chroma_table[16] =
{
	 0 + 0,  8 + 0,  0 + 1,  0 + 2,
	 8 + 1, 16 + 0, 24 + 0, 16 + 1,
	 8 + 2,  0 + 3,  8 + 3, 16 + 2,
	24 + 1, 24 + 2, 16 + 3, 24 + 3
};


/* ------------------------------------------------------------------------ */
/* NAME:		luma_4x4_block_offset_table									*/
/* DESCRIPTION:	The table for converting a luma pel or coefficient number
				into an offset into a 16x16 block.
/- ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET luma_4x4_block_offset_table[16] =
	{   0,   1,   2,   3, 
	   16,  17,  18,  19,
	   32,  33,  34,  35,
	   48,  49,  50,  51};

/* ------------------------------------------------------------------------ */
/* NAME:		chroma_4x4_block_offset_table								*/
/* DESCRIPTION:	The table for converting a chroma pel or coefficient number
				into an offset into a 8x8 block.							*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET chroma_4x4_block_offset_table[16] =
	{   0,   1,   2,   3, 
	    8,   9,  10,  11,
	   16,  17,  18,  19,
	   24,  25,  26,  27};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_8x8_inference_table									*/
/* DESCRIPTION:	The table for converting a raster block index to the raster 
				block index of the block whose motion vector is used in 
				8x8_inference temporal direct mode.							*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_8x8_inference_table[16] =
{
	 0,  0,  3,  3,
	 0,  0,  3,  3,
	12, 12, 15, 15,
	12, 12, 15, 15
};


/* ------------------------------------------------------------------------ */
/* NAME:		h264_frame_zig_zag_position_table							*/
/* DESCRIPTION:	The table for converting decode order into zig zag scan 
				order for frame coded macroblocks							*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_frame_zig_zag_position_table[16] =
{
	0, 1,  4,  8, 
	5, 2,  3,  6,
	9, 12, 13, 10,
	7, 11, 14, 15
};

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
