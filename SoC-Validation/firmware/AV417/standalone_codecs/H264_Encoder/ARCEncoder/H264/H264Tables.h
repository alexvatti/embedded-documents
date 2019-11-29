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

#ifndef	H264_TABLES_H
#define	H264_TABLES_H

#include "EnumTypes.h"

#ifdef __cplusplus
extern "C" 
{
#endif 



extern const UNSIGNED_SEPTET h264_quantiser_chroma_mapping_table[52];


extern const UNSIGNED_SEPTET h264_block_offset_table[7][4];

extern const UNSIGNED_SEPTET h264_block_width_table[7];

extern const UNSIGNED_SEPTET h264_block_height_table[7];

extern const UNSIGNED_SEPTET h264_raster_index_to_8x8_block_index_table[16];

extern const UNSIGNED_SEPTET h264_8x8_block_index_to_raster_index_table[4];

extern const int reverse_h264_8x8_block_index_to_raster_index_table[16];

extern const unsigned short h264_maximum_mb_bits[52];

#define NUMBER_OF_ANNEX_A_LEVELS 15

extern const int annex_a_level_table[NUMBER_OF_ANNEX_A_LEVELS][10];



extern const unsigned char recon_clip_table[];
extern const unsigned char * recon_clip_table_centre;



//////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
