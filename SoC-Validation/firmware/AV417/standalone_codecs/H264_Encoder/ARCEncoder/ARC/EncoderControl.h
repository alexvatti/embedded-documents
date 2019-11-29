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
/* NAME:        h264_encoder_vector_code_length_table                       */
/*          h263_encoder_vector_code_length_table (also used by MPEG4)  */
/* DESCRIPTION: The tables for converting differential motion vectors
            into the number of bits in the codeword. This is not exact 
            and should only be used for rate distortion optimised 
            mode decisions, and should never be used for counting bits 
            that might be generated.
            A lookup table is used for inputs in the range -127 to 128.
            Code lengths for vectors in the range -127 to 127 inclusive
            are correct, all others are set to 15, which is less than
            the correct value.												*/
/* ------------------------------------------------------------------------ */

const unsigned int h263_encoder_signed_code_length_table[256] =
/*{ // orignal version of h263/mpeg4 that was first copied from h264 and then modified
   15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
   13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
   13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,
   11,11,11,11,11,10,10,10,
    8, 8, 8, 7,
    5, 4,
    3,
    1,
    3,
    4, 5,
    7, 8, 8, 8,
   10,10,10,11,11,11,11,11,
   11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,13,
   13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};*/
{ // modify version of h263/mpg4. takes into account vector differences being able to wrap outside the range [-16, 15.5]
   15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    3, 4, 5, 7, 8, 8, 8,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,13,13, // idx=33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63, 0 // start of line h.263/mpeg4(non-short header) limit
   13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11, // idx= 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
   11,11,11,11,11,10,10,10, // idx=17,18,19,20,21,22,23,24
    8, 8, 8, 7, // idx=25,26,27,28
    5, 4, // idx=29,30
    3, // idx=31
    1, // idx=32
    3, // idx=33
    4, 5, // idx=34,35
    7, 8, 8, 8, // idx=36,37,38,39
   10,10,10,11,11,11,11,11, // idx=40,41,42,43,44,45,46,47
   11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,13, // idx=48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
   13,13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10, 8, 8, 8, 7, 5, 4, 3, // idx= 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 // h.263/mpeg4(non-short header) limit
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
   15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};

#define SIZE_OF_MSB_TABLE_MINUS_ONE 31

const unsigned int h264_encoder_signed_code_msb_table[SIZE_OF_MSB_TABLE_MINUS_ONE+1] =
{
    0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

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

/* ------------------------------------------------------------------------ */
/* NAME:             reverse_h264_8x8_block_index_to_raster_index_table              */
/* DESCRIPTION:   reverse lookup of h264_8x8_block_index_to_raster_index_table
                  which maps 0->0, 1->2, 2->8, 3->10                        */
/* ------------------------------------------------------------------------ */

const int reverse_h264_8x8_block_index_to_raster_index_table[16] =
{
   0, 0, 1, 1,
   0, 0, 1, 1,
   2, 2, 3, 3,
   2, 2, 3, 3
};

/* ------------------------------------------------------------------------ */
/* NAME:          h264_8x8_block_index_to_raster_index_table                    */
/* DESCRIPTION:   This is used to map an 8x8 block index (0..3) to a raster 
                  block index (0..15), for the top left 4x4 block of the 8x8 
                  block.                                                        */
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_8x8_block_index_to_raster_index_table[4] =
{
   0, 2,
   8, 10
};


/* ------------------------------------------------------------------------ */
/* NAME:        h264_block_offset_table                                     */
/* DESCRIPTION: The table for mapping h264_block_shape_type_t and block 
            number to index of top left 4x4 block of the block.
            This applies to macroblocks (16x16, 16x8 and 8x16) and to
            8x8 sub-partitions (8x8, 8x4, 4x8 and 4x4). 
            In the latter case to get the actual block index, the index 
            of the top left 4x4 block of the partition must be added to 
            the value from this table.                                  */
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_offset_table[7][4] =
{
   { 0, 0, 0, 0},   // H264_16X16
   { 0, 8, 0, 0},   // H264_16X8
   { 0, 2, 0, 0},   // H264_8X16
   { 0, 0, 0, 0},   // H264_8X8
   { 0, 4, 0, 0},   // H264_8X4
   { 0, 1, 0, 0},   // H264_4X8
   { 0, 1, 4, 5}    // H264_4X4
};

/* ------------------------------------------------------------------------ */
/* NAME:          h264_block_width_table                                        */
/* DESCRIPTION:   The table for mapping h264_block_shape_type_t to the 
            luminance width in pels of the sub-block.                   */
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_width_table[7] =
{
   16,      // H264_16X16
   16,      // H264_16X8
    8,      // H264_8X16
    8,      // H264_8X8
    8,      // H264_8X4
    4,      // H264_4X8
    4,      // H264_4X4
};

/* ------------------------------------------------------------------------ */
/* NAME:        h264_block_height_table                                     */
/* DESCRIPTION: The table for mapping h264_block_shape_type_t to the 
            luminance height in pels of the sub-block.                  */
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_block_height_table[7] =
{
   16,      // H264_16X16
    8,      // H264_16X8
   16,      // H264_8X16
    8,      // H264_8X8
    4,      // H264_8X4
    8,      // H264_4X8
    4,      // H264_4X4
};

/* ------------------------------------------------------------------------ */
/* NAME:        top_left_of_luma_4x4_block_raster_order_table               */
/* DESCRIPTION: The table for converting a luma block number into an offset
            into a 16x16 block giving the position of the top left
            pel of the 4x4 block.
/- ------------------------------------------------------------------------ */

const unsigned char top_left_of_luma_4x4_block_raster_order_table[16] =
   {   0,   4,   8,  12, 
      64,  68,  72,  76,
     128, 132, 136, 140,
     192, 196, 200, 204};

