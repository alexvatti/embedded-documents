//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss'

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
// MetaWare ouput ON                                                  // [   20] 
#define LINE #line                                                    // [   20] 

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

#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"   

#include ".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.h"


// Macro table
    int IntraMP4LumaPrediction[6];
    int IntraMP4LumaPrediction_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Global Function Arguments
// p16    i3  standard                                                // [   35] 
// p16    i7  raw_dataPtr                                             // [   36] 
//------------------------------------------------------------------------------
// Intra Prediction Common Register Workspace
//vec16  vr02  gDCArray                                               // [   39] 
//vec16  vr03  sadval                                                 // [   40] 

//------------------------------------------------------------------------------
// Name:          CalculateSADDC8
// Purpose:       Calculates the DC value for each 8x8 sub-block of a macroblock
// Arguments:     prediction_errorPtr = pointer to residual macroblock
//                dcarrayPtr = pointer to 16 element array to store dc value
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] CalculateDC8                                          // [   49] 
_Asm int Record_CalculateDC8(int *mac_tab)
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
// ****** Informational : Register space for symbol "prediction_errorPtr" already allocated. // [   51] 
// p16    i7  prediction_errorPtr                                     // [   51] 

// Begin                                                              // [   53] 


//vec16  vr12  prediction_error0                                      // [   56] 
//vec16  vr13  prediction_error1                                      // [   57] 
//vec16  vr14  prediction_error2                                      // [   58] 
//vec16  vr15  prediction_error3                                      // [   59] 
//vec16  vr16  prediction_error4                                      // [   60] 
//vec16  vr17  prediction_error5                                      // [   61] 
//vec16  vr18  prediction_error6                                      // [   62] 
//vec16  vr19  prediction_error7                                      // [   63] 
//vec16  vr20  dcarrayTemp0                                           // [   64] 
//vec16  vr21  dcarrayTemp1                                           // [   65] 
//vec16  vr22  AveVal                                                 // [   66] 
    // load prediction data for a 8 rows of 8x8 sub-blocks
    // 8x8 sub-blocks 0 & 1
LINE 69".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD64W            vr12,[i7, 0]   // prediction_error0 raw_dataPtr [   69] 
       VLD64W            vr13,[i7, 16]  // prediction_error1 raw_dataPtr [   70] 
       VLD64W            vr14,[i7, 32]  // prediction_error2 raw_dataPtr [   71] 
       VLD64W            vr15,[i7, 48]  // prediction_error3 raw_dataPtr [   72] 
       VLD64W            vr16,[i7, 64]  // prediction_error4 raw_dataPtr [   73] 
       VLD64W            vr17,[i7, 80]  // prediction_error5 raw_dataPtr [   74] 
       VLD64W            vr18,[i7, 96]  // prediction_error6 raw_dataPtr [   75] 
       VLD64W            vr19,[i7, 112] // prediction_error7 raw_dataPtr [   76] 

    // add rows to each other, resetting the accumulator on the first one
    // 8x8sub-blocks 0 & 1
       VADDW             vr20, vr12, vr13 // dcarrayTemp0 prediction_error0 prediction_error1 [   80] 
       VADDAW            vr20, vr14, vr15 // dcarrayTemp0 prediction_error2 prediction_error3 [   81] 
       VADDAW            vr20, vr16, vr17 // dcarrayTemp0 prediction_error4 prediction_error5 [   82] 
       VADDAW            vr20, vr18, vr19 // dcarrayTemp0 prediction_error6 prediction_error7 [   83] 

     // sum 8 columns of sub-block into result
    // 8x8 sub-blocks 0
       VXSUMW            vr02, vr20, 0xff       // gDCArray dcarrayTemp0 [   87] // every element contains the sum
       VASRRW            vr22, vr02, 6                // AveVal gDCArray [   88] // get the average value
       VASRRW            vr02, vr02, 3              // gDCArray gDCArray [   89] // divide and round by the 8 to get the DC of the DCT
    
 
    
       VSUBW             vr12, vr12, vr22 // prediction_error0 prediction_error0 AveVal [   93] 
       VSUBW             vr13, vr13, vr22 // prediction_error1 prediction_error1 AveVal [   94] 
       VSUBW             vr14, vr14, vr22 // prediction_error2 prediction_error2 AveVal [   95] 
       VSUBW             vr15, vr15, vr22 // prediction_error3 prediction_error3 AveVal [   96] 
       VSUBW             vr16, vr16, vr22 // prediction_error4 prediction_error4 AveVal [   97] 
       VSUBW             vr17, vr17, vr22 // prediction_error5 prediction_error5 AveVal [   98] 
       VSUBW             vr18, vr18, vr22 // prediction_error6 prediction_error6 AveVal [   99] 
       VSUBW             vr19, vr19, vr22 // prediction_error7 prediction_error7 AveVal [  100] 
    
       VABSW             vr03, vr12          // sadval prediction_error0 [  102] 
       VABSAW            vr03, vr13          // sadval prediction_error1 [  103] 
       VABSAW            vr03, vr14          // sadval prediction_error2 [  104] 
       VABSAW            vr03, vr15          // sadval prediction_error3 [  105] 
       VABSAW            vr03, vr16          // sadval prediction_error4 [  106] 
       VABSAW            vr03, vr17          // sadval prediction_error5 [  107] 

    // return to caller
       VJB               vr31, 0                               // VSTACK [  110] 
       VABSAW            vr03, vr18  //DS______ sadval prediction_error6 [  111] 
       VABSAW            vr03, vr19  //DS______ sadval prediction_error7 [  112] 
    // increment dcarray result indicies (shift left by 2 because we process 4
    // rows and 4 columns at a time)
       VXSUMW            vr03, vr03, 0xff  //DS___________ sadval sadval [  115] // every element contains the sum

// End                                                                // [  117] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CalculateDC8                                               // [  119] 


//------------------------------------------------------------------------------
// Name:          IntraModeCostmp4
// Purpose:       Calculates the Intra cost for MPEG4 to compare against inter cost for mode decision
//                This requires a SAD calculation as well as an estimate of the number of bits used for DC terms
// Arguments:     None
//                
// Return Value:  void, return value is stored in Circular Buffer
//------------------------------------------------------------------------------

// [Macro] IntraModeCostmp4                                           // [  129] 
_Asm int Record_IntraModeCostmp4(int *mac_tab)
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
IntraModeCostmp4_RecLoop:
  ld r1,[r17 ,4]
  vrec r1


// Begin                                                              // [  131] 
    // function arguments
// ****** Informational : Register space for symbol "raw_dataPtr" already allocated. // [  133] // has to be same as in subroutine CalculateDC8
// p16    i7  raw_dataPtr                                             // [  133] 
// p16    i0  block_no                                                // [  134] 

// p16    i1  cirPtr                                                  // [  136] 
// s16    vr04.2  blkInOffset                                         // [  137] 
// p16    i8  cost                                                    // [  138] 
// p16    i2  QuantVal                                                // [  139] 

// p16    i9  temp                                                    // [  141] 

// these variables used to determine direction of prediction for each block
//vec16  vr04  DC.28                                                  // [  144] 
//vec16  vr04  DC_left.4                                              // [  144] 
//vec16  vr04  DC_aboveleft.8                                         // [  144] 
//vec16  vr04  DC_above.16                                            // [  144] 
//vec16  vr05  Flag.28                                                // [  145] 
//vec16  vr05  Flag_Left.4                                            // [  145] 
//vec16  vr05  Flag_AboveLeft.8                                       // [  145] 
//vec16  vr05  Flag_Above.16                                          // [  145] 
//vec16  vr06  Diff.28                                                // [  146] 
//vec16  vr06  Diff_Left.4                                            // [  146] 
//vec16  vr06  Diff_AboveLeft.8                                       // [  146] 
//vec16  vr06  Diff_Above.16                                          // [  146] 
// these variables used to store predictions and differences per each luma block
//vec16  vr07  Res                                                    // [  148] 
//vec16  vr07  Res_bk0.1                                              // [  148] 
//vec16  vr07  Res_rub0.2                                             // [  148] 
//vec16  vr07  Res_bk1.4                                              // [  148] 
//vec16  vr07  Res_rub1.8                                             // [  148] 
//vec16  vr07  Res_bk2.16                                             // [  148] 
//vec16  vr07  Res_rub2.32                                            // [  148] 
//vec16  vr07  Res_bk3.64                                             // [  148] 
//vec16  vr07  Res_rub3.128                                           // [  148] 
//vec16  vr08  Pred.85                                                // [  149] 
//vec16  vr08  Pred_bk0.1                                             // [  149] 
//vec16  vr08  Pred_bk1.4                                             // [  149] 
//vec16  vr08  Pred_bk2.16                                            // [  149] 
//vec16  vr08  Pred_bk3.64                                            // [  149] 
//vec16  vr09  DCDiff.85                                              // [  150] 
//vec16  vr09  DCDiff_bk0.1                                           // [  150] 
//vec16  vr09  DCDiff_bk1.4                                           // [  150] 
//vec16  vr09  DCDiff_bk2.16                                          // [  150] 
//vec16  vr09  DCDiff_bk3.64                                          // [  150] 

//vec32  vr10  DCFwdScaler                                            // [  152] 
//vec32  vr10  DCFwdScaler_bk0.3                                      // [  152] 
//vec32  vr10  DCFwdScaler_bk1.12                                     // [  152] 
//vec32  vr10  DCFwdScaler_bk2.48                                     // [  152] 
//vec32  vr10  DCFwdScaler_bk3.192                                    // [  152] 
//vec32  vr11  workarea                                               // [  153] 
//vec16  vr12  ScaleVec                                               // [  154] 
// p16    i10  FixLeft                                                // [  155] 
// s16    vr04.1  Quant                                               // [  156] 
 
// s16    vr13.4  DCPred                                              // [  158] 
// s16    vr14.4  tempdiff                                            // [  159] 
   
// p32    k12  FwdScaler                                              // [  161] 
// s32    vr13.48  temp32                                             // [  162] 
// s16    vr05.1  DCScaler                                            // [  163] 
 
//vec16  vr15  Offset                                                 // [  165] 
    
    
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| raw_.| CirL.|FixBuf|CirBuf| stan.| Quan.|cirPtr| bloc.|
//|VR 1|      |      |FwdS.er       |      | FixL.|  temp|  cost|
//|VR 2|                    gDCArray                           |
//|VR 3|                      sadval                           |
//|VR 4|      |      |      | DC_a.| DC_a.| DC_l.| blkI.| Quant|DC.28   
//|VR 5|      |      |      | Flag.| Flag.| Flag.|      | DCSc.|Flag.28   
//|VR 6|      |      |      | Diff.| Diff.| Diff.|      |      |Diff.28   
//|VR 7| Res_.| Res_.| Res_.| Res_.| Res_.| Res_.| Res_.| Res_.|Res.255   
//|VR 8|      | Pred.|      | Pred.|      | Pred.|      | Pred.|Pred.85   
//|VR 9|      | DCDi.|      | DCDi.|      | DCDi.|      | DCDi.|DCDiff.85   
//|VR10|DCFw.3       |DCFw.2       |DCFw.1       |DCFw.0       |DCFwdScaler.255   
//|VR11|                    workarea                           |
//|VR12|                    ScaleVec                           |
//|VR13|      |      |temp32       |      |DCPred|      |      |
//|VR14|      |      |      |      |      | temp.|      |      |
//|VR15|                      Offset                           |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

LINE 169".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMOVW             vr00, 0, 1                          // block_no [  169] 
       VMOVW             vr01, 0, 1                              // cost [  170] 
       VMOVW             vr01, 0, 2                              // temp [  171] 
       VLD8W_2           vr00,[i4, MPO_QuantValue]    // QuantVal CirBuf [  172] // needed later

  ld r1,[r17,0];                                  // VSTACK CalculateDC8 [  174] // calc whitened SAD
       VJL               vr31, r1                                     // [  174] 
        VMR2W.1         vr04, vr00, vr00  //DS___________ Quant QuantVal [  175] 
    // set the location of the actual pixels for first 8x8 block
LINE 177".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VIM               i1, i4, MPO_PixelCoeffBuffer  //DS cirPtr CirBuf [  177] 
        VMR2W.128       vr00, vr00, vr00  //DS_______ raw_dataPtr cirPtr [  178] 
        
        //  gDCArray(vr02) holds measured DC, sadval holds the sad
  ld r1,[r17,0];                                  // VSTACK CalculateDC8 [  181] 
LINE 181".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VJL               vr31, r1                                     // [  181] 
// ****** Warning : vr07 is marked as used by symbol  //DS______________ [  182] // move bk 0 result
LINE 182".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMVW              vr07, vr02, 1  //DS___________________ gDCArray [  182] 
       VADDW.1           vr01, vr01, vr03  //DS________ cost cost sadval [  183] // increment cost by the sad for the block    
       VADDW.128         vr00, vr00, 8  //DS____ raw_dataPtr raw_dataPtr [  184] 
        
        // gDCArray(vr02) holds measured DC, sadval holds the sad
  ld r1,[r17,0];                                  // VSTACK CalculateDC8 [  187] 
       VJL               vr31, r1                                     // [  187] 
// ****** Warning : vr07 is marked as used by symbol  //DS______________ [  188] // move bk 0 result
LINE 188".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMVW              vr07, vr02, 4  //DS___________________ gDCArray [  188] 
       VADDW.1           vr01, vr01, vr03  //DS________ cost cost sadval [  189] // increment cost by the sad for the block
       VADDW.128         vr00, vr00, 120  //DS__ raw_dataPtr raw_dataPtr [  190] 
        
        //  gDCArray(vr02) holds measured DC, sadval holds the sad
  ld r1,[r17,0];                                  // VSTACK CalculateDC8 [  193] 
       VJL               vr31, r1                                     // [  193] 
// ****** Warning : vr07 is marked as used by symbol  //DS______________ [  194] // move bk 0 result
LINE 194".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMVW              vr07, vr02, 16  //DS__________________ gDCArray [  194] 
       VADDW.1           vr01, vr01, vr03  //DS________ cost cost sadval [  195] // increment cost by the sad for the block        
       VADDW.128         vr00, vr00, 8  //DS____ raw_dataPtr raw_dataPtr [  196] 
        
        //  gDCArray(vr02) holds measured DC, sadval holds the sad
// ****** Warning : vr07 is marked as used by symbol                  // [  199] // move bk 0 result
LINE 199".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMVW              vr07, vr02, 64                      // gDCArray [  199] 
       VADDW.1           vr01, vr01, vr03            // cost cost sadval [  200] // increment cost by the sad for the block
            
            
    // cost now contains the "whitened SAD"
    // read the forward and backwards scaling values for DC prediction
       VIM               i10, i2, SDMOF_mpeg4_fwd_luma_dc_scaler // FixLeft QuantVal [  205] // this array unsigned char
       VLD8W             vr05,[i10, 0]               // DCScaler FixLeft [  206] 
        VMR7W.2         vr01, vr05, vr05                // temp DCScaler [  207] 
LINE 208".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMIVW.255         vr12, i9                       // ScaleVec temp [  208] // in every 16 bit word in the SCaleVec
    // it remains to add the bit cost resulting from DC prediction etc
       VMULW.4           vr00, vr00, 4              // QuantVal QuantVal [  210] // nb this array is an Int array - need 4 bytes
       VIM               i10, i2, SDMOF_mpeg4_luma_dc_scaler // FixLeft QuantVal [  211] 
       VLD32_4           vr13,[i10, 0]                 // temp32 FixLeft [  212] 

        VMVW.48         vr01, vr13                   // FwdScaler temp32 [  214] 
LINE 215".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VADD              vr10, vr62, k12  // DCFwdScaler VZERO FwdScaler [  215] // copy this value to each of the 4 32bit locations in vector

    
    // block 0
    // load neighbouring data
       VLD8W_2           vr05,[i4, MPO_LeftMB]       // Flag_Left CirBuf [  220] 
       VLD8W_4           vr05,[i4, MPO_AboveMB]     // Flag_Above CirBuf [  221] 
       VLD8W_3           vr05,[i4, MPO_AboveLeftMB] // Flag_AboveLeft CirBuf [  222] 
    // now check that the MBs are valid, same ID and Intra
       VAND.28           vr05, vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag Flag [  224] // check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
       VSNEW.28          vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag [  227] 
   // load the DC terms in the buffers
       VLD16_3           vr04,[i5, MPO_Y_IntraPredDC_Coeff-MPO_SIZE_OF_FIXED_ENTRY+6] // DC_aboveleft FixBuf [  229] 
       VLD16_2           vr04,[i6, MPO_Y_IntraPredDC_Coeff+2] // DC_left CirLeft [  230] 
       VLD16_4           vr04,[i5, MPO_Y_IntraPredDC_Coeff+4] // DC_above FixBuf [  231] 
    // overwrite the values with 1024 if sticky is set (i.e. not (valid, same id and intra))
    
       VMOVW.28.S        vr04, 1024                                // DC [  234] 
    // now have A B and C

        VMR1W.4         vr06, vr04, vr04       // Diff_Left DC_aboveleft [  237] 
        VMR7W.16        vr06, vr04, vr04      // Diff_Above DC_aboveleft [  238] 
    
LINE 240".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VDIFW.20          vr06, vr06, vr04                // Diff Diff DC [  240] // abs(B - C) and abs(A - C)
        
        VMR2W.4         vr13, vr04, vr04              // DCPred DC_above [  242] // assume C
 
        VMR2W.4         vr14, vr06, vr06          // tempdiff Diff_Above [  244] 
LINE 245".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLEW.4.F          vr14, vr14, vr06 // tempdiff tempdiff Diff_Left [  245] //set condition flag if |B-C| <= |B-A|    
       VMVW.4.NZ         vr13, vr04                    // DCPred DC_left [  246] // move left accordingly
        VMR2W.1         vr08, vr13, vr13              // Pred_bk0 DCPred [  247] // save in vector Pred
    // have DCprediction for block 0   
    
LINE 250".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VSUBW.1           vr07, vr07, vr08                // Res Res Pred [  250] // subtract prediction from actual
    
       VMUL.3            vr11, vr10, vr07    // workarea DCFwdScaler Res [  252] // quantiser with current dc-scaler multiplier
 
       VASRR.3           vr11, vr11, 20             // workarea workarea [  254] // add 1<<19 and shift down by 20 to get quantised val
    
       VMVW              vr09, vr11, 1                // DCDiff workarea [  256] // save it to DCDiff_bk0
    
       VMULW.1           vr07, vr09, vr05         // Res DCDiff DCScaler [  258] // mult diff by dc_scaler
    
       VADDW.1           vr07, vr07, vr08                // Res Res Pred [  260] // add back prediction (need for predicting other DCs)
    
// now block 1
        VMR6W.4         vr04, vr07, vr07              // DC_left Res_bk0 [  263] 
LINE 264".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD16_3           vr04,[i5, MPO_Y_IntraPredDC_Coeff+4] // DC_aboveleft FixBuf [  264] 
       VLD16_4           vr04,[i5, MPO_Y_IntraPredDC_Coeff+6] // DC_above FixBuf [  265] 

       VLD8W_4           vr05,[i4, MPO_AboveMB]     // Flag_Above CirBuf [  267] 
        VMR1W.8         vr05, vr05, vr05    // Flag_AboveLeft Flag_Above [  268] 
    // no need to check left as in same MB
LINE 270".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VAND.24           vr05, vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag Flag [  270] // check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
       VSNEW.24          vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag [  273] 
  
       VMOVW.24.S        vr04, 1024                                // DC [  275] 
        // now have A B and C
        VMR1W.4         vr06, vr04, vr04       // Diff_Left DC_aboveleft [  277] 
        VMR7W.16        vr06, vr04, vr04      // Diff_Above DC_aboveleft [  278] 
    
LINE 280".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VDIFW.20          vr06, vr06, vr04                // Diff Diff DC [  280] // abs(B - C) and abs(A - C)
        
        VMR2W.4         vr13, vr04, vr04              // DCPred DC_above [  282] // assume B
 
        VMR2W.4         vr14, vr06, vr06          // tempdiff Diff_Above [  284] 
LINE 285".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLEW.4.F          vr14, vr14, vr06 // tempdiff tempdiff Diff_Left [  285] //set condition flag if |B-C| <= |B-A|    
       VMVW.4.NZ         vr13, vr04                    // DCPred DC_left [  286] // move left accordingly
        vmvw.4          vr08, vr13                    // Pred_bk1 DCPred [  287] 
    // have DCprediction for block 1   
    
    
LINE 291".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VSUBW.4           vr07, vr07, vr08                // Res Res Pred [  291] // subtract prediction from actual
    
       VMUL.12           vr11, vr10, vr07    // workarea DCFwdScaler Res [  293] // quantiser with current dc-scaler
 
       VASRR.12          vr11, vr11, 20             // workarea workarea [  295] // add 1<<19
    
       VMVW              vr09, vr11, 4                // DCDiff workarea [  297] // save it to DCDiff_bk0
    
       VMULW.4           vr07, vr09, vr12         // Res DCDiff ScaleVec [  299] // 'DCScaler // mult diff by dc_scaler
    
       VADDW.4           vr07, vr07, vr08                // Res Res Pred [  301] // add back prediction    
    
    // endof block 1
    // block 2  - we know above is good as its the same MB
        VMR4W.16        vr04, vr07, vr07             // DC_above Res_bk0 [  305] 
LINE 306".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD16_2           vr04,[i6, MPO_Y_IntraPredDC_Coeff+6] // DC_left CirLeft [  306] 
       VLD16_3           vr04,[i6, MPO_Y_IntraPredDC_Coeff+2] // DC_aboveleft CirLeft [  307] 
     
       VLD8W_2           vr05,[i4, MPO_LeftMB]       // Flag_Left CirBuf [  309] 
        VMR7W.8         vr05, vr05, vr05     // Flag_AboveLeft Flag_Left [  310] 
        
LINE 312".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VAND.12           vr05, vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag Flag [  312] // check if data valid
    // if everything is good, then they will have the value 112,
    // so set sticky if not that value
       VSNEW.12          vr05, MBNF_VALID+MBNF_VALID_ID+MBNF_INTRA // Flag [  315] 
  
       VMOVW.12.S        vr04, 1024                                // DC [  317] 
        // now have A B and C
        VMR1W.4         vr06, vr04, vr04       // Diff_Left DC_aboveleft [  319] 
        VMR7W.16        vr06, vr04, vr04      // Diff_Above DC_aboveleft [  320] 
    
LINE 322".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VDIFW.20          vr06, vr06, vr04                // Diff Diff DC [  322] // abs(B - C) and abs(A - C)
        
        VMR2W.4         vr13, vr04, vr04              // DCPred DC_above [  324] // assume C
 
        VMR2W.4         vr14, vr06, vr06          // tempdiff Diff_Above [  326] 
LINE 327".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLEW.4.F          vr14, vr14, vr06 // tempdiff tempdiff Diff_Left [  327] //set condition flag if |B-C| <= |B-A|    
       VMVW.4.NZ         vr13, vr04                    // DCPred DC_left [  328] // move left accordingly
        VMR6W.16        vr08, vr13, vr13              // Pred_bk2 DCPred [  329] 
    // have DCprediction for block 2
    
LINE 332".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VSUBW.16          vr07, vr07, vr08                // Res Res Pred [  332] // subtract prediction from actual
    
       VMUL.48           vr11, vr10, vr07    // workarea DCFwdScaler Res [  334] // quantiser with current dc-scaler
 
       VASRR.48          vr11, vr11, 20             // workarea workarea [  336] // add 1<<19
    
       VMVW              vr09, vr11, 16               // DCDiff workarea [  338] // save it to DCDiff_bk0
    
       VMULW.16          vr07, vr09, vr12         // Res DCDiff ScaleVec [  340] // 'DCScaler // mult diff by dc_scaler
    
       VADDW.16          vr07, vr07, vr08                // Res Res Pred [  342] // add back prediction    
    // end of block 2
    
    // block 3 - all are valid
        VMR5W.8         vr04, vr07, vr07         // DC_aboveleft Res_bk0 [  346] // must be a quicker way of doing this ?
        VMR6W.16        vr04, vr07, vr07             // DC_above Res_bk1 [  347] 
        VMR2W.4         vr04, vr07, vr07              // DC_left Res_bk2 [  348] 
        VMR6W.4         vr06, vr07, vr07            // Diff_Left Res_bk0 [  349] 
        VMR4W.16        vr06, vr07, vr07           // Diff_Above Res_bk0 [  350] 
     
LINE 352".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VDIFW.20          vr06, vr06, vr04                // Diff Diff DC [  352] // abs(B - C) and abs(A - C)
        
        VMR2W.4         vr13, vr04, vr04              // DCPred DC_above [  354] // assume C
 
        VMR2W.4         vr14, vr06, vr06          // tempdiff Diff_Above [  356] 
LINE 357".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLEW.4.F          vr14, vr14, vr06 // tempdiff tempdiff Diff_Left [  357] //set condition flag if |B-C| <= |B-A|    
       VMVW.4.NZ         vr13, vr04                    // DCPred DC_left [  358] // move left accordingly
        VMR4W.64        vr08, vr13, vr13              // Pred_bk3 DCPred [  359] 
     // have DCprediction for block 3
    
LINE 362".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VSUBW.64          vr07, vr07, vr08                // Res Res Pred [  362] // subtract prediction from actual
    
       VMUL.192          vr11, vr10, vr07    // workarea DCFwdScaler Res [  364] // quantiser with current dc-scaler
 
       VASRR.192         vr11, vr11, 20             // workarea workarea [  366] // add 1<<19
    
       VMVW              vr09, vr11, 64               // DCDiff workarea [  368] // save it to DCDiff_bk0
    
       VMULW.64          vr07, vr09, vr12         // Res DCDiff ScaleVec [  370] // 'DCScaler // mult diff by dc_scaler
    
       VADDW.64          vr07, vr07, vr08                // Res Res Pred [  372] // add back prediction    
    // end of block 3
    
    // now we must calculate the cost of encoding all 4 DC terms
       VMOVW             vr15, 256+SDMOF_mpeg4_DC_bits_plus_256, 255 // Offset [  376] 
       VADDW.85          vr09, vr09, vr15        // DCDiff DCDiff Offset [  377] // add the LUT offset to each DC difference
       VMVW              vr07, vr62, 255                    // Res VZERO [  378] // zero the result area
   // vsubw       Res, Res, Res // clear Res
        VMR7W.2         vr00, vr09, vr09            // cirPtr DCDiff_bk0 [  380] 
LINE 381".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD8W             vr07,[i1, 0]                  // Res_bk0 cirPtr [  381] // read correct entry of lookup table - number of bits
        VMR1W.2         vr00, vr09, vr09            // cirPtr DCDiff_bk1 [  382] 
LINE 383".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD8W_2           vr07,[i1, 0]                  // Res_bk1 cirPtr [  383] 
        VMR3W.2         vr00, vr09, vr09            // cirPtr DCDiff_bk2 [  384] 
LINE 385".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD8W_4           vr07,[i1, 0]                  // Res_bk2 cirPtr [  385] 
        VMR5W.2         vr00, vr09, vr09            // cirPtr DCDiff_bk3 [  386] 
LINE 387".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VLD8W_6           vr07,[i1, 0]                  // Res_bk3 cirPtr [  387] 
    
       VMOVW             vr07, 5, 128                             // Res [  389] // mode cost of 5  - put in top lane (7)
       VXSUMW.213        vr07, vr07, 255                      // Res Res [  390] //(add the 4 blocks plus the 5 - lanes 0, 2, 4, 6, 7)
    
       VMULW.1           vr07, vr07, vr04       // Res_bk0 Res_bk0 Quant [  392] // multiply by lambda = quant
       VADDW.1           vr01, vr01, vr07           // cost cost Res_bk0 [  393] // add to SAD cost
    
       VST16             vr01,[i4, MPO_WorkArea_BestIntraLumaCost] // cost CirBuf [  395] // save in circular buffer


    // signal end of simd code
//@   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
//@   ld                  r0, [r0, 0]
//@   mov                 r1, Service_IntraComplete
//    vsend               r0, r1, 63
// End                                                                // [  403] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz IntraModeCostmp4_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc IntraModeCostmp4                                           // [  405] 


//------------------------------------------------------------------------------
// Name:          IntraModeCost263
// Purpose:       Calculates the Intra cost for H263 to compare against inter cost for mode decision
//                This requires a SAD calculation as well as the number of bits used for luma DC terms (8x4)
// Arguments:     None
//                
// Return Value:  void, return value is stored in Circular Buffer
//------------------------------------------------------------------------------

// [Macro] IntraModeCost263                                           // [  415] 
_Asm int Record_IntraModeCost263(int *mac_tab)
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
IntraModeCost263_RecLoop:
  ld r1,[r17 ,8]
  vrec r1


// Begin                                                              // [  417] 
    // function arguments
// ****** Informational : Register space for symbol "raw_dataPtr" already allocated. // [  419] // has to be same as in subroutine CalculateDC8
// p16    i7  raw_dataPtr                                             // [  419] 
// p16    i0  block_no                                                // [  420] 

// p16    i1  cirPtr                                                  // [  422] 
// s16    vr04.2  blkInOffset                                         // [  423] 
// p16    i8  cost                                                    // [  424] 
// s16    vr04.1  QuantVal                                            // [  425] 
// p16    i2  temp                                                    // [  426] 
// p16    i9  blockdouble                                             // [  427] 
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0| raw_.| CirL.|FixBuf|CirBuf| stan.|  temp|cirPtr| bloc.|
//|VR 1|      |      |      |      |      |      | bloc.|  cost|
//|VR 2|                    gDCArray                           |
//|VR 3|                      sadval                           |
//|VR 4|      |      |      |      |      |      | blkI.| Quan.|
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

LINE 429".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VMOVW             vr00, 0, 1                          // block_no [  429] 
       VMOVW             vr01, 0, 1                              // cost [  430] 
       VMOVW             vr00, 0, 4                              // temp [  431] 
   
  // main block loop starts here
//MainLoop --------                                                   // [  434] 
 vendrec r3                                                           // [  434] 
 vrec r3                                                              // [  434] 
  
  
        VMR7W.2         vr01, vr00, vr00         // blockdouble block_no [  437] 
LINE 438".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VADDW.2           vr01, vr01, vr01 // blockdouble blockdouble blockdouble [  438] // needed for indexing short arrays

       VIM               i1, i9, SDMOF_mpeg4_raw_pixel_offset // cirPtr blockdouble [  440] 
  
       VLD16_1           vr04,[i1, 0]              // blkInOffset cirPtr [  442] 
    // set the location of the actual pixels
  ld r1,[r17,0];                                  // VSTACK CalculateDC8 [  444] 
       VJL               vr31, r1                                     // [  444] 
       VIM               i1, i4, MPO_PixelCoeffBuffer  //DS cirPtr CirBuf [  445] 
       VADDW.2           vr00, vr00, vr04  //DS cirPtr cirPtr blkInOffset [  446] 
        VMR2W.128       vr00, vr00, vr00  //DS_______ raw_dataPtr cirPtr [  447] // setup data pointer for CalculateDC8


// end of Main Block Loop
LINE 451".\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss"
       VIM               i0, i0, 1                  // block_no block_no [  451] 
// check to see if we have done all blocks
       VIM               i2, i0, -4                     // temp block_no [  453] 
    
       VJP.i2            r3                             // temp MainLoop [  455] 
       VADDW.1           vr01, vr01, vr03  //DS________ cost cost sadval [  456] // increment cost by the sad for the block
       VNOP                //DS_________________________________________ [  457] 
       VNOP                //DS_________________________________________ [  458] 
    // now add mode cost
    //if 263 add 37*Quant (four lots of 8 DC bits plus 5 mode bits)
    
       VLD8W             vr04,[i4, MPO_QuantValue]    // QuantVal CirBuf [  462] 
       VMULW.1           vr04, vr04, 37             // QuantVal QuantVal [  463] 
       VADDW.1           vr01, vr01, vr04          // cost cost QuantVal [  464] 
    
       VST16             vr01,[i4, MPO_WorkArea_BestIntraLumaCost] // cost CirBuf [  466] 


    // signal end of simd code
//@   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
//@   ld                  r0, [r0, 0]
//@   mov                 r1, Service_IntraComplete
//    vsend               r0, r1, 63
// End                                                                // [  474] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz IntraModeCost263_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc IntraModeCost263                                           // [  476] 









const int SIMD_IDX_CalculateDC8 = 0;
const int MACRO_IDX_IntraModeCostmp4 = 1;
const int MACRO_IDX_IntraModeCost263 = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int IntraMP4LumaPrediction_Record(int sa, int SIMDnum)
{
    int *map = IntraMP4LumaPrediction;

    SetSCQ(SIMDnum);

    map[SIMD_IDX_CalculateDC8] = sa;
    sa = Record_CalculateDC8(map);

    map[MACRO_IDX_IntraModeCostmp4] = sa;
    sa = Record_IntraModeCostmp4(map);

    map[MACRO_IDX_IntraModeCost263] = sa;
    sa = Record_IntraModeCost263(map);

    return sa;
}
