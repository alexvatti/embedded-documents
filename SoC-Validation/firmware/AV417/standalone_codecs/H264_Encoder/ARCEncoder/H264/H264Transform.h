/* ------------------------------------------------------------------------ */
/* FILE:			H264Transform.h											*/
/* DESCRIPTION:		Forward and reverse transform declarations for H.264	*/
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

#ifndef	H264_TRANSFORM_H
#define	H264_TRANSFORM_H

#ifdef __cplusplus
extern "C" 
{
#endif 

extern void Inverse4x4DCTransform(int16* data);

extern void Inverse4x4Transform(int16* data, unsigned int line_length);

extern void Inverse4x4TransformDCOnly(int16* data, unsigned int line_length);

extern void Inverse2x2Transform(int16* data);

extern void Forward4x4DCTransform(int16* data);

extern void Forward4x4Transform(int16* data, unsigned int line_length);

extern void Forward4x4TransformSeparate(int16* in_data_ptr, int16* out_data_ptr, unsigned int line_length);

extern void Forward2x2Transform(int16* data);

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
