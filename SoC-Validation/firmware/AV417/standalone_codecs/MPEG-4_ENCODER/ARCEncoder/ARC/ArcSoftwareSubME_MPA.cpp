//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss'

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
#include ".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.h"


// Macro table
    int Software_SubME[5];
    int Software_SubME_Record(int sa, int SIMDnum);

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
#ifdef MPEG4BUILD
#include "ArcMP4SDMTables.h"
#else // MPEG4BUILD
#include "ArcSDMTables.h"
#endif // MPEG4BUILD
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"
#include "ArcSoftwareME_MPA.h"

extern int Software_ME[] ;
extern int Software_ME[] ;
//----------------------------------------------------------------------------------------------------
// Is Macro
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft
// p16    i0  PredX                                                                                                     // [   44] 
// p16    i1  PredY                                                                                                     // [   44] 
// p16    i2  VecX                                                                                                      // [   45] 
// p16    i3  VecY                                                                                                      // [   45] 
// ****** Informational : Register space for symbol "PatternMatch" already allocated.                                   // [   46] 
// p16    i6  PatternMatch                                                                                              // [   46] 
// p16    i7  lambda                                                                                                    // [   47] 
// p16    i8  BestCost                                                                                                  // [   48] 
// p16    i10  lastX                                                                                                    // [   49] 
// p16    i11  lastY                                                                                                    // [   49] 
// p16    i12  StepMult                                                                                                 // [   50] 
//vec16  vr14  result                                                                                                   // [   51] 
//vec16  vr02  PartRes                                                                                                  // [   52] 

// p16    i9  CirPtr                                                                                                    // [   54] 
// p16    i13  RefPtr                                                                                                   // [   54] 

// [Macro] HalfPelME                                                                                                    // [   56] 
_Asm int Record_HalfPelME(int *mac_tab)
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
HalfPelME_RecLoop:
  ld r1,[r17 ,8]
  vrec r1




//  this section needed for repositioning data ready for subpel search
// Begin                                                                                                                // [   61] ....................//*** of repositioning reference data
// need to move reference data
// this depends on the lastX and lastY values calculated in the integer search phase
// p16    i14  offset                                                                                                   // [   64] 
// p16    i15  NewPtr                                                                                                   // [   65] 
//vec16  vr03  ref0                                                                                                     // [   66] 
//vec16  vr04  ref1                                                                                                     // [   66] 
//vec16  vr05  ref2                                                                                                     // [   66] 
//vec16  vr06  ref3                                                                                                     // [   66] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        NewPtr|        offset|        RefPtr|      StepMult|         lastY|         lastX|        CirPtr|      BestCost|
//|VR 2|                                                     PartRes                                                           |
//|VR 3|                                                        ref0                                                           |
//|VR 4|                                                        ref1                                                           |
//|VR 5|                                                        ref2                                                           |
//|VR 6|                                                        ref3                                                           |
//|............................................................................................................................|
//|VR14|                                                      result                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


// start at (1+lastY)*ME_REFERENCE_ARRAY_STRIDE + SDM_ME_REFERENCE_ARRAY
#ifdef MPEG4BUILD
LINE 71".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VIM               i14, i11, 2                                                                       // offset lastY [   71] ....................// 0, 2, or 4
       VMOVW             vr01, 9-1, 2                                                                            // CirPtr [   72] ....................// we only require 16 + 1+ 1 rows of ref 
// set offset to be 0, 1 or 2 lines in
       VMULW.64          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/2                                          // offset offset [   74] 
#else // MPEG4BUILD
       VIM               i14, i11, 4                                                                       // offset lastY [   76] ....................// 0, 4 or 8 
       VMOVW             vr01, 9-1, 2                                                                            // CirPtr [   77] ....................// we only require 16 + 1+ 1 rows of ref 
// set offset to be 0, 1 or 2 lines in
       VMULW.64          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/4                                          // offset offset [   79] 
#endif // MPEG4BUILD

       VJP!i10           r3                                                                            // lastX ShiftZero1 [   82] ....................// last horizontal winner was 0
       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY-2*ME_REFERENCE_ARRAY_STRIDE, 128  //DS_______________________ NewPtr [   83] ....................// dest ptr
       VIM               i13, i14, SDM_ME_REFERENCE_ARRAY  //DS_____________________________________________ RefPtr offset [   84] ....................// source ptr
#ifdef MPEG4BUILD
       VIM               i14, i10, 2  //DS___________________________________________________________________ offset lastX [   86] 
#else // MPEG4BUILD
// ****** Warning : Misplaced delay slot marker found at line number 88 *****
LINE 88".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VIM               i14, i10, 4                                                                       // offset lastX [   88] 
#endif // MPEG4BUILD

              //  Stall: [i14]  ................................      I14  
       VJP!i14           r4                                                                          // offset ShiftRight1 [   91] ....................// last horizontal winner was -1
       VNOP                //DS___________________________________________________________________________________________ [   92] 
       VNOP                //DS___________________________________________________________________________________________ [   93] 
       VNOP                //DS___________________________________________________________________________________________ [   94] 

//ShiftLeft1 --------                                                                                                   // [   96] ....................// last horizontal winner was +1
 vendrec r5                                                                                                             // [   96] 
 vrec r5                                                                                                                // [   96] 
// we need to shift along by 2 pels
// load two rows at a time
LINE 99".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VLD128            vr03,[i13, 0]                                                                      // ref0 RefPtr [   99] 
       VLD128            vr04,[i13, 16]                                                                     // ref1 RefPtr [  100] 
       VLD128            vr05,[i13, 32]                                                                     // ref2 RefPtr [  101] 
       VLD128            vr06,[i13, 48]                                                                     // ref3 RefPtr [  102] 
       VIM               i13, i13, 2*ME_REFERENCE_ARRAY_STRIDE                                            // RefPtr RefPtr [  103] ....................// increment source pointer
       VMR1W             vr03, vr04, vr03                                                                // ref0 ref1 ref0 [  104] ....................// shift by 2 pixels
       VIM               i15, i15, 2*ME_REFERENCE_ARRAY_STRIDE                                            // NewPtr NewPtr [  105] ....................// increment the output pointer
       VSR8              vr04, vr04, 2                                                                        // ref1 ref1 [  106] ....................// shift by 2 pixels (don't worry about the top 2 pels, we son't use them
       VMR1W             vr05, vr06, vr05                                                                // ref2 ref3 ref2 [  107] ....................// shift by 2 pixels
       VST128            vr03,[i15, 0]                                                                      // ref0 NewPtr [  108] ....................// store
       VSR8              vr06, vr06, 2                                                                        // ref3 ref3 [  109] ....................// shift by 2 pixels (don't worry about the top 2 pels, we son't use them


       VJD.i9            i9, r5                                                                // CirPtr CirPtr ShiftLeft1 [  112] 
       VST128            vr04,[i15, 16]  //DS_________________________________________________________________ ref1 NewPtr [  113] 
       VST128            vr05,[i15, 32]  //DS_________________________________________________________________ ref2 NewPtr [  114] ....................// store
       VST128            vr06,[i15, 48]  //DS_________________________________________________________________ ref3 NewPtr [  115] 


       VJP               r6                                                                                   // EndShift1 [  118] ....................// skip to end of repositioning process
       VNOP                //DS___________________________________________________________________________________________ [  119] 
       VNOP                //DS___________________________________________________________________________________________ [  120] 
       VNOP                //DS___________________________________________________________________________________________ [  121] 


//ShiftZero1 --------                                                                                                   // [  124] 
 vendrec r3                                                                                                             // [  124] 
 vrec r3                                                                                                                // [  124] 
// zero pos horizontally - need to shift right by 1 pels

LINE 127".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VLD128            vr03,[i13, 0]                                                                      // ref0 RefPtr [  127] 
       VLD128            vr04,[i13, 16]                                                                     // ref1 RefPtr [  128] 
       VLD128            vr05,[i13, 32]                                                                     // ref2 RefPtr [  129] 
       VLD128            vr06,[i13, 48]                                                                     // ref3 RefPtr [  130] 
       VIM               i13, i13, 2*ME_REFERENCE_ARRAY_STRIDE                                            // RefPtr RefPtr [  131] 
       VMRB              vr03, vr04, vr03                                                                // ref0 ref1 ref0 [  132] ....................// shift by 1 pixels
       VIM               i15, i15, 2*ME_REFERENCE_ARRAY_STRIDE                                            // NewPtr NewPtr [  133] ....................// increment the output pointer
       VSR8              vr04, vr04, 1                                                                        // ref1 ref1 [  134] ....................// shift by 1 pixels
       VMRB              vr05, vr06, vr05                                                                // ref2 ref3 ref2 [  135] ....................// shift by 1 pixels
       VST128            vr03,[i15, 0]                                                                      // ref0 NewPtr [  136] 
       VSR8              vr06, vr06, 1                                                                        // ref3 ref3 [  137] ....................// shift by 1 pixels

       VJD.i9            i9, r3                                                                // CirPtr CirPtr ShiftZero1 [  139] 
       VST128            vr04,[i15, 16]  //DS_________________________________________________________________ ref1 NewPtr [  140] 
       VST128            vr05,[i15, 32]  //DS_________________________________________________________________ ref2 NewPtr [  141] ....................// store
       VST128            vr06,[i15, 48]  //DS_________________________________________________________________ ref3 NewPtr [  142] 

       VJP               r6                                                                                   // EndShift1 [  144] 
       VNOP                //DS___________________________________________________________________________________________ [  145] 
       VNOP                //DS___________________________________________________________________________________________ [  146] 
       VNOP                //DS___________________________________________________________________________________________ [  147] 


//ShiftRight1 --------                                                                                                  // [  150] 
 vendrec r4                                                                                                             // [  150] 
 vrec r4                                                                                                                // [  150] 
// best x direction was +1
// this means that we only need to shift the rows really

LINE 154".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VLD128            vr03,[i13, 0]                                                                      // ref0 RefPtr [  154] 
       VLD128            vr04,[i13, 16]                                                                     // ref1 RefPtr [  155] 
       VIM               i15, i15, 2*ME_REFERENCE_ARRAY_STRIDE                                            // NewPtr NewPtr [  156] 
       VLD128            vr05,[i13, 32]                                                                     // ref2 RefPtr [  157] 
       VLD128            vr06,[i13, 48]                                                                     // ref3 RefPtr [  158] 
       VIM               i13, i13, 2*ME_REFERENCE_ARRAY_STRIDE                                            // RefPtr RefPtr [  159] 
       VST128            vr03,[i15, 0]                                                                      // ref0 NewPtr [  160] 
       VJD.i9            i9, r4                                                               // CirPtr CirPtr ShiftRight1 [  161] 
       VST128            vr04,[i15, 16]  //DS_________________________________________________________________ ref1 NewPtr [  162] 
       VST128            vr05,[i15, 32]  //DS_________________________________________________________________ ref2 NewPtr [  163] 
       VST128            vr06,[i15, 48]  //DS_________________________________________________________________ ref3 NewPtr [  164] 



//EndShift1 --------                                                                                                    // [  168] 
 vendrec r6                                                                                                             // [  168] 
 vrec r6                                                                                                                // [  168] 
// End                                                                                                                  // [  169] ....................//*** of repositioning reference data


// Begin                                                                                                                // [  172] 
// p16    i14  row                                                                                                      // [  173] 
//vec16  vr03  cur0                                                                                                     // [  174] 
//vec16  vr04  cur1                                                                                                     // [  174] 
//vec16  vr05  right0                                                                                                   // [  175] 
//vec16  vr06  right1                                                                                                   // [  175] 
//vec16  vr07  right2                                                                                                   // [  175] 
//vec16  vr08  right3                                                                                                   // [  175] 
//vec16  vr09  left0                                                                                                    // [  176] 
//vec16  vr10  left1                                                                                                    // [  176] 
//vec16  vr11  left2                                                                                                    // [  176] 
//vec16  vr12  left3                                                                                                    // [  176] 
//vec16  vr13  zero0                                                                                                    // [  177] 
//vec16  vr15  zero1                                                                                                    // [  177] 
//vec16  vr16  zero2                                                                                                    // [  177] 
//vec16  vr17  zero3                                                                                                    // [  177] 

//vec16  vr18  inputleft1                                                                                               // [  179] 
//vec16  vr19  inputright1                                                                                              // [  179] 
//vec16  vr20  inputleft2                                                                                               // [  179] 
//vec16  vr21  inputright2                                                                                              // [  179] 

// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |           row|        RefPtr|      StepMult|         lastY|         lastX|        CirPtr|      BestCost|
//|VR 2|                                                     PartRes                                                           |
//|VR 3|                                                        cur0                                                           |
//|VR 4|                                                        cur1                                                           |
//|VR 5|                                                      right0                                                           |
//|VR 6|                                                      right1                                                           |
//|VR 7|                                                      right2                                                           |
//|VR 8|                                                      right3                                                           |
//|VR 9|                                                       left0                                                           |
//|VR10|                                                       left1                                                           |
//|VR11|                                                       left2                                                           |
//|VR12|                                                       left3                                                           |
//|VR13|                                                       zero0                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       zero1                                                           |
//|VR16|                                                       zero2                                                           |
//|VR17|                                                       zero3                                                           |
//|VR18|                                                  inputleft1                                                           |
//|VR19|                                                 inputright1                                                           |
//|VR20|                                                  inputleft2                                                           |
//|VR21|                                                 inputright2                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


  ld r7,[r17,4];                                                                                      // VSTACK initRDvals [  183] 
LINE 183".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VJL               vr31, r7                                                                                       // [  183] 
    // rowLoop start
       VASRW.16          vr01, vr01, 1  //DS____________________________________________________________ StepMult StepMult [  185] 
       VNOP                //DS___________________________________________________________________________________________ [  186] 
       VNOP                //DS___________________________________________________________________________________________ [  187] 



       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 32                                                        // RefPtr [  191] 
///*  The 8 points in the first step are done in this pattern :       */
///*                                                                  */
///*      res2 (-.5,-.5)   res1 (0,-.5)    res0 (.5,-.5)                   */
///*      res5 (-.5, 0)                    res3 (.5, 0)                   */
///*      res8 (-.5, .5)   res7 (0, .5)    res6 (.5, .5)                   */
///*                                                                  */
// load 12 rows of current data (not enough vecs for all 16) 
// last 4 rows will be loaded into cur0 - cur4, done like this to avoid re-shifting reference data that
// is used for several offset calculations
// assumes that the reference array is loaded with last 18 pels per row valid
// representing best displacement -1,0000000000000000,+1
// 
// indices 1 and 2 will hold cycled integer vertical locations i.e.
// (-1/2,   n) (0,  n) and (1/2,  n)
// (-1/2, n+1) (
       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [  207] 


       VLD128            vr19,[i13, 16]                                                              // inputright1 RefPtr [  210] ....................// load line of ref
       VLD128            vr18,[i13, 0]                                                                // inputleft1 RefPtr [  211] ....................// ditto
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  212] ....................// load first line of cur

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  214] ....................// increment to next row
       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  215] ....................// increment ready to read next line

       VMRB              vr15, vr19, vr18                                                  // zero1 inputright1 inputleft1 [  217] ....................// get vector 1 pixel displaced
       VMR1W             vr06, vr19, vr18                                                 // right1 inputright1 inputleft1 [  218] ....................// get vector 2 pixels displaced

       VLD128            vr21,[i13, 16]                                                              // inputright2 RefPtr [  220] ....................// load next line of ref
       VLD128            vr20,[i13, 0]                                                                // inputleft2 RefPtr [  221] ....................// ditto

       VAVRB             vr06, vr06, vr15                                                           // right1 right1 zero1 [  223] ....................// find horizontal half pel displacement
       VAVRB             vr10, vr18, vr15                                                        // left1 inputleft1 zero1 [  224] 
// now have (-1/2,-1) (0,-1) and (1/2,-1) in left1, zero1, right1


       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  228] ....................// increment to next line


       VMRB              vr16, vr21, vr20                                                  // zero2 inputright2 inputleft2 [  231] ....................// shift by 1 pel
       VMR1W             vr07, vr21, vr20                                                 // right2 inputright2 inputleft2 [  232] ....................// shift by 2 pels

    // setup loop counter
    // 16 rows total, 2 row per loop
       VMOVW             vr01, 8-1, 64                                                                              // row [  236] 

       VAVRB             vr11, vr20, vr16                                                        // left2 inputleft2 zero2 [  238] ....................// find horizontal half displacement
       VAVRB             vr07, vr07, vr16                                                           // right2 right2 zero2 [  239] ....................// find horizontal half displacement
//  (-1/2,  0) (0, 0),  (1/2, 0)   in left2, zero2, right2  index


       VAVRB             vr13, vr16, vr15                                                             // zero0 zero2 zero1 [  243] ....................// average data in index 1 and index 2
       VAVRB             vr05, vr07, vr06                                                          // right0 right2 right1 [  244] ....................// ditto
       VAVRB             vr09, vr11, vr10                                                             // left0 left2 left1 [  245] ....................// ditto
// this gives us (-1/2, -1/2) (0, -1/2) and (1/2, -1/2) in left0, zero0, right0


// from here loop

//SubHalfLoop --------                                                                                                  // [  251] 
 vendrec r8                                                                                                             // [  251] 
 vrec r8                                                                                                                // [  251] 


LINE 254".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VLD128            vr19,[i13, 16]                                                              // inputright1 RefPtr [  254] ....................// load more ref data
       VLD128            vr18,[i13, 0]                                                                // inputleft1 RefPtr [  255] ....................// load more ref data

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  257] ....................// increment ref pointer


       VLD128            vr04,[i9, 0]                                                                       // cur1 CirPtr [  260] ....................// load next current data row
       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  261] ....................// increment for next line

       VMRB              vr15, vr19, vr18                                                  // zero1 inputright1 inputleft1 [  263] ....................// find data 1 pel displaced
       VMR1W             vr06, vr19, vr18                                                 // right1 inputright1 inputleft1 [  264] ....................// find data 2 pels displaced


       VLD128            vr21,[i13, 16]                                                              // inputright2 RefPtr [  267] ....................// load next line
       VLD128            vr20,[i13, 0]                                                                // inputleft2 RefPtr [  268] ....................// load next line

       VAVRB             vr06, vr06, vr15                                                           // right1 right1 zero1 [  270] ....................// find 1/2 pel location to the right
       VAVRB             vr10, vr18, vr15                                                        // left1 inputleft1 zero1 [  271] ....................// find 1/2 pel location to the left

       VAVRB             vr17, vr16, vr15                                                             // zero3 zero2 zero1 [  273] ....................// average
       VAVRB             vr12, vr11, vr10                                                             // left3 left2 left1 [  274] ....................// average with previous data
       VAVRB             vr08, vr07, vr06                                                          // right3 right2 right1 [  275] ....................// average
// now    (-1/2,  1/2) (0, 1/2),  (1/2, 1/2) in 3



// now have two rows of horizontal 1/2 locations (left, zero, right)
// and  (-1/2,-1/2) (0,-1/2), (1/2,-1/2)  in 0th index
// and  (-1/2, 1/2) (0, 1/2), (1/2, 1/2)  in 3rd index
// and  (-1/2,  -1) (0,  -1), (1/2,  -1)  in 2th index
// and  (-1/2,   0) (0,   0), (1/2,   0)  in 1th index

// now we can start accumulating SADs
       VSADBW.8          vr02, vr03, vr11                                                            // PartRes cur0 left2 [  287] 
       VSADBAW.8         vr02, vr04, vr10                                                            // PartRes cur1 left1 [  288] 
       VSADBW.16         vr02, vr03, vr07                                                           // PartRes cur0 right2 [  289] 
       VSADBAW.16        vr02, vr04, vr06                                                           // PartRes cur1 right1 [  290] 

       VSADBW.1          vr02, vr03, vr09                                                            // PartRes cur0 left0 [  292] 
       VSADBAW.1         vr02, vr04, vr12                                                            // PartRes cur1 left3 [  293] 
       VSADBW.2          vr02, vr03, vr13                                                            // PartRes cur0 zero0 [  294] 
       VSADBAW.2         vr02, vr04, vr17                                                            // PartRes cur1 zero3 [  295] 

       VMRB              vr16, vr21, vr20                                                  // zero2 inputright2 inputleft2 [  297] ....................// shift by one pel
       VMR1W             vr07, vr21, vr20                                                 // right2 inputright2 inputleft2 [  298] ....................// shift by two pels

       VSADBW.4          vr02, vr03, vr05                                                           // PartRes cur0 right0 [  300] 
       VSADBAW.4         vr02, vr04, vr08                                                           // PartRes cur1 right3 [  301] 


       VAVRB             vr07, vr07, vr16                                                           // right2 right2 zero2 [  304] ....................// average with previous data
       VAVRB             vr11, vr20, vr16                                                        // left2 inputleft2 zero2 [  305] 
//  (-1/2,  1) (0, 1),  (1/2, 1)   in 2th index

       VAVRB             vr05, vr07, vr06                                                          // right0 right2 right1 [  308] 
       VAVRB             vr09, vr11, vr10                                                             // left0 left2 left1 [  309] 
       VAVRB             vr13, vr16, vr15                                                             // zero0 zero2 zero1 [  310] 
// now    (-1/2,  3/2) (0, 3/2),  (1/2, 3/2) in 0
       VSADBW.32         vr02, vr03, vr12                                                            // PartRes cur0 left3 [  312] 
       VSADBAW.32        vr02, vr04, vr09                                                            // PartRes cur1 left0 [  313] 
       VSADBW.64         vr02, vr03, vr17                                                            // PartRes cur0 zero3 [  314] 
       VSADBAW.64        vr02, vr04, vr13                                                            // PartRes cur1 zero0 [  315] 
       VSADBW.128        vr02, vr03, vr08                                                           // PartRes cur0 right3 [  316] 
       VSADBAW.128       vr02, vr04, vr05                                                           // PartRes cur1 right0 [  317] 


       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  320] 
// decrement and loop
       VJD.i14           i14, r8                                                                    // row row SubHalfLoop [  322] 
// to here loop
       VLD128            vr03,[i9, 0]  //DS___________________________________________________________________ cur0 CirPtr [  324] 
       VIM               i9, i9, PCB_CURRENT_STRIDE  //DS___________________________________________________ CirPtr CirPtr [  325] 
       VADDW.255         vr14, vr14, vr02  //DS_____________________________________________________ result result PartRes [  326] 
// End                                                                                                                  // [  327] 

  ld r9,[r17,0];                                                                                       // VSTACK endofIter [  329] 
LINE 329".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VJL               vr31, r9                                                                                       // [  329] 

       VNOP                //DS___________________________________________________________________________________________ [  331] 
       VNOP                //DS___________________________________________________________________________________________ [  332] 
       VNOP                //DS___________________________________________________________________________________________ [  333] 
 
 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz HalfPelME_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc HalfPelME                                                                                                    // [  336] ....................//HalfPelME



// [Macro] SAD_calc                                                                                                     // [  338] 
_Asm int Record_SAD_calc(int *mac_tab)
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
SAD_calc_RecLoop:
  ld r1,[r17 ,12]
  vrec r1

// Begin                                                                                                                // [  339] 
//vec16  vr03  cur0                                                                                                     // [  340] 
//vec16  vr04  ref0                                                                                                     // [  340] 
// s16    vr05.1  skipsign                                                                                              // [  341] 
// s16    vr06.1  Bestsign                                                                                              // [  341] 
// s16    vr07.1  topbit                                                                                                // [  341] 
// p16    i14  counter                                                                                                  // [  342] 
//vec16  vr29  skip.1                                                                                                   // [  343] 
//vec16  vr29  skip_cost.1                                                                                              // [  343] 
//vec16  vr30  final.13                                                                                                 // [  344] 
//vec16  vr30  final_cost.1                                                                                             // [  344] 
//vec16  vr30  final_vecx.4                                                                                             // [  344] 
//vec16  vr30  final_vecy.8                                                                                             // [  344] 
LINE 345".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VDMAWAIT          0x0, 0x7f                                                                                      // [  345] 
       VMOVW             vr07, 0x8000, 1                                                                         // topbit [  346] 
       VMOVW             vr01, 16-1, 64                                                                         // counter [  347] 
       VXMINW.1          vr29, vr00, 128                                                               // skip_cost lambda [  348] 
       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [  349] 
       VIM               i13, i4, MPO_PixelCoeffBuffer+PCB_REFERENCE_Y                                    // RefPtr CirBuf [  350] 

// skip vector will have Motion bits of 2 in general, so use 2*lambda

       VADDW.1           vr29, vr29, vr29                                                 // skip_cost skip_cost skip_cost [  354] ....................// double it

//ThisLoop --------                                                                                                     // [  356] 
 vendrec r3                                                                                                             // [  356] 
 vrec r3                                                                                                                // [  356] 
LINE 357".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  357] 
       VLD128            vr04,[i13, 0]                                                                      // ref0 RefPtr [  358] 

       VJD.i14           i14, r3                                                               // counter counter ThisLoop [  360] 
       VIM               i9, i9, PCB_CURRENT_STRIDE  //DS___________________________________________________ CirPtr CirPtr [  361] 
       VIM               i13, i13, PCB_REFERENCE_STRIDE  //DS_______________________________________________ RefPtr RefPtr [  362] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 363".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VSADBAW.1         vr29, vr03, vr04  //DS_______________________________________________________ skip_cost cur0 ref0 [  363] 

       VXOR.1            vr05, vr29, vr07                                                     // skipsign skip_cost topbit [  365] 
       VXOR.1            vr06, vr30, vr07                                                    // Bestsign final_cost topbit [  366] 

              //  Stall: [vr06]  ...............................      vr06.001
       VLEW.1            i14, vr05, vr06                                                      // counter skipsign Bestsign [  368] ....................// set flag if skip is better than best
              //  Stall: [i14]  ................................      I14  
       VJP!i14           r4                                                                                 // counter End [  369] 
       VNOP                //DS___________________________________________________________________________________________ [  370] 
       VNOP                //DS___________________________________________________________________________________________ [  371] 
       VNOP                //DS___________________________________________________________________________________________ [  372] 
       VMVW              vr30, vr29, 1                                                                       // final skip [  373] 
       VMVW              vr30, vr00, 12                                                                           // final [  374] ....................// save input vecs


//End --------                                                                                                          // [  377] 
 vendrec r4                                                                                                             // [  377] 
 vrec r4                                                                                                                // [  377] 
// End                                                                                                                  // [  378] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz SAD_calc_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SAD_calc                                                                                                     // [  380] 



// [Macro] Set_Skip_Large                                                                                               // [  382] 
_Asm int Record_Set_Skip_Large(int *mac_tab)
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

// Begin                                                                                                                // [  383] 

//vec16  vr30  skipres                                                                                                  // [  385] ....................// to avoid conficts
LINE 386".\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss"
       VMOVW             vr30, 0xFFFF, 1                                                                        // skipres [  386] 
       VMOVW             vr30, 0x0, 12                                                                          // skipres [  387] ....................// zero vectors
// End                                                                                                                  // [  388] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Set_Skip_Large                                                                                               // [  389] 






const int MACRO_IDX_HalfPelME = 2;
const int MACRO_IDX_SAD_calc = 3;
const int MACRO_IDX_Set_Skip_Large = 4;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Software_SubME_Record(int sa, int SIMDnum)
{
    int *map = Software_SubME;

    SetSCQ(SIMDnum);

    map[0] = Software_ME[1];

    map[1] = Software_ME[2];

    map[MACRO_IDX_HalfPelME] = sa;
    sa = Record_HalfPelME(map);

    map[MACRO_IDX_SAD_calc] = sa;
    sa = Record_SAD_calc(map);

    map[MACRO_IDX_Set_Skip_Large] = sa;
    sa = Record_Set_Skip_Large(map);

    return sa;
}
