//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss'

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
#include ".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.h"


// Macro table
    int Software_QPelME[8];
    int Software_QPelME_Record(int sa, int SIMDnum);

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
#include "ArcSoftwareME_MPA.h"

extern int Software_ME[] ;
extern int Software_ME[] ;
//----------------------------------------------------------------------------------------------------
// Is Macro
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft
// p16    i0  PredX                                                                                                     // [   40] 
// p16    i1  PredY                                                                                                     // [   40] 
// p16    i2  VecX                                                                                                      // [   41] 
// p16    i3  VecY                                                                                                      // [   41] 
// ****** Informational : Register space for symbol "PatternMatch" already allocated.                                   // [   42] 
// p16    i6  PatternMatch                                                                                              // [   42] 
// p16    i7  lambda                                                                                                    // [   43] 
// p16    i8  BestCost                                                                                                  // [   44] 
// p16    i10  lastX                                                                                                    // [   45] 
// p16    i11  lastY                                                                                                    // [   45] 
// p16    i12  StepMult                                                                                                 // [   46] 
//vec16  vr14  result                                                                                                   // [   47] 
//vec16  vr02  PartRes                                                                                                  // [   48] 

// p16    i9  CirPtr                                                                                                    // [   50] 
// p16    i13  RefPtr                                                                                                   // [   50] 

//  function QuartPel_half_half
//  function to search for 1/4 motion offsets starting at point which is at 1/2 pel unit 
//  in horiz and in vert direction
// [Subroutine] QuartPelME_half_half                                                                                    // [   55] 
_Asm int Record_QuartPelME_half_half(int *mac_tab)
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


//p16  CirPtr, RefPtr



// p16    i14  row                                                                                                      // [   61] 
//vec16  vr03  cur0                                                                                                     // [   62] 
//vec16  vr04  cur1                                                                                                     // [   62] 

// Begin                                                                                                                // [   64] 


//vec16  vr05  right0                                                                                                   // [   67] 
//vec16  vr06  right1                                                                                                   // [   67] 
//vec16  vr07  right2                                                                                                   // [   67] 
//vec16  vr08  right3                                                                                                   // [   67] 
//vec16  vr09  right4                                                                                                   // [   67] 
//vec16  vr10  left0                                                                                                    // [   68] 
//vec16  vr11  left1                                                                                                    // [   68] 
//vec16  vr12  left2                                                                                                    // [   68] 
//vec16  vr13  left3                                                                                                    // [   68] 
//vec16  vr15  left4                                                                                                    // [   68] 
//vec16  vr16  zero0                                                                                                    // [   69] 
//vec16  vr17  zero1                                                                                                    // [   69] 
//vec16  vr18  zero2                                                                                                    // [   69] 
//vec16  vr19  zero3                                                                                                    // [   69] 
//vec16  vr20  inputleft1                                                                                               // [   70] 
//vec16  vr21  inputleft2                                                                                               // [   70] 
//vec16  vr22  inputright1                                                                                              // [   70] 
//vec16  vr23  inputright2                                                                                              // [   70] 

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
//|VR 9|                                                      right4                                                           |
//|VR10|                                                       left0                                                           |
//|VR11|                                                       left1                                                           |
//|VR12|                                                       left2                                                           |
//|VR13|                                                       left3                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       left4                                                           |
//|VR16|                                                       zero0                                                           |
//|VR17|                                                       zero1                                                           |
//|VR18|                                                       zero2                                                           |
//|VR19|                                                       zero3                                                           |
//|VR20|                                                  inputleft1                                                           |
//|VR21|                                                  inputleft2                                                           |
//|VR22|                                                 inputright1                                                           |
//|VR23|                                                 inputright2                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


     
LINE 75".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [   75] 


///*  The 8 points in this last step are done in this pattern :       */
///*                                                                  */
///*      res2 (-.25,-.25)   res1 (0,-.25)    res0 (.25,-.25)         */
///*      res5 (-.25,   0)                    res3 (.25,   0)         */
///*      res8 (-.25, .25)   res7 (0, .25)    res6 (.25, .25)         */
///*                                                                  */
// load 12 rows of current data (not enough vecs for all 16) 
// last 4 rows will be loaded into cur0 - cur4, done like this to avoid re-shifting reference data that
// is used for several offset calculations


// load rows of 32 pels from reference
// RefPtr set in calling function

       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [   92] 
       VLD128            vr21,[i13, 0]                                                                // inputleft2 RefPtr [   93] 
       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [   94] ....................// increment to next line
// setup loop counter
// 16 rows total, 2 rows per loop
       VMOVW             vr01, 8-1, 64                                                                              // row [   97] 
              //  Stall: [vr21]  ...............................      vr01.064      vr21    
       VMRB              vr18, vr23, vr21                                                  // zero2 inputright2 inputleft2 [   98] 
       VLD128            vr22,[i13, 16]                                                              // inputright1 RefPtr [   99] 
       VLD128            vr20,[i13, 0]                                                                // inputleft1 RefPtr [  100] 
       VSPIB             vr12, vr21, vr18                                                        // left2 inputleft2 zero2 [  101] ....................// find 3/4 pos
       VSPIB             vr07, vr18, vr21                                                       // right2 zero2 inputleft2 [  102] ....................// find 1/4 pos
       VAVRB             vr18, vr18, vr21                                                        // zero2 zero2 inputleft2 [  103] ....................// find 1/2 pos

// from here loop

//SubHHLoop --------                                                                                                    // [  107] 
 vendrec r3                                                                                                             // [  107] 
 vrec r3                                                                                                                // [  107] 

// load next row
LINE 110".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  110] 

       VMRB              vr17, vr22, vr20                                                  // zero1 inputright1 inputleft1 [  112] 

       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  114] ....................// increment ready for next read
       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  115] 
       VSPIB             vr11, vr20, vr17                                                        // left1 inputleft1 zero1 [  116] ....................// find 3/4 pos
       VSPIB             vr06, vr17, vr20                                                       // right1 zero1 inputleft1 [  117] ....................// find 1/4 pos
       VAVRB             vr17, vr17, vr20                                                        // zero1 zero1 inputleft1 [  118] ....................// find 1/2 pos

       VAVRB             vr15, vr11, vr12                                                             // left4 left1 left2 [  120] ....................// (1/4, 1/2)
       VAVRB             vr09, vr06, vr07                                                          // right4 right1 right2 [  121] ....................// (3/4, 1/2)

// combine vertically  3:1
       VSPIB             vr10, vr12, vr11                                                             // left0 left2 left1 [  124] ....................// (1/4, 1/4)
       VSPIB             vr16, vr18, vr17                                                             // zero0 zero2 zero1 [  125] ....................// (1/2, 1/4)
       VSPIB             vr05, vr07, vr06                                                          // right0 right2 right1 [  126] ....................// (3/4, 1/4)

// combine vertically 1:3
       VSPIB             vr13, vr11, vr12                                                             // left3 left1 left2 [  129] ....................// (1/4, 3/4)
       VSPIB             vr19, vr17, vr18                                                             // zero3 zero1 zero2 [  130] ....................// (1/2, 3/4)
       VSPIB             vr08, vr06, vr07                                                          // right3 right1 right2 [  131] ....................// (3/4, 3/4)
// accumulate SAD
       VSADBW.1          vr02, vr03, vr10                                                            // PartRes cur0 left0 [  133] 
       VSADBW.2          vr02, vr03, vr16                                                            // PartRes cur0 zero0 [  134] 
       VSADBW.4          vr02, vr03, vr05                                                           // PartRes cur0 right0 [  135] 
       VSADBW.8          vr02, vr03, vr15                                                            // PartRes cur0 left4 [  136] 
       VSADBW.16         vr02, vr03, vr09                                                           // PartRes cur0 right4 [  137] 
// accumulate SAD
       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [  139] 
       VLD128            vr21,[i13, 0]                                                                // inputleft2 RefPtr [  140] 

       VSADBW.32         vr02, vr03, vr13                                                            // PartRes cur0 left3 [  142] 
       VSADBW.64         vr02, vr03, vr19                                                            // PartRes cur0 zero3 [  143] 
       VSADBW.128        vr02, vr03, vr08                                                           // PartRes cur0 right3 [  144] 
// end of first test row for all 8 positions


       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  148] 

       VMRB              vr18, vr23, vr21                                                  // zero2 inputright2 inputleft2 [  150] 
       VADDW.255         vr14, vr14, vr02                                                         // result result PartRes [  151] 
// load next row
       VLD128            vr04,[i9, 0]                                                                       // cur1 CirPtr [  153] 
       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  154] 
       VSPIB             vr12, vr21, vr18                                                        // left2 inputleft2 zero2 [  155] ....................// find 3/4 pos
       VSPIB             vr07, vr18, vr21                                                       // right2 zero2 inputleft2 [  156] ....................// find 1/4 pos
       VAVRB             vr18, vr18, vr21                                                        // zero2 zero2 inputleft2 [  157] ....................// find 1/2 pos

       VAVRB             vr15, vr11, vr12                                                             // left4 left1 left2 [  159] ....................// (1/4, 1/2)
       VAVRB             vr09, vr06, vr07                                                          // right4 right1 right2 [  160] ....................// (3/4, 1/2)

// combine vertically  3:1
       VSPIB             vr10, vr11, vr12                                                             // left0 left1 left2 [  163] ....................// (1/4, 1/4)
       VSPIB             vr16, vr17, vr18                                                             // zero0 zero1 zero2 [  164] ....................// (1/2, 1/4)
       VSPIB             vr05, vr06, vr07                                                          // right0 right1 right2 [  165] ....................// (3/4, 1/4)
// combine vertically 1:3
       VSPIB             vr13, vr12, vr11                                                             // left3 left2 left1 [  167] ....................// (1/4, 3/4)
       VSPIB             vr19, vr18, vr17                                                             // zero3 zero2 zero1 [  168] ....................// (1/2, 3/4)
       VSPIB             vr08, vr07, vr06                                                          // right3 right2 right1 [  169] ....................// (3/4, 3/4)
// accumulate SAD
       VSADBW.1          vr02, vr04, vr10                                                            // PartRes cur1 left0 [  171] 
       VSADBW.2          vr02, vr04, vr16                                                            // PartRes cur1 zero0 [  172] 
       VSADBW.4          vr02, vr04, vr05                                                           // PartRes cur1 right0 [  173] 

       VSADBW.8          vr02, vr04, vr15                                                            // PartRes cur1 left4 [  175] 
       VSADBW.16         vr02, vr04, vr09                                                           // PartRes cur1 right4 [  176] 
// accumulate SAD
       VSADBW.32         vr02, vr04, vr13                                                            // PartRes cur1 left3 [  178] 
       VSADBW.64         vr02, vr04, vr19                                                            // PartRes cur1 zero3 [  179] 
       VSADBW.128        vr02, vr04, vr08                                                           // PartRes cur1 right3 [  180] 
// end of second test row for all 8 positions

// decrement and loop
       VJD.i14           i14, r3                                                                      // row row SubHHLoop [  184] 
// to here loop


       VLD128            vr22,[i13, 16]  //DS__________________________________________________________ inputright1 RefPtr [  188] 
       VLD128            vr20,[i13, 0]  //DS____________________________________________________________ inputleft1 RefPtr [  189] 
       VADDW.255         vr14, vr14, vr02  //DS_____________________________________________________ result result PartRes [  190] 


    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  194] 
       VNOP                //DS___________________________________________________________________________________________ [  195] 
       VNOP                //DS___________________________________________________________________________________________ [  196] 
       VNOP                //DS___________________________________________________________________________________________ [  197] 
// End                                                                                                                  // [  198] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QuartPelME_half_half                                                                                         // [  200] 


//  function QuartPel_half_int
//  function to search for 1/4 motion offsets starting at point which is at integer pel unit 
//  in vert direction and 1/2 pel in horizontal direction
// [Subroutine] QuartPelME_half_int                                                                                     // [  204] 
_Asm int Record_QuartPelME_half_int(int *mac_tab)
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



//p16  CirPtr, RefPtr

// p16    i14  row                                                                                                      // [  209] 

// Begin                                                                                                                // [  211] 


//vec16  vr03  cur0                                                                                                     // [  214] 
//vec16  vr04  cur1                                                                                                     // [  214] 
//vec16  vr05  right0                                                                                                   // [  215] 
//vec16  vr06  right1                                                                                                   // [  215] 
//vec16  vr07  right2                                                                                                   // [  215] 
//vec16  vr08  right3                                                                                                   // [  215] 
//vec16  vr09  right4                                                                                                   // [  215] 
//vec16  vr10  left0                                                                                                    // [  216] 
//vec16  vr11  left1                                                                                                    // [  216] 
//vec16  vr12  left2                                                                                                    // [  216] 
//vec16  vr13  left3                                                                                                    // [  216] 
//vec16  vr15  left4                                                                                                    // [  216] 
//vec16  vr16  zero0                                                                                                    // [  217] 
//vec16  vr17  zero1                                                                                                    // [  217] 
//vec16  vr18  zero2                                                                                                    // [  217] 
//vec16  vr19  zero3                                                                                                    // [  217] 
//vec16  vr20  inputleft1                                                                                               // [  218] 
//vec16  vr21  inputleft2                                                                                               // [  218] 
//vec16  vr22  inputright1                                                                                              // [  218] 
//vec16  vr23  inputright2                                                                                              // [  218] 

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
//|VR 9|                                                      right4                                                           |
//|VR10|                                                       left0                                                           |
//|VR11|                                                       left1                                                           |
//|VR12|                                                       left2                                                           |
//|VR13|                                                       left3                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       left4                                                           |
//|VR16|                                                       zero0                                                           |
//|VR17|                                                       zero1                                                           |
//|VR18|                                                       zero2                                                           |
//|VR19|                                                       zero3                                                           |
//|VR20|                                                  inputleft1                                                           |
//|VR21|                                                  inputleft2                                                           |
//|VR22|                                                 inputright1                                                           |
//|VR23|                                                 inputright2                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


     
//vmovw        'RefPtr,SDM_ME_REFERENCE_ARRAY
    // rowLoop start
LINE 225".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [  225] 


///*  The 8 points in this last step are done in this pattern :       */
///*                                                                  */
///*      res2 (-.25,-.25)   res1 (0,-.25)    res0 (.25,-.25)         */
///*      res5 (-.25,   0)                    res3 (.25,   0)         */
///*      res8 (-.25, .25)   res7 (0, .25)    res6 (.25, .25)         */
///*                                                                  */
// load 12 rows of current data (not enough vecs for all 16) 
// last 4 rows will be loaded into cur0 - cur4, done like this to avoid re-shifting reference data that
// is used for several offset calculations

       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [  238] 
       VLD128            vr21,[i13, 0]                                                                // inputleft2 RefPtr [  239] 
// setup loop counter
// 16 rows total, 2 rows per loop    

       VLD128            vr22,[i13, 48]                                                              // inputright1 RefPtr [  243] 
       VLD128            vr20,[i13, 32]                                                               // inputleft1 RefPtr [  244] 
       VMOVW             vr01, 8-1, 64                                                                              // row [  245] 
       VMRB              vr18, vr23, vr21                                                  // zero2 inputright2 inputleft2 [  246] 
//left2, zero2, right2 now are at (1/4,0), (1/2,0) and (3/4, 0)



       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE*2                                            // RefPtr RefPtr [  251] 


       VMRB              vr17, vr22, vr20                                                  // zero1 inputright1 inputleft1 [  254] 
       VSPIB             vr12, vr21, vr18                                                        // left2 inputleft2 zero2 [  255] ....................// find 3/4 pos
       VSPIB             vr07, vr18, vr21                                                       // right2 zero2 inputleft2 [  256] ....................// find 1/4 pos

       VSPIB             vr11, vr20, vr17                                                        // left1 inputleft1 zero1 [  258] ....................// find 3/4 pos
       VSPIB             vr06, vr17, vr20                                                       // right1 zero1 inputleft1 [  259] ....................// find 1/4 pos
       VAVRB             vr17, vr17, vr20                                                        // zero1 zero1 inputleft1 [  260] ....................// find 1/2 pos    
// left1, zero1, right1 now are at (1/4,1), (1/2,1)  and (3/4,1)
       VAVRB             vr18, vr18, vr21                                                        // zero2 zero2 inputleft2 [  262] ....................// find 1/2 pos

//SubHILoop --------                                                                                                    // [  264] 
 vendrec r3                                                                                                             // [  264] 
 vrec r3                                                                                                                // [  264] 





LINE 270".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [  270] 
       VLD128            vr21,[i13, 0]                                                                // inputleft2 RefPtr [  271] 

       VSPIB             vr10, vr11, vr12                                                             // left0 left1 left2 [  273] ....................// (1/4, 3/4)
       VSPIB             vr16, vr17, vr18                                                             // zero0 zero1 zero2 [  274] ....................// (1/2, 3/4)
       VSPIB             vr05, vr06, vr07                                                          // right0 right1 right2 [  275] ....................// (3/4, 3/4)

// combine vertically 1:3


       VMRB              vr18, vr23, vr21                                                  // zero2 inputright2 inputleft2 [  280] 

// load next row
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  283] 
       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  284] ....................// increment ready for next read

       VSPIB             vr12, vr21, vr18                                                        // left2 inputleft2 zero2 [  286] ....................// find 3/4 pos
       VSPIB             vr07, vr18, vr21                                                       // right2 zero2 inputleft2 [  287] ....................// find 1/4 pos
       VAVRB             vr18, vr18, vr21                                                        // zero2 zero2 inputleft2 [  288] ....................// find 1/2 pos

// accumulate SAD
       VSADBW.1          vr02, vr03, vr10                                                            // PartRes cur0 left0 [  291] 
       VSADBW.2          vr02, vr03, vr16                                                            // PartRes cur0 zero0 [  292] 
       VSADBW.4          vr02, vr03, vr05                                                           // PartRes cur0 right0 [  293] 

// combine vertically  3:1
       VSPIB             vr13, vr11, vr12                                                             // left3 left1 left2 [  296] ....................// (1/4, 1/4)
       VSPIB             vr19, vr17, vr18                                                             // zero3 zero1 zero2 [  297] ....................// (1/2, 1/4)
       VSPIB             vr08, vr06, vr07                                                          // right3 right1 right2 [  298] ....................// (3/4, 1/4)
// accumulate SAD


       VSADBW.8          vr02, vr03, vr11                                                            // PartRes cur0 left1 [  302] 
       VSADBW.16         vr02, vr03, vr06                                                           // PartRes cur0 right1 [  303] 
       VSADBW.32         vr02, vr03, vr13                                                            // PartRes cur0 left3 [  304] 
       VSADBW.64         vr02, vr03, vr19                                                            // PartRes cur0 zero3 [  305] 
       VSADBW.128        vr02, vr03, vr08                                                           // PartRes cur0 right3 [  306] 

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  308] 
       VLD128            vr04,[i9, 0]                                                                       // cur1 CirPtr [  309] ....................// ready for next row
       VADDW.255         vr14, vr14, vr02                                                         // result result PartRes [  310] 

// end of first test row for all 8 positions

       VLD128            vr22,[i13, 16]                                                              // inputright1 RefPtr [  314] 
       VLD128            vr20,[i13, 0]                                                                // inputleft1 RefPtr [  315] 
// combine vertically  3:1
       VSPIB             vr10, vr12, vr11                                                             // left0 left2 left1 [  317] ....................// (1/4, 1/4)
       VSPIB             vr16, vr18, vr17                                                             // zero0 zero2 zero1 [  318] ....................// (1/2, 1/4)
       VSPIB             vr05, vr07, vr06                                                          // right0 right2 right1 [  319] ....................// (3/4, 1/4)

       VMRB              vr17, vr22, vr20                                                  // zero1 inputright1 inputleft1 [  321] 


       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  324] ....................// increment ready for next read

              //  Stall: [vr17]  ...............................      vr17          I09  
       VSPIB             vr11, vr20, vr17                                                        // left1 inputleft1 zero1 [  326] ....................// find 3/4 pos
       VSPIB             vr06, vr17, vr20                                                       // right1 zero1 inputleft1 [  327] ....................// find 1/4 pos
       VAVRB             vr17, vr17, vr20                                                        // zero1 zero1 inputleft1 [  328] ....................// find 1/2 pos


// accumulate SAD
       VSADBW.1          vr02, vr04, vr10                                                            // PartRes cur1 left0 [  332] 
       VSADBW.2          vr02, vr04, vr16                                                            // PartRes cur1 zero0 [  333] 
       VSADBW.4          vr02, vr04, vr05                                                           // PartRes cur1 right0 [  334] 

// combine vertically  3:1
       VSPIB             vr08, vr07, vr06                                                          // right3 right2 right1 [  337] ....................// (3/4, 1/4)
       VSPIB             vr13, vr12, vr11                                                             // left3 left2 left1 [  338] ....................// (1/4, 1/4)
       VSPIB             vr19, vr18, vr17                                                             // zero3 zero2 zero1 [  339] ....................// (1/2, 1/4)
// accumulate SAD
       VSADBW.8          vr02, vr04, vr12                                                            // PartRes cur1 left2 [  341] 
       VSADBW.16         vr02, vr04, vr07                                                           // PartRes cur1 right2 [  342] 
// accumulate SAD
       VSADBW.32         vr02, vr04, vr13                                                            // PartRes cur1 left3 [  344] 
       VSADBW.64         vr02, vr04, vr19                                                            // PartRes cur1 zero3 [  345] 
       VSADBW.128        vr02, vr04, vr08                                                           // PartRes cur1 right3 [  346] 
// end of second test row for all 8 positions


// decrement and loop
       VJD.i14           i14, r3                                                                      // row row SubHILoop [  351] 

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE  //DS__________________________________________ RefPtr RefPtr [  353] 
       VADDW.255         vr14, vr14, vr02  //DS_____________________________________________________ result result PartRes [  354] 
       VNOP                //DS___________________________________________________________________________________________ [  355] 


    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  359] 
       VNOP                //DS___________________________________________________________________________________________ [  360] 
       VNOP                //DS___________________________________________________________________________________________ [  361] 
       VNOP                //DS___________________________________________________________________________________________ [  362] 
// End                                                                                                                  // [  363] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QuartPelME_half_int                                                                                          // [  365] 




//  function QuartPel_int_half
//  function to search for 1/4 motion offsets starting at point which is at half pel unit 
//  in vert direction and integer pel in horizontal direction
// [Subroutine] QuartPelME_int_half                                                                                     // [  371] 
_Asm int Record_QuartPelME_int_half(int *mac_tab)
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



//p16  CirPtr, RefPtr

// p16    i14  row                                                                                                      // [  376] 

// Begin                                                                                                                // [  378] 


//vec16  vr03  cur0                                                                                                     // [  381] 
//vec16  vr04  cur1                                                                                                     // [  381] 
//vec16  vr05  right0                                                                                                   // [  382] 
//vec16  vr06  right1                                                                                                   // [  382] 
//vec16  vr07  right2                                                                                                   // [  382] 
//vec16  vr08  right3                                                                                                   // [  382] 
//vec16  vr09  right4                                                                                                   // [  382] 
//vec16  vr10  left0                                                                                                    // [  383] 
//vec16  vr11  left1                                                                                                    // [  383] 
//vec16  vr12  left2                                                                                                    // [  383] 
//vec16  vr13  left3                                                                                                    // [  383] 
//vec16  vr15  left4                                                                                                    // [  383] 
//vec16  vr16  zero0                                                                                                    // [  384] 
//vec16  vr17  zero1                                                                                                    // [  384] 
//vec16  vr18  zero2                                                                                                    // [  384] 
//vec16  vr19  zero3                                                                                                    // [  384] 
//vec16  vr20  inputleft1                                                                                               // [  385] 
//vec16  vr21  inputright1                                                                                              // [  385] 
//vec16  vr22  inputleft2                                                                                               // [  385] 
//vec16  vr23  inputright2                                                                                              // [  385] 
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
//|VR 9|                                                      right4                                                           |
//|VR10|                                                       left0                                                           |
//|VR11|                                                       left1                                                           |
//|VR12|                                                       left2                                                           |
//|VR13|                                                       left3                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       left4                                                           |
//|VR16|                                                       zero0                                                           |
//|VR17|                                                       zero1                                                           |
//|VR18|                                                       zero2                                                           |
//|VR19|                                                       zero3                                                           |
//|VR20|                                                  inputleft1                                                           |
//|VR21|                                                 inputright1                                                           |
//|VR22|                                                  inputleft2                                                           |
//|VR23|                                                 inputright2                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


    


///*  The 8 points in this last step are done in this pattern :       */
///*                                                                  */
///*      res2 (-.25,-.25)   res1 (0,-.25)    res0 (.25,-.25)         */
///*      res5 (-.25,   0)                    res3 (.25,   0)         */
///*      res8 (-.25, .25)   res7 (0, .25)    res6 (.25, .25)         */
///*                                                                  */
// load 12 rows of current data (not enough vecs for all 16) 
// last 4 rows will be loaded into cur0 - cur4, done like this to avoid re-shifting reference data that
// is used for several offset calculations

LINE 401".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [  401] 
       VLD128            vr22,[i13, 0]                                                                // inputleft2 RefPtr [  402] 

       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [  404] 
       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  405] 
              //  Stall: [vr22]  ...............................      vr22          I13  
       VMRB              vr18, vr23, vr22                                                  // zero2 inputright2 inputleft2 [  406] 
       VMR1W             vr07, vr23, vr22                                                 // right2 inputright2 inputleft2 [  407] 
// setup loop counter
// 16 rows total, 2 rows per loop    
       VMOVW             vr01, 8-1, 64                                                                              // row [  410] 
       VSPIB             vr12, vr18, vr22                                                        // left2 zero2 inputleft2 [  411] ....................// find 1/4 pos
       VSPIB             vr07, vr18, vr07                                                           // right2 zero2 right2 [  412] ....................// find 3/4 pos

//SubIHLoop --------                                                                                                    // [  414] 
 vendrec r3                                                                                                             // [  414] 
 vrec r3                                                                                                                // [  414] 

// load next row

LINE 418".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VLD128            vr21,[i13, 16]                                                              // inputright1 RefPtr [  418] 
       VLD128            vr20,[i13, 0]                                                                // inputleft1 RefPtr [  419] 
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  420] 
       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  421] ....................// increment ready for next read

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  423] 

       VMRB              vr17, vr21, vr20                                                  // zero1 inputright1 inputleft1 [  425] 
       VMR1W             vr06, vr21, vr20                                                 // right1 inputright1 inputleft1 [  426] 


       VLD128            vr23,[i13, 16]                                                              // inputright2 RefPtr [  429] 
       VLD128            vr22,[i13, 0]                                                                // inputleft2 RefPtr [  430] 

       VSPIB             vr16, vr18, vr17                                                             // zero0 zero2 zero1 [  432] ....................// (  0, 1/4)

       VSPIB             vr06, vr17, vr06                                                           // right1 zero1 right1 [  434] ....................// find 1/4 pos
       VSPIB             vr11, vr17, vr20                                                        // left1 zero1 inputleft1 [  435] ....................// find 3/4 pos   


       VSPIB             vr19, vr17, vr18                                                             // zero3 zero1 zero2 [  438] ....................// (  0, 3/4)

       VSPIB             vr05, vr07, vr06                                                          // right0 right2 right1 [  440] ....................// (3/4, 1/4)
       VSPIB             vr10, vr12, vr11                                                             // left0 left2 left1 [  441] ....................// (1/4, 1/4)
       VAVRB             vr09, vr06, vr07                                                          // right4 right1 right2 [  442] ....................// find 1/2 pos
       VAVRB             vr15, vr11, vr12                                                             // left4 left1 left2 [  443] ....................// find 1/2 pos
       VSPIB             vr08, vr06, vr07                                                          // right3 right1 right2 [  444] ....................// (3/4, 3/4)
       VSPIB             vr13, vr11, vr12                                                             // left3 left1 left2 [  445] ....................// (1/4, 3/4)


       VSADBW.1          vr02, vr03, vr10                                                            // PartRes cur0 left0 [  448] 
       VSADBW.2          vr02, vr03, vr16                                                            // PartRes cur0 zero0 [  449] 
       VSADBW.4          vr02, vr03, vr05                                                           // PartRes cur0 right0 [  450] 
       VSADBW.8          vr02, vr03, vr15                                                            // PartRes cur0 left4 [  451] 
       VSADBW.16         vr02, vr03, vr09                                                           // PartRes cur0 right4 [  452] 
       VSADBW.32         vr02, vr03, vr13                                                            // PartRes cur0 left3 [  453] 
       VSADBW.64         vr02, vr03, vr19                                                            // PartRes cur0 zero3 [  454] 
       VSADBW.128        vr02, vr03, vr08                                                           // PartRes cur0 right3 [  455] 
// end of first test line

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  458] 

       VLD128            vr04,[i9, 0]                                                                       // cur1 CirPtr [  460] 

       VMRB              vr18, vr23, vr22                                                  // zero2 inputright2 inputleft2 [  462] 
       VMR1W             vr07, vr23, vr22                                                 // right2 inputright2 inputleft2 [  463] 


       VADDW.255         vr14, vr14, vr02                                                         // result result PartRes [  466] 

       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  468] ....................// increment ready for next read

       VSPIB             vr07, vr18, vr07                                                           // right2 zero2 right2 [  470] ....................// find 1/4 pos
       VSPIB             vr12, vr18, vr22                                                        // left2 zero2 inputleft2 [  471] ....................// find 3/4 pos

       VSPIB             vr16, vr17, vr18                                                             // zero0 zero1 zero2 [  473] ....................// (  0, 1/4)
       VSPIB             vr05, vr06, vr07                                                          // right0 right1 right2 [  474] ....................// (3/4, 1/4)
       VSPIB             vr10, vr11, vr12                                                             // left0 left1 left2 [  475] ....................// (1/4, 1/4)

       VAVRB             vr09, vr07, vr06                                                          // right4 right2 right1 [  477] ....................// find 1/2 pos
       VAVRB             vr15, vr12, vr11                                                             // left4 left2 left1 [  478] ....................// find 1/2 pos

       VSPIB             vr08, vr07, vr06                                                          // right3 right2 right1 [  480] ....................// (3/4, 3/4)
       VSPIB             vr13, vr12, vr11                                                             // left3 left2 left1 [  481] ....................// (1/4, 3/4)
       VSPIB             vr19, vr18, vr17                                                             // zero3 zero2 zero1 [  482] ....................// (  0, 3/4)


       VSADBW.1          vr02, vr04, vr10                                                            // PartRes cur1 left0 [  485] 
       VSADBW.2          vr02, vr04, vr16                                                            // PartRes cur1 zero0 [  486] 
       VSADBW.4          vr02, vr04, vr05                                                           // PartRes cur1 right0 [  487] 
       VSADBW.8          vr02, vr04, vr15                                                            // PartRes cur1 left4 [  488] 
       VSADBW.16         vr02, vr04, vr09                                                           // PartRes cur1 right4 [  489] 
       VSADBW.32         vr02, vr04, vr13                                                            // PartRes cur1 left3 [  490] 
       VSADBW.64         vr02, vr04, vr19                                                            // PartRes cur1 zero3 [  491] 
// end of second test line

// decrement and loop
       VJD.i14           i14, r3                                                                      // row row SubIHLoop [  495] 
       VSADBW.128        vr02, vr04, vr08  //DS_______________________________________________________ PartRes cur1 right3 [  496] 
       VNOP                //DS___________________________________________________________________________________________ [  497] 
              //  Stall: [vr02]  ...............................      vr02.128
       VADDW.255         vr14, vr14, vr02  //DS_____________________________________________________ result result PartRes [  498] 

  
    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  502] 
       VNOP                //DS___________________________________________________________________________________________ [  503] 
       VNOP                //DS___________________________________________________________________________________________ [  504] 
       VNOP                //DS___________________________________________________________________________________________ [  505] 
// End                                                                                                                  // [  506] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QuartPelME_int_half                                                                                          // [  508] 




//  function QuartPel_int_int
//  function to search for 1/4 motion offsets starting at point which is at integer pel unit 
//  in vert direction and integer pel in horizontal direction
// [Subroutine] QuartPelME_int_int                                                                                      // [  514] 
_Asm int Record_QuartPelME_int_int(int *mac_tab)
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



//p16  CirPtr, RefPtr

// p16    i14  row                                                                                                      // [  519] 

// Begin                                                                                                                // [  521] 


//vec16  vr03  cur0                                                                                                     // [  524] 
//vec16  vr04  cur1                                                                                                     // [  524] 
//vec16  vr05  right0                                                                                                   // [  525] 
//vec16  vr06  right1                                                                                                   // [  525] 
//vec16  vr07  right2                                                                                                   // [  525] 
//vec16  vr08  right3                                                                                                   // [  525] 
//vec16  vr09  right4                                                                                                   // [  525] 
//vec16  vr10  right5                                                                                                   // [  525] 
//vec16  vr11  left0                                                                                                    // [  526] 
//vec16  vr12  left1                                                                                                    // [  526] 
//vec16  vr13  left2                                                                                                    // [  526] 
//vec16  vr15  left3                                                                                                    // [  526] 
//vec16  vr16  left4                                                                                                    // [  526] 
//vec16  vr17  left5                                                                                                    // [  526] 
//vec16  vr18  zero0                                                                                                    // [  527] 
//vec16  vr19  zero1                                                                                                    // [  527] 
//vec16  vr20  zero2                                                                                                    // [  527] 
//vec16  vr21  zero3                                                                                                    // [  527] 
//vec16  vr22  zero4                                                                                                    // [  527] 
//vec16  vr23  zero5                                                                                                    // [  527] 
//vec16  vr24  inputleft1                                                                                               // [  528] 
//vec16  vr25  inputright1                                                                                              // [  528] 
//vec16  vr26  inputleft2                                                                                               // [  529] 
//vec16  vr27  inputright2                                                                                              // [  529] 

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
//|VR 9|                                                      right4                                                           |
//|VR10|                                                      right5                                                           |
//|VR11|                                                       left0                                                           |
//|VR12|                                                       left1                                                           |
//|VR13|                                                       left2                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       left3                                                           |
//|VR16|                                                       left4                                                           |
//|VR17|                                                       left5                                                           |
//|VR18|                                                       zero0                                                           |
//|VR19|                                                       zero1                                                           |
//|VR20|                                                       zero2                                                           |
//|VR21|                                                       zero3                                                           |
//|VR22|                                                       zero4                                                           |
//|VR23|                                                       zero5                                                           |
//|VR24|                                                  inputleft1                                                           |
//|VR25|                                                 inputright1                                                           |
//|VR26|                                                  inputleft2                                                           |
//|VR27|                                                 inputright2                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


     
//vmovw        'RefPtr,SDM_ME_REFERENCE_ARRAY
    // rowLoop start
LINE 536".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VIM               i9, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                       // CirPtr CirBuf [  536] 


///*  The 8 points in this last step are done in this pattern :       */
///*                                                                  */
///*      res2 (-.25,-.25)   res1 (0,-.25)    res0 (.25,-.25)         */
///*      res5 (-.25,   0)                    res3 (.25,   0)         */
///*      res8 (-.25, .25)   res7 (0, .25)    res6 (.25, .25)         */
///*                                                                  */


       VLD128            vr27,[i13, 16]                                                              // inputright2 RefPtr [  547] 
       VLD128            vr26,[i13, 0]                                                                // inputleft2 RefPtr [  548] 

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  550] 
// load next row
       VLD128            vr03,[i9, 0]                                                                       // cur0 CirPtr [  552] 

              //  Stall: [vr26]  ...............................      vr03          vr26    
       VMRB              vr20, vr27, vr26                                                  // zero2 inputright2 inputleft2 [  554] 
       VMR1W             vr07, vr27, vr26                                                 // right2 inputright2 inputleft2 [  555] 

       VLD128            vr25,[i13, 16]                                                              // inputright1 RefPtr [  557] 
       VLD128            vr24,[i13, 0]                                                                // inputleft1 RefPtr [  558] 

       VSPIB             vr07, vr20, vr07                                                           // right2 zero2 right2 [  560] ....................// find 1/4 pos
       VSPIB             vr13, vr20, vr26                                                        // left2 zero2 inputleft2 [  561] ....................// find -1/4 pos
    

       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  564] ....................// increment ready for next read

       VMRB              vr19, vr25, vr24                                                  // zero1 inputright1 inputleft1 [  566] 
       VMR1W             vr06, vr25, vr24                                                 // right1 inputright1 inputleft1 [  567] 
 // setup loop counter
// 16 rows total, 2 rows per loop
       VMOVW             vr01, 8-1, 64                                                                              // row [  570] 

       VSPIB             vr12, vr19, vr24                                                        // left1 zero1 inputleft1 [  572] ....................// find -1/4 pos  
       VSPIB             vr06, vr19, vr06                                                           // right1 zero1 right1 [  573] ....................// find 1/4 pos   
   
//SubIILoop --------                                                                                                    // [  575] ....................// loop starts here 
 vendrec r3                                                                                                             // [  575] 
 vrec r3                                                                                                                // [  575] 



LINE 579".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  579] 

       VSPIB             vr11, vr12, vr13                                                             // left0 left1 left2 [  581] ....................// (1/4, 1/4) - no longer need to save the 2's
       VSPIB             vr18, vr19, vr20                                                             // zero0 zero1 zero2 [  582] ....................// (1/2, 1/4)
       VSPIB             vr05, vr06, vr07                                                          // right0 right1 right2 [  583] ....................// (3/4, 1/4)

       VLD128            vr27,[i13, 16]                                                              // inputright2 RefPtr [  585] 
       VLD128            vr26,[i13, 0]                                                                // inputleft2 RefPtr [  586] 


       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [  589] 
       VLD128            vr04,[i9, 0]                                                                       // cur1 CirPtr [  590] 

       VIM               i9, i9, PCB_CURRENT_STRIDE                                                       // CirPtr CirPtr [  592] ....................// increment ready for next read

       VMRB              vr20, vr27, vr26                                                  // zero2 inputright2 inputleft2 [  594] 
       VMR1W             vr07, vr27, vr26                                                 // right2 inputright2 inputleft2 [  595] 


       VLD128            vr24,[i13, 0]                                                                // inputleft1 RefPtr [  598] 
       VLD128            vr25,[i13, 16]                                                              // inputright1 RefPtr [  599] 

       VSPIB             vr07, vr20, vr07                                                           // right2 zero2 right2 [  601] ....................// find 1/4 pos
       VSPIB             vr13, vr20, vr26                                                        // left2 zero2 inputleft2 [  602] ....................// find 3/4 pos


       VSADBW.1          vr02, vr03, vr11                                                            // PartRes cur0 left0 [  605] 

       VSPIB             vr21, vr19, vr20                                                             // zero3 zero1 zero2 [  607] ....................// (1/4, 3/4)
       VSPIB             vr08, vr06, vr07                                                          // right3 right1 right2 [  608] ....................// (3/4, 3/4)
       VSPIB             vr15, vr12, vr13                                                             // left3 left1 left2 [  609] ....................// (1/4, 3/4)

       VSADBW.2          vr02, vr03, vr18                                                            // PartRes cur0 zero0 [  611] 
       VSADBW.4          vr02, vr03, vr05                                                           // PartRes cur0 right0 [  612] 
       VSADBW.8          vr02, vr03, vr12                                                            // PartRes cur0 left1 [  613] 
       VSADBW.16         vr02, vr03, vr06                                                           // PartRes cur0 right1 [  614] 
       VSADBW.32         vr02, vr03, vr15                                                            // PartRes cur0 left3 [  615] 
       VSADBW.64         vr02, vr03, vr21                                                            // PartRes cur0 zero3 [  616] 
       VSADBW.128        vr02, vr03, vr08                                                           // PartRes cur0 right3 [  617] 

       VSPIB             vr22, vr20, vr19                                                             // zero4 zero2 zero1 [  619] ....................// (1/4, 3/4)
       VSPIB             vr16, vr13, vr12                                                             // left4 left2 left1 [  620] ....................// (1/4, 3/4)
       VSPIB             vr09, vr07, vr06                                                          // right4 right2 right1 [  621] ....................// (3/4, 3/4)

       VMRB              vr19, vr25, vr24                                                  // zero1 inputright1 inputleft1 [  623] 
       VMR1W             vr06, vr25, vr24                                                 // right1 inputright1 inputleft1 [  624] 

       VADDW.255         vr14, vr14, vr02                                                         // result result PartRes [  626] 


// end of first test line

              //  Stall: [vr06]  ...............................      vr06          vr14    
       VSPIB             vr06, vr19, vr06                                                           // right1 zero1 right1 [  631] ....................// find 1/4 pos
       VSPIB             vr12, vr19, vr24                                                        // left1 zero1 inputleft1 [  632] ....................// find 3/4 pos 


       VSPIB             vr23, vr20, vr19                                                             // zero5 zero2 zero1 [  635] ....................// (1/4, 3/4)
       VSPIB             vr10, vr07, vr06                                                          // right5 right2 right1 [  636] ....................// (3/4, 3/4)
       VSPIB             vr17, vr13, vr12                                                             // left5 left2 left1 [  637] ....................// (1/4, 3/4)

       VSADBW.1          vr02, vr04, vr16                                                            // PartRes cur1 left4 [  639] 
       VSADBW.2          vr02, vr04, vr22                                                            // PartRes cur1 zero4 [  640] 
       VSADBW.4          vr02, vr04, vr09                                                           // PartRes cur1 right4 [  641] 
       VSADBW.8          vr02, vr04, vr13                                                            // PartRes cur1 left2 [  642] 
       VSADBW.16         vr02, vr04, vr07                                                           // PartRes cur1 right2 [  643] 
       VSADBW.32         vr02, vr04, vr17                                                            // PartRes cur1 left5 [  644] 
       VSADBW.64         vr02, vr04, vr23                                                            // PartRes cur1 zero5 [  645] 
       VSADBW.128        vr02, vr04, vr10                                                           // PartRes cur1 right5 [  646] 
// end of second test line


// decrement and loop
       VJD.i14           i14, r3                                                                      // row row SubIILoop [  651] 

// now find the minimum
// since it's difficult to find argmin(vector)
// it might be just as easy to test each of the 8 positions sequentially
// load next row
       VLD128            vr03,[i9, 0]  //DS___________________________________________________________________ cur0 CirPtr [  657] 
       VIM               i9, i9, PCB_CURRENT_STRIDE  //DS___________________________________________________ CirPtr CirPtr [  658] ....................// increment ready for next read
       VADDW.255         vr14, vr14, vr02  //DS_____________________________________________________ result result PartRes [  659] 

    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  662] 
       VNOP                //DS___________________________________________________________________________________________ [  663] 
       VNOP                //DS___________________________________________________________________________________________ [  664] 
       VNOP                //DS___________________________________________________________________________________________ [  665] 
// End                                                                                                                  // [  666] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QuartPelME_int_int                                                                                           // [  668] 




// [Macro] QuarterPelSearch                                                                                             // [  671] 
_Asm int Record_QuarterPelSearch(int *mac_tab)
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
QuarterPelSearch_RecLoop:
  ld r1,[r17 ,24]
  vrec r1



// Begin                                                                                                                // [  674] 
// ****** Informational : Register space for symbol "lastX" already allocated.                                          // [  675] 
// p16    i10  lastX                                                                                                    // [  675] 
// ****** Informational : Register space for symbol "lastY" already allocated.                                          // [  675] 
// p16    i11  lastY                                                                                                    // [  675] 
// p16    i15  switch                                                                                                   // [  676] 
// s16    vr03.4  tempX                                                                                                 // [  677] 
// s16    vr03.8  tempY                                                                                                 // [  678] 
// s16    vr03.32  tempRef                                                                                              // [  679] 
//vec16  vr04  vecadd                                                                                                   // [  680] 

// firstly move reference data if required - only needed if lastX=2 
// if lastY == 2, then we simply record that the RefPtr needs to be offset
// and use that in the next step, without having to reposition
// Begin                                                                                                                // [  685] 
//vec16  vr05  ref0                                                                                                     // [  686] 
//vec16  vr06  ref1                                                                                                     // [  686] 
//vec16  vr07  ref2                                                                                                     // [  686] 
//vec16  vr08  ref3                                                                                                     // [  686] 
// p16    i14  offsetY                                                                                                  // [  687] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        switch|       offsetY|        RefPtr|      StepMult|         lastY|         lastX|        CirPtr|      BestCost|
//|VR 2|                                                     PartRes                                                           |
//|VR 3|              |              |       tempRef|              |         tempY|         tempX|              |              |
//|VR 4|                                                      vecadd                                                           |
//|VR 5|                                                        ref0                                                           |
//|VR 6|                                                        ref1                                                           |
//|VR 7|                                                        ref2                                                           |
//|VR 8|                                                        ref3                                                           |
//|............................................................................................................................|
//|VR14|                                                      result                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

LINE 689".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VIM               i14, i11, -1                                                                     // offsetY lastY [  689] ....................// now -3, -1 or 1
              //  Stall: [vr01]  ...............................      I14  
       VMAXW.64          vr01, vr01, 0                                                                  // offsetY offsetY [  690] ....................// 0, 0, or 1
              //  Stall: [vr01]  ...............................      vr01.064
       VMULW.64          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE                                          // offsetY offsetY [  691] ....................// gives 1 2 or 3 lines

              //  Stall: [i14]  ................................      vr01.064
              //  Stall: [i14]  ................................      vr01.064
       VIM               i13, i14, SDM_ME_REFERENCE_ARRAY                                                // RefPtr offsetY [  693] ....................// source ptr
       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // CirPtr [  694] 
       VIM               i15, i10, -2                                                                      // switch lastX [  695] 


              //  Stall: [i15]  ................................      I15  
       VJP.i15           r3                                                                    // switch EndQPelReposition [  698] 
       VNOP                //DS___________________________________________________________________________________________ [  699] 
       VNOP                //DS___________________________________________________________________________________________ [  700] 
       VNOP                //DS___________________________________________________________________________________________ [  701] 

       VMOVW             vr01, (18/2)-1, 16                                                                    // StepMult [  703] 
//ShiftRightQ1new --------                                                                                              // [  704] 
 vendrec r4                                                                                                             // [  704] 
 vrec r4                                                                                                                // [  704] 


LINE 707".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VLD128            vr05,[i13, 0]                                                                      // ref0 RefPtr [  707] 
       VLD128            vr06,[i13, 16]                                                                     // ref1 RefPtr [  708] 
       VLD128            vr07,[i13, 32]                                                                     // ref2 RefPtr [  709] 
       VLD128            vr08,[i13, 48]                                                                     // ref3 RefPtr [  710] 

       VIM               i13, i13, ME_REFERENCE_ARRAY_STRIDE*2                                            // RefPtr RefPtr [  712] 
       VMRB              vr05, vr06, vr05                                                                // ref0 ref1 ref0 [  713] ....................// shift by 1 pixels
       VSR8              vr06, vr06, 1                                                                        // ref1 ref1 [  714] ....................// shift by 1 pixels
       VMRB              vr07, vr08, vr07                                                                // ref2 ref3 ref2 [  715] ....................// shift by 1 pixels
       VSR8              vr08, vr08, 1                                                                        // ref3 ref3 [  716] ....................// shift by 1 pixels

       VST128            vr05,[i9, 0]                                                                       // ref0 CirPtr [  718] 
       VST128            vr06,[i9, 16]                                                                      // ref1 CirPtr [  719] 

       VJD.i12           i12, r4                                                      // StepMult StepMult ShiftRightQ1new [  721] 
       VST128            vr07,[i9, 32]  //DS__________________________________________________________________ ref2 CirPtr [  722] 
       VST128            vr08,[i9, 48]  //DS__________________________________________________________________ ref3 CirPtr [  723] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ CirPtr CirPtr [  724] 


       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 32                                                        // RefPtr [  727] ....................// reset ready for qpel functions

//EndQPelReposition --------                                                                                            // [  729] 
 vendrec r3                                                                                                             // [  729] 
 vrec r3                                                                                                                // [  729] 


// End                                                                                                                  // [  732] 
LINE 733".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
       VMOVW             vr01, 1, 16                                                                           // StepMult [  733] ....................// multiplier for RD costs 

  ld r5,[r17,4];                                                                                      // VSTACK initRDvals [  735] 
       VJL               vr31, r5                                                                                       // [  735] 

  ld r1,[r17,20];  //DS_________________________________________________________________________ vecadd QuartPelME_int_int [  737] 
       VMOVW             vr04, r1, 1  //DS________________________________________________________________________________ [  737] 
  ld r1,[r17,12];  //DS________________________________________________________________________ vecadd QuartPelME_half_int [  738] 
       VMOVW             vr04, r1, 2  //DS________________________________________________________________________________ [  738] 
  ld r1,[r17,16];  //DS________________________________________________________________________ vecadd QuartPelME_int_half [  739] 
       VMOVW             vr04, r1, 4  //DS________________________________________________________________________________ [  739] 
  ld r1,[r17,8];                                                                            // vecadd QuartPelME_half_half [  740] 
       VMOVW             vr04, r1, 8                                                                                    // [  740] 



// last Y was 2 then need to start a line lower

// now need to find out which function to call for qpel
// it depends on whether each component is integer or half pixel so far
       VABSW.12          vr01, vr00                                                               // lastX'lastY VecX'VecY [  748] 
              //  Stall: [vr01]  ...............................      vr01.012
       VAND.12           vr01, vr01, 2                                                          // lastX'lastY lastX'lastY [  749] ....................// 2nd bit set indicates 1/2 pel chosen
              //  Stall: [vr01]  ...............................      vr01.012
       VASRW.4           vr01, vr01, 1                                                                      // lastX lastX [  750] ....................// shift down X to 1 or 0
              //  Stall: [vr01]  ...............................      vr01.004
        VMR1W.4         vr03, vr01, vr01                                                                    // tempX lastY [  751] 
LINE 752".\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss"
              //  Stall: [vr03]  ...............................      vr03.004
              //  Stall: [vr03]  ...............................      vr03.004
       VADDW.4           vr01, vr01, vr03                                                             // lastX lastX tempX [  752] ....................// add Y to X
//move16   switch, lastX
//lastX now has value of 0, to 3 depending on which half pel vals are used
// now use lastX to decide which subroutine to use.


              //  Stall: [i10]  ................................      vr01.004
       VJLT              vr31, vr04, i10                                                            // VSTACK vecadd lastX [  758] ....................// jump to the right bit of code
       VNOP                //DS___________________________________________________________________________________________ [  759] 
       VNOP                //DS___________________________________________________________________________________________ [  760] 
       VNOP                //DS___________________________________________________________________________________________ [  761] 
  ld r6,[r17,0];                                                                                       // VSTACK endofIter [  762] 
       VJL               vr31, r6                                                                                       // [  762] 

       VNOP                //DS___________________________________________________________________________________________ [  764] 
       VNOP                //DS___________________________________________________________________________________________ [  765] 
       VNOP                //DS___________________________________________________________________________________________ [  766] 

// End                                                                                                                  // [  768] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz QuarterPelSearch_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc QuarterPelSearch                                                                                             // [  769] 




const int SIMD_IDX_QuartPelME_half_half = 2;
const int SIMD_IDX_QuartPelME_half_int = 3;
const int SIMD_IDX_QuartPelME_int_half = 4;
const int SIMD_IDX_QuartPelME_int_int = 5;
const int MACRO_IDX_QuarterPelSearch = 6;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Software_QPelME_Record(int sa, int SIMDnum)
{
    int *map = Software_QPelME;

    SetSCQ(SIMDnum);

    map[0] = Software_ME[1];

    map[1] = Software_ME[2];

    map[SIMD_IDX_QuartPelME_half_half] = sa;
    sa = Record_QuartPelME_half_half(map);

    map[SIMD_IDX_QuartPelME_half_int] = sa;
    sa = Record_QuartPelME_half_int(map);

    map[SIMD_IDX_QuartPelME_int_half] = sa;
    sa = Record_QuartPelME_int_half(map);

    map[SIMD_IDX_QuartPelME_int_int] = sa;
    sa = Record_QuartPelME_int_int(map);

    map[MACRO_IDX_QuarterPelSearch] = sa;
    sa = Record_QuarterPelSearch(map);

    return sa;
}
