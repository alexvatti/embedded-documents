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


#ifndef _H264_OFFSET_TABLES_H_
#define _H264_OFFSET_TABLES_H_

/* ------------------------------------------------------------------------ */
/* DECLARATIONS																*/
/* ------------------------------------------------------------------------ */

#include "EnumTypes.h"
extern const UNSIGNED_SEPTET h264_4x4_transmission_to_raster_table[16];

extern const unsigned char top_left_of_luma_4x4_block_bitstream_order_table[16];

extern const unsigned char bottom_left_of_luma_4x4_block_raster_order_table[16];

extern const unsigned char top_left_of_luma_4x4_block_raster_order_table[16];

extern const unsigned char top_right_of_luma_4x4_block_raster_order_table[16];

extern const UNSIGNED_SEPTET top_left_of_chroma_4x4_block_table[4];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_luma_dc_table[16];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_luma_table[16];

extern const UNSIGNED_SEPTET frame_mb_zig_zag_chroma_table[16];

extern const UNSIGNED_SEPTET luma_4x4_block_offset_table[16];

extern const UNSIGNED_SEPTET chroma_4x4_block_offset_table[16];

extern const UNSIGNED_SEPTET h264_8x8_inference_table[16];

extern const UNSIGNED_SEPTET h264_frame_zig_zag_position_table[16];

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
