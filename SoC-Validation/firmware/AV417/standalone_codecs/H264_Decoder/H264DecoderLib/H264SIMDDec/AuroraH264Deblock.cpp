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

#ifdef _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif


#include "ARCMedia.h"
#include "AuroraH264Dec.h"

#include "H264Tables.h"

#if defined __ARC_MPLAYER__
extern "C" {
#include "memctl.h"
}
#endif

// DEBLOCK functions
#ifdef AURORA_DEBLOCK


/* Transpose an 8x8 matrix in vector registers R1 to R8 */
#define TRANSPOSE(VR1,VR2,VR3,VR4,VR5,VR6,VR7,VR8) \
  vexch1  %vr##VR1,%vr##VR2 ` \
  vexch1  %vr##VR3,%vr##VR4 ` \
  vexch1  %vr##VR5,%vr##VR6 ` \
  vexch1  %vr##VR7,%vr##VR8 ` \
  vexch2  %vr##VR1,%vr##VR3 ` \
  vexch2  %vr##VR2,%vr##VR4 ` \
  vexch2  %vr##VR5,%vr##VR7 ` \
  vexch2  %vr##VR6,%vr##VR8 ` \
  vexch4  %vr##VR1,%vr##VR5 ` \
  vexch4  %vr##VR2,%vr##VR6 ` \
  vexch4  %vr##VR3,%vr##VR7 ` \
  vexch4  %vr##VR4,%vr##VR8


/*


; H264 deblocking
; ---- ----------

; NOT FULLY OPTIMIZED

; Two cases of memory alignment for an edge
; On 4 or 8 pixel alignment

; Macro block buffer (Luma)
;  0   4   8               24
; 0xxxxxxxx................	p3
;  xxxxxxxx................	p2
;  xxxxxxxx................	p1
;  xxxxxxxx................	p0
; 4xxxx....****************	q0<		* Macro block edge
;  xxxx....*...+...+...+...	q1		+ Internal edge
;  xxxx....*...+...+...+...	q2		. pixels
;  xxxx....*...+...+...+...	q3		x unknown
;  xxxx....*+++++++++++++++
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;  xxxx....*+++++++++++++++
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;  xxxx....*+++++++++++++++
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;  xxxx....*...+...+...+...
;24       1st 2nd 3rd 4th
;          A       A
;          |       |
;              ppppqqqq
;              32100123
; (8 pixel aligned edges)

; Macro block buffer (Chroma)
;
;;  0   4   8       16
;  0xxxxxxxx........
;   xxxxxxxx........
;  2xxxxxx..********
;   xxxxxx..*...+...
;   xxxxxx..*...+...
;   xxxxxx..*...+...
;   xxxxxx..*+++++++
;   xxxxxx..*...+...
;   xxxxxx..*...+...
;   xxxxxx..*...+...
;           A   A
;           |   |
;         ppqqppqq
;         10011001
*/
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
// Move overlap area for next block
_Asm int MACRO_MoveForNextBlockL( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
    vld32   %vr01,[%i0,20+0 ]
    vld32   %vr02,[%i0,20+24]
    vld32   %vr03,[%i0,20+48]
    vld32   %vr04,[%i0,20+72]
    vst32   %vr01,[%i0,4+0 ]
    vst32   %vr02,[%i0,4+24]
    vst32   %vr03,[%i0,4+48]
    vst32   %vr04,[%i0,4+72]

    vld32   %vr01,[%i0,20+0 +96]
    vld32   %vr02,[%i0,20+24+96]
    vld32   %vr03,[%i0,20+48+96]
    vld32   %vr04,[%i0,20+72+96]
    vst32   %vr01,[%i0,4+0  +96]
    vst32   %vr02,[%i0,4+24 +96]
    vst32   %vr03,[%i0,4+48 +96]
    vst32   %vr04,[%i0,4+72 +96]

    vld32   %vr01,[%i0,20   +192]
    vld32   %vr02,[%i0,20+24+192]
    vld32   %vr03,[%i0,20+48+192]
    vld32   %vr04,[%i0,20+72+192]
    vst32   %vr01,[%i0,4   +192]
    vst32   %vr02,[%i0,4+24+192]
    vst32   %vr03,[%i0,4+48+192]
    vst32   %vr04,[%i0,4+72+192]

    vld32   %vr01,[%i0,20   +288]
    vld32   %vr02,[%i0,20+24+288]
    vld32   %vr03,[%i0,20+48+288]
    vld32   %vr04,[%i0,20+72+288]
    vst32   %vr01,[%i0,4   +288]
    vst32   %vr02,[%i0,4+24+288]
    vst32   %vr03,[%i0,4+48+288]
    vst32   %vr04,[%i0,4+72+288]

    vld32   %vr01,[%i0,20   +384]
    vld32   %vr02,[%i0,20+24+384]
    vld32   %vr03,[%i0,20+48+384]
//    vld32   %vr04,[%i0,20+72+384]
    vst32   %vr01,[%i0,4   +384]
    vst32   %vr02,[%i0,4+24+384]
    vst32   %vr03,[%i0,4+48+384]
//    vst32   %vr04,[%i0,4+72+384]

    vendrec %r0
}

_Asm int MACRO_MoveForNextBlockU( int addr)
{
    %reg    addr
    vrec    addr

// Move chroma u data  ptr in %i1
    vld32   %vr01,[%i1,12]
    vld32   %vr02,[%i1,12+16]
    vld32   %vr03,[%i1,12+32]
    vld32   %vr04,[%i1,12+48]
    vst32   %vr01,[%i1,4]
    vst32   %vr02,[%i1,4+16]
    vst32   %vr03,[%i1,4+32]
    vst32   %vr04,[%i1,4+48]
    
    vld32   %vr01,[%i1,12+64]
    vld32   %vr02,[%i1,12+16+64]
    vld32   %vr03,[%i1,12+32+64]
    vld32   %vr04,[%i1,12+48+64]
    vst32   %vr01,[%i1,4+64]
    vst32   %vr02,[%i1,4+16+64]
    vst32   %vr03,[%i1,4+32+64]
    vst32   %vr04,[%i1,4+48+64]

    vld32   %vr01,[%i1,12+128]
    vst32   %vr01,[%i1,4+128]

    vendrec %r0
}

_Asm int MACRO_MoveForNextBlockV( int addr)
{
    %reg    addr
    vrec    addr

// Move chroma v data  ptr in %i2
    vld32   %vr01,[%i2,12]
    vld32   %vr02,[%i2,12+16]
    vld32   %vr03,[%i2,12+32]
    vld32   %vr04,[%i2,12+48]
    vst32   %vr01,[%i2,4]
    vst32   %vr02,[%i2,4+16]
    vst32   %vr03,[%i2,4+32]
    vst32   %vr04,[%i2,4+48]
    
    vld32   %vr01,[%i2,12+64]
    vld32   %vr02,[%i2,12+16+64]
    vld32   %vr03,[%i2,12+32+64]
    vld32   %vr04,[%i2,12+48+64]
    vst32   %vr01,[%i2,4+64]
    vst32   %vr02,[%i2,4+16+64]
    vst32   %vr03,[%i2,4+32+64]
    vst32   %vr04,[%i2,4+48+64]

    vld32   %vr01,[%i2,12+128]
    vst32   %vr01,[%i2,4+128]

    vendrec %r0
}
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
_Asm void ASMSetupDeblockDMA( int frameTabAddr)
{
    %reg frameTabAddr
// For DMA in
//        vdiwr           %dr0,SDM_d                                 ;//    SDM destination addr
//        vdiwr           %dr1,24                                   ;//    SDM Stride always 24
//        vdiwr           %dr2,0x001414                             ;//    idx 0, blk_ver_size<<8 || block_hor_size
        vdiwr           %dr6,2                                    ;//    Clip mode - NON INTERLACE
        vdiwr           %dr7, frameTabAddr                         ;//    SDM frame table address

// For DMA out
//        vdowr           %dr1,24                                   ;//    SDM Stride always 24
//        vdowr           %dr6,2                                    ;//    Clip mode - NON INTERLACE
        vdowr           %dr7, frameTabAddr                           ;//    SDM frame table address
} 



// Setup frame table 0 for 20 x 20 DMA from referance pitctue
void AuroraH264Dec::SetupDeblockDMA(unsigned char * pic_y, unsigned char * pic_u, unsigned char * pic_v, int picWidth, int picHeight)
{
	unsigned int 	sdm_frame_table_add = m_dma_frames;
        unsigned int ref_pic_phy_add;

// Luma
#if defined(__ARC_MPLAYER__)
        ref_pic_phy_add = h264_get_physical_adrs( (unsigned int) pic_y);
#else
        ref_pic_phy_add = (unsigned int) pic_y;
#endif

	SetDMAFrame(sdm_frame_table_add /* frame table offset from sdm-base*/,
				ref_pic_phy_add /* inp add in ARC memory */,
				picWidth	/* Stride */,
				picWidth	/* xDim */,
				picHeight	/* yDim */);
// Chroma u
#if defined(__ARC_MPLAYER__)
        ref_pic_phy_add = h264_get_physical_adrs( (unsigned int) pic_u);
#else
        ref_pic_phy_add = (unsigned int) pic_u;
#endif

        SetDMAFrame(sdm_frame_table_add+16 /* frame table offset from sdm-base*/,
                                ref_pic_phy_add /* inp add in ARC memory */,
                                picWidth/2        /* Stride */,
                                picWidth/2        /* xDim */,
                                picHeight/2       /* yDim */);

// Chroma v
#if defined(__ARC_MPLAYER__)
        ref_pic_phy_add = h264_get_physical_adrs( (unsigned int) pic_v);
#else
        ref_pic_phy_add = (unsigned int) pic_v;
#endif

        SetDMAFrame(sdm_frame_table_add+32 /* frame table offset from sdm-base*/,
                                ref_pic_phy_add /* inp add in ARC memory */,
                                picWidth/2        /* Stride */,
                                picWidth/2        /* xDim */,
                                picHeight/2       /* yDim */);


        ASMSetupDeblockDMA(sdm_frame_table_add>>9);
}

#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec2")
#endif
_Asm int MACRO_VerticalFilter2(int addr)
{
        %reg    addr
        vrec    addr
//------
//; VR07	-	p3
//; VR08	-	p2
//; VR09	-	p1
//; VR10	-	p0
//; VR11	-	q0
//; VR12	-	q1
//; VR13	-	q2
//; VR14	-	q3

        vld64w  %vr04,[%i1,0]           ;// p3
        vld64w  %vr05,[%i1,24]          ;// p2
        vld64w  %vr06,[%i1,48]          ;// p1
        vld64w  %vr07,[%i1,72]          ;// p0
        vld64w  %vr08,[%i1,96]          ;// q0
        vld64w  %vr09,[%i1,120]         ;// q1
        vld64w  %vr10,[%i1,144]         ;// q2
        vld64w  %vr11,[%i1,168]         ;// q3

		TRANSPOSE(04,05,06,07,08,09,10,11);

        vh264ft %vr12, %vr04, %vr01             ;//
        vh264ft %vr13, %vr05, %vr01             ;//
        vh264ft %vr14, %vr06, %vr01             ;//
        vh264ft %vr15, %vr07, %vr01             ;//

        vh264ft %vr16, %vr08, %vr01             ;//
        vh264ft %vr17, %vr09, %vr01             ;//
        vh264ft %vr18, %vr10, %vr01             ;//
        vh264ft %vr19, %vr11, %vr01             ;//

        vh264fw %vr04, %vr04, %vr12             ;//
        vh264fw %vr05, %vr05, %vr13             ;//
        vh264fw %vr06, %vr06, %vr14             ;//
        vh264fw %vr07, %vr07, %vr15             ;//

        vh264fw %vr08, %vr08, %vr16             ;//
        vh264fw %vr09, %vr09, %vr17             ;//
        vh264fw %vr10, %vr10, %vr18             ;//
        vh264fw %vr11, %vr11, %vr19             ;//

		TRANSPOSE(04,05,06,07,08,09,10,11);

		vasrpwb	%vr05, %vr05, 0		;//	p2	Pack to bytes
		vasrpwb	%vr06, %vr06, 0		;//	p1
		vasrpwb	%vr07, %vr07, 0		;//	p0
		vasrpwb	%vr08, %vr08, 0		;//	q0
		vasrpwb	%vr09, %vr09, 0		;//	q1
		vasrpwb	%vr10, %vr10, 0		;//	q2

        vst64   %vr05,[%i1,24]          ;// p2
        vst64   %vr06,[%i1,48]          ;// p1
        vst64   %vr07,[%i1,72]          ;// p0
        vst64   %vr08,[%i1,96]          ;// q0
        vst64   %vr09,[%i1,120]         ;// q1
        vst64   %vr10,[%i1,144]         ;// q2

        vbaddw  %vr00,%vr00,8           // Next 2 edges along

    	vendrec %r0

}


// Now does 2 edges at a time
_Asm int MACRO_StrongVerticalFilter(int addr)
{
        %reg    addr
        vrec    addr
//------
//; VR07	-	p3
//; VR08	-	p2
//; VR09	-	p1
//; VR10	-	p0
//; VR11	-	q0
//; VR12	-	q1
//; VR13	-	q2
//; VR14	-	q3

        vld64w  %vr07,[%i1,0]           ;// p3
        vld64w  %vr08,[%i1,24]          ;// p2
        vld64w  %vr09,[%i1,48]          ;// p1
        vld64w  %vr10,[%i1,72]          ;// p0
        vld64w  %vr11,[%i1,96]          ;// q0
        vld64w  %vr12,[%i1,120]         ;// q1
        vld64w  %vr13,[%i1,144]         ;// q2
        vld64w  %vr14,[%i1,168]         ;// q3
        
	vdifw	%vr20, %vr10, %vr11	;//	| p0-q0 |			for enable
	vdifw	%vr21, %vr09, %vr10	;//	| p1-p0 |			..
	vdifw	%vr22, %vr11, %vr12	;//	| q0-q1 |			..
	
	vasrw	%vr23, %vr01, 2		;//	(alpha >> 2)		for Sg
	vbaddw	%vr23, %vr23, 2		;//	(alpha >> 2) + 2	for Sg
	vdifw	%vr04, %vr08, %vr10	;//	| p2-p0 |			for ApD
	vdifw	%vr05, %vr13, %vr11	;//	| q2-q0 |			for AqD
	
	vltw	%vr06, %vr20, %vr01	;//	| p0-q0 | < alpha	for enable
	vltw	%vr17, %vr21, %vr02	;//	| p1-p0 | < beta	..
	vltw	%vr18, %vr22, %vr02	;//	| q0-q1 | < beta	..
	vltw	%vr04, %vr04, %vr02	;//	ApD
	vltw	%vr05, %vr05, %vr02	;//	AqD

	vand	%vr06, %vr06, %vr17	;//						for enable

	vaddw	%vr15, %vr08, %vr12	;//	 p2 + q1			for P0 ApD&Sg
	vaddaw	%vr15, %vr09, %vr09	;//	+p1 + p1
	vaddaw	%vr15, %vr10, %vr10	;//	+p0 + p0
	vaddaw	%vr15, %vr11, %vr11	;//	+q0 + q0

	vltw	%vr23, %vr20, %vr23	;//	Sg

	vand	%vr06, %vr06, %vr18	;//	Enable

	vaddw	%vr16, %vr08, %vr09	;//	 p2 + p1			for P1 ApD&Sg
	vaddaw	%vr16, %vr10, %vr11	;//	+p0 + q0

	vaddw	%vr17, %vr08, %vr09	;//	 p2 + p1			for P2 ApD&Sg
	vaddaw	%vr17, %vr10, %vr11	;//	+p0 + q0
	vaddaw	%vr17, %vr07, %vr07	;//	+p3 + p3
	vaddaw	%vr17, %vr08, %vr08	;//	+p2 + p2

	vand	%vr04, %vr04, %vr23	;//	ApD&Sg
	vand	%vr05, %vr05, %vr23	;//	AqD&Sg

	vaddw	%vr18, %vr10, %vr12	;//	 p0 + q1			for P0_NOT_ApD&Sg
	vaddaw	%vr18, %vr09, %vr09	;//	+p1 + p1

	vaddw	%vr19, %vr09, %vr13	;//	 p1 + q2			for Q0 AqD&Sg
	vaddaw	%vr19, %vr10, %vr10	;//	+p0 + p0
	vaddaw	%vr19, %vr11, %vr11	;//	+q0	+ q0
	vaddaw	%vr19, %vr12, %vr12	;//	+q1 + q1

	vbic	%vr23, %vr06, %vr04	;//	Enable & NOT ApD&Sg
	vbic	%vr07, %vr06, %vr05	;//	Enable & NOT AqD&Sg


	vand	%vr04, %vr06, %vr04	;//	Enable & ApD&Sg
	vand	%vr05, %vr06, %vr05	;//	Enable & AqD&Sg

	vaddw	%vr20, %vr10, %vr11	;//	 p0 + q0			for Q1 AqD&Sg
	vaddaw	%vr20, %vr12, %vr13	;//	+q1 + q2

	vaddw	%vr21, %vr13, %vr12	;//	 q2 + q1			for Q2 AqD&Sg
	vaddaw	%vr21, %vr11, %vr10	;//	+q0 + p0
	vaddaw	%vr21, %vr14, %vr14	;//	+q3 + q3
	vaddaw	%vr21, %vr13, %vr13	;//	+q2 + q2

	vaddw	%vr22, %vr11, %vr09	;//	 q0 + p1			for Q0_NOT_AqD&Sg
	vaddaw	%vr22, %vr12, %vr12	;//	+q1 + q1

	vasrrw	%vr15, %vr15, 3		;//	P0 ApD&Sg
	vasrrw	%vr16, %vr16, 2		;//	P1 ApD&Sg
	vasrrw	%vr17, %vr17, 3		;//	P2 ApD&Sg
	vasrrw	%vr18, %vr18, 2		;//	P0_NOT_ApD&Sg

	vasrrw	%vr19, %vr19, 3		;//	Q0 AqD&Sg
	vasrrw	%vr20, %vr20, 2		;//	Q1 AqD&Sg
	vasrrw	%vr21, %vr21, 3		;//	Q2 AqD&Sg
	vasrrw	%vr22, %vr22, 2		;//	Q0_NOT_AqD&Sg

//	vsubw %vr17,%vr17,%vr17);
	vbic	%vr08, %vr08, %vr04	;//	 p2 & !(Enable & ApD&Sg)	for p2
	vandaw	%vr08, %vr17, %vr04	;//	+P2 & Enable & ApD&Sg

	vbic	%vr09, %vr09, %vr04	;//	 p1 & !Enable		for p1
	vandaw	%vr09, %vr16, %vr04	;//	+P1 & Enable & ApD&Sg

	vbic	%vr10, %vr10, %vr06	;//	 p0 & !Enable		for p0
	vandaw	%vr10, %vr15, %vr04	;//	+P0 & Enable & ApD&Sg
	vandaw	%vr10, %vr18, %vr23	;//	+P0_NOT_ApD&Sg & (Enable & NOT ApD&Sg)

	vbic	%vr11, %vr11, %vr06	;//	 q0 & Not Enable		for q0
	vandaw	%vr11, %vr19, %vr05	;//	+Q0 & Enable & AqD&Sg
	vandaw	%vr11, %vr22, %vr07	;//	+Q0_NOT_AqD&Sg & (Enable & NOT AqD&Sg)

	vbic	%vr12, %vr12, %vr05	;//	 q1 & !(Enable & AqD&Sg)	for q1
	vandaw	%vr12, %vr20, %vr05	;//	+Q1 & Enable & AqD&Sg

	vbic	%vr13, %vr13, %vr05	;//	 q2 &  !(Enable & AqD&Sg)	for q2
	vandaw	%vr13, %vr21, %vr05	;//	+Q2 & Enable & AqD&Sg


	vasrpwb	%vr08, %vr08, 0		;//	p2	Pack to bytes
	vasrpwb	%vr09, %vr09, 0		;//	p1
	vasrpwb	%vr10, %vr10, 0		;//	p0
	vasrpwb	%vr11, %vr11, 0		;//	q0
	vasrpwb	%vr12, %vr12, 0		;//	q1
	vasrpwb	%vr13, %vr13, 0		;//	q2

        vst64   %vr08,[%i1,24]          ;// p2
        vst64   %vr09,[%i1,48]          ;// p1
        vst64   %vr10,[%i1,72]          ;// p0
        vst64   %vr11,[%i1,96]          ;// q0
        vst64   %vr12,[%i1,120]         ;// q1
        vst64   %vr13,[%i1,144]         ;// q2

        vbaddw  %vr00,%vr00,8           // Next 2 edges along
//------
//        vor %vr23,%vr23,%vr23
	vendrec        %r0

}





_Asm int MACRO_NormalVerticalFilter(int addr)
{
        %reg addr
        vrec    addr
//------

        vld64w  %vr07,[%i1,0]           ;// p3
        vld64w  %vr08,[%i1,24]          ;// p2
        vld64w  %vr09,[%i1,48]          ;// p1
        vld64w  %vr10,[%i1,72]          ;// p0
        vld64w  %vr11,[%i1,96]          ;// q0
        vld64w  %vr12,[%i1,120]         ;// q1
        vld64w  %vr13,[%i1,144]         ;// q2
        vld64w  %vr14,[%i1,168]         ;// q3


// Ordiary filter

	vdifw	%vr20, %vr10/*P0*/, %vr11/*Q0*/	;//	| p0-q0 |			for enable
	vdifw	%vr21, %vr09/*P1*/, %vr10/*P0*/	;//	| p1-p0 |			..
	vdifw	%vr22, %vr11/*Q0*/, %vr12/*Q1*/	;//	| q0-q1 |			..

	vdifw	%vr19, %vr08/*P2*/, %vr10/*P0*/	;//	| p2-p0 |			for ApD
	vdifw	%vr23, %vr13/*Q2*/, %vr11/*Q0*/	;//	| q2-q0 |			for AqD

	vltw	%vr04, %vr20, %vr01	;//	| p0-q0 | < alpha	for enable
	vltw	%vr05, %vr21, %vr02	;//	| p1-p0 | < beta	..
	vltw	%vr17, %vr22, %vr02	;//	| q0-q1 | < beta	..

	vltw	%vr19, %vr19, %vr02	;//	ApD
	vltw	%vr23, %vr23, %vr02	;//	AqD

	vsubw	%vr20, %vr11/*Q0*/, %vr10/*P0*/	;//	 q0 - p0			for delta

	vand	%vr04, %vr04, %vr05	;//						for enable

	vaddw	%vr22, %vr10/*P0*/, %vr11/*Q0*/	;//	 p0 + q0			for p0q0rs1

	vsubw	%vr05, %vr03, %vr19	;//	 C0 - ApD (-1=true)      for +C

	vaddw	%vr16, %vr20, %vr20	;//	 q0-p0 + q0-p0		for delta
	vaddaw	%vr16, %vr20, %vr20	;//	+q0-p0 + q0-p0
	vsubaw	%vr16, %vr09/*P1*/, %vr12/*Q1*/	;//	+p1 - q1

    vsubw   %vr05, %vr05, %vr23     ;//     + C0 + ApD - AqD        +C

	vasrrw	%vr22, %vr22, 1		;//	p0q0rs1

	vand	%vr04, %vr04, %vr17	;//	Enable
    
	vasrrw	%vr16, %vr16, 3		;//						for delta

	vand	%vr19, %vr19, %vr04	;//	ApD & Enable
	vand	%vr23, %vr23, %vr04	;//	AqD & Enable

    vand    %vr05, %vr05, %vr04 ;// Mask C with enable

	vsubw	%vr17, %vr22, %vr09/*P1*/	;//	 p0q0rs1 - p1	for P1
	vsubaw	%vr17, %vr08/*P2*/, %vr09/*P1*/	;//	+p2 + -p1

	vsubw	%vr18, %vr22, %vr12/*Q1*/	;//	 p0q0rs1 - q1	for Q1
	vsubaw	%vr18, %vr13/*Q2*/, %vr12/*Q1*/	;//	+q2 + -q1

    vbmulw  %vr06, %vr05, -1    ;// -C

	vbmulw	%vr21, %vr03, -1	;//	-Co

	vminw	%vr20, %vr16, %vr05	;//	Min delta,  C

	vasrw	%vr17, %vr17, 1		;//						for P1
	vasrw	%vr18, %vr18, 1		;//						for Q1

	vmaxw	%vr20, %vr20, %vr06	;//	Max delta, -C		delta


	vminw	%vr17, %vr17, %vr03	;//	min P1,  C0			for P1
	vminw	%vr18, %vr18, %vr03	;//	min Q1,  C0			for Q1

	vaddw	%vr10, %vr10/*P0*/, %vr20	;//	p0 + delta			P0
	vsubw	%vr11, %vr11/*Q0*/, %vr20	;//	q0 - delta			Q0

	vmaxw	%vr17, %vr17, %vr21	;//	max P1,  -C0			for P1
	vmaxw	%vr18, %vr18, %vr21	;//	max Q1,  -C0			for Q1

	vaddw	%vr17, %vr09/*P1*/, %vr17	;//	p1 + P1				P1
	vaddw	%vr18, %vr12/*Q1*/, %vr18	;//	q1 + Q1				Q1

	vbic	%vr09/*P1*/, %vr09/*P1*/, %vr19	;//	 p1 & not ApD&Enable
	vandaw	%vr09/*P1*/, %vr17, %vr19	;//	+P1 & ApD&Enable

	vbic	%vr12/*Q1*/, %vr12/*Q1*/, %vr23	;//	 q1 & not AqD&Enable
	vandaw	%vr12/*Q1*/, %vr18, %vr23	;//	+Q1 & AqD&Enable

	vasrpwb	%vr09/*P1*/, %vr09/*P1*/, 0		;//	p1    Pack to bytes
	vasrpwb	%vr10/*P0*/, %vr10/*P0*/, 0		;//	p0
	vasrpwb	%vr11/*Q0*/, %vr11/*Q0*/, 0		;//	q0
	vasrpwb	%vr12/*Q1*/, %vr12/*Q1*/, 0		;//	q1

    vst64   %vr09,[%i1,48]          ;// p1
    vst64   %vr10,[%i1,72]          ;// p0
    vst64   %vr11,[%i1,96]          ;// q0
    vst64   %vr12,[%i1,120]         ;// q1

//------

//        vor %vr23,%vr23,%vr23
	vendrec        %r0
}



_Asm int MACRO_HorzontalFilterAligned(int addr)
{
        %reg addr
        vrec    addr

//        vdiwr           %dr6,2          ;//    Clip mode - NON INTERLACE(block on DMA in not complete
        vld64w  %vr07,[%i1,0+96]           ;// 0
        vld64w  %vr08,[%i1,24+96]          ;// 1
        vld64w  %vr09,[%i1,48+96]          ;// 2
        vld64w  %vr10,[%i1,72+96]          ;// 3
        vbaddw  %vr00,%vr00,96          ;// %I1 to Edge below

        vh264ft %vr11, %vr07, %vr01             ;//
        vh264ft %vr12, %vr08, %vr01             ;//
        vh264ft %vr13, %vr09, %vr01             ;//
        vh264ft %vr14, %vr10, %vr01             ;//

        vh264f  %vr07, %vr07, %vr11             ;//
        vh264f  %vr08, %vr08, %vr12             ;//
        vh264f  %vr09, %vr09, %vr13             ;//
        vh264f  %vr10, %vr10, %vr14             ;//
        
        vst64   %vr07,[%i1,0]           ;// 0
        vst64   %vr08,[%i1,24]          ;// 1
        vst64   %vr09,[%i1,48]          ;// 2
        vst64   %vr10,[%i1,72]          ;// 3


//------        
//        vor %vr23,%vr23,%vr23
        vendrec %r0
    
}

_Asm int MACRO_HorzontalFilter(int addr)
{
        %reg addr
        vrec    addr

//        vdiwr           %dr6,2          ;//    Clip mode - NON INTERLACE(block on DMA in not complete
        vld32wl %vr07,[%i1,0+96]           ;// 0
        vld32wh %vr07,[%i1,0+4+96]         ;// 0
        vld32wl %vr08,[%i1,24+96]          ;// 1
        vld32wh %vr08,[%i1,24+4+96]        ;// 1
        vld32wl %vr09,[%i1,48+96]          ;// 2
        vld32wh %vr09,[%i1,48+4+96]        ;// 2
        vld32wl %vr10,[%i1,72+96]          ;// 3
        vld32wh %vr10,[%i1,72+4+96]        ;// 3
        vbaddw  %vr00,%vr00,96             ;// %I1 to Edge below

        vh264ft %vr11, %vr07, %vr01             ;//
        vh264ft %vr12, %vr08, %vr01             ;//
        vh264ft %vr13, %vr09, %vr01             ;//
        vh264ft %vr14, %vr10, %vr01             ;//

        vh264f  %vr07, %vr07, %vr11             ;//
        vh264f  %vr08, %vr08, %vr12             ;//
        vh264f  %vr09, %vr09, %vr13             ;//
        vh264f  %vr10, %vr10, %vr14             ;//
        
        vst32   %vr07,[%i1,0]           ;// 0
        vst32   %vr08,[%i1,24]          ;// 1
        vst32   %vr09,[%i1,48]          ;// 2
        vst32   %vr10,[%i1,72]          ;// 3

        vst32_2   %vr07,[%i1,0+4]         ;// 0
        vst32_2   %vr08,[%i1,24+4]        ;// 1
        vst32_2   %vr09,[%i1,48+4]        ;// 2
        vst32_2   %vr10,[%i1,72+4]        ;// 3
//------        
//        vor %vr23,%vr23,%vr23
        vendrec %r0
    
}





//------------------------------------------------------------

// TODO Optimize
_Asm int MACRO_LoopFilterEdgeChromaHorizontal(int addr)
{
    %reg addr
    vrec    addr
//------


    // %i4 U pixels
    // %I5 V
    
    // vr01     alpha
    // vr02     beta
    
    // vr20     c
    // vr21     -c
    
    // Data as loaded U & V for lines a & b
    //           7    6     5    4    3   2    1    0
    // vr03     Vp0a Vp1a xxxx xxxx Up0a Up1a xxxx xxxx
    // vr04     Vp0b Vp1b xxxx xxxx Up0b Up1b xxxx xxxx
    // vr05     xxxx xxxx Vq1a Vq0a xxxx xxxx Uq1a Uq0a
    // vr06     xxxx xxxx Vq1b Vq0b xxxx xxxx Uq1b Uq0b    
    vdiwr       %dr6,2                  //    Wait DMA in complete
    vld32wl     %vr03,[%i4,0]           // (a) U
    vld32wh     %vr03,[%i5,0]           // (a) V
    vld32wl     %vr04,[%i4,16]          // (b) U
    vld32wh     %vr04,[%i5,16]          // (b) V
    vld32wl     %vr05,[%i4,4]           // (a)
    vld32wh     %vr05,[%i5,4]           // (a)
    vld32wl     %vr06,[%i4,20]          // (b) 
    vld32wh     %vr06,[%i5,20]          // (b)

    // vr03     0000 0000 Vp0a Vp1a xxxx xxxx Up0a Up1a
    // vr04     0000 0000 Vp0b Vp1b xxxx xxxx Up0b Up1b
    // vr05     xxxx xxxx Vq1a Vq0a xxxx xxxx Uq1a Uq0a
    // vr06     xxxx xxxx Vq1b Vq0b xxxx xxxx Uq1b Uq0b
    vsr8        %vr03,%vr03,4
    vsr8        %vr04,%vr04,4
    
    // vr03     0000 0000[Vp1b]Vp1a xxxx xxxx[Up1b]Up1a
    // vr04     0000 0000 Vp0b[Vp0a]xxxx xxxx Up0b[Up0a]
    // vr05     xxxx xxxx{Vq0b}Vq0a xxxx xxxx{Uq0b}Uq0a
    // vr06     xxxx xxxx Vq1b{Vq1a}xxxx xxxx Uq1b{Uq1a}
    vexch1      %vr03,%vr04
    vexch1      %vr05,%vr06

    // vr03     p1
    // vr04     p0
    // vr05     q0
    // vr06     q1

    vbmulw      %vr21,%vr20,-1          // -c
    
    vdifw       %vr08,%vr03,%vr04       // ABS_PEL_DIFF(p1 - p0)
    vdifw       %vr07,%vr05,%vr04       // ABS_PEL_DIFF(q0 - p0)
    vdifw       %vr09,%vr06,%vr05       // ABS_PEL_DIFF(q1 - q0)
    
    vltw        %vr07,%vr07,%vr01       // ABS_PEL_DIFF(q0 - p0) < alpha
    vltw        %vr08,%vr08,%vr02       // ABS_PEL_DIFF(p1 - p0) < beta
    vltw        %vr09,%vr09,%vr02       // ABS_PEL_DIFF(q1 - q0) < beta
    
    vsubw       %vr12,%vr05,%vr04       // q0-p0
    
    vand        %vr07,%vr07,%vr08       // For enable
    vbmulw      %vr12,%vr12,4
    vsubw       %vr13,%vr03,%vr06       // p1-q1 

// (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
    vand        %vr07,%vr07,%vr09       // Enable
    vaddw       %vr13,%vr13,%vr12       // (q0-p0)<<2) + (p1-q1)
    vand        %vr19,%vr20,%vr07       // Mask C with enables
    
    vasrrw      %vr13,%vr13,3           // (((q0-p0)<<2) + (p1-q1) + 4) >> 3

    vand        %vr21,%vr21,%vr07       // Mask -C with enables
    
    vmaxw       %vr12,%vr13,%vr21       // max -c
    vminw       %vr12,%vr12,%vr19       // min c   (delta)
    
    vaddw       %vr04,%vr04,%vr12       // p0 + delta
    vsubw       %vr05,%vr05,%vr12       // q0 - delta

    // vr03     0000 0000 Vp0a Vp1a xxxx xxxx Up0a Up1a
    // vr04     0000 0000 Vp0b Vp1b xxxx xxxx Up0b Up1b
    // vr05     xxxx xxxx Vq1a Vq0a xxxx xxxx Uq1a Uq0a
    // vr06     xxxx xxxx Vq1b Vq0b xxxx xxxx Uq1b Uq0b
    vexch1      %vr03,%vr04
    vexch1      %vr05,%vr06
    
    vasrpwb     %vr03,%vr03,0
    vasrpwb     %vr04,%vr04,0
    vasrpwb     %vr05,%vr05,0
    vasrpwb     %vr06,%vr06,0
// Store U
    vst16       %vr03,[%i4,2]           // (a) Up0a Up1a
    vst16       %vr04,[%i4,18]          // (b) Up0b Up1b
    vst16       %vr05,[%i4,4]           // (a) Uq1a Uq0a
    vst16       %vr06,[%i4,20]          // (b) Uq1b Uq0b

// Store V

    vst16_2     %vr03,[%i5,2]           // (a) Vp0a Vp1a
    vst16_2     %vr04,[%i5,18]          // (b) Vp0b Vp1b
    vst16_2     %vr05,[%i5,4]           // (a) Vq1a Vq0a
    vst16_2     %vr06,[%i5,20]          // (b) Vq1b Vq0b
    
    vbaddw      %vr00,%vr00,32          // Next line down





//    // i5       ptr -4
//    // vr01     alpha
//    // vr02     beta
//    
//    // vr20     c
//    
//    // lanes 0 & 4  / calc a & b
//    // vr03     p1
//    // vr04     p0
//    // vr05     q0
//    // vr06     q1
//    
//    // vr10     p0 p1 xx xx calc a
//    // vr11     xx xx q1 q0 calc a
//    
//    // vr14     p0 p1 xx xx calc b
//    // vr15     xx xx q1 q0 calc b
//    vxor        %vr10,%vr10,%vr10
//    vxor        %vr11,%vr11,%vr11
//    vxor        %vr14,%vr14,%vr14
//    
//    vdiwr       %dr6,2                  //    Wait DMA in complete
//    vld32wl     %vr10,[%i5,0]           // (a)
//    vld32wl     %vr11,[%i5,4]           // (a)
//    vld32wl     %vr14,[%i5,16]          // (b)
//    vxor        %vr15,%vr15,%vr15
//    vld32wl     %vr15,[%i5,20]          // (b)
//    
////    vr10             // xx xx xx xx p0 p1 xx xx (a)
////    vr11             // xx xx xx xx xx xx q1 q0 (a)
////    vr14             // xx xx xx xx p0 p1 xx xx (b)
////    vr15             // xx xx xx xx xx xx q1 q0 (b)
//    
//    vsr8        %vr03,%vr10,4           // align p1 (a)   xxxxxxxa
//    vmr6aw      %vr03,%vr14,%vr14       // align p1 (b)  +xxxxxbxx[xxxxxbxx] >>6 = xxxbxxxa
//    vsr8        %vr04,%vr10,6           // align p0 (a)   xxxxxxxa
//    vmr7aw      %vr04,%vr14,%vr14       // align p0 (b)  +xxxxbxxx[xxxxbxxx] >>7 = xxxbxxxa
//    
//    vsr8        %vr05,%vr11,0           // align q0 (a)   xxxxxxxa
//    vmr4aw      %vr05,%vr15,%vr15       // align q0 (b)  +xxxxxxxb[xxxxxxxb] >>4 = xxxbxxxa
//    vsr8        %vr06,%vr11,2           // align q1 (a)   xxxxxxxa
//    vmr5aw      %vr06,%vr15,%vr15       // align q0 (b)  +xxxxxxbx[xxxxxxbx] >>5 = xxxbxxxa
//    
//    vdifw       %vr07,%vr05,%vr04       // ABS_PEL_DIFF(q0 - p0)
//    vdifw       %vr08,%vr03,%vr04       // ABS_PEL_DIFF(p1 - p0)
//    vdifw       %vr09,%vr06,%vr05       // ABS_PEL_DIFF(q1 - q0)
//    
//    vltw        %vr07,%vr07,%vr01       // ABS_PEL_DIFF(q0 - p0) < alpha
//    vltw        %vr08,%vr08,%vr02       // ABS_PEL_DIFF(p1 - p0) < beta
//    vltw        %vr09,%vr09,%vr02       // ABS_PEL_DIFF(q1 - q0) < beta
//    
//    vand        %vr07,%vr07,%vr08       // For enable
//
//
//
//// (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
//    vsubw       %vr12,%vr05,%vr04       // q0-p0
//    vsubw       %vr13,%vr03,%vr06       // p1-q1
//    vaddaw      %vr13,%vr12,%vr12       // (q0-p0)<<1) + (p1-q1)
//    vaddaw      %vr13,%vr12,%vr12       // (q0-p0)<<2) + (p1-q1)
//
//    vand        %vr07,%vr07,%vr09       // Enable
//
//    vbmulw      %vr21,%vr20,-1          // -c
//
//    vmvzw       %vr08,%vr07,0x01        // Enable mask 0 0 0 0 0 0 0 A
//
//    vasrrw      %vr13,%vr13,3           // (((q0-p0)<<2) + (p1-q1) + 4) >> 3
//    
//    vmr5w       %vr09,%vr08,%vr08       // 0000000A[0000000A] >>5  = 0000A000
//    
//    vmaxw       %vr12,%vr13,%vr21       // max -c
//    
//    vmvzw       %vr19,%vr07,0x10        // Enable mask 0 0 0 B 0 0 0 0
//    
//    vminw       %vr12,%vr12,%vr20       // min c   (delta)
//
//    vsr8        %vr18,%vr19,8           // 000B0000 >>4  = 0000000B
//    vsr8        %vr19,%vr19,2           // 000B0000 >>4  = 0000B000
//
//    
//    vaddw       %vr04,%vr04,%vr12       // p0 + delta xxxBxxxA
//    vsubw       %vr05,%vr05,%vr12       // q0 - delta xxxBxxxA
//
//    vmr5w       %vr03,%vr04,%vr04       // align p0 (a) xx xx xx xx p0 xx xx xx
//    vmr1w       %vr04,%vr04,%vr04       // align p0 (b) xx xx xx xx p0 xx xx xx
//    vmr4w       %vr06,%vr05,%vr05       // align q0 (b) xx xx xx xx xx xx xx q0
//    
//    
//    vbic        %vr10,%vr10,%vr09       //(a)
//    vandaw      %vr10,%vr03,%vr09       // p0 (a)
//
//    vbic        %vr14,%vr14,%vr19       // (b)
//    vandaw      %vr14,%vr04,%vr19       // p0 (b)
//
//    vbic        %vr11,%vr11,%vr08       //(a)
//    vandaw      %vr11,%vr05,%vr08       // q0 (a)
// 
//    vbic        %vr15,%vr15,%vr18       //(b)
//    vandaw      %vr15,%vr06,%vr18       // q0 (b)
// 
//    vasrpwb     %vr10,%vr10,0           //(a)
//    vasrpwb     %vr11,%vr11,0           //(a)
//    vasrpwb     %vr14,%vr14,0           //(b)
//    vasrpwb     %vr15,%vr15,0           //(b)
//    
//    vst32       %vr10,[%i5,0]           //(a)
//    vst32       %vr11,[%i5,4]           //(a)
//    vst32       %vr14,[%i5,16]          //(b)
//    vst32       %vr15,[%i5,20]          //(b)
//    
//    vbaddw      %vr00,%vr00,32          // Next line down
//
//------
        vendrec %r0

}

// TODO Optimize
_Asm int MACRO_LoopFilterEdgeChromaHorizontalStrong(int addr)
{
    %reg addr
        vrec    addr
//------
    // i4       U
    // i5       V
    // vr01     alpha
    // vr02     beta
    
    
    // vr03     p1
    // vr04     p0
    // vr05     q0
    // vr06     q1
    // Data as loaded U & V for lines a & b
    //           7    6     5    4    3   2    1    0
    // vr03     Vp0a Vp1a xxxx xxxx Up0a Up1a xxxx xxxx
    // vr04     Vp0b Vp1b xxxx xxxx Up0b Up1b xxxx xxxx
    // vr05     xxxx xxxx Vq1a Vq0a xxxx xxxx Uq1a Uq0a
    // vr06     xxxx xxxx Vq1b Vq0b xxxx xxxx Uq1b Uq0b    
    vdiwr       %dr6,2                  //    Wait DMA in complete
    vld32wl     %vr03,[%i4,0]           // (a) U
    vld32wh     %vr03,[%i5,0]           // (a) V
    vld32wl     %vr04,[%i4,16]          // (b) U
    vld32wh     %vr04,[%i5,16]          // (b) V
    vld32wl     %vr05,[%i4,4]           // (a)
    vld32wh     %vr05,[%i5,4]           // (a)
    vld32wl     %vr06,[%i4,20]          // (b) 
    vld32wh     %vr06,[%i5,20]          // (b)

    // vr07     Vp0c Vp1c xxxx xxxx Up0c Up1c xxxx xxxx
    // vr08     Vp0d Vp1d xxxx xxxx Up0d Up1d xxxx xxxx
    // vr09     xxxx xxxx Vq1c Vq0c xxxx xxxx Uq1c Uq0c
    // vr10     xxxx xxxx Vq1d Vq0d xxxx xxxx Uq1d Uq0d    
    vld32wl     %vr07,[%i4,0+32]           // (c) U
    vld32wh     %vr07,[%i5,0+32]           // (c) V
    vld32wl     %vr08,[%i4,16+32]          // (d) U
    vld32wh     %vr08,[%i5,16+32]          // (d) V
    vld32wl     %vr09,[%i4,4+32]           // (c)
    vld32wh     %vr09,[%i5,4+32]           // (c)
    vld32wl     %vr10,[%i4,20+32]          // (d) 
    vld32wh     %vr10,[%i5,20+32]          // (d)



    // vr03     0000 0000 Vp0a Vp1a xxxx xxxx Up0a Up1a
    // vr04     0000 0000 Vp0b Vp1b xxxx xxxx Up0b Up1b
    // vr05     xxxx xxxx Vq1a Vq0a xxxx xxxx Uq1a Uq0a
    // vr06     xxxx xxxx Vq1b Vq0b xxxx xxxx Uq1b Uq0b
    vsr8        %vr03,%vr03,4
    vsr8        %vr04,%vr04,4


    // vr07     Vp0c Vp1c xxxx xxxx Up0c Up1c xxxx xxxx
    // vr08     Vp0d Vp1d xxxx xxxx Up0d Up1d xxxx xxxx
    // vr09     Vq1c Vq0c xxxx xxxx Uq1c Uq0c xxxx xxxx 
    // vr10     Vq1d Vq0d xxxx xxxx Uq1d Uq0d xxxx xxxx   
    vmr6w       %vr09,%vr09,%vr09
    vmr6w       %vr10,%vr10,%vr10

    // vr03     Vp0c Vp1c Vp0a Vp1a Up0c Up1c Up0a Up1a
    // vr04     Vp0d Vp1d Vp0b Vp1b Up0d Up1d Up0b Up1b
    // vr05     Vq1c Vq0c Vq1a Vq0a Uq1c Uq0c Uq1a Uq0a
    // vr06     Vq1d Vq0d Vq1b Vq0b Uq1d Uq0d Uq1b Uq0b
    vmvw        %vr03,%vr07,0xcc
    vmvw        %vr04,%vr08,0xcc
    vmvw        %vr05,%vr09,0xcc
    vmvw        %vr06,%vr10,0xcc

    // vr03     Vp1d Vp1c[Vp1b]Vp1a Up1d Up1c[Up1b]Up1a
    // vr04     Vp0d Vp0c Vp0b[Vp0a]Up0d Up0c Up0b[Up0a]
    // vr05     Vq0d Vq0c{Vq0b}Vq0a Uq0d Uq0c{Uq0b}Uq0a
    // vr06     Vq1d Vq1c Vq1b{Vq1a}Uq1d Uq1c Uq1b{Uq1a}
    vexch1      %vr03,%vr04
    vexch1      %vr05,%vr06
    
    
    vdifw       %vr08,%vr03,%vr04       // ABS_PEL_DIFF(p1 - p0)
    vdifw       %vr07,%vr05,%vr04       // ABS_PEL_DIFF(q0 - p0)
    vdifw       %vr09,%vr06,%vr05       // ABS_PEL_DIFF(q1 - q0)
    
    vltw        %vr07,%vr07,%vr01       // ABS_PEL_DIFF(q0 - p0) < alpha
    vltw        %vr08,%vr08,%vr02       // ABS_PEL_DIFF(p1 - p0) < beta
    vltw        %vr09,%vr09,%vr02       // ABS_PEL_DIFF(q1 - q0) < beta
    
    vand        %vr07,%vr07,%vr08       // For enable
    vand        %vr07,%vr07,%vr09       // Enable
    
    vaddw       %vr10,%vr04,%vr06       // p0 + q1
    vaddaw      %vr10,%vr03,%vr03       // p0 + q1 + (p1 << 1)
    
    vaddw       %vr11,%vr05,%vr03       // q0 + p1
    vaddaw      %vr11,%vr06,%vr06       // q0 + p1 + (q1 << 1)

    vasrrw       %vr10,%vr10,2          // p0+2 >> 2
    vasrrw       %vr11,%vr11,2          // q0+2 >> 2
    
    vbic        %vr04,%vr04,%vr07       // select
    vandaw      %vr04,%vr10,%vr07 
    
    vbic        %vr05,%vr05,%vr07       // select
    vandaw      %vr05,%vr11,%vr07 
 
    // vr03     Vp0c Vp1c Vp0a Vp1a Up0c Up1c Up0a Up1a
    // vr04     Vp0d Vp1d Vp0b Vp1b Up0d Up1d Up0b Up1b
    // vr05     Vq1c Vq0c Vq1a Vq0a Uq1c Uq0c Uq1a Uq0a
    // vr06     Vq1d Vq0d Vq1b Vq0b Uq1d Uq0d Uq1b Uq0b
    vexch1      %vr03,%vr04
    vexch1      %vr05,%vr06
    
    vasrpwb     %vr03,%vr03,0
    vasrpwb     %vr04,%vr04,0
    vasrpwb     %vr05,%vr05,0
    vasrpwb     %vr06,%vr06,0
    
// Store U a b
    vst16       %vr03,[%i4,2]           // (a) Up0a Up1a
    vst16       %vr04,[%i4,18]          // (b) Up0b Up1b
    vst16       %vr05,[%i4,4]           // (a) Uq1a Uq0a
    vst16       %vr06,[%i4,20]          // (b) Uq1b Uq0b

// Store U c d
    
    vst16_1     %vr03,[%i4,2+32]        // (c) Up0c Up1c
    vst16_1     %vr04,[%i4,18+32]       // (d) Up0d Up1d
    vst16_1     %vr05,[%i4,4+32]        // (c) Uq1c Uq0c
    vst16_1     %vr06,[%i4,20+32]       // (d) Uq1d Uq0d


// Store V a b

    vst16_2     %vr03,[%i5,2]           // (a) Vp0a Vp1a
    vst16_2     %vr04,[%i5,18]          // (b) Vp0b Vp1b
    vst16_2     %vr05,[%i5,4]           // (a) Vq1a Vq0a
    vst16_2     %vr06,[%i5,20]          // (b) Vq1b Vq0b

// Store V c d
    
    vst16_3     %vr03,[%i5,2+32]        // (c) Vp0c Vp1c
    vst16_3     %vr04,[%i5,18+32]       // (d) Vp0d Vp1d
    vst16_3     %vr05,[%i5,4+32]        // (c) Vq1c Vq0c
    vst16_3     %vr06,[%i5,20+32]       // (d) Vq1d Vq0d
    
    vbaddw      %vr00,%vr00,64          // Next 2 lines down
 
//------
    vendrec %r0
}


// TODO Optimize
_Asm int MACRO_LoopFilterEdgeChromaVertical(int addr)
{
    %reg addr
        vrec    addr
//------
    // i5       ptr -2*data_inc
    // vr01     alpha
    // vr02     beta
    
    // vr20     c
    // vr21     -c
    
    // vr03     p1
    // vr04     p0
    // vr05     q0
    // vr06     q1
    vdiwr           %dr6,2          ;//    Wait DMA in complete
    
    vld32wl       %vr03,[%i4,0]            // p1   U
    vld32wl       %vr04,[%i4,16]           // p0
    vld32wl       %vr05,[%i4,32]           // q0
    vld32wl       %vr06,[%i4,48]           // q1
    
    vld32wh       %vr03,[%i5,0]            // p1   V
    vld32wh       %vr04,[%i5,16]           // p0
    vld32wh       %vr05,[%i5,32]           // q0
    vld32wh       %vr06,[%i5,48]           // q1
    
    vbmulw      %vr21,%vr20,-1          // -c
    
    vdifw       %vr08,%vr03,%vr04       // ABS_PEL_DIFF(p1 - p0)
    vdifw       %vr07,%vr05,%vr04       // ABS_PEL_DIFF(q0 - p0)
    vdifw       %vr09,%vr06,%vr05       // ABS_PEL_DIFF(q1 - q0)
    
    vltw        %vr07,%vr07,%vr01       // ABS_PEL_DIFF(q0 - p0) < alpha
    vltw        %vr08,%vr08,%vr02       // ABS_PEL_DIFF(p1 - p0) < beta
    vltw        %vr09,%vr09,%vr02       // ABS_PEL_DIFF(q1 - q0) < beta
    
    vsubw       %vr12,%vr05,%vr04       // q0-p0
    
    vand        %vr07,%vr07,%vr08       // For enable
    vbmulw      %vr12,%vr12,4
    vsubw       %vr13,%vr03,%vr06       // p1-q1 

// (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
    vand        %vr07,%vr07,%vr09       // Enable
    vaddw       %vr13,%vr13,%vr12       // (q0-p0)<<2) + (p1-q1)
    vand        %vr19,%vr20,%vr07       // Mask C with enables
    
    vasrrw      %vr13,%vr13,3           // (((q0-p0)<<2) + (p1-q1) + 4) >> 3

    vand        %vr21,%vr21,%vr07       // Mask -C with enables
    
    vmaxw       %vr12,%vr13,%vr21       // max -c
    vminw       %vr12,%vr12,%vr19       // min c   (delta)
    
    vaddw       %vr04,%vr04,%vr12       // p0 + delta
    vsubw       %vr05,%vr05,%vr12       // q0 - delta
         
    vasrpwb     %vr04,%vr04,0
    vasrpwb     %vr05,%vr05,0

    vst32       %vr04,[%i4,16]           // p0  U
    vst32       %vr05,[%i4,32]           // q0
    
    vst32_2       %vr04,[%i5,16]           // p0  V
    vst32_2       %vr05,[%i5,32]           // q0
    
//------
    vendrec %r0
   
}



// TODO Optimize
// Now does complete top line of macro block
_Asm int MACRO_LoopFilterEdgeChromaVerticalStrong(int addr)
{
    %reg addr
        vrec    addr
//------
    // i5       ptr -2*data_inc
    // vr01     alpha
    // vr02     beta
    
    // vr22     Master enable mask
    
    // vr03     p1
    // vr04     p0
    // vr05     q0
    // vr06     q1
    vdiwr           %dr6,2          ;//    Wait DMA in complete
    
    vld64w       %vr03,[%i5,0]            // p1
    vld64w       %vr04,[%i5,16]           // p0
    vld64w       %vr05,[%i5,32]           // q0
    vld64w       %vr06,[%i5,48]           // q1
    
    
    vdifw       %vr08,%vr03,%vr04       // ABS_PEL_DIFF(p1 - p0)
    vdifw       %vr07,%vr05,%vr04       // ABS_PEL_DIFF(q0 - p0)
    vdifw       %vr09,%vr06,%vr05       // ABS_PEL_DIFF(q1 - q0)
    
    vltw        %vr07,%vr07,%vr01       // ABS_PEL_DIFF(q0 - p0) < alpha
    vltw        %vr08,%vr08,%vr02       // ABS_PEL_DIFF(p1 - p0) < beta
    vltw        %vr09,%vr09,%vr02       // ABS_PEL_DIFF(q1 - q0) < beta
    
    vand        %vr07,%vr07,%vr08       // For enable
    vand        %vr07,%vr07,%vr09       // Enable
//    vand        %vr07,%vr07,%vr22       // Master enable
    
    vaddw       %vr10,%vr04,%vr06       // p0 + q1
    vaddaw      %vr10,%vr03,%vr03       // p0 + q1 + (p1 << 1)
    
    vaddw       %vr11,%vr05,%vr03       // q0 + p1
    vaddaw      %vr11,%vr06,%vr06       // q0 + p1 + (q1 << 1)

    vbmulw       %vr04,%vr04,4          // p0 << 2
    vbmulw       %vr05,%vr05,4          // q0 << 2
    
    vbic        %vr04,%vr04,%vr07       // select
    vandaw      %vr04,%vr10,%vr07 
    
    vbic        %vr05,%vr05,%vr07       // select
    vandaw      %vr05,%vr11,%vr07 
 
    vasrrpwb     %vr04,%vr04,2          // (p0+2) >>2
    vasrrpwb     %vr05,%vr05,2          // (q0+2) >>2

    vst64       %vr04,[%i5,16]           // p0
    vst64       %vr05,[%i5,32]           // q0
//------
    vendrec %r0
 
}



// Record macros for deblocking
// Called indirectly from the AuroraH264Dec constructor
void AuroraRecMgr::RecordDeblockMacros()
{
    Record(AM_DB_HorzontalFilterAligned, MACRO_HorzontalFilterAligned(NextAddr));
    Record(AM_DB_HorzontalFilter, MACRO_HorzontalFilter(NextAddr));
    Record(AM_DB_NormalVerticalFilter, MACRO_NormalVerticalFilter(NextAddr));
    Record(AM_DB_StrongVerticalFilter, MACRO_StrongVerticalFilter(NextAddr));
    Record(AM_DB_EdgeChromaHorizontal, MACRO_LoopFilterEdgeChromaHorizontal(NextAddr));
    Record(AM_DB_EdgeChromaHorizontalStrong, MACRO_LoopFilterEdgeChromaHorizontalStrong(NextAddr));
    Record(AM_DB_EdgeChromaVertical, MACRO_LoopFilterEdgeChromaVertical(NextAddr));
    Record(AM_DB_EdgeChromaVerticalStrong, MACRO_LoopFilterEdgeChromaVerticalStrong(NextAddr));
    Record(AM_DB_MoveForNextBlockY, MACRO_MoveForNextBlockL(NextAddr));
    Record(AM_DB_MoveForNextBlockU, MACRO_MoveForNextBlockU(NextAddr));
    Record(AM_DB_MoveForNextBlockV, MACRO_MoveForNextBlockV(NextAddr));
    Record(AM_DB_VerticalFilter2, MACRO_VerticalFilter2(NextAddr));
//    Record(AM_DB_, (NextAddr));
 
}

//#ifdef INLOOP_DBLOCK
//Move overlap area for next block
_Asm int MACRO_CopyTopLine16( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]


    vendrec %r0
}

_Asm int MACRO_CopyTopLine20( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
   vld32   %vr01,[%i0,0 ]
   vld32   %vr02,[%i0,0 + 4 ]
   vld32   %vr03,[%i0,0 + 8 ]
   vld32   %vr04,[%i0,0 + 12 ]
   vst32   %vr01,[%i1,0 ]
   vst32   %vr02,[%i1,0 + 4 ]                    
   vst32   %vr03,[%i1,0 + 8 ]
   vst32   %vr04,[%i1,0 + 12 ]

   vld32   %vr01,[%i0,0 + 16]
   vst32   %vr01,[%i1,0 + 16]


    vendrec %r0
}

_Asm int MACRO_CopyTopLine24( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 4 ]
    vld32   %vr03,[%i0,0 + 8 ]
    vld32   %vr04,[%i0,0 + 12 ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + 4 ]                    
    vst32   %vr03,[%i1,0 + 8 ]
    vst32   %vr04,[%i1,0 + 12 ]

	vld32   %vr01,[%i0,0 + 16]
	vld32   %vr02,[%i0,0 + 20]
	vst32   %vr01,[%i1,0 + 16]
	vst32   %vr02,[%i1,0 + 20]                    
                   
    vendrec %r0
}

_Asm int MACRO_CopyTopLine8( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vst64   %vr01,[%i1,0 ]


    vendrec %r0
}

_Asm int MACRO_CopyTopLine4( int addr)
{
    %reg    addr
    vrec    addr

// Move luma data ptr in %i0
    vld32   %vr01,[%i0,0]
    vst32   %vr01,[%i1,0]


    vendrec %r0
}

_Asm int MACRO_CopyTopLine12( int addr)
{
    %reg    addr
    vrec    addr

//  Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 4 ]
    vld32   %vr03,[%i0,0 + 8 ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + 4 ]                    
    vst32   %vr03,[%i1,0 + 8 ]
    
    vendrec %r0
}


_Asm int MACRO_Copy16x16_off_24_720( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  48,0x1
    vmovw  %vr07,1440,0x2
//  Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07        
    
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07
    
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07        
       
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07
    
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07        
       
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07
       
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
    vaddw   %vr00, %vr00, %vr07        
          
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*24) ]
    vld64   %vr04,[%i0,0 + (1*24) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*720) ]
    vst64   %vr04,[%i1,0 + (1*720) + 8]
                   
    vendrec %r0
}

_Asm int MACRO_Copy16x4_off_24_720( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  48,0x1
    vmovw  %vr07,1440,0x2
//  Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07
                   
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
    vaddw   %vr00, %vr00, %vr07
                                  
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*24) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*720) ]
                                                 
    vendrec %r0
}

_Asm int MACRO_Copy8x8_off_16_360( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  64,0x1
    vmovw  %vr07,1440,0x2
//  Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 16]
    vld64   %vr03,[%i0,0 + 32]
    vld64   %vr04,[%i0,0 + 48]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + (1*360)]
    vst64   %vr03,[%i1,0 + (2*360)]
    vst64   %vr04,[%i1,0 + (3*360)]
    vaddw   %vr00, %vr00, %vr07        
    
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 16]
    vld64   %vr03,[%i0,0 + 32]
    vld64   %vr04,[%i0,0 + 48]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + (1*360)]
    vst64   %vr03,[%i1,0 + (2*360)]
    vst64   %vr04,[%i1,0 + (3*360)]
                   
    vendrec %r0
}

_Asm int MACRO_Copy8x2_off_16_360( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  32,0x1
    vmovw  %vr07,720,0x2
//  Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 16]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + (1*360)]
    vaddw   %vr00, %vr00, %vr07        
    
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 16]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + (1*360)]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 16]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + (1*360)]
    vaddw   %vr00, %vr00, %vr07        
                   
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + 16]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + (1*360)]
    vaddw   %vr00, %vr00, %vr07        
    
                   
    vendrec %r0
}

_Asm int MACRO_Copy8x2_off_360_16_DBInput( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  720,0x1
    vmovw  %vr07,32,0x2
//  Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*360)]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + 16]
    vaddw   %vr00, %vr00, %vr07        
    
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*360)]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + 16]    
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*360)]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + 16]
    vaddw   %vr00, %vr00, %vr07        
                   
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*360)]
    vst16_1   %vr01,[%i1,0 ]
    vst16_1   %vr02,[%i1,0 + 16]
    //vaddw   %vr00, %vr00, %vr07        
    
                   
    vendrec %r0
}

_Asm int MACRO_Copy16x4_off_720_24_DBINPUT( int addr)
{
    %reg    addr
    vrec    addr

    vmovzw %vr07,  1440,0x1
    vmovw  %vr07,48,0x2
//  Move luma data ptr in %i0
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07        

    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07
                   
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
    vaddw   %vr00, %vr00, %vr07
                                  
    vld32   %vr01,[%i0,0 ]
    vld32   %vr02,[%i0,0 + (1*720) ]
    vst32   %vr01,[%i1,0 ]
    vst32   %vr02,[%i1,0 + (1*24) ]
                                                 
    vendrec %r0
}

_Asm int MACRO_Copy16x16_off_16_24_DBInput( int addr)
{
    %reg    addr
    vrec    addr

//    vmovzw %vr07,  48,0x1
//    vmovw  %vr07,1440,0x2
//  Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + 8]
    vld64   %vr03,[%i0,0 + (1*16) ]
    vld64   %vr04,[%i0,0 + (1*16) + 8]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + 8]
    vst64   %vr03,[%i1,0 + (1*24) ]
    vst64   %vr04,[%i1,0 + (1*24) + 8]

    vld64   %vr01,[%i0,0 + (2*16) ]
    vld64   %vr02,[%i0,0 + (2*16) + 8]
    vld64   %vr03,[%i0,0 + (3*16) ]
    vld64   %vr04,[%i0,0 + (3*16) + 8]
    vst64   %vr01,[%i1,0 + (2*24) ]
    vst64   %vr02,[%i1,0 + (2*24) + 8]
    vst64   %vr03,[%i1,0 + (3*24) ]
    vst64   %vr04,[%i1,0 + (3*24) + 8]

    vld64   %vr01,[%i0,0 + (4*16) ]
    vld64   %vr02,[%i0,0 + (4*16) + 8]
    vld64   %vr03,[%i0,0 + (5*16) ]
    vld64   %vr04,[%i0,0 + (5*16) + 8]
    vst64   %vr01,[%i1,0 + (4*24) ]
    vst64   %vr02,[%i1,0 + (4*24) + 8]
    vst64   %vr03,[%i1,0 + (5*24) ]
    vst64   %vr04,[%i1,0 + (5*24) + 8]

    vld64   %vr01,[%i0,0 + (6*16) ]
    vld64   %vr02,[%i0,0 + (6*16) + 8]
    vld64   %vr03,[%i0,0 + (7*16) ]
    vld64   %vr04,[%i0,0 + (7*16) + 8]
    vst64   %vr01,[%i1,0 + (6*24) ]
    vst64   %vr02,[%i1,0 + (6*24) + 8]
    vst64   %vr03,[%i1,0 + (7*24) ]
    vst64   %vr04,[%i1,0 + (7*24) + 8]
                                                                               
    vld64   %vr01,[%i0,0 + (8*16) ]
    vld64   %vr02,[%i0,0 + (8*16) + 8]
    vld64   %vr03,[%i0,0 + (9*16) ]
    vld64   %vr04,[%i0,0 + (9*16) + 8]
    vst64   %vr01,[%i1,0 + (8*24) ]
    vst64   %vr02,[%i1,0 + (8*24) + 8]
    vst64   %vr03,[%i1,0 + (9*24) ]
    vst64   %vr04,[%i1,0 + (9*24) + 8]

    vld64   %vr01,[%i0,0 + (10*16) ]
    vld64   %vr02,[%i0,0 + (10*16) + 8]
    vld64   %vr03,[%i0,0 + (11*16) ]
    vld64   %vr04,[%i0,0 + (11*16) + 8]
    vst64   %vr01,[%i1,0 + (10*24) ]
    vst64   %vr02,[%i1,0 + (10*24) + 8]
    vst64   %vr03,[%i1,0 + (11*24) ]
    vst64   %vr04,[%i1,0 + (11*24) + 8]

    vld64   %vr01,[%i0,0 + (12*16) ]
    vld64   %vr02,[%i0,0 + (12*16) + 8]
    vld64   %vr03,[%i0,0 + (13*16) ]
    vld64   %vr04,[%i0,0 + (13*16) + 8]
    vst64   %vr01,[%i1,0 + (12*24) ]
    vst64   %vr02,[%i1,0 + (12*24) + 8]
    vst64   %vr03,[%i1,0 + (13*24) ]
    vst64   %vr04,[%i1,0 + (13*24) + 8]
                                                                                                             
    vld64   %vr01,[%i0,0 + (14*16) ]
    vld64   %vr02,[%i0,0 + (14*16) + 8]
    vld64   %vr03,[%i0,0 + (15*16) ]
    vld64   %vr04,[%i0,0 + (15*16) + 8]
    vst64   %vr01,[%i1,0 + (14*24) ]
    vst64   %vr02,[%i1,0 + (14*24) + 8]
    vst64   %vr03,[%i1,0 + (15*24) ]
    vst64   %vr04,[%i1,0 + (15*24) + 8]                                                                               
                   
    
    vendrec %r0
}

_Asm int MACRO_Copy8x8_off_8_16_DBInput( int addr)
{
    %reg    addr
    vrec    addr

//    vmovzw %vr07,  48,0x1
//    vmovw  %vr07,1440,0x2
//  Move luma data ptr in %i0
    vld64   %vr01,[%i0,0 ]
    vld64   %vr02,[%i0,0 + (1*8)]
    vld64   %vr03,[%i0,0 + (2*8)]
    vld64   %vr04,[%i0,0 + (3*8)]
    vst64   %vr01,[%i1,0 ]
    vst64   %vr02,[%i1,0 + (1*16)]
    vst64   %vr03,[%i1,0 + (2*16)]
    vst64   %vr04,[%i1,0 + (3*16)]

    vld64   %vr01,[%i0,0 + (4*8)]
    vld64   %vr02,[%i0,0 + (5*8)]
    vld64   %vr03,[%i0,0 + (6*8)]
    vld64   %vr04,[%i0,0 + (7*8)]
    vst64   %vr01,[%i1,0 + (4*16)]
    vst64   %vr02,[%i1,0 + (5*16)]
    vst64   %vr03,[%i1,0 + (6*16)]
    vst64   %vr04,[%i1,0 + (7*16)]
                   
    vendrec %r0
}


void AuroraRecMgr::RecordInloopDBMacros()
{
	Record(AM_COPYTOPLINE16, MACRO_CopyTopLine16(NextAddr));
	Record(AM_COPYTOPLINE20, MACRO_CopyTopLine20(NextAddr));
	Record(AM_COPYTOPLINE24, MACRO_CopyTopLine24(NextAddr));
	Record(AM_COPYTOPLINE8, MACRO_CopyTopLine8(NextAddr));
	Record(AM_COPYTOPLINE4, MACRO_CopyTopLine4(NextAddr));
	Record(AM_COPYTOPLINE12, MACRO_CopyTopLine12(NextAddr));
	Record(AM_COPY16x16_OFF_24_720, MACRO_Copy16x16_off_24_720(NextAddr));
	Record(AM_COPY16x4_OFF_24_720, MACRO_Copy16x4_off_24_720(NextAddr));
	Record(AM_COPY8x8_OFF_16_360, MACRO_Copy8x8_off_16_360(NextAddr));
	Record(AM_COPY8x2_OFF_16_360, MACRO_Copy8x2_off_16_360(NextAddr));
	Record(AM_COPY8x2_OFF_360_16_DBINPUT, MACRO_Copy8x2_off_360_16_DBInput(NextAddr));
	Record(AM_COPY16X4_OFF_720_24_DBINPUT, MACRO_Copy16x4_off_720_24_DBINPUT(NextAddr));
	Record(AM_COPY16X16_OFF_16_24_DBINPUT, MACRO_Copy16x16_off_16_24_DBInput(NextAddr));
	Record(AM_COPY8X8_OFF_8_16_DBINPUT, MACRO_Copy8x8_off_8_16_DBInput(NextAddr));
	
	
}

//#endif
#if defined(I32K_D32K) || defined(I16K_D16K)  || defined(I8K_D8K)
#pragma Code()
#endif
#endif


