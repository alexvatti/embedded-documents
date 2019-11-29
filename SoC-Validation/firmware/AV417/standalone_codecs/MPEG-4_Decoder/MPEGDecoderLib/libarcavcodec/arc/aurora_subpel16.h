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

#ifndef _AURORA_SUBPEL16_H_
#define _AURORA_SUBPEL16_H_

// src buffer stride = 32
// dst buffer stride = 32


_Asm void put_pixels16x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x8_SIMD_   ;
    vrun    r10                               ;
.on     percent_reg
}

_Asm int macro_put_pixels16x8_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels16x8_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x8_X2_SIMD_   ;
    vrun    r10                               ;
.on     percent_reg
}

_Asm int macro_put_pixels16x8_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;


    vld128   vr11,   [i1,  16]    ;
    vld128   vr12,   [i1,  48]    ;
    vld128   vr13,   [i1,  80]    ;
    vld128   vr14,   [i1, 112]    ;
    vld128   vr15,   [i1, 144]    ;
    vld128   vr16,   [i1, 176]    ;
    vld128   vr17,   [i1, 208]    ;
    vld128   vr18,   [i1, 240]    ;

    // shift input
    vmrb    vr11,   vr11,   vr01  ;
    vmrb    vr12,   vr12,   vr02  ;
    vmrb    vr13,   vr13,   vr03  ;
    vmrb    vr14,   vr14,   vr04  ;
    vmrb    vr15,   vr15,   vr05  ;
    vmrb    vr16,   vr16,   vr06  ;
    vmrb    vr17,   vr17,   vr07  ;
    vmrb    vr18,   vr18,   vr08  ;

    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels16x8_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x8_Y2_SIMD_   ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels16x8_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;
    vld128   vr09,   [i1, 256]    ;

    // average with rounding
    vavrb   vr01,   vr01,   vr02    ;
    vavrb   vr02,   vr02,   vr03    ;
    vavrb   vr03,   vr03,   vr04    ;
    vavrb   vr04,   vr04,   vr05    ;
    vavrb   vr05,   vr05,   vr06    ;
    vavrb   vr06,   vr06,   vr07    ;
    vavrb   vr07,   vr07,   vr08    ;
    vavrb   vr08,   vr08,   vr09    ;


    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;


#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels16x8_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS16x8_XY2_SIMD_   ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels16x8_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;
    vld64w   vr06,   [i1, 160]   ;
    vld64w   vr07,   [i1, 192]   ;
    vld64w   vr08,   [i1, 224]   ;
    vld64w   vr09,   [i1, 256]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;
    vld64w   vr16,   [i1, 168]   ;
    vld64w   vr17,   [i1, 200]   ;
    vld64w   vr18,   [i1, 232]   ;
    vld64w   vr19,   [i1, 264]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;
    vasrrpwb vr05,  vr05,  2    ;
    vasrrpwb vr06,  vr06,  2    ;
    vasrrpwb vr07,  vr07,  2    ;
    vasrrpwb vr08,  vr08,  2    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;


    // load input
    vld64w   vr01,   [i1,   8]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  40]   ;
    vld64w   vr03,   [i1,  72]   ;
    vld64w   vr04,   [i1, 104]   ;
    vld64w   vr05,   [i1, 136]   ;
    vld64w   vr06,   [i1, 168]   ;
    vld64w   vr07,   [i1, 200]   ;
    vld64w   vr08,   [i1, 232]   ;
    vld64w   vr09,   [i1, 264]   ;

    vld64w   vr11,   [i1,  16]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  48]   ;
    vld64w   vr13,   [i1,  80]   ;
    vld64w   vr14,   [i1, 112]   ;
    vld64w   vr15,   [i1, 144]   ;
    vld64w   vr16,   [i1, 176]   ;
    vld64w   vr17,   [i1, 208]   ;
    vld64w   vr18,   [i1, 240]   ;
    vld64w   vr19,   [i1, 272]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;
    vasrrpwb vr05,  vr05,  2    ;
    vasrrpwb vr06,  vr06,  2    ;
    vasrrpwb vr07,  vr07,  2    ;
    vasrrpwb vr08,  vr08,  2    ;

    // store result
    vst64   vr01,   [i2,   8]    ;
    vst64   vr02,   [i2,  40]    ;
    vst64   vr03,   [i2,  72]    ;
    vst64   vr04,   [i2, 104]    ;
    vst64   vr05,   [i2, 136]    ;
    vst64   vr06,   [i2, 168]    ;
    vst64   vr07,   [i2, 200]    ;
    vst64   vr08,   [i2, 232]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}



_Asm void put_no_rnd_pixels16x8_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS16x8_X2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels16x8_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;


    vld128   vr11,   [i1,  16]    ;
    vld128   vr12,   [i1,  48]    ;
    vld128   vr13,   [i1,  80]    ;
    vld128   vr14,   [i1, 112]    ;
    vld128   vr15,   [i1, 144]    ;
    vld128   vr16,   [i1, 176]    ;
    vld128   vr17,   [i1, 208]    ;
    vld128   vr18,   [i1, 240]    ;

    // shift input
    vmrb    vr11,   vr11,   vr01  ;
    vmrb    vr12,   vr12,   vr02  ;
    vmrb    vr13,   vr13,   vr03  ;
    vmrb    vr14,   vr14,   vr04  ;
    vmrb    vr15,   vr15,   vr05  ;
    vmrb    vr16,   vr16,   vr06  ;
    vmrb    vr17,   vr17,   vr07  ;
    vmrb    vr18,   vr18,   vr08  ;

    // average without rounding
    vavb   vr01,   vr01,   vr11    ;
    vavb   vr02,   vr02,   vr12    ;
    vavb   vr03,   vr03,   vr13    ;
    vavb   vr04,   vr04,   vr14    ;
    vavb   vr05,   vr05,   vr15    ;
    vavb   vr06,   vr06,   vr16    ;
    vavb   vr07,   vr07,   vr17    ;
    vavb   vr08,   vr08,   vr18    ;

    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_no_rnd_pixels16x8_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS16x8_Y2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels16x8_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;
    vld128   vr09,   [i1, 256]    ;

    // average with rounding
    vavb   vr01,   vr01,   vr02    ;
    vavb   vr02,   vr02,   vr03    ;
    vavb   vr03,   vr03,   vr04    ;
    vavb   vr04,   vr04,   vr05    ;
    vavb   vr05,   vr05,   vr06    ;
    vavb   vr06,   vr06,   vr07    ;
    vavb   vr07,   vr07,   vr08    ;
    vavb   vr08,   vr08,   vr09    ;


    // store result
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_no_rnd_pixels16x8_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS16x8_XY2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels16x8_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // store (1)
    vmovzw   vr21, 1, 0xff   ; //CLARIFY
    //vsubw    vr21, vr21, vr21   ;
    //vbaddw   vr21, vr21, 1   ;

    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;
    vld64w   vr06,   [i1, 160]   ;
    vld64w   vr07,   [i1, 192]   ;
    vld64w   vr08,   [i1, 224]   ;
    vld64w   vr09,   [i1, 256]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;
    vld64w   vr16,   [i1, 168]   ;
    vld64w   vr17,   [i1, 200]   ;
    vld64w   vr18,   [i1, 232]   ;
    vld64w   vr19,   [i1, 264]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    //add '1'
    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;
    vaddw    vr05,  vr05, vr21  ;
    vaddw    vr06,  vr06, vr21  ;
    vaddw    vr07,  vr07, vr21  ;
    vaddw    vr08,  vr08, vr21  ;

    // shift and pack
    vasrpwb vr01,  vr01,  2    ;
    vasrpwb vr02,  vr02,  2    ;
    vasrpwb vr03,  vr03,  2    ;
    vasrpwb vr04,  vr04,  2    ;
    vasrpwb vr05,  vr05,  2    ;
    vasrpwb vr06,  vr06,  2    ;
    vasrpwb vr07,  vr07,  2    ;
    vasrpwb vr08,  vr08,  2    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;


    // load input
    vld64w   vr01,   [i1,   8]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  40]   ;
    vld64w   vr03,   [i1,  72]   ;
    vld64w   vr04,   [i1, 104]   ;
    vld64w   vr05,   [i1, 136]   ;
    vld64w   vr06,   [i1, 168]   ;
    vld64w   vr07,   [i1, 200]   ;
    vld64w   vr08,   [i1, 232]   ;
    vld64w   vr09,   [i1, 264]   ;

    vld64w   vr11,   [i1,  16]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  48]   ;
    vld64w   vr13,   [i1,  80]   ;
    vld64w   vr14,   [i1, 112]   ;
    vld64w   vr15,   [i1, 144]   ;
    vld64w   vr16,   [i1, 176]   ;
    vld64w   vr17,   [i1, 208]   ;
    vld64w   vr18,   [i1, 240]   ;
    vld64w   vr19,   [i1, 272]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    //add '1'
    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;
    vaddw    vr05,  vr05, vr21  ;
    vaddw    vr06,  vr06, vr21  ;
    vaddw    vr07,  vr07, vr21  ;
    vaddw    vr08,  vr08, vr21  ;

    // shift and pack
    vasrpwb vr01,  vr01,  2    ;
    vasrpwb vr02,  vr02,  2    ;
    vasrpwb vr03,  vr03,  2    ;
    vasrpwb vr04,  vr04,  2    ;
    vasrpwb vr05,  vr05,  2    ;
    vasrpwb vr06,  vr06,  2    ;
    vasrpwb vr07,  vr07,  2    ;
    vasrpwb vr08,  vr08,  2    ;

    // store result
    vst64   vr01,   [i2,   8]    ;
    vst64   vr02,   [i2,  40]    ;
    vst64   vr03,   [i2,  72]    ;
    vst64   vr04,   [i2, 104]    ;
    vst64   vr05,   [i2, 136]    ;
    vst64   vr06,   [i2, 168]    ;
    vst64   vr07,   [i2, 200]    ;
    vst64   vr08,   [i2, 232]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels16x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS16x8_SIMD_   ;
    vrun    r10                                ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x8_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;

    // load dst
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;


    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_pixels16x8_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS16x8_X2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x8_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;


    vld128   vr11,   [i1,  16]    ;
    vld128   vr12,   [i1,  48]    ;
    vld128   vr13,   [i1,  80]    ;
    vld128   vr14,   [i1, 112]    ;
    vld128   vr15,   [i1, 144]    ;
    vld128   vr16,   [i1, 176]    ;
    vld128   vr17,   [i1, 208]    ;
    vld128   vr18,   [i1, 240]    ;

    // shift input
    vmrb    vr11,   vr11,   vr01  ;
    vmrb    vr12,   vr12,   vr02  ;
    vmrb    vr13,   vr13,   vr03  ;
    vmrb    vr14,   vr14,   vr04  ;
    vmrb    vr15,   vr15,   vr05  ;
    vmrb    vr16,   vr16,   vr06  ;
    vmrb    vr17,   vr17,   vr07  ;
    vmrb    vr18,   vr18,   vr08  ;

    // average with rounding of inp
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;


    // load dst
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;


#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_pixels16x8_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS16x8_Y2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x8_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;
    vld128   vr09,   [i1, 256]    ;

    // average with rounding (of inp)
    vavrb   vr01,   vr01,   vr02    ;
    vavrb   vr02,   vr02,   vr03    ;
    vavrb   vr03,   vr03,   vr04    ;
    vavrb   vr04,   vr04,   vr05    ;
    vavrb   vr05,   vr05,   vr06    ;
    vavrb   vr06,   vr06,   vr07    ;
    vavrb   vr07,   vr07,   vr08    ;
    vavrb   vr08,   vr08,   vr09    ;

    // load dst
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;


#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels16x8_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS16x8_XY2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_pixels16x8_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;
    vld64w   vr06,   [i1, 160]   ;
    vld64w   vr07,   [i1, 192]   ;
    vld64w   vr08,   [i1, 224]   ;
    vld64w   vr09,   [i1, 256]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;
    vld64w   vr16,   [i1, 168]   ;
    vld64w   vr17,   [i1, 200]   ;
    vld64w   vr18,   [i1, 232]   ;
    vld64w   vr19,   [i1, 264]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;
    vasrrpwb vr05,  vr05,  2    ;
    vasrrpwb vr06,  vr06,  2    ;
    vasrrpwb vr07,  vr07,  2    ;
    vasrrpwb vr08,  vr08,  2    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;

    // load input
    vld64w   vr01,   [i1,   8]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  40]   ;
    vld64w   vr03,   [i1,  72]   ;
    vld64w   vr04,   [i1, 104]   ;
    vld64w   vr05,   [i1, 136]   ;
    vld64w   vr06,   [i1, 168]   ;
    vld64w   vr07,   [i1, 200]   ;
    vld64w   vr08,   [i1, 232]   ;
    vld64w   vr09,   [i1, 264]   ;

    vld64w   vr11,   [i1,  16]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  48]   ;
    vld64w   vr13,   [i1,  80]   ;
    vld64w   vr14,   [i1, 112]   ;
    vld64w   vr15,   [i1, 144]   ;
    vld64w   vr16,   [i1, 176]   ;
    vld64w   vr17,   [i1, 208]   ;
    vld64w   vr18,   [i1, 240]   ;
    vld64w   vr19,   [i1, 272]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;
    vasrrpwb vr05,  vr05,  2    ;
    vasrrpwb vr06,  vr06,  2    ;
    vasrrpwb vr07,  vr07,  2    ;
    vasrrpwb vr08,  vr08,  2    ;

    // load dst
    vld64   vr11,   [i2,   8]    ;
    vld64   vr12,   [i2,  40]    ;
    vld64   vr13,   [i2,  72]    ;
    vld64   vr14,   [i2, 104]    ;
    vld64   vr15,   [i2, 136]    ;
    vld64   vr16,   [i2, 168]    ;
    vld64   vr17,   [i2, 200]    ;
    vld64   vr18,   [i2, 232]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store result
    vst64   vr01,   [i2,   8]    ;
    vst64   vr02,   [i2,  40]    ;
    vst64   vr03,   [i2,  72]    ;
    vst64   vr04,   [i2, 104]    ;
    vst64   vr05,   [i2, 136]    ;
    vst64   vr06,   [i2, 168]    ;
    vst64   vr07,   [i2, 200]    ;
    vst64   vr08,   [i2, 232]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_no_rnd_pixels16x8_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2


#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS16x8_X2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels16x8_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;


    vld128   vr11,   [i1,  16]    ;
    vld128   vr12,   [i1,  48]    ;
    vld128   vr13,   [i1,  80]    ;
    vld128   vr14,   [i1, 112]    ;
    vld128   vr15,   [i1, 144]    ;
    vld128   vr16,   [i1, 176]    ;
    vld128   vr17,   [i1, 208]    ;
    vld128   vr18,   [i1, 240]    ;

    // shift input
    vmrb    vr11,   vr11,   vr01  ;
    vmrb    vr12,   vr12,   vr02  ;
    vmrb    vr13,   vr13,   vr03  ;
    vmrb    vr14,   vr14,   vr04  ;
    vmrb    vr15,   vr15,   vr05  ;
    vmrb    vr16,   vr16,   vr06  ;
    vmrb    vr17,   vr17,   vr07  ;
    vmrb    vr18,   vr18,   vr08  ;

    // average without rounding (of inp)
    vavb   vr01,   vr01,   vr11    ;
    vavb   vr02,   vr02,   vr12    ;
    vavb   vr03,   vr03,   vr13    ;
    vavb   vr04,   vr04,   vr14    ;
    vavb   vr05,   vr05,   vr15    ;
    vavb   vr06,   vr06,   vr16    ;
    vavb   vr07,   vr07,   vr17    ;
    vavb   vr08,   vr08,   vr18    ;

    // load dst
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;


    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_no_rnd_pixels16x8_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS16x8_Y2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels16x8_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  32]    ;
    vld128   vr03,   [i1,  64]    ;
    vld128   vr04,   [i1,  96]    ;
    vld128   vr05,   [i1, 128]    ;
    vld128   vr06,   [i1, 160]    ;
    vld128   vr07,   [i1, 192]    ;
    vld128   vr08,   [i1, 224]    ;
    vld128   vr09,   [i1, 256]    ;

    // average with rounding
    vavb   vr01,   vr01,   vr02    ;
    vavb   vr02,   vr02,   vr03    ;
    vavb   vr03,   vr03,   vr04    ;
    vavb   vr04,   vr04,   vr05    ;
    vavb   vr05,   vr05,   vr06    ;
    vavb   vr06,   vr06,   vr07    ;
    vavb   vr07,   vr07,   vr08    ;
    vavb   vr08,   vr08,   vr09    ;

    // load dst
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  32]    ;
    vld128   vr13,   [i2,  64]    ;
    vld128   vr14,   [i2,  96]    ;
    vld128   vr15,   [i2, 128]    ;
    vld128   vr16,   [i2, 160]    ;
    vld128   vr17,   [i2, 192]    ;
    vld128   vr18,   [i2, 224]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store at dst
    vst128   vr01,   [i2,   0]    ;
    vst128   vr02,   [i2,  32]    ;
    vst128   vr03,   [i2,  64]    ;
    vst128   vr04,   [i2,  96]    ;
    vst128   vr05,   [i2, 128]    ;
    vst128   vr06,   [i2, 160]    ;
    vst128   vr07,   [i2, 192]    ;
    vst128   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_no_rnd_pixels8x8_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS8x8_XY2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels8x8_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // store (1)
    vmovzw   vr21, 1, 0xff   ; //CLARIFY!!
    //vsubw    vr21, vr21, vr21   ;
    //vbaddw   vr21, vr21, 1      ;

    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;
    vld64w   vr06,   [i1, 160]   ;
    vld64w   vr07,   [i1, 192]   ;
    vld64w   vr08,   [i1, 224]   ;
    vld64w   vr09,   [i1, 256]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;
    vld64w   vr16,   [i1, 168]   ;
    vld64w   vr17,   [i1, 200]   ;
    vld64w   vr18,   [i1, 232]   ;
    vld64w   vr19,   [i1, 264]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    //add '1'
    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;
    vaddw    vr05,  vr05, vr21  ;
    vaddw    vr06,  vr06, vr21  ;
    vaddw    vr07,  vr07, vr21  ;
    vaddw    vr08,  vr08, vr21  ;

    // shift and pack
    vasrpwb vr01,  vr01,  2    ;
    vasrpwb vr02,  vr02,  2    ;
    vasrpwb vr03,  vr03,  2    ;
    vasrpwb vr04,  vr04,  2    ;
    vasrpwb vr05,  vr05,  2    ;
    vasrpwb vr06,  vr06,  2    ;
    vasrpwb vr07,  vr07,  2    ;
    vasrpwb vr08,  vr08,  2    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;

// process only one column at a time
#if 0   
    // load input
    vld64w   vr01,   [i1,   8]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  40]   ;
    vld64w   vr03,   [i1,  72]   ;
    vld64w   vr04,   [i1, 104]   ;
    vld64w   vr05,   [i1, 136]   ;
    vld64w   vr06,   [i1, 168]   ;
    vld64w   vr07,   [i1, 200]   ;
    vld64w   vr08,   [i1, 232]   ;
    vld64w   vr09,   [i1, 264]   ;

    vld64w   vr11,   [i1,  16]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  48]   ;
    vld64w   vr13,   [i1,  80]   ;
    vld64w   vr14,   [i1, 112]   ;
    vld64w   vr15,   [i1, 144]   ;
    vld64w   vr16,   [i1, 176]   ;
    vld64w   vr17,   [i1, 208]   ;
    vld64w   vr18,   [i1, 240]   ;
    vld64w   vr19,   [i1, 272]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;
    vmr1w    vr16,  vr16,  vr06  ;
    vmr1w    vr17,  vr17,  vr07  ;
    vmr1w    vr18,  vr18,  vr08  ;
    vmr1w    vr19,  vr19,  vr09  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr05,  vr05,  vr15  ;
    vaddaw   vr05,  vr06,  vr16  ;

    vaddw    vr06,  vr06,  vr16  ;
    vaddaw   vr06,  vr07,  vr17  ;

    vaddw    vr07,  vr07,  vr17  ;
    vaddaw   vr07,  vr08,  vr18  ;

    vaddw    vr08,  vr08,  vr18  ;
    vaddaw   vr08,  vr09,  vr19  ;

    //add '1'
    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;
    vaddw    vr05,  vr05, vr21  ;
    vaddw    vr06,  vr06, vr21  ;
    vaddw    vr07,  vr07, vr21  ;
    vaddw    vr08,  vr08, vr21  ;

    // shift and pack
    vasrpwb vr01,  vr01,  2    ;
    vasrpwb vr02,  vr02,  2    ;
    vasrpwb vr03,  vr03,  2    ;
    vasrpwb vr04,  vr04,  2    ;
    vasrpwb vr05,  vr05,  2    ;
    vasrpwb vr06,  vr06,  2    ;
    vasrpwb vr07,  vr07,  2    ;
    vasrpwb vr08,  vr08,  2    ;

    // load dst
    vld64   vr11,   [i2,   8]    ;
    vld64   vr12,   [i2,  40]    ;
    vld64   vr13,   [i2,  72]    ;
    vld64   vr14,   [i2, 104]    ;
    vld64   vr15,   [i2, 136]    ;
    vld64   vr16,   [i2, 168]    ;
    vld64   vr17,   [i2, 200]    ;
    vld64   vr18,   [i2, 232]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;

    // store result
    vst64   vr01,   [i2,   8]    ;
    vst64   vr02,   [i2,  40]    ;
    vst64   vr03,   [i2,  72]    ;
    vst64   vr04,   [i2, 104]    ;
    vst64   vr05,   [i2, 136]    ;
    vst64   vr06,   [i2, 168]    ;
    vst64   vr07,   [i2, 200]    ;
    vst64   vr08,   [i2, 232]    ;
#endif

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


#endif

