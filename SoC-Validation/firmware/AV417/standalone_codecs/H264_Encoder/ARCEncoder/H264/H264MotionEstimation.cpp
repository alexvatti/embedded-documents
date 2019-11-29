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
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */
/* ------------------------------------------------------------------------ */
/* FILE:			H264MotionEstimation.cpp								*/
/* DESCRIPTION:		H.264 motion estimation functions						*/
/* VERSION:			v2.20													*/
/* DATE:			22 September 2005										*/
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
#include	<math.h>

#include "CH264Encoder.h"
#include "H264EncodedPictureBuffer.h"
#include "H264EncoderControl.h"
#include "H264EncoderTables.h"
#include "H264Tables.h"
#include "H264OffsetTables.h"
#include <assert.h>
#include "ArcSDMTables.h"

#include "picture_encoder.h"
#include "ArcMPC.h"

#include <arc/arc_avu.h>
#include "ArcChannelRoutines.h"
#include "ArcSDMTables_MPA.h"
#include "H264MotionEstimation_MPA.h"
#include "ArcTransformQuant_MPA.h"
#include "ArcDeblock_MPA.h"
#include "VRaptorInfo.h"

#include "ArcChannelRoutines.h"
extern void (*HandleServiceCall)();




//------------------------------------------------------------------------------
// Name:          Service_CalculateSkippedChromaCost
// Purpose:       Service response function CalculateSkippedChromaCost. It sets
//				  the WorstSkippedChromaCost with the return value
// Arguments:     r0 = lower 16 bits of worst skipped chroma cost
//			      r1 = upper 16 bits of worst skipped chroma cost
// Return Value:  void
//------------------------------------------------------------------------------
CHANNEL_SERVICE Service_CalculateSkippedChromaCost (unsigned int r0, unsigned int r1)
{
	// channels can only return 16 bit scalars
	gCH264Encoder.WorstSkippedChromaCost = (r1 << 16) | (r0 & 0xffff);
	
	int cIdx = MgrMPC.IdxFromME;
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

    int skip_ssd_thresh = (2*h264_encoder_8X8_skip_threshold[cb.QuantValue])>>2;

    #ifdef DUMP_DEBUG
    DumpDebug_Value("skip_ssd_thresh", skip_ssd_thresh);
    #endif // DUMP_DEBUG

	// setup pointers for the current, left and above macroblocks in the simd
	int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
	_vr00 = _vmovw(cIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cIdx].Column, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
    
    
    
    if (gCH264Encoder.WorstSkippedChromaCost > skip_ssd_thresh)
    {
    	// decided colour match not close enough
        cb.MBCodingType = MBCT_H264_INTER16X16;
    }
    MgrMPC.ActMPC[cIdx].SkippedOk = true;
    MgrMPC.ActMPC[cIdx].MotionEstOk = true;

    // no need to Redo MakeInterChromaPrediction as inter 16x16 as we are
    // using the same MVs and inter16x16 and skip have the same block size 

#ifdef DUMP_DEBUG
    DumpDebug_Value("MBCodingType (Skip Veto Change)", cb.MBCodingType);
#endif // DUMP_DEBUG
    
	if ( MgrMPC.ActMPC[cIdx].IntraOk )
	{
		gCH264Encoder.ChooseInterOrIntra(cIdx);
	}
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MotionEstimateCalculateSkippedChromaCost			*/
/* DESCRIPTION:	Calculates the skipped cost of a chroma in a P slice
				using the SDD.
				Assumes that CalculateSkippedMotionVectors has been called
				to put the skipped mode vector into
					m_PSliceSkippedHor and
					m_PSliceSkippedVer.

				The results are put into:
					m_SkippedModeSSD
					m_SkippedModeSSDU
					m_SkippedModeSSDV
					m_USkippedDCChromaDifference
					m_VSkippedDCChromaDifference					*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */
void CH264Encoder::MotionEstimateCalculateSkippedChromaCost(int cIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
   // no need to call specific skipped chroma prediction function because the
   // skipped motion vector will be returned in 
   MakeInterChromaPrediction(cIdx);


	// Need to calculate chrominance SSDs - to avoid blocks with reasonable luminance match but the wrong colour!
	// First need to make the prediction


	// calculate skipped dc chroma differences

	// u component

    int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
    int col = MgrMPC.ActMPC[cIdx].Column;

    // setup pointers for the current, left and above macroblocks in the simd
    _vr00 = _vmovw(cIdx, 1);
    _vr00 = _vmovw(col, 2);
    _vr00 = _vmovw(leftIdx, 4);
    _vrun(MACRO_SetCurrentMPC0);

	// calculate maximum 4x4 ssd cost
	_vrun(MACRO_CalculateSkippedChromaCost);			// Channel calls Service_CalculateSkippedChromaCost    

}



/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
