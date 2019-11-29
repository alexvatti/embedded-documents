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


#include "defines.h"
// shapes
#define RECT_SHAPE       0
#define BIN_SHAPE        1
#define BIN_ONLY_SHAPE   2
#define GRAY_SHAPE       3

#define SIMPLE_VO_TYPE             1
#define CORE_VO_TYPE               3
#define MAIN_VO_TYPE               4
#define NBIT_VO_TYPE               5
#define ARTS_VO_TYPE               10
#define ACE_VO_TYPE                12
#define ADV_SIMPLE_VO_TYPE         17

// aspect_ratio_info
#define EXTENDED_PAR 15

//vol_sprite_usage / sprite_enable
#define STATIC_SPRITE 1
#define GMC_SPRITE 2

#define MOTION_MARKER 0x1F001
#define DC_MARKER     0x6B001

#define VOS_STARTCODE        0x1B0
#define USER_DATA_STARTCODE  0x1B2
#define GOP_STARTCODE        0x1B3
#define VISUAL_OBJ_STARTCODE 0x1B5
#define VOP_STARTCODE        0x1B6

extern const unsigned char y_dc_scale_table[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 8, 8, 8, 8,10,12,14,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,34,36,38,40,42,44,46
};
extern const unsigned char c_dc_scale_table[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 8, 8, 8, 8, 9, 9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,20,21,22,23,24,25
};

short ff_mpeg4_default_intra_matrix_shift15[64] = {
	4096,	1928,	1820,	1725,	1560,	1425,	1311,	1214,
	1928,	1820,	1725,	1560,	1425,	1311,	1214,	1170,
	1638,	1560,	1489,	1425,	1365,	1260,	1170,	1092,
	1560,	1489,	1425,	1365,	1260,	1170,	1092,	1024,
	1489,	1425,	1365,	1260,	1170,	1092,	1024,	936,
	1425,	1365,	1260,	1170,	1092,	1024,	936,	862,
	1311,	1260,	1170,	1092,	1024,	936,	862,	799,
	1214,	1170,	1092,	1024,	936,	862,	799,	728
};

short ff_mpeg4_default_non_intra_matrix_shift15[64] = {
	2048,	1928,	1820,	1725,	1638,	1560,	1489,	1425,
	1928,	1820,	1725,	1638,	1560,	1489,	1425,	1365,
	1820,	1725,	1638,	1560,	1489,	1425,	1365,	1311,
	1725,	1638,	1560,	1489,	1425,	1365,	1260,	1214,
	1638,	1560,	1489,	1425,	1311,	1260,	1214,	1170,
	1560,	1489,	1425,	1365,	1260,	1214,	1170,	1092,
	1489,	1425,	1365,	1260,	1214,	1170,	1092,	1057,
	1425,	1365,	1311,	1214,	1170,	1092,	1057,	993
};
unsigned short y_dc_aan_scale_table[32] = 
{
	0,		512,	 512,	 512,	 512,	 410,	 341,	 293,
	256,	241,	 228,	 216,	 205,	 195,	 186,	 178,	
	171,	164,	 158,	 152,	 146,	 141,	 137,	 132,	
	128,	120,	 114,	 108,	 102,	  98,	  93,	  89

};
unsigned short c_dc_aan_scale_table[32] = 
{
	0,		512,	 512,	 512,	 512,	 455,	 455,	 410,
	410,	372,	 372,	 341,	 341,	 315,	 315,	 293,	
	293,	273,	 273,	 256,	 256,	 241,	 241,	 228,	
	228,	216,	 205,	 195,	 186,	 178,	 171,	 164

};
unsigned short y_dc_scale_table_shift15[32] = 
{
	0,		4096,	4096,	4096,	4096,	3277,	2731,	2341,
	2048,	1928,	1820,	1725,	1638,	1560,	1489,	1425,	
	1365,	1311,	1260,	1214,	1170,	1130,	1092,	1057,	
	1024,	964,	910,	862,	819,	780,	745,	712
};

unsigned short c_dc_scale_table_shift15[32] =
{
	0,		4096,	4096,	4096,	4096,	3641,	3641,	3277,	
	3277,	2979,	2979,	2731,	2731,	2521,	2521,	2341,	
	2341,	2185,	2185,	2048,	2048,	1928,	1928,	1820,	
	1820,	1725,	1638,	1560,	1489,	1425,	1365,	1311
};
unsigned int y_dc_scale_table_shift20[32] = 
{
	0,	  131072, 131072, 131072, 131072, 104858,  87382,  74899,
	65536, 61681,  58256,  55189,  52429,  49933,  47663,  45591,	
	43691, 41943,  40330,  38837,  37450,  36158,  34953,  33825,	
	32768, 30841,  29128,  27595,  26215,  24967,  23832,  22796
};

unsigned int c_dc_scale_table_shift20[32] =
{
	0,	  131072, 131072, 131072, 131072, 116509, 116509, 104858,	
	104858,95325,  95325,  87382,  87382,  80660,  80660,  74899,	
	74899, 69905,  69905,  65536,  65536,  61681,  61681,  58256,	
	58256, 55189,  52429,  49933,  47663,  45591,  43691,  41943
};
unsigned short mpeg4_quantiser_table_shift15[32] = 
{
	0,		32768,  16384,	10923,	8192,	6554,	5461,	4681,
	4096,	3641,	3277,	2979,	2731,	2521,	2341,	2185,
	2048,	1928,	1820,	1725,	1638,	1560,	1489,	1425,	
	1365,	1311,	1260,	1214,	1170,	1130,	1092,	1057
};

const unsigned int ff_mpeg4_resync_prefix[8]={
    0x7F00, 0x7E00, 0x7C00, 0x7800, 0x7000, 0x6000, 0x4000, 0x0000
};

static const unsigned char mpeg4_dc_threshold[8]={
    99, 13, 15, 17, 19, 21, 23, 0
};

