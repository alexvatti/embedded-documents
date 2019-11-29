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
/* FILE:			AuroraH264ReconLuma.cpp 								*/
/* DATE:															        */
/* DESCRIPTION:		Interface for the AuroraH264Dec class.					*/
/* ------------------------------------------------------------------------ */

#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#include "H264Tables.h"
_Asm int Macro_ReconLuma_HBlkSz_4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

	// load the inv-xform coefs and filtered data
	vld64	vr01, [i1, 0]	; // ROW1:  inp1- coef vals
	vld64w  vr05, [i2, 0]	; // ROW1:  inp2- filtered data

	vld64	vr02, [i1, 32]	; // ROW2:  inp1- coef vals
	vld64w  vr06, [i2, 24]	; // ROW2:  inp2- filtered data

	vld64	vr03, [i1, 64]	; // ROW3:  inp1- coef vals
	vld64w  vr07, [i2, 48]	; // ROW3:  inp2- filtered data

	vld64	vr04, [i1, 96]	; // ROW4:  inp1- coef vals
	vld64w  vr08, [i2, 72]	; // ROW4:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;
	
	vmovw	vr23, 128, 2		// Incs for ptrs
	vmovw	vr23, 96, 4
	vmovw	vr23, 64, 8

	//store the result
	vst32	vr01, [i3,  0]	; // out- row1
	vst32	vr02, [i3, 16]	; // out- row1
	vst32	vr03, [i3, 32]	; // out- row1
	vst32	vr04, [i3, 48]	; // out- row1
	vaddw	vr00, vr00, vr23		// Inc ptrs

//-------------------------------------
    vendrec r0              ;
.on	percent_reg

}




_Asm int Macro_ReconLuma_HBlkSz_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

	// load the inv-xform coefs and filtered data
	vld128	vr01, [i1, 0]	; // ROW1:  inp1- coef vals
	vld64w  vr05, [i2, 0]	; // ROW1:  inp2- filtered data

	vld128	vr02, [i1, 32]	; // ROW2:  inp1- coef vals
	vld64w  vr06, [i2, 24]	; // ROW2:  inp2- filtered data

	vld128	vr03, [i1, 64]	; // ROW3:  inp1- coef vals
	vld64w  vr07, [i2, 48]	; // ROW3:  inp2- filtered data

	vld128	vr04, [i1, 96]	; // ROW4:  inp1- coef vals
	vld64w  vr08, [i2, 72]	; // ROW4:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vmovw	vr23, 128, 2		// Incs for ptrs
	vmovw	vr23, 96, 4
	vmovw	vr23, 64, 8

	//store the result
	vst64	vr01, [i3, 0]	; // out- row1
	vst64	vr02, [i3, 16]	; // out- row1
	vst64	vr03, [i3, 32]	; // out- row1
	vst64	vr04, [i3, 48]	; // out- row1
	vaddw	vr00, vr00, vr23		// Inc ptrs

//-------------------------------------
    vendrec r0              ;
.on	percent_reg

}




_Asm int Macro_ReconLuma_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

    // process ROW 1 & 2
	// load the inv-xform coefs and filtered data
	vld128	vr01, [i1, 0]	; // ROW1a:  inp1- coef vals
	vld64w  vr05, [i2, 0]	; // ROW1a:  inp2- filtered data

	vld128	vr02, [i1, 16]	; // ROW1b:  inp1- coef vals
	vld64w  vr06, [i2,  8]	; // ROW1b:  inp2- filtered data

	vld128	vr03, [i1, 32]	; // ROW2a:  inp1- coef vals
	vld64w  vr07, [i2, 24]	; // ROW2a:  inp2- filtered data

	vld128	vr04, [i1, 48]	; // ROW2b:  inp1- coef vals
	vld64w  vr08, [i2, 32]	; // ROW2b:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	//store the result
	vst64	vr01, [i3,  0]	; // out- row1
	vst64	vr02, [i3,  8]	; // out- row1
	vst64	vr03, [i3, 16]	; // out- row1
	vst64	vr04, [i3, 24]	; // out- row1

    //-----------------------------------------------------

    
    // process ROW 3 & 4
	vld128	vr01, [i1, 64]	; // ROW3a:  inp1- coef vals
	vld64w  vr05, [i2, 48]	; // ROW3a:  inp2- filtered data

	vld128	vr02, [i1, 80]	; // ROW3b:  inp1- coef vals
	vld64w  vr06, [i2, 56]	; // ROW3b:  inp2- filtered data

	vld128	vr03, [i1, 96]	; // ROW4a:  inp1- coef vals
	vld64w  vr07, [i2, 72]	; // ROW4a:  inp2- filtered data

	vld128	vr04, [i1, 112]	; // ROW4b:  inp1- coef vals
	vld64w  vr08, [i2,  80]	; // ROW4b:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	//store the result
	vst64	vr01, [i3, 32]	; // out- row1
	vst64	vr02, [i3, 40]	; // out- row1
	vst64	vr03, [i3, 48]	; // out- row1
	vst64	vr04, [i3, 56]	; // out- row1
    
    //-----------------------------------------------------



    // process ROW 5 & 6
	// load the inv-xform coefs and filtered data
	vld128	vr01, [i1, 128]	; // ROW1a:  inp1- coef vals
	vld64w  vr05, [i2,  96]	; // ROW1a:  inp2- filtered data

	vld128	vr02, [i1, 144]	; // ROW1b:  inp1- coef vals
	vld64w  vr06, [i2, 104]	; // ROW1b:  inp2- filtered data

	vld128	vr03, [i1, 160]	; // ROW2a:  inp1- coef vals
	vld64w  vr07, [i2, 120]	; // ROW2a:  inp2- filtered data

	vld128	vr04, [i1, 176]	; // ROW2b:  inp1- coef vals
	vld64w  vr08, [i2, 128]	; // ROW2b:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	//store the result
	vst64	vr01, [i3, 64]	; // out- row1
	vst64	vr02, [i3, 72]	; // out- row1
	vst64	vr03, [i3, 80]	; // out- row1
	vst64	vr04, [i3, 88]	; // out- row1

    //-----------------------------------------------------

    
    // process ROW 7 & 8
	vld128	vr01, [i1, 192]	; // ROW3a:  inp1- coef vals
	vld64w  vr05, [i2, 144]	; // ROW3a:  inp2- filtered data

	vld128	vr02, [i1, 208]	; // ROW3b:  inp1- coef vals
	vld64w  vr06, [i2, 152]	; // ROW3b:  inp2- filtered data

	vld128	vr03, [i1, 224]	; // ROW4a:  inp1- coef vals
	vld64w  vr07, [i2, 168]	; // ROW4a:  inp2- filtered data

	vld128	vr04, [i1, 240]	; // ROW4b:  inp1- coef vals
	vld64w  vr08, [i2, 176]	; // ROW4b:  inp2- filtered data

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;
	vmovw	vr23, 256, 2		// Incs for ptrs
	vmovw	vr23, 192, 4
	vmovw	vr23, 128, 8

	//store the result
	vst64	vr01, [i3,  96]	; // out- row1
	vst64	vr02, [i3, 104]	; // out- row1
	vst64	vr03, [i3, 112]	; // out- row1
	vst64	vr04, [i3, 120]	; // out- row1
    
    //-----------------------------------------------------
	vaddw	vr00, vr00, vr23		// Inc ptrs


    vendrec r0              ;
	
.on	percent_reg

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordLumaInterReconMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the luma reconstruction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLumaInterReconMacros()
{
    Record(AM_Inter_ReconLuma_HBlkSz_4, Macro_ReconLuma_HBlkSz_4(NextAddr));
    Record(AM_Inter_ReconLuma_HBlkSz_8, Macro_ReconLuma_HBlkSz_8(NextAddr));
    Record(AM_Inter_ReconLuma_HBlkSz_16, Macro_ReconLuma_HBlkSz_16(NextAddr));
}

