/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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



#include "MP4EncoderControl.h"
#include "MP4EncoderTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMP4SDMTables.h"
#include "ArcFrameBuffers.h"
#include "ArcMotionEstimationModel.h"
#include "VRaptorInfo.h"
#include "ArcMacroRecordSettings.h"

#include <arc/arc_avu.h>

#include "ArcSDMTables_MPA.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcMp4IntraLumaPrediction_MPA.h"
#include "ArcEncoderAddSubtract_MPA.h"
#include "ArcChannel_MPA.h"

extern void (*HandleServiceCall)();


#define AUXREG_ME_VCONFIG                 (VRaptorDevs.MotionEstimator.AuxBase + 0)
#define AUXREG_ME_NAMESTR                 (VRaptorDevs.MotionEstimator.AuxBase + 1)
#define AUXREG_ME_BUILD                   (VRaptorDevs.MotionEstimator.AuxBase + 2)
#define AUXREG_ME_REFERENCE_START         (VRaptorDevs.MotionEstimator.AuxBase + 3)
#define AUXREG_ME_REFERENCE_STRIDE        (VRaptorDevs.MotionEstimator.AuxBase + 4)
#define AUXREG_ME_CURRENT_START           (VRaptorDevs.MotionEstimator.AuxBase + 5)
#define AUXREG_ME_CURRENT_STRIDE          (VRaptorDevs.MotionEstimator.AuxBase + 6)
#define AUXREG_ME_MB_DIMENSION            (VRaptorDevs.MotionEstimator.AuxBase + 7)
#define AUXREG_ME_MB_INDEX                (VRaptorDevs.MotionEstimator.AuxBase + 8)
#define AUXREG_ME_MB_SDM_ADDR             (VRaptorDevs.MotionEstimator.AuxBase + 9)
#define AUXREG_ME_REFERENCE_SDM_START     (VRaptorDevs.MotionEstimator.AuxBase + 10)
#define AUXREG_ME_THRESHOLD_SKIP          (VRaptorDevs.MotionEstimator.AuxBase + 11)
#define AUXREG_ME_THRESHOLD_16x16         (VRaptorDevs.MotionEstimator.AuxBase + 12)
#define AUXREG_ME_THRESHOLD_BLOCK_PATTERN (VRaptorDevs.MotionEstimator.AuxBase + 13)
#define AUXREG_ME_THRESHOLD_SKIP_MAX      (VRaptorDevs.MotionEstimator.AuxBase + 14)
#define AUXREG_ME_CONTROL0                (VRaptorDevs.MotionEstimator.AuxBase + 15)
#define AUXREG_ME_CONTROL1                (VRaptorDevs.MotionEstimator.AuxBase + 16)
#define AUXREG_ME_SETUP                   (VRaptorDevs.MotionEstimator.AuxBase + 17)
#define AUXREG_ME_LAMBDA                  (VRaptorDevs.MotionEstimator.AuxBase + 18)
#define AUXREG_ME_COLLECTION_ID           (VRaptorDevs.MotionEstimator.AuxBase + 19)
#define AUXREG_ME_RESULT                  (VRaptorDevs.MotionEstimator.AuxBase + 20)
#define AUXREG_ME_MB_P0                   (VRaptorDevs.MotionEstimator.AuxBase + 21)
#define AUXREG_ME_MB_P1                   (VRaptorDevs.MotionEstimator.AuxBase + 22)
#define AUXREG_ME_MB_P2                   (VRaptorDevs.MotionEstimator.AuxBase + 23)
#define AUXREG_ME_MB_P3                   (VRaptorDevs.MotionEstimator.AuxBase + 24)
#define AUXREG_ME_SKIP_BR_ADDR            (VRaptorDevs.MotionEstimator.AuxBase + 25)
#define AUXREG_ME_16x16_BR_ADDR           (VRaptorDevs.MotionEstimator.AuxBase + 26)
#define AUXREG_ME_8x8_BR_ADDR             (VRaptorDevs.MotionEstimator.AuxBase + 27)
#define AUXREG_ME_SDM_BR_ADDR             (VRaptorDevs.MotionEstimator.AuxBase + 28)
#define AUXREG_ME_BASE_BR_ADDR            (VRaptorDevs.MotionEstimator.AuxBase + 29)
#define AUXREG_ME_D_POINTER               (VRaptorDevs.MotionEstimator.AuxBase + 30)
#define AUXREG_ME_A_POINTER               (VRaptorDevs.MotionEstimator.AuxBase + 31)
#define AUXREG_ME_CURRENT_POINTER         (VRaptorDevs.MotionEstimator.AuxBase + 32)
#define AUXREG_ME_16x8_BR_ADDR            (VRaptorDevs.MotionEstimator.AuxBase + 33)
#define AUXREG_ME_8x16_BR_ADDR            (VRaptorDevs.MotionEstimator.AuxBase + 34)
#define AUXREG_ME_DEBUG                   (VRaptorDevs.MotionEstimator.AuxBase + 35)

#define CHANREG_ME_REFERENCE_START         (MacroSetting_ChannelCmd_ArcToME | 0)
#define CHANREG_ME_CURRENT_START           (MacroSetting_ChannelCmd_ArcToME | 1)
#define CHANREG_ME_MB_SDM_ADDR             (MacroSetting_ChannelCmd_ArcToME | 2)
#define CHANREG_ME_REFERENCE_SDM_START     (MacroSetting_ChannelCmd_ArcToME | 3)
#define CHANREG_ME_THRESHOLD_SKIP          (MacroSetting_ChannelCmd_ArcToME | 4)
#define CHANREG_ME_THRESHOLD_16x16         (MacroSetting_ChannelCmd_ArcToME | 5)
#define CHANREG_ME_THRESHOLD_BLOCK_PATTERN (MacroSetting_ChannelCmd_ArcToME | 6)
#define CHANREG_ME_THRESHOLD_SKIP_MAX      (MacroSetting_ChannelCmd_ArcToME | 7)
#define CHANREG_ME_CONTROL0                (MacroSetting_ChannelCmd_ArcToME | 8)
#define CHANREG_ME_CONTROL1                (MacroSetting_ChannelCmd_ArcToME | 9)
#define CHANREG_ME_LAMBDA                  (MacroSetting_ChannelCmd_ArcToME | 10)
#define CHANREG_ME_COLLECTION_ID           (MacroSetting_ChannelCmd_ArcToME | 11)
#define CHANREG_ME_SETUP                   (MacroSetting_ChannelCmd_ArcToME | 12)
#define CHANREG_ME_D_POINTER               (MacroSetting_ChannelCmd_ArcToME | 13)
#define CHANREG_ME_A_POINTER               (MacroSetting_ChannelCmd_ArcToME | 14)
#define CHANREG_ME_CURRENT_POINTER         (MacroSetting_ChannelCmd_ArcToME | 15)
#define CHANREG_ME_DEBUG                   (MacroSetting_ChannelCmd_ArcToME | 16)

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

extern  unsigned short aanscaletables[2][32][64];
extern  unsigned short aanscaletables263[2][32][64];

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
CHANNEL_SERVICE	Service_ME_SetCollectionID(unsigned int collectionID)
{
    // enable channels
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

    // set the response function for the SDM branch address (channel out mb)
    branchAddr = (unsigned int)Service_ME_SDMResponse;

    if (HwMe == 1)
    {
    	   WriteToAuxRegME_Hw(AUXREG_ME_SDM_BR_ADDR, branchAddr);
    }
    else
    {
    	   WriteToAuxRegME_Sw(AUXREG_ME_SDM_BR_ADDR, branchAddr);
    }


    // set the response function for the skip branch address (channel out final)
    branchAddr = (unsigned int)Service_ME_SkipResponse;

    if (HwMe == 1)
    {
        WriteToAuxRegME_Hw(AUXREG_ME_SKIP_BR_ADDR, branchAddr);
    }
    else
    {
        WriteToAuxRegME_Sw(AUXREG_ME_SKIP_BR_ADDR, branchAddr);
    }


    // set the response function for the inter predictions;
    //    16x16 branch address (channel out final)
    //    8x8 branch address (channel out final)
    //    16x8 branch address (channel out final)
    //    8x16 branch address (channel out final)
    branchAddr = (unsigned int)Service_ME_InterResponse;

    if (HwMe == 1)
    {
        WriteToAuxRegME_Hw(AUXREG_ME_16x16_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_8x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_16x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Hw(AUXREG_ME_8x16_BR_ADDR, branchAddr);
    }
    else
    {
        WriteToAuxRegME_Sw(AUXREG_ME_16x16_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_8x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_16x8_BR_ADDR, branchAddr);
        WriteToAuxRegME_Sw(AUXREG_ME_8x16_BR_ADDR, branchAddr);
    }
}


//******************************************************************************
// Name:         Service_ME_PictureSetup
// Purpose:      Sets the reference and current locations in main memory and
//               reset the macroblock index position to zero.
// Arguments:    void
// Return Value: CHANNEL_SERVICE
//******************************************************************************
CHANNEL_SERVICE	Service_ME_PictureSetup(void)
{
    if (HwMe == 1)
    {
	    // setup ME reference and current locations in main memory

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
	    if (gArcFrameBuffers.YUVPos == 1)
	    	WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_START, (u_int32) gArcFrameBuffers.PhysOPRefY);
	    else
	    	WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_START, (u_int32) gArcFrameBuffers.PhysOPY);
	    
	    WriteToAuxRegME_Hw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.PhysIPY);
	#else
	    WriteToAuxRegME_Hw(AUXREG_ME_REFERENCE_START, (u_int32) gCH264Encoder.m_EncodedPictureBuffer->GetRefPicture()->m_y);
	    WriteToAuxRegME_Hw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.InputY);
	#endif
	
	    // reset ME unit to start of picture
	    WriteToAuxRegME_Hw(AUXREG_ME_MB_INDEX, 0);
    }
    else  // HwMe != 1
    {
        // setup ME reference and current locations in main memory
        WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_STRIDE, (u_int32) gCH264Encoder.m_EncodedPicture->m_hsize);
        WriteToAuxRegME_Sw(AUXREG_ME_MB_DIMENSION, (u_int32) (((((gCH264Encoder.m_EncodedPicture->m_hsize)+15) >> 4) << ME_MB_DIMENSION__MB_X_DIMENSION_SHIFT) |
                                                    ((((gCH264Encoder.m_EncodedPicture->m_vsize)+15) >> 4) << ME_MB_DIMENSION__MB_Y_DIMENSION_SHIFT)) );
    #ifdef ARC_ENCODER_API
        if (gArcFrameBuffers.YUVPos == 1)
        	WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_START, (u_int32) gArcFrameBuffers.PhysOPRefY);
        else
        	WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_START, (u_int32) gArcFrameBuffers.PhysOPY);
        
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.PhysIPY);
    #else
        WriteToAuxRegME_Sw(AUXREG_ME_REFERENCE_START, (u_int32) gCH264Encoder.m_EncodedPictureBuffer->GetRefPicture()->m_y);
        WriteToAuxRegME_Sw(AUXREG_ME_CURRENT_START, (u_int32) gArcFrameBuffers.InputY);
    #endif

        // reset ME unit to start of picture
        WriteToAuxRegME_Sw(AUXREG_ME_MB_INDEX, 0);
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
	u_int8 quantVal = cb->QuantValue;

	if (HwMe == 1)
	{
		WriteToChannelME_Hw(CHANREG_ME_MB_SDM_ADDR, (u_int32) &(cb->PixelCoeffBuffer));
	    WriteToChannelME_Hw(CHANREG_ME_REFERENCE_SDM_START, (u_int32) &(cb->PixelCoeffBuffer) + PCB_REFERENCE_Y);
	
		if (quantVal != lastQuantVal)
		{
		    if(gCH264Encoder.m_coding_standard > MPEG4) 
			{
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP, (u_int32) max(20,gCH264Encoder.m_SkipSadThresholdMult)*(h263_encoder_8X8_skip_threshold[cb->QuantValue]>>6));
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*h263_encoder_8X8_skip_threshold[cb->QuantValue]>>2);
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) h263_encoder_8X8_skip_threshold[cb->QuantValue]); // n.b. divide by 2 to take into account rounding in quantisation
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) (40*h263_encoder_8X8_skip_threshold[cb->QuantValue]>>6));
			}
			else // MPEG4
			{
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP, (u_int32)max(20,gCH264Encoder.m_SkipSadThresholdMult)*((mpeg4_encoder_8X8_skip_threshold[cb->QuantValue])>>6));
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*mpeg4_encoder_8X8_skip_threshold[cb->QuantValue]>>2);
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) mpeg4_encoder_8X8_skip_threshold[cb->QuantValue]); // n.b. divide by 2 to take into account rounding in quantisation
				WriteToChannelME_Hw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) ((40*mpeg4_encoder_8X8_skip_threshold[cb->QuantValue])>>6));
	
				//gCH264Encoder.m_umvplus = 1; // need this to ensure prediction diff is within range in ME
			}
			WriteToChannelME_Hw(CHANREG_ME_LAMBDA, (u_int32) (cb->QuantValue));
	    	lastQuantVal = quantVal;
		}
	
	    WriteToChannelME_Hw(CHANREG_ME_D_POINTER, (u_int32) lfb);
	
	
	    WriteToChannelME_Hw(CHANREG_ME_A_POINTER, (u_int32) lcb);
	    WriteToChannelME_Hw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
	
	    // start ME
	    WriteToChannelME_Hw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__START_ITER_MASK) << ME_CONTROL0__START_ITER_SHIFT) |
	                                                     ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT)) );
	}
	else // Software ME
	{
		WriteToChannelME_Sw(CHANREG_ME_MB_SDM_ADDR, (u_int32) &(cb->PixelCoeffBuffer));
	    WriteToChannelME_Sw(CHANREG_ME_REFERENCE_SDM_START, (u_int32) &(cb->PixelCoeffBuffer) + PCB_REFERENCE_Y);
	
		if (quantVal != lastQuantVal)
		{
		    if(gCH264Encoder.m_coding_standard > MPEG4) 
			{
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP, (u_int32) max(20,gCH264Encoder.m_SkipSadThresholdMult)*(h263_encoder_8X8_skip_threshold[cb->QuantValue]>>6));
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*h263_encoder_8X8_skip_threshold[cb->QuantValue]>>2);
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) h263_encoder_8X8_skip_threshold[cb->QuantValue]); // n.b. divide by 2 to take into account rounding in quantisation
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) (40*h263_encoder_8X8_skip_threshold[cb->QuantValue]>>6));
			}
			else // MPEG4
			{
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP, (u_int32)max(20,gCH264Encoder.m_SkipSadThresholdMult)*((mpeg4_encoder_8X8_skip_threshold[cb->QuantValue])>>6));
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_16x16, (u_int32) gCH264Encoder.m_Skip8x8ThresholdMult*mpeg4_encoder_8X8_skip_threshold[cb->QuantValue]>>2);
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_BLOCK_PATTERN, (u_int32) mpeg4_encoder_8X8_skip_threshold[cb->QuantValue]); // n.b. divide by 2 to take into account rounding in quantisation
				WriteToChannelME_Sw(CHANREG_ME_THRESHOLD_SKIP_MAX, (u_int32) ((40*mpeg4_encoder_8X8_skip_threshold[cb->QuantValue])>>6));
	
				//gCH264Encoder.m_umvplus = 1; // need this to ensure prediction diff is within range in ME
			}
			WriteToChannelME_Sw(CHANREG_ME_LAMBDA, (u_int32) (cb->QuantValue));
	    	lastQuantVal = quantVal;
		}
	
	    WriteToChannelME_Sw(CHANREG_ME_D_POINTER, (u_int32) lfb);
	
	
	    WriteToChannelME_Sw(CHANREG_ME_A_POINTER, (u_int32) lcb);
	    WriteToChannelME_Sw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
	
	    // start ME
		gArcMotionEstimationModel.Load_Current_MB_Luma(cb, 0);
		gArcMotionEstimationModel.Software_ME_control(cb);

	    /**********************************************************************************************
	    WriteToChannelME_Sw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__START_ITER_MASK) << ME_CONTROL0__START_ITER_SHIFT) |
	                                                     ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT)) );

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
	    
	
	    // get second ME channel response
	    MEResponse = gArcMotionEstimationModel.GetChannelResponse();
	
	    if (MEResponse == NULL)
	    {
	        fprintf(stderr, "ArcChannelMotionEstimation.cpp: Error - Can't get second ME channel response\n");
	        assert(false);
	    }
	    
	    u_int16 cost = MEResponse->RegisterData[0].RegValue & 0xffff;
	
	    // check to see if block pattern is enabled
	    u_int16 blkPattern;
	
	    if (MEResponse->RegisterData[2].valid)
	    {
	        blkPattern = MEResponse->RegisterData[1].RegValue & 0xf;
	    }
	    else
	    {
	        blkPattern = 0;
	    }
	
	    // check winning mode
	    u_int8 candidateType = (gArcMotionEstimationModel.GetAuxReg(AUXREG_ME_RESULT) >> 16) & 0x7;
	
	    if (candidateType == 0) // skip
	    {
	        SET_Generate_ME_SkipResponse_cost(cost);
	        SET_Generate_ME_SkipResponse_blkPattern(blkPattern);
	        _vrun(MACRO_Generate_ME_SkipResponse);
	    }
	    else
	    {
	        SET_Generate_ME_InterResponse_cost(cost);
	        SET_Generate_ME_InterResponse_blkPattern(blkPattern);
	        _vrun(MACRO_Generate_ME_InterResponse);
	    }
	    ***********************************************************************************************/
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
	    WriteToChannelME_Hw(CHANREG_ME_MB_SDM_ADDR, (u_int32) &(cb->PixelCoeffBuffer));
	    WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((gCH264Encoder.m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
	                                                  ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                  ((gCH264Encoder.m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
	                                                  ((gCH264Encoder.m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
	                                                  ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                  ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
													  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
													  ((gCH264Encoder.m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
													  ));
	
	    WriteToChannelME_Hw(CHANREG_ME_D_POINTER, (u_int32) lfb);
	
	
	    WriteToChannelME_Hw(CHANREG_ME_A_POINTER, (u_int32) lcb);
	    WriteToChannelME_Hw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);
	
	    WriteToChannelME_Hw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__START_ITER_MASK) << ME_CONTROL0__START_ITER_SHIFT) | // start ME but don't perform any actual motion estimation
	                                                     ((1 & ME_CONTROL0__NO_ME_MASK) << ME_CONTROL0__NO_ME_SHIFT) |
	                                                     ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT) |
	                                                     ((0 & ME_CONTROL0__PAR_ID_MASK) << ME_CONTROL0__PAR_ID_SHIFT) |
	                                                     ((0x00 & ME_CONTROL0__MV_X_MASK) << ME_CONTROL0__MV_X_SHIFT) |
	                                                     ((0x00 & ME_CONTROL0__MV_Y_MASK) << ME_CONTROL0__MV_Y_SHIFT)) );
	}
	else  // Software ME
	{
	    WriteToChannelME_Sw(CHANREG_ME_MB_SDM_ADDR, (u_int32) &(cb->PixelCoeffBuffer));
	    WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((gCH264Encoder.m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
	                                                  ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                  ((gCH264Encoder.m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
	                                                  ((gCH264Encoder.m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
	                                                  ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                  ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
													  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
													  ((gCH264Encoder.m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
													  ));

	    WriteToChannelME_Sw(CHANREG_ME_D_POINTER, (u_int32) lfb);


	    WriteToChannelME_Sw(CHANREG_ME_A_POINTER, (u_int32) lcb);
	    WriteToChannelME_Sw(CHANREG_ME_CURRENT_POINTER, (u_int32) cb);

	    /****************************************************************************
	    WriteToChannelME_Sw(CHANREG_ME_CONTROL0, (u_int32) (((1 & ME_CONTROL0__START_ITER_MASK) << ME_CONTROL0__START_ITER_SHIFT) | // start ME but don't perform any actual motion estimation
	                                                     ((1 & ME_CONTROL0__NO_ME_MASK) << ME_CONTROL0__NO_ME_SHIFT) |
	                                                     ((1 & ME_CONTROL0__REF_PIX_CACHE_EN_MASK) << ME_CONTROL0__REF_PIX_CACHE_EN_SHIFT) |
	                                                     ((0 & ME_CONTROL0__PAR_ID_MASK) << ME_CONTROL0__PAR_ID_SHIFT) |
	                                                     ((0x00 & ME_CONTROL0__MV_X_MASK) << ME_CONTROL0__MV_X_SHIFT) |
	                                                     ((0x00 & ME_CONTROL0__MV_Y_MASK) << ME_CONTROL0__MV_Y_SHIFT)) );
	    ******************************************************************************/
	    // DMA in current luma macroblock
		gArcMotionEstimationModel.Load_Current_MB_Luma(cb,1);
		
		// wait for current mb dma
		_ASM("vdmawait 0x1, 0x7f"); // wait for only 1 dmas in and max dma outs left in fifos

		// Generate the SDM response 
	    _vrun(MACRO_Generate_ME_SDMResponse);
	}
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
	 int cbIdx = MgrMPC.IdxFromME = MgrMPC.MeQueue.Pull();
	 cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];
	 MgrMPC.ActMPC[cbIdx].LumaInpOk = true;

#ifdef _DEBUG_VRAPTORISATION
	 printf("Service_ME_SDMResponse: MPC Index = %d\n", cbIdx);
#endif
	 
    if (MBCT_MPEG4_INTRA == cb.MBCodingType)
    {
    	MgrMPC.ActMPC[MgrMPC.IdxFromME].IntraOk = true;
    	MgrMPC.ActMPC[MgrMPC.IdxFromME].MBType = MBCT_MPEG4_INTRA;

        // For the very first MB in the frame, kick off SIMD operation from here.
        // For the following MB's, start it at the tail end of the current MB
    	if ((MgrMPC.MacroBlockStartedCount == 1) || (MgrMPC.ActMPC[MgrMPC.ActMPC[cbIdx].LeftIdx].InvTransformsOk))
    	{
    		SET_CallBackDoFwdTransforms_BufIdx(cbIdx);
    		_vrun(MACRO_CallBackDoFwdTransforms);
    	}
    }
    // But for inter blocks we need to wait until the second ME response (inter/skip)
    // before we can do the transforms. But in the meantime, we can start the intra
    // cost evaluation here
    else if ((MgrMPC.MacroBlockStartedCount == 1) || (MgrMPC.ActMPC[MgrMPC.ActMPC[cbIdx].LeftIdx].InvTransformsOk))
    {
    	// setup pointers for the current, left and above macroblocks in the simd
    	_vr00 = _vmovw(cbIdx, 1);
    	_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].Column, 2);
    	_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].LeftIdx, 4);
    	_vrun(MACRO_SetCurrentMPC);

    	//_vr00 = _vmovw(gCH264Encoder.m_coding_standard == MPEG4 ? 0 : 0xffff, 8);
    	if(gCH264Encoder.m_coding_standard == MPEG4 ) 
    	{
    		_vrun(MACRO_IntraModeCostmp4);
    	}
    	else 
    	{
    		_vrun(MACRO_IntraModeCost263);
    	}
	    SET_CallBackIntraComplete_BufIdx(cbIdx);
	    _vrun(MACRO_CallBackIntraComplete);
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
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
    MgrMPC.ActMPC[cIdx].MotionEstOk = true;    

#ifdef _DEBUG_VRAPTORISATION
	 printf("Service_ME_SkipResponse: MPC Index = %d\n", cIdx);
#endif
	 
	 gCH264Encoder.m_bestInterCost = reg0;

    // Perform Motion Estimation for Chroma Skip
    gCH264Encoder.MotionEstimateCalculateSkippedChromaCost();

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
    gCH264Encoder.m_bestInterCost = reg0;

    int cIdx = MgrMPC.IdxFromME;
	int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
	int lastQuant;
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
    MgrMPC.ActMPC[cIdx].MotionEstOk = true;
    MgrMPC.ActMPC[cIdx].SkippedOk = true;	// Not needed
    
#ifdef _DEBUG_VRAPTORISATION
	 printf("Service_ME_InterResponse: MPC Index = %d\n", cIdx);
#endif
	 



	if ( MgrMPC.ActMPC[cIdx].IntraOk )
	{
		// Intra/Inter decision
        gCH264Encoder.ChooseInterOrIntra(cIdx);

        // If we go for inter
        if (MBCT_MPEG4_INTRA != cb.MBCodingType)
        {
    	    gCH264Encoder.MakeInterChromaPrediction263(cIdx);

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

		if( MgrMPC.ActMPC[cIdx].MB_number == gCH264Encoder.m_FirstMacroblockInSlice[0])
		{
			lastQuant = gCH264Encoder.m_LastTransmittedQuant;
		}
		else
		{
			lastQuant = MgrMPC.ActMPC[leftIdx].QuantValue;
		}
		int dquant = cb.QuantValue - lastQuant;
		if((gCH264Encoder.m_coding_standard >= MPEG4) && !gCH264Encoder.m_h263_modified_quant) // may need to restrict quantiser
		{
			if(dquant < -2)
			{
				dquant = -2; // limited
				cb.QuantValue = lastQuant + dquant;
			}
			if(dquant > 2)
			{
				dquant = 2; // limited
				cb.QuantValue = lastQuant + dquant;
			}
		}
		
		MgrMPC.ActMPC[cIdx].Dquant = dquant;
		MgrMPC.ActMPC[cIdx].QuantValue = cb.QuantValue;


        // MPEG4 does not allow quant change with 4 MVs
        if((gCH264Encoder.m_coding_standard == MPEG4) && (cb.MBCodingType == MBCT_MPEG4_INTER8X8)
		    && (MgrMPC.ActMPC[MgrMPC.IdxFromME].Dquant != 0))
	    {
            _vr00 = _vmovw(MgrMPC.IdxFromME, 1);
	        _vr00 = _vmovw(MgrMPC.ActMPC[MgrMPC.IdxFromME].LeftIdx, 2);
            _vr02 = _vmovw(MgrMPC.ActMPC[MgrMPC.IdxFromME].Dquant, 1);
            _vrun(MACRO_RevertQuantValue);
		    MgrMPC.ActMPC[MgrMPC.IdxFromME].Dquant = 0;
			MgrMPC.ActMPC[MgrMPC.IdxFromME].QuantValue = cb.QuantValue;
        }

        // Kick off fwd transform
        SET_CallBackDoFwdTransforms_BufIdx(MgrMPC.IdxFromME);
        _vrun(MACRO_CallBackDoFwdTransforms);
    }

}
