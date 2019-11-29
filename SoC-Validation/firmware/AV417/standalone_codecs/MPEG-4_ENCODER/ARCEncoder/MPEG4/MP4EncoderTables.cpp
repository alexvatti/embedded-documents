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


#include "MP4EncoderTables.h"
#include "MP4Tables.h"


const unsigned char h263_chroma_roundtab[16] = {
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
};
const unsigned char h263_deblock_strength[32] = {

	0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 
	7, 8, 8, 8, 9, 9, 9,10,10,10,11,11,11,12,12,12
};
const unsigned char h263_chroma_qscale_table[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 9,10,10,11,11,12,12,12,13,13,13,14,14,14,14,14,15,15,15,15,15
};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_encoder_initialise_difference_tables					*/
/* DESCRIPTION:	These tables are used for calculating the square of the 
				difference between 8 bit pels and the absolute difference 
				between 8 bit pels. they are best used via the macros
				SD_TABLE and ABS_TABLE.
				It should be filled before use by calling 
				h264_encoder_calculate_square_difference_table				*/
/* ------------------------------------------------------------------------ */

/* this matrix based on mathematical manipulation of the DCT and quantiser
	which indicates that for 263 quantisation if SAD < 20*quant, then no coeffs will be > 0
	As specified in Annex I, page 636 of ISO-IEC-14496-2-2004
*/
const unsigned int h263_encoder_8X8_skip_threshold[32] = 
{
	  0,  30,  50,  65,  80, 100, 120, 140, 160, 180, 
    200, 220, 240, 260, 280, 300, 320, 340, 360, 380,
	400, 420, 440, 480, 500, 520, 540, 560, 580, 600,
	620, 640

};
/* this matrix based on mathematical manipulation of the DCT and quantiser
	which indicates that for mpeg4 quantisation if SAD < 16*quant, then no coeffs will be > 0
	As specified in Annex I, page 636 of ISO-IEC-14496-2-2004.

	However I have also found reference to a paper which says that for Mpeg4, if SAD < 24.39QP, then 
	there is 99% confidence that no coeffs will remain and SAD < 36.58QP gives 95% confidence.
	Note that these values are for 8x8 blocks.  We are now testing on 4x4 blocks.
*/
const unsigned int mpeg4_encoder_8X8_skip_threshold[32] = 
{ // mostly *16 - but need more skips at high data rates - so first few are larger than that
	  0,  24,  40,  54,  64,  80,  96, 112, 128, 144, 
    160, 176, 192, 208, 224, 240, 256, 272, 288, 304,
	320, 336, 352, 368, 384, 400, 416, 432, 448, 464,
	480, 496
};

/* ------------------------------------------------------------------------ */
/* NAME:		h263_maximum_mb_bits[32] 						*/
/* DESCRIPTION:	This table contains maximum number of bits expected per
				MB per quantiser, used for packet end point decision										*/
/* ------------------------------------------------------------------------ */

const unsigned short  h263_maximum_mb_bits[32] = 
{ 
//	3200, 3200, 3200, 3200, 3200, 3100, 3000, 2900, 2800, 2700,
//	2600, 2500, 2431, 2378, 2271, 2215, 2102, 2051, 1950, 1883,
//	1750, 1704, 1611, 1558, 1452, 1396, 1284, 1238, 1147, 1109,
//	1032,  991
//	2000, 2000, 1000,  666,  500,  400,  333, 284,  250,  222,
//	 200,  181,  166,  153,  142,  133,  125, 118,  111,  105,
//	 100,   95,   91,   87,   83,   80,   77,  74,   71,   69,
//	  66,   64
//	1600, 1600, 1600, 1600, 1600, 1550, 1500, 1450, 1400, 1350,
//	1300, 1250, 1215, 1189, 1135, 1107, 1051, 1025,  975,  941,
//	 875,  852,  805,  779,  726,  692,  642,  619,  573,  554,
//	 516,  495
/*
 10000,10000,5000,3333,2500,2000,1667,1429,1250,1111,
  1000,  909, 833, 769, 714, 667, 625, 588, 556, 526,
   500,  476, 455, 435, 417, 400, 385, 370, 357, 345,
   333,  323
*/
	3000, 3000, 3000, 2664, 2000, 1600, 1332, 1132, 1000, 888,
	800, 724, 704, 656, 568, 532, 500, 464, 444, 420,
	400, 380,  360,  348,  332,   320, 308, 296,  288,  276,
	264,  256

};

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
