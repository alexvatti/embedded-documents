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
#if !defined(_AURORA_H264_HOR_FILTERING_H_)
#define _AURORA_H264_HOR_FILTERING_H_

_Asm void LumaHorFiltering_HBlkSz_4_8(unsigned int inp_sdm_add,unsigned int out_sdm_add)
{
	% reg inp_sdm_add, out_sdm_add 
.off	percent_reg

//	mov r10, -5;
//	mov r11, 20;

    // load inp and output pointers
    vmovw	vr00, inp_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, out_sdm_add, 4	;	// load i2 with r2

.on	percent_reg
}

_Asm int Macro_LumaHorFiltering_HBlkSz_4_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load input data - ROW1
	vld64w	vr01, [i1, 0]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i1, 8]	;  // 000 000 000 w13 w12 w11 w10 w09
	// load input data - ROW2
	vld64w	vr08, [i1, 24]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr13, [i1, 32]  ;  // 000 000 000 w13 w12 w11 w10 w09

	//register format when block_hor_size == 4:
	// vr02 = w09 w08 w07 w06 w05 w04 w03 w02
	// vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	// vr04 = 000 000 w09 w08 w07 w06 w05 w04
	// vr05 = 000 000 000 w09 w08 w07 w06 w05
	// vr06 = 000 000 000 000 w09 w08 w07 w06

	vmr1w vr02, vr06, vr01	; // vr02 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr03, vr06, vr01	; // vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr04, vr06, vr01	; // vr04 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr05, vr06, vr01	; // vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr06, vr06, vr01	; // vr06 = w13 w12 w11 w10 w09 w08 w07 w06

	// arrange the registers - ROW 2
	vmr1w vr09, vr13, vr08	; // vr09 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr10, vr13, vr08	; // vr10 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr11, vr13, vr08	; // vr11 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr12, vr13, vr08	; // vr12 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr13, vr13, vr08	; // vr13 = w13 w12 w11 w10 w09 w08 w07 w06

	// broadcast multiply and accumulate - ROW1
	vbmulw	vr07, vr02, (-5)	; //acc  = vr02 * (-5)
	vbmulaw vr07, vr03, (20)	; //acc += vr03 * (20)
	vbmulaw vr07, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr07, vr05, (-5)	; //acc += vr05 * (-5)
	vaddaw  vr07, vr01, vr06; //acc += vr01 + vr06

	// broadcast multiply and accumulate - ROW2
	vbmulw	vr14, vr09, (-5)	; //acc  = vr02 * (-5)
	vbmulaw vr14, vr10, (20)	; //acc += vr03 * (20)
	vbmulaw vr14, vr11, (20)	; //acc += vr04 * (20)
	vbmulaw vr14, vr12, (-5)	; //acc += vr05 * (-5)
	vaddaw  vr14, vr08, vr13; //acc += vr08 + vr13
	
	// load input data - ROW3
	vld64w	vr01, [i1, 48]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i1, 56]  ;  // 000 000 000 w13 w12 w11 w10 w09

	// pack result
	vasrrpwb	vr07, vr07, 5	;	// pack the registers
	vasrrpwb	vr14, vr14, 5	;	// pack the registers

	// load input data - ROW4
	vld64w	vr08, [i1, 72]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr13, [i1, 80]  ;  // 000 000 000 w13 w12 w11 w10 w09

	// store output
	vst64	vr14,   [i2, 24];	// store the result
	vst64	vr07,   [i2, 0]	;	// store the result				

	// arrange the registers - ROW 3
	vmr1w vr02, vr06, vr01	; // vr02 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr03, vr06, vr01	; // vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr04, vr06, vr01	; // vr04 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr05, vr06, vr01	; // vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr06, vr06, vr01	; // vr06 = w13 w12 w11 w10 w09 w08 w07 w06

	// arrange the registers - ROW 4
	vmr1w vr09, vr13, vr08	; // vr09 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr10, vr13, vr08	; // vr10 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr11, vr13, vr08	; // vr11 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr12, vr13, vr08	; // vr12 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr13, vr13, vr08	; // vr13 = w13 w12 w11 w10 w09 w08 w07 w06

	// broadcast multiply and accumulate - ROW3
	vbmulw	vr07, vr02, (-5)	; //acc  = vr02 * (-5)
	vbmulaw vr07, vr03, (20)	; //acc += vr03 * (20)
	vbmulaw vr07, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr07, vr05, (-5)	; //acc += vr05 * (-5)
	vaddaw  vr07, vr01, vr06; //acc += vr01 + vr06

	// broadcast multiply and accumulate - ROW4
	vbmulw	vr14, vr09, (-5)	; //acc  = vr02 * (-5)
	vbmulaw vr14, vr10, (20)	; //acc += vr03 * (20)
	vbmulaw vr14, vr11, (20)	; //acc += vr04 * (20)
	vbmulaw vr14, vr12, (-5)	; //acc += vr05 * (-5)
	vaddaw  vr14, vr08, vr13; //acc += vr08 + vr13
	
	// pack result - row3 & row4
	vasrrpwb	vr07, vr07, 5	;	// pack the registers
	vasrrpwb	vr14, vr14, 5	;	// pack the registers

	// store output - row3 & row4
	vst64	vr07,  [i2, 48]	;	// store the result				
	vst64	vr14,  [i2, 72]	;	// store the result

    vbaddw  vr00,   vr00, (4*24) // Increment pointers i1 & i2
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

_Asm void LumaHorFiltering_HBlkSz_16(unsigned int inp_sdm_add,
												unsigned int out_sdm_add)
{
	% reg inp_sdm_add, out_sdm_add 
.off	percent_reg

//	mov r10, -5;
//	mov r11, 20;

    // load inp and output pointers
    vmovw	vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, out_sdm_add, 4		;	// load i2 with r2

.on	percent_reg
}

_Asm int Macro_LumaHorFiltering_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the input data
	vld64w	vr01, [i1, 0]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 16]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 0]	;	// store the result				
	vst64	vr09,   [i2, 8]	;	// store the result	

	// load the input data - row2
	vld64w	vr01, [i1, 24]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 32]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 40]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 24]	;	// store the result				
	vst64	vr09,   [i2, 32]	;	// store the result	



	// load the input data -- row3
	vld64w	vr01, [i1, 48]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 56]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 64]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 48]	;	// store the result				
	vst64	vr09,   [i2, 56]	;	// store the result	

	// load the input data - row4
	vld64w	vr01, [i1, 72]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 80]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 88]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 72]	;	// store the result				
	vst64	vr09,   [i2, 80]	;	// store the result	


	// PROCESS THE NEXT 4 ROWS

	// load the input data - ROW 5
	vld64w	vr01, [i1, 96]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 104]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 112]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 96]	;	// store the result				
	vst64	vr09,   [i2, 104]	;	// store the result	

	// load the input data - ROW6
	vld64w	vr01, [i1, 120]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 128]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 136]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 120]	;	// store the result				
	vst64	vr09,   [i2, 128]	;	// store the result	



	// load the input data -- ROW7
	vld64w	vr01, [i1, 144]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 152]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 160]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 144]	;	// store the result				
	vst64	vr09,   [i2, 152]	;	// store the result	

	// load the input data - ROW8
	vld64w	vr01, [i1, 168]	;  // w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 176]	;  // w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr20, [i1, 184]  ;  // 000 000 000 w21 w20 w19 w18 w17

	//arrange registers by shifts to the following format:
	// vr01 = w08 w07 w06 w05 w04 w03 w02 w01

	vmr1w vr03, vr02, vr01	; // vr03 = w09 w08 w07 w06 w05 w04 w03 w02
	vmr2w vr04, vr02, vr01	; // vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w vr05, vr02, vr01	; // vr05 = w11 w10 w09 w08 w07 w06 w05 w04
	vmr4w vr06, vr02, vr01	; // vr06 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr5w vr07, vr02, vr01	; // vr07 = w13 w12 w11 w10 w09 w08 w07 w06

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr08, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr08, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr08, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr08, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr08, vr01, vr01; //acc += vr01
	
	// Arrange the next 6 registers
	// vr02 = w16 w15 w14 w13 w12 w11 w10 w09

	vmr1w vr03, vr20, vr02	; // vr03 = w17 w16 w15 w14 w13 w12 w11 w10
	vmr2w vr04, vr20, vr02	; // vr04 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr3w vr05, vr20, vr02	; // vr05 = w19 w18 w17 w16 w15 w14 w13 w12
	vmr4w vr06, vr20, vr02	; // vr06 = w20 w19 w18 w17 w16 w15 w14 w13
	vmr5w vr07, vr20, vr02	; // vr07 = w21 w20 w19 w18 w17 w16 w15 w14

	// at this point acc = vr07
	// now broadcast multiply and accumulate
	vbmulaw vr09, vr03, (-5)	; //acc += vr03 * (-5)
	vbmulaw vr09, vr04, (20)	; //acc += vr04 * (20)
	vbmulaw vr09, vr05, (20)	; //acc += vr05 * (20)
	vbmulaw vr09, vr06, (-5)	; //acc += vr06 * (-5)
	vandaw  vr09, vr02, vr02; //acc += vr02

	// pack first 8 words
	vasrrpwb	vr08, vr08, 5	;	// pack the registers
	// pack next 8 words
	vasrrpwb	vr09, vr09, 5	;	// pack the registers

	//store result
	vst64	vr08,   [i2, 168]	;	// store the result				
	vst64	vr09,   [i2, 176]	;	// store the result	

    vbaddw  vr00, vr00, 8*24

//-------------------------------------
    vendrec r0              ;


.on	percent_reg
}

inline void AuroraH264Dec::PerformInpHorFiltering( SDM_Offset inp_sdm_add, 
							  SDM_Offset out_sdm_add,
							  int output_line_length)
{
    int rowIndex;

    //for each input row
	switch(m_block_hor_size)
	{
    case 4:
    case 8:
            // process 2 rows at a time
        LumaHorFiltering_HBlkSz_4_8(inp_sdm_add,    out_sdm_add);
		for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
		{
            RecMgr.Run(AM_Inter_LumaHorFiltering_HBlkSz_4_8);    
		}
		break;

        case 16:
			//process 1 row at a time
			LumaHorFiltering_HBlkSz_16(inp_sdm_add,	out_sdm_add);

            RecMgr.Run(AM_Inter_LumaHorFiltering_HBlkSz_16);    

    		if(16==m_block_ver_size)
    		{
    			// process the next 8 rows    
                RecMgr.Run(AM_Inter_LumaHorFiltering_HBlkSz_16);    
            }
		break;
    }

	return;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordLumaInterPredHorFilterMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the Chroma Inter prediction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLumaInterPredHorFilterMacros()
{
    Record(AM_Inter_LumaHorFiltering_HBlkSz_4_8, Macro_LumaHorFiltering_HBlkSz_4_8(NextAddr));
    Record(AM_Inter_LumaHorFiltering_HBlkSz_16, Macro_LumaHorFiltering_HBlkSz_16(NextAddr));
}

#endif	//_AURORA_H264_HOR_FILTERING_H_
