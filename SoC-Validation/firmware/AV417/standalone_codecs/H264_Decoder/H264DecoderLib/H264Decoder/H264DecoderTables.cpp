/* ------------------------------------------------------------------------ */
/* FILE:			H264DecoderTables.cpp									*/
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

#if !defined(CAVLC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)

#include "H264EnumTypes.h"
#include "H264Warnings.h"

#include "H264DecoderTables.h"

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_intra_cbp_table								*/
/* DESCRIPTION:	The table for converting codewords to intra cbp bit masks.	*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_decoder_intra_cbp_table[48] =
	{47, 31, 15,  0, 23, 27, 29, 30,
	  7, 11, 13, 14, 39, 43, 45, 46,
	 16,  3,  5, 10, 12, 19, 21, 26,
	 28, 35, 37, 42, 44,  1,  2,  4,
	  8, 17, 18, 20, 24,  6,  9, 22,
	 25, 32, 33, 34, 36, 40, 38, 41};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_inter_cbp_table								*/
/* DESCRIPTION:	The table for converting codewords to inter cbp bit masks.	*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_decoder_inter_cbp_table[48] =
	{ 0, 16,  1,  2,  4,  8, 32,  3,
	  5, 10, 12, 15, 47,  7, 11, 13,
	 14,  6,  9, 31, 35, 37, 42, 44,
	 33, 34, 36, 40, 39, 43, 45, 46,
	 17, 18, 20, 24, 19, 21, 26, 28,
	 23, 27, 29, 30, 22, 25, 38, 41};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
