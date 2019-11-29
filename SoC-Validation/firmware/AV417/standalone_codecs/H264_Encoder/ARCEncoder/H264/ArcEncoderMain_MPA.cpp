//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcEncoderMain_MPA.sss'

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

#include ".\ARCEncoder\H264\ArcEncoderMain_MPA.h"


// Macro table
    int EncoderMain[5];
    int EncoderMain_Record(int sa, int SIMDnum);



#include "ArcMPC.h"
#include "ArcFrameBuffers.h"



//------------------------------------------------------------------------------
// Name:          CopyCurrentChromaToSDM
// Purpose:       Copies the chroma components of the current macroblock from
//                main memory to the current MPC pixel data area in the SDM
// Arguments:     mbX = horizontal position in picture in macroblocks
//                mbY = vertical position in picture in macroblocks
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyCurrentChromaToSDM                                                                           // [   43] 
_Asm int Record_CopyCurrentChromaToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                           // [   44] 
// p16    i1  mbY                                                                                           // [   45] 

// Begin                                                                                                    // [   47] 
// p16    i8  picX                                                                                          // [   48] 
// p16    i9  picY                                                                                          // [   49] 
// p16    i2  dmaOutReg16                                                                                   // [   50] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [   51] 
// p32    k2  dmaOutReg32                                                                                   // [   51] 

    // chroma u

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 57".\ARCEncoder\H264\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, PCB_CURRENT_STRIDE                                               // dmaOutReg32 [   57] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [   58] 

    // dma commands only take 32 bit scalars, however CirBuf is a 16 bit scalar
    // that we want to and an immediate to (and store in a 32 bit scalar).
    // however there are no instructions to do this at present, so just perform
    // a 16 bit operation on the bottom half of the 32 scalar knowing that the
    // previous dr1 setup left the top bits set to zero
    // dr0: sdm address
       VIM               i2, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_U                      // dmaOutReg16 CirBuf [   66] 
       VDMAISET          dr0, k2                                                                // dmaOutReg32 [   67] 

    // dr2: block info
    //   [7:0] = 8 = horizontal block size
    //  [15:8] = 8 = vertical block size
    // [20:16] = FRAME_TABLE_U_CURR_ADDR = frame table index
   mov                 r0, ((FRAME_TABLE_U_INDEX + FRAME_TABLE_CURR_INDEX) << 16) | 0x0808
       VDMAISET          dr2, r0                                                                            // [   74] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [   86] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [   87] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [   90] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [   91] 

    // convert mb co-ordinates into pixel co-ordinates in the chroma plane
       VMULW.3           vr01, vr00, 8                                                    // picX'picY mbX'mbY [   94] 

    // start dma in
       VDMAIRUN          i8, i9                                                                   // picX picY [   97] 


    // chroma u

    // top bits are still assumed to be zero here
    // dr0: sdm address
       VIM               i2, i4, MPO_PixelCoeffBuffer+PCB_CURRENT_V                      // dmaOutReg16 CirBuf [  104] 
       VDMAISET          dr0, k2                                                                // dmaOutReg32 [  105] 

    // this dma reg must be written after dr0, for the reasons given above
    // dr2: block info
    //   [7:0] = 8 = horizontal block size
    //  [15:8] = 8 = vertical block size
    // [20:16] = FRAME_TABLE_V_CURR_ADDR = frame table index
   mov                 r0, ((FRAME_TABLE_V_INDEX + FRAME_TABLE_CURR_INDEX) << 16) | 0x0808
       VDMAISET          dr2, r0                                                                            // [  113] 

    // start dma in
       VDMAIRUN          i8, i9                                                                   // picX picY [  116] 
    

//    // Send channel cmd
//@   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
//@   ld                  r0,[r0,0]
//@   mov                 r1, Service_ChromaDMA   // Arc routine to call when complete
//    vsendr              r0, r1, 63
// End                                                                                                      // [  124] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyCurrentChromaToSDM                                                                           // [  126] 




//------------------------------------------------------------------------------
// Name:          CopyChromaPredToSDM
// Purpose:       Copies the chroma prediction from
//                main memory to the current MPC pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyChromaPredToSDM                                                                              // [  138] 
_Asm int Record_CopyChromaPredToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                           // [  139] 
// p16    i1  mbY                                                                                           // [  140] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  141] 
// p32    k6  sdmAddr                                                                                       // [  141] 
// p32    k8  dr2Val                                                                                        // [  142] 

// Begin                                                                                                    // [  144] 
// p16    i2  dmaOutReg16                                                                                   // [  145] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  146] 
// p32    k2  dmaOutReg32                                                                                   // [  146] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 150".\ARCEncoder\H264\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, CHROMA_WORKING_ARRAY_STRIDE                                      // dmaOutReg32 [  150] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  151] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  154] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_U_REF1_ADDR / FRAME_TABLE_V_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  162] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  174] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  175] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  178] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  179] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  182] 
    
    
// End                                                                                                      // [  185] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyChromaPredToSDM                                                                              // [  187] 



//------------------------------------------------------------------------------
// Name:          CopyMERefToSDM
// Purpose:       Copies the ME reference data from
//                main memory to the reference ME pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyMERefToSDM                                                                                   // [  198] 
_Asm int Record_CopyMERefToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                           // [  199] 
// p16    i1  mbY                                                                                           // [  200] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  201] 
// p32    k6  sdmAddr                                                                                       // [  201] 
// p32    k8  dr2Val                                                                                        // [  202] 

// Begin                                                                                                    // [  204] 
// p16    i2  dmaOutReg16                                                                                   // [  205] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  206] 
// p32    k2  dmaOutReg32                                                                                   // [  206] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 210".\ARCEncoder\H264\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                        // dmaOutReg32 [  210] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  211] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  214] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  222] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  234] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  235] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  238] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  239] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  242] 
    
    
// End                                                                                                      // [  245] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyMERefToSDM                                                                                   // [  247] 


//------------------------------------------------------------------------------
// Name:          CopyMBLumaToSDM
// Purpose:       Copies the MB current luma data from
//                main memory to the MB circular buffer pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------

// [Macro] CopyMBLumaToSDM                                                                                  // [  258] 
_Asm int Record_CopyMBLumaToSDM(int *mac_tab)
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

// p16    i0  mbX                                                                                           // [  259] 
// p16    i1  mbY                                                                                           // [  260] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  261] 
// p32    k6  sdmAddr                                                                                       // [  261] 
// p32    k8  dr2Val                                                                                        // [  262] 

// Begin                                                                                                    // [  264] 
// p16    i2  dmaOutReg16                                                                                   // [  265] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  266] 
// p32    k2  dmaOutReg32                                                                                   // [  266] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 270".\ARCEncoder\H264\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, PCB_CURRENT_STRIDE                                               // dmaOutReg32 [  270] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  271] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  274] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  282] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  294] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  295] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  298] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  299] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  302] 
    
    
// End                                                                                                      // [  305] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyMBLumaToSDM                                                                                  // [  307] 



const int MACRO_IDX_CopyCurrentChromaToSDM = 0;
const int MACRO_IDX_CopyChromaPredToSDM = 1;
const int MACRO_IDX_CopyMERefToSDM = 2;
const int MACRO_IDX_CopyMBLumaToSDM = 3;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int EncoderMain_Record(int sa, int SIMDnum)
{
    int *map = EncoderMain;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_CopyCurrentChromaToSDM] = sa;
    sa = Record_CopyCurrentChromaToSDM(map);

    map[MACRO_IDX_CopyChromaPredToSDM] = sa;
    sa = Record_CopyChromaPredToSDM(map);

    map[MACRO_IDX_CopyMERefToSDM] = sa;
    sa = Record_CopyMERefToSDM(map);

    map[MACRO_IDX_CopyMBLumaToSDM] = sa;
    sa = Record_CopyMBLumaToSDM(map);

    return sa;
}
