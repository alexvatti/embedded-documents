//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss'

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





#include "ArcSDMTables.h"
#include "ArcMPC.h"


//  showstalls
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

#include ".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.h"


// Macro table
    int IntraChromaPrediction[3];
    int IntraChromaPrediction_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Global Function Arguments
// p16    i0  intraValidFlag                                                                                // [   38] // same as luma
// p16    i1  intra_chroma_mode                                                                             // [   39] 
// p16    i2  intraChromaCost                                                                               // [   40] 

//------------------------------------------------------------------------------
// Intra Prediction Common Register Workspace
//vec16  vr02  Su.15                                                                                        // [   44] 
//vec16  vr02  Su_val0.1                                                                                    // [   44] 
//vec16  vr02  Su_val1.2                                                                                    // [   44] 
//vec16  vr02  Su_val2.4                                                                                    // [   44] 
//vec16  vr02  Su_val3.8                                                                                    // [   44] 
//vec16  vr03  Sv.15                                                                                        // [   45] 
//vec16  vr03  Sv_val0.1                                                                                    // [   45] 
//vec16  vr03  Sv_val1.2                                                                                    // [   45] 
//vec16  vr03  Sv_val2.4                                                                                    // [   45] 
//vec16  vr03  Sv_val3.8                                                                                    // [   45] 
//vec16  vr04  Su_shift.15                                                                                  // [   46] 
//vec16  vr04  Su_shift_val0.1                                                                              // [   46] 
//vec16  vr04  Su_shift_val1.2                                                                              // [   46] 
//vec16  vr04  Su_shift_val2.4                                                                              // [   46] 
//vec16  vr04  Su_shift_val3.8                                                                              // [   46] 
//vec16  vr05  Sv_shift.15                                                                                  // [   47] 
//vec16  vr05  Sv_shift_val0.1                                                                              // [   47] 
//vec16  vr05  Sv_shift_val1.2                                                                              // [   47] 
//vec16  vr05  Sv_shift_val2.4                                                                              // [   47] 
//vec16  vr05  Sv_shift_val3.8                                                                              // [   47] 

// s16    vr06.1  gBestIntraChromaCost                                                                      // [   49] 
// s16    vr07.1  gBestIntraChromaMode                                                                      // [   50] 

// s16    vr06.2  gUseIntraChromaDCPredOnly                                                                 // [   52] 



//------------------------------------------------------------------------------
// Name:          GenerateIntra16x8Residual_Calculate16x8SAD
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Subroutine] GenerateIntra16x8Residual_Calculate16x8SAD                                                  // [   63] 
_Asm int Record_GenerateIntra16x8Residual_Calculate16x8SAD(int *mac_tab)
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

// Begin                                                                                                    // [   66] 
// p16    i9  intra_chroma_modeOffset                                                                       // [   67] // intra_chroma_mode = i1

// p16    i3  source_ptr                                                                                    // [   69] 
// p16    i7  prediction_ptr                                                                                // [   70] 
// p16    i8  prediction_error_ptr                                                                          // [   71] 

// p16    i10  row                                                                                          // [   73] 

//vec16  vr08  sad                                                                                          // [   75] 

//vec16  vr09  source0                                                                                      // [   77] 
//vec16  vr10  source1                                                                                      // [   78] 
//vec16  vr11  source2                                                                                      // [   79] 
//vec16  vr12  source3                                                                                      // [   80] 

//vec16  vr13  prediction0                                                                                  // [   82] 
//vec16  vr14  prediction1                                                                                  // [   83] 
//vec16  vr15  prediction2                                                                                  // [   84] 
//vec16  vr16  prediction3                                                                                  // [   85] 

//vec16  vr17  prediction_error0                                                                            // [   87] 
//vec16  vr18  prediction_error1                                                                            // [   88] 
//vec16  vr19  prediction_error2                                                                            // [   89] 
//vec16  vr20  prediction_error3                                                                            // [   90] 

    // setup source and destination pointers
LINE 93".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VIM               i3, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_U                       // source_ptr CirBuf [   93] // will do both u and v

       VMULW.2           vr01, vr00, (16*8)                       // intra_chroma_modeOffset intra_chroma_mode [   95] // 1 byte per pixel
       VIM               i7, i9, SDMOF_IntraPredictionChroma         // prediction_ptr intra_chroma_modeOffset [   96] 

       VMULW.2           vr01, vr00, (16*8*2)                     // intra_chroma_modeOffset intra_chroma_mode [   98] // 2 bytes per pixel
       VIM               i8, i9, SDMOF_IntraPredictionResidsChroma // prediction_error_ptr intra_chroma_modeOffset [   99] 

    // reset sad value
       VMOVW             vr08, 0, 255                                                                   // sad [  102] 

    // setup loop counter
    // 8 rows total, 2 rows per loop of both u and v
       VMOVW             vr01, (8/2)-1, 4                                                               // row [  106] 


    // rowLoop start
//rowLoop --------                                                                                          // [  110] 
 vendrec r3                                                                                                 // [  110] 
 vrec r3                                                                                                    // [  110] 

    // load current (8 bits per pixel)
LINE 113".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VLD64W            vr09,[i3, 0]                                                    // source0 source_ptr [  113] 
       VLD64W            vr10,[i3, 8]                                                    // source1 source_ptr [  114] 
       VLD64W            vr11,[i3, 16]                                                   // source2 source_ptr [  115] 
       VLD64W            vr12,[i3, 24]                                                   // source3 source_ptr [  116] 

    // load prediction (8 bits per pixel)
       VLD64W            vr13,[i7, 0]                                            // prediction0 prediction_ptr [  119] 
       VLD64W            vr14,[i7, 8]                                            // prediction1 prediction_ptr [  120] 
       VLD64W            vr15,[i7, 16]                                           // prediction2 prediction_ptr [  121] 
       VLD64W            vr16,[i7, 24]                                           // prediction3 prediction_ptr [  122] 

    // calculate residual (9 bits stored in 16 bits per pixel)
       VSUBW             vr17, vr09, vr13                             // prediction_error0 source0 prediction0 [  125] 
       VSUBW             vr18, vr10, vr14                             // prediction_error1 source1 prediction1 [  126] 
       VSUBW             vr19, vr11, vr15                             // prediction_error2 source2 prediction2 [  127] 
       VSUBW             vr20, vr12, vr16                             // prediction_error3 source3 prediction3 [  128] 

    // store residual (16 bits per pixel)
       VST128            vr17,[i8, 0]                                // prediction_error0 prediction_error_ptr [  131] 
       VST128            vr18,[i8, 16]                               // prediction_error1 prediction_error_ptr [  132] 
       VST128            vr19,[i8, 32]                               // prediction_error2 prediction_error_ptr [  133] 
       VST128            vr20,[i8, 48]                               // prediction_error3 prediction_error_ptr [  134] 

    // get absolute values
       VABSW             vr17, vr17                                     // prediction_error0 prediction_error0 [  137] 
       VABSW             vr18, vr18                                     // prediction_error1 prediction_error1 [  138] 
       VABSW             vr19, vr19                                     // prediction_error2 prediction_error2 [  139] 
       VABSW             vr20, vr20                                     // prediction_error3 prediction_error3 [  140] 

    // accumulate sad for each column of 4 rows
       VADDW             vr62, vr17, vr18                         // VZERO prediction_error0 prediction_error1 [  143] 
       VADDAW            vr62, vr19, vr20                         // VZERO prediction_error2 prediction_error3 [  144] 
       VADDAW            vr08, vr08, vr62                                                     // sad sad VZERO [  145] 

    // decrement and loop
       VJD.i10           i10, r3                                                            // row row rowLoop [  148] 

    // delay slots start

    // increament pointers
       VIM               i3, i3, PCB_CURRENT_STRIDE*1*2  //DS___________________________ source_ptr source_ptr [  153] // 16 pixels (both u and v), 1 byte per pixel, 2 rows
       VIM               i7, i7, 16*1*2  //DS___________________________________ prediction_ptr prediction_ptr [  154] // 16 pixels (both u and v), 1 byte per pixel, 2 rows
       VIM               i8, i8, 16*2*2  //DS_______________________ prediction_error_ptr prediction_error_ptr [  155] // 16 pixels (both u and v), 2 bytes per pixel, 2 rows

    // delay slots end


    // calculate final sad by summing each column
       VXSUMW.4          vr00, vr08, 0xff                                               // intraChromaCost sad [  161] 

// End                                                                                                      // [  163] 

    // return to caller
LINE 166".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VJB               vr31, 0                                                                     // VSTACK [  166] 

    // delay slots start

    // compare with best cost and use new value if better or equal (this is so
    // the that the mode processing order 2, 1, 0 is essentially reversed to
    // give the priority 0, 1, 2 to match the C model)
       VSUBW.1.F         vr62, vr06, i2  //DS______________________ VZERO gBestIntraChromaCost intraChromaCost [  173] 
       VMIVW.1.GE        vr06, i2  //DS__________________________________ gBestIntraChromaCost intraChromaCost [  174] 
       VMIVW.1.GE        vr07, i1  //DS________________________________ gBestIntraChromaMode intra_chroma_mode [  175] 

    // delay slots end

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntra16x8Residual_Calculate16x8SAD                                                       // [  179] 




//------------------------------------------------------------------------------
// Name:          GenerateIntraChromaPrediction
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] GenerateIntraChromaPrediction                                                                    // [  188] 
_Asm int Record_GenerateIntraChromaPrediction(int *mac_tab)
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
GenerateIntraChromaPrediction_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

    // function arguments
    // intraValidFlag = i0

	// wait for chroma in dma
LINE 193".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VDMAWAIT          0, 0                                                                               // [  193] 

// Begin                                                                                                    // [  195] // 1
    // setup global best cost value to max
   mov                 r0, 0x7fff
LINE 198".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr06, r0, 1                                                   // gBestIntraChromaCost [  198] 

// End                                                                                                      // [  200] 


    // vertical prediction (mode = 2)
//verticalPrediction --------                                                                               // [  204] 
 vendrec r3                                                                                                 // [  204] 
 vrec r3                                                                                                    // [  204] 

// Begin                                                                                                    // [  206] // 1
// p16    i3  aboveOk                                                                                       // [  207] 

    // check if above is valid
LINE 210".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VIA               i3, i0, 2                                                   // aboveOk intraValidFlag [  210] 
       VJP!i3            r4                                                    // aboveOk horizontalPrediction [  211] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  215] // 1

// Begin   //DS_______________________________________________________________________________________________ [  217] // 1
// p16    i3  useIntraChromaDCPredOnly  //DS__________________________________________________________________ [  218] 

//vec16  vr08  A_u  //DS______________________________________________________________________________________ [  220] 
//vec16  vr09  A_v  //DS______________________________________________________________________________________ [  221] 

    // accumulate intra prediction pixels A into S vectors for both u and v
LINE 224".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VLD64W            vr08,[i5, MPO_U_IntraPredPixel]  //DS_____________________________________ A_u FixBuf [  224] 
       VLD64W            vr09,[i5, MPO_V_IntraPredPixel]  //DS_____________________________________ A_v FixBuf [  225] 

       VXSUMW.1          vr02, vr08, 0x0f  //DS___________________________________________________ Su_val0 A_u [  227] 

    // delay slots end

       VXSUMW.2          vr02, vr08, 0xf0                                                       // Su_val1 A_u [  231] 
       VXSUMW.1          vr03, vr09, 0x0f                                                       // Sv_val0 A_v [  232] 

    // check if we are only doing dc prediction
       VIV.2             i3, vr06                        // useIntraChromaDCPredOnly gUseIntraChromaDCPredOnly [  235] 
       VJP.i3            r4                                   // useIntraChromaDCPredOnly horizontalPrediction [  236] 

    // delay slots start

    // accumulate intra prediction pixels A into S vectors for both u and v
       VXSUMW.2          vr03, vr09, 0xf0  //DS___________________________________________________ Sv_val1 A_v [  241] 

    // perform shift with rounding on S0 and S1
       VASRRW.3          vr04, vr02, 2  //DS______________________ Su_shift_val0'Su_shift_val1 Su_val0'Su_val1 [  244] 
       VASRRW.3          vr05, vr03, 2  //DS______________________ Sv_shift_val0'Sv_shift_val1 Sv_val0'Sv_val1 [  245] 

    // delay slots end

// End                                                                                                      // [  249] // 1

// Begin                                                                                                    // [  251] // 1
// p16    i3  row                                                                                           // [  252] 

// p16    i7  a_ptr                                                                                         // [  254] 
// p16    i8  A_ptr                                                                                         // [  255] 

//vec16  vr08  a                                                                                            // [  257] 
//vec16  vr09  A                                                                                            // [  258] 

    // setup global intra_chroma_mode mode value
LINE 261".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr00, MBIM_H264_C_VERTICAL, 2                                    // intra_chroma_mode [  261] 

    // setup destination and source pointers
       VMOVW             vr00, SDMOF_IntraPredictionChroma+(MBIM_H264_C_VERTICAL*16*8), 128           // a_ptr [  264] // (u + v) 1 byte per pixel
    // no need for a source pointer, as the prediction can just be load directly
    //vim                 A_ptr, FixBuf, MPO_U_IntraPredPixel

    // load prediction for both u and v. assumes MPO_U_IntraPredPixel is
    // followed by MPO_V_IntraPredPixel
       VLD128            vr09,[i5, MPO_U_IntraPredPixel]                                           // A FixBuf [  270] 


    // setup loop counter
    // 8 rows total, 1 row per loop
       VMOVW             vr00, 8-1, 8                                                                   // row [  275] 

    // rowLoop start
//rowLoopVerticalPrediction --------                                                                        // [  278] 
 vendrec r5                                                                                                 // [  278] 
 vrec r5                                                                                                    // [  278] 

    // decrement and loop
LINE 281".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VJD.i3            i3, r5                                           // row row rowLoopVerticalPrediction [  281] 

    // delay slots start

    // store prediction for both u and v
       VST128            vr09,[i7, 0]  //DS___________________________________________________________ A a_ptr [  286] 

    // increament pointers
       VIM               i7, i7, 16  //DS_________________________________________________________ a_ptr a_ptr [  289] // 16 pixels (u + v), 1 byte per pixel, 1 row

       VNOP                //DS_______________________________________________________________________________ [  291] 
    // delay slots end


// End                                                                                                      // [  295] // 1

// Begin                                                                                                    // [  297] // 1
    // generate residual
  ld r1,[r17,0];                                          // VSTACK GenerateIntra16x8Residual_Calculate16x8SAD [  299] 
LINE 299".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [  299] 
    
    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [  302] 
       VNOP                //DS_______________________________________________________________________________ [  303] 
       VNOP                //DS_______________________________________________________________________________ [  304] 
    // delay slots end


// End                                                                                                      // [  308] // 1


    // horizontal prediction (mode = 1)
//horizontalPrediction --------                                                                             // [  312] 
 vendrec r4                                                                                                 // [  312] 
 vrec r4                                                                                                    // [  312] 

// Begin                                                                                                    // [  314] // 1
// p16    i3  leftOk                                                                                        // [  315] 

    // check if left is valid
LINE 318".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VIA               i3, i0, 1                                                    // leftOk intraValidFlag [  318] 
       VJP!i3            r6                                                             // leftOk dcPrediction [  319] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  323] // 1

// Begin   //DS_______________________________________________________________________________________________ [  325] // 1
// p16    i3  row  //DS_______________________________________________________________________________________ [  326] 

// p16    i7  a_ptr  //DS_____________________________________________________________________________________ [  328] 
// p16    i8  E_u_ptr  //DS___________________________________________________________________________________ [  329] 
// p16    i9  E_v_ptr  //DS___________________________________________________________________________________ [  330] 

//vec16  vr08  a  //DS________________________________________________________________________________________ [  332] 
//vec16  vr09  E  //DS________________________________________________________________________________________ [  333] 

    // setup global intra chroma mode value
LINE 336".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr00, MBIM_H264_C_HORIZONTAL, 2  //DS______________________________ intra_chroma_mode [  336] 

    // reset S2 and S3 so that they can be accumulated into over each iteration
    // of the loop, for both u and v
       VMOVW             vr02, 0, 12  //DS____________________________________________________ Su_val2'Su_val3 [  340] 
       VMOVW             vr03, 0, 12  //DS____________________________________________________ Sv_val2'Sv_val3 [  341] 

    // delay slots end

    // setup destination and source pointers
       VMOVW             vr00, SDMOF_IntraPredictionChroma+(MBIM_H264_C_HORIZONTAL*16*8), 128         // a_ptr [  346] // (u + v) 1 byte per pixel
       VIM               i8, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*9/2-1 // E_u_ptr CirLeft [  347] 
       VIM               i9, i6, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*9/2-1 // E_v_ptr CirLeft [  348] 

    // setup loop counter
    // 8 rows total, 1 row per loop of both u and v
       VMOVW             vr00, 8-1, 8                                                                   // row [  352] 


// Begin                                                                                                    // [  355] // 2 <- 1
// p16    i10  EUTerm                                                                                       // [  356] 
// p16    i11  EVTerm                                                                                       // [  357] 

//vec16  vr10  rowVec                                                                                       // [  359] 

    // rowLoop start
//rowLoopHorizontalPrediction --------                                                                      // [  362] 
 vendrec r7                                                                                                 // [  362] 
 vrec r7                                                                                                    // [  362] 

    // load 8 bit prediction
LINE 365".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VLD8W_2           vr01,[i8, 0]                                                        // EUTerm E_u_ptr [  365] 
       VLD8W_3           vr01,[i9, 0]                                                        // EVTerm E_v_ptr [  366] 

    // compare row counter to see if we need to accumulate the E term into S2
    // (i.e. first 4 rows 7, 6, 5 and 4) or S3 (i.e. last 4 rows 3, 2, 1 and 0)
    // and broadcast flag across all lanes
       VMIVW             vr10, i3                                                                // rowVec row [  371] 
       VSUBW.255.F       vr62, vr10, (8/2)-1                                                   // VZERO rowVec [  372] // halfway through 8 rows total, 1 row per loop of both u and v
    
       VADDW.4.GT        vr02, vr02, i10                                             // Su_val2 Su_val2 EUTerm [  374] 
       VADDW.4.GT        vr03, vr03, i11                                             // Sv_val2 Sv_val2 EVTerm [  375] 

       VADDW.8.LE        vr02, vr02, i10                                             // Su_val3 Su_val3 EUTerm [  377] 
       VADDW.8.LE        vr03, vr03, i11                                             // Sv_val3 Sv_val3 EVTerm [  378] 

    // duplicate prediction into vector register
       VMIVW.15          vr09, i10                                                                 // E EUTerm [  381] 
       VMIVW.240         vr09, i11                                                                 // E EVTerm [  382] 

    // duplicate prediction into both bytes of the 16 bit lanes
       VMULW             vr09, vr09, 0x0101                                                             // E E [  385] 

    // store prediction
       VST128            vr09,[i7, 0]                                                               // E a_ptr [  388] 

    // decrement and loop
       VJD.i3            i3, r7                                         // row row rowLoopHorizontalPrediction [  391] 

    // delay slots start

    // increament pointers
       VIM               i7, i7, 16  //DS_________________________________________________________ a_ptr a_ptr [  396] // 16 pixels (both u and v), 1 byte per pixel, 1 row
       VIM               i8, i8, PCB_RECONSTRUCTED_STRIDE  //DS_______________________________ E_u_ptr E_u_ptr [  397] 
       VIM               i9, i9, PCB_RECONSTRUCTED_STRIDE  //DS_______________________________ E_v_ptr E_v_ptr [  398] 

    // delay slots end


// Begin                                                                                                    // [  403] // 3
// p16    i12  useIntraChromaDCPredOnly                                                                     // [  404] 

    // check if we are only doing dc prediction
LINE 407".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VIV.2             i12, vr06                       // useIntraChromaDCPredOnly gUseIntraChromaDCPredOnly [  407] 
       VJP.i12           r6                                           // useIntraChromaDCPredOnly dcPrediction [  408] 

    // delay slots start

    // perform shift with rounding on S2 and S3
       VASRRW.12         vr04, vr02, 2  //DS______________________ Su_shift_val2'Su_shift_val3 Su_val2'Su_val3 [  413] 
       VASRRW.12         vr05, vr03, 2  //DS______________________ Sv_shift_val2'Sv_shift_val3 Sv_val2'Sv_val3 [  414] 

       VNOP                //DS_______________________________________________________________________________ [  416] 
    // delay slots end


// End                                                                                                      // [  420] // 3

// End                                                                                                      // [  422] // 2 <- 1

// End                                                                                                      // [  424] // 1

// Begin                                                                                                    // [  426] // 1
    // generate residual
  ld r1,[r17,0];                                          // VSTACK GenerateIntra16x8Residual_Calculate16x8SAD [  428] 
LINE 428".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VJL               vr31, r1                                                                           // [  428] 
    
    // delay slots start
       VNOP                //DS_______________________________________________________________________________ [  431] 
       VNOP                //DS_______________________________________________________________________________ [  432] 
       VNOP                //DS_______________________________________________________________________________ [  433] 
    // delay slots end


// End                                                                                                      // [  437] // 1

    // dc prediction (mode = 0)
//dcPrediction --------                                                                                     // [  440] 
 vendrec r6                                                                                                 // [  440] 
 vrec r6                                                                                                    // [  440] 

// Begin                                                                                                    // [  442] // 1
// p16    i3  a_ptr                                                                                         // [  443] 

//vec16  vr08  intraValidFlagVec                                                                            // [  445] 

//vec16  vr09  UV_AB                                                                                        // [  447] 
//vec16  vr10  UV_CD                                                                                        // [  448] 

    // setup global intra chroma mode value
LINE 451".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr00, MBIM_H264_C_DC, 2                                          // intra_chroma_mode [  451] 


//dcAboveLeft --------                                                                                      // [  454] 
 vendrec r8                                                                                                 // [  454] 
 vrec r8                                                                                                    // [  454] 

// Begin                                                                                                    // [  456] // 2 <- 1
// p16    i8  aboveOkLeftOk                                                                                 // [  457] // intraValidFlag = i0 (Fix explicit lane until scanMP supports lane declarations based on other scalars)

    // compare intraValidFlag to see if both above and left are available, if
    // they're not then jump to try just above
LINE 461".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VSUBW.1           vr01, vr00, 3                                         // aboveOkLeftOk intraValidFlag [  461] 
       VJP.i8            r9                                                           // aboveOkLeftOk dcAbove [  462] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  466] // 2 <- 1

// Begin   //DS_______________________________________________________________________________________________ [  468] // 2 <- 1
// p16    i7  Su_val2_Temp  //DS______________________________________________________________________________ [  469] 
// p16    i8  Sv_val2_Temp  //DS______________________________________________________________________________ [  470] 
// p16    i9  Su_val3_Temp  //DS______________________________________________________________________________ [  471] 
// p16    i10  Sv_val3_Temp  //DS_____________________________________________________________________________ [  472] 

//vec16  vr11  Su2.3  //DS____________________________________________________________________________________ [  474] 
//vec16  vr11  Su2_val02.1  //DS______________________________________________________________________________ [  474] 
//vec16  vr11  Su2_val13.2  //DS______________________________________________________________________________ [  474] 
//vec16  vr12  Sv2.3  //DS____________________________________________________________________________________ [  475] 
//vec16  vr12  Sv2_val02.1  //DS______________________________________________________________________________ [  475] 
//vec16  vr12  Sv2_val13.2  //DS______________________________________________________________________________ [  475] 

//vec16  vr13  Su_shift2.3  //DS______________________________________________________________________________ [  477] 
//vec16  vr13  Su_shift2_val02.1  //DS________________________________________________________________________ [  477] 
//vec16  vr13  Su_shift2_val13.2  //DS________________________________________________________________________ [  477] 
//vec16  vr14  Sv_shift2.3  //DS______________________________________________________________________________ [  478] 
//vec16  vr14  Sv_shift2_val02.1  //DS________________________________________________________________________ [  478] 
//vec16  vr14  Sv_shift2_val13.2  //DS________________________________________________________________________ [  478] 

    // if both above and left are available
    // A = (S0 + S2 + 4) >> 3
    // B = (S1 + 2) >> 2
    // C = (S3 + 2) >> 2
    // D = (S1 + S3 + 4) >> 3
    // load temp scalars ready for calculations
LINE 486".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VIV.4             i7, vr02  //DS__________________________________________________ Su_val2_Temp Su_val2 [  486] 
       VIV.4             i8, vr03  //DS__________________________________________________ Sv_val2_Temp Sv_val2 [  487] 

       VIV.8             i9, vr02  //DS__________________________________________________ Su_val3_Temp Su_val3 [  489] 

    // delay slots end

       VIV.8             i10, vr03                                                     // Sv_val3_Temp Sv_val3 [  493] 

    // calculate S0 + S2
       VADDW.1           vr11, vr02, i7                                      // Su2_val02 Su_val0 Su_val2_Temp [  496] 
       VADDW.1           vr12, vr03, i8                                      // Sv2_val02 Sv_val0 Sv_val2_Temp [  497] 

    // calculate S1 + S3
       VADDW.2           vr11, vr02, i9                                      // Su2_val13 Su_val1 Su_val3_Temp [  500] 
       VADDW.2           vr12, vr03, i10                                     // Sv2_val13 Sv_val1 Sv_val3_Temp [  501] 

    // perform shift with rounding on (S0+S2) and (S1+S3)
       VASRRW.3          vr13, vr11, 3                  // Su_shift2_val02'Su_shift2_val13 Su2_val02'Su2_val13 [  504] 
       VASRRW.3          vr14, vr12, 3                  // Sv_shift2_val02'Sv_shift2_val13 Sv2_val02'Sv2_val13 [  505] 

    // calculate A and B for both u and v
       VXSUMW.3          vr09, vr13, 0x01                                             // UV_AB Su_shift2_val02 [  508] // need to explicitly fix until scanMP support added
       VXSUMW.12         vr09, vr04, 0x02                                               // UV_AB Su_shift_val1 [  509] // need to explicitly fix until scanMP support added
       VXSUMW.48         vr09, vr14, 0x01                                             // UV_AB Sv_shift2_val02 [  510] // need to explicitly fix until scanMP support added
       VXSUMW.192        vr09, vr05, 0x02                                               // UV_AB Sv_shift_val1 [  511] // need to explicitly fix until scanMP support added


    // calculate C and D for both u and v
       VXSUMW.3          vr10, vr04, 0x08                                               // UV_CD Su_shift_val3 [  515] // need to explicitly fix until scanMP support added
       VXSUMW.12         vr10, vr13, 0x02                                             // UV_CD Su_shift2_val13 [  516] // need to explicitly fix until scanMP support added
       VXSUMW.48         vr10, vr05, 0x08                                               // UV_CD Sv_shift_val3 [  517] // need to explicitly fix until scanMP support added

    // jump to prediction generation
       VJP               r10                                                           // dcGeneratePrediction [  520] 

    // delay slots start

       VXSUMW.192        vr10, vr14, 0x02  //DS_________________________________________ UV_CD Sv_shift2_val13 [  524] // need to explicitly fix until scanMP support added

    // duplicate 8 bit prediction in each 16 bit lane to fill both byte positions
       VMULW             vr09, vr09, 0x0101  //DS_________________________________________________ UV_AB UV_AB [  527] 
       VMULW             vr10, vr10, 0x0101  //DS_________________________________________________ UV_CD UV_CD [  528] 

    // delay slots end


// End                                                                                                      // [  533] // 2 <- 1


//dcAbove --------                                                                                          // [  536] 
 vendrec r9                                                                                                 // [  536] 
 vrec r9                                                                                                    // [  536] 

// Begin                                                                                                    // [  538] // 2 <- 1
// p16    i8  aboveOk                                                                                       // [  539] // intraValidFlag = i0 (fix explicit lane until scanMP supports lane declarations based on other scalars)

    // compare intraValidFlag to see if only above is available, if it isn't
    // then jump to try just left
LINE 543".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VSUBW.1           vr01, vr00, 2                                               // aboveOk intraValidFlag [  543] 
       VJP.i8            r11                                                                 // aboveOk dcLeft [  544] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  548] // 2 <- 1

    // if only above is available
    // A = (S0 + 2) >> 2
    // B = (S1 + 2) >> 2
    // C = A
    // D = B
    // calculate A and B for both u and v
LINE 556".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VXSUMW.3          vr09, vr04, 0x01  //DS___________________________________________ UV_AB Su_shift_val0 [  556] // need to explicitly hack until scanMP support added
       VXSUMW.12         vr09, vr04, 0x02  //DS___________________________________________ UV_AB Su_shift_val1 [  557] // need to explicitly hack until scanMP support added
       VXSUMW.48         vr09, vr05, 0x01  //DS___________________________________________ UV_AB Sv_shift_val0 [  558] // need to explicitly hack until scanMP support added

    // delay slots end

       VXSUMW.192        vr09, vr05, 0x02                                               // UV_AB Sv_shift_val1 [  562] // need to explicitly hack until scanMP support added

    // calculate C and D for both u and v
       VXSUMW.3          vr10, vr04, 0x01                                               // UV_CD Su_shift_val0 [  565] // need to explicitly hack until scanMP support added
       VXSUMW.12         vr10, vr04, 0x02                                               // UV_CD Su_shift_val1 [  566] // need to explicitly hack until scanMP support added
       VXSUMW.48         vr10, vr05, 0x01                                               // UV_CD Sv_shift_val0 [  567] // need to explicitly hack until scanMP support added

    // jump to prediction generation
       VJP               r10                                                           // dcGeneratePrediction [  570] 

    // delay slots start

       VXSUMW.192        vr10, vr05, 0x02  //DS___________________________________________ UV_CD Sv_shift_val1 [  574] // need to explicitly hack until scanMP support added

    // duplicate 8 bit prediction in each 16 bit lane to fill both byte positions
       VMULW             vr09, vr09, 0x0101  //DS_________________________________________________ UV_AB UV_AB [  577] 
       VMULW             vr10, vr10, 0x0101  //DS_________________________________________________ UV_CD UV_CD [  578] 

    // delay slots end


//dcLeft --------                                                                                           // [  583] 
 vendrec r11                                                                                                // [  583] 
 vrec r11                                                                                                   // [  583] 

// Begin                                                                                                    // [  585] // 2 <- 1
// p16    i8  leftOk                                                                                        // [  586] // intraValidFlag = i0 (hack explicit lane until scanMP supports lane declarations based on other scalars)

    // compare intraValidFlag to see if only above is available, if it isn't
    // then jump to try just left
LINE 590".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VSUBW.1           vr01, vr00, 1                                                // leftOk intraValidFlag [  590] 
       VJP.i8            r12                                                               // leftOk dcDefault [  591] 

    // delay slots start

// End   //DS_________________________________________________________________________________________________ [  595] // 2 <- 1

    // if only left is available
    // A = (S2 + 2) >> 2
    // B = A
    // C = (S3 + 2) >> 2
    // D = C
    // calculate A and B for both u and v
LINE 603".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VXSUMW.15         vr09, vr04, 0x04  //DS___________________________________________ UV_AB Su_shift_val2 [  603] // need to explicitly hack until scanMP support added
       VXSUMW.240        vr09, vr05, 0x04  //DS___________________________________________ UV_AB Sv_shift_val2 [  604] // need to explicitly hack until scanMP support added

    // calculate C and D for both u and v
       VXSUMW.15         vr10, vr04, 0x08  //DS___________________________________________ UV_CD Su_shift_val3 [  607] // need to explicitly hack until scanMP support added

    // delay slots end


    // jump to prediction generation
       VJP               r10                                                           // dcGeneratePrediction [  613] 

    // delay slots start

       VXSUMW.240        vr10, vr05, 0x08  //DS___________________________________________ UV_CD Sv_shift_val3 [  617] // need to explicitly hack until scanMP support added

    // duplicate 8 bit prediction in each 16 bit lane to fill both byte positions
       VMULW             vr09, vr09, 0x0101  //DS_________________________________________________ UV_AB UV_AB [  620] 
       VMULW             vr10, vr10, 0x0101  //DS_________________________________________________ UV_CD UV_CD [  621] 

    // delay slots end


//dcDefault --------                                                                                        // [  626] 
 vendrec r12                                                                                                // [  626] 
 vrec r12                                                                                                   // [  626] 

    // if neither above or left are available then use default of 128 (for both
    // bytes of the lane)
    // A = 128
    // B = 128
    // C = 128
    // D = 128
    // calculate A and B for both u and v
   mov                 r0, 0x8080
LINE 636".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr09, r0, 255                                                                // UV_AB [  636] 

    // calculate C and D for both u and v
       VMOVW             vr10, r0, 255                                                                // UV_CD [  639] 


//dcGeneratePrediction --------                                                                             // [  642] 
 vendrec r10                                                                                                // [  642] 
 vrec r10                                                                                                   // [  642] 

    // setup destination pointer
LINE 645".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VMOVW             vr00, SDMOF_IntraPredictionChroma+(MBIM_H264_C_DC*16*8), 8                   // a_ptr [  645] // (u + v) 1 byte per pixel

    // store prediction
       VST128            vr09,[i3, 0]                                                           // UV_AB a_ptr [  648] 
       VST128            vr09,[i3, 16]                                                          // UV_AB a_ptr [  649] 
       VST128            vr09,[i3, 32]                                                          // UV_AB a_ptr [  650] 
       VST128            vr09,[i3, 48]                                                          // UV_AB a_ptr [  651] 

       VST128            vr10,[i3, 64]                                                          // UV_CD a_ptr [  653] 

    // generate residual and sad for both u and v
  ld r1,[r17,0];                                          // VSTACK GenerateIntra16x8Residual_Calculate16x8SAD [  656] 
       VJL               vr31, r1                                                                           // [  656] 
    
    // delay slots start

       VST128            vr10,[i3, 80]  //DS______________________________________________________ UV_CD a_ptr [  660] 
       VST128            vr10,[i3, 96]  //DS______________________________________________________ UV_CD a_ptr [  661] 
       VST128            vr10,[i3, 112]  //DS_____________________________________________________ UV_CD a_ptr [  662] 

    // delay slots end


// End                                                                                                      // [  667] // 1

// Begin                                                                                                    // [  669] // 1
    // save result (only need to save mode, as cost is not used in inter/intra
    // decision)
LINE 672".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VST8              vr07,[i4, MPO_WorkArea_BestIntraChromaMode]            // gBestIntraChromaMode CirBuf [  672] 

// End                                                                                                      // [  674] // 1

    // signal end of simd code
// Begin                                                                                                    // [  677] 
// End                                                                                                      // [  678] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz GenerateIntraChromaPrediction_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc GenerateIntraChromaPrediction                                                                    // [  680] 




//------------------------------------------------------------------------------
// Name:          SetIntraModeMPC
// Purpose:       
// Arguments:     
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] SetIntraModeMPC                                                                                  // [  689] 
_Asm int Record_SetIntraModeMPC(int *mac_tab)
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

// p16    i3  sIdx                                                                                          // [  690] // Circular buffer index
    // function arguments
// Begin                                                                                                    // [  693] 
//vec16  k8  intra.3                                                                                        // [  694] // fix until bug fixed in scanMP arbitary allocation and it supports a synonym for vr00 and vr01
//vec16  i8  intra_lumaMode.1                                                                               // [  694] 
//vec16  i9  intra_chromaMode.2                                                                             // [  694] 

// p16    i7  intraMode                                                                                     // [  696] 

    // read best intra luma and chroma modes
LINE 699".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VLD8W             vr01,[i4, MPO_WorkArea_BestIntraLumaMode]                    // intra_lumaMode CirBuf [  699] 
       VLD8W_1           vr01,[i4, MPO_WorkArea_BestIntraChromaMode]                // intra_chromaMode CirBuf [  700] 

    // pack 4 bit values into a byte
       VMULW.2           vr01, vr01, 16                                   // intra_chromaMode intra_chromaMode [  703] // fix until scanMP supports shifts, hould be (1 << INTRAMODE_CHROMA_SHIFT)
       VIV.3             i7, vr01                                                           // intraMode intra [  704] 

    // store combined intra mode
       VST8_7            vr00,[i4, MPO_IntraMode]                                          // intraMode CirBuf [  707] 

// End                                                                                                      // [  709] 

    // signal end of simd code
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]
   mov                 r1, Service_IntraComplete
LINE 715".\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss"
       VSEND             r0, i3, 0                                                                     // sIdx [  715] 
       VSEND             r0, r1, 63                                                                         // [  716] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetIntraModeMPC                                                                                  // [  718] 




const int SIMD_IDX_GenerateIntra16x8Residual_Calculate16x8SAD = 0;
const int MACRO_IDX_GenerateIntraChromaPrediction = 1;
const int MACRO_IDX_SetIntraModeMPC = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int IntraChromaPrediction_Record(int sa, int SIMDnum)
{
    int *map = IntraChromaPrediction;

    SetSCQ(SIMDnum);

    map[SIMD_IDX_GenerateIntra16x8Residual_Calculate16x8SAD] = sa;
    sa = Record_GenerateIntra16x8Residual_Calculate16x8SAD(map);

    map[MACRO_IDX_GenerateIntraChromaPrediction] = sa;
    sa = Record_GenerateIntraChromaPrediction(map);

    map[MACRO_IDX_SetIntraModeMPC] = sa;
    sa = Record_SetIntraModeMPC(map);

    return sa;
}
