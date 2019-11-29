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

#if !defined(_AURORA_H264_VER_FILTERING_H_)
#define _AURORA_H264_VER_FILTERING_H_

/* ------------------------------------------------------------------------ */
/* FUNCTION:	PerformVerticalFiltering                                    */
/* DESCRIPTION:							                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */

_Asm void Luma8x8VerFilterAndRound(unsigned int inp_sdm_add,
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

_Asm int Macro_Luma8x8VerFilterAndRound(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	/* applies vertical filtering to buffer pointed to by i1 and stores
        result at i2 */
	// load the input 13 rows at offset of 24 bytes
	vld64w	vr01, [i1,   0]	;
	vld64w	vr02, [i1,  24]	;
	vld64w	vr03, [i1,  48]	;
	vld64w	vr04, [i1,  72]	;
	vld64w	vr05, [i1,  96]	;
	vld64w	vr06, [i1, 120]	;
	vld64w	vr07, [i1, 144]	;
	vld64w	vr08, [i1, 168]	;
	vld64w	vr09, [i1, 192]	;
	vld64w	vr10, [i1, 216]	;
	vld64w	vr11, [i1, 240]	;
	vld64w	vr12, [i1, 264]	;
	vld64w	vr13, [i1, 288]	;

	// calculate first output
	vbmulw	vr14, vr02, (-5)		; //acc += vr02*(-5)
	vbmulaw vr14, vr03, (20)		; //acc += vr03*(20)
	vbmulaw vr14, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr14, vr05, (-5)		; //acc += vr05*(-5)
	vaddaw	vr14, vr01, vr06	;

	// calculate second output
	vbmulw	vr15, vr03, (-5)		; //acc = vr03*(-5)
	vbmulaw vr15, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr15, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr15, vr06, (-5)		; //acc += vr06*(-5)
	vaddaw	vr15, vr02, vr07	;
 
	vasrrpwb	vr14, vr14, 5	; //pack the 1st output

	// calculate third output
	vbmulw  vr16, vr04, (-5)		; //acc += vr04*(-5)
	vbmulaw vr16, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr16, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr16, vr07, (-5)		; //acc += vr07*(-5)
	vaddaw	vr16, vr03, vr08	;

	vst64		vr14, [i2, 0]	; //store the first result
	vasrrpwb	vr15, vr15, 5	; //pack the second output


	// calculate fourth output
	vbmulw	vr17, vr05, (-5)		; //acc  = vr05*(-5)
	vbmulaw vr17, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr17, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr17, vr08, (-5)		; //acc += vr08*(-5)
	vaddaw	vr17, vr04, vr09	;

	vst64		vr15, [i2, 24]	; //store the second result
	vasrrpwb	vr16, vr16, 5	; //pack the third output

	// calculate the 5th output
	vbmulw	vr18, vr06, (-5)		; //acc  = vr06*(-5)
	vbmulaw vr18, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr18, vr08, (20)		; //acc += vr08*(20)
	vbmulaw vr18, vr09, (-5)		; //acc += vr09*(-5)
	vaddaw	vr18, vr05, vr10	; //acc += vr05+vr10

	vst64	vr16, [i2,48]		; // store the 3rd result
	vasrrpwb	vr17, vr17, 5	; // pack the 4th output


	// calculate the 6th output
	vbmulw	vr19, vr07, (-5)		; //acc  = vr07*(-5)
	vbmulaw vr19, vr08, (20)		; //acc += vr08*(20)
	vbmulaw vr19, vr09, (20)		; //acc += vr09*(20)
	vbmulaw vr19, vr10, (-5)		; //acc += vr10*(-5)
	vaddaw	vr19, vr06, vr11	; //acc += vr06+vr11

	vst64	vr17, [i2, 72]		; // store the 4th result
	vasrrpwb	vr18, vr18, 5	; // pack the 5th output

	// calculate the 7th output
	vbmulw	vr20, vr08, (-5)		; //acc  = vr08*(-5)
	vbmulaw vr20, vr09, (20)		; //acc += vr09*(20)
	vbmulaw vr20, vr10, (20)		; //acc += vr10*(20)
	vbmulaw vr20, vr11, (-5)		; //acc += vr11*(-5)
	vaddaw	vr20, vr07, vr12	; //acc += vr07+vr12

	vst64	vr18, [i2, 96]		; // store the 5th result
	vasrrpwb	vr19, vr19, 5	; // pack the 6th output

	// calculate the 8th output
	vbmulw	vr21, vr09, (-5)		; //acc  = vr08*(-5)
	vbmulaw vr21, vr10, (20)		; //acc += vr09*(20)
	vbmulaw vr21, vr11, (20)		; //acc += vr10*(20)
	vbmulaw vr21, vr12, (-5)		; //acc += vr11*(-5)
	vaddaw	vr21, vr08, vr13	; //acc += vr07+vr12

	vst64	vr19, [i2, 120]		; // store the 6th result

	vasrrpwb	vr20, vr20, 5	; // pack the 7th output
	vasrrpwb	vr21, vr21, 5	; // pack the 8th output

	vst64	vr20, [i2, 144]		; // store the 7th result
	vst64	vr21, [i2, 168]		; // store the 8th result

    vbaddw      vr00,vr00,8
//-------------------------------------
    vendrec r0              ;
 
.on	percent_reg
}

_Asm void Luma4x8VerFilterAndRound(unsigned int inp_sdm_add,
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

_Asm int Macro_Luma4x8VerFilterAndRound(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------



	// load the input 
	vld64w	vr01, [i1,   0]	;
	vld64w	vr02, [i1,  24]	;
	vld64w	vr03, [i1,  48]	;
	vld64w	vr04, [i1,  72]	;
	vld64w	vr05, [i1,  96]	;
	vld64w	vr06, [i1, 120]	;
	vld64w	vr07, [i1, 144]	;
	vld64w	vr08, [i1, 168]	;
	vld64w	vr09, [i1, 192]	;
	
	// calculate first output
	vaddw	vr11, vr01, vr06	;
	vbmulaw vr11, vr02, (-5)	; //acc += vr02*(-5)
	vbmulaw vr11, vr03, (20)		; //acc += vr03*(20)
	vbmulaw vr11, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr11, vr05, (-5)	; //acc += vr05*(-5)
	

	// calculate second output
	vaddw	vr12, vr02, vr07	;
	vbmulaw vr12, vr03, (-5)	; //acc += vr03*(-5)
	vbmulaw vr12, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr12, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr12, vr06, (-5)	; //acc += vr06*(-5)

	vasrrpwb	vr11, vr11, 5	; // pack the first output

	// calculate third output
	vaddw	vr13, vr03, vr08	;
	vbmulaw vr13, vr04, (-5)	; //acc += vr04*(-5)
	vbmulaw vr13, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr13, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr13, vr07, (-5)	; //acc += vr07*(-5)

	
	vst64		vr11, [i2, 0]	; //store the first result
	vasrrpwb	vr12, vr12, 5	; //pack the second output

	// calculate fourth output
	vaddw	vr14, vr04, vr09	;
	vbmulaw vr14, vr05, (-5)		; //acc += vr05*(-5)
	vbmulaw vr14, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr14, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr14, vr08, (-5)		; //acc += vr08*(-5)

	
	vst64		vr12, [i2, 24]	; //store the second result
	vasrrpwb	vr13, vr13, 5	; //pack the third output

	
	vasrrpwb	vr14, vr14, 5	; //pack the fourth output
	vst64		vr13, [i2, 48]	; //store the 3rd result

	//store the 4th result
	vst64		vr14, [i2, 72]	;

    vbaddw      vr00,vr00,8
//-------------------------------------

    vendrec r0              ;

.on	percent_reg

}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	VerticalFilteringAndRounding                                */
/* DESCRIPTION:							                                    */
/* RETURNS:		Pointer to a  buffer                                        */
/* ------------------------------------------------------------------------ */
inline void AuroraH264Dec::VerticalFilteringAndRounding(SDM_Offset inp_ptr, 
							  SDM_Offset out_ptr,
							  int horSize,
							  int horOffset )
{
	int colGroups;
	int colGrpIndx;

	colGroups = (horSize + horOffset + 7)>> 3;

	// club the filter operation for all rows together
	switch(m_block_ver_size)
	{
	case 4:
        Luma4x8VerFilterAndRound(inp_ptr, out_ptr);
    	for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
    	{
            RecMgr.Run(AM_Inter_Luma4x8VerFilterAndRound);    
        }
		break;

	case 8:
       Luma8x8VerFilterAndRound(inp_ptr, out_ptr);
    	for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
    	{
            RecMgr.Run(AM_Inter_Luma8x8VerFilterAndRound);    
        }
		break;

	case 16:
        Luma8x8VerFilterAndRound(inp_ptr, out_ptr);
        for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
        {
            RecMgr.Run(AM_Inter_Luma8x8VerFilterAndRound);    
        }
        Luma8x8VerFilterAndRound(inp_ptr+8*24, out_ptr+8*24);
        for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
        {
            RecMgr.Run(AM_Inter_Luma8x8VerFilterAndRound);    
        }
		break;
	} // end -switch( hor_block_size)
}

_Asm void Luma8x8VerFilterWORounding(unsigned int inp_sdm_add,
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

_Asm int Macro_Luma8x8VerFilterWORounding(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	/* applies vertical filtering to buffer pointed to by i1 and stores
        result at i2 */
	// load the input 13 rows at offset of 24 bytes
	vld64w	vr01, [i1,   0]	;
	vld64w	vr02, [i1,  24]	;
	vld64w	vr03, [i1,  48]	;
	vld64w	vr04, [i1,  72]	;
	vld64w	vr05, [i1,  96]	;
	vld64w	vr06, [i1, 120]	;
	vld64w	vr07, [i1, 144]	;
	vld64w	vr08, [i1, 168]	;
	vld64w	vr09, [i1, 192]	;
	vld64w	vr10, [i1, 216]	;
	vld64w	vr11, [i1, 240]	;
	vld64w	vr12, [i1, 264]	;
	vld64w	vr13, [i1, 288]	;

	// calculate first output
	vbmulw	vr14, vr02, (-5)		; //acc += vr02*(-5)
	vbmulaw vr14, vr03, (20)		; //acc += vr03*(20)
	vbmulaw vr14, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr14, vr05, (-5)		; //acc += vr05*(-5)
	vaddaw	vr14, vr01, vr06	;

	// calculate second output
	vbmulw	vr15, vr03, (-5)		; //acc = vr03*(-5)
	vbmulaw vr15, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr15, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr15, vr06, (-5)		; //acc += vr06*(-5)
	vaddaw	vr15, vr02, vr07	;

	vst128	vr14, [i2, 0]	; //store the first result

	// calculate third output
	vbmulw  vr16, vr04, (-5)		; //acc += vr04*(-5)
	vbmulaw vr16, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr16, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr16, vr07, (-5)		; //acc += vr07*(-5)
	vaddaw	vr16, vr03, vr08	;

	vst128	vr15, [i2,	48]	; //store the second result

	// calculate fourth output
	vbmulw	vr17, vr05, (-5)		; //acc  = vr05*(-5)
	vbmulaw vr17, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr17, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr17, vr08, (-5)		; //acc += vr08*(-5)
	vaddaw	vr17, vr04, vr09	;

	vst128	vr16, [i2,	96]		; // store the 3rd result

	// calculate the 5th output
	vbmulw	vr18, vr06, (-5)		; //acc  = vr06*(-5)
	vbmulaw vr18, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr18, vr08, (20)		; //acc += vr08*(20)
	vbmulaw vr18, vr09, (-5)		; //acc += vr09*(-5)
	vaddaw	vr18, vr05, vr10	; //acc += vr05+vr10

	vst128	vr17, [i2, 144]		; // store the 4th result

	// calculate the 6th output
	vbmulw	vr19, vr07, (-5)		; //acc  = vr07*(-5)
	vbmulaw vr19, vr08, (20)		; //acc += vr08*(20)
	vbmulaw vr19, vr09, (20)		; //acc += vr09*(20)
	vbmulaw vr19, vr10, (-5)		; //acc += vr10*(-5)
	vaddaw	vr19, vr06, vr11	; //acc += vr06+vr11

	vst128	vr18, [i2, 192]		; // store the 5th result

	// calculate the 7th output
	vbmulw	vr20, vr08, (-5)		; //acc  = vr08*(-5)
	vbmulaw vr20, vr09, (20)		; //acc += vr09*(20)
	vbmulaw vr20, vr10, (20)		; //acc += vr10*(20)
	vbmulaw vr20, vr11, (-5)		; //acc += vr11*(-5)
	vaddaw	vr20, vr07, vr12	; //acc += vr07+vr12

	vst128	vr19, [i2, 240]		; // store the 6th result
    
    vmovw   vr23, 8, 2              ; ptr incs
    vmovw   vr23, 16, 4

	// calculate the 8th output
	vbmulw	vr21, vr09, (-5)		; //acc  = vr08*(-5)
	vbmulaw vr21, vr10, (20)		; //acc += vr09*(20)
	vbmulaw vr21, vr11, (20)		; //acc += vr10*(20)
	vbmulaw vr21, vr12, (-5)		; //acc += vr11*(-5)
	vaddaw	vr21, vr08, vr13	; //acc += vr07+vr12

	vst128	vr20, [i2, 288]		; // store the 7th result
	vst128	vr21, [i2, 336]		; // store the 8th result

    vaddw   vr00, vr00, vr23    ; inc ptrs
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

_Asm void Luma4x8VerFilterWORounding(unsigned int inp_sdm_add,
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

_Asm int Macro_Luma4x8VerFilterWORounding(int scm_addr)
{
    %reg        scm_addr
.off	percent_reg

    vrec        scm_addr
//-------------------------------------


	// load the input 
	vld64w	vr01, [i1,   0]	;
	vld64w	vr02, [i1,  24]	;
	vld64w	vr03, [i1,  48]	;
	vld64w	vr04, [i1,  72]	;
	vld64w	vr05, [i1,  96]	;
	vld64w	vr06, [i1, 120]	;
	vld64w	vr07, [i1, 144]	;
	vld64w	vr08, [i1, 168]	;
	vld64w	vr09, [i1, 192]	;
	
	// calculate first output
	vaddw	vr11, vr01, vr06	;
	vbmulaw vr11, vr02, (-5)		; //acc += vr02*(-5)
	vbmulaw vr11, vr03, (20)		; //acc += vr03*(20)
	vbmulaw vr11, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr11, vr05, (-5)		; //acc += vr05*(-5)

	vst128	vr11,  [i2, 0]	; //store the first result


	// calculate second output
	vaddw	vr12, vr02, vr07	;
	vbmulaw vr12, vr03, (-5)		; //acc += vr03*(-5)
	vbmulaw vr12, vr04, (20)		; //acc += vr04*(20)
	vbmulaw vr12, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr12, vr06, (-5)		; //acc += vr06*(-5)

	vst128		vr12, [i2,	48]	; //store the second result

	// calculate third output
	vaddw	vr13, vr03, vr08	;
	vbmulaw vr13, vr04, (-5)		; //acc += vr04*(-5)
	vbmulaw vr13, vr05, (20)		; //acc += vr05*(20)
	vbmulaw vr13, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr13, vr07, (-5)		; //acc += vr07*(-5)

	vst128		vr13,   [i2,	96]	; //store the 3rd result

    vmovw   vr23, 8, 2              ; ptr incs
    vmovw   vr23, 16, 4
    
	// calculate fourth output
	vaddw	vr14, vr04, vr09	;
	vbmulaw vr14, vr05, (-5)		; //acc += vr05*(-5)
	vbmulaw vr14, vr06, (20)		; //acc += vr06*(20)
	vbmulaw vr14, vr07, (20)		; //acc += vr07*(20)
	vbmulaw vr14, vr08, (-5)		; //acc += vr08*(-5)

	vst128		vr14,   [i2,	144]	; //store the 4th result

    vaddw   vr00, vr00, vr23        ; inc ptrs
//-------------------------------------
    vendrec r0              ;

.on	percent_reg
}

inline void AuroraH264Dec::VerticalFilteringWORounding(SDM_Offset inp_ptr, 
							  SDM_Offset out_ptr,
							  int horSize,
							  int horOffset )
{
	int colGroups;
	int colGrpIndx;
	unsigned int inp_sdm_add;
	unsigned int out_sdm_add;


	colGroups = (horSize + horOffset + 7)>> 3;

	// club the filter operation for all rows together
	switch(m_block_ver_size)
	{
	case 4:
        Luma4x8VerFilterWORounding(inp_ptr, out_ptr);
    	for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
    	{
            RecMgr.Run(AM_Inter_Luma4x8VerFilterWORounding);    
        }
		break;

	case 8:
    	for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
    	{
    		inp_sdm_add = inp_ptr + 8*colGrpIndx;
    		out_sdm_add = out_ptr + (8*2*colGrpIndx);

			Luma8x8VerFilterWORounding(inp_sdm_add, out_sdm_add);
            RecMgr.Run(AM_Inter_Luma8x8VerFilterWORounding);    
        }
		break;

	case 16:
        Luma8x8VerFilterWORounding(inp_ptr, out_ptr);
        for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
        {
            RecMgr.Run(AM_Inter_Luma8x8VerFilterWORounding);    
        }
        Luma8x8VerFilterWORounding(inp_ptr+24*8, out_ptr+48*8);
        for( colGrpIndx = 0; colGrpIndx<colGroups; colGrpIndx++)
        {
            RecMgr.Run(AM_Inter_Luma8x8VerFilterWORounding);    
        }
		break;
	} // end - switch( hor_block_size)
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RecordChromaInterPredMacros                 	                        */
/* INP-PARAMS:  void                                                        */
/* RETURNS:		void                                                        */
/* DESCRIPTION:	Records the Chroma Inter prediction macros                                */
/* ------------------------------------------------------------------------ */
void AuroraRecMgr::RecordLumaInterPredVerFilterMacros()
{
    Record(AM_Inter_Luma8x8VerFilterAndRound, Macro_Luma8x8VerFilterAndRound(NextAddr));
    Record(AM_Inter_Luma4x8VerFilterAndRound, Macro_Luma4x8VerFilterAndRound(NextAddr));
    Record(AM_Inter_Luma8x8VerFilterWORounding, Macro_Luma8x8VerFilterWORounding(NextAddr));
    Record(AM_Inter_Luma4x8VerFilterWORounding, Macro_Luma4x8VerFilterWORounding(NextAddr));
}
#endif	//_AURORA_H264_VER_FILTERING_H_
