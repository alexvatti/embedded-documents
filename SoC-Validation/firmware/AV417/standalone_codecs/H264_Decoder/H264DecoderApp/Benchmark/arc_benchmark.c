/*
 * Copyright (c) 2006 ARC International (UK) Ltd
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
 *
 *
 *
 *
 * benchmark : ticks/bandwidth extraction utilities
 *
 *
 *
 */
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code("codesec5")
#endif
#ifdef __BENCHMARK__

#include <stdlib.h>
#include <stdio.h>
#include "arc_benchmark.h"

//#define USE_PROCESSOR_TIMER 1


extern volatile bpr_reg_struct *bprP;
extern unsigned int benchmark_dat[25];
extern FILE *benchmark_file;
static int tot_val = 0;



/* ------------------------------------------------------------------------ */
/* functions                                                                */
/* ------------------------------------------------------------------------ */
void setup_benchmark()
{
  /* setup BPR */
  bprP->BPR_SETUP1 |= 0x00070000; // assign sub node 3 to DMA ASSIST 

  /* print resolution header at the beginning of the benchmark file */
  fprintf(benchmark_file,"resolution = 0x0      \n");
  fprintf(benchmark_file,"TotalCycles\t IdleCycles\t I$\t D$\t DMAI\t DMA0\t DMAAssist\t Total\t");
  fprintf(benchmark_file,"\n");

}



void stop_BPR()
{
  bprP->BPR_CTRL |= 0x1; // set HALT bit 
}



void run_BPR()
{
  bprP->BPR_CTRL &= 0xfffffffe; // reset HALT bit

  #if defined USE_PROCESSOR_TIMER
    // use ARC700 timer
    _sr(2,         0x22);       // CONTROL0 = 2 -- NH
    _sr(0xffffffff,0x23);       // LIMIT0 = -1
    _sr(0,         0x21);       // COUNT0 = 0
  #endif
}



void reset_BPR_Counters()
{
  bprP->BPR_CTRL |= 0x2; /// set CLEAR_CNT bit 

#if defined USE_PROCESSOR_TIMER
  // use ARC700 timer
  _sr(2,         0x22);       // CONTROL0 = 2 -- NH
  _sr(0xffffffff,0x23);       // LIMIT0 = -1
  _sr(0,         0x21);       // COUNT0 = 0
#endif
}



/* 
   STORE : 
   - TOTAL CYCLES
   - TOTAL IDLE CYCLES
   - I$ VALID
   - D$ VALID
   - DMAI VALID
   - DMAO VALID
   - DMA ASSIST VALID

   BPR initiators :
      1 = I$ 
      2 = D$
      5 = DMAI
      4 = DMAO 
      3 = DMA ASSIST 
*/
void capture_BPR_benchmark_data()
{
  short initiator_index[5] = {1,2,5,4,3},i,j;
  unsigned int *benchmarkP = benchmark_dat;
  tot_val = 0;


  /* total nb of cycles per frame */
#if defined USE_PROCESSOR_TIMER
  // use ARC700 timer
  *(benchmarkP++) = _lr(0x21);
#else
  // use BPR counters
  *(benchmarkP++) = bprP->BPR_CYCCNT;
#endif

  /* global : VALID, SUMLAT, IDLE per frame */
  bprP->BPR_INDEX = 0;
  //*(benchmarkP++) = bprP->BPR_VACYCCNT;
  //*(benchmarkP++) = bprP->BPR_SUMLAT;
  *(benchmarkP++) = bprP->BPR_IDLCYCCNT;

  /* VALID CYCLES for I$, D$, DMAI, DMAO, DMA ASSIST */
  for (i = 0 ; i < 2 ; i++) {
    bprP->BPR_INDEX = initiator_index[i];
    tot_val += bprP->BPR_VACYCCNT;
    *(benchmarkP++) = bprP->BPR_VACYCCNT;
  } /* for */
  for (i = 2 ; i < 5 ; i++) {
    bprP->BPR_INDEX = initiator_index[i];
    tot_val += bprP->BPR_VACYCCNT;
    *(benchmarkP++) = bprP->BPR_VACYCCNT;
  } /* for */

  *(benchmarkP++) = tot_val;

  /* SUM LATENCIES CYCLES for I$, D$, DMAI, DMAO, DMA ASSIST */
#if 0
  for (i = 0 ; i < 5 ; i++) {
    bprP->BPR_INDEX = initiator_index[i];
    *(benchmarkP++) = bprP->BPR_SUMLAT;
  } /* for */

  /* IDLE CYCLES for I$, D$, DMAI, DMAO, DMA ASSIST */
  for (i = 0 ; i < 5 ; i++) {
    bprP->BPR_INDEX = initiator_index[i];
    *(benchmarkP++) = bprP->BPR_IDLCYCCNT;
  } /* for */
#endif


  /* print the data to file according to the format of the header defined in
     the function below print_benchmark_header() */
  benchmarkP = benchmark_dat;
  for (j = 0 ; j < 8 ; j++) {
    fprintf(benchmark_file,"%u\t",*(benchmarkP++));
  } /* for */
  fprintf(benchmark_file,"\n");

#if 0
  for (j = 0 ; j < 4 ; j++) { 
    fprintf(benchmark_file,"%u\t",*(benchmarkP++));
  } /* for */
  fprintf(benchmark_file,"\t");

  for (j = 0 ; j < 3 ; j++) { 
    for (i = 0 ; i < 5 ; i++) {
      fprintf(benchmark_file,"%u\t",*(benchmarkP++));
    } /* for */
    fprintf(benchmark_file,"\t");
  } /* for */
#endif

} /* capture_BPR_benchmark_data */



/* print stream resolution to benchmark file */
void print_resolution(int width,int height)
{

  /* go back to the beginning of the file, where resolution must be printed */
  rewind(benchmark_file);
  fprintf(benchmark_file,"resolution = %dx%d\n",width,height);

} /* print_resolution */



/* print the flag "frame is skipped" to file */
void print_mpeg_skipped_frame_flag()
{
  fprintf(benchmark_file,"frame is skipped\n");
} /*  */
#endif
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I16K_D8K) || defined(I8K_D8K) 
#pragma Code()
#endif