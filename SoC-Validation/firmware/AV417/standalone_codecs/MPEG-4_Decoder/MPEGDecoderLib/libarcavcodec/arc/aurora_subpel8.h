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

#ifndef _AURORA_SUBPEL8_H_
#define _AURORA_SUBPEL8_H_

// src buffer stride = 32
// dst buffer stride = 32


_Asm void put_pixels8x4_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x4_SIMD_   ;
    vrun    r10                               ;
.on     percent_reg
}

_Asm int macro_put_pixels8x4_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input
    vld64   vr01,   [i1,   0]   ;
    vld64   vr02,   [i1,  32]   ;
    vld64   vr03,   [i1,  64]   ;
    vld64   vr04,   [i1,  96]   ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}

_Asm void put_pixels8x4_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x4_X2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels8x4_x2_simd(int scm_addr)
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

    // shift input
    vsr8    vr11,   vr01,   1   ;
    vsr8    vr12,   vr02,   1   ;
    vsr8    vr13,   vr03,   1   ;
    vsr8    vr14,   vr04,   1   ;

    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on     percent_reg
}

_Asm void put_pixels8x4_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x4_Y2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels8x4_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input
    vld64   vr01,   [i1,   0]   ;
    vld64   vr02,   [i1,  32]   ;
    vld64   vr03,   [i1,  64]   ;
    vld64   vr04,   [i1,  96]   ;
    vld64   vr05,   [i1, 128]   ;

    // average with rounding
    vavrb   vr01,   vr01,   vr02    ;
    vavrb   vr02,   vr02,   vr03    ;
    vavrb   vr03,   vr03,   vr04    ;
    vavrb   vr04,   vr04,   vr05    ;


    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels8x4_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x4_XY2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels8x4_xy2_simd(int scm_addr)
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

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}



_Asm void put_no_rnd_pixels8x4_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS8x4_X2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels8x4_x2_simd(int scm_addr)
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

    // shift input
    vsr8    vr11,   vr01,   1   ;
    vsr8    vr12,   vr02,   1   ;
    vsr8    vr13,   vr03,   1   ;
    vsr8    vr14,   vr04,   1   ;

    // avg without rounding
    vavb   vr01,   vr01,   vr11    ;
    vavb   vr02,   vr02,   vr12    ;
    vavb   vr03,   vr03,   vr13    ;
    vavb   vr04,   vr04,   vr14    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_no_rnd_pixels8x4_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS8x4_Y2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels8x4_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input
    vld64   vr01,   [i1,   0]   ;
    vld64   vr02,   [i1,  32]   ;
    vld64   vr03,   [i1,  64]   ;
    vld64   vr04,   [i1,  96]   ;
    vld64   vr05,   [i1, 128]   ;

    // average with rounding
    vavb   vr01,   vr01,   vr02    ;
    vavb   vr02,   vr02,   vr03    ;
    vavb   vr03,   vr03,   vr04    ;
    vavb   vr04,   vr04,   vr05    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_no_rnd_pixels8x4_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS8x4_XY2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels8x4_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // store (1)
    //vmovzw   vr21, 1, 0xff   ; CLARIFY
    vsubw    vr21, vr21, vr21   ;
    vbaddw   vr21, vr21, 1   ;

    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;

    // shift and pack
    vasrpwb  vr01,  vr01,  2    ;
    vasrpwb  vr02,  vr02,  2    ;
    vasrpwb  vr03,  vr03,  2    ;
    vasrpwb  vr04,  vr04,  2    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels8x4_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x4_SIMD_   ;
    vrun    r10                               ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x4_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input - src
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store at dst
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_pixels8x4_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x4_X2_SIMD_   ;
    vrun    r10                                  ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x4_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input - src
    vld128  vr01,   [i1,   0]    ;
    vld128  vr02,   [i1,  32]    ;
    vld128  vr03,   [i1,  64]    ;
    vld128  vr04,   [i1,  96]    ;

    // shift input
    vsr8    vr11,   vr01,   1   ;
    vsr8    vr12,   vr02,   1   ;
    vsr8    vr13,   vr03,   1   ;
    vsr8    vr14,   vr04,   1   ;

    //avg with rounding (of inp)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store at dst
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_pixels8x4_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x4_Y2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x4_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input - src
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;

    //avg with rounding (of inp)
    vavrb   vr01,   vr01,   vr02    ;
    vavrb   vr02,   vr02,   vr03    ;
    vavrb   vr03,   vr03,   vr04    ;
    vavrb   vr04,   vr04,   vr05    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store at dst
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels8x4_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst


#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x4_XY2_SIMD_   ;
    vrun    r10                                   ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x4_xy2_simd(int scm_addr)
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

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    // shift, round and pack
    vasrrpwb vr01,  vr01,  2    ;
    vasrrpwb vr02,  vr02,  2    ;
    vasrrpwb vr03,  vr03,  2    ;
    vasrrpwb vr04,  vr04,  2    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void avg_no_rnd_pixels8x4_x2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS8x4_X2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels8x4_x2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input - src
    vld128  vr01,   [i1,   0]    ;
    vld128  vr02,   [i1,  32]    ;
    vld128  vr03,   [i1,  64]    ;
    vld128  vr04,   [i1,  96]    ;

    // shift input
    vsr8    vr11,   vr01,   1   ;
    vsr8    vr12,   vr02,   1   ;
    vsr8    vr13,   vr03,   1   ;
    vsr8    vr14,   vr04,   1   ;

    //avg without rounding (of inp)
    vavb   vr01,   vr01,   vr11    ;
    vavb   vr02,   vr02,   vr12    ;
    vavb   vr03,   vr03,   vr13    ;
    vavb   vr04,   vr04,   vr14    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store at dst
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_no_rnd_pixels8x4_y2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS8x4_Y2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels8x4_y2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // load input - src
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;

    //avg without rounding (of inp)
    vavb   vr01,   vr01,   vr02    ;
    vavb   vr02,   vr02,   vr03    ;
    vavb   vr03,   vr03,   vr04    ;
    vavb   vr04,   vr04,   vr05    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store at dst
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_no_rnd_pixels8x4_xy2_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with src
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_NO_RND_PIXELS8x4_XY2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_avg_no_rnd_pixels8x4_xy2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // store (1)
    //vmovzw   vr21, 1, 0xff   ; CLARIFY
    vsubw    vr21, vr21, vr21   ;
    vbaddw   vr21, vr21, 1   ;

    // load input
    vld64w   vr01,   [i1,   0]   ;      // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w   vr02,   [i1,  32]   ;
    vld64w   vr03,   [i1,  64]   ;
    vld64w   vr04,   [i1,  96]   ;
    vld64w   vr05,   [i1, 128]   ;

    vld64w   vr11,   [i1,   8]   ;      // vr11:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w   vr12,   [i1,  40]   ;
    vld64w   vr13,   [i1,  72]   ;
    vld64w   vr14,   [i1, 104]   ;
    vld64w   vr15,   [i1, 136]   ;

    // shift input
    vmr1w    vr11,  vr11,  vr01  ;      // vr11:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w    vr12,  vr12,  vr02  ;
    vmr1w    vr13,  vr13,  vr03  ;
    vmr1w    vr14,  vr14,  vr04  ;
    vmr1w    vr15,  vr15,  vr05  ;

    // add pixels
    vaddw    vr01,  vr01,  vr11  ;
    vaddaw   vr01,  vr02,  vr12  ;

    vaddw    vr02,  vr02,  vr12  ;
    vaddaw   vr02,  vr03,  vr13  ;

    vaddw    vr03,  vr03,  vr13  ;
    vaddaw   vr03,  vr04,  vr14  ;

    vaddw    vr04,  vr04,  vr14  ;
    vaddaw   vr04,  vr05,  vr15  ;

    vaddw    vr01,  vr01, vr21  ;
    vaddw    vr02,  vr02, vr21  ;
    vaddw    vr03,  vr03, vr21  ;
    vaddw    vr04,  vr04, vr21  ;

    // shift and pack
    vasrpwb  vr01,  vr01,  2    ;
    vasrpwb  vr02,  vr02,  2    ;
    vasrpwb  vr03,  vr03,  2    ;
    vasrpwb  vr04,  vr04,  2    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;

    //avg with rounding (src + dst)
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


#endif
