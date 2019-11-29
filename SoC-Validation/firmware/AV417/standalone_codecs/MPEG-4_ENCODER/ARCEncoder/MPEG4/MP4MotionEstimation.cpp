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

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>
#include	<math.h>

#include "CMP4Encoder.h"
#include "MP4EncodedPictureBuffer.h"
#include "MP4EncoderControl.h"
#include "MP4EncoderTables.h"
#include "ArcMP4SDMTables.h"
#include <arc/arc_avu.h>
#include "ArcChannelRoutines.h"
#include "ArcSDMTables_MPA.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcEncoderAddSubtract_MPA.h"
extern void (*HandleServiceCall)();


// These are values are taken from motest.h, but have been given different names, and slightly different meanings here
// to keep the Eurescom interface the same

#define MAXIMUM_BLOCK_TYPES_TO_SEARCH  4		// Only search 16x16, 16x8, 8x16 and 8x8 block sizes


extern  unsigned short aanscaletables[2][32][64];
extern  unsigned short aanscaletables263[2][32][64];
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
	MgrMPC.ActMPC[MgrMPC.IdxFromME].SkippedOk = true;

	unsigned int cIdx = MgrMPC.IdxFromME;
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_CalculateSkippedChromaCost: MPC Index = %d\n", cIdx);
#endif
   	
    int skip_ssd_thresh;

    if(gCH264Encoder.m_coding_standard > MPEG4) 
	    skip_ssd_thresh = (1*h263_encoder_8X8_skip_threshold[cb.QuantValue])>>2;
	else // MPEG4
	    skip_ssd_thresh = (2*mpeg4_encoder_8X8_skip_threshold[cb.QuantValue])>>2;

    if (gCH264Encoder.WorstSkippedChromaCost > skip_ssd_thresh)
	{
	    // decided colour match not close enough, veto the skip decision
	    cb.MBCodingType = MBCT_MPEG4_INTER16X16;
        if ( MgrMPC.ActMPC[cIdx].IntraOk ) // make sure ConsiderIntra16x16 has finished
		{
			gCH264Encoder.ChooseInterOrIntra(cIdx);
            
            if (MBCT_MPEG4_INTER16X16 == cb.MBCodingType)
            {
                gCH264Encoder.MakeInterChromaPrediction263(cIdx);	// Redo it as inter 16x16
                // generates inter luma and chroma residuals
                // Y block
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_Y])), 0x01);           // current MB address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_Y])), 0x02);       // prediction address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_Y])), 0x04); // prediction error address
                _vrun(MACRO_MakeLumaPredictionError16X16);

                // U block
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_U])), 0x01);    // current U block address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U])), 0x02);  // U prediction address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_U])), 0x04);   // U prediction error address
                _vrun(MACRO_MakeChromaPredictionError);

                // V block
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_V])), 0x01);    // current V block address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V])), 0x02);  // V prediction address
                _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_V])), 0x04);   // V prediction error address
                _vrun(MACRO_MakeChromaPredictionError);
            }

            // Kick off fwd transforms
            SET_CallBackDoFwdTransforms_BufIdx(cIdx);
			_vrun(MACRO_CallBackDoFwdTransforms);
        }
	}
    else
    {
        // Kick off dummy call to transforms
    	if ( MgrMPC.ActMPC[cIdx].IntraOk )
    	{
    		SET_CallBackDoFwdTransforms_BufIdx(cIdx);
    		_vrun(MACRO_CallBackDoFwdTransforms);
    	}
    }

}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MotionEstimateCalculateSkippedChromaCost			*/
/* DESCRIPTION:	Calculates the skipped cost of a chroma in a P slice 
				using the SDD.
				Assumes that CalculateSkippedMotionVectors has been called
				to put the skipped mode vector into
					m_Data->m_PSliceSkippedHor and
					m_Data->m_PSliceSkippedVer.

				The results are put into:
					m_Data->m_SkippedModeSSD
					m_Data->m_SkippedModeSSDU
					m_Data->m_SkippedModeSSDV
					m_Data->m_USkippedDCChromaDifference
					m_Data->m_VSkippedDCChromaDifference					*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */
void CH264Encoder::MotionEstimateCalculateSkippedChromaCost()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxFromME];
    
	MakeSkippedChromaPrediction(MgrMPC.IdxFromME);
	
	// Need to calculate chrominance SSDs - to avoid blocks with reasonable luminance match but the wrong colour!
	// First need to make the prediction

    int cbIdx = MgrMPC.IdxFromME;
    int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;

	// setup pointers for the current, left and above macroblocks in the simd
    _vr00 = _vmovw(cbIdx, 1);
    _vr00 = _vmovw(MgrMPC.CurrentMeMB.Col, 2);
    _vr00 = _vmovw(leftIdx, 4);
    _vrun(MACRO_SetCurrentMPC);

	// calculate maximum 4x4 ssd cost
	_vrun(MACRO_CalculateSkippedChromaCost);	// Channel calls Service_CalculateSkippedChromaCost    

}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
