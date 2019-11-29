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
/* FILE NAME:       AuroraH264ChromaIntraPred.cpp                           */
/* DATE:															        */
/* DESCRIPTION:		SIMD code Chroma inter prediction.                      */
/*																			*/
/* ------------------------------------------------------------------------ */
#ifdef _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"


_Asm void ChromaDCConstMask( )
{
.off	percent_reg

	// clear vr01 & vr02
	vsubw	vr01, vr01, vr01	;
	vsubw	vr02, vr02, vr02	;

	// load 128 in vr01, vr02
	vbaddw	vr01, vr01, 128		;
	vbaddw	vr02, vr02, 128		;

.on	percent_reg
}

_Asm void ChromaDCVerMask( unsigned int sdm_top_line_add )
{
	% reg sdm_top_line_add

.off	percent_reg
    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add


.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
_Asm int Macro_ChromaDCVerMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load the top line
	vld64w	vr01, [i1, 0]		;	// vr01: p07 p06 p05 p04 p03 p02 p01 p00

	vor		vr01, vr01, vr01	; // move vr01 to accumulator
	vsr8aw	vr03, vr01, 2	;	// vr03: xxx xxx xxx (4+5)    xxx xxx xxx (0+1)
	vsr8aw	vr03, vr01, 4	;	// vr03: xxx xxx xxx (4+5+6)  xxx xxx xxx (0+1+2)
	vsr8aw	vr03, vr01, 6	;	// vr03: xxx xxx xxx (4+5+6+7)xxx xxx xxx (0+1+2+3)

	// clear a dummy register
	vsubw	vr10, vr10, vr10	; //
	vmvzw	vr02, vr03, 0x11	; // vr02: 00 00 00 S1 00 00 00 S0

	// round the result
	vasrrw	vr02, vr02, 2	;

	//create mask
	vmr7aw	vr01, vr02, vr10	; // vr01: 00 00 S1 S1 00 00 S0 S0
	vmr6aw	vr01, vr02, vr10	; // vr01: 00 S1 S1 S1 00 S0 S0 S0
	vmr5aw	vr01, vr02, vr10	; // vr01: S1 S1 S1 S1 S0 S0 S0 S0 = acc

	vaddaw	vr02, vr10, vr10	; // vr02: S1 S1 S1 S1 S0 S0 S0 S0 = acc


//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif

_Asm void ChromaDCHorMask( unsigned int sdm_left_line_add )
{
	% reg sdm_left_line_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_left_line_add, 2	;	// load i1 with sdm_left_line_add

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
_Asm int Macro_ChromaDCHorMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


		
	// load the left pels - sdm stride=8
	vld32wl	vr01,	[i1, 0]		;
	vld32wl	vr02,	[i1, 8]		;
	vld32wl	vr03,	[i1, 16]	;
	vld32wl	vr04,	[i1, 24]	;

	vld32wl	vr05,	[i1, 32]	;
	vld32wl	vr06,	[i1, 40]	;
	vld32wl	vr07,	[i1, 48]	;
	vld32wl	vr08,	[i1, 56]	;

	// sum the top4 and bottom4 rows
	vaddw	vr09, vr01, vr02	;
	vaddaw	vr09, vr03, vr04	;

	vaddw	vr10, vr05, vr06	;
	vaddaw	vr10, vr07, vr08	;

	vmvzw	vr11, vr09, 8	;
	vmvzw	vr12, vr10, 8	;

	vasrrw	vr11, vr11, 2	;
	vasrrw	vr12, vr12, 2	;

	vsubw	vr20, vr20, vr20	;

	vor		vr01, vr11, vr11	;
	vsr8aw	vr01, vr11,	2		;
	vsr8aw	vr01, vr11, 4		;
	vsr8aw	vr01, vr11, 6		;

	vor		vr02, vr12, vr12	;
	vsr8aw	vr02, vr12, 2		;
	vsr8aw	vr02, vr12, 4		;
	vsr8aw	vr02, vr12, 6		;

	vmr4w	vr03, vr01, vr20	;
	vaddaw	vr01, vr01, vr20	;

	vmr4w	vr04, vr02, vr20	;
	vaddaw	vr02, vr02, vr20	;


    vendrec r0

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm void ChromaDCVerAndHorMask( unsigned int sdm_top_line_add,
							unsigned int sdm_left_line_add )
{
	% reg sdm_top_line_add, sdm_left_line_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, sdm_left_line_add, 4	;	// load i2 with sdm_top_line_add

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
_Asm int Macro_ChromaDCVerAndHorMask(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------




	// load the top line
	vld64w	vr01, [i1, 0]		;	// vr01: p07 p06 p05 p04 p03 p02 p01 p00
		
	// load the left pels - sdm stride=8
	vld32wl	vr04,	[i2, 0]		;
	vld32wl	vr05,	[i2, 8]		;
	vld32wl	vr06,	[i2, 16]	;
	vld32wl	vr07,	[i2, 24]	;

	vld32wl	vr08,	[i2, 32]	;
	vld32wl	vr09,	[i2, 40]	;
	vld32wl	vr10,	[i2, 48]	;
	vld32wl	vr11,	[i2, 56]	;

	// clear a dummy register
	vsubw	vr20, vr20, vr20 ; // vr20: all zeroes
	//
	vor		vr03, vr01, vr01	;	// move vr01 to accumulator
	vmr7aw	vr03, vr01, vr20	;	// vr03: 
	vmr6aw	vr03, vr01, vr20	;	// vr03: 
	vmr5aw	vr03, vr01, vr20	;	// vr03: (7+6+5+4) xx xx xx (3+2+1+0) xx xx xx xx
                                    
									// vr03: S1 xx xx xx S0 xx xx xx
	// compute S2 and S3
	vaddw	vr12, vr04, vr05	;	
	vaddaw	vr12, vr06, vr07	;	// vr12: xx xx xx xx S2 xx xx xx

	vaddw	vr13, vr08, vr09	;
	vaddaw	vr13, vr10, vr11	;	// vr13: xx xx xx xx S3 xx xx xx

	// add S0+S2
	vaddw	vr12, vr12, vr03 ; // vr12: xx xx xx xx (S0+S2) xx xx xx

	// add S1 and S3
	vmr4w	vr14, vr13, vr20 ; // vr14: S3 xx xx xx 00 00 00 00
	vaddaw	vr14, vr03, vr20 ; // vr14: (S3+S1) xx xx xx xx xx xx xx

	// round the results
	vasrrw	vr03, vr03, 2	; // vr03:  S1  xxxx xxxx xxxx xxxx xxxx xxxx xxxx
	vasrrw	vr13, vr13, 2	; // vr13: xxxx xxxx xxxx xxxx  S3  xxxx xxxx xxxx
	vasrrw	vr12, vr12, 3	; // vr12: xxxx xxxx xxxx xxxx S0+S2 xxx xxxx xxxx
	vasrrw	vr14, vr14, 3	; // vr14: S3+S1 xxx xxxx xxxx xxxx xxxx xxxx xxxx

	// move the result to the correst places
	vmvzw	vr04, vr12, 8	;
	vmvw	vr04, vr03, 128 ; // vr04: S1 00 00 00  S0+S2 00 00 00

	vmvzw	vr05, vr13, 8	; // vr05: 00 00 00 00   S3   00 00 00
	vmvw	vr05, vr14, 128 ; // vr05: S1+s3 00000   S3   00 00 00

	//create mask
	vor		vr01, vr04, vr04 ;
	vsr8aw	vr01, vr04, 2	;
	vsr8aw	vr01, vr04, 4	;
	vsr8aw	vr01, vr04, 6	;

	vor		vr02, vr05, vr05 ;
	vsr8aw	vr02, vr05, 2	;
	vsr8aw	vr02, vr05, 4	;
	vsr8aw	vr02, vr05, 6	;

    vendrec r0

.on	percent_reg

}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm void ChromaIntraDCPred(	unsigned int coef_buf_add,
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
#pragma Code("codesec3")
#endif
_Asm int Macro_ChromaIntraDCPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load coefs -- sdm stride 16 - row 1 to 4
	vld128	vr11, [i2, 0]	;	// row1
	vld128	vr12, [i2, 16]	;	// row2
	vld128	vr13, [i2, 32]	;	// row3
	vld128	vr14, [i2, 48]	;	// row4

	// add pred values
	vaddw	vr11, vr11, vr01	;
	vaddw	vr12, vr12, vr01	;
	vaddw	vr13, vr13, vr01	;
	vaddw	vr14, vr14, vr01	;

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

	// add pred values
	vaddw	vr15, vr15, vr02	; 
	vaddw	vr16, vr16, vr02	;
	vaddw	vr17, vr17, vr02	; 
	vaddw	vr18, vr18, vr02	;

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

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm void Chroma8x4IntraHorPred( unsigned int  sdm_left_line_add,
										unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg sdm_left_line_add, coef_buf_add, out_mcblk_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, sdm_left_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, coef_buf_add,		4	;	// load i2 with coef_buf_add
    vmovw	vr00, out_mcblk_add,	8	;	// load i3 with out_mcblk_add

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
_Asm int Macro_Chroma8x4IntraHorPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// get the left line
	vld32wl	vr01, [i1, 0]	;
	vld32wl	vr02, [i1, 8]	;
	vld32wl	vr03, [i1, 16]	;
	vld32wl	vr04, [i1, 24]	;

	// load coefs -- sdm stride 16 - row 1 to 4
	vld128	vr11, [i2, 0]	;	// row1
	vld128	vr12, [i2, 16]	;	// row2
	vld128	vr13, [i2, 32]	;	// row3
	vld128	vr14, [i2, 48]	;	// row4

	vmvzw	vr05, vr01, 8	;
	vmvzw	vr06, vr02, 8	;
	vmvzw	vr07, vr03, 8	;
	vmvzw	vr08, vr04, 8	;

	vor		vr01, vr05, vr05 ;
	vsr8aw	vr01, vr05, 2	;
	vsr8aw	vr01, vr05, 4	;
	vsr8aw	vr01, vr05, 6	;

	vor		vr02, vr06, vr06 ;
	vsr8aw	vr02, vr06, 2	;
	vsr8aw	vr02, vr06, 4	;
	vsr8aw	vr02, vr06, 6	;

	vor		vr03, vr07, vr07 ;
	vsr8aw	vr03, vr07, 2	;
	vsr8aw	vr03, vr07, 4	;
	vsr8aw	vr03, vr07, 6	;

	vor		vr04, vr08, vr08 ;
	vsr8aw	vr04, vr08, 2	;
	vsr8aw	vr04, vr08, 4	;
	vsr8aw	vr04, vr08, 6	;

	vsubw	vr20, vr20, vr20 ;

	vor		vr05, vr01, vr01 ;
	vmr4aw	vr05, vr01, vr20 ;

	vor		vr06, vr02, vr02 ;
	vmr4aw	vr06, vr02, vr20 ;

	vor		vr07, vr03, vr03 ;
	vmr4aw	vr07, vr03, vr20 ;

	vor		vr08, vr04, vr04 ;
	vmr4aw	vr08, vr04, vr20 ;

	// add pred values
	vaddw	vr11, vr11, vr05	;
	vaddw	vr12, vr12, vr06	;
	vaddw	vr13, vr13, vr07	;
	vaddw	vr14, vr14, vr08	;

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

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm void ChromaIntraVerPredMask( unsigned int  sdm_top_line_add )
{
	% reg sdm_top_line_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add

	;//load top line
	vld64w	vr01, [i1, 0]	;	// vr01: p07 p06 p05 p04 p03 p02 p01 p00
	vld64w	vr02, [i1, 0]	;	// vr01: p07 p06 p05 p04 p03 p02 p01 p00
.on	percent_reg
}

_Asm void Chroma8x8PlanarPred(  unsigned int sdm_top_line_add,
										unsigned int sdm_left_line_add ,
										unsigned int sdm_intra_plprd_coeffs,
										unsigned int coef_buf_add,
										unsigned int out_mcblk_add )
{
	% reg sdm_top_line_add, sdm_left_line_add, sdm_intra_plprd_coeffs, coef_buf_add, out_mcblk_add

.off	percent_reg

    // load inp pointer
    vmovw	vr00, sdm_top_line_add, 2	;	// load i1 with sdm_top_line_add
    vmovw	vr00, sdm_left_line_add, 4	;	// load i2 with sdm_top_line_add
	vmovw	vr00, sdm_intra_plprd_coeffs, 8 ; // i3: multiplication factors
	vmovw	vr00, coef_buf_add,		16	;	// i4: coefficients
	vmovw	vr00, out_mcblk_add,	32	;	// i5: out-pels

.on	percent_reg
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
_Asm int Macro_Chroma8x8PlanarPred(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// calculate H
	vld32wl	vr01, [i1, 0]	;	// vr01: xxx xxx xxx xxx p-1 xxx xxx xxx // save vr01!!
	vld32wl	vr02, [i1, 4] ;	// vr02: xxx xxx xxx xxx  p3  p2  p1  p0
	vld32wl	vr03, [i1, 8] ;  // vr03: xxx xxx xxx xxx  p7  p6  p5  p4 // save vr03!!
	vld128	vr08, [i3, 32];	// vr08:   4   3   2   1   0  -1  -2  -3
	vld128  vr09, [i3, 0] ;  // vr09:   8   7   6   5   4   3   2   1

	// invert vr02
	vsr8	vr04, vr02, 4 ; //vr04: xxx xxx xxx xxx xxx xxx  p3  p2

	// clear a dummy reg
	vsubw	vr10, vr10, vr10 ;	//vr10 is all zeroes

	// exchange vr02 and vr04
	vexch2	vr04, vr02	;	// vr04: xxx xxx xxx xxx  p1  p0  p3  p2
	vsr8	vr05, vr04, 2 ;// vr05: xxx xxx xxx xxx xxx  p1  p0  p3

	vexch1	vr05, vr04	;	// vr05: xxx xxx xxx xxx  p0  p1  p2  p3

	vsr8	vr05, vr05, 2 ; //vr05: xxx xxx xxx xxx xxx  p0  p1  p2

	vmvw	vr05, vr01, 8 ; //vr05: xxx xxx xxx xxx p-1  p0  p1  p2

	vsubw	vr02, vr03, vr05 ;

	vmulw	vr02, vr02, vr09 ;

	vsr8aw	vr04, vr02, 2	;
	vsr8aw	vr04, vr02, 4	;
	vsr8aw	vr04, vr02, 6	;

	// calculate b
	vasrw	vr05, vr04, 4	;
	vaddaw	vr06, vr04, vr10 ;

	vasrrw	vr07, vr06, 1	; // vr07: xx xx xx xx xx xx xx 'b'

	// load the left pels
	vld32wl	vr11, [i2, 0]	;
	vld32wl	vr12, [i2, 8]	;
	vld32wl	vr13, [i2, 16] ;
	//vld32wl	vr14, i2, 24 ;
	vld32wl	vr15, [i2, 32] ;
	vld32wl	vr16, [i2, 40] ;
	vld32wl	vr17, [i2, 48] ;
	vld32wl	vr18, [i2, 56] ;	
	
	// calculate V
	vsubw	vr19, vr15, vr13	;
	vsubw	vr20, vr16, vr12	;
	vsubw	vr21, vr17, vr11	;
	vsubw	vr22, vr18, vr01	;
	
	vbmulw	vr19, vr19, 1	;
	vbmulaw	vr19, vr20, 2	;
	vbmulaw	vr19, vr21, 3	;
	vbmulaw	vr19, vr22, 4	;

	vasrw	vr20, vr19, 4	;
	vaddaw	vr21, vr19, vr10 ;

	vasrrw	vr22, vr21, 1	; //vr22: xx xx xx xx 'c' xx xx xx

	//calculate a
	vaddw	vr03, vr03, vr18 ;
	vbmulw	vr03, vr03, 16;   //vr03: xx xx xx xx 'a' xx xx xx

	vmvzw	vr02, vr07, 1 ;   //vr07: 00 00 00 00  00 00 00 'b'
	vmvzw	vr01, vr03, 8 ;   //vr01: 00 00 00 00 'a' 00 00 00
	vmvzw	vr03, vr22, 8 ;   //vr03: 00 00 00 00 'c' 00 00 00

	vsr8aw	vr06, vr03, 2 ;   //vr06:  00 00 00 00  c  c 00 00
	vsr8aw	vr06, vr03, 4 ;   //vr06:  00 00 00 00  c  c  c 00
	vsr8aw	vr06, vr03, 6 ;   //vr06:  00 00 00 00  c  c  c  c

	vor		vr04, vr01, vr01 ;	//vr04: 00 00 00 00  a 00 00 00
	vsr8aw	vr04, vr01,	 2	 ; //vr04: 00 00 00 00  a  a 00 00
	vsr8aw	vr04, vr01,	 4	 ; //vr04: 00 00 00 00  a  a  a 00
	vsr8aw	vr04, vr01,	 6	 ; //vr04: 00 00 00 00  a  a  a  a

	vor		vr05, vr02, vr02 ; //vr05: 00 00 00 00  00 00 00 b
	vmr7aw	vr05, vr02, vr10 ; //vr05: 00 00 00 00  00 00  b b
	vmr6aw	vr05, vr02, vr10 ; //vr05: 00 00 00 00  00  b  b b
	vmr5aw	vr05, vr02, vr10 ; //vr05: 00 00 00 00   b  b  b b

	vor		vr03, vr06, vr06 ;
	vmr4aw	vr03, vr06, vr10 ; //vr03:  c  c  c  c  c  c  c  c

	vor		vr02, vr05, vr05 ;
	vmr4aw	vr02, vr05, vr10 ; //vr02:  b  b  b  b  b  b  b  b

	vor		vr01, vr04, vr04 ;
	vmr4aw	vr01, vr04, vr10 ;//vr01:  a  a  a  a  a  a  a  a

	vbmulaw	vr04, vr03, (-3) ;//vr04==temp
	vmulaw	vr05, vr02, vr08 ;//vr05==temp2

	// load coefs -- sdm stride 16 - row 1 to 4
	vld128	vr11, [i4, 0]		;	// row1
	vld128	vr12, [i4, 16]	;	// row2
	vld128	vr13, [i4, 32]	;	// row3
	vld128	vr14, [i4, 48]	;	// row4

	// calculate temp2 for rows 2,3,4
	vaddw	vr06, vr03,	vr05; // temp2 += c - row2
	vaddaw	vr07, vr03,	vr10; // temp2 += c - row3
	vaddaw	vr08, vr03,	vr10; // temp2 += c - row4
	vaddaw	vr09, vr03, vr10; // temp2 += c - row5

	// clip/pack vr05, vr06, vr07, vr08
	vasrrpwb	vr05, vr05, 5	;
	vasrrpwb	vr06, vr06, 5	;
	vasrrpwb	vr07, vr07, 5	;
	vasrrpwb	vr08, vr08, 5	;

	//unpack
	vupbw	vr05, vr05	; // row1
	vupbw	vr06, vr06	; // row1
	vupbw	vr07, vr07	; // row1
	vupbw	vr08, vr08	; // row1

	//add coeffs
	vaddw	vr11, vr11, vr05	;
	vaddw	vr12, vr12, vr06	;
	vaddw	vr13, vr13, vr07	;
	vaddw	vr14, vr14, vr08	;

	// clip and pack
	vasrpwb	vr11, vr11, 0	;	//row2
	vasrpwb	vr12, vr12, 0	;
	vasrpwb	vr13, vr13, 0	;	//row2
	vasrpwb	vr14, vr14, 0	;

	// store the result
	vst64	vr11, [i5,  0]	; // out- row1a
	vst64	vr12, [i5,  8]	; // out- row1b
	vst64	vr13, [i5,  16]	; // out- row2a
	vst64	vr14, [i5,  24] ; // out- row2b

	// load coefs -- sdm stride 16 - row 5 to 8
	vld128	vr15, [i4, 64]	;	// row5
	vld128	vr16, [i4, 80]	;	// row6
	vld128	vr17, [i4, 96]	;	// row7
	vld128	vr18, [i4, 112]	;	// row8

	//calc temp2 for rows 6,7,8
	vaddw	vr06, vr03, vr09;
	vaddaw	vr07, vr03, vr10;
	vaddaw	vr08, vr03, vr10;

	// clip/pack vr09, vr06, vr07, vr08
	vasrrpwb	vr09, vr09, 5	; // row5
	vasrrpwb	vr06, vr06, 5	; // row6
	vasrrpwb	vr07, vr07, 5	; // row7
	vasrrpwb	vr08, vr08, 5	; // row8

	//unpack
	vupbw	vr09, vr09	; // row5
	vupbw	vr06, vr06	; // row6
	vupbw	vr07, vr07	; // row7
	vupbw	vr08, vr08	; // row8

	//add coeffs
	vaddw	vr15, vr15, vr09	;
	vaddw	vr16, vr16, vr06	;
	vaddw	vr17, vr17, vr07	;
	vaddw	vr18, vr18, vr08	;

	// clip and pack
	vasrpwb	vr15, vr15, 0	;	//row5
	vasrpwb	vr16, vr16, 0	;  //row6 
	vasrpwb	vr17, vr17, 0	;	//row7
	vasrpwb	vr18, vr18, 0	;	//row8

	// store the result
	vst64	vr15, [i5, 32]	; // out- row5
	vst64	vr16, [i5, 40]	; // out- row6
	vst64	vr17, [i5, 48]	; // out- row7
	vst64	vr18, [i5, 56]	; // out- row8


//-------------------------------------

    vendrec r0

.on	percent_reg
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordChromaIntraPredMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the luma reconstruction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordChromaIntraPredMacros()
{
    Record(AM_Intra_ChromaDCVerMask,        Macro_ChromaDCVerMask(NextAddr));
    Record(AM_Intra_ChromaDCHorMask,        Macro_ChromaDCHorMask(NextAddr));
    Record(AM_Intra_ChromaDCVerAndHorMask,  Macro_ChromaDCVerAndHorMask(NextAddr));
    Record(AM_Intra_ChromaDCPred,           Macro_ChromaIntraDCPred(NextAddr));
    Record(AM_Intra_Chroma8x4HorPred,       Macro_Chroma8x4IntraHorPred(NextAddr));
    Record(AM_Intra_Chroma8x8PlanarPred,    Macro_Chroma8x8PlanarPred(NextAddr));

}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
void AuroraH264Dec::ChromaIntraPred(  SDM_Offset sdm_top_line_add,
							SDM_Offset sdm_left_line_add,
							SDM_Offset coef_buf_add,
							SDM_Offset out_mcblk_add,
							unsigned int pred_mode,
							unsigned int sub_mode)
{

	switch(pred_mode)
	{
	case 0:
		// dc prediction

		// create DC mask
		switch(sub_mode)
		{
		case 3:
			// both above and left
			// U
			ChromaDCVerAndHorMask( sdm_top_line_add +4 ,
							sdm_left_line_add );
            RecMgr.Run(AM_Intra_ChromaDCVerAndHorMask);
            
			ChromaIntraDCPred(	coef_buf_add, out_mcblk_add );
            RecMgr.Run(AM_Intra_ChromaDCPred);
			// V
			ChromaDCVerAndHorMask( sdm_top_line_add +4 +16,
							sdm_left_line_add +64);
            RecMgr.Run(AM_Intra_ChromaDCVerAndHorMask);
            
			ChromaIntraDCPred(	coef_buf_add+128, out_mcblk_add+64 );
            RecMgr.Run(AM_Intra_ChromaDCPred);
            
            
			break;

		case 2:
			// above only
			// U
			ChromaDCVerMask( sdm_top_line_add + 4 );
            RecMgr.Run(AM_Intra_ChromaDCVerMask);    
            
			ChromaIntraDCPred(	coef_buf_add, out_mcblk_add );
            RecMgr.Run(AM_Intra_ChromaDCPred);
			// V
			ChromaDCVerMask( sdm_top_line_add + 4 +16);
            RecMgr.Run(AM_Intra_ChromaDCVerMask);    
            
			ChromaIntraDCPred(	coef_buf_add+128, out_mcblk_add+64 );
            RecMgr.Run(AM_Intra_ChromaDCPred);
            
            
			break;

		case 1:
			// left only
			// U
			ChromaDCHorMask( sdm_left_line_add );
            RecMgr.Run(AM_Intra_ChromaDCHorMask);
            
			ChromaIntraDCPred(	coef_buf_add, out_mcblk_add );
            RecMgr.Run(AM_Intra_ChromaDCPred);
			// V
			ChromaDCHorMask( sdm_left_line_add +64);
            RecMgr.Run(AM_Intra_ChromaDCHorMask);
            
			ChromaIntraDCPred(	coef_buf_add+128, out_mcblk_add+64 );
            RecMgr.Run(AM_Intra_ChromaDCPred);
            
            
			break;

		case 0:
			// const dc
			// U
			ChromaDCConstMask();
            
			ChromaIntraDCPred(	coef_buf_add, out_mcblk_add );
            RecMgr.Run(AM_Intra_ChromaDCPred);
			// V
			ChromaDCConstMask();
            
			ChromaIntraDCPred(	coef_buf_add+128, out_mcblk_add+64 );
            RecMgr.Run(AM_Intra_ChromaDCPred);
            
            
			break;
		}
        
		// perform prediction
		break;
	
	case 1:
		// horizontal prediction
		// U
		Chroma8x4IntraHorPred( sdm_left_line_add, coef_buf_add, out_mcblk_add );

        RecMgr.Run(AM_Intra_Chroma8x4HorPred);

		Chroma8x4IntraHorPred( sdm_left_line_add + (4*8),
							coef_buf_add + (4*8*2), 
							out_mcblk_add + (4*8));

        RecMgr.Run(AM_Intra_Chroma8x4HorPred);
		// V
		Chroma8x4IntraHorPred( sdm_left_line_add+64, coef_buf_add+128, out_mcblk_add+64 );

        RecMgr.Run(AM_Intra_Chroma8x4HorPred);

		Chroma8x4IntraHorPred( sdm_left_line_add+64 + (4*8),
							coef_buf_add+128 + (4*8*2), 
							out_mcblk_add+64 + (4*8));

        RecMgr.Run(AM_Intra_Chroma8x4HorPred);


		break;

	case 2:
		// vertical prediction create the mask as in DC and 
		// use the same code for recon
		// U
		ChromaIntraVerPredMask( sdm_top_line_add +4 );

		ChromaIntraDCPred(	coef_buf_add, out_mcblk_add );
                
        RecMgr.Run(AM_Intra_ChromaDCPred);
		// V
		ChromaIntraVerPredMask( sdm_top_line_add +4 +16);

		ChromaIntraDCPred(	coef_buf_add+128, out_mcblk_add+64 );
                
        RecMgr.Run(AM_Intra_ChromaDCPred);


		break;

	case 3:
		// plane prediction
		
		// calculate masks
		// U
		Chroma8x8PlanarPred(  sdm_top_line_add, sdm_left_line_add,
						m_intra_plmode_coeffs, coef_buf_add, out_mcblk_add );

		// perform prediction
        RecMgr.Run(AM_Intra_Chroma8x8PlanarPred);
		// V
		Chroma8x8PlanarPred(  sdm_top_line_add+16, sdm_left_line_add+64,
						m_intra_plmode_coeffs, coef_buf_add+128, out_mcblk_add+64 );

		// perform prediction
        RecMgr.Run(AM_Intra_Chroma8x8PlanarPred);
		

		break;
		
	}
        
}


