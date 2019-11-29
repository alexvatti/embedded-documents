/* ------------------------------------------------------------------------ */
/* FILE:			H264Tables.cpp											*/
/* DESCRIPTION:		H.264 variable length code tables for decoder			*/
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


#include "H264Tables.h"


/* ------------------------------------------------------------------------ */
/* NAME:		h264_block_offset_table										*/
/* DESCRIPTION:	The table for mapping h264_block_shape_type_t and block 
				number to index of top left 4x4 block of the block.
				This applies to macroblocks (16x16, 16x8 and 8x16) and to
				8x8 sub-partitions (8x8, 8x4, 4x8 and 4x4). 
				In the latter case to get the actual block index, the index 
				of the top left 4x4 block of the partition must be added to 
				the value from this table.									*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_offset_table[7][4] =
{
	{ 0, 0, 0, 0},	// H264_16X16
	{ 0, 8, 0, 0},	// H264_16X8
	{ 0, 2, 0, 0},	// H264_8X16
	{ 0, 0, 0, 0},	// H264_8X8
	{ 0, 4, 0, 0},	// H264_8X4
	{ 0, 1, 0, 0},	// H264_4X8
	{ 0, 1, 4, 5}	// H264_4X4
};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_block_width_table										*/
/* DESCRIPTION:	The table for mapping h264_block_shape_type_t to the 
				luminance width in pels of the sub-block.					*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_width_table[7] =
{
	16,		// H264_16X16
	16,		// H264_16X8
	 8,		// H264_8X16
	 8,		// H264_8X8
	 8,		// H264_8X4
	 4,		// H264_4X8
	 4,		// H264_4X4
};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_block_height_table										*/
/* DESCRIPTION:	The table for mapping h264_block_shape_type_t to the 
				luminance height in pels of the sub-block.					*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_height_table[7] =
{
	16,		// H264_16X16
	 8,		// H264_16X8
	16,		// H264_8X16
	 8,		// H264_8X8
	 4,		// H264_8X4
	 8,		// H264_4X8
	 4,		// H264_4X4
};



/* ------------------------------------------------------------------------ */
/* NAME:		h264_8x8_block_index_to_raster_index_table					*/
/* DESCRIPTION:	This is used to map an 8x8 block index (0..3) to a raster 
				block index (0..15), for the top left 4x4 block of the 8x8 
				block.														*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_8x8_block_index_to_raster_index_table[4] =
{
	0, 2,
	8, 10
};

// reverse lookup of h264_8x8_block_index_to_raster_index_table
// which maps 0->0, 1->2, 2->8, 3->10
const int reverse_h264_8x8_block_index_to_raster_index_table[16] =
{
   0, 0, 1, 1,
   0, 0, 1, 1,
   2, 2, 3, 3,
   2, 2, 3, 3
};




/* ------------------------------------------------------------------------ */
/* NAME:		h264 Annex A level tables									*/
/* DESCRIPTION:	This table contains the (profile and) level limits.			*/
/* ------------------------------------------------------------------------ */

const int annex_a_level_table[NUMBER_OF_ANNEX_A_LEVELS][10] =
{
//	10*level  MB/s   MBs       DPB   Bit Rate        CPB    Vert MV    Log2  Min     Max MV
//                         (bytes)     (bit/s)    (bits)  (QP luma) Vert MV Comp  per 2 MBs
	{  10,   1485,    99,   152064,     64000,    175000,      256,       8,   2,         64},
	{  11,   3000,   396,   345600,    192000,    500000,      512,       9,   2,         64},
	{  12,   6000,   396,   912384,    384000,   1000000,      512,       9,   2,         64},
	{  13,  11880,   396,   912384,    768000,   2000000,      512,       9,   2,         64},
	{  20,  11880,   396,   912384,   2000000,   2000000,      512,       9,   2,         64},
	{  21,  19800,   792,  1824768,   4000000,   4000000,     1024,      10,   2,         64},
	{  22,  20250,  1620,  3110400,   4000000,   4000000,     1024,      10,   2,         64},
	{  30,  40500,  1620,  3110400,  10000000,  10000000,     1024,      10,   2,         32},
	{  31, 108000,  3600,  6912000,  14000000,  14000000,     2048,      11,   4,         16},
	{  32, 216000,  5120,  7864320,  20000000,  20000000,     2048,      11,   4,         16},
	{  40, 245760,  8192, 12582912,  20000000,  25000000,     2048,      11,   4,         16},
	{  41, 245760,  8192, 12582912,  50000000,  62500000,     2048,      11,   2,         16},
	{  42, 491520,  8192, 12582912,  50000000,  62500000,     2048,      11,   2,         16},
	{  50, 589824, 22080, 42301440, 135000000, 135000000,     2048,      11,   2,         16},
	{  51, 983040, 36864, 70778880, 240000000, 240000000,     2048,      11,   2,         16},
};



/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
