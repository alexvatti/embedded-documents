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
/* FILE:			AuroraH264ReconChroma.cpp								*/
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

#define USE_DMA_OUT

_Asm int Macro_ChromaReconBlkSz_2x2(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld32	vr01, [i1, 0]   ;	//row-1 xform coeffs
	vld32	vr02, [i1, 16]  ;	//row-2 xform coeffs

	vld64w	vr03, [i2, 0]   ;	//row-1 inter-pred output
	vld64w	vr04, [i2, 16]  ;	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr03	;
	vaddw	vr02, vr02, vr04	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	
	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3
	
	//store the result
	vst16	vr01, [i3,  0]	; // out- row1
	vst16	vr02, [i3,  8]	; // out- row2
	
	vaddw	vr00, vr00, vr23	// inc ptrs

//-------------------------------------

    vendrec r0

.on	percent_reg

}



_Asm int Macro_ChromaReconBlkSz_2x4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	vld32	vr01, [i1, 0] ;	//row-1 xform coeffs
	vld32	vr02, [i1, 16];	//row-2 xform coeffs
	vld32	vr03, [i1, 32];	//row-4 xform coeffs
	vld32	vr04, [i1, 48];	//row-5 xform coeffs

	vld64w	vr05, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr06, [i2, 16];	//row-1 inter-pred output
	vld64w	vr07, [i2, 32];	//row-1 inter-pred output
	vld64w	vr08, [i2, 48];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst16	vr01, [i3,  0]	; // out- row1
	vst16	vr02, [i3,  8]	; // out- row2
	vst16	vr03, [i3,  16]	; // out- row3
	vst16	vr04, [i3,  24]	; // out- row4
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------
    vendrec r0

.on	percent_reg

}


_Asm int Macro_ChromaReconBlkSz_4x2(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	vld64	vr01, [i1, 0]	;	//row-1 xform coeffs
	vld64	vr02, [i1, 16];	//row-2 xform coeffs

	vld64w	vr03, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr04, [i2, 16];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr03	;
	vaddw	vr02, vr02, vr04	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst32	vr01, [i3,  0]	; // out- row1
	vst32	vr02, [i3,  8]	; // out- row2
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------

    vendrec r0


.on	percent_reg

}


_Asm int Macro_ChromaReconBlkSz_4x4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld64	vr01, [i1, 0]	;	//row-1 xform coeffs
	vld64	vr02, [i1, 16];	//row-2 xform coeffs
	vld64	vr03, [i1, 32];	//row-1 xform coeffs
	vld64	vr04, [i1, 48];	//row-2 xform coeffs

	vld64w	vr05, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr06, [i2, 16];	//row-1 inter-pred output
	vld64w	vr07, [i2, 32];	//row-1 inter-pred output
	vld64w	vr08, [i2, 48];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst32	vr01, [i3,  0]	; // out- row1
	vst32	vr02, [i3,  8]	; // out- row2
	vst32	vr03, [i3,  16]	; // out- row3
	vst32	vr04, [i3,  24]	; // out- row4
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------

    vendrec r0


.on	percent_reg

}


_Asm int Macro_ChromaReconBlkSz_4x8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld64	vr01, [i1, 0] ;	//row-1 xform coeffs
	vld64	vr02, [i1, 16];	//row-2 xform coeffs
	vld64	vr03, [i1, 32];	//row-3 xform coeffs
	vld64	vr04, [i1, 48];	//row-4 xform coeffs

	vld64	vr05, [i1, 64];	//row-5 xform coeffs
	vld64	vr06, [i1, 80];	//row-6 xform coeffs
	vld64	vr07, [i1, 96];	//row-7 xform coeffs
	vld64	vr08, [i1, 112];//row-8 xform coeffs


	vld64w	vr09, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr10, [i2, 16];	//row-1 inter-pred output
	vld64w	vr11, [i2, 32];	//row-1 inter-pred output
	vld64w	vr12, [i2, 48];	//row-1 inter-pred output

	vld64w	vr13, [i2, 64];	//row-1 inter-pred output
	vld64w	vr14, [i2, 80];	//row-1 inter-pred output
	vld64w	vr15, [i2, 96];	//row-1 inter-pred output
	vld64w	vr16, [i2, 112];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr09	;
	vaddw	vr02, vr02, vr10	;
	vaddw	vr03, vr03, vr11	;
	vaddw	vr04, vr04, vr12	;

	vaddw	vr05, vr05, vr13	;
	vaddw	vr06, vr06, vr14	;
	vaddw	vr07, vr07, vr15	;
	vaddw	vr08, vr08, vr16	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vasrpwb vr05, vr05, 0	;
	vasrpwb vr06, vr06, 0	;
	vasrpwb vr07, vr07, 0	;
	vasrpwb vr08, vr08, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst32	vr01, [i3,  0]	; // out- row1
	vst32	vr02, [i3,  8]	; // out- row2
	vst32	vr03, [i3,  16]	; // out- row3
	vst32	vr04, [i3,  24]	; // out- row4

	vst32	vr05, [i3,  32]	; // out- row5
	vst32	vr06, [i3,  40] ; // out- row6
	vst32	vr07, [i3,  48]	; // out- row7
	vst32	vr08, [i3,  56]	; // out- row8
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------

    vendrec r0

.on	percent_reg

}


_Asm int Macro_ChromaReconBlkSz_8x4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld128	vr01, [i1, 0]	;	//row-1 xform coeffs
	vld128	vr02, [i1, 16];	//row-2 xform coeffs
	vld128	vr03, [i1, 32];	//row-1 xform coeffs
	vld128	vr04, [i1, 48];	//row-2 xform coeffs

	vld64w	vr05, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr06, [i2, 16];	//row-1 inter-pred output
	vld64w	vr07, [i2, 32];	//row-1 inter-pred output
	vld64w	vr08, [i2, 48];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr05	;
	vaddw	vr02, vr02, vr06	;
	vaddw	vr03, vr03, vr07	;
	vaddw	vr04, vr04, vr08	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst64	vr01, [i3,  0]	; // out- row1
	vst64	vr02, [i3,  8]	; // out- row2
	vst64	vr03, [i3,  16]	; // out- row3
	vst64	vr04, [i3,  24]	; // out- row4
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------
 
    vendrec r0


.on	percent_reg

}


_Asm int Macro_ChromaReconBlkSz_8x8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld128	vr01, [i1, 0]	;	//row-1 xform coeffs
	vld128	vr02, [i1, 16];	//row-2 xform coeffs
	vld128	vr03, [i1, 32];	//row-1 xform coeffs
	vld128	vr04, [i1, 48];	//row-2 xform coeffs

	vld128	vr05, [i1, 64];	//row-1 xform coeffs
	vld128	vr06, [i1, 80];	//row-2 xform coeffs
	vld128	vr07, [i1, 96];	//row-1 xform coeffs
	vld128	vr08, [i1, 112];	//row-2 xform coeffs


	vld64w	vr09, [i2, 0]	;	//row-1 inter-pred output
	vld64w	vr10, [i2, 16];	//row-1 inter-pred output
	vld64w	vr11, [i2, 32];	//row-1 inter-pred output
	vld64w	vr12, [i2, 48];	//row-1 inter-pred output

	vld64w	vr13, [i2, 64];	//row-1 inter-pred output
	vld64w	vr14, [i2, 80];	//row-1 inter-pred output
	vld64w	vr15, [i2, 96];	//row-1 inter-pred output
	vld64w	vr16, [i2, 112];	//row-1 inter-pred output

	// calculate the output -- sum and clip
	vaddw	vr01, vr01, vr09	;
	vaddw	vr02, vr02, vr10	;
	vaddw	vr03, vr03, vr11	;
	vaddw	vr04, vr04, vr12	;

	vaddw	vr05, vr05, vr13	;
	vaddw	vr06, vr06, vr14	;
	vaddw	vr07, vr07, vr15	;
	vaddw	vr08, vr08, vr16	;

	vasrpwb vr01, vr01, 0	;
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;

	vasrpwb vr05, vr05, 0	;
	vasrpwb vr06, vr06, 0	;
	vasrpwb vr07, vr07, 0	;
	vasrpwb vr08, vr08, 0	;

	vmovw	vr23,128,2			// Increment for i1
	vmovw	vr23,160,4			// Increment for i2
	vmovw	vr23,64,8			// Increment for i3

	//store the result
	vst64	vr01, [i3,  0]	; // out- row1
	vst64	vr02, [i3,  8]	; // out- row2
	vst64	vr03, [i3,  16]	; // out- row3
	vst64	vr04, [i3,  24]	; // out- row4

	vst64	vr05, [i3,  32]	; // out- row5
	vst64	vr06, [i3,  40]	; // out- row6
	vst64	vr07, [i3,  48]	; // out- row7
	vst64	vr08, [i3,  56]	; // out- row8
	
	vaddw	vr00, vr00, vr23	// inc ptrs


//-------------------------------------

    vendrec r0


.on	percent_reg

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordChromaInterReconMacros       	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the chroma reconstruction macros                    */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordChromaInterReconMacros()
{
    Record(AM_Inter_ChromaReconBlkSz_2x2, Macro_ChromaReconBlkSz_2x2(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_2x4, Macro_ChromaReconBlkSz_2x4(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_4x2, Macro_ChromaReconBlkSz_4x2(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_4x4, Macro_ChromaReconBlkSz_4x4(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_4x8, Macro_ChromaReconBlkSz_4x8(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_8x4, Macro_ChromaReconBlkSz_8x4(NextAddr));
    Record(AM_Inter_ChromaReconBlkSz_8x8, Macro_ChromaReconBlkSz_8x8(NextAddr));
        
    ChromaBlockMacros[CHROMA_BLK_IDX_2x2] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_2x2);
    ChromaBlockMacros[CHROMA_BLK_IDX_2x4] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_2x4);
    ChromaBlockMacros[CHROMA_BLK_IDX_4x2] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_4x2);
    ChromaBlockMacros[CHROMA_BLK_IDX_4x4] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_4x4);
    ChromaBlockMacros[CHROMA_BLK_IDX_4x8] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_4x8);
    ChromaBlockMacros[CHROMA_BLK_IDX_8x4] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_8x4);
    ChromaBlockMacros[CHROMA_BLK_IDX_8x8] = GetMacroAddr(AM_Inter_ChromaReconBlkSz_8x8);
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:	AuroraH264Dec::SIMDReconChroma								*/
/* DESCRIPTION:							                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */
