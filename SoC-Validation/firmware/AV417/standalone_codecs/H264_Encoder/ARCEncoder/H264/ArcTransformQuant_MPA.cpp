//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcTransformQuant_MPA.sss'

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

//showstalls
// MetaWare ouput ON                                                                                                    // [   19] 
#define LINE #line                                                                                                      // [   19] 
#include ".\ARCEncoder\H264\ArcTransformQuant_MPA.h"


// Macro table
    int Transform_Buffers[20];
    int Transform_Buffers_Record(int sa, int SIMDnum);

// ScanMP: Including file .\ARCEncoder\H264\..\ARC\SIMD_ABI.ii.
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


//----------------------------------------------------------------------------------------------------
// Is Macro
// Sets the quantization values for each macroblock
// Takes sliceType, chromaQpIndexOffset as the parameters

// [Macro] SetupQuantValues                                                                                             // [   35] 
_Asm int Record_SetupQuantValues(int *mac_tab)
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

    // Params -----------
// s16    vr02.8  SliceType                                                                                             // [   37] 
// s16    vr02.1  ChromaQpIndexOffset                                                                                   // [   38] 

    // End params -------

// p16    i8  SDMAddr                                                                                                   // [   42] 
// s16    vr03.1  LumaDiv                                                                                               // [   43] 
    // Params -----------
// s16    vr04.1  QuantVal                                                                                              // [   45] 
    // End params -------
// s16    vr05.1  LumaMod                                                                                               // [   47] 
// s16    vr06.1  LumaShiftNormal                                                                                       // [   48] 
// s16    vr07.1  ChromaQPIndex                                                                                         // [   49] 
// s16    vr08.1  ChromaQp                                                                                              // [   50] 
// s16    vr09.1  ChromaDiv                                                                                             // [   51] 
// s16    vr10.1  ChromaMod                                                                                             // [   52] 
// s16    vr11.1  ChromaShift                                                                                           // [   53] 

// s16    vr03.8  MBCodingType                                                                                          // [   55] 
// s16    vr04.8  H264ISlice                                                                                            // [   56] 
// s16    vr05.8  H264Intra16x16                                                                                        // [   57] 

// ****** Informational : Register space for symbol "ForceAddr" already allocated.                                      // [   59] 
// p32    k8  ForceAddr                                                                                                 // [   59] 
// s32    vr12.3  TempAddr                                                                                              // [   60] 
// s32    vr13.3  WriteAddr                                                                                             // [   61] 
// s16    vr02.4  ChromaF                                                                                               // [   62] 
// s16    vr03.4  LumaF                                                                                                 // [   63] 
// s32    vr14.3  Lumamod32                                                                                             // [   64] 
// s32    vr15.3  ChromaMod32                                                                                           // [   65] 
// s32    vr02.48  Table                                                                                                // [   66] 

// s32    vr16.3  WriteAddr1                                                                                            // [   68] 

// ****** Informational : Register space for symbol "SDMAddrT" already allocated.                                       // [   70] 
// p16    i6  SDMAddrT                                                                                                  // [   70] 

// p16    i0  SDMAddr1                                                                                                  // [   72] 


    // Calculate LumaF and Lumashift

    // Store Quant value
    // The following instructions can be avoided if table look up method is used
    // LumaDiv calculation
LINE 81".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMULW.1           vr03, vr04, 43                                                                // LumaDiv QuantVal [   81] 
     // Store Quant value
       VST8              vr04,[i4, MPO_QuantValue]                                                      // QuantVal CirBuf [   83] 
       VMOVW             vr00, LUMA_SHIFT_NORMAL, 1                                                            // SDMAddr1 [   84] 

       VASRW.1           vr03, vr03, 8                                                                  // LumaDiv LumaDiv [   86] 
       VMOVW             vr01, LUMA_DIV, 1                                                                      // SDMAddr [   87] 

    // LumaMod Calculation
       VMULW.1           vr05, vr03, 6                                                                  // LumaMod LumaDiv [   90] 
    
     // Store to WorkAreaSDM->LumaDiv
       VST16             vr03,[i8, 0]                                                                   // LumaDiv SDMAddr [   93] 
    
       VMULW.1           vr03, vr03, 2                                                                  // LumaDiv LumaDiv [   95] 
       VMOVW             vr01, LUMA_CHROMA_F, 1                                                                 // SDMAddr [   96] 
       VADDW.1           vr01, vr01, vr03                                                       // SDMAddr SDMAddr LumaDiv [   97] 
       VLD16_2           vr03,[i8, 0]                                                                     // LumaF SDMAddr [   98] 
    
       VMOVW             vr01, LUMA_F, 1                                                                        // SDMAddr [  100] 
       VADDW.1           vr07, vr04, vr02                                    // ChromaQPIndex QuantVal ChromaQpIndexOffset [  101] 
       VSUBW.1           vr05, vr04, vr05                                                      // LumaMod QuantVal LumaMod [  102] 
       VST16_2           vr03,[i8, 0]                                                                     // LumaF SDMAddr [  103] 
    
       VMAXW.1           vr07, vr07, 0                                                      // ChromaQPIndex ChromaQPIndex [  105] 
	// Needed as now it is stored as 16 bit array
       VMULW.1           vr05, vr05, 32                                                                 // LumaMod LumaMod [  107] 




    // Chroma calculations
    // CLIP if less than 0 to 0
    



    // Clip if more than 50 to 50
       VMINW.1           vr07, vr07, 50                                                     // ChromaQPIndex ChromaQPIndex [  119] 
	 
       VMOVW             vr01, SDMOF_h264_quantiser_chroma_mapping_table, 1                                     // SDMAddr [  121] 

       VADDW.1           vr01, vr01, vr07                                                 // SDMAddr SDMAddr ChromaQPIndex [  123] 
	
       VLD8W             vr08,[i8, 0]                                                                  // ChromaQp SDMAddr [  125] 
	
       VMOVW             vr01, CHROMA_DIV, 1                                                                    // SDMAddr [  127] 
	
       VMOVW             vr12, SDMOF_h264_frame_scan_inverse_quantiser_table, 3                                // TempAddr [  129] 

    // ChromaDiv calculation
       VMULW.1           vr09, vr08, 43                                                              // ChromaDiv ChromaQp [  132] 
       VADDW.3           vr13, vr12, vr05                                                    // WriteAddr TempAddr LumaMod [  133] 
       VMOVW             vr00, LUMA_INVERSE_QUANT_TABLE, 64                                                    // SDMAddrT [  134] 
       VASRW.1           vr09, vr09, 8                                                              // ChromaDiv ChromaDiv [  135] 
 

    // ChromaMod Calculation
       VMULW.1           vr10, vr09, 6                                                              // ChromaMod ChromaDiv [  139] 

	 // Store to WorkAreaSDM->ChromaDiv
       VST16             vr09,[i8, 0]                                                                 // ChromaDiv SDMAddr [  142] 
    
       VMULW.1           vr09, vr09, 2                                                              // ChromaDiv ChromaDiv [  144] 
       VMOVW             vr01, LUMA_CHROMA_F, 1                                                                 // SDMAddr [  145] 
       VST16             vr13,[i6, 0]                                                                // WriteAddr SDMAddrT [  146] 
       VADDW.1           vr01, vr01, vr09                                                     // SDMAddr SDMAddr ChromaDiv [  147] 
       VLD16_2           vr02,[i8, 0]                                                                   // ChromaF SDMAddr [  148] 
    
       VMOVW             vr01, CHROMA_F, 1                                                                      // SDMAddr [  150] 
       VSUBW.1           vr10, vr08, vr10                                                  // ChromaMod ChromaQp ChromaMod [  151] 
       VMULW.1           vr10, vr10, 32                                                             // ChromaMod ChromaMod [  152] 
       VST16_2           vr02,[i8, 0]                                                                   // ChromaF SDMAddr [  153] 
    
       VMOVW             vr00, CHROMA_INVERSE_QUANT_TABLE, 64                                                  // SDMAddrT [  155] 

       VADDW.3           vr13, vr12, vr10                                                  // WriteAddr TempAddr ChromaMod [  157] 
       VMOVW             vr12, SDMOF_h264_frame_scan_quantiser_table, 3                                        // TempAddr [  158] 
	
       VADDW.3           vr16, vr12, vr05                                                   // WriteAddr1 TempAddr LumaMod [  160] 
       VST16             vr13,[i6, 0]                                                                // WriteAddr SDMAddrT [  161] 
       VMOVW             vr00, LUMA_QUANT_TABLE, 64                                                            // SDMAddrT [  162] 


       VMOVW             vr01, CHROMA_QUANT_TABLE, 1                                                            // SDMAddr [  165] 

       VADDW.3           vr13, vr12, vr10                                                  // WriteAddr TempAddr ChromaMod [  167] 
       VST16             vr16,[i6, 0]                                                               // WriteAddr1 SDMAddrT [  168] 
       VST16             vr13,[i8, 0]                                                                 // WriteAddr SDMAddr [  169] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetupQuantValues                                                                                             // [  171] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Does the forward hadamard transform for luma DC
// Sends channel reply

// [Macro] TransformQuantiseLumaDC                                                                                      // [  179] 
_Asm int Record_TransformQuantiseLumaDC(int *mac_tab)
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

    // End params -------

// p16    i0  LumaShiftNormal                                                                                           // [  184] 
// p16    i8  Address                                                                                                   // [  185] 
// p16    i1  QuantVal                                                                                                  // [  186] 
// p16    i2  LumaF                                                                                                     // [  187] 
// p16    i3  BuffStart                                                                                                 // [  188] // Start of Luma DC coefficients
//vec32  vr06  LumaQuant                                                                                                // [  189] 
//vec32  vr07  LumaDiv                                                                                                  // [  190] 
// p16    i7  AddressTemp                                                                                               // [  191] 


    // Load LUMA SHIFT
LINE 195".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr01, LUMA_DIV, 1                                                                      // Address [  195] 
       VIM               i3, i4, MPO_Y_DC_CoeffStore0                                                  // BuffStart CirBuf [  196] 
       VLD16             vr07,[i8, 0]                                                                   // LumaDiv Address [  197] 

    // Load LUMA rounding factor
       VMOVW             vr01, LUMA_F, 1                                                                        // Address [  200] 
       VMOVW             vr00, LUMA_QUANT_TABLE, 128                                                        // AddressTemp [  201] 
    
       VLD16_2           vr00,[i8, 0]                                                                     // LumaF Address [  203] 
       VADDW.1           vr00, vr07, 1                                                          // LumaShiftNormal LumaDiv [  204] 
    

    // Load LUMA QUANT factor
       VLD16_7           vr00,[i7, 0]                                                           // AddressTemp AddressTemp [  208] 
 


// Begin                                                                                                                // [  212] 
//vec16  vr02  LumaDCLane0                                                                                              // [  213] 
//vec16  vr03  LumaDCLane1                                                                                              // [  214] 
//vec16  vr04  LumaDCLane2                                                                                              // [  215] 
//vec16  vr05  LumaDCLane3                                                                                              // [  216] 

    // load input data
LINE 219".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VLD64             vr02,[i3, 0]                                                             // LumaDCLane0 BuffStart [  219] 
       VLD64             vr04,[i3, 16]                                                            // LumaDCLane2 BuffStart [  220] 
       VLD64             vr03,[i3, 8]                                                             // LumaDCLane1 BuffStart [  221] 
       VLD64             vr05,[i3, 24]                                                            // LumaDCLane3 BuffStart [  222] 
       VLD16_1           vr00,[i7, 0]                                                              // QuantVal AddressTemp [  223] 
    

    // Froward Transform
    // apply verical transform
    // stage 0
       VADDSUW           vr02, vr04                                                             // LumaDCLane0 LumaDCLane2 [  229] 
       VADDSUW           vr03, vr05                                                             // LumaDCLane1 LumaDCLane3 [  230] 
       VMIVW.255         vr06, i1                                                                    // LumaQuant QuantVal [  231] 

    //stage 1
       VADDSUW           vr02, vr03                                                             // LumaDCLane0 LumaDCLane1 [  234] 
       VADDSUW           vr04, vr05                                                             // LumaDCLane2 LumaDCLane3 [  235] 

    // Right shift each the vertical transform
       VASRW             vr02, vr02, 1                                                          // LumaDCLane0 LumaDCLane0 [  238] 
       VASRW             vr03, vr03, 1                                                          // LumaDCLane1 LumaDCLane1 [  239] 
       VASRW             vr04, vr04, 1                                                          // LumaDCLane2 LumaDCLane2 [  240] 
       VASRW             vr05, vr05, 1                                                          // LumaDCLane3 LumaDCLane3 [  241] 
// End                                                                                                                  // [  242] 

// Begin                                                                                                                // [  244] 
    // Rename the lanes as after the vaddsuw operation
    // LumaDCLanes move to different vector registers then the originally loaded.
//vec16  vr02  LumaDCLane0                                                                                              // [  247] 
//vec16  vr04  LumaDCLane1                                                                                              // [  248] 
//vec16  vr05  LumaDCLane2                                                                                              // [  249] 
//vec16  vr03  LumaDCLane3                                                                                              // [  250] 

    // transpose matrix
LINE 253".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr05                                                             // LumaDCLane0 LumaDCLane2 [  253] 
       VEXCH2            vr04, vr03                                                             // LumaDCLane1 LumaDCLane3 [  254] 
       VEXCH1            vr02, vr04                                                             // LumaDCLane0 LumaDCLane1 [  255] 
       VEXCH1            vr05, vr03                                                             // LumaDCLane2 LumaDCLane3 [  256] 

    // apply horizontal transform
    // stage 0
       VADDSUW           vr02, vr05                                                             // LumaDCLane0 LumaDCLane2 [  260] 
       VADDSUW           vr04, vr03                                                             // LumaDCLane1 LumaDCLane3 [  261] 

    //stage 1
       VADDSUW           vr02, vr04                                                             // LumaDCLane0 LumaDCLane1 [  264] 
       VADDSUW           vr05, vr03                                                             // LumaDCLane2 LumaDCLane3 [  265] 
// End                                                                                                                  // [  266] 

// Begin                                                                                                                // [  268] 
    // Rename the lanes as after the vaddsuw operation
//vec16  vr02  LumaDCLane0                                                                                              // [  270] 
//vec16  vr05  LumaDCLane1                                                                                              // [  271] 
//vec16  vr03  LumaDCLane2                                                                                              // [  272] 
//vec16  vr04  LumaDCLane3                                                                                              // [  273] 

//vec16  vr08  Lane0Sign                                                                                                // [  275] 
//vec16  vr09  Lane1Sign                                                                                                // [  275] 
//vec16  vr10  Lane2Sign                                                                                                // [  275] 
//vec16  vr11  Lane3Sign                                                                                                // [  275] 
    // vec32                Lane0DC32,Lane1DC32,Lane2DC32,Lane3DC32

LINE 278".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr03                                                             // LumaDCLane0 LumaDCLane2 [  278] 
       VEXCH2            vr05, vr04                                                             // LumaDCLane1 LumaDCLane3 [  279] 
       VEXCH1            vr02, vr05                                                             // LumaDCLane0 LumaDCLane1 [  280] 
       VEXCH1            vr03, vr04                                                             // LumaDCLane2 LumaDCLane3 [  281] 


    // Extract the sign
       VSIGNW            vr08, vr02                                                               // Lane0Sign LumaDCLane0 [  285] 
       VSIGNW            vr09, vr05                                                               // Lane1Sign LumaDCLane1 [  286] 
       VSIGNW            vr10, vr03                                                               // Lane2Sign LumaDCLane2 [  287] 
       VSIGNW            vr11, vr04                                                               // Lane3Sign LumaDCLane3 [  288] 

    // Extrac the absolute value
       VABSW             vr02, vr02                                                             // LumaDCLane0 LumaDCLane0 [  291] 
       VABSW             vr05, vr05                                                             // LumaDCLane1 LumaDCLane1 [  292] 
       VABSW             vr03, vr03                                                             // LumaDCLane2 LumaDCLane2 [  293] 
       VABSW             vr04, vr04                                                             // LumaDCLane3 LumaDCLane3 [  294] 

    // Pack from 16 bits to 32 bits


       VMULFW            vr02, vr02, vr06                                             // LumaDCLane0 LumaDCLane0 LumaQuant [  299] 
       VMULFW            vr05, vr05, vr06                                             // LumaDCLane1 LumaDCLane1 LumaQuant [  300] 
       VMULFW            vr03, vr03, vr06                                             // LumaDCLane2 LumaDCLane2 LumaQuant [  301] 
       VMULFW            vr04, vr04, vr06                                             // LumaDCLane3 LumaDCLane3 LumaQuant [  302] 

       VADDW             vr02, vr02, i2                                                   // LumaDCLane0 LumaDCLane0 LumaF [  304] 
       VADDW             vr05, vr05, i2                                                   // LumaDCLane1 LumaDCLane1 LumaF [  305] 
       VADDW             vr03, vr03, i2                                                   // LumaDCLane2 LumaDCLane2 LumaF [  306] 
       VADDW             vr04, vr04, i2                                                   // LumaDCLane3 LumaDCLane3 LumaF [  307] 

    // Pack back to 16 bits

       VASRW             vr02, vr02, i0                                         // LumaDCLane0 LumaDCLane0 LumaShiftNormal [  311] 
       VASRW             vr05, vr05, i0                                         // LumaDCLane1 LumaDCLane1 LumaShiftNormal [  312] 
       VASRW             vr03, vr03, i0                                         // LumaDCLane2 LumaDCLane2 LumaShiftNormal [  313] 
       VASRW             vr04, vr04, i0                                         // LumaDCLane3 LumaDCLane3 LumaShiftNormal [  314] 

    // Restore sign
       VMULW             vr02, vr02, vr08                                             // LumaDCLane0 LumaDCLane0 Lane0Sign [  317] 
       VMULW             vr05, vr05, vr09                                             // LumaDCLane1 LumaDCLane1 Lane1Sign [  318] 
       VMULW             vr03, vr03, vr10                                             // LumaDCLane2 LumaDCLane2 Lane2Sign [  319] 
       VMULW             vr04, vr04, vr11                                             // LumaDCLane3 LumaDCLane3 Lane3Sign [  320] 

    // Store back the result
       VST64             vr02,[i3, 0]                                                             // LumaDCLane0 BuffStart [  323] //vr04
       VST64             vr05,[i3, 8]                                                             // LumaDCLane1 BuffStart [  324] //vr05
       VST64             vr03,[i3, 16]                                                            // LumaDCLane2 BuffStart [  325] //vr06
       VST64             vr04,[i3, 24]                                                            // LumaDCLane3 BuffStart [  326] 
// End                                                                                                                  // [  327] 


  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc TransformQuantiseLumaDC                                                                                      // [  330] 




//==================================================================================================

// Global Parameters for Chroma DC
// Begin                                                                                                                // [  336] 
// p16    i0  BuffStart                                                                                                 // [  337] // Start of Chroma DC coefficients
// p16    i1  ChromaShift                                                                                               // [  338] 
// p16    i2  Address                                                                                                   // [  339] 
// p16    i3  QuantVal                                                                                                  // [  340] 
// p16    i7  ChromaF                                                                                                   // [  341] 
//vec32  vr02  ChromaQuant                                                                                              // [  342] // = vr04
// p16    i8  ConstVal                                                                                                  // [  343] // = vr05
//vec16  vr03  ChromaDCLane0                                                                                            // [  344] 
//vec16  vr04  ChromaDCLane1                                                                                            // [  345] 
//vec16  vr05  ChromaDCLane2                                                                                            // [  346] 
//vec16  vr06  ChromaDCLane3                                                                                            // [  347] 
//vec16  vr07  LinkAddress                                                                                              // [  348] 
// Subroutine, which will be called by TransformQuantiseChromaDC function
// twice for U and V
// BuffStart will contain U and V
// [Subroutine] TransformQuantiseUVDC                                                                                   // [  352] 
_Asm int Record_TransformQuantiseUVDC(int *mac_tab)
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

    // Params -----------

    // End params -------


    // Transform
//vec16  vr08  Lane0Sign                                                                                                // [  359] 
//vec16  vr09  Lane1Sign                                                                                                // [  359] 
//vec16  vr10  Lane2Sign                                                                                                // [  359] 
//vec16  vr11  Lane3Sign                                                                                                // [  359] 

    // Froward Transform
    // stage 0
LINE 363".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VADDSUW           vr03, vr04                                                         // ChromaDCLane0 ChromaDCLane1 [  363] 
 
    // ChromaDCLane0 = (d0 + d2),(d1+d3)
    // ChromaDCLane1 = (d0 - d2) ,(d1-d3)

       VEXCH1            vr03, vr04                                                         // ChromaDCLane0 ChromaDCLane1 [  368] 
   
    
    
    // ChromaDCLane0 = (d0 + d2),(d0 - d2)
    // ChromaDCLane1 = (d1+d3) ,(d1-d3)
    // stage 1
       VADDSUW           vr03, vr04                                                         // ChromaDCLane0 ChromaDCLane1 [  375] 
   
    
    
    // ChromaDCLane0 = ((d0 + d2)+ (d1+d3)),((d0 - d2) + (d1-d3))
    // ChromaDCLane1 = ((d0 + d2) - (d1+d3)) ,((d0 - d2) - (d1-d3))

       VEXCH1            vr03, vr04                                                         // ChromaDCLane0 ChromaDCLane1 [  382] 
   
    
    
    // ChromaDCLane0 = ((d0 + d2)+ (d1+d3)),((d0 + d2) - (d1+d3))
    // ChromaDCLane1 = ((d0 - d2) + (d1-d3)) ,((d0 - d2) - (d1-d3))

    // **********************************************************
    // U quantization

    // Extract the sign
       VSIGNW            vr08, vr03                                                             // Lane0Sign ChromaDCLane0 [  393] 
       VSIGNW            vr09, vr04                                                             // Lane1Sign ChromaDCLane1 [  394] 
   
    // Extrac the absolute value
       VABSW             vr03, vr03                                                         // ChromaDCLane0 ChromaDCLane0 [  397] 
       VABSW             vr04, vr04                                                         // ChromaDCLane1 ChromaDCLane1 [  398] 
   
    // Multiply by the MF factor
       VMULFW            vr03, vr03, vr02                                       // ChromaDCLane0 ChromaDCLane0 ChromaQuant [  401] 
       VMULFW            vr04, vr04, vr02                                       // ChromaDCLane1 ChromaDCLane1 ChromaQuant [  402] 
   

    // ADD the rounding factor
       VADDW             vr03, vr03, i7                                             // ChromaDCLane0 ChromaDCLane0 ChromaF [  406] 
       VADDW             vr04, vr04, i7                                             // ChromaDCLane1 ChromaDCLane1 ChromaF [  407] 
    

    // Pack back to 16 bits and right shift
       VASRW             vr03, vr03, i1                                         // ChromaDCLane0 ChromaDCLane0 ChromaShift [  411] 
       VASRW             vr04, vr04, i1                                         // ChromaDCLane1 ChromaDCLane1 ChromaShift [  412] 
    

    // Restore sign
       VMULW             vr03, vr03, vr08                                         // ChromaDCLane0 ChromaDCLane0 Lane0Sign [  416] 
       VMULW             vr04, vr04, vr09                                         // ChromaDCLane1 ChromaDCLane1 Lane1Sign [  417] 
   

    // **************************************************************

    // Set the U non zero coefficeint flag in the circular buffer
    // These can be 4 16 bit instructions, if needed
    // currently it is 2 32 bit instructions
       VSNEW.15          vr03, vr62                                                                 // ChromaDCLane0 VZERO [  425] 
       VST32             vr03,[i0, 0]                                                           // ChromaDCLane0 BuffStart [  426] 
       VSNEW.15.U        vr04, vr62                                                                 // ChromaDCLane1 VZERO [  427] 
       VST32             vr04,[i0, 4]                                                           // ChromaDCLane1 BuffStart [  428] 
       VTANY.15.S        i8, 1                                                                                 // ConstVal [  429] 
   
    

       VJB               vr07, 0                                                                            // LinkAddress [  433] 
    // Delay slot instructions
       VST8              vr01,[i4, MPO_UV_DC_NonZeroCoeff]  //DS__________________________________________ ConstVal CirBuf [  435] 
       VST32_2           vr03,[i0, 8]  //DS_______________________________________________________ ChromaDCLane0 BuffStart [  436] 
       VST32_2           vr04,[i0, 12]  //DS______________________________________________________ ChromaDCLane1 BuffStart [  437] 

    
   

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc TransformQuantiseUVDC                                                                                        // [  442] 



//==================================================================================================

//----------------------------------------------------------------------------------------------------
// Is Macro
// Does the forward hadamard transform for chroma DC

// [Macro] TransformQuantiseChromaDC                                                                                    // [  450] 
_Asm int Record_TransformQuantiseChromaDC(int *mac_tab)
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

    // Params -----------

    // End params -------
// p16    i9  AdressTemp                                                                                                // [  454] 
    // Load and calculate CHROMA SHIFT
LINE 456".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr00, CHROMA_DIV, 4                                                                    // Address [  456] 
  
       VLD16_1           vr00,[i2, 0]                                                               // ChromaShift Address [  458] 

    // Load and calculate CHROMA rounding factor
       VMOVW             vr00, CHROMA_QUANT_TABLE, 4                                                            // Address [  461] 
       VMOVW             vr01, CHROMA_F, 2                                                                   // AdressTemp [  462] 
       VLD16_7           vr00,[i9, 0]                                                                // ChromaF AdressTemp [  463] 


    // Load CHROMA QUANT factor
       VLD16_1           vr01,[i2, 0]                                                                // AdressTemp Address [  467] 
    
       VIM               i0, i4, MPO_U_DC_CoeffStore                                                   // BuffStart CirBuf [  469] 
       VADDW.2           vr00, vr00, 1                                                          // ChromaShift ChromaShift [  470] 
       VLD32             vr03,[i0, 0]                                                           // ChromaDCLane0 BuffStart [  471] 
       VLD16_3           vr00,[i9, 0]                                                               // QuantVal AdressTemp [  472] 
       VLD32             vr04,[i0, 4]                                                           // ChromaDCLane1 BuffStart [  473] 
   
    
    

    // U V transform
  ld r1,[r17,8];                                                                      // LinkAddress TransformQuantiseUVDC [  479] 
       VJL               vr07, r1                                                                                       // [  479] 
       VLD32_2           vr03,[i0, 8]  //DS_______________________________________________________ ChromaDCLane0 BuffStart [  480] 
       VMIVW.255         vr02, i3  //DS______________________________________________________________ ChromaQuant QuantVal [  481] 
       VLD32_2           vr04,[i0, 12]  //DS______________________________________________________ ChromaDCLane1 BuffStart [  482] 



  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc TransformQuantiseChromaDC                                                                                    // [  486] 



// End                                                                                                                  // [  488] // Global Parameter scope ends here


//----------------------------------------------------------------------------------------------------
// Is Macro
// Does the forward 4x4 transform
// Sends channel reply
// Two adjacent 4x4 blocks will be transforned and quantised at the same time.
// Each call to this subroutine will transform/quantise two 4x4 blocks
// Begin                                                                                                                // [  497] 
    
//vec16  vr02  CoefficientsLane0                                                                                        // [  499] 
//vec16  vr03  CoefficientsLane1                                                                                        // [  500] 
//vec16  vr04  CoefficientsLane2                                                                                        // [  501] 
//vec16  vr05  CoefficientsLane3                                                                                        // [  502] 
//vec16  vr06  ShiftedCoeffs1                                                                                           // [  503] 
//vec16  vr07  ShiftedCoeffs2                                                                                           // [  504] 
//vec16  vr08  TargetAddress                                                                                            // [  505] 
// p16    i8  MBCodingType                                                                                              // [  506] 
// p16    i1  AllCoeffs0                                                                                                // [  507] 
// p16    i9  AllCoeffs1                                                                                                // [  508] 
    //p16                 Address
// p16    i0  DcPtr                                                                                                     // [  510] 
// p16    i2  ShiftNormal                                                                                               // [  511] 
// p16    i3  QuantVal                                                                                                  // [  512] 
// p16    i7  RndingF                                                                                                   // [  513] 
// p16    i10  BuffStart                                                                                                // [  514] // Start of coefficients This will be set by the calling
                                      // function
//vec32  vr09  QuantLane0                                                                                               // [  516] 
//vec32  vr10  QuantLane1                                                                                               // [  517] 
//vec32  vr11  QuantLane2                                                                                               // [  518] 
//vec32  vr12  QuantLane3                                                                                               // [  519] 
//vec16  vr13  Lane0Sign                                                                                                // [  520] 
//vec16  vr14  Lane1Sign                                                                                                // [  520] 
//vec16  vr15  Lane2Sign                                                                                                // [  520] 
//vec16  vr16  Lane3Sign                                                                                                // [  520] 
//vec16  vr17  Lane0Lower32                                                                                             // [  521] 
//vec16  vr18  Lane0Upper32                                                                                             // [  521] 
//vec16  vr19  Lane1Lower32                                                                                             // [  521] 
//vec16  vr20  Lane1Upper32                                                                                             // [  521] 
//vec16  vr21  Lane2Lower32                                                                                             // [  521] 
//vec16  vr22  Lane2Upper32                                                                                             // [  521] 
//vec16  vr23  Lane3Lower32                                                                                             // [  521] 
//vec16  vr24  Lane3Upper32                                                                                             // [  521] 
//vec16  vr25  TotalCoeffs                                                                                              // [  522] 
//vec16  vr26  CoeffSum                                                                                                 // [  523] 
// p16    i11  CoeffMask                                                                                                // [  524] 
// s16    vr27.2  NonZeroCoeff                                                                                          // [  525] 
//vec16  vr00  SmallCoeffLimit                                                                                          // [  526] 
// p16    i12  AllZero                                                                                                  // [  527] 
// s16    vr27.1  Reserved                                                                                              // [  528] // Used to shift NonZeroCoeff
//vec16  vr28  SumOfBlocks                                                                                              // [  529] 
// s16    vr27.4  TotalSum                                                                                              // [  530] 
//vec16  vr29  Offset                                                                                                   // [  531] 
 
    
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|                                             SmallCoeffLimit                                                           |
//|VR 1|              |              |              |       AllZero|     CoeffMask|     BuffStart|    AllCoeffs1|  MBCodingType|
//|VR 2|                                           CoefficientsLane0                                                           |
//|VR 3|                                           CoefficientsLane1                                                           |
//|VR 4|                                           CoefficientsLane2                                                           |
//|VR 5|                                           CoefficientsLane3                                                           |
//|VR 6|                                              ShiftedCoeffs1                                                           |
//|VR 7|                                              ShiftedCoeffs2                                                           |
//|VR 8|                                               TargetAddress                                                           |
//|VR 9|                                                  QuantLane0                                                           |
//|VR10|                                                  QuantLane1                                                           |
//|VR11|                                                  QuantLane2                                                           |
//|VR12|                                                  QuantLane3                                                           |
//|VR13|                                                   Lane0Sign                                                           |
//|VR14|                                                   Lane1Sign                                                           |
//|VR15|                                                   Lane2Sign                                                           |
//|VR16|                                                   Lane3Sign                                                           |
//|VR17|                                                Lane0Lower32                                                           |
//|VR18|                                                Lane0Upper32                                                           |
//|VR19|                                                Lane1Lower32                                                           |
//|VR20|                                                Lane1Upper32                                                           |
//|VR21|                                                Lane2Lower32                                                           |
//|VR22|                                                Lane2Upper32                                                           |
//|VR23|                                                Lane3Lower32                                                           |
//|VR24|                                                Lane3Upper32                                                           |
//|VR25|                                                 TotalCoeffs                                                           |
//|VR26|                                                    CoeffSum                                                           |
//|VR27|              |              |              |              |              |      TotalSum|  NonZeroCoeff|      Reserved|
//|VR28|                                                 SumOfBlocks                                                           |
//|VR29|                                                      Offset                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


// [Subroutine] Forward4x4TransformQuantise                                                                             // [  536] 
_Asm int Record_Forward4x4TransformQuantise(int *mac_tab)
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

    // Params -----------

    // End params -------
LINE 540".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VSUBW.4           vr01, vr01, vr29                                                    // BuffStart BuffStart Offset [  540] 
       VMULW.2           vr27, vr27, 4                                                        // NonZeroCoeff NonZeroCoeff [  541] 
    
    // load input data
       VLD128            vr03,[i10, 32]                                                     // CoefficientsLane1 BuffStart [  544] 
       VLD128            vr04,[i10, 64]                                                     // CoefficientsLane2 BuffStart [  545] 
       VLD128            vr02,[i10, 0]                                                      // CoefficientsLane0 BuffStart [  546] 
       VLD128            vr05,[i10, 96]                                                     // CoefficientsLane3 BuffStart [  547] 


    // Froward Transform
    // apply verical transform
    // stage 0
       VADDSUW           vr03, vr04                                                 // CoefficientsLane1 CoefficientsLane2 [  553] 
       VADDSUW           vr02, vr05                                                 // CoefficientsLane0 CoefficientsLane3 [  554] 

    //stage 1
       VMULW             vr06, vr05, 2                                                 // ShiftedCoeffs1 CoefficientsLane3 [  557] 
       VMULW             vr07, vr04, 2                                                 // ShiftedCoeffs2 CoefficientsLane2 [  558] 
       VADDSUW           vr02, vr03                                                 // CoefficientsLane0 CoefficientsLane1 [  559] 
       VADDSUW           vr06, vr04                                                    // ShiftedCoeffs1 CoefficientsLane2 [  560] 
       VADDSUW           vr05, vr07                                                    // CoefficientsLane3 ShiftedCoeffs2 [  561] 


// Begin                                                                                                                // [  564] 
    // Rename the lanes as after the vaddsuw operation
    // CoefficientsLane move to different vector registers than the the originally loaded.
//vec16  vr02  CoefficientsLane0                                                                                        // [  567] 
//vec16  vr06  CoefficientsLane1                                                                                        // [  568] 
//vec16  vr03  CoefficientsLane2                                                                                        // [  569] 
//vec16  vr07  CoefficientsLane3                                                                                        // [  570] 
//vec16  vr04  ShiftedCoeffs1                                                                                           // [  571] 
//vec16  vr05  ShiftedCoeffs2                                                                                           // [  572] 

    // transpose matrix
LINE 575".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr03                                                 // CoefficientsLane0 CoefficientsLane2 [  575] 
       VEXCH2            vr06, vr07                                                 // CoefficientsLane1 CoefficientsLane3 [  576] 
       VEXCH1            vr02, vr06                                                 // CoefficientsLane0 CoefficientsLane1 [  577] 
       VEXCH1            vr03, vr07                                                 // CoefficientsLane2 CoefficientsLane3 [  578] 

    // apply horizontal transform
    // stage 0
       VADDSUW           vr02, vr07                                                 // CoefficientsLane0 CoefficientsLane3 [  582] 
       VADDSUW           vr06, vr03                                                 // CoefficientsLane1 CoefficientsLane2 [  583] 
    
       VMULW             vr04, vr07, 2                                                 // ShiftedCoeffs1 CoefficientsLane3 [  585] 
       VMULW             vr05, vr03, 2                                                 // ShiftedCoeffs2 CoefficientsLane2 [  586] 
    
    //stage 1
       VADDSUW           vr02, vr06                                                 // CoefficientsLane0 CoefficientsLane1 [  589] 
       VADDSUW           vr04, vr03                                                    // ShiftedCoeffs1 CoefficientsLane2 [  590] 
       VADDSUW           vr07, vr05                                                    // CoefficientsLane3 ShiftedCoeffs2 [  591] 
// End                                                                                                                  // [  592] 

// Begin                                                                                                                // [  594] 
    // Rename the lanes as after the vaddsuw operation
//vec16  vr02  CoefficientsLane0                                                                                        // [  596] 
//vec16  vr04  CoefficientsLane1                                                                                        // [  597] 
//vec16  vr06  CoefficientsLane2                                                                                        // [  598] 
//vec16  vr05  CoefficientsLane3                                                                                        // [  599] 

LINE 601".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr06                                                 // CoefficientsLane0 CoefficientsLane2 [  601] 
       VEXCH2            vr04, vr05                                                 // CoefficientsLane1 CoefficientsLane3 [  602] 
       VEXCH1            vr02, vr04                                                 // CoefficientsLane0 CoefficientsLane1 [  603] 
       VEXCH1            vr06, vr05                                                 // CoefficientsLane2 CoefficientsLane3 [  604] 
    
    ///////////////////////////////////////////////////////////
    // For Intra 16x16 store the Luma DC in case of Luma, for chroma 
    // always store the DC
    
       VST16.i8          vr02,[i0, 0]                                              // MBCodingType CoefficientsLane0 DcPtr [  610] 
       VST16_4.i8        vr02,[i0, 2]                                              // MBCodingType CoefficientsLane0 DcPtr [  611] 
    
       VIM               i0, i0, -4                                                                         // DcPtr DcPtr [  613] 
    
    // Clear the DC component
       VMOVW.i8          vr02, 0                                                         // MBCodingType CoefficientsLane0 [  616] 
  
    // Extract the sign
       VSIGNW            vr14, vr04                                                         // Lane1Sign CoefficientsLane1 [  619] 
       VSIGNW            vr13, vr02                                                         // Lane0Sign CoefficientsLane0 [  620] 
       VSIGNW            vr15, vr06                                                         // Lane2Sign CoefficientsLane2 [  621] 
       VSIGNW            vr16, vr05                                                         // Lane3Sign CoefficientsLane3 [  622] 

    // Extract the absolute value
       VABSW             vr02, vr02                                                 // CoefficientsLane0 CoefficientsLane0 [  625] 
       VABSW             vr04, vr04                                                 // CoefficientsLane1 CoefficientsLane1 [  626] 
       VABSW             vr06, vr06                                                 // CoefficientsLane2 CoefficientsLane2 [  627] 
       VABSW             vr05, vr05                                                 // CoefficientsLane3 CoefficientsLane3 [  628] 


       VMULFW            vr02, vr02, vr09                                // CoefficientsLane0 CoefficientsLane0 QuantLane0 [  631] 
       VMULFW            vr04, vr04, vr10                                // CoefficientsLane1 CoefficientsLane1 QuantLane1 [  632] 
       VMULFW            vr06, vr06, vr11                                // CoefficientsLane2 CoefficientsLane2 QuantLane2 [  633] 
       VMULFW            vr05, vr05, vr12                                // CoefficientsLane3 CoefficientsLane3 QuantLane3 [  634] 
    
 
       VADDW             vr02, vr02, i7                                     // CoefficientsLane0 CoefficientsLane0 RndingF [  637] 
       VADDW             vr04, vr04, i7                                     // CoefficientsLane1 CoefficientsLane1 RndingF [  638] 
       VADDW             vr06, vr06, i7                                     // CoefficientsLane2 CoefficientsLane2 RndingF [  639] 
       VADDW             vr05, vr05, i7                                     // CoefficientsLane3 CoefficientsLane3 RndingF [  640] 
    

    

       VASRW             vr02, vr02, i2                                 // CoefficientsLane0 CoefficientsLane0 ShiftNormal [  645] 
       VASRW             vr04, vr04, i2                                 // CoefficientsLane1 CoefficientsLane1 ShiftNormal [  646] 
       VASRW             vr06, vr06, i2                                 // CoefficientsLane2 CoefficientsLane2 ShiftNormal [  647] 
       VASRW             vr05, vr05, i2                                 // CoefficientsLane3 CoefficientsLane3 ShiftNormal [  648] 
 
    
    // Set the NonZeroCoeff.
       VADDW             vr25, vr02, vr04                               // TotalCoeffs CoefficientsLane0 CoefficientsLane1 [  652] 
       VADDAW            vr25, vr06, vr05                               // TotalCoeffs CoefficientsLane2 CoefficientsLane3 [  653] 
    // Restore sign
       VMULW             vr02, vr02, vr13                                 // CoefficientsLane0 CoefficientsLane0 Lane0Sign [  655] 
       VMULW             vr04, vr04, vr14                                 // CoefficientsLane1 CoefficientsLane1 Lane1Sign [  656] 
       VMULW             vr06, vr06, vr15                                 // CoefficientsLane2 CoefficientsLane2 Lane2Sign [  657] 
    
       VADDW.F           vr25, vr25, 0                                                          // TotalCoeffs TotalCoeffs [  659] 
    
       VMULW             vr05, vr05, vr16                                 // CoefficientsLane3 CoefficientsLane3 Lane3Sign [  661] 
    
    // Small coefficient removal
    // Store the sum of each elements of the block
    // The blocks here are 2 4x4 and a total of 8 values.
    // Small coefficient removal is done on a 8x8 block basis
       VXSUMW.i11        vr26, vr25, 255                                                 // CoeffMask CoeffSum TotalCoeffs [  667] 
       VASRW.8           vr01, vr01, 1                                                              // CoeffMask CoeffMask [  668] 
    
    // First block
       VTANY.15.NZ       i1, 1                                                                               // AllCoeffs0 [  671] 
    // Check for the next block
       VTANY.240.NZ      i9, 2                                                                               // AllCoeffs1 [  673] 
       VOR.2             vr27, vr27, vr00                                          // NonZeroCoeff NonZeroCoeff AllCoeffs0 [  674] 
       VST128            vr02,[i10, 0]                                                      // CoefficientsLane0 BuffStart [  675] 
       VOR.2             vr27, vr27, vr01                                          // NonZeroCoeff NonZeroCoeff AllCoeffs1 [  676] 
    
    
    
    ///////////////////////////////////////////////////////////
    // Store back the result
       VST128            vr04,[i10, 32]                                                     // CoefficientsLane1 BuffStart [  682] 
       VJB               vr08, 0                                                                          // TargetAddress [  683] 
       VST128            vr06,[i10, 64]  //DS_________________________________________________ CoefficientsLane2 BuffStart [  684] 
       VST128            vr05,[i10, 96]  //DS_________________________________________________ CoefficientsLane3 BuffStart [  685] 
       VIM               i10, i10, -16  //DS__________________________________________________________ BuffStart BuffStart [  686] 
// End                                                                                                                  // [  687] // Rename lanes

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Forward4x4TransformQuantise                                                                                  // [  689] 


 //----------------------------------------------------------------------------------------------------
// Is Macro
// Zeros one 8x8 block

// [Subroutine] ZeroBlocks                                                                                              // [  694] 
_Asm int Record_ZeroBlocks(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,20]
  vrec r1


LINE 696".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VST128            vr62,[i10, 0]                                                                  // VZERO BuffStart [  696] 
       VST128            vr62,[i10, 32]                                                                 // VZERO BuffStart [  697] 
       VST128            vr62,[i10, 64]                                                                 // VZERO BuffStart [  698] 
       VST128            vr62,[i10, 96]                                                                 // VZERO BuffStart [  699] 
       VST128            vr62,[i10, 128]                                                                // VZERO BuffStart [  700] 
       VJB               vr08, 0                                                                          // TargetAddress [  701] 
       VST128            vr62,[i10, 160]  //DS____________________________________________________________ VZERO BuffStart [  702] 
       VST128            vr62,[i10, 192]  //DS____________________________________________________________ VZERO BuffStart [  703] 
       VST128            vr62,[i10, 224]  //DS____________________________________________________________ VZERO BuffStart [  704] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ZeroBlocks                                                                                                   // [  706] 



//----------------------------------------------------------------------------------------------------
// Is Macro
// Calls  forward 4x4 transform
// which in turn will do for two adjacent blocks
// This also does the smallcoeff removal

// [Macro] Forward4x4LumaTransformAndSmallCoeff                                                                         // [  714] 
_Asm int Record_Forward4x4LumaTransformAndSmallCoeff(int *mac_tab)
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
Forward4x4LumaTransformAndSmallCoeff_RecLoop:
  ld r1,[r17 ,24]
  vrec r1

    // Params -----------

    // End params -------
// p16    i13  Buff                                                                                                     // [  718] 
    // Load LUMA SHIFT
LINE 720".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr01, LUMA_DIV, 4                                                                    // BuffStart [  720] 
       VMOVW             vr01, LUMA_F, 32                                                                          // Buff [  721] 
       VLD16_2           vr00,[i10, 0]                                                            // ShiftNormal BuffStart [  722] 
    
    // Load LUMA rounding factor
       VLD16_7           vr00,[i13, 0]                                                                     // RndingF Buff [  725] 
       VMOVW             vr01, LUMA_QUANT_TABLE, 4                                                            // BuffStart [  726] 
    // Mask for lane 8
       VMOVW             vr01, 128, 8                                                                         // CoeffMask [  728] 
       VLD16_2           vr01,[i10, 0]                                                              // BuffStart BuffStart [  729] 
    
       VASRW.128         vr00, vr00, 1                                                                  // RndingF RndingF [  731] 

       VMOVW             vr29, (3*32), 255                                                                       // Offset [  733] 
  ld r1,[r17,16];                                                             // TargetAddress Forward4x4TransformQuantise [  734] 
       VMOVW             vr08, r1, 255                                                                                  // [  734] 
	
    // Load LUMA QUANT factor
       VLD64             vr09,[i10, 0]                                                             // QuantLane0 BuffStart [  737] 
       VLD64             vr10,[i10, 8]                                                             // QuantLane1 BuffStart [  738] 
       VLD64             vr11,[i10, 16]                                                            // QuantLane2 BuffStart [  739] 
       VLD64             vr12,[i10, 24]                                                            // QuantLane3 BuffStart [  740] 
    
    //Shift the quant factors
       VMR4W.240         vr09, vr09, vr09                                              // QuantLane0 QuantLane0 QuantLane0 [  743] 
       VMR4W.240         vr10, vr10, vr10                                              // QuantLane1 QuantLane1 QuantLane1 [  744] 
       VMR4W.240         vr11, vr11, vr11                                              // QuantLane2 QuantLane2 QuantLane2 [  745] 
       VMR4W.240         vr12, vr12, vr12                                              // QuantLane3 QuantLane3 QuantLane3 [  746] 
    
    
     
       VADDW.171         vr08, vr08, 1                                                      // TargetAddress TargetAddress [  750] 
    //vld8w               'MBCodingType, [CirBuf, MPO_MBCodingType]
    //vseqw               'MBCodingType, MBCT_H264_INTRA16X16
    //vmovw.s.1           'MBCodingType, 17   //// Mask for lane0 and lane4 where the DC values are 
    //vmovw!s.1           'MBCodingType, 0             
       VJB               vr08, r3                                                       // TargetAddress LumaTransformExit [  755] 
    // Set the start to block14, to do in reverse order.
    // If done in reverse order then NonZeroCoeff needs shifting and oring
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_Y+(12*32+16))  //DS_________________ BuffStart CirBuf [  758] 
       VIM               i0, i4, (MPO_Y_DC_CoeffStore0+28)  //DS_____________________________________________ DcPtr CirBuf [  759] // FOR Block14 and block15
    // Set the initial values for NonZeroCoeff, each of the 16 bit represents if one of the 16 blocks are
    // having non-zero coefficients. DC is not considered.
       VMOVW             vr27, 0, 2  //DS____________________________________________________________________ NonZeroCoeff [  762] 
    


//LumaTransformExit --------                                                                                            // [  766] 
 vendrec r3                                                                                                             // [  766] 
 vrec r3                                                                                                                // [  766] 

    // Store the NonZeroCoeff
LINE 769".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VJP.i8            r4                                                                 // MBCodingType SmallCoeffExit [  769] 

       
    //Small Coefficient removal
    // Do this only if it is not MBCT_H264_INTRA16X16 and MBCT_H264_INTRA4X4
    // MBCT_H264_INTRA4X4 not supported
    // CoeffSum stores the sum of 2 4x4 blocks in each lane
    // -------------------------
    //  7  6  5  4  3 2 1 0

    // -------------------------
    
    // For 8x8 blocks the addition will be  0 + 2, 1 + 3, 4+6, 5+7
    // 1 0 + 3 2
       VXSUM.3           vr26, vr26, 15  //DS___________________________________________________________ CoeffSum CoeffSum [  783] // DELAY SLOT
       VSR8.1            vr27, vr27, 2  //DS____________________________________________________ NonZeroCoeff NonZeroCoeff [  784] // DELAY SLOT
    // 5  4 + 7  6
       VXSUM.12          vr26, vr26, 240  //DS__________________________________________________________ CoeffSum CoeffSum [  786] // DELAY SLOT
    
    // Sum of all coeffs for each 8x8 block is  3  2  1  0
    
     // Calculate the sum of all four 8x8 blocks
       VXSUMW.4          vr27, vr26, 15                                                               // TotalSum CoeffSum [  791] 
    
    // If all four blocks are zero then exit the loop
       VADDW.4.F         vr27, vr27, 0                                                                // TotalSum TotalSum [  794] 
       VTANY.4.Z         i12, 1                                                                                 // AllZero [  795] 
       VJP.i12           r4                                                                      // AllZero SmallCoeffExit [  796] 
    
    // Check if the sum of each block is less than 2 and set the sticky flag
       VSLTW.15          vr26, 2  //DS___________________________________________________________________________ CoeffSum [  799] 
    
    // By default move all values to zero . for less than 2 the value will be zero
       VMOVW             vr26, 0, 15  //DS_______________________________________________________________________ CoeffSum [  802] 
    
    // Any block whose value is more than 2 is set to 9
       VMOVW.15.U        vr26, 9  //DS___________________________________________________________________________ CoeffSum [  805] 
    
    
    // Value for comparing all four blocks
       VMOVW             vr00, 4, 15                                                                    // SmallCoeffLimit [  809] 
    
    // Check if the sum of all blocks is less than 6
       VXSUMW.15         vr28, vr26, 15                                                            // SumOfBlocks CoeffSum [  812] 
       VSLTW.15          vr28, 6                                                                            // SumOfBlocks [  813] 
    
    // If less than 6 then all the blocks has to be zeroes
    // Make i00 to i03 1
       VMOVW.15.S        vr00, 1                                                                        // SmallCoeffLimit [  817] 
    
    // If all the blocks are zeroed then NonZeroCoeff is made zero
    // Not needed as can be done with the individual blocks
    
    // If sum of all blocks is not less than 6 then compare each block to be less than or
    // equal to 4
       VLEW.15.U         vr00, vr26, vr00                                      // SmallCoeffLimit CoeffSum SmallCoeffLimit [  824] 
    
    // If no blocks are to be zero just exit the loop
       VADDW.15.F        vr00, vr00, 0                                                  // SmallCoeffLimit SmallCoeffLimit [  827] 
       VTALL.15.Z        i12, 1                                                                                 // AllZero [  828] 
       VJP.i12           r4                                                                      // AllZero SmallCoeffExit [  829] 
       VNOP                //DS___________________________________________________________________________________________ [  830] 
       VNOP                //DS___________________________________________________________________________________________ [  831] 
       VNOP                //DS___________________________________________________________________________________________ [  832] 
   
    // NOTE with conditional VAND operation the max range for limm data is -16384 to 16383
    // So the mask for the blocks will not work in this case, hence need to move the mask into ARC register
    
    // Zero block 0 conditionally            
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [  838] 
       VJL.i0            vr08, r1                                                                                       // [  838] 
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_Y)  //DS____________________________ BuffStart CirBuf [  839] 
  
   mov             r1, 0xffcc
       VAND.1.NZ         vr27, vr27, r1  //DS___________________________________________________ NonZeroCoeff NonZeroCoeff [  842] 
       VSR8.15           vr63, vr63, 2  //DS________________________________________________________________ VFLAGS VFLAGS [  843] 
    
       VADDW.4           vr01, vr01, 16                                                             // BuffStart BuffStart [  845] 
    
    // Zero block 1 conditionally 
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [  848] 
       VJL.i1            vr08, r1                                                                                       // [  848] 
   mov             r1, 0xff33
       VAND.1.NZ         vr27, vr27, r1  //DS___________________________________________________ NonZeroCoeff NonZeroCoeff [  850] 
       VSR8.15           vr63, vr63, 2  //DS________________________________________________________________ VFLAGS VFLAGS [  851] 
       VNOP                //DS___________________________________________________________________________________________ [  852] 
    
       VADDW.4           vr01, vr01, ((32*8)-16)                                                    // BuffStart BuffStart [  854] 
    
    // Zero block 2 conditionally 
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [  857] 
       VJL.i2            vr08, r1                                                                                       // [  857] 
   mov             r1, 0xccff
       VAND.1.NZ         vr27, vr27, r1  //DS___________________________________________________ NonZeroCoeff NonZeroCoeff [  859] 
       VSR8.15           vr63, vr63, 2  //DS________________________________________________________________ VFLAGS VFLAGS [  860] 
       VNOP                //DS___________________________________________________________________________________________ [  861] 
    
       VADDW.4           vr01, vr01, 16                                                             // BuffStart BuffStart [  863] 
    
    // Zero block 3 conditionally 
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [  866] 
       VJL.i3            vr08, r1                                                                                       // [  866] 
   mov             r1, 0x33ff
       VAND.1.NZ         vr27, vr27, r1  //DS___________________________________________________ NonZeroCoeff NonZeroCoeff [  868] 
       VNOP                //DS___________________________________________________________________________________________ [  869] 
       VNOP                //DS___________________________________________________________________________________________ [  870] 
   

//SmallCoeffExit --------                                                                                               // [  873] 
 vendrec r4                                                                                                             // [  873] 
 vrec r4                                                                                                                // [  873] 

LINE 875".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VIM               i10, i4, MPO_NonZeroCoeffLuma                                                 // BuffStart CirBuf [  875] 
    
    // This is forced to zero as NonZeroCoeffLuma is shifted down at this place
       VST16_0           vr27,[i10, 0]                                                           // NonZeroCoeff BuffStart [  878] 


  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz Forward4x4LumaTransformAndSmallCoeff_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Forward4x4LumaTransformAndSmallCoeff                                                                         // [  881] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Calls  forward 4x4 transform
// which in turn will do for two adjacent blocks
// This also does the smallcoeff removal

// [Macro] Forward4x4ChromaTransformAndSmallCoeff                                                                       // [  890] 
_Asm int Record_Forward4x4ChromaTransformAndSmallCoeff(int *mac_tab)
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
Forward4x4ChromaTransformAndSmallCoeff_RecLoop:
  ld r1,[r17 ,28]
  vrec r1


// p16    i13  Buff                                                                                                     // [  892] 
    // Load Chroma SHIFT
LINE 894".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr01, CHROMA_DIV, 4                                                                  // BuffStart [  894] 
       VMOVW             vr01, CHROMA_F, 32                                                                        // Buff [  895] 
       VLD16_2           vr00,[i10, 0]                                                            // ShiftNormal BuffStart [  896] 
    
    // Load LUMA rounding factor
       VLD16_7           vr00,[i13, 0]                                                                     // RndingF Buff [  899] 
      
    
    
       VMOVW             vr01, 8, 8                                                                           // CoeffMask [  903] 
       VMOVW             vr26, 0, 255                                                                          // CoeffSum [  904] 


    // Load CHROMA QUANT factor
       VMOVW             vr01, CHROMA_QUANT_TABLE, 4                                                          // BuffStart [  908] 
       VASRW.128         vr00, vr00, 1                                                                  // RndingF RndingF [  909] 
       VLD16_2           vr01,[i10, 0]                                                              // BuffStart BuffStart [  910] 
       VMOVW             vr29, ((3*32)+16), 255                                                                  // Offset [  911] 
     
  ld r1,[r17,16];                                                             // TargetAddress Forward4x4TransformQuantise [  913] 
       VMOVW             vr08, r1, 3                                                                                    // [  913] 
       VMOVW             vr01, 17, 1                                                                       // MBCodingType [  914] //// Mask for lane0 and lane4 where the DC values are 
       VADDW.1           vr08, vr08, 1                                                      // TargetAddress TargetAddress [  915] 
       VLD64             vr09,[i10, 0]                                                             // QuantLane0 BuffStart [  916] 
       VLD64             vr10,[i10, 8]                                                             // QuantLane1 BuffStart [  917] 
       VLD64             vr11,[i10, 16]                                                            // QuantLane2 BuffStart [  918] 
       VLD64             vr12,[i10, 24]                                                            // QuantLane3 BuffStart [  919] 
       VMR4W.240         vr09, vr09, vr09                                              // QuantLane0 QuantLane0 QuantLane0 [  920] 
    
    
       VMR4W.240         vr10, vr10, vr10                                              // QuantLane1 QuantLane1 QuantLane1 [  923] 
    
   
       VMR4W.240         vr11, vr11, vr11                                              // QuantLane2 QuantLane2 QuantLane2 [  926] 
    
    
       VMR4W.240         vr12, vr12, vr12                                              // QuantLane3 QuantLane3 QuantLane3 [  929] 
    
    
       VMOVW             vr08, r3, 4                                                       // TargetAddress VTransformExit [  932] 
               
    // Transform for U component
       VJB               vr08, 0                                                                          // TargetAddress [  935] 
    // Set the start to block2, to do in reverse order.
    // If done in reverse order then NonZeroCoeffChroma needs shifting and oring
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_V+(4*32))  //DS_____________________ BuffStart CirBuf [  938] 
       VIM               i0, i4, (MPO_V_DC_CoeffStore+4)  //DS_______________________________________________ DcPtr CirBuf [  939] // FOR Block2 and block3
    // Set the initial values for NonZeroCoeffsChroma, each of the bit represents one of the 4x4 blocks.
    // DC is not considered.
       VMOVW             vr27, 0, 2  //DS____________________________________________________________________ NonZeroCoeff [  942] 

//VTransformExit --------                                                                                               // [  944] 
 vendrec r3                                                                                                             // [  944] 
 vrec r3                                                                                                                // [  944] 
    
    
    // Transform for V component
    
  ld r1,[r17,16];                                                             // TargetAddress Forward4x4TransformQuantise [  949] 
LINE 949".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr08, r1, 3                                                                                    // [  949] 
       VMOVW             vr08, r4, 4                                                       // TargetAddress UTransformExit [  950] 
       VADDW.1           vr08, vr08, 1                                                      // TargetAddress TargetAddress [  951] 
       VJB               vr08, 0                                                                          // TargetAddress [  952] 
    // Set the start to block2, to do in reverse order.
    // If done in reverse order then NonZeroCoeffChroma needs shifting and oring
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_U+(4*32))  //DS_____________________ BuffStart CirBuf [  955] 
       VIM               i0, i4, (MPO_U_DC_CoeffStore+4)  //DS_______________________________________________ DcPtr CirBuf [  956] // FOR Block2 and block3
       VNOP                //DS___________________________________________________________________________________________ [  957] 



//UTransformExit --------                                                                                               // [  961] 
 vendrec r4                                                                                                             // [  961] 
 vrec r4                                                                                                                // [  961] 

    // Small coefficient removal
   
    // CoeffSum stores the sum of 2 4x4 blocks in each lane
    // -------------------------
    //    V V U U

    // -------------------------
    
    // For 8x8 blocks add the both Vs together and same with Us
    // 1 0 + 3 2
LINE 973".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VXSUMW.1          vr26, vr26, 3                                                                // CoeffSum CoeffSum [  973] 
    
    // 5  4 + 7  6
       VXSUMW.2          vr26, vr26, 12                                                               // CoeffSum CoeffSum [  976] 
    
    // Sum of all coeffs for each v and U 1  0
    
    // If all four blocks are zero then exit the loop
       VADDW.F           vr26, vr26, 0                                                                // CoeffSum CoeffSum [  981] 
       VTALL.3.Z         i12, 1                                                                                 // AllZero [  982] 
       VJP.i12           r5                                                                          // AllZero ChromaExit [  983] 
    
    // Move Coeff count to lane1
       VSR8.1            vr27, vr27, 2  //DS____________________________________________________ NonZeroCoeff NonZeroCoeff [  986] 
    // Check if the sum of each block is less than 2 and set the sticky flag
       VSLTW.3           vr26, 2  //DS___________________________________________________________________________ CoeffSum [  988] 
    
    // By default move all values to zero . for less than 2 the value will be zero
       VMOVW             vr26, 0, 3  //DS________________________________________________________________________ CoeffSum [  991] 
    
    // Any block whose value is more than 2 is set to 9
       VMOVW.3.U         vr26, 9                                                                               // CoeffSum [  994] 
    
    
    // Value for comparing all four blocks
       VMOVW             vr00, 4, 3                                                                     // SmallCoeffLimit [  998] 
    
    
    // If sum of all blocks is than or  equal to 4
       VLEW.3            vr00, vr26, vr00                                      // SmallCoeffLimit CoeffSum SmallCoeffLimit [ 1002] 
    
    // If no blocks are to be zero just exit the loop
       VADDW.3.F         vr00, vr00, 0                                                  // SmallCoeffLimit SmallCoeffLimit [ 1005] 
       VTALL.3.Z         i12, 1                                                                                 // AllZero [ 1006] 
       VJP.i12           r5                                                                          // AllZero ChromaExit [ 1007] 
       VNOP                //DS___________________________________________________________________________________________ [ 1008] 
       VNOP                //DS___________________________________________________________________________________________ [ 1009] 
       VNOP                //DS___________________________________________________________________________________________ [ 1010] 
   
    
    // Zero V block conditionally            
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [ 1014] 
       VJL.i0            vr08, r1                                                                                       // [ 1014] 
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_U)  //DS____________________________ BuffStart CirBuf [ 1015] 
    
    // clear v
       VAND.1.NZ         vr27, vr27, 0xf0  //DS_________________________________________________ NonZeroCoeff NonZeroCoeff [ 1018] 
       VSR8.15           vr63, vr63, 2  //DS________________________________________________________________ VFLAGS VFLAGS [ 1019] 
       VNOP                                                                                                             // [ 1020] 
    
    // Zero U block conditionally            
  ld r1,[r17,20];                                                                              // TargetAddress ZeroBlocks [ 1023] 
       VJL.i1            vr08, r1                                                                                       // [ 1023] 
       VIM               i10, i4, (MPO_PixelCoeffBuffer+PCB_RESIDUAL_V)  //DS____________________________ BuffStart CirBuf [ 1024] 
    
    // clear v
       VAND.1.NZ         vr27, vr27, 0x0f  //DS_________________________________________________ NonZeroCoeff NonZeroCoeff [ 1027] 
       VNOP                //DS___________________________________________________________________________________________ [ 1028] 

    
 
//ChromaExit --------                                                                                                   // [ 1032] 
 vendrec r5                                                                                                             // [ 1032] 
 vrec r5                                                                                                                // [ 1032] 

LINE 1034".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VIM               i10, i4, MPO_NonZeroCoeffChroma                                               // BuffStart CirBuf [ 1034] 
    
    // This is forced to zero as NonZeroCoeffLuma is shifted down at this place
       VST8_0            vr27,[i10, 0]                                                           // NonZeroCoeff BuffStart [ 1037] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz Forward4x4ChromaTransformAndSmallCoeff_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Forward4x4ChromaTransformAndSmallCoeff                                                                       // [ 1039] 



// End                                                                                                                  // [ 1041] // Global variables



//############################################################################################################################

//                                  INVERSE TRANSFORM AND QUANTISATION

//#############################################################################################################################


//----------------------------------------------------------------------------------------------------
// Is Macro
// Does the Inverse hadamard transform for luma DC
// and dequantisation

// Global variables for

// Begin                                                                                                                // [ 1059] 

//vec16  vr02  CoeffLane0                                                                                               // [ 1061] 
//vec16  vr03  CoeffLane1                                                                                               // [ 1062] 
//vec16  vr04  CoeffLane2                                                                                               // [ 1063] 
//vec16  vr05  CoeffLane3                                                                                               // [ 1064] 
//vec16  vr06  TargetAddress                                                                                            // [ 1065] 
// p16    i0  MBCodingType                                                                                              // [ 1066] 
// p16    i1  BuffStart                                                                                                 // [ 1067] // Start of Luma DC coefficients
// p16    i2  Address                                                                                                   // [ 1068] 
// p16    i3  DivFactor                                                                                                 // [ 1069] 
// p16    i7  NonZeroCoeff                                                                                              // [ 1070] 
// s16    vr07.128  NonZeroCoeffFlag                                                                                    // [ 1071] 
// s16    vr08.128  NonZeroCoeffVal                                                                                     // [ 1072] 
// p16    i8  SkipFlag                                                                                                  // [ 1073] 
//vec16  vr09  ShiftValue                                                                                               // [ 1074] 
//vec16  vr10  ShiftValueWithDC                                                                                         // [ 1075] 
//vec16  vr11  NonZeroBlocks                                                                                            // [ 1076] 
//vec16  vr12  QuantLane0                                                                                               // [ 1077] 
//vec16  vr13  QuantLane1                                                                                               // [ 1078] 
//vec16  vr14  QuantLane2                                                                                               // [ 1079] 
//vec16  vr15  QuantLane3                                                                                               // [ 1080] 
//vec16  vr16  ShiftedCoeffs1                                                                                           // [ 1081] 
//vec16  vr17  ShiftedCoeffs2                                                                                           // [ 1082] 
//vec16  vr18  LinkRegister                                                                                             // [ 1083] 
//vec16  vr19  Offset                                                                                                   // [ 1084] 
// p16    i9  CoefWriteAddr                                                                                             // [ 1085] 
// p16    i10  UBuffer                                                                                                  // [ 1086] 
// p16    i11  VBuffer                                                                                                  // [ 1087] 
// p16    i12  UAddress                                                                                                 // [ 1088] 
// p16    i13  VAddress                                                                                                 // [ 1089] 

// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|  NonZeroCoeff|       CirLeft|        FixBuf|        CirBuf|     DivFactor|       Address|     BuffStart|  MBCodingType|
//|VR 1|              |              |      VAddress|      UAddress|       VBuffer|       UBuffer| CoefWriteAddr|      SkipFlag|
//|VR 2|                                                  CoeffLane0                                                           |
//|VR 3|                                                  CoeffLane1                                                           |
//|VR 4|                                                  CoeffLane2                                                           |
//|VR 5|                                                  CoeffLane3                                                           |
//|VR 6|                                               TargetAddress                                                           |
//|VR 7| NonZeroCoeff.|              |              |              |              |              |              |              |
//|VR 8| NonZeroCoeff.|              |              |              |              |              |              |              |
//|VR 9|                                                  ShiftValue                                                           |
//|VR10|                                            ShiftValueWithDC                                                           |
//|VR11|                                               NonZeroBlocks                                                           |
//|VR12|                                                  QuantLane0                                                           |
//|VR13|                                                  QuantLane1                                                           |
//|VR14|                                                  QuantLane2                                                           |
//|VR15|                                                  QuantLane3                                                           |
//|VR16|                                              ShiftedCoeffs1                                                           |
//|VR17|                                              ShiftedCoeffs2                                                           |
//|VR18|                                                LinkRegister                                                           |
//|VR19|                                                      Offset                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+



// [Subroutine] InverseTransformLumaDC                                                                                  // [ 1094] 
_Asm int Record_InverseTransformLumaDC(int *mac_tab)
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

    // Params -----------

    // End params -------

// Begin                                                                                                                // [ 1099] 

    // load input data
LINE 1102".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VLD64             vr02,[i1, 0]                                                              // CoeffLane0 BuffStart [ 1102] 
       VLD64             vr04,[i1, 16]                                                             // CoeffLane2 BuffStart [ 1103] 
       VLD64             vr03,[i1, 8]                                                              // CoeffLane1 BuffStart [ 1104] 
       VLD64             vr05,[i1, 24]                                                             // CoeffLane3 BuffStart [ 1105] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_Y                                  // BuffStart CirBuf [ 1106] 

    // Froward Transform
    // apply verical transform
    // stage 0
       VADDSUW           vr02, vr04                                                               // CoeffLane0 CoeffLane2 [ 1111] 
       VADDSUW           vr03, vr05                                                               // CoeffLane1 CoeffLane3 [ 1112] 

    //stage 1
       VADDSUW           vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1115] 
       VADDSUW           vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1116] 
// End                                                                                                                  // [ 1117] 

// Begin                                                                                                                // [ 1119] 
    // Rename the lanes as after the vaddsuw operation
    // LumaDCLanes move to different vector registers then the originally loaded.
//vec16  vr02  CoeffLane0                                                                                               // [ 1122] 
//vec16  vr04  CoeffLane1                                                                                               // [ 1123] 
//vec16  vr05  CoeffLane2                                                                                               // [ 1124] 
//vec16  vr03  CoeffLane3                                                                                               // [ 1125] 

    // transpose matrix
LINE 1128".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr05                                                               // CoeffLane0 CoeffLane2 [ 1128] 
       VEXCH2            vr04, vr03                                                               // CoeffLane1 CoeffLane3 [ 1129] 
       VEXCH1            vr02, vr04                                                               // CoeffLane0 CoeffLane1 [ 1130] 
       VEXCH1            vr05, vr03                                                               // CoeffLane2 CoeffLane3 [ 1131] 

    // apply horizontal transform
    // stage 0
       VADDSUW           vr02, vr05                                                               // CoeffLane0 CoeffLane2 [ 1135] 
       VADDSUW           vr04, vr03                                                               // CoeffLane1 CoeffLane3 [ 1136] 

    //stage 1
       VADDSUW           vr02, vr04                                                               // CoeffLane0 CoeffLane1 [ 1139] 
       VADDSUW           vr05, vr03                                                               // CoeffLane2 CoeffLane3 [ 1140] 
// End                                                                                                                  // [ 1141] 

// Begin                                                                                                                // [ 1143] 
    // Rename the lanes as after the vaddsuw operation
//vec16  vr02  CoeffLane0                                                                                               // [ 1145] 
//vec16  vr05  CoeffLane1                                                                                               // [ 1146] 
//vec16  vr03  CoeffLane2                                                                                               // [ 1147] 
//vec16  vr04  CoeffLane3                                                                                               // [ 1148] 


LINE 1151".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr03                                                               // CoeffLane0 CoeffLane2 [ 1151] 
       VEXCH2            vr05, vr04                                                               // CoeffLane1 CoeffLane3 [ 1152] 
       VEXCH1            vr02, vr05                                                               // CoeffLane0 CoeffLane1 [ 1153] 
       VEXCH1            vr03, vr04                                                               // CoeffLane2 CoeffLane3 [ 1154] 

    // Update the NonZeroCoeff flags

       VGMW              i7, vr04                                                               // NonZeroCoeff CoeffLane3 [ 1158] 
       VST16_0           vr02,[i1, (0*32)+0]                                                       // CoeffLane0 BuffStart [ 1159] 
       VST16_1           vr02,[i1, (0*32)+8]                                                       // CoeffLane0 BuffStart [ 1160] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1161] 
       VST16_2           vr02,[i1, (0*32)+16]                                                      // CoeffLane0 BuffStart [ 1162] 
       VMULW.128         vr07, vr07, 16                                               // NonZeroCoeffFlag NonZeroCoeffFlag [ 1163] 

       VGMW              i7, vr03                                                               // NonZeroCoeff CoeffLane2 [ 1165] 
       VST16_3           vr02,[i1, (0*32)+24]                                                      // CoeffLane0 BuffStart [ 1166] 
       VST16_0           vr05,[i1, (4*32)+0]                                                       // CoeffLane1 BuffStart [ 1167] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1168] 
       VST16_1           vr05,[i1, (4*32)+8]                                                       // CoeffLane1 BuffStart [ 1169] 
       VMULW.128         vr07, vr07, 16                                               // NonZeroCoeffFlag NonZeroCoeffFlag [ 1170] 

       VGMW              i7, vr05                                                               // NonZeroCoeff CoeffLane1 [ 1172] 
       VST16_2           vr05,[i1, (4*32)+16]                                                      // CoeffLane1 BuffStart [ 1173] 
       VST16_3           vr05,[i1, (4*32)+24]                                                      // CoeffLane1 BuffStart [ 1174] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1175] 
       VST16_0           vr03,[i1, (8*32)+0]                                                       // CoeffLane2 BuffStart [ 1176] 
       VMULW.128         vr07, vr07, 16                                               // NonZeroCoeffFlag NonZeroCoeffFlag [ 1177] 

       VGMW              i7, vr02                                                               // NonZeroCoeff CoeffLane0 [ 1179] 
       VST16_1           vr03,[i1, (8*32)+8]                                                       // CoeffLane2 BuffStart [ 1180] 
       VST16_2           vr03,[i1, (8*32)+16]                                                      // CoeffLane2 BuffStart [ 1181] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1182] 
       VST16_3           vr03,[i1, (8*32)+24]                                                      // CoeffLane2 BuffStart [ 1183] 
       VOR.128           vr00, vr08, vr07                                 // NonZeroCoeff NonZeroCoeffVal NonZeroCoeffFlag [ 1184] 
    

    // Store the DC coefficients into the Pixel coeff buffer
   


       VST16_0           vr04,[i1, (12*32)+0]                                                      // CoeffLane3 BuffStart [ 1191] 

       VJB               vr06, 0                                                                          // TargetAddress [ 1193] 
       VST16_1           vr04,[i1, (12*32)+8]  //DS__________________________________________________ CoeffLane3 BuffStart [ 1194] 
       VST16_2           vr04,[i1, (12*32)+16]  //DS_________________________________________________ CoeffLane3 BuffStart [ 1195] 
       VST16_3           vr04,[i1, (12*32)+24]  //DS_________________________________________________ CoeffLane3 BuffStart [ 1196] 
// End                                                                                                                  // [ 1197] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InverseTransformLumaDC                                                                                       // [ 1199] 




// Does the inverse 4x4 transorm
// two blocks at a time


// [Subroutine] Inverse4x4CoeffTransform                                                                                // [ 1206] 
_Asm int Record_Inverse4x4CoeffTransform(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,36]
  vrec r1

    ////////////////////////////////////////////////////

// Begin                                                                                                                // [ 1209] 
//vec16  vr02  CoeffLane0                                                                                               // [ 1210] 
//vec16  vr03  CoeffLane1                                                                                               // [ 1211] 
//vec16  vr04  CoeffLane2                                                                                               // [ 1212] 
//vec16  vr05  CoeffLane3                                                                                               // [ 1213] 
     // apply verical transform
    
LINE 1216".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VASRW             vr16, vr03, 1                                                        // ShiftedCoeffs1 CoeffLane1 [ 1216] 
       VASRW             vr17, vr05, 1                                                        // ShiftedCoeffs2 CoeffLane3 [ 1217] 
    
    // stage 0
       VADDSUW           vr02, vr04                                                               // CoeffLane0 CoeffLane2 [ 1220] 
    // CoeffLane0 = L0 + L2 , CoeffLane2 =  L0 -L2
    
    
       VADDSUW           vr03, vr17                                                           // CoeffLane1 ShiftedCoeffs2 [ 1224] 
    // CoeffLane1 = L1 + L3>>1
    
       VADDSUW           vr16, vr05                                                           // ShiftedCoeffs1 CoeffLane3 [ 1227] 
    // CoeffLane3= L1>>1 - L3
   

    //stage 1
       VADDSUW           vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1232] 
       VADDSUW           vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1233] 
// End                                                                                                                  // [ 1234] 

// Begin                                                                                                                // [ 1236] 
    // Rename the lanes as after the vaddsuw operation
    // CoefficientsLane move to different vector registers than the the originally loaded.
//vec16  vr02  CoeffLane0                                                                                               // [ 1239] 
//vec16  vr04  CoeffLane1                                                                                               // [ 1240] 
//vec16  vr05  CoeffLane2                                                                                               // [ 1241] 
//vec16  vr03  CoeffLane3                                                                                               // [ 1242] 

    // transpose matrix
LINE 1245".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr05                                                               // CoeffLane0 CoeffLane2 [ 1245] 
       VEXCH2            vr04, vr03                                                               // CoeffLane1 CoeffLane3 [ 1246] 
       VEXCH1            vr02, vr04                                                               // CoeffLane0 CoeffLane1 [ 1247] 
       VEXCH1            vr05, vr03                                                               // CoeffLane2 CoeffLane3 [ 1248] 

    // apply horizontal transform
    
       VASRW             vr16, vr04, 1                                                        // ShiftedCoeffs1 CoeffLane1 [ 1252] 
       VASRW             vr17, vr03, 1                                                        // ShiftedCoeffs2 CoeffLane3 [ 1253] 
    
    // stage 0
    // stage 0
       VADDSUW           vr02, vr05                                                               // CoeffLane0 CoeffLane2 [ 1257] 
    // CoeffLane0 = L0 + L2 , CoeffLane2 =  L0 -L2
    
       VADDSUW           vr16, vr03                                                           // ShiftedCoeffs1 CoeffLane3 [ 1260] 
    // CoeffLane3= L1>>1 - L3
    
       VADDSUW           vr04, vr17                                                           // CoeffLane1 ShiftedCoeffs2 [ 1263] 
    // CoeffLane1 = L1 + L3>>1
    
    //stage 1
    //stage 1
       VADDSUW           vr05, vr03                                                               // CoeffLane2 CoeffLane3 [ 1268] 
       VADDSUW           vr02, vr04                                                               // CoeffLane0 CoeffLane1 [ 1269] 
// End                                                                                                                  // [ 1270] 

// Begin                                                                                                                // [ 1272] 
    // Rename the lanes as after the vaddsuw operation
//vec16  vr02  CoeffLane0                                                                                               // [ 1274] 
//vec16  vr05  CoeffLane1                                                                                               // [ 1275] 
//vec16  vr03  CoeffLane2                                                                                               // [ 1276] 
//vec16  vr04  CoeffLane3                                                                                               // [ 1277] 

LINE 1279".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VEXCH2            vr02, vr03                                                               // CoeffLane0 CoeffLane2 [ 1279] 
       VEXCH2            vr05, vr04                                                               // CoeffLane1 CoeffLane3 [ 1280] 
       VEXCH1            vr02, vr05                                                               // CoeffLane0 CoeffLane1 [ 1281] 
       VEXCH1            vr03, vr04                                                               // CoeffLane2 CoeffLane3 [ 1282] 
    
       VADDW             vr02, vr02, 32                                                           // CoeffLane0 CoeffLane0 [ 1284] 
       VADDW             vr05, vr05, 32                                                           // CoeffLane1 CoeffLane1 [ 1285] 
       VADDW             vr03, vr03, 32                                                           // CoeffLane2 CoeffLane2 [ 1286] 
       VADDW             vr04, vr04, 32                                                           // CoeffLane3 CoeffLane3 [ 1287] 
    
       VASRW             vr02, vr02, 6                                                            // CoeffLane0 CoeffLane0 [ 1289] 
       VASRW             vr05, vr05, 6                                                            // CoeffLane1 CoeffLane1 [ 1290] 
       VASRW             vr03, vr03, 6                                                            // CoeffLane2 CoeffLane2 [ 1291] 
       VASRW             vr04, vr04, 6                                                            // CoeffLane3 CoeffLane3 [ 1292] 

    ///////////////////////////////////////////////////
    
       VST128            vr02,[i9, 0]                                                          // CoeffLane0 CoefWriteAddr [ 1296] 

       VJB               vr18, 0                                                                           // LinkRegister [ 1298] 
       VST128            vr05,[i9, 32]  //DS_____________________________________________________ CoeffLane1 CoefWriteAddr [ 1299] 
       VST128            vr03,[i9, 64]  //DS_____________________________________________________ CoeffLane2 CoefWriteAddr [ 1300] 
       VST128            vr04,[i9, 96]  //DS_____________________________________________________ CoeffLane3 CoefWriteAddr [ 1301] 
// End                                                                                                                  // [ 1302] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Inverse4x4CoeffTransform                                                                                     // [ 1304] 



// Does the Inverse quant and transform
// It is done on two 4x4 blocks in parallel
// [Macro] InverseQuantTransformCoeffs                                                                                  // [ 1308] 
_Asm int Record_InverseQuantTransformCoeffs(int *mac_tab)
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
InverseQuantTransformCoeffs_RecLoop:
  ld r1,[r17 ,40]
  vrec r1

    // Params -----------

    // End params -------
    
    // Check if there are coefficients exists in the block.
    // If there are no coefficients in both the blocks then skip dpoing the inverse quant/transform
    // If any of the two blocks has coefficeint then, do the inverse quant/transform for bioth blocks
    // even if one of the blocks has no coefficients
// Begin                                                                                                                // [ 1317] 
//vec16  vr02  CoeffLane0                                                                                               // [ 1318] 
//vec16  vr03  CoeffLane1                                                                                               // [ 1319] 
//vec16  vr04  CoeffLane2                                                                                               // [ 1320] 
//vec16  vr05  CoeffLane3                                                                                               // [ 1321] 
    // the order of doing the block is 0 & 1 , 4&5 2&3 and so on
LINE 1323".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VADDW.2           vr00, vr00, vr19                                                    // BuffStart BuffStart Offset [ 1323] 
       VADDW.2           vr01, vr01, vr19                                            // CoefWriteAddr CoefWriteAddr Offset [ 1324] 
    // Move the scalar into vector lanes and set the flag. two lanes at time at a time
       VMIVW.3.F         vr11, i7                                                            // NonZeroBlocks NonZeroCoeff [ 1326] 
       VLD128            vr05,[i1, 96]                                                             // CoeffLane3 BuffStart [ 1327] 
       VTALL.3.Z         i8, 1                                                                                 // SkipFlag [ 1328] 
       VLD128            vr02,[i1, 0]                                                              // CoeffLane0 BuffStart [ 1329] 
       VJP.i8            r3                                                          // SkipFlag SkipInverseTransformQuant [ 1330] 
       VASRW.128         vr00, vr00, 2  //DS____________________________________________________ NonZeroCoeff NonZeroCoeff [ 1331] 
        
    
    // Load the blocks
     // load input data
       VLD128            vr03,[i1, 32]  //DS_________________________________________________________ CoeffLane1 BuffStart [ 1336] 
       VLD128            vr04,[i1, 64]  //DS_________________________________________________________ CoeffLane2 BuffStart [ 1337] 
    
    
       VMULW             vr02, vr02, vr12                                              // CoeffLane0 CoeffLane0 QuantLane0 [ 1340] 
       VMULW             vr03, vr03, vr13                                              // CoeffLane1 CoeffLane1 QuantLane1 [ 1341] 
       VMULW             vr04, vr04, vr14                                              // CoeffLane2 CoeffLane2 QuantLane2 [ 1342] 
       VMULW             vr05, vr05, vr15                                              // CoeffLane3 CoeffLane3 QuantLane3 [ 1343] 
    
       VLSLVW            vr02, vr02, vr10                                        // CoeffLane0 CoeffLane0 ShiftValueWithDC [ 1345] 
    
  ld r1,[r17,36];                                                                 // LinkRegister Inverse4x4CoeffTransform [ 1347] 
       VJL               vr18, r1                                                                                       // [ 1347] 
       VLSLVW            vr03, vr03, vr09  //DS__________________________________________ CoeffLane1 CoeffLane1 ShiftValue [ 1348] 
       VLSLVW            vr04, vr04, vr09  //DS__________________________________________ CoeffLane2 CoeffLane2 ShiftValue [ 1349] 
       VLSLVW            vr05, vr05, vr09  //DS__________________________________________ CoeffLane3 CoeffLane3 ShiftValue [ 1350] 
       VJB               vr06, 0                                                                          // TargetAddress [ 1351] 
       VIM               i1, i1, 16  //DS_____________________________________________________________ BuffStart BuffStart [ 1352] 
       VIM               i9, i9, 16  //DS_____________________________________________________ CoefWriteAddr CoefWriteAddr [ 1353] 
       VNOP                //DS___________________________________________________________________________________________ [ 1354] 

// End                                                                                                                  // [ 1356] 

//SkipInverseTransformQuant --------                                                                                    // [ 1358] 
 vendrec r3                                                                                                             // [ 1358] 
 vrec r3                                                                                                                // [ 1358] 

LINE 1360".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VST128            vr62,[i9, 0]                                                               // VZERO CoefWriteAddr [ 1360] 
       VST128            vr62,[i9, 32]                                                              // VZERO CoefWriteAddr [ 1361] 
       VST128            vr62,[i9, 64]                                                              // VZERO CoefWriteAddr [ 1362] 
       VST128            vr62,[i9, 96]                                                              // VZERO CoefWriteAddr [ 1363] 
       VJB               vr06, 0                                                                          // TargetAddress [ 1364] 
       VIM               i1, i1, 16  //DS_____________________________________________________________ BuffStart BuffStart [ 1365] 
       VIM               i9, i9, 16  //DS_____________________________________________________ CoefWriteAddr CoefWriteAddr [ 1366] 
       VNOP                //DS___________________________________________________________________________________________ [ 1367] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz InverseQuantTransformCoeffs_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InverseQuantTransformCoeffs                                                                                  // [ 1369] 






// [Macro] InverseTransformDequantLumaCoeffs                                                                            // [ 1374] 
_Asm int Record_InverseTransformDequantLumaCoeffs(int *mac_tab)
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
InverseTransformDequantLumaCoeffs_RecLoop:
  ld r1,[r17 ,44]
  vrec r1

    // Params -----------

    // End params -------
   
    // Luma DC Inverse transform and Dequantise
    
    // Do the Luma inverse transform only if the coding type is MBCT_H264_INTRA16X16
    //vld8w               'MBCodingType, [CirBuf, MPO_MBCodingType]
    //vseqw               'MBCodingType, MBCT_H264_INTRA16X16
    //vmovw.s.1           'MBCodingType, 17   
    //vmovw!s.1           'MBCodingType, 0
  
    
LINE 1388".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VIM               i1, i4, MPO_NonZeroCoeffLuma                                                  // BuffStart CirBuf [ 1388] 
       VMOVW             vr00, LUMA_DIV, 4                                                                      // Address [ 1389] 
       VLD16_7           vr08,[i1, 0]                                                         // NonZeroCoeffVal BuffStart [ 1390] 
    
    // Luma DC inverse transform only.
    // This will store the DC values back into the pixel coeff buffer 
    // Inverse quant is not doen here, but will be done with all the luma coeffs         
  ld r1,[r17,32];                                                                  // TargetAddress InverseTransformLumaDC [ 1395] 
       VJL.i0            vr06, r1                                                                                       // [ 1395] 
       VIM               i1, i4, MPO_Y_DC_CoeffStore0  //DS______________________________________________ BuffStart CirBuf [ 1396] 
       VMOVW             vr07, 0, 128  //DS______________________________________________________________ NonZeroCoeffFlag [ 1397] 
    // Zero NonZeroCoeff
       VMVW              vr00, vr08, 128  //DS_______________________________________________ NonZeroCoeff NonZeroCoeffVal [ 1399] 
    
    
    // Luma Inverse quant and transform  
  
    // Load LUMA Div factor
       VLD16_3           vr00,[i2, 0]                                                                 // DivFactor Address [ 1405] 
       VMOVW             vr01, TRANSFORM_Y, 2                                                             // CoefWriteAddr [ 1406] 
       VMOVW             vr00, LUMA_INVERSE_QUANT_TABLE, 4                                                      // Address [ 1407] 
  ld r1,[r17,40];                                                             // TargetAddress InverseQuantTransformCoeffs [ 1408] 
       VMOVW             vr06, r1, 255                                                                                  // [ 1408] 
       VLD16_2           vr00,[i2, 0]                                                                   // Address Address [ 1409] 
       VMIVW             vr10, i3                                                            // ShiftValueWithDC DivFactor [ 1410] 
       VMIVW             vr09, i3                                                                  // ShiftValue DivFactor [ 1411] 
    
    // For DC component the DivFactor has to be reduced by 2
       VSUBW.i0          vr10, vr10, 2                                                // ShiftValueWithDC ShiftValueWithDC [ 1414] 


    // Load LUMA QUANT factor
    
       VLD64             vr12,[i2, 0]                                                                // QuantLane0 Address [ 1419] 
       VLD64             vr13,[i2, 8]                                                                // QuantLane1 Address [ 1420] 
       VLD64             vr14,[i2, 16]                                                               // QuantLane2 Address [ 1421] 
       VLD64             vr15,[i2, 24]                                                               // QuantLane3 Address [ 1422] 
    
       VMR4W.240         vr12, vr12, vr12                                              // QuantLane0 QuantLane0 QuantLane0 [ 1424] 
    
       VMR4W.240         vr13, vr13, vr13                                              // QuantLane1 QuantLane1 QuantLane1 [ 1426] 
    
       VMR4W.240         vr14, vr14, vr14                                              // QuantLane2 QuantLane2 QuantLane2 [ 1428] 
    
    
       VADDW.171         vr06, vr06, 2                                                      // TargetAddress TargetAddress [ 1431] 
       VJB               vr06, r3                                                // TargetAddress InverseTransformLumaExit [ 1432] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_Y  //DS______________________________ BuffStart CirBuf [ 1433] 
       VMOVW             vr19, (3*32), 255  //DS___________________________________________________________________ Offset [ 1434] 
       VMR4W.240         vr15, vr15, vr15  //DS__________________________________________ QuantLane3 QuantLane3 QuantLane3 [ 1435] 
    

//InverseTransformLumaExit --------                                                                                     // [ 1438] 
 vendrec r3                                                                                                             // [ 1438] 
 vrec r3                                                                                                                // [ 1438] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz InverseTransformDequantLumaCoeffs_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InverseTransformDequantLumaCoeffs                                                                            // [ 1440] 




/////////////////////////////////////////////////////////////////////////////////////

/// Chroma Inverse Transform and Inverse quantization

////////////////////////////////////////////////////////////////////////////////////


//==================================================================================================

//----------------------------------------------------------------------------------------------------
// Is Macro
// Does the Inverse hadamard transform and quantisation for chroma DC

// [Macro] InverseTransformDequantChromaCoeffs                                                                          // [ 1456] 
_Asm int Record_InverseTransformDequantChromaCoeffs(int *mac_tab)
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
InverseTransformDequantChromaCoeffs_RecLoop:
  ld r1,[r17 ,48]
  vrec r1

    // Params -----------

    // End params -------

   // Chroma DC Inverse transform and Dequantize
    
    // Do the U chroma inverse transform   
    
    
    
    // Chroma DC inverse transform only.
    // This will store the DC values back into the pixel coeff buffer 
    // Inverse quant is not doen here, but will be done with all the chroma coeffs   
   // vmovw                CoeffLane0, 0
   // vmovw                CoeffLane1, 0
   // vmovw                CoeffLane2, 0
   // vmovw                CoeffLane3, 0
 	
LINE 1475".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VIM               i11, i4, MPO_V_DC_CoeffStore                                                    // VBuffer CirBuf [ 1475] 
    // Zero NonZeroCoeff
       VMOVW             vr07, 0, 128                                                                  // NonZeroCoeffFlag [ 1477] 
       VIM               i13, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_V                                  // VAddress CirBuf [ 1478] 
    
    
    // load input V data
       VLD32             vr02,[i11, 0]                                                               // CoeffLane0 VBuffer [ 1482] 
       VLD32             vr03,[i11, 4]                                                               // CoeffLane1 VBuffer [ 1483] 
       VIM               i10, i4, MPO_U_DC_CoeffStore                                                    // UBuffer CirBuf [ 1484] 
       VIM               i12, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_U                                  // UAddress CirBuf [ 1485] 
       VMOVW             vr03, 0, 252                                                                        // CoeffLane1 [ 1486] 
 
    
    


    // Inverse Transform
    // stage 0
       VADDSUW           vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1494] 
    
       VMOVW             vr04, 0, 252                                                                        // CoeffLane2 [ 1496] 
    // ChromaDCLane0 = (d0 + d2),(d1+d3)
    // ChromaDCLane1 = (d0 - d2) ,(d1-d3)

       VEXCH1            vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1500] 
       VMOVW             vr05, 0, 252                                                                        // CoeffLane3 [ 1501] 
    
       VLD32             vr04,[i10, 0]                                                               // CoeffLane2 UBuffer [ 1503] 
    
    // ChromaDCLane0 = (d0 + d2),(d0 - d2)
    // ChromaDCLane1 = (d1+d3) ,(d1-d3)
    // stage 1
       VADDSUW           vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1508] 
    // ChromaDCLane0 = ((d0 + d2)+ (d1+d3)),((d0 - d2) + (d1-d3))
    // ChromaDCLane1 = ((d0 + d2) - (d1+d3)) ,((d0 - d2) - (d1-d3))
       VLD32             vr05,[i10, 4]                                                               // CoeffLane3 UBuffer [ 1511] 
       VEXCH1            vr02, vr03                                                               // CoeffLane0 CoeffLane1 [ 1512] 
    // ChromaDCLane0 = ((d0 + d2)+ (d1+d3)),((d0 + d2) - (d1+d3))
    // ChromaDCLane1 = ((d0 - d2) + (d1-d3)) ,((d0 - d2) - (d1-d3))
    
    ///////////////////////////////////////////////////////////////////////////   

    // Update the NonZeroCoeff flags
       VIM               i1, i4, MPO_NonZeroCoeffChroma                                                // BuffStart CirBuf [ 1519] 
       VMOVW             vr02, 0, 252                                                                        // CoeffLane0 [ 1520] 
    
       VGMW              i7, vr03                                                               // NonZeroCoeff CoeffLane1 [ 1522] 
       VLD8W_7           vr08,[i1, 0]                                                         // NonZeroCoeffVal BuffStart [ 1523] 
       VADDSUW           vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1524] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1525] 
       VEXCH1            vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1526] 
       VMULW.128         vr07, vr07, 4                                                // NonZeroCoeffFlag NonZeroCoeffFlag [ 1527] 
    
       VGMW              i7, vr02                                                               // NonZeroCoeff CoeffLane0 [ 1529] 
       VADDSUW           vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1530] 
       VST16_0           vr02,[i13, 0]                                                              // CoeffLane0 VAddress [ 1531] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1532] 
 
    
    

       VEXCH1            vr04, vr05                                                               // CoeffLane2 CoeffLane3 [ 1537] 
    
       VST16_1           vr02,[i13, (4*2)]                                                          // CoeffLane0 VAddress [ 1539] 
       VST16_0           vr03,[i13, (64*2)]                                                         // CoeffLane1 VAddress [ 1540] 

    // ChromaDCLane0 = ((d0 + d2)+ (d1+d3)),((d0 + d2) - (d1+d3))
    // ChromaDCLane1 = ((d0 - d2) + (d1-d3)) ,((d0 - d2) - (d1-d3))
    

    ///////////////////////////////////////////////////////////////////////////   

 
    
       VGMW              i7, vr04                                                               // NonZeroCoeff CoeffLane2 [ 1550] 
       VST16_1           vr03,[i13, (68*2)]                                                         // CoeffLane1 VAddress [ 1551] 
       VST16_0           vr04,[i12, 0]                                                              // CoeffLane2 UAddress [ 1552] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1553] 
       VST16_1           vr04,[i12, (4*2)]                                                          // CoeffLane2 UAddress [ 1554] 
       VMULW.128         vr07, vr07, 4                                                // NonZeroCoeffFlag NonZeroCoeffFlag [ 1555] 
    
       VGMW              i7, vr05                                                               // NonZeroCoeff CoeffLane3 [ 1557] 
       VST16_0           vr05,[i12, (64*2)]                                                         // CoeffLane3 UAddress [ 1558] 
       VST16_1           vr05,[i12, (68*2)]                                                         // CoeffLane3 UAddress [ 1559] 
       VOR.128           vr07, vr07, vr00                                // NonZeroCoeffFlag NonZeroCoeffFlag NonZeroCoeff [ 1560] 
    
 // ********************************** END OF UV DC IQ
    
     // Update the NonZeroCoeff flags
       VMULW.128         vr07, vr07, 4                                                // NonZeroCoeffFlag NonZeroCoeffFlag [ 1565] 
       VMOVW             vr00, CHROMA_DIV, 4                                                                    // Address [ 1566] 
       VMOVW             vr01, TRANSFORM_U, 2                                                             // CoefWriteAddr [ 1567] 
       VLD16_3           vr00,[i2, 0]                                                                 // DivFactor Address [ 1568] 
    // Or the final NonZeroCoeff
       VOR.128           vr00, vr08, vr07                                 // NonZeroCoeff NonZeroCoeffVal NonZeroCoeffFlag [ 1570] 
    
    
    

    // Chroma Inverse quant and transform  

    // Load Chroma Divide factor
       VMOVW             vr00, CHROMA_INVERSE_QUANT_TABLE, 4                                                    // Address [ 1578] 
       VMOVW             vr19, ((3*32)+16), 255                                                                  // Offset [ 1579] 



    // Load CHROMA QUANT factor
       VLD16_2           vr00,[i2, 0]                                                                   // Address Address [ 1584] 
    
       VMIVW             vr10, i3                                                            // ShiftValueWithDC DivFactor [ 1586] 
       VMIVW             vr09, i3                                                                  // ShiftValue DivFactor [ 1587] 
    
    // For DC component the DivFactor has to be reduced by 1
       VSUBW.17          vr10, vr10, 1                                                // ShiftValueWithDC ShiftValueWithDC [ 1590] 
    
       VLD64             vr12,[i2, 0]                                                                // QuantLane0 Address [ 1592] 
       VLD64             vr13,[i2, 8]                                                                // QuantLane1 Address [ 1593] 
       VLD64             vr14,[i2, 16]                                                               // QuantLane2 Address [ 1594] 
       VLD64             vr15,[i2, 24]                                                               // QuantLane3 Address [ 1595] 
    
       VMR4W.240         vr12, vr12, vr12                                              // QuantLane0 QuantLane0 QuantLane0 [ 1597] 
 	
 	
  ld r1,[r17,40];                                                             // TargetAddress InverseQuantTransformCoeffs [ 1600] 
       VMOVW             vr06, r1, 3                                                                                    // [ 1600] 
       VMR4W.240         vr15, vr15, vr15                                              // QuantLane3 QuantLane3 QuantLane3 [ 1601] 
       VADDW.1           vr06, vr06, 2                                                      // TargetAddress TargetAddress [ 1602] 
       VMOVW             vr06, r3, 4                                                    // TargetAddress InverseTransformV [ 1603] 
       VJB               vr06, 0                                                                          // TargetAddress [ 1604] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_U  //DS______________________________ BuffStart CirBuf [ 1605] 
       VMR4W.240         vr13, vr13, vr13  //DS__________________________________________ QuantLane1 QuantLane1 QuantLane1 [ 1606] 
       VMR4W.240         vr14, vr14, vr14  //DS__________________________________________ QuantLane2 QuantLane2 QuantLane2 [ 1607] 


//InverseTransformV --------                                                                                            // [ 1610] 
 vendrec r3                                                                                                             // [ 1610] 
 vrec r3                                                                                                                // [ 1610] 
  ld r1,[r17,40];                                                             // TargetAddress InverseQuantTransformCoeffs [ 1611] 
LINE 1611".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VMOVW             vr06, r1, 3                                                                                    // [ 1611] 
       VMOVW             vr01, TRANSFORM_V, 2                                                             // CoefWriteAddr [ 1612] 
       VADDW.1           vr06, vr06, 2                                                      // TargetAddress TargetAddress [ 1613] 
       VMOVW             vr06, r4, 4                                           // TargetAddress InverseTransformChromaExit [ 1614] 
       VJB               vr06, 0                                                                          // TargetAddress [ 1615] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_TRANSFORM_V  //DS______________________________ BuffStart CirBuf [ 1616] 
       VNOP                //DS___________________________________________________________________________________________ [ 1617] 
       VNOP                //DS___________________________________________________________________________________________ [ 1618] 

        
//InverseTransformChromaExit --------                                                                                   // [ 1621] 
 vendrec r4                                                                                                             // [ 1621] 
 vrec r4                                                                                                                // [ 1621] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz InverseTransformDequantChromaCoeffs_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InverseTransformDequantChromaCoeffs                                                                          // [ 1623] 




////////////////////////////////////////////////////////////////////////////////////


// End                                                                                                                  // [ 1629] // Global variables


// [Macro] CallBackDoFwdTransforms                                                                                      // [ 1632] 
_Asm int Record_CallBackDoFwdTransforms(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,52]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                                    // [ 1634] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 1640".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [ 1640] 
   mov                 r1, Service_DoFwdTransforms   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [ 1642] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoFwdTransforms                                                                                      // [ 1643] 




// [Macro] CallBackDoEntropyEncode                                                                                      // [ 1646] 
_Asm int Record_CallBackDoEntropyEncode(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,56]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                                    // [ 1648] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 1655".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [ 1655] 
   mov                 r1, Service_WriteMacroblock   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [ 1657] 


  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoEntropyEncode                                                                                      // [ 1660] 




// [Macro] CallBackDoInverseTransforms                                                                                  // [ 1663] 
_Asm int Record_CallBackDoInverseTransforms(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,60]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                                    // [ 1665] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 1672".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [ 1672] 
   mov                 r1, Service_InverseTransforms   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [ 1674] 


  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoInverseTransforms                                                                                  // [ 1677] 






// [Macro] RevertToSkipped                                                                                              // [ 1682] 
_Asm int Record_RevertToSkipped(int *mac_tab)
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
RevertToSkipped_RecLoop:
  ld r1,[r17 ,64]
  vrec r1

    // Params -----------
// p16    i0  LastTransQuant                                                                                            // [ 1684] 
    // End params -------


// p16    i1  type                                                                                                      // [ 1689] 
// p16    i2  nzl                                                                                                       // [ 1689] 
// s16    vr02.4  nzc                                                                                                   // [ 1690] 
// s16    vr03.4  nzo                                                                                                   // [ 1690] 
// s16    vr02.1  skp                                                                                                   // [ 1691] 
//vec16  vr04  mv                                                                                                       // [ 1692] 
//vec16  vr05  mvs                                                                                                      // [ 1692] 
//vec16  vr06  mvsIn                                                                                                    // [ 1692] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|              |       CirLeft|        FixBuf|        CirBuf|              |           nzl|          type|LastTransQuant|
//|............................................................................................................................|
//|VR 2|              |              |              |              |              |           nzc|              |           skp|
//|VR 3|              |              |              |              |              |           nzo|              |              |
//|VR 4|                                                          mv                                                           |
//|VR 5|                                                         mvs                                                           |
//|VR 6|                                                       mvsIn                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

//       if (cb.MBCodingType & MBCT_H264_INTER &&
LINE 1695".\ARCEncoder\H264\ArcTransformQuant_MPA.sss"
       VLD8W_1           vr00,[i4, MPO_MBCodingType]                                                        // type CirBuf [ 1695] 
       VIA               i1, i1, MBCT_H264_INTER                                                              // type type [ 1696] 
       VJP!i1            r3                                                                                   // type exit [ 1697] 
//           !(cb.NonZeroCoeffLuma || cb.NonZeroCoeffChroma || cb.UV_DC_NonZeroCoeff))
       VLD16_2.i1        vr00,[i4, MPO_NonZeroCoeffLuma]  //DS____________________________________________ type nzl CirBuf [ 1699] 
       VLD8W_2.i1        vr02,[i4, MPO_NonZeroCoeffChroma]  //DS__________________________________________ type nzc CirBuf [ 1700] 
       VLD8W_2.i1        vr03,[i4, MPO_UV_DC_NonZeroCoeff]  //DS__________________________________________ type nzo CirBuf [ 1701] 
       VOR.4             vr00, vr00, vr02                                                                   // nzl nzl nzc [ 1702] 
       VLD64W            vr04,[i4, MPO_mv]                                                                    // mv CirBuf [ 1703] 
       VOR.4             vr00, vr00, vr03                                                                   // nzl nzl nzo [ 1704] 
       VLD32WL           vr06,[i4, MPO_WorkArea_MvSkip]                                                    // mvsIn CirBuf [ 1705] 
       VJP.i2            r3                                                                                    // nzl exit [ 1706] 
       VXSUMW.85         vr05, vr06, 1  //DS____________________________________________________________________ mvs mvsIn [ 1707] // broadcast to odd
       VXSUMW.170        vr05, vr06, 2  //DS____________________________________________________________________ mvs mvsIn [ 1708] // broadcast to even
//
//           if (cb.mv[0].mvx == cb.WorkArea.MvSkip.mvx &&
//             cb.mv[0].mvy == cb.WorkArea.MvSkip.mvy &&
//             cb.mv[1].mvx == cb.WorkArea.MvSkip.mvx &&
//             cb.mv[1].mvy == cb.WorkArea.MvSkip.mvy &&
//             cb.mv[2].mvx == cb.WorkArea.MvSkip.mvx &&
//             cb.mv[2].mvy == cb.WorkArea.MvSkip.mvy &&
//             cb.mv[3].mvx == cb.WorkArea.MvSkip.mvx &&
//             cb.mv[3].mvy == cb.WorkArea.MvSkip.mvy)
       VSUBW.F           vr62, vr04, vr05  //DS______________________________________________________________ VZERO mv mvs [ 1718] 
       VTALL.Z           i2, 255                                                                                    // nzl [ 1719] 
//                       // Not transmitting the quant so we have to use the old one for the loop filter
//                       // as that's what the decoder will do
//                       cb.QuantValue = m_LastTransmittedQuant[cb.WorkArea.SliceGroup];
//                       cb.MBCodingType = MBCT_H264_SKIPPED;
       VMOVW             vr02, MBCT_H264_SKIPPED, 1                                                                 // skp [ 1724] 
       VST8.i2           vr00,[i4, MPO_QuantValue]                                            // nzl LastTransQuant CirBuf [ 1725] 
       VST8.i2           vr02,[i4, MPO_MBCodingType]                                                     // nzl skp CirBuf [ 1726] 

//exit --------                                                                                                         // [ 1728] 
 vendrec r3                                                                                                             // [ 1728] 
 vrec r3                                                                                                                // [ 1728] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz RevertToSkipped_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc RevertToSkipped                                                                                              // [ 1730] 






const int MACRO_IDX_SetupQuantValues = 0;
const int MACRO_IDX_TransformQuantiseLumaDC = 1;
const int SIMD_IDX_TransformQuantiseUVDC = 2;
const int MACRO_IDX_TransformQuantiseChromaDC = 3;
const int SIMD_IDX_Forward4x4TransformQuantise = 4;
const int SIMD_IDX_ZeroBlocks = 5;
const int MACRO_IDX_Forward4x4LumaTransformAndSmallCoeff = 6;
const int MACRO_IDX_Forward4x4ChromaTransformAndSmallCoeff = 7;
const int SIMD_IDX_InverseTransformLumaDC = 8;
const int SIMD_IDX_Inverse4x4CoeffTransform = 9;
const int MACRO_IDX_InverseQuantTransformCoeffs = 10;
const int MACRO_IDX_InverseTransformDequantLumaCoeffs = 11;
const int MACRO_IDX_InverseTransformDequantChromaCoeffs = 12;
const int MACRO_IDX_CallBackDoFwdTransforms = 13;
const int MACRO_IDX_CallBackDoEntropyEncode = 14;
const int MACRO_IDX_CallBackDoInverseTransforms = 15;
const int MACRO_IDX_RevertToSkipped = 16;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Transform_Buffers_Record(int sa, int SIMDnum)
{
    int *map = Transform_Buffers;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_SetupQuantValues] = sa;
    sa = Record_SetupQuantValues(map);

    map[MACRO_IDX_TransformQuantiseLumaDC] = sa;
    sa = Record_TransformQuantiseLumaDC(map);

    map[SIMD_IDX_TransformQuantiseUVDC] = sa;
    sa = Record_TransformQuantiseUVDC(map);

    map[MACRO_IDX_TransformQuantiseChromaDC] = sa;
    sa = Record_TransformQuantiseChromaDC(map);

    map[SIMD_IDX_Forward4x4TransformQuantise] = sa;
    sa = Record_Forward4x4TransformQuantise(map);

    map[SIMD_IDX_ZeroBlocks] = sa;
    sa = Record_ZeroBlocks(map);

    map[MACRO_IDX_Forward4x4LumaTransformAndSmallCoeff] = sa;
    sa = Record_Forward4x4LumaTransformAndSmallCoeff(map);

    map[MACRO_IDX_Forward4x4ChromaTransformAndSmallCoeff] = sa;
    sa = Record_Forward4x4ChromaTransformAndSmallCoeff(map);

    map[SIMD_IDX_InverseTransformLumaDC] = sa;
    sa = Record_InverseTransformLumaDC(map);

    map[SIMD_IDX_Inverse4x4CoeffTransform] = sa;
    sa = Record_Inverse4x4CoeffTransform(map);

    map[MACRO_IDX_InverseQuantTransformCoeffs] = sa;
    sa = Record_InverseQuantTransformCoeffs(map);

    map[MACRO_IDX_InverseTransformDequantLumaCoeffs] = sa;
    sa = Record_InverseTransformDequantLumaCoeffs(map);

    map[MACRO_IDX_InverseTransformDequantChromaCoeffs] = sa;
    sa = Record_InverseTransformDequantChromaCoeffs(map);

    map[MACRO_IDX_CallBackDoFwdTransforms] = sa;
    sa = Record_CallBackDoFwdTransforms(map);

    map[MACRO_IDX_CallBackDoEntropyEncode] = sa;
    sa = Record_CallBackDoEntropyEncode(map);

    map[MACRO_IDX_CallBackDoInverseTransforms] = sa;
    sa = Record_CallBackDoInverseTransforms(map);

    map[MACRO_IDX_RevertToSkipped] = sa;
    sa = Record_RevertToSkipped(map);

    return sa;
}
