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
 * Asm for all DCT related functions
 */
#ifndef _AURORA_DCT_H_
#define _AURORA_DCT_H_


_Asm void idct_put_aurora_asm (uint8_t* dest, int line_size, int16_t* block)
{
    % reg dest, line_size, block
.off    percent_reg

    /* Get   r15 pointer into I0  */
    /* Get   r13 pointer into I1 */
    vmovzw   vr00,  dest, 0x01
    vmovw    vr00, block, 0x02

#ifdef AURORA_SUBPEL_MACROS
    mov       r10, _MACRO_IDCT_PUT_AURORA_ASM_  ;
    vrun      r10                                ;
.on     percent_reg
}

_Asm int macro_idct_put_aurora_asm(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif
    vld128   vr16,  [ i1, 0x00]   ; 
    vld128   vr01,  [ i1, 0x10]   ; 
    vld128   vr02,  [ i1, 0x20]   ; 
    vld128   vr03,  [ i1, 0x30]   ; 
    vld128   vr04,  [ i1, 0x40]   ;
    vld128   vr05,  [ i1, 0x50]   ;
    vld128   vr06,  [ i1, 0x60]   ;
    vld128   vr07,  [ i1, 0x70]   ;
    /* Convert input coefficients to 12Q4 format and transpose to column fmt
    * TODO: this will eventually be done in the dequantization step
    */
    //mov   r10,16 
    vbmulw   vr16,  vr16,  16 ;
    vbmulw   vr01,  vr01,  16 ;
    vbmulw   vr02,  vr02,  16 ;
    vbmulw   vr03,  vr03,  16 ;
    vbmulw   vr04,  vr04,  16 ;
    vbmulw   vr05,  vr05,  16 ;
    vbmulw   vr06,  vr06,  16 ;
    vbmulw   vr07,  vr07,  16;
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07 ;

    //mov   r10,23170 
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 
    vbmulfw   vr07,  vr02,  12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04;

    //mov   r12,30274 
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06,  12540 ;
    vsubw   vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw   vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;

    //mov   r10,18205 
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;

    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04;

    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07;

    //mov   r10,23170 
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 
    vbmulfw   vr07,  vr02, 12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06, 12540 ;
    vsubw   vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw   vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;

    //mov   r10,18205 
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;
    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;

    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04;

    vasrsrw   vr16,  vr16,  6 ;
    vasrsrw   vr01,  vr01,  6 ;
    vasrsrw   vr02,  vr02,  6 ;
    vasrsrw   vr03,  vr03,  6 ;
    vasrsrw   vr04,  vr04,  6 ;
    vasrsrw   vr05,  vr05,  6 ;
    vasrsrw   vr06,  vr06,  6 ;
    vasrsrw   vr07,  vr07,  6;
    vasrpwb   vr16,  vr16, 0 ; 
    vasrpwb   vr01,  vr01, 0 ;
    vasrpwb   vr02,  vr02, 0 ;
    vasrpwb   vr03,  vr03, 0 ;
    vasrpwb   vr04,  vr04, 0 ;
    vasrpwb   vr05,  vr05, 0 ;
    vasrpwb   vr06,  vr06, 0 ;
    vasrpwb   vr07,  vr07, 0 ;

    vst64   vr16,  [ i0, 0x00]   ; 
    vst64   vr01,  [ i0, 0x10]   ; 
    vst64   vr02,  [ i0, 0x20]   ; 
    vst64   vr03,  [ i0, 0x30]   ; 
    vst64   vr04,  [ i0, 0x40]   ; 
    vst64   vr05,  [ i0, 0x50]   ; 
    vst64   vr06,  [ i0, 0x60]   ; 
    vst64   vr07,  [ i0, 0x70]   ;
    /* Leave the   r13 clean for the next set of coeff's. */
    vxor   vr01,  vr01,  vr01 
    vst128   vr01,  [ i1, 0x00]   ; 
    vst128   vr01,  [ i1, 0x10]   ; 
    vst128   vr01,  [ i1, 0x20]   ; 
    vst128   vr01,  [ i1, 0x30]   ; 
    vst128   vr01,  [ i1, 0x40]   ; 
    vst128   vr01,  [ i1, 0x50]   ; 
    vst128   vr01,  [ i1, 0x60]   ; 
    vst128   vr01,  [ i1, 0x70]   ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif
.on     percent_reg

}

/*----------------------------------------------------------------------------
 *
 * Aurora IDCT function: inter frame residual decoding
 */

_Asm void idct_add_aurora_asm (uint8_t* dest, int line_size, int16_t* block)
{
    %   reg dest, line_size, block

.off    percent_reg

    /* Get dest pointer into I0  */
    /* Get block pointer into I1 */
    vmovzw    vr00,  dest, 0x01
    vmovw     vr00, block, 0x02

    vld128   vr16,  [ i1, 0x00]   ; 
    vld128   vr01,  [ i1, 0x10]   ; 
    vld128   vr02,  [ i1, 0x20]   ; 
    vld128   vr03,  [ i1, 0x30]   ; 
    vld128   vr04,  [ i1, 0x40]   ; 
    vld128   vr05,  [ i1, 0x50]   ; 
    vld128   vr06,  [ i1, 0x60]   ; 
    vld128   vr07,  [ i1, 0x70]   ;
    /* Convert input coefficients to 12Q4 format and transpose to column fmt
    * TODO: this will eventually be done in the dequantization step
    */
    //mov   r10,16 ;
    vbmulw   vr16,  vr16,  16 ;
    vbmulw   vr01,  vr01,  16 ;
    vbmulw   vr02,  vr02,  16 ;
    vbmulw   vr03,  vr03,  16 ;
    vbmulw   vr04,  vr04,  16 ;
    vbmulw   vr05,  vr05,  16 ;
    vbmulw   vr06,  vr06,  16 ;
    vbmulw   vr07,  vr07,  16 ;
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07
    //mov   r10,23170 ;
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 ;
    vbmulfw   vr07,  vr02,  12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06,  12540 ;
    vsubw     vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 ;
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 ;
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw     vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 ;
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw     vr05,  vr09,  vr10 ;
    vaddw     vr06,  vr07,  vr06 ;

    //mov   r10,18205 ;
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;
    vaddsuw   vr01,  vr06 ;
    vsubw     vr04,  vr11,  vr09 ;
    vaddw     vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07
    mov   r10,23170 ;
    vbmulfw   vr16,  vr16,  r10 ;
    vbmulfw   vr08,  vr01,  r10 ;
    vbmulfw   vr01,  vr04,  r10 ;
    vbmulfw   vr04,  vr07,  r10 ;
    mov   r10,12540 ;
    vbmulfw   vr07,  vr02,  r10 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;
    mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  r12 ;
    vbmulfw   vr10,  vr02,  r12 ;
    vbmulfw   vr06,  vr06,  r10 ;
    vsubw   vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;
    mov   r10,32138 ;
    vbmulfw   vr09,  vr03,  r10 ;
    mov   r12,  6393 ;
    vbmulfw   vr07,  vr03,  r12 ;
    vaddw   vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  r12 ;
    vbmulfw   vr06,  vr05,  r10 ;
    mov   r12,27246 ;
    vbmulfw   vr11,  vr04,  r12 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;
    mov   r10,18205 ;
    vbmulfw   vr09,  vr08,  r10 ;
    vbmulfw   vr07,  vr04,  r10 ;
    vbmulfw   vr08,  vr08,  r12 ;
    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04 ;
    vasrsrw   vr16,  vr16,  6 ;
    vasrsrw   vr01,  vr01,  6 ;
    vasrsrw   vr02,  vr02,  6 ;
    vasrsrw   vr03,  vr03,  6 ;
    vasrsrw   vr04,  vr04,  6 ;
    vasrsrw   vr05,  vr05,  6 ;
    vasrsrw   vr06,  vr06,  6 ;
    vasrsrw   vr07,  vr07,  6
    //ADD_BLOCK ("0");
    vld64w   vr08,  [ i0, 0x00]   ;
    vld64w   vr09,  [ i0, 0x10]   ;
    vld64w   vr10,  [ i0, 0x20]   ;
    vld64w   vr11,  [ i0, 0x30]   ;
    vld64w   vr12,  [ i0, 0x40]   ;
    vld64w   vr13,  [ i0, 0x50]   ;
    vld64w   vr14,  [ i0, 0x60]   ;
    vld64w   vr15,  [ i0, 0x70]   ;
    vaddw   vr16,  vr16,  vr08 ;
    vaddw   vr01,  vr01,  vr09 ;
    vaddw   vr02,  vr02,  vr10 ;
    vaddw   vr03,  vr03,  vr11 ;
    vaddw   vr04,  vr04,  vr12 ;
    vaddw   vr05,  vr05,  vr13 ;
    vaddw   vr06,  vr06,  vr14 ;
    vaddw   vr07,  vr07,  vr15 ;
    vasrpwb   vr16,  vr16, 0 ;
    vasrpwb   vr01,  vr01, 0 ;
    vasrpwb   vr02,  vr02, 0 ;
    vasrpwb   vr03,  vr03, 0 ;
    vasrpwb   vr04,  vr04, 0 ;
    vasrpwb   vr05,  vr05, 0 ;
    vasrpwb   vr06,  vr06, 0 ;
    vasrpwb   vr07,  vr07, 0 ;
    vst64   vr16,  [ i0, 0x00]   ; 
    vst64   vr01,  [ i0, 0x10]   ; 
    vst64   vr02,  [ i0, 0x20]   ; 
    vst64   vr03,  [ i0, 0x30]   ; 
    vst64   vr04,  [ i0, 0x40]   ; 
    vst64   vr05,  [ i0, 0x50]   ; 
    vst64   vr06,  [ i0, 0x60]   ; 
    vst64   vr07,  [ i0, 0x70]   ;
    /* Leave the   r13 clean for the next set of coeff's. */
    vxor   vr01,  vr01,  vr01 ;
    vst128   vr01,  [ i1, 0x00]    ;
    vst128   vr01,  [ i1, 0x10]    ;
    vst128   vr01,  [ i1, 0x20]    ;
    vst128   vr01,  [ i1, 0x30]    ;
    vst128   vr01,  [ i1, 0x40]    ;
    vst128   vr01,  [ i1, 0x50]    ;
    vst128   vr01,  [ i1, 0x60]    ;
    vst128   vr01,  [ i1, 0x70]    ;

.on     percent_reg

}

_Asm void idct_add32_aurora_asm (uint8_t* dest, int line_size, int16_t* block)
{
    %   reg dest, line_size, block
.off    percent_reg

    /* Get dest pointer into I0  */
    /* Get block pointer into I1 */
    vmovzw    vr00, dest,  0x01 ;
    vmovw     vr00, block, 0x02 ;


#ifdef AURORA_SUBPEL_MACROS
    mov       r10, _MACRO_IDCT_ADD32_AURORA_ASM_  ;
    vrun      r10                                 ;

.on     percent_reg

}

_Asm int macro_idct_add32_aurora_asm(int scm_addr)
{
   % reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif


    vld128   vr16,  [ i1, 0x00]    ;
    vld128   vr01,  [ i1, 0x10]    ;
    vld128   vr02,  [ i1, 0x20]    ;
    vld128   vr03,  [ i1, 0x30]    ;
    vld128   vr04,  [ i1, 0x40]    ;
    vld128   vr05,  [ i1, 0x50]    ;
    vld128   vr06,  [ i1, 0x60]    ;
    vld128   vr07,  [ i1, 0x70]   ;

    /* Convert input coefficients to 12Q4 format and transpose to column fmt
    * TODO: this will eventually be done in the dequantization step
    */
    //mov   r10,16 ;
    vbmulw   vr16,  vr16,  16 ;
    vbmulw   vr01,  vr01,  16 ;
    vbmulw   vr02,  vr02,  16 ;
    vbmulw   vr03,  vr03,  16 ;
    vbmulw   vr04,  vr04,  16 ;
    vbmulw   vr05,  vr05,  16 ;
    vbmulw   vr06,  vr06,  16 ;
    vbmulw   vr07,  vr07,  16 ;

    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07 ;

    //mov   r10, 23170 ;
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 ;
    vbmulfw   vr07,  vr02, 12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06, 12540 ;
    vsubw   vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 ;
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 ;
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw   vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 ;
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;

    //mov   r10,18205 ;
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;
    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07 ;

    //mov   r10,23170 ;
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 ;
    vbmulfw   vr07,  vr02, 12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06, 12540 ;
    vsubw   vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 ;
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 ;
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw   vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 ;
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;

    //mov   r10,18205 ;
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;
    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04 ;
    vasrsrw   vr16,  vr16,  6 ;
    vasrsrw   vr01,  vr01,  6 ;
    vasrsrw   vr02,  vr02,  6 ;
    vasrsrw   vr03,  vr03,  6 ;
    vasrsrw   vr04,  vr04,  6 ;
    vasrsrw   vr05,  vr05,  6 ;
    vasrsrw   vr06,  vr06,  6 ;
    vasrsrw   vr07,  vr07,  6
    //ADD_BLOCK32 ("0")
    vld64w   vr08,  [ i0, 0x00]   ;
    vld64w   vr09,  [ i0, 0x20]   ;
    vld64w   vr10,  [ i0, 0x40]   ;
    vld64w   vr11,  [ i0, 0x60]   ;
    vld64w   vr12,  [ i0, 0x80]   ;
    vld64w   vr13,  [ i0, 0xA0]   ;
    vld64w   vr14,  [ i0, 0xC0]   ;
    vld64w   vr15,  [ i0, 0xE0]   ;
    vaddw   vr16,  vr16,  vr08
    vaddw   vr01,  vr01,  vr09
    vaddw   vr02,  vr02,  vr10
    vaddw   vr03,  vr03,  vr11
    vaddw   vr04,  vr04,  vr12
    vaddw   vr05,  vr05,  vr13
    vaddw   vr06,  vr06,  vr14
    vaddw   vr07,  vr07,  vr15
    //STORE_MATRIX32(0)
    vasrpwb   vr16,  vr16, 0
    vasrpwb   vr01,  vr01, 0
    vasrpwb   vr02,  vr02, 0
    vasrpwb   vr03,  vr03, 0
    vasrpwb   vr04,  vr04, 0
    vasrpwb   vr05,  vr05, 0
    vasrpwb   vr06,  vr06, 0
    vasrpwb   vr07,  vr07, 0
    vst64   vr16,  [ i0, 0x00]   ;
    vst64   vr01,  [ i0, 0x20]   ;
    vst64   vr02,  [ i0, 0x40]   ;
    vst64   vr03,  [ i0, 0x60]   ;
    vst64   vr04,  [ i0, 0x80]   ;
    vst64   vr05,  [ i0, 0xA0]   ;
    vst64   vr06,  [ i0, 0xC0]   ;
    vst64   vr07,  [ i0, 0xE0]   ;
    /* Leave the   r13 clean for the next set of coeff's. */
    vxor   vr01,  vr01,  vr01 ;
    vst128   vr01,  [ i1, 0x00]    ;
    vst128   vr01,  [ i1, 0x10]    ;
    vst128   vr01,  [ i1, 0x20]    ;
    vst128   vr01,  [ i1, 0x30]    ;
    vst128   vr01,  [ i1, 0x40]    ;
    vst128   vr01,  [ i1, 0x50]    ;
    vst128   vr01,  [ i1, 0x60]    ;
    vst128   vr01,  [ i1, 0x70]    ;


#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on     percent_reg
}


_Asm void idct_add64_aurora_asm (uint8_t* dest, int line_size, int16_t* block)
{
    %   reg dest, line_size, block
.off    percent_reg

    /* Get dest pointer into I0  */
    /* Get block pointer into I1 */
    vmovzw    vr00, dest,  0x01
    vmovw     vr00, block, 0x02

#ifdef AURORA_SUBPEL_MACROS
    mov       r10, _MACRO_IDCT_ADD64_AURORA_ASM_  ;
    vrun      r10                                ;

.on     percent_reg

}

_Asm int macro_idct_add64_aurora_asm(int scm_addr)
{
    %  reg        scm_addr
.off    percent_reg

    vrec        scm_addr
//-------------------------------------
#endif

    vld128   vr16,  [ i1, 0x00]    ;
    vld128   vr01,  [ i1, 0x10]    ;
    vld128   vr02,  [ i1, 0x20]    ;
    vld128   vr03,  [ i1, 0x30]    ;
    vld128   vr04,  [ i1, 0x40]    ;
    vld128   vr05,  [ i1, 0x50]    ;
    vld128   vr06,  [ i1, 0x60]    ;
    vld128   vr07,  [ i1, 0x70]   ;
    /* Convert input coefficients to 12Q4 format and transpose to column fmt
    * TODO: this will eventually be done in the dequantization step
    */
    //mov   r10,16 ;
    vbmulw   vr16,  vr16,  16 ;
    vbmulw   vr01,  vr01,  16 ;
    vbmulw   vr02,  vr02,  16 ;
    vbmulw   vr03,  vr03,  16 ;
    vbmulw   vr04,  vr04,  16 ;
    vbmulw   vr05,  vr05,  16 ;
    vbmulw   vr06,  vr06,  16 ;
    vbmulw   vr07,  vr07,  16
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07

    //mov   r10,23170 ;
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 ;
    vbmulfw   vr07,  vr02, 12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06, 12540 ;
    vsubw     vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 ;
    vbmulfw   vr09,  vr03,  32138 ;

    //mov   r12,  6393 ;
    vbmulfw   vr07,  vr03,  6393 ;
    vaddw     vr03,  vr10,  vr06 ;
    vaddsuw   vr01,  vr02 ;
    vaddsuw   vr16,  vr03 ;
    vbmulfw   vr10,  vr05,  6393 ;
    vbmulfw   vr06,  vr05,  32138 ;

    //mov   r12,27246 ;
    vbmulfw   vr11,  vr04, 27246 ;
    vsubw   vr05,  vr09,  vr10 ;
    vaddw   vr06,  vr07,  vr06 ;

    //mov   r10,18205 ;
    vbmulfw   vr09,  vr08, 18205 ;
    vbmulfw   vr07,  vr04, 18205 ;
    vbmulfw   vr08,  vr08, 27246 ;
    vaddsuw   vr01,  vr06 ;
    vsubw   vr04,  vr11,  vr09 ;
    vaddw   vr07,  vr07,  vr08 ;
    vaddsuw   vr02,  vr05 ;
    vaddsuw   vr16,  vr07 ;
    vaddsuw   vr03,  vr04
    vexch1   vr16,  vr01 ;
    vexch1   vr02,  vr03 ;
    vexch1   vr04,  vr05 ;
    vexch1   vr06,  vr07 ;
    vexch2   vr16,  vr02 ;
    vexch2   vr01,  vr03 ;
    vexch2   vr04,  vr06 ;
    vexch2   vr05,  vr07 ;
    vexch4   vr16,  vr04 ;
    vexch4   vr01,  vr05 ;
    vexch4   vr02,  vr06 ;
    vexch4   vr03,  vr07

    //mov   r10,23170 ;
    vbmulfw   vr16,  vr16, 23170 ;
    vbmulfw   vr08,  vr01, 23170 ;
    vbmulfw   vr01,  vr04, 23170 ;
    vbmulfw   vr04,  vr07, 23170 ;

    //mov   r10,12540 ;
    vbmulfw   vr07,  vr02, 12540 ;
    vaddsuw   vr16,  vr01 ;
    vaddsuw   vr08,  vr04 ;

    //mov   r12,30274 ;
    vbmulfw   vr09,  vr06,  30274 ;
    vbmulfw   vr10,  vr02,  30274 ;
    vbmulfw   vr06,  vr06,  12540 ;
    vsubw     vr02,  vr07,  vr09 ;
    vaddsuw   vr08,  vr03 ;
    vaddsuw   vr04,  vr05 ;

    //mov   r10,32138 ;
    vbmulfw  vr09, vr03,  32138 ;

    //mov   r12,  6393 ;
    vbmulfw  vr07, vr03,  6393 ;
    vaddw    vr03, vr10, vr06 ;
    vaddsuw  vr01, vr02 ;
    vaddsuw  vr16, vr03 ;
    vbmulfw  vr10, vr05,  6393 ;
    vbmulfw  vr06, vr05,  32138 ;

    //mov   r12,27246 ;
    vbmulfw  vr11, vr04,  27246 ;
    vsubw  vr05, vr09, vr10 ;
    vaddw  vr06, vr07, vr06 ;

    //mov   r10,18205 ;
    vbmulfw  vr09,  vr08,  18205 ;
    vbmulfw  vr07,  vr04,  18205 ;
    vbmulfw  vr08,  vr08,  27246 ;
    vaddsuw  vr01, vr06 ;
    vsubw  vr04, vr11, vr09 ;
    vaddw  vr07, vr07, vr08 ;
    vaddsuw  vr02, vr05 ;
    vaddsuw  vr16, vr07 ;
    vaddsuw  vr03, vr04
    vasrsrw  vr16, vr16,6 ;
    vasrsrw  vr01, vr01,6 ;
    vasrsrw  vr02, vr02,6 ;
    vasrsrw  vr03, vr03,6 ;
    vasrsrw  vr04, vr04,6 ;
    vasrsrw  vr05, vr05,6 ;
    vasrsrw  vr06, vr06,6 ;
    vasrsrw  vr07, vr07,6
    //ADD_BLOCK32 ("0")
    vld64w  vr08, [ i0, 0x00]  ;
    vld64w  vr09, [ i0, 0x40]  ;
    vld64w  vr10, [ i0, 0x80]  ;
    vld64w  vr11, [ i0, 0xC0]  ;
    vld64w  vr12, [ i0, 0x100] ;
    vld64w  vr13, [ i0, 0x140] ;
    vld64w  vr14, [ i0, 0x180] ;
    vld64w  vr15, [ i0, 0x1C0] ;

    vaddw  vr16, vr16, vr08 ;
    vaddw  vr01, vr01, vr09 ;
    vaddw  vr02, vr02, vr10 ;
    vaddw  vr03, vr03, vr11 ;
    vaddw  vr04, vr04, vr12 ;
    vaddw  vr05, vr05, vr13 ;
    vaddw  vr06, vr06, vr14 ;
    vaddw  vr07, vr07, vr15 ;
    //STORE_MATRIX32(0)
    vasrpwb  vr16, vr16, 0 ;
    vasrpwb  vr01, vr01, 0 ;
    vasrpwb  vr02, vr02, 0 ;
    vasrpwb  vr03, vr03, 0 ;
    vasrpwb  vr04, vr04, 0 ;
    vasrpwb  vr05, vr05, 0 ;
    vasrpwb  vr06, vr06, 0 ;
    vasrpwb  vr07, vr07, 0 ;
    vst64  vr16, [ i0, 0x00] ;
    vst64  vr01, [ i0, 0x40] ;
    vst64  vr02, [ i0, 0x80] ;
    vst64  vr03, [ i0, 0xC0] ;
    vst64  vr04, [ i0, 0x100] ;
    vst64  vr05, [ i0, 0x140] ;
    vst64  vr06, [ i0, 0x180] ;
    vst64  vr07, [ i0, 0x1C0] ;
    /* Leave the  r13 clean for the next set of coeff's. */
    vxor  vr01, vr01, vr01 ;
    vst128  vr01, [ i1, 0x00] ;
    vst128  vr01, [ i1, 0x10] ;
    vst128  vr01, [ i1, 0x20] ;
    vst128  vr01, [ i1, 0x30] ;
    vst128  vr01, [ i1, 0x40] ;
    vst128  vr01, [ i1, 0x50] ;
    vst128  vr01, [ i1, 0x60] ;
    vst128  vr01, [ i1, 0x70] ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif
.on     percent_reg

}

#endif //_AURORA_DCT_H_
