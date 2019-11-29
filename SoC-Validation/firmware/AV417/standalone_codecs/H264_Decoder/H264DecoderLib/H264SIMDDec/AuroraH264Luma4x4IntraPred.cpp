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

extern short Intra4x4MacroAddrTable[];
extern e_macro_index Intra4x4MacroTable[];
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec4")
#endif
_Asm int Macro_LumaIntra4x4VerPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld32wl vr01, [i1, 0]		;	// vr01: 000 000 000 000 p3 p2 p1 p0

	// load coefs
	vld64	vr02, [i2, 0]		;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr03, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr04, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr05, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12

	// add coef values to pred-values
	vaddw	vr02, vr02, vr01	; 
	vaddw	vr03, vr03, vr01	;
	vaddw	vr04, vr04, vr01	;
	vaddw	vr05, vr05, vr01	;

	// clip & pack the output
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;
	vasrpwb vr05, vr05, 0	;

	//store the result
	vst32	vr02, [i3, 0]	; // out- row1
	vst32	vr03, [i3, 16]	; // out- row2
	vst32	vr04, [i3, 32]	; // out- row3
	vst32	vr05, [i3, 48]	; // out- row4

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}


_Asm int Macro_LumaIntra4x4HorPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// clear 4 registers
	vsubw	vr01, vr01, vr01		;
	vsubw	vr02, vr02, vr02		;
	vsubw	vr03, vr03, vr03		;
	vsubw	vr04, vr04, vr04		;

	// clear a DUMMY register
	vsubw	vr10, vr10, vr10		;	//vr10 = 00....00

	// get the left line
	vld32wl	vr01, [i0, 0]		;
	vld32wl	vr02, [i0, 16]		;
	vld32wl	vr03, [i0, 32]		;
	vld32wl	vr04, [i0, 48]		;

	// shift right by 3 words
	vsr8	vr01,	vr01,	6		;	//vr01: 000 000 000 000 000 000 000 p0
	vsr8	vr02,	vr02,	6		;	//vr02: 000 000 000 000 000 000 000 
	vsr8	vr03,	vr03,	6		;	
	vsr8	vr04,	vr04,	6		;	

	// load the coefficients
	vld64	vr05, [i2, 0]	;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr06, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr07, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr08, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12

	// process row-1
	vaddw	vr05, vr05, vr01	;	//
	vmr7aw	vr05, vr01, vr10	;	// acc += 00 00 00 00 00 00 p0 00
	vmr6aw	vr05, vr01, vr10	;	// acc += 00 00 00 00 00 p0 00 00
	vmr5aw	vr05, vr01, vr10	;	// acc += 00 00 00 00 p0 00 00 00

	
	// process row-2
	vaddw	vr06, vr06, vr02	;	//
	vmr7aw	vr06, vr02, vr10	;	// acc += 00 00 00 00 00 00 p0 00
	vmr6aw	vr06, vr02, vr10	;	// acc += 00 00 00 00 00 p0 00 00
	vmr5aw	vr06, vr02, vr10	;	// acc += 00 00 00 00 p0 00 00 00

	// process row-3
	vaddw	vr07, vr07, vr03	;	//
	vmr7aw	vr07, vr03, vr10	;	// acc += 00 00 00 00 00 00 p0 00
	vmr6aw	vr07, vr03, vr10	;	// acc += 00 00 00 00 00 p0 00 00
	vmr5aw	vr07, vr03, vr10	;	// acc += 00 00 00 00 p0 00 00 00
	
	// process row-4
	vaddw	vr08, vr08, vr04	;	//
	vmr7aw	vr08, vr04, vr10	;	// acc += 00 00 00 00 00 00 p0 00
	vmr6aw	vr08, vr04, vr10	;	// acc += 00 00 00 00 00 p0 00 00
	vmr5aw	vr08, vr04, vr10	;	// acc += 00 00 00 00 p0 00 00 00

	// clip & pack the output
	vasrpwb vr05, vr05, 0	;
	vasrpwb vr06, vr06, 0	;
	vasrpwb vr07, vr07, 0	;
	vasrpwb vr08, vr08, 0	;

	// store the result
	vst32	vr05, [i3, 0]	; // out- row1
	vst32	vr06, [i3, 16]	; // out- row2
	vst32	vr07, [i3, 32]	; // out- row3
	vst32	vr08, [i3, 48]	; // out- row4

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}


_Asm int Macro_LumaIntra4x4DCVer(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld32wl vr01, [i1, 0]	;	// vr01: 000 000 000 000 p3 p2 p1 p0

	// load the coefficients
	vld64	vr05, [i2, 0]	;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr06, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr07, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr08, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12

	// calculate the sum of top line
	vsr8	vr02, vr01,	2	;	//vr02: 000 000 000 000 000 p3 p2 p1
	
	vaddw	vr01, vr01, vr02	;	// acc(0) = p(0) + p(1)
	vsr8aw	vr01, vr02, 2		;	// acc(0) = acc(0) + p(2)
	vsr8aw	vr01, vr02, 4		;	// acc(1) = acc(0) + p(3)

	vmvzw	vr02,	vr01,	0x1	;	// move the result to vr02

	// clear a dummy register
	vsubw	vr10,	vr10,	vr10;

	vasrrw	vr02,	vr02,	2	;	// !!round the dc value!!

	// create a mask with the rounded value
	// at this point vr02 = acc = 000 000 000 000 000 000 000 dc
	vmr7aw	vr03, vr02, vr10	; // acc = 00 00 00 00 00 00 dc dc
	vmr6aw	vr03, vr02, vr10	; // acc = 00 00 00 00 00 dc dc dc
	vmr5aw	vr03, vr02, vr10	; // acc = 00 00 00 00 dc dc dc dc = vr03

	vaddw	vr05, vr05, vr03	;
	vaddw	vr06, vr06, vr03	;
	vaddw	vr07, vr07, vr03	;
	vaddw	vr08, vr08, vr03	;

	// clip & pack the output
	vasrpwb vr05, vr05, 0	;
	vasrpwb vr06, vr06, 0	;
	vasrpwb vr07, vr07, 0	;
	vasrpwb vr08, vr08, 0	;

	// store the result
	vst32	vr05, [i3, 0]	; // out- row1
	vst32	vr06, [i3, 16]	; // out- row2
	vst32	vr07, [i3, 32]	; // out- row3
	vst32	vr08, [i3, 48]	; // out- row4

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}


_Asm int Macro_LumaIntra4x4DCHor(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// get the left line
	vld32wl	vr01, [i0,  0]	;
	vld32wl	vr02, [i0, 16]	;
	vld32wl	vr03, [i0, 32]	;
	vld32wl	vr04, [i0, 48]	;

	// clear a dummy register
	vsubw	vr11,	vr11,	vr11;

	// add the four values
	vaddw	vr01, vr01, vr02	;
	vaddaw	vr01, vr03, vr04	;

	// move the result to vr10 to ensure that only dc-bits are non-zero
	vmvzw	vr10,	vr01,	0x8	;	// vr10: 00 00 00 00 sum 00 00 00

	// round and clip
	vasrrw	vr10, vr10, 2	;		// vr10: 00 00 00 00 dc 00 00 00

	// create mask
	// at this point	   acc = vr10 = 00 00 00 00 dc 00 00 00
	vsr8aw	vr03, vr10, 2	; // acc = 00 00 00 00 dc dc 00 00
	vsr8aw	vr03, vr10, 4	; // acc = 00 00 00 00 dc dc dc 00
	vsr8aw	vr03, vr10, 6	; // acc = 00 00 00 00 dc dc dc dc = vr03

	// load the coefficients
	vld64	vr05, [i2, 0]	;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr06, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr07, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr08, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12

	vaddw	vr05, vr05, vr03	;
	vaddw	vr06, vr06, vr03	;
	vaddw	vr07, vr07, vr03	;
	vaddw	vr08, vr08, vr03	;

	// clip & pack the output
	vasrpwb vr05, vr05, 0	;
	vasrpwb vr06, vr06, 0	;
	vasrpwb vr07, vr07, 0	;
	vasrpwb vr08, vr08, 0	;

	// store the result
	vst32	vr05, [i3, 0]	; // out- row1
	vst32	vr06, [i3, 16]	; // out- row2
	vst32	vr07, [i3, 32]	; // out- row3
	vst32	vr08, [i3, 48]	; // out- row4


//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}




_Asm int Macro_LumaIntra4x4DCVerAndHor(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld32wl vr01, [i1, 0]	;	// vr01: xx xx xx xx p3 p2 p1 p0

	// get the left line - sdm stride = 16
	vld32wl	vr02, [i0,  0]	;	// vr02:
	vld32wl	vr03, [i0, 16]	;	// vr03:
	vld32wl	vr04, [i0, 32]	;	// vr04:
	vld32wl	vr05, [i0, 48]	;	// vr05: 

	//sum the elements of the top line and the left line
	// calculate the sum of top line !!VR20 is DUMMY!!
	vor		vr06, vr01, vr01	;	// acc = xx xx xx xx p3 p2 p1 p0
	vmr7aw	vr06, vr01, vr20	;  // acc = xx xx xx xx (p3 + p2) xx xx xx
	vmr6aw	vr06, vr01, vr20	;	// acc = xx xx xx xx (p3 + p2 + p1) xx xx xx
	vmr5aw	vr06, vr01, vr20	;	// acc = xx xx xx xx (p3 + p2 + p1 + p0) xx xx xx
									// vr06 = acc

	// at this point acc(3rd word) has the sum of top line
	// now calculate sum of left line
	vaddaw	vr06, vr02, vr03	;	// 
	vaddaw	vr06, vr04, vr05	;	// vr06 = xx xx xx xx DC_SUM xx xx xx

	// 3rd word of vr10 has the result
	vmvzw	vr10,	vr06,	8	;	// vr06 = 00 00 00 00 DC_SUM 00 00 00

	// round the dc value
	vasrrw	vr10, vr10, 3	;		// vr10: 00 00 00 00 dc 00 00 00

	// create mask
	// at this point	   acc = vr10 = 00 00 00 00 dc 00 00 00
	vsr8aw	vr03, vr10, 2	; // acc = 00 00 00 00 dc dc 00 00
	vsr8aw	vr03, vr10, 4	; // acc = 00 00 00 00 dc dc dc 00
	vsr8aw	vr03, vr10, 6	; // acc = 00 00 00 00 dc dc dc dc = vr03

	// load the coefficients - SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12
	
	vaddw	vr11, vr11, vr03	;
	vaddw	vr12, vr12, vr03	;
	vaddw	vr13, vr13, vr03	;
	vaddw	vr14, vr14, vr03	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4

//-------------------------------------
    vendrec r0

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif




_Asm int Macro_LumaIntra4x4DCConst(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// clear a register
	vsubw	vr01, vr01, vr01	;
	// create mask
	vbaddw	vr01, vr01, 128		;	// add 128 to all words of vr01

	// load coefs
	vld64	vr02, [i2, 0]		;	// vr02: 000 000 000 000 w3  w2  w1  w0
	vld64	vr03, [i2, 32]	;	// vr03: 000 000 000 000 w7  w6  w5  w4
	vld64	vr04, [i2, 64]	;	// vr04: 000 000 000 000 w11 w10 w9  w8
	vld64	vr05, [i2, 96]	;	// vr05: 000 000 000 000 w15 w14 w13 w12

	// add coef values to dc-value
	vaddw	vr02, vr02, vr01	; 
	vaddw	vr03, vr03, vr01	;
	vaddw	vr04, vr04, vr01	;
	vaddw	vr05, vr05, vr01	;

	// clip & pack the output
	vasrpwb vr02, vr02, 0	;
	vasrpwb vr03, vr03, 0	;
	vasrpwb vr04, vr04, 0	;
	vasrpwb vr05, vr05, 0	;

	//store the result
	vst32	vr02, [i3, 0]	; // out- row1
	vst32	vr03, [i3, 16]	; // out- row2
	vst32	vr04, [i3, 32]	; // out- row3
	vst32	vr05, [i3, 48]	; // out- row4

//-------------------------------------
    vendrec r0

.on	percent_reg
}






_Asm int Macro_LumaIntra4x4DiagDownLeftWOExtrp(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld32wl vr01, [i1, 0]	;	// vr01: 00 00 00 00 p3 p2 p1 p0
	vld32wh	vr01, [i1, 4]	;	// vr01: p7 p6 p5 p4 p3 p2 p1 p0

	// load the coefficients - SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]		;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	vsr8	vr07, vr01, 14	;	// vr06: 00 00 00 00 00 00 00 p7
	
	// shift the rows
	vmr1w	vr02, vr07, vr01	;	// vr02: p7 p7 p6 p5 p4 p3 p2 p1
	vmr2w	vr03, vr07, vr01	;	// vr03: 00 p7 p7 p6 p5 p4 p3 p2
	vmr3w	vr04, vr07, vr01	;	// vr04: 00 00 p7 p7 p6 p5 p4 p3
	vmr4w	vr05, vr07, vr01	;	// vr05: 00 00 00 p7 p7 p6 p5 p4
	vmr5w	vr06, vr07, vr01	;	// vr06: 00 00 00 00 p7 p7 p6 p5

	// row1 (a+2b+c) (b+2c+d) (c+2d+e) (d+2e+f) ((+2)>>2)
	vaddw	vr01, vr01, vr03	;
	vbmulaw	vr01, vr02, 2		;	// output - row1
	
	// row2 (b+2c+d) (c+2d+e) (d+2e+f) (e+2f+g) ((+2)>>2)
	vaddw	vr02, vr02, vr04	;
	vbmulaw	vr02, vr03, 2		;	// output - row2

	// row3 (c+2d+e) (d+2e+f) (e+2f+g) (f+2g+h) ((+2)>>2)
	vaddw	vr03, vr03, vr05	;
	vbmulaw	vr03, vr04, 2		;	// ouput - row3

	// row4 (d+2e+f) (e+2f+g) (f+2g+h) (g+3h) ((+2)>>2)
	vaddw	vr04, vr04, vr06	;
	vbmulaw	vr04, vr05, 2		;	// output - row5

	// round the filter output (add 2 and div by 4)
	vasrrw	vr01, vr01, 2		;
	vasrrw	vr02, vr02, 2		;
	vasrrw	vr03, vr03, 2		;
	vasrrw	vr04, vr04, 2		;

	// add the coeffs
	vaddw	vr11, vr11, vr01	;
	vaddw	vr12, vr12, vr02	;
	vaddw	vr13, vr13, vr03	;
	vaddw	vr14, vr14, vr04	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]  ; // out- row4


//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}





_Asm int Macro_LumaIntra4x4DiagDownLeftWithExtrp(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


    vsubw   vr01, vr01, vr01        ;       // vr01: 00 00 00 00 00 00 00 00

	// load top line
	vld32wl vr01, [i1, 0]		;	// vr01: 00 00 00 00 p3 p2 p1 p0

	// load the coefficients - SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]		;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// extrapolate the last pel
	vsr8	vr07, vr01, 6	;	// vr07: 00 00 00 00 00 00 00 p3
	vsubw	vr08, vr08, vr08 ; // vr08: 00 00 00 00 00 00 00 00

	vmr4w	vr09, vr07, vr08 ;	// vr09: 00 00 00 p3 00 00 00 00
	vmr3aw	vr09, vr07, vr08 ;	// vr09: 00 00 p3 p3 00 00 00 00
	vmr2aw	vr09, vr07, vr08 ;	// vr09: 00 p3 p3 p3 00 00 00 00
	vmr1aw	vr09, vr07, vr08 ;	// vr09: p3 p3 p3 p3 00 00 00 00 = ACC

	vaddaw	vr01, vr08, vr01 ; // vr01 = ACC (vr09) + vr08(=0) + vr01
								// vr01: p3 p3 p3 p3 p3 p2 p1 p0
	// shift the rows
	vmr1w	vr02, vr07, vr01	;	// vr02: p7 p7 p6 p5 p4 p3 p2 p1
	vmr2w	vr03, vr07, vr01	;	// vr03: 00 p7 p7 p6 p5 p4 p3 p2
	vmr3w	vr04, vr07, vr01	;	// vr04: 00 00 p7 p7 p6 p5 p4 p3
	vmr4w	vr05, vr07, vr01	;	// vr05: 00 00 00 p7 p7 p6 p5 p4
	vmr5w	vr06, vr07, vr01	;	// vr06: 00 00 00 00 p7 p7 p6 p5

	// row1 (a+2b+c) (b+2c+d) (c+2d+e) (d+2e+f)
	vaddw	vr01, vr01, vr03	;
	vbmulaw	vr01, vr02, 2		;	// output - row1
	
	// row2 (b+2c+d) (c+2d+e) (d+2e+f) (e+2f+g)
	vaddw	vr02, vr02, vr04	;
	vbmulaw	vr02, vr03, 2		;	// output - row2

	// row3 (c+2d+e) (d+2e+f) (e+2f+g) (f+2g+h)
	vaddw	vr03, vr03, vr05	;
	vbmulaw	vr03, vr04, 2		;	// ouput - row3

	// row4 (d+2e+f) (e+2f+g) (f+2g+h) (g+3h)
	vaddw	vr04, vr04, vr06	;
	vbmulaw	vr04, vr05, 2		;	// output - row5

	// round the filter output add 2 and div by 4
	vasrrw	vr01, vr01, 2		;
	vasrrw	vr02, vr02, 2		;
	vasrrw	vr03, vr03, 2		;
	vasrrw	vr04, vr04, 2		;

	// add the coeffs
	vaddw	vr11, vr11, vr01	;
	vaddw	vr12, vr12, vr02	;
	vaddw	vr13, vr13, vr03	;
	vaddw	vr14, vr14, vr04	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4


//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}



_Asm int Macro_LumaIntra4x4VerLeftWOExtrp(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load top line
	vld32wl vr01, [i1, 0]	;	// vr01: 00 00 00 00 p3 p2 p1 p0
	vld32wh	vr01, [i1, 4]	;	// vr01: p7 p6 p5 p4 p3 p2 p1 p0

	// load the coefficients - SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// shift rows				// vr01: p7 p6 p5 p4 p3 p2 p1 p0
	vsr8	vr02, vr01, 2	;	// vr02: 00 p7 p6 p5 p4 p3 p2 p1
	vsr8	vr03, vr01, 4	;	// vr03: 00 00 p7 p6 p5 p4 p3 p2
	vsr8	vr04, vr01, 6	;	// vr04: 00 00 00 p7 p6 p5 p4 p3

	// row1 (a+b) (b+c) (c+d) (d+e)
	vaddw	vr05, vr01, vr02 ;

	// row2 (a+2b+c) (b+2c+d) (c+2d+e) (d+2e+f)
	vaddw	vr06, vr01, vr03 ;
	vbmulaw vr06, vr02, 2	 ;

	// row3 (b+c) (c+d) (d+e) (e+f)
	vaddw	vr07, vr02, vr03 ;

	// row4 (b+2c+d) (c+2d+e) (d+2e+f) (e+2f+g)
	vaddw	vr08, vr02, vr04 ;
	vbmulaw vr08, vr03, 2	 ;

	// round the filter output
	vasrrw	vr05, vr05, 1		;	// row1
	vasrrw	vr06, vr06, 2		;	// row2
	vasrrw	vr07, vr07, 1		;	// row3
	vasrrw	vr08, vr08, 2		;	// row4

	// add coeffs
	vaddw	vr11, vr11, vr05	;
	vaddw	vr12, vr12, vr06	;
	vaddw	vr13, vr13, vr07	;
	vaddw	vr14, vr14, vr08	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}


_Asm int Macro_LumaIntra4x4VerLeftWithExtrp(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


    vsubw   vr01, vr01, vr01        ;       // vr01: 00 00 00 00 00 00 00 00

	// load top line
	vld32wl vr01, [i1, 0]		;	// vr01: 00 00 00 00 p3 p2 p1 p0

	// load the coefficients - SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// extrapolate the last pel 'd/p3'
	vsr8	vr07, vr01, 6	;	// vr07: 00 00 00 00 00 00 00 p3
	vsubw	vr08, vr08, vr08 ; // vr08: 00 00 00 00 00 00 00 00

	vmr4w	vr09, vr07, vr08 ;	// vr09: 00 00 00 p3 00 00 00 00
	vmr3aw	vr09, vr07, vr08 ;	// vr09: 00 00 p3 p3 00 00 00 00
	vmr2aw	vr09, vr07, vr08 ;	// vr09: 00 p3 p3 p3 00 00 00 00

	vaddaw	vr01, vr08, vr01 ; // vr01 = ACC (vr09) + vr08(=0) + vr01
								// vr01: 00 p3 p3 p3 p3 p2 p1 p0
	
	// shift rows				// vr01: 00 p3 p3 p3 p3 p2 p1 p0
	vsr8	vr02, vr01, 2	;	// vr02: 00 00 p3 p3 p3 p3 p2 p1
	vsr8	vr03, vr01, 4	;	// vr03: 00 00 00 p3 p3 p3 p3 p2
	vsr8	vr04, vr01, 6	;	// vr04: 00 00 00 00 p3 p3 p3 p3

	// row1 (a+b) (b+c) (c+d) (d+e)
	vaddw	vr05, vr01, vr02 ;

	// row2 (a+2b+c) (b+2c+d) (c+2d+e) (d+2e+f)
	vaddw	vr06, vr01, vr03 ;
	vbmulaw vr06, vr02, 2	 ;

	// row3 (b+c) (c+d) (d+e) (e+f)
	vaddw	vr07, vr02, vr03 ;

	// row4 (b+2c+d) (c+2d+e) (d+2e+f) (e+2f+g)
	vaddw	vr08, vr02, vr04 ;
	vbmulaw vr08, vr03, 2	 ;

	// round the filter output
	vasrrw	vr05, vr05, 1		;	// row1
	vasrrw	vr06, vr06, 2		;	// row2
	vasrrw	vr07, vr07, 1		;	// row3
	vasrrw	vr08, vr08, 2		;	// row4

	// add coeffs
	vaddw	vr11, vr11, vr05	;
	vaddw	vr12, vr12, vr06	;
	vaddw	vr13, vr13, vr07	;
	vaddw	vr14, vr14, vr08	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}

//case 4





_Asm int Macro_LumaIntra4x4DiagDownRight(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// clear register for last left pel & a dummy reg
	vsubw	vr06, vr06, vr06	;
	vsubw	vr10, vr10, vr10	;

	// load top line
	vld32wh vr01, [i1, 0]	;	// vr01: pd pc pb pa xx xx xx xx

	// load the top left pel
	vld32wl vr01, [i5, 0]	;	// vr01: pd pc pb pa pm xx xx xx
	
	// load the left pels
	vld32wl	vr03, [i0, 0]	;	// vr03: xx xx xx xx pi xx xx xx
	vld32wl	vr04, [i0, 16]	;	// vr04: xx xx xx xx pj xx xx xx
	vld32wl	vr05, [i0, 32]	;	// vr05: xx xx xx xx pk xx xx xx
	vld32wl	vr06, [i0, 48]	;	// vr06: 00 00 00 00 pl xx xx xx

	// shift the left pels into position
	vsr8	vr03, vr03, 2	;	// vr03: xx xx xx xx xx pi xx xx
	vsr8	vr04, vr04, 4	;	// vr04: xx xx xx xx xx xx pj xx
	vsr8	vr05, vr05, 6	;	// vr05: xx xx xx xx xx xx xx pk
	vsr8	vr06, vr06, 6	;	// vr06: 00 00 00 00 00 00 00 pl

	// put the values in the correst places 
	vmvw	vr01, vr03, 4	;	// vr01: pd pc pb pa pm pi xx xx
	vmvw	vr01, vr04, 2	;	// vr01: pd pc pb pa pm pi pj xx
	vmvw	vr01, vr05, 1	;	// vr01: pd pc pb pa pm pi pj pk

	vmr1w	vr06, vr06, vr10 ;	// vr06: pl 00 00 00 00 00 00 00
	vsr8	vr07, vr01, 14	;	// vr07: 00 00 00 00 00 00 00 pd
	vmr7w	vr01, vr01, vr06 ; // vr01: pc pb pa pm pi pj pk pl

	// load the coefficients rows1&2- SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4

	//shift regs				
								// vr01: pc pb pa pm pi pj pk pl
	vmr1w	vr02, vr07, vr01 ; // vr02: pd pc pb pa pm pi pj pk
	vmr2w	vr03, vr07, vr01 ; // vr03: 00 pd pc pb pa pm pi pj

	// generate output
	// row4 (l+2k+j) (k+2j+i) (j+2i+m) (i+2m+a) (m+2a+b) (a+2b+c) (b+2c+d)
	vaddw	vr10, vr03, vr01 ;
	vbmulaw vr10, vr02, 2	 ;

	// load the coefficients rows3&4 - SDM stride = 16*2 = 32
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// round the output
	vasrrw	vr10, vr10, 2	;	// row4

	// stall
	// stall

	// generate the remaining rows
	// row3 (k+2j+i) (j+2i+m) (i+2m+a) (a+2b+c)
	vsr8	vr09, vr10, 2	;

	// row2 (j+2i+m) (i+2m+a) (m+2a+b) (a+2b+c)
	vsr8	vr08, vr10, 4	;

	// row1 (i+2m+a) (m+2a+b) (a+2b+c) (b+2c+d)
	vsr8	vr07, vr10, 6	;

	// add coeffs
	vaddw	vr14, vr14, vr10	;
	vaddw	vr13, vr13, vr09	;
	vaddw	vr11, vr11, vr07	;
	vaddw	vr12, vr12, vr08	;

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4

//-------------------------------------
    vendrec r0
.on	percent_reg
}
//case 5




_Asm int Macro_LumaIntra4x4VerRight(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load top line
	vld32wh vr01, [i1, 0]		;	// vr01: pd pc pb pa xx xx xx xx

	// load the top left pel
	vld32wl vr01, [i5, 0]		;	// vr01: pd pc pb pa pm xx xx xx

	// load the left pels (we do not require 'l')
	vld32wl	vr03, [i0, 0]	;	// vr03: xx xx xx xx pi xx xx xx
	vld32wl	vr04, [i0, 16]	;	// vr04: xx xx xx xx pj xx xx xx
	vld32wl	vr05, [i0, 32]	;	// vr05: xx xx xx xx pk xx xx xx

	// shift the pels into position
	vsr8	vr03, vr03, 2	;	// vr03: xx xx xx xx xx pi xx xx
	vsr8	vr04, vr04, 4	;	// vr04: xx xx xx xx xx xx pj xx
	vsr8	vr05, vr05, 6	;	// vr05: xx xx xx xx xx xx xx pk

	// put the values in the correst places in vr01
	vmvw	vr01, vr03, 4	;	// vr01: pd pc pb pa pm pi xx xx
	vmvw	vr01, vr04, 2	;	// vr01: pd pc pb pa pm pi pj xx
	vmvw	vr01, vr05, 1	;	// vr01: pd pc pb pa pm pi pj pk

	// load the coefficients rows1&2- SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// create shifted versions of vr01=
	vsr8    vr05, vr01, 8	;	// vr05: 00 00 00 00 pd pc pb pa
	vsr8	vr04, vr01, 6	;	// vr04: 00 00 00 pd pc pb pa pm
	vsr8	vr03, vr01, 4	;	// vr03: 00 00 pd pc pb pa pm pi
	vsr8	vr02, vr01, 2	;	// vr02: 00 pd pc pb pa pm pi pj
								// vr01: pd pc pb pa pm pi pj pk

	// compute rows: 1, 2, 3a, 3b, 4a, 4b

	// row4a	(k+2j+i) [j+2i+m] (i+2m+a) (m+2a+b) (a+2b+c) [b+2c+d]
	vaddw	vr10, vr01, vr03	;
	vbmulaw	vr10, vr02, 2		;

	// row1 (m+a) (a+b) (b+c) (c+d)
	vaddw	vr06, vr04, vr05	;

	// row3b [i+m] (m+a) (a+b) (b+c)
	vaddw	vr09, vr03, vr04	;

	// round the output (row4a)
	vasrrw	vr10, vr10, 2		;

	//round row1 and 3b
	vasrrw	vr06, vr06, 1		;	//row1
	vasrrw	vr09, vr09, 1		;	//row3b

	// row2= (row4a>>4) = (i+2m+a) (m+2a+b) (a+2b+c) (b+2c+d)
	vsr8	vr07, vr10, 4		;

	// row3a = (row4a>>2) (j+2i+m) [i+2m+a] [m+2a+b] [a+2b+c]
	vsr8	vr08, vr10, 2		;

	// move the first element of 3a to 3b to generate row3
	vmvw	vr09, vr08, 1		; //vr09 (j+2i+m) (m+a) (a+b) (b+c)

	// mov 1st element of 4a to 3a to get row4
	vmvw	vr08, vr10, 1		; // vr08 (k+2j+i) (i+2m+a) (m+2a+b) (a+2b+c)
	
	// add the coeffs to the mask
	vaddw	vr11, vr11, vr06	;	//row1
	vaddw	vr12, vr12, vr07	;	//row2
	vaddw	vr13, vr13, vr09	;	//row3
	vaddw	vr14, vr14, vr08	;	//row4

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4
//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_LumaIntra4x4HorDown(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load top line
	vld32wh vr01, [i1, 0]		;	// vr01: pd pc pb pa xx xx xx xx

	// load the top left pel
	vld32wl vr01, [i5, 0]		;	// vr01: pd pc pb pa pm xx xx xx

	// load the left pels
	vld32wl	vr03, [i0, 0]	;	// vr03: xx xx xx xx pi xx xx xx
	vld32wl	vr04, [i0, 16]	;	// vr04: xx xx xx xx pj xx xx xx
	vld32wl	vr05, [i0, 32]	;	// vr05: xx xx xx xx pk xx xx xx
	vld32wl	vr06, [i0, 48]	;	// vr06: xx xx xx xx pl xx xx xx

	vmr7w	vr01, vr01, vr20;	// vr01: pc pb pa pm xx xx xx xx
	
	vmvw	vr01, vr03, 8	;	// vr01: pc pb pa pm pi xx xx xx

	vsr8	vr04, vr04, 2	;	// vr04: xx xx xx xx xx pj xx xx
	vsr8	vr05, vr05, 4	;	// vr05: xx xx xx xx xx xx pk xx
	vsr8	vr06, vr06, 6	;	// vr05: xx xx xx xx xx xx xx pl

	vmvw	vr01, vr04, 4	;	// vr01: pc pb pa pm pi pj xx xx
	vmvw	vr01, vr05, 2	;  // vr01: pc pb pa pm pi pj pk xx
	vmvw	vr01, vr06, 1	;  // vr01: pc pb pa pm pi pj pk pl

	// load the coefficients rows1&2- SDM stride = 16*2 = 32
	vld64	vr15, [i2, 0]	;	// vr15: 000 000 000 000 w3  w2  w1  w0
	vld64	vr16, [i2, 32]	;	// vr16: 000 000 000 000 w7  w6  w5  w4
	vld64	vr17, [i2, 64]	;	// vr17: 000 000 000 000 w11 w10 w9  w8
	vld64	vr18, [i2, 96]	;	// vr18: 000 000 000 000 w15 w14 w13 w12

	vsr8	vr02, vr01, 2	;	// vr02: 00 pc pb pa pm pi pj pk
	vsr8	vr03, vr01, 4	;	// vr03: 00 00 pc pb pa pm pi pj
	vsr8	vr04, vr01, 6	;	// vr04: 00 00 00 pc pb pa pm pi
	vsr8	vr05, vr01, 8	;	// vr05: 00 00 00 00 pc pb pa pm

	// sum2 (l+k) (k+j) (j+i) (i+m) (m+a) (a+b) (b+c)
	vaddw	vr13, vr01, vr02 ;	

	// sum3 (l+2k+j) (k+2j+i) (j+2i+m) (i+2m+a) (m+2a+b) (a+2b+c) xxx xxx
	vaddw	vr14, vr01, vr03 ;
	vbmulaw  vr14, vr02, 2	 ;

	// round sum2/sum3
	vasrrw	vr13, vr13, 1	;
	vasrrw	vr14, vr14, 2	;

	// for row1
	vsr8	vr05, vr13,  6	;	// "(i+m)" (m+a) (a+b) (b+c)
	
	// for row2
	vsr8	vr06, vr13,  4	;	// "(j+i)"    (i+m)     (m+a)    (a+b)    (b+c)
	vsr8	vr07, vr14,  2  ;	// (k+2j+i) "(j+2i+m)" (i+2m+a) (m+2a+b) (a+2b+c) xxx
	vsr8	vr08, vr13,  2	;	//  (k+j)     (j+i)     "(i+m)"  (m+a)    (a+b) (b+c)
							//vr14: (l+2k+j) (k+2j+i) (j+2i+m) "(i+2m+a)" (m+2a+b) (a+2b+c) xxx

	// for row3
							//vr08: "(k+j)"    (j+i)      (i+m)      (m+a)      (a+b) 
							//vr14: (l+2k+j) "(k+2j+i)"  (j+2i+m)   (i+2m+a)
							//vr13:  (l+k)     (k+j)     "(j+i)"    (i+m)      (m+a)
	vmr7w	vr09, vr14, vr20 ;	//    XXX     (l+2k+j)   (k+2j+i)  "(j+2i+m)"  (i+2m+a) 

	// for row4
							//vr13:  "(l+k)"     (k+j)     (j+i)      (i+m)      (m+a)
							//vr09:    XXX    "(l+2k+j)"  (k+2j+i)   (j+2i+m)  (i+2m+a)
	vmr7w	vr10, vr13, vr20 ; //     XXX       (l+k)    "(k+j)"     (j+i)      (i+m)
	vmr6w	vr11, vr14, vr20 ; //     XXX        XXX     (l+2k+j)  "(k+2j+i)" (j+2i+m)

	// compute row1 -- vr01
	vsr8	vr01, vr14, 4	; //vr01: ----   (i+2m+a) (m+2a+b) (a+2b+c)
	vmvw	vr01, vr05, 1	; //vr01: (i+m)  (i+2m+a) (m+2a+b) (a+2b+c)

	//compute row2 -- vr06
	vmvw	vr06, vr07, 2	; //vr06: (j+i)  (j+2i+m)   (m+a)    (a+b)    (b+c)
	vmvw	vr06, vr08, 4	; //vr06: (j+i)  (j+2i+m)   (i+m)   (a+b)     (b+c)
	vmvw	vr06, vr14, 8	; //vr06: (j+i)  (j+2i+m)   (i+m)  "(i+2m+a)" (b+c)

	//compute row3 -- vr14
	vmvw	vr14, vr08, 1	; //vr14: "(k+j)" (k+2j+i)  (j+2i+m)   (i+2m+a)
	vmvw	vr14, vr13, 4	; //vr14:  (k+j)  (k+2j+i)  "(j+i)"    (i+2m+a)
	vmvw	vr14, vr09, 8	; //vr14:  (k+j)  (k+2j+i)   (j+i)    "(j+2i+m)"

	//compute row4:-- vr13
	vmvw	vr13, vr09, 2	; //vr13: "(l+k)"  "(l+2k+j)"     (j+i)      (i+m)
	vmvw	vr13, vr10, 4	; //vr13: "(l+k)"  "(l+2k+j)"    "(k+j)"     (i+m)
	vmvw	vr13, vr11, 8	; //vr13: "(l+k)"  "(l+2k+j)"    "(k+j)"   "(k+2j+i)" 

	// add the coeffs to the mask
	vaddw	vr15, vr15, vr01	;	//row1
	vaddw	vr16, vr16, vr06	;	//row2
	vaddw	vr17, vr17, vr14	;	//row3
	vaddw	vr18, vr18, vr13	;	//row4

	// clip & pack the output
	vasrpwb vr15, vr15, 0	;
	vasrpwb vr16, vr16, 0	;
	vasrpwb vr17, vr17, 0	;
	vasrpwb vr18, vr18, 0	;

	// store the result - sdm stride = 16
	vst32	vr15, [i3, 0]	; // out- row1
	vst32	vr16, [i3, 16]	; // out- row2
	vst32	vr17, [i3, 32]	; // out- row3
	vst32	vr18, [i3, 48]	; // out- row4
	
//-------------------------------------
    vendrec r0
.on	percent_reg
}


_Asm int Macro_LumaIntra4x4HorUp(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	vsubw	vr01, vr01, vr01	;  // clear vr06
	vsubw	vr10, vr10, vr10	;  // clear vr10

	// load the left pels
	vld32wl	vr03, [i0, 0]	;	// vr03: xx xx xx xx pi xx xx xx
	vld32wl	vr04, [i0, 16]	;	// vr04: xx xx xx xx pj xx xx xx
	vld32wl	vr05, [i0, 32]	;	// vr05: xx xx xx xx pk xx xx xx
	vld32wl	vr01, [i0, 48]	;	// vr01: 00 00 00 00 pl xx xx xx

	// shift the pels into position
	vsr8	vr03, vr03, 6	;	// vr03: 00 00 00 xx xx xx xx pi
	vsr8	vr04, vr04, 4	;	// vr04: 00 00 xx xx xx xx pj xx
	vsr8	vr05, vr05, 2	;	// vr05: 00 xx xx xx xx pk xx xx

	vsr8	vr02, vr01,	6	;	// vr02: 00 00 00 00 00 00 00 pl
//	vsr8aw	vr02, vr01, 4	;	// vr02: 00 00 00 00 00 00 pl pl
//	vsr8aw	vr02, vr01, 2	;	// vr02: 00 00 00 00 00 pl pl pl

	// copy the pels to vr01
	vmvw	vr01, vr03,	1	;	// vr01: 00 00 00 00 pl xx xx pi
	vmvw	vr01, vr04,	2	;	// vr01: 00 00 00 00 pl xx pj pi
	vmvw	vr01, vr05,	4	;	// vr01: 00 00 00 00 pl pk pj pi

	vmr4w	vr06, vr02, vr10 ;	// vr06: 00 00 00 pl 00 00 00 00
	vmr3aw	vr06, vr02, vr10 ;	// vr06: 00 00 pl pl 00 00 00 00
	vmr2aw	vr06, vr02, vr10 ;	// vr06: 00 pl pl pl 00 00 00 00
	vaddaw	vr01, vr01, vr10 ;	// vr01: 00 pl pl pl pl pk pj pi
	
	// load the coefficients rows1&2- SDM stride = 16*2 = 32
	vld64	vr11, [i2, 0]	;	// vr11: 000 000 000 000 w3  w2  w1  w0
	vld64	vr12, [i2, 32]	;	// vr12: 000 000 000 000 w7  w6  w5  w4
	vld64	vr13, [i2, 64]	;	// vr13: 000 000 000 000 w11 w10 w9  w8
	vld64	vr14, [i2, 96]	;	// vr14: 000 000 000 000 w15 w14 w13 w12

	// shift vr01
	vsr8	vr02, vr01, 2	;	// vr02: 00 00 pl pl pl pl pk pj
	vsr8	vr03, vr01, 4	;	// vr03: 00 00 00 pl pl pl pl pk
	vsr8	vr04, vr01, 6	;	// vr04: 00 00 00 00 pl pl pl pl

	vaddw	vr05, vr01, vr02	;	// (i+j) (j+k) (k+l) (l+l)

	vaddw	vr06, vr02, vr03	;	// (j+k) (k+l) (l+l) (l+l)
	
	vaddw	vr07, vr03, vr04	;	// (k+l) (l+l) (l+l) (l+l)

	vaddw	vr08, vr01, vr03	;
	vbmulaw	vr08, vr02, 2		;	// (i+2j+k) (j+2k+l) (k+2l+l) (l+2l+l)

	// round the result
	vasrrw	vr05, vr05, 1	;	// (i+j)  (j+k)   (k+l)		L
	vasrrw	vr06, vr06, 1	;	// (j+k)  (k+l)		L		L
	vasrrw	vr07, vr07, 1	;	// (k+l)	L		L		L
	vasrrw	vr08, vr08, 2	;	// (i+2j+k) (j+2k+l) (k+2l+l) (l+2l+l)
							// vr04: L		L		L		L

	vmvw	vr06, vr05, 4	; 	// (j+k)  (k+l)	 "(k+l)"	L		: row2
	vmr7w	vr01, vr05, vr10 ;	//  000	  (i+j)   (j+k)   (k+l)   L	: row1

	vmr7w	vr02, vr08, vr10 ;	//	000 (i+2j+k) (j+2k+l) (k+2l+l) (l+2l+l)
	vmr6w	vr03, vr08, vr10 ;	//	000    000   (i+2j+k) (j+2k+l) (k+2l+l) (l+2l+l)
	vsr8	vr09, vr08, 2	; 	//(j+2k+l) (k+2l+l) (l+2l+l)
	// construct the masks

	//row1: (i+j) (i+2j+k) (j+k) (j+2k+l)
	vmvw	vr01, vr05, 1	;	// "(i+j)"  [i+j]     (j+k)   [k+l]
	vmvw	vr01, vr02, 2	;	//  (i+j) "(i+2j+k)"  (j+k)   [k+l]
	vmvw	vr01, vr03, 8   ;	//  (i+j)  (i+2j+k)   (j+k) "(j+2k+l)"

	//row2				//vr06:    (j+k)    (k+l)     (k+l) 	L		
	vmvw	vr06, vr08, 2	;  // (j+k)  "(j+2k+l)"  (k+l) 	L	
	vmvw	vr06, vr02, 8	;  // (j+k)   (j+2k+l)   (k+l)  "(k+2l+l)"

	//row3
	vmvw	vr07, vr09, 2	;

	//row4 == vr04

	// add the coeffs to the mask
	vaddw	vr11, vr11, vr01	;	//row1
	vaddw	vr12, vr12, vr06	;	//row2
	vaddw	vr13, vr13, vr07	;	//row3
	vaddw	vr14, vr14, vr04	;	//row4

	// clip & pack the output
	vasrpwb vr11, vr11, 0	;
	vasrpwb vr12, vr12, 0	;
	vasrpwb vr13, vr13, 0	;
	vasrpwb vr14, vr14, 0	;

	// store the result - sdm stride = 16
	vst32	vr11, [i3, 0]	; // out- row1
	vst32	vr12, [i3, 16]	; // out- row2
	vst32	vr13, [i3, 32]	; // out- row3
	vst32	vr14, [i3, 48]	; // out- row4

//-------------------------------------
   vendrec r0

.on	percent_reg
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordChromaInterPredMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the Chroma Inter prediction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLuma4x4IntraPredMacros()
{
    Record(AM_Intra_Luma4x4VerPred, Macro_LumaIntra4x4VerPred(NextAddr));
    Record(AM_Intra_Luma4x4HorPred, Macro_LumaIntra4x4HorPred(NextAddr));
    Record(AM_Intra_Luma4x4DCVer,   Macro_LumaIntra4x4DCVer(NextAddr));

    Record(AM_Intra_Luma4x4DCHor,   Macro_LumaIntra4x4DCHor(NextAddr));
    Record(AM_Intra_Luma4x4DCVerAndHor, Macro_LumaIntra4x4DCVerAndHor(NextAddr));
    Record(AM_Intra_Luma4x4DCConst, Macro_LumaIntra4x4DCConst(NextAddr));

    Record(AM_Intra_Luma4x4DiagDownLeftWOExtrp, Macro_LumaIntra4x4DiagDownLeftWOExtrp(NextAddr));
    Record(AM_Intra_Luma4x4DiagDownLeftWithExtrp, Macro_LumaIntra4x4DiagDownLeftWithExtrp(NextAddr));
    Record(AM_Intra_Luma4x4VerLeftWOExtrp,      Macro_LumaIntra4x4VerLeftWOExtrp(NextAddr));

    Record(AM_Intra_Luma4x4VerLeftWithExtrp,Macro_LumaIntra4x4VerLeftWithExtrp(NextAddr));
    Record(AM_Intra_Luma4x4DiagDownRight,   Macro_LumaIntra4x4DiagDownRight(NextAddr));
    Record(AM_Intra_Luma4x4VerRight,        Macro_LumaIntra4x4VerRight(NextAddr));

    Record(AM_Intra_Luma4x4HorDown,     Macro_LumaIntra4x4HorDown(NextAddr));
    Record(AM_Intra_Luma4x4HorUp,        Macro_LumaIntra4x4HorUp(NextAddr));
    
// Init pre-lookedup table of 4x4 macro address from id
    for(int macro4x4 = 0; macro4x4 < 72; macro4x4++)
    	Intra4x4MacroAddrTable[macro4x4] = GetMacroAddr(Intra4x4MacroTable[macro4x4]);
}





