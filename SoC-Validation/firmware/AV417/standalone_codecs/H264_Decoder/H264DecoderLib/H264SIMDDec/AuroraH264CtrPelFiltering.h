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
/* FILE NAME:       AuroraH264CtrPelFiltering.cpp                           */
/* DESCRIPTION:		SIMD code Chroma inter prediction.                      */
/*																			*/
/* ------------------------------------------------------------------------ */
#if !defined(_AURORA_H264_CTRPEL_FILTERING_H_)
#define _AURORA_H264_CTRPEL_FILTERING_H_


_Asm void CPelFilterWOInpRnd_HBlkSz_4(unsigned int inp_sdm_add,	unsigned int out_sdm_add)
{
	% reg inp_sdm_add, out_sdm_add 

    // load inp and output pointers
    vmovw	%vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	%vr00, out_sdm_add, 4		;	// load i2 with r2

.on	percent_reg
}

_Asm int Macro_CPelFilterWOInpRnd_HBlkSz_4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	;// load input data for ROW1
	vld128	%vr01, [%i1, 0]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr02, [%i1, 16]    ;  // %vr02 = 000 000 000 000 000 000 000 w09
	;// load input data for ROW2
	vld128	%vr05, [%i1, 48]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr06, [%i1, 64]    ;  // %vr02 = 000 000 000 000 000 000 000 w09
	;// load input data for ROW3
	vld128	%vr09, [%i1, 96]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr10, [%i1, 112]   ;  // %vr02 = 000 000 000 000 000 000 000 w09
	;// load input data for ROW4
	vld128	%vr13, [%i1, 144]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr14, [%i1, 160]   ;  // %vr02 = 000 000 000 000 000 000 000 w09

	;// arrange the next inp
	vmr2w	%vr03, %vr02, %vr01 ;	// %vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	%vr07, %vr06, %vr05 ;	// %vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	%vr11, %vr10, %vr09 ;	// %vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	%vr15, %vr14, %vr13 ;	// %vr03 = 000 w09 w08 w07 w06 w05 w04 w03

	;// apply filter & store the output in %vr05
	vd6tapf %vr04, %vr01, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf %vr04, %vr03, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf %vr08, %vr05, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf %vr08, %vr07, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf %vr12, %vr09, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf %vr12, %vr11, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf %vr16, %vr13, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf %vr16, %vr15, 0xC ;  // 0 0 0 0 x x 0 0

	;// pack result
	vasrrpwb	%vr04, %vr04, 0	;	// pack the registers
	vasrrpwb	%vr08, %vr08, 0	;	// pack the registers
	vasrrpwb	%vr12, %vr12, 0	;	// pack the registers
	vasrrpwb	%vr16, %vr16, 0	;	// pack the registers

    vmovw       vr23, 8*24, 2   ; ptr incs
    vmovw       vr23, 4*24, 4
	;// store result
	vst64		%vr04,   [%i2, 0]	;	// store the result	
	vst64		%vr08,   [%i2, 24]	;	// store the result	
	vst64		%vr12,   [%i2, 48]	;	// store the result
	vst64		%vr16,   [%i2, 72]	;	// store the result
    
    vaddw       vr00, vr00, vr23    ; inc ptrs
//-------------------------------------
    vendrec r0              ;
}

_Asm void CPelFilterWOInpRnd_HBlkSz_8(unsigned int inp_sdm_add, unsigned int out_sdm_add)
{
	 % reg inp_sdm_add, out_sdm_add 


    ;// load inp and output pointers
    vmovw	%vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	%vr00, out_sdm_add, 4		;	// load i2 with r2

.on	percent_reg
}

_Asm int Macro_CPelFilterWOInpRnd_HBlkSz_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	;// load the input data
	vld128	%vr01, [%i1, 0]		;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr05, [%i1, 16]    ;  // %vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	%vr06, [%i1, 48]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr10, [%i1, 64]    ;  // %vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	%vr11, [%i1, 96]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr15, [%i1, 112]   ;  // %vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	%vr16, [%i1, 144]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr20, [%i1, 160]   ;  // %vr02 = 000 000 000 w13 w12 w11 w10 w09

	;// arrange data
	vmr2w	%vr02, %vr05, %vr01 ;	// %vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr03, %vr05, %vr01 ;	// %vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr04, %vr05, %vr01 ;	// %vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	;// arrange data
	vmr2w	%vr07, %vr10, %vr06 ;	// %vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr08, %vr10, %vr06 ;	// %vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr09, %vr10, %vr06 ;	// %vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	;// arrange data
	vmr2w	%vr12, %vr15, %vr11 ;	// %vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr13, %vr15, %vr11 ;	// %vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr14, %vr15, %vr11 ;	// %vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	;// arrange data
	vmr2w	%vr17, %vr20, %vr16 ;	// %vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr18, %vr20, %vr16 ;	// %vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr19, %vr20, %vr16 ;	// %vr05 = 000 w13 w12 w11 w10 w09 w08 w07

	;// apply filter & store the output in %vr06
	vd6tapf %vr02, %vr02, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr02, %vr01, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr02, %vr03, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf %vr02, %vr04, 0xC0; // x x 0 0 0 0 0 0

	;// apply filter & store the output in %vr06
	vd6tapf %vr07, %vr07, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr07, %vr06, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr07, %vr08, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf %vr07, %vr09, 0xC0; // x x 0 0 0 0 0 0 
	
	;// apply filter & store the output in %vr06
	vd6tapf %vr12, %vr12, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr12, %vr11, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr12, %vr13, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf %vr12, %vr14, 0xC0; // x x 0 0 0 0 0 0 
	;// apply filter & store the output in %vr06
	vd6tapf %vr17, %vr17, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr17, %vr16, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr17, %vr18, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf %vr17, %vr19, 0xC0; // x x 0 0 0 0 0 0 

	;// pack & store the result
	vasrrpwb	%vr02, %vr02, 0	;	// pack the registers
	vasrrpwb	%vr07, %vr07, 0	;	// pack the registers
	vasrrpwb	%vr12, %vr12, 0	;	// pack the registers
	vasrrpwb	%vr17, %vr17, 0	;	// pack the registers

    vmovw       vr23, 8*24, 2   ; ptr incs
    vmovw       vr23, 4*24, 4
	// pack & store the result
	vst64		%vr02,   [%i2, 0]	;	// store the result
	vst64		%vr07,   [%i2, 24];	// store the result
	vst64		%vr12,   [%i2, 48];	// store the result
	vst64		%vr17,   [%i2, 72];	// store the result
    
    vaddw       vr00, vr00, vr23    ; inc ptrs

//-------------------------------------
    vendrec r0              ;


}

_Asm void CPelFilterWOInpRnd_HBlkSz_16(unsigned int inp_sdm_add,
												unsigned int out_sdm_add)
{
	 % reg inp_sdm_add, out_sdm_add

    ;// load inp and output pointers
    vmovw	%vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	%vr00, out_sdm_add, 4		;	// load i2 with r2

.on	percent_reg
}

_Asm int Macro_CPelFilterWOInpRnd_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load input data  ROW-1
	vld128	%vr01, [%i1, 0]		;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr02, [%i1, 16]    ;  // %vr02 = w16 w15 w14 w13 w12 w11 w10 w09
	vld128  %vr03, [%i1, 32]	;	// %vr03 = 000 000 000 w21 w20 w19 w18 w17

	// load input data  ROW-2
	vld128	%vr11, [%i1, 48]	;  // %vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	%vr12, [%i1, 64]    ;  // %vr02 = w16 w15 w14 w13 w12 w11 w10 w09
	vld128  %vr13, [%i1, 80]	;	// %vr03 = 000 000 000 w21 w20 w19 w18 w17

	// arrange inp: row-1/set-1
	vmr2w	%vr04, %vr02, %vr01 ;	// %vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr05, %vr02, %vr01 ;	// %vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr06, %vr02, %vr01 ;	// %vr06 = w14 w13 w12 w11 w10 w09 w08 w07

	// arrange inp: row-1/set-2
	vmr2w	%vr07, %vr03, %vr02 ; // %vr07 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr4w   %vr08, %vr03, %vr02 ; // %vr08 = w20 w19 w18 w17 w16 w15 w14 w13				
	vmr6w	%vr09, %vr03, %vr02 ; // %vr09 = 000 w21 w20 w19 w18 w17 w16 w15

	// apply filter: row-1/set-1
	vd6tapf %vr04, %vr04, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr04, %vr01, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr04, %vr05, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf %vr04, %vr06, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter: row-1/set-2
	vd6tapf %vr07, %vr07, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr07, %vr02, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr07, %vr08, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf %vr07, %vr09, 0xC0; // x x 0 0 0 0 0 0

	// arrange inp: row-2/set-1
	vmr2w	%vr14, %vr12, %vr11 ;	// %vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	%vr15, %vr12, %vr11 ;	// %vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	%vr16, %vr12, %vr11 ;	// %vr06 = w14 w13 w12 w11 w10 w09 w08 w07

	// arrange inp: row-2/set-2
	vmr2w	%vr17, %vr13, %vr12 ; // %vr07 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr4w   %vr18, %vr13, %vr12 ; // %vr08 = w20 w19 w18 w17 w16 w15 w14 w13				
	vmr6w	%vr19, %vr13, %vr12 ; // %vr09 = 000 w21 w20 w19 w18 w17 w16 w15

	// apply filter: row-2/set-1
	vd6tapf %vr14, %vr14, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr14, %vr11, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr14, %vr15, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf %vr14, %vr16, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter: row-2/set-2
	vd6tapf %vr17, %vr17, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf %vr17, %vr12, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf %vr17, %vr18, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf %vr17, %vr19, 0xC0; // x x 0 0 0 0 0 0

	// pack and store the result
	vasrrpwb	%vr04, %vr04, 0	;	// pack the registers: row-1/set-1
	vasrrpwb	%vr07, %vr07, 0	;	// pack the registers: row-1/set-2
	vasrrpwb	%vr14, %vr14, 0	;	// pack the registers: row-1/set-2
	vasrrpwb	%vr17, %vr17, 0	;	// pack the registers: row-2/set-2

    vmovw       vr23, 4*24, 2   ; ptr incs
    vmovw       vr23, 2*24, 4

	vst64	%vr04,   [%i2, 0]	;	// store the result: row-1/set-1
	vst64	%vr07,   [%i2, 8]	;	// store the result: row-1/set-2
	vst64	%vr14,   [%i2, 24];	// store the result: row-2/set-2
	vst64	%vr17,   [%i2, 32];	// store the result: row-2/set-2
    
    vaddw       vr00, vr00, vr23    ; inc ptrs

//-------------------------------------
    vendrec r0              ;

}

inline void AuroraH264Dec::CentrePelFilterWOInpRounding( SDM_Offset inp_sdm_add,
							SDM_Offset out_sdm_add )
{
	int rowIndex;
	
	
	//handle each block size separately
	switch(m_block_hor_size)
	{
	case 4:
        CPelFilterWOInpRnd_HBlkSz_4(inp_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
            RecMgr.Run(AM_Inter_CPelFilterWOInpRnd_HBlkSz_4);
        }
		break;

	case 8:
        CPelFilterWOInpRnd_HBlkSz_8(inp_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
            RecMgr.Run(AM_Inter_CPelFilterWOInpRnd_HBlkSz_8);
        }
		break;

	case 16:
        CPelFilterWOInpRnd_HBlkSz_16(inp_sdm_add, out_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 2)
    	{
    		// process 2 rows simultaneously

            RecMgr.Run(AM_Inter_CPelFilterWOInpRnd_HBlkSz_16);
        }
		break;
		
	}
 
}

_Asm void CPelFilterAndInpRnd_HBlkSz_4(unsigned int inp_sdm_add,
				unsigned int out_sdm_add, unsigned int rounded_inp_sdm_add)
{
	 % reg inp_sdm_add , out_sdm_add , rounded_inp_sdm_add
.off	percent_reg

    ;// load inp and output pointers
    vmovw	vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, out_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00, rounded_inp_sdm_add, 8 ;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_CPelFilterAndInpRnd_HBlkSz_4(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load input data for ROW1
	vld128	vr01, [i1, 0]		;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr02, [i1, 16]    ;  // vr02 = 000 000 000 000 000 000 000 w09
	// load input data for ROW2
	vld128	vr05, [i1, 48]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr06, [i1, 64]    ;  // vr02 = 000 000 000 000 000 000 000 w09
	// load input data for ROW3
	vld128	vr09, [i1, 96]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr10, [i1, 112]   ;  // vr02 = 000 000 000 000 000 000 000 w09
	// load input data for ROW4
	vld128	vr13, [i1, 144]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr14, [i1, 160]   ;  // vr02 = 000 000 000 000 000 000 000 w09

	// arrange the next inp
	vmr2w	vr03, vr02, vr01 ;	// vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	vr07, vr06, vr05 ;	// vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	vr11, vr10, vr09 ;	// vr03 = 000 w09 w08 w07 w06 w05 w04 w03
	vmr2w	vr15, vr14, vr13 ;	// vr03 = 000 w09 w08 w07 w06 w05 w04 w03

	// apply filter & store the output in vr05
	vd6tapf vr04, vr01, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf vr04, vr03, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf vr08, vr05, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf vr08, vr07, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf vr12, vr09, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf vr12, vr11, 0xC ;  // 0 0 0 0 x x 0 0
	vd6tapf vr16, vr13, 3	;  // 0 0 0 0 0 0 x x
	vd6tapf vr16, vr15, 0xC ;  // 0 0 0 0 x x 0 0

	// pack result
	vasrrpwb	vr04, vr04, 0	;	// pack the registers
	vasrrpwb	vr08, vr08, 0	;	// pack the registers
	vasrrpwb	vr12, vr12, 0	;	// pack the registers
	vasrrpwb	vr16, vr16, 0	;	// pack the registers

	// store result
	vst64		vr04,   [i2, 0]	;	// store the result	
	vst64		vr08,   [i2, 24]	;	// store the result	
	vst64		vr12,   [i2, 48]	;	// store the result
	vst64		vr16,   [i2, 72]	;	// store the result
	
	// pack input
	vasrrpwb vr01, vr01, 5	;	//pack input
	vasrrpwb vr05, vr05, 5	;	//pack input
	vasrrpwb vr09, vr09, 5	;	//pack input
	vasrrpwb vr13, vr13, 5	;	//pack input
	
    vmovw   vr23, 8*24, 2   ; Ptr incs
    vmovw   vr23, 4*24, 4+8
	// store input
	vst64	vr01, [i3, 0]		;	//store input					
	vst64	vr05, [i3, 24]	;	//store input					
	vst64	vr09, [i3, 48]	;	//store input					
	vst64	vr13, [i3, 72]	;	//store input

    vaddw   vr00, vr00, vr23    ; inc Ptrs
//-------------------------------------
    vendrec r0              ;

.on	percent_reg

}

_Asm void CPelFilterAndInpRnd_HBlkSz_8(unsigned int inp_sdm_add,
				unsigned int out_sdm_add, unsigned int rounded_inp_sdm_add)
{
	 % reg inp_sdm_add , out_sdm_add , rounded_inp_sdm_add

.off	percent_reg

    ;// load inp and output pointers
    vmovw	vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, out_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00, rounded_inp_sdm_add, 8 ;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_CPelFilterAndInpRnd_HBlkSz_8(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	;// load the input data
	vld128	vr01, [i1, 0]		;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr05, [i1, 16]    ;  // vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	vr06, [i1, 48]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr10, [i1, 64]    ;  // vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	vr11, [i1, 96]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr15, [i1, 112]   ;  // vr02 = 000 000 000 w13 w12 w11 w10 w09
	;// load input data
	vld128	vr16, [i1, 144]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr20, [i1, 160]   ;  // vr02 = 000 000 000 w13 w12 w11 w10 w09

	;// arrange data
	vmr2w	vr02, vr05, vr01 ;	// vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr03, vr05, vr01 ;	// vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr04, vr05, vr01 ;	// vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	// arrange data
	vmr2w	vr07, vr10, vr06 ;	// vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr08, vr10, vr06 ;	// vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr09, vr10, vr06 ;	// vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	// arrange data
	vmr2w	vr12, vr15, vr11 ;	// vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr13, vr15, vr11 ;	// vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr14, vr15, vr11 ;	// vr05 = 000 w13 w12 w11 w10 w09 w08 w07
	// arrange data
	vmr2w	vr17, vr20, vr16 ;	// vr03 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr18, vr20, vr16 ;	// vr04 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr19, vr20, vr16 ;	// vr05 = 000 w13 w12 w11 w10 w09 w08 w07

	// apply filter & store the output in vr06
	vd6tapf vr02, vr02, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr02, vr01, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr02, vr03, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf vr02, vr04, 0xC0; // x x 0 0 0 0 0 0

	// apply filter & store the output in vr06
	vd6tapf vr07, vr07, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr07, vr06, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr07, vr08, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf vr07, vr09, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter & store the output in vr06
	vd6tapf vr12, vr12, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr12, vr11, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr12, vr13, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf vr12, vr14, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter & store the output in vr06
	vd6tapf vr17, vr17, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr17, vr16, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr17, vr18, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf vr17, vr19, 0xC0; // x x 0 0 0 0 0 0 

	// pack & store the result
	vasrrpwb	vr02, vr02, 0	;	// pack the registers
	vasrrpwb	vr07, vr07, 0	;	// pack the registers
	vasrrpwb	vr12, vr12, 0	;	// pack the registers
	vasrrpwb	vr17, vr17, 0	;	// pack the registers

	// pack & store the result
	vst64		vr02,   [i2, 0]	;	// store the result
	vst64		vr07,   [i2, 24]	;	// store the result
	vst64		vr12,   [i2, 48]	;	// store the result
	vst64		vr17,   [i2, 72]	;	// store the result

	// pack and store input
	vasrrpwb vr01, vr01, 5	;	//pack input
	vasrrpwb vr05, vr05, 5	;	//pack input
	vasrrpwb vr06, vr06, 5	;	//pack input
	vasrrpwb vr10, vr10, 5	;	//pack input
	vasrrpwb vr11, vr11, 5	;	//pack input
	vasrrpwb vr15, vr15, 5	;	//pack input
	vasrrpwb vr16, vr16, 5	;	//pack input
	vasrrpwb vr20, vr20, 5	;	//pack input

    vmovw   vr23, 8*24, 2   ; Ptr incs
    vmovw   vr23, 4*24, 4+8

	vst64	vr01, [i3, 0]	;	//store input
	vst64	vr05, [i3, 8]	;	//store input
	vst64	vr06, [i3, 24]	;	//store input
	vst64	vr10, [i3, 32]	;	//store input
	vst64	vr11, [i3, 48]	;	//store input
	vst64	vr15, [i3, 56]	;	//store input
	vst64	vr16, [i3, 72]	;	//store input
	vst64	vr20, [i3, 80]	;	//store input

    vaddw   vr00, vr00, vr23    ; inc Ptrs

//-------------------------------------
    vendrec r0              ;

.on	percent_reg

}

_Asm void CPelFilterAndInpRnd_HBlkSz_16(unsigned int inp_sdm_add,
				unsigned int out_sdm_add, unsigned int rounded_inp_sdm_add)
{
	 % reg inp_sdm_add , out_sdm_add  , rounded_inp_sdm_add 

.off	percent_reg

    ;// load inp and output pointers
    vmovw	vr00, inp_sdm_add, 2		;	// load i1 with r1
    vmovw	vr00, out_sdm_add, 4		;	// load i2 with r2
    vmovw	vr00, rounded_inp_sdm_add, 8 ;	// load i3 with r3

.on	percent_reg
}

_Asm int Macro_CPelFilterAndInpRnd_HBlkSz_16(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load input data  ROW-1
	vld128	vr01, [i1, 0]		;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr02, [i1, 16]    ;  // vr02 = w16 w15 w14 w13 w12 w11 w10 w09
	vld128  vr03, [i1, 32]	;	// vr03 = 000 000 000 w21 w20 w19 w18 w17

	// load input data  ROW-2
	vld128	vr11, [i1, 48]	;  // vr01 = w08 w07 w06 w05 w04 w03 w02 w01
	vld128	vr12, [i1, 64]    ;  // vr02 = w16 w15 w14 w13 w12 w11 w10 w09
	vld128  vr13, [i1, 80]	;	// vr03 = 000 000 000 w21 w20 w19 w18 w17

	// arrange inp: row-1/set-1
	vmr2w	vr04, vr02, vr01 ;	// vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr05, vr02, vr01 ;	// vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr06, vr02, vr01 ;	// vr06 = w14 w13 w12 w11 w10 w09 w08 w07

	// arrange inp: row-1/set-2
	vmr2w	vr07, vr03, vr02 ; // vr07 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr4w   vr08, vr03, vr02 ; // vr08 = w20 w19 w18 w17 w16 w15 w14 w13				
	vmr6w	vr09, vr03, vr02 ; // vr09 = 000 w21 w20 w19 w18 w17 w16 w15

	// apply filter: row-1/set-1
	vd6tapf vr04, vr04, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr04, vr01, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr04, vr05, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf vr04, vr06, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter: row-1/set-2
	vd6tapf vr07, vr07, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr07, vr02, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr07, vr08, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf vr07, vr09, 0xC0; // x x 0 0 0 0 0 0

	// arrange inp: row-2/set-1
	vmr2w	vr14, vr12, vr11 ;	// vr04 = w10 w09 w08 w07 w06 w05 w04 w03
	vmr4w	vr15, vr12, vr11 ;	// vr05 = w12 w11 w10 w09 w08 w07 w06 w05
	vmr6w	vr16, vr12, vr11 ;	// vr06 = w14 w13 w12 w11 w10 w09 w08 w07

	// arrange inp: row-2/set-2
	vmr2w	vr17, vr13, vr12 ; // vr07 = w18 w17 w16 w15 w14 w13 w12 w11
	vmr4w   vr18, vr13, vr12 ; // vr08 = w20 w19 w18 w17 w16 w15 w14 w13				
	vmr6w	vr19, vr13, vr12 ; // vr09 = 000 w21 w20 w19 w18 w17 w16 w15

	// apply filter: row-2/set-1
	vd6tapf vr14, vr14, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr14, vr11, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr14, vr15, 0x30; // 0 0 x x 0 0 0 0 
	vd6tapf vr14, vr16, 0xC0; // x x 0 0 0 0 0 0 
	
	// apply filter: row-2/set-2
	vd6tapf vr17, vr17, 0xC ; // 0 0 0 0 x x 0 0
	vd6tapf vr17, vr12, 0x3	; // 0 0 0 0 0 0 x x
	vd6tapf vr17, vr18, 0x30; // 0 0 x x 0 0 0 0
	vd6tapf vr17, vr19, 0xC0; // x x 0 0 0 0 0 0

	// pack and store the result
	vasrrpwb	vr04, vr04, 0	;	// pack the registers: row-1/set-1
	vasrrpwb	vr07, vr07, 0	;	// pack the registers: row-1/set-2
	vasrrpwb	vr14, vr14, 0	;	// pack the registers: row-1/set-2
	vasrrpwb	vr17, vr17, 0	;	// pack the registers: row-2/set-2

	vst64	vr04,   [i2, 0]	;	// store the result: row-1/set-1
	vst64	vr07,   [i2, 8]	;	// store the result: row-1/set-2
	vst64	vr14,   [i2, 24];	// store the result: row-2/set-2
	vst64	vr17,   [i2, 32];	// store the result: row-2/set-2

	// pack and store input
	vasrrpwb vr01, vr01, 5	;	//pack input
	vasrrpwb vr02, vr02, 5	;	//pack input
	vasrrpwb vr03, vr03, 5	;	//pack input

	vst64	vr01, [i3, 0] ;	//store input
	vst64	vr02, [i3, 8] ;	//store input
	vst64	vr03, [i3, 16] ;	//store input

	// pack and store input
	vasrrpwb vr11, vr11, 5	;	//pack input
	vasrrpwb vr12, vr12, 5	;	//pack input
	vasrrpwb vr13, vr13, 5	;	//pack input

    vmovw   vr23, 4*24, 2   ; Ptr incs
    vmovw   vr23, 2*24, 4+8

	vst64	vr11, [i3, 24] ;	//store input
	vst64	vr12, [i3, 32] ;	//store input
	vst64	vr13, [i3, 40] ;	//store input

    vaddw   vr00, vr00, vr23    ; inc Ptrs

//-------------------------------------
    vendrec r0              ;

.on	percent_reg

}

inline void AuroraH264Dec::CentrePelFilAndInpRounding( SDM_Offset inp_sdm_add,
							SDM_Offset out_sdm_add,
							SDM_Offset rounded_inp_sdm_add )
{
	int rowIndex;

	
	//handle each block size separately
	switch(m_block_hor_size)
	{
	case 4:
        CPelFilterAndInpRnd_HBlkSz_4(inp_sdm_add, out_sdm_add,
                                            rounded_inp_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
            RecMgr.Run(AM_Inter_CPelFilterAndInpRnd_HBlkSz_4);
        }
		break;

	case 8:
        CPelFilterAndInpRnd_HBlkSz_8(inp_sdm_add, out_sdm_add,
                                            rounded_inp_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 4)
    	{
            RecMgr.Run(AM_Inter_CPelFilterAndInpRnd_HBlkSz_8);
        }
		break;

	case 16:
        CPelFilterAndInpRnd_HBlkSz_16(inp_sdm_add, out_sdm_add,
                                            rounded_inp_sdm_add);
    	for(rowIndex=0; rowIndex<m_block_ver_size; rowIndex += 2)
    	{
            RecMgr.Run(AM_Inter_CPelFilterAndInpRnd_HBlkSz_16);
        }
		break;
		
	}

}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordLumaCtrPelFilteringMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the luma reconstruction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLumaCtrPelFilteringMacros()
{
    Record(AM_Inter_CPelFilterWOInpRnd_HBlkSz_4,   Macro_CPelFilterWOInpRnd_HBlkSz_4(NextAddr));
    Record(AM_Inter_CPelFilterWOInpRnd_HBlkSz_8,   Macro_CPelFilterWOInpRnd_HBlkSz_8(NextAddr));
    Record(AM_Inter_CPelFilterWOInpRnd_HBlkSz_16,  Macro_CPelFilterWOInpRnd_HBlkSz_16(NextAddr));
    Record(AM_Inter_CPelFilterAndInpRnd_HBlkSz_4,  Macro_CPelFilterAndInpRnd_HBlkSz_4(NextAddr));
    Record(AM_Inter_CPelFilterAndInpRnd_HBlkSz_8,  Macro_CPelFilterAndInpRnd_HBlkSz_8(NextAddr));
    Record(AM_Inter_CPelFilterAndInpRnd_HBlkSz_16, Macro_CPelFilterAndInpRnd_HBlkSz_16(NextAddr));

}

#endif //_AURORA_H264_CTRPEL_FILTERING_H_
