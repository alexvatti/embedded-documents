//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcFDCT_MPA.sss'

// CONFIDENTIAL AND PROPRIETARY INFORMATION                        //
// Copyright 2007 ARC International (Unpublished)                  //
// All Rights Reserved.                                            //
//                                                                 //
// This document, material and/or software contains confidential   //
// and proprietary information of ARC International and is         //
// protected by copyright, trade secret and other state, federal,  //
// and international laws, and may be embodied in patents issued   //
// or pending.  Its receipt or possession does not convey any      //
// rights to use, reproduce, disclose its contents, or to          //
// manufacture, or sell anything it may describe.  Reverse         //
// engineering is prohibited, and reproduction, disclosure or use  //
// without specific written authorization of ARC International is  //
// strictly forbidden.  ARC and the ARC logotype are trademarks of //
// ARC International.                                              //


// Stall info ON                                                      // [   18] 
// Stall info format: Stall [Reg in Stall] ...... List of Registers currently busy  

//	vec16	Scalars0 = vr00, Scalars1 = vr01

// MetaWare ouput ON                                                  // [   21] 
#define LINE #line                                                    // [   21] 
    

// ScanMP: Including file .\ARCEncoder\MPEG4\..\ARC\SIMD_ABI.ii.
// (c) Arc international 2007
// Creo vrapter ABI using MPC data structures
// To co-operate with ME & EE hardware

// Media processor ABI ---------------------------
//	vec16				Scalars0 = vr00
//	vec16				Scalars1 = vr01
//	**	i0 - i3			Scalar params/scratch
// p16    i4  CirBuf                                                  // [    9] 
// p16    i5  FixBuf                                                  // [   10] 
// p16    i6  CirLeft                                                 // [   11] 
//	**	i7				scratch
// 	**	i8 - i15 		Work - should be preserved
//	**	vr01 - vr04		params
//	**	vr05 - vr30		scratch
//vec16  vr62  VZERO                                                  // [   16] 
//vec16  vr31  VSTACK                                                 // [   17] 
//vec16  vr63  VFLAGS                                                 // [   18] 

// Media processor ABI end -----------------------


// -----------------------------------------------
// CirBuf, FixBuf , CirLeft are set by  "SetCurrentMPC" in ArcSDMTables_MPA.sss
// -----------------------------------------------


// ScanMP: End of included file ..\ARC\SIMD_ABI.ii.

	
	
#include ".\ARCEncoder\MPEG4\ArcFDCT_MPA.h"


// Macro table
    int Fdct[8];
    int Fdct_Record(int sa, int SIMDnum);
	
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"    
    
    
    
// [Macro] FDCT                                                       // [   37] 
_Asm int Record_FDCT(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  mov r2,2    // 2 parse record
FDCT_RecLoop:
  ld r1,[r17 ,0]
  vrec r1


    // Params
// p16    i3  standard                                                // [   40] ....................// i3 0 = MPEG4; 0xffff = H263
    // end Params
// Begin                                                              // [   42] 
// p16    i0  blkOutOffset                                            // [   43] ....................// i1
// p16    i2  rounding                                                // [   44] ....................// i2 rounding or not
// p16    i7  block_no                                                // [   45] ....................// i7

// p16    i8  Intra                                                   // [   47] 
//vec16  vr02  v1                                                     // [   48] 
//vec16  vr03  v2                                                     // [   48] 
//vec16  vr04  v3                                                     // [   48] 
//vec16  vr05  v4                                                     // [   48] 
//vec16  vr06  v5                                                     // [   48] 
//vec16  vr07  v6                                                     // [   48] 
//vec16  vr08  v7                                                     // [   48] 
//vec16  vr09  v8                                                     // [   48] 
//vec16  vr10  v9                                                     // [   48] 
//vec16  vr11  scale1                                                 // [   49] 
//vec16  vr12  scale2                                                 // [   49] 
//vec16  vr13  scale3                                                 // [   49] 
//vec16  vr14  scale4                                                 // [   49] 
//vec16  vr15  scale5                                                 // [   49] 
//vec16  vr16  scale6                                                 // [   49] 
//vec16  vr17  scale7                                                 // [   49] 
//vec16  vr18  scale8                                                 // [   49] 
//vec16  vr19  MaxCoeff                                               // [   50] 

//vec16  vr20  auxreg                                                 // [   52] 
// p16    i9  temp                                                    // [   53] 
// p16    i10  cirPtr                                                 // [   54] 

// p16    i12  ptr                                                    // [   56] 
// s16    vr21.4  blockdouble                                         // [   57] 
//vec16  vr22  normround                                              // [   58] 
//vec16  vr23  vecadd                                                 // [   59] ....................// needs to be setup with addresses of labels
// p16    i13  QuantVal                                               // [   60] 
// p16    i14  dcpredict                                              // [   61] 

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.|      | blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|      |      |      |      |      | bloc.|      |      |
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

// set up case statement addresses at the end for dc prediction
LINE 65".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr23, r3, 1                     // vecadd case0 [   65] 
       VMOVW             vr23, r4, 2                     // vecadd case1 [   66] 
       VMOVW             vr23, r5, 4                     // vecadd case2 [   67] 
       VMOVW             vr23, r6, 8                     // vecadd case3 [   68] 
       VMOVW             vr23, r7, 16                    // vecadd case4 [   69] 
       VMOVW             vr23, r8, 32                    // vecadd case5 [   70] 
       VST16             vr62,[i4, MPO_NonZeroCoeffLuma] // VZERO CirBuf [   71] ....................// clear CBP
    // set saturation limits
       VMOVW             vr19, 2047, 255                     // MaxCoeff [   73] ....................// MPEG4 limits
       VMOVW.i3          vr19, 127                  // standard MaxCoeff [   74] ....................// H263 limits 
    // set Intra flag
       VLD8W             vr01,[i4, MPO_MBCodingType]     // Intra CirBuf [   76] 
              //  Stall: [vr01]  ...............................      vr01.001
              //  Stall: [vr01]  ...............................      vr01.001
              //  Stall: [vr01]  ...............................      vr01.001
       VAND.1            vr01, vr01, MBCT_MPEG4_INTRA     // Intra Intra [   77] 
// Begin                                                              // [   78] 
 
// p16    i1  dummy                                                   // [   80] 
              //  Stall: [vr01]  ...............................      vr01.001
        VMR7W.2         vr00, vr01, vr01                  // dummy Intra [   81] 
    
LINE 83".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr00]  ...............................      vr00.002
              //  Stall: [vr00]  ...............................      vr00.002
       VSNEW.2           vr00, vr62                       // dummy VZERO [   83] 
              //  Stall: [VFLAGS]   ............................      vr63.002
       VMOVW.2.S         vr00, -128                             // dummy [   84] ....................// offset for Intra scaling tables

              //  Stall: [i1]  .................................      vr00.002      vr63.002
       VIM               i12, i1, SDMOF_mpeg4_dct_quant_scaling_table+128 // ptr dummy [   86] 
     // load quantization coefficients  - good for all blocks
              //  Stall: [i12]  ................................      I12  
       VLD128            vr11,[i12, 0x00]                  // scale1 ptr [   88] 
       VLD128            vr12,[i12, 0x10]                  // scale2 ptr [   89] 
       VLD128            vr13,[i12, 0x20]                  // scale3 ptr [   90] 
       VLD128            vr14,[i12, 0x30]                  // scale4 ptr [   91] 
       VLD128            vr15,[i12, 0x40]                  // scale5 ptr [   92] 
       VLD128            vr16,[i12, 0x50]                  // scale6 ptr [   93] 
       VLD128            vr17,[i12, 0x60]                  // scale7 ptr [   94] 
       VLD128            vr18,[i12, 0x70]                  // scale8 ptr [   95] 
// End                                                                // [   96] 
    
    
// Begin                                                              // [   99] 
  // this section sets rounding for the whole MB depending on whether
  // mpeg4 : Intra = rounding true; Inter = rounding false
  // h263  : Even quant = rounding true, Odd quant = rouding false
  
// s16    vr21.32  alg                                                // [  104] ....................// algorithm type (mpeg4 or H263)
// s16    vr24.32  even                                               // [  105] ....................// whether quantval is even
// s16    vr25.32  round                                              // [  106] ....................// whether rounding should be done
// s16    vr26.32  temp                                               // [  107] 

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.|      | blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|      |      |   alg|      |      | bloc.|      |      |
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|VR24|      |      |  even|      |      |      |      |      |
//|VR25|      |      | round|      |      |      |      |      |
//|VR26|      |      |  temp|      |      |      |      |      |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

LINE 110".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VJP!i8            r9                            // Intra endround [  110] 
       VLD8W_5           vr01,[i4, MPO_QuantValue]  //DS QuantVal CirBuf [  111] ....................// needed later
       VMOVW             vr25, 0, 32  //DS________________________ round [  112] 
       VMOVW             vr01, 0, 64  //DS____________________ dcpredict [  113] 

       VAND.32           vr24, vr01, 0x1                // even QuantVal [  115] ....................// check to see if even
       VSEQW.32          vr24, 0                                 // even [  116] ....................// sticky set if quant is even
        VMR6W.32        vr21, vr00, vr00                 // alg standard [  117] 
LINE 118".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr21]  ...............................      vr21.032
              //  Stall: [vr21]  ...............................      vr21.032
       VSNEW.32.S        vr21, 0                                  // alg [  118] ....................// sticky set if quant even and H263 and Intra
              //  Stall: [VFLAGS]   ............................      vr63.032
       VMOVW.32.S        vr25, 1                                // round [  119] ....................// set round if sticky
    
        VMR3W.32        vr24, vr01, vr01                   // even Intra [  121] 
LINE 122".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VSEQW.32          vr24, 1                                 // even [  122] ....................// set sticky if Intra
       VSEQW.32.S        vr21, 0                                  // alg [  123] ....................// set sticky if Intra and Mpeg4
       VMOVW.32.S        vr25, 1                                // round [  124] ....................// set round if sticky
       VMOVW             vr26, 0, 32                             // temp [  125] 
       VMOVW.32.S        vr26, 1                                 // temp [  126] 
              //  Stall: [vr26]  ...............................      vr26.032
        VMR7W.64        vr01, vr26, vr26               // dcpredict temp [  127] ....................// we also need to do dc predict in this case
//endround --------                                                   // [  128] 
 vendrec r9                                                           // [  128] 
 vrec r9                                                              // [  128] 
        VMR3W.4         vr00, vr25, vr25               // rounding round [  129] 
// End                                                                // [  130] 


// if inter, we need to set RoundingValue here to zero, for Intra, it's 1<<14	
LINE 134".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr22, 16384, 1                     // normround [  134] 

// set rounding - needed for mults in transform, and alg specific rounding
// for quant step 
       VMOVW             vr01, 0, 16                              // ptr [  138] 
       VMOVW             vr20, 0, 1                            // auxreg [  139] 
              //  Stall: [vr20]  ...............................      vr20.001
       VADDW.i2          vr20, vr20, vr22 // rounding auxreg auxreg normround [  140] 
  
       VMOVW             vr00, 0, 128                        // block_no [  142] 

  // main block loop starts here
//MainLoop --------                                                   // [  145] 
 vendrec r10                                                          // [  145] 
 vrec r10                                                             // [  145] 
LINE 146".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VJP!i8            r11                          // Intra loadInter [  146] 
        VMR5W.4         vr21, vr00, vr00  //DS_____ blockdouble block_no [  147] 
LINE 148".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr21]  ...............................      vr21.004
              //  Stall: [vr21]  ...............................      vr21.004
       VADDW.4           vr21, vr21, vr21 //DS blockdouble blockdouble blockdouble [  148] ....................// needed for indexing short arrays
    // work out the offset of the data for this block
       VMOVW             vr01, 0, 2  //DS__________________________ temp [  150] 
       VIM               i10, i9, SDMOF_mpeg4_raw_pixel_offset // cirPtr temp [  151] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  152] 
              //  Stall: [i10]  ................................      vr01.004
       VLD16             vr00,[i10, 0]            // blkOutOffset cirPtr [  153] 
    // set the location of the actual pixels
       VIM               i10, i4, MPO_PixelCoeffBuffer  // cirPtr CirBuf [  155] 
              //  Stall: [vr01]  ...............................      vr00.001      I10  
              //  Stall: [vr01]  ...............................      vr00.001
       VADDW.4           vr01, vr01, i0    // cirPtr cirPtr blkOutOffset [  156] 

// first load data into the registers
              //  Stall: [i10]  ................................      vr01.004
       VLD64W            vr02,[i10, 0]                      // v1 cirPtr [  159] ....................// load 8 bytes from input
       VLD64W            vr03,[i10, PCB_CURRENT_STRIDE]     // v2 cirPtr [  160] ....................// load next 8 bytes from input
       VLD64W            vr04,[i10, PCB_CURRENT_STRIDE*2]   // v3 cirPtr [  161] ....................// load next 8 bytes from input
       VLD64W            vr05,[i10, PCB_CURRENT_STRIDE*3]   // v4 cirPtr [  162] ....................// load next 8 bytes from input
       VLD64W            vr06,[i10, PCB_CURRENT_STRIDE*4]   // v5 cirPtr [  163] ....................// load next 8 bytes from input
       VLD64W            vr07,[i10, PCB_CURRENT_STRIDE*5]   // v6 cirPtr [  164] ....................// load next 8 bytes from input
       VLD64W            vr08,[i10, PCB_CURRENT_STRIDE*6]   // v7 cirPtr [  165] ....................// load next 8 bytes from input
       VLD64W            vr09,[i10, PCB_CURRENT_STRIDE*7]   // v8 cirPtr [  166] ....................// load next 8 bytes from input
   
// now set up output pointer
       VMOVW             vr01, 0, 2                              // temp [  169] 

       VJP               r12                                // transform [  171] 
       VIM               i10, i9, SDMOF_mpeg4_diff_pixel_offset //DS cirPtr temp [  172] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21  //DS cirPtr cirPtr blockdouble [  173] 
              //  Stall: [i10]  ................................      vr01.004
       VLD16             vr00,[i10, 0]  //DS________ blkOutOffset cirPtr [  174] 
//loadInter --------                                                  // [  175] 
 vendrec r11                                                          // [  175] 
 vrec r11                                                             // [  175] 

    // work out the offset of the data for this block
LINE 178".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr01, 0, 2                              // temp [  178] 
              //  Stall: [i9]  .................................      vr01.002
       VIM               i10, i9, SDMOF_mpeg4_diff_pixel_offset // cirPtr temp [  179] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  180] 
              //  Stall: [i10]  ................................      vr01.004
       VLD16             vr00,[i10, 0]            // blkOutOffset cirPtr [  181] 

       VIM               i10, i4, MPO_PixelCoeffBuffer  // cirPtr CirBuf [  183] 
              //  Stall: [vr01]  ...............................      vr00.001      I10  
              //  Stall: [vr01]  ...............................      vr00.001
       VADDW.4           vr01, vr01, i0    // cirPtr cirPtr blkOutOffset [  184] 
 
              //  Stall: [i10]  ................................      vr01.004
       VLD128            vr02,[i10, 0]                      // v1 cirPtr [  186] ....................// load 8 shorts from input
       VLD128            vr03,[i10, PCB_RESIDUAL_STRIDE]    // v2 cirPtr [  187] ....................// load next 8 shorts from input
       VLD128            vr04,[i10, PCB_RESIDUAL_STRIDE*2]  // v3 cirPtr [  188] ....................// load next 8 shorts from input
       VLD128            vr05,[i10, PCB_RESIDUAL_STRIDE*3]  // v4 cirPtr [  189] ....................// load next 8 shorts from input
       VLD128            vr06,[i10, PCB_RESIDUAL_STRIDE*4]  // v5 cirPtr [  190] ....................// load next 8 shorts from input
       VLD128            vr07,[i10, PCB_RESIDUAL_STRIDE*5]  // v6 cirPtr [  191] ....................// load next 8 shorts from input
       VLD128            vr08,[i10, PCB_RESIDUAL_STRIDE*6]  // v7 cirPtr [  192] ....................// load next 8 shorts from input
       VLD128            vr09,[i10, PCB_RESIDUAL_STRIDE*7]  // v8 cirPtr [  193] ....................// load next 8 shorts from input

   // move16      blkOutOffset, blkInOffset
//transform --------                                                  // [  196] 
 vendrec r12                                                          // [  196] 
 vrec r12                                                             // [  196] 
// Begin                                                              // [  197] 
// this section performs the AAN forward DCT with scaling
//vec16  vr24  v10                                                    // [  199] ....................// working vecs
//vec16  vr25  v11                                                    // [  199] 
//vec16  vr26  v12                                                    // [  199] 
//vec16  vr27  v13                                                    // [  199] 
//vec16  vr28  v14                                                    // [  199] 
//vec16  vr29  v15                                                    // [  199] 
//vec16  vr30  v24                                                    // [  199] 

// s16    vr21.1  alg                                                 // [  201] 
// p16    i1  DCterm                                                  // [  202] 
// s16    vr21.32  Quant2                                             // [  203] 

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.|DCterm| blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|      |      |Quant2|      |      | bloc.|      |   alg|
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|VR24|                         v10                           |
//|VR25|                         v11                           |
//|VR26|                         v12                           |
//|VR27|                         v13                           |
//|VR28|                         v14                           |
//|VR29|                         v15                           |
//|VR30|                         v24                           |
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

LINE 206".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr01, 0, 16                              // ptr [  206] 
              //  Stall: [i12]  ................................      vr01.016      vr08          vr09    
       VRPUT             vr22,[i12, 5]                  // normround ptr [  207] ....................// set normal rounding
        VMR3W.1         vr21, vr00, vr00                 // alg standard [  208] 
// now transpose the matrix
LINE 210".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VEXCH1            vr02, vr03                             // v1 v2 [  210] ....................// exchange lanes 7,5,3,1 with 6,4,2,0
       VEXCH1            vr04, vr05                             // v3 v4 [  211] ....................// exchange lanes 7,5,3,1 with 6,4,2,0
       VEXCH1            vr06, vr07                             // v5 v6 [  212] ....................// exchange lanes 7,5,3,1 with 6,4,2,0
       VEXCH1            vr08, vr09                             // v7 v8 [  213] ....................// exchange lanes 7,5,3,1 with 6,4,2,0
       VEXCH2            vr02, vr04                             // v1 v3 [  214] ....................// exchange lanes 7,6,3,2 with 5,4,1,0
       VEXCH2            vr03, vr05                             // v2 v4 [  215] ....................// exchange lanes 7,6,3,2 with 5,4,1,0
       VEXCH2            vr06, vr08                             // v5 v7 [  216] ....................// exchange lanes 7,6,3,2 with 5,4,1,0	
       VEXCH2            vr07, vr09                             // v6 v8 [  217] ....................// exchange lanes 7,6,3,2 with 5,4,1,0
              //  Stall: [vr06]  ...............................      vr06          vr07          vr08          vr09    
       VEXCH4            vr02, vr06                             // v1 v5 [  218] ....................// exchange lanes 7,6,5,4 with 3,2,1,0
       VEXCH4            vr03, vr07                             // v2 v6 [  219] ....................// exchange lanes 7,6,5,4 with 3,2,1,0
       VEXCH4            vr04, vr08                             // v3 v7 [  220] ....................// exchange lanes 7,6,5,4 with 3,2,1,0
       VEXCH4            vr05, vr09                             // v4 v8 [  221] ....................// exchange lanes 7,6,5,4 with 3,2,1,0
   	
  // Perform a 1-D FDCT transform on eight rows simultaneously  	


              //  Stall: [vr08]  ...............................      vr04          vr05          vr08          vr09    
       VADDSUW           vr03, vr08                             // v2 v7 [  226] 
       VADDSUW           vr02, vr09                             // v1 v8 [  227] 
       VADDSUW           vr04, vr07                             // v3 v6 [  228] 
       VADDSUW           vr05, vr06                             // v4 v5 [  229] 
       VADDW             vr10, vr08, vr09                    // v9 v7 v8 [  230] 
       VADDW             vr08, vr07, vr08                    // v7 v6 v7 [  231] 
       VADDSUW           vr03, vr04                             // v2 v3 [  232] 
       VADDSUW           vr02, vr05                             // v1 v4 [  233] 
       VBMULFW           vr08, vr08, 23170                      // v7 v7 [  234] 
       VADDW             vr07, vr06, vr07                    // v6 v5 v6 [  235] 
       VADDW             vr04, vr04, vr05                    // v3 v3 v4 [  236] 
       VSUBW             vr06, vr07, vr10                    // v5 v6 v9 [  237] 
       VADDW             vr10, vr10, vr10                    // v9 v9 v9 [  238] 
       VADDSUW           vr09, vr08                             // v8 v7 [  239] 
       VBMULFW           vr04, vr04, 23170                      // v3 v3 [  240] 
       VBMULFW           vr06, vr06, 12540                      // v5 v5 [  241] 
       VBMULFW           vr07, vr07, 17734                      // v6 v6 [  242] 
       VBMULFW           vr10, vr10, 21407                      // v9 v9 [  243] 
       VADDSUW           vr05, vr04                             // v4 v3 [  244] 
       VADDW             vr07, vr07, vr06                    // v6 v6 v5 [  245] 
       VADDW             vr10, vr10, vr06                    // v9 v9 v5 [  246] 
       VADDSUW           vr08, vr07                             // v7 v6 [  247] 
       VADDSUW           vr09, vr10                             // v8 v9 [  248] 
       VADDSUW           vr02, vr03                             // v1 v2 [  249] 
   	
      // Transpose the 8x8 matrix in-place 
       VEXCH1            vr05, vr07                             // v4 v6 [  252] 
       VEXCH1            vr04, vr10                             // v3 v9 [  253] 
       VEXCH1            vr02, vr09                             // v1 v8 [  254] 
       VEXCH1            vr03, vr08                             // v2 v7 [  255] 
              //  Stall: [vr02]  ...............................      vr02          vr03          vr08          vr09    
       VEXCH2            vr02, vr05                             // v1 v4 [  256] 
       VEXCH2            vr09, vr07                             // v8 v6 [  257] 
       VEXCH2            vr03, vr04                             // v2 v3 [  258] 
       VEXCH2            vr08, vr10                             // v7 v9 [  259] 
              //  Stall: [vr03]  ...............................      vr03          vr04          vr08          vr10    
       VEXCH4            vr02, vr03                             // v1 v2 [  260] 
       VEXCH4            vr05, vr04                             // v4 v3 [  261] 
       VEXCH4            vr09, vr08                             // v8 v7 [  262] 
       VEXCH4            vr07, vr10                             // v6 v9 [  263] 

	   // perform a 1-d fdct transform on eight rows simultaneously 
              //  Stall: [vr08]  ...............................      vr07          vr08          vr09          vr10    
       VADDSUW           vr05, vr08                             // v4 v7 [  266] 
       VADDSUW           vr02, vr10                             // v1 v9 [  267] 
       VADDSUW           vr09, vr04                             // v8 v3 [  268] 
       VADDSUW           vr07, vr03                             // v6 v2 [  269] 
       VADDW             vr06, vr04, vr10                    // v5 v3 v9 [  270] 
       VADDW             vr04, vr08, vr04                    // v3 v7 v3 [  271] 
       VADDSUW           vr09, vr05                             // v8 v4 [  272] 
       VADDSUW           vr02, vr07                             // v1 v6 [  273] 
       VBMULFW           vr04, vr04, 23170                      // v3 v3 [  274] 
       VADDW             vr08, vr03, vr08                    // v7 v2 v7 [  275] 
       VADDW             vr05, vr05, vr07                    // v4 v4 v6 [  276] 
       VSUBW             vr03, vr08, vr06                    // v2 v7 v5 [  277] 
       VADDW             vr06, vr06, vr06                    // v5 v5 v5 [  278] 
       VADDSUW           vr02, vr09                             // v1 v8 [  279] 
       VADDSUW           vr10, vr04                             // v9 v3 [  280] 
       VBMULFW           vr05, vr05, 23170                      // v4 v4 [  281] 
       VBMULFW           vr03, vr03, 12540                      // v2 v2 [  282] 
       VBMULFW           vr08, vr08, 17734                      // v7 v7 [  283] 
       VBMULFW           vr06, vr06, 21407                      // v5 v5 [  284] 
       VADDSUW           vr07, vr05                             // v6 v4 [  285] 
       VADDW             vr08, vr08, vr03                    // v7 v7 v2 [  286] 
       VADDW             vr06, vr06, vr03                    // v5 v5 v2 [  287] 
       VADDSUW           vr04, vr08                             // v3 v7 [  288] 
       VADDSUW           vr10, vr06                             // v9 v5 [  289] 

    // set rounding for quantisation

       VRPUT             vr20,[i12, 5]                     // auxreg ptr [  293] 
 
    
       VSEQW.1           vr01, 1                                // Intra [  296] ....................// set sticky if Intra
       VSEQW.1.S         vr21, 0                                  // alg [  297] ....................// 0xffff means H263, 0 is mpeg4, so sticky if intra and MPEG4
    

       VADDW.32          vr21, vr01, vr01    // Quant2 QuantVal QuantVal [  300] ....................// double it to get byte offset
// s16    vr21.16  block                                              // [  301] 
        VMR3W.16        vr21, vr00, vr00               // block block_no [  302] 
LINE 303".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr21]  ...............................      vr21.016
              //  Stall: [vr21]  ...............................      vr21.016
       VSUBW.16.F        vr21, vr21, 4                    // block block [  303] 
    
      
              //  Stall: [vr21]  ...............................      vr21.016      vr63.016
        VMR1W.16        vr01, vr21, vr21                   // ptr Quant2 [  306] 
LINE 307".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [i12]  ................................      vr01.016      vr63.016
              //  Stall: [i12]  ................................      vr01.016      vr63.016
       VIM               i12, i12, SDMOF_mpeg4_aan_luma_dc_scaler // ptr ptr [  307] 

              //  Stall: [vr01]  ...............................      I12  
       VADDW.16.GE       vr01, vr01, 64                       // ptr ptr [  309] ....................// offset for chroma_dc_scalar
    
              //  Stall: [i12]  ................................      vr01.016
       VLD16_1           vr00,[i12, 0]                     // DCterm ptr [  311] ....................// load dc_scaler
              //  Stall: [i1]  .................................      vr00.002
              //  Stall: [i1]  .................................      vr00.002
              //  Stall: [i1]  .................................      vr00.002
       VMIVW.1.S         vr11, i1                       // scale1 DCterm [  312] ....................// replace DC scaling value only if Intra and MPEG4
     
	// save sign of each element
       VSIGNW            vr03, vr02                             // v2 v1 [  315] 
       VSIGNW            vr24, vr10                            // v10 v9 [  316] 
       VSIGNW            vr25, vr07                            // v11 v6 [  317] 
       VSIGNW            vr26, vr08                            // v12 v7 [  318] 
       VSIGNW            vr27, vr09                            // v13 v8 [  319] 
       VSIGNW            vr28, vr04                            // v14 v3 [  320] 
       VSIGNW            vr29, vr05                            // v15 v4 [  321] 
       VSIGNW            vr30, vr06                            // v24 v5 [  322] 
	
	// make all coeffs positive
       VABSW             vr02, vr02                             // v1 v1 [  325] 
       VABSW             vr10, vr10                             // v9 v9 [  326] 
       VABSW             vr07, vr07                             // v6 v6 [  327] 
       VABSW             vr08, vr08                             // v7 v7 [  328] 
       VABSW             vr09, vr09                             // v8 v8 [  329] 
       VABSW             vr04, vr04                             // v3 v3 [  330] 
       VABSW             vr05, vr05                             // v4 v4 [  331] 
       VABSW             vr06, vr06                             // v5 v5 [  332] 


	// perform scaling and quantization on all coefficients 
       VMULFW            vr02, vr02, vr11                // v1 v1 scale1 [  336] 
       VMULFW            vr10, vr10, vr12                // v9 v9 scale2 [  337] 
       VMULFW            vr07, vr07, vr13                // v6 v6 scale3 [  338] 
       VMULFW            vr08, vr08, vr14                // v7 v7 scale4 [  339] 
       VMULFW            vr09, vr09, vr15                // v8 v8 scale5 [  340] 
       VMULFW            vr04, vr04, vr16                // v3 v3 scale6 [  341] 
       VMULFW            vr05, vr05, vr17                // v4 v4 scale7 [  342] 
       VMULFW            vr06, vr06, vr18                // v5 v5 scale8 [  343] 
	
    
       VSEQW.1           vr01, 1                                // Intra [  346] ....................// set sticky if Intra
       VSNEW.1.S         vr21, 0                                  // alg [  347] ....................// 0xffff means H263, 0 is mpeg4, so sticky if intra and H263

              //  Stall: [VFLAGS]   ............................      vr63.001
       VIV.1.S           i1, vr02                           // DCterm v1 [  349] ....................// H263 DC term is subject to limits of 1, 254

              //  Stall: [vr00]  ...............................      vr63.001      I01  
       VASRRW.2          vr00, vr00, 3                  // DCterm DCterm [  351] ....................// shift down 3 with rounding (divide by 8)
              //  Stall: [vr00]  ...............................      vr00.002
              //  Stall: [vr00]  ...............................      vr00.002
       VMINW.2           vr00, vr00, 254                // DCterm DCterm [  352] 
              //  Stall: [vr00]  ...............................      vr00.002
       VMAXW.2           vr00, vr00, 1                  // DCterm DCterm [  353] 
	// saturate to limits whilst still positive and save looking at mins
       VMINW             vr02, vr02, vr19              // v1 v1 MaxCoeff [  355] ....................// saturate at MaxCoeff
       VMINW             vr10, vr10, vr19              // v9 v9 MaxCoeff [  356] 
       VMINW             vr07, vr07, vr19              // v6 v6 MaxCoeff [  357] 
       VMINW             vr08, vr08, vr19              // v7 v7 MaxCoeff [  358] 
       VMINW             vr09, vr09, vr19              // v8 v8 MaxCoeff [  359] 
       VMINW             vr04, vr04, vr19              // v3 v3 MaxCoeff [  360] 
       VMINW             vr05, vr05, vr19              // v4 v4 MaxCoeff [  361] 
       VMINW             vr06, vr06, vr19              // v5 v5 MaxCoeff [  362] 
	
       VMIVW.1.S         vr02, i1                           // v1 DCterm [  364] ....................// put h263 intra dc term back
	// reapply sign
              //  Stall: [vr02]  ...............................      vr02.001
       VMULW             vr02, vr02, vr03                    // v1 v1 v2 [  366] 
       VMULW             vr10, vr10, vr24                   // v9 v9 v10 [  367] 
       VMULW             vr07, vr07, vr25                   // v6 v6 v11 [  368] 
       VMULW             vr08, vr08, vr26                   // v7 v7 v12 [  369] 
       VMULW             vr09, vr09, vr27                   // v8 v8 v13 [  370] 
       VMULW             vr04, vr04, vr28                   // v3 v3 v14 [  371] 
       VMULW             vr05, vr05, vr29                   // v4 v4 v15 [  372] 
       VMULW             vr06, vr06, vr30                   // v5 v5 v24 [  373] 
	


       VIM               i10, i4, MPO_PixelCoeffBuffer  // cirPtr CirBuf [  377] 
              //  Stall: [vr01]  ...............................      vr06          I10  
       VADDW.4           vr01, vr01, i0    // cirPtr cirPtr blkOutOffset [  378] 
	   // store quantized dct coefficients in memory 
              //  Stall: [i10]  ................................      vr01.004
       VST128            vr02,[i10, 0]                      // v1 cirPtr [  380] 
       VST128            vr10,[i10, 32]                     // v9 cirPtr [  381] 
       VST128            vr07,[i10, 64]                     // v6 cirPtr [  382] 
       VST128            vr08,[i10, 96]                     // v7 cirPtr [  383] 
       VST128            vr09,[i10, 128]                    // v8 cirPtr [  384] 
       VST128            vr04,[i10, 160]                    // v3 cirPtr [  385] 
       VST128            vr05,[i10, 192]                    // v4 cirPtr [  386] 
//	vst128 		v5, [cirPtr,  224] <-- moved below in jump delay slot
// End                                                                // [  388] 
// Begin                                                              // [  389] 
 // mpeg4 DC prediction function
 
// s16    vr21.1  leftDC                                              // [  392] ....................// left
// s16    vr21.2  aboveleftDC                                         // [  393] ....................// above left
// s16    vr21.8  aboveDC                                             // [  394] ....................// above
    //vec16   vecadd // needs to be setup with addresses of labels
// p16    i1  FixLeft                                                 // [  396] 
// s16    vr24.2  AboveLeftFlag                                       // [  397] 
// s16    vr24.1  LeftFlag                                            // [  398] 
// s16    vr24.8  AboveFlag                                           // [  399] 
    
// s16    vr25.8  diffup                                              // [  401] 
// s16    vr25.1  diffleft                                            // [  402] 
 
// s16    vr26.1  DCPred                                              // [  404] 
// s16    vr27.1  tempdiff                                            // [  405] 
//vec32  vr28  workarea                                               // [  406] 
// s32    vr29.3  DCFwdScaler                                         // [  407] 
// s16    vr30.1  DCScaler                                            // [  408] 
  //  s16     Quant2 : FixLeft
// s16    vr25.2  Quant3                                              // [  410] 
// s16    vr21.128  chroma                                            // [  411] 
  
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.| FixL.| blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|chroma|      |      |      | abov.| bloc.| abov.|leftDC|
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|VR24|      |      |      |      | Abov.|      | Abov.| Left.|
//|VR25|      |      |      |      |diffup|      |Quant3| diff.|
//|VR26|      |      |      |      |      |      |      |DCPred|
//|VR27|      |      |      |      |      |      |      | temp.|
//|VR28|                    workarea                           |
//|VR29|      |      |      |      |      |      |DCFw.r       |
//|VR30|      |      |      |      |      |      |      | DCSc.|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

// section to do mpeg4 Intra DC prediction
LINE 415".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VJP!i14           r13                     // dcpredict endpredict [  415] ....................// dcpredict was set when working out the rounding
       VST128            vr06,[i10, 224]  //DS________________ v5 cirPtr [  416] 
    // move16   Quant2,  QuantVal
        VMR4W.2         vr25, vr01, vr01  //DS__________ Quant3 QuantVal [  418] 
LINE 419".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr25]  ...............................      vr25.002
              //  Stall: [vr25]  ...............................      vr25.002
       VMULW.2           vr25, vr25, 3  //DS______________ Quant3 Quant3 [  419] 
    
  
       VJPT              vr23, i7                     // vecadd block_no [  422] ....................// jump to the right bit of code
       VNOP                //DS_________________________________________ [  423] 
       VNOP                //DS_________________________________________ [  424] 
       VNOP                //DS_________________________________________ [  425] 
    
//case0 --------  //DS__________________________________________________ [  427] 
 vendrec r3  //DS_______________________________________________________ [  427] 
 vrec r3  //DS__________________________________________________________ [  427] 
LINE 428".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VIM               i1, i5, -MPO_SIZE_OF_FIXED_ENTRY //DS FixLeft FixBuf [  428] 
    // load neighbour flags
       VLD8W             vr24,[i4, MPO_LeftMB]        // LeftFlag CirBuf [  430] 
       VLD8W_3           vr24,[i4, MPO_AboveMB]      // AboveFlag CirBuf [  431] 
       VLD8W_1           vr24,[i4, MPO_AboveLeftMB] // AboveLeftFlag CirBuf [  432] 
    // now check that the MBs are valid, same ID and Intra
              //  Stall: [vr24]  ...............................      vr24.011
              //  Stall: [vr24]  ...............................      vr24.010
              //  Stall: [vr24]  ...............................      vr24.002
       VAND.11           vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag LeftFlag'AboveLeftFlag'AboveFlag [  434] ....................// check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
              //  Stall: [vr24]  ...............................      vr24.011
       VSNEW.11          vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag [  437] 
   // load the DC terms in the buffers
       VLD16_1           vr21,[i1, MPO_Y_IntraPredDC_Coeff+6] // aboveleftDC FixLeft [  439] 
       VLD16             vr21,[i6, MPO_Y_IntraPredDC_Coeff+2] // leftDC CirLeft [  440] 
    
       VJP               r14                                  // predict [  442] 
       VLD16_3           vr21,[i5, MPO_Y_IntraPredDC_Coeff+4] //DS aboveDC FixBuf [  443] 
    // overwrite the values with 1024 if sticky is set (i.e. not (valid, same id and intra))
    
       VMOVW.11.S        vr21, 1024  //DS____ leftDC'aboveleftDC'aboveDC [  446] 
    
       VIM               i1, i13, SDMOF_mpeg4_luma_dc_scaler //DS FixLeft QuantVal [  448] 
    
//case1 --------                                                      // [  450] 
 vendrec r4                                                           // [  450] 
 vrec r4                                                              // [  450] 
LINE 451".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VLD16             vr21,[i4, MPO_Y_IntraPredDC_Coeff] // leftDC CirBuf [  451] ....................// in same MB
    
       VLD8W_3           vr24,[i4, MPO_AboveMB]      // AboveFlag CirBuf [  453] 
              //  Stall: [vr24]  ...............................      vr21.001      vr24.008
              //  Stall: [vr24]  ...............................      vr21.001      vr24.008
              //  Stall: [vr24]  ...............................      vr24.008
        VMR2W.2         vr24, vr24, vr24      // AboveLeftFlag AboveFlag [  454] 
LINE 455".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr24]  ...............................      vr24.002
              //  Stall: [vr24]  ...............................      vr24.002
       VAND.10           vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // AboveLeftFlag'AboveFlag AboveLeftFlag'AboveFlag [  455] ....................// check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value

       VLD16_1           vr21,[i5, MPO_Y_IntraPredDC_Coeff+4] // aboveleftDC FixBuf [  459] 
       VLD16_3           vr21,[i5, MPO_Y_IntraPredDC_Coeff+6] // aboveDC FixBuf [  460] 
      // now check that the MBs are valid, same ID and Intra
       VAND.10           vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // AboveLeftFlag'AboveFlag AboveLeftFlag'AboveFlag [  462] ....................// check if data valid
   
       VJP               r14                                  // predict [  464] 
       VSNEW.10          vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA //DS AboveLeftFlag'AboveFlag [  465] 
              //  Stall: [VFLAGS]   ............................      vr63.010
       VMOVW.10.S        vr21, 1024  //DS___________ aboveleftDC'aboveDC [  466] 
       VIM               i1, i13, SDMOF_mpeg4_luma_dc_scaler //DS FixLeft QuantVal [  467] 
//case2 --------                                                      // [  468] 
 vendrec r5                                                           // [  468] 
 vrec r5                                                              // [  468] 
LINE 469".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VLD16_3           vr21,[i4, MPO_Y_IntraPredDC_Coeff] // aboveDC CirBuf [  469] ....................// current MB
    
       VLD8W             vr24,[i4, MPO_LeftMB]        // LeftFlag CirBuf [  471] 
              //  Stall: [vr24]  ...............................      vr21.008      vr24.001
              //  Stall: [vr24]  ...............................      vr21.008      vr24.001
              //  Stall: [vr24]  ...............................      vr24.001
        VMR7W.2         vr24, vr24, vr24       // AboveLeftFlag LeftFlag [  472] 
    
LINE 474".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VLD16             vr21,[i6, MPO_Y_IntraPredDC_Coeff+6] // leftDC CirLeft [  474] 
       VLD16_1           vr21,[i6, MPO_Y_IntraPredDC_Coeff+2] // aboveleftDC CirLeft [  475] 
    
   // now check that the MBs are valid, same ID and Intra
       VAND.3            vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag LeftFlag'AboveLeftFlag [  478] ....................// check if data valid
       VJP               r14                                  // predict [  479] 
       VSNEW.3           vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA //DS LeftFlag'AboveLeftFlag [  480] 
              //  Stall: [VFLAGS]   ............................      vr63.003
       VMOVW.3.S         vr21, 1024  //DS____________ leftDC'aboveleftDC [  481] 
       VIM               i1, i13, SDMOF_mpeg4_luma_dc_scaler //DS FixLeft QuantVal [  482] 
//case3 --------                                                      // [  483] 
 vendrec r6                                                           // [  483] 
 vrec r6                                                              // [  483] 
LINE 484".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VLD16             vr21,[i4, MPO_Y_IntraPredDC_Coeff+4] // leftDC CirBuf [  484] ....................// all in current MB
       VJP               r14                                  // predict [  485] 
       VLD16_1           vr21,[i4, MPO_Y_IntraPredDC_Coeff] //DS aboveleftDC CirBuf [  486] 
       VLD16_3           vr21,[i4, MPO_Y_IntraPredDC_Coeff+2] //DS aboveDC CirBuf [  487] 
       VIM               i1, i13, SDMOF_mpeg4_luma_dc_scaler //DS FixLeft QuantVal [  488] 
//case4 --------                                                      // [  489] 
 vendrec r7                                                           // [  489] 
 vrec r7                                                              // [  489] 
LINE 490".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VIM               i1, i5, -MPO_SIZE_OF_FIXED_ENTRY // FixLeft FixBuf [  490] 
    // load neighbour flags
       VLD8W             vr24,[i4, MPO_LeftMB]        // LeftFlag CirBuf [  492] 
       VLD8W_3           vr24,[i4, MPO_AboveMB]      // AboveFlag CirBuf [  493] 
       VLD8W_1           vr24,[i4, MPO_AboveLeftMB] // AboveLeftFlag CirBuf [  494] 
    // now check that the MBs are valid, same ID and Intra
              //  Stall: [vr24]  ...............................      vr24.011
              //  Stall: [vr24]  ...............................      vr24.010
              //  Stall: [vr24]  ...............................      vr24.002
       VAND.11           vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag LeftFlag'AboveLeftFlag'AboveFlag [  496] ....................// check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
              //  Stall: [vr24]  ...............................      vr24.011
       VSNEW.11          vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag [  499] 
 
       VLD16             vr21,[i6, MPO_UV_IntraPredDC_Coeff] // leftDC CirLeft [  501] 
       VLD16_1           vr21,[i1, MPO_UV_IntraPredDC_Coeff] // aboveleftDC FixLeft [  502] 
       VLD16_3           vr21,[i5, MPO_UV_IntraPredDC_Coeff] // aboveDC FixBuf [  503] 

       VJP               r14                                  // predict [  505] 
       VMOVW.11.S        vr21, 1024  //DS____ leftDC'aboveleftDC'aboveDC [  506] 
    
       VIM               i1, i13, SDMOF_mpeg4_chroma_dc_scaler //DS FixLeft QuantVal [  508] 
     
       VIM               i10, i4, MPO_U_DC_CoeffStore  //DS cirPtr CirBuf [  510] 
//case5 --------                                                      // [  511] 
 vendrec r8                                                           // [  511] 
 vrec r8                                                              // [  511] 
LINE 512".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VIM               i1, i5, -MPO_SIZE_OF_FIXED_ENTRY // FixLeft FixBuf [  512] 
    // load neighbour flags
       VLD8W             vr24,[i4, MPO_LeftMB]        // LeftFlag CirBuf [  514] 
       VLD8W_3           vr24,[i4, MPO_AboveMB]      // AboveFlag CirBuf [  515] 
       VLD8W_1           vr24,[i4, MPO_AboveLeftMB] // AboveLeftFlag CirBuf [  516] 
    // now check that the MBs are valid, same ID and Intra
              //  Stall: [vr24]  ...............................      vr24.011
              //  Stall: [vr24]  ...............................      vr24.010
              //  Stall: [vr24]  ...............................      vr24.002
       VAND.11           vr24, vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag LeftFlag'AboveLeftFlag'AboveFlag [  518] ....................// check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
              //  Stall: [vr24]  ...............................      vr24.011
       VSNEW.11          vr24, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // LeftFlag'AboveLeftFlag'AboveFlag [  521] 
 
       VLD16             vr21,[i6, MPO_UV_IntraPredDC_Coeff+2] // leftDC CirLeft [  523] 
       VLD16_1           vr21,[i1, MPO_UV_IntraPredDC_Coeff+2] // aboveleftDC FixLeft [  524] 
       VLD16_3           vr21,[i5, MPO_UV_IntraPredDC_Coeff+2] // aboveDC FixBuf [  525] 
    
       VMOVW.11.S        vr21, 1024        // leftDC'aboveleftDC'aboveDC [  527] ....................// set to 1024 if not usable
    
       VIM               i1, i13, SDMOF_mpeg4_chroma_dc_scaler // FixLeft QuantVal [  529] 
        
//    vim          cirPtr, CirBuf, MPO_V_DC_CoeffStore 
       VIM               i10, i4, MPO_U_DC_CoeffStore+2 // cirPtr CirBuf [  532] 
//predict --------                                                    // [  533] 
 vendrec r14                                                          // [  533] 
 vrec r14                                                             // [  533] 
    // having worked out A, B and C we can decide direction of prediction
    // and actual value
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.| FixL.| blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|chroma|      |      |      | abov.| bloc.| abov.|leftDC|
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|VR24|      |      |      |      | Abov.|      | Abov.| Left.|
//|VR25|      |      |      |      |diffup|      |Quant3| diff.|
//|VR26|      |      |      |      |      |      |      |DCPred|
//|VR27|      |      |      |      |      |      |      | temp.|
//|VR28|                    workarea                           |
//|VR29|      |      |      |      |      |      |DCFw.r       |
//|VR30|      |      |      |      |      |      |      | DCSc.|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

        VMR6W.8         vr25, vr21, vr21           // diffup aboveleftDC [  537] 
LINE 538".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr25]  ...............................      vr25.008
              //  Stall: [vr25]  ...............................      vr25.008
       VDIFW.8           vr25, vr25, vr21       // diffup diffup aboveDC [  538] ....................// abs(B - C)
        VMR1W.1         vr25, vr21, vr21         // diffleft aboveleftDC [  539] 
LINE 540".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr25]  ...............................      vr25.001
              //  Stall: [vr25]  ...............................      vr25.001
       VDIFW.1           vr25, vr25, vr21    // diffleft diffleft leftDC [  540] ....................// abs(B - A)
    
        VMR3W.1         vr26, vr21, vr21               // DCPred aboveDC [  542] ....................// assume C
        VMR3W.1         vr27, vr25, vr25              // tempdiff diffup [  543] 
LINE 544".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr27]  ...............................      vr26.001      vr27.001
              //  Stall: [vr27]  ...............................      vr27.001
       VLEW.1.F          vr27, vr27, vr25  // tempdiff tempdiff diffleft [  544] ....................//set condition flag if |B-C| <= |B-A|    
              //  Stall: [VFLAGS]   ............................      vr27.001      vr63.001
       VMVW.1.NZ         vr26, vr21                     // DCPred leftDC [  545] ....................// move left accordingly

       VADDW.2           vr00, vr00, vr25      // FixLeft FixLeft Quant3 [  547] ....................// 4*quantVal needed, as data is int
              //  Stall: [i1]  .................................      vr00.002      vr63.001
       VLD32             vr30,[i1, 0]                // DCScaler FixLeft [  548] 
   // vup.1       workarea, 'DCPred // move 16 bit entry to 32 bit area
              //  Stall: [vr30]  ...............................      vr30.003
              //  Stall: [vr30]  ...............................      vr30.003
              //  Stall: [vr30]  ...............................      vr30.003
       VMUL.3            vr28, vr30, vr26    // workarea DCScaler DCPred [  550] ....................// quantiser with current dc-scaler
              //  Stall: [vr28]  ...............................      vr28.003
              //  Stall: [vr28]  ...............................      vr28.003
       VASRRP.15         vr26, vr28, 20               // DCPred workarea [  551] ....................// add 1<<19
    
    
  
    
              //  Stall: [vr26]  ...............................      vr26.015
              //  Stall: [vr26]  ...............................      vr26.015
       VSUBW.1           vr02, vr02, vr26                // v1 v1 DCPred [  556] ....................// subtract prediction from DC value
    
        VMR6W.2         vr00, vr00, vr00             // FixLeft block_no [  558] 
LINE 559".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
              //  Stall: [vr00]  ...............................      vr00.002
              //  Stall: [vr00]  ...............................      vr00.002
       VAND.2            vr00, vr00, 4                // FixLeft FixLeft [  559] 
              //  Stall: [i1]  .................................      vr00.002
       VJP.i1            r15                       // FixLeft chromasave [  560] ....................// branch if chroma
       VNOP                //DS_________________________________________ [  561] 
       VNOP                //DS_________________________________________ [  562] 
       VNOP                //DS_________________________________________ [  563] 
    
       VIM               i10, i4, 0                     // cirPtr CirBuf [  565] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  566] 
              //  Stall: [i10]  ................................      vr01.004
       VST16             vr02,[i10, MPO_Y_DC_CoeffStore0]   // v1 cirPtr [  567] ....................// store DC diff
    
       VADDW.1           vr02, vr02, vr26                // v1 v1 DCPred [  569] ....................// add prediction back to difference
       VIM               i1, i13, SDMOF_mpeg4_fwd_luma_dc_scaler // FixLeft QuantVal [  570] 
              //  Stall: [i1]  .................................      I01  
       VLD8W             vr30,[i1, 0]                // DCScaler FixLeft [  571] ....................// load dc_scaler
              //  Stall: [vr30]  ...............................      vr30.001
              //  Stall: [vr30]  ...............................      vr30.001
              //  Stall: [vr30]  ...............................      vr30.001
       VMULW.1           vr02, vr02, vr30              // v1 v1 DCScaler [  572] ....................// dequantise
    // need to saturate
              //  Stall: [vr02]  ...............................      vr02.001
              //  Stall: [vr02]  ...............................      vr02.001
       VMINW.1           vr02, vr02, 2047                       // v1 v1 [  574] ....................// saturate to 2047
    
              //  Stall: [vr02]  ...............................      vr02.001
       VST16             vr02,[i10, MPO_Y_IntraPredDC_Coeff] // v1 cirPtr [  576] ....................// store DC
 // now set up output pointer to store for Inverse Q
       VMOVW             vr01, 0, 2                              // temp [  578] 
              //  Stall: [i9]  .................................      vr01.002
       VIM               i10, i9, SDMOF_mpeg4_diff_pixel_offset // cirPtr temp [  579] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  580] 
              //  Stall: [i10]  ................................      vr01.004
       VLD16             vr00,[i10, 0]            // blkOutOffset cirPtr [  581] ....................// load output offset

       
       VJP               r13                               // endpredict [  584] 
       VIM               i10, i4, MPO_PixelCoeffBuffer  //DS cirPtr CirBuf [  585] 
              //  Stall: [vr01]  ...............................      vr00.001      I10  
       VADDW.4           vr01, vr01, i0  //DS cirPtr cirPtr blkOutOffset [  586] 
              //  Stall: [i10]  ................................      vr01.004
       VST16             vr02,[i10, 0]  //DS__________________ v1 cirPtr [  587] ....................// save dc ready to be inverse dct'ed
//chromasave --------                                                 // [  588] 
 vendrec r15                                                          // [  588] 
 vrec r15                                                             // [  588] 

LINE 590".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VST16             vr02,[i10, 0]                      // v1 cirPtr [  590] ....................// store DC diff
    
       VADDW.1           vr02, vr02, vr26                // v1 v1 DCPred [  592] ....................// add prediction back to difference
       VIM               i1, i13, SDMOF_mpeg4_fwd_chroma_dc_scaler // FixLeft QuantVal [  593] 
              //  Stall: [i1]  .................................      I01  
       VLD8W             vr30,[i1, 0]                // DCScaler FixLeft [  594] ....................// load dc_scaler
              //  Stall: [vr30]  ...............................      vr30.001
              //  Stall: [vr30]  ...............................      vr30.001
              //  Stall: [vr30]  ...............................      vr30.001
       VMULW.1           vr02, vr02, vr30              // v1 v1 DCScaler [  595] ....................// dequantise
    // need to saturate
              //  Stall: [vr02]  ...............................      vr02.001
              //  Stall: [vr02]  ...............................      vr02.001
       VMINW.1           vr02, vr02, 2047                       // v1 v1 [  597] 
       VIM               i10, i4, -8                    // cirPtr CirBuf [  598] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  599] 
              //  Stall: [i10]  ................................      vr01.004
       VST16             vr02,[i10, MPO_UV_IntraPredDC_Coeff] // v1 cirPtr [  600] ....................// store DC diff
     
       VMOVW             vr01, 0, 2                              // temp [  602] 
              //  Stall: [i9]  .................................      vr01.002
       VIM               i10, i9, SDMOF_mpeg4_diff_pixel_offset // cirPtr temp [  603] 
              //  Stall: [vr01]  ...............................      I10  
       VADDW.4           vr01, vr01, vr21   // cirPtr cirPtr blockdouble [  604] 
              //  Stall: [i10]  ................................      vr01.004
       VLD16             vr00,[i10, 0]            // blkOutOffset cirPtr [  605] 

       VIM               i10, i4, MPO_PixelCoeffBuffer  // cirPtr CirBuf [  607] 
              //  Stall: [vr01]  ...............................      vr00.001      I10  
              //  Stall: [vr01]  ...............................      vr00.001
       VADDW.4           vr01, vr01, i0    // cirPtr cirPtr blkOutOffset [  608] 
              //  Stall: [i10]  ................................      vr01.004
       VST16             vr02,[i10, 0]                      // v1 cirPtr [  609] ....................// save dc ready to be inverse dct'ed
//endpredict --------                                                 // [  610] 
 vendrec r13                                                          // [  610] 
 vrec r13                                                             // [  610] 
// End                                                                // [  611] 
// Begin                                                              // [  612] 
// section to calculate whether data needs to be sent for this block (cbp)
// s16    vr21.1  cbp                                                 // [  614] 
//vec16  vr24  Inter                                                  // [  615] 
// p16    i1  zero                                                    // [  616] 
// ****** Informational : Register space for symbol "temp" already allocated. // [  617] 
// p16    i0  temp                                                    // [  617] 
// s16    vr25.1  subblock                                            // [  618] 
// s16    vr26.1  shiftval                                            // [  619] 
//vec16  vr27  dcterm                                                 // [  620] 
	// now work out if any coeffs are of interest
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| bloc.| CirL.|FixBuf|CirBuf| stan.| roun.|  zero| blkO.|
//|VR 1|      | dcpr.| Quan.|   ptr|      |cirPtr|  temp| Intra|
//|VR 2|                          v1                           |
//|VR 3|                          v2                           |
//|VR 4|                          v3                           |
//|VR 5|                          v4                           |
//|VR 6|                          v5                           |
//|VR 7|                          v6                           |
//|VR 8|                          v7                           |
//|VR 9|                          v8                           |
//|VR10|                          v9                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                      scale8                           |
//|VR19|                    MaxCoeff                           |
//|VR20|                      auxreg                           |
//|VR21|      |      |      |      |      | bloc.|      |   cbp|
//|VR22|                   normround                           |
//|VR23|                      vecadd                           |
//|VR24|                       Inter                           |
//|VR25|      |      |      |      |      |      |      | subb.|
//|VR26|      |      |      |      |      |      |      | shif.|
//|VR27|                      dcterm                           |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

        vmvw.1          vr00, vr01                         // temp Intra [  623] 
LINE 624".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr00, 0, 2                              // zero [  624] 
       VMOVW             vr27, 0, 255                          // dcterm [  625] ....................// initialise dcterm
       VSNEW.1           vr00, 0                                 // temp [  626] ....................// Intra holds all 1 if Intra, all 0 if Inter
              //  Stall: [VFLAGS]   ............................      vr63.001
       VMVW.1.S          vr27, vr02                         // dcterm v1 [  627] ....................// if intra save dcterm
       VMIVW.1.S         vr02, i1                             // v1 zero [  628] ....................// zero DC term if Intra
    
  
    
    // find out if any non-zero values
              //  Stall: [vr02]  ...............................      vr02.001      vr63.001
       VABSW             vr02, vr02                             // v1 v1 [  633] ....................// take abs of first vec
       VABSAW            vr02, vr10                             // v1 v9 [  634] ....................// accum abs of next etc
       VABSAW            vr02, vr07                             // v1 v6 [  635] 
       VABSAW            vr02, vr08                             // v1 v7 [  636] 
       VABSAW            vr02, vr09                             // v1 v8 [  637] 
       VABSAW            vr02, vr04                             // v1 v3 [  638] 
       VABSAW            vr02, vr05                             // v1 v4 [  639] 
       VABSAW            vr02, vr06                             // v1 v5 [  640] 
              //  Stall: [vr02]  ...............................      vr02    
              //  Stall: [vr02]  ...............................      vr02    
              //  Stall: [vr02]  ...............................      vr02    
       VXSUMW            vr02, vr02, 255                        // v1 v1 [  641] ....................// every element contains sum of all coeffs
    // now check to see if non-zero and set cbp accordingly
              //  Stall: [vr02]  ...............................      vr02    
              //  Stall: [vr02]  ...............................      vr02    
       VSNEW             vr02, 0                                   // v1 [  643] ....................// set sticky if non zero  
     
       VLD16             vr21,[i4, MPO_NonZeroCoeffLuma]   // cbp CirBuf [  645] ....................// load cbp
    
        VMR7W.1         vr25, vr00, vr00            // subblock block_no [  647] 
LINE 648".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VIM               i0, i1, 11                         // temp zero [  648] 
              //  Stall: [vr25]  ...............................      vr21.001      vr25.001      I00  
       VRSUBW.1          vr26, vr25, i0        // shiftval subblock temp [  649] ....................// (11 - block_no)
       VIM               i0, i1, 1                          // temp zero [  650] 
              //  Stall: [vr00]  ...............................      I00  
       VLSLVW.1          vr00, vr00, vr26          // temp temp shiftval [  651] ....................// 1<<(11-block_no)
              //  Stall: [vr00]  ...............................      vr00.001
       VOR.1.S           vr21, vr21, vr00                // cbp cbp temp [  652] ....................// or it with cbp if non-zero
    
   // vsnew        dcterm, 0           //no        // check dc term is non-zero (only poss in Intra)
  //  vlsrw       'temp, 'temp, 6      //no         // create 1<< (5-block_no)
   // vor.s       'cbp, 'cbp, 'temp    //no       // or it with cbp if non-zero
    
              //  Stall: [vr21]  ...............................      vr21.001
       VST16             vr21,[i4, MPO_NonZeroCoeffLuma]   // cbp CirBuf [  658] ....................// store it back
// End                                                                // [  659] 

// check to see if we have done all blocks
LINE 662".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VIM               i7, i7, 1                  // block_no block_no [  662] 
              //  Stall: [i7]  .................................      I07  
       VIM               i9, i7, -6                     // temp block_no [  663] 
              //  Stall: [i9]  .................................      I09  
       VJP.i9            r10                            // temp MainLoop [  664] 
       VNOP                //DS_________________________________________ [  665] 
       VNOP                //DS_________________________________________ [  666] 
       VNOP                //DS_________________________________________ [  667] 
// end of Main Block Loop
    
    


//Finish --------                                                     // [  673] ....................// not currently used
 vendrec r16                                                          // [  673] 
 vrec r16                                                             // [  673] 
    // put back the normal rounding value
LINE 675".\ARCEncoder\MPEG4\ArcFDCT_MPA.sss"
       VMOVW             vr20, 16384, 1                        // auxreg [  675] 
       VMOVW             vr01, 0, 16                              // ptr [  676] 
              //  Stall: [i12]  ................................      vr01.016
       VRPUT             vr20,[i12, 5]                     // auxreg ptr [  677] 
    
// End                                                                // [  679] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FDCT_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FDCT                                                       // [  680] 



const int MACRO_IDX_FDCT = 0;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Fdct_Record(int sa, int SIMDnum)
{
    int *map = Fdct;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_FDCT] = sa;
    sa = Record_FDCT(map);

    return sa;
}
