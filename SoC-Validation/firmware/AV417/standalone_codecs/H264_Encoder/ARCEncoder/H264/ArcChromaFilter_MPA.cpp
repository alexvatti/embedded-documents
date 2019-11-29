//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcChromaFilter_MPA.sss'

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
// MetaWare ouput ON                                                                                        // [   21] 
#define LINE #line                                                                                          // [   21] 

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

#include ".\ARCEncoder\H264\ArcChromaFilter_MPA.h"


// Macro table
    int FilterChroma[2];
    int FilterChroma_Record(int sa, int SIMDnum);



#include "ArcMPC.h"

//---------------------------------------------------------------------------------
// Name:          FilterChromaPred8
// Purpose:       Filters chroma 8x8 or 4x8 predictions using simple bilinear 
//                interpolation
// Arguments:     predBuf:  Input buffer pointer
//                outBuf :  Output buffer pointer
//                iHorFrac: half pel precision horizontally
//                iVerFrac: half pel precision vertically
//                wideBlk:  0 if block width is 4, 1 if block width is 8
// Return Value:  void
//---------------------------------------------------------------------------------
// [Macro] FilterChromaPred8                                                                                // [   43] 
_Asm int Record_FilterChromaPred8(int *mac_tab)
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
FilterChromaPred8_RecLoop:
  ld r1,[r17 ,0]
  vrec r1

// p16    i0  predBuf                                                                                       // [   44] 
// p16    i1  outBuf                                                                                        // [   45] 
// p16    i2  iHorFrac                                                                                      // [   46] 
// p16    i10  iVerFrac                                                                                     // [   47] 
// p16    i3  wideBlk                                                                                       // [   48] 

// Begin                                                                                                    // [   50] 
// p16    i7  subPel                                                                                        // [   51] 
//vec16  vr02  horFrac                                                                                      // [   52] 
//vec16  vr03  verFrac                                                                                      // [   53] 
//vec16  vr04  invHorFrac                                                                                   // [   54] 
//vec16  vr05  invVerFrac                                                                                   // [   55] 
    
//vec16  vr06  chromaLane0                                                                                  // [   57] 
//vec16  vr07  chromaLane1                                                                                  // [   58] 
//vec16  vr08  chromaLane2                                                                                  // [   59] 
//vec16  vr09  chromaLane3                                                                                  // [   60] 
//vec16  vr10  chromaLane4                                                                                  // [   61] 
//vec16  vr11  chromaLane5                                                                                  // [   62] 
//vec16  vr12  chromaLane6                                                                                  // [   63] 
//vec16  vr13  chromaLane7                                                                                  // [   64] 
//vec16  vr14  chromaLane8                                                                                  // [   65] 
    
//vec16  vr15  chromaOffsetLane0                                                                            // [   67] 
//vec16  vr16  chromaOffsetLane1                                                                            // [   68] 
//vec16  vr17  chromaOffsetLane2                                                                            // [   69] 
//vec16  vr18  chromaOffsetLane3                                                                            // [   70] 
//vec16  vr19  chromaOffsetLane4                                                                            // [   71] 
//vec16  vr20  chromaOffsetLane5                                                                            // [   72] 
//vec16  vr21  chromaOffsetLane6                                                                            // [   73] 
//vec16  vr22  chromaOffsetLane7                                                                            // [   74] 
//vec16  vr23  chromaOffsetLane8                                                                            // [   75] 
    
    // Load data to be filtered
LINE 78".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VLD64W            vr06,[i0, 0*16]                                                // chromaLane0 predBuf [   78] 
       VLD64W            vr07,[i0, 1*16]                                                // chromaLane1 predBuf [   79] 
       VLD64W            vr08,[i0, 2*16]                                                // chromaLane2 predBuf [   80] 
       VLD64W            vr09,[i0, 3*16]                                                // chromaLane3 predBuf [   81] 
       VLD64W            vr10,[i0, 4*16]                                                // chromaLane4 predBuf [   82] 
    
    // If none of horFrac or verFrac is set then go straight to DMA out
    // No filtering is to be performed
// s16    vr24.4  iTmpFrac                                                                                  // [   86] 
LINE 87".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VOR.4             vr24, vr00, vr01                                        // iTmpFrac iHorFrac iVerFrac [   87] 
        VMR3W.128       vr00, vr24, vr24                                                    // subPel iTmpFrac [   88] 
LINE 89".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VJP.i7            r3                                                                 // subPel Continue [   89] 
       VLD64W            vr11,[i0, 5*16]  //DS____________________________________________ chromaLane5 predBuf [   90] 
       VLD64W            vr12,[i0, 6*16]  //DS____________________________________________ chromaLane6 predBuf [   91] 
       VLD64W            vr13,[i0, 7*16]  //DS____________________________________________ chromaLane7 predBuf [   92] 
       VASRPWB           vr06, vr06, 0                                              // chromaLane0 chromaLane0 [   93] 
       VASRPWB           vr07, vr07, 0                                              // chromaLane1 chromaLane1 [   94] 
       VASRPWB           vr08, vr08, 0                                              // chromaLane2 chromaLane2 [   95] 
       VASRPWB           vr09, vr09, 0                                              // chromaLane3 chromaLane3 [   96] 
       VASRPWB           vr10, vr10, 0                                              // chromaLane4 chromaLane4 [   97] 
       VJP               r4                                                                          // Output [   98] 
       VASRPWB           vr11, vr11, 0  //DS__________________________________________ chromaLane5 chromaLane5 [   99] 
       VASRPWB           vr12, vr12, 0  //DS__________________________________________ chromaLane6 chromaLane6 [  100] 
       VASRPWB           vr13, vr13, 0  //DS__________________________________________ chromaLane7 chromaLane7 [  101] 
    
//Continue --------                                                                                         // [  103] 
 vendrec r3                                                                                                 // [  103] 
 vrec r3                                                                                                    // [  103] 
    // If horFilter isn't set go straight to the Vertical filtering section
LINE 105".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VJP!i2            r5                                                              // iHorFrac VerFilter [  105] 
       VNOP                //DS_______________________________________________________________________________ [  106] 
       VNOP                //DS_______________________________________________________________________________ [  107] 
       VNOP                //DS_______________________________________________________________________________ [  108] 
    
    // If only horFrac is set then perform horizontal filtering
    // Load in extra column to the right of the 8x8 block
       VLD64W            vr15,[i0, 0*16+8]                                        // chromaOffsetLane0 predBuf [  112] 
       VLD64W            vr16,[i0, 1*16+8]                                        // chromaOffsetLane1 predBuf [  113] 
       VLD64W            vr17,[i0, 2*16+8]                                        // chromaOffsetLane2 predBuf [  114] 
       VLD64W            vr18,[i0, 3*16+8]                                        // chromaOffsetLane3 predBuf [  115] 
       VLD64W            vr19,[i0, 4*16+8]                                        // chromaOffsetLane4 predBuf [  116] 
       VLD64W            vr20,[i0, 5*16+8]                                        // chromaOffsetLane5 predBuf [  117] 
       VLD64W            vr21,[i0, 6*16+8]                                        // chromaOffsetLane6 predBuf [  118] 
       VLD64W            vr22,[i0, 7*16+8]                                        // chromaOffsetLane7 predBuf [  119] 
    
       VMR1W             vr15, vr15, vr06                   // chromaOffsetLane0 chromaOffsetLane0 chromaLane0 [  121] 
       VMR1W             vr16, vr16, vr07                   // chromaOffsetLane1 chromaOffsetLane1 chromaLane1 [  122] 
       VMR1W             vr17, vr17, vr08                   // chromaOffsetLane2 chromaOffsetLane2 chromaLane2 [  123] 
       VMR1W             vr18, vr18, vr09                   // chromaOffsetLane3 chromaOffsetLane3 chromaLane3 [  124] 
       VMR1W             vr19, vr19, vr10                   // chromaOffsetLane4 chromaOffsetLane4 chromaLane4 [  125] 
    
    // If verFrac is also set then go to 2D filter
    // The above loads and VMRs aren't wasted as they are also needed for
    // the 2D case anyway
       VJP.i10           r6                                                               // iVerFrac 2dFilter [  130] 
       VMR1W             vr20, vr20, vr11  //DS_______________ chromaOffsetLane5 chromaOffsetLane5 chromaLane5 [  131] 
       VMR1W             vr21, vr21, vr12  //DS_______________ chromaOffsetLane6 chromaOffsetLane6 chromaLane6 [  132] 
       VMR1W             vr22, vr22, vr13  //DS_______________ chromaOffsetLane7 chromaOffsetLane7 chromaLane7 [  133] 
    
    //actual horizontal filter
       VMIVW             vr02, i2                                                          // horFrac iHorFrac [  136] 
       VRSUBW            vr04, vr02, 8                                                   // invHorFrac horFrac [  137] 
       VMULW             vr06, vr06, vr04                                // chromaLane0 chromaLane0 invHorFrac [  138] 
       VMULAW            vr06, vr15, vr02                             // chromaLane0 chromaOffsetLane0 horFrac [  139] 
       VMULW             vr07, vr07, vr04                                // chromaLane1 chromaLane1 invHorFrac [  140] 
       VMULAW            vr07, vr16, vr02                             // chromaLane1 chromaOffsetLane1 horFrac [  141] 
       VMULW             vr08, vr08, vr04                                // chromaLane2 chromaLane2 invHorFrac [  142] 
       VMULAW            vr08, vr17, vr02                             // chromaLane2 chromaOffsetLane2 horFrac [  143] 
       VMULW             vr09, vr09, vr04                                // chromaLane3 chromaLane3 invHorFrac [  144] 
       VMULAW            vr09, vr18, vr02                             // chromaLane3 chromaOffsetLane3 horFrac [  145] 
       VMULW             vr10, vr10, vr04                                // chromaLane4 chromaLane4 invHorFrac [  146] 
       VMULAW            vr10, vr19, vr02                             // chromaLane4 chromaOffsetLane4 horFrac [  147] 
       VMULW             vr11, vr11, vr04                                // chromaLane5 chromaLane5 invHorFrac [  148] 
       VMULAW            vr11, vr20, vr02                             // chromaLane5 chromaOffsetLane5 horFrac [  149] 
       VMULW             vr12, vr12, vr04                                // chromaLane6 chromaLane6 invHorFrac [  150] 
        
       VJP               r7                                                                   // RoundAndShift [  152] 
       VMULAW            vr12, vr21, vr02  //DS_________________________ chromaLane6 chromaOffsetLane6 horFrac [  153] 
       VMULW             vr13, vr13, vr04  //DS____________________________ chromaLane7 chromaLane7 invHorFrac [  154] 
       VMULAW            vr13, vr22, vr02  //DS_________________________ chromaLane7 chromaOffsetLane7 horFrac [  155] 
    
//VerFilter --------                                                                                        // [  157] 
 vendrec r5                                                                                                 // [  157] 
 vrec r5                                                                                                    // [  157] 
LINE 158".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VMIVW             vr03, i10                                                         // verFrac iVerFrac [  158] 
       VRSUBW            vr05, vr03, 8                                                   // invVerFrac verFrac [  159] 
    // If only verFilter is set then load the extra lane at the bottom of the block
       VLD64W            vr14,[i0, 8*16]                                                // chromaLane8 predBuf [  161] 
    // perform actual vertical filter
       VMULW             vr06, vr06, vr05                                // chromaLane0 chromaLane0 invVerFrac [  163] 
       VMULAW            vr06, vr07, vr03                                   // chromaLane0 chromaLane1 verFrac [  164] 
       VMULW             vr07, vr07, vr05                                // chromaLane1 chromaLane1 invVerFrac [  165] 
       VMULAW            vr07, vr08, vr03                                   // chromaLane1 chromaLane2 verFrac [  166] 
       VMULW             vr08, vr08, vr05                                // chromaLane2 chromaLane2 invVerFrac [  167] 
       VMULAW            vr08, vr09, vr03                                   // chromaLane2 chromaLane3 verFrac [  168] 
       VMULW             vr09, vr09, vr05                                // chromaLane3 chromaLane3 invVerFrac [  169] 
       VMULAW            vr09, vr10, vr03                                   // chromaLane3 chromaLane4 verFrac [  170] 
       VMULW             vr10, vr10, vr05                                // chromaLane4 chromaLane4 invVerFrac [  171] 
       VMULAW            vr10, vr11, vr03                                   // chromaLane4 chromaLane5 verFrac [  172] 
       VMULW             vr11, vr11, vr05                                // chromaLane5 chromaLane5 invVerFrac [  173] 
       VMULAW            vr11, vr12, vr03                                   // chromaLane5 chromaLane6 verFrac [  174] 
       VMULW             vr12, vr12, vr05                                // chromaLane6 chromaLane6 invVerFrac [  175] 
       VMULAW            vr12, vr13, vr03                                   // chromaLane6 chromaLane7 verFrac [  176] 
       VMULW             vr13, vr13, vr05                                // chromaLane7 chromaLane7 invVerFrac [  177] 
       VMULAW            vr13, vr14, vr03                                   // chromaLane7 chromaLane8 verFrac [  178] 
    
//RoundAndShift --------                                                                                    // [  180] 
 vendrec r7                                                                                                 // [  180] 
 vrec r7                                                                                                    // [  180] 
LINE 181".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VASRRPWB          vr06, vr06, 3                                              // chromaLane0 chromaLane0 [  181] 
       VASRRPWB          vr07, vr07, 3                                              // chromaLane1 chromaLane1 [  182] 
       VASRRPWB          vr08, vr08, 3                                              // chromaLane2 chromaLane2 [  183] 
       VASRRPWB          vr09, vr09, 3                                              // chromaLane3 chromaLane3 [  184] 
       VASRRPWB          vr10, vr10, 3                                              // chromaLane4 chromaLane4 [  185] 
       VASRRPWB          vr11, vr11, 3                                              // chromaLane5 chromaLane5 [  186] 
       VASRRPWB          vr12, vr12, 3                                              // chromaLane6 chromaLane6 [  187] 
       VASRRPWB          vr13, vr13, 3                                              // chromaLane7 chromaLane7 [  188] 
    
    // Go to Output
       VJP               r4                                                                          // Output [  191] 
    // Don't need "nops" here as the 3 following instructions won't affect the
    // continuation of the program at the jump target

    
//2dFilter --------  //DS_____________________________________________________________________________________ [  196] 
 vendrec r6  //DS_____________________________________________________________________________________________ [  196] 
 vrec r6  //DS________________________________________________________________________________________________ [  196] 
    // Set the filter coefficients
//vec16  vr25  m0  //DS_______________________________________________________________________________________ [  198] 
//vec16  vr26  m1  //DS_______________________________________________________________________________________ [  199] 
//vec16  vr27  m2  //DS_______________________________________________________________________________________ [  200] 
//vec16  vr28  m3  //DS_______________________________________________________________________________________ [  201] 
LINE 202".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VMIVW             vr02, i2  //DS______________________________________________________ horFrac iHorFrac [  202] 
       VRSUBW            vr04, vr02, 8  //DS_______________________________________________ invHorFrac horFrac [  203] 
       VMIVW             vr03, i10  //DS_____________________________________________________ verFrac iVerFrac [  204] 
       VRSUBW            vr05, vr03, 8                                                   // invVerFrac verFrac [  205] 
       VMULW             vr25, vr04, vr05                                          // m0 invHorFrac invVerFrac [  206] 
       VMULW             vr26, vr02, vr05                                             // m1 horFrac invVerFrac [  207] 
       VMULW             vr27, vr04, vr03                                             // m2 invHorFrac verFrac [  208] 
       VMULW             vr28, vr02, vr03                                                // m3 horFrac verFrac [  209] 
    // Load in the extra lane + the extra pixel
       VLD64W            vr14,[i0, 8*16]                                                // chromaLane8 predBuf [  211] 
       VLD64W            vr23,[i0, 8*16+8]                                        // chromaOffsetLane8 predBuf [  212] 
       VMR1W             vr23, vr23, vr14                   // chromaOffsetLane8 chromaOffsetLane8 chromaLane8 [  213] 
    
    // 2D filtering
       VMULW             vr06, vr06, vr25                                        // chromaLane0 chromaLane0 m0 [  216] 
       VMULAW            vr06, vr15, vr26                                  // chromaLane0 chromaOffsetLane0 m1 [  217] 
       VMULAW            vr06, vr07, vr27                                        // chromaLane0 chromaLane1 m2 [  218] 
       VMULAW            vr06, vr16, vr28                                  // chromaLane0 chromaOffsetLane1 m3 [  219] 
       VMULW             vr07, vr07, vr25                                        // chromaLane1 chromaLane1 m0 [  220] 
       VMULAW            vr07, vr16, vr26                                  // chromaLane1 chromaOffsetLane1 m1 [  221] 
       VMULAW            vr07, vr08, vr27                                        // chromaLane1 chromaLane2 m2 [  222] 
       VMULAW            vr07, vr17, vr28                                  // chromaLane1 chromaOffsetLane2 m3 [  223] 
       VMULW             vr08, vr08, vr25                                        // chromaLane2 chromaLane2 m0 [  224] 
       VMULAW            vr08, vr17, vr26                                  // chromaLane2 chromaOffsetLane2 m1 [  225] 
       VMULAW            vr08, vr09, vr27                                        // chromaLane2 chromaLane3 m2 [  226] 
       VMULAW            vr08, vr18, vr28                                  // chromaLane2 chromaOffsetLane3 m3 [  227] 
       VMULW             vr09, vr09, vr25                                        // chromaLane3 chromaLane3 m0 [  228] 
       VMULAW            vr09, vr18, vr26                                  // chromaLane3 chromaOffsetLane3 m1 [  229] 
       VMULAW            vr09, vr10, vr27                                        // chromaLane3 chromaLane4 m2 [  230] 
       VMULAW            vr09, vr19, vr28                                  // chromaLane3 chromaOffsetLane4 m3 [  231] 
       VMULW             vr10, vr10, vr25                                        // chromaLane4 chromaLane4 m0 [  232] 
       VMULAW            vr10, vr19, vr26                                  // chromaLane4 chromaOffsetLane4 m1 [  233] 
       VMULAW            vr10, vr11, vr27                                        // chromaLane4 chromaLane5 m2 [  234] 
       VMULAW            vr10, vr20, vr28                                  // chromaLane4 chromaOffsetLane5 m3 [  235] 
       VMULW             vr11, vr11, vr25                                        // chromaLane5 chromaLane5 m0 [  236] 
       VMULAW            vr11, vr20, vr26                                  // chromaLane5 chromaOffsetLane5 m1 [  237] 
       VMULAW            vr11, vr12, vr27                                        // chromaLane5 chromaLane6 m2 [  238] 
       VMULAW            vr11, vr21, vr28                                  // chromaLane5 chromaOffsetLane6 m3 [  239] 
       VMULW             vr12, vr12, vr25                                        // chromaLane6 chromaLane6 m0 [  240] 
       VMULAW            vr12, vr21, vr26                                  // chromaLane6 chromaOffsetLane6 m1 [  241] 
       VMULAW            vr12, vr13, vr27                                        // chromaLane6 chromaLane7 m2 [  242] 
       VMULAW            vr12, vr22, vr28                                  // chromaLane6 chromaOffsetLane7 m3 [  243] 
       VMULW             vr13, vr13, vr25                                        // chromaLane7 chromaLane7 m0 [  244] 
       VMULAW            vr13, vr22, vr26                                  // chromaLane7 chromaOffsetLane7 m1 [  245] 
       VMULAW            vr13, vr14, vr27                                        // chromaLane7 chromaLane8 m2 [  246] 
       VMULAW            vr13, vr23, vr28                                  // chromaLane7 chromaOffsetLane8 m3 [  247] 

       VASRRPWB          vr06, vr06, 6                                              // chromaLane0 chromaLane0 [  249] 
       VASRRPWB          vr07, vr07, 6                                              // chromaLane1 chromaLane1 [  250] 
       VASRRPWB          vr08, vr08, 6                                              // chromaLane2 chromaLane2 [  251] 
       VASRRPWB          vr09, vr09, 6                                              // chromaLane3 chromaLane3 [  252] 
       VASRRPWB          vr10, vr10, 6                                              // chromaLane4 chromaLane4 [  253] 
       VASRRPWB          vr11, vr11, 6                                              // chromaLane5 chromaLane5 [  254] 
       VASRRPWB          vr12, vr12, 6                                              // chromaLane6 chromaLane6 [  255] 
       VASRRPWB          vr13, vr13, 6                                              // chromaLane7 chromaLane7 [  256] 

//Output --------                                                                                           // [  258] 
 vendrec r4                                                                                                 // [  258] 
 vrec r4                                                                                                    // [  258] 
    // If the filtered block is 8-pixels wide
LINE 260".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VST64.i3          vr06,[i1, 0*16]                                         // wideBlk chromaLane0 outBuf [  260] 
       VST64.i3          vr07,[i1, 1*16]                                         // wideBlk chromaLane1 outBuf [  261] 
       VST64.i3          vr08,[i1, 2*16]                                         // wideBlk chromaLane2 outBuf [  262] 
       VST64.i3          vr09,[i1, 3*16]                                         // wideBlk chromaLane3 outBuf [  263] 
       VST64.i3          vr10,[i1, 4*16]                                         // wideBlk chromaLane4 outBuf [  264] 
       VST64.i3          vr11,[i1, 5*16]                                         // wideBlk chromaLane5 outBuf [  265] 
       VST64.i3          vr12,[i1, 6*16]                                         // wideBlk chromaLane6 outBuf [  266] 
       VST64.i3          vr13,[i1, 7*16]                                         // wideBlk chromaLane7 outBuf [  267] 
    // If the filtered block is 4-pixels wide
       VST32!i3          vr06,[i1, 0*16]                                         // wideBlk chromaLane0 outBuf [  269] 
       VST32!i3          vr07,[i1, 1*16]                                         // wideBlk chromaLane1 outBuf [  270] 
       VST32!i3          vr08,[i1, 2*16]                                         // wideBlk chromaLane2 outBuf [  271] 
       VST32!i3          vr09,[i1, 3*16]                                         // wideBlk chromaLane3 outBuf [  272] 
       VST32!i3          vr10,[i1, 4*16]                                         // wideBlk chromaLane4 outBuf [  273] 
       VST32!i3          vr11,[i1, 5*16]                                         // wideBlk chromaLane5 outBuf [  274] 
       VST32!i3          vr12,[i1, 6*16]                                         // wideBlk chromaLane6 outBuf [  275] 
       VST32!i3          vr13,[i1, 7*16]                                         // wideBlk chromaLane7 outBuf [  276] 

// End                                                                                                      // [  278] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FilterChromaPred8_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FilterChromaPred8                                                                                // [  280] 




//---------------------------------------------------------------------------------
// Name:          FilterChromaPred4
// Purpose:       Filters chroma 8x4 or 4x4 predictions using simple bilinear 
//                interpolation
// Arguments:     predBuf: Input buffer pointer
//                outBuf : Output buffer pointer
//                horFrac: half pel precision horizontally
//                verFrac: half pel precision vertically
// Return Value:  void
//---------------------------------------------------------------------------------
// [Macro] FilterChromaPred4                                                                                // [  293] 
_Asm int Record_FilterChromaPred4(int *mac_tab)
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
FilterChromaPred4_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

// p16    i0  predBuf                                                                                       // [  294] 
// p16    i1  outBuf                                                                                        // [  295] 
// p16    i2  iHorFrac                                                                                      // [  296] 
// p16    i10  iVerFrac                                                                                     // [  297] 
// p16    i3  wideBlk                                                                                       // [  298] 

// Begin                                                                                                    // [  300] 
// p16    i7  subPel                                                                                        // [  301] 
//vec16  vr02  horFrac                                                                                      // [  302] 
//vec16  vr03  verFrac                                                                                      // [  303] 
//vec16  vr04  invHorFrac                                                                                   // [  304] 
//vec16  vr05  invVerFrac                                                                                   // [  305] 
    
//vec16  vr06  chromaLane0                                                                                  // [  307] 
//vec16  vr07  chromaLane1                                                                                  // [  308] 
//vec16  vr08  chromaLane2                                                                                  // [  309] 
//vec16  vr09  chromaLane3                                                                                  // [  310] 
//vec16  vr10  chromaLane4                                                                                  // [  311] 
    
//vec16  vr11  chromaOffsetLane0                                                                            // [  313] 
//vec16  vr12  chromaOffsetLane1                                                                            // [  314] 
//vec16  vr13  chromaOffsetLane2                                                                            // [  315] 
//vec16  vr14  chromaOffsetLane3                                                                            // [  316] 
//vec16  vr15  chromaOffsetLane4                                                                            // [  317] 
    
    // Load data to be filtered
LINE 320".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VLD64W            vr06,[i0, 0*16]                                                // chromaLane0 predBuf [  320] 
    
    // If none of horFrac or verFrac is set then go straight to DMA out
    // No filtering is to be performed
// s16    vr16.4  iTmpFrac                                                                                  // [  324] 
LINE 325".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VOR.4             vr16, vr00, vr01                                        // iTmpFrac iHorFrac iVerFrac [  325] 
        VMR3W.128       vr00, vr16, vr16                                                    // subPel iTmpFrac [  326] 
LINE 327".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VJP.i7            r3                                                                 // subPel Continue [  327] 
       VLD64W            vr07,[i0, 1*16]  //DS____________________________________________ chromaLane1 predBuf [  328] 
       VLD64W            vr08,[i0, 2*16]  //DS____________________________________________ chromaLane2 predBuf [  329] 
       VLD64W            vr09,[i0, 3*16]  //DS____________________________________________ chromaLane3 predBuf [  330] 
       VASRPWB           vr06, vr06, 0                                              // chromaLane0 chromaLane0 [  331] 
       VJP               r4                                                                          // Output [  332] 
       VASRPWB           vr07, vr07, 0  //DS__________________________________________ chromaLane1 chromaLane1 [  333] 
       VASRPWB           vr08, vr08, 0  //DS__________________________________________ chromaLane2 chromaLane2 [  334] 
       VASRPWB           vr09, vr09, 0  //DS__________________________________________ chromaLane3 chromaLane3 [  335] 
    
//Continue --------                                                                                         // [  337] 
 vendrec r3                                                                                                 // [  337] 
 vrec r3                                                                                                    // [  337] 
    // If horFilter isn't set go straight to the Vertical filtering section
LINE 339".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VJP!i2            r5                                                              // iHorFrac VerFilter [  339] 
       VNOP                //DS_______________________________________________________________________________ [  340] 
       VNOP                //DS_______________________________________________________________________________ [  341] 
       VNOP                //DS_______________________________________________________________________________ [  342] 
    
    // If only horFrac is set then perform horizontal filtering
    // Load in extra column to the right of the 8x8 block
       VLD64W            vr11,[i0, 0*16+8]                                        // chromaOffsetLane0 predBuf [  346] 
       VLD64W            vr12,[i0, 1*16+8]                                        // chromaOffsetLane1 predBuf [  347] 
       VLD64W            vr13,[i0, 2*16+8]                                        // chromaOffsetLane2 predBuf [  348] 
       VLD64W            vr14,[i0, 3*16+8]                                        // chromaOffsetLane3 predBuf [  349] 
    
       VMR1W             vr11, vr11, vr06                   // chromaOffsetLane0 chromaOffsetLane0 chromaLane0 [  351] 
    
    // If verFrac is also set then go to 2D filter
    // The above loads and VMRs aren't wasted as they are also needed for
    // the 2D case anyway
       VJP.i10           r6                                                               // iVerFrac 2dFilter [  356] 
       VMR1W             vr12, vr12, vr07  //DS_______________ chromaOffsetLane1 chromaOffsetLane1 chromaLane1 [  357] 
       VMR1W             vr13, vr13, vr08  //DS_______________ chromaOffsetLane2 chromaOffsetLane2 chromaLane2 [  358] 
       VMR1W             vr14, vr14, vr09  //DS_______________ chromaOffsetLane3 chromaOffsetLane3 chromaLane3 [  359] 
    
    //actual horizontal filter
       VMIVW             vr02, i2                                                          // horFrac iHorFrac [  362] 
       VRSUBW            vr04, vr02, 8                                                   // invHorFrac horFrac [  363] 
       VMULW             vr06, vr06, vr04                                // chromaLane0 chromaLane0 invHorFrac [  364] 
       VMULAW            vr06, vr11, vr02                             // chromaLane0 chromaOffsetLane0 horFrac [  365] 
       VMULW             vr07, vr07, vr04                                // chromaLane1 chromaLane1 invHorFrac [  366] 
       VMULAW            vr07, vr12, vr02                             // chromaLane1 chromaOffsetLane1 horFrac [  367] 
       VMULW             vr08, vr08, vr04                                // chromaLane2 chromaLane2 invHorFrac [  368] 
        
       VJP               r7                                                                   // RoundAndShift [  370] 
       VMULAW            vr08, vr13, vr02  //DS_________________________ chromaLane2 chromaOffsetLane2 horFrac [  371] 
       VMULW             vr09, vr09, vr04  //DS____________________________ chromaLane3 chromaLane3 invHorFrac [  372] 
       VMULAW            vr09, vr14, vr02  //DS_________________________ chromaLane3 chromaOffsetLane3 horFrac [  373] 
    
//VerFilter --------                                                                                        // [  375] 
 vendrec r5                                                                                                 // [  375] 
 vrec r5                                                                                                    // [  375] 
LINE 376".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VMIVW             vr03, i10                                                         // verFrac iVerFrac [  376] 
       VRSUBW            vr05, vr03, 8                                                   // invVerFrac verFrac [  377] 
    // If only verFilter is set then load the extra lane at the bottom of the block
       VLD64W            vr10,[i0, 4*16]                                                // chromaLane4 predBuf [  379] 
    // perform actual vertical filter
       VMULW             vr06, vr06, vr05                                // chromaLane0 chromaLane0 invVerFrac [  381] 
       VMULAW            vr06, vr07, vr03                                   // chromaLane0 chromaLane1 verFrac [  382] 
       VMULW             vr07, vr07, vr05                                // chromaLane1 chromaLane1 invVerFrac [  383] 
       VMULAW            vr07, vr08, vr03                                   // chromaLane1 chromaLane2 verFrac [  384] 
       VMULW             vr08, vr08, vr05                                // chromaLane2 chromaLane2 invVerFrac [  385] 
       VMULAW            vr08, vr09, vr03                                   // chromaLane2 chromaLane3 verFrac [  386] 
       VMULW             vr09, vr09, vr05                                // chromaLane3 chromaLane3 invVerFrac [  387] 
       VMULAW            vr09, vr10, vr03                                   // chromaLane3 chromaLane4 verFrac [  388] 
    
//RoundAndShift --------                                                                                    // [  390] 
 vendrec r7                                                                                                 // [  390] 
 vrec r7                                                                                                    // [  390] 
LINE 391".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VASRRPWB          vr06, vr06, 3                                              // chromaLane0 chromaLane0 [  391] 
       VASRRPWB          vr07, vr07, 3                                              // chromaLane1 chromaLane1 [  392] 
       VASRRPWB          vr08, vr08, 3                                              // chromaLane2 chromaLane2 [  393] 
       VASRRPWB          vr09, vr09, 3                                              // chromaLane3 chromaLane3 [  394] 
    
    // Go to Output
       VJP               r4                                                                          // Output [  397] 
    // Don't need "nops" here as the 3 following instructions won't affect the
    // continuation of the program at the jump target

    
//2dFilter --------  //DS_____________________________________________________________________________________ [  402] 
 vendrec r6  //DS_____________________________________________________________________________________________ [  402] 
 vrec r6  //DS________________________________________________________________________________________________ [  402] 
    // Set the filter coefficients
//vec16  vr17  m0  //DS_______________________________________________________________________________________ [  404] 
//vec16  vr18  m1  //DS_______________________________________________________________________________________ [  405] 
//vec16  vr19  m2  //DS_______________________________________________________________________________________ [  406] 
//vec16  vr20  m3  //DS_______________________________________________________________________________________ [  407] 
LINE 408".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VMIVW             vr02, i2  //DS______________________________________________________ horFrac iHorFrac [  408] 
       VRSUBW            vr04, vr02, 8  //DS_______________________________________________ invHorFrac horFrac [  409] 
       VMIVW             vr03, i10  //DS_____________________________________________________ verFrac iVerFrac [  410] 
       VRSUBW            vr05, vr03, 8                                                   // invVerFrac verFrac [  411] 
       VMULW             vr17, vr04, vr05                                          // m0 invHorFrac invVerFrac [  412] 
       VMULW             vr18, vr02, vr05                                             // m1 horFrac invVerFrac [  413] 
       VMULW             vr19, vr04, vr03                                             // m2 invHorFrac verFrac [  414] 
       VMULW             vr20, vr02, vr03                                                // m3 horFrac verFrac [  415] 
    // Load in the extra lane + the extra pixel
       VLD64W            vr10,[i0, 4*16]                                                // chromaLane4 predBuf [  417] 
       VLD64W            vr15,[i0, 4*16+8]                                        // chromaOffsetLane4 predBuf [  418] 
       VMR1W             vr15, vr15, vr10                   // chromaOffsetLane4 chromaOffsetLane4 chromaLane4 [  419] 
    
    // 2D filtering
       VMULW             vr06, vr06, vr17                                        // chromaLane0 chromaLane0 m0 [  422] 
       VMULAW            vr06, vr11, vr18                                  // chromaLane0 chromaOffsetLane0 m1 [  423] 
       VMULAW            vr06, vr07, vr19                                        // chromaLane0 chromaLane1 m2 [  424] 
       VMULAW            vr06, vr12, vr20                                  // chromaLane0 chromaOffsetLane1 m3 [  425] 
       VMULW             vr07, vr07, vr17                                        // chromaLane1 chromaLane1 m0 [  426] 
       VMULAW            vr07, vr12, vr18                                  // chromaLane1 chromaOffsetLane1 m1 [  427] 
       VMULAW            vr07, vr08, vr19                                        // chromaLane1 chromaLane2 m2 [  428] 
       VMULAW            vr07, vr13, vr20                                  // chromaLane1 chromaOffsetLane2 m3 [  429] 
       VMULW             vr08, vr08, vr17                                        // chromaLane2 chromaLane2 m0 [  430] 
       VMULAW            vr08, vr13, vr18                                  // chromaLane2 chromaOffsetLane2 m1 [  431] 
       VMULAW            vr08, vr09, vr19                                        // chromaLane2 chromaLane3 m2 [  432] 
       VMULAW            vr08, vr14, vr20                                  // chromaLane2 chromaOffsetLane3 m3 [  433] 
       VMULW             vr09, vr09, vr17                                        // chromaLane3 chromaLane3 m0 [  434] 
       VMULAW            vr09, vr14, vr18                                  // chromaLane3 chromaOffsetLane3 m1 [  435] 
       VMULAW            vr09, vr10, vr19                                        // chromaLane3 chromaLane4 m2 [  436] 
       VMULAW            vr09, vr15, vr20                                  // chromaLane3 chromaOffsetLane4 m3 [  437] 

       VASRRPWB          vr06, vr06, 6                                              // chromaLane0 chromaLane0 [  439] 
       VASRRPWB          vr07, vr07, 6                                              // chromaLane1 chromaLane1 [  440] 
       VASRRPWB          vr08, vr08, 6                                              // chromaLane2 chromaLane2 [  441] 
       VASRRPWB          vr09, vr09, 6                                              // chromaLane3 chromaLane3 [  442] 

//Output --------                                                                                           // [  444] 
 vendrec r4                                                                                                 // [  444] 
 vrec r4                                                                                                    // [  444] 
    // If the filtered block is 8-pixels wide
LINE 446".\ARCEncoder\H264\ArcChromaFilter_MPA.sss"
       VST64.i3          vr06,[i1, 0*16]                                         // wideBlk chromaLane0 outBuf [  446] 
       VST64.i3          vr07,[i1, 1*16]                                         // wideBlk chromaLane1 outBuf [  447] 
       VST64.i3          vr08,[i1, 2*16]                                         // wideBlk chromaLane2 outBuf [  448] 
       VST64.i3          vr09,[i1, 3*16]                                         // wideBlk chromaLane3 outBuf [  449] 
    // If the filtered block is 4-pixels wide
       VST32!i3          vr06,[i1, 0*16]                                         // wideBlk chromaLane0 outBuf [  451] 
       VST32!i3          vr07,[i1, 1*16]                                         // wideBlk chromaLane1 outBuf [  452] 
       VST32!i3          vr08,[i1, 2*16]                                         // wideBlk chromaLane2 outBuf [  453] 
       VST32!i3          vr09,[i1, 3*16]                                         // wideBlk chromaLane3 outBuf [  454] 

    
// End                                                                                                      // [  457] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FilterChromaPred4_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FilterChromaPred4                                                                                // [  459] 






const int MACRO_IDX_FilterChromaPred8 = 0;
const int MACRO_IDX_FilterChromaPred4 = 1;

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

    map[MACRO_IDX_FilterChromaPred8] = sa;
    sa = Record_FilterChromaPred8(map);

    map[MACRO_IDX_FilterChromaPred4] = sa;
    sa = Record_FilterChromaPred4(map);

    return sa;
}
