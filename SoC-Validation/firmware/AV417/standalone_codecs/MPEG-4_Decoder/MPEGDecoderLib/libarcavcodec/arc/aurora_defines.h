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
 * TOP LEVEL AURORA DEFINES
 */

#ifndef _AURORA_DEFINES_H_
#define _AURORA_DEFINES_H_

#define AURORA_QPEL

#define     SDM_BUF_STRIDE      32


#ifndef __GNUC__ /* Only defined if using gcc, so the below is only
                  * included when using hcac. */
#define SDM_BASE _lr(0xA6)
#define SCM_BASE _lr(0xA7)
#else
#define SDM_BASE 0xC0F00000
#endif

#ifdef __ARC_MPLAYER__
#define     SDM_BASE_ADD        ((unsigned int)(SDM_Bytes))
#else
#define     SDM_BASE_ADD        _lr(0xA6)
#endif

#define AURORA_QPEL_ASM
#define AURORA_DEQUANT
#define AURORA_MPEG_MACROS

#ifdef AURORA_MPEG_MACROS
#define AURORA_SUBPEL_MACROS
#define AURORA_MPEG_DEQUANT_MACROS
#define AURORA_8_H_FILTER_MACROS
#define AURORA_16_H_FILTER_MACROS
//#define PRINT_AURORA_MACRO_ADDR
#endif  //AURORA_MPEG_MACROS

#define    _MACRO_SHIFT_PIXELS8x9_SIMD_                              0x0
#define    _MACRO_PUT_PIXELS8x8_SIMD_                                0x1C
#define    _MACRO_PUT_PIXELS8x8_L2_SIMD_                             0x2D
#define    _MACRO_PUT_PIXELS8x9_L2_SIMD_                             0x4E
#define    _MACRO_PUT_NO_RND_PIXELS8x8_L2_SIMD_                      0x73
#define    _MACRO_PUT_NO_RND_PIXELS8x9_L2_SIMD_                      0x94
#define    _MACRO_AVG_PIXELS8x8_SIMD_                                0xB9
#define    _MACRO_AVG_PIXELS8x8_L2_SIMD_                             0xDA
#define    _MACRO_SHIFT_PIXELS16x9_SIMD_                             0x10B
#define    _MACRO_SHIFT_PIXELS16x17_SIMD_                            0x130
#define    _MACRO_PUT_PIXELS16x16_SIMD_                              0x175
#define    _MACRO_PUT_PIXELS16x16_L2_SIMD_                           0x196
#define    _MACRO_PUT_PIXELS16x17_L2_SIMD_                          0x1D7
#define    _MACRO_PUT_NO_RND_PIXELS16x16_L2_SIMD_                   0x21C
#define    _MACRO_PUT_NO_RND_PIXELS16x17_L2_SIMD_                   0x25D
#define    _MACRO_AVG_PIXELS16x16_SIMD_                             0x2A2
#define    _MACRO_AVG_PIXELS_16x16_L2_SIMD_                         0x2E3
#define    _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 0x344
#define    _MACRO_PUT_MPEG4_QPEL8x5_H_LOWPASS_SIMD_                 0x3B8
#define    _MACRO_PUT_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 0x447
#define    _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_          0x4A1
#define    _MACRO_PUT_NO_RND_MPEG4_QPEL8x5_H_LOWPASS_SIMD_          0x519
#define    _MACRO_PUT_NO_RND_MPEG4_QPEL8x8_V_LOWPASS_SIMD_          0x5AD
#define    _MACRO_AVG_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 0x608
#define    _MACRO_AVG_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 0x672
#define    _MACRO_PUT_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 0x6ED
#define    _MACRO_PUT_NO_RND_MPEG4_QPEL8x16_V_LOWPASS_SIMD_          0x79F
#define    _MACRO_AVG_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 0x852
#define    _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 0x924
#define    _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_          0x952
#define    _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 0x97F


#define    _MACRO_PUT_PIXELS8x4_SIMD_                       0x9B0
#define    _MACRO_PUT_PIXELS8x4_X2_SIMD_                    0x9B9
#define    _MACRO_PUT_PIXELS8x4_Y2_SIMD_                    0x9CA
#define    _MACRO_PUT_PIXELS8x4_XY2_SIMD_                   0x9D8
#define    _MACRO_PUT_NO_RND_PIXELS8x4_X2_SIMD_             0x9F8
#define    _MACRO_PUT_NO_RND_PIXELS8x4_Y2_SIMD_             0xA09
#define    _MACRO_PUT_NO_RND_PIXELS8x4_XY2_SIMD_            0xA17
#define    _MACRO_AVG_PIXELS8x4_SIMD_                       0xA3D
#define    _MACRO_AVG_PIXELS8x4_X2_SIMD_                    0xA4E
#define    _MACRO_AVG_PIXELS8x4_Y2_SIMD_                    0xA67
#define    _MACRO_AVG_PIXELS8x4_XY2_SIMD_                   0xA7D
#define    _MACRO_AVG_NO_RND_PIXELS8x4_X2_SIMD_             0xAA5
#define    _MACRO_AVG_NO_RND_PIXELS8x4_Y2_SIMD_             0xABE
#define    _MACRO_AVG_NO_RND_PIXELS8x4_XY2_SIMD_            0xAD4
#define    _MACRO_PUT_PIXELS16x8_SIMD_                      0xB02
#define    _MACRO_PUT_PIXELS16x8_X2_SIMD_                   0xB13
#define    _MACRO_PUT_PIXELS16x8_Y2_SIMD_                   0xB3C
#define    _MACRO_PUT_PIXELS16x8_XY2_SIMD_                  0xB56
#define    _MACRO_PUT_NO_RND_PIXELS16x8_X2_SIMD_            0xBCD
#define    _MACRO_PUT_NO_RND_PIXELS16x8_Y2_SIMD_            0xBF6
#define    _MACRO_PUT_NO_RND_PIXELS16x8_XY2_SIMD_           0xC10
#define    _MACRO_AVG_PIXELS16x8_SIMD_                      0xC98
#define    _MACRO_AVG_PIXELS16x8_X2_SIMD_                   0xCB9
#define    _MACRO_AVG_PIXELS16x8_Y2_SIMD_                   0xCF2
#define    _MACRO_AVG_PIXELS16x8_XY2_SIMD_                  0xD1C
#define    _MACRO_AVG_NO_RND_PIXELS16x8_X2_SIMD_            0xDB3
#define    _MACRO_AVG_NO_RND_PIXELS16x8_Y2_SIMD_            0xDEC
#define    _MACRO_AVG_NO_RND_PIXELS8x8_XY2_SIMD_            0xE16
#define    _MACRO_DEINTERLEAVE_LUMA_SIMD_                   0x1029
#define    _MACRO_DEINTERLEAVE_CHROMA_SIMD_                 0x104A
#define    _MACRO_IDCT_PUT_AURORA_ASM_                      0xE6B
#define    _MACRO_IDCT_ADD32_AURORA_ASM_                    0xEF5
#define    _MACRO_IDCT_ADD64_AURORA_ASM_                    0xF8F
#define    _MACRO_DCT_UNQUANTIZE_MPEG2_INTRA_SIMD_          0x105B

#define    _MACRO_DCT_UNQUANTIZE_MPEG2_INTER_SIMD_          0x10BB
#define    _MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_           0x1127
#define    _MACRO_DCT_UNQUANTIZE_H263_INTER_SIMD_           0x115B
#define    _MACRO_MPEG1_DEQUANT_INTRA_SIMD_                 0x118C
#define    _MACRO_MPEG1_DEQUANT_INTER_SIMD_                 0x11E8
#define    _MACRO_MPEG2_DEQUANT_INTRA_SIMD_                 0x1261
#define    _MACRO_MPEG2_DEQUANT_INTER_SIMD_                 0x12BE

//#define    _MACRO_DCT_UNQUANTIZE_MPEG2_INTER_SIMD_          0x10B9
//#define    _MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_           0x1125
//#define    _MACRO_DCT_UNQUANTIZE_H263_INTER_SIMD_           0x1159
//#define    _MACRO_MPEG1_DEQUANT_INTRA_SIMD_                 0x118A
//#define    _MACRO_MPEG1_DEQUANT_INTER_SIMD_                 0x11E6
//#define    _MACRO_MPEG2_DEQUANT_INTRA_SIMD_                 0x125F
//#define    _MACRO_MPEG2_DEQUANT_INTER_SIMD_                 0x12BC

#endif //_AURORA_DEFINES_H_
