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
#ifndef _AURORA_DMA_H_
#define _AURORA_DMA_H_

#undef OLD_DMA_CODE
#define DMA_FT_WIDTH 16
/* Where we store the DMA table in the SDM. */
#define DMA_TABLE_POINTER 0x0 /* to 0x0FF*/

#define DMA_TABLE_LENGTH (32*16)

struct DMA_Frame_struct
{
    unsigned int BaseAddr;  // 30 bits in hardware
    unsigned int Stride;    // 14 bits in hardware
    unsigned int X_Dim;     // 12 bits in hardware
    unsigned int Y_Dim;     // 12 bits in hardware
};

typedef struct DMA_Frame_struct DMA_Frame;

/******************** DMA FUNCTIONALITY ****************************/
/* This macro stores a single entry in the dma table.
 * WARNING: This macro assumes (and will break if these aren't met):
 *            - 0 <= INDEX < 32
 *            - (DATA % 4) == 0
 *            - (STRIDE % 4) == 0
 *            - 0 <= STRIDE < 2^16
 *            - (XDIM % 4) == 0
 *            - 0 < XDIM < 2^12
 *            - (YDIM % 4) == 0
 *            - 0 < YDIM < 2^12
 */
#define SET_DMA_FRAME(INDEX,DATA,STRIDE,XDIM,YDIM) {                    \
    DMA_Frame *dma_frame =                                              \
        (DMA_Frame *)(SDM_Bytes+DMA_TABLE_POINTER+(16*(INDEX)));        \
    dma_frame->BaseAddr = ((unsigned int)(DATA));                       \
    dma_frame->Stride = ((unsigned int)(STRIDE));                       \
    dma_frame->X_Dim = (unsigned int)((XDIM) - 1);                      \
    dma_frame->Y_Dim = (unsigned int)((YDIM) - 1);                      \
}

/* One or both of these should be run once a set of new dma frames have been
 * set to ensure that the new values are read into the DMA channels.
 * WARNING: Clobbers values in each channels dr2 registers. */
#define RELOAD_DMA_IN_TABLE {     \
    VDIWRi(DI2,0x0);              \
}

#define RELOAD_DMA_OUT_TABLE {     \
    VDOWRi(DO2,0x0);               \
}


//MISC DEFINES
#define DO0 "0"
#define DO1 "1"
#define DO2 "2"
#define DO3 "3"
#define DO4 "4"
#define DO5 "5"
#define DO6 "6"
#define DO7 "7"
#define DI0 "0"
#define DI1 "1"
#define DI2 "2"
#define DI3 "3"
#define DI4 "4"
#define DI5 "5"
#define DI6 "6"
#define DI7 "7"

#define VDIWRi(a,b)     _ASM("    vdiwr   %dr" a "," #b)
#define VDOWRi(a,b)     _ASM("    vdowr   %dr" a "," #b)
#define VDIWRr(a,b)     _ASM("    vdiwr   %dr" a "," b)
#define VDOWRr(a,b)     _ASM("    vdowr   %dr" a "," b)


_Asm AsmSetDMAFrame(int offset, unsigned int baseAddr, unsigned int stride, unsigned int xDim, unsigned int yDim)
{
    %reg offset, baseAddr, stride, xDim, yDim
    
    vmovw       %vr00,offset,0x10       // i4

    vmovw       %vr01, baseAddr,0x1
    lsr         baseAddr, baseAddr,16
    vmovw       %vr01, baseAddr,0x2
    
    vmovw       %vr01, stride,0x4
    lsr         stride, stride,16
    vmovw       %vr01, stride,0x8

    vmovw       %vr01, xDim,0x10
    lsr         xDim, xDim,16
    vmovw       %vr01, xDim,0x20

    vmovw       %vr01, yDim,0x40
    lsr         yDim, yDim,16
    vmovw       %vr01, yDim,0x80

    vst128       %vr01,[%i4,0]
}

/*
void SetDMAFrame(int offset, unsigned int baseAddr, unsigned int stride, unsigned int xDim, unsigned int yDim)
{
    AsmSetDMAFrame(offset, baseAddr, stride, xDim-1, yDim-1);
}
*/

#endif  //#define _AURORA_DMA_H_
