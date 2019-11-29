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

_Asm void deinterleave_luma_simd(unsigned int block)
{
  % reg block
.off    percent_reg
    vmovzw    vr00,block,0x1 ;

#ifdef AURORA_SUBPEL_MACROS
    mov       r10, _MACRO_DEINTERLEAVE_LUMA_SIMD_  ;
    vrun      r10                                ;
.on     percent_reg
}

_Asm int macro_deinterleave_luma_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    vld128     vr01,[ i0,0x00] ;
    vld128     vr02,[ i0,0x20] ;
    vld128     vr03,[ i0,0x40] ;
    vld128     vr04,[ i0,0x60] ;
    vld128     vr05,[ i0,0x80] ;
    vld128     vr06,[ i0,0xA0] ;
    vld128     vr07,[ i0,0xC0] ;
    vld128     vr08,[ i0,0xE0] ;

    vld128     vr11,[ i0,0x10] ;
    vld128     vr12,[ i0,0x30] ;
    vld128     vr13,[ i0,0x50] ;
    vld128     vr14,[ i0,0x70] ;
    vld128     vr15,[ i0,0x90] ;
    vld128     vr16,[ i0,0xB0] ;
    vld128     vr17,[ i0,0xD0] ;
    vld128     vr18,[ i0,0xF0] ;

    vst128     vr01,[ i0,0x00] ;
    vst128     vr11,[ i0,0x20] ;
    vst128     vr02,[ i0,0x40] ;
    vst128     vr12,[ i0,0x60] ;
    vst128     vr03,[ i0,0x80] ;
    vst128     vr13,[ i0,0xA0] ;
    vst128     vr04,[ i0,0xC0] ;
    vst128     vr14,[ i0,0xE0] ;
    vst128     vr05,[ i0,0x100] ;
    vst128     vr15,[ i0,0x120] ;
    vst128     vr06,[ i0,0x140] ;
    vst128     vr16,[ i0,0x160] ;
    vst128     vr07,[ i0,0x180] ;
    vst128     vr17,[ i0,0x1A0] ;
    vst128     vr08,[ i0,0x1C0] ;
    vst128     vr18,[ i0,0x1E0] ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on     percent_reg
}

/* Interleaves two 8 pixel columns of height 4. */
_Asm void deinterleave_chroma_simd (unsigned int block)
{
  % reg block
.off    percent_reg
    vmovzw    vr00,block,0x1 ;

#ifdef AURORA_SUBPEL_MACROS
    mov       r10, _MACRO_DEINTERLEAVE_CHROMA_SIMD_  ;
    vrun      r10                                ;
.on     percent_reg
}

_Asm int macro_deinterleave_chroma_simd(int scm_addr)
{
     % reg        scm_addr
.off    percent_reg

      vrec        scm_addr
//-------------------------------------
#endif

    vld64     vr01,[ i0,0x00] ;
    vld64     vr02,[ i0,0x20] ;
    vld64     vr03,[ i0,0x40] ;
    vld64     vr04,[ i0,0x60] ;

    vld64     vr11,[ i0,0x08] ;
    vld64     vr12,[ i0,0x28] ;
    vld64     vr13,[ i0,0x48] ;
    vld64     vr14,[ i0,0x68] ;

    vst64     vr01,[ i0,0x00] ;
    vst64     vr11,[ i0,0x20] ;
    vst64     vr02,[ i0,0x40] ;
    vst64     vr12,[ i0,0x60] ;
    vst64     vr03,[ i0,0x80] ;
    vst64     vr13,[ i0,0xA0] ;
    vst64     vr04,[ i0,0xC0] ;
    vst64     vr14,[ i0,0xE0] ;

#if defined(AURORA_SUBPEL_MACROS)
//-------------------------------------
    vendrec   r0              ;
#endif

.on     percent_reg
}

