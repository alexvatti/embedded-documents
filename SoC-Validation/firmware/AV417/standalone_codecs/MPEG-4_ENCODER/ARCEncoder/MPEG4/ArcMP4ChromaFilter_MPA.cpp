//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss'

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
// MetaWare ouput ON                                                                                        // [   20] 
#define LINE #line                                                                                          // [   20] 

// ScanMP: Including file .\ARCEncoder\MPEG4\..\ARC\SIMD_ABI.ii.
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

#include ".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.h"


// Macro table
    int FilterChroma[1];
    int FilterChroma_Record(int sa, int SIMDnum);



#include "ArcMPC.h"

//------------------------------------------------------------------------------
// Name:          FilterChromaPred
// Purpose:       Filters Chroma Prediction using simple bilinear interpolation
// Arguments:     predBuf: Input buffer pointer
//                outBuf : Output buffer pointer
//                horFrac: half pel precision horizontally
//                verFrac: half pel precision vertically
//                rndCtrl: Rounding control parameter
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] FilterChromaPred                                                                                 // [   40] 
_Asm int Record_FilterChromaPred(int *mac_tab)
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
FilterChromaPred_RecLoop:
  ld r1,[r17 ,0]
  vrec r1

// p16    i0  predBuf                                                                                       // [   41] 
// p16    i1  outBuf                                                                                        // [   42] 
// p16    i2  horFrac                                                                                       // [   43] 
// p16    i10  verFrac                                                                                      // [   44] 
// p16    i3  rndCtrl                                                                                       // [   45] 

// Begin                                                                                                    // [   47] 
// ****** Informational : Register space for symbol "subPel" already allocated.                             // [   48] 
// p16    i4  subPel                                                                                        // [   48] 
    
//vec16  vr02  chromaLane0                                                                                  // [   50] 
//vec16  vr03  chromaLane1                                                                                  // [   51] 
//vec16  vr04  chromaLane2                                                                                  // [   52] 
//vec16  vr05  chromaLane3                                                                                  // [   53] 
//vec16  vr06  chromaLane4                                                                                  // [   54] 
//vec16  vr07  chromaLane5                                                                                  // [   55] 
//vec16  vr08  chromaLane6                                                                                  // [   56] 
//vec16  vr09  chromaLane7                                                                                  // [   57] 
//vec16  vr10  chromaLane8                                                                                  // [   58] 
    
//vec16  vr11  chromaOffsetLane0                                                                            // [   60] 
//vec16  vr12  chromaOffsetLane1                                                                            // [   61] 
//vec16  vr13  chromaOffsetLane2                                                                            // [   62] 
//vec16  vr14  chromaOffsetLane3                                                                            // [   63] 
//vec16  vr15  chromaOffsetLane4                                                                            // [   64] 
//vec16  vr16  chromaOffsetLane5                                                                            // [   65] 
//vec16  vr17  chromaOffsetLane6                                                                            // [   66] 
//vec16  vr18  chromaOffsetLane7                                                                            // [   67] 
//vec16  vr19  chromaOffsetLane8                                                                            // [   68] 
    
    // Load data to be filtered
LINE 71".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VLD64W            vr02,[i0, 0*16]                                                // chromaLane0 predBuf [   71] 
       VLD64W            vr03,[i0, 1*16]                                                // chromaLane1 predBuf [   72] 
       VLD64W            vr04,[i0, 2*16]                                                // chromaLane2 predBuf [   73] 
       VLD64W            vr05,[i0, 3*16]                                                // chromaLane3 predBuf [   74] 
       VLD64W            vr06,[i0, 4*16]                                                // chromaLane4 predBuf [   75] 
//  vld64w              chromaLane5, [predBuf, 5*16] <--)
//  vld64w              chromaLane6, [predBuf, 6*16] <--> moved to jump delay slots below
//  vld64w              chromaLane7, [predBuf, 7*16] <--)
    
    // If none of horFrac or verFrac is set then go straight to DMA out
    // No filtering is to be performed
// s16    vr20.4  tmpFrac                                                                                   // [   82] 
LINE 83".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VOR.4             vr20, vr00, vr01                                           // tmpFrac horFrac verFrac [   83] 
        VMR6W.16        vr00, vr20, vr20                                                     // subPel tmpFrac [   84] 
LINE 85".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VJP.i4            r3                                                                 // subPel Continue [   85] 
       VLD64W            vr07,[i0, 5*16]  //DS____________________________________________ chromaLane5 predBuf [   86] 
       VLD64W            vr08,[i0, 6*16]  //DS____________________________________________ chromaLane6 predBuf [   87] 
       VLD64W            vr09,[i0, 7*16]  //DS____________________________________________ chromaLane7 predBuf [   88] 


       VASRPWB           vr02, vr02, 0                                              // chromaLane0 chromaLane0 [   91] 
       VASRPWB           vr03, vr03, 0                                              // chromaLane1 chromaLane1 [   92] 
       VASRPWB           vr04, vr04, 0                                              // chromaLane2 chromaLane2 [   93] 
       VASRPWB           vr05, vr05, 0                                              // chromaLane3 chromaLane3 [   94] 
       VASRPWB           vr06, vr06, 0                                              // chromaLane4 chromaLane4 [   95] 
       VJP               r4                                                                          // Output [   96] 
       VASRPWB           vr07, vr07, 0  //DS__________________________________________ chromaLane5 chromaLane5 [   97] 
       VASRPWB           vr08, vr08, 0  //DS__________________________________________ chromaLane6 chromaLane6 [   98] 
       VASRPWB           vr09, vr09, 0  //DS__________________________________________ chromaLane7 chromaLane7 [   99] 
    
//Continue --------                                                                                         // [  101] 
 vendrec r3                                                                                                 // [  101] 
 vrec r3                                                                                                    // [  101] 
    // If horFilter isn't set go straight to the Vertical filtering section
LINE 103".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VJP!i2            r5                                                               // horFrac VerFilter [  103] 
    
    // If only horFrac is set then perform horizontal filtering
    // Load in extra column to the right of the 8x8 block
       VLD64W            vr11,[i0, 0*16+8]  //DS____________________________________ chromaOffsetLane0 predBuf [  107] 
       VLD64W            vr12,[i0, 1*16+8]  //DS____________________________________ chromaOffsetLane1 predBuf [  108] 
       VLD64W            vr13,[i0, 2*16+8]  //DS____________________________________ chromaOffsetLane2 predBuf [  109] 
       VLD64W            vr14,[i0, 3*16+8]                                        // chromaOffsetLane3 predBuf [  110] 
       VLD64W            vr15,[i0, 4*16+8]                                        // chromaOffsetLane4 predBuf [  111] 
       VLD64W            vr16,[i0, 5*16+8]                                        // chromaOffsetLane5 predBuf [  112] 
       VLD64W            vr17,[i0, 6*16+8]                                        // chromaOffsetLane6 predBuf [  113] 
       VLD64W            vr18,[i0, 7*16+8]                                        // chromaOffsetLane7 predBuf [  114] 
    
       VMR1W             vr11, vr11, vr02                   // chromaOffsetLane0 chromaOffsetLane0 chromaLane0 [  116] 
       VMR1W             vr12, vr12, vr03                   // chromaOffsetLane1 chromaOffsetLane1 chromaLane1 [  117] 
       VMR1W             vr13, vr13, vr04                   // chromaOffsetLane2 chromaOffsetLane2 chromaLane2 [  118] 
       VMR1W             vr14, vr14, vr05                   // chromaOffsetLane3 chromaOffsetLane3 chromaLane3 [  119] 
       VMR1W             vr15, vr15, vr06                   // chromaOffsetLane4 chromaOffsetLane4 chromaLane4 [  120] 
//  vmr1w               chromaOffsetLane5, chromaOffsetLane5, chromaLane5 <--)
//  vmr1w               chromaOffsetLane6, chromaOffsetLane6, chromaLane6 <--> moved to jump delay slots below
//  vmr1w               chromaOffsetLane7, chromaOffsetLane7, chromaLane7 <--)
    
    // If verFrac is also set then go to 2D filter
    // The above loads and VMRs aren't wasted as they are also needed for
    // the 2D case anyway
       VJP.i10           r6                                                                // verFrac 2dFilter [  128] 
       VMR1W             vr16, vr16, vr07  //DS_______________ chromaOffsetLane5 chromaOffsetLane5 chromaLane5 [  129] 
       VMR1W             vr17, vr17, vr08  //DS_______________ chromaOffsetLane6 chromaOffsetLane6 chromaLane6 [  130] 
       VMR1W             vr18, vr18, vr09  //DS_______________ chromaOffsetLane7 chromaOffsetLane7 chromaLane7 [  131] 
    
    //actual horizontal filter
       VADDW             vr02, vr02, vr11                         // chromaLane0 chromaLane0 chromaOffsetLane0 [  134] 
       VADDW             vr03, vr03, vr12                         // chromaLane1 chromaLane1 chromaOffsetLane1 [  135] 
       VADDW             vr04, vr04, vr13                         // chromaLane2 chromaLane2 chromaOffsetLane2 [  136] 
       VADDW             vr05, vr05, vr14                         // chromaLane3 chromaLane3 chromaOffsetLane3 [  137] 
       VADDW             vr06, vr06, vr15                         // chromaLane4 chromaLane4 chromaOffsetLane4 [  138] 
//  vaddw               chromaLane5, chromaLane5, chromaOffsetLane5 <--)
//  vaddw               chromaLane6, chromaLane6, chromaOffsetLane6 <--> moved to jump delay slots below
//  vaddw               chromaLane7, chromaLane7, chromaOffsetLane7 <--)
        
       VJP               r7                                                                   // RoundAndShift [  143] 
       VADDW             vr07, vr07, vr16  //DS_____________________ chromaLane5 chromaLane5 chromaOffsetLane5 [  144] 
       VADDW             vr08, vr08, vr17  //DS_____________________ chromaLane6 chromaLane6 chromaOffsetLane6 [  145] 
       VADDW             vr09, vr09, vr18  //DS_____________________ chromaLane7 chromaLane7 chromaOffsetLane7 [  146] 
    
    // If only verFilter is set then load the extra lane at the bottom of the block
//VerFilter --------                                                                                        // [  149] 
 vendrec r5                                                                                                 // [  149] 
 vrec r5                                                                                                    // [  149] 
LINE 150".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VLD64W            vr10,[i0, 8*16]                                                // chromaLane8 predBuf [  150] 
    // perform actual vertical filter
       VADDW             vr02, vr02, vr03                               // chromaLane0 chromaLane0 chromaLane1 [  152] 
       VADDW             vr03, vr03, vr04                               // chromaLane1 chromaLane1 chromaLane2 [  153] 
       VADDW             vr04, vr04, vr05                               // chromaLane2 chromaLane2 chromaLane3 [  154] 
       VADDW             vr05, vr05, vr06                               // chromaLane3 chromaLane3 chromaLane4 [  155] 
       VADDW             vr06, vr06, vr07                               // chromaLane4 chromaLane4 chromaLane5 [  156] 
       VADDW             vr07, vr07, vr08                               // chromaLane5 chromaLane5 chromaLane6 [  157] 
       VADDW             vr08, vr08, vr09                               // chromaLane6 chromaLane6 chromaLane7 [  158] 
       VADDW             vr09, vr09, vr10                               // chromaLane7 chromaLane7 chromaLane8 [  159] 
    
//RoundAndShift --------                                                                                    // [  161] 
 vendrec r7                                                                                                 // [  161] 
 vrec r7                                                                                                    // [  161] 
LINE 162".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VSUBW             vr02, vr02, i3                                     // chromaLane0 chromaLane0 rndCtrl [  162] 
       VSUBW             vr03, vr03, i3                                     // chromaLane1 chromaLane1 rndCtrl [  163] 
       VSUBW             vr04, vr04, i3                                     // chromaLane2 chromaLane2 rndCtrl [  164] 
       VSUBW             vr05, vr05, i3                                     // chromaLane3 chromaLane3 rndCtrl [  165] 
       VSUBW             vr06, vr06, i3                                     // chromaLane4 chromaLane4 rndCtrl [  166] 
       VSUBW             vr07, vr07, i3                                     // chromaLane5 chromaLane5 rndCtrl [  167] 
       VSUBW             vr08, vr08, i3                                     // chromaLane6 chromaLane6 rndCtrl [  168] 
       VSUBW             vr09, vr09, i3                                     // chromaLane7 chromaLane7 rndCtrl [  169] 

       VASRRPWB          vr02, vr02, 1                                              // chromaLane0 chromaLane0 [  171] 
       VASRRPWB          vr03, vr03, 1                                              // chromaLane1 chromaLane1 [  172] 
       VASRRPWB          vr04, vr04, 1                                              // chromaLane2 chromaLane2 [  173] 
       VASRRPWB          vr05, vr05, 1                                              // chromaLane3 chromaLane3 [  174] 
       VASRRPWB          vr06, vr06, 1                                              // chromaLane4 chromaLane4 [  175] 
       VASRRPWB          vr07, vr07, 1                                              // chromaLane5 chromaLane5 [  176] 
       VASRRPWB          vr08, vr08, 1                                              // chromaLane6 chromaLane6 [  177] 
       VASRRPWB          vr09, vr09, 1                                              // chromaLane7 chromaLane7 [  178] 
    
    // Go to Output
       VJP               r4                                                                          // Output [  181] 

    
//2dFilter --------  //DS_____________________________________________________________________________________ [  184] 
 vendrec r6  //DS_____________________________________________________________________________________________ [  184] 
 vrec r6  //DS________________________________________________________________________________________________ [  184] 
LINE 185".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VLD64W            vr10,[i0, 8*16]  //DS____________________________________________ chromaLane8 predBuf [  185] 
       VLD64W            vr19,[i0, 8*16+8]  //DS____________________________________ chromaOffsetLane8 predBuf [  186] 
       VMR1W             vr19, vr19, vr10  //DS_______________ chromaOffsetLane8 chromaOffsetLane8 chromaLane8 [  187] 
    
    // 2D filtering
       VADDW             vr02, vr02, vr11                         // chromaLane0 chromaLane0 chromaOffsetLane0 [  190] 
       VADDAW            vr02, vr03, vr12                         // chromaLane0 chromaLane1 chromaOffsetLane1 [  191] 
       VADDW             vr03, vr03, vr12                         // chromaLane1 chromaLane1 chromaOffsetLane1 [  192] 
       VADDAW            vr03, vr04, vr13                         // chromaLane1 chromaLane2 chromaOffsetLane2 [  193] 
       VADDW             vr04, vr04, vr13                         // chromaLane2 chromaLane2 chromaOffsetLane2 [  194] 
       VADDAW            vr04, vr05, vr14                         // chromaLane2 chromaLane3 chromaOffsetLane3 [  195] 
       VADDW             vr05, vr05, vr14                         // chromaLane3 chromaLane3 chromaOffsetLane3 [  196] 
       VADDAW            vr05, vr06, vr15                         // chromaLane3 chromaLane4 chromaOffsetLane4 [  197] 
       VADDW             vr06, vr06, vr15                         // chromaLane4 chromaLane4 chromaOffsetLane4 [  198] 
       VADDAW            vr06, vr07, vr16                         // chromaLane4 chromaLane5 chromaOffsetLane5 [  199] 
       VADDW             vr07, vr07, vr16                         // chromaLane5 chromaLane5 chromaOffsetLane5 [  200] 
       VADDAW            vr07, vr08, vr17                         // chromaLane5 chromaLane6 chromaOffsetLane6 [  201] 
       VADDW             vr08, vr08, vr17                         // chromaLane6 chromaLane6 chromaOffsetLane6 [  202] 
       VADDAW            vr08, vr09, vr18                         // chromaLane6 chromaLane7 chromaOffsetLane7 [  203] 
       VADDW             vr09, vr09, vr18                         // chromaLane7 chromaLane7 chromaOffsetLane7 [  204] 
       VADDAW            vr09, vr10, vr19                         // chromaLane7 chromaLane8 chromaOffsetLane8 [  205] 

       VSUBW             vr02, vr02, i3                                     // chromaLane0 chromaLane0 rndCtrl [  207] 
       VSUBW             vr03, vr03, i3                                     // chromaLane1 chromaLane1 rndCtrl [  208] 
       VSUBW             vr04, vr04, i3                                     // chromaLane2 chromaLane2 rndCtrl [  209] 
       VSUBW             vr05, vr05, i3                                     // chromaLane3 chromaLane3 rndCtrl [  210] 
       VSUBW             vr06, vr06, i3                                     // chromaLane4 chromaLane4 rndCtrl [  211] 
       VSUBW             vr07, vr07, i3                                     // chromaLane5 chromaLane5 rndCtrl [  212] 
       VSUBW             vr08, vr08, i3                                     // chromaLane6 chromaLane6 rndCtrl [  213] 
       VSUBW             vr09, vr09, i3                                     // chromaLane7 chromaLane7 rndCtrl [  214] 

       VASRRPWB          vr02, vr02, 2                                              // chromaLane0 chromaLane0 [  216] 
       VASRRPWB          vr03, vr03, 2                                              // chromaLane1 chromaLane1 [  217] 
       VASRRPWB          vr04, vr04, 2                                              // chromaLane2 chromaLane2 [  218] 
       VASRRPWB          vr05, vr05, 2                                              // chromaLane3 chromaLane3 [  219] 
       VASRRPWB          vr06, vr06, 2                                              // chromaLane4 chromaLane4 [  220] 
       VASRRPWB          vr07, vr07, 2                                              // chromaLane5 chromaLane5 [  221] 
       VASRRPWB          vr08, vr08, 2                                              // chromaLane6 chromaLane6 [  222] 
       VASRRPWB          vr09, vr09, 2                                              // chromaLane7 chromaLane7 [  223] 

//Output --------                                                                                           // [  225] 
 vendrec r4                                                                                                 // [  225] 
 vrec r4                                                                                                    // [  225] 
LINE 226".\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss"
       VST64             vr02,[i1, 0*16]                                                 // chromaLane0 outBuf [  226] 
       VST64             vr03,[i1, 1*16]                                                 // chromaLane1 outBuf [  227] 
       VST64             vr04,[i1, 2*16]                                                 // chromaLane2 outBuf [  228] 
       VST64             vr05,[i1, 3*16]                                                 // chromaLane3 outBuf [  229] 
       VST64             vr06,[i1, 4*16]                                                 // chromaLane4 outBuf [  230] 
       VST64             vr07,[i1, 5*16]                                                 // chromaLane5 outBuf [  231] 
       VST64             vr08,[i1, 6*16]                                                 // chromaLane6 outBuf [  232] 
       VST64             vr09,[i1, 7*16]                                                 // chromaLane7 outBuf [  233] 

// End                                                                                                      // [  235] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FilterChromaPred_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FilterChromaPred                                                                                 // [  237] 





const int MACRO_IDX_FilterChromaPred = 0;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int FilterChroma_Record(int sa, int SIMDnum)
{
    int *map = FilterChroma;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_FilterChromaPred] = sa;
    sa = Record_FilterChromaPred(map);

    return sa;
}
