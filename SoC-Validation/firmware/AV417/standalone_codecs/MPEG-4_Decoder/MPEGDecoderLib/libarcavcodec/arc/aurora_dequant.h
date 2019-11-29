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


_Asm void dct_unquantize_mpeg2_intra_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix,
                                            int dc_scale, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, dc_scale, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load dc scaling factor
    vmovzw   vr20, dc_scale, 0x1  ; // vr17: 00 00 00 00  00 00 00 dc

    // load ac scaling factor
    vmovzw   vr21, qscale, 0xfe   ; // vr18: ac ac ac ac  ac ac ac 00
    vmovzw   vr22, qscale, 0xff   ; // vr18: ac ac ac ac  ac ac ac ac

#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_DCT_UNQUANTIZE_MPEG2_INTRA_SIMD_  ;
    vrun      r10                                ;
.on     percent_reg
}

_Asm int macro_dct_unquantize_mpeg2_intra_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    vmovzw   vr18,      1, 0x01   ; // vr18: 00 00 00 00  00 00 00 01
    vmovzw   vr19,      0, 0xff   ; // vr19: 00 00 00 00  00 00 00 00

    // load bit mask for vsummw
    vmovzw   vr23, 0xffff, 0xff     ; // vr23: all 1s

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // calculate dc coef
    vmulw    vr20, vr20, vr01    ;
    vbmaxw   vr20, vr20, 0       ;
    vbminw   vr20, vr20, 2047    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    vmulw   vr01, vr01, vr21  ;   // this zeroes out the dc value which will be added later
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    // shift the ac coeffs
    vasrw   vr01, vr01, 3   ;
    vasrw   vr02, vr02, 3   ;
    vasrw   vr03, vr03, 3   ;
    vasrw   vr04, vr04, 3   ;

    // multiply them with the sign bit
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // add the dc value to row0
    vaddw   vr01, vr01, vr20 ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr09, vr05, vr06 ;
    vaddaw  vr09, vr07, vr08 ;

    //store the 4 rows
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;
 
    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    vmulw   vr01, vr01, vr22  ;
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    // shift the ac coeffs
    vasrw   vr01, vr01, 3   ;
    vasrw   vr02, vr02, 3   ;
    vasrw   vr03, vr03, 3   ;
    vasrw   vr04, vr04, 3   ;

    // multiply them with the sign bit
    vmulw   vr01, vr01, vr05    ;
    vmulw   vr02, vr02, vr06    ;
    vmulw   vr03, vr03, vr07    ;
    vmulw   vr04, vr04, vr08    ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr10, vr05, vr06 ;
    vaddaw  vr10, vr07, vr08 ;
    vaddaw  vr10, vr09, vr18 ;  // invert the last bit

    // sum = sum & 1
    vand    vr10, vr10, vr18 ;  // vr10: 00 00 00 00  00 00 00 0x

    vmr1w   vr10, vr10, vr19 ;  // vr10: 0x 00 00 00  00 00 00 00

    // block[63] ^= (sum&1)
    vxor    vr04, vr04, vr10 ;

    //store the 4 rows
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}


_Asm void dct_unquantize_mpeg2_inter_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load ac scaling factor
    vmovzw   vr20, qscale, 0xff     ; // vr20: ac ac ac ac  ac ac ac ac


#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_DCT_UNQUANTIZE_MPEG2_INTER_SIMD_  ;
    vrun      r10                                ;
.on     percent_reg
}

_Asm int macro_dct_unquantize_mpeg2_inter_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif



    // load bit mask for vsummw
    vmovzw   vr21, 0xffff, 0xff     ; // vr21: all 1s

    // clear a dummy reg
    vxor     vr22, vr22, vr22  ;    // vr22: 00 00 00 00 00 00 00 00
    vmovzw   vr23, 0x1, 1   ;       // vr23: 00 00 00 00 00 00 00 01

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    // level = |level|
    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // left shift by 1 OR multiply by 2
    // level = level<<1
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // add 1 to each value
    // level = (level<<1) + 1
    vbaddw  vr01, vr01, 1 ;
    vbaddw  vr02, vr02, 1 ;
    vbaddw  vr03, vr03, 1 ;
    vbaddw  vr04, vr04, 1 ;

    // multiply by qscale
    // level = ((level<<1)+1) * qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;

    // multiply by quantization matrix
    // level = (((level<<1)+1) * qscale) * quant_matrix
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // right shift by 4
    // level = (((level<<1)+1) * qscale * quant_matrix)>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    // multiply by sign
    // level = ((((level<<1)+1) * qscale * quant_matrix)>>4) * (sign)
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // find sum of all values
    vsummw  vr15, vr01, vr21 ;
    vsummw  vr16, vr02, vr21 ;
    vsummw  vr17, vr03, vr21 ;
    vsummw  vr18, vr04, vr21 ;

    vaddw   vr19, vr15, vr16 ;
    vaddaw  vr19, vr17, vr18 ;

    // store the result
    //store the 4 rows
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;
 
    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // left shift by 1 OR multiply by 2
    // level = level<<1
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // add 1 to each value
    // level = (level<<1) + 1
    vbaddw  vr01, vr01, 1 ;
    vbaddw  vr02, vr02, 1 ;
    vbaddw  vr03, vr03, 1 ;
    vbaddw  vr04, vr04, 1 ;

    // multiply by qscale
    // level = ((level<<1)+1) * qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;

    // multiply by quantization matrix
    // level = (((level<<1)+1) * qscale) * quant_matrix
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // right shift by 4
    // level = (((level<<1)+1) * qscale * quant_matrix)>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    // multiply by sign
    // level = ((((level<<1)+1) * qscale * quant_matrix)>>4) * (sign)
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // find sum of all values
    vsummw  vr15, vr01, vr21 ;
    vsummw  vr16, vr02, vr21 ;
    vsummw  vr17, vr03, vr21 ;
    vsummw  vr18, vr04, vr21 ;

    vaddw   vr19, vr19, vr15 ;
    vaddaw  vr19, vr16, vr17 ;
    vaddaw  vr19, vr18, vr21 ;  // vr21 = -1, remember that sum is initialized to (-1)

    vand    vr19, vr19, vr23 ;  // vr23: 00 00 00 00 00 00 00 01
    vmr1w   vr19, vr19, vr22 ;  // vr22: 00 00 00 00 00 00 00 00
                                // vr19: 0x 00 00 00 00 00 00 00

    vxor    vr04, vr04, vr19 ;

    // store result
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}

_Asm void dct_unquantize_h263_intra_simd(unsigned int dct_coeff_buffer, int dc_scale, int qmul, int qadd)
{
    %   reg dct_coeff_buffer, dc_scale, qmul, qadd
.off    percent_reg
    
    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovzw   vr20, qmul, 0xff ;
    vmovzw   vr21, qadd, 0xff ;

    vmovzw   vr22, dc_scale, 0x1 ;    // vr22: 00 00 00 00 00 00 00 dc
    vmovzw   vr23, 0xffff, 0xfe ;   // vr23: xx xx xx xx xx xx xx 00


#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_dct_unquantize_h263_intra_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;
    vld128   vr05,   [i1,  64]    ;
    vld128   vr06,   [i1,  80]    ;
    vld128   vr07,   [i1,  96]    ;
    vld128   vr08,   [i1, 112]    ;

    // extract and scale the dc value
    vmulw    vr09, vr01, vr22 ;

    // extract sign
    vsignw  vr11, vr01  ;
    vsignw  vr12, vr02  ;
    vsignw  vr13, vr03  ;
    vsignw  vr14, vr04  ;
    vsignw  vr15, vr05  ;
    vsignw  vr16, vr06  ;
    vsignw  vr17, vr07  ;
    vsignw  vr18, vr08  ;

    // multiply qadd with sign: -1/0/1
    vmulw   vr11, vr11, vr21 ;
    vmulw   vr12, vr12, vr21 ;
    vmulw   vr13, vr13, vr21 ;
    vmulw   vr14, vr14, vr21 ;
    vmulw   vr15, vr15, vr21 ;
    vmulw   vr16, vr16, vr21 ;
    vmulw   vr17, vr17, vr21 ;
    vmulw   vr18, vr18, vr21 ;

    // multiply with qmul
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;
    vmulw   vr05, vr05, vr20 ;
    vmulw   vr06, vr06, vr20 ;
    vmulw   vr07, vr07, vr20 ;
    vmulw   vr08, vr08, vr20 ;

    // add qadd
    vaddw   vr01, vr01, vr11 ;
    vaddw   vr02, vr02, vr12 ;
    vaddw   vr03, vr03, vr13 ;
    vaddw   vr04, vr04, vr14 ;
    vaddw   vr05, vr05, vr15 ;
    vaddw   vr06, vr06, vr16 ;
    vaddw   vr07, vr07, vr17 ;
    vaddw   vr08, vr08, vr18 ;

    // extract ac values of row0
    vand   vr01, vr01, vr23 ;
    // add dc and ac values of row0
    vaddw  vr01, vr01, vr09 ;

    // store the result
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;
    vst128   vr05,   [i1,  64]    ;
    vst128   vr06,   [i1,  80]    ;
    vst128   vr07,   [i1,  96]    ;
    vst128   vr08,   [i1, 112]    ;


#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif


.on    percent_reg
}


_Asm void dct_unquantize_h263_inter_simd(unsigned int dct_coeff_buffer, int qmul, int qadd)
{
    %   reg dct_coeff_buffer, qmul, qadd
.off    percent_reg
    
    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovzw   vr20, qmul, 0xff ;
    vmovzw   vr21, qadd, 0xff ;


#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_DCT_UNQUANTIZE_H263_INTER_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_dct_unquantize_h263_inter_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;
    vld128   vr05,   [i1,  64]    ;
    vld128   vr06,   [i1,  80]    ;
    vld128   vr07,   [i1,  96]    ;
    vld128   vr08,   [i1, 112]    ;

    // extract sign: -1/0/1
    vsignw  vr11, vr01  ;
    vsignw  vr12, vr02  ;
    vsignw  vr13, vr03  ;
    vsignw  vr14, vr04  ;
    vsignw  vr15, vr05  ;
    vsignw  vr16, vr06  ;
    vsignw  vr17, vr07  ;
    vsignw  vr18, vr08  ;

    // multiply qadd with sign: -1/0/1
    vmulw   vr11, vr11, vr21 ;
    vmulw   vr12, vr12, vr21 ;
    vmulw   vr13, vr13, vr21 ;
    vmulw   vr14, vr14, vr21 ;
    vmulw   vr15, vr15, vr21 ;
    vmulw   vr16, vr16, vr21 ;
    vmulw   vr17, vr17, vr21 ;
    vmulw   vr18, vr18, vr21 ;

    // multiply coeffs with qmul
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;
    vmulw   vr05, vr05, vr20 ;
    vmulw   vr06, vr06, vr20 ;
    vmulw   vr07, vr07, vr20 ;
    vmulw   vr08, vr08, vr20 ;

    // add qadd
    vaddw   vr01, vr01, vr11 ;
    vaddw   vr02, vr02, vr12 ;
    vaddw   vr03, vr03, vr13 ;
    vaddw   vr04, vr04, vr14 ;
    vaddw   vr05, vr05, vr15 ;
    vaddw   vr06, vr06, vr16 ;
    vaddw   vr07, vr07, vr17 ;
    vaddw   vr08, vr08, vr18 ;

    // store the result
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;
    vst128   vr05,   [i1,  64]    ;
    vst128   vr06,   [i1,  80]    ;
    vst128   vr07,   [i1,  96]    ;
    vst128   vr08,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}

_Asm void mpeg1_dequant_intra_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix,
                                            int dc_coeff, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, dc_coeff, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load dc coeff
    vmovzw   vr20, dc_coeff, 0x1  ; // vr20: 00 00 00 00  00 00 00 dc

    // load ac scaling factor
    vmovzw   vr21, qscale, 0xfe   ; // vr21: ac ac ac ac  ac ac ac 00
    vmovzw   vr22, qscale, 0xff   ; // vr22: ac ac ac ac  ac ac ac ac

#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_MPEG1_DEQUANT_INTRA_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_mpeg1_dequant_intra_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif


    vmovzw   vr18,      1, 0xfe   ; // vr23: 01 01 01 01  01 01 01 00
    vmovzw   vr19,      1, 0xff   ; // vr23: 01 01 01 01  01 01 01 01

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    //level= (level*quant_matrix[j])*qscale
    vmulw   vr01, vr01, vr21  ;   // this zeroes out the dc value which will be added later
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    //level= (level*qscale*quant_matrix[j])>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    vsubw   vr01, vr01, vr18 ;
    vsubw   vr02, vr02, vr19 ;
    vsubw   vr03, vr03, vr19 ;
    vsubw   vr04, vr04, vr19 ;

    vor     vr01, vr01, vr18 ;
    vor     vr02, vr02, vr19 ;
    vor     vr03, vr03, vr19 ;
    vor     vr04, vr04, vr19 ;

    // multiply sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // add the dc value to row0
    vaddw   vr01, vr01, vr20 ;

    //store the 4 rows
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;
 
    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    //level= (level*quant_matrix[j])*qscale
    vmulw   vr01, vr01, vr22  ;
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    //level= (level*qscale*quant_matrix[j])>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    vsubw   vr01, vr01, vr19 ;
    vsubw   vr02, vr02, vr19 ;
    vsubw   vr03, vr03, vr19 ;
    vsubw   vr04, vr04, vr19 ;

    vor     vr01, vr01, vr19 ;
    vor     vr02, vr02, vr19 ;
    vor     vr03, vr03, vr19 ;
    vor     vr04, vr04, vr19 ;

    // multiply sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    //store the result
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}

_Asm void mpeg1_dequant_inter_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load scaling factor
    vmovzw   vr20, qscale, 0xff   ; // vr21: ac ac ac ac  ac ac ac ac
    vmovzw   vr21,      1, 0xff   ; // vr22: 01 01 01 01  01 01 01 01

#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_MPEG1_DEQUANT_INTER_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_mpeg1_dequant_inter_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // level = level*2
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // level = (level*2) + 1
    vaddw   vr01, vr01, vr21 ;
    vaddw   vr02, vr02, vr21 ;
    vaddw   vr03, vr03, vr21 ;
    vaddw   vr04, vr04, vr21 ;

    // level = (level*2 + 1)*qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;


    // level = ((level*2 + 1)*qscale)*quant_matrix[j]
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // level = ((level*2 + 1)*qscale*quant_matrix[j])>>5
    vasrw   vr01, vr01, 5   ;
    vasrw   vr02, vr02, 5   ;
    vasrw   vr03, vr03, 5   ;
    vasrw   vr04, vr04, 5   ;

    // level = level - 1
    vsubw   vr01, vr01, vr21 ;
    vsubw   vr02, vr02, vr21 ;
    vsubw   vr03, vr03, vr21 ;
    vsubw   vr04, vr04, vr21 ;

    // level = (level -1)|1
    vor     vr01, vr01, vr21 ;
    vor     vr02, vr02, vr21 ;
    vor     vr03, vr03, vr21 ;
    vor     vr04, vr04, vr21 ;

    // multiply with sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

	// check for min
	vbmaxw  vr01, vr01, (-2048) ;
	vbmaxw  vr02, vr02, (-2048) ;
	vbmaxw  vr03, vr03, (-2048) ;
	vbmaxw  vr04, vr04, (-2048) ;

	// check for max
	vbminw  vr01, vr01, (2047) ;
	vbminw  vr02, vr02, (2047) ;
	vbminw  vr03, vr03, (2047) ;
	vbminw  vr04, vr04, (2047) ;

    //store the 4 rows
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;
 
    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // level = level*2
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // level = (level*2) + 1
    vaddw   vr01, vr01, vr21 ;
    vaddw   vr02, vr02, vr21 ;
    vaddw   vr03, vr03, vr21 ;
    vaddw   vr04, vr04, vr21 ;

    // level = (level*2 + 1)*qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;


    // level = ((level*2 + 1)*qscale)*quant_matrix[j]
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // level = ((level*2 + 1)*qscale*quant_matrix[j])>>5
    vasrw   vr01, vr01, 5   ;
    vasrw   vr02, vr02, 5   ;
    vasrw   vr03, vr03, 5   ;
    vasrw   vr04, vr04, 5   ;

    // level = level - 1
    vsubw   vr01, vr01, vr21 ;
    vsubw   vr02, vr02, vr21 ;
    vsubw   vr03, vr03, vr21 ;
    vsubw   vr04, vr04, vr21 ;

    // level = (level -1)|1
    vor     vr01, vr01, vr21 ;
    vor     vr02, vr02, vr21 ;
    vor     vr03, vr03, vr21 ;
    vor     vr04, vr04, vr21 ;

    // multiply with sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

	// check for min
	vbmaxw  vr01, vr01, (-2048) ;
	vbmaxw  vr02, vr02, (-2048) ;
	vbmaxw  vr03, vr03, (-2048) ;
	vbmaxw  vr04, vr04, (-2048) ;

	// check for max
	vbminw  vr01, vr01, (2047) ;
	vbminw  vr02, vr02, (2047) ;
	vbminw  vr03, vr03, (2047) ;
	vbminw  vr04, vr04, (2047) ;

    //store the result
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}

_Asm void mpeg2_dequant_intra_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix,
                                            int dc_coeff, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, dc_coeff, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load dc coeff
    vmovzw   vr20, dc_coeff, 0x1  ; // vr20: 00 00 00 00  00 00 00 dc

    // load ac scaling factor
    vmovzw   vr21, qscale, 0xfe   ; // vr21: ac ac ac ac  ac ac ac 00
    vmovzw   vr22, qscale, 0xff   ; // vr22: ac ac ac ac  ac ac ac ac

#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_MPEG2_DEQUANT_INTRA_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_mpeg2_dequant_intra_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif


    vmovzw   vr18,      1, 0x01   ; // vr18: 00 00 00 00  00 00 00 01
    vmovzw   vr19,      0, 0xff   ; // vr19: 00 00 00 00  00 00 00 00

    // load bit mask for vsummw
    vmovzw   vr23, 0xffff, 0xff     ; // vr23: all 1s

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    // level= (level*quant_matrix[j])
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    //level= (level*quant_matrix[j])*qscale
    vmulw   vr01, vr01, vr21  ;   // this zeroes out the dc value which will be added later
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    //level= (level*qscale*quant_matrix[j])>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    // multiply sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // add the dc value to row0
    vaddw   vr01, vr01, vr20 ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr09, vr05, vr06 ;
    vaddaw  vr09, vr07, vr08 ;

    //store result
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;

    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // multiply the ac coeffs with quant_matrix
    // level= (level*quant_matrix[j])
    vmulw   vr01, vr01, vr11  ;
    vmulw   vr02, vr02, vr12  ;
    vmulw   vr03, vr03, vr13  ;
    vmulw   vr04, vr04, vr14  ;

    // multiply ac coeffs with qscale
    //level= (level*quant_matrix[j])*qscale
    vmulw   vr01, vr01, vr22  ;
    vmulw   vr02, vr02, vr22  ;
    vmulw   vr03, vr03, vr22  ;
    vmulw   vr04, vr04, vr22  ;

    //level= (level*qscale*quant_matrix[j])>>4
    vasrw   vr01, vr01, 4   ;
    vasrw   vr02, vr02, 4   ;
    vasrw   vr03, vr03, 4   ;
    vasrw   vr04, vr04, 4   ;

    // multiply sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr10, vr05, vr06 ;
    vaddaw  vr10, vr07, vr08 ;
    vaddaw  vr10, vr09, vr18 ;  // invert the last bit

    // sum = sum & 1
    vand    vr10, vr10, vr18 ;  // vr10: 00 00 00 00  00 00 00 0x

    vmr1w   vr10, vr10, vr19 ;  // vr10: 0x 00 00 00  00 00 00 00

    // block[63] ^= (sum&1)
    vxor    vr04, vr04, vr10 ;

    //store the result
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}

_Asm void mpeg2_dequant_inter_simd(unsigned int dct_coeff_buffer, unsigned int quant_matrix, int qscale)
{
    %   reg dct_coeff_buffer, quant_matrix, qscale
.off    percent_reg

    // load the pointers
    vmovw    vr00, dct_coeff_buffer, 2        ;    // load i1 with dct_coeff_buffer
    vmovw    vr00, quant_matrix,     4        ;    // load i2 with quant_matrix

    // load scaling factor
    vmovzw   vr20, qscale, 0xff   ; // 

    vmovzw   vr18,      1, 0x01   ; // vr18: 00 00 00 00  00 00 00 01
    vmovzw   vr19,      0, 0xff   ; // vr19: 00 00 00 00  00 00 00 00

#ifdef AURORA_MPEG_DEQUANT_MACROS
    mov       r10, _MACRO_MPEG2_DEQUANT_INTER_SIMD_  ;
    vrun      r10                                           ;
.on     percent_reg
}

_Asm int macro_mpeg2_dequant_inter_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif


    // load bit mask for vsummw
    vmovzw   vr23, 0xffff, 0xff     ; // vr23: all 1s

    // Load all the coeffs (buffer stride 8*2=16)
    // this is like a normal 8x8 array of shorts
    vld128   vr01,   [i1,   0]    ;
    vld128   vr02,   [i1,  16]    ;
    vld128   vr03,   [i1,  32]    ;
    vld128   vr04,   [i1,  48]    ;

    // Load the quantization matrix
    // this is again like a normal 8x8 array of shorts
    vld128   vr11,   [i2,   0]    ;
    vld128   vr12,   [i2,  16]    ;
    vld128   vr13,   [i2,  32]    ;
    vld128   vr14,   [i2,  48]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // level = |level|*2
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // level = (|level|*2) + 1
    vbaddw   vr01, vr01, 1 ;
    vbaddw   vr02, vr02, 1 ;
    vbaddw   vr03, vr03, 1 ;
    vbaddw   vr04, vr04, 1 ;

    // level = (|level|*2 + 1)*qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;


    // level = ((level*2 + 1)*qscale)*quant_matrix[j]
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // level = ((level*2 + 1)*qscale*quant_matrix[j])>>5
    vasrw   vr01, vr01, 5   ;
    vasrw   vr02, vr02, 5   ;
    vasrw   vr03, vr03, 5   ;
    vasrw   vr04, vr04, 5   ;

    // level = level * sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

	// check for min
	vbmaxw  vr01, vr01, (-2048) ;
	vbmaxw  vr02, vr02, (-2048) ;
	vbmaxw  vr03, vr03, (-2048) ;
	vbmaxw  vr04, vr04, (-2048) ;

	// check for max
	vbminw  vr01, vr01, (2047) ;
	vbminw  vr02, vr02, (2047) ;
	vbminw  vr03, vr03, (2047) ;
	vbminw  vr04, vr04, (2047) ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr09, vr05, vr06 ;
    vaddaw  vr09, vr07, vr08 ;

    //store result
    vst128   vr01,   [i1,   0]    ;
    vst128   vr02,   [i1,  16]    ;
    vst128   vr03,   [i1,  32]    ;
    vst128   vr04,   [i1,  48]    ;

    // load the next 4 rows
    vld128   vr01,   [i1,  64]    ;
    vld128   vr02,   [i1,  80]    ;
    vld128   vr03,   [i1,  96]    ;
    vld128   vr04,   [i1, 112]    ;

    // load the quant_matrix
    vld128   vr11,   [i2,  64]    ;
    vld128   vr12,   [i2,  80]    ;
    vld128   vr13,   [i2,  96]    ;
    vld128   vr14,   [i2, 112]    ;

    // split the sign and abs value of the coeffs
    vsignw  vr05, vr01  ;
    vsignw  vr06, vr02  ;
    vsignw  vr07, vr03  ;
    vsignw  vr08, vr04  ;

    vabsw   vr01, vr01  ;
    vabsw   vr02, vr02  ;
    vabsw   vr03, vr03  ;
    vabsw   vr04, vr04  ;

    // level = level*2
    vbmulw  vr01, vr01, 2 ;
    vbmulw  vr02, vr02, 2 ;
    vbmulw  vr03, vr03, 2 ;
    vbmulw  vr04, vr04, 2 ;

    // level = (level*2) + 1
    vbaddw   vr01, vr01, 1 ;
    vbaddw   vr02, vr02, 1 ;
    vbaddw   vr03, vr03, 1 ;
    vbaddw   vr04, vr04, 1 ;

    // level = (level*2 + 1)*qscale
    vmulw   vr01, vr01, vr20 ;
    vmulw   vr02, vr02, vr20 ;
    vmulw   vr03, vr03, vr20 ;
    vmulw   vr04, vr04, vr20 ;


    // level = ((level*2 + 1)*qscale)*quant_matrix[j]
    vmulw   vr01, vr01, vr11 ;
    vmulw   vr02, vr02, vr12 ;
    vmulw   vr03, vr03, vr13 ;
    vmulw   vr04, vr04, vr14 ;

    // level = ((level*2 + 1)*qscale*quant_matrix[j])>>5
    vasrw   vr01, vr01, 5   ;
    vasrw   vr02, vr02, 5   ;
    vasrw   vr03, vr03, 5   ;
    vasrw   vr04, vr04, 5   ;

    // level = level * sign
    vmulw   vr01, vr01, vr05 ;
    vmulw   vr02, vr02, vr06 ;
    vmulw   vr03, vr03, vr07 ;
    vmulw   vr04, vr04, vr08 ;

	// check for min
	vbmaxw  vr01, vr01, (-2048) ;
	vbmaxw  vr02, vr02, (-2048) ;
	vbmaxw  vr03, vr03, (-2048) ;
	vbmaxw  vr04, vr04, (-2048) ;

	// check for max
	vbminw  vr01, vr01, (2047) ;
	vbminw  vr02, vr02, (2047) ;
	vbminw  vr03, vr03, (2047) ;
	vbminw  vr04, vr04, (2047) ;

    // find sum of all values
    vsummw  vr05, vr01, vr23 ;
    vsummw  vr06, vr02, vr23 ;
    vsummw  vr07, vr03, vr23 ;
    vsummw  vr08, vr04, vr23 ;

    vaddw   vr10, vr05, vr06 ;
    vaddaw  vr10, vr07, vr08 ;
    vaddaw  vr10, vr09, vr18 ;  // invert the last bit

    // sum = sum & 1
    vand    vr10, vr10, vr18 ;  // vr10: 00 00 00 00  00 00 00 0x

    vmr1w   vr10, vr10, vr19 ;  // vr10: 0x 00 00 00  00 00 00 00

    // block[63] ^= (sum&1)
    vxor    vr04, vr04, vr10 ;

    //store the result
    vst128   vr01,   [i1,  64]    ;
    vst128   vr02,   [i1,  80]    ;
    vst128   vr03,   [i1,  96]    ;
    vst128   vr04,   [i1, 112]    ;

#if defined(AURORA_MPEG_DEQUANT_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on    percent_reg
}
