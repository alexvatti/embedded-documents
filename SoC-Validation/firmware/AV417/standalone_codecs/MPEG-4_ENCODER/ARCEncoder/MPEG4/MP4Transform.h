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

#ifndef	H264_TRANSFORM_H
#define	H264_TRANSFORM_H

#ifdef __cplusplus
extern "C" 
{
#endif 

extern void Inverse4x4DCTransform(int* data);

extern void Inverse4x4Transform(int* data, unsigned int line_length);

extern void Inverse4x4TransformDCOnly(int* data, unsigned int line_length);

extern void Inverse2x2Transform(int* data);

extern void Forward4x4DCTransform(int* data);

extern void Forward4x4Transform(int* data, unsigned int line_length);

extern void Forward4x4TransformSeparate(int* in_data_ptr, int* out_data_ptr, unsigned int line_length);

extern void Forward2x2Transform(int* data);

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
