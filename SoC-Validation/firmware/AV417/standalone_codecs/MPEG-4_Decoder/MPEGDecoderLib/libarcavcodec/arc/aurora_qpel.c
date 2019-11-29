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
#include "../dsputil.h"
#include "aurora_asm.h" /* for START_OF_FIXED_SDM */
#include "aurora_defines.h"
#include "aurora_memory.h"
#include "aurora_qpel.h"
#include "aurora_dma.h"


extern uint8_t cropTbl[256 + 2 * MAX_NEG_CROP];




#undef  op_put
#undef  op_put_no_rnd
#undef  op_avg
#define op_put(a, b) a = cm[((b) + 16)>>5]
#define op_put_no_rnd(a, b) a = cm[((b) + 15)>>5]
#define op_avg(a, b) a = (((a)+cm[((b) + 16)>>5]+1)>>1)

#if !defined(AURORA_8_H_FILTER_MACROS)
void put_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

    dst_sdm_add += (4*SDM_BUF_STRIDE);
    src_sdm_add += (4*SDM_BUF_STRIDE);
    put_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);
    
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    
    for(i=0; i<8; i++)
    {
        op_put(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));
        op_put(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));
        op_put(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));
        op_put(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));
        op_put(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));
        op_put(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));
        op_put(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));
        op_put(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}


void put_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

    dst_sdm_add += (4*SDM_BUF_STRIDE);
    src_sdm_add += (4*SDM_BUF_STRIDE);
    put_mpeg4_qpel8x5_h_lowpass_simd(dst_sdm_add, src_sdm_add);

#else

    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<9; i++)
    {
        op_put(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));
        op_put(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));
        op_put(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));
        op_put(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));
        op_put(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));
        op_put(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));
        op_put(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));
        op_put(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

void put_no_rnd_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

    dst_sdm_add += (4*SDM_BUF_STRIDE);
    src_sdm_add += (4*SDM_BUF_STRIDE);
    put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<8; i++)
    {
        op_put_no_rnd(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));\
        op_put_no_rnd(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));\
        op_put_no_rnd(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));\
        op_put_no_rnd(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));\
        op_put_no_rnd(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));\
        op_put_no_rnd(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));\
        op_put_no_rnd(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));\
        op_put_no_rnd(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));\
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

void put_no_rnd_mpeg4_qpel8x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_no_rnd_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

    dst_sdm_add += (4*SDM_BUF_STRIDE);
    src_sdm_add += (4*SDM_BUF_STRIDE);
    put_no_rnd_mpeg4_qpel8x5_h_lowpass_simd(dst_sdm_add, src_sdm_add);

#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<9; i++)
    {
        op_put_no_rnd(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));\
        op_put_no_rnd(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));\
        op_put_no_rnd(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));\
        op_put_no_rnd(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));\
        op_put_no_rnd(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));\
        op_put_no_rnd(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));\
        op_put_no_rnd(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));\
        op_put_no_rnd(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));\
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

void avg_mpeg4_qpel8x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    avg_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);

    dst_sdm_add += (4*SDM_BUF_STRIDE);
    src_sdm_add += (4*SDM_BUF_STRIDE);
    avg_mpeg4_qpel8x4_h_lowpass_simd(dst_sdm_add, src_sdm_add);
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<8; i++)
    {
        op_avg(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));
        op_avg(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));
        op_avg(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));
        op_avg(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));
        op_avg(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));
        op_avg(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));
        op_avg(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));
        op_avg(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}
#endif  //AURORA_SUBPEL_MACROS

void put_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add, src_sdm_add);
    put_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add+8, src_sdm_add+8);
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    const int w=16;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        const int src9= src[9*SDM_BUF_STRIDE];
        const int src10= src[10*SDM_BUF_STRIDE];
        const int src11= src[11*SDM_BUF_STRIDE];
        const int src12= src[12*SDM_BUF_STRIDE];
        const int src13= src[13*SDM_BUF_STRIDE];
        const int src14= src[14*SDM_BUF_STRIDE];
        const int src15= src[15*SDM_BUF_STRIDE];
        const int src16= src[16*SDM_BUF_STRIDE];
        op_put(dst[ 0], (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ));
        op_put(dst[ 1*SDM_BUF_STRIDE], (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ));
        op_put(dst[ 2*SDM_BUF_STRIDE], (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ));
        op_put(dst[ 3*SDM_BUF_STRIDE], (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ));
        op_put(dst[ 4*SDM_BUF_STRIDE], (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ));
        op_put(dst[ 5*SDM_BUF_STRIDE], (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ));
        op_put(dst[ 6*SDM_BUF_STRIDE], (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10));
        op_put(dst[ 7*SDM_BUF_STRIDE], (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11));
        op_put(dst[ 8*SDM_BUF_STRIDE], (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12));
        op_put(dst[ 9*SDM_BUF_STRIDE], (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13));
        op_put(dst[10*SDM_BUF_STRIDE], (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14));
        op_put(dst[11*SDM_BUF_STRIDE], (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15));
        op_put(dst[12*SDM_BUF_STRIDE], (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16));
        op_put(dst[13*SDM_BUF_STRIDE], (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16));
        op_put(dst[14*SDM_BUF_STRIDE], (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15));
        op_put(dst[15*SDM_BUF_STRIDE], (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14));
        dst++;
        src++;
    }
#endif
}

void put_no_rnd_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add, src_sdm_add);
    put_no_rnd_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add+8, src_sdm_add+8);
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);

    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    const int w=16;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        const int src9= src[9*SDM_BUF_STRIDE];
        const int src10= src[10*SDM_BUF_STRIDE];
        const int src11= src[11*SDM_BUF_STRIDE];
        const int src12= src[12*SDM_BUF_STRIDE];
        const int src13= src[13*SDM_BUF_STRIDE];
        const int src14= src[14*SDM_BUF_STRIDE];
        const int src15= src[15*SDM_BUF_STRIDE];
        const int src16= src[16*SDM_BUF_STRIDE];
        op_put_no_rnd(dst[ 0 ], (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ));
        op_put_no_rnd(dst[ 1*SDM_BUF_STRIDE], (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ));
        op_put_no_rnd(dst[ 2*SDM_BUF_STRIDE], (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ));
        op_put_no_rnd(dst[ 3*SDM_BUF_STRIDE], (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ));
        op_put_no_rnd(dst[ 4*SDM_BUF_STRIDE], (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ));
        op_put_no_rnd(dst[ 5*SDM_BUF_STRIDE], (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ));
        op_put_no_rnd(dst[ 6*SDM_BUF_STRIDE], (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10));
        op_put_no_rnd(dst[ 7*SDM_BUF_STRIDE], (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11));
        op_put_no_rnd(dst[ 8*SDM_BUF_STRIDE], (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12));
        op_put_no_rnd(dst[ 9*SDM_BUF_STRIDE], (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13));
        op_put_no_rnd(dst[10*SDM_BUF_STRIDE], (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14));
        op_put_no_rnd(dst[11*SDM_BUF_STRIDE], (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15));
        op_put_no_rnd(dst[12*SDM_BUF_STRIDE], (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16));
        op_put_no_rnd(dst[13*SDM_BUF_STRIDE], (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16));
        op_put_no_rnd(dst[14*SDM_BUF_STRIDE], (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15));
        op_put_no_rnd(dst[15*SDM_BUF_STRIDE], (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14));
        dst++;
        src++;
    }
#endif
}

void avg_mpeg4_qpel16x16_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
#ifdef AURORA_QPEL_ASM
    avg_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add, src_sdm_add);
    avg_mpeg4_qpel8x16_v_lowpass_simd(dst_sdm_add+8, src_sdm_add+8);
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    const int w=16;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        const int src9= src[9*SDM_BUF_STRIDE];
        const int src10= src[10*SDM_BUF_STRIDE];
        const int src11= src[11*SDM_BUF_STRIDE];
        const int src12= src[12*SDM_BUF_STRIDE];
        const int src13= src[13*SDM_BUF_STRIDE];
        const int src14= src[14*SDM_BUF_STRIDE];
        const int src15= src[15*SDM_BUF_STRIDE];
        const int src16= src[16*SDM_BUF_STRIDE];
        op_avg(dst[ 0 ], (src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ));
        op_avg(dst[ 1*SDM_BUF_STRIDE], (src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ));
        op_avg(dst[ 2*SDM_BUF_STRIDE], (src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ));
        op_avg(dst[ 3*SDM_BUF_STRIDE], (src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ));
        op_avg(dst[ 4*SDM_BUF_STRIDE], (src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ));
        op_avg(dst[ 5*SDM_BUF_STRIDE], (src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ));
        op_avg(dst[ 6*SDM_BUF_STRIDE], (src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10));
        op_avg(dst[ 7*SDM_BUF_STRIDE], (src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11));
        op_avg(dst[ 8*SDM_BUF_STRIDE], (src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12));
        op_avg(dst[ 9*SDM_BUF_STRIDE], (src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13));
        op_avg(dst[10*SDM_BUF_STRIDE], (src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14));
        op_avg(dst[11*SDM_BUF_STRIDE], (src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15));
        op_avg(dst[12*SDM_BUF_STRIDE], (src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16));
        op_avg(dst[13*SDM_BUF_STRIDE], (src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16));
        op_avg(dst[14*SDM_BUF_STRIDE], (src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15));
        op_avg(dst[15*SDM_BUF_STRIDE], (src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14));
        dst++;
        src++;
    }
#endif
}


void put_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h)
{
#ifdef AURORA_QPEL_ASM
    int i;    
    for(i=0; i<h; i++)
    {
        put_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    
    for(i=0; i<h; i++)
    {
        op_put(dst[ 0], (src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]));
        op_put(dst[ 1], (src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]));
        op_put(dst[ 2], (src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]));
        op_put(dst[ 3], (src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]));
        op_put(dst[ 4], (src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]));
        op_put(dst[ 5], (src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]));
        op_put(dst[ 6], (src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]));
        op_put(dst[ 7], (src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]));
        op_put(dst[ 8], (src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]));
        op_put(dst[ 9], (src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]));
        op_put(dst[10], (src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]));
        op_put(dst[11], (src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]));
        op_put(dst[12], (src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]));
        op_put(dst[13], (src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]));
        op_put(dst[14], (src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]));
        op_put(dst[15], (src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

void put_no_rnd_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h){
#ifdef AURORA_QPEL_ASM 
    int i;    
    for(i=0; i<h; i++)
    {
        put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    
    for(i=0; i<h; i++)
    {
        op_put_no_rnd(dst[ 0], (src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]));
        op_put_no_rnd(dst[ 1], (src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]));
        op_put_no_rnd(dst[ 2], (src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]));
        op_put_no_rnd(dst[ 3], (src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]));
        op_put_no_rnd(dst[ 4], (src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]));
        op_put_no_rnd(dst[ 5], (src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]));
        op_put_no_rnd(dst[ 6], (src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]));
        op_put_no_rnd(dst[ 7], (src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]));
        op_put_no_rnd(dst[ 8], (src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]));
        op_put_no_rnd(dst[ 9], (src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]));
        op_put_no_rnd(dst[10], (src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]));
        op_put_no_rnd(dst[11], (src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]));
        op_put_no_rnd(dst[12], (src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]));
        op_put_no_rnd(dst[13], (src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]));
        op_put_no_rnd(dst[14], (src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]));
        op_put_no_rnd(dst[15], (src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

void avg_mpeg4_qpel16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add, int h)
{
#ifdef AURORA_QPEL_ASM 
    int i;    
    for(i=0; i<h; i++)
    {
        avg_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
#else
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;

    for(i=0; i<h; i++)
    {
        op_avg(dst[ 0], (src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]));
        op_avg(dst[ 1], (src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]));
        op_avg(dst[ 2], (src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]));
        op_avg(dst[ 3], (src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]));
        op_avg(dst[ 4], (src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]));
        op_avg(dst[ 5], (src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]));
        op_avg(dst[ 6], (src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]));
        op_avg(dst[ 7], (src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]));
        op_avg(dst[ 8], (src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]));
        op_avg(dst[ 9], (src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]));
        op_avg(dst[10], (src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]));
        op_avg(dst[11], (src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]));
        op_avg(dst[12], (src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]));
        op_avg(dst[13], (src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]));
        op_avg(dst[14], (src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]));
        op_avg(dst[15], (src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]));
        dst+=SDM_BUF_STRIDE;
        src+=SDM_BUF_STRIDE;
    }
#endif
}

#if !defined(AURORA_16_H_FILTER_MACROS)
void put_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<8; i++)
    {
        put_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<9; i++)
    {
        put_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<16; i++)
    {
        put_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<17; i++)
    {
        put_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}


void put_no_rnd_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<8; i++)
    {
        put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_no_rnd_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<9; i++)
    {
        put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_no_rnd_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<16; i++)
    {
        put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void put_no_rnd_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<17; i++)
    {
        put_no_rnd_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}


void avg_mpeg4_qpel16x8_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<8; i++)
    {
        avg_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void avg_mpeg4_qpel16x9_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<9; i++)
    {
        avg_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void avg_mpeg4_qpel16x16_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<16; i++)
    {
        avg_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}

void avg_mpeg4_qpel16x17_h_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int i;    
    for(i=0; i<17; i++)
    {
        avg_mpeg4_qpel16x1_h_lowpass_simd(dst_sdm_add, src_sdm_add);
        dst_sdm_add += SDM_BUF_STRIDE;
        src_sdm_add += SDM_BUF_STRIDE;
    }
}
#endif

#ifndef AURORA_QPEL_ASM
void shift_pixels8x9_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    for(y=0; y<9; y++)
    {
        for(x=0; x<8; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = src[y*SDM_BUF_STRIDE + x + 1];
        }
    }
}
void shift_pixels16x17_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);

    for(y=0; y<17; y++)  // shift 17 rows
    {
        for(x=0; x<16; x++)  // row width = 16
        {
            dst[y*SDM_BUF_STRIDE + x] = src[y*SDM_BUF_STRIDE + x + 1];
        }
    }

}
void shift_pixels16x9_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);

    for(y=0; y<9; y++)  // shift 17 rows
    {
        for(x=0; x<16; x++)  // row width = 16
        {
            dst[y*SDM_BUF_STRIDE + x] = src[y*SDM_BUF_STRIDE + x + 1];
        }
    }

}


void put_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);

    for(y=0; y<8; y++)
    {
        for(x=0; x<8; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = src[y*SDM_BUF_STRIDE + x];
        }
    }

}

void put_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add){
    uint8_t *block;
    uint8_t *pixels;
    block = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    pixels = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    put_pixels8x8_simd(dst_sdm_add  , src_sdm_add  );
    put_pixels8x8_simd(dst_sdm_add+8, src_sdm_add+8);

    put_pixels8x8_simd(dst_sdm_add+(32*8), src_sdm_add+(32*8));
    put_pixels8x8_simd(dst_sdm_add+8+(32*8), src_sdm_add+8+(32*8));

}

void put_no_rnd_pixels16x16_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{

    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);

    for(y=0; y<16; y++)  
    {
        for(x=0; x<16; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)no_rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]);
        }
    }
}


void put_no_rnd_pixels16x17_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{

    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);


    for(y=0; y<17; y++)  
    {
        for(x=0; x<16; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)no_rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]);
        }
    }
}


void put_no_rnd_pixels8x8_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int i;
    int w;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);


    for(i=0; i<8; i++){
        for(w=0; w<8; w++)
        {
            dst[i*SDM_BUF_STRIDE + w] = (uint8_t)((src1[i*SDM_BUF_STRIDE + w] + src2[i*SDM_BUF_STRIDE + w ] )/2);
        }
    }
}

void put_no_rnd_pixels8x9_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int i;
    int w;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);

    for(i=0; i<9; i++){
        for(w=0; w<8; w++)
        {
            dst[i*SDM_BUF_STRIDE + w] = (uint8_t)((src1[i*SDM_BUF_STRIDE + w] + src2[i*SDM_BUF_STRIDE + w ] )/2);
        }
    }
}


void put_pixels16x16_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add){
    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(y=0; y<16; y++)  // shift 17 rows
    {
        for(x=0; x<16; x++)  // row width = 16
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]);
        }
    }
}


void put_pixels16x17_l2_simd(unsigned int dst_sdm_add, 
                    unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(y=0; y<17; y++)  // shift 17 rows
    {
        for(x=0; x<16; x++)  // row width = 16
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]);
        }
    }
}


void put_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int i;
    int w;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(i=0; i<8; i++){
        for(w=0; w<8; w++)
        {
            dst[i*SDM_BUF_STRIDE + w] = (uint8_t)((src1[i*SDM_BUF_STRIDE + w] + src2[i*SDM_BUF_STRIDE + w ] + 1)/2);
        }
    }
}

void put_pixels8x9_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int i;
    int w;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(i=0; i<9; i++){
        for(w=0; w<8; w++)
        {
            dst[i*SDM_BUF_STRIDE + w] = (uint8_t)((src1[i*SDM_BUF_STRIDE + w] + src2[i*SDM_BUF_STRIDE + w ] + 1)/2);
        }
    }
}

void avg_pixels16x16_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    for(y=0; y<16; y++)
    {
        for(x=0; x<16; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)rnd_avg32(dst[y*SDM_BUF_STRIDE + x], src[y*SDM_BUF_STRIDE + x]);
        }
    }
}

void avg_pixels8x8_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    for(y=0; y<8; y++)
    {
        for(x=0; x<8; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] = (uint8_t)rnd_avg32(dst[y*SDM_BUF_STRIDE + x], src[y*SDM_BUF_STRIDE + x]);
        }
    }
}

void avg_pixels16x16_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{

    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(y=0; y<16; y++)
    {
        for(x=0; x<16; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] =  (uint8_t)rnd_avg32(dst[y*SDM_BUF_STRIDE + x],
                rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]));
        }
    }
}

void avg_pixels8x8_l2_simd(unsigned int dst_sdm_add, unsigned int src1_sdm_add, unsigned int src2_sdm_add)
{
    int x, y;
    unsigned char * dst;
    unsigned char * src1;
    unsigned char * src2;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src1 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src1_sdm_add);
    src2 = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src2_sdm_add);
    for(y=0; y<8; y++)
    {
        for(x=0; x<8; x++)
        {
            dst[y*SDM_BUF_STRIDE + x] =  (uint8_t)rnd_avg32(dst[y*SDM_BUF_STRIDE + x],
                rnd_avg32(src1[y*SDM_BUF_STRIDE + x], src2[y*SDM_BUF_STRIDE + x]));
        }
    }
}

void put_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    const int w=8;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        op_put(dst[0], (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4));
        op_put(dst[1*SDM_BUF_STRIDE], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5));
        op_put(dst[2*SDM_BUF_STRIDE], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6));
        op_put(dst[3*SDM_BUF_STRIDE], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7));
        op_put(dst[4*SDM_BUF_STRIDE], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8));
        op_put(dst[5*SDM_BUF_STRIDE], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));
        op_put(dst[6*SDM_BUF_STRIDE], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));
        op_put(dst[7*SDM_BUF_STRIDE], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));
        dst++;
        src++;
    }
}


void put_no_rnd_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    const int w=8;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        op_put_no_rnd(dst[0], (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4));
        op_put_no_rnd(dst[1*SDM_BUF_STRIDE], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5));
        op_put_no_rnd(dst[2*SDM_BUF_STRIDE], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6));
        op_put_no_rnd(dst[3*SDM_BUF_STRIDE], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7));
        op_put_no_rnd(dst[4*SDM_BUF_STRIDE], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8));
        op_put_no_rnd(dst[5*SDM_BUF_STRIDE], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));
        op_put_no_rnd(dst[6*SDM_BUF_STRIDE], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));
        op_put_no_rnd(dst[7*SDM_BUF_STRIDE], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));
        dst++;
        src++;
    }
}

void avg_mpeg4_qpel8x8_v_lowpass_simd(unsigned int dst_sdm_add, unsigned int src_sdm_add)
{
    unsigned char * dst;
    unsigned char * src;
    dst = (unsigned char *)( SDM_BASE_ADD + (unsigned int)dst_sdm_add);
    src = (unsigned char *)( SDM_BASE_ADD + (unsigned int)src_sdm_add);
    const int w=8;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    int i;
    for(i=0; i<w; i++)
    {
        const int src0= src[0];
        const int src1= src[1*SDM_BUF_STRIDE];
        const int src2= src[2*SDM_BUF_STRIDE];
        const int src3= src[3*SDM_BUF_STRIDE];
        const int src4= src[4*SDM_BUF_STRIDE];
        const int src5= src[5*SDM_BUF_STRIDE];
        const int src6= src[6*SDM_BUF_STRIDE];
        const int src7= src[7*SDM_BUF_STRIDE];
        const int src8= src[8*SDM_BUF_STRIDE];
        op_avg(dst[0], (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4));
        op_avg(dst[1*SDM_BUF_STRIDE], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5));
        op_avg(dst[2*SDM_BUF_STRIDE], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6));
        op_avg(dst[3*SDM_BUF_STRIDE], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7));
        op_avg(dst[4*SDM_BUF_STRIDE], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8));
        op_avg(dst[5*SDM_BUF_STRIDE], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));
        op_avg(dst[6*SDM_BUF_STRIDE], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));
        op_avg(dst[7*SDM_BUF_STRIDE], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));
        dst++;
        src++;
    }
}



#endif  // AURORA_QPEL_ASM

