/* ------------------------------------------------------------------------ */
/* FILE:			H264Tables.cpp											*/
/* DESCRIPTION:		H.264 variable length code tables for decoder			*/
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

#ifndef	H264_TABLES_H
#define	H264_TABLES_H

#include "H264EnumTypes.h"

#ifdef __cplusplus
extern "C" 
{
#endif 

extern const char h264_frame_scan_inverse_quantiser_table[6][16];




extern const UNSIGNED_SEPTET h264_quantiser_chroma_mapping_table[52];


extern const UNSIGNED_SEPTET h264_block_offset_table[7][4];

extern const UNSIGNED_SEPTET h264_block_width_table[7];

extern const UNSIGNED_SEPTET h264_block_height_table[7];

extern const UNSIGNED_SEPTET h264_raster_index_to_8x8_block_index_table[16];

extern const UNSIGNED_SEPTET h264_8x8_block_index_to_raster_index_table[4];

extern const unsigned char ALPHA_TABLE[37];

extern const UNSIGNED_SEPTET BETA_TABLE[37];

extern const UNSIGNED_SEPTET CLIP_TABLE[3][37];
#ifndef AURORA_DEBLOCK
extern const h264_filter_edge_type h264_filter_edge_table[2][16];
#endif
//---
#define CLIP_FOR_FILTER(x) _max(_min(x,36),0)
//extern const UNSIGNED_SEPTET clip_quant_for_loop_filter_table[76];
//
//#define CLIP_FOR_FILTER(x) clip_quant_for_loop_filter_table[x+27]
//---

extern const UNSIGNED_SEPTET clip_chroma_quant_table[76];

#define CLIP_CHROMA_QUANT(x) clip_chroma_quant_table[x+12]

#define NUMBER_OF_ANNEX_A_LEVELS 15

extern const int annex_a_level_table[NUMBER_OF_ANNEX_A_LEVELS][10];

//////////////////////////////////////////////////////////////////////////
// recon_clip_table
//////////////////////////////////////////////////////////////////////////

extern const unsigned char recon_clip_table[];
extern const unsigned char * recon_clip_table_centre;

#define RECON_CLIP_TABLE_MIN (-511)
#define RECON_CLIP_TABLE_MAX ( 766)

#ifdef _DEBUG

unsigned char recon_clip(int x);

#define RECON_CLIP(x) recon_clip(x)

#else

//#define RECON_CLIP(x) recon_clip_table[(x)-RECON_CLIP_TABLE_MIN]

#define RECON_CLIP(x) _max(_min(x,255),0)
//#define RECON_CLIP(x) recon_clip_table_centre[(x)]

#endif

//////////////////////////////////////////////////////////////////////////
// abs_pel_diff_table
//////////////////////////////////////////////////////////////////////////

#define ABS_PEL_DIFF(x) _abs(x)
//extern const unsigned char abs_pel_diff_table[];
//
//#define ABS_PEL_DIFF_TABLE_MIN (-255)
//#define ABS_PEL_DIFF_TABLE_MAX ( 255)
//
//#ifdef _DEBUG
//
//unsigned char abs_pel_diff(int x);
//
//#define ABS_PEL_DIFF(x) abs_pel_diff(x)
//
//#else
//
//#define ABS_PEL_DIFF(x) ABS(x)
//
//#endif
//////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
