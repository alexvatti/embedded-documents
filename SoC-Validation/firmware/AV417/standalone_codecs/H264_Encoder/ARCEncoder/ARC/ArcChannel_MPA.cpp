//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcChannel_MPA.sss'

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

//------------------------------------------------------------------------------
// File:        ArcChannel_MPA.sss
// Description: 
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// ScanMP settings
// Stall info ON                                                      // [   25] 
// Stall info format: Stall [Reg in Stall] ...... List of Registers currently busy  

// MetaWare ouput ON                                                                                        // [   27] 
#define LINE #line                                                                                          // [   27] 

#include ".\ARCEncoder\ARC\ArcChannel_MPA.h"


// Macro table
    int ArcChannel[3];
    int ArcChannel_Record(int sa, int SIMDnum);



//------------------------------------------------------------------------------
// Includes
#include "ArcMacroRecordSettings.h"



//------------------------------------------------------------------------------
// Macros

//------------------------------------------------------------------------------
// Name:      Generate_ME_SDMResponse
// Purpose:   Generate a fake ME channel response for the SDM (first) channel
//            response
// Arguments: intraValidFlag = 2 bit flag indicating which neighbour MBs are
//            intra
//            skipMv = skipped motion components
// Returns:   void
//------------------------------------------------------------------------------
// [Macro] Generate_ME_SDMResponse                                                                          // [   52] 
_Asm int Record_Generate_ME_SDMResponse(int *mac_tab)
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
// p16    i0  intraValidFlag                                                                                // [   54] 
// p16    i1  skipMv                                                                                        // [   56] 

// Begin                                                                                                    // [   59] 

   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]

    // send channel commands
LINE 65".\ARCEncoder\ARC\ArcChannel_MPA.sss"
       VSEND             r0, i0, 0                                                                          // [   65] 
       VSEND             r0, i1, 1                                                                          // [   66] 

   mov                 r1, Service_ME_SDMResponse 
       VSENDR            r0, r1, 63                                                                         // [   69] ....................// channel number not used at present

// End                                                                                                      // [   71] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Generate_ME_SDMResponse                                                                          // [   73] 




//------------------------------------------------------------------------------
// Name:      Generate_ME_SkipResponse
// Purpose:   Generate a fake ME channel response for the skip (second) channel
//            response
// Arguments: cost = skipped cost (sad)
//            blkPattern = block pattern flag
// Returns:   void
//------------------------------------------------------------------------------
// [Macro] Generate_ME_SkipResponse                                                                         // [   84] 
_Asm int Record_Generate_ME_SkipResponse(int *mac_tab)
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
// p16    i0  cost                                                                                          // [   86] 
// p16    i1  blkPattern                                                                                    // [   88] 

// Begin                                                                                                    // [   91] 

   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]

    // send channel commands
LINE 97".\ARCEncoder\ARC\ArcChannel_MPA.sss"
       VSEND             r0, i0, 0                                                                          // [   97] 
       VSEND             r0, i1, 1                                                                          // [   98] 

   mov                 r1, Service_ME_SkipResponse
       VSENDR            r0, r1, 63                                                                         // [  101] ....................// channel number not used at present

// End                                                                                                      // [  103] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Generate_ME_SkipResponse                                                                         // [  105] 




//------------------------------------------------------------------------------
// Name:      Generate_ME_InterResponse
// Purpose:   Generate a fake ME channel response for the inter (second) channel
//            response
// Arguments: cost = inter cost
//            blkPattern = block pattern flag
// Returns:   void
//------------------------------------------------------------------------------
// [Macro] Generate_ME_InterResponse                                                                        // [  116] 
_Asm int Record_Generate_ME_InterResponse(int *mac_tab)
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
// p16    i0  cost                                                                                          // [  118] 
// p16    i1  blkPattern                                                                                    // [  120] 

// Begin                                                                                                    // [  123] 

   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]

    // send channel commands
LINE 129".\ARCEncoder\ARC\ArcChannel_MPA.sss"
       VSEND             r0, i0, 0                                                                          // [  129] 
       VSEND             r0, i1, 1                                                                          // [  130] 

   mov                 r1, Service_ME_InterResponse
       VSENDR            r0, r1, 63                                                                         // [  133] ....................// channel number not used at present

// End                                                                                                      // [  135] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Generate_ME_InterResponse                                                                        // [  137] 




const int MACRO_IDX_Generate_ME_SDMResponse = 0;
const int MACRO_IDX_Generate_ME_SkipResponse = 1;
const int MACRO_IDX_Generate_ME_InterResponse = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int ArcChannel_Record(int sa, int SIMDnum)
{
    int *map = ArcChannel;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_Generate_ME_SDMResponse] = sa;
    sa = Record_Generate_ME_SDMResponse(map);

    map[MACRO_IDX_Generate_ME_SkipResponse] = sa;
    sa = Record_Generate_ME_SkipResponse(map);

    map[MACRO_IDX_Generate_ME_InterResponse] = sa;
    sa = Record_Generate_ME_InterResponse(map);

    return sa;
}
