//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcSDMTables_MPA.sss'

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


//    showstalls
// MetaWare ouput ON                                                                                                    // [   20] 
#define LINE #line                                                                                                      // [   20] 
#include ".\ARCEncoder\H264\ArcSDMTables_MPA.h"


// Macro table
    int SDM_Buffers[8];
    int SDM_Buffers_Record(int sa, int SIMDnum);

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
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft
// [Macro] SetCurrentMPC0                                                                                               // [   38] 
_Asm int Record_SetCurrentMPC0(int *mac_tab)
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
// p16    i0  cIdx                                                                                                      // [   40] // Circular buffer index
// p16    i1  Column                                                                                                    // [   41] // MacroBlock column
// p16    i2  leftIdx                                                                                                   // [   42] // Circular buffer index of macro block to left
    // End params -------
// Setup pointers
LINE 46".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VMULW.5           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY                           // cIdx'leftIdx cIdx'leftIdx [   46] 
       VMULW.2           vr00, vr00, MPO_SIZE_OF_FIXED_ENTRY                                              // Column Column [   47] 
       VIM               i4, i0, SDMOF_CircularBuffs                                                        // CirBuf cIdx [   48] 
       VIM               i5, i1, SDMOF_FixedBuffs                                                         // FixBuf Column [   49] 
       VIM               i6, i2, SDMOF_CircularBuffs                                                    // CirLeft leftIdx [   50] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetCurrentMPC0                                                                                               // [   51] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Init ciruclar buffer entry
// Assumes current MPC is set
// Sends channel reply
// [Macro] InitCurrentMPC                                                                                               // [   59] 
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
//  vst8                VZERO,[CirBuf,MPO_QuantValue]
//  vst8                VZERO,[CirBuf,MPO_MBCodingType]     // MBCT_H264_UNKNOWN
LINE 63".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VST16             vr62,[i4, MPO_NonZeroCoeffLuma]                                                   // VZERO CirBuf [   63] 
       VST8              vr62,[i4, MPO_NonZeroCoeffChroma]                                                 // VZERO CirBuf [   64] 
       VST8              vr62,[i4, MPO_WorkArea_BestIntraChromaMode]                                       // VZERO CirBuf [   65] 
       VST128            vr62,[i4, MPO_CoeffCountSB]                                                       // VZERO CirBuf [   66] 
       VST8              vr62,[i4, MPO_UV_DC_NonZeroCoeff]                                                 // VZERO CirBuf [   67] 
// Begin                                                                                                                // [   68] 
// s16    vr02.1  m1                                                                                                    // [   69] 
LINE 70".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VMOVW             vr02, -1, 1                                                                                 // m1 [   70] 
       VST16             vr02,[i4, MPO_WorkArea_BestIntraLumaCost]                                            // m1 CirBuf [   71] 
// End                                                                                                                  // [   72] 
LINE 73".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VST8              vr62,[i4, MPO_WorkArea_BestIntraLumaMode]                                         // VZERO CirBuf [   73] 
       VST8              vr62,[i4, MPO_WorkArea_BestIntraChromaMode]                                       // VZERO CirBuf [   74] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc InitCurrentMPC                                                                                               // [   76] 






//----------------------------------------------------------------------------------------------------
// Is Macro
// SIMD memory copy. Must be in multiples of 64 Bytes (512 bits = 4 vectors)
//debuginfo
// [Macro] MemCopy64Byte                                                                                                // [   85] 
_Asm int Record_MemCopy64Byte(int *mac_tab)
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
// p16    i0  From                                                                                                      // [   87] // Source ptr
// p16    i1  To                                                                                                        // [   88] // Dest Prr
// p16    i2  Size                                                                                                      // [   89] // Length in 4 vectors (must be greater than zero)
    // End params -------

// Begin                                                                                                                // [   93] 
//vec16  vr02  data0                                                                                                    // [   94] 
//vec16  vr03  data1                                                                                                    // [   95] 
//vec16  vr04  data2                                                                                                    // [   96] 
//vec16  vr05  data3                                                                                                    // [   97] 

    // adjust loop counter for vjd instruction
LINE 100".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VIM               i2, i2, -1                                                                           // Size Size [  100] 

//loop --------                                                                                                         // [  102] 
 vendrec r3                                                                                                             // [  102] 
 vrec r3                                                                                                                // [  102] 
    // load data from source
LINE 104".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VLD128            vr02,[i0, 0]                                                                        // data0 From [  104] 
       VLD128            vr03,[i0, 16]                                                                       // data1 From [  105] 
       VLD128            vr04,[i0, 32]                                                                       // data2 From [  106] 
       VLD128            vr05,[i0, 48]                                                                       // data3 From [  107] 

    // store data at destination
       VST128            vr02,[i1, 0]                                                                          // data0 To [  110] 
       VST128            vr03,[i1, 16]                                                                         // data1 To [  111] 

    // decrement and loop
       VJD.i2            i2, r3                                                                          // Size Size loop [  114] 

    // delay slot start

    // store data at destination
       VST128            vr04,[i1, 32]  //DS_____________________________________________________________________ data2 To [  119] 
       VST128            vr05,[i1, 48]  //DS_____________________________________________________________________ data3 To [  120] 

    // increment pointers
       VADDW.3           vr00, vr00, 64  //DS_____________________________________________________________ From'To From'To [  123] 

    // delay slot end


// End                                                                                                                  // [  128] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc MemCopy64Byte                                                                                                // [  130] 





// [Macro] CallBackStartME                                                                                              // [  134] 
_Asm int Record_CallBackStartME(int *mac_tab)
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
// p16    i0  BufIdx                                                                                                    // [  136] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 143".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [  143] 
   mov                 r1, Service_StartME   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [  145] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackStartME                                                                                              // [  147] 





// [Macro] CallBackStartDeblock                                                                                         // [  151] 
_Asm int Record_CallBackStartDeblock(int *mac_tab)
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
// p16    i0  BufIdx                                                                                                    // [  153] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 160".\ARCEncoder\H264\ArcSDMTables_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [  160] 
   mov                 r1, Service_StartDeblock   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [  162] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackStartDeblock                                                                                         // [  164] 















const int MACRO_IDX_SetCurrentMPC0 = 0;
const int MACRO_IDX_InitCurrentMPC = 1;
const int MACRO_IDX_MemCopy64Byte = 2;
const int MACRO_IDX_CallBackStartME = 3;
const int MACRO_IDX_CallBackStartDeblock = 4;

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

    map[MACRO_IDX_SetCurrentMPC0] = sa;
    sa = Record_SetCurrentMPC0(map);

    map[MACRO_IDX_InitCurrentMPC] = sa;
    sa = Record_InitCurrentMPC(map);

    map[MACRO_IDX_MemCopy64Byte] = sa;
    sa = Record_MemCopy64Byte(map);

    map[MACRO_IDX_CallBackStartME] = sa;
    sa = Record_CallBackStartME(map);

    map[MACRO_IDX_CallBackStartDeblock] = sa;
    sa = Record_CallBackStartDeblock(map);

    return sa;
}
