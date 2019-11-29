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
_Asm int Macro_LumaIntra16x8VerPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld64w	vr01, [i1, 0]	;	// vr01: p07 p06 p05 p04 p03 p02 p01 p00
	vld64w	vr02, [i1, 8]	;	// vr02: p15 p14 p13 p12 p11 p10 p09 p08

	// load coefs -- sdm stride 32 - first 4 rows
	vld128	vr11, [i2, 0]		;	// row1a
	vld128	vr12, [i2, 16]	;	// row1b

	vld128	vr13, [i2, 32]	;	// row2a
	vld128	vr14, [i2, 48]	;	// row2b

	vld128	vr15, [i2, 64]	;	// row3a
	vld128	vr16, [i2, 80]	;	// row3b

	vld128	vr17, [i2, 96]	;	// row4a
	vld128	vr18, [i2, 112]	;	// row4b
	
	// add coef values to pred-values
	vaddw	vr11, vr11, vr01	; 
	vaddw	vr12, vr12, vr02	;

	vaddw	vr13, vr13, vr01	; 
	vaddw	vr14, vr14, vr02	;

	vaddw	vr15, vr15, vr01	; 
	vaddw	vr16, vr16, vr02	;

	vaddw	vr17, vr17, vr01	; 
	vaddw	vr18, vr18, vr02	;

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


	//process next 4 rows
	// load coefs -- sdm stride 32 - rows 4 to 8
	vld128	vr11, [i2, 128]	;	// row4a
	vld128	vr12, [i2, 144]	;	// row4b

	vld128	vr13, [i2, 160]	;	// row5a
	vld128	vr14, [i2, 176]	;	// row5b

	vld128	vr15, [i2, 192]	;	// row6a
	vld128	vr16, [i2, 208]	;	// row6b

	vld128	vr17, [i2, 224]	;	// row7a
	vld128	vr18, [i2, 240]	;	// row8b

	// add coef values to pred-values
	vaddw	vr11, vr11, vr01	; 
	vaddw	vr12, vr12, vr02	;

	vaddw	vr13, vr13, vr01	; 
	vaddw	vr14, vr14, vr02	;

	vaddw	vr15, vr15, vr01	; 
	vaddw	vr16, vr16, vr02	;

	vaddw	vr17, vr17, vr01	; 
	vaddw	vr18, vr18, vr02	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;

	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	vmovzw	vr23, 8*16*2, 4	// Ptr incs
	vmovw	vr23, 8*16, 8
	//store the result
	vst64	vr11, [i3, 64]	; // out- row4a
	vst64	vr12, [i3, 72]	; // out- row4b

	vst64	vr13, [i3, 80]	; // out- row5a
	vst64	vr14, [i3, 88]	; // out- row5b
	
	vst64	vr15, [i3, 96]	; // out- row6a
	vst64	vr16, [i3, 104]	; // out- row6b

	vst64	vr17, [i3, 112]	; // out- row7a
	vst64	vr18, [i3, 120]	; // out- row7b
	
	vaddw	vr00, vr00,vr23		// Inc ptrs
//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_LumaIntra16x4HorPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// clear a DUMMY register
	vsubw	vr10, vr10, vr10		;	//vr10 = 00....00

	// get the left line
	vld32wl	vr01, [i1, 0]	;
	vld32wl	vr02, [i1, 16]	;
	vld32wl	vr03, [i1, 32]	;
	vld32wl	vr04, [i1, 48]	;

	// shift right by 3 words
	vmvzw	vr05,	vr01,	8		;	//vr01: 000 000 000 000 000 000 000 p0
	vsr8aw	vr01,	vr05,	2		;
	vsr8aw	vr01,	vr05,	4		;
	vsr8aw	vr01,	vr05,	6		;

	vmvzw	vr06,	vr02,	8		;	//vr01: 000 000 000 000 000 000 000 p0
	vsr8aw	vr02,	vr06,	2		;
	vsr8aw	vr02,	vr06,	4		;
	vsr8aw	vr02,	vr06,	6		;

	vmvzw	vr07,	vr03,	8		;	//vr01: 000 000 000 000 000 000 000 p0
	vsr8aw	vr03,	vr07,	2		;
	vsr8aw	vr03,	vr07,	4		;
	vsr8aw	vr03,	vr07,	6		;

	vmvzw	vr08,	vr04,	8		;	//vr01: 000 000 000 000 000 000 000 p0
	vsr8aw	vr04,	vr08,	2		;
	vsr8aw	vr04,	vr08,	4		;
	vsr8aw	vr04,	vr08,	6		;

	// create mask -- row1:
	vor		vr05, vr01, vr01	;
	vmr4aw	vr05, vr01, vr10	;

	// create mask -- row2:
	vor		vr06, vr02, vr02	;
	vmr4aw	vr06, vr02, vr10	;

	// create mask -- row3:
	vor		vr07, vr03, vr03	;
	vmr4aw	vr07, vr03, vr10	;

	// create mask -- row4:
	vor		vr08, vr04, vr04	;
	vmr4aw	vr08, vr04, vr10	;

	// load coefs -- sdm stride 32 - first 4 rows
	vld128	vr11, [i2, 0]	;	// row1a
	vld128	vr12, [i2, 16]	;	// row1b

	vld128	vr13, [i2, 32]	;	// row2a
	vld128	vr14, [i2, 48]	;	// row2b

	vld128	vr15, [i2, 64]	;	// row3a
	vld128	vr16, [i2, 80]	;	// row3b

	vld128	vr17, [i2, 96]	;	// row4a
	vld128	vr18, [i2, 112]	;	// row4b
	
	
	// add coef values to pred-values
	vaddw	vr11, vr11, vr05	; 
	vaddw	vr12, vr12, vr05	;

	vaddw	vr13, vr13, vr06	; 
	vaddw	vr14, vr14, vr06	;

	vaddw	vr15, vr15, vr07	; 
	vaddw	vr16, vr16, vr07	;

	vaddw	vr17, vr17, vr08	; 
	vaddw	vr18, vr18, vr08	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;

	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	vmovzw	vr23,64,2+8		// incs for ptrs
	vmovw	vr23,128,4

	//store the result
	vst64	vr11, [i3, 0]	; // out- row1a
	vst64	vr12, [i3, 8]	; // out- row1b

	vst64	vr13, [i3, 16]	; // out- row2a
	vst64	vr14, [i3, 24]	; // out- row2b
	
	vst64	vr15, [i3, 32]	; // out- row3a
	vst64	vr16, [i3, 40]	; // out- row3b

	vst64	vr17, [i3, 48]	; // out- row4a
	vst64	vr18, [i3, 56]	; // out- row4b
	
	vaddw	vr00,vr00,vr23	// inc ptrs

//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_Luma16DCVerMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the top line
	vld32wl	vr02, [i1, 0]	;	// vr02: 000 000 000 000 p03 p02 p01 p00
	vld32wl	vr03, [i1, 4]	;	// vr03: 000 000 000 000 p07 p06 p05 p04

	vld32wl	vr04, [i1, 8]	;	// vr04: 000 000 000 000 p11 p10 p09 p08
	vld32wl	vr05, [i1, 12]	;	// vr05: 000 000 000 000 p15 p14 p13 p12

	vaddw	vr06, vr02, vr03	;
	vaddaw	vr06, vr04, vr05	;

	vsr8aw	vr07, vr06, 2	;
	vsr8aw	vr07, vr06, 4	;
	vsr8aw	vr07, vr06, 6	;	// vr07: 00 00 00 00 xx xx xx dc

	//clear a register vr10;
	vsubw	vr10, vr10, vr10	;

	// round the dc output
	vasrrw	vr07, vr07, 4	;	// dc_prediction = (dc_prediction + 8) >> 4

	// make the remaining elements zero
	vmvzw	vr07, vr07, 1	;	// vr07: 00 00 00 00 00 00 00 dc

	// create mask
	vmr7aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 00 00 dc dc
	vmr6aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 00 dc dc dc
	vmr5aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 dc dc dc dc
	vmr4aw	vr01, vr07, vr10 ; // vr01: 00 00 00 dc dc dc dc dc
	vmr3aw	vr01, vr07, vr10 ; // vr01: 00 00 dc dc dc dc dc dc
	vmr2aw	vr01, vr07, vr10 ; // vr01: 00 dc dc dc dc dc dc dc
	vmr1aw	vr01, vr07, vr10 ; // vr01: dc dc dc dc dc dc dc dc
//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_Luma16DCHorMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the left pels
	vld32wl	vr01, [i1, 0]		;
	vld32wl	vr02, [i1, 16]		;
	vld32wl	vr03, [i1, 32]		;
	vld32wl	vr04, [i1, 48]		;

	vld32wl	vr05, [i1, 64]		;
	vld32wl	vr06, [i1, 80]		;
	vld32wl	vr07, [i1, 96]		;
	vld32wl	vr08, [i1, 112]		;

	vld32wl	vr09, [i1, 128]		;
	vld32wl	vr10, [i1, 144]		;
	vld32wl	vr11, [i1, 160]		;
	vld32wl	vr12, [i1, 176]		;

	vld32wl	vr13, [i1, 192]		;
	vld32wl	vr14, [i1, 208]		;
	vld32wl	vr15, [i1, 224]		;
	vld32wl	vr16, [i1, 240]		;

	// calculate the sum
	vaddw	vr01,	vr01, vr02	;
	vaddaw	vr01,	vr03, vr04	;

	vaddaw	vr01,	vr05, vr06	;
	vaddaw	vr01,	vr07, vr08	;

	vaddaw	vr01,	vr09, vr10	;
	vaddaw	vr01,	vr11, vr12	;

	vaddaw	vr01,	vr13, vr14	;
	vaddaw	vr01,	vr15, vr16	;

	// round the dc output
	vasrrw	vr07, vr01, 4	;	// dc_prediction = (dc_prediction + 8) >> 4

	//clear a dummy register
	vsubw	vr20, vr20, vr20 ;

	// make the remaining elements zero
	vmvzw	vr07, vr07, 8	; // vr07: 00 00 00 00 dc 00 00 00

	// create mask
	vsr8aw	vr02, vr07, 2	; // vr02: 00 00 00 00 dc dc 00 00
	vsr8aw	vr02, vr07, 4	; // vr02: 00 00 00 00 dc dc dc 00
	vsr8aw	vr02, vr07, 6	; // vr02: 00 00 00 00 dc dc dc dc

	vor		vr01, vr02, vr02 ;
	vmr4aw	vr01, vr02, vr20 ;
//-------------------------------------
    vendrec r0
.on	percent_reg
}



_Asm int Macro_Luma16DCVerAndHorMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the left pels
	vld32wl	vr01, [i2, 0]		;
	vld32wl	vr02, [i2, 16]		;
	vld32wl	vr03, [i2, 32]		;
	vld32wl	vr04, [i2, 48]		;

	vld32wl	vr05, [i2, 64]		;
	vld32wl	vr06, [i2, 80]		;
	vld32wl	vr07, [i2, 96]		;
	vld32wl	vr08, [i2, 112]		;

	vld32wl	vr09, [i2, 128]		;
	vld32wl	vr10, [i2, 144]		;
	vld32wl	vr11, [i2, 160]	    ;
	vld32wl	vr12, [i2, 176]		;

	vld32wl	vr13, [i2, 192]		;
	vld32wl	vr14, [i2, 208]		;
	vld32wl	vr15, [i2, 224]		;
	vld32wl	vr16, [i2, 240]		;

	// calculate the sum
	vaddw	vr01,	vr01, vr02	;
	vaddaw	vr01,	vr03, vr04	;

	vaddaw	vr01,	vr05, vr06	;
	vaddaw	vr01,	vr07, vr08	;

	vaddaw	vr01,	vr09, vr10	;
	vaddaw	vr01,	vr11, vr12	;

	vaddaw	vr01,	vr13, vr14	;
	vaddaw	vr01,	vr15, vr16	;

	// load the top line
	vld32wl	vr02, [i1, 0]		;	// vr02: 000 000 000 000 p03 p02 p01 p00
	vld32wl	vr03, [i1, 4]		;	// vr03: 000 000 000 000 p07 p06 p05 p04

	vld32wl	vr04, [i1, 8]		;	// vr04: 000 000 000 000 p11 p10 p09 p08
	vld32wl	vr05, [i1, 12]	;	// vr05: 000 000 000 000 p15 p14 p13 p12

	vaddw	vr02, vr02, vr03	;
	vaddaw	vr02, vr04, vr05	;	// sum of vert in lower 4 words

	vsr8aw	vr07, vr01, 6	;	//
	vsr8aw	vr07, vr02, 2	;
	vsr8aw	vr07, vr02, 4	;
	vsr8aw	vr07, vr02, 6	;

	//clear a register vr10;
	vsubw	vr10, vr10, vr10	;

	// round the dc output
	vasrrw	vr07, vr07, 5	;	// dc_prediction = (dc_prediction + 16) >> 5

	// make the remaining elements zero
	vmvzw	vr07, vr07, 1	;	// vr07: 00 00 00 00 00 00 00 dc

	// create mask
	vmr7aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 00 00 dc dc
	vmr6aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 00 dc dc dc
	vmr5aw	vr01, vr07, vr10 ; // vr01: 00 00 00 00 dc dc dc dc
	vmr4aw	vr01, vr07, vr10 ; // vr01: 00 00 00 dc dc dc dc dc
	vmr3aw	vr01, vr07, vr10 ; // vr01: 00 00 dc dc dc dc dc dc
	vmr2aw	vr01, vr07, vr10 ; // vr01: 00 dc dc dc dc dc dc dc
	vmr1aw	vr01, vr07, vr10 ; // vr01: dc dc dc dc dc dc dc dc
//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_LumaIntra16x8DCPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load coefs -- sdm stride 32 - first 4 rows
	vld128	vr11, [i2, 0]		;	// row1a
	vld128	vr12, [i2, 16]	;	// row1b

	vld128	vr13, [i2, 32]	;	// row2a
	vld128	vr14, [i2, 48]	;	// row2b

	vld128	vr15, [i2, 64]	;	// row3a
	vld128	vr16, [i2, 80]	;	// row3b

	vld128	vr17, [i2, 96]	;	// row4a
	vld128	vr18, [i2, 112]	;	// row4b

	// add coef values to pred-values
	vaddw	vr11, vr11, vr01	; 
	vaddw	vr12, vr12, vr01	;

	vaddw	vr13, vr13, vr01	; 
	vaddw	vr14, vr14, vr01	;

	vaddw	vr15, vr15, vr01	; 
	vaddw	vr16, vr16, vr01	;

	vaddw	vr17, vr17, vr01	; 
	vaddw	vr18, vr18, vr01	;

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

	// add coef values to pred-values
	vaddw	vr11, vr11, vr01	; 
	vaddw	vr12, vr12, vr01	;

	vaddw	vr13, vr13, vr01	; 
	vaddw	vr14, vr14, vr01	;

	vaddw	vr15, vr15, vr01	; 
	vaddw	vr16, vr16, vr01	;

	vaddw	vr17, vr17, vr01	; 
	vaddw	vr18, vr18, vr01	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;

	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;
	
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	vmovzw	vr23, 8*16*2, 4
	vmovw	vr23, 8*16, 8
	//store the result
	vst64	vr11, [i3, 64]	; // out- row1a
	vst64	vr12, [i3, 72]	; // out- row1b

	vst64	vr13, [i3, 80]	; // out- row2a
	vst64	vr14, [i3, 88]	; // out- row2b
	
	vst64	vr15, [i3, 96]	; // out- row3a
	vst64	vr16, [i3, 104]	; // out- row3b

	vst64	vr17, [i3, 112]	; // out- row4a
	vst64	vr18, [i3, 120]	; // out- row4b

	vaddw	vr00,vr00,vr23	// inc ptrs

//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_Luma16x16CalcPlaneModeParams(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// calculate H
	vld32wl	vr01, [i1, 0]  ;    // vr01: 000 000 000 000 p-1 xxx xxx xxx
	vld32wh	vr02, [i1, 4]  ;	// vr02: p03 p02 p01 p00 xxx xxx xxx xxx
	vld32wl vr02, [i1, 8]  ;    // vr02: p03 p02 p01 p00 p07 p06 p05 p04
	
	vld32wl	vr03, [i1, 12] ;
    vld32wh vr03, [i1, 16] ;    // vr03: p15 p14 p13 p12 p11 p10 p09 p08
    
	//load the multiplication factors
	vld128	vr09, [i3, 0] ;	// vr09: 008 007 006 005 004 003 002 001
	vld128	vr23, [i3, 16];	// vr23: 000  -1  -2  -3  -4  -5  -6  -7

	//shift vr01
	vsr8	vr06, vr01, 6 ;// vr01: 000 000 000 000 000 000 000 p-1

	// clear a register
	vsubw	vr10, vr10, vr10 ; //vr10: 00000000000

	// invert each half of vr02 to : p03 p02 p01 p00 p07 p06 p05 p04
	vsr8	vr04, vr02, 4 ; // vr04: 000 000 p03 p02 p01 p00 p07 p06
	vexch2	vr04, vr02	  ;	// vr04: p01 p00 p03 p02 p05 p04 p07 p06

	// invert each 32 bit word of vr02
	vsr8	vr05, vr04, 2 ;// vr05: 000 p01 p00 p03 p02 p05 p04 p07
	vexch1	vr05, vr04	;	// vr05: p00 p01 p02 p03 p04 p05 p06 p07

	vmr1w	vr06, vr06, vr05;//vr07: p-1 p00 p01 p02 p03 p04 p05 p06

	// calculate H
	vsubw	vr07, vr03, vr06 ;
	vmulw	vr07, vr07, vr09 ;

	vsr8aw	vr08, vr07, 2	;
	vsr8aw	vr08, vr07, 4	;
	vsr8aw	vr08, vr07, 6	;
	vsr8aw	vr08, vr07, 8	;
	vsr8aw	vr08, vr07, 10	;
	vsr8aw	vr08, vr07, 12	;
	vsr8aw	vr08, vr07, 14	;

	// calculate b
	vasrw	vr06, vr08, 2	;
	vaddaw	vr07, vr08, vr10;

	vasrrw	vr07, vr07, 4	;
    
	// calculate V
	vld32wl	vr17, [i2, 176]	;
	vld32wl	vr18, [i2,  48]	;

	vld32wl	vr15, [i2, 160]	;
	vld32wl	vr16, [i2,  64]	;

	vld32wl	vr13, [i2, 144]	;
	vld32wl	vr14, [i2,  80]	;

	vld32wl	vr11, [i2, 128]	;
	vld32wl	vr12, [i2,  96]	;

	vsubw	vr22, vr17, vr18 ;
	vsubw	vr21, vr15, vr16 ;
	vsubw	vr20, vr13, vr14 ;
	vsubw	vr19, vr11, vr12 ;

	vbmulaw	vr18, vr20, 2	; 
	vbmulaw	vr18, vr21, 3	; 
	vbmulaw	vr18, vr22, 4	; 

	vld32wl	vr11, [i2, 192]	;
	vld32wl	vr12, [i2,  32]	;

	vld32wl	vr13, [i2, 208]	;
	vld32wl	vr14, [i2,  16]	;

	vld32wl	vr15, [i2, 224]	;
	vld32wl	vr16, [i2,	 0]	;

	vld32wl	vr17, [i2, 240]	;	// *(I_ptr + 15)

	vsubw	vr19, vr11, vr12 ;
	vsubw	vr20, vr13, vr14 ;
	vsubw	vr21, vr15, vr16 ;
	vsubw	vr22, vr17, vr01 ;

	vbmulw	vr19, vr19, 5	;
	vbmulaw	vr19, vr20, 6	;
	vbmulaw	vr19, vr21, 7	;
	vbmulaw	vr19, vr22, 8	; // vr19: xx xx xx xx sum xx xx xx

	vaddaw	vr19, vr18, vr10 ;

	// c = ( (V + V/4) + 8 ) >> 4
	vasrw	vr20, vr19, 2	;
	vaddaw	vr20, vr19, vr10;

	vasrrw	vr20, vr20, 4	;

	// calculate a
	vsr8	vr11, vr17, 6	;
	vsr8aw	vr11, vr03, 14	;

	vbmulw	vr11, vr11, 16	;

	vmvzw	vr04, vr11, 1	;	// vr01: 00 00 00 00 00 00 00 a
	vmvzw	vr05, vr07, 1	;	// vr02: 00 00 00 00 00 00 00 b
	vmvzw	vr06, vr20, 8	;	// vr03: 00 00 00 00  c 00 00 00

	// create mask for c
	vsr8	vr03, vr06, 2 ;
	vsr8aw	vr03, vr06, 4 ;
	vsr8aw	vr03, vr06, 6 ;
	vmr7aw	vr03, vr06, vr10 ;
	vmr6aw	vr03, vr06, vr10 ;
	vmr5aw	vr03, vr06, vr10 ;
	vmr4aw	vr03, vr06, vr10 ;
	vaddaw	vr03, vr06, vr10 ;

	// create mask for b
	vmr7w	vr02, vr05, vr10 ;
	vmr6aw	vr02, vr05, vr10 ;
	vmr5aw	vr02, vr05, vr10 ;
	vmr4aw	vr02, vr05, vr10 ;
	vmr3aw	vr02, vr05, vr10 ;
	vmr2aw	vr02, vr05, vr10 ;
	vmr1aw	vr02, vr05, vr10 ;
	vaddaw	vr02, vr05, vr10 ;

	// create mask for a
	vmr7w	vr01, vr04, vr10 ;
	vmr6aw	vr01, vr04, vr10 ;
	vmr5aw	vr01, vr04, vr10 ;
	vmr4aw	vr01, vr04, vr10 ;
	vmr3aw	vr01, vr04, vr10 ;
	vmr2aw	vr01, vr04, vr10 ;
	vmr1aw	vr01, vr04, vr10 ;
	vaddaw	vr01, vr04, vr10 ;

	// create temp
	vbmulaw	vr04, vr03, (-8)	;

	vmulw	vr05, vr02, vr23	;
	vmulw	vr06, vr02, vr09	;

	vaddw	vr07, vr05, vr04	;
	vaddw	vr08, vr06, vr04	;

//-------------------------------------
    vendrec r0
.on	percent_reg
}


	// vr01: a
	// vr02: b
	// vr03: c
	// vr04: temp (for last row)
	// vr05: (last_temp) +  0 -1b -2b -3b -4b -5b -6b -7b
	// vr06: (last_temp) + 8b  7b  6b  5b  4b  3b  2b  1b


_Asm int Macro_Luma16x4IntraPlanarPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	
	// load coefs -- sdm stride 32 - first 4 rows
	vld128	vr16, [i1, 0]		;	// row1a
	vld128	vr17, [i1, 16]	;	// row1b

	vld128	vr18, [i1, 32]	;	// row2a
	vld128	vr19, [i1, 48]	;	// row2b

	// row1
	vaddw	vr07, vr07, vr03 ; // ROW1: add 'c' to vr07 and vr08
	vaddw	vr08, vr08, vr03 ;

	vasrrpwb	vr09, vr07, 5	; // ROW1: round / clip / pack
	vasrrpwb	vr10, vr08, 5	;

	// row2
	vaddw	vr07, vr07, vr03 ;	// row2: add 'c' to vr07 and vr08
	vaddw	vr08, vr08, vr03 ;
	
	vasrrpwb	vr11, vr07, 5	; // row2: round / clip / pack
	vasrrpwb	vr12, vr08, 5	;

	// unpack 
	vupbw	vr09, vr09	; // row1
	vupbw	vr10, vr10	;
	vupbw	vr11, vr11	; //row2
	vupbw	vr12, vr12	;

	// add coeffs 
	vaddw	vr09,	vr16, vr09	; //row1
	vaddw	vr10,	vr17, vr10	;
	vaddw	vr11,	vr18, vr11	; //row2
	vaddw	vr12,	vr19, vr12	;

	// clip and pack
	vasrpwb	vr09, vr09, 0	;	//row1
	vasrpwb	vr10, vr10, 0	;
	vasrpwb	vr11, vr11, 0	;	//row2
	vasrpwb	vr12, vr12, 0	;

	// store result
	vst64	vr09, [i2, 0]	;	
	vst64	vr10, [i2, 8]	;	
	vst64	vr11, [i2, 16]	;	
	vst64	vr12, [i2, 24]	;
	vaddw	vr00,vr00,vr23	;	inc ptrs

//-------------------------------------
    vendrec r0


.on	percent_reg
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordLuma16x16IntraPredMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the luma reconstruction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLuma16x16IntraPredMacros()
{
    Record(AM_Intra_Luma16x8VerPred,    Macro_LumaIntra16x8VerPred(NextAddr));
    Record(AM_Intra_Luma16x4HorPred,    Macro_LumaIntra16x4HorPred(NextAddr));
    Record(AM_Intra_Luma16DCVerMask,    Macro_Luma16DCVerMask(NextAddr));
    Record(AM_Intra_Luma16DCHorMask,    Macro_Luma16DCHorMask(NextAddr));
    Record(AM_Intra_Luma16DCVerAndHorMask, Macro_Luma16DCVerAndHorMask(NextAddr));
    Record(AM_Intra_Luma16x8DCPred,     Macro_LumaIntra16x8DCPred(NextAddr));
    Record(AM_Intra_Luma16x16CalcPlaneModeParams,     Macro_Luma16x16CalcPlaneModeParams(NextAddr));
    Record(AM_Intra_Luma16x4PlanarPred,     Macro_Luma16x4IntraPlanarPred(NextAddr));

}
