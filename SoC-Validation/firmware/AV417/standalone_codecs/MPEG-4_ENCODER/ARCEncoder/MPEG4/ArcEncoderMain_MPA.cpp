//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss'

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

#include ".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.h"


// Macro table
    int EncoderMain[15];
    int EncoderMain_Record(int sa, int SIMDnum);



#include "ArcMPC.h"
#include "ArcFrameBuffers.h"
#include "ArcMP4SDMTables.h"



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
LINE 57".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
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

// End                                                                                                      // [  118] 
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyCurrentChromaToSDM                                                                           // [  119] 




//------------------------------------------------------------------------------
// Name:          CopyChromaPredToSDM
// Purpose:       Copies the chroma prediction from
//                main memory to the current MPC pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyChromaPredToSDM                                                                              // [  131] 
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

// p16    i0  mbX                                                                                           // [  132] 
// p16    i1  mbY                                                                                           // [  133] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  134] 
// p32    k6  sdmAddr                                                                                       // [  134] 
// p32    k8  dr2Val                                                                                        // [  135] 

// Begin                                                                                                    // [  137] 
// p16    i2  dmaOutReg16                                                                                   // [  138] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  139] 
// p32    k2  dmaOutReg32                                                                                   // [  139] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 143".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, CHROMA_WORKING_ARRAY_STRIDE                                      // dmaOutReg32 [  143] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  144] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  147] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_U_REF1_ADDR / FRAME_TABLE_V_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  155] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  167] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  168] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  171] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  172] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  175] 
    
//    vdmawait			0,0
    
// End                                                                                                      // [  179] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyChromaPredToSDM                                                                              // [  181] 



//------------------------------------------------------------------------------
// Name:          OutputMacroblock
// Purpose:       Copies the reconstructed MB from SDM to main memory
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                ChromaU = non zero for U component and zero for V component
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] OutputMacroblock                                                                                 // [  192] 
_Asm int Record_OutputMacroblock(int *mac_tab)
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

// p16    i0  mbX                                                                                           // [  193] 
// p16    i1  mbY                                                                                           // [  194] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  195] 
// p32    k6  sdmAddr                                                                                       // [  195] 
// p32    k8  dr2Val                                                                                        // [  196] 

// Begin                                                                                                    // [  198] 
// p16    i2  dmaOutReg16                                                                                   // [  199] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  200] 
// p32    k2  dmaOutReg32                                                                                   // [  200] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 204".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, 16                                                               // dmaOutReg32 [  204] 
       VDMAOSET          dr1, k2                                                                // dmaOutReg32 [  205] 

    // dr0: sdm address
       VDMAOSET          dr0, k6                                                                    // sdmAddr [  208] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_U_REF1_ADDR / FRAME_TABLE_V_REF1_ADDR
       VDMAOSET          dr2, k8                                                                     // dr2Val [  216] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  228] 
       VDMAOSET          dr6, k2                                                                // dmaOutReg32 [  229] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  232] 
       VDMAOSET          dr7, k2                                                                // dmaOutReg32 [  233] 

    // start dma in
       VDMAORUN          i0, i1                                                                     // mbX mbY [  236] 
    
// End                                                                                                      // [  238] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc OutputMacroblock                                                                                 // [  240] 




// [Macro] SdmDmaIn                                                                                         // [  243] 
_Asm int Record_SdmDmaIn(int *mac_tab)
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

// p32    k0  sdmAddr                                                                                       // [  244] 
// p32    k2  extMemAddr                                                                                    // [  245] 
// ****** Informational : Register space for symbol "blkSize" already allocated.                            // [  246] 
// p32    k4  blkSize                                                                                       // [  246] 
    
// Begin                                                                                                    // [  248] 
    // dr0: sdm address
LINE 250".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VDMAISET          dr0, k0                                                                    // sdmAddr [  250] 
    
    // dr2: block info
    // Place block size information and frame table index 
    //   [7:0] = horizontal block size: 64*sizeof(unsigned short)
    //  [15:8] = vertical block size:   1
    // [20:16] = FRAME_TABLE:           31 (2D mode)
       VDMAISET          dr2, k4                                                                    // blkSize [  257] 

    // dr4: system memory address (contained in frame table)
       VDMAISET          dr4, k2                                                                 // extMemAddr [  260] 

    // start dma in
       VDMAIRUN          i0, i0                                                                             // [  263] 
    
       VDMAWAIT          0, 0                                                                               // [  265] 
    
// End                                                                                                      // [  267] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SdmDmaIn                                                                                         // [  269] 




// [Macro] RevertToSkipped                                                                                  // [  272] 
_Asm int Record_RevertToSkipped(int *mac_tab)
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
RevertToSkipped_RecLoop:
  ld r1,[r17 ,16]
  vrec r1

    // Params -----------
// p16    i0  cbIdx                                                                                         // [  274] 
// p16    i1  dquant                                                                                        // [  275] 
// p16    i2  LastTransQuant                                                                                // [  276] 
    // End params -------
    
    
// Begin                                                                                                    // [  283] 
// p16    i3  mbtype                                                                                        // [  284] 
// p16    i7  cbp                                                                                           // [  284] 
// s16    vr02.1  skp                                                                                       // [  285] 
// p16    i8  quant                                                                                         // [  286] 
// p16    i9  mv0                                                                                           // [  286] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|         cbp|     CirLeft|      FixBuf|      CirBuf|      mbtype| LastTransQ.|      dquant|       cbIdx|
//|VR 1|            |            |            |            |            |            |         mv0|       quant|
//|VR 2|            |            |            |            |            |            |            |         skp|
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

        
    
//       if ( (DQuant == 0) &&    
LINE 291".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VJP.i1            r3                                                                     // dquant exit [  291] 
       VLD8W_2.i1        vr00,[i4, MPO_QuantValue]  //DS_________________________ dquant LastTransQuant CirBuf [  292] // No reverting => LastTransQuant <- qant
    
//            (cb.MBCodingType == MBCT_MPEG4_INTER16X16) &&
       VLD8W_3           vr00,[i4, MPO_MBCodingType]  //DS______________________________________ mbtype CirBuf [  295] 
       VIA               i3, i3, MBCT_MPEG4_INTER16X16  //DS____________________________________ mbtype mbtype [  296] 
       VJP!i3            r3                                                                     // mbtype exit [  297] 
       VLD8W_2!i3        vr00,[i4, MPO_QuantValue]  //DS_________________________ mbtype LastTransQuant CirBuf [  298] // No reverting => LastTransQuant <- qant

//            (cbp == 0) &&
       VLD16_7           vr00,[i4, MPO_NonZeroCoeffLuma]  //DS_____________________________________ cbp CirBuf [  301] 
       VIA               i7, i7, 0xfc0  //DS__________________________________________________________ cbp cbp [  302] 
       VJP.i7            r3                                                                        // cbp exit [  303] 
       VLD8W_2.i7        vr00,[i4, MPO_QuantValue]  //DS____________________________ cbp LastTransQuant CirBuf [  304] // No reverting => LastTransQuant <- qant

//            (mv[0].mvx == 0) && (mv[0].mvy == 0) )            
       VLD16_1           vr01,[i4, MPO_mv]  //DS___________________________________________________ mv0 CirBuf [  307] 
       VNOP                //DS_______________________________________________________________________________ [  308] 
       VJP.i9            r3                                                                        // mv0 exit [  309] 
       VLD8W_2.i9        vr00,[i4, MPO_QuantValue]  //DS____________________________ mv0 LastTransQuant CirBuf [  310] // No reverting => LastTransQuant <- qant
       VNOP                //DS_______________________________________________________________________________ [  311] 
    
//             Not transmitting the quant so we have to use the old one
//             as that's what the decoder will do
//             cb.QuantValue = m_LastTransmittedQuant[cb.WorkArea.SliceGroup];
//             cb.MBCodingType = MBCT_H264_SKIPPED;
       VMOVW             vr02, MBCT_MPEG4_SKIPPED, 1  //DS________________________________________________ skp [  317] 
       VMOVW             vr00, 0, 2                                                                  // dquant [  318] 
       VST8_2            vr00,[i4, MPO_QuantValue]                                    // LastTransQuant CirBuf [  319] 
       VST8              vr02,[i4, MPO_MBCodingType]                                             // skp CirBuf [  320] 
    
//exit --------                                                                                             // [  322] 
 vendrec r3                                                                                                 // [  322] 
 vrec r3                                                                                                    // [  322] 
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0, [r0, 0]

    // channels can only return 16 bit scalars
LINE 327".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                    // cbIdx [  327] 
       VSEND             r0, i1, 1                                                                   // dquant [  328] 
       VSEND             r0, i2, 2                                                           // LastTransQuant [  329] 

   mov                 r1, Service_DoEntropyEncode
       VSEND             r0, r1, 63                                                                         // [  332] 
// End                                                                                                      // [  333] 
    
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz RevertToSkipped_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc RevertToSkipped                                                                                  // [  335] 



//------------------------------------------------------------------------------
// Name:          CopyMERefToSDM
// Purpose:       Copies the ME reference data from
//                main memory to the reference ME pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------
// [Macro] CopyMERefToSDM                                                                                   // [  346] 
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
  ld r1,[r17 ,20]
  vrec r1

// p16    i0  mbX                                                                                           // [  347] 
// p16    i1  mbY                                                                                           // [  348] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  349] 
// p32    k6  sdmAddr                                                                                       // [  349] 
// p32    k8  dr2Val                                                                                        // [  350] 

// Begin                                                                                                    // [  352] 
// p16    i2  dmaOutReg16                                                                                   // [  353] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  354] 
// p32    k2  dmaOutReg32                                                                                   // [  354] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 358".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, ME_REFERENCE_ARRAY_STRIDE                                        // dmaOutReg32 [  358] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  359] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  362] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  370] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  382] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  383] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  386] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  387] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  390] 
    
    
// End                                                                                                      // [  393] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyMERefToSDM                                                                                   // [  395] 


//------------------------------------------------------------------------------
// Name:          CopyMBLumaToSDM
// Purpose:       Copies the MB current luma data from
//                main memory to the MB circular buffer pixel data area in the SDM
// Arguments:     mbX     = horizontal position in picture
//                mbY     = vertical position in picture
//                dr2Val  = FrameTableIndex | VertBlkSize | HorBlkSize
// Return Value:  void
//------------------------------------------------------------------------------

// [Macro] CopyMBLumaToSDM                                                                                  // [  406] 
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
  ld r1,[r17 ,24]
  vrec r1

// p16    i0  mbX                                                                                           // [  407] 
// p16    i1  mbY                                                                                           // [  408] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [  409] 
// p32    k6  sdmAddr                                                                                       // [  409] 
// p32    k8  dr2Val                                                                                        // [  410] 

// Begin                                                                                                    // [  412] 
// p16    i2  dmaOutReg16                                                                                   // [  413] 
// ****** Informational : Register space for symbol "dmaOutReg32" already allocated.                        // [  414] 
// p32    k2  dmaOutReg32                                                                                   // [  414] 

    // this has the additional side effect of clearing the top bits of dmaOutReg32
    // dr1: sdm stride
LINE 418".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VMOV.12           vr00, PCB_CURRENT_STRIDE                                               // dmaOutReg32 [  418] 
       VDMAISET          dr1, k2                                                                // dmaOutReg32 [  419] 

    // dr0: sdm address
       VDMAISET          dr0, k6                                                                    // sdmAddr [  422] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE_Y_REF1_ADDR
       VDMAISET          dr2, k8                                                                     // dr2Val [  430] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //  [1:0] = "10" = non-interlaced clip mode
    //    [2] =  '0' = disable double linestride
    // [15:8] =  n/a = clip value
       VMOV.12           vr00, 0x2                                                              // dmaOutReg32 [  442] 
       VDMAISET          dr6, k2                                                                // dmaOutReg32 [  443] 

    // dr7: frame table base address
       VMOV.12           vr00, SDMOF_FrameTabDMA                                                // dmaOutReg32 [  446] 
       VDMAISET          dr7, k2                                                                // dmaOutReg32 [  447] 

    // start dma in
       VDMAIRUN          i0, i1                                                                     // mbX mbY [  450] 
    
    
// End                                                                                                      // [  453] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyMBLumaToSDM                                                                                  // [  455] 



// [Macro] CallBackStartME                                                                                  // [  457] 
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
  ld r1,[r17 ,28]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  459] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 465".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  465] 
   mov                 r1, Service_StartME   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  467] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackStartME                                                                                  // [  469] 



// [Macro] CallBackIntraComplete                                                                            // [  471] 
_Asm int Record_CallBackIntraComplete(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,32]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  473] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 479".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  479] 
   mov                 r1, Service_IntraComplete   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  481] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackIntraComplete                                                                            // [  483] 




// [Macro] CallBackDoFwdTransforms                                                                          // [  486] 
_Asm int Record_CallBackDoFwdTransforms(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,36]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  488] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 494".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  494] 
   mov                 r1, Service_DoFwdTransforms   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  496] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoFwdTransforms                                                                          // [  498] 




// [Macro] CallBackDoEntropyEncode                                                                          // [  501] 
_Asm int Record_CallBackDoEntropyEncode(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,40]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  503] 
// p16    i1  dquant                                                                                        // [  504] 
// p16    i2  lastQuant                                                                                     // [  505] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP00ToArc
   ld                  r0,[r0,0]
LINE 513".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  513] 
       VSEND             r0, i1, 1                                                                   // dquant [  514] 
       VSEND             r0, i2, 2                                                                // lastQuant [  515] 
   mov                 r1, Service_DoEntropyEncode   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  517] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackDoEntropyEncode                                                                          // [  519] 




// [Macro] CallBackStartDeblock                                                                             // [  522] 
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
  ld r1,[r17 ,44]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  524] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP01ToArc
   ld                  r0,[r0,0]
LINE 530".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  530] 
   mov                 r1, Service_StartDeblock   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  532] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackStartDeblock                                                                             // [  534] 



// [Macro] CallBackReleaseBuffer                                                                            // [  536] 
_Asm int Record_CallBackReleaseBuffer(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,48]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  538] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP01ToArc
   ld                  r0,[r0,0]
LINE 544".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  544] 
   mov                 r1, Service_ReleaseBuffer   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  546] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackReleaseBuffer                                                                            // [  548] 




// [Macro] CallBackWriteOutMB                                                                               // [  551] 
_Asm int Record_CallBackWriteOutMB(int *mac_tab)
{
 %reg mac_tab
.off percent_reg
  push r13
  push r14
  push r15
  push r16
  push r17
  mov r17, mac_tab
  ld r1,[r17 ,52]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [  553] 
    // End params -------
    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP01ToArc
   ld                  r0,[r0,0]
LINE 559".\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [  559] 
   mov                 r1, Service_CallBackWriteOutMB   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [  561] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackWriteOutMB                                                                               // [  563] 




const int MACRO_IDX_CopyCurrentChromaToSDM = 0;
const int MACRO_IDX_CopyChromaPredToSDM = 1;
const int MACRO_IDX_OutputMacroblock = 2;
const int MACRO_IDX_SdmDmaIn = 3;
const int MACRO_IDX_RevertToSkipped = 4;
const int MACRO_IDX_CopyMERefToSDM = 5;
const int MACRO_IDX_CopyMBLumaToSDM = 6;
const int MACRO_IDX_CallBackStartME = 7;
const int MACRO_IDX_CallBackIntraComplete = 8;
const int MACRO_IDX_CallBackDoFwdTransforms = 9;
const int MACRO_IDX_CallBackDoEntropyEncode = 10;
const int MACRO_IDX_CallBackStartDeblock = 11;
const int MACRO_IDX_CallBackReleaseBuffer = 12;
const int MACRO_IDX_CallBackWriteOutMB = 13;

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

    map[MACRO_IDX_OutputMacroblock] = sa;
    sa = Record_OutputMacroblock(map);

    map[MACRO_IDX_SdmDmaIn] = sa;
    sa = Record_SdmDmaIn(map);

    map[MACRO_IDX_RevertToSkipped] = sa;
    sa = Record_RevertToSkipped(map);

    map[MACRO_IDX_CopyMERefToSDM] = sa;
    sa = Record_CopyMERefToSDM(map);

    map[MACRO_IDX_CopyMBLumaToSDM] = sa;
    sa = Record_CopyMBLumaToSDM(map);

    map[MACRO_IDX_CallBackStartME] = sa;
    sa = Record_CallBackStartME(map);

    map[MACRO_IDX_CallBackIntraComplete] = sa;
    sa = Record_CallBackIntraComplete(map);

    map[MACRO_IDX_CallBackDoFwdTransforms] = sa;
    sa = Record_CallBackDoFwdTransforms(map);

    map[MACRO_IDX_CallBackDoEntropyEncode] = sa;
    sa = Record_CallBackDoEntropyEncode(map);

    map[MACRO_IDX_CallBackStartDeblock] = sa;
    sa = Record_CallBackStartDeblock(map);

    map[MACRO_IDX_CallBackReleaseBuffer] = sa;
    sa = Record_CallBackReleaseBuffer(map);

    map[MACRO_IDX_CallBackWriteOutMB] = sa;
    sa = Record_CallBackWriteOutMB(map);

    return sa;
}
