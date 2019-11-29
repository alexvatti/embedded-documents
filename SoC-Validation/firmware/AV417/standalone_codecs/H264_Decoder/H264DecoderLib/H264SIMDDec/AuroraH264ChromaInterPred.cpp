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
/* FILE NAME:       AuroraH264ChromaInterPred.cpp                           */
/* DATE:															        */
/* DESCRIPTION:		SIMD code Chroma inter prediction.                      */
/*																			*/
/* ------------------------------------------------------------------------ */
#if defined _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"

//define CHROMA_BLK_SZ_XxY
#define CHROMA_BLK_SZ_2x2	(0x0202)
#define CHROMA_BLK_SZ_2x4	(0x0204)
#define CHROMA_BLK_SZ_4x2	(0x0402)
#define CHROMA_BLK_SZ_4x4	(0x0404)
#define CHROMA_BLK_SZ_4x8	(0x0408)
#define CHROMA_BLK_SZ_8x4	(0x0804)
#define CHROMA_BLK_SZ_8x8	(0x0808)


_Asm int Macro_FilterChromaBlkSz_Nx2(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr

	// handles blocks of size 2x2 and 4x2

	// clear a dummy reg since we have time
	vsubw	vr06, vr06, vr06	;	// clear vr06

	// load the inp data
	vld64w	vr01, [i1, 0]		;  //row1 000 000 000 w05 w04 w03 w02 w01
	vld64w  vr03, [i1, 16]		;	//row2
	vld64w  vr05, [i1, 32]		;	//row3

	// shift row1, row 2 & row3
	vmr1w	vr02, vr06, vr01	; //row1s 000 000 000 000 w05 w04 w03 w02
	vmr1w	vr04, vr06, vr03	; //row1s 000 000 000 000 w05 w04 w03 w02
	vmr1w	vr06, vr06, vr05	; //row1s 000 000 000 000 w05 w04 w03 w02

	// generate the first output
	// out1 = row1*r9 + row1s*r10 + row2*r11 + row2s*r12
	vmulw	vr07, vr01, vr19	;	// acc = vr07 = vr01*r9
	vmulaw	vr07, vr02, vr20	;	// acc = vr07 = vr02*r10
	vmulaw	vr07, vr03, vr21	;	// acc = vr07 = vr03*r11
	vmulaw	vr07, vr04, vr22	;	// acc = vr07 = vr04*r12

	// generate the second output
	vmulw	vr08, vr03, vr19	;
	vmulaw	vr08, vr04, vr20	;
	vmulaw	vr08, vr05, vr21	;
	vmulaw	vr08, vr06, vr22	;

	// round and pack the output
	vasrrpwb	vr07, vr07, 6	;	// row1: round and pack the output
	vasrrpwb	vr08, vr08, 6	;	// row2: round and pack the output

	// store the result
	vst32	vr07,  [i1, 0]	;	// row1: store the result				
	vst32	vr08,  [i1, 16]	;	// row2: store the result				

//-------------------------------------
    vendrec r0              ;
.on	percent_reg

}

_Asm int Macro_FilterChromaBlkSz_Nx4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr


	// clear a dummy reg since we have time
	vsubw	vr10, vr10, vr10	;	// clear vr06

	// load the inp data
	vld64w	vr01, [i1, 0]		;  //row1 000 000 000 w05 w04 w03 w02 w01
	vld64w  vr03, [i1, 16]		;	//row2
	vld64w  vr05, [i1, 32]		;	//row3
	vld64w  vr07, [i1, 48]		;	//row4
	vld64w  vr09, [i1, 64]		;	//row5

	// shift row1, row2, row3, row4 & row5
	vmr1w	vr02, vr10, vr01	; //row1s 000 000 000 000 w05 w04 w03 w02
	vmr1w	vr04, vr10, vr03	; //row2s
	vmr1w	vr06, vr10, vr05	; //row3s
	vmr1w	vr08, vr10, vr07	; //row4s
	vmr1w	vr10, vr10, vr09	; //row5s

	// generate the first output
	// out1 = row1*r9 + row1s*r10 + row2*r11 + row2s*r12
	vmulw	vr11, vr01, vr19	;	// acc = vr09 = vr01*r9
	vmulaw	vr11, vr02, vr20	;	// acc = vr09 = vr02*r10
	vmulaw	vr11, vr03, vr21	;	// acc = vr09 = vr03*r11
	vmulaw	vr11, vr04, vr22	;	// acc = vr09 = vr04*r12

	// generate the second output
	vmulw	vr12, vr03, vr19	;
	vmulaw	vr12, vr04, vr20	;
	vmulaw	vr12, vr05, vr21	;
	vmulaw	vr12, vr06, vr22	;

	vasrrpwb	vr11, vr11, 6	;	// row1: round and pack the output

	// generate the third output
	vmulw	vr13, vr05, vr19	;
	vmulaw	vr13, vr06, vr20	;
	vmulaw	vr13, vr07, vr21	;
	vmulaw	vr13, vr08, vr22	;

	vst32	    vr11,  [i1, 0]	;	// row1: store the result
	vasrrpwb	vr12, vr12, 6	;	// row2: round and pack the output

	// generate the fourth output
	vmulw	vr14, vr07, vr19	;
	vmulaw	vr14, vr08, vr20	;
	vmulaw	vr14, vr09, vr21	;
	vmulaw	vr14, vr10, vr22	;

	vst32	    vr12,  [i1, 16]	;	// row2: store the result
	vasrrpwb	vr13, vr13, 6	;	// row3: round and pack the output
	vasrrpwb	vr14, vr14, 6	;	// row4: round and pack the output

	vst32	vr13,  [i1, 32]	;	// row3: store the result
	vst32	vr14,  [i1, 48]	;	// row4: store the result

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0              ;
.on	percent_reg

}





_Asm int Macro_FilterChromaBlkSz_4x8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

	// load the inp data - 9 rows of width 4(+1)
	vld64w	vr01, [i1,  0]		;  //row1  w08 w07 w06 w05 w04 w03 w02 w01
	vld64w  vr03, [i1, 16]		;	//row2
	vld64w  vr05, [i1, 32]		;	//row3
	vld64w  vr07, [i1, 48]		;	//row4
	vld64w  vr09, [i1, 64]		;	//row5
	vld64w  vr11, [i1, 80]		;	//row6
	vld64w  vr13, [i1, 96]		;	//row7
	vld64w  vr15, [i1, 112]		;	//row8
	vld64w  vr17, [i1, 128]	    ;	//row9

	// shift the 9 rows
	vmr1w	vr02, vr18, vr01	;
	vmr1w	vr04, vr18, vr03	;
	vmr1w	vr06, vr18, vr05	;
	vmr1w	vr08, vr18, vr07	;
	vmr1w	vr10, vr18, vr09	;

	vmr1w	vr12, vr18, vr11	;
	vmr1w	vr14, vr18, vr13	;
	vmr1w	vr16, vr18, vr15	;
	vmr1w	vr18, vr18, vr17	;

	// generate the first output
	// out1 = row1*r9 + row1s*r10 + row2*r11 + row2s*r12
	vmulw	vr01, vr01, vr19	;	// acc = vr09 = vr01*r9
	vmulaw	vr01, vr02, vr20	;	// acc = vr09 = vr02*r10
	vmulaw	vr01, vr03, vr21	;	// acc = vr09 = vr03*r11
	vmulaw	vr01, vr04, vr22	;	// acc = vr09 = vr04*r12

	// generate the second output
	vmulw	vr03, vr03, vr19	;
	vmulaw	vr03, vr04, vr20	;
	vmulaw	vr03, vr05, vr21	;
	vmulaw	vr03, vr06, vr22	;

	vasrrpwb	vr01, vr01, 6	;	// row1: round and pack the output


	// generate the third output
	vmulw	vr05, vr05, vr19	;
	vmulaw	vr05, vr06, vr20	;
	vmulaw	vr05, vr07, vr21	;
	vmulaw	vr05, vr08, vr22	;

	vst32	    vr01,  [i1, 0]	;	// row1: store the result
	vasrrpwb	vr03, vr03, 6	;	// row2: round and pack the output

	// generate the fourth output
	vmulw	vr07, vr07, vr19	;
	vmulaw	vr07, vr08, vr20	;
	vmulaw	vr07, vr09, vr21	;
	vmulaw	vr07, vr10, vr22	;

	vst32	    vr03,  [i1, 16]	;	// row2: store the result
	vasrrpwb	vr05, vr05, 6	;	// row3: round and pack the output

	// generate the fifth output
	vmulw	vr09, vr09, vr19		;
	vmulaw	vr09, vr10, vr20		;
	vmulaw	vr09, vr11, vr21		;
	vmulaw	vr09, vr12, vr22		;

	vst32	    vr05,  [i1, 32]	;	// row3: store the result
	vasrrpwb	vr07, vr07, 6	;	// row4: round and pack the output

	// generate the sixth output
	vmulw	vr11, vr11, vr19		;
	vmulaw	vr11, vr12, vr20		;
	vmulaw	vr11, vr13, vr21		;
	vmulaw	vr11, vr14, vr22		;

	vst32	    vr07,  [i1, 48]	;	// row4: store the result
	vasrrpwb	vr09, vr09, 6	;	// row5: round and pack the output

	// generate the seventh output
	vmulw	vr13, vr13, vr19		;
	vmulaw	vr13, vr14, vr20		;
	vmulaw	vr13, vr15, vr21		;
	vmulaw	vr13, vr16, vr22		;

	vst32	    vr09,  [i1, 64]	;	// row5: store the result
	vasrrpwb	vr11, vr11, 6	;	// row6: round and pack the output

	// generate the eigth output
	vmulw	vr15, vr15, vr19		;
	vmulaw	vr15, vr16, vr20		;
	vmulaw	vr15, vr17, vr21		;
	vmulaw	vr15, vr18, vr22		;


	vst32	    vr11,  [i1, 80]	;	// row6: store the result
	vasrrpwb	vr13, vr13, 6	;	// row7: round and pack the output

	vasrrpwb	vr15, vr15, 6	;	// row8: round and pack the output
	vst32	    vr13,  [i1, 96]	;	// row7: store the result
	
	vst32	    vr15,  [i1, 112];	// row8: store the result

    vendrec r0              ;
.on	percent_reg

}


_Asm void FilterChromaBlkSz_8x4( unsigned int sdm_buf_add )
{
	// handles block size 8x4 - hor_size = 8 ver_size =4
	// should be called twice to handle block size 8x8
	% reg sdm_buf_add
.off	percent_reg

	// load inp / output pointer
    vmovw	vr00, sdm_buf_add, 2			;	// load i1 with r1
.on	percent_reg
}

_Asm int Macro_FilterChromaBlkSz_8x4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr


	// load the inp data - 5rows of length 9 each
	vld64w	vr01, [i1,  0]		;  //row1  w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1,  8]		;  //row1a 000 000 000 000 000 000 000 w09

	vld64w  vr03, [i1, 16]		;	//row2
	vld64w  vr04, [i1, 24]		;	//row2a

	vld64w  vr05, [i1, 32]		;	//row3
	vld64w  vr06, [i1, 40]		;	//row2a

	vld64w  vr07, [i1, 48]		;	//row4
	vld64w  vr08, [i1, 56]		;	//row4a

	vld64w  vr09, [i1, 64]		;	//row5
	vld64w  vr10, [i1, 72]		;	//row5a

	// shift the 5 rows
	vmr1w	vr02, vr02, vr01	;
	vmr1w	vr04, vr04, vr03	;
	vmr1w	vr06, vr06, vr05	;
	vmr1w	vr08, vr08, vr07	;
	vmr1w	vr10, vr10, vr09	;

	// generate the first output
	// out1 = row1*r9 + row1s*r10 + row2*r11 + row2s*r12
	vmulw	vr01, vr01, vr19		;	// acc = vr09 = vr01*r9
	vmulaw	vr01, vr02, vr20		;	// acc = vr09 = vr02*r10
	vmulaw	vr01, vr03, vr21		;	// acc = vr09 = vr03*r11
	vmulaw	vr01, vr04, vr22		;	// acc = vr09 = vr04*r12

	// generate the second output
	vmulw	vr03, vr03, vr19		;
	vmulaw	vr03, vr04, vr20		;
	vmulaw	vr03, vr05, vr21		;
	vmulaw	vr03, vr06, vr22		;

	vasrrpwb	vr01, vr01, 6	;	// row1: round and pack the output

	// generate the third output
	vmulw	vr05, vr05, vr19		;
	vmulaw	vr05, vr06, vr20		;
	vmulaw	vr05, vr07, vr21		;
	vmulaw	vr05, vr08, vr22		;

	vst64	    vr01,  [i1, 0]	;	// row1: store the result
	vasrrpwb	vr03, vr03, 6	;	// row2: round and pack the output
	
	// generate the fourth output
	vmulw	vr07, vr07, vr19		;
	vmulaw	vr07, vr08, vr20		;
	vmulaw	vr07, vr09, vr21		;
	vmulaw	vr07, vr10, vr22		;

	vst64	    vr03,  [i1, 16]	;	// row2: store the result
	vasrrpwb	vr05, vr05, 6	;	// row3: round and pack the output
	
	vasrrpwb	vr07, vr07, 6	;	// row4: round and pack the output
	vst64	    vr05,  [i1, 32]	;	// row3: store the result
	
	vst64	    vr07,  [i1, 48]	;	// row4: store the result

//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0              ;
.on	percent_reg

}

_Asm void FilterChromaBlkSz_8x8( unsigned int sdm_buf_add )
{
	// handles block size 8x4 - hor_size = 8 ver_size =4
	// should be called twice to handle block size 8x8
	% reg sdm_buf_add
.off	percent_reg

	// load inp / output pointer
    vmovw	vr00, sdm_buf_add, 2			;	// load i1 with r1
.on	percent_reg
}

_Asm int Macro_FilterChromaBlkSz_8x8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load the inp data - 5rows of length 9 each
	vld64w	vr01, [i1,  0]		;  //row1  w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1,  8]		;  //row1a 000 000 000 000 000 000 000 w09

	vld64w  vr03, [i1, 16]		;	//row2
	vld64w  vr04, [i1, 24]		;	//row2a

	vld64w  vr05, [i1, 32]		;	//row3
	vld64w  vr06, [i1, 40]		;	//row2a

	vld64w  vr07, [i1, 48]		;	//row4
	vld64w  vr08, [i1, 56]		;	//row4a

	vld64w  vr09, [i1, 64]		;	//row5
	vld64w  vr10, [i1, 72]		;	//row5a

	// shift the 5 rows
	vmr1w	vr02, vr02, vr01	;
	vmr1w	vr04, vr04, vr03	;
	vmr1w	vr06, vr06, vr05	;
	vmr1w	vr08, vr08, vr07	;
	vmr1w	vr10, vr10, vr09	;

	// generate the first output
	// out1 = row1*vr19 + row1s*vr20 + row2*vr21 + row2s*vr22
	vmulw	vr01, vr01, vr19		;	// acc = vr09 = vr01*r9
	vmulaw	vr01, vr02, vr20		;	// acc = vr09 = vr02*r10
	vmulaw	vr01, vr03, vr21		;	// acc = vr09 = vr03*r11
	vmulaw	vr01, vr04, vr22		;	// acc = vr09 = vr04*r12

	// generate the second output
	vmulw	vr03, vr03, vr19		;
	vmulaw	vr03, vr04, vr20		;
	vmulaw	vr03, vr05, vr21		;
	vmulaw	vr03, vr06, vr22		;

	// generate the third output
	vmulw	vr05, vr05, vr19		;
	vmulaw	vr05, vr06, vr20		;
	vmulaw	vr05, vr07, vr21		;
	vmulaw	vr05, vr08, vr22		;
	
	// generate the fourth output
	vmulw	vr07, vr07, vr19		;
	vmulaw	vr07, vr08, vr20		;
	vmulaw	vr07, vr09, vr21		;
	vmulaw	vr07, vr10, vr22		;

	vasrrpwb	vr01, vr01, 6	;	// row1: round and pack the output
	vasrrpwb	vr03, vr03, 6	;	// row2: round and pack the output
	vasrrpwb	vr05, vr05, 6	;	// row3: round and pack the output
	vasrrpwb	vr07, vr07, 6	;	// row4: round and pack the output

	vst64	    vr01,  [i1, 0]	;	// row1: store the result
	vst64	    vr03,  [i1, 16]	;	// row2: store the result
	vst64	    vr05,  [i1, 32]	;	// row3: store the result
	vst64	    vr07,  [i1, 48]	;	// row4: store the result



    // generate rows 5 to 8
   	vld64w	vr11, [i1, 80]		;  //row6  w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr12, [i1, 88]		;  //row6a 000 000 000 000 000 000 000 w09

	vld64w  vr13, [i1, 96]		;	//row7
	vld64w  vr14, [i1, 104]		;	//row7a

	vld64w  vr15, [i1, 112]		;	//row8
	vld64w  vr16, [i1, 120]		;	//row8a

	vld64w  vr17, [i1, 128]		;	//row9
	vld64w  vr18, [i1, 136]		;	//row9a

	// shift the 4 rows
	vmr1w	vr12, vr12, vr11	;
	vmr1w	vr14, vr14, vr13	;
	vmr1w	vr16, vr16, vr15	;
	vmr1w	vr18, vr18, vr17	;


    //generate the 5th output
	// out = row1*vr19 + row1s*vr20 + row2*vr21 + row2s*vr22
	vmulw	vr09, vr09, vr19		;	// acc = vr09 = vr01*r9
	vmulaw	vr09, vr10, vr20		;	// acc = vr09 = vr02*r10
	vmulaw	vr09, vr11, vr21		;	// acc = vr09 = vr03*r11
	vmulaw	vr09, vr12, vr22		;	// acc = vr09 = vr04*r12

    //generate the 6th output
	// out = row1*vr19 + row1s*vr20 + row2*vr21 + row2s*vr22
	vmulw	vr11, vr11, vr19		;	// acc = vr09 = vr01*r9
	vmulaw	vr11, vr12, vr20		;	// acc = vr09 = vr02*r10
	vmulaw	vr11, vr13, vr21		;	// acc = vr09 = vr03*r11
	vmulaw	vr11, vr14, vr22		;	// acc = vr09 = vr04*r12

    //generate the 7th output
	// out = row1*vr19 + row1s*vr20 + row2*vr21 + row2s*vr22
	vmulw	vr13, vr13, vr19		;
	vmulaw	vr13, vr14, vr20		;
	vmulaw	vr13, vr15, vr21		;
	vmulaw	vr13, vr16, vr22		;

    //generate the 8th output
	// out = row1*vr19 + row1s*vr20 + row2*vr21 + row2s*vr22
	vmulw	vr15, vr15, vr19		;
	vmulaw	vr15, vr16, vr20		;
	vmulaw	vr15, vr17, vr21		;
	vmulaw	vr15, vr18, vr22		;


	vasrrpwb	vr09, vr09, 6	;	// row5: round and pack the output
	vasrrpwb	vr11, vr11, 6	;	// row6: round and pack the output
	vasrrpwb	vr13, vr13, 6	;	// row7: round and pack the output
	vasrrpwb	vr15, vr15, 6	;	// row8: round and pack the output

	vst64	    vr09,  [i1, 64]	;	// row5: store the result
	vst64	    vr11,  [i1, 80]	;	// row6: store the result
	vst64	    vr13,  [i1, 96]	;	// row7: store the result
	vst64	    vr15,  [i1, 112];	// row8: store the result


//-------------------------------------
    vor     vr23, vr23, vr23 ;
    vendrec r0              ;
.on	percent_reg

}

_Asm void LoadChromaFilterConsts(       unsigned int w0,
									    unsigned int w1,
										unsigned int w2,
										unsigned int w3 )
{
	% reg w0, w1, w2, w3 
.off	percent_reg

	// load inp / output pointer
    vmovw  vr19, w0, 0xff  ;
    vmovw  vr20, w1, 0xff  ;
    vmovw  vr21, w2, 0xff  ;
    vmovw  vr22, w3, 0xff  ;
        
.on	percent_reg

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordChromaInterPredMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the Chroma Inter prediction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordChromaInterPredMacros()
{
    Record(AM_Inter_FilterChromaBlkSz_Nx2, Macro_FilterChromaBlkSz_Nx2(NextAddr));
    Record(AM_Inter_FilterChromaBlkSz_Nx4, Macro_FilterChromaBlkSz_Nx4(NextAddr));
    Record(AM_Inter_FilterChromaBlkSz_4x8, Macro_FilterChromaBlkSz_4x8(NextAddr));
    Record(AM_Inter_FilterChromaBlkSz_8x4, Macro_FilterChromaBlkSz_8x4(NextAddr));
    Record(AM_Inter_FilterChromaBlkSz_8x8, Macro_FilterChromaBlkSz_8x8(NextAddr));
}

void AuroraH264Dec::SIMDFilterChromaASM(        int fractional_hor_vector,
                                			    int fractional_ver_vector,
                                			    int block_hor_size,
                                			    int block_ver_size)
{
	int inv_fractional_hor_vector;
	int inv_fractional_ver_vector;
    int input_ver_size;
    int input_hor_size;
	unsigned int block_index;
	unsigned int w0, w1, w2, w3;

	// required for copying data
	if (0 != fractional_hor_vector)
	{
		input_hor_size = block_hor_size + 1;
	}
	else
	{
		input_hor_size = block_hor_size;
	}

	if (0 != fractional_ver_vector)
	{
		input_ver_size = block_ver_size + 1;
	}
	else
	{
		input_ver_size = block_ver_size;
	}


	inv_fractional_hor_vector = 8 - fractional_hor_vector;
	inv_fractional_ver_vector = 8 - fractional_ver_vector;

	w0 = inv_fractional_hor_vector	* inv_fractional_ver_vector;
	w1 = fractional_hor_vector		* inv_fractional_ver_vector;
	w2 = inv_fractional_hor_vector	* fractional_ver_vector;
	w3 = fractional_hor_vector		* fractional_ver_vector;
    
	block_index = (block_hor_size<<8) | (block_ver_size);

    //load the weights in the vectors register
    _vr00 = _vmovw( m_u_work_buf, 2);                   // load i1 with r1
    LoadChromaFilterConsts(w0, w1, w2, w3 );

	switch(block_index)
	{
	case CHROMA_BLK_SZ_2x2:
	case CHROMA_BLK_SZ_4x2:
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_Nx2);    
        _vr00 = _vmovw( m_v_work_buf, 2);                   // load i1 with r1
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_Nx2);    
		break;

	case CHROMA_BLK_SZ_2x4:
	case CHROMA_BLK_SZ_4x4:
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_Nx4);    
        _vr00 = _vmovw( m_v_work_buf, 2);                   // load i1 with r1
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_Nx4);    
    break;

	case CHROMA_BLK_SZ_4x8:
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_4x8);    
        _vr00 = _vmovw( m_v_work_buf, 2);                   // load i1 with r1
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_4x8);    
		break;

	case CHROMA_BLK_SZ_8x4:
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_8x4);    
        _vr00 = _vmovw( m_v_work_buf, 2);                   // load i1 with r1
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_8x4);    
		break;

	case CHROMA_BLK_SZ_8x8:
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_8x8);    
        _vr00 = _vmovw( m_v_work_buf, 2);                   // load i1 with r1
        RecMgr.Run(AM_Inter_FilterChromaBlkSz_8x8);    
		break;
	}

    return;
}
