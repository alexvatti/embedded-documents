//ScanMp
//Scanning file: '.\ARCEncoder\H264\H264MotionEstimation_MPA.sss'

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




#include "ArcMPC.h"
#include "ArcChannelRoutines.h"

// showstalls
// MetaWare ouput ON                                                                                        // [   26] 
#define LINE #line                                                                                          // [   26] 

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

#include ".\ARCEncoder\H264\H264MotionEstimation_MPA.h"


// Macro table
    int H264MotionEstimation[8];
    int H264MotionEstimation_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Name:          CalculateSkippedChromaCost
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CalculateSkippedChromaCost                                                                       // [   40] 
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
  ld r1,[r17 ,0]
  vrec r1

    // function arguments

// Begin                                                                                                    // [   43] 
    // return value
// p32    k0  WorstSkippedChromaCost                                                                        // [   45] // i0 & i1
// ****** Informational : Register space for symbol "WorstSkippedChromaCostLow" already allocated.          // [   46] // synonym for low 16 bit portion
// p16    i0  WorstSkippedChromaCostLow                                                                     // [   46] 
// ****** Informational : Register space for symbol "WorstSkippedChromaCostHigh" already allocated.         // [   47] // synonym for high 16 bit portion
// p16    i1  WorstSkippedChromaCostHigh                                                                    // [   47] 

// p16    i2  cpel                                                                                          // [   49] // current pointer
// p16    i3  ppel                                                                                          // [   50] // prediction pointer

// p16    i7  innerRow                                                                                      // [   52] 
// p16    i8  outerRow                                                                                      // [   53] 

//vec32  vr02  current0                                                                                     // [   55] 
//vec32  vr03  current1                                                                                     // [   56] 
//vec32  vr04  current2                                                                                     // [   57] 
//vec32  vr05  current3                                                                                     // [   58] 

//vec32  vr06  prediction0                                                                                  // [   60] 
//vec32  vr07  prediction1                                                                                  // [   61] 
//vec32  vr08  prediction2                                                                                  // [   62] 
//vec32  vr09  prediction3                                                                                  // [   63] 

//vec32  vr10  ssd0                                                                                         // [   65] 
//vec32  vr11  ssd1                                                                                         // [   66] 
//vec32  vr12  ssd2                                                                                         // [   67] 
//vec32  vr13  ssd3                                                                                         // [   68] 

//vec32  vr14  cost0                                                                                        // [   70] 
//vec32  vr15  cost1                                                                                        // [   71] 
//vec32  vr16  cost2                                                                                        // [   72] 
//vec32  vr17  cost3                                                                                        // [   73] 


    // setup source pointers
LINE 77".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VIM               i2, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_U                             // cpel CirBuf [   77] // will do both u and v
       VIM               i3, i4, MPO_PixelCoeffBuffer+PCB_REFERENCE_U                           // ppel CirBuf [   78] // will do both u and v

    // reset worst cost
       VMOVW             vr00, 0, 3                                                  // WorstSkippedChromaCost [   81] 

    // setup outer loop counter
    // 2 iterations total, first row of 4x4s, then second row of 4x4s (for u and v)
       VMOVW             vr01, 2-1, 1                                                              // outerRow [   85] 


    // outerRowLoop start
//outerRowLoop --------                                                                                     // [   89] 
 vendrec r3                                                                                                 // [   89] 
 vrec r3                                                                                                    // [   89] 

    // reset 4x4 costs
LINE 92".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VMOVW             vr14, 0, 255                                                                 // cost0 [   92] 
       VMOVW             vr15, 0, 255                                                                 // cost1 [   93] 
       VMOVW             vr16, 0, 255                                                                 // cost2 [   94] 
       VMOVW             vr17, 0, 255                                                                 // cost3 [   95] 

    // setup inner loop counter
    // 4 rows total, 1 row per loop (u + v)
       VMOVW             vr00, (4/1)-1, 128                                                        // innerRow [   99] 


    // innerRowLoop start
//innerRowLoop --------                                                                                     // [  103] 
 vendrec r4                                                                                                 // [  103] 
 vrec r4                                                                                                    // [  103] 

    // load current u and v (8 bits per pixel)
LINE 106".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VLD32WL           vr02,[i2, 0]                                                         // current0 cpel [  106] // u
       VLD32WL           vr03,[i2, 4]                                                         // current1 cpel [  107] // u
       VLD32WL           vr04,[i2, 8]                                                         // current2 cpel [  108] // v
       VLD32WL           vr05,[i2, 12]                                                        // current3 cpel [  109] // v

    // unpack current from 16 bit lanes into 32 bit lanes
       VUP.255           vr02, vr02                                                       // current0 current0 [  112] 
       VUP.255           vr03, vr03                                                       // current1 current1 [  113] 
       VUP.255           vr04, vr04                                                       // current2 current2 [  114] 
       VUP.255           vr05, vr05                                                       // current3 current3 [  115] 

    // load prediction u and v (8 bits per pixel)
       VLD32WL           vr06,[i3, 0]                                                      // prediction0 ppel [  118] // u
       VLD32WL           vr07,[i3, 4]                                                      // prediction1 ppel [  119] // u
       VLD32WL           vr08,[i3, 8]                                                      // prediction2 ppel [  120] // v
       VLD32WL           vr09,[i3, 12]                                                     // prediction3 ppel [  121] // v

    // unpack prediction from 16 bit lanes into 32 bit lanes
       VUP.255           vr06, vr06                                                 // prediction0 prediction0 [  124] 
       VUP.255           vr07, vr07                                                 // prediction1 prediction1 [  125] 
       VUP.255           vr08, vr08                                                 // prediction2 prediction2 [  126] 
       VUP.255           vr09, vr09                                                 // prediction3 prediction3 [  127] 

    // calculate residual i.e. the differences for 1 row of u and v (9 bits
    // stored in 32 bits per pixel)
       VSUB              vr10, vr02, vr06                                         // ssd0 current0 prediction0 [  131] 
       VSUB              vr11, vr03, vr07                                         // ssd1 current1 prediction1 [  132] 
       VSUB              vr12, vr04, vr08                                         // ssd2 current2 prediction2 [  133] 
       VSUB              vr13, vr05, vr09                                         // ssd3 current3 prediction3 [  134] 

    // calculate sum of square differences for for 1 row of u and v (18 bits)
       VMUL              vr10, vr10, vr10                                                    // ssd0 ssd0 ssd0 [  137] 
       VMUL              vr11, vr11, vr11                                                    // ssd1 ssd1 ssd1 [  138] 
       VMUL              vr12, vr12, vr12                                                    // ssd2 ssd2 ssd2 [  139] 
       VMUL              vr13, vr13, vr13                                                    // ssd3 ssd3 ssd3 [  140] 

    // sum 4x4 ssds vertically
       VADD              vr14, vr14, vr10                                                  // cost0 cost0 ssd0 [  143] 
       VADD              vr15, vr15, vr11                                                  // cost1 cost1 ssd1 [  144] 
       VADD              vr16, vr16, vr12                                                  // cost2 cost2 ssd2 [  145] 

    // decrement and loop
       VJD.i7            i7, r4                                              // innerRow innerRow innerRowLoop [  148] 
       VADD              vr17, vr17, vr13  //DS______________________________________________ cost3 cost3 ssd3 [  149] 
    // increament pointers
       VIM               i2, i2, PCB_CURRENT_STRIDE  //DS___________________________________________ cpel cpel [  151] 
       VIM               i3, i3, PCB_REFERENCE_STRIDE  //DS_________________________________________ ppel ppel [  152] 


    // sum 4x4 ssds horizontally
       VXSUM.3           vr14, vr14, 0xff                                                       // cost0 cost0 [  156] 
       VXSUM.3           vr15, vr15, 0xff                                                       // cost1 cost1 [  157] 
       VXSUM.3           vr16, vr16, 0xff                                                       // cost2 cost2 [  158] 

    // find max cost of 4x4 ssd and save
       VMAX.3            vr00, vr00, vr14               // WorstSkippedChromaCost WorstSkippedChromaCost cost0 [  161] 

    // sum 4x4 ssds horizontally
       VXSUM.3           vr17, vr17, 0xff                                                       // cost3 cost3 [  164] 

    // decrement and loop
       VJD.i8            i8, r3                                              // outerRow outerRow outerRowLoop [  167] 
    // find max cost of 4x4 ssd and save
       VMAX.3            vr00, vr00, vr15  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost1 [  169] 
       VMAX.3            vr00, vr00, vr16  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost2 [  170] 
       VMAX.3            vr00, vr00, vr17  //DS___________ WorstSkippedChromaCost WorstSkippedChromaCost cost3 [  171] 


    // return value in i0 & i1 = WorstSkippedChromaCost

    // signal end of simd code
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]

    // channels can only return 16 bit scalars
       VSEND             r0, i0, 0                                                // WorstSkippedChromaCostLow [  181] 
       VSEND             r0, i1, 1                                               // WorstSkippedChromaCostHigh [  182] 

   mov                 r1, Service_CalculateSkippedChromaCost
       VSEND             r0, r1, 63                                                                         // [  185] 
// End                                                                                                      // [  186] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CalculateSkippedChromaCost                                                                       // [  188] 





//------------------------------------------------------------------------------
// Name:          MakeLumaChromaPredictionInterError
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] MakeLumaChromaPredictionInterError                                                               // [  198] 
_Asm int Record_MakeLumaChromaPredictionInterError(int *mac_tab)
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

// Begin                                                                                                    // [  201] 
// p16    i0  source_ptr                                                                                    // [  202] 
// p16    i1  prediction_ptr                                                                                // [  203] 
// p16    i2  prediction_error_ptr                                                                          // [  204] 

// p16    i3  row                                                                                           // [  206] 

//vec16  vr02  source0                                                                                      // [  208] 
//vec16  vr03  source1                                                                                      // [  209] 
//vec16  vr04  source2                                                                                      // [  210] 
//vec16  vr05  source3                                                                                      // [  211] 

//vec16  vr06  prediction0                                                                                  // [  213] 
//vec16  vr07  prediction1                                                                                  // [  214] 
//vec16  vr08  prediction2                                                                                  // [  215] 
//vec16  vr09  prediction3                                                                                  // [  216] 

//vec16  vr10  prediction_error0                                                                            // [  218] 
//vec16  vr11  prediction_error1                                                                            // [  219] 
//vec16  vr12  prediction_error2                                                                            // [  220] 
//vec16  vr13  prediction_error3                                                                            // [  221] 

    // setup arguments for GenerateIntra16x16Residual
LINE 224".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VIM               i0, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                       // source_ptr CirBuf [  224] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_REFERENCE_Y                 // prediction_ptr CirBuf [  225] 
       VIM               i2, i4, MPO_PixelCoeffBuffer+PCB_RESIDUAL_Y            // prediction_error_ptr CirBuf [  226] 


    // setup loop counter
    // 24 rows total, 2 rows per loop
       VMOVW             vr00, (24/2)-1, 8                                                              // row [  231] 


    // rowLoop start
//rowLoop --------                                                                                          // [  235] 
 vendrec r3                                                                                                 // [  235] 
 vrec r3                                                                                                    // [  235] 

    // load current (8 bits per pixel)
LINE 238".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VLD64W            vr02,[i0, 0]                                                    // source0 source_ptr [  238] 
       VLD64W            vr03,[i0, 8]                                                    // source1 source_ptr [  239] 
       VLD64W            vr04,[i0, 16]                                                   // source2 source_ptr [  240] 
       VLD64W            vr05,[i0, 24]                                                   // source3 source_ptr [  241] 

    // load prediction (8 bits per pixel)
       VLD64W            vr06,[i1, 0]                                            // prediction0 prediction_ptr [  244] 
       VLD64W            vr07,[i1, 8]                                            // prediction1 prediction_ptr [  245] 
       VLD64W            vr08,[i1, 16]                                           // prediction2 prediction_ptr [  246] 
       VLD64W            vr09,[i1, 24]                                           // prediction3 prediction_ptr [  247] 

    // calculate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr10, vr02, vr06                             // prediction_error0 source0 prediction0 [  250] 
       VSUBW             vr11, vr03, vr07                             // prediction_error1 source1 prediction1 [  251] 
       VSUBW             vr12, vr04, vr08                             // prediction_error2 source2 prediction2 [  252] 
       VSUBW             vr13, vr05, vr09                             // prediction_error3 source3 prediction3 [  253] 

    // store residual (16 bits per pixel)
       VST128            vr10,[i2, 0]                                // prediction_error0 prediction_error_ptr [  256] 
       VST128            vr11,[i2, 16]                               // prediction_error1 prediction_error_ptr [  257] 
       VST128            vr12,[i2, 32]                               // prediction_error2 prediction_error_ptr [  258] 
       VST128            vr13,[i2, 48]                               // prediction_error3 prediction_error_ptr [  259] 

    // decrement and loop
       VJD.i3            i3, r3                                                             // row row rowLoop [  262] 
    // increament pointers
       VIM               i0, i0, PCB_CURRENT_STRIDE*2  //DS_____________________________ source_ptr source_ptr [  264] // 16 pixels, 1 byte per pixel, 2 rows
       VIM               i1, i1, PCB_REFERENCE_STRIDE*2  //DS___________________ prediction_ptr prediction_ptr [  265] // 16 pixels, 1 byte per pixel, 2 rows
       VIM               i2, i2, 16*2*2  //DS_______________________ prediction_error_ptr prediction_error_ptr [  266] // 16 pixels, 2 bytes per pixel, 2 rows


// End                                                                                                      // [  269] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc MakeLumaChromaPredictionInterError                                                               // [  271] 




// Select new macroblock type
// bestInterCost	:		Best cost found for inter prediction (from ME via Arc)
// cIdx				:		Circular buffer index
// Calls either Arc function Service_UseInter or Service_UseIntra via channel
// [Macro] ChooseInterOrIntra                                                                               // [  278] 
_Asm int Record_ChooseInterOrIntra(int *mac_tab)
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
ChooseInterOrIntra_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

    // Params ---------------------------
// s32    vr02.3  bestInterCost                                                                             // [  280] 
// p16    i0  cIdx                                                                                          // [  281] 
    // End Params -----------------------
// s32    vr03.3  BestIntraLumaCost                                                                         // [  284] 
//vec16  vr04  multiplier                                                                                   // [  285] 
// p16    i1  aboveFM                                                                                       // [  286] 
// p16    i2  leftFM                                                                                        // [  286] 
// p16    i3  UseIntra                                                                                      // [  286] 
// p16    i7  CurrentSkip                                                                                   // [  286] 
//vec16  vr05  kIntra                                                                                       // [  287] 
//vec16  vr06  kOne                                                                                         // [  287] 
// s16    vr02.4  kIntraMode                                                                                // [  288] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0| CurrentSkip|     CirLeft|      FixBuf|      CirBuf|    UseIntra|      leftFM|     aboveFM|        cIdx|
//|............................................................................................................|
//|VR 2|            |            |            |            |            |  kIntraMode|bestInterC.t             |
//|VR 3|            |            |            |            |            |            |BestIntraL.ost             |
//|VR 4|                                          multiplier                                                   |
//|VR 5|                                              kIntra                                                   |
//|VR 6|                                                kOne                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 290".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VLD8W_7           vr00,[i4, MPO_MBCodingType]                                     // CurrentSkip CirBuf [  290] 
       VMVW              vr03, vr62, 3                                              // BestIntraLumaCost VZERO [  291] 
       VLD16             vr03,[i4, MPO_WorkArea_BestIntraLumaCost]                 // BestIntraLumaCost CirBuf [  292] 
       VLD8W_1           vr00,[i4, MPO_AboveMB]                                              // aboveFM CirBuf [  293] 
       VLD8W_2           vr00,[i4, MPO_LeftMB]                                                // leftFM CirBuf [  294] 
       VIA               i7, i7, MBCT_H264_SKIPPED                                  // CurrentSkip CurrentSkip [  295] 
       VAND.6            vr00, vr00, MBNF_VALID                               // aboveFM'leftFM aboveFM'leftFM [  296] 
       VMOV              vr04, 16                                                                // multiplier [  297] 
       VJP.i7            r3                                                              // CurrentSkip IsSkip [  298] 
       VLD8W_1.i1        vr00,[i5, MPO_MBCodingType]  //DS_____________________________ aboveFM aboveFM FixBuf [  299] 
       VLD8W_2.i2        vr00,[i6, MPO_MBCodingType]  //DS______________________________ leftFM leftFM CirLeft [  300] 
       VMOVW             vr05, MBCT_H264_INTRA16X16, 255  //DS_________________________________________ kIntra [  301] 
       VMOV              vr06, 1                                                                       // kOne [  302] 
       VEQW.6            vr00, vr00, vr05                              // aboveFM'leftFM aboveFM'leftFM kIntra [  303] 
       VSUB.i1           vr04, vr04, vr06                                // aboveFM multiplier multiplier kOne [  304] 
       VSUB.i2           vr04, vr04, vr06                                 // leftFM multiplier multiplier kOne [  305] 
       VMUL.3            vr03, vr03, vr04                    // BestIntraLumaCost BestIntraLumaCost multiplier [  306] 
       VASR.3            vr03, vr03, 4                                  // BestIntraLumaCost BestIntraLumaCost [  307] 
       VLT.3             vr03, vr03, vr02                 // BestIntraLumaCost BestIntraLumaCost bestInterCost [  308] 
       VMOVW             vr02, MBCT_H264_INTRA16X16, 4                                           // kIntraMode [  309] 
       VIV.3             i3, vr03                                                // UseIntra BestIntraLumaCost [  310] // Move to I reg
       VJP.i3            r4                                                              // UseIntra CallIntra [  311] 
       VST8_2.i3         vr02,[i4, MPO_MBCodingType]  //DS_________________________ UseIntra kIntraMode CirBuf [  312] 
       VNOP                //DS_______________________________________________________________________________ [  313] 
       VNOP                //DS_______________________________________________________________________________ [  314] 
//IsSkip --------                                                                                           // [  315] 
 vendrec r3                                                                                                 // [  315] 
 vrec r3                                                                                                    // [  315] 
LINE 316".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VJP               r5                                                                            // Exit [  316] 
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]
   mov                 r1, Service_UseInter
       VSEND             r0, i0, 0  //DS_________________________________________________________________ cIdx [  320] 
       VSEND             r0, r1, 63  //DS_____________________________________________________________________ [  321] 
       VNOP                //DS_______________________________________________________________________________ [  322] 
//CallIntra --------                                                                                        // [  323] 
 vendrec r4                                                                                                 // [  323] 
 vrec r4                                                                                                    // [  323] 
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]
   mov                 r1, Service_UseIntra
LINE 327".\ARCEncoder\H264\H264MotionEstimation_MPA.sss"
       VSEND             r0, i0, 0                                                                     // cIdx [  327] 
       VSEND             r0, r1, 63                                                                         // [  328] 
//Exit --------                                                                                             // [  329] 
 vendrec r5                                                                                                 // [  329] 
 vrec r5                                                                                                    // [  329] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ChooseInterOrIntra_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ChooseInterOrIntra                                                                               // [  330] 







const int MACRO_IDX_CalculateSkippedChromaCost = 0;
const int MACRO_IDX_MakeLumaChromaPredictionInterError = 1;
const int MACRO_IDX_ChooseInterOrIntra = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int H264MotionEstimation_Record(int sa, int SIMDnum)
{
    int *map = H264MotionEstimation;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_CalculateSkippedChromaCost] = sa;
    sa = Record_CalculateSkippedChromaCost(map);

    map[MACRO_IDX_MakeLumaChromaPredictionInterError] = sa;
    sa = Record_MakeLumaChromaPredictionInterError(map);

    map[MACRO_IDX_ChooseInterOrIntra] = sa;
    sa = Record_ChooseInterOrIntra(map);

    return sa;
}
