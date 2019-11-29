/*
 * Copyright (c) 2005 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * implement core Hpel functions
 */

#ifndef _AURORA_HPEL16_H_
#define _AURORA_HPEL16_H_



_Asm void put_mpeg4_qpel8x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_MPEG4_QPEL8x16_V_LOWPASS_SIMD_  ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_mpeg4_qpel8x16_v_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64w   vr01,   [i1,   0]    ; // src[0]
    vld64w   vr02,   [i1,  32]    ; // src[1]
    vld64w   vr03,   [i1,  64]    ; // src[2]
    vld64w   vr04,   [i1,  96]    ; // src[3]
    vld64w   vr05,   [i1, 128]    ; // src[4]
    vld64w   vr06,   [i1, 160]    ; // src[5]
    vld64w   vr07,   [i1, 192]    ; // src[6]
    vld64w   vr08,   [i1, 224]    ; // src[7]
    vld64w   vr09,   [i1, 256]    ; // src[8]
    vld64w   vr10,   [i1, 288]    ; // src[9]
    vld64w   vr11,   [i1, 320]    ; // src[10]
    vld64w   vr12,   [i1, 352]    ; // src[11]
    vld64w   vr13,   [i1, 384]    ; // src[12]
    vld64w   vr14,   [i1, 416]    ; // src[13]
    vld64w   vr15,   [i1, 448]    ; // src[14]
    vld64w   vr16,   [i1, 480]    ; // src[15]
    vld64w   vr17,   [i1, 512]    ; // src[16]

    ;// calculate row0: (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 )
    vaddw   vr18, vr01, vr02    ;
    vaddw   vr19, vr01, vr03    ;
    vaddw   vr20, vr02, vr04    ;
    vaddw   vr21, vr03, vr05    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row1: (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 )
    vaddw   vr18, vr02, vr03    ;
    vaddw   vr19, vr01, vr04    ;
    vaddw   vr20, vr01, vr05    ;
    vaddw   vr21, vr02, vr06    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2,   0]     ;
    vst64   vr18, [i2,  32]     ;
  
    ;// calculate row2: (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 )
    vaddw   vr18, vr03, vr04    ;
    vaddw   vr19, vr02, vr05    ;
    vaddw   vr20, vr01, vr06    ;
    vaddw   vr21, vr01, vr07    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row3: (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 )
    vaddw   vr18, vr04, vr05    ;
    vaddw   vr19, vr03, vr06    ;
    vaddw   vr20, vr02, vr07    ;
    vaddw   vr21, vr01, vr08    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2,  64]     ;
    vst64   vr18, [i2,  96]     ;

    ;// calculate row4: (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 )
    vaddw   vr18, vr05, vr06    ;
    vaddw   vr19, vr04, vr07    ;
    vaddw   vr20, vr03, vr08    ;
    vaddw   vr21, vr02, vr09    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row5: (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 )
    vaddw   vr18, vr06, vr07    ;
    vaddw   vr19, vr05, vr08    ;
    vaddw   vr20, vr04, vr09    ;
    vaddw   vr21, vr03, vr10    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 128]     ;
    vst64   vr18, [i2, 160]     ;

    ;//calculate row6: (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10)
    vaddw   vr18, vr07, vr08    ;
    vaddw   vr19, vr06, vr09    ;
    vaddw   vr20, vr05, vr10    ;
    vaddw   vr21, vr04, vr11    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row7: (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11)
    vaddw   vr18, vr08, vr09    ;
    vaddw   vr19, vr07, vr10    ;
    vaddw   vr20, vr06, vr11    ;
    vaddw   vr21, vr05, vr12    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 192]     ;
    vst64   vr18, [i2, 224]     ;

    ;//calculate row8: (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12)
    vaddw   vr18, vr09, vr10    ;
    vaddw   vr19, vr08, vr11    ;
    vaddw   vr20, vr07, vr12    ;
    vaddw   vr21, vr06, vr13    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row9: (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13)
    vaddw   vr18, vr10, vr11    ;
    vaddw   vr19, vr09, vr12    ;
    vaddw   vr20, vr08, vr13    ;
    vaddw   vr21, vr07, vr14    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 256]     ;
    vst64   vr18, [i2, 288]     ;

    ;//calculate row10: (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14)
    vaddw   vr18, vr11, vr12    ;
    vaddw   vr19, vr10, vr13    ;
    vaddw   vr20, vr09, vr14    ;
    vaddw   vr21, vr08, vr15    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row11: (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15)
    vaddw   vr18, vr12, vr13    ;
    vaddw   vr19, vr11, vr14    ;
    vaddw   vr20, vr10, vr15    ;
    vaddw   vr21, vr09, vr16    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 320]     ;
    vst64   vr18, [i2, 352]     ;

    ;//calculate row12: (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16)
    vaddw   vr18, vr13, vr14    ;
    vaddw   vr19, vr12, vr15    ;
    vaddw   vr20, vr11, vr16    ;
    vaddw   vr21, vr10, vr17    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row13: (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16)
    vaddw   vr18, vr14, vr15    ;
    vaddw   vr19, vr13, vr16    ;
    vaddw   vr20, vr12, vr17    ;
    vaddw   vr21, vr11, vr17    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 384]     ;
    vst64   vr18, [i2, 416]     ;

    ;//calculate row14: (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15)
    vaddw   vr18, vr15, vr16    ;
    vaddw   vr19, vr14, vr17    ;
    vaddw   vr20, vr13, vr17    ;
    vaddw   vr21, vr12, vr16    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row15: (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14)
    vaddw   vr18, vr16, vr17    ;
    vaddw   vr19, vr15, vr17    ;
    vaddw   vr20, vr14, vr16    ;
    vaddw   vr21, vr13, vr15    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 448]     ;
    vst64   vr18, [i2, 480]     ;
#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a register
    vsubw   vr23, vr23, vr23    ;

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x16_V_LOWPASS_SIMD_   ;
    vrun    r10                                                     ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64w   vr01,   [i1,   0]    ; // src[0]
    vld64w   vr02,   [i1,  32]    ; // src[1]
    vld64w   vr03,   [i1,  64]    ; // src[2]
    vld64w   vr04,   [i1,  96]    ; // src[3]
    vld64w   vr05,   [i1, 128]    ; // src[4]
    vld64w   vr06,   [i1, 160]    ; // src[5]
    vld64w   vr07,   [i1, 192]    ; // src[6]
    vld64w   vr08,   [i1, 224]    ; // src[7]
    vld64w   vr09,   [i1, 256]    ; // src[8]
    vld64w   vr10,   [i1, 288]    ; // src[9]
    vld64w   vr11,   [i1, 320]    ; // src[10]
    vld64w   vr12,   [i1, 352]    ; // src[11]
    vld64w   vr13,   [i1, 384]    ; // src[12]
    vld64w   vr14,   [i1, 416]    ; // src[13]
    vld64w   vr15,   [i1, 448]    ; // src[14]
    vld64w   vr16,   [i1, 480]    ; // src[15]
    vld64w   vr17,   [i1, 512]    ; // src[16]

    // create a mask of (-1)
    vbaddw   vr23, vr23, (-1)   ;

    ;// calculate row0: (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 )
    vaddw   vr18, vr01, vr02    ;
    vaddw   vr19, vr01, vr03    ;
    vaddw   vr20, vr02, vr04    ;
    vaddw   vr21, vr03, vr05    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;// calculate row1: (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 )
    vaddw   vr18, vr02, vr03    ;
    vaddw   vr19, vr01, vr04    ;
    vaddw   vr20, vr01, vr05    ;
    vaddw   vr21, vr02, vr06    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vsubaw  vr18, vr23, vr21    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2,   0]     ;
    vst64   vr18, [i2,  32]     ;
  
    ;// calculate row2: (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 )
    vaddw   vr18, vr03, vr04    ;
    vaddw   vr19, vr02, vr05    ;
    vaddw   vr20, vr01, vr06    ;
    vaddw   vr21, vr01, vr07    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;// calculate row3: (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 )
    vaddw   vr18, vr04, vr05    ;
    vaddw   vr19, vr03, vr06    ;
    vaddw   vr20, vr02, vr07    ;
    vaddw   vr21, vr01, vr08    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vsubaw  vr18, vr23, vr21    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2,  64]     ;
    vst64   vr18, [i2,  96]     ;

    ;// calculate row4: (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 )
    vaddw   vr18, vr05, vr06    ;
    vaddw   vr19, vr04, vr07    ;
    vaddw   vr20, vr03, vr08    ;
    vaddw   vr21, vr02, vr09    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;// calculate row5: (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 )
    vaddw   vr18, vr06, vr07    ;
    vaddw   vr19, vr05, vr08    ;
    vaddw   vr20, vr04, vr09    ;
    vaddw   vr21, vr03, vr10    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vsubaw  vr18, vr23, vr21    ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 128]     ;
    vst64   vr18, [i2, 160]     ;

    ;//calculate row6: (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10)
    vaddw   vr18, vr07, vr08    ;
    vaddw   vr19, vr06, vr09    ;
    vaddw   vr20, vr05, vr10    ;
    vaddw   vr21, vr04, vr11    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;//calculate row7: (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11)
    vaddw   vr18, vr08, vr09    ;
    vaddw   vr19, vr07, vr10    ;
    vaddw   vr20, vr06, vr11    ;
    vaddw   vr21, vr05, vr12    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vsubaw   vr18, vr23, vr21   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 192]     ;
    vst64   vr18, [i2, 224]     ;

    ;//calculate row8: (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12)
    vaddw   vr18, vr09, vr10    ;
    vaddw   vr19, vr08, vr11    ;
    vaddw   vr20, vr07, vr12    ;
    vaddw   vr21, vr06, vr13    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;//calculate row9: (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13)
    vaddw   vr18, vr10, vr11    ;
    vaddw   vr19, vr09, vr12    ;
    vaddw   vr20, vr08, vr13    ;
    vaddw   vr21, vr07, vr14    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vsubaw   vr18, vr23, vr21   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 256]     ;
    vst64   vr18, [i2, 288]     ;

    ;//calculate row10: (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14)
    vaddw   vr18, vr11, vr12    ;
    vaddw   vr19, vr10, vr13    ;
    vaddw   vr20, vr09, vr14    ;
    vaddw   vr21, vr08, vr15    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;//calculate row11: (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15)
    vaddw   vr18, vr12, vr13    ;
    vaddw   vr19, vr11, vr14    ;
    vaddw   vr20, vr10, vr15    ;
    vaddw   vr21, vr09, vr16    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vsubaw   vr18, vr23, vr21   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 320]     ;
    vst64   vr18, [i2, 352]     ;

    ;//calculate row12: (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16)
    vaddw   vr18, vr13, vr14    ;
    vaddw   vr19, vr12, vr15    ;
    vaddw   vr20, vr11, vr16    ;
    vaddw   vr21, vr10, vr17    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;//calculate row13: (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16)
    vaddw   vr18, vr14, vr15    ;
    vaddw   vr19, vr13, vr16    ;
    vaddw   vr20, vr12, vr17    ;
    vaddw   vr21, vr11, vr17    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vsubaw   vr18, vr23, vr21   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 384]     ;
    vst64   vr18, [i2, 416]     ;

    ;//calculate row14: (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15)
    vaddw   vr18, vr15, vr16    ;
    vaddw   vr19, vr14, vr17    ;
    vaddw   vr20, vr13, vr17    ;
    vaddw   vr21, vr12, vr16    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vsubaw   vr22, vr23, vr21   ;

    ;//calculate row15: (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14)
    vaddw   vr18, vr16, vr17    ;
    vaddw   vr19, vr15, vr17    ;
    vaddw   vr20, vr14, vr16    ;
    vaddw   vr21, vr13, vr15    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vsubaw   vr18, vr23, vr21   ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vst64   vr22, [i2, 448]     ;
    vst64   vr18, [i2, 480]     ;
#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}

_Asm void avg_mpeg4_qpel8x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_MPEG4_QPEL8x16_V_LOWPASS_SIMD_   ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_avg_mpeg4_qpel8x16_v_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64w   vr01,   [i1,   0]    ; // src[0]
    vld64w   vr02,   [i1,  32]    ; // src[1]
    vld64w   vr03,   [i1,  64]    ; // src[2]
    vld64w   vr04,   [i1,  96]    ; // src[3]
    vld64w   vr05,   [i1, 128]    ; // src[4]
    vld64w   vr06,   [i1, 160]    ; // src[5]
    vld64w   vr07,   [i1, 192]    ; // src[6]
    vld64w   vr08,   [i1, 224]    ; // src[7]
    vld64w   vr09,   [i1, 256]    ; // src[8]
    vld64w   vr10,   [i1, 288]    ; // src[9]
    vld64w   vr11,   [i1, 320]    ; // src[10]
    vld64w   vr12,   [i1, 352]    ; // src[11]
    vld64w   vr13,   [i1, 384]    ; // src[12]
    vld64w   vr14,   [i1, 416]    ; // src[13]
    vld64w   vr15,   [i1, 448]    ; // src[14]
    vld64w   vr16,   [i1, 480]    ; // src[15]
    vld64w   vr17,   [i1, 512]    ; // src[16]

    ;// calculate row0: (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 )
    vaddw   vr18, vr01, vr02    ;
    vaddw   vr19, vr01, vr03    ;
    vaddw   vr20, vr02, vr04    ;
    vaddw   vr21, vr03, vr05    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row1: (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 )
    vaddw   vr18, vr02, vr03    ;
    vaddw   vr19, vr01, vr04    ;
    vaddw   vr20, vr01, vr05    ;
    vaddw   vr21, vr02, vr06    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vld64   vr19, [i2,   0]     ;
    vld64   vr20, [i2,  32]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2,   0]     ;
    vst64   vr18, [i2,  32]     ;
  
    ;// calculate row2: (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 )
    vaddw   vr18, vr03, vr04    ;
    vaddw   vr19, vr02, vr05    ;
    vaddw   vr20, vr01, vr06    ;
    vaddw   vr21, vr01, vr07    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row3: (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 )
    vaddw   vr18, vr04, vr05    ;
    vaddw   vr19, vr03, vr06    ;
    vaddw   vr20, vr02, vr07    ;
    vaddw   vr21, vr01, vr08    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vld64   vr19, [i2,  64]     ;
    vld64   vr20, [i2,  96]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2,  64]     ;
    vst64   vr18, [i2,  96]     ;

    ;// calculate row4: (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 )
    vaddw   vr18, vr05, vr06    ;
    vaddw   vr19, vr04, vr07    ;
    vaddw   vr20, vr03, vr08    ;
    vaddw   vr21, vr02, vr09    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;// calculate row5: (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 )
    vaddw   vr18, vr06, vr07    ;
    vaddw   vr19, vr05, vr08    ;
    vaddw   vr20, vr04, vr09    ;
    vaddw   vr21, vr03, vr10    ;

    vbmulw  vr18, vr18,   20    ;
    vbmulaw vr18, vr19, (-6)    ;
    vbmulaw vr18, vr20,    3    ;
    vbmulaw vr18, vr21, (-1)    ;

    vld64   vr19, [i2, 128]     ;
    vld64   vr20, [i2, 160]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 128]     ;
    vst64   vr18, [i2, 160]     ;

    ;//calculate row6: (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10)
    vaddw   vr18, vr07, vr08    ;
    vaddw   vr19, vr06, vr09    ;
    vaddw   vr20, vr05, vr10    ;
    vaddw   vr21, vr04, vr11    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row7: (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11)
    vaddw   vr18, vr08, vr09    ;
    vaddw   vr19, vr07, vr10    ;
    vaddw   vr20, vr06, vr11    ;
    vaddw   vr21, vr05, vr12    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vld64   vr19, [i2, 192]     ;
    vld64   vr20, [i2, 224]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 192]     ;
    vst64   vr18, [i2, 224]     ;

    ;//calculate row8: (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12)
    vaddw   vr18, vr09, vr10    ;
    vaddw   vr19, vr08, vr11    ;
    vaddw   vr20, vr07, vr12    ;
    vaddw   vr21, vr06, vr13    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row9: (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13)
    vaddw   vr18, vr10, vr11    ;
    vaddw   vr19, vr09, vr12    ;
    vaddw   vr20, vr08, vr13    ;
    vaddw   vr21, vr07, vr14    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vld64   vr19, [i2, 256]     ;
    vld64   vr20, [i2, 288]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 256]     ;
    vst64   vr18, [i2, 288]     ;

    ;//calculate row10: (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14)
    vaddw   vr18, vr11, vr12    ;
    vaddw   vr19, vr10, vr13    ;
    vaddw   vr20, vr09, vr14    ;
    vaddw   vr21, vr08, vr15    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row11: (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15)
    vaddw   vr18, vr12, vr13    ;
    vaddw   vr19, vr11, vr14    ;
    vaddw   vr20, vr10, vr15    ;
    vaddw   vr21, vr09, vr16    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vld64   vr19, [i2, 320]     ;
    vld64   vr20, [i2, 352]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 320]     ;
    vst64   vr18, [i2, 352]     ;

    ;//calculate row12: (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16)
    vaddw   vr18, vr13, vr14    ;
    vaddw   vr19, vr12, vr15    ;
    vaddw   vr20, vr11, vr16    ;
    vaddw   vr21, vr10, vr17    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row13: (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16)
    vaddw   vr18, vr14, vr15    ;
    vaddw   vr19, vr13, vr16    ;
    vaddw   vr20, vr12, vr17    ;
    vaddw   vr21, vr11, vr17    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vld64   vr19, [i2, 384]     ;
    vld64   vr20, [i2, 416]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 384]     ;
    vst64   vr18, [i2, 416]     ;

    ;//calculate row14: (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15)
    vaddw   vr18, vr15, vr16    ;
    vaddw   vr19, vr14, vr17    ;
    vaddw   vr20, vr13, vr17    ;
    vaddw   vr21, vr12, vr16    ;

    vbmulw   vr22, vr18,  20    ;
    vbmulaw  vr22, vr19, (-6)   ;
    vbmulaw  vr22, vr20,   3    ;
    vbmulaw  vr22, vr21, (-1)   ;

    ;//calculate row15: (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14)
    vaddw   vr18, vr16, vr17    ;
    vaddw   vr19, vr15, vr17    ;
    vaddw   vr20, vr14, vr16    ;
    vaddw   vr21, vr13, vr15    ;

    vbmulw   vr18, vr18,  20    ;
    vbmulaw  vr18, vr19, (-6)   ;
    vbmulaw  vr18, vr20,   3    ;
    vbmulaw  vr18, vr21, (-1)   ;

    vld64   vr19, [i2, 448]     ;
    vld64   vr20, [i2, 480]     ;

    vasrrpwb vr22, vr22, 5      ;
    vasrrpwb vr18, vr18, 5      ;

    vavrb   vr22, vr22, vr19    ;
    vavrb   vr18, vr18, vr20    ;

    vst64   vr22, [i2, 448]     ;
    vst64   vr18, [i2, 480]     ;
#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}


//Use vr22 as increment for i1 and i2
_Asm void put_mpeg4_qpel16x1_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_put_mpeg4_qpel16x1_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64w   vr01,   [i1,   0]    ; // vr01: p07  p06  p05  p04  p03  p02  p01  p00
    vld64w   vr02,   [i1,   8]    ; // vr02: p15  p14  p13  p12  p11  p10  p09  p08
    vld64w   vr03,   [i1,  16]    ; // vr03: XXX  XXX  XXX  XXX  XXX  XXX  XXX  p16

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

    vmr1w    vr12, vr02, vr01   ;   //vr12: p08  p07  p06  p05  p04  p03  p02  p01
    vmr1w    vr13, vr03, vr02   ;   //vr13: p16  p15  p14  p13  p12  p11  p10  p09
    vmr2w    vr14, vr02, vr01   ;   //vr14: p09  p08  p07  p06  p05  p04  p03  p02
    vmr3w    vr16, vr02, vr01   ;   //vr16: p10  p09  p08  p07  p06  p05  p04  p03
    vmr4w    vr18, vr02, vr01   ;   //vr18: p11  p10  p09  p08  p07  p06  p05  p04

    vmr7w    vr07, vr02, vr01   ;   //vr07: p14  p13  p12  p11  p10  p09  p08  p07
    vmr6w    vr09, vr02, vr01   ;   //vr09: p13  p12  p11  p10  p09  p08  p07  p06
    vmr5w    vr11, vr02, vr01   ;   //vr11: p12  p11  p10  p09  p08  p07  p06  p05

    vmr1w    vr15, vr23, vr13   ;   //vr15: 000  p16  p15  p14  p13  p12  p11  p10
    vmvw     vr15, vr13, 0x80   ;   //vr15: p16  p16  p15  p14  p13  p12  p11  p10

    vmr7w    vr06, vr01, vr23   ;   //vr06: p06  p05  p04  p03  p02  p01  p00  000
    vmvw     vr06, vr01,  0x1   ;   //vr06: p06  p05  p04  p03  p02  p01  p00  p00

    // 2 stalls
    vmr1w    vr17, vr23, vr15   ;   //vr17: 000  p16  p16  p15  p14  p13  p12  p11
    vmvw     vr17, vr02, 0x80   ;   //vr17: p15  p16  p16  p15  p14  p13  p12  p11

    // 2 stalls
    vmr7w    vr08, vr06, vr23   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01
    vmvw     vr08, vr12,  0x1   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01

    // 2 stalls
    vmr1w    vr19, vr23, vr17   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12
    vmvw     vr19, vr07, 0x80   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12

    // 2 stalls
    vmr7w    vr10, vr08, vr23   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02
    vmvw     vr10, vr14,  0x1   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02

    vaddw   vr01, vr01, vr12    ;   //vr01: src[0] + src[1]
    vaddw   vr03, vr06, vr14    ;
    vaddw   vr04, vr08, vr16    ;
    vaddw   vr05, vr10, vr18    ;

    vbmulw  vr01, vr01,   20    ;
    vbmulaw vr01, vr03,  (-6)   ;
    vbmulaw vr01, vr04,    3    ;
    vbmulaw vr01, vr05,  (-1)   ;

    vaddw   vr02, vr02, vr13    ;
    vaddw   vr03, vr07, vr15    ;
    vaddw   vr04, vr09, vr17    ;
    vaddw   vr05, vr11, vr19    ;

    vbmulw  vr02, vr02,   20    ;
    vbmulaw vr02, vr03,  (-6)   ;
    vbmulaw vr02, vr04,    3    ;
    vbmulaw vr02, vr05,  (-1)   ;
    
    vasrrpwb vr01, vr01, 5      ;
    vasrrpwb vr02, vr02, 5      ;

    vst64  vr01, [i2,   0]      ;
    vst64  vr02, [i2,   8]      ;

    vbaddw vr00, vr00, 32       ;   //increment the indices by 32

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}



//Use vr22 as increment for i1 and i2
_Asm void put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //create a mask of (-1)
    vmovzw   vr23, (-1), 0xff   ;

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif
    // load input
    vld64w   vr01,   [i1,   0]    ; // vr01: p07  p06  p05  p04  p03  p02  p01  p00
    vld64w   vr02,   [i1,   8]    ; // vr02: p15  p14  p13  p12  p11  p10  p09  p08
    vld64w   vr03,   [i1,  16]    ; // vr03: XXX  XXX  XXX  XXX  XXX  XXX  XXX  p16

    vmr1w    vr12, vr02, vr01   ;   //vr12: p08  p07  p06  p05  p04  p03  p02  p01
    vmr1w    vr13, vr03, vr02   ;   //vr13: p16  p15  p14  p13  p12  p11  p10  p09
    vmr2w    vr14, vr02, vr01   ;   //vr14: p09  p08  p07  p06  p05  p04  p03  p02
    vmr3w    vr16, vr02, vr01   ;   //vr16: p10  p09  p08  p07  p06  p05  p04  p03
    vmr4w    vr18, vr02, vr01   ;   //vr18: p11  p10  p09  p08  p07  p06  p05  p04

    vmr7w    vr07, vr02, vr01   ;   //vr07: p14  p13  p12  p11  p10  p09  p08  p07
    vmr6w    vr09, vr02, vr01   ;   //vr09: p13  p12  p11  p10  p09  p08  p07  p06
    vmr5w    vr11, vr02, vr01   ;   //vr11: p12  p11  p10  p09  p08  p07  p06  p05

    vmr1w    vr15, vr23, vr13   ;   //vr15:  -1  p16  p15  p14  p13  p12  p11  p10
    vmvw     vr15, vr13, 0x80   ;   //vr15: p16  p16  p15  p14  p13  p12  p11  p10

    vmr7w    vr06, vr01, vr23   ;   //vr06: p06  p05  p04  p03  p02  p01  p00   -1
    vmvw     vr06, vr01,  0x1   ;   //vr06: p06  p05  p04  p03  p02  p01  p00  p00

    // 2 stalls
    vmr1w    vr17, vr23, vr15   ;   //vr17:  -1  p16  p16  p15  p14  p13  p12  p11
    vmvw     vr17, vr02, 0x80   ;   //vr17: p15  p16  p16  p15  p14  p13  p12  p11

    // 2 stalls
    vmr7w    vr08, vr06, vr23   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01
    vmvw     vr08, vr12,  0x1   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01

    // 2 stalls
    vmr1w    vr19, vr23, vr17   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12
    vmvw     vr19, vr07, 0x80   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12

    // 2 stalls
    vmr7w    vr10, vr08, vr23   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02
    vmvw     vr10, vr14,  0x1   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02

    vaddw   vr01, vr01, vr12    ;   //vr01: src[0] + src[1]
    vaddw   vr03, vr06, vr14    ;
    vaddw   vr04, vr08, vr16    ;
    vaddw   vr05, vr10, vr18    ;

    vbmulw  vr01, vr01,   20    ;
    vbmulaw vr01, vr03,  (-6)   ;
    vbmulaw vr01, vr04,    3    ;
    vsubaw  vr01, vr23,  vr05   ;
//    vbmulaw vr01, vr05,  (-1)   ;

    vaddw   vr02, vr02, vr13    ;
    vaddw   vr03, vr07, vr15    ;
    vaddw   vr04, vr09, vr17    ;
    vaddw   vr05, vr11, vr19    ;

    vbmulw  vr02, vr02,   20    ;
    vbmulaw vr02, vr03,  (-6)   ;
    vbmulaw vr02, vr04,    3    ;
    vsubaw  vr02, vr23,  vr05   ;
//    vbmulaw vr02, vr05,  (-1)   ;
    
    vasrrpwb vr01, vr01, 5      ;
    vasrrpwb vr02, vr02, 5      ;

    vst64  vr01, [i2,   0]      ;
    vst64  vr02, [i2,   8]      ;

    vbaddw vr00, vr00, 32       ;   //increment the indices by 32

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}


//Use vr22 as increment for i1 and i2
_Asm void avg_mpeg4_qpel16x1_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_avg_mpeg4_qpel16x1_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif
    // load input
    vld64w   vr01,   [i1,   0]    ; // vr01: p07  p06  p05  p04  p03  p02  p01  p00
    vld64w   vr02,   [i1,   8]    ; // vr02: p15  p14  p13  p12  p11  p10  p09  p08
    vld64w   vr03,   [i1,  16]    ; // vr03: XXX  XXX  XXX  XXX  XXX  XXX  XXX  p16

    vmr1w    vr12, vr02, vr01   ;   //vr12: p08  p07  p06  p05  p04  p03  p02  p01
    vmr1w    vr13, vr03, vr02   ;   //vr13: p16  p15  p14  p13  p12  p11  p10  p09
    vmr2w    vr14, vr02, vr01   ;   //vr14: p09  p08  p07  p06  p05  p04  p03  p02
    vmr3w    vr16, vr02, vr01   ;   //vr16: p10  p09  p08  p07  p06  p05  p04  p03
    vmr4w    vr18, vr02, vr01   ;   //vr18: p11  p10  p09  p08  p07  p06  p05  p04

    vmr7w    vr07, vr02, vr01   ;   //vr07: p14  p13  p12  p11  p10  p09  p08  p07
    vmr6w    vr09, vr02, vr01   ;   //vr09: p13  p12  p11  p10  p09  p08  p07  p06
    vmr5w    vr11, vr02, vr01   ;   //vr11: p12  p11  p10  p09  p08  p07  p06  p05

    vmr1w    vr15, vr23, vr13   ;   //vr15:  -1  p16  p15  p14  p13  p12  p11  p10
    vmvw     vr15, vr13, 0x80   ;   //vr15: p16  p16  p15  p14  p13  p12  p11  p10

    vmr7w    vr06, vr01, vr23   ;   //vr06: p06  p05  p04  p03  p02  p01  p00   -1
    vmvw     vr06, vr01,  0x1   ;   //vr06: p06  p05  p04  p03  p02  p01  p00  p00

    // 2 stalls
    vmr1w    vr17, vr23, vr15   ;   //vr17:  -1  p16  p16  p15  p14  p13  p12  p11
    vmvw     vr17, vr02, 0x80   ;   //vr17: p15  p16  p16  p15  p14  p13  p12  p11

    // 2 stalls
    vmr7w    vr08, vr06, vr23   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01
    vmvw     vr08, vr12,  0x1   ;   //vr08: p05  p04  p03  p02  p01  p00  p00  p01

    // 2 stalls
    vmr1w    vr19, vr23, vr17   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12
    vmvw     vr19, vr07, 0x80   ;   //vr19: p14  p15  p16  p16  p15  p14  p13  p12

    // 2 stalls
    vmr7w    vr10, vr08, vr23   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02
    vmvw     vr10, vr14,  0x1   ;   //vr10: p04  p03  p02  p01  p00  p00  p01  p02

    vaddw   vr01, vr01, vr12    ;   //vr01: src[0] + src[1]
    vaddw   vr03, vr06, vr14    ;
    vaddw   vr04, vr08, vr16    ;
    vaddw   vr05, vr10, vr18    ;

    vbmulw  vr01, vr01,   20    ;
    vbmulaw vr01, vr03,  (-6)   ;
    vbmulaw vr01, vr04,    3    ;
    vbmulaw vr01, vr05,  (-1)   ;

    //load dst
    vld64   vr06, [i2,  0]      ;
    vld64   vr08, [i2,  8]      ;
    
    vaddw   vr02, vr02, vr13    ;
    vaddw   vr03, vr07, vr15    ;
    vaddw   vr04, vr09, vr17    ;
    vaddw   vr05, vr11, vr19    ;

    vbmulw  vr02, vr02,   20    ;
    vbmulaw vr02, vr03,  (-6)   ;
    vbmulaw vr02, vr04,    3    ;
    vbmulaw vr02, vr05,  (-1)   ;
    
    vasrrpwb vr01, vr01, 5      ;
    vasrrpwb vr02, vr02, 5      ;

    vavrb  vr01, vr01, vr06     ;
    vavrb  vr02, vr02, vr08     ;
    
    vst64  vr01, [i2,   0]      ;
    vst64  vr02, [i2,   8]      ;

    vbaddw vr00, vr00, 32       ;   //increment the indices by 32

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}

#if defined(AURORA_16_H_FILTER_MACROS)

_Asm void put_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst
    mov     r10, _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8

.on     percent_reg
}

_Asm void put_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst
    mov     r10, _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9

.on     percent_reg
}

_Asm void put_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst
    mov     r10, _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16

.on     percent_reg
}

_Asm void put_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst
    mov     r10, _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16
    vrun    r10  ;  //row-17

.on     percent_reg
}


_Asm void put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //create a mask of (-1)
    vmovzw   vr23, (-1), 0xff   ;

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8

.on     percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //create a mask of (-1)
    vmovzw   vr23, (-1), 0xff   ;

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9

.on     percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //create a mask of (-1)
    vmovzw   vr23, (-1), 0xff   ;

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16

.on     percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //create a mask of (-1)
    vmovzw   vr23, (-1), 0xff   ;

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16
    vrun    r10  ;  //row-17

.on     percent_reg
}


_Asm void avg_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

    mov     r10, _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8

.on     percent_reg
}

_Asm void avg_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

    mov     r10, _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9

.on     percent_reg
}

_Asm void avg_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

    mov     r10, _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16

.on     percent_reg
}

_Asm void avg_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a dummy register
    vsubw    vr23, vr23, vr23   ;

    mov     r10, _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_   ;

    vrun    r10  ;  //row-1
    vrun    r10  ;  //row-2
    vrun    r10  ;  //row-3
    vrun    r10  ;  //row-4
    vrun    r10  ;  //row-5
    vrun    r10  ;  //row-6
    vrun    r10  ;  //row-7
    vrun    r10  ;  //row-8
    vrun    r10  ;  //row-9
    vrun    r10  ;  //row-10
    vrun    r10  ;  //row-11
    vrun    r10  ;  //row-12
    vrun    r10  ;  //row-13
    vrun    r10  ;  //row-14
    vrun    r10  ;  //row-15
    vrun    r10  ;  //row-16
    vrun    r10  ;  //row-17

.on     percent_reg
}


#endif

#endif
