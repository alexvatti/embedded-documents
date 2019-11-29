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

/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderAddSubtract.cpp								*/
/* DESCRIPTION:		H.264 encoder add and subtract functions				*/
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

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "CH264Encoder.h"
#include "H264EncoderControl.h"
#include "H264OffsetTables.h"
#include "H264Tables.h"
#include "ArcSDMTables.h"
#include <string.h>			// TEMP for memcpy
#include "ArcMPC.h"

#include <arc/arc_avu.h>
#include "ArcSDMTables_MPA.h"
#include "ArcEncoderAddSubtract_MPA.h"
extern void (*HandleServiceCall)();

#include "ArcChannelRoutines.h"


// Copy Intra prediction residual into MPC
void CH264Encoder::GetIntraLumaChromaPredictionAndError(int sIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[sIdx];

    int bestIntraLumaMode = cb.WorkArea.BestIntraLumaMode;
    int bestIntraChromaMode = cb.WorkArea.BestIntraChromaMode;

    // luma
    SET_MemCopy64Byte_From((int)(WorkAreaSDM->IntraPredictionLuma[bestIntraLumaMode]));
    SET_MemCopy64Byte_To((int)(cb.PixelCoeffBuffer + PCB_REFERENCE_Y));
    SET_MemCopy64Byte_Size(256/64);
    _vrun(MACRO_MemCopy64Byte);
    SET_MemCopy64Byte_From((int)(WorkAreaSDM->IntraPredictionResidsLuma[bestIntraLumaMode]));
    SET_MemCopy64Byte_To((int)(cb.PixelCoeffBuffer + PCB_RESIDUAL_Y));
    SET_MemCopy64Byte_Size(512/64);
    _vrun(MACRO_MemCopy64Byte);

    // chroma
    SET_MemCopy64Byte_From((int)(WorkAreaSDM->IntraPredictionChroma[bestIntraChromaMode]));
    SET_MemCopy64Byte_To((int)(cb.PixelCoeffBuffer + PCB_REFERENCE_U));
    SET_MemCopy64Byte_Size(128/64);
    _vrun(MACRO_MemCopy64Byte);
    SET_MemCopy64Byte_From((int)(WorkAreaSDM->IntraPredictionResidsChroma[bestIntraChromaMode]));
    SET_MemCopy64Byte_To((int)(cb.PixelCoeffBuffer + PCB_RESIDUAL_U));
    SET_MemCopy64Byte_Size(256/64);
    _vrun(MACRO_MemCopy64Byte);

}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ReconstructLuma16X16							*/
/* DESCRIPTION:	This adds the prediction to the coded prediction error, 
				clips, and reconstructs into the encoded picture.
				Operates on the current 16x16 macroblock luma data.			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::ReconstructLuma16X16(int idx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[idx];

    _vr00 = _vmovw((unsigned int)(TRANSFORM_Y), 0x01);    // Y residual block address in SDM
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_REFERENCE_Y), 0x02);    // Y prediction address in SDM
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y+96), 0x04);    // Y reconstructed block address in SDM
    _vrun(MACRO_ReconstructLuma16X16);

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ReconstructChroma								*/
/* DESCRIPTION:	This adds the prediction to the coded prediction error, 
				clips, and reconstructs into the encoded picture.
				Operates on the current macroblock chroma data.				*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::ReconstructChroma(int idx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[idx];


    // U + V blocks
    _vr00 = _vmovw((unsigned int)(TRANSFORM_U), 0x01);    // U residual block address in SDM
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_REFERENCE_U), 0x02);   // U prediction address in SDM
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_U+96), 0x04);     // U reconstructed block address in SDM
    _vrun(MACRO_ReconstructChroma);

	return true;
}



// Copies reference area of MPC to reconstructed area of MPC (overlapping copy) stride 16 to stride 24
void CH264Encoder::ReconstructSkipped(int idx)
{
	//MgrMPC.IdxReconstruct = idx;
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[idx];

    // Because luma and chroma Xform blocks are contiguous in SDM, this macro
    // call will take care of both in one go
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_REFERENCE_Y), 0x01);      // Y prediction block address in SDM
    _vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y+96), 0x02);  // Y reconstructed block address in SDM
    _vrun(MACRO_ReconstructSkipped);



 }




/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
