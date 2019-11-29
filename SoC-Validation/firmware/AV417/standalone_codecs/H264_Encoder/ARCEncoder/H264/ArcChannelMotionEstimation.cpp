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




#include "ArcChannelRoutines.h"
#include "CH264Encoder.h"
#include "H264EncoderTables.h"
#include "ArcSDMTables.h"
#include "ArcFrameBuffers.h"

#include "ArcMotionEstimationModel.h"



#include <arc/arc_avu.h>
#include "ArcIntraLumaPrediction_MPA.h"
#include "ArcIntraLumaPredictionFast_MPA.h"
#include "ArcIntraChromaPrediction_MPA.h"

#include "H264MotionEstimation_MPA.h"
#include "ArcTransformQuant_MPA.h"

#include "ArcSDMTables_MPA.h"
#include "ArcChannel_MPA.h"



// global intra valid flag
int gIntraValidFlag;



//******************************************************************************
// Name:         Service_ME_Initialise
// Purpose:      Initialises the ME by enabling channels
// Arguments:    void
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_Initialise(void)
{
    // enable channels
    if (HwMe == 1)
    {
        WriteToAuxRegME_Hw(AUXREG_ME_BASE_BR_ADDR, 0x00000000);
    }
    else
    {
        WriteToAuxRegME_Sw(AUXREG_ME_BASE_BR_ADDR, 0x00000000);
    }
}


//******************************************************************************
// Name:         Service_ME_SetCollectionID
// Purpose:      Sets the collection id register in the ME. This value is
//               to the corresponding location in the mpc after an iteration
// Arguments:    void
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE Service_ME_SetCollectionID(unsigned int collectionID)
{
    if (HwMe == 1)
    {
        WriteToChannelME_Hw(CHANREG_ME_COLLECTION_ID, collectionID);
    }
    else
    {
        WriteToChannelME_Sw(CHANREG_ME_COLLECTION_ID, collectionID);
    }
}


//******************************************************************************
// Name:         Service_ME_SetBranchAddresses
// Purpose:      Sets the response function branch addresses that will be used
//               by the best prediction
// Arguments:    void
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_SetBranchAddresses(void)
{
    unsigned int branchAddr;

    if (HwMe == 1)
    {
        // set the response function for the SDM branch address (channel out mb)
        branchAddr = (unsigned int) Service_ME_SDMResponse;
    
        WriteToAuxRegME_Hw(AUXREG_ME_SDM_BR_ADDR, branchAddr);
    
    
        // set the response function for the skip branch address (channel out final)
        branchAddr = (unsigned int) Service_ME_SkipResponse;
    
        WriteToAuxRegME_Hw(AUXREG_ME_SKIP_BR_ADDR, branchAddr);
    
    
        // set the response function for the inter predictions;
        //    16x16 branch address (channel out final)
        //    8x8 branch address (channel out final)
        //    16x8 branch address (channel out final)
        //    8x16 branch address (channel out final)
        branchAddr = (unsigned int) Service_ME_InterResponse;
    
        WriteToAuxRegME_Hw(AUXREG_ME_16x16_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_8x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_16x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_8x16_BR_ADDR, branchAddr);
    }
    else
    {
        // set the response function for the SDM branch address (channel out mb)
        branchAddr = (unsigned int) Service_ME_SDMResponse;
    
        WriteToAuxRegME_Sw(AUXREG_ME_SDM_BR_ADDR, branchAddr);
    
    
        // set the response function for the skip branch address (channel out final)
        branchAddr = (unsigned int) Service_ME_SkipResponse;
    
        WriteToAuxRegME_Sw(AUXREG_ME_SKIP_BR_ADDR, branchAddr);
    
    
        // set the response function for the inter predictions;
        //    16x16 branch address (channel out final)
        //    8x8 branch address (channel out final)
        //    16x8 branch address (channel out final)
        //    8x16 branch address (channel out final)
        branchAddr = (unsigned int) Service_ME_InterResponse;
    
        WriteToAuxRegME_Sw(AUXREG_ME_16x16_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_8x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_16x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_8x16_BR_ADDR, branchAddr);
    }
}


//******************************************************************************
//Name:         Service_ME_StreamSetup
//Purpose:      Sets the reference and current strides and current location in main memory and
//            reset the macroblock index position to zero.
//Arguments:    void
//Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_StreamSetup(void)
{
	// setup ME reference and current locations in main memory

	// when changing the AUXREG_ME_REFERENCE_STRIDE we need to toggle
	// ME_CONTROL0__REF_PIX_CACHE to get the hardware to flush the me cache.
	// this is unless we change AUXREG_ME_REFERENCE_START after the stride, as
	// this will cause a flush anyway
    if (HwMe == 1)
    {
		// MeBusConv:
		// if width is an odd number of macroblocks then make it even
		if (gCH264Encoder.m_EncodedPicture->m_hsize & 0x10)
		{
	    	WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_STRIDE, (u_int32) (gCH264Encoder.m_EncodedPicture->m_hsize + 16));
		}
		else
		{
	    	WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
		}

    	WriteToAuxRegME_Hw(AUXREG_ME_CURRENT_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
    	WriteToAuxRegME_Hw(AUXREG_ME_MB_DIMENSION, (u_int32) (((((gCH264Encoder.m_EncodedPicture->m_hsize)+15) >> 4) << ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) |
                                                           ((((gCH264Encoder.m_EncodedPicture->m_vsize)+15) >> 4) << ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT)) );
    #ifdef ARC_ENCODER_API
    	WriteToAuxRegME_Hw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.PhysIPY);
    #else
    	WriteToAuxRegME_Hw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.InputY);
    #endif
    
     // reset ME unit to start of picture
    	WriteToAuxRegME_Hw(AUXREG_ME_MB_INDEX, 0);
    }
    else
    {
        WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
        WriteToAuxRegME_Sw(AUXREG_ME_MB_DIMENSION, (u_int32) (((((gCH264Encoder.m_EncodedPicture->m_hsize)+15) >> 4) << ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) |
                                                           ((((gCH264Encoder.m_EncodedPicture->m_vsize)+15) >> 4) << ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT)) );
    #ifdef ARC_ENCODER_API
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.PhysIPY);
    #else
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.InputY);
    #endif
    
     // reset ME unit to start of picture
        WriteToAuxRegME_Sw(AUXREG_ME_MB_INDEX, 0);
    }
}


//******************************************************************************
//Name:         Service_ME_PictureSetup
//Purpose:      Sets the reference location in main memory
//Arguments:    void
//Return Value: CHANNEL_SERVICE
//******************************************************************************
static u_int32 ME_RefPtr;
CHANNEL_SERVICE Service_ME_PictureSetup(void)
{
    if (gArcFrameBuffers.YUVPos == 1)
    {
#ifdef ARC_ENCODER_API
	ME_RefPtr = (u_int32) gArcFrameBuffers.PhysOPRefY;
#else
	ME_RefPtr = (u_int32) gArcFrameBuffers.OutputRefY;
#endif
    }
    else
    {
#ifdef ARC_ENCODER_API
	ME_RefPtr = (u_int32) gArcFrameBuffers.PhysOPY;
#else
	ME_RefPtr = (u_int32) gArcFrameBuffers.OutputY;
#endif
    }

    if (HwMe == 1)
    {
        WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_START, ME_RefPtr);
    }
    else
    {
        WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_START, ME_RefPtr);
    }
}
 	
//******************************************************************************
// Name:         Service_ME_InterIteration
// Purpose:      Starts the ME for an inter macroblock. This causes the choosen
//               ME modes to be iterated over to find the best prediction
// Arguments:    cb = current mb (in circular buffer)
//               lcb = left (A) mb (in circular buffer)
//               lfb = above left (D) mb (in fixed buffer)
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_InterIteration(cMPCcb *cb, cMPCcb *lcb, cMPCfb *lfb)
{
    static u_int8 lastQuantVal = 0xff;

    u_int32 pcbAddr = (u_int32) cb->PixelCoeffBuffer;
    u_int8 quantVal = cb->QuantValue;

    if (HwMe == 1)
    {
    	WriteToChannelME_Hw(CHANREG_ME_MB_SDM_ADDR, pcbAddr);
        WriteToChannelME_Hw(CHANREG_ME_REFERENCE_SDM_START, pcbAddr + PCB_REFERENCE_Y);
    
        if (quantVal != lastQuantVal)
        {
            WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP, (u_int32) gCH264Encoder.m_SkipSadThresholdMult*h264_encoder_8X8_skip_threshold[quantVal]>>5);
            WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*h264_encoder_8X8_skip_threshold[quantVal]>>2);
            WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) h264_encoder_8X8_skip_threshold[quantVal]*2); 
            WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) ((5*h264_encoder_8X8_skip_threshold[quantVal])>>2));
            WriteToChannelME_Hw(CHANREG_ME_LAMBDA, (u_int32) h264_encoder_rd_sad_lambda[quantVal]);
    
        	lastQuantVal = quantVal;
        }
        
        WriteToChannelME_Hw(CHANREG_ME_D_POINTER, (u_int32) lfb);
        WriteToChannelME_Hw(CHANREG_ME_A_POINTER, (u_int32) lcb);
        WriteToChannelME_Hw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
    
        // inferred kick of the ME
        //WriteToChannelME_Hw(CHANREG_ME_CONTROL0, (u_int32) ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT));
        WriteToChannelME_Hw(CHANREG_ME_CONTROL0, 0x8);
    }
    else // not HwMe
    {
        WriteToChannelME_Sw(CHANREG_ME_MB_SDM_ADDR, pcbAddr);
        WriteToChannelME_Sw(CHANREG_ME_REFERENCE_SDM_START, pcbAddr + PCB_REFERENCE_Y);
    
        if (quantVal != lastQuantVal)
        {
            WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP, (u_int32) gCH264Encoder.m_SkipSadThresholdMult*h264_encoder_8X8_skip_threshold[quantVal]>>5);
            WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*h264_encoder_8X8_skip_threshold[quantVal]>>2);
            WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) h264_encoder_8X8_skip_threshold[quantVal]*2); 
            WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) ((5*h264_encoder_8X8_skip_threshold[quantVal])>>2));
            WriteToChannelME_Sw(CHANREG_ME_LAMBDA, (u_int32) h264_encoder_rd_sad_lambda[quantVal]);
    
            lastQuantVal = quantVal;
        }
        
        WriteToChannelME_Sw(CHANREG_ME_D_POINTER, (u_int32) lfb);
        WriteToChannelME_Sw(CHANREG_ME_A_POINTER, (u_int32) lcb);
        WriteToChannelME_Sw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
    
        // inferred kick of the ME
		gArcMotionEstimationModel.Load_Current_MB_Luma(cb, 0);
        //WriteToChannelME_Sw(CHANREG_ME_CONTROL0, (u_int32) ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT));
        //WriteToChannelME_Sw(CHANREG_ME_CONTROL0, 0x8);
		
		gArcMotionEstimationModel.Software_ME_control(cb);
        // need to collect ME results and kick of SIMD macro to return the results
        // via a channel
     
    }

}


//******************************************************************************
// Name:         Service_ME_IntraIteration
// Purpose:      Starts the ME for an inra macroblock. This causes the current
//               luma mb to be fetched into the sdm and the neighbour mpc info
//               to be updated
// Arguments:    cb = current mb (in circular buffer)
//               lcb = left (A) mb (in circular buffer)
//               lfb = above left (D) mb (in fixed buffer)
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_IntraIteration(cMPCcb *cb, cMPCcb *lcb, cMPCfb *lfb)
{
    if (HwMe == 1)
    {
        WriteToChannelME_Hw(CHANREG_ME_MB_SDM_ADDR, (u_int32) cb->PixelCoeffBuffer);
        WriteToChannelME_Hw(CHANREG_ME_D_POINTER, (u_int32) lfb);
        WriteToChannelME_Hw(CHANREG_ME_A_POINTER, (u_int32) lcb);
        WriteToChannelME_Hw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
    
        // inferred kick of the ME
        //WriteToChannelME_Hw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__NO_ME_MASK) << ME_CONTROL0__NO_ME_SHIFT) |
        //                                                 ((0 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT)) );
        // keep rce off for intra frames to improve me performance
        WriteToChannelME_Hw(CHANREG_ME_CONTROL0, (u_int32) 0x4);
    }
    else
    {
        WriteToChannelME_Sw(CHANREG_ME_MB_SDM_ADDR, (u_int32) cb->PixelCoeffBuffer);
        WriteToChannelME_Sw(CHANREG_ME_D_POINTER, (u_int32) lfb);
        WriteToChannelME_Sw(CHANREG_ME_A_POINTER, (u_int32) lcb);
        WriteToChannelME_Sw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
    
        // inferred kick of the ME
        //WriteToChannelME_Sw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__NO_ME_MASK) << ME_CONTROL0__NO_ME_SHIFT) |
        //                                                 ((0 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT)) );
        // keep rce off for intra frames to improve me performance
        //WriteToChannelME_Sw(CHANREG_ME_CONTROL0, (u_int32) 0x4);

		gArcMotionEstimationModel.Load_Current_MB_Luma(cb,1);
	
		gArcMotionEstimationModel.CreateChannelCommandMB();
		// wait for current mb dma
		 _ASM("vdmawait 0x1, 0x7f"); // wait for only 1 dmas in and max dma outs left in fifos

       // need to collect ME results and kick of SIMD macro to return the results
        // via a channel
        struct Response * MEResponse;
    
        // get first ME channel response
        MEResponse = gArcMotionEstimationModel.GetChannelResponse();
    
        if (MEResponse == NULL)
        {
            fprintf(stderr, "ArcChannelMotionEstimation.cpp: Error - Can't get first ME channel response\n");
            assert(false);
        }
    
        u_int16 intraValidFlag = MEResponse->RegisterData[0].RegValue & 0x3;
        u_int16 skipMv = MEResponse->RegisterData[1].RegValue & 0xffff;
    
        SET_Generate_ME_SDMResponse_intraValidFlag(intraValidFlag);
        SET_Generate_ME_SDMResponse_skipMv(skipMv);
        _vrun(MACRO_Generate_ME_SDMResponse);
    }
}


//******************************************************************************
// Start intra prediction on SIMD unit
// validBits 	:	Flags for left & above legal for prediction use
// cIdx			:	Circlar buffer index
void DoIntraPrediction(int validBits, int cIdx)
{
    // set global intra valid flag
	gIntraValidFlag = validBits;
	// setup pointers for the current, left and above macroblocks in the simd
	_vr00 = _vmovw(cIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cIdx].Column, 2);
	_vr00 = _vmovw(MgrMPC.ActMPC[cIdx].LeftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
	
    // setup input arguments for MACRO_ConstrainIntraValidFlag
    SET_Global_intraValidFlag(validBits);
    SET_ConstrainIntraValidFlag_constIntraPred(gCH264Encoder.m_PictureParameterSet->constrained_intra_pred_flag); // constrained_intra_pred_idc

    // combine constrained_intra_pred_idc flag with validBits before input
    // to intra prediction processes
    _vrun(MACRO_ConstrainIntraValidFlag);

	// perform intra luma prediction
    if (gCH264Encoder.m_UseWhitening)
    {
	    // setup input arguments for MACRO_GenerateIntra16x16Prediction
	    SET_Global_gUseIntraLumaDCPredOnly(gCH264Encoder.m_UseIntraDCpredOnly);
	    SET_Global_gUseWhitening(1);
	
	    // intra luma prediction with whitening
	    _vrun(MACRO_GenerateIntra16x16Prediction);
    }
    else
    {
	    // intra luma prediction without whitening
    	if (gCH264Encoder.m_UseIntraDCpredOnly)
    	{
    		// intra luma prediction dc only
    		_vrun(MACRO_GenerateIntra16x16PredictionFastDC);
    	}
    	else if (gCH264Encoder.m_UseIntraVertPredOnly)
    	{
    		// intra luma prediction vertical only (if above not available then
    		// it will call MACRO_GenerateIntra16x16PredictionFastDC)
    		_vrun(MACRO_GenerateIntra16x16PredictionFastVertical);
    	}
    	else
    	{
    		// intra luma prediction vertical, horizontal and dc
    		_vrun(MACRO_GenerateIntra16x16PredictionFastAll);
    	}
    }

    // setup input arguments for MACRO_GenerateIntraChromaPrediction
    SET_Global_gUseIntraChromaDCPredOnly(gCH264Encoder.m_UseIntraDCpredOnly || gCH264Encoder.m_UseIntraVertPredOnly);

    // perform intra chroma prediction
    _vrun(MACRO_GenerateIntraChromaPrediction);

    // set intra mode in mpcbc
	SET_SetIntraModeMPC_sIdx(cIdx);
    _vrun(MACRO_SetIntraModeMPC); // Calls Service_IntraComplete via channel
}



//******************************************************************************
// Name:         Service_ME_SDMResponse
// Purpose:      Store ME results from channel out mb, and kick off intra
//               prediction
// Arguments:    reg0 = 2 bit intra valid pattern
//               reg1 = skipped MV
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_SDMResponse(unsigned int reg0, unsigned int reg1)
{
    int cIdx = MgrMPC.IdxFromME = MgrMPC.MeQueue.Pull();	// Read from queue of item currently in ME
    MgrMPC.ActMPC[cIdx].LumaInpOk = true;
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

    cb.WorkArea.MvSkip.mvx = reg1 & 0xff;
    cb.WorkArea.MvSkip.mvy = reg1 >> 8;

#ifdef DUMP_DEBUG
    DumpDebug_Value("chanMeToArc_IntraValid", static_cast<int> (reg0));
#endif // DUMP_DEBUG

    
    if(MgrMPC.IntraStatus.Enable)
    {
    	MPC_Manager::ActMPC_info &ampc = MgrMPC.ActMPC[cIdx];
    	if(ampc.Column == 0 || MgrMPC.ActMPC[ampc.LeftIdx].InvTransformsOk)
    	{
    		MgrMPC.IntraStatus.Idx = NO_INTRA_SCHED;
    		DoIntraPrediction(reg0, cIdx);
    	}
    	else
    	{
//    		if(MgrMPC.IntraStatus.Idx != NO_INTRA_SCHED )	// Debug code
//    		{
//    			printf("Intra schedule error\n");
//    		}
    		MgrMPC.IntraStatus.Idx = cIdx;
    		MgrMPC.IntraStatus.ValidBits = reg0;
    	}
    }
    else
    {
    	MgrMPC.IntraStatus.Idx = NO_INTRA_SCHED;
    	MgrMPC.ActMPC[cIdx].IntraOk = true;
    }

}


//******************************************************************************
// Name:         Service_ME_SkipResponse
// Purpose:      Store ME results from channel out final, and perform skipped
//               chroma veto
// Arguments:    reg0 = me_cost
//               reg1 = me_blk_pattern if block pattern functionality enabled
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_SkipResponse(unsigned int reg0, unsigned int reg1)
{
    int cIdx = MgrMPC.IdxFromME;
	//printf("Skip %d\n", cIdx);
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
//    MgrMPC.ActMPC[cIdx].MotionEstOk = true;

    gCH264Encoder.bestInterCost = reg0;

    //  me_blk_pattern (reg1) Not used for H264

    // perform motion estimation for chroma skip
    gCH264Encoder.MotionEstimateCalculateSkippedChromaCost(cIdx); // Calls MACRO_CalculateSkippedChromaCost that calls Service_CalculateSkippedChromaCost 

}




//******************************************************************************
// Name:         Service_ME_InterResponse
// Purpose:      Store ME results from channel out final
// Arguments:    reg0 = me_cost
//               reg1 = me_blk_pattern if block pattern functionality enabled
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_InterResponse(unsigned int reg0, unsigned int reg1)
{
    gCH264Encoder.bestInterCost = reg0;
    int cIdx = MgrMPC.IdxFromME;
    MgrMPC.ActMPC[cIdx].MotionEstOk = true;
    MgrMPC.ActMPC[cIdx].SkippedOk = true;	// Skip not needed

	// setup pointers for the current, left and above macroblocks in the simd
	int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
	_vr00 = _vmovw(cIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cIdx].Column, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
    gCH264Encoder.MakeInterChromaPrediction(cIdx);

	if ( MgrMPC.ActMPC[cIdx].IntraOk )
	{
		gCH264Encoder.ChooseInterOrIntra(cIdx);
	}
	
    //  me_blk_pattern (reg1) Not used

}

