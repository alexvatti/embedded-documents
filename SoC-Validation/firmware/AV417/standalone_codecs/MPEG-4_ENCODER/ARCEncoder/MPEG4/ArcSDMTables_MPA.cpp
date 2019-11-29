//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss'

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


//	showstalls
// MetaWare ouput ON                                                                                                    // [   20] 
#define LINE #line                                                                                                      // [   20] 
#include ".\ARCEncoder\MPEG4\ArcSDMTables_MPA.h"


// Macro table
    int SDM_Buffers[5];
    int SDM_Buffers_Record(int sa, int SIMDnum);

// ScanMP: Including file .\ARCEncoder\MPEG4\..\ARC\SIMD_ABI.ii.
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
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"


//----------------------------------------------------------------------------------------------------
// Is Macro
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft
// [Macro] SetCurrentMPC                                                                                                // [   37] 
_Asm int Record_SetCurrentMPC(int *mac_tab)
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
// p16    i0  cIdx                                                                                                      // [   39] // Circular buffer index
// p16    i1  Column                                                                                                    // [   40] // MacroBlock column
// p16    i2  leftIdx                                                                                                   // [   41] // Circular buffer index of macro block to left
	// End params -------
// Setup pointers
LINE 44".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VMULW.5           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY                           // cIdx'leftIdx cIdx'leftIdx [   44] 
       VMULW.2           vr00, vr00, MPO_SIZE_OF_FIXED_ENTRY                                              // Column Column [   45] 
       VIM               i4, i0, SDMOF_CircularBuffs                                                        // CirBuf cIdx [   46] 
       VIM               i5, i1, SDMOF_FixedBuffs                                                         // FixBuf Column [   47] 
       VIM               i6, i2, SDMOF_CircularBuffs                                                    // CirLeft leftIdx [   48] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetCurrentMPC                                                                                                // [   49] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Init ciruclar buffer entry
// Assumes current MPC is set
// Sends channel reply
// [Macro] InitCurrentMPC                                                                                               // [   57] 
_Asm int Record_InitCurrentMPC(int *mac_tab)
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

	// No params ------
//	vst8				VZERO,[CirBuf,MPO_QuantValue]
//	vst8				VZERO,[CirBuf,MPO_MBCodingType]		// MBCT_H264_UNKNOWN
LINE 61".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VST16             vr62,[i4, MPO_NonZeroCoeffLuma]                                                   // VZERO CirBuf [   61] 
       VST8              vr62,[i4, MPO_NonZeroCoeffChroma]                                                 // VZERO CirBuf [   62] 
       VST8              vr62,[i4, MPO_WorkArea_BestIntraChromaMode]                                       // VZERO CirBuf [   63] 
       VST128            vr62,[i4, MPO_CoeffCountSB]                                                       // VZERO CirBuf [   64] 
       VST8              vr62,[i4, MPO_UV_DC_NonZeroCoeff]                                                 // VZERO CirBuf [   65] 
// Begin                                                                                                                // [   66] 
// s16    vr02.1  m1                                                                                                    // [   67] 
LINE 68".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VMOVW             vr02, -1, 1                                                                                 // m1 [   68] 
       VST16             vr02,[i4, MPO_WorkArea_BestIntraLumaCost]                                            // m1 CirBuf [   69] 
// End                                                                                                                  // [   70] 
LINE 71".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VST8              vr62,[i4, MPO_WorkArea_BestIntraLumaMode]                                         // VZERO CirBuf [   71] 
       VST8              vr62,[i4, MPO_WorkArea_BestIntraChromaMode]                                       // VZERO CirBuf [   72] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InitCurrentMPC                                                                                               // [   74] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Sets the quantization values for each macroblock
// Takes sliceType, chromaQpIndexOffset as the parameters

// [Macro] SetQuantValue                                                                                                // [   82] 
_Asm int Record_SetQuantValue(int *mac_tab)
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
// p16    i0  cIdx                                                                                                      // [   84] // Circular buffer index
// p16    i1  leftIdx                                                                                                   // [   85] // Circular buffer index of macro block to left
// s16    vr02.1  quantVal                                                                                              // [   86] // Quantisation Parameter
    // End params -------
// Setup pointers
LINE 89".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VMULW.3           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY                           // cIdx'leftIdx cIdx'leftIdx [   89] 
       VIM               i4, i0, SDMOF_CircularBuffs                                                        // CirBuf cIdx [   90] 

    // Store Quant value
       VST8              vr02,[i4, MPO_QuantValue]                                                      // quantVal CirBuf [   93] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetQuantValue                                                                                                // [   94] 



//----------------------------------------------------------------------------------------------------
// Is Macro
// Sets the quantization values for each macroblock
// Takes sliceType, chromaQpIndexOffset as the parameters

// [Macro] RevertQuantValue                                                                                             // [  101] 
_Asm int Record_RevertQuantValue(int *mac_tab)
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
// p16    i0  cIdx                                                                                                      // [  103] // Circular buffer index
// p16    i1  leftIdx                                                                                                   // [  104] // Circular buffer index of macro block to left
// s16    vr02.1  dquant                                                                                                // [  105] // Quantisation Parameter differential value
    // End params -------
// Begin                                                                                                                // [  107] 
// s16    vr03.1  quantVal                                                                                              // [  108] // Quantisation Parameter
    
// Setup pointers
LINE 111".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VMULW.3           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY                           // cIdx'leftIdx cIdx'leftIdx [  111] 
       VIM               i4, i0, SDMOF_CircularBuffs                                                        // CirBuf cIdx [  112] 

    // Store Quant value
       VLD8W             vr03,[i4, MPO_QuantValue]                                                      // quantVal CirBuf [  115] 
       VSUBW.1           vr03, vr03, vr02                                                      // quantVal quantVal dquant [  116] 
       VST8              vr03,[i4, MPO_QuantValue]                                                      // quantVal CirBuf [  117] 
// End                                                                                                                  // [  118] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc RevertQuantValue                                                                                             // [  119] 



//------------------------------------------------------------------------------------------------------
// Is Macro

// [Macro] CopyToFixedAfterReconstructMP4                                                                               // [  124] 
_Asm int Record_CopyToFixedAfterReconstructMP4(int *mac_tab)
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

// Begin                                                                                                                // [  125] 
//vec16  vr02  CtrlParams                                                                                               // [  126] 
//vec16  vr03  MotionVecs                                                                                               // [  127] 
//vec16  vr04  YIntraPredDC                                                                                             // [  128] 
//vec16  vr05  UVIntraPredDC                                                                                            // [  129] 

LINE 131".\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss"
       VLD128            vr02,[i4, MPO_LeftMB]                                                        // CtrlParams CirBuf [  131] 
       VLD128            vr03,[i4, MPO_mv]                                                            // MotionVecs CirBuf [  132] 
       VLD64             vr04,[i4, MPO_Y_IntraPredDC_Coeff]                                         // YIntraPredDC CirBuf [  133] 
       VLD32             vr05,[i4, MPO_UV_IntraPredDC_Coeff]                                       // UVIntraPredDC CirBuf [  134] 
	
       VST128            vr02,[i5, MPO_LeftMB]                                                        // CtrlParams FixBuf [  136] 
       VST128            vr03,[i5, MPO_mv]                                                            // MotionVecs FixBuf [  137] 
       VST64             vr04,[i5, MPO_Y_IntraPredDC_Coeff]                                         // YIntraPredDC FixBuf [  138] 
       VST32             vr05,[i5, MPO_UV_IntraPredDC_Coeff]                                       // UVIntraPredDC FixBuf [  139] 
// End                                                                                                                  // [  140] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyToFixedAfterReconstructMP4                                                                               // [  141] 





const int MACRO_IDX_SetCurrentMPC = 0;
const int MACRO_IDX_InitCurrentMPC = 1;
const int MACRO_IDX_SetQuantValue = 2;
const int MACRO_IDX_RevertQuantValue = 3;
const int MACRO_IDX_CopyToFixedAfterReconstructMP4 = 4;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int SDM_Buffers_Record(int sa, int SIMDnum)
{
    int *map = SDM_Buffers;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_SetCurrentMPC] = sa;
    sa = Record_SetCurrentMPC(map);

    map[MACRO_IDX_InitCurrentMPC] = sa;
    sa = Record_InitCurrentMPC(map);

    map[MACRO_IDX_SetQuantValue] = sa;
    sa = Record_SetQuantValue(map);

    map[MACRO_IDX_RevertQuantValue] = sa;
    sa = Record_RevertQuantValue(map);

    map[MACRO_IDX_CopyToFixedAfterReconstructMP4] = sa;
    sa = Record_CopyToFixedAfterReconstructMP4(map);

    return sa;
}
