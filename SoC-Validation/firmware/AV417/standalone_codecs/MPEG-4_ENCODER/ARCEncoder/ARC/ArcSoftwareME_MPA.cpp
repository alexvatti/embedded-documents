//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareME_MPA.sss'

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
#include ".\ARCEncoder\ARC\ArcSoftwareME_MPA.h"


// Macro table
    int Software_ME[10];
    int Software_ME_Record(int sa, int SIMDnum);

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


//----------------------------------------------------------------------------------------------------
// Is Macro
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft


//global scope needed as these vals needed for subroutine

//vec16  vr02  cur0                                                                                                     // [   46] ....................// vectors for holding the current data
//vec16  vr03  cur1                                                                                                     // [   46] 
//vec16  vr04  cur2                                                                                                     // [   46] 
//vec16  vr05  cur3                                                                                                     // [   46] 
//vec16  vr06  cur4                                                                                                     // [   47] 
//vec16  vr07  cur5                                                                                                     // [   47] 
//vec16  vr08  cur6                                                                                                     // [   47] 
//vec16  vr09  cur7                                                                                                     // [   47] 
//vec16  vr10  cur8                                                                                                     // [   48] 
//vec16  vr11  cur9                                                                                                     // [   48] 
//vec16  vr12  cur10                                                                                                    // [   48] 
//vec16  vr13  cur11                                                                                                    // [   48] 
//vec16 cur12, cur13, cur14, cur15
//vec16  vr14  result                                                                                                   // [   50] 
// p16    i0  PredX                                                                                                     // [   51] 
// p16    i1  PredY                                                                                                     // [   52] 
// p16    i2  VecX                                                                                                      // [   53] 
// p16    i3  VecY                                                                                                      // [   53] 
// p16    i10  lastX                                                                                                    // [   54] ....................// same lanes as VecX and VecY
// p16    i11  lastY                                                                                                    // [   54] 
// ****** Informational : Register space for symbol "PatternMatch" already allocated.                                   // [   55] 
// p16    i6  PatternMatch                                                                                              // [   55] 
// p16    i7  lambda                                                                                                    // [   56] 
// p16    i8  BestCost                                                                                                  // [   57] 
// p16    i12  StepMult                                                                                                 // [   58] 
// p16    i9  RefPtr                                                                                                    // [   59] 

// [Macro] Generate_Soft_ME_Response                                                                                    // [   61] 
_Asm int Record_Generate_Soft_ME_Response(int *mac_tab)
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

// p16    i14  IntraValid                                                                                               // [   62] 
// p16    i15  SkipVec                                                                                                  // [   64] 

// Begin                                                                                                                // [   68] 



// first get some params
LINE 73".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr01, SDMOF_IntraPredictionResidsLuma, 2                                                // RefPtr [   73] 

              //  Stall: [i9]  .................................      vr01.002
       VLD16_6           vr01,[i9, 0]                                                                 // IntraValid RefPtr [   75] 
       VLD16_7           vr01,[i9, 2]                                                                    // SkipVec RefPtr [   76] 

   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
    // send channel commands
              //  Stall: [i14]  ................................      vr01.192
              //  Stall: [i14]  ................................      vr01.192
       VSEND             r0, i14, 0                                                                                     // [   81] 
       VSEND             r0, i15, 1                                                                                     // [   82] 

   mov                 r1, Service_ME_SDMResponse
       VSENDR            r0, r1, 63                                                                                     // [   85] ....................// channel number not used at present

       VSEND             r0, i8, 0                                                                             // BestCost [   87] 
   mov                 r1, Service_ME_InterResponse
       VSENDR            r0, r1, 63                                                                                     // [   89] ....................// return to caller
   // vjb                 VSTACK, 0
   // ~vnop
   // ~vnop
   // ~vnop
    
// End                                                                                                                  // [   95] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc Generate_Soft_ME_Response                                                                                    // [   96] 




// function endofIter
// this is called at the end of an integer step 
// and it assumes that the relative zero displacement for the step
// is set in Best Cost, whilst the other 8 position costs are in res0 - res8
// StepMult is set to step displacement value for the previous step (4, 2, 1)
// lambda is set up with the Rate Distortion lambda for the current quant
// VecX and VecY represents the current origin and will get updated by the new displacement
// lastx, lastY will contain the relative displacement of the last step - needed to realign the
// reference area



// [Subroutine] endofIter                                                                                               // [  111] 
_Asm int Record_endofIter(int *mac_tab)
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

// Begin                                                                                                                // [  112] ....................//2
// p16    i15  BestIndex                                                                                                // [  113] 
// p16    i13  newmin                                                                                                   // [  114] 
//vec16  vr15  horiz                                                                                                    // [  115] 
//vec16  vr16  vert                                                                                                     // [  115] 
//vec16  vr17  CostVec                                                                                                  // [  116] 
//vec16  vr18  temp                                                                                                     // [  117] 
//vec16  vr19  mask                                                                                                     // [  117] 
//vec16  vr20  resinvert                                                                                                // [  118] 
//vec16  vr21  topbit                                                                                                   // [  119] 
// p16    i14  row                                                                                                      // [  120] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|     BestIndex|           row|        newmin|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       horiz                                                           |
//|VR16|                                                        vert                                                           |
//|VR17|                                                     CostVec                                                           |
//|VR18|                                                        temp                                                           |
//|VR19|                                                        mask                                                           |
//|VR20|                                                   resinvert                                                           |
//|VR21|                                                      topbit                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

// find best position
// we have a problem that most of the compare functions are signed
// whereas our data is unsigned
// get around this 
LINE 126".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr21, 0x8000, 255                                                                       // topbit [  126] 
       VXMINW.255        vr17, vr01, 1                                                                 // CostVec BestCost [  127] 

// it's difficult to find argmin(vector)

       VMOVW             vr01, 0, 140                                                             // BestIndex'lastX'lastY [  131] ....................// in case (0,0) is best - these needt to be set for repositioning
       VXOR              vr20, vr14, vr21                                                       // resinvert result topbit [  132] ....................// change to signed

       VXOR              vr17, vr17, vr21                                                        // CostVec CostVec topbit [  134] ....................// change to signed for comparison
    
       VNEW.255.F        vr62, vr62, vr62                                                             // VZERO VZERO VZERO [  136] ....................// clear flags
   
       VLTW.i6.F         vr19, vr20, vr17                                           // PatternMatch mask resinvert CostVec [  138] ....................// check to see if any are less than current best
       VMOVW!i6          vr19, 0                                                                      // PatternMatch mask [  139] ....................// clear the ones we aren't interested in
     
       VCLRSTK.255                                                                                                      // [  141] ....................// clear sticky flags   // as above
       VGMW              i13, vr19                                                                          // newmin mask [  142] ....................// create bit mask for lanes that are less than best
    
       VMVW              vr19, vr62, 255                                                                     // mask VZERO [  144] ....................// may not need this, if no new min, but here to use stalls
 
       VMOVW             vr01, SDM_LANE_LOOKUP, 2                                                                // RefPtr [  146] 
       VJB!i13           vr31, 0                                                                          // newmin VSTACK [  147] ....................// if so, we can return, nothing left to do BestCost is left alone
       
       VXMINW.255        vr17, vr20, i6  //DS______________________________________________ CostVec resinvert PatternMatch [  149] ....................// put min out of those tested in every lane of CostVec
       VLD128            vr18,[i9, 0]  //DS___________________________________________________________________ temp RefPtr [  150] ....................// read lane look up vector
       VMOVW             vr01, SDM_ME_OFFSET_TABLE, 2  //DS________________________________________________________ RefPtr [  151] 

       VSEQW.i6          vr20, vr17                                                      // PatternMatch resinvert CostVec [  153] ....................// get bit pattern of mins - but may have more than 1
       VMVW.255.S        vr19, vr18                                                                           // mask temp [  154] ....................// mask will contain the lane bits for the minima                                   
   
  
       VLD128            vr15,[i9, 0]                                                                      // horiz RefPtr [  157] ....................// load offsets
       VLD128            vr16,[i9, 16]                                                                      // vert RefPtr [  158] 
   
       VXOR              vr17, vr17, vr21                                                        // CostVec CostVec topbit [  160] ....................// put value back to unsigned
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 161".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VXMAXW.128        vr01, vr19, 255                                                                 // BestIndex mask [  161] ....................// chose the highest lane with the bigest val



 
// initialise the 9 sums


       VMVW              vr01, vr17, 1                                                                 // BestCost CostVec [  169] ....................// put into scalar to save vecs   
    
    
       VMVW              vr01, vr62, 12                                                               // lastX'lastY VZERO [  172] ....................// set lastX and lastY to zero - already done ?

       VMULW             vr15, vr15, i12                                                           // horiz horiz StepMult [  174] 
       VMULW             vr16, vr16, i12                                                             // vert vert StepMult [  175] 

    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  178] 
       VIV.i15           i10, vr15  //DS____________________________________________________________ BestIndex lastX horiz [  179] ....................// get best horiz component
       VIV.i15           i11, vr16  //DS_____________________________________________________________ BestIndex lastY vert [  180] ....................// get best horiz component
              //  Stall: [vr01]  ...............................      I11  
       VADDW.12          vr00, vr00, vr01  //DS___________________________________________ VecX'VecY VecX'VecY lastX'lastY [  181] ....................// // add to previous best vector


// End                                                                                                                  // [  184] 
  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc endofIter                                                                                                    // [  185] 


// subroutine to set up initial Rate Distortion costs for each of the 8 running sums
// [Subroutine] initRDvals                                                                                              // [  187] 
_Asm int Record_initRDvals(int *mac_tab)
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


// Begin                                                                                                                // [  189] 
//vec16  vr15  horiz                                                                                                    // [  190] 
//vec16  vr16  vert                                                                                                     // [  190] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |              |              |      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                       horiz                                                           |
//|VR16|                                                        vert                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


LINE 193".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr01, SDM_ME_OFFSET_TABLE, 2                                                            // RefPtr [  193] 
              //  Stall: [i9]  .................................      vr01.002
       VLD128            vr15,[i9, 0]                                                                      // horiz RefPtr [  194] 
       VLD128            vr16,[i9, 16]                                                                      // vert RefPtr [  195] 
// now start to set up Rate Distortion for next stage
              //  Stall: [vr15]  ...............................      vr15          vr16    
              //  Stall: [vr15]  ...............................      vr15          vr16    
       VMULW             vr15, vr15, i12                                                           // horiz horiz StepMult [  197] ....................// multiply by step multiplication
       VMULW             vr16, vr16, i12                                                             // vert vert StepMult [  198] ....................// multiply by step multiplication

              //  Stall: [vr15]  ...............................      vr15          vr16    
       VADDW             vr15, vr15, i2                                                                // horiz horiz VecX [  200] ....................// add the current vector component (centre of next step)
       VADDW             vr16, vr16, i3                                                                  // vert vert VecY [  201] ....................// add the current vector component

       VSUBW             vr15, vr15, i0                                                               // horiz horiz PredX [  203] ....................// subtract predicted vec component  
       VSUBW             vr16, vr16, i1                                                                 // vert vert PredY [  204] ....................// subtract predicted vec component

       VABSW             vr15, vr15                                                                         // horiz horiz [  206] ....................// get magnitude of MVDx
       VABSW             vr16, vr16                                                                           // vert vert [  207] ....................// get magnitude of MVDy

       VNORMW            vr15, vr15                                                                         // horiz horiz [  209] ....................// this is effectively 16-log(base2) of the numbers
       VNORMW            vr16, vr16                                                                           // vert vert [  210] ....................// this is effectively 16-log(base2) of the numbers
              //  Stall: [vr16]  ...............................      vr16    
       VADDW             vr14, vr16, vr15                                                             // result vert horiz [  211] ....................// add both components together
// result is now 32 - (log2(MVDx) + log2(MVDy))
// actual bits are given by 2*log(base2) + 1 summed for x and y
    // return to caller
       VJB               vr31, 0                                                                                 // VSTACK [  215] 
       VADDW             vr14, vr14, vr14  //DS______________________________________________________ result result result [  216] ....................// double it to get 64 - 2*(log2(MVDx) + log2(MVDy))
              //  Stall: [vr14]  ...............................      vr14    
       VRSUBW            vr14, vr14, 62  //DS_______________________________________________________________ result result [  217] ....................// subtract from 64 and add 2 to get 2*(log2(MVDx) + log2(MVDy)) + 2
              //  Stall: [vr14]  ...............................      vr14    
       VMULW             vr14, vr14, i7  //DS________________________________________________________ result result lambda [  218] ....................// multiply by current lambda



// End                                                                                                                  // [  222] ....................//1

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc initRDvals                                                                                                   // [  224] 






// function IntegerME - does a 3step search for a 16x16 Inter MB in the reference array
// which is assumed to be 32x32 pixels arranged +/-8 around the current MB
// the first step has 9 points
// the second and third steps have 8 points each (missing out middle point 4)
// PredX and PredY contain the predicted motion vector in 1/4 pel units
// By the end of the function, VecX, VecY should have the best vector found, BestCost should have the best
// RD cost and SDM_ME_REFERENCE_ARRAY should have the best integer location at offset 1 from the top and the right
// edge of the 32*32 array.  This should make it easier for sub-pixel searches to be done
// [Macro] IntegerME                                                                                                    // [  237] 
_Asm int Record_IntegerME(int *mac_tab)
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
IntegerME_RecLoop:
  ld r1,[r17 ,12]
  vrec r1





// Begin                                                                                                                // [  242] ....................//*** of function
// Begin                                                                                                                // [  243] 
// p16    i15  skipVec                                                                                                  // [  244] 
// p16    i14  IntraValid                                                                                               // [  245] 


// first park some params
LINE 249".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr01, SDMOF_IntraPredictionResidsLuma, 2                                                // RefPtr [  249] 
//vmovw  'VecX'VecY, 0  // set vecs to zero

              //  Stall: [i9]  .................................      vr01.002
       VST16_6           vr01,[i9, 0]                                                                 // IntraValid RefPtr [  252] 
       VST16_7           vr01,[i9, 2]                                                                    // skipVec RefPtr [  253] 
// End                                                                                                                  // [  254] 
// Begin                                                                                                                // [  255] ....................//*** of scope of res4
// p16    i13  res4                                                                                                     // [  256] ....................// only needed in first step
// Begin                                                                                                                // [  257] ....................//*** of RD initialisation for first step
// initialise the 9 sums to the rate distortion approximation function
// set each lane equal to this value
// this sets the vectors res0 to res8 with the RD cost for each one's MV
//vec16  vr20  temp                                                                                                     // [  261] ....................// to make sure it doesn't conflict with anything in initRDvals
//vec16  vr15  tempres.3                                                                                                // [  262] 
//vec16  vr15  tempres_Ptrx.1                                                                                           // [  262] 
//vec16  vr15  tempres_Ptry.2                                                                                           // [  262] 


// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |              |          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |              |              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|............................................................................................................................|
//|VR20|                                                        temp                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

// read quantiser


//vmovw 'StepMult, 16 or 8// steps of 4 pels in 1/4 pel units this set by calling arc code
  ld r1,[r17,8];                                                                                      // VSTACK initRDvals [  270] 
LINE 270".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJL               vr31, r1                                                                                       // [  270] 
// now do res4 which couldn't fit in the vector
// it is (0,0)
// ****** Warning : Unmarked delay slot found at line number 273 *****
LINE 273".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [  273] 
// ****** Warning : Unmarked delay slot found at line number 274 *****
LINE 274".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [  274] 
// ****** Warning : Unmarked delay slot found at line number 275 *****
LINE 275".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [  275] 
// End                                                                                                                  // [  287] ....................//*** of initialisation of RD for first step
// Begin                                                                                                                // [  288] ....................//*** of first integer step
//vec16  vr15  ref0                                                                                                     // [  289] 
//vec16  vr16  ref1                                                                                                     // [  289] 
//vec16  vr17  ref2                                                                                                     // [  289] 
//vec16  vr18  ref3                                                                                                     // [  289] 
//vec16  vr19  ref4                                                                                                     // [  290] 
//vec16  vr20  ref5                                                                                                     // [  290] 
//vec16  vr21  ref6                                                                                                     // [  290] 
//vec16  vr22  ref7                                                                                                     // [  290] 
//vec16  vr23  temp0                                                                                                    // [  291] 
//vec16  vr24  temp1                                                                                                    // [  292] 
//vec16  vr25  temp2                                                                                                    // [  293] 
//vec16  vr26  temp3                                                                                                    // [  294] 
//vec16  vr27  PartRes                                                                                                  // [  295] 
// s16    vr28.32  Partres4                                                                                             // [  296] ....................// this will hold the (0,0) displacement SAD cost

// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |              |          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                        ref0                                                           |
//|VR16|                                                        ref1                                                           |
//|VR17|                                                        ref2                                                           |
//|VR18|                                                        ref3                                                           |
//|VR19|                                                        ref4                                                           |
//|VR20|                                                        ref5                                                           |
//|VR21|                                                        ref6                                                           |
//|VR22|                                                        ref7                                                           |
//|VR23|                                                       temp0                                                           |
//|VR24|                                                       temp1                                                           |
//|VR25|                                                       temp2                                                           |
//|VR26|                                                       temp3                                                           |
//|VR27|                                                     PartRes                                                           |
//|VR28|              |              |      Partres4|              |              |              |              |              |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

    // setup loop counter 
    // 16 rows total, 4 rows per loop
 
// move reference pointer to point to (4,-4)
//vim RefPtr, RefPtr, ME_REFERENCE_ARRAY_STRIDE*3+ME_REFERENCE_ARRAY_STRIDE-23
LINE 304".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VDMAWAIT          0x0, 0x7f                                                                                      // [  304] 
//  The 9 points in the first step are done in this pattern :       
//                                                                  
//      res2 (-4,-4)   res1 (0,-4)    res0 (4,-4)                   
//      res5 (-4, 0)   res4 (0, 0)    res3 (4, 0)                   
//      res8 (-4, 4)   res7 (0, 4)    res6 (4, 4)                   
//                                   
       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [  311] 
// load 4rows of 32 pels from reference                               
// load 16 rows of current data - i.e. entire luma of MB
       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y]                                      // cur0 CirBuf [  314] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE]                   // cur1 CirBuf [  315] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*2]                 // cur2 CirBuf [  316] 
       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*3]                 // cur3 CirBuf [  317] 
       VLD128            vr06,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*4]                 // cur4 CirBuf [  318] 
       VLD128            vr07,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*5]                 // cur5 CirBuf [  319] 
       VLD128            vr08,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*6]                 // cur6 CirBuf [  320] 
       VLD128            vr09,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*7]                 // cur7 CirBuf [  321] 
       VLD128            vr10,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*8]                 // cur8 CirBuf [  322] 
       VLD128            vr11,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*9]                 // cur9 CirBuf [  323] 
       VLD128            vr12,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*10]               // cur10 CirBuf [  324] 
       VLD128            vr13,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*11]               // cur11 CirBuf [  325] 

       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*4+16]                                          // ref4 RefPtr [  327] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*4]                                             // ref0 RefPtr [  328] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*5+16]                                          // ref5 RefPtr [  329] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*5]                                             // ref1 RefPtr [  330] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*6+16]                                          // ref6 RefPtr [  331] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*6]                                             // ref2 RefPtr [  332] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*7+16]                                          // ref7 RefPtr [  333] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*7]                                             // ref3 RefPtr [  334] 
// accumulate sad for (4, -4) first 4 rows
       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  336] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  337] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  338] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  339] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  340] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  341] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  342] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  343] 

// accumulate sad for (0, -4) first 4 rows
       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  346] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  347] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  348] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  349] 
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  350] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  351] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  352] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  353] 
// accumulate sad for (-4, -4) first 4 rows
       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  355] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  356] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  357] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  358] 
       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  359] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  360] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  361] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  362] 


// load in next 4 rows of 32 pels of reference
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*8+16]                                          // ref4 RefPtr [  366] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*8]                                             // ref0 RefPtr [  367] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*9+16]                                          // ref5 RefPtr [  368] 

       VADDW.7           vr14, vr14, vr27                                                         // result result PartRes [  370] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*9]                                             // ref1 RefPtr [  371] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*10+16]                                         // ref6 RefPtr [  372] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*10]                                            // ref2 RefPtr [  373] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*11+16]                                         // ref7 RefPtr [  374] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*11]                                            // ref3 RefPtr [  375] 

// accumulate sad for (4, 0) first 4 rows
// accumulate sad for (4,-4) second 4 rows
       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  379] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  380] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  381] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  382] 
       VSADBW.8          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  383] 
       VSADBAW.8         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  384] 
       VSADBAW.8         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  385] 
       VSADBAW.8         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  386] 
       VSADBW.1          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  387] 
       VSADBAW.1         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  388] 
       VSADBAW.1         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  389] 
       VSADBAW.1         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  390] 


       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  393] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  394] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  395] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  396] 

//vsadbw 'Partres4, cur0, temp0
//vsadbaw 'Partres4, cur1, temp1
//vsadbaw 'Partres4, cur2, temp2
//vsadbaw 'Partres4, cur3, temp3
       VSADBW.2          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  402] 
       VSADBAW.2         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  403] 
       VSADBAW.2         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  404] 
       VSADBAW.2         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  405] 

       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  407] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  408] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  409] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  410] 

       VSADBW.16         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  412] 
       VSADBAW.16        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  413] 
       VSADBAW.16        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  414] 
       VSADBAW.16        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  415] 
       VSADBW.4          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  416] 
       VSADBAW.4         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  417] 
       VSADBAW.4         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  418] 
       VSADBAW.4         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  419] 

// accumulate sad for (4, 4) first 4 rows
// accumulate sad for (0, 4) second 4 rows
// accumulate sad for (-4, 4) third 4 rows
// accumulate sad for (0, 4) first 4 rows
// accumulate sad for (0, 0) second 4 rows
// accumulate sad for (0, -4) third 4 rows
// accumulate sad for (-4, 4) first 4 rows
// accumulate sad for (-4, 0) second 4 rows
// accumulate sad for (-4, -4) third 4 rows
// load next 4 rows of 32 pels
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*12+16]                                         // ref4 RefPtr [  431] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*12]                                            // ref0 RefPtr [  432] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*13+16]                                         // ref5 RefPtr [  433] 

       VADDW.31          vr14, vr14, vr27                                                         // result result PartRes [  435] 
//vaddw 'res4, 'res4, 'Partres4

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*13]                                            // ref1 RefPtr [  438] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*14+16]                                         // ref6 RefPtr [  439] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*14]                                            // ref2 RefPtr [  440] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*15+16]                                         // ref7 RefPtr [  441] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*15]                                            // ref3 RefPtr [  442] 


       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  445] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  446] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  447] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  448] 
       VSADBW.32         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  449] 
       VSADBAW.32        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  450] 
       VSADBAW.32        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  451] 
       VSADBAW.32        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  452] 

       VSADBW.8          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  454] 
       VSADBAW.8         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  455] 
       VSADBAW.8         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  456] 
       VSADBAW.8         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  457] 

       VSADBW.1          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  459] 
       VSADBAW.1         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  460] 
       VSADBAW.1         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  461] 
       VSADBAW.1         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  462] 

       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  464] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  465] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  466] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  467] 

       VSADBW.64         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  469] 
       VSADBAW.64        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  470] 
       VSADBAW.64        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  471] 
       VSADBAW.64        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  472] 
//vsadbw 'Partres4, cur4, temp0
//vsadbaw 'Partres4, cur5, temp1
//vsadbaw 'Partres4, cur6, temp2
//vsadbaw 'Partres4, cur7, temp3
       VSADBW.2          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  477] 
       VSADBAW.2         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  478] 
       VSADBAW.2         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  479] 
       VSADBAW.2         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  480] 

       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  482] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  483] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  484] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  485] 

       VSADBW.128        vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  487] 
       VSADBAW.128       vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  488] 
       VSADBAW.128       vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  489] 
       VSADBAW.128       vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  490] 
       VSADBW.16         vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  491] 
       VSADBAW.16        vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  492] 
       VSADBAW.16        vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  493] 
       VSADBAW.16        vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  494] 
       VSADBW.4          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  495] 
       VSADBAW.4         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  496] 
       VSADBAW.4         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  497] 
       VSADBAW.4         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  498] 

// accumulate sad for (4, 4) second 4 rows
// accumulate sad for (4, 0) third 4 rows
// accumulate sad for (4, -4) last 4 rows
// accumulate sad for (0, 4) second 4 rows
// accumulate sad for (0, 0)  third 4 rows
// accumulate sad for (0, -4) last 4 rows
// accumulate sad for (-4, 4) second 4 rows
// accumulate sad for (-4, 0) third 4 rows
// accumulate sad for (-4, -4) last 4 rows
       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*12]                // cur0 CirBuf [  509] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*13]                // cur1 CirBuf [  510] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*14]                // cur2 CirBuf [  511] 

       VADDW.255         vr14, vr14, vr27                                                         // result result PartRes [  513] 
//vaddw 'res4, 'res4, 'Partres4

       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*15]                // cur3 CirBuf [  516] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*16+16]                                         // ref4 RefPtr [  517] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*16]                                            // ref0 RefPtr [  518] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*17+16]                                         // ref5 RefPtr [  519] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*17]                                            // ref1 RefPtr [  520] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*18+16]                                         // ref6 RefPtr [  521] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*18]                                            // ref2 RefPtr [  522] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*19+16]                                         // ref7 RefPtr [  523] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*19]                                            // ref3 RefPtr [  524] 

       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  526] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  527] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  528] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  529] 
       VSADBW.32         vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  530] 
       VSADBAW.32        vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  531] 
       VSADBAW.32        vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  532] 
       VSADBAW.32        vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  533] 
       VSADBW.8          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  534] 
       VSADBAW.8         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  535] 
       VSADBAW.8         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  536] 
       VSADBAW.8         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  537] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  538] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  539] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  540] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  541] 

       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  543] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  544] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  545] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  546] 
       VSADBW.64         vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  547] 
       VSADBAW.64        vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  548] 
       VSADBAW.64        vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  549] 
       VSADBAW.64        vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  550] 
//vsadbw 'Partres4, cur8, temp0
//vsadbaw 'Partres4, cur9, temp1
//vsadbaw 'Partres4, cur10, temp2
//vsadbaw 'Partres4, cur11, temp3
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  555] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  556] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  557] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  558] 

       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  560] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  561] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  562] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  563] 
       VSADBW.128        vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  564] 
       VSADBAW.128       vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  565] 
       VSADBAW.128       vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  566] 
       VSADBAW.128       vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  567] 
       VSADBW.16         vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  568] 
       VSADBAW.16        vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  569] 
       VSADBAW.16        vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  570] 
       VSADBAW.16        vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  571] 
       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  572] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  573] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  574] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  575] 

// accumulate sad for (4, 4) third 4 rows
// accumulate sad for (4, 0) last 4 rows
// accumulate sad for (0, 4) third 4 rows
// accumulate sad for (0, 0) last 4 rows
// accumulate sad for (-4, 4) third 4 rows
// accumulate sad for (-4, 0) last 4 rows
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*20+16]                                         // ref4 RefPtr [  583] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*20]                                            // ref0 RefPtr [  584] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*21+16]                                         // ref5 RefPtr [  585] 

       VADDW.255         vr14, vr14, vr27                                                         // result result PartRes [  587] 
//vaddw 'res4, 'res4, 'Partres4

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*21]                                            // ref1 RefPtr [  590] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*22+16]                                         // ref6 RefPtr [  591] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*22]                                            // ref2 RefPtr [  592] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*23+16]                                         // ref7 RefPtr [  593] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*23]                                            // ref3 RefPtr [  594] 


       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  597] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  598] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  599] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  600] 
       VSADBW.32         vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  601] 
       VSADBAW.32        vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  602] 
       VSADBAW.32        vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  603] 
       VSADBAW.32        vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  604] 

       VSADBW.8          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  606] 
       VSADBAW.8         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  607] 
       VSADBAW.8         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  608] 
       VSADBAW.8         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  609] 

       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  611] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  612] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  613] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  614] 
       VSADBW.64         vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  615] 
       VSADBAW.64        vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  616] 
       VSADBAW.64        vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  617] 
       VSADBAW.64        vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  618] 
//vsadbw 'Partres4, cur0, temp0
//vsadbaw 'Partres4, cur1, temp1
//vsadbaw 'Partres4, cur2, temp2
//vsadbaw 'Partres4, cur3, temp3

       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  624] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  625] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  626] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  627] 
       VSADBW.128        vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  628] 
       VSADBAW.128       vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  629] 
       VSADBAW.128       vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  630] 
       VSADBAW.128       vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  631] 
       VSADBW.16         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  632] 
       VSADBAW.16        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  633] 
       VSADBAW.16        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  634] 
       VSADBAW.16        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  635] 

// accumulate sad for (4, 4) last 4 rows
// accumulate sad for (4, 0) last 4 rows
// accumulate sad for (4, 4) last 4 rows
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*24+16]                                         // ref4 RefPtr [  640] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*24]                                            // ref0 RefPtr [  641] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*25+16]                                         // ref5 RefPtr [  642] 

       VADDW.248         vr14, vr14, vr27                                                         // result result PartRes [  644] ....................// not top 3 sums
//vaddw 'res4, 'res4, 'Partres4

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*25]                                            // ref1 RefPtr [  647] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*26+16]                                         // ref6 RefPtr [  648] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*26]                                            // ref2 RefPtr [  649] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*27+16]                                         // ref7 RefPtr [  650] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*27]                                            // ref3 RefPtr [  651] 

       VMR2W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  653] 
       VMR2W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  654] 
       VMR2W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  655] 
       VMR2W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  656] 
       VSADBW.32         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  657] 
       VSADBAW.32        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  658] 
       VSADBAW.32        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  659] 
       VSADBAW.32        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  660] 

       VMR4W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  662] 
       VMR4W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  663] 
       VMR4W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  664] 
       VMR4W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  665] 
       VSADBW.64         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  666] 
       VSADBAW.64        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  667] 
       VSADBAW.64        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  668] 
       VSADBAW.64        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  669] 
       VMR6W             vr23, vr19, vr15                                                               // temp0 ref4 ref0 [  670] 
       VMR6W             vr24, vr20, vr16                                                               // temp1 ref5 ref1 [  671] 
       VMR6W             vr25, vr21, vr17                                                               // temp2 ref6 ref2 [  672] 
       VMR6W             vr26, vr22, vr18                                                               // temp3 ref7 ref3 [  673] 
       VSADBW.128        vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  674] 
       VSADBAW.128       vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  675] 
       VSADBAW.128       vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  676] 
       VSADBAW.128       vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  677] 

//move16 BestCost, res4 //- this now done in stepOne, but should be the same

              //  Stall: [vr27]  ...............................      vr27.128
              //  Stall: [vr27]  ...............................      vr27.128
              //  Stall: [vr27]  ...............................      vr27.128
       VADDW.224         vr14, vr14, vr27                                                         // result result PartRes [  681] 


// End                                                                                                                  // [  684] ....................//*** of first integer step
// End                                                                                                                  // [  685] ....................//*** end of scope of res4
  ld r1,[r17,4];                                                                                       // VSTACK endofIter [  686] 
LINE 686".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJL               vr31, r1                                                                                       // [  686] 

       VNOP                //DS___________________________________________________________________________________________ [  688] 
       VNOP                //DS___________________________________________________________________________________________ [  689] 
       VNOP                //DS___________________________________________________________________________________________ [  690] 

// Begin                                                                                                                // [  692] ....................//*** of repositioning of reference data
// we need to shift the reference array so that the data is placed in the top right of the reference array

// this depends on the VecX and VecY
// p16    i13  offset                                                                                                   // [  696] 
// p16    i14  NewPtr                                                                                                   // [  697] 
// p16    i15  counter                                                                                                  // [  697] 
//vec16  vr15  ref0                                                                                                     // [  698] 
//vec16  vr16  ref1                                                                                                     // [  698] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|       counter|        NewPtr|        offset|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                        ref0                                                           |
//|VR16|                                                        ref1                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


// start at (4+lastY)*ME_REFERENCE_ARRAY_STRIDE  SDM_ME_REFERENCE_ARRAY
#ifdef MPEG4BUILD
LINE 703".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VIM               i13, i11, 8                                                                       // offset lastY [  703] ....................// contains 0, 8, or 16
       VMOVW             vr01, 24-1, 128                                                                        // counter [  704] ....................// we need 16 + 4 + 4 rows in ref
       VMULW.32          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/2                                          // offset offset [  705] 
#else 
       VIM               i13, i11, 16                                                                      // offset lastY [  707] ....................// contains 0, 16, or 32
       VMOVW             vr01, 24-1, 128                                                                        // counter [  708] ....................// we need 16 + 4 + 4 rows in ref
       VMULW.32          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/4                                          // offset offset [  709] 
#endif // MPEG4BUILD


  
       VJP!i10           r3                                                                             // lastX ShiftZero [  714] ....................// if zero displacement horiz
       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY-ME_REFERENCE_ARRAY_STRIDE, 64  //DS__________________________ NewPtr [  715] ....................// destination pointer
       VIM               i9, i13, SDM_ME_REFERENCE_ARRAY  //DS______________________________________________ RefPtr offset [  716] ....................// source pointer
#ifdef MPEG4BUILD
       VIM               i13, i10, 8  //DS___________________________________________________________________ offset lastX [  718] 
#else
// ****** Warning : Misplaced delay slot marker found at line number 720 *****
LINE 720".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VIM               i13, i10, 16                                                                      // offset lastX [  720] 
#endif

              //  Stall: [i13]  ................................      I13  
       VJP!i13           r4                                                                           // offset ShiftLeft4 [  723] ....................// if disp by -4 horiz
       VNOP                //DS___________________________________________________________________________________________ [  724] 
       VNOP                //DS___________________________________________________________________________________________ [  725] 
       VNOP                //DS___________________________________________________________________________________________ [  726] 

// Begin                                                                                                                // [  728] ....................// 1
//vec16  vr17  ref1_shift                                                                                               // [  729] 

//ShiftRight4 --------                                                                                                  // [  731] ....................// displacement by 4 horiz
 vendrec r5                                                                                                             // [  731] 
 vrec r5                                                                                                                // [  731] 
// we need to shift along by 8 pels

// load two continguous vectors
LINE 735".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [  735] 
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [  736] 

// increment counters
       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE                                              // NewPtr NewPtr [  739] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE                                                // RefPtr RefPtr [  740] 

       VSR8              vr17, vr16, 8                                                                  // ref1_shift ref1 [  742] ....................// shift left one by 8 - we won't use first 8 pels
       VMR4W             vr15, vr16, vr15                                                                // ref0 ref1 ref0 [  743] ....................// shift by 8 


       VJD.i15           i15, r5                                                            // counter counter ShiftRight4 [  746] 
       VST128            vr17,[i14, 16]  //DS___________________________________________________________ ref1_shift NewPtr [  747] ....................// store 16 pels
       VST128            vr15,[i14, 0]  //DS__________________________________________________________________ ref0 NewPtr [  748] ....................// store 8 pels
       VNOP                //DS___________________________________________________________________________________________ [  749] 

// End                                                                                                                  // [  751] ....................// 1

LINE 753".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJP               r6                                                                                   // EndShift4 [  753] 
       VNOP                //DS___________________________________________________________________________________________ [  754] 
       VNOP                //DS___________________________________________________________________________________________ [  755] 
       VNOP                //DS___________________________________________________________________________________________ [  756] 

// Begin                                                                                                                // [  758] ....................// 1
//vec16  vr17  ref1_shift                                                                                               // [  759] 

//ShiftZero --------                                                                                                    // [  761] 
 vendrec r3                                                                                                             // [  761] 
 vrec r3                                                                                                                // [  761] 
// zero pos horizontally - need to shift right by 4 pels

LINE 764".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [  764] 
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [  765] 

       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE                                              // NewPtr NewPtr [  767] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE                                                // RefPtr RefPtr [  768] 

       VSR8              vr17, vr16, 4                                                                  // ref1_shift ref1 [  770] ....................// shift by 4 pels
       VMR2W             vr15, vr16, vr15                                                                // ref0 ref1 ref0 [  771] ....................// shift by 4 pels 

       VJD.i15           i15, r3                                                              // counter counter ShiftZero [  773] 
       VST128            vr17,[i14, 16]  //DS___________________________________________________________ ref1_shift NewPtr [  774] 
       VST128            vr15,[i14, 0]  //DS__________________________________________________________________ ref0 NewPtr [  775] 
       VNOP                //DS___________________________________________________________________________________________ [  776] 

// End                                                                                                                  // [  778] ....................// 1

LINE 780".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJP               r6                                                                                   // EndShift4 [  780] 
       VNOP                //DS___________________________________________________________________________________________ [  781] 
       VNOP                //DS___________________________________________________________________________________________ [  782] 
       VNOP                //DS___________________________________________________________________________________________ [  783] 


//ShiftLeft4 --------                                                                                                   // [  786] 
 vendrec r4                                                                                                             // [  786] 
 vrec r4                                                                                                                // [  786] 
// best x direction was -4
// this means that we only need to shift the rows really


LINE 791".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [  791] 
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [  792] 

       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE                                              // NewPtr NewPtr [  794] 

       VJD.i15           i15, r4                                                             // counter counter ShiftLeft4 [  796] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE  //DS____________________________________________ RefPtr RefPtr [  797] 
       VST128            vr16,[i14, 16]  //DS_________________________________________________________________ ref1 NewPtr [  798] 
       VST128            vr15,[i14, 0]  //DS__________________________________________________________________ ref0 NewPtr [  799] 


//EndShift4 --------                                                                                                    // [  802] 
 vendrec r6                                                                                                             // [  802] 
 vrec r6                                                                                                                // [  802] 
// End                                                                                                                  // [  803] ....................//*** of repositioning of reference data
// Begin                                                                                                                // [  804] ....................//*** of second integer step
// now do +/- 2  step search 
//vec16  vr15  ref0                                                                                                     // [  806] 
//vec16  vr16  ref4                                                                                                     // [  806] 
//vec16  vr17  ref1                                                                                                     // [  807] 
//vec16  vr18  ref5                                                                                                     // [  807] 
//vec16  vr19  ref2                                                                                                     // [  808] 
//vec16  vr20  ref6                                                                                                     // [  808] 
//vec16  vr21  ref3                                                                                                     // [  809] 
//vec16  vr22  ref7                                                                                                     // [  809] 
//vec16  vr23  temp0                                                                                                    // [  810] 
//vec16  vr24  temp1                                                                                                    // [  810] 
//vec16  vr25  temp2                                                                                                    // [  810] 
//vec16  vr26  temp3                                                                                                    // [  810] 
//vec16  vr27  PartRes                                                                                                  // [  811] 
// p16    i13  flag                                                                                                     // [  812] 
//vec16  vr28  topbit                                                                                                   // [  813] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |              |          flag|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                        ref0                                                           |
//|VR16|                                                        ref4                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref5                                                           |
//|VR19|                                                        ref2                                                           |
//|VR20|                                                        ref6                                                           |
//|VR21|                                                        ref3                                                           |
//|VR22|                                                        ref7                                                           |
//|VR23|                                                       temp0                                                           |
//|VR24|                                                       temp1                                                           |
//|VR25|                                                       temp2                                                           |
//|VR26|                                                       temp3                                                           |
//|VR27|                                                     PartRes                                                           |
//|VR28|                                                      topbit                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

//vmovw 'StepMult, 8 // steps of 2 pels in 1/4 pel units
LINE 816".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VASRW.16          vr01, vr01, 1                                                                // StepMult StepMult [  816] 

  ld r1,[r17,8];                                                                                      // VSTACK initRDvals [  818] 
       VJL               vr31, r1                                                                                       // [  818] 
       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y]  //DS__________________________________ cur0 CirBuf [  819] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE]  //DS_______________ cur1 CirBuf [  820] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*2]  //DS_____________ cur2 CirBuf [  821] 


       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [  824] 
// accumulate sad for (2, -2) first 4 rows
//find next 4 pels
// accumulate sad for (0, -2) first 4 rows
// find -2 location
// accumulate sad for (-2, -2) first 4 rows


       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*3]                 // cur3 CirBuf [  832] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*2+16]                                          // ref4 RefPtr [  833] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*2]                                             // ref0 RefPtr [  834] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*3+16]                                          // ref5 RefPtr [  835] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*3]                                             // ref1 RefPtr [  836] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*4+16]                                          // ref6 RefPtr [  837] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*4]                                             // ref2 RefPtr [  838] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*5+16]                                          // ref7 RefPtr [  839] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*5]                                             // ref3 RefPtr [  840] 
       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  841] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  842] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  843] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  844] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  845] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  846] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  847] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  848] 
       VSADBW.8          vr27, vr02, vr25                                                            // PartRes cur0 temp2 [  849] 
       VSADBAW.8         vr27, vr03, vr26                                                            // PartRes cur1 temp3 [  850] 

       VMR2W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  852] 
       VMR2W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  853] 
       VMR2W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  854] 
       VMR2W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  855] 
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  856] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  857] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  858] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  859] 

       VMR3W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  861] 
       VMR3W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  862] 
       VMR3W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  863] 
       VMR3W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  864] 
       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  865] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  866] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  867] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  868] 
       VSADBW.16         vr27, vr03, vr26                                                            // PartRes cur1 temp3 [  869] 
       VSADBAW.16        vr27, vr02, vr25                                                            // PartRes cur0 temp2 [  870] 

// accumulate sad for (2, 0) first  rows 2- 6
// accumulate sad for (2, -2) second 4 rows
// accumulate sad for (0, -2) second 4 rows
// accumulate sad for (-2, 0) first 4 rows
// accumulate sad for (-2, -2) second 4 rows
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*6+16]                                          // ref4 RefPtr [  877] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*6]                                             // ref0 RefPtr [  878] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*7+16]                                          // ref5 RefPtr [  879] 

       VADDW.31          vr14, vr14, vr27                                                         // result result PartRes [  881] 

       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*7]                                             // ref1 RefPtr [  883] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*8+16]                                          // ref6 RefPtr [  884] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*8]                                             // ref2 RefPtr [  885] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*9+16]                                          // ref7 RefPtr [  886] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*9]                                             // ref3 RefPtr [  887] 
       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  888] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  889] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  890] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  891] 
       VSADBW.32         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  892] 
       VSADBAW.32        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  893] 
       VSADBAW.32        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  894] 
       VSADBAW.32        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  895] 
       VSADBW.8          vr27, vr04, vr23                                                            // PartRes cur2 temp0 [  896] 
       VSADBAW.8         vr27, vr05, vr24                                                            // PartRes cur3 temp1 [  897] 
       VSADBAW.8         vr27, vr06, vr25                                                            // PartRes cur4 temp2 [  898] 
       VSADBAW.8         vr27, vr07, vr26                                                            // PartRes cur5 temp3 [  899] 
       VSADBW.1          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  900] 
       VSADBAW.1         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  901] 
       VSADBAW.1         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  902] 
       VSADBAW.1         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  903] 

       VMR2W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  905] 
       VMR2W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  906] 
       VMR2W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  907] 
       VMR2W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  908] 
       VSADBW.64         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  909] 
       VSADBAW.64        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  910] 
       VSADBAW.64        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  911] 
       VSADBAW.64        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  912] 
       VSADBW.2          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  913] 
       VSADBAW.2         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  914] 
       VSADBAW.2         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  915] 
       VSADBAW.2         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  916] 
       VMR3W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  917] 
       VMR3W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  918] 
       VMR3W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  919] 
       VMR3W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  920] 
       VSADBW.128        vr27, vr02, vr23                                                            // PartRes cur0 temp0 [  921] 
       VSADBAW.128       vr27, vr03, vr24                                                            // PartRes cur1 temp1 [  922] 
       VSADBAW.128       vr27, vr04, vr25                                                            // PartRes cur2 temp2 [  923] 
       VSADBAW.128       vr27, vr05, vr26                                                            // PartRes cur3 temp3 [  924] 
       VSADBW.16         vr27, vr04, vr23                                                            // PartRes cur2 temp0 [  925] 
       VSADBAW.16        vr27, vr05, vr24                                                            // PartRes cur3 temp1 [  926] 
       VSADBAW.16        vr27, vr06, vr25                                                            // PartRes cur4 temp2 [  927] 
       VSADBAW.16        vr27, vr07, vr26                                                            // PartRes cur5 temp3 [  928] 
       VSADBW.4          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  929] 
       VSADBAW.4         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  930] 
       VSADBAW.4         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  931] 
       VSADBAW.4         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  932] 

// accumulate sad for (2, 0) first  rows 2- 6
// accumulate sad for (2, -2) second 4 rows
// accumulate sad for (0, -2) second 4 rows

// accumulate sad for (-2, 0) first 4 rows
// accumulate sad for (-2, -2) second 4 rows

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*10+16]                                         // ref4 RefPtr [  941] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*10]                                            // ref0 RefPtr [  942] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*11+16]                                         // ref5 RefPtr [  943] 

       VADDW.255         vr14, vr14, vr27                                                         // result result PartRes [  945] 

       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*11]                                            // ref1 RefPtr [  947] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*12+16]                                         // ref6 RefPtr [  948] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*12]                                            // ref2 RefPtr [  949] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*13+16]                                         // ref7 RefPtr [  950] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*13]                                            // ref3 RefPtr [  951] 
       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  952] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  953] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  954] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  955] 
       VSADBW.32         vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  956] 
       VSADBAW.32        vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  957] 
       VSADBAW.32        vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  958] 
       VSADBAW.32        vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  959] 
       VSADBW.8          vr27, vr08, vr23                                                            // PartRes cur6 temp0 [  960] 
       VSADBAW.8         vr27, vr09, vr24                                                            // PartRes cur7 temp1 [  961] 
       VSADBAW.8         vr27, vr10, vr25                                                            // PartRes cur8 temp2 [  962] 
       VSADBAW.8         vr27, vr11, vr26                                                            // PartRes cur9 temp3 [  963] 
       VSADBW.1          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  964] 
       VSADBAW.1         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  965] 
       VSADBAW.1         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  966] 
       VSADBAW.1         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  967] 
       VMR2W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  968] 
       VMR2W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  969] 
       VMR2W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  970] 
       VMR2W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  971] 
       VSADBW.64         vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  972] 
       VSADBAW.64        vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  973] 
       VSADBAW.64        vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  974] 
       VSADBAW.64        vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  975] 
       VSADBW.2          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  976] 
       VSADBAW.2         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  977] 
       VSADBAW.2         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  978] 
       VSADBAW.2         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  979] 
       VMR3W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [  980] 
       VMR3W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [  981] 
       VMR3W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [  982] 
       VMR3W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [  983] 
       VSADBW.128        vr27, vr06, vr23                                                            // PartRes cur4 temp0 [  984] 
       VSADBAW.128       vr27, vr07, vr24                                                            // PartRes cur5 temp1 [  985] 
       VSADBAW.128       vr27, vr08, vr25                                                            // PartRes cur6 temp2 [  986] 
       VSADBAW.128       vr27, vr09, vr26                                                            // PartRes cur7 temp3 [  987] 
       VSADBW.16         vr27, vr08, vr23                                                            // PartRes cur6 temp0 [  988] 
       VSADBAW.16        vr27, vr09, vr24                                                            // PartRes cur7 temp1 [  989] 
       VSADBAW.16        vr27, vr10, vr25                                                            // PartRes cur8 temp2 [  990] 
       VSADBAW.16        vr27, vr11, vr26                                                            // PartRes cur9 temp3 [  991] 
       VSADBW.4          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [  992] 
       VSADBAW.4         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [  993] 
       VSADBAW.4         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [  994] 
       VSADBAW.4         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [  995] 

// accumulate sad for (2, 0) first  rows 2- 6
// accumulate sad for (2, -2) second 4 rows
// accumulate sad for (0, -2) second 4 rows
// accumulate sad for (-2, 0) first 4 rows
// accumulate sad for (-2, -2) second 4 rows

       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*12]                // cur0 CirBuf [ 1003] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*13]                // cur1 CirBuf [ 1004] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*14]                // cur2 CirBuf [ 1005] 

       VADDW.255         vr14, vr14, vr27                                                         // result result PartRes [ 1007] 

       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*15]                // cur3 CirBuf [ 1009] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*14+16]                                         // ref4 RefPtr [ 1010] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*14]                                            // ref0 RefPtr [ 1011] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*15+16]                                         // ref5 RefPtr [ 1012] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*15]                                            // ref1 RefPtr [ 1013] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*16+16]                                         // ref6 RefPtr [ 1014] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*16]                                            // ref2 RefPtr [ 1015] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*17+16]                                         // ref7 RefPtr [ 1016] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*17]                                            // ref3 RefPtr [ 1017] 
       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1018] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1019] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1020] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1021] 
       VSADBW.32         vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1022] 
       VSADBAW.32        vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1023] 
       VSADBAW.32        vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1024] 
       VSADBAW.32        vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1025] 
       VSADBW.8          vr27, vr12, vr23                                                           // PartRes cur10 temp0 [ 1026] 
       VSADBAW.8         vr27, vr13, vr24                                                           // PartRes cur11 temp1 [ 1027] 
       VSADBAW.8         vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1028] 
       VSADBAW.8         vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1029] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1030] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1031] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1032] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1033] 
       VMR2W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1034] 
       VMR2W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1035] 
       VMR2W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1036] 
       VMR2W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1037] 
       VSADBW.64         vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1038] 
       VSADBAW.64        vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1039] 
       VSADBAW.64        vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1040] 
       VSADBAW.64        vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1041] 
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1042] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1043] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1044] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1045] 
       VMR3W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1046] 
       VMR3W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1047] 
       VMR3W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1048] 
       VMR3W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1049] 
       VSADBW.128        vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1050] 
       VSADBAW.128       vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1051] 
       VSADBAW.128       vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1052] 
       VSADBAW.128       vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1053] 
       VSADBW.16         vr27, vr12, vr23                                                           // PartRes cur10 temp0 [ 1054] 
       VSADBAW.16        vr27, vr13, vr24                                                           // PartRes cur11 temp1 [ 1055] 
       VSADBAW.16        vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1056] 
       VSADBAW.16        vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1057] 
       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1058] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1059] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1060] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1061] 

// accumulate sad for (2, 0) first  rows 2- 6
// accumulate sad for (-2, 0) first 4 rows

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*18+16]                                         // ref4 RefPtr [ 1066] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*18]                                            // ref0 RefPtr [ 1067] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*19+16]                                         // ref5 RefPtr [ 1068] 

       VADDW.255         vr14, vr14, vr27                                                         // result result PartRes [ 1070] 
       VMOVW             vr28, 0x8000, 255                                                                       // topbit [ 1071] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*19]                                            // ref1 RefPtr [ 1072] 
       VXOR              vr23, vr14, vr28                                                           // temp0 result topbit [ 1073] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*20+16]                                         // ref6 RefPtr [ 1074] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*20]                                            // ref2 RefPtr [ 1075] 

//check here if all partial results are already greater than current origin

       VXMINW.255        vr23, vr23, i6                                                        // temp0 temp0 PatternMatch [ 1079] 
       VXOR.1            vr01, vr01, vr28                                                      // BestCost BestCost topbit [ 1080] 
              //  Stall: [vr23]  ...............................      vr01.001      vr19          vr23    
       VLTW.1            i13, vr23, vr01                                                            // flag temp0 BestCost [ 1081] 
       VXOR.1            vr01, vr01, vr28                                                      // BestCost BestCost topbit [ 1082] 
       VAND.32           vr01, vr01, 0x1                                                                      // flag flag [ 1083] ....................// only interested in lowest bit
       VMOVW             vr01, 0, 12                                                                        // lastX'lastY [ 1084] ....................// need this to set up next stage reference data
       VJP!i13           r7                                                                       // flag EndofSecondStage [ 1085] ....................// omit rest as all partial sums are already greater

       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*21+16]  //DS_____________________________________ ref7 RefPtr [ 1087] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*21]  //DS________________________________________ ref3 RefPtr [ 1088] 
       VMR1W             vr23, vr16, vr15  //DS___________________________________________________________ temp0 ref4 ref0 [ 1089] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1090] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1091] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1092] 
       VSADBW.32         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1093] 
       VSADBAW.32        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1094] 
       VSADBAW.32        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1095] 
       VSADBAW.32        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1096] 
       VSADBW.8          vr27, vr04, vr23                                                            // PartRes cur2 temp0 [ 1097] 
       VSADBAW.8         vr27, vr05, vr24                                                            // PartRes cur3 temp1 [ 1098] 
       VMR2W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1099] 
       VMR2W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1100] 
       VMR2W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1101] 
       VMR2W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1102] 
       VSADBW.64         vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1103] 
       VSADBAW.64        vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1104] 
       VSADBAW.64        vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1105] 
       VSADBAW.64        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1106] 
       VMR3W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1107] 
       VMR3W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1108] 
       VMR3W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1109] 
       VMR3W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1110] 
       VSADBW.128        vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1111] 
       VSADBAW.128       vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1112] 
       VSADBAW.128       vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1113] 
       VSADBAW.128       vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1114] 


  ld r1,[r17,4];                                                                                       // VSTACK endofIter [ 1117] 
       VJL               vr31, r1                                                                                       // [ 1117] 
       VSADBW.16         vr27, vr04, vr23  //DS________________________________________________________ PartRes cur2 temp0 [ 1118] 
       VSADBAW.16        vr27, vr05, vr24  //DS________________________________________________________ PartRes cur3 temp1 [ 1119] 
              //  Stall: [vr27]  ...............................      vr27.016
              //  Stall: [vr27]  ...............................      vr27.016
              //  Stall: [vr27]  ...............................      vr27.016
       VADDW.248         vr14, vr14, vr27  //DS_____________________________________________________ result result PartRes [ 1120] ....................// not first 3

//EndofSecondStage --------                                                                                             // [ 1122] 
 vendrec r7                                                                                                             // [ 1122] 
 vrec r7                                                                                                                // [ 1122] 

// End                                                                                                                  // [ 1124] ....................//*** of second integer step

// Begin                                                                                                                // [ 1126] ....................//*** of repositioning of reference data
// need to move reference data
// this depends on the lastX and lastY values
// p16    i13  offset                                                                                                   // [ 1129] 
// p16    i14  NewPtr                                                                                                   // [ 1130] 
// p16    i15  counter                                                                                                  // [ 1130] 
//vec16  vr15  ref0                                                                                                     // [ 1131] 
//vec16  vr16  ref1                                                                                                     // [ 1131] 
//vec16  vr17  ref2                                                                                                     // [ 1132] 
//vec16  vr18  ref3                                                                                                     // [ 1132] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|       counter|        NewPtr|        offset|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                        ref0                                                           |
//|VR16|                                                        ref1                                                           |
//|VR17|                                                        ref2                                                           |
//|VR18|                                                        ref3                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+


// start at (2+lastY)*ME_REFERENCE_ARRAY_STRIDE + SDM_ME_REFERENCE_ARRAY
#ifdef MPEG4BUILD
LINE 1137".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VIM               i13, i11, 4                                                                       // offset lastY [ 1137] ....................// 0, 4 or 8
       VMOVW             vr01, 10-1, 128                                                                        // counter [ 1138] ....................// we only require 16 + 2+ 2 rows of ref
       VMULW.32          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/2                                          // offset offset [ 1139] 
#else
       VIM               i13, i11, 8                                                                       // offset lastY [ 1141] ....................// 0, 8 or 16
       VMOVW             vr01, 10-1, 128                                                                        // counter [ 1142] ....................// we only require 16 + 2+ 2 rows of ref
       VMULW.32          vr01, vr01, ME_REFERENCE_ARRAY_STRIDE/4                                          // offset offset [ 1143] 
#endif //MPEG4BUILD


       VJP!i10           r8                                                                            // lastX ShiftZero2 [ 1147] ....................// last horizontal winner was 0
       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY-2*ME_REFERENCE_ARRAY_STRIDE, 64  //DS________________________ NewPtr [ 1148] ....................// dest ptr
       VIM               i9, i13, SDM_ME_REFERENCE_ARRAY  //DS______________________________________________ RefPtr offset [ 1149] ....................// source ptr
#ifdef MPEG4BUILD
       VIM               i13, i10, -4  //DS__________________________________________________________________ offset lastX [ 1151] 
#else
// ****** Warning : Misplaced delay slot marker found at line number 1153 *****
LINE 1153".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VIM               i13, i10, -8                                                                      // offset lastX [ 1153] 
#endif

              //  Stall: [i13]  ................................      I13  
       VJP!i13           r9                                                                          // offset ShiftRight2 [ 1156] ....................// last horizontal winner was +2
       VNOP                //DS___________________________________________________________________________________________ [ 1157] 
       VNOP                //DS___________________________________________________________________________________________ [ 1158] 
       VNOP                //DS___________________________________________________________________________________________ [ 1159] 


//ShiftLeft2 --------                                                                                                   // [ 1162] 
 vendrec r10                                                                                                            // [ 1162] 
 vrec r10                                                                                                               // [ 1162] 
// best x direction was +2
// this means that we only need to shift the rows really
LINE 1165".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [ 1165] 
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [ 1166] 
       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE*2                                            // NewPtr NewPtr [ 1167] 
       VLD128            vr17,[i9, 32]                                                                      // ref2 RefPtr [ 1168] 
       VLD128            vr18,[i9, 48]                                                                      // ref3 RefPtr [ 1169] 
       VST128            vr15,[i14, 0]                                                                      // ref0 NewPtr [ 1170] 
       VST128            vr16,[i14, 16]                                                                     // ref1 NewPtr [ 1171] 
       VJD.i15           i15, r10                                                            // counter counter ShiftLeft2 [ 1172] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ RefPtr RefPtr [ 1173] 
       VST128            vr17,[i14, 32]  //DS_________________________________________________________________ ref2 NewPtr [ 1174] 
       VST128            vr18,[i14, 48]  //DS_________________________________________________________________ ref3 NewPtr [ 1175] 


       VJP               r11                                                                                  // EndShift2 [ 1178] 
       VNOP                //DS___________________________________________________________________________________________ [ 1179] 
       VNOP                //DS___________________________________________________________________________________________ [ 1180] 
       VNOP                //DS___________________________________________________________________________________________ [ 1181] 


// Begin                                                                                                                // [ 1184] ....................// 1
//vec16  vr19  ref1_shift                                                                                               // [ 1185] 
//vec16  vr20  ref3_shift                                                                                               // [ 1185] 

//ShiftRight2 --------                                                                                                  // [ 1187] ....................// last horizontal winner was -2
 vendrec r9                                                                                                             // [ 1187] 
 vrec r9                                                                                                                // [ 1187] 
// we need to shift along by 4 pels
LINE 1189".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [ 1189] 
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [ 1190] 
       VLD128            vr17,[i9, 32]                                                                      // ref2 RefPtr [ 1191] 
       VLD128            vr18,[i9, 48]                                                                      // ref3 RefPtr [ 1192] 
// increment the pointers
       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE*2                                            // NewPtr NewPtr [ 1194] 

       VSR8              vr19, vr16, 4                                                                  // ref1_shift ref1 [ 1196] ....................// shift by 4 pixels
       VMR2W             vr15, vr16, vr15                                                                // ref0 ref1 ref0 [ 1197] ....................// shift by 4 pixels
       VSR8              vr20, vr18, 4                                                                  // ref3_shift ref3 [ 1198] ....................// shift by 4 pixels
       VMR2W             vr17, vr18, vr17                                                                // ref2 ref3 ref2 [ 1199] ....................// shift by 4 pixels
       VST128            vr19,[i14, 16]                                                               // ref1_shift NewPtr [ 1200] 
       VST128            vr15,[i14, 0]                                                                      // ref0 NewPtr [ 1201] 
       VJD.i15           i15, r9                                                            // counter counter ShiftRight2 [ 1202] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ RefPtr RefPtr [ 1203] 
       VST128            vr20,[i14, 48]  //DS___________________________________________________________ ref3_shift NewPtr [ 1204] 
       VST128            vr17,[i14, 32]  //DS_________________________________________________________________ ref2 NewPtr [ 1205] ....................// store

       VJP               r11                                                                                  // EndShift2 [ 1207] 
       VNOP                //DS___________________________________________________________________________________________ [ 1208] 
       VNOP                //DS___________________________________________________________________________________________ [ 1209] 
       VNOP                //DS___________________________________________________________________________________________ [ 1210] 

// End                                                                                                                  // [ 1212] ....................// 1


// Begin                                                                                                                // [ 1215] ....................// 1
//vec16  vr19  ref1_shift                                                                                               // [ 1216] 
//vec16  vr20  ref3_shift                                                                                               // [ 1216] 

//ShiftZero2 --------                                                                                                   // [ 1218] 
 vendrec r8                                                                                                             // [ 1218] 
 vrec r8                                                                                                                // [ 1218] 
// zero pos horizontally - need to shift right by 2 pels
LINE 1220".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr15,[i9, 0]                                                                       // ref0 RefPtr [ 1220] 
       VLD128            vr16,[i9, 16]                                                                      // ref1 RefPtr [ 1221] 
       VLD128            vr17,[i9, 32]                                                                      // ref2 RefPtr [ 1222] 
       VLD128            vr18,[i9, 48]                                                                      // ref3 RefPtr [ 1223] 
       VIM               i14, i14, ME_REFERENCE_ARRAY_STRIDE*2                                            // NewPtr NewPtr [ 1224] 

       VSR8              vr19, vr16, 2                                                                  // ref1_shift ref1 [ 1226] ....................// shift by 2 pixels
       VMR1W             vr15, vr16, vr15                                                                // ref0 ref1 ref0 [ 1227] ....................// shift by 2 pixels
       VSR8              vr20, vr18, 2                                                                  // ref3_shift ref3 [ 1228] ....................// shift by 2 pixels
       VMR1W             vr17, vr18, vr17                                                                // ref2 ref3 ref2 [ 1229] ....................// shift by 2 pixels
       VST128            vr19,[i14, 16]                                                               // ref1_shift NewPtr [ 1230] 
       VST128            vr15,[i14, 0]                                                                      // ref0 NewPtr [ 1231] 
       VJD.i15           i15, r8                                                             // counter counter ShiftZero2 [ 1232] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ RefPtr RefPtr [ 1233] 
       VST128            vr20,[i14, 48]  //DS___________________________________________________________ ref3_shift NewPtr [ 1234] 
       VST128            vr17,[i14, 32]  //DS_________________________________________________________________ ref2 NewPtr [ 1235] 

// End                                                                                                                  // [ 1237] ....................// 1






//EndShift2 --------                                                                                                    // [ 1244] 
 vendrec r11                                                                                                            // [ 1244] 
 vrec r11                                                                                                               // [ 1244] 
// End                                                                                                                  // [ 1245] ....................//***  of repositioning of reference data
// change RefPtr to point to start of best +/- offset in Ref array

// Begin                                                                                                                // [ 1248] ....................//*** of third integer step 
// now do the final +/- 1 integer step
//vec16  vr15  ref0                                                                                                     // [ 1250] 
//vec16  vr16  ref4                                                                                                     // [ 1250] 
//vec16  vr17  ref1                                                                                                     // [ 1251] 
//vec16  vr18  ref5                                                                                                     // [ 1251] 
//vec16  vr19  ref2                                                                                                     // [ 1252] 
//vec16  vr20  ref6                                                                                                     // [ 1252] 
//vec16  vr21  ref3                                                                                                     // [ 1253] 
//vec16  vr22  ref7                                                                                                     // [ 1253] 
//vec16  vr23  temp0                                                                                                    // [ 1254] 
//vec16  vr24  temp1                                                                                                    // [ 1254] 
//vec16  vr25  temp2                                                                                                    // [ 1254] 
//vec16  vr26  temp3                                                                                                    // [ 1254] 
//vec16  vr27  PartRes                                                                                                  // [ 1255] 
// p16    i13  flag                                                                                                     // [ 1256] 
//vec16  vr28  topbit                                                                                                   // [ 1257] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|              |              |          flag|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|                                                        ref0                                                           |
//|VR16|                                                        ref4                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref5                                                           |
//|VR19|                                                        ref2                                                           |
//|VR20|                                                        ref6                                                           |
//|VR21|                                                        ref3                                                           |
//|VR22|                                                        ref7                                                           |
//|VR23|                                                       temp0                                                           |
//|VR24|                                                       temp1                                                           |
//|VR25|                                                       temp2                                                           |
//|VR26|                                                       temp3                                                           |
//|VR27|                                                     PartRes                                                           |
//|VR28|                                                      topbit                                                           |
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

//vmovw 'StepMult, 4 // steps of 1 pels in 1/4 pel units
LINE 1260".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VASRW.16          vr01, vr01, 1                                                                // StepMult StepMult [ 1260] 
  ld r1,[r17,8];                                                                                      // VSTACK initRDvals [ 1261] 
       VJL               vr31, r1                                                                                       // [ 1261] 
       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*0]  //DS_____________ cur0 CirBuf [ 1262] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*1]  //DS_____________ cur1 CirBuf [ 1263] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*2]  //DS_____________ cur2 CirBuf [ 1264] 

       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [ 1266] 

// accumulate sad for (1, -1) first 4 rows
// accumulate sad for (0, -1) first 4 rows
// accumulate sad for (-1, -1) first 4 rows

       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*3]                 // cur3 CirBuf [ 1272] 

       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*1+16]                                          // ref4 RefPtr [ 1274] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*1]                                             // ref0 RefPtr [ 1275] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*2+16]                                          // ref5 RefPtr [ 1276] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*2]                                             // ref1 RefPtr [ 1277] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*3+16]                                          // ref6 RefPtr [ 1278] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*3]                                             // ref2 RefPtr [ 1279] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*4+16]                                          // ref7 RefPtr [ 1280] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*4]                                             // ref3 RefPtr [ 1281] 

       VMRB              vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1283] ....................// shift by 1
       VMRB              vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1284] 
       VMRB              vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1285] 
       VMRB              vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1286] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1287] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1288] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1289] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1290] 
       VSADBW.8          vr27, vr02, vr24                                                            // PartRes cur0 temp1 [ 1291] 
       VSADBAW.8         vr27, vr03, vr25                                                            // PartRes cur1 temp2 [ 1292] 
       VSADBAW.8         vr27, vr04, vr26                                                            // PartRes cur2 temp3 [ 1293] 
       VSADBW.32         vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1294] 
       VSADBAW.32        vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1295] 
       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1296] ....................// shift by 2
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1297] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1298] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1299] 
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1300] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1301] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1302] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1303] 
       VSADBW.64         vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1304] 
       VSADBAW.64        vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1305] 

       VSR8              vr16, vr16, 2                                                                        // ref4 ref4 [ 1307] ....................// shift ref4 by 2 
       VSR8              vr18, vr18, 2                                                                        // ref5 ref5 [ 1308] ....................// shift ref5 by 2
       VSR8              vr20, vr20, 2                                                                        // ref6 ref6 [ 1309] ....................// shift ref6 by 2
       VSR8              vr22, vr22, 2                                                                        // ref7 ref7 [ 1310] ....................// shift ref7 by 2
       VMRB              vr23, vr16, vr23                                                              // temp0 ref4 temp0 [ 1311] ....................// shift by an extra 1 - i.e 3 in all
       VMRB              vr24, vr18, vr24                                                              // temp1 ref5 temp1 [ 1312] ....................// shift by an extra 1
       VMRB              vr25, vr20, vr25                                                              // temp2 ref6 temp2 [ 1313] ....................// shift by an extra 1
       VMRB              vr26, vr22, vr26                                                              // temp3 ref7 temp3 [ 1314] ....................// shift by an extra 1

       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1316] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1317] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1318] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1319] 
       VSADBW.16         vr27, vr02, vr24                                                            // PartRes cur0 temp1 [ 1320] 
       VSADBAW.16        vr27, vr03, vr25                                                            // PartRes cur1 temp2 [ 1321] 
       VSADBAW.16        vr27, vr04, vr26                                                            // PartRes cur2 temp3 [ 1322] 
       VSADBW.128        vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1323] 
       VSADBAW.128       vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1324] 


// accumulate sad for (1, -1) first 4 rows
// accumulate sad for (0, -1) first 4 rows
// accumulate sad for (-1, -1) first 4 rows
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*5+16]                                          // ref4 RefPtr [ 1330] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*5]                                             // ref0 RefPtr [ 1331] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*6+16]                                          // ref5 RefPtr [ 1332] 

       VADDW             vr14, vr14, vr27                                                         // result result PartRes [ 1334] 

       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*6]                                             // ref1 RefPtr [ 1336] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*7+16]                                          // ref6 RefPtr [ 1337] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*7]                                             // ref2 RefPtr [ 1338] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*8+16]                                          // ref7 RefPtr [ 1339] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*8]                                             // ref3 RefPtr [ 1340] 

       VMRB              vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1342] 
       VMRB              vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1343] 
       VMRB              vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1344] 
       VMRB              vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1345] 

       VSADBW.1          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [ 1347] 
       VSADBAW.1         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [ 1348] 
       VSADBAW.1         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [ 1349] 
       VSADBAW.1         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [ 1350] 
       VSADBW.8          vr27, vr05, vr23                                                            // PartRes cur3 temp0 [ 1351] 
       VSADBAW.8         vr27, vr06, vr24                                                            // PartRes cur4 temp1 [ 1352] 
       VSADBAW.8         vr27, vr07, vr25                                                            // PartRes cur5 temp2 [ 1353] 
       VSADBAW.8         vr27, vr08, vr26                                                            // PartRes cur6 temp3 [ 1354] 
       VSADBW.32         vr27, vr04, vr23                                                            // PartRes cur2 temp0 [ 1355] 
       VSADBAW.32        vr27, vr05, vr24                                                            // PartRes cur3 temp1 [ 1356] 
       VSADBAW.32        vr27, vr06, vr25                                                            // PartRes cur4 temp2 [ 1357] 
       VSADBAW.32        vr27, vr07, vr26                                                            // PartRes cur5 temp3 [ 1358] 

       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1360] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1361] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1362] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1363] 
       VSADBW.2          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [ 1364] 
       VSADBAW.2         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [ 1365] 
       VSADBAW.2         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [ 1366] 
       VSADBAW.2         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [ 1367] 
       VSADBW.64         vr27, vr04, vr23                                                            // PartRes cur2 temp0 [ 1368] 
       VSADBAW.64        vr27, vr05, vr24                                                            // PartRes cur3 temp1 [ 1369] 
       VSADBAW.64        vr27, vr06, vr25                                                            // PartRes cur4 temp2 [ 1370] 
       VSADBAW.64        vr27, vr07, vr26                                                            // PartRes cur5 temp3 [ 1371] 

       VSR8              vr16, vr16, 2                                                                        // ref4 ref4 [ 1373] ....................// shift ref4 by 2
       VSR8              vr18, vr18, 2                                                                        // ref5 ref5 [ 1374] ....................// shift ref5 by 2
       VSR8              vr20, vr20, 2                                                                        // ref6 ref6 [ 1375] ....................// shift ref6 by 2
       VSR8              vr22, vr22, 2                                                                        // ref7 ref7 [ 1376] ....................// shift ref7 by 2
       VMRB              vr23, vr16, vr23                                                              // temp0 ref4 temp0 [ 1377] ....................// shift by an extra 1
       VMRB              vr24, vr18, vr24                                                              // temp1 ref5 temp1 [ 1378] ....................// shift by an extra 1
       VMRB              vr25, vr20, vr25                                                              // temp2 ref6 temp2 [ 1379] ....................// shift by an extra 1
       VMRB              vr26, vr22, vr26                                                              // temp3 ref7 temp3 [ 1380] ....................// shift by an extra 1
       VSADBW.4          vr27, vr06, vr23                                                            // PartRes cur4 temp0 [ 1381] 
       VSADBAW.4         vr27, vr07, vr24                                                            // PartRes cur5 temp1 [ 1382] 
       VSADBAW.4         vr27, vr08, vr25                                                            // PartRes cur6 temp2 [ 1383] 
       VSADBAW.4         vr27, vr09, vr26                                                            // PartRes cur7 temp3 [ 1384] 
       VSADBW.16         vr27, vr05, vr23                                                            // PartRes cur3 temp0 [ 1385] 
       VSADBAW.16        vr27, vr06, vr24                                                            // PartRes cur4 temp1 [ 1386] 
       VSADBAW.16        vr27, vr07, vr25                                                            // PartRes cur5 temp2 [ 1387] 
       VSADBAW.16        vr27, vr08, vr26                                                            // PartRes cur6 temp3 [ 1388] 
       VSADBW.128        vr27, vr04, vr23                                                            // PartRes cur2 temp0 [ 1389] 
       VSADBAW.128       vr27, vr05, vr24                                                            // PartRes cur3 temp1 [ 1390] 
       VSADBAW.128       vr27, vr06, vr25                                                            // PartRes cur4 temp2 [ 1391] 
       VSADBAW.128       vr27, vr07, vr26                                                            // PartRes cur5 temp3 [ 1392] 


// accumulate sad for (1, -1) first 4 rows
// accumulate sad for (0, -1) first 4 rows
// accumulate sad for (-1, -1) first 4 rows
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*9+16]                                          // ref4 RefPtr [ 1398] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*9]                                             // ref0 RefPtr [ 1399] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*10+16]                                         // ref5 RefPtr [ 1400] 

       VADDW             vr14, vr14, vr27                                                         // result result PartRes [ 1402] 

       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*10]                                            // ref1 RefPtr [ 1404] 
       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*11+16]                                         // ref6 RefPtr [ 1405] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*11]                                            // ref2 RefPtr [ 1406] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*12+16]                                         // ref7 RefPtr [ 1407] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*12]                                            // ref3 RefPtr [ 1408] 

       VMRB              vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1410] 
       VMRB              vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1411] 
       VMRB              vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1412] 
       VMRB              vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1413] 
       VSADBW.1          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1414] 
       VSADBAW.1         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1415] 
       VSADBAW.1         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1416] 
       VSADBAW.1         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1417] 
       VSADBW.8          vr27, vr09, vr23                                                            // PartRes cur7 temp0 [ 1418] 
       VSADBAW.8         vr27, vr10, vr24                                                            // PartRes cur8 temp1 [ 1419] 
       VSADBAW.8         vr27, vr11, vr25                                                            // PartRes cur9 temp2 [ 1420] 
       VSADBAW.8         vr27, vr12, vr26                                                           // PartRes cur10 temp3 [ 1421] 
       VSADBW.32         vr27, vr08, vr23                                                            // PartRes cur6 temp0 [ 1422] 
       VSADBAW.32        vr27, vr09, vr24                                                            // PartRes cur7 temp1 [ 1423] 
       VSADBAW.32        vr27, vr10, vr25                                                            // PartRes cur8 temp2 [ 1424] 
       VSADBAW.32        vr27, vr11, vr26                                                            // PartRes cur9 temp3 [ 1425] 

       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1427] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1428] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1429] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1430] 
       VSADBW.2          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1431] 
       VSADBAW.2         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1432] 
       VSADBAW.2         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1433] 
       VSADBAW.2         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1434] 

       VSADBW.64         vr27, vr08, vr23                                                            // PartRes cur6 temp0 [ 1436] 
       VSADBAW.64        vr27, vr09, vr24                                                            // PartRes cur7 temp1 [ 1437] 
       VSADBAW.64        vr27, vr10, vr25                                                            // PartRes cur8 temp2 [ 1438] 
       VSADBAW.64        vr27, vr11, vr26                                                            // PartRes cur9 temp3 [ 1439] 


       VSR8              vr16, vr16, 2                                                                        // ref4 ref4 [ 1442] ....................// shift ref4 by 2
       VSR8              vr18, vr18, 2                                                                        // ref5 ref5 [ 1443] ....................// shift ref5 by 2
       VSR8              vr20, vr20, 2                                                                        // ref6 ref6 [ 1444] ....................// shift ref6 by 2
       VSR8              vr22, vr22, 2                                                                        // ref7 ref7 [ 1445] ....................// shift ref7 by 2
       VMRB              vr23, vr16, vr23                                                              // temp0 ref4 temp0 [ 1446] ....................// shift by an extra 1
       VMRB              vr24, vr18, vr24                                                              // temp1 ref5 temp1 [ 1447] ....................// shift by an extra 1
       VMRB              vr25, vr20, vr25                                                              // temp2 ref6 temp2 [ 1448] ....................// shift by an extra 1
       VMRB              vr26, vr22, vr26                                                              // temp3 ref7 temp3 [ 1449] ....................// shift by an extra 1
       VSADBW.4          vr27, vr10, vr23                                                            // PartRes cur8 temp0 [ 1450] 
       VSADBAW.4         vr27, vr11, vr24                                                            // PartRes cur9 temp1 [ 1451] 
       VSADBAW.4         vr27, vr12, vr25                                                           // PartRes cur10 temp2 [ 1452] 
       VSADBAW.4         vr27, vr13, vr26                                                           // PartRes cur11 temp3 [ 1453] 
       VSADBW.16         vr27, vr09, vr23                                                            // PartRes cur7 temp0 [ 1454] 
       VSADBAW.16        vr27, vr10, vr24                                                            // PartRes cur8 temp1 [ 1455] 
       VSADBAW.16        vr27, vr11, vr25                                                            // PartRes cur9 temp2 [ 1456] 
       VSADBAW.16        vr27, vr12, vr26                                                           // PartRes cur10 temp3 [ 1457] 
       VSADBW.128        vr27, vr08, vr23                                                            // PartRes cur6 temp0 [ 1458] 
       VSADBAW.128       vr27, vr09, vr24                                                            // PartRes cur7 temp1 [ 1459] 
       VSADBAW.128       vr27, vr10, vr25                                                            // PartRes cur8 temp2 [ 1460] 
       VSADBAW.128       vr27, vr11, vr26                                                            // PartRes cur9 temp3 [ 1461] 
 

// accumulate sad for (1, -1) first 4 rows
// accumulate sad for (0, -1) first 4 rows
// accumulate sad for (-1, -1) first 4 rows

       VLD128            vr02,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*12]                // cur0 CirBuf [ 1468] 
       VLD128            vr03,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*13]                // cur1 CirBuf [ 1469] 
       VLD128            vr04,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*14]                // cur2 CirBuf [ 1470] 

       VADDW             vr14, vr14, vr27                                                         // result result PartRes [ 1472] 
       VMOVW             vr28, 0x8000, 255                                                                       // topbit [ 1473] 
              //  Stall: [vr28]  ...............................      vr04          vr28    
       VXOR              vr23, vr14, vr28                                                           // temp0 result topbit [ 1474] 
       VLD128            vr05,[i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y+PCB_CURRENT_STRIDE*15]                // cur3 CirBuf [ 1475] 
       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*13+16]                                         // ref4 RefPtr [ 1476] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*13]                                            // ref0 RefPtr [ 1477] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*14+16]                                         // ref5 RefPtr [ 1478] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*14]                                            // ref1 RefPtr [ 1479] 

//check here if all partial results are already greater than current origin



       VXMINW.255        vr23, vr23, i6                                                        // temp0 temp0 PatternMatch [ 1485] 
       VXOR.1            vr01, vr01, vr28                                                      // BestCost BestCost topbit [ 1486] 
              //  Stall: [vr23]  ...............................      vr01.001      vr17          vr23    
       VLTW.1            i13, vr23, vr01                                                            // flag temp0 BestCost [ 1487] 
       VXOR.1            vr01, vr01, vr28                                                      // BestCost BestCost topbit [ 1488] 
       VAND.32           vr01, vr01, 0x1                                                                      // flag flag [ 1489] ....................// only interested in lowest bit
       VMOVW             vr01, 0, 12                                                                        // lastX'lastY [ 1490] ....................// need this to set up next stage reference data
       VJP!i13           r12                                                                       // flag EndofThirdStage [ 1491] ....................// omit rest as all partial sums are already greater


       VLD128            vr20,[i9, ME_REFERENCE_ARRAY_STRIDE*15+16]  //DS_____________________________________ ref6 RefPtr [ 1494] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE*15]  //DS________________________________________ ref2 RefPtr [ 1495] 
       VLD128            vr22,[i9, ME_REFERENCE_ARRAY_STRIDE*16+16]  //DS_____________________________________ ref7 RefPtr [ 1496] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE*16]                                            // ref3 RefPtr [ 1497] 

       VMRB              vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1499] 
       VMRB              vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1500] 
       VMRB              vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1501] 
       VMRB              vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1502] 
       VSADBW.1          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1503] 
       VSADBAW.1         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1504] 
       VSADBAW.1         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1505] 
       VSADBAW.1         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1506] 
       VSADBW.8          vr27, vr13, vr23                                                           // PartRes cur11 temp0 [ 1507] 
       VSADBAW.8         vr27, vr02, vr24                                                            // PartRes cur0 temp1 [ 1508] 
       VSADBAW.8         vr27, vr03, vr25                                                            // PartRes cur1 temp2 [ 1509] 
       VSADBAW.8         vr27, vr04, vr26                                                            // PartRes cur2 temp3 [ 1510] 
       VSADBW.32         vr27, vr12, vr23                                                           // PartRes cur10 temp0 [ 1511] 
       VSADBAW.32        vr27, vr13, vr24                                                           // PartRes cur11 temp1 [ 1512] 
       VSADBAW.32        vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1513] 
       VSADBAW.32        vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1514] 

       VMR1W             vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1516] 
       VMR1W             vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1517] 
       VMR1W             vr25, vr20, vr19                                                               // temp2 ref6 ref2 [ 1518] 
       VMR1W             vr26, vr22, vr21                                                               // temp3 ref7 ref3 [ 1519] 
       VSADBW.2          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1520] 
       VSADBAW.2         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1521] 
       VSADBAW.2         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1522] 
       VSADBAW.2         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1523] 
       VSADBW.64         vr27, vr12, vr23                                                           // PartRes cur10 temp0 [ 1524] 
       VSADBAW.64        vr27, vr13, vr24                                                           // PartRes cur11 temp1 [ 1525] 
       VSADBAW.64        vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1526] 
       VSADBAW.64        vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1527] 
       VSR8              vr16, vr16, 2                                                                        // ref4 ref4 [ 1528] ....................// shift ref4 by 2
       VSR8              vr18, vr18, 2                                                                        // ref5 ref5 [ 1529] ....................// shift ref5 by 2
       VSR8              vr20, vr20, 2                                                                        // ref6 ref6 [ 1530] ....................// shift ref6 by 2
       VSR8              vr22, vr22, 2                                                                        // ref7 ref7 [ 1531] ....................// shift ref7 by 2
       VMRB              vr23, vr16, vr23                                                              // temp0 ref4 temp0 [ 1532] ....................// shift by an extra 1
       VMRB              vr24, vr18, vr24                                                              // temp1 ref5 temp1 [ 1533] ....................// shift by an extra 1
       VMRB              vr25, vr20, vr25                                                              // temp2 ref6 temp2 [ 1534] ....................// shift by an extra 1
       VMRB              vr26, vr22, vr26                                                              // temp3 ref7 temp3 [ 1535] ....................// shift by an extra 1
       VSADBW.4          vr27, vr02, vr23                                                            // PartRes cur0 temp0 [ 1536] 
       VSADBAW.4         vr27, vr03, vr24                                                            // PartRes cur1 temp1 [ 1537] 
       VSADBAW.4         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1538] 
       VSADBAW.4         vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1539] 
       VSADBW.16         vr27, vr13, vr23                                                           // PartRes cur11 temp0 [ 1540] 
       VSADBAW.16        vr27, vr02, vr24                                                            // PartRes cur0 temp1 [ 1541] 
       VSADBAW.16        vr27, vr03, vr25                                                            // PartRes cur1 temp2 [ 1542] 
       VSADBAW.16        vr27, vr04, vr26                                                            // PartRes cur2 temp3 [ 1543] 
       VSADBW.128        vr27, vr12, vr23                                                           // PartRes cur10 temp0 [ 1544] 
       VSADBAW.128       vr27, vr13, vr24                                                           // PartRes cur11 temp1 [ 1545] 
       VSADBAW.128       vr27, vr02, vr25                                                            // PartRes cur0 temp2 [ 1546] 
       VSADBAW.128       vr27, vr03, vr26                                                            // PartRes cur1 temp3 [ 1547] 


       VLD128            vr16,[i9, ME_REFERENCE_ARRAY_STRIDE*17+16]                                         // ref4 RefPtr [ 1550] 
       VLD128            vr15,[i9, ME_REFERENCE_ARRAY_STRIDE*17]                                            // ref0 RefPtr [ 1551] 
       VLD128            vr18,[i9, ME_REFERENCE_ARRAY_STRIDE*18+16]                                         // ref5 RefPtr [ 1552] 
       VLD128            vr17,[i9, ME_REFERENCE_ARRAY_STRIDE*18]                                            // ref1 RefPtr [ 1553] 
   
       VADDW             vr14, vr14, vr27                                                         // result result PartRes [ 1555] 

       VMRB              vr23, vr16, vr15                                                               // temp0 ref4 ref0 [ 1557] 
              //  Stall: [vr17]  ...............................      vr17          vr23    
       VMRB              vr24, vr18, vr17                                                               // temp1 ref5 ref1 [ 1558] 
       VSADBW.8          vr27, vr05, vr23                                                            // PartRes cur3 temp0 [ 1559] 
       VSADBW.32         vr27, vr04, vr23                                                            // PartRes cur2 temp0 [ 1560] 
       VSADBAW.32        vr27, vr05, vr24                                                            // PartRes cur3 temp1 [ 1561] 
       VMR1W             vr25, vr16, vr15                                                               // temp2 ref4 ref0 [ 1562] 
       VMR1W             vr26, vr18, vr17                                                               // temp3 ref5 ref1 [ 1563] 

       VSR8              vr16, vr16, 2                                                                        // ref4 ref4 [ 1565] ....................// shift ref4 by 2
       VSR8              vr18, vr18, 2                                                                        // ref5 ref5 [ 1566] ....................// shift ref5 by 2
       VSADBW.64         vr27, vr04, vr25                                                            // PartRes cur2 temp2 [ 1567] 
       VSADBAW.64        vr27, vr05, vr26                                                            // PartRes cur3 temp3 [ 1568] 
       VMRB              vr23, vr16, vr25                                                              // temp0 ref4 temp2 [ 1569] ....................// shift by an extra 1
       VMRB              vr24, vr18, vr26                                                              // temp1 ref5 temp3 [ 1570] ....................// shift by an extra 1
              //  Stall: [vr23]  ...............................      vr23          vr24          vr27.064
       VSADBW.16         vr27, vr05, vr23                                                            // PartRes cur3 temp0 [ 1571] 


  ld r1,[r17,4];                                                                                       // VSTACK endofIter [ 1574] 
       VJL               vr31, r1                                                                                       // [ 1574] 
       VSADBW.128        vr27, vr04, vr23  //DS________________________________________________________ PartRes cur2 temp0 [ 1575] 
       VSADBAW.128       vr27, vr05, vr24  //DS________________________________________________________ PartRes cur3 temp1 [ 1576] 
              //  Stall: [vr27]  ...............................      vr27.128
              //  Stall: [vr27]  ...............................      vr27.128
              //  Stall: [vr27]  ...............................      vr27.128
       VADDW.248         vr14, vr14, vr27  //DS_____________________________________________________ result result PartRes [ 1577] 

//EndofThirdStage --------                                                                                              // [ 1579] 
 vendrec r12                                                                                                            // [ 1579] 
 vrec r12                                                                                                               // [ 1579] 
// End                                                                                                                  // [ 1580] ....................//*** of third integer step




// End                                                                                                                  // [ 1585] ....................//*** of function

    
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz IntegerME_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc IntegerME                                                                                                    // [ 1588] 


    
// [Macro] SaveVectors                                                                                                  // [ 1590] 
_Asm int Record_SaveVectors(int *mac_tab)
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
SaveVectors_RecLoop:
  ld r1,[r17 ,16]
  vrec r1


// Begin                                                                                                                // [ 1592] 
// save motion vectors to circular buffer
//vec16  vr15  bestvec                                                                                                  // [ 1594] 
// s16    vr16.4  vecXshift                                                                                             // [ 1595] 
// s16    vr17.4  tempX                                                                                                 // [ 1596] 
// s16    vr16.8  vecYshift                                                                                             // [ 1597] 
// p16    i13  flag                                                                                                     // [ 1598] 
//vec16  vr29  A.1                                                                                                      // [ 1599] 
//vec16  vr29  A_codingtype.1                                                                                           // [ 1599] 
//vec16  vr30  skip.13                                                                                                  // [ 1600] 
//vec16  vr30  skip_cost.1                                                                                              // [ 1600] 
//vec16  vr30  skip_vecx.4                                                                                              // [ 1600] 
//vec16  vr30  skip_vecy.8                                                                                              // [ 1600] 
//vec16  vr18  topbit                                                                                                   // [ 1601] 
//vec16  vr19  skipsign                                                                                                 // [ 1602] 
//vec16  vr20  Bestsign                                                                                                 // [ 1603] 
    
LINE 1605".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr18, 0x8000, 255                                                                       // topbit [ 1605] 

       VXMINW.255        vr20, vr01, 1                                                                // Bestsign BestCost [ 1607] 
       VXMINW.255        vr19, vr30, 1                                                               // skipsign skip_cost [ 1608] 
              //  Stall: [vr19]  ...............................      vr19.001      vr20.001
              //  Stall: [vr19]  ...............................      vr19.001
       VXOR              vr19, vr19, vr18                                                      // skipsign skipsign topbit [ 1609] 
       VXOR              vr20, vr20, vr18                                                      // Bestsign Bestsign topbit [ 1610] 
        
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1612".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
              //  Stall: [vr20]  ...............................      vr20    
       VLEW.32           i13, vr19, vr20                                                         // flag skipsign Bestsign [ 1612] ....................// set flag if skip is better than best

              //  Stall: [i13]  ................................      I13  
       VJP!i13           r3                                                                                // flag CarryOn [ 1614] ....................// normal vec is best
       VNOP                //DS___________________________________________________________________________________________ [ 1615] 
       VNOP                //DS___________________________________________________________________________________________ [ 1616] 
       VNOP                //DS___________________________________________________________________________________________ [ 1617] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1618".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr00, vr30, 12                                                                            // skip [ 1618] ....................// move the vecx and vecy 
    // need to get skip vectors from parking place
  //  ~vmovw 'RefPtr, SDMOF_IntraPredictionResidsLuma
  //  ~vld16 'vecXshift,    [RefPtr,2]
  //  ~vnop
        vmvw.1          vr01, vr30                                                                   // BestCost skip_cost [ 1623] 
  //  vjp .End
  //  ~vnop
  //  ~vnop
  //  ~vnop
//CarryOn --------                                                                                                      // [ 1628] 
 vendrec r3                                                                                                             // [ 1628] 
 vrec r3                                                                                                                // [ 1628] 
    
LINE 1630".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMULW.8           vr16, vr00, 256                                                                 // vecYshift VecY [ 1630] 

              //  Stall: [vr16]  ...............................      vr16.008
              //  Stall: [vr16]  ...............................      vr16.008
        VMR1W.4         vr16, vr16, vr16                                                            // vecXshift vecYshift [ 1632] 
LINE 1633".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VAND.4            vr17, vr00, 0xFF                                                                    // tempX VecX [ 1633] 
              //  Stall: [vr16]  ...............................      vr16.004      vr17.004
       VADDW.4           vr16, vr16, vr17                                                     // vecXshift vecXshift tempX [ 1634] 
    
//End --------                                                                                                          // [ 1636] 
 vendrec r4                                                                                                             // [ 1636] 
 vrec r4                                                                                                                // [ 1636] 
 #ifdef MPEG4BUILD   
LINE 1638".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr29, MBCT_MPEG4_INTER16X16, 1                                                    // A_codingtype [ 1638] 
 #else // MPEG4BUILD
       VMOVW             vr29, MBCT_H264_INTER16X16, 1                                                     // A_codingtype [ 1640] 
 #endif // MPEG4BUILD
    
   // vjp             .Generate_Soft_ME_Response

              //  Stall: [vr29]  ...............................      vr29.001
       VST8              vr29,[i4, MPO_MBCodingType]                                                // A_codingtype CirBuf [ 1645] ....................// set coding type
       VXMINW.15         vr15, vr16, 4                                                                // bestvec vecXshift [ 1646] ....................// only lane 2
              //  Stall: [vr15]  ...............................      vr15.004
              //  Stall: [vr15]  ...............................      vr15.004
       VST64             vr15,[i4, 0x10]                                                                 // bestvec CirBuf [ 1647] ....................// save 4 copies for each 8x8 block
    
// End                                                                                                                  // [ 1649] 
   
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz SaveVectors_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SaveVectors                                                                                                  // [ 1651] 



// function to perform limited step one functionality
// initially to look at (-8,0) (0,0) and (8,0)
// [Macro] ZeroDisplacement                                                                                             // [ 1655] 
_Asm int Record_ZeroDisplacement(int *mac_tab)
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
ZeroDisplacement_RecLoop:
  ld r1,[r17 ,20]
  vrec r1


// Begin                                                                                                                // [ 1657] 

LINE 1659".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr00, 0, 12                                                                          // VecX'VecY [ 1659] ....................// set vecs to zero
//vec16  vr29  savevals1.194                                                                                            // [ 1660] 
//vec16  vr29  savevals1_point.2                                                                                        // [ 1660] 
//vec16  vr29  savevals1_posX.64                                                                                        // [ 1660] 
//vec16  vr29  savevals1_posY.128                                                                                       // [ 1660] 
// p16    i13  res4                                                                                                     // [ 1661] ....................// only needed in first step
// initialise the 9 sums to the rate distortion approximation function
// set each lane equal to this value
// this sets the vectors res0 to res8 with the RD cost for each one's MV
//vec16  vr20  temp                                                                                                     // [ 1665] ....................// to make sure it doesn't conflict with anything in initRDvals
//vec16  vr15  tempres.3                                                                                                // [ 1666] 
//vec16  vr15  tempres_Ptrx.1                                                                                           // [ 1666] 
//vec16  vr15  tempres_Ptry.2                                                                                           // [ 1666] 

//vec16  vr16  ref0                                                                                                     // [ 1668] 
//vec16  vr17  ref1                                                                                                     // [ 1668] 
//vec16  vr18  ref2                                                                                                     // [ 1668] 
//vec16  vr19  ref3                                                                                                     // [ 1668] 
//vec16  vr21  ref4                                                                                                     // [ 1668] 
//vec16  vr22  ref5                                                                                                     // [ 1668] 
//vec16  vr23  PartRes                                                                                                  // [ 1669] 
//vec16  vr24  bestvec                                                                                                  // [ 1669] 
// s16    vr15.32  Partres4                                                                                             // [ 1670] ....................// this will hold the (0,0) displacement SAD cost

// p16    i14  counter                                                                                                  // [ 1672] 
// p16    i15  CurPtr                                                                                                   // [ 1673] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        CurPtr|       counter|          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |      Partres4|              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|VR16|                                                        ref0                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref2                                                           |
//|VR19|                                                        ref3                                                           |
//|VR20|                                                        temp                                                           |
//|VR21|                                                        ref4                                                           |
//|VR22|                                                        ref5                                                           |
//|VR23|                                                     PartRes                                                           |
//|VR24|                                                     bestvec                                                           |
//|............................................................................................................................|
//|VR29|savevals1_posY|savevals1_posX|              |              |              |              | savevals1_po.|              |savevals1.194   
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

// read quantiser


LINE 1678".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VDMAWAIT          0x0, 0x7f                                                                                      // [ 1678] ....................// make sure DMA of ref has completed
// ****** Warning : Misplaced delay slot marker found at line number 1679 *****
LINE 1679".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VABSW.3           vr15, vr00                                               // tempres_Ptrx'tempres_Ptry PredX'PredY [ 1679] 

              //  Stall: [vr15]  ...............................      vr15.003
       VNORMW.3          vr15, vr15                                 // tempres_Ptrx'tempres_Ptry tempres_Ptrx'tempres_Ptry [ 1681] 
              //  Stall: [vr15]  ...............................      vr15.003
       VXSUMW.255        vr20, vr15, 3                                                   // temp tempres_Ptrx'tempres_Ptry [ 1682] ....................// add the two and put in all lanesv

//vmovw result, 0xFFFF // initialise all to very high

       VMOVW             vr01, 8-1, 64                                                                          // counter [ 1686] ....................// 8 pairs of rows

              //  Stall: [vr20]  ...............................      vr01.064      vr20.003
       VMULW             vr20, vr20, 2                                                                        // temp temp [ 1688] 

       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [ 1690] 
              //  Stall: [vr20]  ...............................      vr01.002      vr20    
       VRSUBW            vr20, vr20, 62                                                                       // temp temp [ 1691] 

              //  Stall: [vr20]  ...............................      vr20    
       VMULW             vr20, vr20, i7                                                                // temp temp lambda [ 1693] 


       VIM               i9, i9, 8*ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [ 1696] 
       VIM               i15, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                      // CurPtr CirBuf [ 1697] 
       VMVW              vr01, vr20, 32                                                                       // res4 temp [ 1698] 

// this loop is the first set of 8 rows needed for the three
// zero vertical displacement SADs.  It is also the second set of 
// 8 rows needed for the -8 vertical displacement SADs
//OnlyLoop --------                                                                                                     // [ 1703] 
 vendrec r3                                                                                                             // [ 1703] 
 vrec r3                                                                                                                // [ 1703] 
LINE 1704".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 1704] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 1705] 

       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 1707] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 1708] 
       VLD128            vr02,[i15, 0]                                                                      // cur0 CurPtr [ 1709] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE]                                                     // cur1 CurPtr [ 1710] 

       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 1712] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 1713] ....................// shift by 8 pels

       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2                                              // RefPtr RefPtr [ 1715] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1716".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBW.32         vr15, vr02, vr18                                                            // Partres4 cur0 ref2 [ 1716] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1717".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBAW.32        vr15, vr03, vr22                                                            // Partres4 cur1 ref5 [ 1717] 


       VJD.i14           i14, r3                                                               // counter counter OnlyLoop [ 1720] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 1721] 
              //  Stall: [vr15]  ...............................      vr15.032      I15  
       VADDW.32          vr01, vr01, vr15  //DS________________________________________________________ res4 res4 Partres4 [ 1722] 
       VNOP                //DS___________________________________________________________________________________________ [ 1723] 

        VMR5W.1         vr01, vr01, vr01                                                                  // BestCost res4 [ 1725] 

LINE 1727".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr01, 0, 12                                                                        // lastX'lastY [ 1727] ....................// set vecs to zero

// End                                                                                                                  // [ 1729] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ZeroDisplacement_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ZeroDisplacement                                                                                             // [ 1731] 





// function to perform limited step one functionality
// initially to look at (-8,0) (0,0) and (8,0)
// [Macro] StepOne                                                                                                      // [ 1737] 
_Asm int Record_StepOne(int *mac_tab)
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
StepOne_RecLoop:
  ld r1,[r17 ,24]
  vrec r1


// Begin                                                                                                                // [ 1739] 

//vmovw  'VecX'VecY, 0  // set vecs to zero
//vec16  vr29  savevals1.194                                                                                            // [ 1742] 
//vec16  vr29  savevals1_point.2                                                                                        // [ 1742] 
//vec16  vr29  savevals1_posX.64                                                                                        // [ 1742] 
//vec16  vr29  savevals1_posY.128                                                                                       // [ 1742] 
// p16    i13  res4                                                                                                     // [ 1743] ....................// only needed in first step
// initialise the 9 sums to the rate distortion approximation function
// set each lane equal to this value
// this sets the vectors res0 to res8 with the RD cost for each one's MV
//vec16  vr20  temp                                                                                                     // [ 1747] ....................// to make sure it doesn't conflict with anything in initRDvals
//vec16  vr15  tempres.3                                                                                                // [ 1748] 
//vec16  vr15  tempres_Ptrx.1                                                                                           // [ 1748] 
//vec16  vr15  tempres_Ptry.2                                                                                           // [ 1748] 

//vec16  vr16  ref0                                                                                                     // [ 1750] 
//vec16  vr17  ref1                                                                                                     // [ 1750] 
//vec16  vr18  ref2                                                                                                     // [ 1750] 
//vec16  vr19  ref3                                                                                                     // [ 1750] 
//vec16  vr21  ref4                                                                                                     // [ 1750] 
//vec16  vr22  ref5                                                                                                     // [ 1750] 
//vec16  vr23  PartRes                                                                                                  // [ 1751] 
//vec16  vr24  bestvec                                                                                                  // [ 1751] 
// s16    vr15.32  Partres4                                                                                             // [ 1752] ....................// this will hold the (0,0) displacement SAD cost

// p16    i14  counter                                                                                                  // [ 1754] 
// p16    i15  CurPtr                                                                                                   // [ 1755] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        CurPtr|       counter|          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |      Partres4|              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|VR16|                                                        ref0                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref2                                                           |
//|VR19|                                                        ref3                                                           |
//|VR20|                                                        temp                                                           |
//|VR21|                                                        ref4                                                           |
//|VR22|                                                        ref5                                                           |
//|VR23|                                                     PartRes                                                           |
//|VR24|                                                     bestvec                                                           |
//|............................................................................................................................|
//|VR29|savevals1_posY|savevals1_posX|              |              |              |              | savevals1_po.|              |savevals1.194   
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

// read quantiser
LINE 1758".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr13, vr00, 255                                                                          // cur11 [ 1758] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1759".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr29, vr01, 194                                                                      // savevals1 [ 1759] 

  ld r1,[r17,8];                                                                                      // VSTACK initRDvals [ 1761] 
       VJL               vr31, r1                                                                                       // [ 1761] 
// now do res4 which couldn't fit in the vector
// it is (0,0)
// ****** Warning : Unmarked delay slot found at line number 1764 *****
LINE 1764".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1764] 
// ****** Warning : Unmarked delay slot found at line number 1765 *****
LINE 1765".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1765] 
// ****** Warning : Unmarked delay slot found at line number 1766 *****
LINE 1766".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1766] 
       VDMAWAIT          0x0, 0x7f                                                                                      // [ 1767] ....................// make sure DMA of ref has completed
// ****** Warning : Misplaced delay slot marker found at line number 1768 *****
LINE 1768".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VABSW.3           vr15, vr00                                               // tempres_Ptrx'tempres_Ptry PredX'PredY [ 1768] 

              //  Stall: [vr15]  ...............................      vr15.003
       VNORMW.3          vr15, vr15                                 // tempres_Ptrx'tempres_Ptry tempres_Ptrx'tempres_Ptry [ 1770] 
              //  Stall: [vr15]  ...............................      vr15.003
       VXSUMW.255        vr20, vr15, 3                                                   // temp tempres_Ptrx'tempres_Ptry [ 1771] ....................// add the two and put in all lanesv

//vmovw result, 0xFFFF // initialise all to very high

       VMOVW             vr01, 4-1, 64                                                                          // counter [ 1775] ....................// 4 pairs of rows

              //  Stall: [vr20]  ...............................      vr01.064      vr20.003
       VMULW             vr20, vr20, 2                                                                        // temp temp [ 1777] 

       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [ 1779] 
              //  Stall: [vr20]  ...............................      vr01.002      vr20    
       VRSUBW            vr20, vr20, 62                                                                       // temp temp [ 1780] 

              //  Stall: [vr20]  ...............................      vr20    
       VMULW             vr20, vr20, i7                                                                // temp temp lambda [ 1782] 


       VIM               i9, i9, 8*ME_REFERENCE_ARRAY_STRIDE                                              // RefPtr RefPtr [ 1785] 
       VIM               i15, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                      // CurPtr CirBuf [ 1786] 
       VMVW              vr01, vr20, 32                                                                       // res4 temp [ 1787] 

// this loop is the first set of 8 rows needed for the three
// zero vertical displacement SADs.  It is also the second set of 
// 8 rows needed for the -8 vertical displacement SADs
//FirstLoop --------                                                                                                    // [ 1792] 
 vendrec r3                                                                                                             // [ 1792] 
 vrec r3                                                                                                                // [ 1792] 
LINE 1793".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 1793] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 1794] 
       VLD128            vr02,[i15, 0]                                                                      // cur0 CurPtr [ 1795] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE]                                                     // cur1 CurPtr [ 1796] 
//vld128 cur2,[CurPtr,PCB_CURRENT_STRIDE*8]
//vld128 cur3,[CurPtr,PCB_CURRENT_STRIDE*9]
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 1799] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 1800] 

       VSADBW.8          vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 1802] 
              //  Stall: [vr19]  ...............................      vr19          vr21          vr23.008
       VSADBAW.8         vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 1803] 

       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 1805] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 1806] ....................// shift by 8 pels

       VSADBW.16         vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 1808] 
       VSADBAW.16        vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 1809] 

// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1811".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBW.32         vr15, vr02, vr18                                                            // Partres4 cur0 ref2 [ 1811] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1812".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBAW.32        vr15, vr03, vr22                                                            // Partres4 cur1 ref5 [ 1812] 

//vsadbw.2  PartRes, cur2, ref2
//vsadbaw.2 PartRes, cur3, ref5

       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2                                              // RefPtr RefPtr [ 1817] 


       VJD.i14           i14, r3                                                              // counter counter FirstLoop [ 1820] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 1821] 
       VADDW.32          vr01, vr01, vr15  //DS________________________________________________________ res4 res4 Partres4 [ 1822] 
       VADDW.255         vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 1823] 

       VMOVW             vr01, 4-1, 64                                                                          // counter [ 1825] ....................// 4 pairs of rows
// this loop is the second set of 8 rows needed for the three
// zero vertical displacement SADs. It is also the first set of 
// 8 rows needed for the +8 vertical displacement SADs
//SecondLoop --------                                                                                                   // [ 1829] 
 vendrec r4                                                                                                             // [ 1829] 
 vrec r4                                                                                                                // [ 1829] 
LINE 1830".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 1830] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 1831] 

       VLD128            vr02,[i15, 0]                                                                      // cur0 CurPtr [ 1833] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE]                                                     // cur1 CurPtr [ 1834] 

//vld128 cur2,[CurPtr,-PCB_CURRENT_STRIDE*8]
//vld128 cur3,[CurPtr,-PCB_CURRENT_STRIDE*9]
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 1838] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 1839] 

       VSADBW.8          vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 1841] 
              //  Stall: [vr19]  ...............................      vr19          vr21          vr23.008
       VSADBAW.8         vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 1842] 

       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 1844] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 1845] ....................// shift by 8 pels

       VSADBW.16         vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 1847] 
       VSADBAW.16        vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 1848] 
//vsadbw.64  PartRes, cur2, ref2
//vsadbaw.64 PartRes, cur3, ref5
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1851".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBW.32         vr15, vr02, vr18                                                            // Partres4 cur0 ref2 [ 1851] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1852".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBAW.32        vr15, vr03, vr22                                                            // Partres4 cur1 ref5 [ 1852] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2                                              // RefPtr RefPtr [ 1853] 



       VJD.i14           i14, r4                                                             // counter counter SecondLoop [ 1857] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 1858] 
       VADDW.32          vr01, vr01, vr15  //DS________________________________________________________ res4 res4 Partres4 [ 1859] 
       VADDW.255         vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 1860] 


        VMR5W.1         vr01, vr01, vr01                                                                  // BestCost res4 [ 1863] 

//vaddw.224 result, result, PartRes

  ld r1,[r17,4];                                                                                       // VSTACK endofIter [ 1867] 
LINE 1867".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJL               vr31, r1                                                                                       // [ 1867] 

       VNOP                //DS___________________________________________________________________________________________ [ 1869] 
       VNOP                //DS___________________________________________________________________________________________ [ 1870] 
       VNOP                //DS___________________________________________________________________________________________ [ 1871] 

       VJP!i2            r5                                                                                // VecX EndFunc [ 1873] ....................// continue as per usual
// ****** Warning : Unmarked delay slot found at line number 1874 *****
LINE 1874".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1874] 
// ****** Warning : Unmarked delay slot found at line number 1875 *****
LINE 1875".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1875] 
// ****** Warning : Unmarked delay slot found at line number 1876 *****
LINE 1876".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1876] 
// need to DMA in different reference data
// Begin                                                                                                                // [ 1878] 
//vec16  vr25  temp0                                                                                                    // [ 1879] 
//vec16  vr26  temp1                                                                                                    // [ 1879] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        CurPtr|       counter|          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |      Partres4|              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|VR16|                                                        ref0                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref2                                                           |
//|VR19|                                                        ref3                                                           |
//|VR20|                                                        temp                                                           |
//|VR21|                                                        ref4                                                           |
//|VR22|                                                        ref5                                                           |
//|VR23|                                                     PartRes                                                           |
//|VR24|                                                     bestvec                                                           |
//|VR25|                                                       temp0                                                           |
//|VR26|                                                       temp1                                                           |
//|............................................................................................................................|
//|VR29|savevals1_posY|savevals1_posX|              |              |              |              | savevals1_po.|              |savevals1.194   
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

LINE 1881".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr25, vr00, 255                                                                          // temp0 [ 1881] ....................// save current state
       VMVW              vr26, vr01, 255                                                                          // temp1 [ 1882] ....................// save current state

        vmvw.2          vr01, vr29                                                               // RefPtr savevals1_point [ 1884] 
        VMR6W.1         vr00, vr29, vr29                                                           // PredX savevals1_posX [ 1885] 
        VMR6W.2         vr00, vr29, vr29                                                           // PredY savevals1_posY [ 1886] 
 // add offset to PredX VecX/4
              //  Stall: [vr00]  ...............................      vr00.003
              //  Stall: [vr00]  ...............................      vr00.002
        VMR2W.1         vr01, vr00, vr00                                                                  // BestCost VecX [ 1888] 
#ifdef MPEG4BUILD
LINE 1890".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
              //  Stall: [vr01]  ...............................      vr01.001
              //  Stall: [vr01]  ...............................      vr01.001
       VASRW.3           vr01, vr01, 1                                                  // BestCost'RefPtr BestCost'RefPtr [ 1890] 
#else // MPEG4BUILD
              //  Stall: [vr01]  ...............................      vr01.003
       VASRW.3           vr01, vr01, 2                                                  // BestCost'RefPtr BestCost'RefPtr [ 1892] 
#endif // MPEG4BUILD
              //  Stall: [vr01]  ...............................      vr01.003
       VADDW.1           vr00, vr00, vr01                                                          // PredX PredX BestCost [ 1894] 
 
       VMOVW             vr00, 0, 128                                                                            // lambda [ 1896] ....................//_vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
       VMOVW             vr01, 0x2020, 1                                                                       // BestCost [ 1897] ....................//    _vr01 = _vmovw(0x2020, 0x01); // block size 32x32
       VMOVW             vr00, SDM_ME_REFERENCE_ARRAY, 64                                                       // CirLeft [ 1898] ....................//   _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x40); // SDM address where to store Y ref dat


// ****** Informational : Register space for symbol "sdmAddr" already allocated.                                        // [ 1901] 
// p32    k6  sdmAddr                                                                                                   // [ 1901] 
// ****** Informational : Register space for symbol "dr2Val" already allocated.                                         // [ 1902] 
// p32    k8  dr2Val                                                                                                    // [ 1902] 

// Begin                                                                                                                // [ 1904] 
// ****** Informational : Register space for symbol "dmaOutReg16" already allocated.                                    // [ 1905] 
// p16    i2  dmaOutReg16                                                                                               // [ 1905] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                                    // [ 1906] 
// p32    k2  dmaOutReg32                                                                                               // [ 1906] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 1910".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                                    // dmaOutReg32 [ 1910] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr1, k2                                                                            // dmaOutReg32 [ 1911] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                                // sdmAddr [ 1914] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                                 // dr2Val [ 1922] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                                          // dmaOutReg32 [ 1934] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr6, k2                                                                            // dmaOutReg32 [ 1935] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                            // dmaOutReg32 [ 1938] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr7, k2                                                                            // dmaOutReg32 [ 1939] 

    // start dma in
       VDMAIRUN          i0, i1                                                                             // PredX PredY [ 1942] 
    
    
// End                                                                                                                  // [ 1945] 


LINE 1948".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr00, vr25, 255                                                                          // temp0 [ 1948] ....................// restore current state
       VMVW              vr01, vr26, 255                                                                          // temp1 [ 1949] ....................// restore current state

// End                                                                                                                  // [ 1951] 
//EndFunc --------                                                                                                      // [ 1952] 
 vendrec r5                                                                                                             // [ 1952] 
 vrec r5                                                                                                                // [ 1952] 
// End                                                                                                                  // [ 1953] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz StepOne_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc StepOne                                                                                                      // [ 1954] 


// function to perform full step one functionality
//                      (-8,-8) (0,-8)  (8,-8)
// initially to look at (-8, 0) (0, 0)  (8, 0)
//                      (-8, 8) (0, 8)  (8, 8)
// [Macro] FullStepOne                                                                                                  // [ 1959] 
_Asm int Record_FullStepOne(int *mac_tab)
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
FullStepOne_RecLoop:
  ld r1,[r17 ,28]
  vrec r1


// Begin                                                                                                                // [ 1961] 

//vmovw  'VecX'VecY, 0  // set vecs to zero
//vec16  vr29  savevals1.194                                                                                            // [ 1964] 
//vec16  vr29  savevals1_point.2                                                                                        // [ 1964] 
//vec16  vr29  savevals1_posX.64                                                                                        // [ 1964] 
//vec16  vr29  savevals1_posY.128                                                                                       // [ 1964] 
// p16    i13  res4                                                                                                     // [ 1965] ....................// only needed in first step
// initialise the 9 sums to the rate distortion approximation function
// set each lane equal to this value
// this sets the vectors res0 to res8 with the RD cost for each one's MV
//vec16  vr20  temp                                                                                                     // [ 1969] ....................// to make sure it doesn't conflict with anything in initRDvals
//vec16  vr15  tempres.3                                                                                                // [ 1970] 
//vec16  vr15  tempres_Ptrx.1                                                                                           // [ 1970] 
//vec16  vr15  tempres_Ptry.2                                                                                           // [ 1970] 

//vec16  vr16  ref0                                                                                                     // [ 1972] 
//vec16  vr17  ref1                                                                                                     // [ 1972] 
//vec16  vr18  ref2                                                                                                     // [ 1972] 
//vec16  vr19  ref3                                                                                                     // [ 1972] 
//vec16  vr21  ref4                                                                                                     // [ 1972] 
//vec16  vr22  ref5                                                                                                     // [ 1972] 
//vec16  vr23  PartRes                                                                                                  // [ 1973] 
//vec16  vr24  bestvec                                                                                                  // [ 1973] 
// s16    vr15.32  Partres4                                                                                             // [ 1974] ....................// this will hold the (0,0) displacement SAD cost

// p16    i14  counter                                                                                                  // [ 1976] 
// p16    i15  CurPtr                                                                                                   // [ 1977] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        CurPtr|       counter|          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |      Partres4|              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|VR16|                                                        ref0                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref2                                                           |
//|VR19|                                                        ref3                                                           |
//|VR20|                                                        temp                                                           |
//|VR21|                                                        ref4                                                           |
//|VR22|                                                        ref5                                                           |
//|VR23|                                                     PartRes                                                           |
//|VR24|                                                     bestvec                                                           |
//|............................................................................................................................|
//|VR29|savevals1_posY|savevals1_posX|              |              |              |              | savevals1_po.|              |savevals1.194   
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

// read quantiser
LINE 1980".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr13, vr00, 255                                                                          // cur11 [ 1980] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1981".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr29, vr01, 194                                                                      // savevals1 [ 1981] 

  ld r1,[r17,8];                                                                                      // VSTACK initRDvals [ 1983] 
       VJL               vr31, r1                                                                                       // [ 1983] 
// now do res4 which couldn't fit in the vector
// it is (0,0)
// ****** Warning : Unmarked delay slot found at line number 1986 *****
LINE 1986".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1986] 
// ****** Warning : Unmarked delay slot found at line number 1987 *****
LINE 1987".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1987] 
// ****** Warning : Unmarked delay slot found at line number 1988 *****
LINE 1988".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 1988] 
       VDMAWAIT          0x0, 0x7f                                                                                      // [ 1989] ....................// make sure DMA of ref has completed
// ****** Warning : Misplaced delay slot marker found at line number 1990 *****
LINE 1990".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VABSW.3           vr15, vr00                                               // tempres_Ptrx'tempres_Ptry PredX'PredY [ 1990] 

              //  Stall: [vr15]  ...............................      vr15.003
       VNORMW.3          vr15, vr15                                 // tempres_Ptrx'tempres_Ptry tempres_Ptrx'tempres_Ptry [ 1992] 
              //  Stall: [vr15]  ...............................      vr15.003
       VXSUMW.255        vr20, vr15, 3                                                   // temp tempres_Ptrx'tempres_Ptry [ 1993] ....................// add the two and put in all lanesv

//vmovw result, 0xFFFF // initialise all to very high

       VMOVW             vr01, 4-1, 64                                                                          // counter [ 1997] ....................// 4 pairs of rows

              //  Stall: [vr20]  ...............................      vr01.064      vr20.003
       VMULW             vr20, vr20, 2                                                                        // temp temp [ 1999] 

       VMOVW             vr01, SDM_ME_REFERENCE_ARRAY, 2                                                         // RefPtr [ 2001] 
              //  Stall: [vr20]  ...............................      vr01.002      vr20    
       VRSUBW            vr20, vr20, 62                                                                       // temp temp [ 2002] 

              //  Stall: [vr20]  ...............................      vr20    
       VMULW             vr20, vr20, i7                                                                // temp temp lambda [ 2004] 


//vim RefPtr, RefPtr, 8*ME_REFERENCE_ARRAY_STRIDE
       VIM               i15, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                      // CurPtr CirBuf [ 2008] 
              //  Stall: [vr20]  ...............................      vr20          I15  
       VMVW              vr01, vr20, 32                                                                       // res4 temp [ 2009] 

//LoopZero --------                                                                                                     // [ 2011] 
 vendrec r3                                                                                                             // [ 2011] 
 vrec r3                                                                                                                // [ 2011] 
LINE 2012".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 2012] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 2013] 
       VLD128            vr02,[i15, 0]                                                                      // cur0 CurPtr [ 2014] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE]                                                     // cur1 CurPtr [ 2015] 


       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 2018] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 2019] 

       VSADBW.1          vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 2021] 
       VSADBW.4          vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 2022] 

       VSADBAW.1         vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 2024] 
       VSADBAW.4         vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 2025] 

       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 2027] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 2028] ....................// shift by 8 pels
              //  Stall: [vr18]  ...............................      vr18          vr22          vr23.004
       VSADBW.2          vr23, vr02, vr18                                                             // PartRes cur0 ref2 [ 2029] 
       VSADBAW.2         vr23, vr03, vr22                                                             // PartRes cur1 ref5 [ 2030] 


       VJD.i14           i14, r3                                                               // counter counter LoopZero [ 2033] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ RefPtr RefPtr [ 2034] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 2035] 
       VADDW.7           vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 2036] 

// this loop is the first set of 8 rows needed for the three
// zero vertical displacement SADs.  It is also the second set of 
// 8 rows needed for the -8 vertical displacement SADs

       VMOVW             vr01, 4-1, 64                                                                          // counter [ 2042] ....................// 4 pairs of rows
//LoopOne --------                                                                                                      // [ 2043] 
 vendrec r4                                                                                                             // [ 2043] 
 vrec r4                                                                                                                // [ 2043] 
LINE 2044".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 2044] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 2045] 
       VLD128            vr02,[i15, -PCB_CURRENT_STRIDE*8]                                                  // cur0 CurPtr [ 2046] 
       VLD128            vr03,[i15, -PCB_CURRENT_STRIDE*7]                                                  // cur1 CurPtr [ 2047] 
       VLD128            vr04,[i15, PCB_CURRENT_STRIDE*0]                                                   // cur2 CurPtr [ 2048] 
       VLD128            vr05,[i15, PCB_CURRENT_STRIDE*1]                                                   // cur3 CurPtr [ 2049] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 2050] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 2051] 

       VSADBW.8          vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 2053] 
              //  Stall: [vr19]  ...............................      vr19          vr21          vr23.008
       VSADBAW.8         vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 2054] 
       VSADBW.1          vr23, vr04, vr16                                                             // PartRes cur2 ref0 [ 2055] 
       VSADBAW.1         vr23, vr05, vr19                                                             // PartRes cur3 ref3 [ 2056] 

       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 2058] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 2059] ....................// shift by 8 pels

       VSADBW.16         vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 2061] 
       VSADBAW.16        vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 2062] 
       VSADBW.4          vr23, vr04, vr17                                                             // PartRes cur2 ref1 [ 2063] 
       VSADBAW.4         vr23, vr05, vr21                                                             // PartRes cur3 ref4 [ 2064] 

// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 2066".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBW.32         vr15, vr02, vr18                                                            // Partres4 cur0 ref2 [ 2066] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 2067".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBAW.32        vr15, vr03, vr22                                                            // Partres4 cur1 ref5 [ 2067] 

       VSADBW.2          vr23, vr04, vr18                                                             // PartRes cur2 ref2 [ 2069] 
       VSADBAW.2         vr23, vr05, vr22                                                             // PartRes cur3 ref5 [ 2070] 

       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2                                              // RefPtr RefPtr [ 2072] 


       VJD.i14           i14, r4                                                                // counter counter LoopOne [ 2075] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 2076] 
       VADDW.32          vr01, vr01, vr15  //DS________________________________________________________ res4 res4 Partres4 [ 2077] 
       VADDW.31          vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 2078] 

//reset current pointer
       VIM               i15, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_Y                                      // CurPtr CirBuf [ 2081] 

       VMOVW             vr01, 4-1, 64                                                                          // counter [ 2083] ....................// 4 pairs of rows
// this loop is the second set of 8 rows needed for the three
// zero vertical displacement SADs. It is also the first set of 
// 8 rows needed for the +8 vertical displacement SADs
//LoopTwo --------                                                                                                      // [ 2087] 
 vendrec r5                                                                                                             // [ 2087] 
 vrec r5                                                                                                                // [ 2087] 
LINE 2088".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 2088] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 2089] 

       VLD128            vr04,[i15, PCB_CURRENT_STRIDE*0]                                                   // cur2 CurPtr [ 2091] 
       VLD128            vr05,[i15, PCB_CURRENT_STRIDE*1]                                                   // cur3 CurPtr [ 2092] 
       VLD128            vr02,[i15, PCB_CURRENT_STRIDE*8]                                                   // cur0 CurPtr [ 2093] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE*9]                                                   // cur1 CurPtr [ 2094] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 2095] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 2096] 

       VSADBW.8          vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 2098] 
              //  Stall: [vr19]  ...............................      vr19          vr21          vr23.008
       VSADBAW.8         vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 2099] 
       VSADBW.32         vr23, vr04, vr16                                                             // PartRes cur2 ref0 [ 2100] 
       VSADBAW.32        vr23, vr05, vr19                                                             // PartRes cur3 ref3 [ 2101] 
       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 2102] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 2103] ....................// shift by 8 pels

       VSADBW.16         vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 2105] 
       VSADBAW.16        vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 2106] 
       VSADBW.128        vr23, vr04, vr17                                                             // PartRes cur2 ref1 [ 2107] 
       VSADBAW.128       vr23, vr05, vr21                                                             // PartRes cur3 ref4 [ 2108] 

       VSADBW.64         vr23, vr04, vr18                                                             // PartRes cur2 ref2 [ 2110] 
       VSADBAW.64        vr23, vr05, vr22                                                             // PartRes cur3 ref5 [ 2111] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 2112".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBW.32         vr15, vr02, vr18                                                            // Partres4 cur0 ref2 [ 2112] 
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 2113".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VSADBAW.32        vr15, vr03, vr22                                                            // Partres4 cur1 ref5 [ 2113] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2                                              // RefPtr RefPtr [ 2114] 

       VJD.i14           i14, r5                                                                // counter counter LoopTwo [ 2116] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 2117] 
       VADDW.32          vr01, vr01, vr15  //DS________________________________________________________ res4 res4 Partres4 [ 2118] 
       VADDW.248         vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 2119] 


       VMOVW             vr01, 4-1, 64                                                                          // counter [ 2122] ....................// 4 pairs of rows
// current pointer is now at 8th row
//LoopThree --------                                                                                                    // [ 2124] 
 vendrec r6                                                                                                             // [ 2124] 
 vrec r6                                                                                                                // [ 2124] 
LINE 2125".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VLD128            vr16,[i9, 0]                                                                       // ref0 RefPtr [ 2125] 
       VLD128            vr17,[i9, 16]                                                                      // ref1 RefPtr [ 2126] 

       VLD128            vr02,[i15, PCB_CURRENT_STRIDE*0]                                                   // cur0 CurPtr [ 2128] 
       VLD128            vr03,[i15, PCB_CURRENT_STRIDE*1]                                                   // cur1 CurPtr [ 2129] 
       VLD128            vr19,[i9, ME_REFERENCE_ARRAY_STRIDE]                                               // ref3 RefPtr [ 2130] 
       VLD128            vr21,[i9, ME_REFERENCE_ARRAY_STRIDE+16]                                            // ref4 RefPtr [ 2131] 

 
       VSADBW.32         vr23, vr02, vr16                                                             // PartRes cur0 ref0 [ 2134] 
              //  Stall: [vr19]  ...............................      vr19          vr21          vr23.032
       VSADBAW.32        vr23, vr03, vr19                                                             // PartRes cur1 ref3 [ 2135] 
       VMR4W             vr18, vr17, vr16                                                                // ref2 ref1 ref0 [ 2136] ....................// shift by 8 pels
       VMR4W             vr22, vr21, vr19                                                                // ref5 ref4 ref3 [ 2137] ....................// shift by 8 pels

       VSADBW.128        vr23, vr02, vr17                                                             // PartRes cur0 ref1 [ 2139] 
       VSADBAW.128       vr23, vr03, vr21                                                             // PartRes cur1 ref4 [ 2140] 

       VSADBW.64         vr23, vr02, vr18                                                             // PartRes cur0 ref2 [ 2142] 
       VSADBAW.64        vr23, vr03, vr22                                                             // PartRes cur1 ref5 [ 2143] 

       VJD.i14           i14, r6                                                              // counter counter LoopThree [ 2145] 
       VIM               i9, i9, ME_REFERENCE_ARRAY_STRIDE*2  //DS__________________________________________ RefPtr RefPtr [ 2146] 
       VIM               i15, i15, PCB_CURRENT_STRIDE*2  //DS_______________________________________________ CurPtr CurPtr [ 2147] 
       VADDW.224         vr14, vr14, vr23  //DS_____________________________________________________ result result PartRes [ 2148] 


        VMR5W.1         vr01, vr01, vr01                                                                  // BestCost res4 [ 2151] 

//vaddw.224 result, result, PartRes

  ld r1,[r17,4];                                                                                       // VSTACK endofIter [ 2155] 
LINE 2155".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VJL               vr31, r1                                                                                       // [ 2155] 

       VNOP                //DS___________________________________________________________________________________________ [ 2157] 
       VNOP                //DS___________________________________________________________________________________________ [ 2158] 
       VNOP                //DS___________________________________________________________________________________________ [ 2159] 
        VMR1W.4         vr01, vr00, vr00                                                                     // lastX VecY [ 2160] 
LINE 2161".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
              //  Stall: [vr01]  ...............................      vr01.004
              //  Stall: [vr01]  ...............................      vr01.004
       VOR.4             vr01, vr01, vr00                                                              // lastX lastX VecX [ 2161] 

              //  Stall: [i10]  ................................      vr01.004
       VJP!i10           r7                                                                           // lastX EndFullFunc [ 2163] ....................// continue as per usual as zero was best anyway
// ****** Warning : Unmarked delay slot found at line number 2164 *****
LINE 2164".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 2164] 
// ****** Warning : Unmarked delay slot found at line number 2165 *****
LINE 2165".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 2165] 
// ****** Warning : Unmarked delay slot found at line number 2166 *****
LINE 2166".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VNOP                //DS___________________________________________________________________________________________ [ 2166] 
// need to DMA in different reference data
// Begin                                                                                                                // [ 2168] 
//vec16  vr25  temp0                                                                                                    // [ 2169] 
//vec16  vr26  temp1                                                                                                    // [ 2169] 
// REGMAP 
//+============================================================================================================================+
//|REG |      7       |      6       |      5       |      4       |      3       |      2       |      1       |      0       |
//+============================================================================================================================+
//|VR 0|        lambda|       CirLeft|        FixBuf|        CirBuf|          VecY|          VecX|         PredY|         PredX|
//|VR 1|        CurPtr|       counter|          res4|      StepMult|         lastY|         lastX|        RefPtr|      BestCost|
//|VR 2|                                                        cur0                                                           |
//|VR 3|                                                        cur1                                                           |
//|VR 4|                                                        cur2                                                           |
//|VR 5|                                                        cur3                                                           |
//|VR 6|                                                        cur4                                                           |
//|VR 7|                                                        cur5                                                           |
//|VR 8|                                                        cur6                                                           |
//|VR 9|                                                        cur7                                                           |
//|VR10|                                                        cur8                                                           |
//|VR11|                                                        cur9                                                           |
//|VR12|                                                       cur10                                                           |
//|VR13|                                                       cur11                                                           |
//|VR14|                                                      result                                                           |
//|VR15|              |              |      Partres4|              |              |              |  tempres_Ptry|  tempres_Ptrx|tempres.3   
//|VR16|                                                        ref0                                                           |
//|VR17|                                                        ref1                                                           |
//|VR18|                                                        ref2                                                           |
//|VR19|                                                        ref3                                                           |
//|VR20|                                                        temp                                                           |
//|VR21|                                                        ref4                                                           |
//|VR22|                                                        ref5                                                           |
//|VR23|                                                     PartRes                                                           |
//|VR24|                                                     bestvec                                                           |
//|VR25|                                                       temp0                                                           |
//|VR26|                                                       temp1                                                           |
//|............................................................................................................................|
//|VR29|savevals1_posY|savevals1_posX|              |              |              |              | savevals1_po.|              |savevals1.194   
//|............................................................................................................................|
//|VR31|                                                      VSTACK                                                           |
//|............................................................................................................................|
//|VR62|                                                       VZERO                                                           |
//|VR63|                                                      VFLAGS                                                           |
//+----------------------------------------------------------------------------------------------------------------------------+

LINE 2171".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr25, vr00, 255                                                                          // temp0 [ 2171] ....................// save current state
       VMVW              vr26, vr01, 255                                                                          // temp1 [ 2172] ....................// save current state


        VMR6W.1         vr00, vr29, vr29                                                           // PredX savevals1_posX [ 2175] 
        VMR6W.2         vr00, vr29, vr29                                                           // PredY savevals1_posY [ 2176] 
 // add offset to PredX VecX/4
              //  Stall: [vr00]  ...............................      vr00.003
              //  Stall: [vr00]  ...............................      vr00.002
        VMR2W.1         vr01, vr00, vr00                                                                  // BestCost VecX [ 2178] 
        VMR2W.2         vr01, vr00, vr00                                                                    // RefPtr VecY [ 2179] 
#ifdef MPEG4BUILD
LINE 2181".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
              //  Stall: [vr01]  ...............................      vr01.003
              //  Stall: [vr01]  ...............................      vr01.002
       VASRW.3           vr01, vr01, 1                                                  // BestCost'RefPtr BestCost'RefPtr [ 2181] 
#else // MPEG4BUILD
              //  Stall: [vr01]  ...............................      vr01.003
       VASRW.3           vr01, vr01, 2                                                  // BestCost'RefPtr BestCost'RefPtr [ 2183] 
#endif // MPEG4BUILD
              //  Stall: [vr01]  ...............................      vr01.003
       VADDW.3           vr00, vr00, vr01                                       // PredX'PredY PredX'PredY BestCost'RefPtr [ 2185] 
        vmvw.2          vr01, vr29                                                               // RefPtr savevals1_point [ 2186] 
LINE 2187".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOVW             vr00, 0, 128                                                                            // lambda [ 2187] ....................//_vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
       VMOVW             vr01, 0x2020, 1                                                                       // BestCost [ 2188] ....................//    _vr01 = _vmovw(0x2020, 0x01); // block size 32x32
       VMOVW             vr00, SDM_ME_REFERENCE_ARRAY, 64                                                       // CirLeft [ 2189] ....................//   _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x40); // SDM address where to store Y ref dat


// ****** Informational : Register space for symbol "sdmAddr" already allocated.                                        // [ 2192] 
// p32    k6  sdmAddr                                                                                                   // [ 2192] 
// ****** Informational : Register space for symbol "dr2Val" already allocated.                                         // [ 2193] 
// p32    k8  dr2Val                                                                                                    // [ 2193] 

// Begin                                                                                                                // [ 2195] 
// ****** Informational : Register space for symbol "dmaOutReg16" already allocated.                                    // [ 2196] 
// p16    i2  dmaOutReg16                                                                                               // [ 2196] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                                    // [ 2197] 
// p32    k2  dmaOutReg32                                                                                               // [ 2197] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 2201".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                                    // dmaOutReg32 [ 2201] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr1, k2                                                                            // dmaOutReg32 [ 2202] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                                // sdmAddr [ 2205] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                                 // dr2Val [ 2213] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                                          // dmaOutReg32 [ 2225] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr6, k2                                                                            // dmaOutReg32 [ 2226] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                            // dmaOutReg32 [ 2229] 
              //  Stall: [k2]  .................................      vr00.012
       VDMAISET          dr7, k2                                                                            // dmaOutReg32 [ 2230] 

    // start dma in
       VDMAIRUN          i0, i1                                                                             // PredX PredY [ 2233] 
    
    
// End                                                                                                                  // [ 2236] 

LINE 2238".\ARCEncoder\ARC\ArcSoftwareME_MPA.sss"
       VMVW              vr01, vr00, 4                                                                       // lastX VecX [ 2238] ....................// restore lastX

       VMVW              vr00, vr25, 255                                                                          // temp0 [ 2240] ....................// restore current state
       VMVW              vr01, vr26, 255                                                                          // temp1 [ 2241] ....................// restore current state

// End                                                                                                                  // [ 2243] 
//EndFullFunc --------                                                                                                  // [ 2244] 
 vendrec r7                                                                                                             // [ 2244] 
 vrec r7                                                                                                                // [ 2244] 

// End                                                                                                                  // [ 2246] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FullStepOne_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FullStepOne                                                                                                  // [ 2247] 




const int MACRO_IDX_Generate_Soft_ME_Response = 0;
const int SIMD_IDX_endofIter = 1;
const int SIMD_IDX_initRDvals = 2;
const int MACRO_IDX_IntegerME = 3;
const int MACRO_IDX_SaveVectors = 4;
const int MACRO_IDX_ZeroDisplacement = 5;
const int MACRO_IDX_StepOne = 6;
const int MACRO_IDX_FullStepOne = 7;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Software_ME_Record(int sa, int SIMDnum)
{
    int *map = Software_ME;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_Generate_Soft_ME_Response] = sa;
    sa = Record_Generate_Soft_ME_Response(map);

    map[SIMD_IDX_endofIter] = sa;
    sa = Record_endofIter(map);

    map[SIMD_IDX_initRDvals] = sa;
    sa = Record_initRDvals(map);

    map[MACRO_IDX_IntegerME] = sa;
    sa = Record_IntegerME(map);

    map[MACRO_IDX_SaveVectors] = sa;
    sa = Record_SaveVectors(map);

    map[MACRO_IDX_ZeroDisplacement] = sa;
    sa = Record_ZeroDisplacement(map);

    map[MACRO_IDX_StepOne] = sa;
    sa = Record_StepOne(map);

    map[MACRO_IDX_FullStepOne] = sa;
    sa = Record_FullStepOne(map);

    return sa;
}
