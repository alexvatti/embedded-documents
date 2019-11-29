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
#include "aurora_defines.h"
#include "aurora_qpel.h"


#ifndef AURORA_QPEL_ASM

static void put_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    int i; 
    for(i=0; i<h; i++)
    { 
        uint32_t a,b; 
        a= (((*(&src1[i*src_stride1 ] + 3))<<24) | ((*(&src1[i*src_stride1 ] + 2))<<16) | ((*(&src1[i*src_stride1 ] + 1))<<8) | *(&src1[i*src_stride1 ] + 0)); 
        b= (((*(&src2[i*src_stride2 ] + 3))<<24) | ((*(&src2[i*src_stride2 ] + 2))<<16) | ((*(&src2[i*src_stride2 ] + 1))<<8) | *(&src2[i*src_stride2 ] + 0)); 
        *((uint32_t*)&dst[i*dst_stride ]) = rnd_avg32(a, b); a= (((*(&src1[i*src_stride1+4] + 3))<<24) | ((*(&src1[i*src_stride1+4] + 2))<<16) | ((*(&src1[i*src_stride1+4] + 1))<<8) | *(&src1[i*src_stride1+4] + 0)); 
        b= (((*(&src2[i*src_stride2+4] + 3))<<24) | ((*(&src2[i*src_stride2+4] + 2))<<16) | ((*(&src2[i*src_stride2+4] + 1))<<8) | *(&src2[i*src_stride2+4] + 0)); 
        *((uint32_t*)&dst[i*dst_stride+4]) = rnd_avg32(a, b); 
    }
}

static void put_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    int i; 
    for(i=0; i<h; i++)
    { 
        *((uint32_t*)(block )) = (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
        *((uint32_t*)(block+4)) = (((*(pixels+4 + 3))<<24) | ((*(pixels+4 + 2))<<16) | ((*(pixels+4 + 1))<<8) | *(pixels+4 + 0)); 
        pixels+=line_size; 
        block +=line_size; 
    }
}

static void put_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);
}

static void put_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);
}

static void put_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    int j; 
    for(j=0; j<2; j++)
    { 
        int i; 
        const uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
        const uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
        uint32_t l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x02020202UL;
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
        uint32_t l1,h1; 
        pixels+=line_size; 
        for(i=0; i<h; i+=2)
        { 
            uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l1= (a&0x03030303UL) + (b&0x03030303UL);
            h1= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL);
            pixels+=line_size; block +=line_size; 
            a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x02020202UL; 
            h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL); 
            pixels+=line_size; block +=line_size; 
        } 
        pixels+=4-line_size*(h+1); 
        block +=4-line_size*h; 
    }
}


static void put_no_rnd_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    int i; 
    for(i=0; i<h; i++)
    { 
        uint32_t a,b; 
        a= (((*(&src1[i*src_stride1 ] + 3))<<24) | ((*(&src1[i*src_stride1 ] + 2))<<16) | ((*(&src1[i*src_stride1 ] + 1))<<8) | *(&src1[i*src_stride1 ] + 0)); 
        b= (((*(&src2[i*src_stride2 ] + 3))<<24) | ((*(&src2[i*src_stride2 ] + 2))<<16) | ((*(&src2[i*src_stride2 ] + 1))<<8) | *(&src2[i*src_stride2 ] + 0)); 
        *((uint32_t*)&dst[i*dst_stride ]) = no_rnd_avg32(a, b); 
        a= (((*(&src1[i*src_stride1+4] + 3))<<24) | ((*(&src1[i*src_stride1+4] + 2))<<16) | ((*(&src1[i*src_stride1+4] + 1))<<8) | *(&src1[i*src_stride1+4] + 0)); 
        b= (((*(&src2[i*src_stride2+4] + 3))<<24) | ((*(&src2[i*src_stride2+4] + 2))<<16) | ((*(&src2[i*src_stride2+4] + 1))<<8) | *(&src2[i*src_stride2+4] + 0)); 
        *((uint32_t*)&dst[i*dst_stride+4]) = no_rnd_avg32(a, b); 
    }
}

static void put_no_rnd_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    put_pixels8_simd(block, pixels, line_size, h);
}

static void put_no_rnd_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_no_rnd_pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);
}

static void put_no_rnd_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_no_rnd_pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);
}

static void put_no_rnd_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    int j; 
    for(j=0; j<2; j++)
    { 
        int i; 
        const uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
        const uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
        uint32_t l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x01010101UL; uint32_t h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
        uint32_t l1,h1; 
        pixels+=line_size; 
        for(i=0; i<h; i+=2)
        { 
            uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l1= (a&0x03030303UL) + (b&0x03030303UL); 
            h1= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL); 
            pixels+=line_size; 
            block +=line_size; 
            a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x01010101UL; h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL); pixels+=line_size; block +=line_size; 
        } 
        pixels+=4-line_size*(h+1); 
        block +=4-line_size*h; 
    }
}

static void avg_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    int i; 
    for(i=0; i<h; i++)
    { 
        uint32_t a,b; 
        a= (((*(&src1[i*src_stride1 ] + 3))<<24) | ((*(&src1[i*src_stride1 ] + 2))<<16) | ((*(&src1[i*src_stride1 ] + 1))<<8) | *(&src1[i*src_stride1 ] + 0)); 
        b= (((*(&src2[i*src_stride2 ] + 3))<<24) | ((*(&src2[i*src_stride2 ] + 2))<<16) | ((*(&src2[i*src_stride2 ] + 1))<<8) | *(&src2[i*src_stride2 ] + 0)); 
        *((uint32_t*)&dst[i*dst_stride ]) = rnd_avg32(*((uint32_t*)&dst[i*dst_stride ]), rnd_avg32(a, b)); 
        a= (((*(&src1[i*src_stride1+4] + 3))<<24) | ((*(&src1[i*src_stride1+4] + 2))<<16) | ((*(&src1[i*src_stride1+4] + 1))<<8) | *(&src1[i*src_stride1+4] + 0)); 
        b= (((*(&src2[i*src_stride2+4] + 3))<<24) | ((*(&src2[i*src_stride2+4] + 2))<<16) | ((*(&src2[i*src_stride2+4] + 1))<<8) | *(&src2[i*src_stride2+4] + 0)); 
        *((uint32_t*)&dst[i*dst_stride+4]) = rnd_avg32(*((uint32_t*)&dst[i*dst_stride+4]), rnd_avg32(a, b)); 
    }
}

static void avg_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    int i; 
    for(i=0; i<h; i++)
    { 
        *((uint32_t*)(block )) = rnd_avg32(*((uint32_t*)(block )), (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)));
        *((uint32_t*)(block+4)) = rnd_avg32(*((uint32_t*)(block+4)), (((*(pixels+4 + 3))<<24) | ((*(pixels+4 + 2))<<16) | ((*(pixels+4 + 1))<<8) | *(pixels+4 + 0))); 
        pixels+=line_size; 
        block +=line_size; 
    }
}

static void avg_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);
}

static void avg_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);
}

static void avg_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    int j; 
    for(j=0; j<2; j++)
    { 
        int i; 
        const uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
        const uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
        uint32_t l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x02020202UL; 
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
        uint32_t l1,h1; 
        pixels+=line_size; 
        for(i=0; i<h; i+=2)
        { 
            uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l1= (a&0x03030303UL) + (b&0x03030303UL);
            h1= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = rnd_avg32(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL)); 
            pixels+=line_size; 
            block +=line_size; 
            a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x02020202UL; 
            h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); *((uint32_t*)block) = rnd_avg32(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL)); 
            pixels+=line_size; 
            block +=line_size; 
        } 
        pixels+=4-line_size*(h+1); 
        block +=4-line_size*h; 
    }
}

static void avg_no_rnd_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    int i; 
    for(i=0; i<h; i++)
    { 
        uint32_t a,b; 
        a= (((*(&src1[i*src_stride1 ] + 3))<<24) | ((*(&src1[i*src_stride1 ] + 2))<<16) | ((*(&src1[i*src_stride1 ] + 1))<<8) | *(&src1[i*src_stride1 ] + 0)); 
        b= (((*(&src2[i*src_stride2 ] + 3))<<24) | ((*(&src2[i*src_stride2 ] + 2))<<16) | ((*(&src2[i*src_stride2 ] + 1))<<8) | *(&src2[i*src_stride2 ] + 0)); 
        *((uint32_t*)&dst[i*dst_stride ]) = rnd_avg32(*((uint32_t*)&dst[i*dst_stride ]), no_rnd_avg32(a, b)); 
        a= (((*(&src1[i*src_stride1+4] + 3))<<24) | ((*(&src1[i*src_stride1+4] + 2))<<16) | ((*(&src1[i*src_stride1+4] + 1))<<8) | *(&src1[i*src_stride1+4] + 0)); 
        b= (((*(&src2[i*src_stride2+4] + 3))<<24) | ((*(&src2[i*src_stride2+4] + 2))<<16) | ((*(&src2[i*src_stride2+4] + 1))<<8) | *(&src2[i*src_stride2+4] + 0)); 
        *((uint32_t*)&dst[i*dst_stride+4]) = rnd_avg32(*((uint32_t*)&dst[i*dst_stride+4]), no_rnd_avg32(a, b));
    }
}

static void avg_no_rnd_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_simd(block, pixels, line_size, h);
}

static void avg_no_rnd_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_no_rnd_pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);
}

static void avg_no_rnd_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_no_rnd_pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);
}

static void avg_no_rnd_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    int j; 
    for(j=0; j<2; j++)
    { 
        int i; 
        const uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
        const uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
        uint32_t l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x01010101UL; 
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
        uint32_t l1,h1; 
        pixels+=line_size; 
        for(i=0; i<h; i+=2)
        { 
            uint32_t a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            uint32_t b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l1= (a&0x03030303UL) + (b&0x03030303UL); 
            h1= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = rnd_avg32(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL)); 
            pixels+=line_size; 
            block +=line_size; 
            a= (((*(pixels + 3))<<24) | ((*(pixels + 2))<<16) | ((*(pixels + 1))<<8) | *(pixels + 0)); 
            b= (((*(pixels+1 + 3))<<24) | ((*(pixels+1 + 2))<<16) | ((*(pixels+1 + 1))<<8) | *(pixels+1 + 0)); 
            l0= (a&0x03030303UL) + (b&0x03030303UL) + 0x01010101UL; 
            h0= ((a&0xFCFCFCFCUL)>>2) + ((b&0xFCFCFCFCUL)>>2); 
            *((uint32_t*)block) = rnd_avg32(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL)); 
            pixels+=line_size; 
            block +=line_size; 
        } 
        pixels+=4-line_size*(h+1); 
        block +=4-line_size*h; 
    }
}


//##########################################################################
// subpel16



static void put_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_simd(block , pixels , line_size, h); 
    put_pixels8_simd(block+8, pixels+8, line_size, h);
}

static void put_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_x2_simd(block , pixels , line_size, h); 
    put_pixels8_x2_simd(block+8, pixels+8, line_size, h);
}

static void put_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_y2_simd(block , pixels , line_size, h); 
    put_pixels8_y2_simd(block+8, pixels+8, line_size, h);
}

static void put_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_xy2_simd(block , pixels , line_size, h); 
    put_pixels8_xy2_simd(block+8, pixels+8, line_size, h);
}


static void avg_pixels16_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    avg_pixels8_l2(dst , src1 , src2 , dst_stride, src_stride1, src_stride2, h); 
    avg_pixels8_l2(dst+8, src1+8, src2+8, dst_stride, src_stride1, src_stride2, h);
}

static void avg_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_simd(block , pixels , line_size, h); 
    avg_pixels8_simd(block+8, pixels+8, line_size, h);
}

static void avg_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_x2_simd(block , pixels , line_size, h); 
    avg_pixels8_x2_simd(block+8, pixels+8, line_size, h);
}

static void avg_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_y2_simd(block , pixels , line_size, h); 
    avg_pixels8_y2_simd(block+8, pixels+8, line_size, h);
}

static void avg_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_xy2_simd(block , pixels , line_size, h); 
    avg_pixels8_xy2_simd(block+8, pixels+8, line_size, h);
}

static void put_no_rnd_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_pixels8_simd(block , pixels , line_size, h); 
    put_pixels8_simd(block+8, pixels+8, line_size, h);
}

static void put_no_rnd_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_no_rnd_pixels8_x2_simd(block , pixels , line_size, h); 
    put_no_rnd_pixels8_x2_simd(block+8, pixels+8, line_size, h);
}

static void put_no_rnd_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_no_rnd_pixels8_y2_simd(block , pixels , line_size, h); 
    put_no_rnd_pixels8_y2_simd(block+8, pixels+8, line_size, h);
}

static void put_no_rnd_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    put_no_rnd_pixels8_xy2_simd(block , pixels , line_size, h); 
    put_no_rnd_pixels8_xy2_simd(block+8, pixels+8, line_size, h);
}


static void avg_no_rnd_pixels16_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, int src_stride1, int src_stride2, int h)
{ 
    avg_no_rnd_pixels8_l2(dst , src1 , src2 , dst_stride, src_stride1, src_stride2, h); 
    avg_no_rnd_pixels8_l2(dst+8, src1+8, src2+8, dst_stride, src_stride1, src_stride2, h);
}

static void avg_no_rnd_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_pixels8_simd(block , pixels , line_size, h); 
    avg_pixels8_simd(block+8, pixels+8, line_size, h);
}

static void avg_no_rnd_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_no_rnd_pixels8_x2_simd(block , pixels , line_size, h); 
    avg_no_rnd_pixels8_x2_simd(block+8, pixels+8, line_size, h);
}

static void avg_no_rnd_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_no_rnd_pixels8_y2_simd(block , pixels , line_size, h); 
    avg_no_rnd_pixels8_y2_simd(block+8, pixels+8, line_size, h);
}

static void avg_no_rnd_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    avg_no_rnd_pixels8_xy2_simd(block , pixels , line_size, h); 
    avg_no_rnd_pixels8_xy2_simd(block+8, pixels+8, line_size, h);
}


// subpel16
//##########################################################################

#else

static void put_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels8x8_simd((unsigned int)block, (unsigned int)pixels);
    }
    else
    {
        put_pixels8x4_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void put_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
        put_pixels8x4_x2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        put_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void put_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 

    if(h==8)
    {
        put_pixels8x8_l2_simd((unsigned int)block, (unsigned int)pixels,
                            (unsigned int)pixels + SDM_BUF_STRIDE);
    }
    else
    {
        put_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
    }
}


static void put_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 

    if(h==8)
    {
        put_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
        put_pixels8x4_xy2_simd((unsigned int)block + 4*SDM_BUF_STRIDE, (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        put_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
    }
}


static void put_no_rnd_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels8x8_simd((unsigned int)block, (unsigned int)pixels);
    }
    else
    {
        put_pixels8x4_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void put_no_rnd_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_no_rnd_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
        put_no_rnd_pixels8x4_x2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        put_no_rnd_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void put_no_rnd_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 

    if(h==8)
    {
        put_no_rnd_pixels8x8_l2_simd((unsigned int)block, (unsigned int)pixels,
                            (unsigned int)pixels + SDM_BUF_STRIDE);
    }
    else
    {
        put_no_rnd_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
    }
}


static void put_no_rnd_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 

    if(h==8)
    {
        put_no_rnd_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
        put_no_rnd_pixels8x4_xy2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                        (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        put_no_rnd_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
    }
}


static void avg_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        avg_pixels8x8_simd((unsigned int)block, (unsigned int)pixels);
    }
    else
    {
        avg_pixels8x4_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        avg_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
        avg_pixels8x4_x2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
        avg_pixels8x4_y2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        avg_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
        avg_pixels8x4_xy2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_no_rnd_pixels8_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels8x8_simd((unsigned int)block, (unsigned int)pixels);
    }
    else
    {
        avg_pixels8x4_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_no_rnd_pixels8_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_no_rnd_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels8x4_x2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_no_rnd_pixels8x4_x2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_no_rnd_pixels8_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_no_rnd_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels8x4_y2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_no_rnd_pixels8x4_y2_simd((unsigned int)block, (unsigned int)pixels);
    }
}

static void avg_no_rnd_pixels8_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        avg_no_rnd_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels8x4_xy2_simd((unsigned int)block + 4*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 4*SDM_BUF_STRIDE);
    }
    else
    {
        avg_no_rnd_pixels8x4_xy2_simd((unsigned int)block, (unsigned int)pixels);
    }
}


//subpel-16
static void put_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
        put_pixels16x8_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
        put_pixels16x8_x2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
        put_pixels16x8_y2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{
    if(h==8)
    {
        put_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
        put_pixels16x8_xy2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_no_rnd_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
        put_pixels16x8_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_no_rnd_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_no_rnd_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_no_rnd_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
        put_no_rnd_pixels16x8_x2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_no_rnd_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_no_rnd_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_no_rnd_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
        put_no_rnd_pixels16x8_y2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                    (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void put_no_rnd_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        put_no_rnd_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        put_no_rnd_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
        put_no_rnd_pixels16x8_xy2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                    (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}



static void avg_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
        avg_pixels16x8_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_pixels16x8_x2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_pixels16x8_y2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_pixels16x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_pixels16x8_xy2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_no_rnd_pixels16_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_pixels16x8_simd( (unsigned int)block, (unsigned int)pixels);
        avg_pixels16x8_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_no_rnd_pixels16_x2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_no_rnd_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_no_rnd_pixels16x8_x2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels16x8_x2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_no_rnd_pixels16_y2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_no_rnd_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
    }
    else    //h==16
    {
        avg_no_rnd_pixels16x8_y2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels16x8_y2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                    (unsigned int)pixels + 8*SDM_BUF_STRIDE);
    }
}

static void avg_no_rnd_pixels16_xy2_simd(uint8_t *block, const uint8_t *pixels, int line_size, int h)
{ 
    if(h==8)
    {
        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block + 8, (unsigned int)pixels + 8);
    }
    else    //h==16
    {
        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block, (unsigned int)pixels);
        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block + 8, (unsigned int)pixels + 8);

        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE,
                                    (unsigned int)pixels + 8*SDM_BUF_STRIDE);
        avg_no_rnd_pixels8x8_xy2_simd( (unsigned int)block + 8*SDM_BUF_STRIDE + 8,
                                    (unsigned int)pixels + 8*SDM_BUF_STRIDE + 8);
    }
}



#endif

void dsputil_subpel_init(DSPContext* c)
{

    c->put_pixels_tab[1][0]     = put_pixels8_simd;
    c->put_pixels_tab[1][1]     = put_pixels8_x2_simd;
    c->put_pixels_tab[1][2]     = put_pixels8_y2_simd;
    c->put_pixels_tab[1][3]     = put_pixels8_xy2_simd;

    c->avg_pixels_tab[1][0]     = avg_pixels8_simd;
    c->avg_pixels_tab[1][1]     = avg_pixels8_x2_simd;
    c->avg_pixels_tab[1][2]     = avg_pixels8_y2_simd;
    c->avg_pixels_tab[1][3]     = avg_pixels8_xy2_simd;

    c->put_no_rnd_pixels_tab[1][0] = put_pixels8_simd;
    c->put_no_rnd_pixels_tab[1][1] = put_no_rnd_pixels8_x2_simd;
    c->put_no_rnd_pixels_tab[1][2] = put_no_rnd_pixels8_y2_simd;
    c->put_no_rnd_pixels_tab[1][3] = put_no_rnd_pixels8_xy2_simd;

    c->avg_no_rnd_pixels_tab[1][0] = avg_pixels8_simd;
    c->avg_no_rnd_pixels_tab[1][1] = avg_no_rnd_pixels8_x2_simd;
    c->avg_no_rnd_pixels_tab[1][2] = avg_no_rnd_pixels8_y2_simd;
    c->avg_no_rnd_pixels_tab[1][3] = avg_no_rnd_pixels8_xy2_simd;

    c->put_pixels_tab[0][0] = put_pixels16_simd;
    c->put_pixels_tab[0][1] = put_pixels16_x2_simd;
    c->put_pixels_tab[0][2] = put_pixels16_y2_simd;
    c->put_pixels_tab[0][3] = put_pixels16_xy2_simd;
    
    c->put_no_rnd_pixels_tab[0][0] = put_pixels16_simd;
    c->put_no_rnd_pixels_tab[0][1] = put_no_rnd_pixels16_x2_simd;
    c->put_no_rnd_pixels_tab[0][2] = put_no_rnd_pixels16_y2_simd;
    c->put_no_rnd_pixels_tab[0][3] = put_no_rnd_pixels16_xy2_simd;


    c->avg_pixels_tab[0][0] = avg_pixels16_simd;
    c->avg_pixels_tab[0][1] = avg_pixels16_x2_simd;
    c->avg_pixels_tab[0][2] = avg_pixels16_y2_simd;
    c->avg_pixels_tab[0][3] = avg_pixels16_xy2_simd;

    c->avg_no_rnd_pixels_tab[0][0] = avg_pixels16_simd;
    c->avg_no_rnd_pixels_tab[0][1] = avg_no_rnd_pixels16_x2_simd;
    c->avg_no_rnd_pixels_tab[0][2] = avg_no_rnd_pixels16_y2_simd;
    c->avg_no_rnd_pixels_tab[0][3] = avg_no_rnd_pixels16_xy2_simd;

}


