//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss'

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




// Stall info ON                                                      // [   20] 
// Stall info format: Stall [Reg in Stall] ...... List of Registers currently busy  

// MetaWare ouput ON                                                                                        // [   22] 
#define LINE #line                                                                                          // [   22] 

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

#include ".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.h"


// Macro table
    int EncoderAddSubtract[3];
    int EncoderAddSubtract_Record(int sa, int SIMDnum);



#include "ArcMPC.h"
#include "ArcFrameBuffers.h"


// Reconstructed buffer
//                PCB_RECONSTRUCTED_STRIDE
//                  /
//      |----------+-----------|
//              1111111111222222
//      012345678901234567890123
// 0    *xxxxxxxAAAAAAAAAAAAAAAA        *   PCB_RECONSTRUCTED_Y
// 1    xxxxxxxxAAAAAAAAAAAAAAAA
// 2    xxxxxxxxAAAAAAAAAAAAAAAA
// 3    xxxxxxxxAAAAAAAAAAAAAAAA
// 4    xxxxLLLL0000000011111111
// 5    xxxxLLLL0000000011111111
// 6    xxxxLLLL0000000011111111
// 7    xxxxLLLL0000000011111111
// 8    xxxxLLLL0000000011111111
// 9    xxxxLLLL0000000011111111
//10    xxxxLLLL0000000011111111
//11    xxxxLLLL0000000011111111
//12    xxxxLLLL2222222233333333
//13    xxxxLLLL2222222233333333
//14    xxxxLLLL2222222233333333
//15    xxxxLLLL2222222233333333
//16    xxxxLLLL2222222233333333
//17    xxxxLLLL2222222233333333
//18    xxxxLLLL2222222233333333
//19    xxxxLLLL2222222233333333
//20    #xxxxxxxxxxx@xxxxxxxxxxx        #   PCB_RECONSTRUCTED_U     @ PCB_RECONSTRUCTED_V
//21    xxxxxxxxxxxxxxxxxxxxxxxx
//22    xxxxAAAAAAAAxxxxAAAAAAAA
//23    xxxxAAAAAAAAxxxxAAAAAAAA
//24    xxLL44444444xxLL55555555
//25    xxLL44444444xxLL55555555
//26    xxLL44444444xxLL55555555
//27    xxLL44444444xxLL55555555
//28    xxLL44444444xxLL55555555
//29    xxLL44444444xxLL55555555
//30    xxLL44444444xxLL55555555
//31    xxLL44444444xxLL55555555


//------------------------------------------------------------------------------
// Name:          ReconstructLuma16X16
// Purpose:       Computes the reconstructed luma block by adding the prediction
//                data to the prediction block, clipping to [0:255] then storing 
//                the result in the reconstructed data buffer in SDM
// Arguments:     ea     = residual block address in SDM
//                pa     = prediction address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructLuma16X16                                                                             // [   84] 
_Asm int Record_ReconstructLuma16X16(int *mac_tab)
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

// p16    i0  ea                                                                                            // [   85] 
// p16    i1  pa                                                                                            // [   86] 
// p16    i2  ra                                                                                            // [   87] 
// Begin                                                                                                    // [   88] 
// p16    i3  lpCnt                                                                                         // [   89] ....................// loop count

//vec16  vr02  errLane0                                                                                     // [   91] 
//vec16  vr03  errLane1                                                                                     // [   92] 
//vec16  vr04  errLane2                                                                                     // [   93] 
//vec16  vr05  errLane3                                                                                     // [   94] 

//vec16  vr06  predLane0                                                                                    // [   96] 
//vec16  vr07  predLane1                                                                                    // [   97] 
//vec16  vr08  predLane2                                                                                    // [   98] 
//vec16  vr09  predLane3                                                                                    // [   99] 
    
    // Initialise the loop count
    // The luma MB (16x16) is divided into 8 slices of 2 rows each.
    // One slice is processed per iteration
LINE 104".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 7, 8                                                                   // lpCnt [  104] 
    
//LpStart --------                                                                                          // [  106] 
 vendrec r3                                                                                                 // [  106] 
 vrec r3                                                                                                    // [  106] 
LINE 107".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*16]                                                        // errLane0 ea [  107] 
       VLD128            vr03,[i0, 1*16]                                                        // errLane1 ea [  108] 
       VLD128            vr04,[i0, 2*16]                                                        // errLane2 ea [  109] 
       VLD128            vr05,[i0, 3*16]                                                        // errLane3 ea [  110] 

       VLD64W            vr06,[i1, 0*8]                                                        // predLane0 pa [  112] 
       VLD64W            vr07,[i1, 1*8]                                                        // predLane1 pa [  113] 
       VLD64W            vr08,[i1, 2*8]                                                        // predLane2 pa [  114] 
       VLD64W            vr09,[i1, 3*8]                                                        // predLane3 pa [  115] 
    
       VADDW             vr02, vr02, vr06                                       // errLane0 errLane0 predLane0 [  117] 
       VADDW             vr03, vr03, vr07                                       // errLane1 errLane1 predLane1 [  118] 
       VADDW             vr04, vr04, vr08                                       // errLane2 errLane2 predLane2 [  119] 
       VADDW             vr05, vr05, vr09                                       // errLane3 errLane3 predLane3 [  120] 
    
       VASRPWB           vr02, vr02, 0                                                    // errLane0 errLane0 [  122] 
       VASRPWB           vr03, vr03, 0                                                    // errLane1 errLane1 [  123] 
       VASRPWB           vr04, vr04, 0                                                    // errLane2 errLane2 [  124] 
       VASRPWB           vr05, vr05, 0                                                    // errLane3 errLane3 [  125] 
    
       VST64             vr02,[i2, 1*8]                                                         // errLane0 ra [  127] 
       VST64             vr03,[i2, 2*8]                                                         // errLane1 ra [  128] 
       VST64             vr04,[i2, 4*8]                                                         // errLane2 ra [  129] 
       VST64             vr05,[i2, 5*8]                                                         // errLane3 ra [  130] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [  135] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ ea ea [  136] 
       VIM               i1, i1, 32  //DS_______________________________________________________________ pa pa [  137] 
       VIM               i2, i2, 48  //DS_______________________________________________________________ ra ra [  138] 
    
    
// End                                                                                                      // [  141] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructLuma16X16                                                                             // [  143] 



//------------------------------------------------------------------------------
// Name:          ReconstructChroma
// Purpose:       Computes the reconstructed chroma block by adding the prediction
//                data to the prediction block, clipping to [0:255] then storing 
//                the result in the reconstructed data buffer in SDM
// Arguments:     ea     = residual block address in SDM
//                pa     = prediction address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructChroma                                                                                // [  155] 
_Asm int Record_ReconstructChroma(int *mac_tab)
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
ReconstructChroma_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

// p16    i0  ea                                                                                            // [  156] 
// p16    i1  pa                                                                                            // [  157] 
// p16    i2  ra                                                                                            // [  158] 
// Begin                                                                                                    // [  159] 
// p16    i3  lpCnt                                                                                         // [  160] ....................// loop count

//vec16  vr02  UerrLane0                                                                                    // [  162] 
//vec16  vr03  UerrLane1                                                                                    // [  163] 
//vec16  vr04  UerrLane2                                                                                    // [  164] 
//vec16  vr05  UerrLane3                                                                                    // [  165] 

//vec16  vr06  UpredLane0                                                                                   // [  167] 
//vec16  vr07  UpredLane1                                                                                   // [  168] 
//vec16  vr08  UpredLane2                                                                                   // [  169] 
//vec16  vr09  UpredLane3                                                                                   // [  170] 

//vec16  vr10  VerrLane0                                                                                    // [  172] 
//vec16  vr11  VerrLane1                                                                                    // [  173] 
//vec16  vr12  VerrLane2                                                                                    // [  174] 
//vec16  vr13  VerrLane3                                                                                    // [  175] 

//vec16  vr14  VpredLane0                                                                                   // [  177] 
//vec16  vr15  VpredLane1                                                                                   // [  178] 
//vec16  vr16  VpredLane2                                                                                   // [  179] 
//vec16  vr17  VpredLane3                                                                                   // [  180] 
    
    // Initialise the loop count
    // The chroma block (8x8) is divided into 2 slices of 4 rows each.
    // One slice is processed per iteration
LINE 185".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 1, 8                                                                   // lpCnt [  185] 
    
//LpStart --------                                                                                          // [  187] 
 vendrec r3                                                                                                 // [  187] 
 vrec r3                                                                                                    // [  187] 
LINE 188".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*32]                                                       // UerrLane0 ea [  188] 
       VLD128            vr03,[i0, 1*32]                                                       // UerrLane1 ea [  189] 
       VLD128            vr04,[i0, 2*32]                                                       // UerrLane2 ea [  190] 
       VLD128            vr05,[i0, 3*32]                                                       // UerrLane3 ea [  191] 
       VLD128            vr10,[i0, 0*32+16]                                                    // VerrLane0 ea [  192] 
       VLD128            vr11,[i0, 1*32+16]                                                    // VerrLane1 ea [  193] 
       VLD128            vr12,[i0, 2*32+16]                                                    // VerrLane2 ea [  194] 
       VLD128            vr13,[i0, 3*32+16]                                                    // VerrLane3 ea [  195] 

       VLD64W            vr06,[i1, 0*16]                                                      // UpredLane0 pa [  197] 
       VLD64W            vr07,[i1, 1*16]                                                      // UpredLane1 pa [  198] 
       VLD64W            vr08,[i1, 2*16]                                                      // UpredLane2 pa [  199] 
       VLD64W            vr09,[i1, 3*16]                                                      // UpredLane3 pa [  200] 
       VLD64W            vr14,[i1, 0*16+8]                                                    // VpredLane0 pa [  201] 
       VLD64W            vr15,[i1, 1*16+8]                                                    // VpredLane1 pa [  202] 
       VLD64W            vr16,[i1, 2*16+8]                                                    // VpredLane2 pa [  203] 
       VLD64W            vr17,[i1, 3*16+8]                                                    // VpredLane3 pa [  204] 
    
       VADDW             vr02, vr02, vr06                                    // UerrLane0 UerrLane0 UpredLane0 [  206] 
       VADDW             vr03, vr03, vr07                                    // UerrLane1 UerrLane1 UpredLane1 [  207] 
       VADDW             vr04, vr04, vr08                                    // UerrLane2 UerrLane2 UpredLane2 [  208] 
       VADDW             vr05, vr05, vr09                                    // UerrLane3 UerrLane3 UpredLane3 [  209] 
       VADDW             vr10, vr10, vr14                                    // VerrLane0 VerrLane0 VpredLane0 [  210] 
       VADDW             vr11, vr11, vr15                                    // VerrLane1 VerrLane1 VpredLane1 [  211] 
       VADDW             vr12, vr12, vr16                                    // VerrLane2 VerrLane2 VpredLane2 [  212] 
       VADDW             vr13, vr13, vr17                                    // VerrLane3 VerrLane3 VpredLane3 [  213] 
    
       VASRPWB           vr02, vr02, 0                                                  // UerrLane0 UerrLane0 [  215] 
       VASRPWB           vr03, vr03, 0                                                  // UerrLane1 UerrLane1 [  216] 
       VASRPWB           vr04, vr04, 0                                                  // UerrLane2 UerrLane2 [  217] 
       VASRPWB           vr05, vr05, 0                                                  // UerrLane3 UerrLane3 [  218] 
       VASRPWB           vr10, vr10, 0                                                  // VerrLane0 VerrLane0 [  219] 
       VASRPWB           vr11, vr11, 0                                                  // VerrLane1 VerrLane1 [  220] 
       VASRPWB           vr12, vr12, 0                                                  // VerrLane2 VerrLane2 [  221] 
       VASRPWB           vr13, vr13, 0                                                  // VerrLane3 VerrLane3 [  222] 
    
       VST32_0           vr02,[i2, 0*24+4]                                                     // UerrLane0 ra [  224] 
       VST32_2           vr02,[i2, 0*24+8]                                                     // UerrLane0 ra [  225] 
       VST32_0           vr03,[i2, 1*24+4]                                                     // UerrLane1 ra [  226] 
       VST32_2           vr03,[i2, 1*24+8]                                                     // UerrLane1 ra [  227] 
       VST32_0           vr04,[i2, 2*24+4]                                                     // UerrLane2 ra [  228] 
       VST32_2           vr04,[i2, 2*24+8]                                                     // UerrLane2 ra [  229] 
       VST32_0           vr05,[i2, 3*24+4]                                                     // UerrLane3 ra [  230] 
       VST32_2           vr05,[i2, 3*24+8]                                                     // UerrLane3 ra [  231] 
       VST32_0           vr10,[i2, 0*24+16]                                                    // VerrLane0 ra [  232] 
       VST32_2           vr10,[i2, 0*24+20]                                                    // VerrLane0 ra [  233] 
       VST32_0           vr11,[i2, 1*24+16]                                                    // VerrLane1 ra [  234] 
       VST32_2           vr11,[i2, 1*24+20]                                                    // VerrLane1 ra [  235] 
       VST32_0           vr12,[i2, 2*24+16]                                                    // VerrLane2 ra [  236] 
       VST32_2           vr12,[i2, 2*24+20]                                                    // VerrLane2 ra [  237] 
       VST32_0           vr13,[i2, 3*24+16]                                                    // VerrLane3 ra [  238] 
       VST32_2           vr13,[i2, 3*24+20]                                                    // VerrLane3 ra [  239] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i3            i3, r3                                                         // lpCnt lpCnt LpStart [  244] 
       VIM               i0, i0, 128  //DS______________________________________________________________ ea ea [  245] 
       VIM               i1, i1, 64  //DS_______________________________________________________________ pa pa [  246] 
       VIM               i2, i2, 96  //DS_______________________________________________________________ ra ra [  247] 
    
// End                                                                                                      // [  249] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructChroma_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructChroma                                                                                // [  251] 



//------------------------------------------------------------------------------
// Name:          ReconstructSkipped
// Purpose:       Copies the skipped prediction into the 
//                reconstructed data buffer in SDM
// Arguments:     pa     = prediction block address in SDM
//                ra     = reconstructed block address in SDM
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] ReconstructSkipped                                                                               // [  261] 
_Asm int Record_ReconstructSkipped(int *mac_tab)
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
ReconstructSkipped_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

// p16    i0  pa                                                                                            // [  262] 
// p16    i1  ra                                                                                            // [  263] 
// Begin                                                                                                    // [  264] 
// p16    i2  lpCnt                                                                                         // [  265] ....................// loop count

//vec16  vr02  predLane0                                                                                    // [  267] 
//vec16  vr03  predLane1                                                                                    // [  268] 
//vec16  vr04  predLane2                                                                                    // [  269] 
//vec16  vr05  predLane3                                                                                    // [  270] 

    //----------------------
    // Luma
    //----------------------
    // Initialise the loop count
    // The luma MB is divided into 4 slices of 4 rows each.
    // One slice is processed per iteration
LINE 278".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VMOVW             vr00, 3, 4                                                                   // lpCnt [  278] 
    
//YLpStart --------                                                                                         // [  280] 
 vendrec r3                                                                                                 // [  280] 
 vrec r3                                                                                                    // [  280] 
LINE 281".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VLD128            vr02,[i0, 0*16]                                                       // predLane0 pa [  281] 
       VLD128            vr03,[i0, 1*16]                                                       // predLane1 pa [  282] 
       VLD128            vr04,[i0, 2*16]                                                       // predLane2 pa [  283] 
       VLD128            vr05,[i0, 3*16]                                                       // predLane3 pa [  284] 
    
       VST64             vr02,[i1, 0*24+8]                                                     // predLane0 ra [  286] 
       VST32_4           vr02,[i1, 0*24+16]                                                    // predLane0 ra [  287] 
       VST32_6           vr02,[i1, 0*24+20]                                                    // predLane0 ra [  288] 
       VST64             vr03,[i1, 1*24+8]                                                     // predLane1 ra [  289] 
       VST32_4           vr03,[i1, 1*24+16]                                                    // predLane1 ra [  290] 
       VST32_6           vr03,[i1, 1*24+20]                                                    // predLane1 ra [  291] 
       VST64             vr04,[i1, 2*24+8]                                                     // predLane2 ra [  292] 
       VST32_4           vr04,[i1, 2*24+16]                                                    // predLane2 ra [  293] 
       VST32_6           vr04,[i1, 2*24+20]                                                    // predLane2 ra [  294] 
       VST64             vr05,[i1, 3*24+8]                                                     // predLane3 ra [  295] 
       VST32_4           vr05,[i1, 3*24+16]                                                    // predLane3 ra [  296] 
       VST32_6           vr05,[i1, 3*24+20]                                                    // predLane3 ra [  297] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i2            i2, r3                                                        // lpCnt lpCnt YLpStart [  302] 
       VIM               i0, i0, 64  //DS_______________________________________________________________ pa pa [  303] 
       VIM               i1, i1, 96  //DS_______________________________________________________________ ra ra [  304] 
       VNOP                //DS_______________________________________________________________________________ [  305] 
   
   
    //----------------------
    // Chroma
    //----------------------
       VIM               i1, i1, 96                                                                   // ra ra [  311] 
    // Initialise the loop count
    // The Chroma blocks are divided into 4 slices of 2 rows each.
    // One slice is processed per iteration
       VMOVW             vr00, 3, 4                                                                   // lpCnt [  315] 
    
//UVLpStart --------                                                                                        // [  317] 
 vendrec r4                                                                                                 // [  317] 
 vrec r4                                                                                                    // [  317] 
LINE 318".\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss"
       VLD64             vr02,[i0, 0*8]                                                        // predLane0 pa [  318] 
       VLD64             vr03,[i0, 1*8]                                                        // predLane1 pa [  319] 
       VLD64             vr04,[i0, 2*8]                                                        // predLane2 pa [  320] 
       VLD64             vr05,[i0, 3*8]                                                        // predLane3 pa [  321] 
    
       VST32_0           vr02,[i1, 0*12+4]                                                     // predLane0 ra [  323] 
       VST32_2           vr02,[i1, 0*12+8]                                                     // predLane0 ra [  324] 
       VST32_0           vr03,[i1, 1*12+4]                                                     // predLane1 ra [  325] 
       VST32_2           vr03,[i1, 1*12+8]                                                     // predLane1 ra [  326] 
       VST32_0           vr04,[i1, 2*12+4]                                                     // predLane2 ra [  327] 
       VST32_2           vr04,[i1, 2*12+8]                                                     // predLane2 ra [  328] 
       VST32_0           vr05,[i1, 3*12+4]                                                     // predLane3 ra [  329] 
       VST32_2           vr05,[i1, 3*12+8]                                                     // predLane3 ra [  330] 
    
    // decrement the loop count and jump back at the loop start
    // if loop count is non-zero. The buffer pointers are updated
    // during the 3 cycle delay slot
       VJD.i2            i2, r4                                                       // lpCnt lpCnt UVLpStart [  335] 
       VIM               i0, i0, 32  //DS_______________________________________________________________ pa pa [  336] 
       VIM               i1, i1, 48  //DS_______________________________________________________________ ra ra [  337] 
       VNOP                //DS_______________________________________________________________________________ [  338] 
    
    
// End                                                                                                      // [  341] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz ReconstructSkipped_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc ReconstructSkipped                                                                               // [  343] 




const int MACRO_IDX_ReconstructLuma16X16 = 0;
const int MACRO_IDX_ReconstructChroma = 1;
const int MACRO_IDX_ReconstructSkipped = 2;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int EncoderAddSubtract_Record(int sa, int SIMDnum)
{
    int *map = EncoderAddSubtract;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_ReconstructLuma16X16] = sa;
    sa = Record_ReconstructLuma16X16(map);

    map[MACRO_IDX_ReconstructChroma] = sa;
    sa = Record_ReconstructChroma(map);

    map[MACRO_IDX_ReconstructSkipped] = sa;
    sa = Record_ReconstructSkipped(map);

    return sa;
}
