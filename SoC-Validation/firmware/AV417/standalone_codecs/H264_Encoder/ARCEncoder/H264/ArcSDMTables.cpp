/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "ArcMPC.h"
#include "ArcSDMTables.h"
#include "EnumTypes.h"
#include "ArcEEtables.h"
#include "ArcEEOffsetTables.h"
#include "VRaptorInfo.h"
#include "ArcChannelRoutines.h"
#include "H264LoopFilter.h"

#include <ctype.h>
#include <arc/arc_simd.h>
#include "ArcSDMTables_MPA.h"
#include "ArcDeblock_MPA.h"


/* ------------------------------------------------------------------------ */
/* NAME:		h264 loop filter tables										*/
/* DESCRIPTION:	These tables are used in the loop filter.
				No loop filtering is done when index is zero. Ignoring 
				filter offsets, no filtering is done when QP < 16, so 
				the tables are only defined for QP 15..51, with all values 
				of QP < 16 using the QP 15 entries. The index into the 
				tables should therefore be related to QP-15.
				The clip table has no data for strength 0 (as not needed) 
				and none for strength 4 (as same as strength 3). 
				The index into the table should therefore be strength-1		*/
/* ------------------------------------------------------------------------ */

// joint table
#define CoStrongeBetaAlpha(alpha,beta,c0) c0,0,beta,alpha

const unsigned char Init_Copy_AlphaBetaCoTable[3][37][4] =
{
// strength 1
	{
	  CoStrongeBetaAlpha(0,		0, 		0),
	  CoStrongeBetaAlpha(4,		2, 		0), 
	  CoStrongeBetaAlpha(4,		2, 		0),
	  CoStrongeBetaAlpha(5,		2, 		0),
	  CoStrongeBetaAlpha(6,		3, 		0),
	  CoStrongeBetaAlpha(7,		3, 		0),
	  CoStrongeBetaAlpha(8,		3, 		0),
	  CoStrongeBetaAlpha(9,		3, 		0),
	  CoStrongeBetaAlpha(10,	4, 		1),
	  CoStrongeBetaAlpha(12,	4, 		1),
	  CoStrongeBetaAlpha(13,	4, 		1),
	  CoStrongeBetaAlpha(15,	6, 		1),
	  CoStrongeBetaAlpha(17,	6, 		1),
	  CoStrongeBetaAlpha(20,	7, 		1),
	  CoStrongeBetaAlpha(22,	7, 		1),
	  CoStrongeBetaAlpha(25,	8, 		1),
	  CoStrongeBetaAlpha(28,	8, 		1),
	  CoStrongeBetaAlpha(32,	9, 		1),
	  CoStrongeBetaAlpha(36,	9, 		2),
	  CoStrongeBetaAlpha(40,	10,		2),
	  CoStrongeBetaAlpha(45,	10,		2),
	  CoStrongeBetaAlpha(50,	11,		2),
	  CoStrongeBetaAlpha(56,	11,		3),
	  CoStrongeBetaAlpha(63,	12,		3),
	  CoStrongeBetaAlpha(71,	12,		3),
	  CoStrongeBetaAlpha(80,	13,		4),
	  CoStrongeBetaAlpha(90,	13,		4),
	  CoStrongeBetaAlpha(101,	14,		4),
	  CoStrongeBetaAlpha(113,	14,		5),
	  CoStrongeBetaAlpha(127,	15,		6),
	  CoStrongeBetaAlpha(144,	15,		6),
	  CoStrongeBetaAlpha(162,	16,		7),
	  CoStrongeBetaAlpha(182,	16,		8),
	  CoStrongeBetaAlpha(203,	17,		9),
	  CoStrongeBetaAlpha(226,	17,		10),
	  CoStrongeBetaAlpha(255,	18,		11),
	  CoStrongeBetaAlpha(255,	18,		13)
	},
// strength 2
	{
	  CoStrongeBetaAlpha(0,		0, 		0),
	  CoStrongeBetaAlpha(4,		2, 		0),
	  CoStrongeBetaAlpha(4,		2, 		0),
	  CoStrongeBetaAlpha(5,		2, 		0),
	  CoStrongeBetaAlpha(6,		3, 		0),
	  CoStrongeBetaAlpha(7,		3, 		0),
	  CoStrongeBetaAlpha(8,		3, 		1),
	  CoStrongeBetaAlpha(9,		3, 		1),
	  CoStrongeBetaAlpha(10,	4, 		1),
	  CoStrongeBetaAlpha(12,	4, 		1),
	  CoStrongeBetaAlpha(13,	4, 		1),
	  CoStrongeBetaAlpha(15,	6, 		1),
	  CoStrongeBetaAlpha(17,	6, 		1),
	  CoStrongeBetaAlpha(20,	7, 		1),
	  CoStrongeBetaAlpha(22,	7, 		1),
	  CoStrongeBetaAlpha(25,	8, 		1),
	  CoStrongeBetaAlpha(28,	8, 		2),
	  CoStrongeBetaAlpha(32,	9, 		2),
	  CoStrongeBetaAlpha(36,	9, 		2),
	  CoStrongeBetaAlpha(40,	10,		2),
	  CoStrongeBetaAlpha(45,	10,		3),
	  CoStrongeBetaAlpha(50,	11,		3),
	  CoStrongeBetaAlpha(56,	11,		3),
	  CoStrongeBetaAlpha(63,	12,		4),
	  CoStrongeBetaAlpha(71,	12,		4),
	  CoStrongeBetaAlpha(80,	13,		5),
	  CoStrongeBetaAlpha(90,	13,		5),
	  CoStrongeBetaAlpha(101,	14,		6),
	  CoStrongeBetaAlpha(113,	14,		7),
	  CoStrongeBetaAlpha(127,	15,		8),
	  CoStrongeBetaAlpha(144,	15,		8),
	  CoStrongeBetaAlpha(162,	16,		10),
	  CoStrongeBetaAlpha(182,	16,		11),
	  CoStrongeBetaAlpha(203,	17,		12),
	  CoStrongeBetaAlpha(226,	17,		13),
	  CoStrongeBetaAlpha(255,	18,		15),
	  CoStrongeBetaAlpha(255,	18,		17)
	},
// strength 3
	{
	  CoStrongeBetaAlpha(0,		0, 		0),
	  CoStrongeBetaAlpha(4,		2, 		0),
	  CoStrongeBetaAlpha(4,		2, 		1),
	  CoStrongeBetaAlpha(5,		2, 		1),
	  CoStrongeBetaAlpha(6,		3, 		1),
	  CoStrongeBetaAlpha(7,		3, 		1),
	  CoStrongeBetaAlpha(8,		3, 		1),
	  CoStrongeBetaAlpha(9,		3, 		1),
	  CoStrongeBetaAlpha(10,	4, 		1),
	  CoStrongeBetaAlpha(12,	4, 		1),
	  CoStrongeBetaAlpha(13,	4, 		1),
	  CoStrongeBetaAlpha(15,	6, 		1),
	  CoStrongeBetaAlpha(17,	6, 		2),
	  CoStrongeBetaAlpha(20,	7, 		2),
	  CoStrongeBetaAlpha(22,	7, 		2),
	  CoStrongeBetaAlpha(25,	8, 		2),
	  CoStrongeBetaAlpha(28,	8, 		3),
	  CoStrongeBetaAlpha(32,	9, 		3),
	  CoStrongeBetaAlpha(36,	9, 		3),
	  CoStrongeBetaAlpha(40,	10,		4),
	  CoStrongeBetaAlpha(45,	10,		4),
	  CoStrongeBetaAlpha(50,	11,		4),
	  CoStrongeBetaAlpha(56,	11,		5),
	  CoStrongeBetaAlpha(63,	12,		6),
	  CoStrongeBetaAlpha(71,	12,		6),
	  CoStrongeBetaAlpha(80,	13,		7),
	  CoStrongeBetaAlpha(90,	13,		8),
	  CoStrongeBetaAlpha(101,	14,		9),
	  CoStrongeBetaAlpha(113,	14,		10),
	  CoStrongeBetaAlpha(127,	15,		11),
	  CoStrongeBetaAlpha(144,	15,		13),
	  CoStrongeBetaAlpha(162,	16,		14),
	  CoStrongeBetaAlpha(182,	16,		16),
	  CoStrongeBetaAlpha(203,	17,		18),
	  CoStrongeBetaAlpha(226,	17,		20),
	  CoStrongeBetaAlpha(255,	18,		23),
	  CoStrongeBetaAlpha(255,	18,		25)
	}

};




/* ------------------------------------------------------------------------ */
/* NAME:		h264_inverse_quantiser_table								*/
/* DESCRIPTION:	The table used for performing the inverse quantisation.
				The value depends on the quantiser modulo 6, as well as the 
				position within the 4x4 block. As this is always used with 
				an index that is the total run, the tables are given in
				scan order rather than raster order: there is one table for
				frame scan order and one for field scan order.				*/
/* ------------------------------------------------------------------------ */
/* not wanted on voyage
const char Init_Copy_h264_frame_scan_inverse_quantiser_table[6][16] =
{
//    0   1   4   8   5   2   3   6   9  12  13  10   7  11  14  15   Raster index
	{10, 13, 13, 10, 16, 10, 13, 13, 13, 13, 16, 10, 16, 13, 13, 16},
	{11, 14, 14, 11, 18, 11, 14, 14, 14, 14, 18, 11, 18, 14, 14, 18},
	{13, 16, 16, 13, 20, 13, 16, 16, 16, 16, 20, 13, 20, 16, 16, 20},
	{14, 18, 18, 14, 23, 14, 18, 18, 18, 18, 23, 14, 23, 18, 18, 23},
	{16, 20, 20, 16, 25, 16, 20, 20, 20, 20, 25, 16, 25, 20, 20, 25},
	{18, 23, 23, 18, 29, 18, 23, 23, 23, 23, 29, 18, 29, 23, 23, 29}
};*/
const short Init_Copy_h264_raster_scan_inverse_quantiser_table[6][16] =
{
//    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,   Raster index
	{10, 13, 10, 13, 13, 16, 13, 16, 10, 13, 10, 13, 13, 16, 13, 16},
	{11, 14, 11, 14, 14, 18, 14, 18, 11, 14, 11, 14, 14, 18, 14, 18},
	{13, 16, 13, 16, 16, 20, 16, 20, 13, 16, 13, 16, 16, 20, 16, 20},
	{14, 18, 14, 18, 18, 23, 18, 23, 14, 18, 14, 18, 18, 23, 18, 23},
	{16, 20, 16, 20, 20, 25, 20, 25, 16, 20, 16, 20, 20, 25, 20, 25},
	{18, 23, 18, 23, 23, 29, 23, 29, 18, 23, 18, 23, 23, 29, 23, 29}
};
/* ------------------------------------------------------------------------ */
/* NAME:		h264_quantiser_table										*/
/* DESCRIPTION:	The table used for performing the forward quantisation.
				The value depends on the quantiser modulo 6, as well as the 
				position within the 4x4 block. As this is always used with 
				an index that is the total run, the tables are given in
				scan order rather than raster order: there is one table for
				frame scan order and one for field scan order.
				Note this is used in the decoder for SP picture decoding.	*/
/* ------------------------------------------------------------------------ */
/* not wanted on voyage
const short Init_Copy_h264_frame_scan_quantiser_table[6][16] =
{
//       0      1      4      8      5      2      3      6      9     12     13     10      7     11     14     15   Raster index
	{13107,  8066,  8066, 13107,  5243, 13107,  8066,  8066,  8066,  8066,  5243, 13107,  5243,  8066,  8066,  5243},
	{11916,  7490,  7490, 11916,  4660, 11916,  7490,  7490,  7490,  7490,  4660, 11916,  4660,  7490,  7490,  4660},
	{10082,  6554,  6554, 10082,  4194, 10082,  6554,  6554,  6554,  6554,  4194, 10082,  4194,  6554,  6554,  4194},
	{ 9362,  5825,  5825,  9362,  3647,  9362,  5825,  5825,  5825,  5825,  3647,  9362,  3647,  5825,  5825,  3647},
	{ 8192,  5243,  5243,  8192,  3353,  8192,  5243,  5243,  5243,  5243,  3353,  8192,  3353,  5243,  5243,  3353},
	{ 7282,  4559,  4559,  7282,  2893,  7282,  4559,  4559,  4559,  4559,  2893,  7282,  2893,  4559,  4559,  2893}
};*/

const short Init_Copy_h264_raster_scan_quantiser_table[6][16] =
{
//       0      1       2      3     4       5      6      7      8      9       10     11   12     13     14     15   Raster index
	{13107,  8066,  13107,  8066,  8066,   5243,  8066,   5243, 13107,  8066,  13107, 8066, 8066,  5243,   8066,  5243},
	{11916,  7490,  11916,  7490,  7490,   4660,  7490,   4660, 11916,  7490,  11916, 7490, 7490,  4660,   7490,  4660},
	{10082,  6554,  10082,  6554,  6554,   4194,  6554,   4194, 10082,  6554,  10082, 6554, 6554,  4194,   6554,  4194},
	{ 9362,  5825,   9362,  5825,  5825,   3647,  5825,   3647,  9362,  5825,   9362, 5825, 5825,  3647,   5825,  3647},
	{ 8192,  5243,   8192,  5243,  5243,   3353,  5243,   3353,  8192,  5243,   8192, 5243, 5243,  3353,   5243,  3353},
	{ 7282,  4559,   7282,  4559,  4559,   2893,  4559,   2893,  7282,  4559,   7282, 4559, 4559,  2893,   4559,  2893}
};					

const short TopCoeffCountSBBitPos[8]  = {32,40,48,56,64,72,80,88};  // positions for blocks 12,13,14,15,18,19,22,23
const short CurrCoeffCountSBBitPos[8] = {64,69,74,79,96,101,116,121};

// offsets into PixelCoeffBuffer of circ buffer, for each 8x8 mpeg4/h263 block
/*unsigned short MP4RawBlockOffset[6] = // when raw pixel data
{
	PCB_CURRENT_Y + 0,
	PCB_CURRENT_Y + (PCB_CURRENT_STRIDE/2),
	PCB_CURRENT_Y + 8 * PCB_CURRENT_STRIDE,
	PCB_CURRENT_Y + 8 * PCB_CURRENT_STRIDE + (PCB_CURRENT_STRIDE/2),
	PCB_CURRENT_U,
	PCB_CURRENT_V
};
unsigned short MP4DiffBlockOffset[6] = // when difference predicted data and result of DCT
{
	PCB_RESIDUAL_Y + 0,
	PCB_RESIDUAL_Y + (PCB_RESIDUAL_STRIDE/2),
	PCB_RESIDUAL_Y + 8 * PCB_RESIDUAL_STRIDE,
	PCB_RESIDUAL_Y + 8 * PCB_RESIDUAL_STRIDE + (PCB_RESIDUAL_STRIDE/2),
	PCB_RESIDUAL_U,
	PCB_RESIDUAL_V
};
*/
//-----------------------------------------------------------------------------------------------------------


/* ------------------------------------------------------------------------ */
/* NAME:		h264_quantiser_chroma_mapping_table							*/
/* DESCRIPTION:	The table used to map quantiser indices to quantisers to be 
				used for chroma.
				The index is QPluma, and the table output is QPchroma, 
				thus allowing for QPluma in the range 0..51 
				and QPchroma in the range 0..39.							*/
/* ------------------------------------------------------------------------ */
// Now with padding for clipping.
// Access with index + 16
const UNSIGNED_SEPTET Init_Copy_h264_quantiser_chroma_mapping_tableClipping[16+52+16] = 
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // for under clipping
    // 0 <
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30,
    31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38,
    39, 39, 39, 39,
    // > 52
    39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39  // over clipping
};


/* ------------------------------------------------------------------------ */
/* NAME:		h264_quantiser_chroma_mapping_table							*/
/* DESCRIPTION:	The table used to map quantiser indices to quantisers to be 
				used for chroma.
				The index is QPluma, and the table output is QPchroma, 
				thus allowing for QPluma in the range 0..51 
				and QPchroma in the range 0..39.							*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_quantiser_chroma_mapping_table[52] = 
{
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30,
  31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 38,
  39, 39, 39, 39
};


// moved here from H264EncoderTables
/* ------------------------------------------------------------------------ */
/* NAME:		h264_encoder_rd_sad_lambda									*/
/* DESCRIPTION:	This table is used for rate distortion optimised mode 
				decisions. It applies to sum of absolute differences.
				 
				It was copied from the test model, and is given by the 
				equation sqrt(0.85 * 2 ^ (QP/3) )							*/
/* ------------------------------------------------------------------------ */

const unsigned char h264_encoder_rd_sad_lambda[52] =
{
		1,  1,  1,  1,  1,  1,	1,  1,  1,  1,
		1,  1,	1,  1,  1,  1,  1,  2,  2,  2,
		2,  3,  3,  3,  4,  4,  5,  5,	6,  7,
		7,  8,  9, 10, 12, 13, 15, 17, 19, 21,
	   23, 26, 30, 33, 37, 42, 47, 53, 59, 66,
	   74, 83
};


/* -------------------------------------------------------------------------*/
/* NAME:		h264_encoder_8X8_skip_threshold								*/
/* DESCRIPTION	This table is used to decide early block fixing in the Inter mode 
				decision. Each of the 8x8 blocks must have a SAD beneath the threshold
				for the given quantiser for the MB to be deemed to be skipped*/
/* -------------------------------------------------------------------------*/
const unsigned char h264_encoder_8X8_skip_threshold[52] = 
{
		2,  2,  2,  3,  3,  3,  4,  4,  4,  5,
		5,  6,  7,  8,  8,  9, 10, 12, 13, 15,
	   16, 18, 20, 23, 25, 29, 32, 36, 40, 45,
	   	50, 57, 63, 71, 80, 90,//100,113,126,142,
	  //159,179,200,225,252,283,318,357,400,449,
	  //504,566
	   90, 90, 90, 90,
	   90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
	   90, 90

};

/* -------------------------------------------------------------------------*/
/* NAME:		h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32								*/
/* DESCRIPTION	This table is copied from the above h264_encoder_8X8_skip_threshold */
/*              table but is pre multiplied and divided for use in the intra */
/*              prediction process */
/* -------------------------------------------------------------------------*/
#define MD32(x) (((x) * MIN_SKIP_16X16) / 32)
const unsigned char h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32[52] = 
{
		MD32(2),  MD32(2),  MD32(2),  MD32(3),  MD32(3),  MD32(3),  MD32(4),  MD32(4),  MD32(4),  MD32(5),
		MD32(5),  MD32(6),  MD32(7),  MD32(8),  MD32(8),  MD32(9), MD32(10), MD32(12), MD32(13), MD32(15),
	   MD32(16), MD32(18), MD32(20), MD32(23), MD32(25), MD32(29), MD32(32), MD32(36), MD32(40), MD32(45),
	   	MD32(50), MD32(57), MD32(63), MD32(71), MD32(80), MD32(90),//100,113,126,142,
	  //159,179,200,225,252,283,318,357,400,449,
	  //504,566
	   MD32(90), MD32(90), MD32(90), MD32(90),
	   MD32(90), MD32(90), MD32(90), MD32(90), MD32(90), MD32(90), MD32(90), MD32(90), MD32(90), MD32(90),
	   MD32(90), MD32(90)

};
const short software_me_offset[2][8] = 
{
	{ // horiz offsets
		-1, // 4, -4
		0, // 0, -4
		1, //-4, -4

		-1, // 4,  0
		//0, // 0,  0
		1, //-4,  0

		-1, // 4,  4
		0, // 0,  4
		1 //-4,  4
	},
	{
		// 2nd step positions relative to first step best location
		-1,	// 2, -2
		-1,		// 0, -2
		-1,	//-2, -2

		0, // 2,  0
		//0, // 0,  0
		0, //-2,  0

		1,	// 1,  1
		1,		// 0,  1
		1		//-1,  1

	}
};
const short software_me_lanes[8] = {1,2,4,8,16,32,64,128};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_LumaChromaFForIntraOrISlice							*/
/* DESCRIPTION:	stores 1 << qbits for Luma and chroma						*/
/* ------------------------------------------------------------------------ */
const unsigned short h264_LumaChromaFForIntraOrISlice[8]={1,1,1,2,5,10,21,42};



//Called via ptr
static void HandleServiceCall_RealFunction()
{
 _ASM("chop");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
}
void (*HandleServiceCall)() = HandleServiceCall_RealFunction;








struct SDM_WorkArea	* WorkAreaSDM = NULL;

void SDM_WorkArea::Init_SDM_WorkArea()
{
    printf("SDM size required: %d Bytes (%dKBytes)\n", sizeof(SDM_WorkArea), (sizeof(SDM_WorkArea) + 1023) /1024);
    printf("sizeof(cMPCfb) %04x\n",sizeof(cMPCfb));
    printf("sizeof(cMPCcb) %04x\n",sizeof(cMPCcb));


#pragma Off(Behaved)            
	WorkAreaSDM = (SDM_WorkArea *)VRaptorDevs.SDM_Base;
#pragma On(Behaved)            

	
	memcpy(WorkAreaSDM->AlphaBetaCoTable, Init_Copy_AlphaBetaCoTable, 3*37*4);
    memcpy(WorkAreaSDM->h264_4x4_transmission_to_raster_table, ::h264_4x4_transmission_to_raster_table, 16);
	memcpy(WorkAreaSDM->h264_frame_scan_quantiser_table,Init_Copy_h264_raster_scan_quantiser_table,96*sizeof(short));
	memcpy(WorkAreaSDM->h264_frame_scan_inverse_quantiser_table, Init_Copy_h264_raster_scan_inverse_quantiser_table, 96*sizeof(short));
    memcpy(WorkAreaSDM->h264_quantiser_chroma_mapping_tableClipping, Init_Copy_h264_quantiser_chroma_mapping_tableClipping, (16+52+16)*sizeof(unsigned char) );
    memcpy(WorkAreaSDM->h264_quantiser_chroma_mapping_table, ::h264_quantiser_chroma_mapping_table, (52)*sizeof(signed char) );
    memcpy(WorkAreaSDM->h264_LumaChromaFForIntraOrISlice, ::h264_LumaChromaFForIntraOrISlice, (8)*sizeof(unsigned short) );
    memcpy(WorkAreaSDM->h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32, ::h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32, 52*sizeof(u_int8));
    memcpy(WorkAreaSDM->h264_encoder_rd_sad_lambda, ::h264_encoder_rd_sad_lambda, 52*sizeof(u_int8));
	memcpy(WorkAreaSDM->MotionEstOffset, ::software_me_offset, 16*sizeof(u_int16));
	memcpy(WorkAreaSDM->LaneLookUp, ::software_me_lanes, 8*sizeof(u_int16));
}





// Copy circular buffer MPC to fixed buffer
void SDM_WorkArea::CopyToFixedAfterDeblockH264(int cirIdx)
{
	SetSCQ(SCQ1);

	int col = MgrMPC.ActMPC[cirIdx].Column;
	_vr00 = _vmovw(cirIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vrun(MACRO_CopyToFixedAfterDeblockH264);
	SetSCQ(0);
}






//-----------------------------------------------------------------------------------------------------------


// Manager for MPC data structures 
MPC_Manager MgrMPC;

MPC_Manager::MPC_Manager()
{
	AllocCirBufIdx = 0;
	for(int m = 0; m < MAX_CIRC_BUFFS; m++)
	{
		ActMPC[m].IsUsed = false;
	}
    Picture.SliceGroupMap = NULL;
    
    IntraStatus.Idx = NO_INTRA_SCHED;
    IntraStatus.Enable = true;
    IntraStatus.ValidBits = 0;
    CountInME = 0;
}



MPC_Manager::~MPC_Manager()
{
    free(Picture.SliceGroupMap);
}





// Returns index of free MPC in array or -1 for error
int MPC_Manager::AllocMPC()
{
    
	int sIdx = AllocCirBufIdx+1;
	if(sIdx == MAX_CIRC_BUFFS)
	{
		sIdx = 0;
	}

	while(sIdx != AllocCirBufIdx)
	{
		if(!ActMPC[sIdx].IsUsed)
		{
			// Step on to next macroblcok for prediction 
			//NextCurrentMacroblock();
		    if(++CurrentMeMB.Col == Picture.Width)
			{
				CurrentMeMB.Col = 0;
				CurrentMeMB.Row++;
			}
			
            _vr00 = _vmovw(sIdx, 1);
            _vr00 = _vmovw(CurrentMeMB.Col, 2);
            _vr00 = _vmovw(AllocCirBufIdx, 4);
            _vrun(MACRO_SetCurrentMPC0);
            _vrun(MACRO_InitCurrentMPC);
            
			ActMPC[sIdx].IsUsed = true;
			ActMPC[sIdx].LumaInpOk = false;
			ActMPC[sIdx].ChromaInpOk = false;
			ActMPC[sIdx].IntraOk = false;
			ActMPC[sIdx].MotionEstOk = false;
			ActMPC[sIdx].SkippedOk = false;
			ActMPC[sIdx].FwdTransformsOk = false;
			ActMPC[sIdx].InvTransformsOk = false;
			ActMPC[sIdx].EntropyEncodeOk = false;
			ActMPC[sIdx].DeblockOk = false;
			
            ActMPC[sIdx].LeftIdx = AllocCirBufIdx; // Assume old index is to left, should only be accessed in this case
            ActMPC[sIdx].Column = CurrentMeMB.Col;
            ActMPC[sIdx].Row = CurrentMeMB.Row;
            AllocCirBufIdx = sIdx;

			return sIdx;
		}
		if(++sIdx == MAX_CIRC_BUFFS)
		{
			sIdx = 0;
		}
	}
	return -1;
}



void MPC_Manager::FreeMPC(int idx)
{
#ifdef _DEBUG
	if(!ActMPC[idx].IsUsed)
	{
		printf("This could be bad!!! MPC is already freed\n");
	}
#endif
	ActMPC[idx].IsUsed = false;
}

// Sets picture size in units of MB
void MPC_Manager::Init(short wid, short hgt)
{
	AllocCirBufIdx = 0;
	for(int m = 0; m < MAX_CIRC_BUFFS; m++)
	{
		ActMPC[m].IsUsed = false;
		ActMPC[m].BufferLimit = 0;
	}

	Picture.Width = wid;
	Picture.Hgt = hgt;
    if(Picture.SliceGroupMap)
        free(Picture.SliceGroupMap);

    // Allocate slice group map (Could be static array at later date)
    Picture.SliceGroupMap = static_cast<signed char*>( malloc(wid*hgt) );
    assert(Picture.SliceGroupMap);
    if(Picture.SliceGroupMap)
    {
        for(int i = 0; i < wid*hgt; i++)
        {
            Picture.SliceGroupMap[i] = 0;
        }
    }
}


void MPC_Manager::ResetTopLeft()
{
	CurrentMeMB.Col = -1;
	CurrentMeMB.Row = 0;
	MacroBlockStartedCount = 0;
	MacroBlocksCompleteCount = 0;
	RestartCount = 0;
}
