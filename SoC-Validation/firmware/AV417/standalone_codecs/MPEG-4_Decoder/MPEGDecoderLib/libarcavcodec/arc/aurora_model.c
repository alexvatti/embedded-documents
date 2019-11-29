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
 * aurora_model.c
 *
 * Nigel Topham, April 2005.
 */

#define _AURORA_MODEL_C_
//#include <stdio.h>
#include "aurora_asm.h"
#include "../common.h"

#undef printf

///////////////////////////////////////////////////////////////////////////////////////////
//
// Declare vector registers VR[0] to VR[23] as an array of 8 integers. In fact, only 16
// bits will ever be used. Every assignment to a VR is checked to see that the values
// assigned are always in range for signed 16-bit integers. This catches overflow.
//
// These are addressed as VR[vreg_num][vreg_element]
//
short VR[24][8];
int R[37];

///////////////////////////////////////////////////////////////////////////////////////////
//
// Declare vector registers range-tracking variables
//
// These are subscripted with [vreg_num][vreg_element] and store the maximum and minimum
// value (as a 32-bit range) that any computation has attempted to assign to that 
// vector element.
//
int vmin[24][8], vmax[24][8];

///////////////////////////////////////////////////////////////////////////////////////////
//
// Performance counters and performance counting functions
//
static unsigned clk;          // current time
static unsigned instructions; // number of instructions executed
static unsigned stalls;       // number of stall cycles
static unsigned vdef[24];     // time at which each VR is available

unsigned aurora_cycles () { return clk; }
unsigned aurora_stalls () { return stalls; }
unsigned aurora_instructions () { return instructions; }
void     DMA_ctor ();

void aurora_reset ()
{
    int i;
    clk = 0;
    instructions = 0;
    stalls = 0;
    for (i = 0; i < 8; i++) vdef[i] = 0;
    DMA_ctor ();
}

void aurora_stats ()
{
  /* printf ("Executed %d SIMD operations in %d cycles, with %d dependency stalls\n",
            instructions, clk, stalls);
   printf ("SIMD pipeline efficiency = %5.2f%%\n", (double)instructions*100.0/clk);*/
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// Various non-API functions which implement approximately cycle-accurate timing of
// SIMD instruction execution on Aurora.

void dyadic_op (int dst, int src1, int src2, int lat)
{
   int est;
   int ast;
   int delay;
   int t1 = vdef[src1];
   int t2 = vdef[src2];
   
   ++clk;
   est = MAX(t1, t2);
   ast = MAX(est, clk);
   delay = ast - clk;

#ifdef DEBUG_TIMING
   if (delay > 0) {
      /*printf ("** @clk %d: vdef[%d] = %d, vdef[%d] = %d, ast = %d, delay = %d\n", 
      clk, src1, vdef[src1], src2, vdef[src2], ast, delay);*/
   }
#endif

   stalls += delay;
   clk = clk + delay;
   vdef[dst] = clk + lat;
   ++instructions;
}

void dual_dest_op (int src1, int src2, int lat)
{
   int est;
   int ast;
   int delay;
   int t1 = vdef[src1];
   int t2 = vdef[src2];
   
   ++clk;
   est = MAX(t1, t2);
   ast = MAX(est, clk);
   delay = ast - clk;

#ifdef DEBUG_TIMING
   if (delay > 0) {
     // printf ("** @clk %d: vdef[%d] = %d, ast = %d, delay = %d\n", clk, src1, vdef[src1], ast, delay);
   }
#endif

   stalls += delay;
   clk = clk + delay;
   vdef[src1] = clk + lat;
   vdef[src2] = clk + lat;
   ++instructions;
}

void monadic_op (int dst, int src1, int lat)
{
   int est;
   int ast;
   int delay;
   
   ++clk;
   est = vdef[src1];
   ast = MAX(est, clk);
   delay = ast - clk;
  
#ifdef DEBUG_TIMING
   if (delay > 0) {
     // printf ("** @clk %d: vdef[%d] = %d, ast = %d, delay = %d\n", clk, src1, vdef[src1], ast, delay);
   }
#endif
   
   stalls += delay;
   clk = clk + delay;
   vdef[dst] = clk + lat;
   ++instructions;
}

void store_op (int src1)
{
   int est;
   int ast;
   int delay;
   
   ++clk;
   est = vdef[src1];
   ast = MAX(est, clk);
   delay = ast - clk;

#ifdef DEBUG_TIMING
   if (delay > 0) {
      //printf ("** @clk %d: vdef[%d] = %d, ast = %d, delay = %d\n", clk, src1, vdef[src1], ast, delay);
   }
#endif

   stalls += delay;
   clk = clk + delay;
   ++instructions;
}

void load_op (int dst)
{
   ++clk;
   vdef[dst] = clk + LAT_LOAD;
   ++instructions;
#ifdef DEBUG_TIMING
   debug_timing ();
#endif
}

void debug_timing ()
{
  // printf ("clk %3d\tinst %3d\n", clk, instructions);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Declare SDM
//
///////////////////////////////////////////////////////////////////////////////////////////

//#define SDM_SIZE	32768
#define SDM_MASK	0x0000ffffUL

//SIMD_word SDM_Memory[(SDM_SIZE/2)];
uint8_t SDM_Memory[SDM_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////
//
// DMA model
//
///////////////////////////////////////////////////////////////////////////////////////////

//--------------- Private DMA data --------------------------------------------------------

extern unsigned char * SDM_Bytes;      // DMA model's pointer to SDM base
int DMA_RegIn[8];               // DMA registers for inbound port
int DMA_RegOut[8];              // DMA registers for outbound port


//--------------- DMA functional API implementation ---------------------------------------

// Initialise SDM base pointer within the DMA model
void DMA_SetSDM (uint8_t *sdm)
{
	SDM_Bytes = (unsigned char *)sdm;
}

// Setup frame structure in SDM at a given offset
void DMA_SetFrame (int offset, unsigned char *baseAddr, int stride, int xDim, int yDim)
{
	DMA_Frame *frame = (DMA_Frame *)(SDM_Bytes+offset);
	frame->BaseAddr  = ((int)baseAddr) >> 8;
	frame->Stride    = stride;
	frame->X_Dim     = xDim;
	frame->Y_Dim     = yDim;
}

// DMA model constructor - must be called during model initialisation
void DMA_ctor ()
{
	int i;
	
	DMA_SetSDM (SDM_Memory);
	
	for (i = 0; i < 8; i++) {
		DMA_RegIn[i]  = 0;
		DMA_RegOut[i] = 0;
	}
}

typedef int bool;
static const int false = 0;
static const int true = 1;
	
// Perform DMA move from main memory to SDM
void DMA_RunIn()
{
    int base = DMA_RegIn[FT_BASE_ADR];

    int idx = (DMA_RegIn[DMA_BLK] >> 16) & 31;
    int w = DMA_RegIn[DMA_BLK] & 255;
    int h = (DMA_RegIn[DMA_BLK] >> 8) & 255;

    int sStride = DMA_RegIn[SDM_STRIDE];

    unsigned char * sAddr = SDM_Bytes+DMA_RegIn[SDM_DEST_ADR];

    DMA_Frame *ftb = (DMA_Frame *)(SDM_Bytes+base);

    unsigned char *fpix = (unsigned char *)(ftb[idx].BaseAddr);
    int fw = ftb[idx].X_Dim;
    int fh = ftb[idx].Y_Dim;
    int fStride = (ftb[idx].Stride);
    int fx = (DMA_RegIn[DMA_LOC]<<(32-13))>>(32-13);
    int fy = (DMA_RegIn[DMA_LOC]<<(32-29))>>((32-29)+(32-16));
    int x, y, ds = 0;

    if(DMA_RegIn[DMA_CFG] & 4)
    {
        ds = 1;
        fStride *= 2;
    }
    int cm = DMA_RegIn[DMA_CFG] & 3;

    for(y = fy; y < fy+h; y++)
    {
        for(x = fx; x < fx+w; x++)
        {
            bool clip = false;
            int py = y;
            if(py < 0)
            {
                if (ds) {
                    if (fy % 2) {
                        /* Odd line */
                        py = 1;
                    } else {
                        /* Even line */
                        py = 0;
                    }
                } else {
                    py = 0;
                }
                clip = true;
            }
            else if(py > ((fh-1)>>ds))
            {
                if (ds) {
                    if (fy % 2) {
                        /* Odd line */
                        py = ((fh-1)>>1)-1;
                    } else {
                        /* Even line */
                        py = (fh-1)>>1;
                    }
                } else {
                    py = fh-1;
                }
                clip = true;
            }
            int px = x;
            if(px < 0)
            {
                px = 0;
                clip = true;
            }
            else if(px > fw-1)
            {
                px = fw-1;
                clip = true;
            }

            if(clip)
            {
                switch(cm)
                {
                case 0:
                    *sAddr = DMA_RegIn[DMA_CFG] >> 8;
                    break;
                case 1:
                    // assert("Reserved clip mode" == 0); // report an error here
                    break;
                case 2:
                    *sAddr = fpix[px + py*fStride];
                    break;
                case 3:
                    // assert("Interlaced clip Not implemented" == 0); // report error
                    break;
                }
            }
            else
            {
                *sAddr = fpix[px + py*fStride];
            }
            sAddr++;
        }
        sAddr += sStride-w;
    }
}

// Perform DMA move from SDM to main memory
void DMA_RunOut()
{
    int base = DMA_RegOut[FT_BASE_ADR];

    int idx = (DMA_RegOut[DMA_BLK] >> 16) & 31;
    int w = DMA_RegOut[DMA_BLK] & 255;
    int h = (DMA_RegOut[DMA_BLK] >> 8) & 255;

    int sStride = DMA_RegOut[SDM_STRIDE];

    unsigned char * sAddr = SDM_Bytes+DMA_RegOut[SDM_DEST_ADR];

    DMA_Frame *ftb = (DMA_Frame *)(SDM_Bytes+base);

    unsigned char *fpix = (unsigned char *)(ftb[idx].BaseAddr);
    int fw = ftb[idx].X_Dim;
    int fh = ftb[idx].Y_Dim;
    int fStride = (ftb[idx].Stride);
    int fx = DMA_RegOut[DMA_LOC] & 4095;
    int fy = (DMA_RegOut[DMA_LOC] >> 16) & 4095;
    int x, y;

    if(DMA_RegOut[DMA_CFG] & 4)
    {
        fStride *= 2;
    }
    //int cm = DMA_RegOut[DMA_CFG] & 3;

    for(y = fy; y < fy+h; y++)
    {
        for(x = fx; x < fx+w; x++)
        {
            bool clip = false;
            if(y < 0)
            {
                clip = true;
            }
            else if(y > fh-1)
            {
                clip = true;
            }
            if(x < 0)
            {
                clip = true;
            }
            else if(x > fw-1)
            {
                clip = true;
            }

            if(clip)
            {
                // assert("Clipping on DMA out?" == 0); // report error
            }
            else
            {
                fpix[x + y*fStride] = *sAddr;
            }
            sAddr++;
        }
        sAddr += sStride-w;
    }
}
