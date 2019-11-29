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

#ifndef	H264_DECODER_TABLES_H
#define	H264_DECODER_TABLES_H

#ifdef __cplusplus
extern "C" 
{
#endif 

extern const UNSIGNED_SEPTET h264_decoder_intra_cbp_table[48];

extern const UNSIGNED_SEPTET h264_decoder_inter_cbp_table[48];

#ifdef __cplusplus
}
#endif

#endif

#endif
/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
