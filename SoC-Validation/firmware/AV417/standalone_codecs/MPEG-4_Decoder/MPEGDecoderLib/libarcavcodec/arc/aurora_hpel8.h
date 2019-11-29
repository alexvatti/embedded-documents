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

#ifndef _AURORA_HPEL8_H_
#define _AURORA_HPEL8_H_

// src buffer stride = 32
// dst buffer stride = 32
/*
07     1    p8   p7   p6   p5   p4   p3   p2   p1   p0   p0   p1   p2
08     3         p8   p7   p6   p5   p4   p3   p2   p1   p0   p0   p1
09    -6              p8   p7   p6   p5   p4   p3   p2   p1   p0   p0
01    20                   p8   p7   p6   p5   p4   p3   p2   p1   p0
10    20                   p8   p7   p6   p5   p4   p3   p2   p1   p0
11    -6                        p8   p8   p7   p6   p5   p4   p3   p2   p1   p0    
12     3                        p7   p8   p8   p7   p6   p5   p4   p3   p2   p1   p0  
13     1                        p6   p7   p8   p8   p7   p6   p5   p4   p3   p2   p1   p0

01                               R7   R6   R5   R4   R3   R2   R1   R0


14     1    p8   p7   p6   p5   p4   p3   p2   p1   p0   p0   p1   p2
15     3         p8   p7   p6   p5   p4   p3   p2   p1   p0   p0   p1
16    -6              p8   p7   p6   p5   p4   p3   p2   p1   p0   p0
03    20                   p8   p7   p6   p5   p4   p3   p2   p1   p0
17    20                   p8   p7   p6   p5   p4   p3   p2   p1   p0
18    -6                        p8   p8   p7   p6   p5   p4   p3   p2   p1   p0    
19     3                        p7   p8   p8   p7   p6   p5   p4   p3   p2   p1   p0  
20     1                        p6   p7   p8   p8   p7   p6   p5   p4   p3   p2   p1   p0

03                              R7   R6   R5   R4   R3   R2   R1   R0

*/
_Asm void put_mpeg4_qpel8x4_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_mpeg4_qpel8x4_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load 2 rows
    vld64w  vr01,   [i1,   0]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,   8]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  32]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1,  40]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,   0]    ;
    vst64   vr03, [i2,  32]    ;


    // load next 2 rows
    vld64w  vr01,   [i1,  64]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,  72]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  96]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1, 104]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,  64]    ;
    vst64   vr03, [i2,  96]    ;

    vbaddw  vr00, vr00, 128;    //32*4=128

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif
.on percent_reg
}

_Asm void put_mpeg4_qpel8x5_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)

{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_MPEG4_QPEL8x5_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_mpeg4_qpel8x5_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load 2 rows
    vld64w  vr01,   [i1,   0]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,   8]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  32]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1,  40]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,   0]    ;
    vst64   vr03, [i2,  32]    ;


    // load next 2 rows
    vld64w  vr01,   [i1,  64]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,  72]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  96]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1, 104]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,  64]    ;
    vst64   vr03, [i2,  96]    ;

    // load 5th row
    vld64w  vr01,   [i1, 128]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1, 136]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;

    vaddw   vr01, vr01, vr02    ;

    vaddw   vr02, vr07, vr13    ;

    vbmulw  vr02, vr02, (-1)    ;

    vaddw   vr01, vr01, vr02    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;

    // store result
    vst64   vr01, [i2, 128]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

_Asm void put_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_MPEG4_QPEL8x8_V_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_mpeg4_qpel8x8_v_lowpass_simd(int scm_addr)
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

    ;// calculate row0: (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4)
    vaddw   vr11, vr01, vr02    ;
    vaddw   vr12, vr01, vr03    ;
    vaddw   vr13, vr02, vr04    ;
    vaddw   vr14, vr03, vr05    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row1: (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5)
    vaddw   vr16, vr02, vr03    ;
    vaddw   vr17, vr01, vr04    ;
    vaddw   vr18, vr01, vr05    ;
    vaddw   vr19, vr02, vr06    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2,   0]     ;
    vst64   vr20, [i2,  32]     ;
  
    ;// calculate row2: (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6)
    vaddw   vr11, vr03, vr04    ;
    vaddw   vr12, vr02, vr05    ;
    vaddw   vr13, vr01, vr06    ;
    vaddw   vr14, vr01, vr07    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row3: (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7)
    vaddw   vr16, vr04, vr05    ;
    vaddw   vr17, vr03, vr06    ;
    vaddw   vr18, vr02, vr07    ;
    vaddw   vr19, vr01, vr08    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2,  64]     ;
    vst64   vr20, [i2,  96]     ;

    ;// calculate row4: (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8)
    vaddw   vr11, vr05, vr06    ;
    vaddw   vr12, vr04, vr07    ;
    vaddw   vr13, vr03, vr08    ;
    vaddw   vr14, vr02, vr09    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row5: (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8)
    vaddw   vr16, vr06, vr07    ;
    vaddw   vr17, vr05, vr08    ;
    vaddw   vr18, vr04, vr09    ;
    vaddw   vr19, vr03, vr09    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2, 128]     ;
    vst64   vr20, [i2, 160]     ;

    ;// calculate row6: (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7)
    vaddw   vr11, vr07, vr08    ;
    vaddw   vr12, vr06, vr09    ;
    vaddw   vr13, vr05, vr09    ;
    vaddw   vr14, vr04, vr08    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row7: (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6)
    vaddw   vr16, vr08, vr09    ;
    vaddw   vr17, vr07, vr09    ;
    vaddw   vr18, vr06, vr08    ;
    vaddw   vr19, vr05, vr07    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2, 192]     ;
    vst64   vr20, [i2, 224]     ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load 2 rows
    vld64w  vr01,   [i1,   0]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,   8]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  32]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1,  40]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    ;// clear a dummy register
    vsubw   vr23,   vr23, vr23  ;   // vr23 = all zero
    ;// store (-1) in a register
    vbaddw  vr22,   vr23, (-1)  ;   // vr22 =  -1  -1  -1  -1  -1  -1  -1  -1
    
    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // add (-1)
    vaddw   vr01, vr01, vr22    ;
    vaddw   vr03, vr03, vr22    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,   0]    ;
    vst64   vr03, [i2,  32]    ;


    // load next 2 rows
    vld64w  vr01,   [i1,  64]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,  72]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  96]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1, 104]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // add (-1)
    vaddw   vr01, vr01, vr22    ;
    vaddw   vr03, vr03, vr22    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5      ;
    vasrrpwb vr03, vr03, 5      ;

    // store result
    vst64   vr01, [i2,  64]    ;
    vst64   vr03, [i2,  96]    ;

    vbaddw  vr00, vr00, 128    ;    //32*4=128

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

_Asm void put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)

{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x5_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load 2 rows
    vld64w  vr01,   [i1,   0]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,   8]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  32]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1,  40]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;
    ;// store (-1) in a register
    vbaddw  vr22,   vr23, (-1)  ;   // vr22 =  -1  -1  -1  -1  -1  -1  -1  -1

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // add (-1)
    vaddw   vr01, vr01, vr22    ;
    vaddw   vr03, vr03, vr22    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,   0]    ;
    vst64   vr03, [i2,  32]    ;


    // load next 2 rows
    vld64w  vr01,   [i1,  64]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,  72]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  96]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1, 104]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // add (-1)
    vaddw   vr01, vr01, vr22    ;
    vaddw   vr03, vr03, vr22    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // store result
    vst64   vr01, [i2,  64]    ;
    vst64   vr03, [i2,  96]    ;

    // load 5th row
    vld64w  vr01,   [i1, 128]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1, 136]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;

    vaddw   vr01, vr01, vr02    ;

    vaddw   vr02, vr07, vr13    ;

    vbmulw  vr02, vr02, (-1)    ;

    vaddw   vr01, vr01, vr02    ;

    // add (-1)
    vaddw   vr01, vr01, vr22    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5      ;

    // store result
    vst64   vr01, [i2, 128]    ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}


_Asm void put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    //clear a register
    vsubw   vr23, vr23, vr23    ;

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x8_V_LOWPASS_SIMD_    ;
    vrun    r10                                                     ;
.on     percent_reg
}

_Asm int macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd(int scm_addr)
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

    // create a mask of (-1)
    vbaddw   vr23, vr23, (-1)   ;

    ;// calculate row0: (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4)
    vaddw   vr11, vr01, vr02    ;
    vaddw   vr12, vr01, vr03    ;
    vaddw   vr13, vr02, vr04    ;
    vaddw   vr14, vr03, vr05    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vsubaw   vr15, vr23, vr14   ;

    ;// calculate row1: (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5)
    vaddw   vr16, vr02, vr03    ;
    vaddw   vr17, vr01, vr04    ;
    vaddw   vr18, vr01, vr05    ;
    vaddw   vr19, vr02, vr06    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vsubaw  vr20, vr23, vr19    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2,   0]     ;
    vst64   vr20, [i2,  32]     ;
  
    ;// calculate row2: (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6)
    vaddw   vr11, vr03, vr04    ;
    vaddw   vr12, vr02, vr05    ;
    vaddw   vr13, vr01, vr06    ;
    vaddw   vr14, vr01, vr07    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vsubaw   vr15, vr23, vr14   ;

    ;// calculate row3: (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7)
    vaddw   vr16, vr04, vr05    ;
    vaddw   vr17, vr03, vr06    ;
    vaddw   vr18, vr02, vr07    ;
    vaddw   vr19, vr01, vr08    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vsubaw  vr20, vr23, vr19    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2,  64]     ;
    vst64   vr20, [i2,  96]     ;

    ;// calculate row4: (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8)
    vaddw   vr11, vr05, vr06    ;
    vaddw   vr12, vr04, vr07    ;
    vaddw   vr13, vr03, vr08    ;
    vaddw   vr14, vr02, vr09    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vsubaw   vr15, vr23, vr14   ;

    ;// calculate row5: (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8)
    vaddw   vr16, vr06, vr07    ;
    vaddw   vr17, vr05, vr08    ;
    vaddw   vr18, vr04, vr09    ;
    vaddw   vr19, vr03, vr09    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vsubaw  vr20, vr23, vr19    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2, 128]     ;
    vst64   vr20, [i2, 160]     ;

    ;// calculate row6: (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7)
    vaddw   vr11, vr07, vr08    ;
    vaddw   vr12, vr06, vr09    ;
    vaddw   vr13, vr05, vr09    ;
    vaddw   vr14, vr04, vr08    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vsubaw   vr15, vr23, vr14   ;

    ;// calculate row7: (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6)
    vaddw   vr16, vr08, vr09    ;
    vaddw   vr17, vr07, vr09    ;
    vaddw   vr18, vr06, vr08    ;
    vaddw   vr19, vr05, vr07    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vsubaw  vr20, vr23, vr19    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vst64   vr15, [i2, 192]     ;
    vst64   vr20, [i2, 224]     ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

_Asm void avg_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_MPEG4_QPEL8x8_V_LOWPASS_SIMD_    ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_avg_mpeg4_qpel8x8_v_lowpass_simd(int scm_addr)
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

    ;// calculate row0: (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4)
    // load dst 
    vld64   vr21, [i2,   0]     ;
    vld64   vr22, [i2,  32]     ;

    vaddw   vr11, vr01, vr02    ;
    vaddw   vr12, vr01, vr03    ;
    vaddw   vr13, vr02, vr04    ;
    vaddw   vr14, vr03, vr05    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row1: (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5)
    vaddw   vr16, vr02, vr03    ;
    vaddw   vr17, vr01, vr04    ;
    vaddw   vr18, vr01, vr05    ;
    vaddw   vr19, vr02, vr06    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vavrb   vr15, vr15, vr21    ;
    vavrb   vr20, vr20, vr22    ;
    
    vst64   vr15, [i2,   0]     ;
    vst64   vr20, [i2,  32]     ;
  
    ;// calculate row2: (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6)
    // load dst 
    vld64   vr21, [i2,  64]     ;
    vld64   vr22, [i2,  96]     ;

    vaddw   vr11, vr03, vr04    ;
    vaddw   vr12, vr02, vr05    ;
    vaddw   vr13, vr01, vr06    ;
    vaddw   vr14, vr01, vr07    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row3: (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7)
    vaddw   vr16, vr04, vr05    ;
    vaddw   vr17, vr03, vr06    ;
    vaddw   vr18, vr02, vr07    ;
    vaddw   vr19, vr01, vr08    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vavrb   vr15, vr15, vr21    ;
    vavrb   vr20, vr20, vr22    ;

    vst64   vr15, [i2,  64]     ;
    vst64   vr20, [i2,  96]     ;

    ;// calculate row4: (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8)
    // load dst 
    vld64   vr21, [i2, 128]     ;
    vld64   vr22, [i2, 160]     ;

    vaddw   vr11, vr05, vr06    ;
    vaddw   vr12, vr04, vr07    ;
    vaddw   vr13, vr03, vr08    ;
    vaddw   vr14, vr02, vr09    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row5: (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8)
    vaddw   vr16, vr06, vr07    ;
    vaddw   vr17, vr05, vr08    ;
    vaddw   vr18, vr04, vr09    ;
    vaddw   vr19, vr03, vr09    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vavrb   vr15, vr15, vr21    ;
    vavrb   vr20, vr20, vr22    ;

    vst64   vr15, [i2, 128]     ;
    vst64   vr20, [i2, 160]     ;

    ;// calculate row6: (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7)
    // load dst 
    vld64   vr21, [i2, 192]     ;
    vld64   vr22, [i2, 224]     ;

    vaddw   vr11, vr07, vr08    ;
    vaddw   vr12, vr06, vr09    ;
    vaddw   vr13, vr05, vr09    ;
    vaddw   vr14, vr04, vr08    ;

    vbmulw   vr15, vr11,  20    ;
    vbmulaw  vr15, vr12, (-6)   ;
    vbmulaw  vr15, vr13,   3    ;
    vbmulaw  vr15, vr14, (-1)   ;

    ;// calculate row7: (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6)
    vaddw   vr16, vr08, vr09    ;
    vaddw   vr17, vr07, vr09    ;
    vaddw   vr18, vr06, vr08    ;
    vaddw   vr19, vr05, vr07    ;

    vbmulw  vr20, vr16,   20    ;
    vbmulaw vr20, vr17, (-6)    ;
    vbmulaw vr20, vr18,    3    ;
    vbmulaw vr20, vr19, (-1)    ;

    vasrrpwb vr15, vr15, 5      ;
    vasrrpwb vr20, vr20, 5      ;

    vavrb   vr15, vr15, vr21    ;
    vavrb   vr20, vr20, vr22    ;

    vst64   vr15, [i2, 192]     ;
    vst64   vr20, [i2, 224]     ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

_Asm void avg_mpeg4_qpel8x4_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

#ifdef AURORA_SUBPEL_MACROS
    mov     r10, _MACRO_AVG_MPEG4_QPEL8x4_H_LOWPASS_SIMD_    ;
    vrun    r10                                              ;
.on     percent_reg
}

_Asm int macro_avg_mpeg4_qpel8x4_h_lowpass_simd(int scm_addr)
{
    %reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    // load 2 rows
    vld64w  vr01,   [i1,   0]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,   8]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  32]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1,  40]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // clear a dummy register
    vsubw   vr23,   vr23, vr23  ;
    // load dst 
    vld64   vr21, [i2,   0]     ;
    vld64   vr22, [i2,  32]     ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // add dst
    vavrb   vr01, vr01, vr21    ;
    vavrb   vr03, vr03, vr22    ;

    // store result
    vst64   vr01, [i2,   0]    ;
    vst64   vr03, [i2,  32]    ;


    // load next 2 rows
    vld64w  vr01,   [i1,  64]  ;    // vr01:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr02,   [i1,  72]  ;    // vr02:   XX  XX  XX  XX  XX  XX  XX  p8
    vld64w  vr03,   [i1,  96]  ;    // vr03:   p7  p6  p5  p4  p3  p2  p1  p0
    vld64w  vr04,   [i1, 104]  ;    // vr04:   XX  XX  XX  XX  XX  XX  XX  p8

    // load dst 
    vld64   vr21, [i2,  64]     ;
    vld64   vr22, [i2,  96]     ;

    vmr7w   vr09,   vr01, vr23  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  00
    vmr7w   vr16,   vr03, vr23  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  00

    vmvw    vr09,   vr01,  0x1  ;   // vr09:   p6  p5  p4  p3  p2  p1  p0  p0
    vmvw    vr16,   vr03,  0x1  ;   // vr16:   p6  p5  p4  p3  p2  p1  p0  p0

    vmr1w   vr10,   vr02, vr01  ;   // vr10:   p8  p7  p6  p5  p4  p3  p2  p1
    vmr1w   vr17,   vr04, vr03  ;   // vr17:   p8  p7  p6  p5  p4  p3  p2  p1

    vmr2w   vr11,   vr02, vr01  ;   // vr11:   XX  p8  p7  p6  p5  p4  p3  p2
    vmr2w   vr18,   vr04, vr03  ;   // vr18:   XX  p8  p7  p6  p5  p4  p3  p2

    vmvw    vr11,   vr10, 0x80  ;   // vr11:   p8  p8  p7  p6  p5  p4  p3  p2
    vmvw    vr18,   vr17, 0x80  ;   // vr18:   p8  p8  p7  p6  p5  p4  p3  p2

    vmr7w   vr08,   vr09, vr23  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  00
    vmr7w   vr15,   vr16, vr23  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  00

    vmvw    vr08,   vr10,  0x1  ;   // vr08:   p5  p4  p3  p2  p1  p0  p0  p1
    vmvw    vr15,   vr17,  0x1  ;   // vr15:   p5  p4  p3  p2  p1  p0  p0  p1

    vmr1w   vr12,   vr23, vr11  ;   // vr12:   00  p8  p8  p7  p6  p5  p4  p3
    vmr1w   vr19,   vr23, vr18  ;   // vr19:   00  p8  p8  p7  p6  p5  p4  p3

    vmvw    vr12,   vr01, 0x80  ;   // vr12:   p7  p8  p8  p7  p6  p5  p4  p3
    vmvw    vr19,   vr03, 0x80  ;   // vr19:   p7  p8  p8  p7  p6  p5  p4  p3

    vmr7w   vr07,  vr08,  vr23  ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  00
    vmr7w   vr14,  vr15,  vr23  ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  00

    vmvw    vr07,  vr11,  0x1   ;   // vr07:   p4  p3  p2  p1  p0  p0  p1  p2
    vmvw    vr14,  vr18,  0x1   ;   // vr14:   p4  p3  p2  p1  p0  p0  p1  p2

    vmr1w   vr13,  vr23,  vr12  ;   // vr13:   00  p7  p8  p8  p7  p6  p5  p4
    vmr1w   vr20,  vr23,  vr19  ;   // vr20:   00  p7  p8  p8  p7  p6  p5  p4

    vmvw    vr13,  vr09,  0x80  ;   // vr13:   p6  p7  p8  p8  p7  p6  p5  p4
    vmvw    vr20,  vr16,  0x80  ;   // vr20:   p6  p7  p8  p8  p7  p6  p5  p4

    // store result in vr01, vr03
    // store temp values in vr02, vr04

    vaddw   vr01, vr01, vr10    ;   // vr01: (src[0] + src[1])
    vaddw   vr03, vr03, vr17    ;   // vr03: (src[0] + src[1])

    vbmulw  vr01, vr01, 20      ;   // vr01: 20*(src[0] + src[1])
    vbmulw  vr03, vr03, 20      ;   // vr03: 20*(src[0] + src[1])

    vaddw   vr02, vr09, vr11    ;   // vr02: (src[-1] + src[2])
    vaddw   vr04, vr16, vr18    ;   // vr04: (src[-1] + src[2])

    vbmulw  vr02, vr02, (-6)    ;   // vr02: (-6)*(src[-1] + src[2])
    vbmulw  vr04, vr04, (-6)    ;   // vr04: (-6)*(src[-1] + src[2])

    vaddw   vr01, vr01, vr02    ;   // vr01: vr01 + vr02
    vaddw   vr03, vr03, vr04    ;   // vr03: vr03 + vr04

    vaddw   vr02, vr08, vr12    ;   // vr02: (src[-2] + src[3])
    vaddw   vr04, vr15, vr19    ;   // vr04: (src[-2] + src[3])

    vbmulw  vr02, vr02,    3    ;
    vbmulw  vr04, vr04,    3    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    vaddw   vr02, vr07, vr13    ;
    vaddw   vr04, vr14, vr20    ;

    vbmulw  vr02, vr02, (-1)    ;
    vbmulw  vr04, vr04, (-1)    ;

    vaddw   vr01, vr01, vr02    ;
    vaddw   vr03, vr03, vr04    ;

    // clip-round and pack the result
    vasrrpwb vr01, vr01, 5       ;
    vasrrpwb vr03, vr03, 5       ;

    // add dst
    vavrb   vr01, vr01, vr21    ;
    vavrb   vr03, vr03, vr22    ;

    // store result
    vst64   vr01, [i2,  64]    ;
    vst64   vr03, [i2,  96]    ;

    vbaddw  vr00, vr00, 128;    //32*4=128

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec r0              ;
#endif

.on percent_reg
}

#if defined(AURORA_8_H_FILTER_MACROS)
_Asm void put_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    mov     r10, _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
    vrun    r10                                             ;

.on percent_reg

}

_Asm void put_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    mov     r10, _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
    mov     r10, _MACRO_PUT_MPEG4_QPEL8x5_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;

.on percent_reg

}

_Asm void put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
    vrun    r10                                             ;

.on     percent_reg

}

_Asm void put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;
    mov     r10, _MACRO_PUT_NO_RND_MPEG4_QPEL8x5_H_LOWPASS_SIMD_   ;
    vrun    r10                                             ;

.on     percent_reg

}

_Asm void avg_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    % reg dst_sdm_add, src_sdm_add 
.off    percent_reg

    // load inp and output pointers
    vmovw  vr00, src_sdm_add, 2    ;   // load i1 with src
    vmovw  vr00, dst_sdm_add, 4    ;   // load i2 with dst

    mov     r10, _MACRO_AVG_MPEG4_QPEL8x4_H_LOWPASS_SIMD_    ;
    vrun    r10                                              ;
    vrun    r10                                              ;

.on     percent_reg
}
#endif  //AURORA_SUBPEL_MACROS

#endif
