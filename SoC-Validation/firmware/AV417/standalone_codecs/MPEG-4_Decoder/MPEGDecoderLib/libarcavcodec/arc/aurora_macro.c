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

#ifdef PRINT_AURORA_MACRO_ADDR
//#include <stdio.h>  /* For printfs*/
#endif
#include "../dsputil.h"
#include "aurora_defines.h"
#include "aurora_qpel.h"    /* for all assembly functions */
#include "aurora_dct.h"
#include "aurora_dequant.h"
#include "aurora_deinterleave.h"

static unsigned int next_scm_addr = 0;
static int MpegMacroAddr[256];


#ifdef AURORA_MPEG_MACROS

enum mpg_macro_index
{
    // QPEL8
    aurora_macro_shift_pixels8x9_simd,
    aurora_macro_put_pixels8x8_simd,
    aurora_macro_put_pixels8x8_l2_simd,
    aurora_macro_put_pixels8x9_l2_simd,
    aurora_macro_put_no_rnd_pixels8x8_l2_simd,
    aurora_macro_put_no_rnd_pixels8x9_l2_simd,
    aurora_macro_avg_pixels8x8_simd,
    aurora_macro_avg_pixels8x8_l2_simd,

    //QPEL16
    aurora_macro_shift_pixels16x9_simd,
    aurora_macro_shift_pixels16x17_simd,
    aurora_macro_put_pixels16x16_simd,
    aurora_macro_put_pixels16x16_l2_simd,
    aurora_macro_put_pixels16x17_l2_simd,
    aurora_macro_put_no_rnd_pixels16x16_l2_simd,
    aurora_macro_put_no_rnd_pixels16x17_l2_simd,
    aurora_macro_avg_pixels16x16_simd,
    aurora_macro_avg_pixels16x16_l2_simd,
    
    //HPEL8
    aurora_macro_put_mpeg4_qpel8x4_h_lowpass_simd,
    aurora_macro_put_mpeg4_qpel8x5_h_lowpass_simd,
    aurora_macro_put_mpeg4_qpel8x8_v_lowpass_simd,
    aurora_macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd,
    aurora_macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd,
    aurora_macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd,
    aurora_macro_avg_mpeg4_qpel8x8_v_lowpass_simd,
    aurora_macro_avg_mpeg4_qpel8x4_h_lowpass_simd,
    
    //HPEL16
    aurora_macro_put_mpeg4_qpel8x16_v_lowpass_simd,
    aurora_macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd,
    aurora_macro_avg_mpeg4_qpel8x16_v_lowpass_simd,
    aurora_macro_put_mpeg4_qpel16x1_h_lowpass_simd,
    aurora_macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd,
    aurora_macro_avg_mpeg4_qpel16x1_h_lowpass_simd,

    //SUBPEL8
    aurora_macro_put_pixels8x4_simd,
    aurora_macro_put_pixels8x4_x2_simd,
    aurora_macro_put_pixels8x4_y2_simd,
    aurora_macro_put_pixels8x4_xy2_simd,
    aurora_macro_put_no_rnd_pixels8x4_x2_simd,
    aurora_macro_put_no_rnd_pixels8x4_y2_simd,
    aurora_macro_put_no_rnd_pixels8x4_xy2_simd,
    aurora_macro_avg_pixels8x4_simd,
    aurora_macro_avg_pixels8x4_x2_simd,
    aurora_macro_avg_pixels8x4_y2_simd,
    aurora_macro_avg_pixels8x4_xy2_simd,
    aurora_macro_avg_no_rnd_pixels8x4_x2_simd,
    aurora_macro_avg_no_rnd_pixels8x4_y2_simd,
    aurora_macro_avg_no_rnd_pixels8x4_xy2_simd,

    //SUBPEL16
    aurora_macro_put_pixels16x8_simd,
    aurora_macro_put_pixels16x8_x2_simd,
    aurora_macro_put_pixels16x8_y2_simd,
    aurora_macro_put_pixels16x8_xy2_simd,

    aurora_macro_put_no_rnd_pixels16x8_x2_simd,
    aurora_macro_put_no_rnd_pixels16x8_y2_simd,
    aurora_macro_put_no_rnd_pixels16x8_xy2_simd,

    aurora_macro_avg_pixels16x8_simd,
    aurora_macro_avg_pixels16x8_x2_simd,
    aurora_macro_avg_pixels16x8_y2_simd,
    aurora_macro_avg_pixels16x8_xy2_simd,

    aurora_macro_avg_no_rnd_pixels16x8_x2_simd,
    aurora_macro_avg_no_rnd_pixels16x8_y2_simd,
    aurora_macro_avg_no_rnd_pixels8x8_xy2_simd,

    //De-interleave
    aurora_macro_deinterleave_luma_simd,
    aurora_macro_deinterleave_chroma_simd,

    // DCT related functions
    aurora_macro_idct_put_aurora_asm,
    aurora_macro_idct_add32_aurora_asm,
    aurora_macro_idct_add64_aurora_asm,

    // DEQUANT functions
    aurora_macro_dct_unquantize_mpeg2_intra_simd,
    aurora_macro_dct_unquantize_mpeg2_inter_simd,
    aurora_macro_dct_unquantize_h263_intra_simd,
    aurora_macro_dct_unquantize_h263_inter_simd,
    aurora_macro_mpeg1_dequant_intra_simd,
    aurora_macro_mpeg1_dequant_inter_simd,
    aurora_macro_mpeg2_dequant_intra_simd,
    aurora_macro_mpeg2_dequant_inter_simd,

    //-MAX IDX
    aurora_max_macro_idx
};

inline void RecordMacro(int idx, int end)
{
    MpegMacroAddr[idx] = next_scm_addr;
    next_scm_addr = end+1;
}

#ifdef AURORA_SUBPEL_MACROS
void record_aurora_qpel8()
{
    RecordMacro( aurora_macro_shift_pixels8x9_simd,     macro_shift_pixels8x9_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels8x8_simd,       macro_put_pixels8x8_simd(next_scm_addr));

    RecordMacro( aurora_macro_put_pixels8x8_l2_simd,    macro_put_pixels8x8_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels8x9_l2_simd,    macro_put_pixels8x9_l2_simd(next_scm_addr));

    RecordMacro( aurora_macro_put_no_rnd_pixels8x8_l2_simd, macro_put_no_rnd_pixels8x8_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels8x9_l2_simd, macro_put_no_rnd_pixels8x9_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x8_simd,       macro_avg_pixels8x8_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x8_l2_simd,    macro_avg_pixels8x8_l2_simd(next_scm_addr));

}

void record_aurora_qpel16()
{
    RecordMacro( aurora_macro_shift_pixels16x9_simd,    macro_shift_pixels16x9_simd(next_scm_addr));
    RecordMacro( aurora_macro_shift_pixels16x17_simd,   macro_shift_pixels16x17_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x16_simd,     macro_put_pixels16x16_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x16_l2_simd,  macro_put_pixels16x16_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x17_l2_simd,  macro_put_pixels16x17_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels16x16_l2_simd, macro_put_no_rnd_pixels16x16_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels16x17_l2_simd, macro_put_no_rnd_pixels16x17_l2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x16_simd,     macro_avg_pixels16x16_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x16_l2_simd,  macro_avg_pixels16x16_l2_simd(next_scm_addr));
}

void record_aurora_hpel8()
{
    RecordMacro( aurora_macro_put_mpeg4_qpel8x4_h_lowpass_simd,     macro_put_mpeg4_qpel8x4_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_mpeg4_qpel8x5_h_lowpass_simd,     macro_put_mpeg4_qpel8x5_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_mpeg4_qpel8x8_v_lowpass_simd,     macro_put_mpeg4_qpel8x8_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd, macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd, macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd, macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_mpeg4_qpel8x8_v_lowpass_simd,     macro_avg_mpeg4_qpel8x8_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_mpeg4_qpel8x4_h_lowpass_simd,     macro_avg_mpeg4_qpel8x4_h_lowpass_simd(next_scm_addr));
}

void record_aurora_hpel16()
{
    RecordMacro( aurora_macro_put_mpeg4_qpel8x16_v_lowpass_simd,        macro_put_mpeg4_qpel8x16_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd, macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_mpeg4_qpel8x16_v_lowpass_simd,        macro_avg_mpeg4_qpel8x16_v_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_mpeg4_qpel16x1_h_lowpass_simd,        macro_put_mpeg4_qpel16x1_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd, macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_mpeg4_qpel16x1_h_lowpass_simd,        macro_avg_mpeg4_qpel16x1_h_lowpass_simd(next_scm_addr));

}

void record_aurora_subpel8()
{
    RecordMacro( aurora_macro_put_pixels8x4_simd,               macro_put_pixels8x4_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels8x4_x2_simd,            macro_put_pixels8x4_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels8x4_y2_simd,            macro_put_pixels8x4_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels8x4_xy2_simd,           macro_put_pixels8x4_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels8x4_x2_simd,     macro_put_no_rnd_pixels8x4_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels8x4_y2_simd,     macro_put_no_rnd_pixels8x4_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels8x4_xy2_simd,    macro_put_no_rnd_pixels8x4_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x4_simd,               macro_avg_pixels8x4_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x4_x2_simd,            macro_avg_pixels8x4_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x4_y2_simd,            macro_avg_pixels8x4_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels8x4_xy2_simd,           macro_avg_pixels8x4_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels8x4_x2_simd,     macro_avg_no_rnd_pixels8x4_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels8x4_y2_simd,     macro_avg_no_rnd_pixels8x4_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels8x4_xy2_simd,    macro_avg_no_rnd_pixels8x4_xy2_simd(next_scm_addr));
}

void record_aurora_subpel16()
{
    RecordMacro( aurora_macro_put_pixels16x8_simd,               macro_put_pixels16x8_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x8_x2_simd,            macro_put_pixels16x8_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x8_y2_simd,            macro_put_pixels16x8_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_pixels16x8_xy2_simd,           macro_put_pixels16x8_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels16x8_x2_simd,     macro_put_no_rnd_pixels16x8_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels16x8_y2_simd,     macro_put_no_rnd_pixels16x8_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_put_no_rnd_pixels16x8_xy2_simd,    macro_put_no_rnd_pixels16x8_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x8_simd,               macro_avg_pixels16x8_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x8_x2_simd,            macro_avg_pixels16x8_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x8_y2_simd,            macro_avg_pixels16x8_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_pixels16x8_xy2_simd,           macro_avg_pixels16x8_xy2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels16x8_x2_simd,     macro_avg_no_rnd_pixels16x8_x2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels16x8_y2_simd,     macro_avg_no_rnd_pixels16x8_y2_simd(next_scm_addr));
    RecordMacro( aurora_macro_avg_no_rnd_pixels8x8_xy2_simd,    macro_avg_no_rnd_pixels8x8_xy2_simd(next_scm_addr));
}

void record_aurora_dct_macros()
{
    RecordMacro( aurora_macro_idct_put_aurora_asm,               macro_idct_put_aurora_asm(next_scm_addr));
    RecordMacro( aurora_macro_idct_add32_aurora_asm,             macro_idct_add32_aurora_asm(next_scm_addr));
    RecordMacro( aurora_macro_idct_add64_aurora_asm,             macro_idct_add64_aurora_asm(next_scm_addr));
}

void record_aurora_deinterleave_macros()
{
    RecordMacro( aurora_macro_deinterleave_luma_simd,            macro_deinterleave_luma_simd(next_scm_addr));
    RecordMacro( aurora_macro_deinterleave_chroma_simd,          macro_deinterleave_chroma_simd(next_scm_addr));
}
#endif

#ifdef AURORA_MPEG_DEQUANT_MACROS
void record_aurora_dequant_macros()
{
    RecordMacro( aurora_macro_dct_unquantize_mpeg2_intra_simd,   macro_dct_unquantize_mpeg2_intra_simd(next_scm_addr));
    RecordMacro( aurora_macro_dct_unquantize_mpeg2_inter_simd,   macro_dct_unquantize_mpeg2_inter_simd(next_scm_addr));
    RecordMacro( aurora_macro_dct_unquantize_h263_intra_simd,    macro_dct_unquantize_h263_intra_simd(next_scm_addr));
    RecordMacro( aurora_macro_dct_unquantize_h263_inter_simd,    macro_dct_unquantize_h263_inter_simd(next_scm_addr));

    RecordMacro( aurora_macro_mpeg1_dequant_intra_simd,     macro_mpeg1_dequant_intra_simd(next_scm_addr));
    RecordMacro( aurora_macro_mpeg1_dequant_inter_simd,     macro_mpeg1_dequant_inter_simd(next_scm_addr));
    RecordMacro( aurora_macro_mpeg2_dequant_intra_simd,     macro_mpeg2_dequant_intra_simd(next_scm_addr));
    RecordMacro( aurora_macro_mpeg2_dequant_inter_simd,     macro_mpeg2_dequant_inter_simd(next_scm_addr));
}
#endif //AURORA_MPEG_DEQUANT_MACROS


#ifdef PRINT_AURORA_MACRO_ADDR
    // QPEL8
#undef   _MACRO_SHIFT_PIXELS8x9_SIMD_              /*    aurora_shift_pixels8x9_simd,*/
#undef   _MACRO_PUT_PIXELS8x8_SIMD_                /*    aurora_macro_put_pixels8x8_simd, */
#undef   _MACRO_PUT_PIXELS8x8_L2_SIMD_             /*    aurora_macro_put_pixels8x8_l2_simd, */
#undef   _MACRO_PUT_PIXELS8x9_L2_SIMD_             /*    aurora_macro_put_pixels8x9_l2_simd, */
#undef   _MACRO_PUT_NO_RND_PIXELS8x8_L2_SIMD_      /*    aurora_macro_put_no_rnd_pixels8x8_l2_simd, */
#undef   _MACRO_PUT_NO_RND_PIXELS8x9_L2_SIMD_      /*    aurora_macro_put_no_rnd_pixels8x9_l2_simd, */
#undef   _MACRO_AVG_PIXELS8x8_SIMD_                /*    aurora_macro_avg_pixels8x8_simd, */
#undef   _MACRO_AVG_PIXELS8x8_L2_SIMD_             /*    aurora_macro_avg_pixels8x8_l2_simd, */

    //QPEL16
#undef   _MACRO_SHIFT_PIXELS16x9_SIMD_             /*    aurora_macro_shift_pixels16x9_simd, */
#undef   _MACRO_SHIFT_PIXELS16x17_SIMD_            /*    aurora_macro_shift_pixels16x17_simd, */
#undef   _MACRO_PUT_PIXELS16x16_SIMD_              /*    aurora_macro_put_pixels16x16_simd, */
#undef   _MACRO_PUT_PIXELS16x16_L2_SIMD_           /*    aurora_macro_put_pixels16x16_l2_simd, */
#undef   _MACRO_PUT_PIXELS16x17_L2_SIMD_           /*    aurora_macro_put_pixels16x17_l2_simd, */
#undef   _MACRO_PUT_NO_RND_PIXELS16x16_L2_SIMD_    /*    aurora_macro_put_no_rnd_pixels16x16_l2_simd, */
#undef   _MACRO_PUT_NO_RND_PIXELS16x17_L2_SIMD_    /*    aurora_macro_put_no_rnd_pixels16x17_l2_simd, */
#undef   _MACRO_AVG_PIXELS16x16_SIMD_              /*    aurora_macro_avg_pixels16x16_simd, */
#undef   _MACRO_AVG_PIXELS_16x16_L2_SIMD_          /*    aurora_macro_avg_pixels16x16_l2_simd, */
    
    //HPEL8
#undef   _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 /*    aurora_macro_put_mpeg4_qpel8x4_h_lowpass_simd  */
#undef   _MACRO_PUT_MPEG4_QPEL8x5_H_LOWPASS_SIMD_                 /*    aurora_macro_put_mpeg4_qpel8x5_h_lowpass_simd, */
#undef   _MACRO_PUT_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 /*    aurora_macro_put_mpeg4_qpel8x8_v_lowpass_simd, */
#undef   _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_          /*    aurora_macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd, */
#undef   _MACRO_PUT_NO_RND_MPEG4_QPEL8x5_H_LOWPASS_SIMD_          /*    aurora_macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd, */
#undef   _MACRO_PUT_NO_RND_MPEG4_QPEL8x8_V_LOWPASS_SIMD_          /*    aurora_macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd, */
#undef   _MACRO_AVG_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 /*    aurora_macro_avg_mpeg4_qpel8x8_v_lowpass_simd, */
#undef   _MACRO_AVG_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 /*    aurora_macro_avg_mpeg4_qpel8x4_h_lowpass_simd, */

    //HPEL16
#undef   _MACRO_PUT_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 /*  aurora_macro_put_mpeg4_qpel8x16_v_lowpass_simd, */
#undef   _MACRO_PUT_NO_RND_MPEG4_QPEL8x16_V_LOWPASS_SIMD_          /*  aurora_macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd, */
#undef   _MACRO_AVG_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 /*  aurora_macro_avg_mpeg4_qpel8x16_v_lowpass_simd, */
#undef   _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 /*  aurora_macro_put_mpeg4_qpel16x1_h_lowpass_simd, */
#undef   _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_          /*  aurora_macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd, */
#undef   _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 /*  aurora_macro_avg_mpeg4_qpel16x1_h_lowpass_simd */

    //SUBPEL8
#undef    _MACRO_PUT_PIXELS8x4_SIMD_               /*    aurora_macro_put_pixels8x4_simd, */
#undef    _MACRO_PUT_PIXELS8x4_X2_SIMD_            /*    aurora_macro_put_pixels8x4_x2_simd     */
#undef    _MACRO_PUT_PIXELS8x4_Y2_SIMD_            /*    aurora_macro_put_pixels8x4_y2_simd     */
#undef    _MACRO_PUT_PIXELS8x4_XY2_SIMD_           /*    aurora_macro_put_pixels8x4_xy2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS8x4_X2_SIMD_     /*    aurora_macro_put_no_rnd_pixels8x4_x2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS8x4_Y2_SIMD_     /*    aurora_macro_put_no_rnd_pixels8x4_y2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS8x4_XY2_SIMD_    /*    aurora_macro_put_no_rnd_pixels8x4_xy2_simd     */
#undef    _MACRO_AVG_PIXELS8x4_SIMD_               /*    aurora_macro_avg_pixels8x4_simd     */
#undef    _MACRO_AVG_PIXELS8x4_X2_SIMD_            /*    aurora_macro_avg_pixels8x4_x2_simd     */
#undef    _MACRO_AVG_PIXELS8x4_Y2_SIMD_            /*    aurora_macro_avg_pixels8x4_y2_simd     */
#undef    _MACRO_AVG_PIXELS8x4_XY2_SIMD_           /*    aurora_macro_avg_pixels8x4_xy2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS8x4_X2_SIMD_     /*    aurora_macro_avg_no_rnd_pixels8x4_x2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS8x4_Y2_SIMD_     /*    aurora_macro_avg_no_rnd_pixels8x4_y2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS8x4_XY2_SIMD_    /*    aurora_macro_avg_no_rnd_pixels8x4_xy2_simd     */

    //SUBPEL16
#undef    _MACRO_PUT_PIXELS16x8_SIMD_               /*    aurora_macro_put_pixels16x8_simd, */
#undef    _MACRO_PUT_PIXELS16x8_X2_SIMD_            /*    aurora_macro_put_pixels16x8_x2_simd     */
#undef    _MACRO_PUT_PIXELS16x8_Y2_SIMD_            /*    aurora_macro_put_pixels16x8_y2_simd     */
#undef    _MACRO_PUT_PIXELS16x8_XY2_SIMD_           /*    aurora_macro_put_pixels16x8_xy2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS16x8_X2_SIMD_     /*    aurora_macro_put_no_rnd_pixels16x8_x2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS16x8_Y2_SIMD_     /*    aurora_macro_put_no_rnd_pixels16x8_y2_simd     */
#undef    _MACRO_PUT_NO_RND_PIXELS16x8_XY2_SIMD_    /*    aurora_macro_put_no_rnd_pixels16x8_xy2_simd     */
#undef    _MACRO_AVG_PIXELS16x8_SIMD_               /*    aurora_macro_avg_pixels16x8_simd     */
#undef    _MACRO_AVG_PIXELS16x8_X2_SIMD_            /*    aurora_macro_avg_pixels16x8_x2_simd     */
#undef    _MACRO_AVG_PIXELS16x8_Y2_SIMD_            /*    aurora_macro_avg_pixels16x8_y2_simd     */
#undef    _MACRO_AVG_PIXELS16x8_XY2_SIMD_           /*    aurora_macro_avg_pixels16x8_xy2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS16x8_X2_SIMD_     /*    aurora_macro_avg_no_rnd_pixels16x8_x2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS16x8_Y2_SIMD_     /*    aurora_macro_avg_no_rnd_pixels16x8_y2_simd     */
#undef    _MACRO_AVG_NO_RND_PIXELS8x8_XY2_SIMD_     /*    aurora_macro_avg_no_rnd_pixels8x8_xy2_simd     */

    //DCT
#undef    _MACRO_IDCT_PUT_AURORA_ASM_               /*    aurora_macro_idct_put_aurora_asm      */
#undef    _MACRO_IDCT_ADD32_AURORA_ASM_             /*    aurora_macro_idct_add32_aurora_asm    */
#undef    _MACRO_IDCT_ADD64_AURORA_ASM_             /*    aurora_macro_idct_add64_aurora_asm    */

#undef    _MACRO_DCT_UNQUANTIZE_MPEG2_INTRA_SIMD_   /*    aurora_macro_dct_unquantize_mpeg2_intra_simd  */
#undef    _MACRO_DCT_UNQUANTIZE_MPEG2_INTER_SIMD_   /*    aurora_macro_dct_unquantize_mpeg2_inter_simd  */
#undef    _MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_    /*    aurora_macro_dct_unquantize_h263_intra_simd   */
#undef    _MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_    /*    aurora_macro_dct_unquantize_h263_inter_simd   */

#undef    _MACRO_MPEG1_DEQUANT_INTRA_SIMD_          /*    aurora_macro_mpeg1_dequant_intra_simd */
#undef    _MACRO_MPEG1_DEQUANT_INTER_SIMD_          /*    aurora_macro_mpeg1_dequant_inter_simd */
#undef    _MACRO_MPEG2_DEQUANT_INTRA_SIMD_          /*    aurora_macro_mpeg2_dequant_intra_simd */
#undef    _MACRO_MPEG2_DEQUANT_INTER_SIMD_          /*    aurora_macro_mpeg2_dequant_inter_simd */

void print_aurora_macro_addr()
{

 
    exit(0);
}
#else
void check_aurora_macro_addr()
{

#ifdef AURORA_SUBPEL_MACROS

    if( ( _MACRO_SHIFT_PIXELS8x9_SIMD_              !=    MpegMacroAddr[aurora_macro_shift_pixels8x9_simd] ) ||
        ( _MACRO_PUT_PIXELS8x8_SIMD_                !=    MpegMacroAddr[aurora_macro_put_pixels8x8_simd] ) ||
        ( _MACRO_PUT_PIXELS8x8_L2_SIMD_             !=    MpegMacroAddr[aurora_macro_put_pixels8x8_l2_simd] ) ||
        ( _MACRO_PUT_PIXELS8x9_L2_SIMD_             !=    MpegMacroAddr[aurora_macro_put_pixels8x9_l2_simd] ) ||
        ( _MACRO_PUT_NO_RND_PIXELS8x8_L2_SIMD_      !=    MpegMacroAddr[aurora_macro_put_no_rnd_pixels8x8_l2_simd] ) ||
        ( _MACRO_PUT_NO_RND_PIXELS8x9_L2_SIMD_      !=    MpegMacroAddr[aurora_macro_put_no_rnd_pixels8x9_l2_simd] ) ||
        ( _MACRO_AVG_PIXELS8x8_SIMD_                !=    MpegMacroAddr[aurora_macro_avg_pixels8x8_simd] ) ||
        ( _MACRO_AVG_PIXELS8x8_L2_SIMD_             !=    MpegMacroAddr[aurora_macro_avg_pixels8x8_l2_simd] ) ||
        ( _MACRO_SHIFT_PIXELS16x17_SIMD_            !=    MpegMacroAddr[aurora_macro_shift_pixels16x17_simd] ) ||
        ( _MACRO_PUT_PIXELS16x16_SIMD_              !=    MpegMacroAddr[aurora_macro_put_pixels16x16_simd] ) ||
        ( _MACRO_PUT_PIXELS16x16_L2_SIMD_           !=    MpegMacroAddr[aurora_macro_put_pixels16x16_l2_simd] ) ||
        ( _MACRO_PUT_PIXELS16x17_L2_SIMD_           !=    MpegMacroAddr[aurora_macro_put_pixels16x17_l2_simd] ) ||
        ( _MACRO_PUT_NO_RND_PIXELS16x16_L2_SIMD_    !=    MpegMacroAddr[aurora_macro_put_no_rnd_pixels16x16_l2_simd] ) ||
        ( _MACRO_PUT_NO_RND_PIXELS16x17_L2_SIMD_    !=    MpegMacroAddr[aurora_macro_put_no_rnd_pixels16x17_l2_simd] ) ||
        ( _MACRO_AVG_PIXELS16x16_SIMD_              !=    MpegMacroAddr[aurora_macro_avg_pixels16x16_simd] ) ||
        ( _MACRO_AVG_PIXELS_16x16_L2_SIMD_          !=    MpegMacroAddr[aurora_macro_avg_pixels16x16_l2_simd] ) ||
        ( _MACRO_PUT_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 !=    MpegMacroAddr[aurora_macro_put_mpeg4_qpel8x4_h_lowpass_simd] ) ||
        ( _MACRO_PUT_MPEG4_QPEL8x5_H_LOWPASS_SIMD_                 !=    MpegMacroAddr[aurora_macro_put_mpeg4_qpel8x5_h_lowpass_simd] ) ||
        ( _MACRO_PUT_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 !=    MpegMacroAddr[aurora_macro_put_mpeg4_qpel8x8_v_lowpass_simd] ) ||
        ( _MACRO_PUT_NO_RND_MPEG4_QPEL8x4_H_LOWPASS_SIMD_          !=    MpegMacroAddr[aurora_macro_put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd] ) ||
        ( _MACRO_PUT_NO_RND_MPEG4_QPEL8x5_H_LOWPASS_SIMD_          !=    MpegMacroAddr[aurora_macro_put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd] ) ||
        ( _MACRO_PUT_NO_RND_MPEG4_QPEL8x8_V_LOWPASS_SIMD_          !=    MpegMacroAddr[aurora_macro_put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd] ) ||
        ( _MACRO_AVG_MPEG4_QPEL8x8_V_LOWPASS_SIMD_                 !=    MpegMacroAddr[aurora_macro_avg_mpeg4_qpel8x8_v_lowpass_simd] ) ||
        ( _MACRO_AVG_MPEG4_QPEL8x4_H_LOWPASS_SIMD_                 !=    MpegMacroAddr[aurora_macro_avg_mpeg4_qpel8x4_h_lowpass_simd] ) ||
        ( _MACRO_PUT_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 !=  MpegMacroAddr[aurora_macro_put_mpeg4_qpel8x16_v_lowpass_simd] ) ||
        ( _MACRO_PUT_NO_RND_MPEG4_QPEL8x16_V_LOWPASS_SIMD_          !=  MpegMacroAddr[aurora_macro_put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd] ) ||
        ( _MACRO_AVG_MPEG4_QPEL8x16_V_LOWPASS_SIMD_                 !=  MpegMacroAddr[aurora_macro_avg_mpeg4_qpel8x16_v_lowpass_simd] ) ||
        ( _MACRO_PUT_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 !=  MpegMacroAddr[aurora_macro_put_mpeg4_qpel16x1_h_lowpass_simd] ) ||
        ( _MACRO_PUT_NO_RND_MPEG4_QPEL16x1_H_LOWPASS_SIMD_          !=  MpegMacroAddr[aurora_macro_put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd] ) ||
        ( _MACRO_AVG_MPEG4_QPEL16x1_H_LOWPASS_SIMD_                 !=  MpegMacroAddr[aurora_macro_avg_mpeg4_qpel16x1_h_lowpass_simd] ) )
    {
        exit(0);
    }

    if( ( _MACRO_PUT_PIXELS8x4_SIMD_               !=  MpegMacroAddr[aurora_macro_put_pixels8x4_simd]) ||
        ( _MACRO_PUT_PIXELS8x4_X2_SIMD_            !=  MpegMacroAddr[aurora_macro_put_pixels8x4_x2_simd]) ||
        ( _MACRO_PUT_PIXELS8x4_Y2_SIMD_            !=  MpegMacroAddr[aurora_macro_put_pixels8x4_y2_simd]) ||
        ( _MACRO_PUT_PIXELS8x4_XY2_SIMD_           !=  MpegMacroAddr[aurora_macro_put_pixels8x4_xy2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS8x4_X2_SIMD_     !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels8x4_x2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS8x4_Y2_SIMD_     !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels8x4_y2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS8x4_XY2_SIMD_    !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels8x4_xy2_simd]) ||
        ( _MACRO_AVG_PIXELS8x4_SIMD_               !=  MpegMacroAddr[aurora_macro_avg_pixels8x4_simd]) ||
        ( _MACRO_AVG_PIXELS8x4_X2_SIMD_            !=  MpegMacroAddr[aurora_macro_avg_pixels8x4_x2_simd]) ||
        ( _MACRO_AVG_PIXELS8x4_Y2_SIMD_            !=  MpegMacroAddr[aurora_macro_avg_pixels8x4_y2_simd]) ||
        ( _MACRO_AVG_PIXELS8x4_XY2_SIMD_           !=  MpegMacroAddr[aurora_macro_avg_pixels8x4_xy2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS8x4_X2_SIMD_     !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels8x4_x2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS8x4_Y2_SIMD_     !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels8x4_y2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS8x4_XY2_SIMD_    !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels8x4_xy2_simd]) )
    {
        exit(0);
    }

    if( ( _MACRO_PUT_PIXELS16x8_SIMD_               !=  MpegMacroAddr[aurora_macro_put_pixels16x8_simd]) ||
        ( _MACRO_PUT_PIXELS16x8_X2_SIMD_            !=  MpegMacroAddr[aurora_macro_put_pixels16x8_x2_simd]) ||
        ( _MACRO_PUT_PIXELS16x8_Y2_SIMD_            !=  MpegMacroAddr[aurora_macro_put_pixels16x8_y2_simd]) ||
        ( _MACRO_PUT_PIXELS16x8_XY2_SIMD_           !=  MpegMacroAddr[aurora_macro_put_pixels16x8_xy2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS16x8_X2_SIMD_     !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels16x8_x2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS16x8_Y2_SIMD_     !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels16x8_y2_simd]) ||
        ( _MACRO_PUT_NO_RND_PIXELS16x8_XY2_SIMD_    !=  MpegMacroAddr[aurora_macro_put_no_rnd_pixels16x8_xy2_simd]) ||
        ( _MACRO_AVG_PIXELS16x8_SIMD_               !=  MpegMacroAddr[aurora_macro_avg_pixels16x8_simd]) ||
        ( _MACRO_AVG_PIXELS16x8_X2_SIMD_            !=  MpegMacroAddr[aurora_macro_avg_pixels16x8_x2_simd]) ||
        ( _MACRO_AVG_PIXELS16x8_Y2_SIMD_            !=  MpegMacroAddr[aurora_macro_avg_pixels16x8_y2_simd]) ||
        ( _MACRO_AVG_PIXELS16x8_XY2_SIMD_           !=  MpegMacroAddr[aurora_macro_avg_pixels16x8_xy2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS16x8_X2_SIMD_     !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels16x8_x2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS16x8_Y2_SIMD_     !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels16x8_y2_simd]) ||
        ( _MACRO_AVG_NO_RND_PIXELS8x8_XY2_SIMD_     !=  MpegMacroAddr[aurora_macro_avg_no_rnd_pixels8x8_xy2_simd]) )
    {
        exit(0);
    }

    if( ( _MACRO_DEINTERLEAVE_LUMA_SIMD_            !=  MpegMacroAddr[aurora_macro_deinterleave_luma_simd]) ||
        ( _MACRO_DEINTERLEAVE_CHROMA_SIMD_          !=  MpegMacroAddr[aurora_macro_deinterleave_chroma_simd]) )
    {
        exit(0);
    }
        
    if( ( _MACRO_IDCT_PUT_AURORA_ASM_     !=    MpegMacroAddr[aurora_macro_idct_put_aurora_asm]) ||
        ( _MACRO_IDCT_ADD32_AURORA_ASM_   !=    MpegMacroAddr[aurora_macro_idct_add32_aurora_asm]) ||
        ( _MACRO_IDCT_ADD64_AURORA_ASM_   !=    MpegMacroAddr[aurora_macro_idct_add64_aurora_asm]) )

    {
        exit(0);
    }
#endif

#ifdef AURORA_MPEG_DEQUANT_MACROS
    if( (_MACRO_DCT_UNQUANTIZE_MPEG2_INTRA_SIMD_ != MpegMacroAddr[aurora_macro_dct_unquantize_mpeg2_intra_simd]) ||
        (_MACRO_DCT_UNQUANTIZE_MPEG2_INTER_SIMD_ != MpegMacroAddr[aurora_macro_dct_unquantize_mpeg2_inter_simd]) ||
        (_MACRO_DCT_UNQUANTIZE_H263_INTRA_SIMD_  != MpegMacroAddr[aurora_macro_dct_unquantize_h263_intra_simd]) ||
        (_MACRO_DCT_UNQUANTIZE_H263_INTER_SIMD_  != MpegMacroAddr[aurora_macro_dct_unquantize_h263_inter_simd]) ||
        (_MACRO_MPEG1_DEQUANT_INTRA_SIMD_        != MpegMacroAddr[aurora_macro_mpeg1_dequant_intra_simd]) ||
        (_MACRO_MPEG1_DEQUANT_INTER_SIMD_        != MpegMacroAddr[aurora_macro_mpeg1_dequant_inter_simd]) ||
        (_MACRO_MPEG2_DEQUANT_INTRA_SIMD_        != MpegMacroAddr[aurora_macro_mpeg2_dequant_intra_simd]) ||
        (_MACRO_MPEG2_DEQUANT_INTER_SIMD_        != MpegMacroAddr[aurora_macro_mpeg2_dequant_inter_simd]) )
    {
        exit(0);
    }
#endif

}

#endif


void aurora_mpeg_record_macros()
{
    int idx;

#ifndef PRINT_AURORA_MACRO_ADDR
    // Check for re-recording
    if(next_scm_addr != 0)
    {
        // Macros have been already recorded so just check them
        check_aurora_macro_addr();
        return;
    }
#endif

    // clear macro address array
    for(idx=0; idx<aurora_max_macro_idx; idx++)
    {
        MpegMacroAddr[idx] = 0;
    }

#ifdef AURORA_SUBPEL_MACROS    
    record_aurora_qpel8();
    record_aurora_qpel16();
    record_aurora_hpel8();
    record_aurora_hpel16();
    record_aurora_subpel8();
    record_aurora_subpel16();
    record_aurora_dct_macros();
    record_aurora_deinterleave_macros();
#endif

#ifdef AURORA_MPEG_DEQUANT_MACROS
    record_aurora_dequant_macros();
#endif

#ifdef PRINT_AURORA_MACRO_ADDR
    print_aurora_macro_addr();
#else
    check_aurora_macro_addr();
#endif
}

#endif  //AURORA_MPEG_MACROS
