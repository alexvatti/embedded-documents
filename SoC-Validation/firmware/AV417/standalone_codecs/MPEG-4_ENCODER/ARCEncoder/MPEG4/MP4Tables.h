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

#ifndef	H264_TABLES_H
#define	H264_TABLES_H

//#include "ArcMotionEstimation.h"
#include "defines.h"
#include "EnumTypes.h"
#ifdef __cplusplus
extern "C" 
{
#endif 

extern const unsigned char h263_chroma_roundtab[16];

extern const unsigned char h263_deblock_strength[32];

extern const unsigned char h263_chroma_qscale_table[32];

#define UPDOWNRAMP(x, strength) (x < 0 ? -max(0, -x - max(0,2*(-x -strength))) : max(0,x - max(0,2*(x-strength)))) 

#define CLIPD1(p, d) (d < 0 ? (p < d ? d : p > -d ? -d : p) : (p < -d ? -d : p > d ? d : p))

extern const unsigned short h263_quantiser_table_shift15[32];

extern const unsigned short h263_maximum_mb_bits[32];

extern const UNSIGNED_SEPTET h264_block_width_table[7];

extern const UNSIGNED_SEPTET h264_8x8_block_index_to_raster_index_table[4];

#define NUMBER_OF_ANNEX_X_LEVELS 9

const int annex_x_level_table[NUMBER_OF_ANNEX_X_LEVELS][4] =
{
	 //bitrate, MBs, min pic interval/90000, level
		 64000,  99, 6006,10,
		128000,  99, 3003,20,
		128000, 396, 6006,20,
		384000, 396, 3003,30,
	   2048000, 396, 3003,40,
	    128000,  99, 6006,45,
	   4096000, 396, 1500,50,
	   8192000, 810, 1500,60,
	  16384000, 1620,1500,70
};

//////////////////////////////////////////////////////////////////////////
// recon_clip_table
//////////////////////////////////////////////////////////////////////////

extern const unsigned char recon_clip_table[];
extern const unsigned char * recon_clip_table_centre;

#define RECON_CLIP_TABLE_MIN (-511)
#define RECON_CLIP_TABLE_MAX ( 766)

//////////////////////////////////////////////////////////////////////////
// abs_pel_diff_table
//////////////////////////////////////////////////////////////////////////

extern const unsigned char abs_pel_diff_table[];

#define ABS_PEL_DIFF_TABLE_MIN (-255)
#define ABS_PEL_DIFF_TABLE_MAX ( 255)

#ifdef _DEBUG

unsigned char abs_pel_diff(int x);

#define ABS_PEL_DIFF(x) abs_pel_diff(x)

#else

#define ABS_PEL_DIFF(x) abs_pel_diff_table[(x)-ABS_PEL_DIFF_TABLE_MIN]

#endif

//////////////////////////////////////////////////////////////////////////

extern unsigned char uni_DCtab_lum_len[512];

#ifdef __cplusplus
}
#endif

// shapes
#define RECT_SHAPE       0
#define BIN_SHAPE        1
#define BIN_ONLY_SHAPE   2
#define GRAY_SHAPE       3

// Visual Object Types
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

extern const short ff_mpeg4_default_intra_matrix[64];
extern const short ff_mpeg4_default_non_intra_matrix[64];
extern unsigned short (*p_aanscaletables)[2][32][64];
extern unsigned short (*p_aanscaletables263)[2][32][64];

const unsigned char ff_mba_length[6]={
      6,   7,   9,  11,  13,  14
};

const unsigned char ff_zigzag_direct_copy[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
