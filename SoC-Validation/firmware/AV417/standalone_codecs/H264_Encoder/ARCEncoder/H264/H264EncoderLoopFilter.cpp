/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderLoopFilter.cpp								*/
/* DESCRIPTION:		H.264 Encoder Loop Filter functions						*/
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


#define ENCODER_LOOP_FILTER

#include	"H264LoopFilter.h"
#include "ArcMPC.h"
#include <ctype.h>
#include <arc/arc_simd.h>
#include "ArcSDMTables_MPA.h"
#include "ArcDeblock_MPA.h"
#include "ArcFrameBuffers.h"
#include "../ARC/ArcChannelRoutines.h"
#include "VRaptorInfo.h"

//Called via ptr
extern void (*HandleServiceCall)();





/* ------------------------------------------------------------------------ */
/* FUNCTION:	DeblockAndOutput						
 DESCRIPTION:	Performs loop filtering on the whole Non-MB-AFF frame.
				Must be called after complete frame has been 
				encoded/decoded.
				When Q is appended to a variable, it indicates a parameter
				of the current block or macroblock
				When P is appended to a variable, it indicates a parameter
				of the neighbouring block or macroblock */

/* ------------------------------------------------------------------------ */

void DeblockAndOutput(CBTPicture * picture, int dbIdx)
{
    cMPCcb &dbcb = WorkAreaSDM->CircularBuffs[dbIdx];
    int dbLeftIdx = MgrMPC.ActMPC[dbIdx].LeftIdx;
    cMPCcb &dbLeftcb = WorkAreaSDM->CircularBuffs[dbLeftIdx];
    int col = MgrMPC.ActMPC[dbIdx].Column;
    cMPCfb &dbfb = WorkAreaSDM->FixedBuffs[col];
    
   
// Init current MPC to be deblock
    SetSCQ(SCQ1);
	_vr00 = _vmovw(dbIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vr00 = _vmovw(dbLeftIdx, 4);
	_vrun(MACRO_SetCurrentMPC1);
	_vrun(MACRO_CopyToFixedAfterReconstructH264);
	SET_DeblockMacroBlockSetup_chromaQIO(MgrMPC.Deblock.ChromaQ_IdxOffset);
	SET_DeblockMacroBlockSetup_DeblockMode(MgrMPC.Deblock.Mode);
	_vrun(MACRO_DeblockMacroBlockSetup);
	
    _vrun(MACRO_DeblockFetchPixelsFromNeigbours);

    _vrun(MACRO_HorizontalDeblock);
    _vrun(MACRO_VerticalDeblock);
	

    // Output deblocked version to picture
    {        // pass parameters
       _vr02 = _vmovw(col, 0x55);       // Column 
       _vr02 = _vmovw(MgrMPC.Deblock.Row, 0xAA);       // Row
       _vr03 = _vmovw(picture->m_hsize,   0x55);       // Horizontal size
       _vr03 = _vmovw(picture->m_vsize,   0xAA);       // Vertical size
       _vr04 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y + 8), 0x1);   // SDM start address for Y
       _vr04 = _vmovw(0, 0x2);   //  Need to support 32  bit intrinsics SDM start address
       
       //  SDM start address for U
       _vr05 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_U + 4), 0x1);   // SDM start address for U
       _vr05 = _vmovw(0, 0x2);   //  Need to support 32  bit intrinsics SDM start address
       
//     SDM start address for U
       _vr06 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_V + 4), 0x1);   // SDM start address for U
       _vr06 = _vmovw(0, 0x2);   // HCK Need to support 32  bit intrinsics SDM start address

       if (!gArcFrameBuffers.YUVPos)
       {
           _vr07 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
           _vr08 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
           _vr09 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
       }
       else
       {
           _vr07 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
           _vr08 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
           _vr09 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
       }

       _vrun(MACRO_WriteDeblockedBlock);
       SetSCQ(0);
    }
}






// Output pixels to frame buffer from reconstructed MPC , performing no deblock
void OutputOnly(CBTPicture * picture, int dbIdx)
{
    // Output non deblocked version
    cMPCcb &dbcb = WorkAreaSDM->CircularBuffs[dbIdx];
    int col = MgrMPC.ActMPC[dbIdx].Column;
    int dbLeftIdx = MgrMPC.ActMPC[dbIdx].LeftIdx;


    // pass parameters
    SetSCQ(SCQ1);
	_vr00 = _vmovw(dbIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vr00 = _vmovw(dbLeftIdx, 4);
	_vrun(MACRO_SetCurrentMPC1);
	_vrun(MACRO_CopyToFixedAfterReconstructH264);

   _vr00 = _vmovw(MgrMPC.Deblock.Col, 0x1);       // Column 
   _vr00 = _vmovw(MgrMPC.Deblock.Row, 0x2);       // Row
   _vr02 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y + 8 + (4*PCB_RECONSTRUCTED_STRIDE)), 0x40);   // SDM start address for Y
   _vr02 = _vmovw(0, 0x80);   //  Need to support 32  bit intrinsics SDM start address
   
   //  SDM start address for U
   _vr03 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_U + 4 + (4*PCB_RECONSTRUCTED_STRIDE)), 0x40);   // SDM start address for U
   _vr03 = _vmovw(0, 0x80);   //  Need to support 32  bit intrinsics SDM start address
   
// SDM start address for U
   _vr04 = _vmovw((unsigned int)(dbcb.PixelCoeffBuffer + PCB_RECONSTRUCTED_V + 4 + (4*PCB_RECONSTRUCTED_STRIDE)), 0x40);   // SDM start address for U
   _vr04 = _vmovw(0, 0x80);   //  Need to support 32  bit intrinsics SDM start address

   if (!gArcFrameBuffers.YUVPos)
   {
       _vr05 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
       _vr06 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
       _vr07 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x80);
   }
   else
   {
       _vr05 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
       _vr06 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
       _vr07 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x80);
   }

   _vrun(MACRO_WriteOutputBlock);
	SetSCQ(0);
}




// H264 deblock ------------------------------------------------------------------
// Does deblock on reconstructed MPC for 1 macroblock & outputs resulting pixels to frame buffer
// Must be delayed by 2 or more macroblock or will corupt data being used by current
// Free MPC circular buffer entry
void DoH264DeblockFilter(CBTPicture * picture, int dbIdx)
{
    if (1 != MgrMPC.Deblock.Mode)
    {
        DeblockAndOutput(picture, dbIdx);
    }
    else
    {
        OutputOnly(picture, dbIdx);
    }

    WorkAreaSDM->CopyToFixedAfterDeblockH264(dbIdx);		// Copy circular buffer MPC to fixed buffer

    // Move on to next column
    MgrMPC.Deblock.Col++;
    MgrMPC.Deblock.LumaOffset += 16;
    MgrMPC.Deblock.ChromaOffset += 8;

    // Move to next row ?
    if(MgrMPC.Deblock.Col == MgrMPC.Picture.Width)
    {
        MgrMPC.Deblock.Col = 0;
        MgrMPC.Deblock.Row++;
        MgrMPC.Deblock.LumaOffset += MgrMPC.Picture.Width*16*15;
        MgrMPC.Deblock.ChromaOffset += MgrMPC.Picture.Width*8*7;
    }

	SetSCQ(SCQ1);
	SET_CallBackReleaseBuffer_BufIdx(dbIdx);
	_vrun(MACRO_CallBackReleaseBuffer);
	SetSCQ(0);
}



CHANNEL_SERVICE Service_StartDeblock(int pIdx)
{
	// Deblock queue
	static short dbq0 = -1;
	static short dbq1 = -1;
	if(dbq1 >= 0)
	{
        DoH264DeblockFilter(gCH264Encoder.m_EncodedPicture, dbq1);
        dbq1 = -1;
	}
	
	dbq1 = dbq0;
	dbq0 = pIdx;

}


CHANNEL_SERVICE Service_ReleaseBuffer(int sIdx)
{
    // Free MPC circular buffer entry
    MgrMPC.ActMPC[sIdx].DeblockOk = true;
    MgrMPC.FreeMPC(sIdx);
    MgrMPC.MacroBlocksCompleteCount++;
	if( MgrMPC.MacroBlockStartedCount == gCH264Encoder.m_NumberOfMacroblocksInPicture && MgrMPC.MacroBlocksCompleteCount >= gCH264Encoder.m_NumberOfMacroblocksInPicture-2)
	{
		SET_CallBackStartDeblock_BufIdx(-1);
		_vrun(MACRO_CallBackStartDeblock);
	}
    if(MgrMPC.RestartCount && (MgrMPC.MacroBlocksCompleteCount -  MgrMPC.MacroBlockStartedCount < 3))
    {
    	MgrMPC.RestartCount--;
    	Service_StartME(0);
    }
}


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
