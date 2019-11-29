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

#ifndef _AURORA_QPEL_H_
#define _AURORA_QPEL_H_

extern void dsputil_qpel_init(DSPContext* c);


extern void put_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h);
extern void put_no_rnd_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h);
extern void avg_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h);

#if !defined(AURORA_8_H_FILTER_MACROS)
extern void put_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
#endif

#if !defined(AURORA_16_H_FILTER_MACROS)
extern void put_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
#endif


#ifndef AURORA_QPEL_ASM
extern void shift_pixels8x9_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void shift_pixels16x17_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_no_rnd_pixels16x17_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_no_rnd_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_no_rnd_pixels8x9_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_pixels16x17_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_pixels8x9_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void avg_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void avg_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add);
extern void put_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);
extern void avg_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add);

#else

#include "aurora_qpel8.h"
#include "aurora_qpel16.h"
#include "aurora_hpel8.h"
#include "aurora_hpel16.h"
#include "aurora_subpel8.h"
#include "aurora_subpel16.h"

#endif//AURORA_QPEL_ASM

#endif
