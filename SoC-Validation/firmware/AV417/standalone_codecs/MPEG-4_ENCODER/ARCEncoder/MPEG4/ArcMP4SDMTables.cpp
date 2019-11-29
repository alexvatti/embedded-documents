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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "EnumTypes.h"
#include "ArcEEtables.h"
#include "ArcEEOffsetTables.h"
#include "VRaptorInfo.h"
#include "ArcChannelRoutines.h"

#include <ctype.h>
#include <arc/arc_simd.h>
#include "ArcSDMTables_MPA.h"



// offsets into PixelCoeffBuffer of circ buffer, for each 8x8 mpeg4/h263 block
unsigned short MP4RawBlockOffset[6] = // when raw pixel data
{
	PCB_CURRENT_Y + 0,
	PCB_CURRENT_Y + (PCB_CURRENT_STRIDE/2),
	PCB_CURRENT_Y + 8 * PCB_CURRENT_STRIDE,
	PCB_CURRENT_Y + 8 * PCB_CURRENT_STRIDE + (PCB_CURRENT_STRIDE/2),
	PCB_CURRENT_U,
	PCB_CURRENT_V
};
unsigned short MP4DiffBlockOffset[6] = // when difference predicted data and result of DCT
{
	PCB_RESIDUAL_Y + 0,
	PCB_RESIDUAL_Y + (PCB_RESIDUAL_STRIDE/2),
	PCB_RESIDUAL_Y + 8 * PCB_RESIDUAL_STRIDE,
	PCB_RESIDUAL_Y + 8 * PCB_RESIDUAL_STRIDE + (PCB_RESIDUAL_STRIDE/2),
	PCB_RESIDUAL_U,
	PCB_RESIDUAL_V
};
unsigned short MP4PredBlockOffset[6] = // when difference predicted data and result of DCT
{
	PCB_REFERENCE_Y + 0,
	PCB_REFERENCE_Y + 8,
	PCB_REFERENCE_Y + 8 * PCB_REFERENCE_STRIDE,
	PCB_REFERENCE_Y + 8 * PCB_REFERENCE_STRIDE + 8,
	PCB_REFERENCE_U,
	PCB_REFERENCE_V
};
unsigned short MP4FinalBlockOffset[6] = // when difference predicted data and result of DCT
{
	PCB_RECONSTRUCTED_Y + 0,
	PCB_RECONSTRUCTED_Y + 8,
	PCB_RECONSTRUCTED_Y + 8 * 16,
	PCB_RECONSTRUCTED_Y + 8 * 16 + 8,
	PCB_RECONSTRUCTED_Y + 16 * 16,//PCB_RECONSTRUCTED_U, if deblock
	PCB_RECONSTRUCTED_Y + 16 * 16 + 8//PCB_RECONSTRUCTED_V if deblock
};
//-----------------------------------------------------------------------------------------------------------

const short software_me_offset[2][8] =
{
	{ // horiz offsets
		-1, // 1, -1
		0, // 0, -1
		1, //-1, -1

		-1, // 1,  0
		//0, // 0,  0
		1, //-1,  0

		-1, // 1,  1
		0, // 0,  1
		1 //-1,  1
	},
	{
		// 2nd step positions relative to first step best location
		-1,	// 1, -1
		-1,		// 0, -1
		-1,	//-1, -1

		0, // 1,  0
		//0, // 0,  0
		0, //-1,  0

		1,	// 1,  1
		1,		// 0,  1
		1		//-1,  1

	}
};
const short software_me_lanes[8] = {1,2,4,8,16,32,64,128};



//Called via ptr
static void HandleServiceCall_RealFunction()
{
 _ASM("chop");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
 _ASM("chex");
}
void (*HandleServiceCall)() = HandleServiceCall_RealFunction;


struct SDM_WorkArea	* WorkAreaSDM = NULL;

void SDM_WorkArea::Init_SDM_WorkArea()
{
   // printf("SDM memory required: %dK\n", (sizeof(SDM_WorkArea) + 1023) /1024);
   // printf("sizeof(cMPCfb) %04x\n",sizeof(cMPCfb));
   //  printf("sizeof(cMPCcb) %04x\n",sizeof(cMPCcb));
	//PRINTSDMOFS(FrameTabDMA);
	//PRINTSDMOFS(IntraPredictionLuma);
	//PRINTSDMOFS(IntraPredictionChroma);
	//PRINTSDMOFS(IntraPredictionResidsLuma);
	//PRINTSDMOFS(IntraPredictionResidsChroma);
	//PRINTSDMOFS(AlphaBetaCoTable);
	//PRINTSDMOFS(h264_frame_scan_quantiser_table);
	//PRINTSDMOFS(h264_frame_scan_inverse_quantiser_table);
	//PRINTSDMOFS(h264_4x4_transmission_to_raster_table);
	//PRINTSDMOFS(h264_quantiser_chroma_mapping_tableClipping);
	//PRINTSDMOFS(FixedBuffs);
	//PRINTSDMOFS(CircularBuffs);

	WorkAreaSDM = (SDM_WorkArea *)VRaptorDevs.SDM_Base;

	for(int d = 0; d< DEBLOCK_Q_SIZE;d++)
		WorkAreaSDM->DeblockQ[d] = -1;

}



// Copy circular buffer MPC to fixed buffer for MPEG4/H263 encode
void SDM_WorkArea::CopyToFixedAfterReconstructMP4(int cirIdx)
{
#ifdef HARDWARE_SIMD
//// setup pointers for the current, left and above macroblocks in the simd
	_vr00 = _vmovw(cirIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cirIdx].Column, 2);
	_vr00 = _vmovw(MgrMPC.ActMPC[cirIdx].LeftIdx, 4);
	_vrun(MACRO_SetCurrentMPC);
	_vrun(MACRO_CopyToFixedAfterReconstructMP4);
#else
    int i;
    int col = MgrMPC.ActMPC[cirIdx].Column;
	cMPCfb &abv = FixedBuffs[col];
	cMPCcb &cb = CircularBuffs[cirIdx];

    abv.LeftMB = cb.LeftMB;
	abv.AboveMB = cb.AboveMB;
	abv.AboveRightMB = cb.AboveRightMB;
	abv.AboveLeftMB = cb.AboveLeftMB;

	abv.NonZeroCoeffLuma = cb.NonZeroCoeffLuma;
	abv.NonZeroCoeffChroma = cb.NonZeroCoeffChroma;

	abv.UV_DC_NonZeroCoeff = cb.UV_DC_NonZeroCoeff;

    for(i =0; i< 4; i++)
		abv.Y_IntraPredDC_Coeff[i] = cb.Y_IntraPredDC_Coeff[i];
	abv.UV_IntraPredDC_Coeff[0] = cb.UV_IntraPredDC_Coeff[0];
	abv.UV_IntraPredDC_Coeff[1] = cb.UV_IntraPredDC_Coeff[1];

	abv.MBCodingType = cb.MBCodingType;
	abv.IntraMode = cb.IntraMode;
	abv.QuantValue = cb.QuantValue;
	abv.CollectionID = cb.CollectionID;

	//// Motion Vectors Data Table
	for(i =0; i< 4; i++)
	{
		abv.mv[i] = cb.mv[i];
		abv.mvp[i] = cb.mvp[i];
	}
#endif // HARDWARE_SIMD
}





//-      --      --      --      --      --      --      --      --      --      --      -
// Temp functions to queue / delay dablock
void SDM_WorkArea::AddForDeblock(int cIdxDb)
{
	// Danger quick hack (slow, safe-ish) fixed delay queue
#ifdef _DEBUG
	if(DeblockQ[DEBLOCK_Q_SIZE-1] != -1)
	{
		printf("Bad: Deblock delay queue overwrite\n");
	}
#endif
	DeblockQ[DEBLOCK_Q_SIZE-1] = cIdxDb;
}



int SDM_WorkArea::GetForDeblock()
{
	// Damger quick hack (slow, safe-ish) fixed delay queue
	int out = DeblockQ[0];
	for(int d = 1; d< DEBLOCK_Q_SIZE;d++)
		DeblockQ[d-1] = DeblockQ[d];
	DeblockQ[DEBLOCK_Q_SIZE-1] = -1;
	return out;
}



//-----------------------------------------------------------------------------------------------------------


// Manager for MPC data structures
MPC_Manager MgrMPC;

MPC_Manager::MPC_Manager()
{
	AllocCirBufIdx = 0;
	for(int m = 0; m < SDM_WorkArea::MAX_CIRCULAR_BUFFS; m++)
	{
		ActMPC[m].IsUsed = false;
	}
    Picture.SliceGroupMap = NULL;
}



MPC_Manager::~MPC_Manager()
{
    free(Picture.SliceGroupMap);
}





// Returns index of free MPC in array or -1 for error
int MPC_Manager::AllocMPC()
{
	int sIdx = AllocCirBufIdx+1;
	if(sIdx == SDM_WorkArea::MAX_CIRCULAR_BUFFS)
	{
		sIdx = 0;
	}

	while(sIdx != AllocCirBufIdx)
	{
		if(!ActMPC[sIdx].IsUsed)
		{
			NextCurrentMacroblock();

			ActMPC[sIdx].IsUsed = true;
			ActMPC[sIdx].LumaInpOk = false;
			ActMPC[sIdx].IntraOk = false;
			ActMPC[sIdx].MotionEstOk = false;
			ActMPC[sIdx].SkippedOk = false;
			ActMPC[sIdx].FwdTransformsOk = false;
			ActMPC[sIdx].InvTransformsOk = false;
			ActMPC[sIdx].EntropyEncodeOk = false;
			ActMPC[sIdx].EENotStarted = false;

            ActMPC[sIdx].LeftIdx = AllocCirBufIdx; // Assume old index is to left, should only be accessed in this case
            ActMPC[sIdx].Column = CurrentMeMB.Col;
            ActMPC[sIdx].Row = CurrentMeMB.Row;
			ActMPC[sIdx].MB_number = CurrentMeMB.Row*Picture.Width + CurrentMeMB.Col;
            ActMPC[sIdx].MBType = MBCT_MPEG4_UNKNOWN;
            ActMPC[sIdx].State = InME;
			AllocCirBufIdx = sIdx;
            cMPCcb &cb = WorkAreaSDM->CircularBuffs[sIdx];

            _vr00 = _vmovw(sIdx, 1);
            _vr00 = _vmovw(CurrentMeMB.Col, 2);
            _vr00 = _vmovw(ActMPC[sIdx].LeftIdx, 4);
            _vrun(MACRO_SetCurrentMPC);
            _vrun(MACRO_InitCurrentMPC);

			return sIdx;
		}
		if(++sIdx == SDM_WorkArea::MAX_CIRCULAR_BUFFS)
		{
			sIdx = 0;
		}
	}
#ifdef _DEBUG
	printf("This could be bad!!! Run out of MPCs\n");
#endif
	return -1;
}



void MPC_Manager::FreeMPC(int idx)
{
#ifdef _DEBUG
	if(!ActMPC[idx].IsUsed)
	{
		printf("This could be bad!!! MPC is already freed\n");
	}
#endif
	ActMPC[idx].IsUsed = false;
}

// Sets picture size in units of MB
void MPC_Manager::Init(short wid, short hgt)
{
	AllocCirBufIdx = 0;
	for(int m = 0; m < SDM_WorkArea::MAX_CIRCULAR_BUFFS; m++)
	{
		ActMPC[m].IsUsed = false;
		ActMPC[m].BufferLimit = 0;
	}

	Picture.Width = wid;
	Picture.Hgt = hgt;
    if(Picture.SliceGroupMap)
        free(Picture.SliceGroupMap);

    // Allocate slice group map (Could be static array at later date)
    Picture.SliceGroupMap = static_cast<signed char*>( malloc(wid*hgt) );
    assert(Picture.SliceGroupMap);
    if(Picture.SliceGroupMap)
    {
        for(int i = 0; i < wid*hgt; i++)
        {
            Picture.SliceGroupMap[i] = 0;
        }
    }
}


void MPC_Manager::ResetTopLeft()
{
	CurrentMeMB.Col = -1;
	CurrentMeMB.Row = 0;
	MacroBlockStartedCount = 0;
	MacroBlocksCompleteCount = 0;
	MacroBlockThroughSimdCount = 0;
	MacroBlockThroughDblkCount = 0;
}


// Step on to next macroblcok for prediction
void MPC_Manager::NextCurrentMacroblock()
{
	if(++CurrentMeMB.Col == Picture.Width)
	{
		CurrentMeMB.Col = 0;
		CurrentMeMB.Row++;
	}
}

void MPC_Manager::SetupMacroBlock()
{
    int bNumber = CurrentMeMB.Row * Picture.Width +CurrentMeMB.Col;
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[IdxSetupME];
    cb.WorkArea.SliceGroup = MgrMPC.Picture.SliceGroupMap[bNumber];

}
