/* ------------------------------------------------------------------------ */
/* FILE:			H264OffsetTables.h										*/
/* DESCRIPTION:		BT H.264 offset tables header file.						*/
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

#ifndef _H264_OFFSET_TABLES_H_
#define _H264_OFFSET_TABLES_H_

/* ------------------------------------------------------------------------ */
/* DECLARATIONS																*/
/* ------------------------------------------------------------------------ */

extern const UNSIGNED_SEPTET h264_4x4_transmission_to_raster_table[16];

extern const unsigned char top_left_of_luma_4x4_block_bitstream_order_table[16];

//extern const unsigned char bottom_left_of_luma_4x4_block_raster_order_table[16];

extern const unsigned char top_left_of_luma_4x4_block_raster_order_table[16];

extern const unsigned char top_right_of_luma_4x4_block_raster_order_table[16];

extern const UNSIGNED_SEPTET top_left_of_chroma_4x4_block_table[4];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_luma_dc_table[16];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_luma_table[16];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_chroma_table[16];

extern const UNSIGNED_SEPTET luma_4x4_block_offset_table[16];

//extern const UNSIGNED_SEPTET chroma_4x4_block_offset_table[16];

//extern const UNSIGNED_SEPTET h264_8x8_inference_table[16];

//extern const UNSIGNED_SEPTET h264_frame_zig_zag_position_table[16];

extern unsigned int h264_encoder_vector_code_length(int mvd);


#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
