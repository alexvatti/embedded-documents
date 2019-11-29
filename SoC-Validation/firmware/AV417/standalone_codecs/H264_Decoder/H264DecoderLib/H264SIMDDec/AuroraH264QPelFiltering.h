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
#if !defined(_AURORA_H264_QPEL_FILTERING_H_)
#define _AURORA_H264_QPEL_FILTERING_H_


_Asm void QPelFiltering_HBlkSz_4_8(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8		;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_QPelFiltering_HBlkSz_4_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	vld64w		vr01,   [i1, 0]	; // ROW1:  inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w		vr02,   [i2, 0]	; // ROW1:  inp2: w08 w07 w06 w05 w04 w03 w02 w01

	vld64w		vr04,	[i1, 24]	; // ROW2: load the input1 data
	vld64w		vr05,   [i2, 24]	; // ROW2: load the input2 data

	vld64w		vr07,	[i1, 48]	; // ROW3: load the input1 data
	vld64w		vr08,   [i2, 48]	; // ROW3: load the input2 data

	vld64w		vr10,	[i1, 72]	; // ROW4: load the input1 data
	vld64w		vr11,   [i2, 72]	; // ROW4: load the input2 data

	vaddw	vr03, vr01, vr02	; // ROW1: vr03 = vr01 + vr02
	vaddw	vr06, vr04, vr05	; // ROW2: vr06 = vr04 + vr05
	vaddw	vr09, vr07, vr08	; // ROW3: vr09 = vr07 + vr08
	vaddw	vr12, vr10, vr11	; // ROW4: vr06 = vr04 + vr05

	vasrrpwb	vr03, vr03,   1	; // ROW1: round/pack the result
	vasrrpwb	vr06, vr06,   1	; // ROW2: round/pack the result
	vasrrpwb	vr09, vr09,   1	; // ROW3: round/pack the result
	vasrrpwb	vr12, vr12,   1	; // ROW4: round/pack the result

	vst64		vr03,   [i3, 0]	; // ROW1: store output
	vst64		vr06,	[i3, 24]	; // ROW2: store output
	vst64		vr09,	[i3, 48]	; // ROW3: store output
	vst64		vr12,	[i3, 72]	; // ROW4: store output
    vbaddw      vr00,   vr00, 4*24
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

_Asm void QPelFiltering_HBlkSz_16(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8		;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_QPelFiltering_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

    // process rows 1 & 2
	// load the input data
	vld64w	vr01, [i1, 0]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i1, 24]	    ;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr04, [i1, 32]	    ;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
		
	vld64w	vr05, [i2, 0]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i2, 8]	    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr07, [i2, 24]      ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr08, [i2, 32]      ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// calculate the output
	vaddw	vr01, vr01, vr05	; // vr05 = vr01 + vr03
	vaddw	vr02, vr02, vr06	; // vr06 = vr02 + vr04

	vaddw	vr03, vr03, vr07	; // vr11 = vr07 + vr09
	vaddw	vr04, vr04, vr08	; // vr12 = vr08 + vr10

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result
	vasrrpwb	vr03, vr03, 1	;	// round/pack the result
	vasrrpwb	vr04, vr04, 1	;	// round/pack the result

	vst64	vr01, [i3,  0] ;	//store output
	vst64	vr02, [i3,  8] ;	//store output
	vst64	vr03, [i3, 24] ;	//store output
	vst64	vr04, [i3, 32] ;	//store output

    // process rows 3 & 4
	// load the input data
	vld64w	vr01, [i1, 48]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 56]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i1, 72]	    ;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr04, [i1, 80]	    ;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
		
	vld64w	vr05, [i2, 48]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i2, 56]	    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr07, [i2, 72]      ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr08, [i2, 80]      ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// calculate the output
	vaddw	vr01, vr01, vr05	; // vr05 = vr01 + vr03
	vaddw	vr02, vr02, vr06	; // vr06 = vr02 + vr04

	vaddw	vr03, vr03, vr07	; // vr11 = vr07 + vr09
	vaddw	vr04, vr04, vr08	; // vr12 = vr08 + vr10

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result
	vasrrpwb	vr03, vr03, 1	;	// round/pack the result
	vasrrpwb	vr04, vr04, 1	;	// round/pack the result

	vst64	vr01, [i3, 48] ;	//store output
	vst64	vr02, [i3, 56] ;	//store output
	vst64	vr03, [i3, 72] ;	//store output
	vst64	vr04, [i3, 80] ;	//store output

    // process rows 5 & 6
	// load the input data
	vld64w	vr01, [i1,  96]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 104]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i1, 120]	    ;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr04, [i1, 128]	    ;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
		
	vld64w	vr05, [i2, 96]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i2, 104]	    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr07, [i2, 120]      ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr08, [i2, 128]      ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// calculate the output
	vaddw	vr01, vr01, vr05	; // vr05 = vr01 + vr03
	vaddw	vr02, vr02, vr06	; // vr06 = vr02 + vr04

	vaddw	vr03, vr03, vr07	; // vr11 = vr07 + vr09
	vaddw	vr04, vr04, vr08	; // vr12 = vr08 + vr10

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result
	vasrrpwb	vr03, vr03, 1	;	// round/pack the result
	vasrrpwb	vr04, vr04, 1	;	// round/pack the result

	vst64	vr01, [i3, 96] ;	//store output
	vst64	vr02, [i3, 104] ;	//store output
	vst64	vr03, [i3, 120] ;	//store output
	vst64	vr04, [i3, 128] ;	//store output


    // process rows 7 & 8
	// load the input data
	vld64w	vr01, [i1, 144]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 152]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i1, 168]	    ;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr04, [i1, 176]	    ;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
		
	vld64w	vr05, [i2, 144]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr06, [i2, 152]	    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr07, [i2, 168]      ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr08, [i2, 176]      ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// calculate the output
	vaddw	vr01, vr01, vr05	; // vr05 = vr01 + vr03
	vaddw	vr02, vr02, vr06	; // vr06 = vr02 + vr04

	vaddw	vr03, vr03, vr07	; // vr11 = vr07 + vr09
	vaddw	vr04, vr04, vr08	; // vr12 = vr08 + vr10

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result
	vasrrpwb	vr03, vr03, 1	;	// round/pack the result
	vasrrpwb	vr04, vr04, 1	;	// round/pack the result

	vst64	vr01, [i3, 144] ;	//store output
	vst64	vr02, [i3, 152] ;	//store output
	vst64	vr03, [i3, 168] ;	//store output
	vst64	vr04, [i3, 176] ;	//store output
    vbaddw      vr00,   vr00, 192
    
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	QPelFiltering	                    */
/* DESCRIPTION:							                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */
inline void AuroraH264Dec::QPelFiltering( SDM_Offset inp1_sdm_add,
						SDM_Offset inp2_sdm_add,
						SDM_Offset out_sdm_add)
{
	int rowIndex;

	//handle each block size separately
	switch(m_block_hor_size)
	{
	case 4:
	case 8:
        QPelFiltering_HBlkSz_4_8(inp1_sdm_add,
                        inp2_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
			//process 4 rows at a time

            RecMgr.Run(AM_Inter_LumaQPelFiltering_HBlkSz_4_8);

        }
		break;
        
	case 16:
		//process first 8 rows in parallel
		{
			QPelFiltering_HBlkSz_16(inp1_sdm_add,
						inp2_sdm_add, out_sdm_add);

	        RecMgr.Run(AM_Inter_LumaQPelFiltering_HBlkSz_16);

		}
		if( 16 == m_block_ver_size)
		{

            RecMgr.Run(AM_Inter_LumaQPelFiltering_HBlkSz_16);
			
		}
		break;
        
	}
	return;
}


_Asm void Shft2QPelFil_HBlkSz_4(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8		;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft2QPelFil_HBlkSz_4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	;// load the input data
	vld64w	vr01, [i1, 0]	; // ROW1: inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i2, 0]   ; // ROW1: inp2: w08 w07 w06 w05 w04 w03 w02 w01

	vld64w	vr04, [i1, 24]	; // ROW2: load the input1 data
	vld64w	vr05, [i2, 24]	; // ROW2: load the input2 data

	vld64w	vr07, [i1, 48]	; // ROW3: load the input1 data
	vld64w	vr08, [i2, 48]	; // ROW3: load the input2 data

	vld64w	vr10, [i1, 72]	; // ROW4: load the input1 data
	vld64w	vr11, [i2, 72]	; // ROW4: load the input2 data

	vmr2w   vr01, vr20, vr01 ; // shift right by 2, rejects the first 2 col
	vmr2w   vr04, vr20, vr04 ; // shift right by 2, rejects the first 2 col
	vmr2w   vr07, vr20, vr07 ; // shift right by 2, rejects the first 2 col
	vmr2w   vr10, vr20, vr10 ; // shift right by 2, rejects the first 2 col

	vaddw	vr03, vr01, vr02	; // ROW1: output vr03 = vr01 + vr02
	vaddw	vr06, vr04, vr05	; // ROW2: vr06 = vr04 + vr05
	vaddw	vr09, vr07, vr08	; // ROW3: vr09 = vr07 + vr08
	vaddw	vr12, vr10, vr11	; // ROW4: vr06 = vr04 + vr05

	vasrrpwb	vr03, vr03,   1	; // ROW1: round/pack the result
	vasrrpwb	vr06, vr06,   1	; // ROW2: round/pack the result
	vasrrpwb	vr09, vr09,   1	; // ROW3: round/pack the result
	vasrrpwb	vr12, vr12,   1	; // ROW4: round/pack the result

	vst64	vr03, [i3, 0]	; // ROW1: store output
	vst64	vr06, [i3, 24]	; // ROW2: store output
	vst64	vr09, [i3, 48]	; // ROW3: store output
	vst64	vr12, [i3, 72]	; // ROW4: store output
//-------------------------------------
    vbaddw  vr00, vr00, 4*24    ; bump pointers
    vendrec r0              ;


.on	percent_reg
}

_Asm void Shft2QPelFil_HBlkSz_8(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4	;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8	;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft2QPelFil_HBlkSz_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the input data
	vld64w	vr01, [i1, 0]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i2, 0]	;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01

	vld64w	vr05, [i1, 24]	; // ROW2: input1
	vld64w	vr06, [i1, 32]	; // ROW2: input1
	vld64w	vr07, [i2, 24]	; // ROW2: input2

	vld64w	vr09, [i1, 48]	; // ROW3: input1
	vld64w	vr10, [i1, 56]	; // ROW3: input1
	vld64w	vr11, [i2, 48]	; // ROW3: input2

	vld64w	vr13, [i1, 72]	; // ROW4: input1
	vld64w	vr14, [i1, 80]	; // ROW4: input1
	vld64w	vr15, [i2, 72]	; // ROW4: input2

	vmr2w   vr01, vr02, vr01 ; // shift right by 2 to reject the first 2 columns
	vmr2w   vr05, vr06, vr05 ; // shift right by 2 to reject the first 2 columns
	vmr2w   vr09, vr10, vr09 ; // shift right by 2 to reject the first 2 columns
	vmr2w   vr13, vr14, vr13 ; // shift right by 2 to reject the first 2 columns

	vaddw	vr04, vr01, vr03	; // vr04 = vr01 + vr03
	vaddw	vr08, vr05, vr07	; // vr04 = vr01 + vr03
	vaddw	vr12, vr09, vr11	; // vr04 = vr01 + vr03
	vaddw	vr16, vr13, vr15	; // vr04 = vr01 + vr03
	
	vasrrpwb	vr04, vr04, 1	; // round/pack the result
	vasrrpwb	vr08, vr08, 1	; // round/pack the result
	vasrrpwb	vr12, vr12, 1	; // round/pack the result
	vasrrpwb	vr16, vr16, 1	; // round/pack the result
	
	vst64		vr04, [i3, 0]		; // store output
	vst64		vr08, [i3, 24]		; // store output
	vst64		vr12, [i3, 48]		; // store output
	vst64		vr16, [i3, 72]		; // store output
    vbaddw      vr00, vr00, 4*24    ; bump pointers

//-------------------------------------
    vendrec r0              ;
.on	percent_reg
}

_Asm void Shft2QPelFil_HBlkSz_16(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4	;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8	;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft2QPelFil_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------

	// load input data - ROW 1
	vld64w	vr01, [i1, 0]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 16]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	vld64w	vr04, [i2, 0]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 8]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 2
	vld64w	vr06, [i1, 24]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 32]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 40]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	vld64w	vr09, [i2, 24]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 32]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 0]		;	//store output
	vst64	vr02, [i3, 8]		;	//store output

	vst64	vr06, [i3, 24]	;	//store output
	vst64	vr07, [i3, 32]	;	//store output


	// load input data - ROW 3
	vld64w	vr01, [i1, 48]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 56]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 64]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 48]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 56]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 4
	vld64w	vr06, [i1, 72]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 80]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 88]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 72]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 80]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 48]		;	//store output
	vst64	vr02, [i3, 56]		;	//store output

	vst64	vr06, [i3, 72]	;	//store output
	vst64	vr07, [i3, 80]	;	//store output




	// load input data - ROW 5
	vld64w	vr01, [i1, 96]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 104]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 112]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 96]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 104]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 6
	vld64w	vr06, [i1, 120]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 128]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 136]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 120]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 128]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 96]		;	//store output
	vst64	vr02, [i3, 104]		;	//store output

	vst64	vr06, [i3, 120]	;	//store output
	vst64	vr07, [i3, 128]	;	//store output


	// load input data - ROW 7
	vld64w	vr01, [i1, 144]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 152]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 160]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 144]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 152]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 8
	vld64w	vr06, [i1, 168]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 176]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 184]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 168]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 176]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr2w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr2w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 144]		;	//store output
	vst64	vr02, [i3, 152]		;	//store output

	vst64	vr06, [i3, 168]	;	//store output
	vst64	vr07, [i3, 176]	;	//store output

    vbaddw  vr00, vr00, 8*24
//-------------------------------------
    vendrec r0              ;
.on	percent_reg
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:	ShiftBy2AndQPelFiltering	                    */
/* DESCRIPTION:	discards the first 2 columns of inp1						                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */
inline void AuroraH264Dec::ShiftBy2AndQPelFiltering( SDM_Offset inp1_sdm_add,
						SDM_Offset inp2_sdm_add,
						SDM_Offset out_sdm_add) // a, e, i, p
{
	int rowIndex;


	//handle each block size separately
	switch(m_block_hor_size)
	{
	case 4:
        Shft2QPelFil_HBlkSz_4(inp1_sdm_add, inp2_sdm_add,out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex +=4)
    	{
			//process 4 rows together
										 
            RecMgr.Run(AM_Inter_LumaShft2QPelFil_HBlkSz_4);

        }
		break;

	case 8:
        Shft2QPelFil_HBlkSz_8(inp1_sdm_add, inp2_sdm_add,
                                         out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex +=4)
    	{
			//process 4 rows together

            RecMgr.Run(AM_Inter_LumaShft2QPelFil_HBlkSz_8);

        }
		break;

	case 16:
		// process first 8 rows in parallel
    	{
			Shft2QPelFil_HBlkSz_16(inp1_sdm_add, inp2_sdm_add,
										 out_sdm_add);

            RecMgr.Run(AM_Inter_LumaShft2QPelFil_HBlkSz_16);
    	}
		if( 16 == m_block_ver_size)
		{
			// process next 8 rows

            RecMgr.Run(AM_Inter_LumaShft2QPelFil_HBlkSz_16);
        }
		break;

	}
}

_Asm void Shft3QPelFil_HBlkSz_4(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4	;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8	;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft3QPelFil_HBlkSz_4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the input data
	vld64w	vr01, [i1, 0]		; // ROW1: inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i2, 0]	    ; // ROW1: inp2: w08 w07 w06 w05 w04 w03 w02 w01

	vld64w	vr04, [i1, 24]	; // ROW2: load the input1 data
	vld64w	vr05, [i2, 24]	; // ROW2: load the input2 data

	vld64w	vr07, [i1, 48]	; // ROW3: load the input1 data
	vld64w	vr08, [i2, 48]	; // ROW3: load the input2 data

	vld64w	vr10, [i1, 72]	; // ROW4: load the input1 data
	vld64w	vr11, [i2, 72]	; // ROW4: load the input2 data

	vmr3w   vr01, vr20, vr01 ; // shift right by 2, rejects the first 2 col
	vmr3w   vr04, vr20, vr04 ; // shift right by 2, rejects the first 2 col
	vmr3w   vr07, vr20, vr07 ; // shift right by 2, rejects the first 2 col
	vmr3w   vr10, vr20, vr10 ; // shift right by 2, rejects the first 2 col

	vaddw	vr03, vr01, vr02	; // ROW1: output vr03 = vr01 + vr02
	vaddw	vr06, vr04, vr05	; // ROW2: vr06 = vr04 + vr05
	vaddw	vr09, vr07, vr08	; // ROW3: vr09 = vr07 + vr08
	vaddw	vr12, vr10, vr11	; // ROW4: vr06 = vr04 + vr05

	vasrrpwb	vr03, vr03,   1	; // ROW1: round/pack the result
	vasrrpwb	vr06, vr06,   1	; // ROW2: round/pack the result
	vasrrpwb	vr09, vr09,   1	; // ROW3: round/pack the result
	vasrrpwb	vr12, vr12,   1	; // ROW4: round/pack the result

	vst64	vr03, [i3, 0]	; // ROW1: store output
	vst64	vr06, [i3, 24]	; // ROW2: store output
	vst64	vr09, [i3, 48]	; // ROW3: store output
	vst64	vr12, [i3, 72]	; // ROW4: store output
    
    vbaddw      vr00, vr00, 4*24  ; Up pointers
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}


_Asm void Shft3QPelFil_HBlkSz_8(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4	;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8	;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft3QPelFil_HBlkSz_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the input data
	vld64w	vr01, [i1, 0]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w	vr03, [i2, 0]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01

	vld64w	vr05, [i1, 24]	; // ROW2: input1
	vld64w	vr06, [i1, 32]	; // ROW2: input1
	vld64w	vr07, [i2, 24]	; // ROW2: input2

	vld64w	vr09, [i1, 48]	; // ROW3: input1
	vld64w	vr10, [i1, 56]	; // ROW3: input1
	vld64w	vr11, [i2, 48]	; // ROW3: input2

	vld64w	vr13, [i1, 72]	; // ROW4: input1
	vld64w	vr14, [i1, 80]	; // ROW4: input1
	vld64w	vr15, [i2, 72]	; // ROW4: input2

	vmr3w   vr01, vr02, vr01 ; // shift right by 2 to reject the first 2 columns
	vmr3w   vr05, vr06, vr05 ; // shift right by 2 to reject the first 2 columns
	vmr3w   vr09, vr10, vr09 ; // shift right by 2 to reject the first 2 columns
	vmr3w   vr13, vr14, vr13 ; // shift right by 2 to reject the first 2 columns

	vaddw	vr04, vr01, vr03	; // vr04 = vr01 + vr03
	vaddw	vr08, vr05, vr07	; // vr04 = vr01 + vr03
	vaddw	vr12, vr09, vr11	; // vr04 = vr01 + vr03
	vaddw	vr16, vr13, vr15	; // vr04 = vr01 + vr03
	
	vasrrpwb	vr04, vr04, 1	; // round/pack the result
	vasrrpwb	vr08, vr08, 1	; // round/pack the result
	vasrrpwb	vr12, vr12, 1	; // round/pack the result
	vasrrpwb	vr16, vr16, 1	; // round/pack the result
	
	vst64	vr04, [i3, 0]		; // store output
	vst64	vr08, [i3,24]		; // store output
	vst64	vr12, [i3,48]		; // store output
	vst64	vr16, [i3,72]		; // store output

    vbaddw  vr00, vr00, 4*24
//-------------------------------------
    vendrec r0              ;

.on	percent_reg

}

_Asm void Shft3QPelFil_HBlkSz_16(unsigned int inp1_sdm_add,
										 unsigned int inp2_sdm_add,
										unsigned int out_sdm_add)
{
	% reg inp1_sdm_add, inp2_sdm_add, out_sdm_add
.off	percent_reg

    // load inp and output pointers
    vmovw	vr00, inp1_sdm_add, 2	;	// load i1 with r1
    vmovw	vr00, inp2_sdm_add, 4	;	// load i2 with r2
    vmovw	vr00,  out_sdm_add, 8	;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_Shft3QPelFil_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load input data - ROW 1
	vld64w	vr01, [i1, 0]		;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 8]		;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 16]		;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 0]	    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 8]	    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 2
	vld64w	vr06, [i1, 24]  ;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 32]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 40]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17

    vld64w	vr09, [i2, 24]  ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 32]  ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 0]		;	//store output
	vst64	vr02, [i3, 8]		;	//store output
	vst64	vr06, [i3, 24]	;	//store output
	vst64	vr07, [i3, 32]	;	//store output




	// load input data - ROW 3
	vld64w	vr01, [i1, 48]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 56]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 64]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 48]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 56]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 4
	vld64w	vr06, [i1, 72]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 80]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 88]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 72]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 80]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 48]		;	//store output
	vst64	vr02, [i3, 56]		;	//store output

	vst64	vr06, [i3, 72]	;	//store output
	vst64	vr07, [i3, 80]	;	//store output





	// load input data - ROW 5
	vld64w	vr01, [i1, 96]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 104]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 112]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 96]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 104]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 6
	vld64w	vr06, [i1, 120]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 128]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 136]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 120]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 128]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 96]		;	//store output
	vst64	vr02, [i3, 104]		;	//store output

	vst64	vr06, [i3, 120]	;	//store output
	vst64	vr07, [i3, 128]	;	//store output




	// load input data - ROW 7
	vld64w	vr01, [i1, 144]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr02, [i1, 152]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr03, [i1, 160]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr04, [i2, 144]    ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr05, [i2, 152]    ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	// load input data - ROW 8
	vld64w	vr06, [i1, 168]	;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr07, [i1, 176]	;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
	vld64w  vr08, [i1, 184]	;  // inp1: 000 000 000 w21 w20 w19 w18 w17
	
	vld64w	vr09, [i2, 168]   ;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
	vld64w	vr10, [i2, 176]   ;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr01, vr02, vr01 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr02, vr03, vr02 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	//shift right by 2 to reject the first 2 columns
	vmr3w   vr06, vr07, vr06 ; // vr06= w10 w09 w08 w07 w06 w05 w04 w03
	vmr3w   vr07, vr08, vr07 ; // vr07= w18 w17 w16 w15 w14 w13 w12 w11

	// calculate the output
	vaddw	vr01, vr04, vr01	; // vr08 = vr06 + vr04
	vaddw	vr02, vr05, vr02	; // vr09 = vr07 + vr05

	// calculate the output
	vaddw	vr06, vr09, vr06	; // vr08 = vr06 + vr04
	vaddw	vr07, vr10, vr07	; // vr09 = vr07 + vr05

	// round, pack and save the result
	vasrrpwb	vr01, vr01, 1	;	// round/pack the result
	vasrrpwb	vr02, vr02, 1	;	// round/pack the result

	// round, pack and save the result
	vasrrpwb	vr06, vr06, 1	;	// round/pack the result
	vasrrpwb	vr07, vr07, 1	;	// round/pack the result

	vst64	vr01, [i3, 144]		;	//store output
	vst64	vr02, [i3, 152]		;	//store output

	vst64	vr06, [i3, 168]	;	//store output
	vst64	vr07, [i3, 176]	;	//store output


    vbaddw  vr00, vr00, 8*24

//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	ShiftBy3AndQPelFiltering	                    */
/* DESCRIPTION:	discards the first 2 columns of inp1						                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */
inline void AuroraH264Dec::ShiftBy3AndQPelFiltering( SDM_Offset inp1_sdm_add,
						SDM_Offset inp2_sdm_add,
						SDM_Offset out_sdm_add) // c, g, k, r
{
	int rowIndex;

	//handle each block size separately
	switch(m_block_hor_size)
	{
	case 4:
        Shft3QPelFil_HBlkSz_4( inp1_sdm_add, inp2_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
    		//process 4 rows together
    		
            RecMgr.Run(AM_Inter_LumaShft3QPelFil_HBlkSz_4);

        }
    	break;

	case 8:
		//process 4 rows together
        Shft3QPelFil_HBlkSz_8( inp1_sdm_add, inp2_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{

            RecMgr.Run(AM_Inter_LumaShft3QPelFil_HBlkSz_8);
        }
    	break;

	case 16:
		//process first 8 rows together
    	{
    		Shft3QPelFil_HBlkSz_16( inp1_sdm_add, inp2_sdm_add, out_sdm_add);

            RecMgr.Run(AM_Inter_LumaShft3QPelFil_HBlkSz_16);
    	}
		if( 16 == m_block_ver_size)
		{
            RecMgr.Run(AM_Inter_LumaShft3QPelFil_HBlkSz_16);
        }
		break;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordLumaInterPredQPelFilterMacros       	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the chroma reconstruction macros                    */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLumaInterPredQPelFilterMacros()
{
    Record(AM_Inter_LumaQPelFiltering_HBlkSz_4_8, Macro_QPelFiltering_HBlkSz_4_8(NextAddr));
    Record(AM_Inter_LumaQPelFiltering_HBlkSz_16,  Macro_QPelFiltering_HBlkSz_16(NextAddr));
    Record(AM_Inter_LumaShft2QPelFil_HBlkSz_4,    Macro_Shft2QPelFil_HBlkSz_4(NextAddr));
    Record(AM_Inter_LumaShft2QPelFil_HBlkSz_8,    Macro_Shft2QPelFil_HBlkSz_8(NextAddr));
    Record(AM_Inter_LumaShft2QPelFil_HBlkSz_16,   Macro_Shft2QPelFil_HBlkSz_16(NextAddr));
    Record(AM_Inter_LumaShft3QPelFil_HBlkSz_4,    Macro_Shft3QPelFil_HBlkSz_4(NextAddr));
    Record(AM_Inter_LumaShft3QPelFil_HBlkSz_8,    Macro_Shft3QPelFil_HBlkSz_8(NextAddr));
    Record(AM_Inter_LumaShft3QPelFil_HBlkSz_16,   Macro_Shft3QPelFil_HBlkSz_16(NextAddr));
}

#endif //_AURORA_H264_QPEL_FILTERING_H_
