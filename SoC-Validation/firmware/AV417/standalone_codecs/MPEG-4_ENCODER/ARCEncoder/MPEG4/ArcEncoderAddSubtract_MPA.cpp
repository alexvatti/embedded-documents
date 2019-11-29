//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss'

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




//  showstalls
// MetaWare ouput ON                                                                                        // [   22] 
#define LINE #line                                                                                          // [   22] 

// ScanMP: Including file .\ARCEncoder\MPEG4\..\ARC\SIMD_ABI.ii.
// (c) Arc international 2007
// Creo vrapter ABI using MPC data structures
// To co-operate with ME & EE hardware

// Media processor ABI ---------------------------
//	vec16				Scalars0 = vr00
//	vec16				Scalars1 = vr01
//	**	i0 - i3			Scalar params/scratch
// p16    i4  CirBuf                                                                                        // [    9] 
// p16    i5  FixBuf                                                                                        // [   10] 
// p16    i6  CirLeft                                                                                       // [   11] 
//	**	i7				scratch
// 	**	i8 - i15 		Work - should be preserved
//	**	vr01 - vr04		params
//	**	vr05 - vr30		scratch
//vec16  vr62  VZERO                                                                                        // [   16] 
//vec16  vr31  VSTACK                                                                                       // [   17] 
//vec16  vr63  VFLAGS                                                                                       // [   18] 

// Media processor ABI end -----------------------


// -----------------------------------------------
// CirBuf, FixBuf , CirLeft are set by  "SetCurrentMPC" in ArcSDMTables_MPA.sss
// -----------------------------------------------


// ScanMP: End of included file ..\ARC\SIMD_ABI.ii.

#include ".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.h"


// Macro table
    int EncoderAddSubtract[7];
    int EncoderAddSubtract_Record(int sa, int SIMDnum);



#include "ArcMPC.h"
#include "ArcFrameBuffers.h"


//------------------------------------------------------------------------------
// Name:          MakeLumaPredictionError16X16
// Purpose:       Computes the residual block by subtracting the prediction
//                data from the current block and storing the residual in 
//                the residual data buffer in SDM
// Arguments:     ca     = current block address in SDM
//                pa     = prediction address in SDM
//                ea     = residual block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] MakeLumaPredictionError16X16                                                                     // [   44] 
_Asm int Record_MakeLumaPredictionError16X16(int *mac_tab)
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
MakeLumaPredictionError16X16_RecLoop:
  ld r1,[r17 ,0]
  vrec r1

// p16    i0  ca                                                                                            // [   45] 
// p16    i1  pa                                                                                            // [   46] 
// p16    i2  ea                                                                                            // [   47] 
// Begin                                                                                                    // [   48] 
// p16    i3  lpCnt                                                                                         // [   49] // loop count

//vec16  vr02  curLane0                                                                                     // [   51] 
//vec16  vr03  curLane1                                                                                     // [   52] 
//vec16  vr04  curLane2                                                                                     // [   53] 
//vec16  vr05  curLane3                                                                                     // [   54] 

//vec16  vr06  predLane0                                                                                    // [   56] 
//vec16  vr07  predLane1                                                                                    // [   57] 
//vec16  vr08  predLane2                                                                                    // [   58] 
//vec16  vr09  predLane3                                                                                    // [   59] 
    
    // Initialise the loop count
    // The luma MB (16x16) is divided into 8 slices of 2 rows each.
    // One row is processed per iteration
LINE 64".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 8-1, 8                                                                 // lpCnt [   64] 
    
//LpStart --------                                                                                          // [   66] 
 vendrec r3                                                                                                 // [   66] 
 vrec r3                                                                                                    // [   66] 
LINE 67".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD64W            vr02,[i0, 0*8]                                                         // curLane0 ca [   67] 
       VLD64W            vr03,[i0, 1*8]                                                         // curLane1 ca [   68] 
       VLD64W            vr04,[i0, 2*8]                                                         // curLane2 ca [   69] 
       VLD64W            vr05,[i0, 3*8]                                                         // curLane3 ca [   70] 

       VLD64W            vr06,[i1, 0*8]                                                        // predLane0 pa [   72] 
       VLD64W            vr07,[i1, 1*8]                                                        // predLane1 pa [   73] 
       VLD64W            vr08,[i1, 2*8]                                                        // predLane2 pa [   74] 
       VLD64W            vr09,[i1, 3*8]                                                        // predLane3 pa [   75] 
    
       VSUBW             vr02, vr02, vr06                                       // curLane0 curLane0 predLane0 [   77] 
       VSUBW             vr03, vr03, vr07                                       // curLane1 curLane1 predLane1 [   78] 
       VSUBW             vr04, vr04, vr08                                       // curLane2 curLane2 predLane2 [   79] 
       VSUBW             vr05, vr05, vr09                                       // curLane3 curLane3 predLane3 [   80] 
    
       VST128            vr02,[i2, 0*16]                                                        // curLane0 ea [   82] 
       VST128            vr03,[i2, 1*16]                                                        // curLane1 ea [   83] 
       VST128            vr04,[i2, 2*16]                                                        // curLane2 ea [   84] 
       VST128            vr05,[i2, 3*16]                                                        // curLane3 ea [   85] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [   90] 
       VIM               i0, i0, 32  //DS_______________________________________________________________ ca ca [   91] 
       VIM               i1, i1, 32  //DS_______________________________________________________________ pa pa [   92] 
       VIM               i2, i2, 64  //DS_______________________________________________________________ ea ea [   93] 
        
// End                                                                                                      // [   95] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz MakeLumaPredictionError16X16_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc MakeLumaPredictionError16X16                                                                     // [   97] 



//------------------------------------------------------------------------------
// Name:          MakeChromaPredictionError
// Purpose:       Computes the residual block by subtracting the prediction
//                data from the current block and storing the residual in 
//                the residual data buffer in SDM
// Arguments:     ca     = current block address in SDM
//                pa     = prediction address in SDM
//                ea     = residual block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] MakeChromaPredictionError                                                                        // [  109] 
_Asm int Record_MakeChromaPredictionError(int *mac_tab)
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
MakeChromaPredictionError_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

// p16    i0  ca                                                                                            // [  110] 
// p16    i1  pa                                                                                            // [  111] 
// p16    i2  ea                                                                                            // [  112] 
// Begin                                                                                                    // [  113] 
// p16    i3  lpCnt                                                                                         // [  114] // loop count

//vec16  vr02  curLane0                                                                                     // [  116] 
//vec16  vr03  curLane1                                                                                     // [  117] 
//vec16  vr04  curLane2                                                                                     // [  118] 
//vec16  vr05  curLane3                                                                                     // [  119] 

//vec16  vr06  predLane0                                                                                    // [  121] 
//vec16  vr07  predLane1                                                                                    // [  122] 
//vec16  vr08  predLane2                                                                                    // [  123] 
//vec16  vr09  predLane3                                                                                    // [  124] 
    
    // Initialise the loop count
    // The chroma block (8x8) is divided into 2 slices of 4 rows each.
    // One row is processed per iteration
LINE 129".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 2-1, 8                                                                 // lpCnt [  129] 
    
//LpStart --------                                                                                          // [  131] 
 vendrec r3                                                                                                 // [  131] 
 vrec r3                                                                                                    // [  131] 
LINE 132".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD64W            vr02,[i0, 0*16]                                                        // curLane0 ca [  132] 
       VLD64W            vr03,[i0, 1*16]                                                        // curLane1 ca [  133] 
       VLD64W            vr04,[i0, 2*16]                                                        // curLane2 ca [  134] 
       VLD64W            vr05,[i0, 3*16]                                                        // curLane3 ca [  135] 

       VLD64W            vr06,[i1, 0*16]                                                       // predLane0 pa [  137] 
       VLD64W            vr07,[i1, 1*16]                                                       // predLane1 pa [  138] 
       VLD64W            vr08,[i1, 2*16]                                                       // predLane2 pa [  139] 
       VLD64W            vr09,[i1, 3*16]                                                       // predLane3 pa [  140] 
    
       VSUBW             vr02, vr02, vr06                                       // curLane0 curLane0 predLane0 [  142] 
       VSUBW             vr03, vr03, vr07                                       // curLane1 curLane1 predLane1 [  143] 
       VSUBW             vr04, vr04, vr08                                       // curLane2 curLane2 predLane2 [  144] 
       VSUBW             vr05, vr05, vr09                                       // curLane3 curLane3 predLane3 [  145] 
    
       VST128            vr02,[i2, 0*32]                                                        // curLane0 ea [  147] 
       VST128            vr03,[i2, 1*32]                                                        // curLane1 ea [  148] 
       VST128            vr04,[i2, 2*32]                                                        // curLane2 ea [  149] 
       VST128            vr05,[i2, 3*32]                                                        // curLane3 ea [  150] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [  155] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ ca ca [  156] 
       VIM               i1, i1, 64  //DS_______________________________________________________________ pa pa [  157] 
       VIM               i2, i2, 128  //DS______________________________________________________________ ea ea [  158] 
        
// End                                                                                                      // [  160] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz MakeChromaPredictionError_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc MakeChromaPredictionError                                                                        // [  162] 



//------------------------------------------------------------------------------
// Name:          CalculateSkippedChromaCost
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CalculateSkippedChromaCost                                                                       // [  170] 
_Asm int Record_CalculateSkippedChromaCost(int *mac_tab)
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
CalculateSkippedChromaCost_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

    // function arguments

// Begin                                                                                                    // [  173] 
    // return value
// p32    k0  WorstSkippedChromaCost                                                                        // [  175] // i0 & i1
// ****** Informational : Register space for symbol "WorstSkippedChromaCostLow" already allocated.          // [  176] // synonym for low 16 bit portion
// p16    i0  WorstSkippedChromaCostLow                                                                     // [  176] 
// ****** Informational : Register space for symbol "WorstSkippedChromaCostHigh" already allocated.         // [  177] // synonym for high 16 bit portion
// p16    i1  WorstSkippedChromaCostHigh                                                                    // [  177] 

// p16    i2  cpel                                                                                          // [  179] // current pointer
// p16    i3  ppel                                                                                          // [  180] // prediction pointer

// p16    i7  innerRow                                                                                      // [  182] 
// p16    i8  outerRow                                                                                      // [  183] 

//vec32  vr02  current0                                                                                     // [  185] 
//vec32  vr03  current1                                                                                     // [  186] 
//vec32  vr04  current2                                                                                     // [  187] 
//vec32  vr05  current3                                                                                     // [  188] 

//vec32  vr06  prediction0                                                                                  // [  190] 
//vec32  vr07  prediction1                                                                                  // [  191] 
//vec32  vr08  prediction2                                                                                  // [  192] 
//vec32  vr09  prediction3                                                                                  // [  193] 

//vec32  vr10  ssd0                                                                                         // [  195] 
//vec32  vr11  ssd1                                                                                         // [  196] 
//vec32  vr12  ssd2                                                                                         // [  197] 
//vec32  vr13  ssd3                                                                                         // [  198] 

//vec32  vr14  cost0                                                                                        // [  200] 
//vec32  vr15  cost1                                                                                        // [  201] 
//vec32  vr16  cost2                                                                                        // [  202] 
//vec32  vr17  cost3                                                                                        // [  203] 


    // setup source pointers
LINE 207".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VIM               i2, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_U                             // cpel CirBuf [  207] // will do both u and v
       VIM               i3, i4, MPO_PixelCoeffBuffer+PCB_REFERENCE_U                           // ppel CirBuf [  208] // will do both u and v

    // reset worst cost
       VMOVW             vr00, 0, 3                                                  // WorstSkippedChromaCost [  211] 

    // setup outer loop counter
    // 2 iterations total, first row of 4x4s, then second row of 4x4s (for u and v)
       VMOVW             vr01, 2-1, 1                                                              // outerRow [  215] 


    // outerRowLoop start
//outerRowLoop --------                                                                                     // [  219] 
 vendrec r3                                                                                                 // [  219] 
 vrec r3                                                                                                    // [  219] 

    // reset 4x4 costs
LINE 222".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr14, 0, 255                                                                 // cost0 [  222] 
       VMOVW             vr15, 0, 255                                                                 // cost1 [  223] 
       VMOVW             vr16, 0, 255                                                                 // cost2 [  224] 
       VMOVW             vr17, 0, 255                                                                 // cost3 [  225] 

    // setup inner loop counter
    // 4 rows total, 1 row per loop (u + v)
       VMOVW             vr00, (4/1)-1, 128                                                        // innerRow [  229] 


    // innerRowLoop start
//innerRowLoop --------                                                                                     // [  233] 
 vendrec r4                                                                                                 // [  233] 
 vrec r4                                                                                                    // [  233] 

    // load current u and v (8 bits per pixel)
LINE 236".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD32WL           vr02,[i2, 0]                                                         // current0 cpel [  236] // u
       VLD32WL           vr03,[i2, 4]                                                         // current1 cpel [  237] // u
       VLD32WL           vr04,[i2, 8]                                                         // current2 cpel [  238] // v
       VLD32WL           vr05,[i2, 12]                                                        // current3 cpel [  239] // v

    // unpack current from 16 bit lanes into 32 bit lanes
       VUP.255           vr02, vr02                                                       // current0 current0 [  242] 
       VUP.255           vr03, vr03                                                       // current1 current1 [  243] 
       VUP.255           vr04, vr04                                                       // current2 current2 [  244] 
       VUP.255           vr05, vr05                                                       // current3 current3 [  245] 

    // load prediction u and v (8 bits per pixel)
       VLD32WL           vr06,[i3, 0]                                                      // prediction0 ppel [  248] // u
       VLD32WL           vr07,[i3, 4]                                                      // prediction1 ppel [  249] // u
       VLD32WL           vr08,[i3, 8]                                                      // prediction2 ppel [  250] // v
       VLD32WL           vr09,[i3, 12]                                                     // prediction3 ppel [  251] // v

    // unpack prediction from 16 bit lanes into 32 bit lanes
       VUP.255           vr06, vr06                                                 // prediction0 prediction0 [  254] 
       VUP.255           vr07, vr07                                                 // prediction1 prediction1 [  255] 
       VUP.255           vr08, vr08                                                 // prediction2 prediction2 [  256] 
       VUP.255           vr09, vr09                                                 // prediction3 prediction3 [  257] 

    // calculate residual i.e. the differences for 1 row of u and v (9 bits
    // stored in 32 bits per pixel)
       VSUB              vr10, vr02, vr06                                         // ssd0 current0 prediction0 [  261] 
       VSUB              vr11, vr03, vr07                                         // ssd1 current1 prediction1 [  262] 
       VSUB              vr12, vr04, vr08                                         // ssd2 current2 prediction2 [  263] 
       VSUB              vr13, vr05, vr09                                         // ssd3 current3 prediction3 [  264] 

    // calculate sum of square differences for for 1 row of u and v (18 bits)
       VMUL              vr10, vr10, vr10                                                    // ssd0 ssd0 ssd0 [  267] 
       VMUL              vr11, vr11, vr11                                                    // ssd1 ssd1 ssd1 [  268] 
       VMUL              vr12, vr12, vr12                                                    // ssd2 ssd2 ssd2 [  269] 
       VMUL              vr13, vr13, vr13                                                    // ssd3 ssd3 ssd3 [  270] 

    // sum 4x4 ssds vertically
       VADD              vr14, vr14, vr10                                                  // cost0 cost0 ssd0 [  273] 
       VADD              vr15, vr15, vr11                                                  // cost1 cost1 ssd1 [  274] 
       VADD              vr16, vr16, vr12                                                  // cost2 cost2 ssd2 [  275] 

    // decrement and loop
       VJD.i7            i7, r4                                              // innerRow innerRow innerRowLoop [  278] 

    // delay slots start
       VADD              vr17, vr17, vr13  //DS______________________________________________ cost3 cost3 ssd3 [  281] 

    // increament pointers
       VIM               i2, i2, PCB_CURRENT_STRIDE  //DS___________________________________________ cpel cpel [  284] 
       VIM               i3, i3, PCB_REFERENCE_STRIDE  //DS_________________________________________ ppel ppel [  285] 
    // delay slots end


    // sum 4x4 ssds horizontally
       VXSUM.3           vr14, vr14, 0xff                                                       // cost0 cost0 [  290] 
       VXSUM.3           vr15, vr15, 0xff                                                       // cost1 cost1 [  291] 
       VXSUM.3           vr16, vr16, 0xff                                                       // cost2 cost2 [  292] 
       VXSUM.3           vr17, vr17, 0xff                                                       // cost3 cost3 [  293] 

    // find max cost of 4x4 ssd and save
       VMAX.3            vr00, vr00, vr14               // WorstSkippedChromaCost WorstSkippedChromaCost cost0 [  296] 

    // decrement and loop
       VJD.i8            i8, r3                                              // outerRow outerRow outerRowLoop [  299] 

    // delay slots start
       VMAX.3            vr00, vr00, vr15  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost1 [  302] 
       VMAX.3            vr00, vr00, vr16  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost2 [  303] 
       VMAX.3            vr00, vr00, vr17  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost3 [  304] 
    // delay slots end


    // return value in i0 & i1 = WorstSkippedChromaCost

    // signal end of simd code
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]

    // channels can only return 16 bit scalars
       VSEND             r0, i0, 0                                                // WorstSkippedChromaCostLow [  315] 
       VSEND             r0, i1, 1                                               // WorstSkippedChromaCostHigh [  316] 

   mov                 r1, Service_CalculateSkippedChromaCost
       VSEND             r0, r1, 63                                                                         // [  319] 
// End                                                                                                      // [  320] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz CalculateSkippedChromaCost_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CalculateSkippedChromaCost                                                                       // [  322] 




//------------------------------------------------------------------------------
// Name:          ReconstructLuma16X16
// Purpose:       Computes the reconstructed luma block by adding the prediction
//                data to the prediction block, clipping to [0:255] then storing 
//                the result in the reconstructed data buffer in SDM
// Arguments:     ea     = residual block address in SDM
//                pa     = prediction address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructLuma16X16                                                                             // [  335] 
_Asm int Record_ReconstructLuma16X16(int *mac_tab)
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
ReconstructLuma16X16_RecLoop:
  ld r1,[r17 ,12]
  vrec r1

// p16    i0  ea                                                                                            // [  336] 
// p16    i1  pa                                                                                            // [  337] 
// p16    i2  ra                                                                                            // [  338] 
// Begin                                                                                                    // [  339] 
// p16    i3  lpCnt                                                                                         // [  340] // loop count

//vec16  vr02  errLane0                                                                                     // [  342] 
//vec16  vr03  errLane1                                                                                     // [  343] 
//vec16  vr04  errLane2                                                                                     // [  344] 
//vec16  vr05  errLane3                                                                                     // [  345] 

//vec16  vr06  predLane0                                                                                    // [  347] 
//vec16  vr07  predLane1                                                                                    // [  348] 
//vec16  vr08  predLane2                                                                                    // [  349] 
//vec16  vr09  predLane3                                                                                    // [  350] 
    
    // Initialise the loop count
    // The luma MB (16x16) is divided into 8 slices of 2 rows each.
    // One row is processed per iteration
LINE 355".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 8-1, 8                                                                 // lpCnt [  355] 
    
//LpStart --------                                                                                          // [  357] 
 vendrec r3                                                                                                 // [  357] 
 vrec r3                                                                                                    // [  357] 
LINE 358".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*16]                                                        // errLane0 ea [  358] 
       VLD128            vr03,[i0, 1*16]                                                        // errLane1 ea [  359] 
       VLD128            vr04,[i0, 2*16]                                                        // errLane2 ea [  360] 
       VLD128            vr05,[i0, 3*16]                                                        // errLane3 ea [  361] 

       VLD64W            vr06,[i1, 0*8]                                                        // predLane0 pa [  363] 
       VLD64W            vr07,[i1, 1*8]                                                        // predLane1 pa [  364] 
       VLD64W            vr08,[i1, 2*8]                                                        // predLane2 pa [  365] 
       VLD64W            vr09,[i1, 3*8]                                                        // predLane3 pa [  366] 
    
       VADDW             vr02, vr02, vr06                                       // errLane0 errLane0 predLane0 [  368] 
       VADDW             vr03, vr03, vr07                                       // errLane1 errLane1 predLane1 [  369] 
       VADDW             vr04, vr04, vr08                                       // errLane2 errLane2 predLane2 [  370] 
       VADDW             vr05, vr05, vr09                                       // errLane3 errLane3 predLane3 [  371] 
    
       VASRPWB           vr02, vr02, 0                                                    // errLane0 errLane0 [  373] 
       VASRPWB           vr03, vr03, 0                                                    // errLane1 errLane1 [  374] 
       VASRPWB           vr04, vr04, 0                                                    // errLane2 errLane2 [  375] 
       VASRPWB           vr05, vr05, 0                                                    // errLane3 errLane3 [  376] 
    
       VST64             vr02,[i2, 0*8]                                                         // errLane0 ra [  378] 
       VST64             vr03,[i2, 1*8]                                                         // errLane1 ra [  379] 
       VST64             vr04,[i2, 2*8]                                                         // errLane2 ra [  380] 
       VST64             vr05,[i2, 3*8]                                                         // errLane3 ra [  381] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [  386] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ ea ea [  387] 
       VIM               i1, i1, 32  //DS_______________________________________________________________ pa pa [  388] 
       VIM               i2, i2, 32  //DS_______________________________________________________________ ra ra [  389] 
    
    
// End                                                                                                      // [  392] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructLuma16X16_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructLuma16X16                                                                             // [  394] 



//------------------------------------------------------------------------------
// Name:          ReconstructChroma
// Purpose:       Computes the reconstructed chroma block by adding the prediction
//                data to the prediction block, clipping to [0:255] then storing 
//                the result in the reconstructed data buffer in SDM
// Arguments:     ea     = residual block address in SDM
//                pa     = prediction address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructChroma                                                                                // [  406] 
_Asm int Record_ReconstructChroma(int *mac_tab)
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
ReconstructChroma_RecLoop:
  ld r1,[r17 ,16]
  vrec r1

// p16    i0  ea                                                                                            // [  407] 
// p16    i1  pa                                                                                            // [  408] 
// p16    i2  ra                                                                                            // [  409] 
// Begin                                                                                                    // [  410] 
// p16    i3  lpCnt                                                                                         // [  411] // loop count

//vec16  vr02  errLane0                                                                                     // [  413] 
//vec16  vr03  errLane1                                                                                     // [  414] 
//vec16  vr04  errLane2                                                                                     // [  415] 
//vec16  vr05  errLane3                                                                                     // [  416] 

//vec16  vr06  predLane0                                                                                    // [  418] 
//vec16  vr07  predLane1                                                                                    // [  419] 
//vec16  vr08  predLane2                                                                                    // [  420] 
//vec16  vr09  predLane3                                                                                    // [  421] 
    
    // Initialise the loop count
    // The chroma block (8x8) is divided into 2 slices of 4 rows each.
    // One row is processed per iteration
LINE 426".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 2-1, 8                                                                 // lpCnt [  426] 
    
//LpStart --------                                                                                          // [  428] 
 vendrec r3                                                                                                 // [  428] 
 vrec r3                                                                                                    // [  428] 
LINE 429".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*32]                                                        // errLane0 ea [  429] 
       VLD128            vr03,[i0, 1*32]                                                        // errLane1 ea [  430] 
       VLD128            vr04,[i0, 2*32]                                                        // errLane2 ea [  431] 
       VLD128            vr05,[i0, 3*32]                                                        // errLane3 ea [  432] 

       VLD64W            vr06,[i1, 0*16]                                                       // predLane0 pa [  434] 
       VLD64W            vr07,[i1, 1*16]                                                       // predLane1 pa [  435] 
       VLD64W            vr08,[i1, 2*16]                                                       // predLane2 pa [  436] 
       VLD64W            vr09,[i1, 3*16]                                                       // predLane3 pa [  437] 
    
       VADDW             vr02, vr02, vr06                                       // errLane0 errLane0 predLane0 [  439] 
       VADDW             vr03, vr03, vr07                                       // errLane1 errLane1 predLane1 [  440] 
       VADDW             vr04, vr04, vr08                                       // errLane2 errLane2 predLane2 [  441] 
       VADDW             vr05, vr05, vr09                                       // errLane3 errLane3 predLane3 [  442] 
    
       VASRPWB           vr02, vr02, 0                                                    // errLane0 errLane0 [  444] 
       VASRPWB           vr03, vr03, 0                                                    // errLane1 errLane1 [  445] 
       VASRPWB           vr04, vr04, 0                                                    // errLane2 errLane2 [  446] 
       VASRPWB           vr05, vr05, 0                                                    // errLane3 errLane3 [  447] 
    
       VST64             vr02,[i2, 0*16]                                                        // errLane0 ra [  449] 
       VST64             vr03,[i2, 1*16]                                                        // errLane1 ra [  450] 
       VST64             vr04,[i2, 2*16]                                                        // errLane2 ra [  451] 
       VST64             vr05,[i2, 3*16]                                                        // errLane3 ra [  452] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [  457] 
       VIM               i0, i0, 128  //DS______________________________________________________________ ea ea [  458] 
       VIM               i1, i1, 64  //DS_______________________________________________________________ pa pa [  459] 
       VIM               i2, i2, 64  //DS_______________________________________________________________ ra ra [  460] 
    
    
// End                                                                                                      // [  463] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructChroma_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructChroma                                                                                // [  465] 



//------------------------------------------------------------------------------
// Name:          ReconstructIntra
// Purpose:       Computes the reconstructed Intra luma + chroma blocks by 
//                clipping to [0:255] then storing the result in the 
//                reconstructed data buffer in SDM
// Arguments:     ea     = residual block address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructIntra                                                                                 // [  476] 
_Asm int Record_ReconstructIntra(int *mac_tab)
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
ReconstructIntra_RecLoop:
  ld r1,[r17 ,20]
  vrec r1

// p16    i0  ea                                                                                            // [  477] 
// p16    i1  ra                                                                                            // [  478] 
// Begin                                                                                                    // [  479] 
// p16    i2  lpCnt                                                                                         // [  480] // loop count

//vec16  vr02  errLane0                                                                                     // [  482] 
//vec16  vr03  errLane1                                                                                     // [  483] 
//vec16  vr04  errLane2                                                                                     // [  484] 
//vec16  vr05  errLane3                                                                                     // [  485] 
    
    // Initialise the loop count
    // The luma + chroma MB (16x24) is divided into 12 slices of 2 rows each.
    // One row is processed per iteration
LINE 490".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 12-1, 4                                                                // lpCnt [  490] 
    
//LpStart --------                                                                                          // [  492] 
 vendrec r3                                                                                                 // [  492] 
 vrec r3                                                                                                    // [  492] 
LINE 493".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*16]                                                        // errLane0 ea [  493] 
       VLD128            vr03,[i0, 1*16]                                                        // errLane1 ea [  494] 
       VLD128            vr04,[i0, 2*16]                                                        // errLane2 ea [  495] 
       VLD128            vr05,[i0, 3*16]                                                        // errLane3 ea [  496] 
    
       VASRPWB           vr02, vr02, 0                                                    // errLane0 errLane0 [  498] 
       VASRPWB           vr03, vr03, 0                                                    // errLane1 errLane1 [  499] 
       VASRPWB           vr04, vr04, 0                                                    // errLane2 errLane2 [  500] 
       VASRPWB           vr05, vr05, 0                                                    // errLane3 errLane3 [  501] 
    
       VST64             vr02,[i1, 0*8]                                                         // errLane0 ra [  503] 
       VST64             vr03,[i1, 1*8]                                                         // errLane1 ra [  504] 
       VST64             vr04,[i1, 2*8]                                                         // errLane2 ra [  505] 
       VST64             vr05,[i1, 3*8]                                                         // errLane3 ra [  506] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i2            i2, r3                                                         // lpCnt lpCnt LpStart [  511] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ ea ea [  512] 
       VIM               i1, i1, 32  //DS_______________________________________________________________ ra ra [  513] 
       VNOP                //DS_______________________________________________________________________________ [  514] 
    
// End                                                                                                      // [  516] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructIntra_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructIntra                                                                                 // [  518] 



//------------------------------------------------------------------------------
// Name:          ReconstructSkipped
// Purpose:       Copies the skipped prediction into the 
//                reconstructed data buffer in SDM
// Arguments:     pa     = prediction block address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructSkipped                                                                               // [  528] 
_Asm int Record_ReconstructSkipped(int *mac_tab)
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
ReconstructSkipped_RecLoop:
  ld r1,[r17 ,24]
  vrec r1

// p16    i0  pa                                                                                            // [  529] 
// p16    i1  ra                                                                                            // [  530] 
// Begin                                                                                                    // [  531] 
// p16    i2  lpCnt                                                                                         // [  532] // loop count

//vec16  vr02  predLane0                                                                                    // [  534] 
//vec16  vr03  predLane1                                                                                    // [  535] 
//vec16  vr04  predLane2                                                                                    // [  536] 
//vec16  vr05  predLane3                                                                                    // [  537] 
    
    // Initialise the loop count
    // The luma + chroma MB (16x24) is divided into 6 slices of 4 rows each.
    // One row is processed per iteration
LINE 542".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 6-1, 4                                                                 // lpCnt [  542] 
    
//LpStart --------                                                                                          // [  544] 
 vendrec r3                                                                                                 // [  544] 
 vrec r3                                                                                                    // [  544] 
LINE 545".\ARCEncoder\MPEG4\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*16]                                                       // predLane0 pa [  545] 
       VLD128            vr03,[i0, 1*16]                                                       // predLane1 pa [  546] 
       VLD128            vr04,[i0, 2*16]                                                       // predLane2 pa [  547] 
       VLD128            vr05,[i0, 3*16]                                                       // predLane3 pa [  548] 
    
       VST128            vr02,[i1, 0*16]                                                       // predLane0 ra [  550] 
       VST128            vr03,[i1, 1*16]                                                       // predLane1 ra [  551] 
       VST128            vr04,[i1, 2*16]                                                       // predLane2 ra [  552] 
       VST128            vr05,[i1, 3*16]                                                       // predLane3 ra [  553] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i2            i2, r3                                                         // lpCnt lpCnt LpStart [  558] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ pa pa [  559] 
       VIM               i1, i1, 64  //DS_______________________________________________________________ ra ra [  560] 
       VNOP                //DS_______________________________________________________________________________ [  561] 
    
// End                                                                                                      // [  563] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructSkipped_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructSkipped                                                                               // [  565] 




const int MACRO_IDX_MakeLumaPredictionError16X16 = 0;
const int MACRO_IDX_MakeChromaPredictionError = 1;
const int MACRO_IDX_CalculateSkippedChromaCost = 2;
const int MACRO_IDX_ReconstructLuma16X16 = 3;
const int MACRO_IDX_ReconstructChroma = 4;
const int MACRO_IDX_ReconstructIntra = 5;
const int MACRO_IDX_ReconstructSkipped = 6;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int EncoderAddSubtract_Record(int sa, int SIMDnum)
{
    int *map = EncoderAddSubtract;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_MakeLumaPredictionError16X16] = sa;
    sa = Record_MakeLumaPredictionError16X16(map);

    map[MACRO_IDX_MakeChromaPredictionError] = sa;
    sa = Record_MakeChromaPredictionError(map);

    map[MACRO_IDX_CalculateSkippedChromaCost] = sa;
    sa = Record_CalculateSkippedChromaCost(map);

    map[MACRO_IDX_ReconstructLuma16X16] = sa;
    sa = Record_ReconstructLuma16X16(map);

    map[MACRO_IDX_ReconstructChroma] = sa;
    sa = Record_ReconstructChroma(map);

    map[MACRO_IDX_ReconstructIntra] = sa;
    sa = Record_ReconstructIntra(map);

    map[MACRO_IDX_ReconstructSkipped] = sa;
    sa = Record_ReconstructSkipped(map);

    return sa;
}
