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
put_no_rnd_qpel_pixels_tab[<block_type>][<dyx>]

block_type  =   0:  for 16x16 blocks
                1:  for 8x8 blocks

dyx         =   ((motion_y & 3) << 2) | (motion_x & 3);

function nomenclature:  put/avg_qpel8_mcXY_simd

    where X/Y specify the horizontal/vertical pixel location
*/

#include "../dsputil.h"
#include "aurora_defines.h"
#include "aurora_asm.h"
#include "aurora_memory.h"
#include "aurora_qpel.h"

extern uint8_t cropTbl[256 + 2 * MAX_NEG_CROP];

#undef PIXOP2
#undef op_avg
#undef op_put

#define op_put(a, b) a = b

#undef op_avg
#undef op_put
#undef op_avg_no_rnd
#undef op_put_no_rnd


#define op_put(a, b) a = cm[((b) + 16)>>5]

static void put_qpel8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels8x8_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_qpel8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
    put_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)shftd_src, (unsigned int)half);
}

static void put_qpel8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_qpel8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);
}

static void put_qpel8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_qpel8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}
static void put_qpel8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void put_qpel8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH+SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void put_qpel8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}
static void put_qpel8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH+SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void put_qpel8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void put_qpel8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void put_qpel8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void put_qpel16_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels16x16_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel16_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_qpel16_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 16);
    put_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel16_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)shftd_src, (unsigned int)half);
}

static void put_qpel16_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_qpel16_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel16_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);
}
static void put_qpel16_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}
static void put_qpel16_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_qpel16_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_qpel16_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_qpel16_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_qpel16_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_qpel16_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

static void put_qpel16_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

static void put_qpel16_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

/* BLOCK SIZE 16x8*/
//------Generic Functions-START
static void put_pixels16x8_l2_simd(unsigned int dst,  unsigned int src1,  unsigned int src2)
{
    put_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src1, (unsigned int)src2);
    put_pixels8x8_l2_simd((unsigned int)dst + 8, (unsigned int)src1 + 8, (unsigned int)src2 + 8);
}

static void put_pixels16x9_l2_simd( unsigned int dst,  unsigned int src1,  unsigned int src2)
{
    put_pixels8x9_l2_simd((unsigned int)dst,(unsigned int)src1, (unsigned int)src2);
    put_pixels8x9_l2_simd((unsigned int)dst + 8, (unsigned int)src1 + 8, (unsigned int)src2 + 8);
}

static void put_mpeg4_qpel16x8_v_lowpass_simd(unsigned int dst, unsigned int src)
{
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst+8, (unsigned int)src+8);
}

//-------Generic Functions-END

static void put_qpel16x8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels16x8_simd((unsigned int)dst, (unsigned int)src);
}

static void put_qpel16x8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);          //b(-1)
    put_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);          //b(-1)
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);   //e(-1)
}

static void put_qpel16x8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 8);           //b(-1)
    put_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);           //b(-1)
}

static void put_qpel16x8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);              //b(-1)
    put_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);              //b(-1)
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)shftd_src, (unsigned int)half); //f(-1)
}

static void put_qpel16x8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);       //g
}

static void put_qpel16x8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);                //c
}

static void put_qpel16x8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    put_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);                                    //m
    
}

static void put_qpel16x8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //h

}

static void put_qpel16x8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);       //j
}

static void put_qpel16x8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    put_pixels16x8_l2_simd((unsigned int)dst, 
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                        //n
}

static void put_qpel16x8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    put_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                            //p
}

static void put_qpel16x8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //d
    put_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //i
}

static void put_qpel16x8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);     //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);        //b(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);   //d
    put_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                //o
}

static void put_qpel16x8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);         //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);//e(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);          //k
}

static void put_qpel16x8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);                  //l
}

static void put_qpel16x8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9); //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);  //d
}






#undef op_put
//----------------------------------------------------------------------------------------
//----- ROUNDING CONTROL IS 0
//----------------------------------------------------------------------------------------


#define op_put(a, b) a = b


#define op_put_no_rnd(a, b) a = cm[((b) + 15)>>5]
static void put_no_rnd_qpel8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels8x8_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst, 
        (unsigned int)src, (unsigned int)half);
}

static void put_no_rnd_qpel8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
    put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)shftd_src, (unsigned int)half);
}

static void put_no_rnd_qpel8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_no_rnd_qpel8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);
}

static void put_no_rnd_qpel8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV );
}

static void put_no_rnd_qpel8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_no_rnd_qpel8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_no_rnd_qpel8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_no_rnd_qpel8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_no_rnd_qpel8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_no_rnd_qpel8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

static void put_no_rnd_qpel8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

static void put_no_rnd_qpel8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

static void put_no_rnd_qpel16_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels16x16_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel16_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);
}

static void put_no_rnd_qpel16_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 16);
    put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel16_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst, 
        (unsigned int)shftd_src, (unsigned int)half);
}

static void put_no_rnd_qpel16_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)src, (unsigned int)half);
}

static void put_no_rnd_qpel16_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel16_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)src+ stride, (unsigned int)half);
}
static void put_no_rnd_qpel16_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_no_rnd_qpel16_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH, 
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}

static void put_no_rnd_qpel16_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH+ SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_no_rnd_qpel16_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH+SDM_BUF_STRIDE, (unsigned int)halfHV);
}

static void put_no_rnd_qpel16_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH, (unsigned int)halfHV);
}
static void put_no_rnd_qpel16_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    put_no_rnd_pixels16x16_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void put_no_rnd_qpel16_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void put_no_rnd_qpel16_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_pixels16x17_l2_simd((unsigned int)halfH, 
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void put_no_rnd_qpel16_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}


/* BLOCK SIZE 16x8*/
//------Generic Functions-START
static void put_no_rnd_pixels16x8_l2_simd(unsigned int dst,  unsigned int src1,  unsigned int src2)
{
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src1, (unsigned int)src2);
    put_no_rnd_pixels8x8_l2_simd((unsigned int)dst + 8, (unsigned int)src1 + 8, (unsigned int)src2 + 8);
}

static void put_no_rnd_pixels16x9_l2_simd( unsigned int dst,  unsigned int src1,  unsigned int src2)
{
    put_no_rnd_pixels8x9_l2_simd((unsigned int)dst,(unsigned int)src1, (unsigned int)src2);
    put_no_rnd_pixels8x9_l2_simd((unsigned int)dst + 8, (unsigned int)src1 + 8, (unsigned int)src2 + 8);
}

static void put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd(unsigned int dst, unsigned int src)
{
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
    put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst+8, (unsigned int)src+8);
}

//-------Generic Functions-END

static void put_no_rnd_qpel16x8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    put_pixels16x8_simd((unsigned int)dst, (unsigned int)src);
}

static void put_no_rnd_qpel16x8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);          //b(-1)
    put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);          //b(-1)
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);   //e(-1)
}

static void put_no_rnd_qpel16x8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 8);           //b(-1)
    put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);           //b(-1)
}

static void put_no_rnd_qpel16x8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);              //b(-1)
    put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);              //b(-1)
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)shftd_src, (unsigned int)half); //f(-1)
}

static void put_no_rnd_qpel16x8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);       //g
}

static void put_no_rnd_qpel16x8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);                //c
}

static void put_no_rnd_qpel16x8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);                                    //m
    
}

static void put_no_rnd_qpel16x8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //h

}

static void put_no_rnd_qpel16x8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);       //j
}

static void put_no_rnd_qpel16x8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, 
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                        //n
}

static void put_no_rnd_qpel16x8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                            //p
}

static void put_no_rnd_qpel16x8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //d
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //i
}

static void put_no_rnd_qpel16x8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);     //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);   //d
    put_no_rnd_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                //o
}

static void put_no_rnd_qpel16x8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);         //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);//e(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);          //k
}

static void put_no_rnd_qpel16x8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);                  //l
}

static void put_no_rnd_qpel16x8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_no_rnd_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9); //b(-1)
    put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_no_rnd_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);  //d
}


//----------------------------------------------------------------------------------------

#undef op_avg


#define op_avg(a, b) a = (((a)+cm[((b) + 16)>>5]+1)>>1)
static void avg_qpel8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    avg_pixels8x8_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);
}

static void avg_qpel8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
    avg_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)shftd_src, (unsigned int)half);
}

static void avg_qpel8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);
}

static void avg_qpel8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src + stride, (unsigned int)half);
}
static void avg_qpel8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH+SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH+SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void avg_qpel8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels8x9_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels8x9_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void avg_qpel8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    put_mpeg4_qpel8x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void avg_qpel16_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    avg_pixels16x16_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel16_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);
}

static void avg_qpel16_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    avg_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 16);
    avg_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel16_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 16);
    put_mpeg4_qpel16x16_h_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)shftd_src, (unsigned int)half);
}

static void avg_qpel16_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);
}

static void avg_qpel16_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    avg_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel16_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)half, (unsigned int)src);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)src + stride, (unsigned int)half);
}
static void avg_qpel16_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel16_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel16_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel16_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, 
                            (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel16_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);
}
static void avg_qpel16_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);
    avg_pixels16x16_l2_simd((unsigned int)dst, (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);
}
static void avg_qpel16_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)src);
    avg_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void avg_qpel16_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x17_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    put_pixels16x17_l2_simd((unsigned int)halfH,
        (unsigned int)halfH, (unsigned int)shftd_src);
    avg_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}
static void avg_qpel16_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 17);
    put_mpeg4_qpel16x17_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);
    avg_mpeg4_qpel16x16_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);
}

/* BLOCK SIZE 16x8*/
//------Generic Functions-START
static void avg_pixels16x8_l2_simd(unsigned int dst,  unsigned int src1,  unsigned int src2)
{
    avg_pixels8x8_l2_simd((unsigned int)dst, (unsigned int)src1, (unsigned int)src2);
    avg_pixels8x8_l2_simd((unsigned int)dst + 8, (unsigned int)src1 + 8, (unsigned int)src2 + 8);
}

static void avg_mpeg4_qpel16x8_v_lowpass_simd(unsigned int dst, unsigned int src)
{
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);
    avg_mpeg4_qpel8x8_v_lowpass_simd((unsigned int)dst+8, (unsigned int)src+8);
}
//-------Generic Functions-END

static void avg_qpel16x8_mc00_simd (uint8_t *dst, uint8_t *src, int stride){
    avg_pixels16x8_simd((unsigned int)dst, (unsigned int)src);
}

static void avg_qpel16x8_mc10_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);          //b(-1)
    put_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);          //b(-1)
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);   //e(-1)
}

static void avg_qpel16x8_mc20_simd(uint8_t *dst, uint8_t *src, int stride){
//    avg_mpeg4_qpel16_h_lowpass_simd((unsigned int)dst, (unsigned int)src, 8);           //b(-1)
    avg_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)dst, (unsigned int)src);           //b(-1)
}

static void avg_qpel16x8_mc30_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)half, (unsigned int)src, 8);              //b(-1)
    put_mpeg4_qpel16x8_h_lowpass_simd((unsigned int)half, (unsigned int)src);              //b(-1)
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)shftd_src, (unsigned int)half); //f(-1)
}

static void avg_qpel16x8_mc01_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)src, (unsigned int)half);       //g
}

static void avg_qpel16x8_mc02_simd(uint8_t *dst, uint8_t *src, int stride){
    avg_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)src);                //c
}

static void avg_qpel16x8_mc03_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *half = (uint8_t *)sdm_ver_filter_output_buffer;
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)half, (unsigned int)src);               //c
    avg_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)src + stride, (unsigned int)half);                                    //m
    
}

static void avg_qpel16x8_mc11_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);             //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //h

}

static void avg_qpel16x8_mc31_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);       //j
}

static void avg_qpel16x8_mc13_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);    //e(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //k
    avg_pixels16x8_l2_simd((unsigned int)dst, 
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                        //n
}

static void avg_qpel16x8_mc33_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);               //l
    avg_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                            //p
}

static void avg_qpel16x8_mc21_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);             //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);           //d
    avg_pixels16x8_l2_simd((unsigned int)dst, (unsigned int)halfH, (unsigned int)halfHV);   //i
}

static void avg_qpel16x8_mc23_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *halfHV = (uint8_t *)sdm_cpel_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);     //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)halfHV, (unsigned int)halfH);   //d
    avg_pixels16x8_l2_simd((unsigned int)dst,
        (unsigned int)halfH + SDM_BUF_STRIDE, (unsigned int)halfHV);                //o
}

static void avg_qpel16x8_mc12_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);         //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)src);//e(-1)
    avg_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);          //k
}

static void avg_qpel16x8_mc32_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
    uint8_t *shftd_src = (uint8_t *)sdm_shifted_input_buffer;
    shift_pixels16x9_simd((unsigned int)shftd_src, (unsigned int)src);
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9);                 //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    put_pixels16x9_l2_simd((unsigned int)halfH, (unsigned int)halfH, (unsigned int)shftd_src);  //f(-1)
    avg_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);                  //l
}

static void avg_qpel16x8_mc22_simd(uint8_t *dst, uint8_t *src, int stride){
    uint8_t *halfH = (uint8_t *)sdm_hor_filter_output_buffer;
//    put_mpeg4_qpel16_h_lowpass_simd((unsigned int)halfH, (unsigned int)src, 9); //b(-1)
    put_mpeg4_qpel16x9_h_lowpass_simd((unsigned int)halfH, (unsigned int)src);                 //b(-1)
    avg_mpeg4_qpel16x8_v_lowpass_simd((unsigned int)dst, (unsigned int)halfH);  //d
}






#undef op_avg
#undef op_put
#undef op_avg_no_rnd
#undef op_put_no_rnd


//----------------------------------------------------------------------------------------
void dsputil_qpel_init(DSPContext* c )
{
#define dspfunc(PFX1, PFX2, IDX, NUM) \
    c->PFX1 ## _pixels_tab[IDX][ 0] = PFX2 ## NUM ## _mc00_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 1] = PFX2 ## NUM ## _mc10_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 2] = PFX2 ## NUM ## _mc20_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 3] = PFX2 ## NUM ## _mc30_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 4] = PFX2 ## NUM ## _mc01_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 5] = PFX2 ## NUM ## _mc11_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 6] = PFX2 ## NUM ## _mc21_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 7] = PFX2 ## NUM ## _mc31_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 8] = PFX2 ## NUM ## _mc02_simd; \
    c->PFX1 ## _pixels_tab[IDX][ 9] = PFX2 ## NUM ## _mc12_simd; \
    c->PFX1 ## _pixels_tab[IDX][10] = PFX2 ## NUM ## _mc22_simd; \
    c->PFX1 ## _pixels_tab[IDX][11] = PFX2 ## NUM ## _mc32_simd; \
    c->PFX1 ## _pixels_tab[IDX][12] = PFX2 ## NUM ## _mc03_simd; \
    c->PFX1 ## _pixels_tab[IDX][13] = PFX2 ## NUM ## _mc13_simd; \
    c->PFX1 ## _pixels_tab[IDX][14] = PFX2 ## NUM ## _mc23_simd; \
    c->PFX1 ## _pixels_tab[IDX][15] = PFX2 ## NUM ## _mc33_simd

    dspfunc(put_qpel, put_qpel, 0, 16);
    dspfunc(put_qpel, put_qpel, 1, 8);
    dspfunc(put_qpel, put_qpel, 2, 16x8);

    dspfunc(put_no_rnd_qpel, put_no_rnd_qpel, 0, 16);
    dspfunc(put_no_rnd_qpel, put_no_rnd_qpel, 1, 8);
    dspfunc(put_no_rnd_qpel, put_no_rnd_qpel, 2, 16x8);

    dspfunc(avg_qpel, avg_qpel, 0, 16);
    dspfunc(avg_qpel, avg_qpel, 1, 8);
    dspfunc(avg_qpel, avg_qpel, 2, 16x8);
#undef   dspfunc
}

