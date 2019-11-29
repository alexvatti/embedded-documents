//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss'

//debuginfo
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


// Stall info ON                                                      // [   26] 
// Stall info format: Stall [Reg in Stall] ...... List of Registers currently busy  

// MetaWare ouput ON                                                                                        // [   28] 
#define LINE #line                                                                                          // [   28] 

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

#include ".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.h"


// Macro table
    int IntraLumaPredictionFast[3];
    int IntraLumaPredictionFast_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Global Function Arguments
// p16    i0  intraValidFlag                                                                                // [   39] ....................// same as chroma
// p16    i1  intra_16x16_mode                                                                              // [   41] 
// p16    i2  intraLumaCost                                                                                 // [   42] 
// p16    i9  intra_16x16_modeOffset                                                                        // [   43] ....................// needs to be in the same lane as intra_16x16_mode = i1




//------------------------------------------------------------------------------
// Name:          GenerateIntra16x16PredictionFastAll
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] GenerateIntra16x16PredictionFastAll                                                              // [   54] 
_Asm int Record_GenerateIntra16x16PredictionFastAll(int *mac_tab)
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
GenerateIntra16x16PredictionFastAll_RecLoop:
  ld r1,[r17 ,0]
  vrec r1

    // function arguments
    // intraValidFlag = i0

// Begin                                                                                                    // [   58] ....................//1
// p16    i3  aboveOk                                                                                       // [   59] 
// p16    i7  leftOk                                                                                        // [   60] 

// p16    i8  predPtr                                                                                       // [   62] 

// p16    i10  lambdaMult10_30                                                                              // [   64] 

//vec16  vr02  vertPred8                                                                                    // [   66] 
//vec16  vr03  vertPred16Lo                                                                                 // [   67] 
//vec16  vr03  vertPred16Lo_Term0.1                                                                         // [   67] 
//vec16  vr03  vertPred16Lo_Term1.2                                                                         // [   67] 
//vec16  vr03  vertPred16Lo_Term2.4                                                                         // [   67] 
//vec16  vr03  vertPred16Lo_Term3.8                                                                         // [   67] 
//vec16  vr03  vertPred16Lo_Term4.16                                                                        // [   67] 
//vec16  vr03  vertPred16Lo_Term5.32                                                                        // [   67] 
//vec16  vr03  vertPred16Lo_Term6.64                                                                        // [   67] 
//vec16  vr03  vertPred16Lo_Term7.128                                                                       // [   67] 
//vec16  vr04  vertPred16Hi                                                                                 // [   68] 
//vec16  vr04  vertPred16Hi_Term0.1                                                                         // [   68] 
//vec16  vr04  vertPred16Hi_Term1.2                                                                         // [   68] 
//vec16  vr04  vertPred16Hi_Term2.4                                                                         // [   68] 
//vec16  vr04  vertPred16Hi_Term3.8                                                                         // [   68] 
//vec16  vr04  vertPred16Hi_Term4.16                                                                        // [   68] 
//vec16  vr04  vertPred16Hi_Term5.32                                                                        // [   68] 
//vec16  vr04  vertPred16Hi_Term6.64                                                                        // [   68] 
//vec16  vr04  vertPred16Hi_Term7.128                                                                       // [   68] 

//vec16  vr05  horizPred16Lo                                                                                // [   70] 
//vec16  vr05  horizPred16Lo_Term0.1                                                                        // [   70] 
//vec16  vr05  horizPred16Lo_Term1.2                                                                        // [   70] 
//vec16  vr05  horizPred16Lo_Term2.4                                                                        // [   70] 
//vec16  vr05  horizPred16Lo_Term3.8                                                                        // [   70] 
//vec16  vr05  horizPred16Lo_Term4.16                                                                       // [   70] 
//vec16  vr05  horizPred16Lo_Term5.32                                                                       // [   70] 
//vec16  vr05  horizPred16Lo_Term6.64                                                                       // [   70] 
//vec16  vr05  horizPred16Lo_Term7.128                                                                      // [   70] 
//vec16  vr06  horizPred16Hi                                                                                // [   71] 
//vec16  vr06  horizPred16Hi_Term0.1                                                                        // [   71] 
//vec16  vr06  horizPred16Hi_Term1.2                                                                        // [   71] 
//vec16  vr06  horizPred16Hi_Term2.4                                                                        // [   71] 
//vec16  vr06  horizPred16Hi_Term3.8                                                                        // [   71] 
//vec16  vr06  horizPred16Hi_Term4.16                                                                       // [   71] 
//vec16  vr06  horizPred16Hi_Term5.32                                                                       // [   71] 
//vec16  vr06  horizPred16Hi_Term6.64                                                                       // [   71] 
//vec16  vr06  horizPred16Hi_Term7.128                                                                      // [   71] 

//vec16  vr07  dcPred8                                                                                      // [   73] 
//vec16  vr08  dcPred16                                                                                     // [   74] 

//vec16  vr09  intraValidFlagVec                                                                            // [   76] 

//vec16  vr10  vertSad                                                                                      // [   78] 
//vec16  vr11  horizSad                                                                                     // [   79] 
//vec16  vr12  dcSad                                                                                        // [   80] 

// Begin                                                                                                    // [   82] ....................// 2
// p16    i11  quantValue                                                                                   // [   83] 
// p16    i12  lambdaLutPtr                                                                                 // [   84] 

    // get quantiser value
LINE 87".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD8W_3           vr01,[i4, MPO_QuantValue]                                        // quantValue CirBuf [   87] 

    // broadcast intraValidFlag across all lanes
       VMIVW             vr09, i0                                          // intraValidFlagVec intraValidFlag [   90] 

    // zero dc_prediction, as it's formed by accumulation
       VMOVW             vr08, 0, 255                                                              // dcPred16 [   93] 

    // check if above is valid
       VIA               i3, i0, 2                                                   // aboveOk intraValidFlag [   96] 

    // setup global value pointers
       VIM               i12, i11, SDMOF_h264_encoder_rd_sad_lambda                 // lambdaLutPtr quantValue [   99] 

    // check if left is valid
       VIA               i7, i0, 1                                                    // leftOk intraValidFlag [  102] 

    // setup global lambda value
       VLD8W_2           vr01,[i12, 0]                                         // lambdaMult10_30 lambdaLutPtr [  105] 

    // reset dc sad
       VMOVW             vr12, 0, 255                                                                 // dcSad [  108] 

    // if above or left is valid then set corresponding lane to 0xffff
       VNEW.136          vr00, vr00, vr62                               // aboveOk'leftOk aboveOk'leftOk VZERO [  111] 

    // set vertical sad to max value
   mov                 r0, 0xffff
       VMOVW             vr10, r0, 255                                                              // vertSad [  115] 

    // set horizontal sad to max value
   mov                 r0, 0xffff
       VMOVW             vr11, r0, 255                                                             // horizSad [  119] 

    // we do a final adjust of the cost by adding 10 times lambda
       VMULW.4           vr01, vr01, 10                                     // lambdaMult10_30 lambdaMult10_30 [  122] 

// End                                                                                                      // [  124] ....................// 2

    // setup source pointer for horizontal
LINE 127".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VIM.i7            i8, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+PCB_RECONSTRUCTED_STRIDE-1 // leftOk predPtr CirLeft [  127] 



    // vertical prediction (mode = 0)
//verticalPrediction --------                                                                               // [  132] 
 vendrec r3                                                                                                 // [  132] 
 vrec r3                                                                                                    // [  132] 

    // check if above is valid
LINE 135".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VJP!i3            r4                                                    // aboveOk horizontalPrediction [  135] 

    // delay slots start

    // get intra prediction pixels for A
       VLD64W.i3         vr03,[i5, MPO_Y_IntraPredPixel+0]  //DS__________________ aboveOk vertPred16Lo FixBuf [  140] 
       VLD64W.i3         vr04,[i5, MPO_Y_IntraPredPixel+8]  //DS__________________ aboveOk vertPred16Hi FixBuf [  141] 
       VLD128.i3         vr02,[i5, MPO_Y_IntraPredPixel+0]  //DS_____________________ aboveOk vertPred8 FixBuf [  142] 

    // delay slots end
    

    // following can't be in delay slots
    // reset sad if doing vertical
       VMOVW             vr10, 0, 255                                                               // vertSad [  149] 

    // following can't be in delay slots
    // accumulate intra prediction pixels A into dc prediction vector
       VADDW             vr08, vr03, vr04                                // dcPred16 vertPred16Lo vertPred16Hi [  153] 


    // horizontal prediction (mode = 1)
//horizontalPrediction --------                                                                             // [  157] 
 vendrec r4                                                                                                 // [  157] 
 vrec r4                                                                                                    // [  157] 

    // check if left is valid
LINE 160".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VJP!i7            r5                                                             // leftOk dcPrediction [  160] 

    // delay slots start

    // load 8 bit predictions
       VLD8W.i7          vr05,[i8, 0*PCB_RECONSTRUCTED_STRIDE]  //DS_______ leftOk horizPred16Lo_Term0 predPtr [  165] 
       VLD8W_1.i7        vr05,[i8, 1*PCB_RECONSTRUCTED_STRIDE]  //DS_______ leftOk horizPred16Lo_Term1 predPtr [  166] 
       VLD8W_2.i7        vr05,[i8, 2*PCB_RECONSTRUCTED_STRIDE]  //DS_______ leftOk horizPred16Lo_Term2 predPtr [  167] 

    // delay slots end


       VLD8W_3           vr05,[i8, 3*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Lo_Term3 predPtr [  172] 
       VLD8W_4           vr05,[i8, 4*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Lo_Term4 predPtr [  173] 
       VLD8W_5           vr05,[i8, 5*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Lo_Term5 predPtr [  174] 
       VLD8W_6           vr05,[i8, 6*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Lo_Term6 predPtr [  175] 
       VLD8W_7           vr05,[i8, 7*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Lo_Term7 predPtr [  176] 

       VLD8W             vr06,[i8, 8*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Hi_Term0 predPtr [  178] 
       VLD8W_1           vr06,[i8, 9*PCB_RECONSTRUCTED_STRIDE]                  // horizPred16Hi_Term1 predPtr [  179] 
       VLD8W_2           vr06,[i8, 10*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term2 predPtr [  180] 
       VLD8W_3           vr06,[i8, 11*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term3 predPtr [  181] 
       VLD8W_4           vr06,[i8, 12*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term4 predPtr [  182] 
       VLD8W_5           vr06,[i8, 13*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term5 predPtr [  183] 
       VLD8W_6           vr06,[i8, 14*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term6 predPtr [  184] 
       VLD8W_7           vr06,[i8, 15*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term7 predPtr [  185] 

    // accumulate intra prediction pixel
              //  Stall: [vr06]  ...............................      vr06.224
              //  Stall: [vr06]  ...............................      vr06.192
              //  Stall: [vr06]  ...............................      vr06.128
       VADDW             vr62, vr05, vr06                                 // VZERO horizPred16Lo horizPred16Hi [  188] 

    // following can't be in delay slots
    // reset sad if doing horizontal
       VMOVW             vr11, 0, 255                                                              // horizSad [  192] 

    // accumulate prediction into dc prediction vector
       VADDAW            vr08, vr08, vr62                                           // dcPred16 dcPred16 VZERO [  195] 


    // dc prediction (mode = 2)
//dcPrediction --------                                                                                     // [  199] 
 vendrec r5                                                                                                 // [  199] 
 vrec r5                                                                                                    // [  199] 

    // compare intraValidFlag to see if neither above and left are available and
    // broadcast flag across all lanes
LINE 203".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VSUBW.255.F       vr62, vr09, 0                                              // VZERO intraValidFlagVec [  203] 

    // clear the sticky flags
       VCLRSTK.255                                                                                          // [  206] 

    // set the sticky flags if both above and left are available
       VSEQW.255         vr09, 3                                                          // intraValidFlagVec [  209] 

    // setup source value
    // if neither above or left are available then use default of 128
   mov                 r0, 0x0080
       VMOVW.255.Z       vr08, r0                                                                  // dcPred16 [  214] 

    // sum all lanes of dc_prediction vector and broadcast across whole vector
              //  Stall: [vr08]  ...............................      vr08          vr63    
       VXSUMW.255.NZ     vr08, vr08, 0xff                                                 // dcPred16 dcPred16 [  217] 

    // if both above and left are available then add rounding of 16 and divide by 32
              //  Stall: [vr08]  ...............................      vr08    
              //  Stall: [vr08]  ...............................      vr08    
       VASRRW.255.S.NZ   vr08, vr08, 5                                                    // dcPred16 dcPred16 [  220] 
    
    // if only one of above or left are available then add rounding of 8 and divide by 16
              //  Stall: [vr08]  ...............................      vr08    
              //  Stall: [vr08]  ...............................      vr08    
       VASRRW.255.U.NZ   vr08, vr08, 4                                                    // dcPred16 dcPred16 [  223] 

    // duplicate prediction into both bytes of the 16 bit lanes to for the 8 bit version
              //  Stall: [vr08]  ...............................      vr08    
              //  Stall: [vr08]  ...............................      vr08    
       VMULW.255         vr07, vr08, 0x0101                                                // dcPred8 dcPred16 [  226] 


    // calculate sads
// Begin                                                                                                    // [  230] ....................// 2
// p16    i11  srcPtr                                                                                       // [  231] 
// p16    i12  resPtr                                                                                       // [  232] 
// p16    i13  horizPtr                                                                                     // [  233] 

// p16    i14  row                                                                                          // [  235] 

//vec16  vr13  source0                                                                                      // [  237] 
//vec16  vr14  source1                                                                                      // [  238] 
//vec16  vr15  source2                                                                                      // [  239] 
//vec16  vr16  source3                                                                                      // [  240] 

//vec16  vr17  residual0                                                                                    // [  242] 
//vec16  vr18  residual1                                                                                    // [  243] 
//vec16  vr19  residual2                                                                                    // [  244] 
//vec16  vr20  residual3                                                                                    // [  245] 

//vec16  vr21  horizPred16A                                                                                 // [  247] 
//vec16  vr22  horizPred16B                                                                                 // [  248] 

    // setup loop counter
    // 16 rows total, 2 rows per loop
LINE 252".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VMOVW             vr01, (16/2)-1, 64                                                             // row [  252] 

    // setup arguments for sad calculation
       VIM               i11, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                          // srcPtr CirBuf [  255] 

    // only use predPtr for horizontal sad calculation
       VIM               i8, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+PCB_RECONSTRUCTED_STRIDE-1 // predPtr CirLeft [  258] 

// Begin                                                                                                    // [  260] ....................// 3
//vec16  vr23  vertTemp                                                                                     // [  261] 
//vec16  vr24  horizTemp                                                                                    // [  262] 
//vec16  vr25  dcTemp                                                                                       // [  263] 


    // sadLoop start
//sadLoop --------                                                                                          // [  267] 
 vendrec r6                                                                                                 // [  267] 
 vrec r6                                                                                                    // [  267] 

    // load current (8 bits per pixel)
LINE 270".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD64W            vr13,[i11, 0]                                                       // source0 srcPtr [  270] 
       VLD64W            vr14,[i11, 8]                                                       // source1 srcPtr [  271] 
       VLD64W            vr15,[i11, 16]                                                      // source2 srcPtr [  272] 
       VLD64W            vr16,[i11, 24]                                                      // source3 srcPtr [  273] 


    // accumulate vertical sad by calculating residual (9 bits stored in 16 bits per pixel)
    // and then absoluting before accumulating
       VDIFW             vr62, vr13, vr03                                        // VZERO source0 vertPred16Lo [  278] 
       VDIFAW            vr62, vr14, vr04                                        // VZERO source1 vertPred16Hi [  279] 
       VDIFAW            vr62, vr15, vr03                                        // VZERO source2 vertPred16Lo [  280] 
       VDIFAW            vr23, vr16, vr04                                     // vertTemp source3 vertPred16Hi [  281] 


    // duplicate horizontal prediction into all vector lanes
       VXSUMW.255        vr21, vr05, 1                                     // horizPred16A horizPred16Lo_Term0 [  285] 
       VXSUMW.255        vr22, vr05, 2                                     // horizPred16B horizPred16Lo_Term1 [  286] 

    // shift vectors down by 2 lanes to get the next terms in lane 0 and 1
       VMR2W             vr05, vr06, vr05                         // horizPred16Lo horizPred16Hi horizPred16Lo [  289] 
       VMR2W             vr06, vr62, vr06                                 // horizPred16Hi VZERO horizPred16Hi [  290] 

    // accumulate horizontal sad by calculating residual (9 bits stored in 16 bits per pixel)
    // and then absoluting before accumulating
       VDIFW             vr62, vr13, vr21                                        // VZERO source0 horizPred16A [  294] 
       VDIFAW            vr62, vr14, vr21                                        // VZERO source1 horizPred16A [  295] 
       VDIFAW            vr62, vr15, vr22                                        // VZERO source2 horizPred16B [  296] 
       VDIFAW            vr24, vr16, vr22                                    // horizTemp source3 horizPred16B [  297] 


    // accumulate dc sad by calculating residual (9 bits stored in 16 bits per pixel)
    // and then absoluting before accumulating
       VDIFW             vr62, vr13, vr08                                            // VZERO source0 dcPred16 [  302] 
       VDIFAW            vr62, vr14, vr08                                            // VZERO source1 dcPred16 [  303] 
       VDIFAW            vr62, vr15, vr08                                            // VZERO source2 dcPred16 [  304] 
       VDIFAW            vr25, vr16, vr08                                           // dcTemp source3 dcPred16 [  305] 

    // increament pointers
       VIM               i11, i11, PCB_CURRENT_STRIDE*2                                       // srcPtr srcPtr [  308] ....................// 16 pixels, 1 byte per pixel, 2 rows

    // decrement and loop
       VJD.i14           i14, r6                                                            // row row sadLoop [  311] 

    // delay slots start

    // sum temporary accumulated sads into total sads
       VADDW.i3          vr10, vr10, vr23  //DS______________________________ aboveOk vertSad vertSad vertTemp [  316] 
       VADDW.i7          vr11, vr11, vr24  //DS____________________________ leftOk horizSad horizSad horizTemp [  317] 
       VADDW             vr12, vr12, vr25  //DS____________________________________________ dcSad dcSad dcTemp [  318] 

    // delay slots end


// End                                                                                                      // [  323] ....................// 3

    // sum across all lanes of the sads back into all the lanes, so that compare will set flags across all lanes
LINE 326".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VXSUMW.i3         vr10, vr10, 0xff                                           // aboveOk vertSad vertSad [  326] 
       VXSUMW.i7         vr11, vr11, 0xff                                          // leftOk horizSad horizSad [  327] 
       VXSUMW.255        vr12, vr12, 0xff                                                       // dcSad dcSad [  328] 

// Begin                                                                                                    // [  330] ....................// 3
//vec16  vr23  constHex8000                                                                                 // [  331] 

    // exclusive-or the sad values to force 16 bit unsigned values into 16 bit signed values that can be compared
LINE 334".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VMOVW             vr23, 0x8000, 255                                                     // constHex8000 [  334] 
              //  Stall: [vr23]  ...............................      vr12          vr23    
       VXOR              vr10, vr10, vr23                                      // vertSad vertSad constHex8000 [  335] 
       VXOR              vr11, vr11, vr23                                    // horizSad horizSad constHex8000 [  336] 
       VXOR              vr12, vr12, vr23                                          // dcSad dcSad constHex8000 [  337] 


    // compare vertical and horizontal sads
       VSUBW.F           vr62, vr10, vr11                                            // VZERO vertSad horizSad [  341] 

    // if sad comparison is less than or equal then choose vertical
              //  Stall: [VFLAGS]   ............................      vr62          vr63    
       VMVW.4.LE         vr00, vr10                                                   // intraLumaCost vertSad [  344] 

    // else choose horizontal
       VMVW.4.GT         vr00, vr11                                                  // intraLumaCost horizSad [  347] 

    // if sad comparison is less than or equal then choose vertical
       VMOVW.2.LE        vr00, MBIM_H264_Y_VERTICAL                                        // intra_16x16_mode [  350] ....................// setup global intra16x16 mode value

    // else choose horizontal
       VMOVW.2.GT        vr00, MBIM_H264_Y_HORIZONTAL                                      // intra_16x16_mode [  353] ....................// setup global intra16x16 mode value

    // compare dc and best so far sads
       VSUBW.F           vr62, vr12, i2                                           // VZERO dcSad intraLumaCost [  356] 

    // if sad comparison is less than then choose dc, otherwise stick with the best so far
              //  Stall: [VFLAGS]   ............................      vr62          vr63    
       VMOVW.2.LT        vr00, MBIM_H264_Y_DC                                              // intra_16x16_mode [  359] ....................// setup global intra16x16 mode value
       VMVW.4.LT         vr00, vr12                                                     // intraLumaCost dcSad [  360] 

    // exclusive-or the best sad value to restore to a 16 bit unsigned value
              //  Stall: [vr00]  ...............................      vr00.004      vr63    
       VXOR.4            vr00, vr00, vr23                          // intraLumaCost intraLumaCost constHex8000 [  363] 

// End                                                                                                      // [  365] ....................// 3


    // setup offset for prediction pointer
LINE 369".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VMULW.2           vr01, vr00, (16*16)                        // intra_16x16_modeOffset intra_16x16_mode [  369] ....................// 1 byte per pixel

// Begin                                                                                                    // [  371] ....................// 3
// p16    i15  jumpFlag                                                                                     // [  372] 

    // setup jump flag
LINE 375".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VIV.2             i15, vr00                                                // jumpFlag intra_16x16_mode [  375] 

    // setup source pointer for residual generation
       VIM               i11, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                          // srcPtr CirBuf [  378] 

    // setup prediction pointer for residual generation
       VIM               i8, i9, SDMOF_IntraPredictionLuma                   // predPtr intra_16x16_modeOffset [  381] 


    // if best intra mode is 0 then choose vertical
       VJP!i15           r7                                                         // jumpFlag verticalResult [  385] 

    // delay slots start

    // setup offset for residual pointer
       VMULW.2           vr01, vr00, (16*16*2)  //DS__________________ intra_16x16_modeOffset intra_16x16_mode [  390] ....................// 2 bytes per pixel

    // decreament jmupFlag and try next mode
       VIM               i15, i15, -1  //DS_________________________________________________ jumpFlag jumpFlag [  393] 

    // setup loop counter
    // 16 rows total, 2 rows per loop
       VMOVW             vr01, (16/2)-1, 64  //DS_________________________________________________________ row [  397] 

    // delay slots end


    // if best intra mode is 1 then choose horizontal
       VJP!i15           r8                                                       // jumpFlag horizontalResult [  403] 

    // delay slots start

    // setup source pointer
       VIM               i13, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+PCB_RECONSTRUCTED_STRIDE-1 //DS horizPtr CirLeft [  408] 

    // setup residual pointer for residual generation
       VIM               i12, i9, SDMOF_IntraPredictionResidsLuma  //DS_________ resPtr intra_16x16_modeOffset [  411] 

    // nothing else to setup before starting horizontal result loop
       VNOP                //DS_______________________________________________________________________________ [  414] 

    // delay slots end


    // if best intra mode is 0 then choose dc

// End                                                                                                      // [  421] ....................// 3

    // dc prediction choosen
//dcResult --------                                                                                         // [  424] 
 vendrec r9                                                                                                 // [  424] 
 vrec r9                                                                                                    // [  424] 

    // nothing else to setup before starting dc result loop


    // dcResidLoop start
//dcResidLoop --------                                                                                      // [  430] 
 vendrec r10                                                                                                // [  430] 
 vrec r10                                                                                                   // [  430] 

    // load current (8 bits per pixel)
LINE 433".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD64W            vr13,[i11, 0]                                                       // source0 srcPtr [  433] 
       VLD64W            vr14,[i11, 8]                                                       // source1 srcPtr [  434] 
       VLD64W            vr15,[i11, 16]                                                      // source2 srcPtr [  435] 
       VLD64W            vr16,[i11, 24]                                                      // source3 srcPtr [  436] 

    // store prediction (8 bits per pixel)
       VST128            vr07,[i8, 0]                                                       // dcPred8 predPtr [  439] 
       VST128            vr07,[i8, 16]                                                      // dcPred8 predPtr [  440] 

    // generate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr17, vr13, vr08                                        // residual0 source0 dcPred16 [  443] 
       VSUBW             vr18, vr14, vr08                                        // residual1 source1 dcPred16 [  444] 
       VSUBW             vr19, vr15, vr08                                        // residual2 source2 dcPred16 [  445] 
       VSUBW             vr20, vr16, vr08                                        // residual3 source3 dcPred16 [  446] 

    // store residual (16 bits per pixel)
       VST128            vr17,[i12, 0]                                                     // residual0 resPtr [  449] 
       VST128            vr18,[i12, 16]                                                    // residual1 resPtr [  450] 
       VST128            vr19,[i12, 32]                                                    // residual2 resPtr [  451] 
       VST128            vr20,[i12, 48]                                                    // residual3 resPtr [  452] 

    // decrement and loop
       VJD.i14           i14, r10                                                       // row row dcResidLoop [  455] 

    // delay slots start

    // increament pointers
       VIM               i11, i11, PCB_CURRENT_STRIDE*2  //DS___________________________________ srcPtr srcPtr [  460] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i8, i8, 16*2  //DS___________________________________________________ predPtr predPtr [  461] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i12, i12, 16*2*2  //DS_________________________________________________ resPtr resPtr [  462] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


    // jump to end
       VJP               r11                                                                           // exit [  468] 

    // delay slots start

    // scale final cost for comparison against inter
       VADDW.4           vr00, vr00, i10  //DS____________________ intraLumaCost intraLumaCost lambdaMult10_30 [  473] 

    // save mode
       VST8_1            vr00,[i4, MPO_WorkArea_BestIntraLumaMode]  //DS______________ intra_16x16_mode CirBuf [  476] 

    // save cost
       VST16_2           vr00,[i4, MPO_WorkArea_BestIntraLumaCost]  //DS_________________ intraLumaCost CirBuf [  479] 

    // delay slots end


    // vertical prediction choosen
//verticalResult --------                                                                                   // [  485] 
 vendrec r7                                                                                                 // [  485] 
 vrec r7                                                                                                    // [  485] 

    // setup residual pointer for residual generation
LINE 488".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VIM               i12, i9, SDMOF_IntraPredictionResidsLuma             // resPtr intra_16x16_modeOffset [  488] 

    // vertResidLoop start
//vertResidLoop --------                                                                                    // [  491] 
 vendrec r12                                                                                                // [  491] 
 vrec r12                                                                                                   // [  491] 

    // load current (8 bits per pixel)
LINE 494".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD64W            vr13,[i11, 0]                                                       // source0 srcPtr [  494] 
       VLD64W            vr14,[i11, 8]                                                       // source1 srcPtr [  495] 
       VLD64W            vr15,[i11, 16]                                                      // source2 srcPtr [  496] 
       VLD64W            vr16,[i11, 24]                                                      // source3 srcPtr [  497] 

    // store prediction (8 bits per pixel)
       VST128            vr02,[i8, 0]                                                     // vertPred8 predPtr [  500] 
       VST128            vr02,[i8, 16]                                                    // vertPred8 predPtr [  501] 

    // generate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr17, vr13, vr03                                    // residual0 source0 vertPred16Lo [  504] 
       VSUBW             vr18, vr14, vr04                                    // residual1 source1 vertPred16Hi [  505] 
       VSUBW             vr19, vr15, vr03                                    // residual2 source2 vertPred16Lo [  506] 
       VSUBW             vr20, vr16, vr04                                    // residual3 source3 vertPred16Hi [  507] 

    // store residual (16 bits per pixel)
       VST128            vr17,[i12, 0]                                                     // residual0 resPtr [  510] 
       VST128            vr18,[i12, 16]                                                    // residual1 resPtr [  511] 
       VST128            vr19,[i12, 32]                                                    // residual2 resPtr [  512] 
       VST128            vr20,[i12, 48]                                                    // residual3 resPtr [  513] 

    // decrement and loop
       VJD.i14           i14, r12                                                     // row row vertResidLoop [  516] 

    // delay slots start

    // increament pointers
       VIM               i11, i11, PCB_CURRENT_STRIDE*2  //DS___________________________________ srcPtr srcPtr [  521] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i8, i8, 16*2  //DS___________________________________________________ predPtr predPtr [  522] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i12, i12, 16*2*2  //DS_________________________________________________ resPtr resPtr [  523] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


    // jump to end
       VJP               r11                                                                           // exit [  529] 

    // delay slots start

    // scale final cost for comparison against inter
       VADDW.4           vr00, vr00, i10  //DS____________________ intraLumaCost intraLumaCost lambdaMult10_30 [  534] 

    // save mode
       VST8_1            vr00,[i4, MPO_WorkArea_BestIntraLumaMode]  //DS______________ intra_16x16_mode CirBuf [  537] 

    // save cost
       VST16_2           vr00,[i4, MPO_WorkArea_BestIntraLumaCost]  //DS_________________ intraLumaCost CirBuf [  540] 

    // delay slots end


    // horizontal prediction choosen
//horizontalResult --------                                                                                 // [  546] 
 vendrec r8                                                                                                 // [  546] 
 vrec r8                                                                                                    // [  546] 

    // all arguments have already been set up in the delay slots of the best cost decision

    // horizResidLoop start
//horizResidLoop --------                                                                                   // [  551] 
 vendrec r13                                                                                                // [  551] 
 vrec r13                                                                                                   // [  551] 

    // load 8 bit predictions into 16 bit lanes
LINE 554".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD8W             vr05,[i13, 0*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Lo_Term0 horizPtr [  554] 
       VLD8W             vr06,[i13, 1*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term0 horizPtr [  555] 

    // load current (8 bits per pixel)
       VLD64W            vr13,[i11, 0]                                                       // source0 srcPtr [  558] 
       VLD64W            vr14,[i11, 8]                                                       // source1 srcPtr [  559] 
       VLD64W            vr15,[i11, 16]                                                      // source2 srcPtr [  560] 
       VLD64W            vr16,[i11, 24]                                                      // source3 srcPtr [  561] 

    // duplicate horizontal prediction into all vector lanes
       VXSUMW.255        vr05, vr05, 1                                    // horizPred16Lo horizPred16Lo_Term0 [  564] 
       VXSUMW.255        vr06, vr06, 1                                    // horizPred16Hi horizPred16Hi_Term0 [  565] 

    // generate residual (9 bits stored in 16 bits per pixel)
              //  Stall: [vr05]  ...............................      vr05.001      vr06.001      vr16    
       VSUBW             vr17, vr13, vr05                                   // residual0 source0 horizPred16Lo [  568] 
       VSUBW             vr18, vr14, vr05                                   // residual1 source1 horizPred16Lo [  569] 
       VSUBW             vr19, vr15, vr06                                   // residual2 source2 horizPred16Hi [  570] 
       VSUBW             vr20, vr16, vr06                                   // residual3 source3 horizPred16Hi [  571] 

    // duplicate 8bit horizontal prediction into both bytes of the 16 bit lanes
       VMULW             vr05, vr05, 0x0101                                     // horizPred16Lo horizPred16Lo [  574] 
       VMULW             vr06, vr06, 0x0101                                     // horizPred16Hi horizPred16Hi [  575] 

    // store residual (16 bits per pixel)
       VST128            vr17,[i12, 0]                                                     // residual0 resPtr [  578] 
       VST128            vr18,[i12, 16]                                                    // residual1 resPtr [  579] 
       VST128            vr19,[i12, 32]                                                    // residual2 resPtr [  580] 
       VST128            vr20,[i12, 48]                                                    // residual3 resPtr [  581] 

    // store prediction (8 bits per pixel)
       VST128            vr05,[i8, 0]                                                 // horizPred16Lo predPtr [  584] 
       VST128            vr06,[i8, 16]                                                // horizPred16Hi predPtr [  585] 

    // increament pointers
       VIM               i13, i13, PCB_RECONSTRUCTED_STRIDE*2                             // horizPtr horizPtr [  588] ....................// 2 rows

    // decrement and loop
       VJD.i14           i14, r13                                                    // row row horizResidLoop [  591] 

    // delay slots start

    // increament pointers
       VIM               i11, i11, PCB_CURRENT_STRIDE*2  //DS___________________________________ srcPtr srcPtr [  596] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i8, i8, 16*2  //DS___________________________________________________ predPtr predPtr [  597] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i12, i12, 16*2*2  //DS_________________________________________________ resPtr resPtr [  598] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


    // scale final cost for comparison against inter
       VADDW.4           vr00, vr00, i10                        // intraLumaCost intraLumaCost lambdaMult10_30 [  604] 

    // save mode
       VST8_1            vr00,[i4, MPO_WorkArea_BestIntraLumaMode]                  // intra_16x16_mode CirBuf [  607] 

    // save cost
       VST16_2           vr00,[i4, MPO_WorkArea_BestIntraLumaCost]                     // intraLumaCost CirBuf [  610] 


//exit --------                                                                                             // [  613] 
 vendrec r11                                                                                                // [  613] 
 vrec r11                                                                                                   // [  613] 

// End                                                                                                      // [  615] ....................// 2

// End                                                                                                      // [  617] ....................// 1

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz GenerateIntra16x16PredictionFastAll_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x16PredictionFastAll                                                              // [  619] ....................// GenerateIntra16x16PredictionFastAll





//------------------------------------------------------------------------------
// Name:          GenerateIntra16x16PredictionFastDC
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] GenerateIntra16x16PredictionFastDC                                                               // [  629] 
_Asm int Record_GenerateIntra16x16PredictionFastDC(int *mac_tab)
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
GenerateIntra16x16PredictionFastDC_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

    // function arguments
    // intraValidFlag = i0

// Begin                                                                                                    // [  633] ....................//1
// p16    i3  aboveOk                                                                                       // [  634] 
// p16    i7  leftOk                                                                                        // [  635] 

// p16    i8  quantValue                                                                                    // [  637] 
// p16    i10  lambdaLutPtr                                                                                 // [  638] 
// p16    i11  lambdaMult10_30                                                                              // [  639] 

// p16    i12  predPtr                                                                                      // [  641] 
// p16    i13  srcPtr                                                                                       // [  642] 
// p16    i14  resPtr                                                                                       // [  643] 

// p16    i15  row                                                                                          // [  645] 


//vec16  vr02  vertPred16Lo                                                                                 // [  648] 
//vec16  vr02  vertPred16Lo_Term0.1                                                                         // [  648] 
//vec16  vr02  vertPred16Lo_Term1.2                                                                         // [  648] 
//vec16  vr02  vertPred16Lo_Term2.4                                                                         // [  648] 
//vec16  vr02  vertPred16Lo_Term3.8                                                                         // [  648] 
//vec16  vr02  vertPred16Lo_Term4.16                                                                        // [  648] 
//vec16  vr02  vertPred16Lo_Term5.32                                                                        // [  648] 
//vec16  vr02  vertPred16Lo_Term6.64                                                                        // [  648] 
//vec16  vr02  vertPred16Lo_Term7.128                                                                       // [  648] 
//vec16  vr03  vertPred16Hi                                                                                 // [  649] 
//vec16  vr03  vertPred16Hi_Term0.1                                                                         // [  649] 
//vec16  vr03  vertPred16Hi_Term1.2                                                                         // [  649] 
//vec16  vr03  vertPred16Hi_Term2.4                                                                         // [  649] 
//vec16  vr03  vertPred16Hi_Term3.8                                                                         // [  649] 
//vec16  vr03  vertPred16Hi_Term4.16                                                                        // [  649] 
//vec16  vr03  vertPred16Hi_Term5.32                                                                        // [  649] 
//vec16  vr03  vertPred16Hi_Term6.64                                                                        // [  649] 
//vec16  vr03  vertPred16Hi_Term7.128                                                                       // [  649] 

//vec16  vr04  horizPred16Lo                                                                                // [  651] 
//vec16  vr04  horizPred16Lo_Term0.1                                                                        // [  651] 
//vec16  vr04  horizPred16Lo_Term1.2                                                                        // [  651] 
//vec16  vr04  horizPred16Lo_Term2.4                                                                        // [  651] 
//vec16  vr04  horizPred16Lo_Term3.8                                                                        // [  651] 
//vec16  vr04  horizPred16Lo_Term4.16                                                                       // [  651] 
//vec16  vr04  horizPred16Lo_Term5.32                                                                       // [  651] 
//vec16  vr04  horizPred16Lo_Term6.64                                                                       // [  651] 
//vec16  vr04  horizPred16Lo_Term7.128                                                                      // [  651] 
//vec16  vr05  horizPred16Hi                                                                                // [  652] 
//vec16  vr05  horizPred16Hi_Term0.1                                                                        // [  652] 
//vec16  vr05  horizPred16Hi_Term1.2                                                                        // [  652] 
//vec16  vr05  horizPred16Hi_Term2.4                                                                        // [  652] 
//vec16  vr05  horizPred16Hi_Term3.8                                                                        // [  652] 
//vec16  vr05  horizPred16Hi_Term4.16                                                                       // [  652] 
//vec16  vr05  horizPred16Hi_Term5.32                                                                       // [  652] 
//vec16  vr05  horizPred16Hi_Term6.64                                                                       // [  652] 
//vec16  vr05  horizPred16Hi_Term7.128                                                                      // [  652] 

//vec16  vr06  dcPred8                                                                                      // [  654] 
//vec16  vr07  dcPred16                                                                                     // [  655] 

//vec16  vr08  intraValidFlagVec                                                                            // [  657] 

//vec16  vr09  dcSad                                                                                        // [  659] 
//vec16  vr10  dcTemp                                                                                       // [  660] 

//vec16  vr11  source0                                                                                      // [  662] 
//vec16  vr12  source1                                                                                      // [  663] 
//vec16  vr13  source2                                                                                      // [  664] 
//vec16  vr14  source3                                                                                      // [  665] 

//vec16  vr15  residual0                                                                                    // [  667] 
//vec16  vr16  residual1                                                                                    // [  668] 
//vec16  vr17  residual2                                                                                    // [  669] 
//vec16  vr18  residual3                                                                                    // [  670] 


    // get quantiser value
LINE 674".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD8W             vr01,[i4, MPO_QuantValue]                                        // quantValue CirBuf [  674] 

    // broadcast intraValidFlag across all lanes
       VMIVW             vr08, i0                                          // intraValidFlagVec intraValidFlag [  677] 

    // zero dc_prediction, as it's formed by accumulation
       VMOVW             vr07, 0, 255                                                              // dcPred16 [  680] 

    // check if above is valid
       VIA               i3, i0, 2                                                   // aboveOk intraValidFlag [  683] 

    // setup global value pointers
       VIM               i10, i8, SDMOF_h264_encoder_rd_sad_lambda                  // lambdaLutPtr quantValue [  686] 

    // check if left is valid
       VIA               i7, i0, 1                                                    // leftOk intraValidFlag [  689] 

    // setup global lambda value
       VLD8W_3           vr01,[i10, 0]                                         // lambdaMult10_30 lambdaLutPtr [  692] 

    // reset dc sad
       VMOVW             vr09, 0, 255                                                                 // dcSad [  695] 

    // if above or left is valid then set corresponding lane to 0xffff
       VNEW.136          vr00, vr00, vr62                               // aboveOk'leftOk aboveOk'leftOk VZERO [  698] 

    // we do a final adjust of the cost by adding 10 times lambda
              //  Stall: [vr01]  ...............................      vr00.136      vr01.008
       VMULW.8           vr01, vr01, 10                                     // lambdaMult10_30 lambdaMult10_30 [  701] 


    // setup source pointer for horizontal
       VIM.i7            i12, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+PCB_RECONSTRUCTED_STRIDE-1 // leftOk predPtr CirLeft [  705] 


    // vertical prediction (mode = 0)
//verticalPrediction --------                                                                               // [  709] 
 vendrec r3                                                                                                 // [  709] 
 vrec r3                                                                                                    // [  709] 

    // check if above is valid
LINE 712".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VJP!i3            r4                                                    // aboveOk horizontalPrediction [  712] 

    // delay slots start

    // get intra prediction pixels for A
       VLD64W.i3         vr02,[i5, MPO_Y_IntraPredPixel+0]  //DS__________________ aboveOk vertPred16Lo FixBuf [  717] 
       VLD64W.i3         vr03,[i5, MPO_Y_IntraPredPixel+8]  //DS__________________ aboveOk vertPred16Hi FixBuf [  718] 

       VNOP                //DS_______________________________________________________________________________ [  720] 
    // delay slots end
    

    // following can't be in delay slots
    // accumulate intra prediction pixels A into dc prediction vector
       VADDW             vr07, vr02, vr03                                // dcPred16 vertPred16Lo vertPred16Hi [  726] 


    // horizontal prediction (mode = 1)
//horizontalPrediction --------                                                                             // [  730] 
 vendrec r4                                                                                                 // [  730] 
 vrec r4                                                                                                    // [  730] 

    // check if left is valid
LINE 733".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VJP!i7            r5                                                             // leftOk dcPrediction [  733] 

    // delay slots start

    // load 8 bit predictions
       VLD8W.i7          vr04,[i12, 0*PCB_RECONSTRUCTED_STRIDE]  //DS______ leftOk horizPred16Lo_Term0 predPtr [  738] 
       VLD8W_1.i7        vr04,[i12, 1*PCB_RECONSTRUCTED_STRIDE]  //DS______ leftOk horizPred16Lo_Term1 predPtr [  739] 
       VLD8W_2.i7        vr04,[i12, 2*PCB_RECONSTRUCTED_STRIDE]  //DS______ leftOk horizPred16Lo_Term2 predPtr [  740] 

    // delay slots end


       VLD8W_3           vr04,[i12, 3*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Lo_Term3 predPtr [  745] 
       VLD8W_4           vr04,[i12, 4*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Lo_Term4 predPtr [  746] 
       VLD8W_5           vr04,[i12, 5*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Lo_Term5 predPtr [  747] 
       VLD8W_6           vr04,[i12, 6*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Lo_Term6 predPtr [  748] 
       VLD8W_7           vr04,[i12, 7*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Lo_Term7 predPtr [  749] 

       VLD8W             vr05,[i12, 8*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term0 predPtr [  751] 
       VLD8W_1           vr05,[i12, 9*PCB_RECONSTRUCTED_STRIDE]                 // horizPred16Hi_Term1 predPtr [  752] 
       VLD8W_2           vr05,[i12, 10*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term2 predPtr [  753] 
       VLD8W_3           vr05,[i12, 11*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term3 predPtr [  754] 
       VLD8W_4           vr05,[i12, 12*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term4 predPtr [  755] 
       VLD8W_5           vr05,[i12, 13*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term5 predPtr [  756] 
       VLD8W_6           vr05,[i12, 14*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term6 predPtr [  757] 
       VLD8W_7           vr05,[i12, 15*PCB_RECONSTRUCTED_STRIDE]                // horizPred16Hi_Term7 predPtr [  758] 

    // accumulate intra prediction pixel
              //  Stall: [vr05]  ...............................      vr05.224
              //  Stall: [vr05]  ...............................      vr05.192
              //  Stall: [vr05]  ...............................      vr05.128
       VADDW             vr62, vr04, vr05                                 // VZERO horizPred16Lo horizPred16Hi [  761] 

    // accumulate prediction into dc prediction vector
              //  Stall: [vr62]  ...............................      vr62    
       VADDAW            vr07, vr07, vr62                                           // dcPred16 dcPred16 VZERO [  764] 


    // dc prediction (mode = 2)
//dcPrediction --------                                                                                     // [  768] 
 vendrec r5                                                                                                 // [  768] 
 vrec r5                                                                                                    // [  768] 

    // compare intraValidFlag to see if neither above and left are available and
    // broadcast flag across all lanes
LINE 772".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VSUBW.255.F       vr62, vr08, 0                                              // VZERO intraValidFlagVec [  772] 

    // clear the sticky flags
       VCLRSTK.255                                                                                          // [  775] 

    // set the sticky flags if both above and left are available
       VSEQW.255         vr08, 3                                                          // intraValidFlagVec [  778] 

    // setup source value
    // if neither above or left are available then use default of 128
   mov                 r0, 0x0080
       VMOVW.255.Z       vr07, r0                                                                  // dcPred16 [  783] 

    // sum all lanes of dc_prediction vector and broadcast across whole vector
              //  Stall: [vr07]  ...............................      vr07          vr63    
       VXSUMW.255.NZ     vr07, vr07, 0xff                                                 // dcPred16 dcPred16 [  786] 

    // if both above and left are available then add rounding of 16 and divide by 32
              //  Stall: [vr07]  ...............................      vr07    
              //  Stall: [vr07]  ...............................      vr07    
       VASRRW.255.S.NZ   vr07, vr07, 5                                                    // dcPred16 dcPred16 [  789] 
    
    // if only one of above or left are available then add rounding of 8 and divide by 16
              //  Stall: [vr07]  ...............................      vr07    
              //  Stall: [vr07]  ...............................      vr07    
       VASRRW.255.U.NZ   vr07, vr07, 4                                                    // dcPred16 dcPred16 [  792] 

    // duplicate prediction into both bytes of the 16 bit lanes to for the 8 bit version
              //  Stall: [vr07]  ...............................      vr07    
              //  Stall: [vr07]  ...............................      vr07    
       VMULW.255         vr06, vr07, 0x0101                                                // dcPred8 dcPred16 [  795] 


    // calculate sads

    // setup loop counter
    // 16 rows total, 2 rows per loop
       VMOVW             vr01, (16/2)-1, 128                                                            // row [  802] 

    // set mode to dc
       VMOVW             vr00, MBIM_H264_Y_DC, 2                                           // intra_16x16_mode [  805] 

    // setup source pointer for residual generation
       VIM               i13, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                          // srcPtr CirBuf [  808] 

    // setup prediction pointer for residual generation
       VMOVW             vr01, SDMOF_IntraPredictionLuma+MBIM_H264_Y_DC*(16*16), 16                 // predPtr [  811] ....................// 1 byte per pixel

    // setup residual pointer for residual generation
       VMOVW             vr01, SDMOF_IntraPredictionResidsLuma+MBIM_H264_Y_DC*(16*16*2), 64          // resPtr [  814] ....................// 2 bytes per pixel


    // dcSadResidLoop start
//dcSadResidLoop --------                                                                                   // [  818] 
 vendrec r6                                                                                                 // [  818] 
 vrec r6                                                                                                    // [  818] 

    // load current (8 bits per pixel)
LINE 821".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD64W            vr11,[i13, 0]                                                       // source0 srcPtr [  821] 
       VLD64W            vr12,[i13, 8]                                                       // source1 srcPtr [  822] 
       VLD64W            vr13,[i13, 16]                                                      // source2 srcPtr [  823] 
       VLD64W            vr14,[i13, 24]                                                      // source3 srcPtr [  824] 

    // store prediction (8 bits per pixel)
       VST128            vr06,[i12, 0]                                                      // dcPred8 predPtr [  827] 
       VST128            vr06,[i12, 16]                                                     // dcPred8 predPtr [  828] 

    // generate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr15, vr11, vr07                                        // residual0 source0 dcPred16 [  831] 
       VSUBW             vr16, vr12, vr07                                        // residual1 source1 dcPred16 [  832] 
       VSUBW             vr17, vr13, vr07                                        // residual2 source2 dcPred16 [  833] 
       VSUBW             vr18, vr14, vr07                                        // residual3 source3 dcPred16 [  834] 

    // get absolute value and accumulate
       VABSW             vr62, vr15                                                         // VZERO residual0 [  837] 
       VABSAW            vr62, vr16                                                         // VZERO residual1 [  838] 
       VABSAW            vr62, vr17                                                         // VZERO residual2 [  839] 
       VABSAW            vr10, vr18                                                        // dcTemp residual3 [  840] 

    // store residual (16 bits per pixel)
       VST128            vr15,[i14, 0]                                                     // residual0 resPtr [  843] 
       VST128            vr16,[i14, 16]                                                    // residual1 resPtr [  844] 
       VST128            vr17,[i14, 32]                                                    // residual2 resPtr [  845] 
       VST128            vr18,[i14, 48]                                                    // residual3 resPtr [  846] 

    // sum temporary accumulated sad into total sad
       VADDW             vr09, vr09, vr10                                                // dcSad dcSad dcTemp [  849] 

    // decrement and loop
       VJD.i15           i15, r6                                                     // row row dcSadResidLoop [  852] 

    // delay slots start

    // increament pointers
       VIM               i13, i13, PCB_CURRENT_STRIDE*2  //DS___________________________________ srcPtr srcPtr [  857] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i12, i12, 16*2  //DS_________________________________________________ predPtr predPtr [  858] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i14, i14, 16*2*2  //DS_________________________________________________ resPtr resPtr [  859] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


    // sum across all lanes of the sads back into all the lanes, so that compare will set flags across all lanes
       VXSUMW.255        vr09, vr09, 0xff                                                       // dcSad dcSad [  865] 

    // scale final cost for comparison against inter
              //  Stall: [vr09]  ...............................      vr09    
              //  Stall: [vr09]  ...............................      vr09    
       VADDW.4           vr00, vr09, i11                                // intraLumaCost dcSad lambdaMult10_30 [  868] 

    // save mode
       VST8_1            vr00,[i4, MPO_WorkArea_BestIntraLumaMode]                  // intra_16x16_mode CirBuf [  871] 

    // save cost
       VST16_2           vr00,[i4, MPO_WorkArea_BestIntraLumaCost]                     // intraLumaCost CirBuf [  874] 

// End                                                                                                      // [  876] ....................// 1

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz GenerateIntra16x16PredictionFastDC_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x16PredictionFastDC                                                               // [  878] ....................// GenerateIntra16x16PredictionFastDC




//------------------------------------------------------------------------------
// Name:          GenerateIntra16x16PredictionFastVertical
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] GenerateIntra16x16PredictionFastVertical                                                         // [  887] 
_Asm int Record_GenerateIntra16x16PredictionFastVertical(int *mac_tab)
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
GenerateIntra16x16PredictionFastVertical_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

    // function arguments
    // intraValidFlag = i0

// Begin                                                                                                    // [  891] ....................//1
// p16    i3  aboveOk                                                                                       // [  892] 

// p16    i7  quantValue                                                                                    // [  894] 
// p16    i8  lambdaLutPtr                                                                                  // [  895] 
// p16    i10  lambdaMult10_30                                                                              // [  896] 

// p16    i11  predPtr                                                                                      // [  898] 
// p16    i12  srcPtr                                                                                       // [  899] 
// p16    i13  resPtr                                                                                       // [  900] 

// p16    i14  row                                                                                          // [  902] 

//vec16  vr02  vertPred8                                                                                    // [  904] 
//vec16  vr03  vertPred16Lo                                                                                 // [  905] 
//vec16  vr03  vertPred16Lo_Term0.1                                                                         // [  905] 
//vec16  vr03  vertPred16Lo_Term1.2                                                                         // [  905] 
//vec16  vr03  vertPred16Lo_Term2.4                                                                         // [  905] 
//vec16  vr03  vertPred16Lo_Term3.8                                                                         // [  905] 
//vec16  vr03  vertPred16Lo_Term4.16                                                                        // [  905] 
//vec16  vr03  vertPred16Lo_Term5.32                                                                        // [  905] 
//vec16  vr03  vertPred16Lo_Term6.64                                                                        // [  905] 
//vec16  vr03  vertPred16Lo_Term7.128                                                                       // [  905] 
//vec16  vr04  vertPred16Hi                                                                                 // [  906] 
//vec16  vr04  vertPred16Hi_Term0.1                                                                         // [  906] 
//vec16  vr04  vertPred16Hi_Term1.2                                                                         // [  906] 
//vec16  vr04  vertPred16Hi_Term2.4                                                                         // [  906] 
//vec16  vr04  vertPred16Hi_Term3.8                                                                         // [  906] 
//vec16  vr04  vertPred16Hi_Term4.16                                                                        // [  906] 
//vec16  vr04  vertPred16Hi_Term5.32                                                                        // [  906] 
//vec16  vr04  vertPred16Hi_Term6.64                                                                        // [  906] 
//vec16  vr04  vertPred16Hi_Term7.128                                                                       // [  906] 

//vec16  vr05  intraValidFlagVec                                                                            // [  908] 

//vec16  vr06  vertSad                                                                                      // [  910] 
//vec16  vr07  vertTemp                                                                                     // [  911] 

//vec16  vr08  source0                                                                                      // [  913] 
//vec16  vr09  source1                                                                                      // [  914] 
//vec16  vr10  source2                                                                                      // [  915] 
//vec16  vr11  source3                                                                                      // [  916] 

//vec16  vr12  residual0                                                                                    // [  918] 
//vec16  vr13  residual1                                                                                    // [  919] 
//vec16  vr14  residual2                                                                                    // [  920] 
//vec16  vr15  residual3                                                                                    // [  921] 


    // check if above is valid
LINE 925".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VIA               i3, i0, 2                                                   // aboveOk intraValidFlag [  925] 

    // set final cost to max value
   mov                 r0, 0x7fff
       VMOVW             vr00, r0, 4                                                          // intraLumaCost [  929] 

    // vertical prediction (mode = 0)
//verticalPrediction --------                                                                               // [  932] 
 vendrec r3                                                                                                 // [  932] 
 vrec r3                                                                                                    // [  932] 

    // check if above is valid
  ld r1,[r17,4];                                                 // aboveOk GenerateIntra16x16PredictionFastDC [  935] 
LINE 935".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VJP!i3            r1                                                                                 // [  935] 

    // delay slots start

    // get quantiser value
       VLD8W_7           vr00,[i4, MPO_QuantValue]  //DS____________________________________ quantValue CirBuf [  940] 

    // if above or left is valid then set corresponding lane to 0xffff
       VNEW.8            vr00, vr00, vr62  //DS_________________________________________ aboveOk aboveOk VZERO [  943] 

    // broadcast intraValidFlag across all lanes
       VMIVW             vr05, i0  //DS______________________________________ intraValidFlagVec intraValidFlag [  946] 

    // delay slots end
    

    // setup global value pointers
       VIM               i8, i7, SDMOF_h264_encoder_rd_sad_lambda                   // lambdaLutPtr quantValue [  952] 

    // reset sad
       VMOVW             vr06, 0, 255                                                               // vertSad [  955] 

    // setup global lambda value
       VLD8W_2           vr01,[i8, 0]                                          // lambdaMult10_30 lambdaLutPtr [  958] 


    // get intra prediction pixels for A
       VLD64W            vr03,[i5, MPO_Y_IntraPredPixel+0]                              // vertPred16Lo FixBuf [  962] 
       VLD64W            vr04,[i5, MPO_Y_IntraPredPixel+8]                              // vertPred16Hi FixBuf [  963] 
       VLD128            vr02,[i5, MPO_Y_IntraPredPixel+0]                                 // vertPred8 FixBuf [  964] 

    // we do a final adjust of the cost by adding 10 times lambda
       VMULW.4           vr01, vr01, 10                                     // lambdaMult10_30 lambdaMult10_30 [  967] 


    // calculate sads

    // setup loop counter
    // 16 rows total, 2 rows per loop
       VMOVW             vr01, (16/2)-1, 64                                                             // row [  974] 

    // set mode to vertical
       VMOVW             vr00, MBIM_H264_Y_VERTICAL, 2                                     // intra_16x16_mode [  977] 

    // setup source pointer for residual generation
       VIM               i12, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                          // srcPtr CirBuf [  980] 

    // setup prediction pointer for residual generation
       VMOVW             vr01, SDMOF_IntraPredictionLuma+MBIM_H264_Y_VERTICAL*(16*16), 8            // predPtr [  983] ....................// 1 byte per pixel

    // setup residual pointer for residual generation
       VMOVW             vr01, SDMOF_IntraPredictionResidsLuma+MBIM_H264_Y_VERTICAL*(16*16*2), 32    // resPtr [  986] ....................// 2 bytes per pixel


    // vertSadResidLoop start
//vertSadResidLoop --------                                                                                 // [  990] 
 vendrec r4                                                                                                 // [  990] 
 vrec r4                                                                                                    // [  990] 

    // load current (8 bits per pixel)
LINE 993".\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss"
       VLD64W            vr08,[i12, 0]                                                       // source0 srcPtr [  993] 
       VLD64W            vr09,[i12, 8]                                                       // source1 srcPtr [  994] 
       VLD64W            vr10,[i12, 16]                                                      // source2 srcPtr [  995] 
       VLD64W            vr11,[i12, 24]                                                      // source3 srcPtr [  996] 

    // store prediction (8 bits per pixel)
       VST128            vr02,[i11, 0]                                                    // vertPred8 predPtr [  999] 
       VST128            vr02,[i11, 16]                                                   // vertPred8 predPtr [ 1000] 

    // generate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr12, vr08, vr03                                    // residual0 source0 vertPred16Lo [ 1003] 
       VSUBW             vr13, vr09, vr04                                    // residual1 source1 vertPred16Hi [ 1004] 
       VSUBW             vr14, vr10, vr03                                    // residual2 source2 vertPred16Lo [ 1005] 
       VSUBW             vr15, vr11, vr04                                    // residual3 source3 vertPred16Hi [ 1006] 

    // get absolute value and accumulate
       VABSW             vr62, vr12                                                         // VZERO residual0 [ 1009] 
       VABSAW            vr62, vr13                                                         // VZERO residual1 [ 1010] 
       VABSAW            vr62, vr14                                                         // VZERO residual2 [ 1011] 
       VABSAW            vr07, vr15                                                      // vertTemp residual3 [ 1012] 

    // store residual (16 bits per pixel)
       VST128            vr12,[i13, 0]                                                     // residual0 resPtr [ 1015] 
       VST128            vr13,[i13, 16]                                                    // residual1 resPtr [ 1016] 
       VST128            vr14,[i13, 32]                                                    // residual2 resPtr [ 1017] 
       VST128            vr15,[i13, 48]                                                    // residual3 resPtr [ 1018] 

    // sum temporary accumulated sad into total sad
       VADDW             vr06, vr06, vr07                                          // vertSad vertSad vertTemp [ 1021] 

    // decrement and loop
       VJD.i14           i14, r4                                                   // row row vertSadResidLoop [ 1024] 

    // delay slots start

    // increament pointers
       VIM               i12, i12, PCB_CURRENT_STRIDE*2  //DS___________________________________ srcPtr srcPtr [ 1029] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i11, i11, 16*2  //DS_________________________________________________ predPtr predPtr [ 1030] ....................// 16 pixels, 1 byte per pixel, 2 rows
       VIM               i13, i13, 16*2*2  //DS_________________________________________________ resPtr resPtr [ 1031] ....................// 16 pixels, 2 bytes per pixel, 2 rows

    // delay slots end


    // sum across all lanes of the sads back into all the lanes, so that compare will set flags across all lanes
       VXSUMW.255        vr06, vr06, 0xff                                                   // vertSad vertSad [ 1037] 

    // scale final cost for comparison against inter
              //  Stall: [vr06]  ...............................      vr06    
              //  Stall: [vr06]  ...............................      vr06    
       VADDW.4           vr00, vr06, i10                              // intraLumaCost vertSad lambdaMult10_30 [ 1040] 

    // save mode
       VST8_1            vr00,[i4, MPO_WorkArea_BestIntraLumaMode]                  // intra_16x16_mode CirBuf [ 1043] 

    // save cost
       VST16_2           vr00,[i4, MPO_WorkArea_BestIntraLumaCost]                     // intraLumaCost CirBuf [ 1046] 

// End                                                                                                      // [ 1048] ....................// 1

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz GenerateIntra16x16PredictionFastVertical_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x16PredictionFastVertical                                                         // [ 1050] ....................// GenerateIntra16x16PredictionFastVertical




const int MACRO_IDX_GenerateIntra16x16PredictionFastAll = 0;
const int MACRO_IDX_GenerateIntra16x16PredictionFastDC = 1;
const int MACRO_IDX_GenerateIntra16x16PredictionFastVertical = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int IntraLumaPredictionFast_Record(int sa, int SIMDnum)
{
    int *map = IntraLumaPredictionFast;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_GenerateIntra16x16PredictionFastAll] = sa;
    sa = Record_GenerateIntra16x16PredictionFastAll(map);

    map[MACRO_IDX_GenerateIntra16x16PredictionFastDC] = sa;
    sa = Record_GenerateIntra16x16PredictionFastDC(map);

    map[MACRO_IDX_GenerateIntra16x16PredictionFastVertical] = sa;
    sa = Record_GenerateIntra16x16PredictionFastVertical(map);

    return sa;
}
