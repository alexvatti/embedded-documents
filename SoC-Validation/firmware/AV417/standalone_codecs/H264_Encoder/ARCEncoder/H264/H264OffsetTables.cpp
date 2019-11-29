/* ------------------------------------------------------------------------ */
/* FILE:			H264OffsetTables.cpp									*/
/* DESCRIPTION:		H.264 offset tables										*/
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

#include "EnumTypes.h"

#include "H264OffsetTables.h"

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

///* ------------------------------------------------------------------------ */
///* NAME:		top_right_of_luma_4x4_block_raster_order_table				*/
///* DESCRIPTION:	The table for converting a luma block number into an offset
//				into a 16x16 block giving the position of the top right
//				pel of the 4x4 block.
///- ------------------------------------------------------------------------ */
//
//const unsigned char top_right_of_luma_4x4_block_raster_order_table[16] =
//	{   3,   7,  11,  15, 
//	   67,  71,  75,  79,
//	  131, 135, 139, 143,
//	  195, 199, 203, 207};

/* ------------------------------------------------------------------------ */
/* NAME:		bottom_left_of_luma_4x4_block_raster_order_table			*/
/* DESCRIPTION:	The table for converting a luma block number into an offset
				into a 16x16 block giving the position of the bottom left
				pel of the 4x4 block.
/- ------------------------------------------------------------------------ */
/*
const unsigned char bottom_left_of_luma_4x4_block_raster_order_table[16] =
	{  48,  52,  56,  60, 
	  112, 116, 120, 124,
	  176, 180, 184, 188,
	  240, 244, 248, 252};
*/
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

const UNSIGNED_SEPTET top_left_of_chroma_4x4_block_table[4] =
	{   0,   4,  32,  36};

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
/*
const UNSIGNED_SEPTET chroma_4x4_block_offset_table[16] =
	{   0,   1,   2,   3, 
	    8,   9,  10,  11,
	   16,  17,  18,  19,
	   24,  25,  26,  27};
*/
/* ------------------------------------------------------------------------ */
/* NAME:		h264_8x8_inference_table									*/
/* DESCRIPTION:	The table for converting a raster block index to the raster 
				block index of the block whose motion vector is used in 
				8x8_inference temporal direct mode.							*/
/* ------------------------------------------------------------------------ */
/*
const UNSIGNED_SEPTET h264_8x8_inference_table[16] =
{
	 0,  0,  3,  3,
	 0,  0,  3,  3,
	12, 12, 15, 15,
	12, 12, 15, 15
};*/


/* ------------------------------------------------------------------------ */
/* NAME:		h264_frame_zig_zag_position_table							*/
/* DESCRIPTION:	The table for converting decode order into zig zag scan 
				order for frame coded macroblocks							*/
/* ------------------------------------------------------------------------ */
/*
const UNSIGNED_SEPTET h264_frame_zig_zag_position_table[16] =
{
	0, 1,  4,  8, 
	5, 2,  3,  6,
	9, 12, 13, 10,
	7, 11, 14, 15
};
*/
/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* NAME:		h264_encoder_vector_code_length								*/
/* DESCRIPTION:	The table for converting differential motion vectors
				into the number of bits in the codeword. This is not exact
				and should only be used for rate distortion optimised
				mode decisions, and should never be used for counting bits
				that might be generated.
				A lookup table is used for inputs in the range -127 to 128.
				Code lengths for vectors in the range -127 to 127 inclusive
				are correct, all others are set to 15, which is less than
				the correct value.											*/
/* ------------------------------------------------------------------------ */

const unsigned int h264_encoder_signed_code_length_table[256] =
{
	15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
	 9, 9, 9, 9, 9, 9, 9, 9,
	 7, 7, 7, 7,
	 5, 5,
	 3,
	 1,
	 3,
	 5, 5,
	 7, 7, 7, 7,
	 9, 9, 9, 9, 9, 9, 9, 9,
	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};

#define SIZE_OF_MSB_TABLE_MINUS_ONE 31

const unsigned int h264_encoder_signed_code_msb_table[SIZE_OF_MSB_TABLE_MINUS_ONE+1] =
{
	 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

unsigned int h264_encoder_vector_code_length(int mvd)
{
	return h264_encoder_signed_code_length_table[(mvd+128) & 255]   |
		   h264_encoder_signed_code_msb_table[((mvd+128) >> 8) & SIZE_OF_MSB_TABLE_MINUS_ONE];
}

