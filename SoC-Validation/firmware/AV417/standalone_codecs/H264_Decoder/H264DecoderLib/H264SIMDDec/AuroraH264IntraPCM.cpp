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

/* ------------------------------------------------------------------------ */
/* FILE:			AuroraH264Dec.h											*/
/* DATE:															        */
/* DESCRIPTION:		Interface for the AuroraH264Dec class.					*/
/* ------------------------------------------------------------------------ */
#ifdef _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec2")
#endif
_Asm void IntraReconPCMLumaAsm16x8( unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg coef_buf_add, out_mcblk_add

.off	percent_reg
    // load inp and output pointers
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add
.on	percent_reg
}

_Asm int IntraReconPCMLuma16x8_MACRO(int addr)
{
	% reg addr
.off	percent_reg

	vrec	addr
	// load coefs -- sdm stride 32 - first 4 rows
	vld128	vr11, [i2, 0]		;	// row1a
	vld128	vr12, [i2, 16]	;	// row1b

	vld128	vr13, [i2, 32]	;	// row2a
	vld128	vr14, [i2, 48]	;	// row2b

	vld128	vr15, [i2, 64]	;	// row3a
	vld128	vr16, [i2, 80]	;	// row3b

	vld128	vr17, [i2, 96]	;	// row4a
	vld128	vr18, [i2, 112]	;	// row4b


	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;

	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	//store the result
	vst64	vr11, [i3, 0]	; // out- row1a
	vst64	vr12, [i3, 8]	; // out- row1b

	vst64	vr13, [i3, 16]	; // out- row2a
	vst64	vr14, [i3, 24]	; // out- row2b
	
	vst64	vr15, [i3, 32]	; // out- row3a
	vst64	vr16, [i3, 40]	; // out- row3b

	vst64	vr17, [i3, 48]	; // out- row4a
	vst64	vr18, [i3, 56]	; // out- row4b

	// load coefs -- sdm stride 32 - next 4 rows
	vld128	vr11, [i2, 128]	;	// row1a
	vld128	vr12, [i2, 144]	;	// row1b

	vld128	vr13, [i2, 160]	;	// row2a
	vld128	vr14, [i2, 176]	;	// row2b

	vld128	vr15, [i2, 192]	;	// row3a
	vld128	vr16, [i2, 208]	;	// row3b

	vld128	vr17, [i2, 224]	;	// row4a
	vld128	vr18, [i2, 240]	;	// row4b

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;

	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	//store the result
	vst64	vr11, [i3, 64]	; // out- row1a
	vst64	vr12, [i3, 72]	; // out- row1b

	vst64	vr13, [i3, 80]	; // out- row2a
	vst64	vr14, [i3, 88]	; // out- row2b
	
	vst64	vr15, [i3, 96]	; // out- row3a
	vst64	vr16, [i3, 104]	; // out- row3b

	vst64	vr17, [i3, 112]	; // out- row4a
	vst64	vr18, [i3, 120]	; // out- row4b
	vendrec	r0
.on	percent_reg
}

_Asm void IntraReconPCMChromaAsm( unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg coef_buf_add, out_mcblk_add

.off	percent_reg
    // load inp and output pointers
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add
.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm int IntraReconPCMChroma_MACRO(int addr)
{
	% reg addr
.off	percent_reg
	vrec	addr
	// load coefs -- sdm stride 16 - row 1 to 4
	vld128	vr11, [i2, 0]	;	// row1
	vld128	vr12, [i2, 16]	;	// row2
	vld128	vr13, [i2, 32]	;	// row3
	vld128	vr14, [i2, 48]	;	// row4

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	// store the result
	vst64	vr11, [i3, 0]	; // out- row1a
	vst64	vr12, [i3, 8]	; // out- row1b
	vst64	vr13, [i3, 16]	; // out- row2a
	vst64	vr14, [i3, 24]	; // out- row2b

	// load coefs -- sdm stride 16 - row 5 to 8
	vld128	vr15, [i2, 64]	;	// row5
	vld128	vr16, [i2, 80]	;	// row6
	vld128	vr17, [i2, 96]	;	// row7
	vld128	vr18, [i2, 112]	;	// row8

	// clip & pack the output
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	// store the result
	vst64	vr15, [i3, 32]	; // out- row5
	vst64	vr16, [i3, 40]	; // out- row6
	vst64	vr17, [i3, 48]	; // out- row7
	vst64	vr18, [i3, 56]	; // out- row8
	vendrec r0
.on	percent_reg
}


void AuroraH264Dec::IntraReconPCMLuma( SDM_Offset coef_buf_add,
							SDM_Offset out_mcblk_add )
{

	// clip and pack the coeffs to the output buffer
	// process 8 rows at a time
	IntraReconPCMLumaAsm16x8( coef_buf_add, out_mcblk_add );
	RecMgr.Run(AM_PCM_IntraReconPCMLuma16x8);

	IntraReconPCMLumaAsm16x8( ( coef_buf_add + (8*16*2) ), 
						( out_mcblk_add + (8*16)) );
	RecMgr.Run(AM_PCM_IntraReconPCMLuma16x8);
}

void AuroraH264Dec::IntraReconPCMChroma( SDM_Offset coef_buf_add,
							SDM_Offset out_mcblk_add )
{

	// clip and pack the coeffs to the output buffer
	IntraReconPCMChromaAsm(	coef_buf_add, out_mcblk_add );
	RecMgr.Run(AM_PCM_IntraReconPCMChroma);

}

void AuroraRecMgr::RecordIntraPCM_Macros()
{
    Record(AM_PCM_IntraReconPCMLuma16x8, IntraReconPCMLuma16x8_MACRO(NextAddr));
    Record(AM_PCM_IntraReconPCMChroma, IntraReconPCMChroma_MACRO(NextAddr));
}

