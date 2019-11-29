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
/* FILE NAME:       AuroraH264Xforms.cpp                                    */
/* DATE:			                										*/
/* DESCRIPTION:		SIMD code for inverse transforms.                       */
/*                    Contains the Macro as well as non-macro version       */
/*																			*/
/* ------------------------------------------------------------------------ */

#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif
#include <memory.h>

#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#include "H264EnumTypes.h"
#include "H264OffsetTables.h"
#include "H264Tables.h"
/* ------------------------------------------------------------------------ */
/* FUNCTION:	Macro_InlineLumaTransform              	                    */
/* INP-PARAMS:  scm_addr : Address of the next free location in SCM         */
/* RETURNS:		Address of the last instruction which was recorded          */
/* DESCRIPTION:	Macro For inverse Luma transform                            */
/* ASSUMPTIONS: i1=start address of Input buffer                            */
/* ------------------------------------------------------------------------ */
_Asm int Macro_InlineLumaTransform(int scm_addr)
{
    %reg        scm_addr
    vrec        scm_addr
//-------------------------------------
	// load input data
	vld128	%vr04, [%i1, 0]

	vld128	%vr05, [%i1, 32]
	vld128	%vr06, [%i1, 64]
	vld128	%vr07, [%i1, 96]

	// apply horizontal transform
	// stage 0
	vaddsuw	%vr04, %vr06
	vasrw	%vr08, %vr05, 1
	vasrw	%vr09, %vr07, 1
	vsubw	%vr10, %vr08, %vr07
 	vaddw	%vr11, %vr09, %vr05

	//stage 1
	vaddsuw	%vr04, %vr11
	vaddsuw	%vr06, %vr10
	vmvw %vr05, %vr06, 0xff
	vmvw %vr06, %vr10, 0xff
	vmvw %vr07, %vr11, 0xff
	// take transpose
	vexch2	%vr04, %vr06
	vexch2	%vr05, %vr07
	vexch1	%vr04, %vr05
 	vexch1  %vr06, %vr07
	// apply vertical transform
	// stage 0
	vaddsuw	%vr04, %vr06
	vasrw	%vr08, %vr05, 1
	vasrw	%vr09, %vr07, 1
	vsubw	%vr10, %vr08, %vr07
	vaddw	%vr11, %vr09, %vr05

	//stage 1
	vaddsuw	%vr04, %vr11
	vaddsuw	%vr06, %vr10
	vmvw %vr05, %vr06, 0xff
	vmvw %vr06, %vr10, 0xff
	vmvw %vr07, %vr11, 0xff	
	// right shift and round
	vasrrw %vr04, %vr04, 6
	vasrrw %vr05, %vr05, 6
	vasrrw %vr06, %vr06, 6
	vasrrw %vr07, %vr07, 6
        
	// store the result back in SDM memory
	vst128	%vr04, [%i1, 0]
	vst128	%vr05, [%i1, 32]
	vst128	%vr06, [%i1, 64]
    vst128	%vr07, [%i1, 96]
//-------------------------------------
    vbaddw	%vr00,%vr00,16
    vendrec     %r0
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Macro_InlineChromaTransform           	                    */
/* RETURNS:		Address of the last instruction which was recorded          */
/* INP-PARAMS:  scm_addr : Address of the next free location in SCM         */
/* DESCRIPTION:	Macro For inverse Chroma transform                          */
/* ASSUMPTIONS: i1=start address of Input buffer                            */
/* ------------------------------------------------------------------------ */
_Asm int Macro_InlineChromaTransform(int scm_addr)
{
    %reg        scm_addr
    vrec        scm_addr
//-------------------------------------
	// load input data
	vld128	%vr04, [%i1, 0]			;
	vld128	%vr05, [%i1, 16]		;
	vld128	%vr06, [%i1, 32]		;
	vld128	%vr07, [%i1, 48]		;
	// apply horizontal transform
	// stage 0
	vaddsuw	%vr04, %vr06			;
	vasrw	%vr08, %vr05, 1		;
	vasrw	%vr09, %vr07, 1		;
	vsubw	%vr10, %vr08, %vr07	;
	vaddw	%vr11, %vr09, %vr05	;
	//stage 1
	vaddsuw	%vr04, %vr11			;
	vaddsuw	%vr06, %vr10			;
	vmvw %vr05, %vr06, 0xff		;
	vmvw %vr06, %vr10, 0xff		;
	vmvw %vr07, %vr11, 0xff		;
	// take transpose
	vexch2	%vr04, %vr06			;
	vexch2	%vr05, %vr07			;
	vexch1	%vr04, %vr05			;
	vexch1  %vr06, %vr07			;

	// apply vertical transform
	// stage 0
	vaddsuw	%vr04, %vr06			;
	vasrw	%vr08, %vr05, 1		;
	vasrw	%vr09, %vr07, 1		;
	vsubw	%vr10, %vr08, %vr07	;
	vaddw	%vr11, %vr09, %vr05	;
	//stage 1
	vaddsuw	%vr04, %vr11			;
	vaddsuw	%vr06, %vr10			;
	vmvw %vr05, %vr06, 0xff		;
	vmvw %vr06, %vr10, 0xff		;
	vmvw %vr07, %vr11, 0xff		;
	// right shift and round
	vasrrw %vr04, %vr04, 6		;
	vasrrw %vr05, %vr05, 6		;
	vasrrw %vr06, %vr06, 6		;
	vasrrw %vr07, %vr07, 6		;

	// store the result back in SDM memory
	vst128	%vr04, [%i1, 0]			;
	vst128	%vr05, [%i1, 16]		;
	vst128	%vr06, [%i1, 32]		;
	vst128	%vr07, [%i1, 48]		;
	vbaddw	%vr00, %vr00,64
//-------------------------------------
    vendrec     %r0
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordXformMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the transform macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordXformMacros()
{
    Record(AM_Xform_Luma, Macro_InlineLumaTransform(NextAddr));
    Record(AM_Xform_Chroma, Macro_InlineChromaTransform(NextAddr));
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	InlineLumaTransform                 	                    */
/* INP-PARAMS:  block_start_add_sdm : SDM address of inp/out buffer         */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Loads the index registers with the following values         */
/*                  i1=start address of Input buffer                        */
/* ------------------------------------------------------------------------ */
_Asm void InlineLumaTransform(unsigned int block_start_add_sdm)
{
    % reg block_start_add_sdm


	; //load vector registers with data
	vmovw	%vr00, block_start_add_sdm, 2  ;// load i1 with r1
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	InlineChromaTransform                 	                    */
/* INP-PARAMS:  block_start_add_sdm : SDM address of inp/out buffer         */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Loads the index registers with the following values         */
/*                  i1=start address of Input buffer                        */
/* ------------------------------------------------------------------------ */
_Asm void InlineChromaTransform(unsigned int block_start_add_sdm)
{
    //update the right shift value register
	% reg block_start_add_sdm

    // load vector registers with data
    vmovw   %vr00, block_start_add_sdm, 2          ; // load i1 with r1
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse2DLumaTransform                 	                    */
/* INP-PARAMS:                                                              */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	                                                            */
/* ------------------------------------------------------------------------ */
void AuroraH264Dec::Inverse2DLumaTransform( SDM_Offset buf_add_sdm)
{
	// calculate buffer address in SDM w.r.t SDM base address

	// apply transform on each 4x4 block
	// perform inverse transform on each 4x8 block of data
	for(unsigned int block_offset_sdm = 0; block_offset_sdm<512; block_offset_sdm += 128)
	{
		// find the start address in SDM

	    InlineLumaTransform(buf_add_sdm + block_offset_sdm);

        RecMgr.Run(AM_Xform_Luma);


//	    InlineLumaTransform(buf_add_sdm + block_offset_sdm+16);

        RecMgr.Run(AM_Xform_Luma);

	}

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse2DChromaTransform               	                    */
/* INP-PARAMS:                                                              */
/* RETURNS:		void                                                        */
/* DESCRIPTION:                                                             */
/* ------------------------------------------------------------------------ */
void AuroraH264Dec::Inverse2DChromaTransform( SDM_Offset buf_add_sdm)
{
	InlineChromaTransform(buf_add_sdm);
    RecMgr.Run(AM_Xform_Chroma);
//	InlineChromaTransform(buf_add_sdm+64);
    RecMgr.Run(AM_Xform_Chroma);
//	InlineChromaTransform(buf_add_sdm+128);
    RecMgr.Run(AM_Xform_Chroma);
//	InlineChromaTransform(buf_add_sdm+64+128);
    RecMgr.Run(AM_Xform_Chroma);
}
