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
 * implement core Qpel functions
 */

#ifndef _AURORA_QPEL8_H_
#define _AURORA_QPEL8_H_

// src buffer stride = 32
// dst buffer stride = 32

_Asm void shift_pixels8x9_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_SHIFT_PIXELS8x9_SIMD_   ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_shift_pixels8x9_simd(int scm_addr)
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

    // shift input
    vsr8    vr01,   vr01,   1   ;
    vsr8    vr02,   vr02,   1   ;
    vsr8    vr03,   vr03,   1   ;
    vsr8    vr04,   vr04,   1   ;
    vsr8    vr05,   vr05,   1   ;
    vsr8    vr06,   vr06,   1   ;
    vsr8    vr07,   vr07,   1   ;
    vsr8    vr08,   vr08,   1   ;
    vsr8    vr09,   vr09,   1   ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;
    vst64   vr09,   [i2, 256]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x8_SIMD_
    vrun    r10
.on     percent_reg
}

_Asm int macro_put_pixels8x8_simd(int scm_addr)
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
    vld64   vr06,   [i1, 160]   ;
    vld64   vr07,   [i1, 192]   ;
    vld64   vr08,   [i1, 224]   ;

    // store result
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2
#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x8_L2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels8x8_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;

    // load input - src2
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

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
    vst64   vr01,   [i3,   0]    ;
    vst64   vr02,   [i3,  32]    ;
    vst64   vr03,   [i3,  64]    ;
    vst64   vr04,   [i3,  96]    ;
    vst64   vr05,   [i3, 128]    ;
    vst64   vr06,   [i3, 160]    ;
    vst64   vr07,   [i3, 192]    ;
    vst64   vr08,   [i3, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_pixels8x9_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_PIXELS8x9_L2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_put_pixels8x9_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;
    vld64   vr09,   [i1, 256]    ;

    // load input - src2
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;
    vld64   vr19,   [i2, 256]    ;

    // average with rounding
    vavrb   vr01,   vr01,   vr11    ;
    vavrb   vr02,   vr02,   vr12    ;
    vavrb   vr03,   vr03,   vr13    ;
    vavrb   vr04,   vr04,   vr14    ;
    vavrb   vr05,   vr05,   vr15    ;
    vavrb   vr06,   vr06,   vr16    ;
    vavrb   vr07,   vr07,   vr17    ;
    vavrb   vr08,   vr08,   vr18    ;
    vavrb   vr09,   vr09,   vr19    ;

    // store at dst
    vst64   vr01,   [i3,   0]    ;
    vst64   vr02,   [i3,  32]    ;
    vst64   vr03,   [i3,  64]    ;
    vst64   vr04,   [i3,  96]    ;
    vst64   vr05,   [i3, 128]    ;
    vst64   vr06,   [i3, 160]    ;
    vst64   vr07,   [i3, 192]    ;
    vst64   vr08,   [i3, 224]    ;
    vst64   vr09,   [i3, 256]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}


_Asm void put_no_rnd_pixels8x8_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS8x8_L2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels8x8_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src1
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;

    // load input - src2
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;

    // store at dst
    vst64   vr01,   [i3,   0]    ;
    vst64   vr02,   [i3,  32]    ;
    vst64   vr03,   [i3,  64]    ;
    vst64   vr04,   [i3,  96]    ;
    vst64   vr05,   [i3, 128]    ;
    vst64   vr06,   [i3, 160]    ;
    vst64   vr07,   [i3, 192]    ;
    vst64   vr08,   [i3, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void put_no_rnd_pixels8x9_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_PIXELS8x9_L2_SIMD_   ;
    vrun    r10                                         ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_pixels8x9_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    // process the first 8 rows
    // load input - src1
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;
    vld64   vr09,   [i1, 256]    ;

    // load input - src2
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;
    vld64   vr19,   [i2, 256]    ;

    // average without rounding
    vavb    vr01,   vr01,   vr11    ;
    vavb    vr02,   vr02,   vr12    ;
    vavb    vr03,   vr03,   vr13    ;
    vavb    vr04,   vr04,   vr14    ;
    vavb    vr05,   vr05,   vr15    ;
    vavb    vr06,   vr06,   vr16    ;
    vavb    vr07,   vr07,   vr17    ;
    vavb    vr08,   vr08,   vr18    ;
    vavb    vr09,   vr09,   vr19    ;

    // store at dst
    vst64   vr01,   [i3,   0]    ;
    vst64   vr02,   [i3,  32]    ;
    vst64   vr03,   [i3,  64]    ;
    vst64   vr04,   [i3,  96]    ;
    vst64   vr05,   [i3, 128]    ;
    vst64   vr06,   [i3, 160]    ;
    vst64   vr07,   [i3, 192]    ;
    vst64   vr08,   [i3, 224]    ;
    vst64   vr09,   [i3, 256]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 4        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x8_SIMD_     ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x8_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // process the first 8 rows
    // load input - src
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;

    // load dst
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

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
    vst64   vr01,   [i2,   0]    ;
    vst64   vr02,   [i2,  32]    ;
    vst64   vr03,   [i2,  64]    ;
    vst64   vr04,   [i2,  96]    ;
    vst64   vr05,   [i2, 128]    ;
    vst64   vr06,   [i2, 160]    ;
    vst64   vr07,   [i2, 192]    ;
    vst64   vr08,   [i2, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

_Asm void avg_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    % reg dst_sdm_add, src1_sdm_add,  src2_sdm_add
.off    percent_reg

    // load inp and output pointers
    vmovw    vr00, src1_sdm_add, 2        ;    // load i1 with r1
    vmovw    vr00, src2_sdm_add, 4        ;    // load i1 with r1
    vmovw    vr00, dst_sdm_add, 8        ;    // load i2 with r2

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_PIXELS8x8_L2_SIMD_  ;
    vrun    r10                                 ;
.on     percent_reg
}

_Asm int macro_avg_pixels8x8_l2_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load input - src1
    vld64   vr01,   [i1,   0]    ;
    vld64   vr02,   [i1,  32]    ;
    vld64   vr03,   [i1,  64]    ;
    vld64   vr04,   [i1,  96]    ;
    vld64   vr05,   [i1, 128]    ;
    vld64   vr06,   [i1, 160]    ;
    vld64   vr07,   [i1, 192]    ;
    vld64   vr08,   [i1, 224]    ;

    // load input - src2
    vld64   vr11,   [i2,   0]    ;
    vld64   vr12,   [i2,  32]    ;
    vld64   vr13,   [i2,  64]    ;
    vld64   vr14,   [i2,  96]    ;
    vld64   vr15,   [i2, 128]    ;
    vld64   vr16,   [i2, 160]    ;
    vld64   vr17,   [i2, 192]    ;
    vld64   vr18,   [i2, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // load dst
    vld64   vr11,   [i3,   0]    ;
    vld64   vr12,   [i3,  32]    ;
    vld64   vr13,   [i3,  64]    ;
    vld64   vr14,   [i3,  96]    ;
    vld64   vr15,   [i3, 128]    ;
    vld64   vr16,   [i3, 160]    ;
    vld64   vr17,   [i3, 192]    ;
    vld64   vr18,   [i3, 224]    ;

    // average with rounding
    vavrb    vr01,   vr01,   vr11    ;
    vavrb    vr02,   vr02,   vr12    ;
    vavrb    vr03,   vr03,   vr13    ;
    vavrb    vr04,   vr04,   vr14    ;
    vavrb    vr05,   vr05,   vr15    ;
    vavrb    vr06,   vr06,   vr16    ;
    vavrb    vr07,   vr07,   vr17    ;
    vavrb    vr08,   vr08,   vr18    ;

    // store at dst
    vst64   vr01,   [i3,   0]    ;
    vst64   vr02,   [i3,  32]    ;
    vst64   vr03,   [i3,  64]    ;
    vst64   vr04,   [i3,  96]    ;
    vst64   vr05,   [i3, 128]    ;
    vst64   vr06,   [i3, 160]    ;
    vst64   vr07,   [i3, 192]    ;
    vst64   vr08,   [i3, 224]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on     percent_reg
}

#endif
