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

#ifndef	ARC_CHANNEL_ROUTINES_H
#define	ARC_CHANNEL_ROUTINES_H


#define CHANNEL_SERVICE extern "C" void
#include <stdio.h>
#include "ArcMPC.h"

#include <arc/arc_avu.h>

#include "ArcMotionEstimationModel.h"
#include "VRaptorInfo.h"
#include "ArcMacroRecordSettings.h"


// ME Auxiliary Registers and Channel Definitions
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



// Buffer management --------------------------------------------------------------------------------------

//CHANNEL_SERVICE  Service_Dummy();

CHANNEL_SERVICE  Service_ChromaDMA();


// Prediction ---------------------------------------------------------------------------------------------

CHANNEL_SERVICE Service_GenerateIntra16x16Prediction(int intraValidFlag, bool m_UseIntraDCpredOnly,bool m_UseIntraVertPredOnly, bool m_UseWhitening);
CHANNEL_SERVICE Service_MakeLumaPredictionInterError16X16();

CHANNEL_SERVICE Service_IntraComplete(int sIdx);

CHANNEL_SERVICE Service_DoFwdTransforms(int sIdx);
CHANNEL_SERVICE Service_StartME(int sIdx);
CHANNEL_SERVICE Service_StartDeblock(int sIdx);
CHANNEL_SERVICE Service_ReleaseBuffer(int sIdx);


CHANNEL_SERVICE Service_UseInter(int sIdx);
CHANNEL_SERVICE Service_UseIntra(int sIdx);

// Quantisation & transforms ------------------------------------------------------------------------------

struct TransformQuantInfo
{
	unsigned int LumaMod;
	int16        LumaDiv;
	//unsigned int LumaShiftNormal;
	unsigned int LumaF;

	unsigned int ChromaQp;
	unsigned int ChromaMod;
	int16        ChromaDiv;
	//unsigned int ChromaShift;
	int ChromaF;

	const short * LumaInverseQuantTable;
	const short* LumaQuantTable;
	const short * ChromaInverseQuantTable;
	const short * ChromaQuantTable;

	void SetupQuant(int sliceType, int chromaQpIndexOffset);
};

extern TransformQuantInfo TransQuantData;

// TEMP control as param will be SMD buffer index or ptr
CHANNEL_SERVICE	Service_SetupQuant(int sliceType, int chromaQpIndexOffset);	// Setup quant tables etc
CHANNEL_SERVICE Service_SmallCoefRemovalLuma(int first_coefficient);



// ######################################################
//
// EE SPECIFIC DATA STRUCTURES AND RESPONSE FUNCTIONS
//
//#######################################################
//


// Functions for EE and channels

void Initialise_Channels(void);
void Initialise_Entropy_Encoder(unsigned int packer_id);
void WriteRBSPTrailingBits(void);
void WriteBitStreamData(unsigned int reg1);
void WriteDecodedPictureBufferManagement(void);
int CheckForSliceEnd(unsigned int unit_sel);

CHANNEL_SERVICE	Service_WriteSequenceHeader(void);
CHANNEL_SERVICE	Service_WriteUserData(void);
CHANNEL_SERVICE	Service_WritePictureParameter(unsigned char picture_parameter_set_id);
CHANNEL_SERVICE Service_WriteSliceHeader(unsigned int first_mb_in_slice, int mbNum);
CHANNEL_SERVICE	Service_WriteMacroblock(int cirIdx);
CHANNEL_SERVICE	Service_InverseTransforms(int cirIdx);

CHANNEL_SERVICE	Service_SequenceHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_PicParameterHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_UserDataResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);

CHANNEL_SERVICE	Service_SliceHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_SliceEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_MacroBlockEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);

// MPEG4/H263 specific Service routines
CHANNEL_SERVICE	Service_WriteMpeg4VolHeader(int vo_number, int vol_number);
CHANNEL_SERVICE	Service_Mpeg4VolHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_WriteMpeg4UserData();
CHANNEL_SERVICE	Service_Mpeg4UserDataResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);


CHANNEL_SERVICE	Service_WriteMpeg4PictureHeader(unsigned int quant);
CHANNEL_SERVICE	Service_Mpeg4PictureHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_WriteMpeg4PacketHeader(int mb_number, unsigned char quant_value);
CHANNEL_SERVICE	Service_Mpeg4PacketHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_PictureEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_WriteMacroblockMP4(int cIdx, int dquant);

//******************************************************************************
// ME ChannelRountines

CHANNEL_SERVICE	Service_ME_Initialise(void);
CHANNEL_SERVICE	Service_ME_SetCollectionID(unsigned int collectionID);
CHANNEL_SERVICE	Service_ME_SetBranchAddresses(void);
CHANNEL_SERVICE	Service_ME_StreamSetup(void);
CHANNEL_SERVICE	Service_ME_PictureSetup(void);
CHANNEL_SERVICE	Service_ME_InterIteration(cMPCcb *cb, cMPCcb *lcb, cMPCfb *lfb);
CHANNEL_SERVICE	Service_ME_IntraIteration(cMPCcb *cb, cMPCcb *lcb, cMPCfb *lfb);

CHANNEL_SERVICE	Service_ME_SDMResponse(unsigned int reg0, unsigned int reg1);
CHANNEL_SERVICE	Service_ME_SkipResponse(unsigned int reg0, unsigned int reg1);
CHANNEL_SERVICE	Service_ME_InterResponse(unsigned int reg0, unsigned int reg1);

CHANNEL_SERVICE Service_MakeLumaPredictionError16X16();

void DoIntraPrediction(int validBits, int cIdx);

CHANNEL_SERVICE Service_InitTimer(int n);
CHANNEL_SERVICE Service_StopTimer();
CHANNEL_SERVICE Service_TimerLimitResponse(unsigned int reg0);
CHANNEL_SERVICE Service_DisableTimer();
CHANNEL_SERVICE Service_EnableTimer();
//******************************************************************************
// General purpose channel rountines

// Write to channel #define, merged as one for both EE and ME
// Later on when every thing works on hardware, then only this #define needs changing
// No other part of the code needs to be changed

#define SendChannelCommandEE(data, cmd) _chsd((data), (cmd))
#define WriteToAuxRegEE(addr,data) _sr((data), (addr))

#define WriteToChannelME_Hw(cmd, data) _chsd((data), (cmd))
#define WriteToAuxRegME_Hw(addr, data) _sr((data), (addr))

#define WriteToChannelME_Sw(cmd, data) gArcMotionEstimationModel.SendChannelCommand((data), (cmd))
#define WriteToAuxRegME_Sw(addr, data) gArcMotionEstimationModel.SetAuxReg((addr), (data))


// ME & EE Channel Write Commands
#define WriteToChannelEE(cmd, data) SendChannelCommandEE((data), (cmd))


CHANNEL_SERVICE	Service_WriteH263PictureHeader(unsigned int quant);
CHANNEL_SERVICE	Service_H263PictureHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);
CHANNEL_SERVICE	Service_WriteH263GobHeader(unsigned int quant);
CHANNEL_SERVICE	Service_H263GobHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2);

#if defined HARDWARE_EE || HARDWARE_ME
CHANNEL_SERVICE Service_GetChannelResponse(void);
#endif

CHANNEL_SERVICE Service_GetChannelResponseModel(void);


#ifndef ARC_ENCODER_API
extern FILE *bitstream_file;
#endif

CHANNEL_SERVICE Service_CalculateSkippedChromaCost (unsigned int r0, unsigned int r1);


#endif //ARC_CHANNEL_ROUTINES_H
