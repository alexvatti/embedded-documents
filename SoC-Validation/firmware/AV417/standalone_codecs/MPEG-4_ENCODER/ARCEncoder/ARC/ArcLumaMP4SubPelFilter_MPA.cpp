//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss'

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
#include ".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.h"


// Macro table
    int LumaSubPelFilter[4];
    int LumaSubPelFilter_Record(int sa, int SIMDnum);

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
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"


//------------------------------------------------------------------------------
// Name:          CopyLumaPredToSDM
// Purpose:       Copies the luma prediction from
//                main memory to the current MPC pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyLumaPredToSDM                                                                                            // [   41] 
_Asm int Record_CopyLumaPredToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                                       // [   42] 
// p16    i1  mbY                                                                                                       // [   43] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                                        // [   44] 
// p32    k6  sdmAddr                                                                                                   // [   44] 
// p32    k8  dr2Val                                                                                                    // [   45] 

// Begin                                                                                                                // [   47] 
// p16    i2  dmaOutReg16                                                                                               // [   48] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                                    // [   49] 
// p32    k2  dmaOutReg32                                                                                               // [   49] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 53".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                                    // dmaOutReg32 [   53] 
       VDMAISET          dr1, k2                                                                            // dmaOutReg32 [   54] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                                // sdmAddr [   57] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                                 // dr2Val [   65] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                                          // dmaOutReg32 [   77] 
       VDMAISET          dr6, k2                                                                            // dmaOutReg32 [   78] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                            // dmaOutReg32 [   81] 
       VDMAISET          dr7, k2                                                                            // dmaOutReg32 [   82] 

    // start dma in
       VDMAIRUN          i0, i1                                                                                 // mbX mbY [   85] 
    
//    vdmawait          0,0
    
// End                                                                                                                  // [   89] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyLumaPredToSDM                                                                                            // [   91] 



// [Macro] CallBackDoLumaSubPelFilter                                                                                   // [   93] 
_Asm int Record_CallBackDoLumaSubPelFilter(int *mac_tab)
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
// p16    i0  BufIdx                                                                                                    // [   95] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 102".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VSEND             r0, i0, 0                                                                               // BufIdx [  102] 
   mov                 r1, Service_DoLumaSubPelFilter   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                                     // [  104] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoLumaSubPelFilter                                                                                   // [  106] 





//------------------------------------------------------------------------------
// Name:          FilterLumamaPred
// Purpose:       Filters Luma Prediction using simple bilinear interpolation
// Arguments:     predBuf: Input buffer pointer
//                outBuf : Output buffer pointer
//                horFrac: half pel precision horizontally
//                verFrac: half pel precision vertically
//                rndCtrl: Rounding control parameter
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] FilterLumaPred                                                                                               // [  120] 
_Asm int Record_FilterLumaPred(int *mac_tab)
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
FilterLumaPred_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

// p16    i0  predBuf                                                                                                   // [  121] 
// p16    i1  outBuf                                                                                                    // [  122] 
// p16    i2  horFrac                                                                                                   // [  123] 
// p16    i10  verFrac                                                                                                  // [  124] 
// p16    i3  rndCtrl                                                                                                   // [  125] 

// Begin                                                                                                                // [  127] 
// ****** Informational : Register space for symbol "subPel" already allocated.                                         // [  128] 
// p16    i4  subPel                                                                                                    // [  128] 
    
//vec16  vr02  lumaLane0                                                                                                // [  130] 
//vec16  vr03  lumaLane1                                                                                                // [  131] 
//vec16  vr04  lumaLane2                                                                                                // [  132] 
//vec16  vr05  lumaLane3                                                                                                // [  133] 
//vec16  vr06  lumaLane4                                                                                                // [  134] 
//vec16  vr07  lumaLane5                                                                                                // [  135] 
//vec16  vr08  lumaLane6                                                                                                // [  136] 
//vec16  vr09  lumaLane7                                                                                                // [  137] 
//vec16  vr10  lumaLane8                                                                                                // [  138] 
    
//vec16  vr11  lumaOffsetLane0                                                                                          // [  140] 
//vec16  vr12  lumaOffsetLane1                                                                                          // [  141] 
//vec16  vr13  lumaOffsetLane2                                                                                          // [  142] 
//vec16  vr14  lumaOffsetLane3                                                                                          // [  143] 
//vec16  vr15  lumaOffsetLane4                                                                                          // [  144] 
//vec16  vr16  lumaOffsetLane5                                                                                          // [  145] 
//vec16  vr17  lumaOffsetLane6                                                                                          // [  146] 
//vec16  vr18  lumaOffsetLane7                                                                                          // [  147] 
//vec16  vr19  lumaOffsetLane8                                                                                          // [  148] 
    
    // Load data to be filtered
LINE 151".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr02,[i0, 0*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane0 predBuf [  151] 
       VLD64W            vr03,[i0, 1*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane1 predBuf [  152] 
       VLD64W            vr04,[i0, 2*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane2 predBuf [  153] 
       VLD64W            vr05,[i0, 3*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane3 predBuf [  154] 
       VLD64W            vr06,[i0, 4*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane4 predBuf [  155] 
//  vld64w              lumaLane5, [predBuf, 5*ME_REFERENCE_ARRAY_STRIDE] <--)
//  vld64w              lumaLane6, [predBuf, 6*ME_REFERENCE_ARRAY_STRIDE] <--> moved to jump delay slots below
//  vld64w              lumaLane7, [predBuf, 7*ME_REFERENCE_ARRAY_STRIDE] <--)
    
    // If none of horFrac or verFrac is set then go straight to DMA out
    // No filtering is to be performed
// s16    vr20.4  tmpFrac                                                                                               // [  162] 
LINE 163".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VOR.4             vr20, vr00, vr01                                                       // tmpFrac horFrac verFrac [  163] 
        VMR6W.16        vr00, vr20, vr20                                                                 // subPel tmpFrac [  164] 
LINE 165".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VJP.i4            r3                                                                             // subPel Continue [  165] 
// ****** Warning : Unmarked delay slot found at line number 166 *****
LINE 166".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr07,[i0, 5*ME_REFERENCE_ARRAY_STRIDE]  //DS___________________________________ lumaLane5 predBuf [  166] 
// ****** Warning : Unmarked delay slot found at line number 167 *****
LINE 167".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr08,[i0, 6*ME_REFERENCE_ARRAY_STRIDE]  //DS___________________________________ lumaLane6 predBuf [  167] 
// ****** Warning : Unmarked delay slot found at line number 168 *****
LINE 168".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr09,[i0, 7*ME_REFERENCE_ARRAY_STRIDE]  //DS___________________________________ lumaLane7 predBuf [  168] 


       VASRPWB           vr02, vr02, 0                                                              // lumaLane0 lumaLane0 [  171] 
       VASRPWB           vr03, vr03, 0                                                              // lumaLane1 lumaLane1 [  172] 
       VASRPWB           vr04, vr04, 0                                                              // lumaLane2 lumaLane2 [  173] 
       VASRPWB           vr05, vr05, 0                                                              // lumaLane3 lumaLane3 [  174] 
       VASRPWB           vr06, vr06, 0                                                              // lumaLane4 lumaLane4 [  175] 
       VJP               r4                                                                                      // Output [  176] 
// ****** Warning : Unmarked delay slot found at line number 177 *****
LINE 177".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VASRPWB           vr07, vr07, 0  //DS__________________________________________________________ lumaLane5 lumaLane5 [  177] 
// ****** Warning : Unmarked delay slot found at line number 178 *****
LINE 178".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VASRPWB           vr08, vr08, 0  //DS__________________________________________________________ lumaLane6 lumaLane6 [  178] 
// ****** Warning : Unmarked delay slot found at line number 179 *****
LINE 179".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VASRPWB           vr09, vr09, 0  //DS__________________________________________________________ lumaLane7 lumaLane7 [  179] 
    
//Continue --------                                                                                                     // [  181] 
 vendrec r3                                                                                                             // [  181] 
 vrec r3                                                                                                                // [  181] 
    // If horFilter isn't set go straight to the Vertical filtering section
LINE 183".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VJP!i2            r5                                                                           // horFrac VerFilter [  183] 
    
    // If only horFrac is set then perform horizontal filtering
    // Load in extra column to the right of the 8x8 block
// ****** Warning : Unmarked delay slot found at line number 187 *****
LINE 187".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr11,[i0, 0*ME_REFERENCE_ARRAY_STRIDE+8]  //DS___________________________ lumaOffsetLane0 predBuf [  187] 
// ****** Warning : Unmarked delay slot found at line number 188 *****
LINE 188".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr12,[i0, 1*ME_REFERENCE_ARRAY_STRIDE+8]  //DS___________________________ lumaOffsetLane1 predBuf [  188] 
// ****** Warning : Unmarked delay slot found at line number 189 *****
LINE 189".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr13,[i0, 2*ME_REFERENCE_ARRAY_STRIDE+8]  //DS___________________________ lumaOffsetLane2 predBuf [  189] 
       VLD64W            vr14,[i0, 3*ME_REFERENCE_ARRAY_STRIDE+8]                               // lumaOffsetLane3 predBuf [  190] 
       VLD64W            vr15,[i0, 4*ME_REFERENCE_ARRAY_STRIDE+8]                               // lumaOffsetLane4 predBuf [  191] 
       VLD64W            vr16,[i0, 5*ME_REFERENCE_ARRAY_STRIDE+8]                               // lumaOffsetLane5 predBuf [  192] 
       VLD64W            vr17,[i0, 6*ME_REFERENCE_ARRAY_STRIDE+8]                               // lumaOffsetLane6 predBuf [  193] 
       VLD64W            vr18,[i0, 7*ME_REFERENCE_ARRAY_STRIDE+8]                               // lumaOffsetLane7 predBuf [  194] 
    
       VMR1W             vr11, vr11, vr02                                     // lumaOffsetLane0 lumaOffsetLane0 lumaLane0 [  196] 
       VMR1W             vr12, vr12, vr03                                     // lumaOffsetLane1 lumaOffsetLane1 lumaLane1 [  197] 
       VMR1W             vr13, vr13, vr04                                     // lumaOffsetLane2 lumaOffsetLane2 lumaLane2 [  198] 
       VMR1W             vr14, vr14, vr05                                     // lumaOffsetLane3 lumaOffsetLane3 lumaLane3 [  199] 
       VMR1W             vr15, vr15, vr06                                     // lumaOffsetLane4 lumaOffsetLane4 lumaLane4 [  200] 
//  vmr1w               lumaOffsetLane5, lumaOffsetLane5, lumaLane5 <--)
//  vmr1w               lumaOffsetLane6, lumaOffsetLane6, lumaLane6 <--> moved to jump delay slots below
//  vmr1w               lumaOffsetLane7, lumaOffsetLane7, lumaLane7 <--)
    
    // If verFrac is also set then go to 2D filter
    // The above loads and VMRs aren't wasted as they are also needed for
    // the 2D case anyway
       VJP.i10           r6                                                                            // verFrac 2dFilter [  208] 
// ****** Warning : Unmarked delay slot found at line number 209 *****
LINE 209".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VMR1W             vr16, vr16, vr07  //DS_________________________________ lumaOffsetLane5 lumaOffsetLane5 lumaLane5 [  209] 
// ****** Warning : Unmarked delay slot found at line number 210 *****
LINE 210".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VMR1W             vr17, vr17, vr08  //DS_________________________________ lumaOffsetLane6 lumaOffsetLane6 lumaLane6 [  210] 
// ****** Warning : Unmarked delay slot found at line number 211 *****
LINE 211".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VMR1W             vr18, vr18, vr09  //DS_________________________________ lumaOffsetLane7 lumaOffsetLane7 lumaLane7 [  211] 
    
    //actual horizontal filter
       VADDW             vr02, vr02, vr11                                           // lumaLane0 lumaLane0 lumaOffsetLane0 [  214] 
       VADDW             vr03, vr03, vr12                                           // lumaLane1 lumaLane1 lumaOffsetLane1 [  215] 
       VADDW             vr04, vr04, vr13                                           // lumaLane2 lumaLane2 lumaOffsetLane2 [  216] 
       VADDW             vr05, vr05, vr14                                           // lumaLane3 lumaLane3 lumaOffsetLane3 [  217] 
       VADDW             vr06, vr06, vr15                                           // lumaLane4 lumaLane4 lumaOffsetLane4 [  218] 
//  vaddw               lumaLane5, lumaLane5, lumaOffsetLane5 <--)
//  vaddw               lumaLane6, lumaLane6, lumaOffsetLane6 <--> moved to jump delay slots below
//  vaddw               lumaLane7, lumaLane7, lumaOffsetLane7 <--)
        
       VJP               r7                                                                               // RoundAndShift [  223] 
// ****** Warning : Unmarked delay slot found at line number 224 *****
LINE 224".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VADDW             vr07, vr07, vr16  //DS_______________________________________ lumaLane5 lumaLane5 lumaOffsetLane5 [  224] 
// ****** Warning : Unmarked delay slot found at line number 225 *****
LINE 225".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VADDW             vr08, vr08, vr17  //DS_______________________________________ lumaLane6 lumaLane6 lumaOffsetLane6 [  225] 
// ****** Warning : Unmarked delay slot found at line number 226 *****
LINE 226".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VADDW             vr09, vr09, vr18  //DS_______________________________________ lumaLane7 lumaLane7 lumaOffsetLane7 [  226] 
    
    // If only verFilter is set then load the extra lane at the bottom of the block
//VerFilter --------                                                                                                    // [  229] 
 vendrec r5                                                                                                             // [  229] 
 vrec r5                                                                                                                // [  229] 
LINE 230".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr10,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]                                       // lumaLane8 predBuf [  230] 
    // perform actual vertical filter
       VADDW             vr02, vr02, vr03                                                 // lumaLane0 lumaLane0 lumaLane1 [  232] 
       VADDW             vr03, vr03, vr04                                                 // lumaLane1 lumaLane1 lumaLane2 [  233] 
       VADDW             vr04, vr04, vr05                                                 // lumaLane2 lumaLane2 lumaLane3 [  234] 
       VADDW             vr05, vr05, vr06                                                 // lumaLane3 lumaLane3 lumaLane4 [  235] 
       VADDW             vr06, vr06, vr07                                                 // lumaLane4 lumaLane4 lumaLane5 [  236] 
       VADDW             vr07, vr07, vr08                                                 // lumaLane5 lumaLane5 lumaLane6 [  237] 
       VADDW             vr08, vr08, vr09                                                 // lumaLane6 lumaLane6 lumaLane7 [  238] 
       VADDW             vr09, vr09, vr10                                                 // lumaLane7 lumaLane7 lumaLane8 [  239] 
    
//RoundAndShift --------                                                                                                // [  241] 
 vendrec r7                                                                                                             // [  241] 
 vrec r7                                                                                                                // [  241] 
LINE 242".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VSUBW             vr02, vr02, i3                                                     // lumaLane0 lumaLane0 rndCtrl [  242] 
       VSUBW             vr03, vr03, i3                                                     // lumaLane1 lumaLane1 rndCtrl [  243] 
       VSUBW             vr04, vr04, i3                                                     // lumaLane2 lumaLane2 rndCtrl [  244] 
       VSUBW             vr05, vr05, i3                                                     // lumaLane3 lumaLane3 rndCtrl [  245] 
       VSUBW             vr06, vr06, i3                                                     // lumaLane4 lumaLane4 rndCtrl [  246] 
       VSUBW             vr07, vr07, i3                                                     // lumaLane5 lumaLane5 rndCtrl [  247] 
       VSUBW             vr08, vr08, i3                                                     // lumaLane6 lumaLane6 rndCtrl [  248] 
       VSUBW             vr09, vr09, i3                                                     // lumaLane7 lumaLane7 rndCtrl [  249] 

       VASRRPWB          vr02, vr02, 1                                                              // lumaLane0 lumaLane0 [  251] 
       VASRRPWB          vr03, vr03, 1                                                              // lumaLane1 lumaLane1 [  252] 
       VASRRPWB          vr04, vr04, 1                                                              // lumaLane2 lumaLane2 [  253] 
       VASRRPWB          vr05, vr05, 1                                                              // lumaLane3 lumaLane3 [  254] 
       VASRRPWB          vr06, vr06, 1                                                              // lumaLane4 lumaLane4 [  255] 
       VASRRPWB          vr07, vr07, 1                                                              // lumaLane5 lumaLane5 [  256] 
       VASRRPWB          vr08, vr08, 1                                                              // lumaLane6 lumaLane6 [  257] 
       VASRRPWB          vr09, vr09, 1                                                              // lumaLane7 lumaLane7 [  258] 
    
    // Go to Output
       VJP               r4                                                                                      // Output [  261] 

    
// ****** Warning : Unmarked delay slot found at line number 264 *****
LINE 264".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
//2dFilter --------  //DS_________________________________________________________________________________________________ [  264] 
 vendrec r6  //DS_________________________________________________________________________________________________________ [  264] 
 vrec r6  //DS____________________________________________________________________________________________________________ [  264] 
// ****** Warning : Unmarked delay slot found at line number 265 *****
LINE 265".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr10,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]  //DS___________________________________ lumaLane8 predBuf [  265] 
// ****** Warning : Unmarked delay slot found at line number 266 *****
LINE 266".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD64W            vr19,[i0, 8*ME_REFERENCE_ARRAY_STRIDE+8]  //DS___________________________ lumaOffsetLane8 predBuf [  266] 
// ****** Warning : Unmarked delay slot found at line number 267 *****
LINE 267".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VMR1W             vr19, vr19, vr10  //DS_________________________________ lumaOffsetLane8 lumaOffsetLane8 lumaLane8 [  267] 
    
    // 2D filtering
       VADDW             vr02, vr02, vr11                                           // lumaLane0 lumaLane0 lumaOffsetLane0 [  270] 
       VADDAW            vr02, vr03, vr12                                           // lumaLane0 lumaLane1 lumaOffsetLane1 [  271] 
       VADDW             vr03, vr03, vr12                                           // lumaLane1 lumaLane1 lumaOffsetLane1 [  272] 
       VADDAW            vr03, vr04, vr13                                           // lumaLane1 lumaLane2 lumaOffsetLane2 [  273] 
       VADDW             vr04, vr04, vr13                                           // lumaLane2 lumaLane2 lumaOffsetLane2 [  274] 
       VADDAW            vr04, vr05, vr14                                           // lumaLane2 lumaLane3 lumaOffsetLane3 [  275] 
       VADDW             vr05, vr05, vr14                                           // lumaLane3 lumaLane3 lumaOffsetLane3 [  276] 
       VADDAW            vr05, vr06, vr15                                           // lumaLane3 lumaLane4 lumaOffsetLane4 [  277] 
       VADDW             vr06, vr06, vr15                                           // lumaLane4 lumaLane4 lumaOffsetLane4 [  278] 
       VADDAW            vr06, vr07, vr16                                           // lumaLane4 lumaLane5 lumaOffsetLane5 [  279] 
       VADDW             vr07, vr07, vr16                                           // lumaLane5 lumaLane5 lumaOffsetLane5 [  280] 
       VADDAW            vr07, vr08, vr17                                           // lumaLane5 lumaLane6 lumaOffsetLane6 [  281] 
       VADDW             vr08, vr08, vr17                                           // lumaLane6 lumaLane6 lumaOffsetLane6 [  282] 
       VADDAW            vr08, vr09, vr18                                           // lumaLane6 lumaLane7 lumaOffsetLane7 [  283] 
       VADDW             vr09, vr09, vr18                                           // lumaLane7 lumaLane7 lumaOffsetLane7 [  284] 
       VADDAW            vr09, vr10, vr19                                           // lumaLane7 lumaLane8 lumaOffsetLane8 [  285] 

       VSUBW             vr02, vr02, i3                                                     // lumaLane0 lumaLane0 rndCtrl [  287] 
       VSUBW             vr03, vr03, i3                                                     // lumaLane1 lumaLane1 rndCtrl [  288] 
       VSUBW             vr04, vr04, i3                                                     // lumaLane2 lumaLane2 rndCtrl [  289] 
       VSUBW             vr05, vr05, i3                                                     // lumaLane3 lumaLane3 rndCtrl [  290] 
       VSUBW             vr06, vr06, i3                                                     // lumaLane4 lumaLane4 rndCtrl [  291] 
       VSUBW             vr07, vr07, i3                                                     // lumaLane5 lumaLane5 rndCtrl [  292] 
       VSUBW             vr08, vr08, i3                                                     // lumaLane6 lumaLane6 rndCtrl [  293] 
       VSUBW             vr09, vr09, i3                                                     // lumaLane7 lumaLane7 rndCtrl [  294] 

       VASRRPWB          vr02, vr02, 2                                                              // lumaLane0 lumaLane0 [  296] 
       VASRRPWB          vr03, vr03, 2                                                              // lumaLane1 lumaLane1 [  297] 
       VASRRPWB          vr04, vr04, 2                                                              // lumaLane2 lumaLane2 [  298] 
       VASRRPWB          vr05, vr05, 2                                                              // lumaLane3 lumaLane3 [  299] 
       VASRRPWB          vr06, vr06, 2                                                              // lumaLane4 lumaLane4 [  300] 
       VASRRPWB          vr07, vr07, 2                                                              // lumaLane5 lumaLane5 [  301] 
       VASRRPWB          vr08, vr08, 2                                                              // lumaLane6 lumaLane6 [  302] 
       VASRRPWB          vr09, vr09, 2                                                              // lumaLane7 lumaLane7 [  303] 

//Output --------                                                                                                       // [  305] 
 vendrec r4                                                                                                             // [  305] 
 vrec r4                                                                                                                // [  305] 
LINE 306".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VST64             vr02,[i1, 0*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane0 outBuf [  306] 
       VST64             vr03,[i1, 1*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane1 outBuf [  307] 
       VST64             vr04,[i1, 2*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane2 outBuf [  308] 
       VST64             vr05,[i1, 3*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane3 outBuf [  309] 
       VST64             vr06,[i1, 4*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane4 outBuf [  310] 
       VST64             vr07,[i1, 5*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane5 outBuf [  311] 
       VST64             vr08,[i1, 6*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane6 outBuf [  312] 
       VST64             vr09,[i1, 7*ME_REFERENCE_ARRAY_STRIDE]                                        // lumaLane7 outBuf [  313] 

// End                                                                                                                  // [  315] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz FilterLumaPred_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc FilterLumaPred                                                                                               // [  317] 



//------------------------------------------------------------------------------
// Name:          StoreFilteredLumaPred
// Purpose:       
//                
// Arguments:     src
//                dest
//                
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] StoreFilteredLumaPred                                                                                        // [  328] 
_Asm int Record_StoreFilteredLumaPred(int *mac_tab)
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

// p16    i0  src                                                                                                       // [  329] 
// p16    i1  dst                                                                                                       // [  330] 
// Begin                                                                                                                // [  331] 
//vec16  vr02  v1                                                                                                       // [  332] 
//vec16  vr03  v2                                                                                                       // [  333] 
//vec16  vr04  v3                                                                                                       // [  334] 
//vec16  vr05  v4                                                                                                       // [  335] 
    
LINE 337".\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss"
       VLD128            vr02,[i0, 0*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [  337] 
       VLD128            vr03,[i0, 1*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [  338] 
       VLD128            vr04,[i0, 2*ME_REFERENCE_ARRAY_STRIDE]                                                  // v3 src [  339] 
       VLD128            vr05,[i0, 3*ME_REFERENCE_ARRAY_STRIDE]                                                  // v4 src [  340] 
       VST128            vr02,[i1, 0*PCB_REFERENCE_STRIDE]                                                       // v1 dst [  341] 
       VST128            vr03,[i1, 1*PCB_REFERENCE_STRIDE]                                                       // v2 dst [  342] 
       VST128            vr04,[i1, 2*PCB_REFERENCE_STRIDE]                                                       // v3 dst [  343] 
       VST128            vr05,[i1, 3*PCB_REFERENCE_STRIDE]                                                       // v4 dst [  344] 
    
       VLD128            vr02,[i0, 4*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [  346] 
       VLD128            vr03,[i0, 5*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [  347] 
       VLD128            vr04,[i0, 6*ME_REFERENCE_ARRAY_STRIDE]                                                  // v3 src [  348] 
       VLD128            vr05,[i0, 7*ME_REFERENCE_ARRAY_STRIDE]                                                  // v4 src [  349] 
       VST128            vr02,[i1, 4*PCB_REFERENCE_STRIDE]                                                       // v1 dst [  350] 
       VST128            vr03,[i1, 5*PCB_REFERENCE_STRIDE]                                                       // v2 dst [  351] 
       VST128            vr04,[i1, 6*PCB_REFERENCE_STRIDE]                                                       // v3 dst [  352] 
       VST128            vr05,[i1, 7*PCB_REFERENCE_STRIDE]                                                       // v4 dst [  353] 
    
       VLD128            vr02,[i0, 8*ME_REFERENCE_ARRAY_STRIDE]                                                  // v1 src [  355] 
       VLD128            vr03,[i0, 9*ME_REFERENCE_ARRAY_STRIDE]                                                  // v2 src [  356] 
       VLD128            vr04,[i0, 10*ME_REFERENCE_ARRAY_STRIDE]                                                 // v3 src [  357] 
       VLD128            vr05,[i0, 11*ME_REFERENCE_ARRAY_STRIDE]                                                 // v4 src [  358] 
       VST128            vr02,[i1, 8*PCB_REFERENCE_STRIDE]                                                       // v1 dst [  359] 
       VST128            vr03,[i1, 9*PCB_REFERENCE_STRIDE]                                                       // v2 dst [  360] 
       VST128            vr04,[i1, 10*PCB_REFERENCE_STRIDE]                                                      // v3 dst [  361] 
       VST128            vr05,[i1, 11*PCB_REFERENCE_STRIDE]                                                      // v4 dst [  362] 
    
       VLD128            vr02,[i0, 12*ME_REFERENCE_ARRAY_STRIDE]                                                 // v1 src [  364] 
       VLD128            vr03,[i0, 13*ME_REFERENCE_ARRAY_STRIDE]                                                 // v2 src [  365] 
       VLD128            vr04,[i0, 14*ME_REFERENCE_ARRAY_STRIDE]                                                 // v3 src [  366] 
       VLD128            vr05,[i0, 15*ME_REFERENCE_ARRAY_STRIDE]                                                 // v4 src [  367] 
       VST128            vr02,[i1, 12*PCB_REFERENCE_STRIDE]                                                      // v1 dst [  368] 
       VST128            vr03,[i1, 13*PCB_REFERENCE_STRIDE]                                                      // v2 dst [  369] 
       VST128            vr04,[i1, 14*PCB_REFERENCE_STRIDE]                                                      // v3 dst [  370] 
       VST128            vr05,[i1, 15*PCB_REFERENCE_STRIDE]                                                      // v4 dst [  371] 
// End                                                                                                                  // [  372] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc StoreFilteredLumaPred                                                                                        // [  373] 



const int MACRO_IDX_CopyLumaPredToSDM = 0;
const int MACRO_IDX_CallBackDoLumaSubPelFilter = 1;
const int MACRO_IDX_FilterLumaPred = 2;
const int MACRO_IDX_StoreFilteredLumaPred = 3;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int LumaSubPelFilter_Record(int sa, int SIMDnum)
{
    int *map = LumaSubPelFilter;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_CopyLumaPredToSDM] = sa;
    sa = Record_CopyLumaPredToSDM(map);

    map[MACRO_IDX_CallBackDoLumaSubPelFilter] = sa;
    sa = Record_CallBackDoLumaSubPelFilter(map);

    map[MACRO_IDX_FilterLumaPred] = sa;
    sa = Record_FilterLumaPred(map);

    map[MACRO_IDX_StoreFilteredLumaPred] = sa;
    sa = Record_StoreFilteredLumaPred(map);

    return sa;
}
