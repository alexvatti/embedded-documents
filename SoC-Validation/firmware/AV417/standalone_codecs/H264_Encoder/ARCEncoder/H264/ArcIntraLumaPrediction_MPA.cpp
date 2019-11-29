//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss'

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
// ARC International.                                              //





#include "ArcSDMTables.h"
#include "ArcMPC.h"
#include "defines.h"


// Stall info ON                                                      // [   25] 
// Stall info format: Stall [Reg in Stall] ...... List of Registers currently busy  

// MetaWare ouput ON                                                                                        // [   27] 
#define LINE #line                                                                                          // [   27] 

// ScanMP: Including file .\ARCEncoder\H264\..\ARC\SIMD_ABI.ii.
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

#include ".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.h"


// Macro table
    int IntraLumaPrediction[6];
    int IntraLumaPrediction_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Global Function Arguments
// p16    i0  intraValidFlag                                                                                // [   38] ....................// same as chroma
// p16    i1  intra_16x16_mode                                                                              // [   40] 
// p16    i2  intraLumaCost                                                                                 // [   41] 
//                  useWhitening // see declaration below

//------------------------------------------------------------------------------
// Intra Prediction Common Register Workspace
//vec16  vr02  gDCArray0                                                                                    // [   46] 
//vec16  vr03  gDCArray1                                                                                    // [   47] 
// hack column into definiton until .$NAME functionality added to scanMP
// these will go into vr04
// s16    vr04.1  gThreshold                                                                                // [   50] 
// s16    vr04.2  gLambda                                                                                   // [   51] 
// s16    vr04.4  gLambdaMult16                                                                             // [   52] 
// s16    vr04.8  gLambdaMult10_30                                                                          // [   53] 
// s16    vr04.16  gBestIntraLumaCost                                                                       // [   54] 

// resid_sad's will be located before gBestIntraLumaMode. this only works because gBestIntraLumaMode is
// defined as in the same lane as gBestIntraLumaCost which is not in the same
// lane as any of the resid_sad's below which are defined and only used in Calculate4x4SADs

//    // place these first, so that they will all get put in the same vector, as
//    // scanMP doesn't support a command to do this at presents
//    // hack column into instruction until .$NAME functionality added to scanMP 
//    s16                 resid_sad0 : i0
//    s16                 resid_sad1 : i1
//    s16                 resid_sad2 : i2
//    s16                 resid_sad3 : i3

// s16    vr05.4  gUseIntraLumaDCPredOnly                                                                   // [   68] ....................// force to not be in vr04 to avoid stalls
// s16    vr05.8  gUseWhitening                                                                             // [   70] 
// s16    vr05.16  gBestIntraLumaMode                                                                       // [   72] 


// only used by GenerateIntra16x16Prediction but define globally to stop
// it getting overwritten by subroutines
//vec16  vr06  dc_prediction                                                                                // [   77] 



//------------------------------------------------------------------------------
// Name:          ConstrainIntraValidFlag
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ConstrainIntraValidFlag                                                                          // [   87] 
_Asm int Record_ConstrainIntraValidFlag(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,0]
  vrec r1

    // function arguments
    //p16                 intraValidFlag = i0 // defined as global for luma and chroma
// p16    i8  constIntraPred                                                                                // [   90] ....................// constrained_intra_pred_flag

// Begin                                                                                                    // [   93] 
// p16    i3  leftMBIntra                                                                                   // [   94] 
// p16    i7  aboveMBIntra                                                                                  // [   95] 

    // reset neighbour flags to 1 because we only care about the intra flags if
    // constrained_intra_pred_flag is set
LINE 99".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VMOVW             vr00, 1, 8                                                             // leftMBIntra [   99] 
       VMOVW             vr00, 1, 128                                                          // aboveMBIntra [  100] 

    // only load the neighbour flags if constrained intra prediction flag is set
       VLD8W_3.i8        vr00,[i4, MPO_LeftMB]                            // constIntraPred leftMBIntra CirBuf [  103] 
       VLD8W_7.i8        vr00,[i4, MPO_AboveMB]                          // constIntraPred aboveMBIntra CirBuf [  104] 

    // check if neighbour flags are intra
              //  Stall: [i3]  .................................      vr00.136
              //  Stall: [i3]  .................................      vr00.136
       VIA.i8            i3, i3, MBNF_INTRA                          // constIntraPred leftMBIntra leftMBIntra [  107] 
       VIA.i8            i7, i7, MBNF_INTRA                        // constIntraPred aboveMBIntra aboveMBIntra [  108] 

    // clear neighbour bit in intra valid flag if constrained intra pred and
    // intra are not set
       VIA!i3            i0, i0, 2                                // leftMBIntra intraValidFlag intraValidFlag [  112] ....................// zero (clear) left bit (bit 0)
              //  Stall: [i0]  .................................      I00  
       VIA!i7            i0, i0, 1                               // aboveMBIntra intraValidFlag intraValidFlag [  113] ....................// zero (clear) above bit (bit 1)

// End                                                                                                      // [  115] 


  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ConstrainIntraValidFlag                                                                          // [  118] 




//------------------------------------------------------------------------------
// Name:          CalculateDC
// Purpose:       Calculates the DC value for each 4x4 sub-block of a macroblock
// Arguments:     prediction_errorPtr = pointer to residual macroblock
//                dcarrayPtr = pointer to 16 element array to store dc value
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] CalculateDC                                                                                 // [  128] 
_Asm int Record_CalculateDC(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,4]
  vrec r1

    // function arguments
// p16    i3  prediction_errorPtr                                                                           // [  130] 

// Begin                                                                                                    // [  132] 
    // res_Idx0_1 = effects both lanes 0 & 1 (this allows it to be used for the
    //             divide by 16 at the end, knowing that res_Idx1 will overwrite
    //             any values from the sumation
    // res_Idx1 = only effects lane 1
//vec16  k8  res.3                                                                                          // [  137] 
//vec16  i8  res_Idx0_1.1                                                                                   // [  137] 
//vec16  i9  res_Idx1.2                                                                                     // [  137] 

//vec16  vr07  prediction_error0                                                                            // [  139] 
//vec16  vr08  prediction_error1                                                                            // [  140] 
//vec16  vr09  prediction_error2                                                                            // [  141] 
//vec16  vr10  prediction_error3                                                                            // [  142] 
//vec16  vr11  prediction_error4                                                                            // [  143] 
//vec16  vr12  prediction_error5                                                                            // [  144] 
//vec16  vr13  prediction_error6                                                                            // [  145] 
//vec16  vr14  prediction_error7                                                                            // [  146] 
//vec16  vr15  dcarrayTemp0                                                                                 // [  147] 
//vec16  vr16  dcarrayTemp1                                                                                 // [  148] 

// p16    i7  row                                                                                           // [  150] 

    // setup loop counter
    // 16 rows total, 4 rows per loop
LINE 154".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VMOVW             vr00, (16/4)-1, 128                                                            // row [  154] 

    // setup result indicies
       VMOVW             vr01, 3, 1                                                              // res_Idx0_1 [  157] ....................// lane 0 & 1
       VMOVW             vr01, 2, 2                                                                // res_Idx1 [  158] ....................// lane 1

    // rowLoop start
//rowLoop --------                                                                                          // [  161] 
 vendrec r3                                                                                                 // [  161] 
 vrec r3                                                                                                    // [  161] 
    
    // load prediction data for a 4 rows of 4x4 sub-blocks
    // 4x4 sub-blocks 0 & 1
LINE 165".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD128            vr07,[i3, 0]                                 // prediction_error0 prediction_errorPtr [  165] 
       VLD128            vr08,[i3, 32]                                // prediction_error1 prediction_errorPtr [  166] 
       VLD128            vr09,[i3, 64]                                // prediction_error2 prediction_errorPtr [  167] 
       VLD128            vr10,[i3, 96]                                // prediction_error3 prediction_errorPtr [  168] 

    // 4x4 sub-blocks 2 & 3
       VLD128            vr11,[i3, 16]                                // prediction_error4 prediction_errorPtr [  171] 
       VLD128            vr12,[i3, 48]                                // prediction_error5 prediction_errorPtr [  172] 
       VLD128            vr13,[i3, 80]                                // prediction_error6 prediction_errorPtr [  173] 
       VLD128            vr14,[i3, 112]                               // prediction_error7 prediction_errorPtr [  174] 


    // add rows to each other, resetting the accumulator on the first one
    // 4x4 sub-blocks 0 & 1
       VADDW             vr15, vr07, vr08                  // dcarrayTemp0 prediction_error0 prediction_error1 [  179] 
       VADDAW            vr15, vr09, vr10                  // dcarrayTemp0 prediction_error2 prediction_error3 [  180] 

    // 4x4 sub-blocks 2 & 3
       VADDW             vr16, vr11, vr12                  // dcarrayTemp1 prediction_error4 prediction_error5 [  183] 
       VADDAW            vr16, vr13, vr14                  // dcarrayTemp1 prediction_error6 prediction_error7 [  184] 


    // increament pointer (16 pixels, 2 bytes per pixel, 4 rows)
       VIM               i3, i3, 16*2*4                             // prediction_errorPtr prediction_errorPtr [  188] 


    // sum 4 columns of sub-block into result
    // 4x4 sub-blocks 0
       VXSUMW.i8         vr02, vr15, 0x0f                                 // res_Idx0_1 gDCArray0 dcarrayTemp0 [  193] 

    // 4x4 sub-blocks 1
       VXSUMW.i9         vr02, vr15, 0xf0                                   // res_Idx1 gDCArray0 dcarrayTemp0 [  196] 


    // decrement and loop
       VJD.i7            i7, r3                                                             // row row rowLoop [  200] 

    // delay slots start

    // 4x4 sub-blocks 2
       VXSUMW.i8         vr03, vr16, 0x0f  //DS_____________________________ res_Idx0_1 gDCArray1 dcarrayTemp1 [  205] 

    // 4x4 sub-blocks 3
       VXSUMW.i9         vr03, vr16, 0xf0  //DS_______________________________ res_Idx1 gDCArray1 dcarrayTemp1 [  208] 


    // increament dcarray result indicies (shift left by 2 because we process 4
    // rows and 4 columns at a time)
       VMULW.3           vr01, vr01, 4  //DS__________________________________________________________ res res [  213] 

    // delay slots end


    // for 4x4 sub-blocks 0 & 1, 4 & 5, 8 & 9, 12 & 13 (raster order)
    // store the sign bit
       VSUBW.F           vr62, vr02, 0                                                      // VZERO gDCArray0 [  220] 

    // negate values that are negative
   mov                 r0, 0xffff
              //  Stall: [VFLAGS]   ............................      vr62          vr63    
       VMULW.LT          vr02, vr02, r0                                                 // gDCArray0 gDCArray0 [  224] 

    // generate 4x4 sub-block dc value by dividing the sum by 16 with rounding
              //  Stall: [vr02]  ...............................      vr02          vr63    
              //  Stall: [vr02]  ...............................      vr02          vr63    
       VASRRW            vr02, vr02, 4                                                  // gDCArray0 gDCArray0 [  227] 

    // negate values that were previously negative at the start
   mov                 r0, 0xffff
              //  Stall: [vr02]  ...............................      vr02    
              //  Stall: [vr02]  ...............................      vr02    
       VMULW.LT          vr02, vr02, r0                                                 // gDCArray0 gDCArray0 [  231] 

    // for 4x4 sub-blocks 2 & 3, 6 & 7, 10 & 11, 14 & 15 (raster order)
    // store the sign bit
       VSUBW.F           vr62, vr03, 0                                                      // VZERO gDCArray1 [  235] 


    // return to caller
       VJB               vr31, 0                                                                     // VSTACK [  239] 

    // delay slots start

    // negate values that are negative
       VMULW.LT          vr03, vr03, r0  //DS_____________________________________________ gDCArray1 gDCArray1 [  245] 

    // generate 4x4 sub-block dc value by dividing the sum by 16 with rounding
              //  Stall: [vr03]  ...............................      vr03          vr63    
              //  Stall: [vr03]  ...............................      vr03          vr63    
       VASRRW            vr03, vr03, 4  //DS______________________________________________ gDCArray1 gDCArray1 [  248] 

    // negate values that were previously negative at the start
              //  Stall: [vr03]  ...............................      vr03    
              //  Stall: [vr03]  ...............................      vr03    
       VMULW.LT          vr03, vr03, r0  //DS_____________________________________________ gDCArray1 gDCArray1 [  252] 


    // store the results
    // gDCArray0 contains the dc values for the rater numbered 4x4 sub-blocks 0, 1, 4, 5, 8, 9, 12 & 13
    // gDCArray1 contains the dc values for the rater numbered 4x4 sub-blocks 2, 3, 6, 7, 10, 11, 14 & 15
    //vst128              gDCArray0, dcarrayPtr, 0
    //vst128              gDCArray1, dcarrayPtr, 16

    // delay slots end

// End                                                                                                      // [  263] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CalculateDC                                                                                      // [  265] 




//------------------------------------------------------------------------------
// Name:          Calculate4x4SADs
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] Calculate4x4SADs                                                                            // [  274] 
_Asm int Record_Calculate4x4SADs(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,8]
  vrec r1

    // function arguments
// p16    i3  residualPtr                                                                                   // [  276] 
// p16    i7  threshold                                                                                     // [  277] 

// Begin                                                                                                    // [  279] 
    // return result
// ****** Informational : Register space for symbol "sad" already allocated.                                // [  281] ....................// this is used as a synonym for intraLumaCost
// p16    i2  sad                                                                                           // [  281] 

    // place these first, so that they will all get put in the same vector, as
    // scanMP doesn't support a command to do this at presents
    // hack column into instruction until .$NAME functionality added to scanMP 
    //s16                 resid_sad0 : i0
    //s16                 resid_sad1 : i1
    //s16                 resid_sad2 : i2
    //s16                 resid_sad3 : i3

//vec16  vr07  resid.15                                                                                     // [  291] 
//vec16  vr07  resid_sad0.1                                                                                 // [  291] 
//vec16  vr07  resid_sad1.2                                                                                 // [  291] 
//vec16  vr07  resid_sad2.4                                                                                 // [  291] 
//vec16  vr07  resid_sad3.8                                                                                 // [  291] 

//vec16  vr08  coded.15                                                                                     // [  293] 
//vec16  vr08  coded_flag0.1                                                                                // [  293] 
//vec16  vr08  coded_flag1.2                                                                                // [  293] 
//vec16  vr08  coded_flag2.4                                                                                // [  293] 
//vec16  vr08  coded_flag3.8                                                                                // [  293] 

// p16    i8  codedFlag                                                                                     // [  295] 
// p16    i9  lambdaMult16                                                                                  // [  296] 
// p16    i10  sum                                                                                          // [  297] 

// p16    i11  row                                                                                          // [  299] 

//vec16  vr09  residual0                                                                                    // [  301] 
//vec16  vr10  residual1                                                                                    // [  302] 
//vec16  vr11  residual2                                                                                    // [  303] 
//vec16  vr12  residual3                                                                                    // [  304] 
//vec16  vr13  residual4                                                                                    // [  305] 
//vec16  vr14  residual5                                                                                    // [  306] 
//vec16  vr15  residual6                                                                                    // [  307] 
//vec16  vr16  residual7                                                                                    // [  308] 

//vec16  vr17  resid_sadTemp0                                                                               // [  310] 
//vec16  vr18  resid_sadTemp1                                                                               // [  311] 

//vec16  vr19  resid_sadSum                                                                                 // [  313] 


    // setup defaults
    // hack column into instruction until .$NAME functionality added to scanMP 
LINE 318".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIV.4             i9, vr04                                                // lambdaMult16 gLambdaMult16 [  318] 
       VMOVW             vr08, 0, 15                                                                  // coded [  319] 
       VMOVW             vr00, 0, 4                                                                     // sad [  320] 
       VMOVW             vr19, 0, 255                                                          // resid_sadSum [  321] 

    // setup loop counter
    // 16 rows total, 4 rows per loop
       VMOVW             vr01, (16/4)-1, 8                                                              // row [  325] 


    // rowLoop start
//rowLoop --------                                                                                          // [  329] 
 vendrec r3                                                                                                 // [  329] 
 vrec r3                                                                                                    // [  329] 

    // load prediction data for a row of 4x4 sub-blocks
LINE 332".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD128            vr09,[i3, 0]                                                 // residual0 residualPtr [  332] 
       VLD128            vr10,[i3, 32]                                                // residual1 residualPtr [  333] 
       VLD128            vr11,[i3, 64]                                                // residual2 residualPtr [  334] 
       VLD128            vr12,[i3, 96]                                                // residual3 residualPtr [  335] 
       VLD128            vr13,[i3, 16]                                                // residual4 residualPtr [  336] 
       VLD128            vr14,[i3, 48]                                                // residual5 residualPtr [  337] 
       VLD128            vr15,[i3, 80]                                                // residual6 residualPtr [  338] 
       VLD128            vr16,[i3, 112]                                               // residual7 residualPtr [  339] 
    
    // get absolute values
       VABSW             vr09, vr09                                                     // residual0 residual0 [  342] 
       VABSW             vr10, vr10                                                     // residual1 residual1 [  343] 
       VABSW             vr11, vr11                                                     // residual2 residual2 [  344] 
       VABSW             vr12, vr12                                                     // residual3 residual3 [  345] 
       VABSW             vr13, vr13                                                     // residual4 residual4 [  346] 
       VABSW             vr14, vr14                                                     // residual5 residual5 [  347] 
       VABSW             vr15, vr15                                                     // residual6 residual6 [  348] 
       VABSW             vr16, vr16                                                     // residual7 residual7 [  349] 

    // calculate 4x4 sads by;
    // first, adding rows to each other, resetting the accumulator on the first
    // one
    // 4x4 sub-blocks 0 & 1
       VADDW             vr17, vr09, vr10                                // resid_sadTemp0 residual0 residual1 [  355] 
       VADDAW            vr17, vr11, vr12                                // resid_sadTemp0 residual2 residual3 [  356] 

    // 4x4 sub-blocks 2 & 3
       VADDW             vr18, vr13, vr14                                // resid_sadTemp1 residual4 residual5 [  359] 
       VADDAW            vr18, vr15, vr16                                // resid_sadTemp1 residual6 residual7 [  360] 

    // second, summing 4 columns of sub-block into result
    // 4x4 sub-blocks 0
              //  Stall: [vr17]  ...............................      vr17          vr18    
       VXSUMW.1          vr07, vr17, 0x0f                                         // resid_sad0 resid_sadTemp0 [  364] 

    // 4x4 sub-blocks 1
       VXSUMW.2          vr07, vr17, 0xf0                                         // resid_sad1 resid_sadTemp0 [  367] 

    // 4x4 sub-blocks 2
       VXSUMW.4          vr07, vr18, 0x0f                                         // resid_sad2 resid_sadTemp1 [  370] 

    // 4x4 sub-blocks 3
       VXSUMW.8          vr07, vr18, 0xf0                                         // resid_sad3 resid_sadTemp1 [  373] 

    // compare sads for all 4x4 subblocks 0, 1, 2 & 3 with threshold
    // need to explicitly set lane predication until scanMP is fixed
              //  Stall: [vr07]  ...............................      vr07.012
              //  Stall: [vr07]  ...............................      vr07.008
       VSUBW.15.F        vr62, vr07, i7                                               // VZERO resid threshold [  377] 


    // decrement and loop
       VJD.i11           i11, r3                                                            // row row rowLoop [  381] 

    // delay slots start

    // if any are greater than the threshold then set coded
//    vtany.0x0f.gt       coded, lambdaMult16
       VMIVW.15.GT       vr08, i9  //DS____________________________________________________ coded lambdaMult16 [  387] 

    // accumulate the 4x4 sub-block sads, assumes resid_sad0, 1, 2 & 3 are all
    // in the same vector next to each other
    // need to explicitly set lane predication until scanMP is fixed
       VADDW.15          vr19, vr19, vr07  //DS_______________________________ resid_sadSum resid_sadSum resid [  392] 

    // increament pointer (16 pixels, 2 bytes per pixel, 4 rows)
       VIM               i3, i3, 16*2*4  //DS_________________________________________ residualPtr residualPtr [  395] 

    // delay slots end


    // return result in i2

    // return to caller
       VJB               vr31, 0                                                                     // VSTACK [  403] 

    // delay slots start

    // calculate final sad by summing each row of 4x4 sub-blocks
    // hack column into instruction until .$NAME functionality added to scanMP 
       VXSUMW.4          vr00, vr19, 0x0f  //DS______________________________________________ sad resid_sadSum [  409] 

    // adjust sad if 4x4 dc sub-block will be coded
       VIV.15            i8, vr08  //DS_______________________________________________________ codedFlag coded [  412] 
              //  Stall: [vr00]  ...............................      vr00.004      I08  
       VADDW.4           vr00, vr00, i8  //DS_______________________________________________ sad sad codedFlag [  413] 

    // delay slots end

// End                                                                                                      // [  417] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Calculate4x4SADs                                                                                 // [  419] 




//------------------------------------------------------------------------------
// Name:          WhitenResidual_CalculateSAD
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] WhitenResidual_CalculateSAD                                                                 // [  428] 
_Asm int Record_WhitenResidual_CalculateSAD(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,12]
  vrec r1

    // function arguments

// Begin                                                                                                    // [  431] 
    // need to hack explicit lane number as ScanMP does not yet support defining
    // p16's to be in the same lane as another p16
// p16    i9  intra_16x16_modeOffset                                                                        // [  434] ....................// intra_16x16_mode = i1

    // call CalculateDC
  ld r1,[r17,4];                                                                         // VSTACK CalculateDC [  437] 
LINE 437".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [  437] 

    // delay slots start

    // setup arguments for CalculateDC
       VMULW.2           vr01, vr00, (16*16*2)  //DS__________________ intra_16x16_modeOffset intra_16x16_mode [  442] ....................// 2 bytes per pixel
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VIM               i3, i9, SDMOF_IntraPredictionResidsLuma  //DS_________________ intra_16x16_modeOffset [  443] 

       VNOP                //DS_______________________________________________________________________________ [  445] 
    // delay slots end

    // results returned in gDCArray0 & gDCArray1

// Begin                                                                                                    // [  450] 
// p16    i3  dcIdx0                                                                                        // [  451] 
// p16    i7  dcIdx1                                                                                        // [  452] 

// p16    i8  prediction_errorPtr                                                                           // [  454] 
// p16    i10  whitePredErrorPtr                                                                            // [  455] 

// p16    i11  row                                                                                          // [  457] 

// p16    i12  dcTerm0                                                                                      // [  459] 
// p16    i13  dcTerm1                                                                                      // [  460] 
// p16    i14  dcTerm2                                                                                      // [  461] 
// p16    i15  dcTerm3                                                                                      // [  462] 

//vec16  vr07  prediction_error0                                                                            // [  464] 
//vec16  vr08  prediction_error1                                                                            // [  465] 
//vec16  vr09  prediction_error2                                                                            // [  466] 
//vec16  vr10  prediction_error3                                                                            // [  467] 
//vec16  vr11  prediction_error4                                                                            // [  468] 
//vec16  vr12  prediction_error5                                                                            // [  469] 
//vec16  vr13  prediction_error6                                                                            // [  470] 
//vec16  vr14  prediction_error7                                                                            // [  471] 

//vec16  vr15  dcVec0                                                                                       // [  473] 
//vec16  vr16  dcVec1                                                                                       // [  474] 

//vec16  vr17  whitePredError0                                                                              // [  476] 
//vec16  vr18  whitePredError1                                                                              // [  477] 
//vec16  vr19  whitePredError2                                                                              // [  478] 
//vec16  vr20  whitePredError3                                                                              // [  479] 
//vec16  vr21  whitePredError4                                                                              // [  480] 
//vec16  vr22  whitePredError5                                                                              // [  481] 
//vec16  vr23  whitePredError6                                                                              // [  482] 
//vec16  vr24  whitePredError7                                                                              // [  483] 


    // setup pointers
    // possible optimisation is to stiore intra_16x16_modeOffset in intra_16x16_mode, as 
    // intra_16x16_mode is possibly not needed
LINE 489".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VMULW.2           vr01, vr00, (16*16*2)                      // intra_16x16_modeOffset intra_16x16_mode [  489] ....................// 2 bytes per pixel
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VIM               i8, i9, SDMOF_IntraPredictionResidsLuma // prediction_errorPtr intra_16x16_modeOffset [  490] 

       VMOVW             vr01, SDMOF_WhitePredError, 4                                    // whitePredErrorPtr [  492] 

    // setup loop counter
    // 16 rows total, 4 rows per loop
    // we need to perform 4 rows at a time to match the dc indexing scheme for
    // the dc array that is generated in CalculateDC
       VMOVW             vr01, (16/4)-1, 8                                                              // row [  498] 

    // setup result indicies
       VMOVW             vr00, 1, 8                                                                  // dcIdx0 [  501] ....................// lane 0
       VMOVW             vr00, 2, 128                                                                // dcIdx1 [  502] ....................// lane 1


    // rowLoop start
//rowLoop --------                                                                                          // [  506] 
 vendrec r3                                                                                                 // [  506] 
 vrec r3                                                                                                    // [  506] 

    // load prediction data for a 4 rows of 4x4 sub-blocks 0, 1, 2 & 3
LINE 509".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD128            vr07,[i8, 0]                                 // prediction_error0 prediction_errorPtr [  509] 
       VLD128            vr08,[i8, 16]                                // prediction_error1 prediction_errorPtr [  510] 
       VLD128            vr09,[i8, 32]                                // prediction_error2 prediction_errorPtr [  511] 
       VLD128            vr10,[i8, 48]                                // prediction_error3 prediction_errorPtr [  512] 
       VLD128            vr11,[i8, 64]                                // prediction_error4 prediction_errorPtr [  513] 
       VLD128            vr12,[i8, 80]                                // prediction_error5 prediction_errorPtr [  514] 
       VLD128            vr13,[i8, 96]                                // prediction_error6 prediction_errorPtr [  515] 
       VLD128            vr14,[i8, 112]                               // prediction_error7 prediction_errorPtr [  516] 

    // get dc term for each row of 4x4 sub-blocks
       VIV.i3            i12, vr02                                                 // dcIdx0 dcTerm0 gDCArray0 [  519] 
       VIV.i7            i13, vr02                                                 // dcIdx1 dcTerm1 gDCArray0 [  520] 
       VIV.i3            i14, vr03                                                 // dcIdx0 dcTerm2 gDCArray1 [  521] 
       VIV.i7            i15, vr03                                                 // dcIdx1 dcTerm3 gDCArray1 [  522] 

    // broadcast dc term across all pixels of each row of each 4x4 sub-block
       VMIVW.15          vr15, i12                                                           // dcVec0 dcTerm0 [  525] 
       VMIVW.240         vr15, i13                                                           // dcVec0 dcTerm1 [  526] 
       VMIVW.15          vr16, i14                                                           // dcVec1 dcTerm2 [  527] 
       VMIVW.240         vr16, i15                                                           // dcVec1 dcTerm3 [  528] 

    // only update the dc term indicies every other loop, debcuase
    


    // we require 4 dc values from the gDCArray to process each row. however
    // we only process 2 rows every loop, therefore we only want to update them
    // every other loop. we do this by comparing the loop counter to even vaules
    

    // update dc term indicies by shifting left by 2 because we process 4 rows
    // and 4 columns at a time
       VMULW.136         vr00, vr00, 4                                          // dcIdx0'dcIdx1 dcIdx0'dcIdx1 [  541] 

    // perform whitening on residual
       VSUBW             vr17, vr07, vr15                          // whitePredError0 prediction_error0 dcVec0 [  544] 
       VSUBW             vr18, vr08, vr16                          // whitePredError1 prediction_error1 dcVec1 [  545] 
       VSUBW             vr19, vr09, vr15                          // whitePredError2 prediction_error2 dcVec0 [  546] 
       VSUBW             vr20, vr10, vr16                          // whitePredError3 prediction_error3 dcVec1 [  547] 
       VSUBW             vr21, vr11, vr15                          // whitePredError4 prediction_error4 dcVec0 [  548] 
       VSUBW             vr22, vr12, vr16                          // whitePredError5 prediction_error5 dcVec1 [  549] 
       VSUBW             vr23, vr13, vr15                          // whitePredError6 prediction_error6 dcVec0 [  550] 
       VSUBW             vr24, vr14, vr16                          // whitePredError7 prediction_error7 dcVec1 [  551] 

    // store whitened residual
       VST128            vr17,[i10, 0]                                    // whitePredError0 whitePredErrorPtr [  554] 
       VST128            vr18,[i10, 16]                                   // whitePredError1 whitePredErrorPtr [  555] 
       VST128            vr19,[i10, 32]                                   // whitePredError2 whitePredErrorPtr [  556] 
       VST128            vr20,[i10, 48]                                   // whitePredError3 whitePredErrorPtr [  557] 
       VST128            vr21,[i10, 64]                                   // whitePredError4 whitePredErrorPtr [  558] 
       VST128            vr22,[i10, 80]                                   // whitePredError5 whitePredErrorPtr [  559] 
       VST128            vr23,[i10, 96]                                   // whitePredError6 whitePredErrorPtr [  560] 


    // decrement and loop
       VJD.i11           i11, r3                                                            // row row rowLoop [  564] 

    // delay slots start

    // store whitened residual
       VST128            vr24,[i10, 112]  //DS______________________________ whitePredError7 whitePredErrorPtr [  569] 

    // increament pointers (16 pixels, 2 bytes per pixel, 4 rows)
       VIM               i10, i10, 16*2*4  //DS___________________________ whitePredErrorPtr whitePredErrorPtr [  572] 
       VIM               i8, i8, 16*2*4  //DS_________________________ prediction_errorPtr prediction_errorPtr [  573] 

    // delay slots end


// End                                                                                                      // [  578] 

// End                                                                                                      // [  580] 

// Begin                                                                                                    // [  582] 

    // call CalculateDC
  ld r1,[r17,8];                                                                    // VSTACK Calculate4x4SADs [  585] 
LINE 585".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [  585] 

    // delay slots start

    // calculate sad
    // set up arguments for Calculate4x4SADs
       VMOVW             vr00, SDMOF_WhitePredError, 8  //DS_______________________________________________ i3 [  591] 
       VIV.1             i7, vr04  //DS____________________________________________________________ gThreshold [  592] 

       VNOP                //DS_______________________________________________________________________________ [  594] 
    // delay slots end


    // sad result returned in i2
// End                                                                                                      // [  599] 

// Begin                                                                                                    // [  601] 
    // return result from Calculate4x4SADs is intraLumaCost

//vec16  vr07  sumdc                                                                                        // [  604] 
//vec16  vr08  saddc0                                                                                       // [  605] 
//vec16  vr09  saddc1                                                                                       // [  606] 


    // calculate the dc value for the 4x4 sub-block of dc terms by summing all
    // the dc terms and then dividing by 16 with rounding
LINE 611".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VADDW             vr07, vr02, vr03                                         // sumdc gDCArray0 gDCArray1 [  611] 
              //  Stall: [vr07]  ...............................      vr07    
       VXSUMW.255        vr07, vr07, 0xff                                                       // sumdc sumdc [  612] 
              //  Stall: [vr07]  ...............................      vr07    
              //  Stall: [vr07]  ...............................      vr07    
       VASRRW            vr07, vr07, 4                                                          // sumdc sumdc [  613] 

    // whiten the 4x4 sub-block of dc terms and calculate cost of dc block
              //  Stall: [vr07]  ...............................      vr07    
              //  Stall: [vr07]  ...............................      vr07    
       VSUBW             vr08, vr02, vr07                                            // saddc0 gDCArray0 sumdc [  616] 
              //  Stall: [vr08]  ...............................      vr08    
       VABSW             vr08, vr08                                                           // saddc0 saddc0 [  617] 
       VSUBW             vr09, vr03, vr07                                            // saddc1 gDCArray1 sumdc [  618] 
              //  Stall: [vr09]  ...............................      vr09    
       VABSW             vr09, vr09                                                           // saddc1 saddc1 [  619] 


    // return to caller
       VJB               vr31, 0                                                                     // VSTACK [  623] 

    // delay slots start

       VADDW             vr08, vr08, vr09  //DS__________________________________________ saddc0 saddc0 saddc1 [  627] 
              //  Stall: [vr08]  ...............................      vr08    
       VXSUMW.255        vr08, vr08, 0xff  //DS_________________________________________________ saddc0 saddc0 [  628] 

    // add cost of dc block to the sad
              //  Stall: [vr08]  ...............................      vr08    
              //  Stall: [vr08]  ...............................      vr08    
       VADDW.4           vr00, vr00, vr08  //DS____________________________ intraLumaCost intraLumaCost saddc0 [  631] 

    // delay slots end


// End                                                                                                      // [  636] 

    // return result in i2

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc WhitenResidual_CalculateSAD                                                                      // [  640] 




//------------------------------------------------------------------------------
// Name:          GenerateIntra16x16Residual
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] GenerateIntra16x16Residual                                                                  // [  649] 
_Asm int Record_GenerateIntra16x16Residual(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,16]
  vrec r1

    // function arguments

// Begin                                                                                                    // [  652] 
    // need to hack explicit lane number as ScanMP does not yet support defining
    // p16's to be in the same lane as another p16
// p16    i9  intra_16x16_modeOffset                                                                        // [  655] ....................// intra_16x16_mode = i1

// p16    i3  source_ptr                                                                                    // [  657] 
// p16    i7  prediction_ptr                                                                                // [  658] 
// p16    i8  prediction_error_ptr                                                                          // [  659] 

// p16    i10  row                                                                                          // [  661] 

//vec16  vr07  source0                                                                                      // [  663] 
//vec16  vr08  source1                                                                                      // [  664] 
//vec16  vr09  source2                                                                                      // [  665] 
//vec16  vr10  source3                                                                                      // [  666] 

//vec16  vr11  prediction0                                                                                  // [  668] 
//vec16  vr12  prediction1                                                                                  // [  669] 
//vec16  vr13  prediction2                                                                                  // [  670] 
//vec16  vr14  prediction3                                                                                  // [  671] 

//vec16  vr15  prediction_error0                                                                            // [  673] 
//vec16  vr16  prediction_error1                                                                            // [  674] 
//vec16  vr17  prediction_error2                                                                            // [  675] 
//vec16  vr18  prediction_error3                                                                            // [  676] 

    // setup arguments for GenerateIntra16x16Residual
LINE 679".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIM               i3, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                       // source_ptr CirBuf [  679] 
       VMULW.2           vr01, vr00, (16*16)                        // intra_16x16_modeOffset intra_16x16_mode [  680] ....................// 1 byte per pixel
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VIM               i7, i9, SDMOF_IntraPredictionLuma            // prediction_ptr intra_16x16_modeOffset [  681] 
       VMULW.2           vr01, vr00, (16*16*2)                      // intra_16x16_modeOffset intra_16x16_mode [  682] ....................// 2 bytes per pixel
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VIM               i8, i9, SDMOF_IntraPredictionResidsLuma // prediction_error_ptr intra_16x16_modeOffset [  683] 

    // setup loop counter
    // 16 rows total, 2 rows per loop
       VMOVW             vr01, (16/2)-1, 4                                                              // row [  687] 


    // rowLoop start
//rowLoop --------                                                                                          // [  691] 
 vendrec r3                                                                                                 // [  691] 
 vrec r3                                                                                                    // [  691] 

    // load current (8 bits per pixel)
LINE 694".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD64W            vr07,[i3, 0]                                                    // source0 source_ptr [  694] 
       VLD64W            vr08,[i3, 8]                                                    // source1 source_ptr [  695] 
       VLD64W            vr09,[i3, 16]                                                   // source2 source_ptr [  696] 
       VLD64W            vr10,[i3, 24]                                                   // source3 source_ptr [  697] 

    // load prediction (8 bits per pixel)
       VLD64W            vr11,[i7, 0]                                            // prediction0 prediction_ptr [  700] 
       VLD64W            vr12,[i7, 8]                                            // prediction1 prediction_ptr [  701] 
       VLD64W            vr13,[i7, 16]                                           // prediction2 prediction_ptr [  702] 
       VLD64W            vr14,[i7, 24]                                           // prediction3 prediction_ptr [  703] 

    // calculate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr15, vr07, vr11                             // prediction_error0 source0 prediction0 [  706] 
       VSUBW             vr16, vr08, vr12                             // prediction_error1 source1 prediction1 [  707] 
       VSUBW             vr17, vr09, vr13                             // prediction_error2 source2 prediction2 [  708] 
       VSUBW             vr18, vr10, vr14                             // prediction_error3 source3 prediction3 [  709] 

    // store residual (16 bits per pixel)
       VST128            vr15,[i8, 0]                                // prediction_error0 prediction_error_ptr [  712] 
       VST128            vr16,[i8, 16]                               // prediction_error1 prediction_error_ptr [  713] 
       VST128            vr17,[i8, 32]                               // prediction_error2 prediction_error_ptr [  714] 
       VST128            vr18,[i8, 48]                               // prediction_error3 prediction_error_ptr [  715] 

    // decrement and loop
       VJD.i10           i10, r3                                                            // row row rowLoop [  718] 

    // delay slots start

    // increament pointers
       VIM               i3, i3, PCB_CURRENT_STRIDE*2  //DS_____________________________ source_ptr source_ptr [  723] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i7, i7, 16*2  //DS_____________________________________ prediction_ptr prediction_ptr [  724] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i8, i8, 16*2*2  //DS_______________________ prediction_error_ptr prediction_error_ptr [  725] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


// End                                                                                                      // [  730] 

    // return to caller
LINE 733".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJB               vr31, 0                                                                     // VSTACK [  733] 

    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [  736] 
       VNOP                //DS_______________________________________________________________________________ [  737] 
       VNOP                //DS_______________________________________________________________________________ [  738] 
    // delay slots end

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x16Residual                                                                       // [  741] 




//------------------------------------------------------------------------------
// Name:          GenerateIntra16x16Prediction
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] GenerateIntra16x16Prediction                                                                     // [  750] 
_Asm int Record_GenerateIntra16x16Prediction(int *mac_tab)
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
GenerateIntra16x16Prediction_RecLoop:
  ld r1,[r17 ,20]
  vrec r1

    // function arguments
    // intraValidFlag = i0

// Begin                                                                                                    // [  754] ....................// 1
// p16    i3  quantValue                                                                                    // [  755] 
// p16    i7  thresholdLutPtr                                                                               // [  756] 
// p16    i8  lambdaLutPtr                                                                                  // [  757] 
// p16    i9  lambda                                                                                        // [  758] 

    // get quantiser value
LINE 761".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD8W_3           vr00,[i4, MPO_QuantValue]                                        // quantValue CirBuf [  761] 

    // setup global best cost value to max
   mov                 r0, 0x7fff
       VMOVW             vr04, r0, 16                                                    // gBestIntraLumaCost [  765] 

    // zero dc_prediction, as it's formed by accumulation
       VMOVW             vr06, 0, 255                                                         // dc_prediction [  768] 

    // setup global value pointers
              //  Stall: [i3]  .................................      vr00.008      vr06    
       VIM               i8, i3, SDMOF_h264_encoder_rd_sad_lambda                   // lambdaLutPtr quantValue [  771] 
       VIM               i7, i3, SDMOF_h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32 // thresholdLutPtr quantValue [  772] 

    // setup global lambda value
       VLD8W_1           vr01,[i8, 0]                                                   // lambda lambdaLutPtr [  775] 

    // setup global threshold value
       VLD8W             vr04,[i7, 0]                                            // gThreshold thresholdLutPtr [  778] 
    // no need to do  the following as it's included in the lut
    //vmulw               'gThreshold, 'gThreshold, MIN_SKIP_16X16
    //vasrw               'gThreshold, 'gThreshold, 5

    // test to see if we have enough time to perform whitening
       VSUBW.8.F         vr62, vr05, 1                                                  // VZERO gUseWhitening [  784] 

    // setup global multiplied lambda values
              //  Stall: [i9]  .................................      vr01.002      vr04.001      vr62.008      vr63.008
       VMIVW.14          vr04, i9                             // gLambda'gLambdaMult16'gLambdaMult10_30 lambda [  787] 

    // setup global multiplied lambda values
              //  Stall: [vr04]  ...............................      vr04.014      vr63.008
       VMULW.4           vr04, vr04, 16                                         // gLambdaMult16 gLambdaMult16 [  790] 

    // if so then we do a final adjust of the cost by adding 30 times lambda
    // otherwise we add 10 times lambda
       VMULW.8.Z         vr04, vr04, 30                                   // gLambdaMult10_30 gLambdaMult10_30 [  794] 
              //  Stall: [vr04]  ...............................      vr04.012
              //  Stall: [vr04]  ...............................      vr04.008
       VMULW.8.NZ        vr04, vr04, 10                                   // gLambdaMult10_30 gLambdaMult10_30 [  795] 

// End                                                                                                      // [  797] ....................// 1


    // vertical prediction (mode = 0)
//verticalPrediction --------                                                                               // [  801] 
 vendrec r3                                                                                                 // [  801] 
 vrec r3                                                                                                    // [  801] 

// Begin                                                                                                    // [  803] ....................// 1
// p16    i3  aboveOk                                                                                       // [  804] 

    // check if above is valid
LINE 807".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIA               i3, i0, 2                                                   // aboveOk intraValidFlag [  807] 
              //  Stall: [i3]  .................................      vr04.008      I03  
       VJP!i3            r4                                                    // aboveOk horizontalPrediction [  808] 

    // delay slots start

    // shouldn't really allow delay slots to go across scope boundaries as it could
    // cause a stall in the jump destination, however we are more concerned about
    // optimising the most commonly occurring case. which in this case is also the
    // worst case.

// End   //DS_________________________________________________________________________________________________ [  817] ....................// 1

// Begin   //DS_______________________________________________________________________________________________ [  819] ....................// 1
// p16    i3  useIntraLumaDCPredOnly  //DS____________________________________________________________________ [  820] 

//vec16  vr07  A_lo  //DS_____________________________________________________________________________________ [  822] 
//vec16  vr08  A_high  //DS___________________________________________________________________________________ [  823] 

    // get intra prediction pixels for A
LINE 826".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD64W            vr07,[i5, MPO_Y_IntraPredPixel+0]  //DS__________________________________ A_lo FixBuf [  826] 
       VLD64W            vr08,[i5, MPO_Y_IntraPredPixel+8]  //DS________________________________ A_high FixBuf [  827] 

       VNOP                //DS_______________________________________________________________________________ [  829] 
    // delay slots end

    // can't go in a delay slot
    // accumulate intra prediction pixels A into dc prediction vector
       VADDW             vr06, vr07, vr08                                         // dc_prediction A_lo A_high [  834] 

    // check if we are only doing dc prediction
       VIV.4             i3, vr05                            // useIntraLumaDCPredOnly gUseIntraLumaDCPredOnly [  837] 
              //  Stall: [i3]  .................................      I03  
       VJP.i3            r4                                     // useIntraLumaDCPredOnly horizontalPrediction [  838] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  842] ....................// 1

// Begin   //DS_______________________________________________________________________________________________ [  844] ....................// 1
// p16    i3  row  //DS_______________________________________________________________________________________ [  845] 

// p16    i7  a_ptr  //DS_____________________________________________________________________________________ [  847] 
// p16    i8  A_ptr  //DS_____________________________________________________________________________________ [  848] 

//vec16  vr07  a  //DS________________________________________________________________________________________ [  850] 
//vec16  vr08  A  //DS________________________________________________________________________________________ [  851] 


    // load prediction
LINE 855".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD128            vr08,[i5, MPO_Y_IntraPredPixel]  //DS_______________________________________ A FixBuf [  855] 

    // setup loop counter
    // 16 rows total, 1 row per loop
       VMOVW             vr00, 16-1, 8  //DS______________________________________________________________ row [  859] 

    // setup destination and source pointers
       VMOVW             vr00, SDMOF_IntraPredictionLuma+(MBIM_H264_Y_VERTICAL*16*16), 128  //DS________ a_ptr [  862] ....................// 1 byte per pixel
    // no need for a source pointer, as the prediction can just be load directly
    //vim                 A_ptr, FixBuf, MPO_Y_IntraPredPixel

    // delay slots end

    // setup global intra16x16 mode value
       VMOVW             vr00, MBIM_H264_Y_VERTICAL, 2                                     // intra_16x16_mode [  869] 


    // rowLoop start
//rowLoopVerticalPrediction --------                                                                        // [  873] 
 vendrec r5                                                                                                 // [  873] 
 vrec r5                                                                                                    // [  873] 

    // decrement and loop
LINE 876".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJD.i3            i3, r5                                           // row row rowLoopVerticalPrediction [  876] 

    // delay slots start

    // store prediction
       VST128            vr08,[i7, 0]  //DS___________________________________________________________ A a_ptr [  881] 

    // increament pointers
       VIM               i7, i7, 16  //DS_________________________________________________________ a_ptr a_ptr [  884] ....................// 16 pixels, 1 byte per pixel, 1 row

       VNOP                //DS_______________________________________________________________________________ [  886] 
    // delay slots end

// End                                                                                                      // [  889] ....................// 1

    // generate residual for whitening
  ld r1,[r17,16];                                                         // VSTACK GenerateIntra16x16Residual [  892] 
LINE 892".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [  892] 
    
    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [  895] 
       VNOP                //DS_______________________________________________________________________________ [  896] 
       VNOP                //DS_______________________________________________________________________________ [  897] 
    // delay slots end


    // whiten residual and calculate sad
  ld r1,[r17,12];                                                        // VSTACK WhitenResidual_CalculateSAD [  902] 
       VJL               vr31, r1                                                                           // [  902] 

    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [  905] 
       VNOP                //DS_______________________________________________________________________________ [  906] 
       VNOP                //DS_______________________________________________________________________________ [  907] 
    // delay slots end


    // compare with best cost and use new value only if better (this preserves
    // the priority of the mode processing order of 0, 1, 2)
       VSUBW.16.F        vr62, vr04, i2                              // VZERO gBestIntraLumaCost intraLumaCost [  913] 
              //  Stall: [VFLAGS]   ............................      vr62.016      vr63.016
       VMIVW.16.GT       vr04, i2                                          // gBestIntraLumaCost intraLumaCost [  914] 
       VMOVW.16.GT       vr05, MBIM_H264_Y_VERTICAL                                      // gBestIntraLumaMode [  915] 


    // horizontal prediction (mode = 1)
//horizontalPrediction --------                                                                             // [  919] 
 vendrec r4                                                                                                 // [  919] 
 vrec r4                                                                                                    // [  919] 

// Begin                                                                                                    // [  921] ....................// 1
// p16    i3  leftOk                                                                                        // [  922] 

    // check if left is valid
LINE 925".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIA               i3, i0, 1                                                    // leftOk intraValidFlag [  925] 
              //  Stall: [i3]  .................................      I03  
       VJP!i3            r6                                                             // leftOk dcPrediction [  926] 

    // delay slots start

    // shouldn't really allow delay slots to go across scope boundaries as it could
    // cause a stall in the jump destination, however we are more concerned about
    // optimising the most commonly occurring case. which in this case is also the
    // worst case.

// End   //DS_________________________________________________________________________________________________ [  935] ....................// 1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// optimised to here from start of function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Begin   //DS_______________________________________________________________________________________________ [  945] ....................// 1
// p16    i3  row  //DS_______________________________________________________________________________________ [  946] 

// p16    i7  a_ptr  //DS_____________________________________________________________________________________ [  948] 
// p16    i8  E_ptr  //DS_____________________________________________________________________________________ [  949] 

//vec16  vr07  a  //DS________________________________________________________________________________________ [  951] 
//vec16  vr08  E  //DS________________________________________________________________________________________ [  952] 

    // setup global intra16x16 mode value
LINE 955".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VMOVW             vr00, MBIM_H264_Y_HORIZONTAL, 2  //DS_______________________________ intra_16x16_mode [  955] 

    // setup destination and source pointers
       VMOVW             vr00, SDMOF_IntraPredictionLuma+(MBIM_H264_Y_HORIZONTAL*16*16), 128  //DS______ a_ptr [  958] ....................// 1 byte per pixel
       VIM               i8, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+PCB_RECONSTRUCTED_STRIDE-1 //DS E_ptr CirLeft [  959] 

    // delay slots end

    // setup loop counter
    // 16 rows total, 1 row per loop
       VMOVW             vr00, 16-1, 8                                                                  // row [  965] 


// Begin                                                                                                    // [  968] ....................// 2 <- 1
// p16    i9  ETerm                                                                                         // [  969] 

    // rowLoop start
//rowLoopHorizontalPrediction --------                                                                      // [  972] 
 vendrec r7                                                                                                 // [  972] 
 vrec r7                                                                                                    // [  972] 

    // load 8 bit prediction
LINE 975".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VLD8W_1           vr01,[i8, 0]                                                           // ETerm E_ptr [  975] 

    // accumulate intra prediction pixel E into dc prediction vector
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VADDW.1           vr06, vr06, i9                                   // dc_prediction dc_prediction ETerm [  978] 

    // duplicate prediction into both bytes of the 16 bit lanes
       VMULW.2           vr01, vr01, 0x0101                                                     // ETerm ETerm [  981] 
    // duplicate prediction into vector register
              //  Stall: [i9]  .................................      vr01.002
              //  Stall: [i9]  .................................      vr01.002
       VMIVW             vr08, i9                                                                   // E ETerm [  983] 

    // decrement and loop
       VJD.i3            i3, r7                                         // row row rowLoopHorizontalPrediction [  986] 

    // delay slots start

    // store prediction
       VST128            vr08,[i7, 0]  //DS___________________________________________________________ E a_ptr [  991] 

    // increament pointers
       VIM               i7, i7, 16  //DS_________________________________________________________ a_ptr a_ptr [  994] ....................// 16 pixels, 1 byte per pixel, 1 row
       VIM               i8, i8, PCB_RECONSTRUCTED_STRIDE  //DS___________________________________ E_ptr E_ptr [  995] 

    // delay slots end

// End                                                                                                      // [  999] ....................// 2 <- 1

// End                                                                                                      // [ 1001] ....................// 1

// Begin                                                                                                    // [ 1003] ....................// 1
// p16    i3  useIntraLumaDCPredOnly                                                                        // [ 1004] 

    // check if we are only doing dc prediction
LINE 1007".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIV.4             i3, vr05                            // useIntraLumaDCPredOnly gUseIntraLumaDCPredOnly [ 1007] 
              //  Stall: [i3]  .................................      I03  
       VJP.i3            r6                                             // useIntraLumaDCPredOnly dcPrediction [ 1008] 

    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [ 1011] 
       VNOP                //DS_______________________________________________________________________________ [ 1012] 
       VNOP                //DS_______________________________________________________________________________ [ 1013] 
    // delay slots end


// End                                                                                                      // [ 1017] ....................// 1

    // generate residual for whitening
  ld r1,[r17,16];                                                         // VSTACK GenerateIntra16x16Residual [ 1020] 
LINE 1020".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [ 1020] 
    
    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [ 1023] 
       VNOP                //DS_______________________________________________________________________________ [ 1024] 
       VNOP                //DS_______________________________________________________________________________ [ 1025] 
    // delay slots end


    // whiten residual and calculate sad
  ld r1,[r17,12];                                                        // VSTACK WhitenResidual_CalculateSAD [ 1030] 
       VJL               vr31, r1                                                                           // [ 1030] 

    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [ 1033] 
       VNOP                //DS_______________________________________________________________________________ [ 1034] 
       VNOP                //DS_______________________________________________________________________________ [ 1035] 
    // delay slots end


    // compare with best cost and use new value only if better (this preserves
    // the priority of the mode processing order of 0, 1, 2)
       VSUBW.16.F        vr62, vr04, i2                              // VZERO gBestIntraLumaCost intraLumaCost [ 1041] 
              //  Stall: [VFLAGS]   ............................      vr62.016      vr63.016
       VMIVW.16.GT       vr04, i2                                          // gBestIntraLumaCost intraLumaCost [ 1042] 
       VMOVW.16.GT       vr05, MBIM_H264_Y_HORIZONTAL                                    // gBestIntraLumaMode [ 1043] 


    // dc prediction (mode = 2)
//dcPrediction --------                                                                                     // [ 1047] 
 vendrec r6                                                                                                 // [ 1047] 
 vrec r6                                                                                                    // [ 1047] 

// Begin                                                                                                    // [ 1049] ....................// 1
// p16    i3  a_ptr                                                                                         // [ 1050] 

//vec16  vr07  intraValidFlagVec                                                                            // [ 1052] 

    // setup global intra16x16 mode value
LINE 1055".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VMOVW             vr00, MBIM_H264_Y_DC, 2                                           // intra_16x16_mode [ 1055] 

    // compare intraValidFlag to see if both above and left are available and
    // broadcast flag across all lanes
       VMIVW             vr07, i0                                          // intraValidFlagVec intraValidFlag [ 1059] 
              //  Stall: [vr07]  ...............................      vr07    
       VSUBW.255.F       vr62, vr07, 3                                              // VZERO intraValidFlagVec [ 1060] 

    // save result of compare in all the sticky flags
       VCLRSTK.255                                                                                          // [ 1063] 
       VSETSTK.255.Z                                                                                        // [ 1064] 


    // compare intraValidFlag to see if neither above and left are available and
    // broadcast flag across all lanes
       VSUBW.255.F       vr62, vr07, 0                                              // VZERO intraValidFlagVec [ 1069] 

    // setup source value
    // if neither above or left are available then use default of 128 (for both
    // bytes of the lane)
   mov                 r0, 0x8080
              //  Stall: [VFLAGS]   ............................      vr62          vr63    
       VMOVW.255.Z       vr06, r0                                                             // dc_prediction [ 1075] 

    // sum all lanes of dc_prediction vector and broadcast across whole vector
              //  Stall: [vr06]  ...............................      vr06          vr63    
       VXSUMW.255.NZ     vr06, vr06, 0xff                                       // dc_prediction dc_prediction [ 1078] 

    // if both above and left are available then add rounding of 16 and divide by 32
              //  Stall: [vr06]  ...............................      vr06    
              //  Stall: [vr06]  ...............................      vr06    
       VASRRW.255.S.NZ   vr06, vr06, 5                                          // dc_prediction dc_prediction [ 1081] 
    
    // if only one of above or left are available then add rounding of 8 and divide by 16
              //  Stall: [vr06]  ...............................      vr06    
              //  Stall: [vr06]  ...............................      vr06    
       VASRRW.255.U.NZ   vr06, vr06, 4                                          // dc_prediction dc_prediction [ 1084] 

    // duplicate prediction into both bytes of the 16 bit lanes
              //  Stall: [vr06]  ...............................      vr06    
              //  Stall: [vr06]  ...............................      vr06    
       VMULW.255.NZ      vr06, vr06, 0x0101                                     // dc_prediction dc_prediction [ 1087] 

    // setup destination pointer
       VMOVW             vr00, SDMOF_IntraPredictionLuma+(MBIM_H264_Y_DC*16*16), 8                    // a_ptr [ 1090] ....................// 1 byte per pixel

    // store prediction
              //  Stall: [vr06]  ...............................      vr00.008      vr06    
       VST128            vr06,[i3, 0]                                                   // dc_prediction a_ptr [ 1093] 
       VST128            vr06,[i3, 16]                                                  // dc_prediction a_ptr [ 1094] 
       VST128            vr06,[i3, 32]                                                  // dc_prediction a_ptr [ 1095] 
       VST128            vr06,[i3, 48]                                                  // dc_prediction a_ptr [ 1096] 

       VST128            vr06,[i3, 64]                                                  // dc_prediction a_ptr [ 1098] 
       VST128            vr06,[i3, 80]                                                  // dc_prediction a_ptr [ 1099] 
       VST128            vr06,[i3, 96]                                                  // dc_prediction a_ptr [ 1100] 
       VST128            vr06,[i3, 112]                                                 // dc_prediction a_ptr [ 1101] 

       VST128            vr06,[i3, 128]                                                 // dc_prediction a_ptr [ 1103] 
       VST128            vr06,[i3, 144]                                                 // dc_prediction a_ptr [ 1104] 
       VST128            vr06,[i3, 160]                                                 // dc_prediction a_ptr [ 1105] 
       VST128            vr06,[i3, 176]                                                 // dc_prediction a_ptr [ 1106] 

       VST128            vr06,[i3, 192]                                                 // dc_prediction a_ptr [ 1108] 


    // generate residual for whitening
  ld r1,[r17,16];                                                         // VSTACK GenerateIntra16x16Residual [ 1112] 
       VJL               vr31, r1                                                                           // [ 1112] 
    
    // delay slots start

    // store prediction
       VST128            vr06,[i3, 208]  //DS_____________________________________________ dc_prediction a_ptr [ 1117] 
       VST128            vr06,[i3, 224]  //DS_____________________________________________ dc_prediction a_ptr [ 1118] 
       VST128            vr06,[i3, 240]  //DS_____________________________________________ dc_prediction a_ptr [ 1119] 

    // delay slots end


    // whiten residual and calculate sad
  ld r1,[r17,12];                                                        // VSTACK WhitenResidual_CalculateSAD [ 1125] 
       VJL               vr31, r1                                                                           // [ 1125] 

    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [ 1128] 
       VNOP                //DS_______________________________________________________________________________ [ 1129] 
       VNOP                //DS_______________________________________________________________________________ [ 1130] 
    // delay slots end


    // compare with best cost and use new value only if better (this preserves
    // the priority of the mode processing order of 0, 1, 2)
       VSUBW.16.F        vr62, vr04, i2                              // VZERO gBestIntraLumaCost intraLumaCost [ 1136] 
              //  Stall: [VFLAGS]   ............................      vr62.016      vr63.016
       VMIVW.16.GT       vr04, i2                                          // gBestIntraLumaCost intraLumaCost [ 1137] 
       VMOVW.16.GT       vr05, MBIM_H264_Y_DC                                            // gBestIntraLumaMode [ 1138] 

// End                                                                                                      // [ 1140] ....................// 1

// Begin                                                                                                    // [ 1142] ....................// 1
// p16    i3  lambdaMult10_30                                                                               // [ 1143] 

    // scale final cost for comparison against inter
LINE 1146".\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss"
       VIV.8             i3, vr04                                          // lambdaMult10_30 gLambdaMult10_30 [ 1146] 
              //  Stall: [i3]  .................................      I03  
       VADDW.16          vr04, vr04, i3               // gBestIntraLumaCost gBestIntraLumaCost lambdaMult10_30 [ 1147] 


    // save results
       VST8_4            vr05,[i4, MPO_WorkArea_BestIntraLumaMode]                // gBestIntraLumaMode CirBuf [ 1151] 
       VST16_4           vr04,[i4, MPO_WorkArea_BestIntraLumaCost]                // gBestIntraLumaCost CirBuf [ 1152] 

// End                                                                                                      // [ 1154] ....................// 1


  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz GenerateIntra16x16Prediction_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x16Prediction                                                                     // [ 1157] 




const int MACRO_IDX_ConstrainIntraValidFlag = 0;
const int SIMD_IDX_CalculateDC = 1;
const int SIMD_IDX_Calculate4x4SADs = 2;
const int SIMD_IDX_WhitenResidual_CalculateSAD = 3;
const int SIMD_IDX_GenerateIntra16x16Residual = 4;
const int MACRO_IDX_GenerateIntra16x16Prediction = 5;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int IntraLumaPrediction_Record(int sa, int SIMDnum)
{
    int *map = IntraLumaPrediction;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_ConstrainIntraValidFlag] = sa;
    sa = Record_ConstrainIntraValidFlag(map);

    map[SIMD_IDX_CalculateDC] = sa;
    sa = Record_CalculateDC(map);

    map[SIMD_IDX_Calculate4x4SADs] = sa;
    sa = Record_Calculate4x4SADs(map);

    map[SIMD_IDX_WhitenResidual_CalculateSAD] = sa;
    sa = Record_WhitenResidual_CalculateSAD(map);

    map[SIMD_IDX_GenerateIntra16x16Residual] = sa;
    sa = Record_GenerateIntra16x16Residual(map);

    map[MACRO_IDX_GenerateIntra16x16Prediction] = sa;
    sa = Record_GenerateIntra16x16Prediction(map);

    return sa;
}
