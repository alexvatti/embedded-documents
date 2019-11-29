/* ------------------------------------------------------------------------ */
/* FILE:			H264Transform.h											*/
/* DESCRIPTION:		Forward and reverse transform declarations for H.264	*/
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

#ifndef	H264_TRANSFORM_H
#define	H264_TRANSFORM_H

#ifdef __cplusplus
extern "C" 
{
#endif 

extern void Inverse4x4DCTransform(short* data);

//extern void Inverse4x4Transform(short* data, unsigned int line_length);

/* ------------------------------------------------------------------------ */
/* FUNCTION:    Inverse4x4TransformDCOnly                                   */
/* DESCRIPTION: Performs the inverse 4x4 transform when only the DC 
                coefficient is present.
                The output data overwrites the input data.
                Line length field indicates the pointer offsets that 
                this function should use to get from one pel to the 
                immediately below one. For example, when the data being 
                transformed is within a 16x16 block, the value should be 16.
                The output is equal to:
                    (input DC coeff + 32) >> 6.

*/
/* RETURNS:     Void.                                                       */
/* ------------------------------------------------------------------------ */

inline void Inverse4x4TransformDCOnly(short* data, unsigned int line_length)
{
    unsigned output = (unsigned short)(((*data) + 32) >> 6);

    int out32 = output | (output << 16);
    line_length = line_length >> 1;
    int *data32 = (int *)data;
    
    *(data32+0) = out32;
    *(data32+1) = out32;

    _nop();                 // For DMP issues writing to SDM
    data32 += line_length;

    *(data32+0) = out32;
    *(data32+1) = out32;

    _nop();                 // For DMP issues writing to SDM
    data32 += line_length;

    *(data32+0) = out32;
    *(data32+1) = out32;

    _nop();                 // For DMP issues writing to SDM
    data32 += line_length;

    *(data32+0) = out32;
    *(data32+1) = out32;
}

extern void Inverse2x2Transform(short* data);

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
