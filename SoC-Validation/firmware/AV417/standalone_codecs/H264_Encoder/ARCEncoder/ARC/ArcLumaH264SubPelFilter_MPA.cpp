//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss'

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

// MetaWare ouput ON                                                                                                    // [   20] 
#define LINE #line                                                                                                      // [   20] 
#include ".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.h"


// Macro table
    int LumaSubPelFilter[9];
    int LumaSubPelFilter_Record(int sa, int SIMDnum);

// ScanMP: Including file .\ARCEncoder\ARC\..\ARC\SIMD_ABI.ii.
// (c) Arc international 2007
// Creo vrapter ABI using MPC data structures
// To co-operate with ME & EE hardware

// Media processor ABI ---------------------------
//	vec16				Scalars0 = vr00
//	vec16				Scalars1 = vr01
//	**	i0 - i3			Scalar params/scratch
// p16    i4  CirBuf                                                                                                    // [    9] 
// p16    i5  FixBuf                                                                                                    // [   10] 
// p16    i6  CirLeft                                                                                                   // [   11] 
//	**	i7				scratch
// 	**	i8 - i15 		Work - should be preserved
//	**	vr01 - vr04		params
//	**	vr05 - vr30		scratch
//vec16  vr62  VZERO                                                                                                    // [   16] 
//vec16  vr31  VSTACK                                                                                                   // [   17] 
//vec16  vr63  VFLAGS                                                                                                   // [   18] 

// Media processor ABI end -----------------------


// -----------------------------------------------
// CirBuf, FixBuf , CirLeft are set by  "SetCurrentMPC" in ArcSDMTables_MPA.sss
// -----------------------------------------------


// ScanMP: End of included file ..\ARC\SIMD_ABI.ii.

#include "ArcMPC.h"
#include "ArcSDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"


//------------------------------------------------------------------------------
// Name:          CopyLumaPredToSDM
// Purpose:       Copies the luma prediction from
//                main memory to the current MPC pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyLumaPredToSDM                                                                                            // [   41] 
_Asm int Record_CopyLumaPredToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                                       // [   42] 
// p16    i1  mbY                                                                                                       // [   43] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                                        // [   44] 
// p32    k6  sdmAddr                                                                                                   // [   44] 
// p32    k8  dr2Val                                                                                                    // [   45] 

// Begin                                                                                                                // [   47] 
// p16    i2  dmaOutReg16                                                                                               // [   48] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                                    // [   49] 
// p32    k2  dmaOutReg32                                                                                               // [   49] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 53".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                                    // dmaOutReg32 [   53] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr1, k2                                                                            // dmaOutReg32 [   54] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                                // sdmAddr [   57] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                                 // dr2Val [   65] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                                          // dmaOutReg32 [   77] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr6, k2                                                                            // dmaOutReg32 [   78] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                            // dmaOutReg32 [   81] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr7, k2                                                                            // dmaOutReg32 [   82] 

    // start dma in
       VDMAIRUN          i0, i1                                                                                 // mbX mbY [   85] 
    
//    vdmawait          0,0
    
// End                                                                                                                  // [   89] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyLumaPredToSDM                                                                                            // [   91] 



// [Macro] CallBackDoLumaSubPelFilter                                                                                   // [   93] 
_Asm int Record_CallBackDoLumaSubPelFilter(int *mac_tab)
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

    // Params -----------
// p16    i0  BufIdx                                                                                                    // [   95] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 102".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [  102] 
   mov                 r1, Service_DoLumaSubPelFilter   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [  104] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoLumaSubPelFilter                                                                                   // [  106] 




//------------------------------------------------------------------------------
// Name:          PerformInpHorFiltering
// Purpose:       
//                
// Arguments:     inBuf
//                outBuf
//                
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] PerformInpHorFiltering                                                                                       // [  118] 
_Asm int Record_PerformInpHorFiltering(int *mac_tab)
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
PerformInpHorFiltering_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

// p16    i0  inBuf                                                                                                     // [  119] 
// p16    i1  outBuf                                                                                                    // [  120] 
// Begin                                                                                                                // [  121] 
// p16    i2  count                                                                                                     // [  122] 
//vec16  vr02  inLft                                                                                                    // [  123] 
//vec16  vr03  inMdl                                                                                                    // [  124] 
//vec16  vr04  inRgt                                                                                                    // [  125] 
//vec16  vr05  outLft                                                                                                   // [  126] 
//vec16  vr06  outRgt                                                                                                   // [  127] 
//vec16  vr07  tmp1                                                                                                     // [  128] 
//vec16  vr08  tmp2                                                                                                     // [  129] 
//vec16  vr09  tmp3                                                                                                     // [  130] 
//vec16  vr10  tmp4                                                                                                     // [  131] 
//vec16  vr11  tmp5                                                                                                     // [  132] 
    
LINE 134".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VMOVW             vr00, 16-1, 4                                                                            // count [  134] 
//lpStart --------                                                                                                      // [  135] 
 vendrec r3                                                                                                             // [  135] 
 vrec r3                                                                                                                // [  135] 
    // load the input data
LINE 137".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD64W            vr02,[i0, 0]                                                                       // inLft inBuf [  137] ....................;  // w08 w07 w06 w05 w04 w03 w02 w01
       VLD64W            vr03,[i0, 8]                                                                       // inMdl inBuf [  138] ....................;  // w16 w15 w14 w13 w12 w11 w10 w09
       VLD64W            vr04,[i0, 16]                                                                      // inRgt inBuf [  139] ....................;  // 000 000 000 w21 w20 w19 w18 w17

    //arrange registers by shifts to the following format:
    // inLft = w08 w07 w06 w05 w04 w03 w02 w01

              //  Stall: [vr03]  ...............................      vr02          vr03          vr04    
              //  Stall: [vr03]  ...............................      vr03          vr04    
       VMR1W             vr07, vr03, vr02                                                              // tmp1 inMdl inLft [  144] ....................; // tmp1 = w09 w08 w07 w06 w05 w04 w03 w02
       VMR2W             vr08, vr03, vr02                                                              // tmp2 inMdl inLft [  145] ....................; // tmp2 = w10 w09 w08 w07 w06 w05 w04 w03
       VMR3W             vr09, vr03, vr02                                                              // tmp3 inMdl inLft [  146] ....................; // tmp3 = w11 w10 w09 w08 w07 w06 w05 w04
       VMR4W             vr10, vr03, vr02                                                              // tmp4 inMdl inLft [  147] ....................; // tmp4 = w12 w11 w10 w09 w08 w07 w06 w05
       VMR5W             vr11, vr03, vr02                                                              // tmp5 inMdl inLft [  148] ....................; // tmp5 = w13 w12 w11 w10 w09 w08 w07 w06

    // at this point acc = tmp5
    // now broadcast multiply and accumulate
       VBMULAW           vr05, vr07, (-5)                                                                   // outLft tmp1 [  152] ....................; //acc += tmp1 * (-5)
       VBMULAW           vr05, vr08, (20)                                                                   // outLft tmp2 [  153] ....................; //acc += tmp2 * (20)
       VBMULAW           vr05, vr09, (20)                                                                   // outLft tmp3 [  154] ....................; //acc += tmp3 * (20)
       VBMULAW           vr05, vr10, (-5)                                                                   // outLft tmp4 [  155] ....................; //acc += tmp4 * (-5)
       VANDAW            vr05, vr02, vr02                                                            // outLft inLft inLft [  156] ....................; //acc += inLft
    
    // Arrange the next 6 registers
    // inMdl = w16 w15 w14 w13 w12 w11 w10 w09

       VMR1W             vr07, vr04, vr03                                                              // tmp1 inRgt inMdl [  161] ....................; // tmp1 = w17 w16 w15 w14 w13 w12 w11 w10
       VMR2W             vr08, vr04, vr03                                                              // tmp2 inRgt inMdl [  162] ....................; // tmp2 = w18 w17 w16 w15 w14 w13 w12 w11
       VMR3W             vr09, vr04, vr03                                                              // tmp3 inRgt inMdl [  163] ....................; // tmp3 = w19 w18 w17 w16 w15 w14 w13 w12
       VMR4W             vr10, vr04, vr03                                                              // tmp4 inRgt inMdl [  164] ....................; // tmp4 = w20 w19 w18 w17 w16 w15 w14 w13
       VMR5W             vr11, vr04, vr03                                                              // tmp5 inRgt inMdl [  165] ....................; // tmp5 = w21 w20 w19 w18 w17 w16 w15 w14

    // at this point acc = tmp5
    // now broadcast multiply and accumulate
       VBMULAW           vr06, vr07, (-5)                                                                   // outRgt tmp1 [  169] ....................; //acc += tmp1 * (-5)
       VBMULAW           vr06, vr08, (20)                                                                   // outRgt tmp2 [  170] ....................; //acc += tmp2 * (20)
       VBMULAW           vr06, vr09, (20)                                                                   // outRgt tmp3 [  171] ....................; //acc += tmp3 * (20)
       VBMULAW           vr06, vr10, (-5)                                                                   // outRgt tmp4 [  172] ....................; //acc += tmp4 * (-5)
       VANDAW            vr06, vr03, vr03                                                            // outRgt inMdl inMdl [  173] ....................; //acc += inMdl

    // pack first 8 words
       VASRRPWB          vr05, vr05, 5                                                                    // outLft outLft [  176] ....................;   // pack the registers
    
       VIM               i0, i0, ME_REFERENCE_ARRAY_STRIDE                                                  // inBuf inBuf [  178] 
    // pack next 8 words
              //  Stall: [vr06]  ...............................      vr05          vr06          I00  
       VASRRPWB          vr06, vr06, 5                                                                    // outRgt outRgt [  180] ....................;   // pack the registers

    //store result
       VJD.i2            i2, r3                                                                     // count count lpStart [  183] 
       VST64             vr05,[i1, 0]  //DS_________________________________________________________________ outLft outBuf [  184] ....................;   // store the result             
       VST64             vr06,[i1, 8]  //DS_________________________________________________________________ outRgt outBuf [  185] ....................;   // store the result 
       VIM               i1, i1, ME_REFERENCE_ARRAY_STRIDE  //DS____________________________________________ outBuf outBuf [  186] 
   


// End                                                                                                                  // [  190] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz PerformInpHorFiltering_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc PerformInpHorFiltering                                                                                       // [  191] 



//------------------------------------------------------------------------------
// Name:          QPelFiltering
// Purpose:       
//                
// Arguments:     in1Buf
//                in2Buf
//                outBuf
//                shift
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] QPelFiltering                                                                                                // [  203] 
_Asm int Record_QPelFiltering(int *mac_tab)
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
QPelFiltering_RecLoop:
  ld r1,[r17 ,12]
  vrec r1

// p16    i0  in1Buf                                                                                                    // [  204] 
// p16    i1  in2Buf                                                                                                    // [  205] 
// p16    i2  outBuf                                                                                                    // [  206] 
// p16    i3  iShift                                                                                                    // [  207] 
// Begin                                                                                                                // [  208] 
// p16    i7  count                                                                                                     // [  209] 
//vec16  vr02  vShift                                                                                                   // [  210] 
//vec16  vr03  a1Lft                                                                                                    // [  211] 
//vec16  vr04  a1Mdl                                                                                                    // [  212] 
//vec16  vr05  a1Rgt                                                                                                    // [  213] 
//vec16  vr06  a2Lft                                                                                                    // [  214] 
//vec16  vr07  a2Mdl                                                                                                    // [  215] 
//vec16  vr08  a2Rgt                                                                                                    // [  216] 
//vec16  vr09  b1Lft                                                                                                    // [  217] 
//vec16  vr10  b1Rgt                                                                                                    // [  218] 
//vec16  vr11  b2Lft                                                                                                    // [  219] 
//vec16  vr12  b2Rgt                                                                                                    // [  220] 

LINE 222".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VMIVW             vr02, i3                                                                         // vShift iShift [  222] 
       VMOVW             vr00, 8-1, 128                                                                           // count [  223] 
//lpStart --------                                                                                                      // [  224] 
 vendrec r3                                                                                                             // [  224] 
 vrec r3                                                                                                                // [  224] 
    // load input data - ROW 1
LINE 226".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD64W            vr03,[i0, 0]                                                                      // a1Lft in1Buf [  226] ....................;  // inp1: w08 w07 w06 w05 w04 w03 w02 w01
       VLD64W            vr04,[i0, 8]                                                                      // a1Mdl in1Buf [  227] ....................;  // inp1: w16 w15 w14 w13 w12 w11 w10 w09
       VLD64W.i3         vr05,[i0, 16]                                                              // iShift a1Rgt in1Buf [  228] ....................;  // inp1: 000 000 000 w21 w20 w19 w18 w17
       VLD64W            vr09,[i1, 0]                                                                      // b1Lft in2Buf [  229] ....................;  // inp2: w08 w07 w06 w05 w04 w03 w02 w01
       VLD64W            vr10,[i1, 8]                                                                      // b1Rgt in2Buf [  230] ....................;  // inp2: w16 w15 w14 w13 w12 w11 w10 w09

    // load input data - ROW 2
       VLD64W            vr06,[i0, ME_REFERENCE_ARRAY_STRIDE]                                              // a2Lft in1Buf [  233] ....................; // inp1: w08 w07 w06 w05 w04 w03 w02 w01
       VLD64W            vr07,[i0, ME_REFERENCE_ARRAY_STRIDE+8]                                            // a2Mdl in1Buf [  234] ....................; // inp1: w16 w15 w14 w13 w12 w11 w10 w09
       VLD64W.i3         vr08,[i0, ME_REFERENCE_ARRAY_STRIDE+16]                                    // iShift a2Rgt in1Buf [  235] ....................; // inp1: 000 000 000 w21 w20 w19 w18 w17
       VLD64W            vr11,[i1, ME_REFERENCE_ARRAY_STRIDE]                                              // b2Lft in2Buf [  236] ....................; // inp2: w08 w07 w06 w05 w04 w03 w02 w01
       VLD64W            vr12,[i1, ME_REFERENCE_ARRAY_STRIDE+8]                                            // b2Rgt in2Buf [  237] ....................; // inp2: w16 w15 w14 w13 w12 w11 w10 w09

    //shift right by 2 to reject the first 2 columns
       VSEQW             vr02, 2                                                                                 // vShift [  240] 
       VMR2W.S           vr03, vr04, vr03                                                             // a1Lft a1Mdl a1Lft [  241] ....................; // a2Lft= w10 w09 w08 w07 w06 w05 w04 w03
       VMR2W.S           vr04, vr05, vr04                                                             // a1Mdl a1Rgt a1Mdl [  242] ....................; // a2Mdl= w18 w17 w16 w15 w14 w13 w12 w11

    //shift right by 2 to reject the first 2 columns
       VMR2W.S           vr06, vr07, vr06                                                             // a2Lft a2Mdl a2Lft [  245] ....................; // a2Lft= w10 w09 w08 w07 w06 w05 w04 w03
       VMR2W.S           vr07, vr08, vr07                                                             // a2Mdl a2Rgt a2Mdl [  246] ....................; // a2Mdl= w18 w17 w16 w15 w14 w13 w12 w11

    //shift right by 3 to reject the first 2 columns
       VSEQW             vr02, 3                                                                                 // vShift [  249] 
       VMR3W.S           vr03, vr04, vr03                                                             // a1Lft a1Mdl a1Lft [  250] ....................; // a2Lft= w11 w10 w09 w08 w07 w06 w05 w04
       VMR3W.S           vr04, vr05, vr04                                                             // a1Mdl a1Rgt a1Mdl [  251] ....................; // a2Mdl= w19 w18 w17 w16 w15 w14 w13 w12

    //shift right by 3 to reject the first 2 columns
       VMR3W.S           vr06, vr07, vr06                                                             // a2Lft a2Mdl a2Lft [  254] ....................; // a2Lft= w11 w10 w09 w08 w07 w06 w05 w04
       VMR3W.S           vr07, vr08, vr07                                                             // a2Mdl a2Rgt a2Mdl [  255] ....................; // a2Mdl= w19 w18 w17 w16 w15 w14 w13 w12

    // calculate the output
       VADDW             vr03, vr09, vr03                                                             // a1Lft b1Lft a1Lft [  258] ....................; // a2Rgt = a2Lft + b1Lft
       VADDW             vr04, vr10, vr04                                                             // a1Mdl b1Rgt a1Mdl [  259] ....................; // b2Lft = a2Mdl + b1Rgt

    // calculate the output
       VADDW             vr06, vr11, vr06                                                             // a2Lft b2Lft a2Lft [  262] ....................; // a2Rgt = a2Lft + b1Lft
       VADDW             vr07, vr12, vr07                                                             // a2Mdl b2Rgt a2Mdl [  263] ....................; // b2Lft = a2Mdl + b1Rgt

    // round, pack and save the result
       VASRRPWB          vr03, vr03, 1                                                                      // a1Lft a1Lft [  266] ....................; // round/pack the result
       VASRRPWB          vr04, vr04, 1                                                                      // a1Mdl a1Mdl [  267] ....................; // round/pack the result

    // round, pack and save the result
       VASRRPWB          vr06, vr06, 1                                                                      // a2Lft a2Lft [  270] ....................; // round/pack the result
       VASRRPWB          vr07, vr07, 1                                                                      // a2Mdl a2Mdl [  271] ....................; // round/pack the result

       VST64             vr03,[i2, 0]                                                                      // a1Lft outBuf [  273] ....................; //store output
       VST64             vr04,[i2, 8]                                                                      // a1Mdl outBuf [  274] ....................; //store output

       VST64             vr06,[i2, ME_REFERENCE_ARRAY_STRIDE]                                              // a2Lft outBuf [  276] ....................;//store output
       VST64             vr07,[i2, ME_REFERENCE_ARRAY_STRIDE+8]                                            // a2Mdl outBuf [  277] ....................;//store output

       VJD.i7            i7, r3                                                                     // count count lpStart [  279] 
       VIM               i0, i0, 2*ME_REFERENCE_ARRAY_STRIDE  //DS__________________________________________ in1Buf in1Buf [  280] 
       VIM               i1, i1, 2*ME_REFERENCE_ARRAY_STRIDE  //DS__________________________________________ in2Buf in2Buf [  281] 
       VIM               i2, i2, 2*ME_REFERENCE_ARRAY_STRIDE  //DS__________________________________________ outBuf outBuf [  282] 

// End                                                                                                                  // [  284] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz QPelFiltering_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QPelFiltering                                                                                                // [  285] 



//------------------------------------------------------------------------------
// Name:          VerticalFilteringAndRounding
// Purpose:       
//                
// Arguments:     inBuf
//                outBuf
//                iters
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] VerticalFilteringAndRounding                                                                                 // [  296] 
_Asm int Record_VerticalFilteringAndRounding(int *mac_tab)
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
VerticalFilteringAndRounding_RecLoop:
  ld r1,[r17 ,16]
  vrec r1

// p16    i0  inBuf                                                                                                     // [  297] 
// p16    i1  outBuf                                                                                                    // [  298] 
// p16    i2  iters                                                                                                     // [  299] 
// Begin                                                                                                                // [  300] 
//vec16  vr02  row00                                                                                                    // [  301] 
//vec16  vr03  row01                                                                                                    // [  302] 
//vec16  vr04  row02                                                                                                    // [  303] 
//vec16  vr05  row03                                                                                                    // [  304] 
//vec16  vr06  row04                                                                                                    // [  305] 
//vec16  vr07  row05                                                                                                    // [  306] 
//vec16  vr08  row06                                                                                                    // [  307] 
//vec16  vr09  row07                                                                                                    // [  308] 
//vec16  vr10  row08                                                                                                    // [  309] 
//vec16  vr11  row09                                                                                                    // [  310] 
//vec16  vr12  row10                                                                                                    // [  311] 
//vec16  vr13  row11                                                                                                    // [  312] 
//vec16  vr14  row12                                                                                                    // [  313] 
//vec16  vr15  row13                                                                                                    // [  314] 
//vec16  vr16  row14                                                                                                    // [  315] 
//vec16  vr17  row15                                                                                                    // [  316] 
//vec16  vr18  row16                                                                                                    // [  317] 
//vec16  vr19  row17                                                                                                    // [  318] 
//vec16  vr20  row18                                                                                                    // [  319] 
//vec16  vr21  row19                                                                                                    // [  320] 
//vec16  vr22  row20                                                                                                    // [  321] 
//vec16  vr23  work1                                                                                                    // [  322] 
//vec16  vr24  work2                                                                                                    // [  323] 
//vec16  vr25  work3                                                                                                    // [  324] 
    
LINE 326".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VIM               i2, i2, -1                                                                         // iters iters [  326] 
//lpStart --------                                                                                                      // [  327] 
 vendrec r3                                                                                                             // [  327] 
 vrec r3                                                                                                                // [  327] 
    // applies vertical filtering to buffer pointed to by inBuf and stores
    // result at outBuf
    // load the input 21 rows at offset of ME_REFERENCE_ARRAY_STRIDE bytes
LINE 331".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD64W            vr02,[i0, 0*ME_REFERENCE_ARRAY_STRIDE]                                             // row00 inBuf [  331] 
       VLD64W            vr03,[i0, 1*ME_REFERENCE_ARRAY_STRIDE]                                             // row01 inBuf [  332] 
       VLD64W            vr04,[i0, 2*ME_REFERENCE_ARRAY_STRIDE]                                             // row02 inBuf [  333] 
       VLD64W            vr05,[i0, 3*ME_REFERENCE_ARRAY_STRIDE]                                             // row03 inBuf [  334] 
       VLD64W            vr06,[i0, 4*ME_REFERENCE_ARRAY_STRIDE]                                             // row04 inBuf [  335] 
       VLD64W            vr07,[i0, 5*ME_REFERENCE_ARRAY_STRIDE]                                             // row05 inBuf [  336] 
       VLD64W            vr08,[i0, 6*ME_REFERENCE_ARRAY_STRIDE]                                             // row06 inBuf [  337] 
       VLD64W            vr09,[i0, 7*ME_REFERENCE_ARRAY_STRIDE]                                             // row07 inBuf [  338] 
       VLD64W            vr10,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]                                             // row08 inBuf [  339] 
       VLD64W            vr11,[i0, 9*ME_REFERENCE_ARRAY_STRIDE]                                             // row09 inBuf [  340] 
       VLD64W            vr12,[i0, 10*ME_REFERENCE_ARRAY_STRIDE]                                            // row10 inBuf [  341] 
       VLD64W            vr13,[i0, 11*ME_REFERENCE_ARRAY_STRIDE]                                            // row11 inBuf [  342] 
       VLD64W            vr14,[i0, 12*ME_REFERENCE_ARRAY_STRIDE]                                            // row12 inBuf [  343] 
       VLD64W            vr15,[i0, 13*ME_REFERENCE_ARRAY_STRIDE]                                            // row13 inBuf [  344] 
       VLD64W            vr16,[i0, 14*ME_REFERENCE_ARRAY_STRIDE]                                            // row14 inBuf [  345] 
       VLD64W            vr17,[i0, 15*ME_REFERENCE_ARRAY_STRIDE]                                            // row15 inBuf [  346] 
       VLD64W            vr18,[i0, 16*ME_REFERENCE_ARRAY_STRIDE]                                            // row16 inBuf [  347] 
       VLD64W            vr19,[i0, 17*ME_REFERENCE_ARRAY_STRIDE]                                            // row17 inBuf [  348] 
       VLD64W            vr20,[i0, 18*ME_REFERENCE_ARRAY_STRIDE]                                            // row18 inBuf [  349] 
       VLD64W            vr21,[i0, 19*ME_REFERENCE_ARRAY_STRIDE]                                            // row19 inBuf [  350] 
       VLD64W            vr22,[i0, 20*ME_REFERENCE_ARRAY_STRIDE]                                            // row20 inBuf [  351] 

    // calculate first output
       VBMULW            vr23, vr03, (-5)                                                                   // work1 row01 [  354] ....................; //acc += vr02*(-5)
       VBMULAW           vr23, vr04, (20)                                                                   // work1 row02 [  355] ....................; //acc += vr03*(20)
       VBMULAW           vr23, vr05, (20)                                                                   // work1 row03 [  356] ....................; //acc += vr04*(20)
       VBMULAW           vr23, vr06, (-5)                                                                   // work1 row04 [  357] ....................; //acc += vr05*(-5)
       VADDAW            vr23, vr02, vr07                                                             // work1 row00 row05 [  358] 

    // calculate second output
       VBMULW            vr24, vr04, (-5)                                                                   // work2 row02 [  361] ....................; //acc = vr03*(-5)
       VBMULAW           vr24, vr05, (20)                                                                   // work2 row03 [  362] ....................; //acc += vr04*(20)
       VBMULAW           vr24, vr06, (20)                                                                   // work2 row04 [  363] ....................; //acc += vr05*(20)
       VBMULAW           vr24, vr07, (-5)                                                                   // work2 row05 [  364] ....................; //acc += vr06*(-5)
       VADDAW            vr24, vr03, vr08                                                             // work2 row01 row06 [  365] 
 
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  367] ....................; //pack the 1st output

    // calculate third output
       VBMULW            vr25, vr05, (-5)                                                                   // work3 row03 [  370] ....................; //acc += vr04*(-5)
       VBMULAW           vr25, vr06, (20)                                                                   // work3 row04 [  371] ....................; //acc += vr05*(20)
       VBMULAW           vr25, vr07, (20)                                                                   // work3 row05 [  372] ....................; //acc += vr06*(20)
       VBMULAW           vr25, vr08, (-5)                                                                   // work3 row06 [  373] ....................; //acc += vr07*(-5)
       VADDAW            vr25, vr04, vr09                                                             // work3 row02 row07 [  374] 

       VST64             vr23,[i1, 0*ME_REFERENCE_ARRAY_STRIDE]                                            // work1 outBuf [  376] ....................; //store the first result
       VASRRPWB          vr24, vr24, 5                                                                      // work2 work2 [  377] ....................; //pack the second output


    // calculate fourth output
       VBMULW            vr23, vr06, (-5)                                                                   // work1 row04 [  381] ....................; //acc  = vr05*(-5)
       VBMULAW           vr23, vr07, (20)                                                                   // work1 row05 [  382] ....................; //acc += vr06*(20)
       VBMULAW           vr23, vr08, (20)                                                                   // work1 row06 [  383] ....................; //acc += vr07*(20)
       VBMULAW           vr23, vr09, (-5)                                                                   // work1 row07 [  384] ....................; //acc += vr08*(-5)
       VADDAW            vr23, vr05, vr10                                                             // work1 row03 row08 [  385] 

       VST64             vr24,[i1, 1*ME_REFERENCE_ARRAY_STRIDE]                                            // work2 outBuf [  387] ....................; //store the second result
       VASRRPWB          vr25, vr25, 5                                                                      // work3 work3 [  388] ....................; //pack the third output

    // calculate the 5th output
       VBMULW            vr24, vr07, (-5)                                                                   // work2 row05 [  391] ....................; //acc  = vr06*(-5)
       VBMULAW           vr24, vr08, (20)                                                                   // work2 row06 [  392] ....................; //acc += vr07*(20)
       VBMULAW           vr24, vr09, (20)                                                                   // work2 row07 [  393] ....................; //acc += vr08*(20)
       VBMULAW           vr24, vr10, (-5)                                                                   // work2 row08 [  394] ....................; //acc += vr09*(-5)
       VADDAW            vr24, vr06, vr11                                                             // work2 row04 row09 [  395] ....................; //acc += vr05+vr10

       VST64             vr25,[i1, 2*ME_REFERENCE_ARRAY_STRIDE]                                            // work3 outBuf [  397] ....................; // store the 3rd result
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  398] ....................; // pack the 4th output


    // calculate the 6th output
       VBMULW            vr25, vr08, (-5)                                                                   // work3 row06 [  402] ....................; //acc  = vr07*(-5)
       VBMULAW           vr25, vr09, (20)                                                                   // work3 row07 [  403] ....................; //acc += vr08*(20)
       VBMULAW           vr25, vr10, (20)                                                                   // work3 row08 [  404] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr11, (-5)                                                                   // work3 row09 [  405] ....................; //acc += vr10*(-5)
       VADDAW            vr25, vr07, vr12                                                             // work3 row05 row10 [  406] ....................; //acc += vr06+vr11

       VST64             vr23,[i1, 3*ME_REFERENCE_ARRAY_STRIDE]                                            // work1 outBuf [  408] ....................; // store the 4th result
       VASRRPWB          vr24, vr24, 5                                                                      // work2 work2 [  409] ....................; // pack the 5th output

    // calculate the 7th output
       VBMULW            vr23, vr09, (-5)                                                                   // work1 row07 [  412] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr10, (20)                                                                   // work1 row08 [  413] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr11, (20)                                                                   // work1 row09 [  414] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr12, (-5)                                                                   // work1 row10 [  415] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr08, vr13                                                             // work1 row06 row11 [  416] ....................; //acc += vr07+vr12

       VST64             vr24,[i1, 4*ME_REFERENCE_ARRAY_STRIDE]                                            // work2 outBuf [  418] ....................; // store the 5th result
       VASRRPWB          vr25, vr25, 5                                                                      // work3 work3 [  419] ....................; // pack the 6th output

    // calculate the 8th output
       VBMULW            vr24, vr10, (-5)                                                                   // work2 row08 [  422] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr11, (20)                                                                   // work2 row09 [  423] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr12, (20)                                                                   // work2 row10 [  424] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr13, (-5)                                                                   // work2 row11 [  425] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr09, vr14                                                             // work2 row07 row12 [  426] ....................; //acc += vr07+vr12

       VST64             vr25,[i1, 5*ME_REFERENCE_ARRAY_STRIDE]                                            // work3 outBuf [  428] ....................; // store the 6th result
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  429] ....................; // pack the 6th output

    // calculate the 9th output
       VBMULW            vr25, vr11, (-5)                                                                   // work3 row09 [  432] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr12, (20)                                                                   // work3 row10 [  433] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr13, (20)                                                                   // work3 row11 [  434] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr14, (-5)                                                                   // work3 row12 [  435] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr10, vr15                                                             // work3 row08 row13 [  436] ....................; //acc += vr07+vr12

       VST64             vr23,[i1, 6*ME_REFERENCE_ARRAY_STRIDE]                                            // work1 outBuf [  438] ....................; // store the 6th result
       VASRRPWB          vr24, vr24, 5                                                                      // work2 work2 [  439] ....................; // pack the 6th output

    // calculate the 10th output
       VBMULW            vr23, vr12, (-5)                                                                   // work1 row10 [  442] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr13, (20)                                                                   // work1 row11 [  443] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr14, (20)                                                                   // work1 row12 [  444] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr15, (-5)                                                                   // work1 row13 [  445] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr11, vr16                                                             // work1 row09 row14 [  446] ....................; //acc += vr07+vr12

       VST64             vr24,[i1, 7*ME_REFERENCE_ARRAY_STRIDE]                                            // work2 outBuf [  448] ....................; // store the 6th result
       VASRRPWB          vr25, vr25, 5                                                                      // work3 work3 [  449] ....................; // pack the 6th output

    // calculate the 11th output
       VBMULW            vr24, vr13, (-5)                                                                   // work2 row11 [  452] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr14, (20)                                                                   // work2 row12 [  453] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr15, (20)                                                                   // work2 row13 [  454] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr16, (-5)                                                                   // work2 row14 [  455] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr12, vr17                                                             // work2 row10 row15 [  456] ....................; //acc += vr07+vr12

       VST64             vr25,[i1, 8*ME_REFERENCE_ARRAY_STRIDE]                                            // work3 outBuf [  458] ....................; // store the 6th result
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  459] ....................; // pack the 6th output

    // calculate the 12th output
       VBMULW            vr25, vr14, (-5)                                                                   // work3 row12 [  462] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr15, (20)                                                                   // work3 row13 [  463] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr16, (20)                                                                   // work3 row14 [  464] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr17, (-5)                                                                   // work3 row15 [  465] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr13, vr18                                                             // work3 row11 row16 [  466] ....................; //acc += vr07+vr12

       VST64             vr23,[i1, 9*ME_REFERENCE_ARRAY_STRIDE]                                            // work1 outBuf [  468] ....................; // store the 6th result
       VASRRPWB          vr24, vr24, 5                                                                      // work2 work2 [  469] ....................; // pack the 6th output

    // calculate the 13th output
       VBMULW            vr23, vr15, (-5)                                                                   // work1 row13 [  472] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr16, (20)                                                                   // work1 row14 [  473] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr17, (20)                                                                   // work1 row15 [  474] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr18, (-5)                                                                   // work1 row16 [  475] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr14, vr19                                                             // work1 row12 row17 [  476] ....................; //acc += vr07+vr12

       VST64             vr24,[i1, 10*ME_REFERENCE_ARRAY_STRIDE]                                           // work2 outBuf [  478] ....................; // store the 6th result
       VASRRPWB          vr25, vr25, 5                                                                      // work3 work3 [  479] ....................; // pack the 6th output

    // calculate the 14th output
       VBMULW            vr24, vr16, (-5)                                                                   // work2 row14 [  482] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr17, (20)                                                                   // work2 row15 [  483] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr18, (20)                                                                   // work2 row16 [  484] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr19, (-5)                                                                   // work2 row17 [  485] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr15, vr20                                                             // work2 row13 row18 [  486] ....................; //acc += vr07+vr12

       VST64             vr25,[i1, 11*ME_REFERENCE_ARRAY_STRIDE]                                           // work3 outBuf [  488] ....................; // store the 6th result
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  489] ....................; // pack the 6th output

    // calculate the 15th output
       VBMULW            vr25, vr17, (-5)                                                                   // work3 row15 [  492] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr18, (20)                                                                   // work3 row16 [  493] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr19, (20)                                                                   // work3 row17 [  494] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr20, (-5)                                                                   // work3 row18 [  495] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr16, vr21                                                             // work3 row14 row19 [  496] ....................; //acc += vr07+vr12

       VST64             vr23,[i1, 12*ME_REFERENCE_ARRAY_STRIDE]                                           // work1 outBuf [  498] ....................; // store the 6th result
       VASRRPWB          vr24, vr24, 5                                                                      // work2 work2 [  499] ....................; // pack the 6th output

    // calculate the 16th output
       VBMULW            vr23, vr18, (-5)                                                                   // work1 row16 [  502] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr19, (20)                                                                   // work1 row17 [  503] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr20, (20)                                                                   // work1 row18 [  504] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr21, (-5)                                                                   // work1 row19 [  505] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr17, vr22                                                             // work1 row15 row20 [  506] ....................; //acc += vr07+vr12

       VST64             vr24,[i1, 13*ME_REFERENCE_ARRAY_STRIDE]                                           // work2 outBuf [  508] ....................; // store the 6th result

       VASRRPWB          vr25, vr25, 5                                                                      // work3 work3 [  510] ....................; // pack the 7th output
              //  Stall: [vr23]  ...............................      vr23          vr25    
       VASRRPWB          vr23, vr23, 5                                                                      // work1 work1 [  511] ....................; // pack the 8th output
    
       VIM               i0, i0, 8                                                                          // inBuf inBuf [  513] 
    

       VJD.i2            i2, r3                                                                     // iters iters lpStart [  516] 
       VST64             vr25,[i1, 14*ME_REFERENCE_ARRAY_STRIDE]  //DS_______________________________________ work3 outBuf [  517] ....................; // store the 7th result
       VST64             vr23,[i1, 15*ME_REFERENCE_ARRAY_STRIDE]  //DS_______________________________________ work1 outBuf [  518] ....................; // store the 8th result
       VIM               i1, i1, 8  //DS____________________________________________________________________ outBuf outBuf [  519] 
  

// End                                                                                                                  // [  522] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz VerticalFilteringAndRounding_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc VerticalFilteringAndRounding                                                                                 // [  523] 



//------------------------------------------------------------------------------
// Name:          VerticalFilteringWORounding
// Purpose:       
//                
// Arguments:     inBuf
//                outBuf
//                iters
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] VerticalFilteringWORounding                                                                                  // [  534] 
_Asm int Record_VerticalFilteringWORounding(int *mac_tab)
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
VerticalFilteringWORounding_RecLoop:
  ld r1,[r17 ,20]
  vrec r1

// p16    i0  inBuf                                                                                                     // [  535] 
// p16    i1  outBuf                                                                                                    // [  536] 
// p16    i2  iters                                                                                                     // [  537] 
// Begin                                                                                                                // [  538] 
//vec16  vr02  row00                                                                                                    // [  539] 
//vec16  vr03  row01                                                                                                    // [  540] 
//vec16  vr04  row02                                                                                                    // [  541] 
//vec16  vr05  row03                                                                                                    // [  542] 
//vec16  vr06  row04                                                                                                    // [  543] 
//vec16  vr07  row05                                                                                                    // [  544] 
//vec16  vr08  row06                                                                                                    // [  545] 
//vec16  vr09  row07                                                                                                    // [  546] 
//vec16  vr10  row08                                                                                                    // [  547] 
//vec16  vr11  row09                                                                                                    // [  548] 
//vec16  vr12  row10                                                                                                    // [  549] 
//vec16  vr13  row11                                                                                                    // [  550] 
//vec16  vr14  row12                                                                                                    // [  551] 
//vec16  vr15  row13                                                                                                    // [  552] 
//vec16  vr16  row14                                                                                                    // [  553] 
//vec16  vr17  row15                                                                                                    // [  554] 
//vec16  vr18  row16                                                                                                    // [  555] 
//vec16  vr19  row17                                                                                                    // [  556] 
//vec16  vr20  row18                                                                                                    // [  557] 
//vec16  vr21  row19                                                                                                    // [  558] 
//vec16  vr22  row20                                                                                                    // [  559] 
//vec16  vr23  work1                                                                                                    // [  560] 
//vec16  vr24  work2                                                                                                    // [  561] 
//vec16  vr25  work3                                                                                                    // [  562] 
    
LINE 564".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VIM               i2, i2, -1                                                                         // iters iters [  564] 
//lpStart --------                                                                                                      // [  565] 
 vendrec r3                                                                                                             // [  565] 
 vrec r3                                                                                                                // [  565] 
    // applies vertical filtering to buffer pointed to by inBuf and stores
    // result at outBuf
    // load the input 21 rows at offset of ME_REFERENCE_ARRAY_STRIDE bytes
LINE 569".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD64W            vr02,[i0, 0*ME_REFERENCE_ARRAY_STRIDE]                                             // row00 inBuf [  569] 
       VLD64W            vr03,[i0, 1*ME_REFERENCE_ARRAY_STRIDE]                                             // row01 inBuf [  570] 
       VLD64W            vr04,[i0, 2*ME_REFERENCE_ARRAY_STRIDE]                                             // row02 inBuf [  571] 
       VLD64W            vr05,[i0, 3*ME_REFERENCE_ARRAY_STRIDE]                                             // row03 inBuf [  572] 
       VLD64W            vr06,[i0, 4*ME_REFERENCE_ARRAY_STRIDE]                                             // row04 inBuf [  573] 
       VLD64W            vr07,[i0, 5*ME_REFERENCE_ARRAY_STRIDE]                                             // row05 inBuf [  574] 
       VLD64W            vr08,[i0, 6*ME_REFERENCE_ARRAY_STRIDE]                                             // row06 inBuf [  575] 
       VLD64W            vr09,[i0, 7*ME_REFERENCE_ARRAY_STRIDE]                                             // row07 inBuf [  576] 
       VLD64W            vr10,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]                                             // row08 inBuf [  577] 
       VLD64W            vr11,[i0, 9*ME_REFERENCE_ARRAY_STRIDE]                                             // row09 inBuf [  578] 
       VLD64W            vr12,[i0, 10*ME_REFERENCE_ARRAY_STRIDE]                                            // row10 inBuf [  579] 
       VLD64W            vr13,[i0, 11*ME_REFERENCE_ARRAY_STRIDE]                                            // row11 inBuf [  580] 
       VLD64W            vr14,[i0, 12*ME_REFERENCE_ARRAY_STRIDE]                                            // row12 inBuf [  581] 
       VLD64W            vr15,[i0, 13*ME_REFERENCE_ARRAY_STRIDE]                                            // row13 inBuf [  582] 
       VLD64W            vr16,[i0, 14*ME_REFERENCE_ARRAY_STRIDE]                                            // row14 inBuf [  583] 
       VLD64W            vr17,[i0, 15*ME_REFERENCE_ARRAY_STRIDE]                                            // row15 inBuf [  584] 
       VLD64W            vr18,[i0, 16*ME_REFERENCE_ARRAY_STRIDE]                                            // row16 inBuf [  585] 
       VLD64W            vr19,[i0, 17*ME_REFERENCE_ARRAY_STRIDE]                                            // row17 inBuf [  586] 
       VLD64W            vr20,[i0, 18*ME_REFERENCE_ARRAY_STRIDE]                                            // row18 inBuf [  587] 
       VLD64W            vr21,[i0, 19*ME_REFERENCE_ARRAY_STRIDE]                                            // row19 inBuf [  588] 
       VLD64W            vr22,[i0, 20*ME_REFERENCE_ARRAY_STRIDE]                                            // row20 inBuf [  589] 

    // calculate first output
       VBMULW            vr23, vr03, (-5)                                                                   // work1 row01 [  592] ....................; //acc += vr02*(-5)
       VBMULAW           vr23, vr04, (20)                                                                   // work1 row02 [  593] ....................; //acc += vr03*(20)
       VBMULAW           vr23, vr05, (20)                                                                   // work1 row03 [  594] ....................; //acc += vr04*(20)
       VBMULAW           vr23, vr06, (-5)                                                                   // work1 row04 [  595] ....................; //acc += vr05*(-5)
       VADDAW            vr23, vr02, vr07                                                             // work1 row00 row05 [  596] 

    // calculate second output
       VBMULW            vr24, vr04, (-5)                                                                   // work2 row02 [  599] ....................; //acc = vr03*(-5)
       VBMULAW           vr24, vr05, (20)                                                                   // work2 row03 [  600] ....................; //acc += vr04*(20)
       VBMULAW           vr24, vr06, (20)                                                                   // work2 row04 [  601] ....................; //acc += vr05*(20)
       VBMULAW           vr24, vr07, (-5)                                                                   // work2 row05 [  602] ....................; //acc += vr06*(-5)
       VADDAW            vr24, vr03, vr08                                                             // work2 row01 row06 [  603] 
 
    // calculate third output
       VBMULW            vr25, vr05, (-5)                                                                   // work3 row03 [  606] ....................; //acc += vr04*(-5)
       VBMULAW           vr25, vr06, (20)                                                                   // work3 row04 [  607] ....................; //acc += vr05*(20)
       VBMULAW           vr25, vr07, (20)                                                                   // work3 row05 [  608] ....................; //acc += vr06*(20)
       VBMULAW           vr25, vr08, (-5)                                                                   // work3 row06 [  609] ....................; //acc += vr07*(-5)
       VADDAW            vr25, vr04, vr09                                                             // work3 row02 row07 [  610] 

       VST128            vr23,[i1, 0*ME_WORKING_ARRAY_STRIDE]                                              // work1 outBuf [  612] ....................; //store the first result

    // calculate fourth output
       VBMULW            vr23, vr06, (-5)                                                                   // work1 row04 [  615] ....................; //acc  = vr05*(-5)
       VBMULAW           vr23, vr07, (20)                                                                   // work1 row05 [  616] ....................; //acc += vr06*(20)
       VBMULAW           vr23, vr08, (20)                                                                   // work1 row06 [  617] ....................; //acc += vr07*(20)
       VBMULAW           vr23, vr09, (-5)                                                                   // work1 row07 [  618] ....................; //acc += vr08*(-5)
       VADDAW            vr23, vr05, vr10                                                             // work1 row03 row08 [  619] 

       VST128            vr24,[i1, 1*ME_WORKING_ARRAY_STRIDE]                                              // work2 outBuf [  621] ....................; //store the second result

    // calculate the 5th output
       VBMULW            vr24, vr07, (-5)                                                                   // work2 row05 [  624] ....................; //acc  = vr06*(-5)
       VBMULAW           vr24, vr08, (20)                                                                   // work2 row06 [  625] ....................; //acc += vr07*(20)
       VBMULAW           vr24, vr09, (20)                                                                   // work2 row07 [  626] ....................; //acc += vr08*(20)
       VBMULAW           vr24, vr10, (-5)                                                                   // work2 row08 [  627] ....................; //acc += vr09*(-5)
       VADDAW            vr24, vr06, vr11                                                             // work2 row04 row09 [  628] ....................; //acc += vr05+vr10

       VST128            vr25,[i1, 2*ME_WORKING_ARRAY_STRIDE]                                              // work3 outBuf [  630] ....................; // store the 3rd result

    // calculate the 6th output
       VBMULW            vr25, vr08, (-5)                                                                   // work3 row06 [  633] ....................; //acc  = vr07*(-5)
       VBMULAW           vr25, vr09, (20)                                                                   // work3 row07 [  634] ....................; //acc += vr08*(20)
       VBMULAW           vr25, vr10, (20)                                                                   // work3 row08 [  635] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr11, (-5)                                                                   // work3 row09 [  636] ....................; //acc += vr10*(-5)
       VADDAW            vr25, vr07, vr12                                                             // work3 row05 row10 [  637] ....................; //acc += vr06+vr11

       VST128            vr23,[i1, 3*ME_WORKING_ARRAY_STRIDE]                                              // work1 outBuf [  639] ....................; // store the 4th result

    // calculate the 7th output
       VBMULW            vr23, vr09, (-5)                                                                   // work1 row07 [  642] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr10, (20)                                                                   // work1 row08 [  643] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr11, (20)                                                                   // work1 row09 [  644] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr12, (-5)                                                                   // work1 row10 [  645] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr08, vr13                                                             // work1 row06 row11 [  646] ....................; //acc += vr07+vr12

       VST128            vr24,[i1, 4*ME_WORKING_ARRAY_STRIDE]                                              // work2 outBuf [  648] ....................; // store the 5th result

    // calculate the 8th output
       VBMULW            vr24, vr10, (-5)                                                                   // work2 row08 [  651] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr11, (20)                                                                   // work2 row09 [  652] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr12, (20)                                                                   // work2 row10 [  653] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr13, (-5)                                                                   // work2 row11 [  654] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr09, vr14                                                             // work2 row07 row12 [  655] ....................; //acc += vr07+vr12

       VST128            vr25,[i1, 5*ME_WORKING_ARRAY_STRIDE]                                              // work3 outBuf [  657] ....................; // store the 6th result

    // calculate the 9th output
       VBMULW            vr25, vr11, (-5)                                                                   // work3 row09 [  660] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr12, (20)                                                                   // work3 row10 [  661] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr13, (20)                                                                   // work3 row11 [  662] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr14, (-5)                                                                   // work3 row12 [  663] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr10, vr15                                                             // work3 row08 row13 [  664] ....................; //acc += vr07+vr12

       VST128            vr23,[i1, 6*ME_WORKING_ARRAY_STRIDE]                                              // work1 outBuf [  666] ....................; // store the 6th result

    // calculate the 10th output
       VBMULW            vr23, vr12, (-5)                                                                   // work1 row10 [  669] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr13, (20)                                                                   // work1 row11 [  670] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr14, (20)                                                                   // work1 row12 [  671] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr15, (-5)                                                                   // work1 row13 [  672] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr11, vr16                                                             // work1 row09 row14 [  673] ....................; //acc += vr07+vr12

       VST128            vr24,[i1, 7*ME_WORKING_ARRAY_STRIDE]                                              // work2 outBuf [  675] ....................; // store the 6th result

    // calculate the 11th output
       VBMULW            vr24, vr13, (-5)                                                                   // work2 row11 [  678] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr14, (20)                                                                   // work2 row12 [  679] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr15, (20)                                                                   // work2 row13 [  680] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr16, (-5)                                                                   // work2 row14 [  681] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr12, vr17                                                             // work2 row10 row15 [  682] ....................; //acc += vr07+vr12

       VST128            vr25,[i1, 8*ME_WORKING_ARRAY_STRIDE]                                              // work3 outBuf [  684] ....................; // store the 6th result

    // calculate the 12th output
       VBMULW            vr25, vr14, (-5)                                                                   // work3 row12 [  687] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr15, (20)                                                                   // work3 row13 [  688] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr16, (20)                                                                   // work3 row14 [  689] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr17, (-5)                                                                   // work3 row15 [  690] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr13, vr18                                                             // work3 row11 row16 [  691] ....................; //acc += vr07+vr12

       VST128            vr23,[i1, 9*ME_WORKING_ARRAY_STRIDE]                                              // work1 outBuf [  693] ....................; // store the 6th result

    // calculate the 13th output
       VBMULW            vr23, vr15, (-5)                                                                   // work1 row13 [  696] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr16, (20)                                                                   // work1 row14 [  697] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr17, (20)                                                                   // work1 row15 [  698] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr18, (-5)                                                                   // work1 row16 [  699] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr14, vr19                                                             // work1 row12 row17 [  700] ....................; //acc += vr07+vr12

       VST128            vr24,[i1, 10*ME_WORKING_ARRAY_STRIDE]                                             // work2 outBuf [  702] ....................; // store the 6th result

    // calculate the 14th output
       VBMULW            vr24, vr16, (-5)                                                                   // work2 row14 [  705] ....................; //acc  = vr08*(-5)
       VBMULAW           vr24, vr17, (20)                                                                   // work2 row15 [  706] ....................; //acc += vr09*(20)
       VBMULAW           vr24, vr18, (20)                                                                   // work2 row16 [  707] ....................; //acc += vr10*(20)
       VBMULAW           vr24, vr19, (-5)                                                                   // work2 row17 [  708] ....................; //acc += vr11*(-5)
       VADDAW            vr24, vr15, vr20                                                             // work2 row13 row18 [  709] ....................; //acc += vr07+vr12

       VST128            vr25,[i1, 11*ME_WORKING_ARRAY_STRIDE]                                             // work3 outBuf [  711] ....................; // store the 6th result

    // calculate the 15th output
       VBMULW            vr25, vr17, (-5)                                                                   // work3 row15 [  714] ....................; //acc  = vr08*(-5)
       VBMULAW           vr25, vr18, (20)                                                                   // work3 row16 [  715] ....................; //acc += vr09*(20)
       VBMULAW           vr25, vr19, (20)                                                                   // work3 row17 [  716] ....................; //acc += vr10*(20)
       VBMULAW           vr25, vr20, (-5)                                                                   // work3 row18 [  717] ....................; //acc += vr11*(-5)
       VADDAW            vr25, vr16, vr21                                                             // work3 row14 row19 [  718] ....................; //acc += vr07+vr12

       VST128            vr23,[i1, 12*ME_WORKING_ARRAY_STRIDE]                                             // work1 outBuf [  720] ....................; // store the 6th result

    // calculate the 16th output
       VBMULW            vr23, vr18, (-5)                                                                   // work1 row16 [  723] ....................; //acc  = vr08*(-5)
       VBMULAW           vr23, vr19, (20)                                                                   // work1 row17 [  724] ....................; //acc += vr09*(20)
       VBMULAW           vr23, vr20, (20)                                                                   // work1 row18 [  725] ....................; //acc += vr10*(20)
       VBMULAW           vr23, vr21, (-5)                                                                   // work1 row19 [  726] ....................; //acc += vr11*(-5)
       VADDAW            vr23, vr17, vr22                                                             // work1 row15 row20 [  727] ....................; //acc += vr07+vr12

       VST128            vr24,[i1, 13*ME_WORKING_ARRAY_STRIDE]                                             // work2 outBuf [  729] ....................; // store the 6th result
       VST128            vr25,[i1, 14*ME_WORKING_ARRAY_STRIDE]                                             // work3 outBuf [  730] ....................; // store the 7th result

       VJD.i2            i2, r3                                                                     // iters iters lpStart [  732] 
       VST128            vr23,[i1, 15*ME_WORKING_ARRAY_STRIDE]  //DS_________________________________________ work1 outBuf [  733] ....................; // store the 8th result
       VIM               i0, i0, 8  //DS______________________________________________________________________ inBuf inBuf [  734] 
       VIM               i1, i1, 16  //DS___________________________________________________________________ outBuf outBuf [  735] 
 

// End                                                                                                                  // [  738] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz VerticalFilteringWORounding_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc VerticalFilteringWORounding                                                                                  // [  739] 



//------------------------------------------------------------------------------
// Name:          CentrePelFilterWOInpRounding
// Purpose:       
//                
// Arguments:     inBuf
//                outBuf
//                
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CentrePelFilterWOInpRounding                                                                                 // [  750] 
_Asm int Record_CentrePelFilterWOInpRounding(int *mac_tab)
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
CentrePelFilterWOInpRounding_RecLoop:
  ld r1,[r17 ,24]
  vrec r1

// p16    i0  inBuf                                                                                                     // [  751] 
// p16    i1  outBuf                                                                                                    // [  752] 
// Begin                                                                                                                // [  753] 
// p16    i2  iters                                                                                                     // [  754] 
//vec16  vr02  a1Lft                                                                                                    // [  755] 
//vec16  vr03  a1Mdl                                                                                                    // [  756] 
//vec16  vr04  a1Rgt                                                                                                    // [  757] 
//vec16  vr05  a2Lft                                                                                                    // [  758] 
//vec16  vr06  a2Mdl                                                                                                    // [  759] 
//vec16  vr07  a2Rgt                                                                                                    // [  760] 
//vec16  vr08  a1Offset1                                                                                                // [  761] 
//vec16  vr09  a1Offset2                                                                                                // [  762] 
//vec16  vr10  a1Offset3                                                                                                // [  763] 
//vec16  vr11  a1Offset4                                                                                                // [  764] 
//vec16  vr12  a1Offset5                                                                                                // [  765] 
//vec16  vr13  a1Offset6                                                                                                // [  766] 
//vec16  vr14  a2Offset1                                                                                                // [  767] 
//vec16  vr15  a2Offset2                                                                                                // [  768] 
//vec16  vr16  a2Offset3                                                                                                // [  769] 
//vec16  vr17  a2Offset4                                                                                                // [  770] 
//vec16  vr18  a2Offset5                                                                                                // [  771] 
//vec16  vr19  a2Offset6                                                                                                // [  772] 

LINE 774".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VMOVW             vr00, 8-1, 4                                                                             // iters [  774] 
//lpStart --------                                                                                                      // [  775] 
 vendrec r3                                                                                                             // [  775] 
 vrec r3                                                                                                                // [  775] 
    // load input data  ROW-1
LINE 777".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD128            vr02,[i0, 0]                                                                       // a1Lft inBuf [  777] ....................;  // a1Lft = w08 w07 w06 w05 w04 w03 w02 w01
       VLD128            vr03,[i0, 16]                                                                      // a1Mdl inBuf [  778] ....................;  // a1Mdl = w16 w15 w14 w13 w12 w11 w10 w09
       VLD128            vr04,[i0, 32]                                                                      // a1Rgt inBuf [  779] ....................;   // a1Rgt = 000 000 000 w21 w20 w19 w18 w17

    // load input data  ROW-2
       VLD128            vr05,[i0, ME_WORKING_ARRAY_STRIDE]                                                 // a2Lft inBuf [  782] ....................;  // a1Lft = w08 w07 w06 w05 w04 w03 w02 w01
       VLD128            vr06,[i0, ME_WORKING_ARRAY_STRIDE+16]                                              // a2Mdl inBuf [  783] ....................;  // a1Mdl = w16 w15 w14 w13 w12 w11 w10 w09
       VLD128            vr07,[i0, ME_WORKING_ARRAY_STRIDE+32]                                              // a2Rgt inBuf [  784] ....................;  // a1Rgt = 000 000 000 w21 w20 w19 w18 w17

    // arrange inp: row-1/set-1
       VMR2W             vr08, vr03, vr02                                                         // a1Offset1 a1Mdl a1Lft [  787] ....................;  // a1Offset1 = w10 w09 w08 w07 w06 w05 w04 w03
       VMR4W             vr09, vr03, vr02                                                         // a1Offset2 a1Mdl a1Lft [  788] ....................;  // a1Offset2 = w12 w11 w10 w09 w08 w07 w06 w05
       VMR6W             vr10, vr03, vr02                                                         // a1Offset3 a1Mdl a1Lft [  789] ....................;  // a1Offset3 = w14 w13 w12 w11 w10 w09 w08 w07

    // arrange inp: row-1/set-2
       VMR2W             vr11, vr04, vr03                                                         // a1Offset4 a1Rgt a1Mdl [  792] ....................; // a1Offset4 = w18 w17 w16 w15 w14 w13 w12 w11
       VMR4W             vr12, vr04, vr03                                                         // a1Offset5 a1Rgt a1Mdl [  793] ....................; // a1Offset5 = w20 w19 w18 w17 w16 w15 w14 w13                
       VMR6W             vr13, vr04, vr03                                                         // a1Offset6 a1Rgt a1Mdl [  794] ....................; // a1Offset6 = 000 w21 w20 w19 w18 w17 w16 w15

    // apply filter: row-1/set-1
       VD6TAPF           vr08, vr08, 0xC                                                            // a1Offset1 a1Offset1 [  797] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr08, vr02, 0x3                                                                // a1Offset1 a1Lft [  798] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr08, vr09, 0x30                                                           // a1Offset1 a1Offset2 [  799] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr08, vr10, 0xC0                                                           // a1Offset1 a1Offset3 [  800] ....................; // x x 0 0 0 0 0 0 
    
    // apply filter: row-1/set-2
       VD6TAPF           vr11, vr11, 0xC                                                            // a1Offset4 a1Offset4 [  803] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr11, vr03, 0x3                                                                // a1Offset4 a1Mdl [  804] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr11, vr12, 0x30                                                           // a1Offset4 a1Offset5 [  805] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr11, vr13, 0xC0                                                           // a1Offset4 a1Offset6 [  806] ....................; // x x 0 0 0 0 0 0

    // arrange inp: row-2/set-1
       VMR2W             vr14, vr06, vr05                                                         // a2Offset1 a2Mdl a2Lft [  809] ....................;  // a1Offset1 = w10 w09 w08 w07 w06 w05 w04 w03
       VMR4W             vr15, vr06, vr05                                                         // a2Offset2 a2Mdl a2Lft [  810] ....................;  // a1Offset2 = w12 w11 w10 w09 w08 w07 w06 w05
       VMR6W             vr16, vr06, vr05                                                         // a2Offset3 a2Mdl a2Lft [  811] ....................;  // a1Offset3 = w14 w13 w12 w11 w10 w09 w08 w07

    // arrange inp: row-2/set-2
       VMR2W             vr17, vr07, vr06                                                         // a2Offset4 a2Rgt a2Mdl [  814] ....................; // a1Offset4 = w18 w17 w16 w15 w14 w13 w12 w11
       VMR4W             vr18, vr07, vr06                                                         // a2Offset5 a2Rgt a2Mdl [  815] ....................; // a1Offset5 = w20 w19 w18 w17 w16 w15 w14 w13                
       VMR6W             vr19, vr07, vr06                                                         // a2Offset6 a2Rgt a2Mdl [  816] ....................; // a1Offset6 = 000 w21 w20 w19 w18 w17 w16 w15

    // apply filter: row-2/set-1
       VD6TAPF           vr14, vr14, 0xC                                                            // a2Offset1 a2Offset1 [  819] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr14, vr05, 0x3                                                                // a2Offset1 a2Lft [  820] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr14, vr15, 0x30                                                           // a2Offset1 a2Offset2 [  821] ....................; // 0 0 x x 0 0 0 0 
       VD6TAPF           vr14, vr16, 0xC0                                                           // a2Offset1 a2Offset3 [  822] ....................; // x x 0 0 0 0 0 0 
    
    // apply filter: row-2/set-2
       VD6TAPF           vr17, vr17, 0xC                                                            // a2Offset4 a2Offset4 [  825] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr17, vr06, 0x3                                                                // a2Offset4 a2Mdl [  826] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr17, vr18, 0x30                                                           // a2Offset4 a2Offset5 [  827] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr17, vr19, 0xC0                                                           // a2Offset4 a2Offset6 [  828] ....................; // x x 0 0 0 0 0 0

    // pack and store the result
       VASRRPWB          vr08, vr08, 0                                                              // a1Offset1 a1Offset1 [  831] ....................;   // pack the registers: row-1/set-1
       VASRRPWB          vr11, vr11, 0                                                              // a1Offset4 a1Offset4 [  832] ....................;   // pack the registers: row-1/set-2
       VASRRPWB          vr14, vr14, 0                                                              // a2Offset1 a2Offset1 [  833] ....................;   // pack the registers: row-1/set-2
       VASRRPWB          vr17, vr17, 0                                                              // a2Offset4 a2Offset4 [  834] ....................;   // pack the registers: row-2/set-2

       VST64             vr08,[i1, 0]                                                                  // a1Offset1 outBuf [  836] ....................;   // store the result: row-1/set-1
       VST64             vr11,[i1, 8]                                                                  // a1Offset4 outBuf [  837] ....................;   // store the result: row-1/set-2
       VST64             vr14,[i1, ME_REFERENCE_ARRAY_STRIDE]                                          // a2Offset1 outBuf [  838] ....................;   // store the result: row-2/set-2
 
       VJD.i2            i2, r3                                                                     // iters iters lpStart [  840] 
       VST64             vr17,[i1, ME_REFERENCE_ARRAY_STRIDE+8]  //DS____________________________________ a2Offset4 outBuf [  841] ....................;   // store the result: row-2/set-2
       VIM               i0, i0, 2*ME_WORKING_ARRAY_STRIDE  //DS______________________________________________ inBuf inBuf [  842] 
       VIM               i1, i1, 2*ME_REFERENCE_ARRAY_STRIDE  //DS__________________________________________ outBuf outBuf [  843] 
  
    
// End                                                                                                                  // [  846] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz CentrePelFilterWOInpRounding_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CentrePelFilterWOInpRounding                                                                                 // [  847] 



//------------------------------------------------------------------------------
// Name:          CentrePelFilAndInpRounding
// Purpose:       
//                
// Arguments:     inBuf
//                out1Buf
//                out2Buf
//                
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CentrePelFilAndInpRounding                                                                                   // [  859] 
_Asm int Record_CentrePelFilAndInpRounding(int *mac_tab)
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
CentrePelFilAndInpRounding_RecLoop:
  ld r1,[r17 ,28]
  vrec r1

// p16    i0  inBuf                                                                                                     // [  860] 
// p16    i1  out1Buf                                                                                                   // [  861] 
// p16    i2  out2Buf                                                                                                   // [  862] 
// Begin                                                                                                                // [  863] 
// p16    i3  iters                                                                                                     // [  864] 
//vec16  vr02  a1Lft                                                                                                    // [  865] 
//vec16  vr03  a1Mdl                                                                                                    // [  866] 
//vec16  vr04  a1Rgt                                                                                                    // [  867] 
//vec16  vr05  a2Lft                                                                                                    // [  868] 
//vec16  vr06  a2Mdl                                                                                                    // [  869] 
//vec16  vr07  a2Rgt                                                                                                    // [  870] 
//vec16  vr08  a1Offset1                                                                                                // [  871] 
//vec16  vr09  a1Offset2                                                                                                // [  872] 
//vec16  vr10  a1Offset3                                                                                                // [  873] 
//vec16  vr11  a1Offset4                                                                                                // [  874] 
//vec16  vr12  a1Offset5                                                                                                // [  875] 
//vec16  vr13  a1Offset6                                                                                                // [  876] 
//vec16  vr14  a2Offset1                                                                                                // [  877] 
//vec16  vr15  a2Offset2                                                                                                // [  878] 
//vec16  vr16  a2Offset3                                                                                                // [  879] 
//vec16  vr17  a2Offset4                                                                                                // [  880] 
//vec16  vr18  a2Offset5                                                                                                // [  881] 
//vec16  vr19  a2Offset6                                                                                                // [  882] 

LINE 884".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VMOVW             vr00, 8-1, 8                                                                             // iters [  884] 
//lpStart --------                                                                                                      // [  885] 
 vendrec r3                                                                                                             // [  885] 
 vrec r3                                                                                                                // [  885] 
    // load input data  ROW-1
LINE 887".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD128            vr02,[i0, 0]                                                                       // a1Lft inBuf [  887] ....................;  // a1Lft = w08 w07 w06 w05 w04 w03 w02 w01
       VLD128            vr03,[i0, 16]                                                                      // a1Mdl inBuf [  888] ....................;  // a1Mdl = w16 w15 w14 w13 w12 w11 w10 w09
       VLD128            vr04,[i0, 32]                                                                      // a1Rgt inBuf [  889] ....................;   // a1Rgt = 000 000 000 w21 w20 w19 w18 w17

    // load input data  ROW-2
       VLD128            vr05,[i0, ME_WORKING_ARRAY_STRIDE]                                                 // a2Lft inBuf [  892] ....................;  // a1Lft = w08 w07 w06 w05 w04 w03 w02 w01
       VLD128            vr06,[i0, ME_WORKING_ARRAY_STRIDE+16]                                              // a2Mdl inBuf [  893] ....................;  // a1Mdl = w16 w15 w14 w13 w12 w11 w10 w09
       VLD128            vr07,[i0, ME_WORKING_ARRAY_STRIDE+32]                                              // a2Rgt inBuf [  894] ....................;  // a1Rgt = 000 000 000 w21 w20 w19 w18 w17

    // arrange inp: row-1/set-1
       VMR2W             vr08, vr03, vr02                                                         // a1Offset1 a1Mdl a1Lft [  897] ....................;  // a1Offset1 = w10 w09 w08 w07 w06 w05 w04 w03
       VMR4W             vr09, vr03, vr02                                                         // a1Offset2 a1Mdl a1Lft [  898] ....................;  // a1Offset2 = w12 w11 w10 w09 w08 w07 w06 w05
       VMR6W             vr10, vr03, vr02                                                         // a1Offset3 a1Mdl a1Lft [  899] ....................;  // a1Offset3 = w14 w13 w12 w11 w10 w09 w08 w07

    // arrange inp: row-1/set-2
       VMR2W             vr11, vr04, vr03                                                         // a1Offset4 a1Rgt a1Mdl [  902] ....................; // a1Offset4 = w18 w17 w16 w15 w14 w13 w12 w11
       VMR4W             vr12, vr04, vr03                                                         // a1Offset5 a1Rgt a1Mdl [  903] ....................; // a1Offset5 = w20 w19 w18 w17 w16 w15 w14 w13                
       VMR6W             vr13, vr04, vr03                                                         // a1Offset6 a1Rgt a1Mdl [  904] ....................; // a1Offset6 = 000 w21 w20 w19 w18 w17 w16 w15

    // apply filter: row-1/set-1
       VD6TAPF           vr08, vr08, 0xC                                                            // a1Offset1 a1Offset1 [  907] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr08, vr02, 0x3                                                                // a1Offset1 a1Lft [  908] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr08, vr09, 0x30                                                           // a1Offset1 a1Offset2 [  909] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr08, vr10, 0xC0                                                           // a1Offset1 a1Offset3 [  910] ....................; // x x 0 0 0 0 0 0 
    
    // apply filter: row-1/set-2
       VD6TAPF           vr11, vr11, 0xC                                                            // a1Offset4 a1Offset4 [  913] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr11, vr03, 0x3                                                                // a1Offset4 a1Mdl [  914] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr11, vr12, 0x30                                                           // a1Offset4 a1Offset5 [  915] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr11, vr13, 0xC0                                                           // a1Offset4 a1Offset6 [  916] ....................; // x x 0 0 0 0 0 0

    // arrange inp: row-2/set-1
       VMR2W             vr14, vr06, vr05                                                         // a2Offset1 a2Mdl a2Lft [  919] ....................;  // a1Offset1 = w10 w09 w08 w07 w06 w05 w04 w03
       VMR4W             vr15, vr06, vr05                                                         // a2Offset2 a2Mdl a2Lft [  920] ....................;  // a1Offset2 = w12 w11 w10 w09 w08 w07 w06 w05
       VMR6W             vr16, vr06, vr05                                                         // a2Offset3 a2Mdl a2Lft [  921] ....................;  // a1Offset3 = w14 w13 w12 w11 w10 w09 w08 w07

    // arrange inp: row-2/set-2
       VMR2W             vr17, vr07, vr06                                                         // a2Offset4 a2Rgt a2Mdl [  924] ....................; // a1Offset4 = w18 w17 w16 w15 w14 w13 w12 w11
       VMR4W             vr18, vr07, vr06                                                         // a2Offset5 a2Rgt a2Mdl [  925] ....................; // a1Offset5 = w20 w19 w18 w17 w16 w15 w14 w13                
       VMR6W             vr19, vr07, vr06                                                         // a2Offset6 a2Rgt a2Mdl [  926] ....................; // a1Offset6 = 000 w21 w20 w19 w18 w17 w16 w15

    // apply filter: row-2/set-1
       VD6TAPF           vr14, vr14, 0xC                                                            // a2Offset1 a2Offset1 [  929] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr14, vr05, 0x3                                                                // a2Offset1 a2Lft [  930] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr14, vr15, 0x30                                                           // a2Offset1 a2Offset2 [  931] ....................; // 0 0 x x 0 0 0 0 
       VD6TAPF           vr14, vr16, 0xC0                                                           // a2Offset1 a2Offset3 [  932] ....................; // x x 0 0 0 0 0 0 
    
    // apply filter: row-2/set-2
       VD6TAPF           vr17, vr17, 0xC                                                            // a2Offset4 a2Offset4 [  935] ....................; // 0 0 0 0 x x 0 0
       VD6TAPF           vr17, vr06, 0x3                                                                // a2Offset4 a2Mdl [  936] ....................; // 0 0 0 0 0 0 x x
       VD6TAPF           vr17, vr18, 0x30                                                           // a2Offset4 a2Offset5 [  937] ....................; // 0 0 x x 0 0 0 0
       VD6TAPF           vr17, vr19, 0xC0                                                           // a2Offset4 a2Offset6 [  938] ....................; // x x 0 0 0 0 0 0

    // pack and store the result
       VASRRPWB          vr08, vr08, 0                                                              // a1Offset1 a1Offset1 [  941] ....................;   // pack the registers: row-1/set-1
       VASRRPWB          vr11, vr11, 0                                                              // a1Offset4 a1Offset4 [  942] ....................;   // pack the registers: row-1/set-2
       VASRRPWB          vr14, vr14, 0                                                              // a2Offset1 a2Offset1 [  943] ....................;   // pack the registers: row-1/set-2
       VASRRPWB          vr17, vr17, 0                                                              // a2Offset4 a2Offset4 [  944] ....................;   // pack the registers: row-2/set-2

       VST64             vr08,[i2, 0]                                                                 // a1Offset1 out2Buf [  946] ....................;   // store the result: row-1/set-1
       VST64             vr11,[i2, 8]                                                                 // a1Offset4 out2Buf [  947] ....................;   // store the result: row-1/set-2
       VST64             vr14,[i2, ME_REFERENCE_ARRAY_STRIDE]                                         // a2Offset1 out2Buf [  948] ....................;   // store the result: row-2/set-2
       VST64             vr17,[i2, ME_REFERENCE_ARRAY_STRIDE+8]                                       // a2Offset4 out2Buf [  949] ....................;   // store the result: row-2/set-2

    // pack and store input
       VASRRPWB          vr02, vr02, 5                                                                      // a1Lft a1Lft [  952] ....................;   //pack input
       VASRRPWB          vr03, vr03, 5                                                                      // a1Mdl a1Mdl [  953] ....................;   //pack input
       VASRRPWB          vr04, vr04, 5                                                                      // a1Rgt a1Rgt [  954] ....................;   //pack input

       VST64             vr02,[i1, 0]                                                                     // a1Lft out1Buf [  956] ....................; //store input
       VST64             vr03,[i1, 8]                                                                     // a1Mdl out1Buf [  957] ....................; //store input
       VST64             vr04,[i1, 16]                                                                    // a1Rgt out1Buf [  958] ....................;    //store input

    // pack and store input
       VASRRPWB          vr05, vr05, 5                                                                      // a2Lft a2Lft [  961] ....................;   //pack input
       VASRRPWB          vr06, vr06, 5                                                                      // a2Mdl a2Mdl [  962] ....................;   //pack input
       VASRRPWB          vr07, vr07, 5                                                                      // a2Rgt a2Rgt [  963] ....................;   //pack input

       VST64             vr05,[i1, ME_REFERENCE_ARRAY_STRIDE]                                             // a2Lft out1Buf [  965] ....................;    //store input
       VST64             vr06,[i1, ME_REFERENCE_ARRAY_STRIDE+8]                                           // a2Mdl out1Buf [  966] ....................;    //store input
       VST64             vr07,[i1, ME_REFERENCE_ARRAY_STRIDE+16]                                          // a2Rgt out1Buf [  967] ....................;    //store input

       VJD.i3            i3, r3                                                                     // iters iters lpStart [  969] 
       VIM               i0, i0, 2*ME_WORKING_ARRAY_STRIDE  //DS______________________________________________ inBuf inBuf [  970] 
       VIM               i1, i1, 2*ME_REFERENCE_ARRAY_STRIDE  //DS________________________________________ out1Buf out1Buf [  971] 
       VIM               i2, i2, 2*ME_REFERENCE_ARRAY_STRIDE  //DS________________________________________ out2Buf out2Buf [  972] 

// End                                                                                                                  // [  974] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz CentrePelFilAndInpRounding_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CentrePelFilAndInpRounding                                                                                   // [  975] 



//------------------------------------------------------------------------------
// Name:          StoreFilteredLumaPred
// Purpose:       
//                
// Arguments:     src
//                dest
//                
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] StoreFilteredLumaPred                                                                                        // [  986] 
_Asm int Record_StoreFilteredLumaPred(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,32]
  vrec r1

// p16    i0  src                                                                                                       // [  987] 
// p16    i1  dst                                                                                                       // [  988] 
// Begin                                                                                                                // [  989] 
//vec16  vr02  v1                                                                                                       // [  990] 
//vec16  vr03  v2                                                                                                       // [  991] 
//vec16  vr04  v3                                                                                                       // [  992] 
//vec16  vr05  v4                                                                                                       // [  993] 
    
LINE 995".\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss"
       VLD128            vr02,[i0, 0*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [  995] 
       VLD128            vr03,[i0, 1*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [  996] 
       VLD128            vr04,[i0, 2*ME_REFERENCE_ARRAY_STRIDE]                                                  // v3 src [  997] 
       VLD128            vr05,[i0, 3*ME_REFERENCE_ARRAY_STRIDE]                                                  // v4 src [  998] 
       VST128            vr02,[i1, 0*PCB_REFERENCE_STRIDE]                                                       // v1 dst [  999] 
       VST128            vr03,[i1, 1*PCB_REFERENCE_STRIDE]                                                       // v2 dst [ 1000] 
       VST128            vr04,[i1, 2*PCB_REFERENCE_STRIDE]                                                       // v3 dst [ 1001] 
       VST128            vr05,[i1, 3*PCB_REFERENCE_STRIDE]                                                       // v4 dst [ 1002] 
    
       VLD128            vr02,[i0, 4*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [ 1004] 
       VLD128            vr03,[i0, 5*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [ 1005] 
       VLD128            vr04,[i0, 6*ME_REFERENCE_ARRAY_STRIDE]                                                  // v3 src [ 1006] 
       VLD128            vr05,[i0, 7*ME_REFERENCE_ARRAY_STRIDE]                                                  // v4 src [ 1007] 
       VST128            vr02,[i1, 4*PCB_REFERENCE_STRIDE]                                                       // v1 dst [ 1008] 
       VST128            vr03,[i1, 5*PCB_REFERENCE_STRIDE]                                                       // v2 dst [ 1009] 
       VST128            vr04,[i1, 6*PCB_REFERENCE_STRIDE]                                                       // v3 dst [ 1010] 
       VST128            vr05,[i1, 7*PCB_REFERENCE_STRIDE]                                                       // v4 dst [ 1011] 
    
       VLD128            vr02,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [ 1013] 
       VLD128            vr03,[i0, 9*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [ 1014] 
       VLD128            vr04,[i0, 10*ME_REFERENCE_ARRAY_STRIDE]                                                 // v3 src [ 1015] 
       VLD128            vr05,[i0, 11*ME_REFERENCE_ARRAY_STRIDE]                                                 // v4 src [ 1016] 
       VST128            vr02,[i1, 8*PCB_REFERENCE_STRIDE]                                                       // v1 dst [ 1017] 
       VST128            vr03,[i1, 9*PCB_REFERENCE_STRIDE]                                                       // v2 dst [ 1018] 
       VST128            vr04,[i1, 10*PCB_REFERENCE_STRIDE]                                                      // v3 dst [ 1019] 
       VST128            vr05,[i1, 11*PCB_REFERENCE_STRIDE]                                                      // v4 dst [ 1020] 
    
       VLD128            vr02,[i0, 12*ME_REFERENCE_ARRAY_STRIDE]                                                 // v1 src [ 1022] 
       VLD128            vr03,[i0, 13*ME_REFERENCE_ARRAY_STRIDE]                                                 // v2 src [ 1023] 
       VLD128            vr04,[i0, 14*ME_REFERENCE_ARRAY_STRIDE]                                                 // v3 src [ 1024] 
       VLD128            vr05,[i0, 15*ME_REFERENCE_ARRAY_STRIDE]                                                 // v4 src [ 1025] 
       VST128            vr02,[i1, 12*PCB_REFERENCE_STRIDE]                                                      // v1 dst [ 1026] 
       VST128            vr03,[i1, 13*PCB_REFERENCE_STRIDE]                                                      // v2 dst [ 1027] 
       VST128            vr04,[i1, 14*PCB_REFERENCE_STRIDE]                                                      // v3 dst [ 1028] 
       VST128            vr05,[i1, 15*PCB_REFERENCE_STRIDE]                                                      // v4 dst [ 1029] 
// End                                                                                                                  // [ 1030] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc StoreFilteredLumaPred                                                                                        // [ 1031] 



const int MACRO_IDX_CopyLumaPredToSDM = 0;
const int MACRO_IDX_CallBackDoLumaSubPelFilter = 1;
const int MACRO_IDX_PerformInpHorFiltering = 2;
const int MACRO_IDX_QPelFiltering = 3;
const int MACRO_IDX_VerticalFilteringAndRounding = 4;
const int MACRO_IDX_VerticalFilteringWORounding = 5;
const int MACRO_IDX_CentrePelFilterWOInpRounding = 6;
const int MACRO_IDX_CentrePelFilAndInpRounding = 7;
const int MACRO_IDX_StoreFilteredLumaPred = 8;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int LumaSubPelFilter_Record(int sa, int SIMDnum)
{
    int *map = LumaSubPelFilter;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_CopyLumaPredToSDM] = sa;
    sa = Record_CopyLumaPredToSDM(map);

    map[MACRO_IDX_CallBackDoLumaSubPelFilter] = sa;
    sa = Record_CallBackDoLumaSubPelFilter(map);

    map[MACRO_IDX_PerformInpHorFiltering] = sa;
    sa = Record_PerformInpHorFiltering(map);

    map[MACRO_IDX_QPelFiltering] = sa;
    sa = Record_QPelFiltering(map);

    map[MACRO_IDX_VerticalFilteringAndRounding] = sa;
    sa = Record_VerticalFilteringAndRounding(map);

    map[MACRO_IDX_VerticalFilteringWORounding] = sa;
    sa = Record_VerticalFilteringWORounding(map);

    map[MACRO_IDX_CentrePelFilterWOInpRounding] = sa;
    sa = Record_CentrePelFilterWOInpRounding(map);

    map[MACRO_IDX_CentrePelFilAndInpRounding] = sa;
    sa = Record_CentrePelFilAndInpRounding(map);

    map[MACRO_IDX_StoreFilteredLumaPred] = sa;
    sa = Record_StoreFilteredLumaPred(map);

    return sa;
}
