//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcDeblock_MPA.sss'

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


 // showstalls
// MetaWare ouput ON                                                                                        // [   20] 
#define LINE #line                                                                                          // [   20] 

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

#include ".\ARCEncoder\H264\ArcDeblock_MPA.h"


// Macro table
    int Deblock[12];
    int Deblock_Record(int sa, int SIMDnum);

#include "ArcMPC.h"
#include "ArcSDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"

#define BigButSafe 2000 // nice big number that won't overflow anything

//----------------------------------------------------------------------------------------------------
// Deblock register workspace
// s16    vr02.1  chromaQp                                                                                  // [   36] 
// s16    vr03.1  ChromaQ_IdxOffset                                                                         // [   37] 
// s16    vr02.2  BlkIntra                                                                                  // [   38] // 0xfff when current block  intra
// s16    vr02.4  LeftIntra                                                                                 // [   39] // 0xfff when left block  intra
// s16    vr02.8  AboveIntra                                                                                // [   40] // 0xfff when above block  intra
// s16    vr02.16  InternalYab                                                                              // [   41] 
// s16    vr02.32  InternalUVab                                                                             // [   42] 
// s16    vr02.64  lumaNzCoef                                                                               // [   43] 
//vec16  vr04  filStrength                                                                                  // [   44] // 4 x Filter strengths
//vec16  vr05  initFilStrength                                                                              // [   45] 
//vec16  vr06  abIdxsY                                                                                      // [   46] // Filter params luma
//vec16  vr07  abIdxsUV                                                                                     // [   47] // Filter params chroma
// p16    i14  leftDbValid                                                                                  // [   48] // Same slice & ....
// p16    i15  aboveDbValid                                                                                 // [   49] // Same slice & ....
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Begin                                                                                                    // [   53] 
// Horizontal deblock common vars
//vec16  vr08  strongFilter                                                                                 // [   55] // 4 x strong filter flags


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Is Macro
// Init members of MPC circular buffer
// Sets CirBuf
// Sets FixBuf
// Sets CirLeft
// [Macro] SetCurrentMPC1                                                                                   // [   65] 
_Asm int Record_SetCurrentMPC1(int *mac_tab)
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

    // Params -----------
// p16    i0  cIdx                                                                                          // [   67] // Circular buffer index
// p16    i1  Column                                                                                        // [   68] // MacroBlock column
// p16    i2  leftIdx                                                                                       // [   69] // Circular buffer index of macro block to left
    // End params -------
// Setup pointers
LINE 73".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMULW.5           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY               // cIdx'leftIdx cIdx'leftIdx [   73] 
       VMULW.2           vr00, vr00, MPO_SIZE_OF_FIXED_ENTRY                                  // Column Column [   74] 
       VIM               i4, i0, SDMOF_CircularBuffs                                            // CirBuf cIdx [   75] 
       VIM               i5, i1, SDMOF_FixedBuffs                                             // FixBuf Column [   76] 
       VIM               i6, i2, SDMOF_CircularBuffs                                        // CirLeft leftIdx [   77] 
    
    
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc SetCurrentMPC1                                                                                   // [   80] 




//----------------------------------------------------------------------------------------------------
// Is Macro
// Moves MPC circular buffer members from circular buffer up to fixed
// Assumes current MPC is set

// [Macro] CopyToFixedAfterReconstructH264                                                                  // [   88] // function moved here
_Asm int Record_CopyToFixedAfterReconstructH264(int *mac_tab)
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

//vec16  vr09  luma                                                                                         // [   89] 
//vec16  vr10  chromaU                                                                                      // [   89] 
//vec16  vr11  chromaV                                                                                      // [   89] 
LINE 90".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD128            vr09,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+19*PCB_RECONSTRUCTED_STRIDE+8] // luma CirBuf [   90] 
       VLD32             vr10,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*11+4] // chromaU CirBuf [   91] 
       VLD32_2           vr10,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*11+4+4] // chromaU CirBuf [   92] 
       VLD32             vr11,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*11+4] // chromaV CirBuf [   93] 
       VLD32_2           vr11,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*11+4+4] // chromaV CirBuf [   94] 

       VST128            vr09,[i5, MPO_Y_IntraPredPixel]                                        // luma FixBuf [   96] 
       VST64             vr10,[i5, MPO_U_IntraPredPixel]                                     // chromaU FixBuf [   97] 
       VST64             vr11,[i5, MPO_V_IntraPredPixel]                                     // chromaV FixBuf [   98] 

// Y and U/V coeffs
// s32    vr03.12  lcf                                                                                      // [  101] 
// s32    vr03.48  ccf                                                                                      // [  101] 
LINE 102".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD32_2           vr03,[i4, MPO_CoeffCountSB+8]                                           // lcf CirBuf [  102] 
       VLD32_4           vr03,[i4, MPO_CoeffCountSB+12]                                          // ccf CirBuf [  103] 
       VST32_2           vr03,[i5, MPO_Intra4x4ModeCoeffCountSB+8]                               // lcf FixBuf [  104] 
       VST32_4           vr03,[i5, MPO_Intra4x4ModeCoeffCountSB+12]                              // ccf FixBuf [  105] 
    
  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyToFixedAfterReconstructH264                                                                  // [  107] // CopyToFixedAfterReconstructH264





//----------------------------------------------------------------------------------------------------
// Is Macro
// Assumes current MPC is setup
// General setup coded required per macro block

// [Macro] DeblockMacroBlockSetup                                                                           // [  116] 
_Asm int Record_DeblockMacroBlockSetup(int *mac_tab)
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
DeblockMacroBlockSetup_RecLoop:
  ld r1,[r17 ,8]
  vrec r1

    // Params -----------
// p16    i0  chromaQIO                                                                                     // [  118] // MgrMPC.Deblock.ChromaQ_IdxOffset
// p16    i1  DeblockMode                                                                                   // [  119] // MgrMPC.Deblock.Mode
    
    // End params -------
// Begin                                                                                                    // [  123] 
// s16    vr09.1  QuantValue                                                                                // [  124] 
    

LINE 127".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr03, vr00, 1                                          // ChromaQ_IdxOffset chromaQIO [  127] 
       VLD8W             vr09,[i4, MPO_QuantValue]                                        // QuantValue CirBuf [  128] 
       VADDW.1           vr02, vr09, i0                                       // chromaQp QuantValue chromaQIO [  129] 
       VADDW.1           vr00, vr02, 16+SDMOF_h264_quantiser_chroma_mapping_tableClipping // chromaQIO chromaQp [  130] 
       VLD8W             vr02,[i0, 0]                                                    // chromaQp chromaQIO [  131] 



// Test neighbours for being intra
       VLD8W_1           vr02,[i4, MPO_MBCodingType]                                        // BlkIntra CirBuf [  136] 
       VLD8W_2           vr02,[i6, MPO_MBCodingType]                                      // LeftIntra CirLeft [  137] 
       VLD8W_3           vr02,[i5, MPO_MBCodingType]                                      // AboveIntra FixBuf [  138] 
       VMOVW             vr01, 0, 192                                              // leftDbValid'aboveDbValid [  139] 
       VAND.14           vr02, vr02, MBCT_H264_INTRA // BlkIntra'LeftIntra'AboveIntra BlkIntra'LeftIntra'AboveIntra [  140] 
       VNEW.14           vr02, vr02, vr62 // BlkIntra'LeftIntra'AboveIntra BlkIntra'LeftIntra'AboveIntra VZERO [  141] 

// Calculate the luma and chroma a and b indices for centre, left and top edges
// Centre edges, depends only on this macroblock's quant
        VMR4W.16        vr02, vr09, vr09                                             // InternalYab QuantValue [  145] 
        VMR3W.32        vr02, vr02, vr02                                              // InternalUVab chromaQp [  146] 
LINE 147".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VSUBW.48          vr02, vr02, 15                   // InternalYab'InternalUVab InternalYab'InternalUVab [  147] 
       VMINW.48          vr02, vr02, 36                   // InternalYab'InternalUVab InternalYab'InternalUVab [  148] 
       VMVW              vr08, vr62, 255                                                 // strongFilter VZERO [  149] 
       VMAXW.48          vr02, vr02, 0                    // InternalYab'InternalUVab InternalYab'InternalUVab [  150] 



    // Pre-process left neighbours into 16bit mask
// s16    vr03.64  lumcTmp                                                                                  // [  155] 
// s16    vr09.64  lumcMsk                                                                                  // [  155] 
LINE 156".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD16_6           vr02,[i4, MPO_NonZeroCoeffLuma]                                  // lumaNzCoef CirBuf [  156] 
       VMOVW             vr09, 0x7777, 64                                                           // lumcMsk [  157] // vband can only do 15 bits
       VAND.64           vr03, vr02, vr09                                        // lumcTmp lumaNzCoef lumcMsk [  158] 
       VADDW.64          vr03, vr03, vr03                                           // lumcTmp lumcTmp lumcTmp [  159] // lumaNzCoef=  lumaNzCoef | lumcTmp + lumcTmp
       VOR.64            vr02, vr02, vr03                                     // lumaNzCoef lumaNzCoef lumcTmp [  160] 

// p16    i2  isIntra                                                                                       // [  162] 
        VMR7W.4         vr00, vr02, vr02                                                   // isIntra BlkIntra [  163] 
// p16    i8  leftBad                                                                                       // [  164] 
LINE 165".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W             vr01,[i4, MPO_LeftMB]                                               // leftBad CirBuf [  165] 
       VMOVW.i2          vr04, 3                                                        // isIntra filStrength [  166] 
       VMOVW!i2          vr04, 0                                                        // isIntra filStrength [  167] 
       VAND.4            vr00, vr00, 1                                                      // isIntra isIntra [  168] // Convert to 1 or zero
       VMOVW.i2          vr08, 1                                                       // isIntra strongFilter [  169] 

       VAND.1            vr01, vr01, MBNF_VALID                                             // leftBad leftBad [  171] 
       VJP!i8            r3                                                                  // leftBad NoLeft [  172] 
       VEQW.1            vr01, vr01, vr62  //DS_________________________________________ leftBad leftBad VZERO [  173] // 0xffff if not ok
       VOR.1             vr04, vr04, vr01  //DS_______________________________ filStrength filStrength leftBad [  174] 

// s16    vr10.1  avgLumaQ  //DS______________________________________________________________________________ [  176] // Average luma quant,  Average chroma quant
// s16    vr11.1  avgChromQ  //DS_____________________________________________________________________________ [  176] 

LINE 178".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W             vr10,[i6, MPO_QuantValue]  //DS_____________________________________ avgLumaQ CirLeft [  178] 
       VADDW.1           vr11, vr10, vr03                              // avgChromQ avgLumaQ ChromaQ_IdxOffset [  179] 
       VADDW.1           vr10, vr09, vr10                                      // avgLumaQ QuantValue avgLumaQ [  180] 
       VASRRW.1          vr10, vr10, 1                                                    // avgLumaQ avgLumaQ [  181] 
// p16    i3  Temp                                                                                          // [  182] 
        VMR5W.8         vr00, vr11, vr11                                                     // Temp avgChromQ [  183] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|            |     CirLeft|      FixBuf|      CirBuf|        Temp|     isIntra| DeblockMode|   chromaQIO|
//|VR 1|aboveDbValid| leftDbValid|            |            |            |            |            |     leftBad|
//|VR 2|            |  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |     lumcTmp|            |            |            |            |            | ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                        strongFilter                                                   |
//|VR 9|            |     lumcMsk|            |            |            |            |            |  QuantValue|
//|VR10|            |            |            |            |            |            |            |    avgLumaQ|
//|VR11|            |            |            |            |            |            |            |   avgChromQ|
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 185".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VADDW.8           vr00, vr00, 16+SDMOF_h264_quantiser_chroma_mapping_tableClipping         // Temp Temp [  185] 
       VLD8W             vr11,[i3, 0]                                                        // avgChromQ Temp [  186] 
    // Pre-process left neighbours into 16bit mask
       VLD16_6           vr03,[i6, MPO_NonZeroCoeffLuma]                                    // lumcTmp CirLeft [  188] 
       VADDW.1           vr11, vr11, vr02                                      // avgChromQ avgChromQ chromaQp [  189] 
       VMOVW             vr09, 0x1111, 64                                                           // lumcMsk [  190] 
       VASRRW.1          vr11, vr11, 1                                                  // avgChromQ avgChromQ [  191] 
       VLSRW.64          vr03, vr03, 3                                                      // lumcTmp lumcTmp [  192] 
       VSUBW.1           vr10, vr10, 15                                                   // avgLumaQ avgLumaQ [  193] //CLIP_FOR_FILTER(average_luma_quant - 15)
       VAND.64           vr03, vr03, vr09                                           // lumcTmp lumcTmp lumcMsk [  194] 
       VSUBW.1           vr11, vr11, 15                                                 // avgChromQ avgChromQ [  195] // CLIP_FOR_FILTER(average_chroma_quant - 15)
       VOR.64            vr02, vr02, vr03                                     // lumaNzCoef lumaNzCoef lumcTmp [  196] 
       VMINW.1           vr10, vr10, 36                                                   // avgLumaQ avgLumaQ [  197] 
       VMINW.1           vr11, vr11, 36                                                 // avgChromQ avgChromQ [  198] 
       VMAXW.1           vr10, vr10, 0                                                    // avgLumaQ avgLumaQ [  199] 
       VMAXW.1           vr11, vr11, 0                                                  // avgChromQ avgChromQ [  200] 
       VMVW              vr06, vr10, 1                                                     // abIdxsY avgLumaQ [  201] // abIdxsY[0]
       VMVW              vr07, vr11, 1                                                   // abIdxsUV avgChromQ [  202] // abIdxsUV[0]

// End                                                                                                      // [  204] 

// Begin                                                                                                    // [  206] 
// s16    vr09.1  LeftCoID                                                                                  // [  207] 
// s16    vr10.1  CurCoID                                                                                   // [  208] 
// s16    vr11.1  mTmp                                                                                      // [  208] 
        VMR1W.1         vr11, vr00, vr00                                                   // mTmp DeblockMode [  209] 
LINE 210".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W             vr09,[i6, MPO_CollectionID]                                       // LeftCoID CirLeft [  210] 
       VLD8W             vr10,[i4, MPO_CollectionID]                                         // CurCoID CirBuf [  211] 
       VCLRSTK                                                                                              // [  212] 
       VSEQW.1           vr11, 2                                                                       // mTmp [  213] 
       VSNEW.1.S         vr09, vr10                                                        // LeftCoID CurCoID [  214] 
       VMOVW.1.S         vr04, -1                                                               // filStrength [  215] 
        VMR2W.1         vr11, vr02, vr02                                                     // mTmp LeftIntra [  216] 
LINE 217".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VTALL.1.U         i14, 0xffff                                                            // leftDbValid [  217] 
       VSUBW.1.U.F       vr62, vr11, 0                                                           // VZERO mTmp [  218] 
       VMOVW.1.U.NZ      vr08, 1                                                               // strongFilter [  219] 
       VMOVW.1.U.NZ      vr04, 3                                                                // filStrength [  220] 
// End                                                                                                      // [  221] 

//NoLeft --------                                                                                           // [  223] 
 vendrec r3                                                                                                 // [  223] 
 vrec r3                                                                                                    // [  223] 

// Begin                                                                                                    // [  225] // Above valid for deblock
// s16    vr02.128  aboveID                                                                                 // [  226] 
// s16    vr03.128  CurCoID                                                                                 // [  226] 
// s16    vr09.128  mTmp                                                                                    // [  226] 

LINE 228".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W_7           vr01,[i4, MPO_AboveMB]                                         // aboveDbValid CirBuf [  228] 
       VLD8W_7           vr02,[i5, MPO_CollectionID]                                         // aboveID FixBuf [  229] 
       VLD8W_7           vr03,[i4, MPO_CollectionID]                                         // CurCoID CirBuf [  230] 
        VMR2W.128       vr09, vr00, vr00                                                   // mTmp DeblockMode [  231] 
LINE 232".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VAND.128          vr01, vr01, MBNF_VALID                                   // aboveDbValid aboveDbValid [  232] 
       VSNEW.128         vr09, 2                                                                       // mTmp [  233] // mode != 2 || aboveID == CurCoID
       VXSUMW.14         vr06, vr02, 16                                                 // abIdxsY InternalYab [  234] 
       VSEQW.128.U       vr02, vr03                                                         // aboveID CurCoID [  235] 
       VSNEW.128.S       vr01, 0                                                               // aboveDbValid [  236] 
       VXSUMW.14         vr07, vr02, 32                                               // abIdxsUV InternalUVab [  237] 
       VTALL.128.S       i15, 0xffff                                                           // aboveDbValid [  238] 
 
       VMVW              vr05, vr04, 255                                        // initFilStrength filStrength [  240] 

// End                                                                                                      // [  242] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz DeblockMacroBlockSetup_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc DeblockMacroBlockSetup                                                                           // [  244] 









//----------------------------------------------------------------------------------------------------
// Is Macro
// Assumes current MPC is setup
// Copies pixel data from above & left buffers ready for deblock
// [Macro] DeblockFetchPixelsFromNeigbours                                                                  // [  256] 
_Asm int Record_DeblockFetchPixelsFromNeigbours(int *mac_tab)
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
DeblockFetchPixelsFromNeigbours_RecLoop:
  ld r1,[r17 ,12]
  vrec r1

    // Params -----------
    // End params -------
// Test if above & left exists
// p16    i0  AboveFlags                                                                                    // [  260] 
// p16    i1  LeftFlags                                                                                     // [  260] 
//vec16  vr09  IsValid                                                                                      // [  261] 

LINE 263".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W_1           vr00,[i4, MPO_LeftMB]                                             // LeftFlags CirBuf [  263] 
       VLD8W             vr00,[i4, MPO_AboveMB]                                           // AboveFlags CirBuf [  264] 
       VAND.3            vr00, vr00, MBNF_VALID                   // AboveFlags'LeftFlags AboveFlags'LeftFlags [  265] // above, left exist?
       VJP!i0            r3                                                              // AboveFlags noAbove [  266] 

// Begin   //DS_______________________________________________________________________________________________ [  268] 
// Copy 4 pix high block from fixed to top of current
// Luma
//vec16  vr10  lumaL  //DS____________________________________________________________________________________ [  271] 
//vec16  vr11  lumaH  //DS____________________________________________________________________________________ [  271] 
// p16    i2  fixPtr  //DS____________________________________________________________________________________ [  272] 
// p16    i3  lcnt  //DS______________________________________________________________________________________ [  272] 
// p16    i7  cirPtr  //DS____________________________________________________________________________________ [  272] 

LINE 274".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i2, i5, 0  //DS________________________________________________________ fixPtr FixBuf [  274] 
       VIM               i7, i4, 0  //DS________________________________________________________ cirPtr CirBuf [  275] 
       VMOVW             vr00, 4-1, 8  //DS______________________________________________________________ lcnt [  276] // Do 4 times
//aboveLoop --------                                                                                        // [  277] 
 vendrec r4                                                                                                 // [  277] 
 vrec r4                                                                                                    // [  277] 
LINE 278".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD64             vr10,[i2, MPO_Y_HorizPixelStore0]                                     // lumaL fixPtr [  278] 
       VLD64             vr11,[i2, MPO_Y_HorizPixelStore0+8]                                   // lumaH fixPtr [  279] 
       VIM               i2, i2, 16                                                           // fixPtr fixPtr [  280] 
       VJD.i3            i3, r4                                                         // lcnt lcnt aboveLoop [  281] 
       VST64             vr10,[i7, MPO_PixelCoeffBuffer+8]  //DS_________________________________ lumaL cirPtr [  282] 
       VST64             vr11,[i7, MPO_PixelCoeffBuffer+8+8]  //DS_______________________________ lumaH cirPtr [  283] 
       VIM               i7, i7, PCB_RECONSTRUCTED_STRIDE  //DS_________________________________ cirPtr cirPtr [  284] 
// Chroma
//vec16  vr12  chromaU0                                                                                     // [  286] 
//vec16  vr13  chromaU1                                                                                     // [  286] 
//vec16  vr14  chromaV0                                                                                     // [  286] 
//vec16  vr15  chromaV1                                                                                     // [  286] 

LINE 288".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD64             vr12,[i5, MPO_U_HorizPixelStore0]                                  // chromaU0 FixBuf [  288] 
       VLD64             vr13,[i5, MPO_U_HorizPixelStore1]                                  // chromaU1 FixBuf [  289] 
       VLD64             vr14,[i5, MPO_V_HorizPixelStore0]                                  // chromaV0 FixBuf [  290] 
       VLD64             vr15,[i5, MPO_V_HorizPixelStore1]                                  // chromaV1 FixBuf [  291] 

       VST32             vr12,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(2)+4] // chromaU0 CirBuf [  293] 
       VST32_2           vr12,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(2)+4+4] // chromaU0 CirBuf [  294] 
       VST32             vr13,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(3)+4] // chromaU1 CirBuf [  295] 
       VST32_2           vr13,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(3)+4+4] // chromaU1 CirBuf [  296] 
       VST32             vr14,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(2)+4] // chromaV0 CirBuf [  297] 
       VST32_2           vr14,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(2)+4+4] // chromaV0 CirBuf [  298] 
       VST32             vr15,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(3)+4] // chromaV1 CirBuf [  299] 
       VST32_2           vr15,[i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(3)+4+4] // chromaV1 CirBuf [  300] 
// End                                                                                                      // [  301] 

//noAbove --------                                                                                          // [  303] 
 vendrec r3                                                                                                 // [  303] 
 vrec r3                                                                                                    // [  303] 
LINE 304".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJP!i1            r5                                                                // LeftFlags noLeft [  304] 

// Begin   //DS_______________________________________________________________________________________________ [  306] 
// Copy 4 pix wide luma from right edge of block to left
// to left edge of current
// Luma
// s32    vr03.12  luma  //DS_________________________________________________________________________________ [  310] 
// p16    i2  left  //DS______________________________________________________________________________________ [  311] 
// p16    i3  current  //DS___________________________________________________________________________________ [  311] 
// p16    i7  offset  //DS____________________________________________________________________________________ [  311] 
// p16    i8  lCount  //DS____________________________________________________________________________________ [  311] 

LINE 313".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMOVW             vr01, 20-1, 1  //DS___________________________________________________________ lCount [  313] // 20 times
       VIM               i2, i6, MPO_PixelCoeffBuffer+20  //DS___________________________________ left CirLeft [  314] 
       VIM               i3, i4, MPO_PixelCoeffBuffer+4-PCB_RECONSTRUCTED_STRIDE  //DS_________ current CirBuf [  315] 
//leftLoopLuma --------                                                                                     // [  316] 
 vendrec r6                                                                                                 // [  316] 
 vrec r6                                                                                                    // [  316] 
LINE 317".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJD.i8            i8, r6                                                  // lCount lCount leftLoopLuma [  317] 
       VLD32_2           vr03,[i2, 0]  //DS_________________________________________________________ luma left [  318] 
       VADDW.12          vr00, vr00, PCB_RECONSTRUCTED_STRIDE  //DS_________________ left'current left'current [  319] 
       VST32_2           vr03,[i3, 0]  //DS______________________________________________________ luma current [  320] 

// Chroma
// s16    vr02.128  chromaU                                                                                 // [  323] 
// s16    vr03.2  chromaV                                                                                   // [  323] 
LINE 324".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMOVW             vr01, 12-1, 1                                                               // lCount [  324] // 12 times
       VIM               i2, i6, MPO_PixelCoeffBuffer                                          // left CirLeft [  325] 
       VIM               i3, i4, MPO_PixelCoeffBuffer-PCB_RECONSTRUCTED_STRIDE               // current CirBuf [  326] 
//leftLoopChroma --------                                                                                   // [  327] 
 vendrec r7                                                                                                 // [  327] 
 vrec r7                                                                                                    // [  327] 
LINE 328".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD16_7           vr02,[i2, PCB_RECONSTRUCTED_U+10]                                     // chromaU left [  328] 
       VLD16_1           vr03,[i2, PCB_RECONSTRUCTED_V+10]                                     // chromaV left [  329] 
       VJD.i8            i8, r7                                                // lCount lCount leftLoopChroma [  330] 
       VADDW.12          vr00, vr00, PCB_RECONSTRUCTED_STRIDE  //DS_________________ left'current left'current [  331] 
       VST16_7           vr02,[i3, PCB_RECONSTRUCTED_U+2]  //DS_______________________________ chromaU current [  332] 
       VST16_1           vr03,[i3, PCB_RECONSTRUCTED_V+2]  //DS_______________________________ chromaV current [  333] 
// End                                                                                                      // [  334] 

//noLeft --------                                                                                           // [  336] 
 vendrec r5                                                                                                 // [  336] 
 vrec r5                                                                                                    // [  336] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz DeblockFetchPixelsFromNeigbours_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc DeblockFetchPixelsFromNeigbours                                                                  // [  338] 





//----------------------------------------------------------------------------------------------------
// Is Macro
// Assumes current MPC is setup
// Horizonal deblock edges
// [Macro] HorizontalDeblock                                                                                // [  346] 
_Asm int Record_HorizontalDeblock(int *mac_tab)
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
HorizontalDeblock_RecLoop:
  ld r1,[r17 ,16]
  vrec r1

    // Params -----------
    // End params -------

// p16    i0  lumaPtr                                                                                       // [  350] 
// p16    i1  chromaPtr                                                                                     // [  350] 

LINE 352".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i0, i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+8-4 // lumaPtr CirBuf [  352] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*4 // chromaPtr CirBuf [  353] 


// Build a 4 entry jump vector (in lumaJmp, chromaJmp) & deblock parrams for inner loop SIMD style

// Loop over each row of sub macroblocks
// p16    i2  c8x8                                                                                          // [  359] 
// p16    i3  l8x8                                                                                          // [  359] 
// p16    i7  row                                                                                           // [  360] 
//vec16  vr09  mvXY                                                                                         // [  361] 
//vec16  vr10  CmvXY                                                                                        // [  361] 
//vec16  vr11  mvd                                                                                          // [  361] 
//vec16  vr12  const4                                                                                       // [  362] 
// p16    i8  coefMsk                                                                                       // [  363] 
LINE 364".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMOVW             vr12, 4, 255                                                                // const4 [  364] 
       VMOVW             vr00, 0, 128                                                                   // row [  365] 
       VMVW              vr04, vr05, 255                                        // filStrength initFilStrength [  366] 
       VMOVW             vr04, -1, 240                                                          // filStrength [  367] 
        VMR6W.1         vr01, vr02, vr02                                                 // coefMsk lumaNzCoef [  368] 

//HorRowLoop --------                                                                                       // [  370] 
 vendrec r3                                                                                                 // [  370] 
 vrec r3                                                                                                    // [  370] 
    // Sticky flags hold mv diff >= 4 flags
LINE 372".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIA               i2, i7, 1                                                                 // c8x8 row [  372] 
       VJP.i2            r4                                                                 // c8x8 NotRow0or2 [  373] 
       VMIVW.12          vr00, i7  //DS_________________________________________________________ c8x8'l8x8 row [  374] // rows 0 & 2
       VADDW.12          vr00, vr00, vr00  //DS_________________________________ c8x8'l8x8 c8x8'l8x8 c8x8'l8x8 [  375] // * sizeof MV_t (2)
       VADDW.8           vr00, vr00, i6  //DS_______________________________________________ l8x8 l8x8 CirLeft [  376] 
       VADDW.4           vr00, vr00, i4                                                    // c8x8 c8x8 CirBuf [  377] 
       VLD16_0           vr10,[i2, MPO_mv+0]                                                     // CmvXY c8x8 [  378] // .. .. .. .. .. .. ..  yx0
       VLD16_1           vr10,[i2, MPO_mv+2]                                                     // CmvXY c8x8 [  379] // .. .. .. .. .. .. yx1 yx0
       VLD16_3.i14       vr09,[i3, MPO_mv+2]                                          // leftDbValid mvXY l8x8 [  380] // yxL  ................
       VLD16_3!i14       vr09,[i2, MPO_mv+0]                                          // leftDbValid mvXY c8x8 [  381] // yx0  ................
       VMRGW             vr10, vr10, vr10                                                 // CmvXY CmvXY CmvXY [  382] //           xy1 xy1 yx0 yx0
       VUPSBW            vr09, vr09                                                               // mvXY mvXY [  383] // yL xL ...................
       VUPSBW            vr10, vr10                                                             // CmvXY CmvXY [  384] //  y1 x1 y1 x1 y0 x0 y0 x0
       VMR6W             vr09, vr10, vr09                                                   // mvXY CmvXY mvXY [  385] //  y1 x1 y0 x0 y0 x0 yL xL = [y1 x1 y1 x1 y0 x0 y0 x0 , yL xL .........]
       VDIFW             vr11, vr10, vr09                                                    // mvd CmvXY mvXY [  386] 
       VLEW              vr11, vr12, vr11                                                    // mvd const4 mvd [  387] //  4 < (x & y diffs)
       VABSW             vr11, vr11                                                                 // mvd mvd [  388] // Make pos, so packing pair to 16 bit will always be non zero when needed
       VASRP.255         vr11, vr11, 0                                                              // mvd mvd [  389] // or together by packing into 4 values
       VSNEW             vr11, 0                                                                        // mvd [  390] // .s == (xy diffs >= 4)

//NotRow0or2 --------                                                                                       // [  392] 
 vendrec r4                                                                                                 // [  392] 
 vrec r4                                                                                                    // [  392] 
//vec16  vr13  strength                                                                                     // [  393] 
//vec16  vr14  strongs                                                                                      // [  393] 
//vec16  vr15  lumaJmp                                                                                      // [  394] // Per lane jump vectors
//vec16  vr16  chromaJmp                                                                                    // [  394] 
//vec16  vr17  dbParams                                                                                     // [  395] 
//vec16  vr18  idxYs                                                                                        // [  396] 
//vec16  vr19  idxUVs                                                                                       // [  396] 
// Begin                                                                                                    // [  397] 
// p16    i9  maskNzStr                                                                                     // [  398] 
LINE 399".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VADDW.F           vr13, vr04, 0                                                 // strength filStrength [  399] // str nz?
       VMOVW             vr15, r5, 15                                                  // lumaJmp horzSkipLuma [  400] 
       VADDW.i8.Z.F      vr13, vr62, 2                                               // coefMsk strength VZERO [  401] // z & coefs
       VADDW.S.Z.F       vr13, vr62, 1                                                       // strength VZERO [  402] // z & mv diff >= 4
       VLTW.2            i9, vr62, vr13                                            // maskNzStr VZERO strength [  403] // strength gt zero mask to scalar
       VMOVW             vr15, r6, 240                                                 // lumaJmp horizRowDone [  404] // Finished when these get popped
       VJP!i9            r7                                                          // maskNzStr horizRowSkip [  405] // Nothing to do all strengths zero
       VADDW.F           vr18, vr06, vr62  //DS___________________________________________ idxYs abIdxsY VZERO [  406] 
       VMOVW.i9.NZ       vr15, r8  //DS______________________________________ maskNzStr lumaJmp doHorzEdgeLuma [  407] // When strength nz &
       VADDW.F           vr19, vr07, vr62  //DS_________________________________________ idxUVs abIdxsUV VZERO [  408] 
       VSUBW             vr13, vr13, 1                                                    // strength strength [  409] // for indexing AlphaBetaCoTable[str-1][idxY]
       VMVW              vr14, vr08, 255                                               // strongs strongFilter [  410] 
       VBMULW            vr13, vr13, 4*37                                                 // strength strength [  411] // for indexing AlphaBetaCoTable[str-1][idxY]
       VBMULW            vr18, vr18, 4                                                          // idxYs idxYs [  412] 
       VBMULW            vr19, vr19, 4                                                        // idxUVs idxUVs [  413] 
       VJB               vr15, r6                                                      // lumaJmp horizRowDone [  414] // pop & jump to first luma
       VMOVW             vr16, r9, 255  //DS__________________________________________ chromaJmp horzSkipCroma [  415] // Default
       VMOVW.i9.NZ       vr16, r10  //DS_____________________________________ maskNzStr chromaJmp doHorzChroma [  416] // When strength nz && abIdxsUV[column] != 0
       VMOVW             vr16, r9, 170  //DS__________________________________________ chromaJmp horzSkipCroma [  417] // Nothing to do on odd lanes
// End                                                                                                      // [  418] 

//doHorzEdgeLuma --------                                                                                   // [  420] 
 vendrec r8                                                                                                 // [  420] 
 vrec r8                                                                                                    // [  420] 
// p16    i9  strong                                                                                        // [  421] 
// p16    i10  str                                                                                          // [  422] 
// Begin                                                                                                    // [  423] 
// p16    i11  idx                                                                                          // [  424] 
// p16    i12  unaligned                                                                                    // [  424] 
LINE 425".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VPOPW             i11, vr18, 0                                                             // idx idxYs [  425] 
       VPOPW             i10, vr13, 0                                                          // str strength [  426] // WorkAreaSDM->AlphaBetaCoTable[str-1][idxY]
       VIA               i12, i0, 7                                                       // unaligned lumaPtr [  427] 
       VADDW.8           vr01, vr01, i10                                                        // idx idx str [  428] 
       VJP.i12           r11                                              // unaligned doHorzEdgeLumaUnaligned [  429] 
       VLD32WLE          vr17,[i11, SDMOF_AlphaBetaCoTable]  //DS________________________________ dbParams idx [  430] 
       VPOPW             i9, vr14, 0  //DS_____________________________________________________ strong strongs [  431] 
       VMIVW.2           vr17, i9  //DS_______________________________________________________ dbParams strong [  432] 
// End                                                                                                      // [  433] 

// Aligned version
// Begin                                                                                                    // [  436] 
//vec16  vr20  line1                                                                                        // [  437] 
//vec16  vr21  line2                                                                                        // [  437] 
//vec16  vr22  line3                                                                                        // [  437] 
//vec16  vr23  line4                                                                                        // [  437] 
//vec16  vr24  inter1                                                                                       // [  438] 
//vec16  vr25  inter2                                                                                       // [  438] 
//vec16  vr26  inter3                                                                                       // [  438] 
//vec16  vr27  inter4                                                                                       // [  438] 
    
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|         row|     CirLeft|      FixBuf|      CirBuf|        l8x8|        c8x8|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |            |         str|      strong|     coefMsk|
//|VR 2|            |  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |            |            | ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                        strongFilter                                                   |
//|VR 9|                                                mvXY                                                   |
//|VR10|                                               CmvXY                                                   |
//|VR11|                                                 mvd                                                   |
//|VR12|                                              const4                                                   |
//|VR13|                                            strength                                                   |
//|VR14|                                             strongs                                                   |
//|VR15|                                             lumaJmp                                                   |
//|VR16|                                           chromaJmp                                                   |
//|VR17|                                            dbParams                                                   |
//|VR18|                                               idxYs                                                   |
//|VR19|                                              idxUVs                                                   |
//|VR20|                                               line1                                                   |
//|VR21|                                               line2                                                   |
//|VR22|                                               line3                                                   |
//|VR23|                                               line4                                                   |
//|VR24|                                              inter1                                                   |
//|VR25|                                              inter2                                                   |
//|VR26|                                              inter3                                                   |
//|VR27|                                              inter4                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

    
// Deblock 6x4 pixel edge aligned on 8 byte boundry
LINE 443".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD64W            vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                // line1 lumaPtr [  443] 
       VLD64W            vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1]                                // line2 lumaPtr [  444] 
       VLD64W            vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2]                                // line3 lumaPtr [  445] 
       VLD64W            vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3]                                // line4 lumaPtr [  446] 
       VH264FT           vr24, vr20, vr17                                             // inter1 line1 dbParams [  447] 
       VH264FT           vr25, vr21, vr17                                             // inter2 line2 dbParams [  448] 
       VH264FT           vr26, vr22, vr17                                             // inter3 line3 dbParams [  449] 
       VH264FT           vr27, vr23, vr17                                             // inter4 line4 dbParams [  450] 
       VH264F            vr20, vr20, vr24                                                // line1 line1 inter1 [  451] 
       VH264F            vr21, vr21, vr25                                                // line2 line2 inter2 [  452] 
       VH264F            vr22, vr22, vr26                                                // line3 line3 inter3 [  453] 
       VH264F            vr23, vr23, vr27                                                // line4 line4 inter4 [  454] 
       VST64             vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                // line1 lumaPtr [  455] 
       VJB               vr16, 0                                                                  // chromaJmp [  456] 
       VST64             vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1]  //DS____________________________ line2 lumaPtr [  457] 
       VST64             vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2]  //DS____________________________ line3 lumaPtr [  458] 
       VST64             vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3]  //DS____________________________ line4 lumaPtr [  459] 
// End                                                                                                      // [  460] 

//doHorzEdgeLumaUnaligned --------                                                                          // [  462] 
 vendrec r11                                                                                                // [  462] 
 vrec r11                                                                                                   // [  462] 

// Begin                                                                                                    // [  464] 
//vec16  vr20  line1                                                                                        // [  465] 
//vec16  vr21  line2                                                                                        // [  465] 
//vec16  vr22  line3                                                                                        // [  465] 
//vec16  vr23  line4                                                                                        // [  465] 
//vec16  vr24  inter1                                                                                       // [  466] 
//vec16  vr25  inter2                                                                                       // [  466] 
//vec16  vr26  inter3                                                                                       // [  466] 
//vec16  vr27  inter4                                                                                       // [  466] 
// Deblock 6x4 pixel edge
LINE 468".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD32WL           vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                // line1 lumaPtr [  468] 
       VLD32WH           vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0+4]                              // line1 lumaPtr [  469] 
       VLD32WL           vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1]                                // line2 lumaPtr [  470] 
       VLD32WH           vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1+4]                              // line2 lumaPtr [  471] 
       VLD32WL           vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2]                                // line3 lumaPtr [  472] 
       VLD32WH           vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2+4]                              // line3 lumaPtr [  473] 
       VLD32WL           vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3]                                // line4 lumaPtr [  474] 
       VLD32WH           vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3+4]                              // line4 lumaPtr [  475] 
       VH264FT           vr24, vr20, vr17                                             // inter1 line1 dbParams [  476] 
       VH264FT           vr25, vr21, vr17                                             // inter2 line2 dbParams [  477] 
       VH264FT           vr26, vr22, vr17                                             // inter3 line3 dbParams [  478] 
       VH264FT           vr27, vr23, vr17                                             // inter4 line4 dbParams [  479] 
       VH264F            vr20, vr20, vr24                                                // line1 line1 inter1 [  480] 
       VH264F            vr21, vr21, vr25                                                // line2 line2 inter2 [  481] 
       VH264F            vr22, vr22, vr26                                                // line3 line3 inter3 [  482] 
       VH264F            vr23, vr23, vr27                                                // line4 line4 inter4 [  483] 
       VST32             vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                // line1 lumaPtr [  484] 
       VST32             vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1]                                // line2 lumaPtr [  485] 
       VST32             vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2]                                // line3 lumaPtr [  486] 
       VST32             vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3]                                // line4 lumaPtr [  487] 
       VST32_2           vr20,[i0, PCB_RECONSTRUCTED_STRIDE*0+4]                              // line1 lumaPtr [  488] 
       VJB               vr16, 0                                                                  // chromaJmp [  489] 
       VST32_2           vr21,[i0, PCB_RECONSTRUCTED_STRIDE*1+4]  //DS__________________________ line2 lumaPtr [  490] 
       VST32_2           vr22,[i0, PCB_RECONSTRUCTED_STRIDE*2+4]  //DS__________________________ line3 lumaPtr [  491] 
       VST32_2           vr23,[i0, PCB_RECONSTRUCTED_STRIDE*3+4]  //DS__________________________ line4 lumaPtr [  492] 
// End                                                                                                      // [  493] 


//horzSkipLuma --------                                                                                     // [  496] 
 vendrec r5                                                                                                 // [  496] 
 vrec r5                                                                                                    // [  496] 
// Begin                                                                                                    // [  497] 
// p16    i11  idx                                                                                          // [  498] 
LINE 499".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJB               vr16, 0                                                                  // chromaJmp [  499] 
       VPOPW             i10, vr13, 0  //DS______________________________________________________ str strength [  500] // WorkAreaSDM->AlphaBetaCoTable[str-1][idxY]
       VPOPW             i11, vr18, 0  //DS_________________________________________________________ idx idxYs [  501] 
       VPOPW             i9, vr14, 0  //DS_____________________________________________________ strong strongs [  502] 
// End                                                                                                      // [  503] 

// Chroma horizontal deblock
// Deblock 2 pixel high edge for other U & V
//doHorzChroma --------                                                                                     // [  507] 
 vendrec r10                                                                                                // [  507] 
 vrec r10                                                                                                   // [  507] 
// Colour layout in MPC reconstructed format
//24    xxLLuuuuuuuuxxLLvvvvvvvv                L =  pixels for left block
//25    xxLLuuuuuuuuxxLLvvvvvvvv
//
//                 u           u                        v              v
//  A -> x  x  p1 p0 q0 q1 p1 p0 q0 q1 u  u  x  x  p1 p0 q0 q1  p1 p0 q0 q1 v  v
//  B -> x  x  p1 p0 q0 q1 p1 p0 q0 q1 u  u  x  x  p1 p0 q0 q1  p1 p0 q0 q1 v  v
//             -----------                         -----------
//Offset 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23

// Begin                                                                                                    // [  518] 
// p16    i11  idx                                                                                          // [  519] 
LINE 520".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VPOPW             i11, vr19, 0                                                            // idx idxUVs [  520] 
       VADDW.8           vr01, vr01, i10                                                        // idx idx str [  521] 

//vec16  vr20  p1                                                                                           // [  523] 
//vec16  vr21  p0                                                                                           // [  523] 
//vec16  vr22  q0                                                                                           // [  523] 
//vec16  vr23  q1                                                                                           // [  523] 
// Begin                                                                                                    // [  524] 
// p16    i12  alpha                                                                                        // [  525] 
// p16    i13  beta                                                                                         // [  525] 
                                                    //                  1        0
LINE 527".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD16_0           vr20,[i1, 2]                                                          // p1 chromaPtr [  527] // p1- ......       x   [Up0A Up1A]
       VLD16_0           vr21,[i1, 2+24]                                                       // p0 chromaPtr [  528] // p0- ......       x   [Up0B Up1B]
       VLD16_0           vr22,[i1, 4]                                                          // q0 chromaPtr [  529] // q0- ......       x   [Uq1A Uq0A]
       VLD16_0           vr23,[i1, 4+24]                                                       // q1 chromaPtr [  530] // q1- ......       x   [Uq1B Uq0B]

       VIM               i11, i11, SDMOF_AlphaBetaCoTable                                           // idx idx [  532] 
                                                    //                  1        0
       VLD16_1           vr20,[i1, 14]                                                         // p1 chromaPtr [  534] // p1- ......[Vp0A Vp1A] Up0A Up1A
       VLD16_1           vr21,[i1, 14+24]                                                      // p0 chromaPtr [  535] // p0- ......[Vp0B Vp1B] Up0B Up1B
       VLD16_1           vr22,[i1, 16]                                                         // q0 chromaPtr [  536] // q0- ......[Vq1A Vq0A] Uq1A Uq0A
       VLD16_1           vr23,[i1, 16+24]                                                      // q1 chromaPtr [  537] // q1- ......[Vq1B Vq0B] Uq1B Uq0B

       VLD8W_4           vr01,[i11, 3]                                                            // alpha idx [  539] 
       VLD8W_5           vr01,[i11, 2]                                                             // beta idx [  540] 
                                                    //             3     2     1     0
       VUPBW             vr20, vr20                                                                   // p1 p1 [  542] // p1- ...... Vp0A  Vp1A  Up0A  Up1A
       VUPBW             vr21, vr21                                                                   // p0 p0 [  543] // p0- ...... Vp0B  Vp1B  Up0B  Up1B
       VUPBW             vr22, vr22                                                                   // q0 q0 [  544] // q0- ...... Vq1A  Vq0A  Uq1A  Uq0A
       VUPBW             vr23, vr23                                                                   // q1 q1 [  545] // q1- ...... Vq1B  Vq0B  Uq1B  Uq0B

       VEXCH1            vr20, vr21                                                                   // p1 p0 [  547] // p1- ......[Vp1B] Vp1A [Up1B] Up1A
                                                    // q1- ...... Vp0B [Vp0A] Up0B [Up0A]

       VEXCH1            vr22, vr23                                                                   // q0 q1 [  550] // q0- ......[Vq0B] Vq0A [Uq0B] Uq0A
                                                    // q1- ...... Vq1B [Vq1A] Uq1B [Uq1A]

//if ( (ABS(q0 - p0) < alpha) && (ABS(p1 - p0) < beta) && (ABS(q1 - q0) < beta) )
//vec16  vr24  q0p0Dif                                                                                      // [  554] 
//vec16  vr25  p1p0Dif                                                                                      // [  554] 
//vec16  vr26  q1q0Dif                                                                                      // [  554] 
LINE 555".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VDIFW             vr24, vr22, vr21                                                     // q0p0Dif q0 p0 [  555] 
       VDIFW             vr25, vr20, vr21                                                     // p1p0Dif p1 p0 [  556] 
       VDIFW             vr26, vr23, vr22                                                     // q1q0Dif q1 q0 [  557] 
       VJP.i9            r12                                                        // strong horzStrongChroma [  558] 
       VSUBW.F           vr62, vr24, i12  //DS____________________________________________ VZERO q0p0Dif alpha [  559] 
       VSUBW.LT.F        vr62, vr25, i13  //DS_____________________________________________ VZERO p1p0Dif beta [  560] 
       VSUBW.LT.F        vr62, vr26, i13  //DS_____________________________________________ VZERO q1q0Dif beta [  561] 
// End                                                                                                      // [  562] 

//vec16  vr24  pp                                                                                           // [  564] 
//vec16  vr25  qq                                                                                           // [  564] 

//            int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
// Begin                                                                                                    // [  567] 
//vec16  vr26  delta                                                                                        // [  568] 
// p16    i12  c0                                                                                           // [  569] 
//vec16  vr27  c1                                                                                           // [  570] 
//vec16  vr28  mc1                                                                                          // [  570] 

// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|         row|     CirLeft|      FixBuf|      CirBuf|        l8x8|        c8x8|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |          c0|         idx|         str|      strong|     coefMsk|
//|VR 2|            |  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |            |            | ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                        strongFilter                                                   |
//|VR 9|                                                mvXY                                                   |
//|VR10|                                               CmvXY                                                   |
//|VR11|                                                 mvd                                                   |
//|VR12|                                              const4                                                   |
//|VR13|                                            strength                                                   |
//|VR14|                                             strongs                                                   |
//|VR15|                                             lumaJmp                                                   |
//|VR16|                                           chromaJmp                                                   |
//|VR17|                                            dbParams                                                   |
//|VR18|                                               idxYs                                                   |
//|VR19|                                              idxUVs                                                   |
//|VR20|                                                  p1                                                   |
//|VR21|                                                  p0                                                   |
//|VR22|                                                  q0                                                   |
//|VR23|                                                  q1                                                   |
//|VR24|                                                  pp                                                   |
//|VR25|                                                  qq                                                   |
//|VR26|                                               delta                                                   |
//|VR27|                                                  c1                                                   |
//|VR28|                                                 mc1                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

    
LINE 574".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W_4           vr01,[i11, 0]                                                               // c0 idx [  574] 
       VSUBW             vr26, vr22, vr21                                                       // delta q0 p0 [  575] 
       VMULW             vr26, vr26, 4                                                          // delta delta [  576] 
       VSUBAW            vr26, vr20, vr23                                                       // delta p1 q1 [  577] 
       VMIVW             vr27, i12                                                                    // c1 c0 [  578] 
       VASRRW            vr26, vr26, 3                                                          // delta delta [  579] 
       VADDW             vr27, vr27, 1                                                                // c1 c1 [  580] 
       VSUBW             vr28, vr62, vr27                                                      // mc1 VZERO c1 [  581] 
       VMINW             vr26, vr26, vr27                                                    // delta delta c1 [  582] 
       VMAXW             vr26, vr26, vr28                                                   // delta delta mc1 [  583] 
       VADDW             vr24, vr21, vr26                                                       // pp p0 delta [  584] 
       VJP               r13                                                                // horzStoreChroma [  585] 
       VSUBW             vr25, vr22, vr26  //DS___________________________________________________ qq q0 delta [  586] 
       VASRPWB           vr24, vr24, 0  //DS____________________________________________________________ pp pp [  587] // clip 0 - 255
       VASRPWB           vr25, vr25, 0  //DS____________________________________________________________ qq qq [  588] 
// End                                                                                                      // [  589] 

// Strong
//horzStrongChroma --------                                                                                 // [  592] 
 vendrec r12                                                                                                // [  592] 
 vrec r12                                                                                                   // [  592] 
//            P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);
//            Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
LINE 595".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VADDW             vr24, vr20, vr20                                                          // pp p1 p1 [  595] // p1 << 1
       VADDAW            vr24, vr21, vr23                                                          // pp p0 q1 [  596] // + p0 + q1
       VADDW             vr25, vr23, vr23                                                          // qq q1 q1 [  597] // q1 << 1
       VADDAW            vr25, vr22, vr20                                                          // qq q0 p1 [  598] // + q0 + p1
       VASRRPWB          vr24, vr24, 2                                                                // pp pp [  599] // + 2) >> 2
       VASRRPWB          vr25, vr25, 2                                                                // qq qq [  600] // + 2) >> 2

//horzStoreChroma --------                                                                                  // [  602] 
 vendrec r13                                                                                                // [  602] 
 vrec r13                                                                                                   // [  602] 
LINE 603".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VUPBW.LT          vr21, vr24                                                                   // p0 pp [  603] 
       VUPBW.LT          vr22, vr25                                                                   // q0 qq [  604] 
                                                    //             3     2     1     0
       VEXCH1            vr20, vr21                                                                   // p1 p0 [  606] // p1- ......[Vp0A] Vp1A [Up0A] Up1A
                                                    // p0- ...... Vp0B [Vp1B] Up0B [Up1B]

       VEXCH1            vr22, vr23                                                                   // q0 q1 [  609] // q0- ......[Vq1A] Vq0A [Uq1A] Uq0A
                                                    // q1- ...... Vq1B [Vq0B] Uq1B [Uq0B]

       VST8_1            vr20,[i1, 3]                                                          // p1 chromaPtr [  612] // Up0A
       VST8_0            vr22,[i1, 4]                                                          // q0 chromaPtr [  613] // Uq0A
       VST8_3            vr20,[i1, 15]                                                         // p1 chromaPtr [  614] // Vp0A
       VST8_2            vr22,[i1, 16]                                                         // q0 chromaPtr [  615] // Vq0A

       VST8_1            vr21,[i1, 24+3]                                                       // p0 chromaPtr [  617] // Up0B
       VJP               r14                                                                    // horzColDone [  618] 
       VST8_0            vr23,[i1, 24+4]  //DS___________________________________________________ q1 chromaPtr [  619] // Uq0B
       VST8_3            vr21,[i1, 24+15]  //DS__________________________________________________ p0 chromaPtr [  620] // Vp0B
       VST8_2            vr23,[i1, 24+16]  //DS__________________________________________________ q1 chromaPtr [  621] // Vq0B

// End                                                                                                      // [  623] 
//horzSkipCroma --------                                                                                    // [  624] 
 vendrec r9                                                                                                 // [  624] 
 vrec r9                                                                                                    // [  624] 
// Begin                                                                                                    // [  625] 
// p16    i11  idx                                                                                          // [  626] 
LINE 627".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VPOPW             i11, vr19, 0                                                            // idx idxUVs [  627] 
// End                                                                                                      // [  628] 
//horzColDone --------                                                                                      // [  629] 
 vendrec r14                                                                                                // [  629] 
 vrec r14                                                                                                   // [  629] 
LINE 630".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJB               vr15, r6                                                      // lumaJmp horizRowDone [  630] 
       VIM               i0, i0, 4  //DS______________________________________________________ lumaPtr lumaPtr [  631] 
       VADDW.2           vr00, vr00, 2  //DS______________________________________________ chromaPtr chromaPtr [  632] 
       VNOP                //DS_______________________________________________________________________________ [  633] 

//horizRowSkip --------                                                                                     // [  635] 
 vendrec r7                                                                                                 // [  635] 
 vrec r7                                                                                                    // [  635] 
LINE 636".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i0, i0, 4*4                                                        // lumaPtr lumaPtr [  636] 
       VADDW.2           vr00, vr00, 2*4                                                // chromaPtr chromaPtr [  637] 

//horizRowDone --------                                                                                     // [  639] 
 vendrec r6                                                                                                 // [  639] 
 vrec r6                                                                                                    // [  639] 
// Begin                                                                                                    // [  640] 
// p16    i11  notLastRow                                                                                   // [  641] 
LINE 642".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i11, i7, -3                                                         // notLastRow row [  642] 
       VIM               i7, i7, 1                                                                  // row row [  643] 
       VJP.i11           r3                                                           // notLastRow HorRowLoop [  644] 
       VLSRW.1           vr01, vr01, 4  //DS__________________________________________________ coefMsk coefMsk [  645] 
       VADDW.1           vr00, vr00, PCB_RECONSTRUCTED_STRIDE*4-16  //DS______________________ lumaPtr lumaPtr [  646] 
       VADDW.2           vr00, vr00, PCB_RECONSTRUCTED_STRIDE*2-8  //DS___________________ chromaPtr chromaPtr [  647] 
// End                                                                                                      // [  648] 

  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz HorizontalDeblock_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc HorizontalDeblock                                                                                // [  650] 


//----------------------------------------------------------------------------------------------------

// End of scope for horizontal deblock shared vars
// End                                                                                                      // [  654] 


//----------------------------------------------------------------------------------------------------





//----------------------------------------------------------------------------------------------------
// Is Macro
// Assumes current MPC is setup
// Vertical deblock edges
// Sends channel reply
// [Macro] VerticalDeblock                                                                                  // [  668] 
_Asm int Record_VerticalDeblock(int *mac_tab)
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
VerticalDeblock_RecLoop:
  ld r1,[r17 ,20]
  vrec r1

    // Params -----------
    // End params -------
// p16    i0  lumaPtr                                                                                       // [  671] 
// p16    i1  chromaPtr                                                                                     // [  671] 

LINE 673".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i0, i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_Y+PCB_RECONSTRUCTED_STRIDE*4+8 // lumaPtr CirBuf [  673] 
       VIM               i1, i4, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*4 // chromaPtr CirBuf [  674] 
    
// s16    vr02.128  intAbIdxsY                                                                              // [  676] 
// s16    vr03.2  intAbIdxsUV                                                                               // [  676] 
// p16    i2  lumaCoef                                                                                      // [  677] 
// s16    vr03.4  coefNz                                                                                    // [  678] 
// p16    i3  strongFilter                                                                                  // [  679] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|            |     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |            |            |            |            |
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

        // Edges depend on this sub block's and the above sub block's nz coefs
        //unsigned int coefm = dbcb.NonZeroCoeffLuma | (dbcb.NonZeroCoeffLuma << 4);
LINE 683".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD16_2           vr00,[i4, MPO_NonZeroCoeffLuma]                                    // lumaCoef CirBuf [  683] 
       VXSUMW            vr06, vr02, 16                                                 // abIdxsY InternalYab [  684] // Broadcast
       VXSUMW            vr07, vr02, 32                                               // abIdxsUV InternalUVab [  685] 
       VMULW.4           vr03, vr00, 16                                                     // coefNz lumaCoef [  686] 
       VMVW              vr02, vr06, 128                                                 // intAbIdxsY abIdxsY [  687] 
       VMVW              vr03, vr07, 2                                                 // intAbIdxsUV abIdxsUV [  688] 
       VOR.4             vr00, vr03, vr00                                          // lumaCoef coefNz lumaCoef [  689] 

//        MV_t *mvAbovePtr = dbcb.mv; // Line of mv above current sub block
// p16    i7  mvPtr                                                                                         // [  692] 
//vec16  vr08  RowJump                                                                                      // [  693] 
LINE 694".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i7, i4, MPO_mv                                                        // mvPtr CirBuf [  694] 
       VMOVW             vr04, -1, 255                                                          // filStrength [  695] 
//        // Looking up
//        if(aboveDbValid)
//        {
       VJP!i15           r3                                                            // aboveDbValid NoAbove [  699] 
       VMOVW             vr00, 0, 8  //DS________________________________________________________ strongFilter [  700] 
       VMOVW             vr08, r4, 7  //DS_________________________________________________ RowJump RowTopLoop [  701] // Jump back to top 3 times
       VMOVW             vr08, r5, 8  //DS________________________________________________ RowJump RowLoopExit [  702] 
    
// Begin                                                                                                    // [  704] 
//            int average_luma_quant = (dbcb.QuantValue + dbfb.QuantValue + 1) >> 1;
// p16    i8  avgLumaQ                                                                                      // [  706] 
// s16    vr09.1  Quant                                                                                     // [  707] 
// s16    vr10.1  abvQuant                                                                                  // [  707] 
// s16    vr11.1  avChromaQ                                                                                 // [  708] 
// s16    vr12.1  avChoTp                                                                                   // [  708] 
// p16    i9  tChPtr                                                                                        // [  709] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |            |            |      tChPtr|    avgLumaQ|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|            |            |            |            |            |            |            |       Quant|
//|VR10|            |            |            |            |            |            |            |    abvQuant|
//|VR11|            |            |            |            |            |            |            |   avChromaQ|
//|VR12|            |            |            |            |            |            |            |     avChoTp|
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 711".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W             vr09,[i4, MPO_QuantValue]                                             // Quant CirBuf [  711] 
       VLD8W             vr10,[i5, MPO_QuantValue]                                          // abvQuant FixBuf [  712] 
       VLD8W             vr11,[i5, MPO_QuantValue]                                         // avChromaQ FixBuf [  713] 
       VADDW.1           vr01, vr09, vr10                                           // avgLumaQ Quant abvQuant [  714] 
       VADDW.1           vr11, vr11, vr03                             // avChromaQ avChromaQ ChromaQ_IdxOffset [  715] 
       VASRRW.1          vr01, vr01, 1                                                    // avgLumaQ avgLumaQ [  716] 
       VMIVW             vr06, i8                                                          // abIdxsY avgLumaQ [  717] 
//            abIdxY = CLIP_FOR_FILTER(average_luma_quant - 15);
        VMR7W.2         vr01, vr11, vr11                                                   // tChPtr avChromaQ [  719] 
LINE 720".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VSUBW             vr06, vr06, 15                                                     // abIdxsY abIdxsY [  720] 
       VIM               i9, i9, 16+SDMOF_h264_quantiser_chroma_mapping_tableClipping         // tChPtr tChPtr [  721] 
       VMAXW             vr06, vr06, 0                                                      // abIdxsY abIdxsY [  722] 
       VLD8W             vr12,[i9, 0]                                                        // avChoTp tChPtr [  723] 
       VMINW             vr06, vr06, 255                                                    // abIdxsY abIdxsY [  724] 
//
//            int average_chroma_quant = 
//                (
//                    chromaQp +
//                    WorkAreaSDM->h264_quantiser_chroma_mapping_tableClipping[dbfb.QuantValue + MgrMPC.Deblock.ChromaQ_IdxOffset + 16] + 1
//                ) >> 1;

       VADDW.1           vr12, vr02, vr12                                          // avChoTp chromaQp avChoTp [  732] 
//            strong_filtering[0] = strong_filtering[1] = strong_filtering[2] = strong_filtering[3] = false;
       VMOVW             vr00, 0, 8                                                            // strongFilter [  734] 
       VASRRW.1          vr12, vr12, 1                                                      // avChoTp avChoTp [  735] 
//            abIdxUV = CLIP_FOR_FILTER(average_chroma_quant  - 15);
       VSUBW.1           vr12, vr12, 15                                                     // avChoTp avChoTp [  737] 
       VMVW              vr04, vr62, 255                                                  // filStrength VZERO [  738] 
       VMAXW.1           vr12, vr12, 0                                                      // avChoTp avChoTp [  739] 
// p16    i10  intra                                                                                        // [  740] 
LINE 741".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIV.10            i10, vr02                                                // intra AboveIntra'BlkIntra [  741] // both should be 0 or 0xffff
       VMINW.1           vr12, vr12, 255                                                    // avChoTp avChoTp [  742] 
// s16    vr09.4  abvCoef                                                                                   // [  743] 
LINE 744".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD16_2           vr09,[i5, MPO_NonZeroCoeffLuma]                                     // abvCoef FixBuf [  744] 
       VXSUMW            vr07, vr12, 1                                                     // abIdxsUV avChoTp [  745] // Broadcast
//            strength[0] = strength[1] = strength[2] = strength[3] = 0;  

//            if (block_coded_intra || aboveMB_block_coded_intra)
//            {
//                strength[0] = strength[1] = strength[2] = strength[3] = 3;  
       VMOVW.i10         vr04, 3                                                          // intra filStrength [  751] 
//                strong_filtering[0] = strong_filtering[1] = strong_filtering[2] = strong_filtering[3] = true; // Implies strong filtering is possible but not guaranteed
        VMR7W.8         vr00, vr01, vr01                                                 // strongFilter intra [  753] 
//            }
//            coefm |= (dbfb.NonZeroCoeffLuma >> 12);
LINE 756".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLSRW.4           vr09, vr09, 12                                                     // abvCoef abvCoef [  756] 
       VOR.4             vr00, vr00, vr09                                         // lumaCoef lumaCoef abvCoef [  757] 
//            mvAbovePtr = dbfb.mv + 2;
       VIM               i7, i5, MPO_mv+4                                                      // mvPtr FixBuf [  759] 
//        } // above exists
// End                                                                                                      // [  761] 

//NoAbove --------                                                                                          // [  763] 
 vendrec r3                                                                                                 // [  763] 
 vrec r3                                                                                                    // [  763] 
 
 
//vec16  vr09  mvTop                                                                                        // [  766] 
//vec16  vr10  mvMid                                                                                        // [  766] 
//vec16  vr11  mvBot                                                                                        // [  766] 
// p16    i8  evenRow                                                                                       // [  767] 
LINE 768".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMOVW             vr01, 0xffff, 1                                                            // evenRow [  768] 
       VLD32             vr09,[i7, 0]                                                           // mvTop mvPtr [  769] // Could be top line mv this block or bot line mv of above
       VLD32             vr10,[i4, MPO_mv]                                                     // mvMid CirBuf [  770] // Top line mv this block
// Begin                                                                                                    // [  771] 
LINE 772".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD32             vr11,[i4, MPO_mv+4]                                                   // mvBot CirBuf [  772] // Bottom line mv this block
// Begin                                                                                                    // [  773] 
//vec16  vr12  const4                                                                                       // [  774] 
LINE 775".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMOVW             vr12, 4, 255                                                                // const4 [  775] 
       VUPSBW            vr09, vr09                                                             // mvTop mvTop [  776] 
       VUPSBW            vr10, vr10                                                             // mvMid mvMid [  777] 
       VUPSBW            vr11, vr11                                                             // mvBot mvBot [  778] 
       VDIFW             vr09, vr09, vr10                                                 // mvTop mvTop mvMid [  779] 
       VDIFW             vr10, vr10, vr11                                                 // mvMid mvMid mvBot [  780] 
       VLEW              vr09, vr12, vr09                                                // mvTop const4 mvTop [  781] 
       VLEW              vr10, vr12, vr10                                                // mvMid const4 mvMid [  782] 
// End                                                                                                      // [  783] 
// End                                                                                                      // [  784] 
LINE 785".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMULW             vr06, vr06, 4                                                      // abIdxsY abIdxsY [  785] 
       VMULW.128         vr02, vr02, 4                                                // intAbIdxsY intAbIdxsY [  786] 
       VMULW             vr07, vr07, 4                                                    // abIdxsUV abIdxsUV [  787] 
       VMULW.2           vr03, vr03, 4                                              // intAbIdxsUV intAbIdxsUV [  788] 
 
 

//RowTopLoop --------                                                                                       // [  792] 
 vendrec r4                                                                                                 // [  792] 
 vrec r4                                                                                                    // [  792] 
    // Calc strength
LINE 794".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VADDW.F           vr62, vr04, vr62                                           // VZERO filStrength VZERO [  794] 
       VSNE              vr09, 0                                                                      // mvTop [  795] // use x & y as low high of 32 bit
       VMOVW.i2.Z.F      vr04, 2                                                       // lumaCoef filStrength [  796] 
       VMOVW.i8.S.Z.F    vr04, 1                                                        // evenRow filStrength [  797] 
//  vtall.15.z // early out from here
       VMAXW             vr04, vr04, 0                                              // filStrength filStrength [  799] 
//vec16  vr12  alpha                                                                                        // [  800] 
//vec16  vr13  beta                                                                                         // [  800] 
//vec16  vr14  c0                                                                                           // [  800] 
// Begin                                                                                                    // [  801] 
//    int alpha =             WorkAreaSDM->AlphaBetaCoTable[strength-1][index_ab][ABC0_ALPHA];
//    UNSIGNED_SEPTET beta =  WorkAreaSDM->AlphaBetaCoTable[strength-1][index_ab][ABC0_BETA];
//    int c0 =                WorkAreaSDM->AlphaBetaCoTable[strength-1][index_ab][ABC0_C0];
// p16    i9  idxY                                                                                          // [  805] 
// p16    i10  str                                                                                          // [  805] 
// p16    i11  idxUV                                                                                        // [  805] 
// Begin                                                                                                    // [  806] 
//vec16  vr15  idxs                                                                                         // [  807] 
//    vec16                   
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                idxs                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 810".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VSUBW.F           vr15, vr04, 1                                                     // idxs filStrength [  810] 
       VMULW             vr15, vr15, 4*37                                                         // idxs idxs [  811] 
       VADDW             vr06, vr15, vr06                                              // abIdxsY idxs abIdxsY [  812] 
       VADDW             vr06, vr06, SDMOF_AlphaBetaCoTable                                 // abIdxsY abIdxsY [  813] // index into alpha, beta & co table
       VADDW.GE          vr07, vr15, vr07                                            // abIdxsUV idxs abIdxsUV [  814] 
       VADDW.GE          vr07, vr07, SDMOF_AlphaBetaCoTable                               // abIdxsUV abIdxsUV [  815] // index into alpha, beta & co table
       VMVW.LT           vr07, vr62                                                          // abIdxsUV VZERO [  816] 
// End                                                                                                      // [  817] 
 
 // Do chroma first
 
  // vertical chroma
// Begin                                                                                                    // [  822] 
//vec16  vr15  p1U                                                                                          // [  823] 
//vec16  vr16  p0U                                                                                          // [  823] 
//vec16  vr17  q0U                                                                                          // [  823] 
//vec16  vr18  q1U                                                                                          // [  823] 
//vec16  vr19  p1V                                                                                          // [  824] 
//vec16  vr20  p0V                                                                                          // [  824] 
//vec16  vr21  q0V                                                                                          // [  824] 
//vec16  vr22  q1V                                                                                          // [  824] 
//vec16  vr23  macInpU                                                                                      // [  825] 
//vec16  vr24  macInpV                                                                                      // [  825] 
//vec16  vr25  dp1p0                                                                                        // [  825] 
//vec16  vr26  dq1q0                                                                                        // [  825] 
//vec16  vr27  c                                                                                            // [  825] 
//vec16  vr28  mc                                                                                           // [  825] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                 p1U                                                   |
//|VR16|                                                 p0U                                                   |
//|VR17|                                                 q0U                                                   |
//|VR18|                                                 q1U                                                   |
//|VR19|                                                 p1V                                                   |
//|VR20|                                                 p0V                                                   |
//|VR21|                                                 q0V                                                   |
//|VR22|                                                 q1V                                                   |
//|VR23|                                             macInpU                                                   |
//|VR24|                                             macInpV                                                   |
//|VR25|                                               dp1p0                                                   |
//|VR26|                                               dq1q0                                                   |
//|VR27|                                                   c                                                   |
//|VR28|                                                  mc                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 827".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJP!i8            r6                                                              // evenRow ChromaDone [  827] 
       VMVW              vr12, vr62, 255  //DS____________________________________________________ alpha VZERO [  828] 
       VMVW              vr13, vr62, 255  //DS_____________________________________________________ beta VZERO [  829] 
       VMVW              vr14, vr62, 255  //DS_______________________________________________________ c0 VZERO [  830] 
       VSWAP             vr00, vr07                                                                // abIdxsUV [  831] // Swap indxs into i0,i1,i2 & i3
       VLD8W.i0          vr12,[i0, ABC0_ALPHA]                                                        // alpha [  832] 
       VLD8W.i0          vr13,[i0, ABC0_BETA]                                                          // beta [  833] 
       VLD8W.i0          vr14,[i0, ABC0_C0]                                                              // c0 [  834] 
       VXSUMW.3          vr12, vr12, 1                                                          // alpha alpha [  835] // broadcast across lanes
       VXSUMW.3          vr13, vr13, 1                                                            // beta beta [  836] 
       VXSUMW.3          vr14, vr14, 1                                                                // c0 c0 [  837] 
       VLD8W_2.i1        vr12,[i1, ABC0_ALPHA]                                                        // alpha [  838] 
       VLD8W_2.i1        vr13,[i1, ABC0_BETA]                                                          // beta [  839] 
       VLD8W_2.i1        vr14,[i1, ABC0_C0]                                                              // c0 [  840] 
       VXSUMW.12         vr12, vr12, 4                                                          // alpha alpha [  841] // broadcast across lanes
       VXSUMW.12         vr13, vr13, 4                                                            // beta beta [  842] 
       VXSUMW.12         vr14, vr14, 4                                                                // c0 c0 [  843] 
       VLD8W_4.i2        vr12,[i2, ABC0_ALPHA]                                                        // alpha [  844] 
       VLD8W_4.i2        vr13,[i2, ABC0_BETA]                                                          // beta [  845] 
       VLD8W_4.i2        vr14,[i2, ABC0_C0]                                                              // c0 [  846] 
       VXSUMW.48         vr12, vr12, 16                                                         // alpha alpha [  847] // broadcast across lanes
       VXSUMW.48         vr13, vr13, 16                                                           // beta beta [  848] 
       VXSUMW.48         vr14, vr14, 16                                                               // c0 c0 [  849] 
       VLD8W_6.i3        vr12,[i3, ABC0_ALPHA]                                                        // alpha [  850] 
       VLD8W_6.i3        vr13,[i3, ABC0_BETA]                                                          // beta [  851] 
       VLD8W_6.i3        vr14,[i3, ABC0_C0]                                                              // c0 [  852] 
       VXSUMW.192        vr12, vr12, 64                                                         // alpha alpha [  853] // broadcast across lanes
       VXSUMW.192        vr13, vr13, 64                                                           // beta beta [  854] 
       VXSUMW.192        vr14, vr14, 64                                                               // c0 c0 [  855] 
       VSWAP             vr00, vr07                                                                // abIdxsUV [  856] // restore vr00

// Load chroma U & V 
//          int c = params[0] + 1;
       VADDW             vr27, vr14, 1                                                                 // c c0 [  860] 
       VLD32WL           vr15,[i1, PCB_RECONSTRUCTED_STRIDE*-2+4]                             // p1U chromaPtr [  861] 
       VLD32WH           vr15,[i1, PCB_RECONSTRUCTED_STRIDE*-2+8]                             // p1U chromaPtr [  862] 
       VLD32WL           vr16,[i1, PCB_RECONSTRUCTED_STRIDE*-1+4]                             // p0U chromaPtr [  863] 
       VLD32WH           vr16,[i1, PCB_RECONSTRUCTED_STRIDE*-1+8]                             // p0U chromaPtr [  864] 
    
       VLD32WL           vr17,[i1, PCB_RECONSTRUCTED_STRIDE*0+4]                              // q0U chromaPtr [  866] 
       VLD32WH           vr17,[i1, PCB_RECONSTRUCTED_STRIDE*0+8]                              // q0U chromaPtr [  867] 
    
       VLD32WL           vr18,[i1, PCB_RECONSTRUCTED_STRIDE*1+4]                              // q1U chromaPtr [  869] 
       VLD32WH           vr18,[i1, PCB_RECONSTRUCTED_STRIDE*1+8]                              // q1U chromaPtr [  870] 
    
       VLD64W            vr19,[i1, PCB_RECONSTRUCTED_STRIDE*-2+16]                            // p1V chromaPtr [  872] 
       VLD64W            vr20,[i1, PCB_RECONSTRUCTED_STRIDE*-1+16]                            // p0V chromaPtr [  873] 
       VJP.i3            r7                                                       // strongFilter ChromaStrong [  874] 
       VLD64W            vr21,[i1, PCB_RECONSTRUCTED_STRIDE*0+16]  //DS_________________________ q0V chromaPtr [  875] 
       VLD64W            vr22,[i1, PCB_RECONSTRUCTED_STRIDE*1+16]  //DS_________________________ q1V chromaPtr [  876] 
       VSUBW             vr28, vr62, vr27  //DS____________________________________________________ mc VZERO c [  877] 
// Chroma U
// Normal filter
//  if ( (ABS(q0 - p0) < alpha) && (ABS(p1 - p0) < beta) && (ABS(q1 - q0) < beta) )
       VDIFW             vr23, vr16, vr17                                                   // macInpU p0U q0U [  881] 
       VDIFW             vr25, vr15, vr16                                                     // dp1p0 p1U p0U [  882] 
       VDIFW             vr26, vr18, vr17                                                     // dq1q0 q1U q0U [  883] 
       VSLTW             vr23, vr12                                                           // macInpU alpha [  884] 
       VSLTW.S           vr25, vr13                                                              // dp1p0 beta [  885] 
       VSLTW.S           vr26, vr13                                                              // dq1q0 beta [  886] 
//          int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
       VSUBW             vr23, vr17, vr16                                                   // macInpU q0U p0U [  888] 
       VMULW             vr23, vr23, 4                                                      // macInpU macInpU [  889] 
       VSUBAW            vr23, vr15, vr18                                                   // macInpU p1U q1U [  890] 
// Chroma V
// Normal filter
//  if ( (ABS(q0 - p0) < alpha) && (ABS(p1 - p0) < beta) && (ABS(q1 - q0) < beta) )
       VDIFW             vr24, vr20, vr21                                                   // macInpV p0V q0V [  894] 
       VDIFW             vr25, vr19, vr20                                                     // dp1p0 p1V p0V [  895] 
       VDIFW             vr26, vr22, vr21                                                     // dq1q0 q1V q0V [  896] 
       VASRRW            vr23, vr23, 3                                                      // macInpU macInpU [  897] 
//          int delta = CLIP(-c, c, macro_input);
       VMAXW             vr23, vr23, vr28                                                // macInpU macInpU mc [  899] 
       VMINW             vr23, vr23, vr27                                                 // macInpU macInpU c [  900] 
//          P0 = RECON_CLIP(p0 + delta);
       VADDW.S           vr16, vr16, vr23                                                   // p0U p0U macInpU [  902] 
//          Q0 = RECON_CLIP(q0 - delta);
       VSUBW.S           vr17, vr17, vr23                                                   // q0U q0U macInpU [  904] 

       VSLTW             vr24, vr12                                                           // macInpV alpha [  906] 
       VSLTW.S           vr25, vr13                                                              // dp1p0 beta [  907] 
       VSLTW.S           vr26, vr13                                                              // dq1q0 beta [  908] 
//          int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
       VSUBW             vr24, vr21, vr20                                                   // macInpV q0V p0V [  910] 
       VMULW             vr24, vr24, 4                                                      // macInpV macInpV [  911] 
       VSUBAW            vr24, vr19, vr22                                                   // macInpV p1V q1V [  912] 
       VASRRW            vr24, vr24, 3                                                      // macInpV macInpV [  913] 
//          int delta = CLIP(-c, c, macro_input);
       VMAXW             vr24, vr24, vr28                                                // macInpV macInpV mc [  915] 
       VJP               r8                                                                 // ChromaWriteBack [  916] 
       VMINW             vr24, vr24, vr27  //DS_____________________________________________ macInpV macInpV c [  917] 
//          P0 = RECON_CLIP(p0 + delta);
       VADDW.S           vr20, vr20, vr24  //DS_______________________________________________ p0V p0V macInpV [  919] 
//          Q0 = RECON_CLIP(q0 - delta);
       VSUBW.S           vr21, vr21, vr24  //DS_______________________________________________ q0V q0V macInpV [  921] 


//ChromaStrong --------                                                                                     // [  924] 
 vendrec r7                                                                                                 // [  924] 
 vrec r7                                                                                                    // [  924] 
// Strong filter U
LINE 926".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VDIFW             vr23, vr16, vr17                                                   // macInpU p0U q0U [  926] 
       VDIFW             vr25, vr15, vr16                                                     // dp1p0 p1U p0U [  927] 
       VDIFW             vr26, vr18, vr17                                                     // dq1q0 q1U q0U [  928] 
       VSLTW             vr23, vr12                                                           // macInpU alpha [  929] 
       VSLTW.S           vr25, vr13                                                              // dp1p0 beta [  930] 
       VSLTW.S           vr26, vr13                                                              // dq1q0 beta [  931] 
//            P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);
       VADDW.S           vr16, vr16, vr15                                                       // p0U p0U p1U [  933] 
       VADDAW.S          vr16, vr18, vr15                                                       // p0U q1U p1U [  934] 
// Strong filter V
       VDIFW             vr24, vr20, vr21                                                   // macInpV p0V q0V [  936] 
       VDIFW             vr25, vr19, vr20                                                     // dp1p0 p1V p0V [  937] 
       VDIFW             vr26, vr22, vr21                                                     // dq1q0 q1V q0V [  938] 
       VASRRW.S          vr16, vr16, 2                                                              // p0U p0U [  939] 
//            Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
       VADDW.S           vr17, vr17, vr18                                                       // q0U q0U q1U [  941] 
       VADDAW.S          vr17, vr15, vr18                                                       // q0U p1U q1U [  942] 
       VSUBW.F           vr62, vr24, vr12                                               // VZERO macInpV alpha [  943] 
       VSUBW.N.F         vr62, vr25, vr13                                                  // VZERO dp1p0 beta [  944] 
       VSUBW.N.F         vr62, vr26, vr13                                                  // VZERO dq1q0 beta [  945] 
       VASRRW.S          vr17, vr17, 2                                                              // q0U q0U [  946] 

//            P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);
       VADDW.N           vr20, vr20, vr19                                                       // p0V p0V p1V [  949] 
       VADDAW.N          vr20, vr22, vr19                                                       // p0V q1V p1V [  950] 
//            Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
       VADDW.N           vr21, vr21, vr22                                                       // q0V q0V q1V [  952] 
       VADDAW.N          vr21, vr19, vr22                                                       // q0V p1V q1V [  953] 
       VASRRW.N          vr20, vr20, 2                                                              // p0V p0V [  954] 
       VASRRW.N          vr21, vr21, 2                                                              // q0V q0V [  955] 


//ChromaWriteBack --------                                                                                  // [  958] 
 vendrec r8                                                                                                 // [  958] 
 vrec r8                                                                                                    // [  958] 
LINE 959".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VASRPWB           vr16, vr16, 0                                                              // p0U p0U [  959] 
       VASRPWB           vr17, vr17, 0                                                              // q0U q0U [  960] 
       VASRPWB           vr20, vr20, 0                                                              // p0V p0V [  961] 
       VASRPWB           vr21, vr21, 0                                                              // q0V q0V [  962] 
       VST32             vr16,[i1, PCB_RECONSTRUCTED_STRIDE*-1+4]                             // p0U chromaPtr [  963] 
       VST32_2           vr16,[i1, PCB_RECONSTRUCTED_STRIDE*-1+8]                             // p0U chromaPtr [  964] 
       VST32             vr17,[i1, PCB_RECONSTRUCTED_STRIDE*0+4]                              // q0U chromaPtr [  965] 
       VST32_2           vr17,[i1, PCB_RECONSTRUCTED_STRIDE*0+8]                              // q0U chromaPtr [  966] 
       VST64             vr20,[i1, PCB_RECONSTRUCTED_STRIDE*-1+16]                            // p0V chromaPtr [  967] 
       VST64             vr21,[i1, PCB_RECONSTRUCTED_STRIDE*0+16]                             // q0V chromaPtr [  968] 

//ChromaDone --------                                                                                       // [  970] 
 vendrec r6                                                                                                 // [  970] 
 vrec r6                                                                                                    // [  970] 
// End                                                                                                      // [  971] 
 
 
 // Now Luma
 
// do vertical filter on whole MB width. So 16 pixels wide in 4x4 lanes, 2 across registers
LINE 977".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr06, vr62, 240                                                      // abIdxsY VZERO [  977] // setup loop until zero popped
       VPOPW             i10, vr04, 0                                                       // str filStrength [  978] 
       VPOPW             i9, vr06, 0                                                           // idxY abIdxsY [  979] 
       VPOPW             i11, vr07, 0                                                        // idxUV abIdxsUV [  980] 
//ParamLp --------                                                                                          // [  981] 
 vendrec r9                                                                                                 // [  981] 
 vrec r9                                                                                                    // [  981] 
LINE 982".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VJP!i10           r10                                                                  // str VEdgeDone [  982] // strength zero so zkip

       VLD8W.i10         vr12,[i9, ABC0_ALPHA]  //DS___________________________________________ str alpha idxY [  984] 
       VLD8W.i10         vr13,[i9, ABC0_BETA]  //DS_____________________________________________ str beta idxY [  985] 
       VLD8W.i10         vr14,[i9, ABC0_C0]  //DS_________________________________________________ str c0 idxY [  986] 
       VXSUMW.15         vr12, vr12, 1                                                          // alpha alpha [  987] // broadcast across lanes
       VXSUMW.15         vr13, vr13, 1                                                            // beta beta [  988] 
       VXSUMW.15         vr14, vr14, 1                                                                // c0 c0 [  989] 

//vec16  vr15  p3                                                                                           // [  991] 
//vec16  vr16  p2                                                                                           // [  991] 
//vec16  vr17  p1                                                                                           // [  991] 
//vec16  vr18  p0                                                                                           // [  991] 
//vec16  vr19  q0                                                                                           // [  991] 
//vec16  vr20  q1                                                                                           // [  991] 
//vec16  vr21  q2                                                                                           // [  991] 
//vec16  vr22  q3                                                                                           // [  991] 
//vec16  vr23  macInp                                                                                       // [  992] 
// Begin                                                                                                    // [  993] 
//vec16  vr24  dp1p0                                                                                        // [  994] 
//vec16  vr25  dq1q0                                                                                        // [  994] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                  p3                                                   |
//|VR16|                                                  p2                                                   |
//|VR17|                                                  p1                                                   |
//|VR18|                                                  p0                                                   |
//|VR19|                                                  q0                                                   |
//|VR20|                                                  q1                                                   |
//|VR21|                                                  q2                                                   |
//|VR22|                                                  q3                                                   |
//|VR23|                                              macInp                                                   |
//|VR24|                                               dp1p0                                                   |
//|VR25|                                               dq1q0                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 996".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD32WL           vr15,[i0, PCB_RECONSTRUCTED_STRIDE*-4]                                  // p3 lumaPtr [  996] 
       VLD32WL           vr16,[i0, PCB_RECONSTRUCTED_STRIDE*-3]                                  // p2 lumaPtr [  997] 
       VLD32WL           vr17,[i0, PCB_RECONSTRUCTED_STRIDE*-2]                                  // p1 lumaPtr [  998] 
       VLD32WL           vr18,[i0, PCB_RECONSTRUCTED_STRIDE*-1]                                  // p0 lumaPtr [  999] 
       VLD32WL           vr19,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                   // q0 lumaPtr [ 1000] 
       VLD32WL           vr20,[i0, PCB_RECONSTRUCTED_STRIDE*1]                                   // q1 lumaPtr [ 1001] 
       VLD32WL           vr21,[i0, PCB_RECONSTRUCTED_STRIDE*2]                                   // q2 lumaPtr [ 1002] 
       VLD32WL           vr22,[i0, PCB_RECONSTRUCTED_STRIDE*3]                                   // q3 lumaPtr [ 1003] 

       VDIFW             vr23, vr19, vr18                                                      // macInp q0 p0 [ 1005] //ABS(q0 - p0)
//if ( (macInp < alpha) && (ABS(p1 - p0) < beta) && (ABS(q1 - q0) < beta) )
       VDIFW             vr24, vr17, vr18                                                       // dp1p0 p1 p0 [ 1007] 
       VDIFW             vr25, vr20, vr19                                                       // dq1q0 q1 q0 [ 1008] 
       VJP.i3            r11                                                          // strongFilter DoStrong [ 1009] 
       VSLTW             vr23, vr12  //DS________________________________________________________ macInp alpha [ 1010] 
       VSLTW.S           vr24, vr13  //DS__________________________________________________________ dp1p0 beta [ 1011] 
       VSLTW.S           vr25, vr13  //DS__________________________________________________________ dq1q0 beta [ 1012] 
// End                                                                                                      // [ 1013] 
 // Normal vertical luma ------------------
 
// Begin                                                                                                    // [ 1016] 
//vec16  vr24  apLtBeta                                                                                     // [ 1017] 
//vec16  vr25  aqLtBeta                                                                                     // [ 1017] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                  p3                                                   |
//|VR16|                                                  p2                                                   |
//|VR17|                                                  p1                                                   |
//|VR18|                                                  p0                                                   |
//|VR19|                                                  q0                                                   |
//|VR20|                                                  q1                                                   |
//|VR21|                                                  q2                                                   |
//|VR22|                                                  q3                                                   |
//|VR23|                                              macInp                                                   |
//|VR24|                                            apLtBeta                                                   |
//|VR25|                                            aqLtBeta                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

//                apLtBeta = ABS(p2 - p0) < beta
//                aqLtBeta = ABS(q2 - q0) < beta
LINE 1021".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VDIFW             vr24, vr16, vr18                                                    // apLtBeta p2 p0 [ 1021] 
       VDIFW             vr25, vr21, vr19                                                    // aqLtBeta q2 q0 [ 1022] 
       VLTW              vr24, vr24, vr13                                            // apLtBeta apLtBeta beta [ 1023] 
       VLTW              vr25, vr25, vr13                                            // aqLtBeta aqLtBeta beta [ 1024] 
// Begin                                                                                                    // [ 1025] 
//vec16  vr26  p0q0Rs1                                                                                      // [ 1026] 
//vec16  vr27  delta                                                                                        // [ 1026] 
//vec16  vr28  c                                                                                            // [ 1027] 
//vec16  vr29  mc                                                                                           // [ 1027] 
//vec16  vr30  mc0                                                                                          // [ 1028] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                  p3                                                   |
//|VR16|                                                  p2                                                   |
//|VR17|                                                  p1                                                   |
//|VR18|                                                  p0                                                   |
//|VR19|                                                  q0                                                   |
//|VR20|                                                  q1                                                   |
//|VR21|                                                  q2                                                   |
//|VR22|                                                  q3                                                   |
//|VR23|                                              macInp                                                   |
//|VR24|                                            apLtBeta                                                   |
//|VR25|                                            aqLtBeta                                                   |
//|VR26|                                             p0q0Rs1                                                   |
//|VR27|                                               delta                                                   |
//|VR28|                                                   c                                                   |
//|VR29|                                                  mc                                                   |
//|VR30|                                                 mc0                                                   |
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

 //                macInp = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;
//                c = c0 + apLtBeta + aqLtBeta;
LINE 1032".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VSUBW             vr28, vr14, vr24                                                     // c c0 apLtBeta [ 1032] // do sub because -1 is true
       VSUBW             vr23, vr19, vr18                                                      // macInp q0 p0 [ 1033] 
       VSUBAW            vr23, vr19, vr18                                                      // macInp q0 p0 [ 1034] 
       VSUBAW            vr23, vr19, vr18                                                      // macInp q0 p0 [ 1035] 
       VSUBAW            vr23, vr19, vr18                                                      // macInp q0 p0 [ 1036] 
       VSUBAW            vr23, vr17, vr20                                                      // macInp p1 q1 [ 1037] 
       VSUBW             vr28, vr28, vr25                                                      // c c aqLtBeta [ 1038] 
//                p0q0Rs1 = (p0 + q0 + 1) >> 1;
       VADDW             vr26, vr18, vr19                                                     // p0q0Rs1 p0 q0 [ 1040] 
       VSUBW             vr29, vr62, vr28                                                        // mc VZERO c [ 1041] 
       VASRRW            vr23, vr23, 3                                                        // macInp macInp [ 1042] 

       VASRRW            vr26, vr26, 1                                                      // p0q0Rs1 p0q0Rs1 [ 1044] 
//                delta = CLIP(-c, c, macInp);
       VMAXW             vr27, vr29, vr23                                                   // delta mc macInp [ 1046] 
       VMINW             vr27, vr27, vr28                                                     // delta delta c [ 1047] 
    
//                P0 = RECON_CLIP(p0 + delta);
       VADDW.S           vr18, vr18, vr27                                                       // p0 p0 delta [ 1050] 
//                Q0 = RECON_CLIP(q0 - delta);
//                    macInp = (p2 + p0q0Rs1 - (p1 << 1)) >> 1;
       VSUBW             vr23, vr16, vr17                                                      // macInp p2 p1 [ 1053] 
       VSUBAW            vr23, vr26, vr17                                                 // macInp p0q0Rs1 p1 [ 1054] 
       VSUBW.S           vr19, vr19, vr27                                                       // q0 q0 delta [ 1055] 
//                if (apLtBeta)
       VADDW.F           vr62, vr24, vr62                                              // VZERO apLtBeta VZERO [ 1057] 
//                {
       VASRW             vr23, vr23, 1                                                        // macInp macInp [ 1059] 
       VSUBW             vr30, vr62, vr14                                                      // mc0 VZERO c0 [ 1060] 
//                    P1 = (PIXEL)(p1 + CLIP(-c0, c0, macInp));
       VMINW             vr23, vr23, vr14                                                  // macInp macInp c0 [ 1062] 
       VMAXW             vr23, vr23, vr30                                                 // macInp macInp mc0 [ 1063] 
       VADDW.S.NZ        vr17, vr17, vr23                                                      // p1 p1 macInp [ 1064] 
//                }
//                if (aqLtBeta)
//                {
//                    macInp = (q2 + p0q0Rs1 - (q1 << 1)) >> 1;
       VSUBW             vr23, vr21, vr20                                                      // macInp q2 q1 [ 1069] 
       VSUBAW            vr23, vr26, vr20                                                 // macInp p0q0Rs1 q1 [ 1070] 
       VADDW.F           vr62, vr25, vr62                                              // VZERO aqLtBeta VZERO [ 1071] 
       VASRW             vr23, vr23, 1                                                        // macInp macInp [ 1072] 
//                    Q1 = (PIXEL)(q1 + CLIP(-c0, c0, macInp));
       VJP               r12                                                                  // WriteBackLuma [ 1074] 
       VMAXW             vr23, vr23, vr30  //DS_____________________________________________ macInp macInp mc0 [ 1075] 
       VMINW             vr23, vr23, vr14  //DS______________________________________________ macInp macInp c0 [ 1076] 
       VADDW.S.NZ        vr20, vr20, vr23  //DS__________________________________________________ q1 q1 macInp [ 1077] 

// End                                                                                                      // [ 1079] 
//                }
// End                                                                                                      // [ 1081] 

//DoStrong --------                                                                                         // [ 1083] // Strong verticcal luma --------
 vendrec r11                                                                                                // [ 1083] 
 vrec r11                                                                                                   // [ 1083] 
// Begin                                                                                                    // [ 1084] 
// p16    i12  apLtBeta                                                                                     // [ 1085] 
// p16    i13  aqLtBeta                                                                                     // [ 1085] 
// Begin                                                                                                    // [ 1086] 
//vec16  vr24  pTmp                                                                                         // [ 1087] 
//vec16  vr25  qTmp                                                                                         // [ 1087] 
//vec16  vr26  smallGap                                                                                     // [ 1087] 
//                // Strong filtering for some intra macroblock boundaries
//                bool small_gap = (macro_input < ((alpha >> 2) + 2));
LINE 1090".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VASRW             vr26, vr12, 2                                                       // smallGap alpha [ 1090] 
       VMOVW             vr24, BigButSafe, 255                                                         // pTmp [ 1091] 
       VADDW             vr26, vr26, 2                                                    // smallGap smallGap [ 1092] 
       VMOVW             vr25, BigButSafe, 255                                                         // qTmp [ 1093] 
       VSUBW.F           vr62, vr23, vr26                                             // VZERO macInp smallGap [ 1094] 
//                bool apLtBeta = (ABS(p2 - p0) < beta) && small_gap
       VDIFW.S.N         vr24, vr16, vr18                                                        // pTmp p2 p0 [ 1096] // Otherwise BigButSafe
       VILTW             i12, vr24, vr13                                                 // apLtBeta pTmp beta [ 1097] 
//                bool aqLtBeta = (ABS(q2 - q0) < beta) && small_gap
       VDIFW.S.N         vr25, vr21, vr19                                                        // qTmp q2 q0 [ 1099] 
       VILTW             i13, vr25, vr13                                                 // aqLtBeta qTmp beta [ 1100] 
// End                                                                                                      // [ 1101] 
 
//vec16  vr24  op1                                                                                          // [ 1103] 
//vec16  vr25  op0                                                                                          // [ 1103] 
//vec16  vr26  oq0                                                                                          // [ 1103] 
//vec16  vr27  oq1                                                                                          // [ 1103] 
//vec16  vr28  tmp                                                                                          // [ 1103] 
//vec16  vr29  tmq                                                                                          // [ 1103] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|    aqLtBeta|    apLtBeta|       idxUV|         str|        idxY|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|VR15|                                                  p3                                                   |
//|VR16|                                                  p2                                                   |
//|VR17|                                                  p1                                                   |
//|VR18|                                                  p0                                                   |
//|VR19|                                                  q0                                                   |
//|VR20|                                                  q1                                                   |
//|VR21|                                                  q2                                                   |
//|VR22|                                                  q3                                                   |
//|VR23|                                              macInp                                                   |
//|VR24|                                                 op1                                                   |
//|VR25|                                                 op0                                                   |
//|VR26|                                                 oq0                                                   |
//|VR27|                                                 oq1                                                   |
//|VR28|                                                 tmp                                                   |
//|VR29|                                                 tmq                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

//                // Copy some original pels first
//                int op1 = p1;
//                int op0 = p0;
//                int oq0 = q0;
//                int oq1 = q1;
LINE 1110".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr24, vr17, 255                                                             // op1 p1 [ 1110] 
       VMVW              vr25, vr18, 255                                                             // op0 p0 [ 1111] 
       VMVW              vr26, vr19, 255                                                             // oq0 q0 [ 1112] 
       VMVW              vr27, vr20, 255                                                             // oq1 q1 [ 1113] 
 
//                if (ap_less_than_beta)
//                {
//                    P0 = (PIXEL)((p2 + oq1 + ((op1 + op0 + oq0) << 1) + 4) >> 3);
       VADDW.i12.S       vr18, vr16, vr27                                                // apLtBeta p0 p2 oq1 [ 1118] 
       VADDAW.i12.S      vr18, vr24, vr25                                               // apLtBeta p0 op1 op0 [ 1119] 
       VADDAW.i12.S      vr18, vr26, vr24                                               // apLtBeta p0 oq0 op1 [ 1120] 
       VADDAW.i12.S      vr18, vr25, vr26                                               // apLtBeta p0 op0 oq0 [ 1121] 
//                    P1 = (PIXEL)((p2 + op1 + op0 + oq0 + 2) >> 2);
       VADDW.i12.S       vr17, vr16, vr24                                                // apLtBeta p1 p2 op1 [ 1123] 
       VADDAW.i12.S      vr17, vr25, vr26                                               // apLtBeta p1 op0 oq0 [ 1124] 
//                    P2 = (PIXEL)((p2 + op1 + op0 + oq0 + ((p3 + p2) << 1) + 4) >> 3);
       VADDW.i12.S       vr28, vr16, vr24                                               // apLtBeta tmp p2 op1 [ 1126] 
       VADDAW.i12.S      vr28, vr25, vr26                                              // apLtBeta tmp op0 oq0 [ 1127] 
       VADDAW.i12.S      vr28, vr15, vr15                                                // apLtBeta tmp p3 p3 [ 1128] 
       VADDAW.i12.S      vr28, vr16, vr16                                                // apLtBeta tmp p2 p2 [ 1129] 

       VASRRW.i12.S      vr18, vr18, 3                                                       // apLtBeta p0 p0 [ 1131] 
       VASRRW.i12.S      vr17, vr17, 2                                                       // apLtBeta p1 p1 [ 1132] 
       VASRRW.i12.S      vr16, vr28, 3                                                      // apLtBeta p2 tmp [ 1133] 
    
//                }
//                else
//                {
//                    P0 = (PIXEL)((op0 + oq1 + (op1 << 1) + 2) >> 2);

//                }
//                if (aq_less_than_beta)
//                {
//                    Q0 = (PIXEL)((op1 + q2 + ((op0 + oq0 + oq1) << 1) + 4) >> 3);
       VADDW.i13.S       vr19, vr24, vr21                                                // aqLtBeta q0 op1 q2 [ 1144] 
       VADDAW.i13.S      vr19, vr25, vr25                                               // aqLtBeta q0 op0 op0 [ 1145] 
       VADDAW.i13.S      vr19, vr26, vr26                                               // aqLtBeta q0 oq0 oq0 [ 1146] 
       VADDAW.i13.S      vr19, vr27, vr27                                               // aqLtBeta q0 oq1 oq1 [ 1147] 
//                    Q1 = (PIXEL)((op0 + oq0 + oq1 + q2 + 2) >> 2);
       VADDW.i13.S       vr20, vr25, vr26                                               // aqLtBeta q1 op0 oq0 [ 1149] 
       VADDAW.i13.S      vr20, vr27, vr21                                                // aqLtBeta q1 oq1 q2 [ 1150] 
//                    Q2 = (PIXEL)((q2 + oq1 + oq0 + op0 + ((q3 + q2) << 1) + 4) >> 3);
       VADDW.i13.S       vr29, vr21, vr27                                               // aqLtBeta tmq q2 oq1 [ 1152] 
       VADDAW.i13.S      vr29, vr26, vr25                                              // aqLtBeta tmq oq0 op0 [ 1153] 
       VADDAW.i13.S      vr29, vr22, vr22                                                // aqLtBeta tmq q3 q3 [ 1154] 
       VADDAW.i13.S      vr29, vr21, vr21                                                // aqLtBeta tmq q2 q2 [ 1155] 
    
       VASRRW.i13.S      vr19, vr19, 3                                                       // aqLtBeta q0 q0 [ 1157] 
       VASRRW.i13.S      vr20, vr20, 2                                                       // aqLtBeta q1 q1 [ 1158] 
       VASRRW.i13.S      vr21, vr29, 3                                                      // aqLtBeta q2 tmq [ 1159] 
//                }
//                else
//                {
//                    Q0 = (PIXEL)((oq0 + op1 + (oq1 << 1) + 2) >> 2);
       VADDW!i13.S       vr19, vr26, vr24                                               // aqLtBeta q0 oq0 op1 [ 1164] 
       VADDAW!i13.S      vr19, vr27, vr27                                               // aqLtBeta q0 oq1 oq1 [ 1165] 
       VADDW!i12.S       vr18, vr25, vr27                                               // apLtBeta p0 op0 oq1 [ 1166] 
       VADDAW!i12.S      vr18, vr24, vr24                                               // apLtBeta p0 op1 op1 [ 1167] 
    
       VASRRW!i13.S      vr19, vr19, 2                                                       // aqLtBeta q0 q0 [ 1169] 
       VASRRW!i12.S      vr18, vr18, 2                                                       // apLtBeta p0 p0 [ 1170] 
//                }
       VASRPWB           vr16, vr16, 0                                                                // p2 p2 [ 1172] 
       VASRPWB           vr21, vr21, 0                                                                // q2 q2 [ 1173] 
        
       VST32             vr16,[i0, PCB_RECONSTRUCTED_STRIDE*-3]                                  // p2 lumaPtr [ 1175] 
       VST32             vr21,[i0, PCB_RECONSTRUCTED_STRIDE*2]                                   // q2 lumaPtr [ 1176] 

// End                                                                                                      // [ 1178] 
//WriteBackLuma --------                                                                                    // [ 1179] 
 vendrec r12                                                                                                // [ 1179] 
 vrec r12                                                                                                   // [ 1179] 
LINE 1180".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VASRPWB           vr17, vr17, 0                                                                // p1 p1 [ 1180] 
       VASRPWB           vr18, vr18, 0                                                                // p0 p0 [ 1181] 
       VASRPWB           vr19, vr19, 0                                                                // q0 q0 [ 1182] 
       VASRPWB           vr20, vr20, 0                                                                // q1 q1 [ 1183] 
       VST32             vr17,[i0, PCB_RECONSTRUCTED_STRIDE*-2]                                  // p1 lumaPtr [ 1184] 
       VST32             vr18,[i0, PCB_RECONSTRUCTED_STRIDE*-1]                                  // p0 lumaPtr [ 1185] 
       VST32             vr19,[i0, PCB_RECONSTRUCTED_STRIDE*0]                                   // q0 lumaPtr [ 1186] 
       VST32             vr20,[i0, PCB_RECONSTRUCTED_STRIDE*1]                                   // q1 lumaPtr [ 1187] 

 
//VEdgeDone --------                                                                                        // [ 1190] 
 vendrec r10                                                                                                // [ 1190] 
 vrec r10                                                                                                   // [ 1190] 
LINE 1191".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VPOPW             i9, vr06, 0                                                           // idxY abIdxsY [ 1191] 
       VPOPW             i11, vr07, 0                                                        // idxUV abIdxsUV [ 1192] 
       VJP.i9            r9                                                                    // idxY ParamLp [ 1193] 
       VPOPW             i10, vr04, 0  //DS___________________________________________________ str filStrength [ 1194] 
       VIM               i0, i0, 4  //DS______________________________________________________ lumaPtr lumaPtr [ 1195] 
       VIM               i1, i1, 2  //DS__________________________________________________ chromaPtr chromaPtr [ 1196] 
    
// End                                                                                                      // [ 1198] 
    
// label RowLoopEnd
// Begin                                                                                                    // [ 1201] 
// p16    i9  intra                                                                                         // [ 1202] 
// REGMAP 
//+============================================================================================================+
//|REG |     7      |     6      |     5      |     4      |     3      |     2      |     1      |     0      |
//+============================================================================================================+
//|VR 0|       mvPtr|     CirLeft|      FixBuf|      CirBuf|strongFilter|    lumaCoef|   chromaPtr|     lumaPtr|
//|VR 1|aboveDbValid| leftDbValid|            |            |            |            |       intra|     evenRow|
//|VR 2|  intAbIdxsY|  lumaNzCoef|InternalUVab| InternalYab|  AboveIntra|   LeftIntra|    BlkIntra|    chromaQp|
//|VR 3|            |            |            |            |            |      coefNz| intAbIdxsUV| ChromaQ_Id.|
//|VR 4|                                         filStrength                                                   |
//|VR 5|                                     initFilStrength                                                   |
//|VR 6|                                             abIdxsY                                                   |
//|VR 7|                                            abIdxsUV                                                   |
//|VR 8|                                             RowJump                                                   |
//|VR 9|                                               mvTop                                                   |
//|VR10|                                               mvMid                                                   |
//|VR11|                                               mvBot                                                   |
//|VR12|                                               alpha                                                   |
//|VR13|                                                beta                                                   |
//|VR14|                                                  c0                                                   |
//|............................................................................................................|
//|VR31|                                              VSTACK                                                   |
//|............................................................................................................|
//|VR62|                                               VZERO                                                   |
//|VR63|                                              VFLAGS                                                   |
//+------------------------------------------------------------------------------------------------------------+

LINE 1204".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIV.2             i9, vr02                                                            // intra BlkIntra [ 1204] //  should be 0 or 0xffff
       VLSRW.4           vr00, vr00, 4                                                    // lumaCoef lumaCoef [ 1205] 
       VMVW              vr00, vr62, 8                                                   // strongFilter VZERO [ 1206] 
       VMVW              vr04, vr62, 255                                                  // filStrength VZERO [ 1207] 
       VXSUMW            vr06, vr02, 128                                                 // abIdxsY intAbIdxsY [ 1208] 
       VXSUMW            vr07, vr03, 2                                                 // abIdxsUV intAbIdxsUV [ 1209] 
       VMOVW.i9          vr04, 3                                                          // intra filStrength [ 1210] 
       VEQW.1            vr01, vr01, vr62                                             // evenRow evenRow VZERO [ 1211] // 0 / 0xffff
       VJB               vr08, 0                                                                    // RowJump [ 1212] 
       VIM               i0, i0, PCB_RECONSTRUCTED_STRIDE*4-16  //DS__________________________ lumaPtr lumaPtr [ 1213] 
       VIM               i1, i1, PCB_RECONSTRUCTED_STRIDE*2-8  //DS_______________________ chromaPtr chromaPtr [ 1214] 
       VMVW.i8           vr09, vr10  //DS_________________________________________________ evenRow mvTop mvMid [ 1215] 
// End                                                                                                      // [ 1216] 
 
//RowLoopExit --------                                                                                      // [ 1218] 
 vendrec r5                                                                                                 // [ 1218] 
 vrec r5                                                                                                    // [ 1218] 
 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz VerticalDeblock_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc VerticalDeblock                                                                                  // [ 1220] 




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// WRITE DEBLOCKED/UnDeblock FUNCTIONS /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GLOBAL Variables for DMA out

// Begin                                                                                                    // [ 1229] 
	//p32                     RowColumn=k0    	// Row Col parameter
    //p32                     HSzVsz=k8     		// Horizontal/Vertical frame size
// ****** Informational : Register space for symbol "RowColumn" already allocated.                          // [ 1232] // Row Col parameter
//vec32  vr02  RowColumn                                                                                    // [ 1232] 
// ****** Informational : Register space for symbol "HSzVsz" already allocated.                             // [ 1233] // Horizontal/Vertical frame size
//vec32  vr03  HSzVsz                                                                                       // [ 1233] 
// ****** Informational : Register space for symbol "YSdmaddr" already allocated.                           // [ 1234] 
//vec32  vr04  YSdmaddr                                                                                     // [ 1234] 
// ****** Informational : Register space for symbol "USdmaddr" already allocated.                           // [ 1235] 
//vec32  vr05  USdmaddr                                                                                     // [ 1235] 
// ****** Informational : Register space for symbol "VSdmaddr" already allocated.                           // [ 1236] 
//vec32  vr06  VSdmaddr                                                                                     // [ 1236] 
// ****** Informational : Register space for symbol "YfrmIdx" already allocated.                            // [ 1237] 
//vec32  vr07  YfrmIdx                                                                                      // [ 1237] 
//vec32  vr08  UfrmIdx                                                                                      // [ 1238] 
//vec32  vr09  VfrmIdx                                                                                      // [ 1239] 
// p32    k0  sdmAddr                                                                                       // [ 1240] 
    // End params -------
// ****** Informational : Register space for symbol "BLK_REG" already allocated.                            // [ 1242] 
// p32    k14  BLK_REG                                                                                      // [ 1242] 
	
// ****** Informational : Register space for symbol "loc_x" already allocated.                              // [ 1244] 
// p16    i0  loc_x                                                                                         // [ 1244] 
// ****** Informational : Register space for symbol "loc_y" already allocated.                              // [ 1245] 
// p16    i1  loc_y                                                                                         // [ 1245] 
// p32    k8  SdmStride                                                                                     // [ 1246] 
//vec32  vr10  RetAddr                                                                                      // [ 1247] 

// [Subroutine] DmaOutPixelData                                                                             // [ 1249] 
_Asm int Record_DmaOutPixelData(int *mac_tab)
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


// Begin                                                                                                    // [ 1251] 
// ****** Informational : Register space for symbol "DmaReg" already allocated.                             // [ 1252] 
// p32    k8  DmaReg                                                                                        // [ 1252] 
    

LINE 1255".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VDMAOSET          dr1, k8                                                                  // SdmStride [ 1255] 

    // dr0: sdm address
       VDMAOSET          dr0, k0                                                                    // sdmAddr [ 1258] 

    // dr2: block info
    // Place block size information and frame table index 
    // in r0 for chroma U and in r1 for chroma V
    //   [7:0] = horizontal block size
    //  [15:8] = vertical block size
    // [20:16] = FRAME_TABLE ADDR
       VDMAOSET          dr2, k14                                                                   // BLK_REG [ 1266] 

    // dr3: location (setup by vdmairun)

    // dr4: system memory address (contained in frame table)

    // dr5: system memory stride (contained in frame table)

    // dr6: config
    //    [2] =  '0' = disable double linestride
       VMOV.3            vr01, 0x0                                                                   // DmaReg [ 1276] 
       VDMAOSET          dr6, k8                                                                     // DmaReg [ 1277] 

    // dr7: frame table base address
       VMOV.3            vr01, SDMOF_FrameTabDMA                                                     // DmaReg [ 1280] 
       VJB               vr10, 0                                                                    // RetAddr [ 1281] 
       VDMAOSET          dr7, k8  //DS_________________________________________________________________ DmaReg [ 1282] 

    // Set loc_x,loc_y
       VMVW              vr00, vr02, 3  //DS________________________________________________ sdmAddr RowColumn [ 1285] 
       VDMAORUN          i0, i1  //DS_____________________________________________________________ loc_x loc_y [ 1286] 
    
    
// End                                                                                                      // [ 1289] 

  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc DmaOutPixelData                                                                                  // [ 1291] 


 	

// Is Macro
// Does the DMA out for deblocked block of pixels
// Sends channel reply
// [Macro] WriteDeblockedBlock                                                                              // [ 1297] 
_Asm int Record_WriteDeblockedBlock(int *mac_tab)
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
 	// parameters  of WriteDeblockedBlock function
    
    // End params -------
    
// Begin                                                                                                    // [ 1303] 

//vec32  vr11  RowsColumns                                                                                  // [ 1305] 
//vec32  vr12  AddrOffset                                                                                   // [ 1306] 
//vec32  vr13  AddrStrideOffset                                                                             // [ 1307] 
//vec32  vr14  UVAddrStrideOffset                                                                           // [ 1308] 
//vec32  vr15  Shiftval                                                                                     // [ 1309] 
//vec32  vr16  BlkXInc                                                                                      // [ 1310] 
//vec32  vr17  BlkYInc                                                                                      // [ 1311] 

    // SET the BLK_REG DMA register
    // Default block_x and block_y is 12
    // FRAME_TABLE_Y_CURR_ADDR is 0
    //+ PCB_RECONSTRUCTED_Y + 8
LINE 1317".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr01, vr07, 128                                                    // BLK_REG YfrmIdx [ 1317] 
       VMOVW             vr15, 4, 255                                                              // Shiftval [ 1318] // For divide by 16
       VMOVW             vr01, 0x0c0c, 64                                                           // BLK_REG [ 1319] 
       VASRVW            vr03, vr03, vr15                                            // HSzVsz HSzVsz Shiftval [ 1320] 
       VSUBW             vr03, vr03, 1                                                        // HSzVsz HSzVsz [ 1321] 
       VSNEW             vr03, vr02                                                        // HSzVsz RowColumn [ 1322] 
       VMOVW             vr16, 0x4, 64                                                              // BlkXInc [ 1323] 
       VMOVW             vr17, 0x0400, 64                                                           // BlkYInc [ 1324] 
       VADDW.64.U        vr01, vr01, vr16                                           // BLK_REG BLK_REG BlkXInc [ 1325] 
       VSR8.240          vr63, vr63, 2                                                        // VFLAGS VFLAGS [ 1326] 
       VADDW.64.U        vr01, vr01, vr17                                           // BLK_REG BLK_REG BlkYInc [ 1327] 
 	
 	
 	
 	// Set the SDM stride
       VMOV.3            vr01, PCB_RECONSTRUCTED_STRIDE                                           // SdmStride [ 1332] 
 	
 	
	// Set the SDM start address 	
 	// sdmAddr passed will
 	//vmvw				'sdmAddr, YSdmaddr
       VMOV              vr12, 4                                                                 // AddrOffset [ 1338] 
       VUP.255           vr11, vr02                                                   // RowsColumns RowColumn [ 1339] 
       VADD.F            vr11, vr11, vr62                                     // RowsColumns RowsColumns VZERO [ 1340] 
       VSUB.3.NZ         vr04, vr04, vr12                                      // YSdmaddr YSdmaddr AddrOffset [ 1341] 
 	
 	// UV SDM ADDRESS 	
       VMUL.3            vr14, vr01, 2                                         // UVAddrStrideOffset SdmStride [ 1344] 
       VADD              vr05, vr05, vr14                              // USdmaddr USdmaddr UVAddrStrideOffset [ 1345] 
       VADD              vr06, vr06, vr14                              // VSdmaddr VSdmaddr UVAddrStrideOffset [ 1346] 
       VMOV              vr12, 2                                                                 // AddrOffset [ 1347] 
       VSUB.3.NZ         vr05, vr05, vr12                                      // USdmaddr USdmaddr AddrOffset [ 1348] 
       VSUB.3.NZ         vr06, vr06, vr12                                      // VSdmaddr VSdmaddr AddrOffset [ 1349] 
 	
       VSR8.15           vr63, vr63, 4                                                        // VFLAGS VFLAGS [ 1351] 
       VMUL.3.Z          vr13, vr01, 4                                           // AddrStrideOffset SdmStride [ 1352] // (y+4)*PCB_RECONSTRUCTED_STRIDE
       VADD.Z            vr04, vr04, vr13                                // YSdmaddr YSdmaddr AddrStrideOffset [ 1353] // (y+4)*PCB_RECONSTRUCTED_STRIDE
 	
 	
 	// UV
       VADD.Z            vr05, vr05, vr14                              // USdmaddr USdmaddr UVAddrStrideOffset [ 1357] 
       VADD.Z            vr06, vr06, vr14                              // VSdmaddr VSdmaddr UVAddrStrideOffset [ 1358] 
 	
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1360".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr04, 3                                                     // sdmAddr YSdmaddr [ 1360] 
 	
 	
 	
 	
 		
 	// Set the LOC_REG CHECK
 	// loc_x, loc_y
       VADDW.F           vr02, vr02, 0                                                  // RowColumn RowColumn [ 1368] 
       VMULW.3           vr02, vr02, 16                                                 // RowColumn RowColumn [ 1369] 
       VSUBW.3.NZ        vr02, vr02, 4                                                  // RowColumn RowColumn [ 1370] 
 		
 	
 	// Adjust the block size as well
 		
  ld r1,[r17,24];                                                                   // RetAddr DmaOutPixelData [ 1375] 
       VJL               vr10, r1                                                                           // [ 1375] 
       VADDW.64.NZ       vr01, vr01, vr16  //DS_______________________________________ BLK_REG BLK_REG BlkXInc [ 1376] 
       VSR8.240          vr63, vr63, 2  //DS____________________________________________________ VFLAGS VFLAGS [ 1377] 
       VADDW.64.NZ       vr01, vr01, vr17  //DS_______________________________________ BLK_REG BLK_REG BlkYInc [ 1378] 
    
    
    // U DMA OUT ================================================================================================================
  
   
       VASRW.64          vr01, vr01, 1                                                      // BLK_REG BLK_REG [ 1384] 
       VMVW              vr01, vr08, 128                                                    // BLK_REG UfrmIdx [ 1385] 
 	
 	
 	// Set the SDM stride
       VMOV.3            vr01, PCB_RECONSTRUCTED_STRIDE                                           // SdmStride [ 1389] 
 	
 	
 		// Set the LOC_REG CHECK
 	// loc_x, loc_y
 	
       VSR8.3            vr02, vr02, 4                                                  // RowColumn RowColumn [ 1395] 
       VMULW.3           vr02, vr02, 8                                                  // RowColumn RowColumn [ 1396] 
  ld r1,[r17,24];                                                                   // RetAddr DmaOutPixelData [ 1397] 
       VJL               vr10, r1                                                                           // [ 1397] 
       VADDW.F           vr02, vr02, 0  //DS______________________________________________ RowColumn RowColumn [ 1398] 
       VSUBW.3.NZ        vr02, vr02, 2  //DS______________________________________________ RowColumn RowColumn [ 1399] // Y component has substarcted an offset of 4 / UV does only 2
	
	// Set the SDM start address 	
 	// sdmAddr passed will
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1403".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr05, 3  //DS_________________________________________________ sdmAddr USdmaddr [ 1403] 
  
   
     // V DMA OUT ================================================================================================================
     
  	//vasrw.0x40				 'BLK_REG, 'BLK_REG, 1 
  ld r1,[r17,24];                                                                   // RetAddr DmaOutPixelData [ 1409] 
       VJL               vr10, r1                                                                           // [ 1409] 
       VMVW              vr01, vr09, 128  //DS________________________________________________ BLK_REG VfrmIdx [ 1410] 
 	
 	
 	// Set the SDM stride
       VMOV.3            vr01, PCB_RECONSTRUCTED_STRIDE  //DS_______________________________________ SdmStride [ 1414] 
 	
 	
 		// Set the LOC_REG CHECK
 	// loc_x, loc_y
 	
 	//vsr8.3                 RowColumn, RowColumn, 4		
 	//vmulw.3				  'RowColumn, 'RowColumn, 8
 	//vaddw.f				  'RowColumn, 'RowColumn, 0			
 	//vsubw.nz.3			  'RowColumn, 'RowColumn, 2	// Y component has substarcted an offset of 4 / UV does only 2
	
	// Set the SDM start address 	
 	// sdmAddr passed will
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1427".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr06, 3  //DS_________________________________________________ sdmAddr VSdmaddr [ 1427] 

 
// End                                                                                                      // [ 1430] 
 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc WriteDeblockedBlock                                                                              // [ 1433] 



// End                                                                                                      // [ 1435] // End of global variable scope



// [Macro] WriteOutputBlock                                                                                 // [ 1439] 
_Asm int Record_WriteOutputBlock(int *mac_tab)
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
// p32    k0  RowColumn                                                                                     // [ 1442] 
// ****** Informational : Register space for symbol "BLK_REG" already allocated.                            // [ 1443] 
// p32    k14  BLK_REG                                                                                      // [ 1443] 
// ****** Informational : Register space for symbol "YSdmAddr" already allocated.                           // [ 1444] 
//vec32  vr02  YSdmAddr                                                                                     // [ 1444] 
// ****** Informational : Register space for symbol "USdmAddr" already allocated.                           // [ 1445] 
//vec32  vr03  USdmAddr                                                                                     // [ 1445] 
// ****** Informational : Register space for symbol "VSdmAddr" already allocated.                           // [ 1446] 
//vec32  vr04  VSdmAddr                                                                                     // [ 1446] 
// ****** Informational : Register space for symbol "YfrmIdx" already allocated.                            // [ 1447] 
//vec32  vr05  YfrmIdx                                                                                      // [ 1447] 
// ****** Informational : Register space for symbol "UfrmIdx" already allocated.                            // [ 1448] 
//vec32  vr06  UfrmIdx                                                                                      // [ 1448] 
// ****** Informational : Register space for symbol "VfrmIdx" already allocated.                            // [ 1449] 
//vec32  vr07  VfrmIdx                                                                                      // [ 1449] 
// ****** Informational : Register space for symbol "sdmAddr" already allocated.                            // [ 1450] 
// p32    k6  sdmAddr                                                                                       // [ 1450] 
 	
   // parameters  of MACRO_WriteOutputBlock function

// Begin                                                                                                    // [ 1454] 
// ****** Informational : Register space for symbol "loc_x" already allocated.                              // [ 1455] 
// p16    i0  loc_x                                                                                         // [ 1455] 
// ****** Informational : Register space for symbol "loc_y" already allocated.                              // [ 1456] 
// p16    i1  loc_y                                                                                         // [ 1456] 
	
	
	// Config
   mov                 r0, 0
LINE 1461".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VDMAOSET          dr6, r0                                                                            // [ 1461] 
	
	// Frame table base address
   mov                 r0, SDMOF_FrameTabDMA
       VDMAOSET          dr7, r0                                                                            // [ 1465] 
	
   mov                 r0, PCB_RECONSTRUCTED_STRIDE
       VDMAOSET          dr1, r0                                                                            // [ 1468] 
	
	// Y output
	
	// BLOCK_REG
       VMOVW             vr01, 0x1010, 64                                                           // BLK_REG [ 1473] 
       VMVW              vr01, vr05, 128                                                    // BLK_REG YfrmIdx [ 1474] 
       VDMAOSET          dr2, k14                                                                   // BLK_REG [ 1475] 
	
	// SDM start address	
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1478".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr02, 192                                                   // sdmAddr YSdmAddr [ 1478] 
       VDMAOSET          dr0, k6                                                                    // sdmAddr [ 1479] 
	
	// LOC_X LOC_Y
       VMULW.3           vr00, vr00, 16                                                 // RowColumn RowColumn [ 1482] 
       VDMAORUN          i0, i1                                                                 // loc_x loc_y [ 1483] 
	
	
	// U output
	// BLOCK_REG
       VASRW.64          vr01, vr01, 1                                                      // BLK_REG BLK_REG [ 1488] 
       VMVW              vr01, vr06, 128                                                    // BLK_REG UfrmIdx [ 1489] 
       VDMAOSET          dr2, k14                                                                   // BLK_REG [ 1490] 
	
	// SDM start address	
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1493".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr03, 192                                                   // sdmAddr USdmAddr [ 1493] 
       VDMAOSET          dr0, k6                                                                    // sdmAddr [ 1494] 
	
	// LOC_X LOC_Y
       VASRW.3           vr00, vr00, 1                                                  // RowColumn RowColumn [ 1497] 
       VDMAORUN          i0, i1                                                                 // loc_x loc_y [ 1498] 
	
	
	// V output
	
	// SDM start address	
// ****** Warning : Masks for operands 1 & 2 do not match *****
LINE 1504".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMVW              vr00, vr04, 192                                                   // sdmAddr VSdmAddr [ 1504] 
       VDMAOSET          dr0, k6                                                                    // sdmAddr [ 1505] 
	
       VMVW              vr01, vr07, 128                                                    // BLK_REG VfrmIdx [ 1507] 
       VDMAOSET          dr2, k14                                                                   // BLK_REG [ 1508] 
	
	// LOC_X LOC_Y
       VDMAORUN          i0, i1                                                                 // loc_x loc_y [ 1511] 
	

// End                                                                                                      // [ 1514] 

  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc WriteOutputBlock                                                                                 // [ 1516] 





// [Macro] CallBackReleaseBuffer                                                                            // [ 1520] 
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
  ld r1,[r17 ,36]
  vrec r1

    // Params -----------
// p16    i0  BufIdx                                                                                        // [ 1522] 
    // End params -------

    // Send channel cmd
   mov                 r0, MacroSetting_ChannelNum_MP01ToArc
   ld                  r0,[r0,0]
LINE 1529".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VSEND             r0, i0, 0                                                                   // BufIdx [ 1529] 
   mov                 r1, Service_ReleaseBuffer   // Arc routine to call when complete
       VSENDR            r0, r1, 63                                                                         // [ 1531] 


  vendrec r0 ; [Macro]
  add r0,r0,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CallBackReleaseBuffer                                                                            // [ 1534] 





//----------------------------------------------------------------------------------------------------
// Is Macro
// Over-writes fixed buffer flags from circular buffer.
// This makes them invalid when read from macro block to right!!!!
// Moves deblocked version of pixel data up to fixed buffer
// Sends channel reply
// [Macro] CopyToFixedAfterDeblockH264                                                                      // [ 1544] 
_Asm int Record_CopyToFixedAfterDeblockH264(int *mac_tab)
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
CopyToFixedAfterDeblockH264_RecLoop:
  ld r1,[r17 ,40]
  vrec r1

    // Params -----------
// p16    i0  cIdx                                                                                          // [ 1546] // Circular buffer index
// p16    i1  Column                                                                                        // [ 1547] // MacroBlock column
    // End params -------
// Setup pointers
// p16    i2  Current                                                                                       // [ 1550] // Current circular buffer
// p16    i3  Above                                                                                         // [ 1551] // Above fixed buffer
LINE 1552".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VMULW.1           vr00, vr00, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY                               // cIdx cIdx [ 1552] 
       VMULW.2           vr00, vr00, MPO_SIZE_OF_FIXED_ENTRY                                  // Column Column [ 1553] 
//	vdmawait			0x7f,0												------ Not needed unless number of circular buffers is reduced to 6 or less
       VIM               i2, i0, SDMOF_CircularBuffs                                           // Current cIdx [ 1555] 
       VIM               i3, i1, SDMOF_FixedBuffs                                              // Above Column [ 1556] 

// Begin                                                                                                    // [ 1558] 
//vec16  vr08  NeighbourFlagsEtc                                                                            // [ 1559] 
//vec16  vr09  MotionVecs                                                                                   // [ 1559] 
//vec16  vr10  HorPix0L                                                                                     // [ 1560] 
//vec16  vr11  HorPix1L                                                                                     // [ 1560] 
//vec16  vr12  HorPix2L                                                                                     // [ 1560] 
//vec16  vr13  HorPix3L                                                                                     // [ 1560] 
//vec16  vr14  HorPix0H                                                                                     // [ 1560] 
//vec16  vr15  HorPix1H                                                                                     // [ 1560] 
//vec16  vr16  HorPix2H                                                                                     // [ 1560] 
//vec16  vr17  HorPix3H                                                                                     // [ 1560] 

LINE 1562".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD128            vr08,[i2, MPO_LeftMB]                                    // NeighbourFlagsEtc Current [ 1562] // Flags etc
       VLD128            vr09,[i2, MPO_mv]                                               // MotionVecs Current [ 1563] // Notion vectors

    // Horizontal pixel overlap buffer
       VLD64             vr10,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-4)+8+0] // HorPix0L Current [ 1566] 
       VLD64             vr14,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-4)+8+8] // HorPix0H Current [ 1567] 
       VLD64             vr11,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-3)+8+0] // HorPix1L Current [ 1568] 
       VLD64             vr15,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-3)+8+8] // HorPix1H Current [ 1569] 
       VLD64             vr12,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-2)+8+0] // HorPix2L Current [ 1570] 
       VLD64             vr16,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-2)+8+8] // HorPix2H Current [ 1571] 
       VLD64             vr13,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-1)+8+0] // HorPix3L Current [ 1572] 
       VLD64             vr17,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-1)+8+8] // HorPix3H Current [ 1573] 

    // Store all out to above versions in fixed buffer
       VST128            vr08,[i3, MPO_LeftMB]                                      // NeighbourFlagsEtc Above [ 1576] 
       VST128            vr09,[i3, MPO_mv]                                                 // MotionVecs Above [ 1577] 
       VST64             vr10,[i3, MPO_Y_HorizPixelStore0+0]                                 // HorPix0L Above [ 1578] 
       VST64             vr14,[i3, MPO_Y_HorizPixelStore0+8]                                 // HorPix0H Above [ 1579] 
       VST64             vr11,[i3, MPO_Y_HorizPixelStore0+16]                                // HorPix1L Above [ 1580] 
       VST64             vr15,[i3, MPO_Y_HorizPixelStore0+24]                                // HorPix1H Above [ 1581] 
       VST64             vr12,[i3, MPO_Y_HorizPixelStore0+32]                                // HorPix2L Above [ 1582] 
       VST64             vr16,[i3, MPO_Y_HorizPixelStore0+40]                                // HorPix2H Above [ 1583] 
       VST64             vr13,[i3, MPO_Y_HorizPixelStore0+48]                                // HorPix3L Above [ 1584] 
       VST64             vr17,[i3, MPO_Y_HorizPixelStore0+56]                                // HorPix3H Above [ 1585] 
// End                                                                                                      // [ 1586] 

// Begin                                                                                                    // [ 1588] 
    // Move over chrom pixel data overlap
//vec16  vr08  PixU0                                                                                        // [ 1590] 
//vec16  vr09  PixV0                                                                                        // [ 1590] 
//vec16  vr10  PixU1                                                                                        // [ 1590] 
//vec16  vr11  PixV1                                                                                        // [ 1590] 
// p16    i7  LeftAboveFlags                                                                                // [ 1591] 
// s16    vr02.128  IsValid                                                                                 // [ 1592] 

LINE 1594".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VLD8W_7           vr00,[i2, MPO_LeftMB]                                       // LeftAboveFlags Current [ 1594] 
       VMOVW             vr02, MBNF_VALID, 128                                                      // IsValid [ 1595] 

       VLD32             vr08,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-2)+4] // PixU0 Current [ 1597] 
       VLD32_2           vr08,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-2)+4+4] // PixU0 Current [ 1598] 
       VLD32             vr10,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-1)+4] // PixU1 Current [ 1599] 
       VLD32_2           vr10,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-1)+4+4] // PixU1 Current [ 1600] 

       VAND.128          vr00, vr00, vr02                             // LeftAboveFlags LeftAboveFlags IsValid [ 1602] // Left above exist?

       VLD32             vr09,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-2)+4] // PixV0 Current [ 1604] 
       VLD32_2           vr09,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-2)+4+4] // PixV0 Current [ 1605] 
       VLD32             vr11,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-1)+4] // PixV1 Current [ 1606] 
       VLD32_2           vr11,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-1)+4+4] // PixV1 Current [ 1607] 

       VST64             vr08,[i3, MPO_U_HorizPixelStore0]                                      // PixU0 Above [ 1609] 
       VJP!i7            r3                                                             // LeftAboveFlags exit [ 1610] 
       VST64             vr10,[i3, MPO_U_HorizPixelStore1]  //DS__________________________________ PixU1 Above [ 1611] 
       VST64             vr09,[i3, MPO_V_HorizPixelStore0]  //DS__________________________________ PixV0 Above [ 1612] 
       VST64             vr11,[i3, MPO_V_HorizPixelStore1]  //DS__________________________________ PixV1 Above [ 1613] 

// End                                                                                                      // [ 1615] 

// Begin                                                                                                    // [ 1617] 
    // Fix up bottom right corner pixel data for the macro block to the top left,
    // as could have been modified by deblock
    // Copies from current buffer to left above
// p16    i7  leftAbove                                                                                     // [ 1621] 
// s32    vr03.12  luma0                                                                                    // [ 1622] 
// s32    vr03.48  luma1                                                                                    // [ 1622] 
// s32    vr03.192  luma2                                                                                   // [ 1622] 
// s32    vr08.3  luma3                                                                                     // [ 1622] 
// s16    vr02.128  chromaU0                                                                                // [ 1623] 
// s16    vr03.2  chromaU1                                                                                  // [ 1623] 
// s16    vr08.4  chromaV0                                                                                  // [ 1623] 
// s16    vr08.8  chromaV1                                                                                  // [ 1623] 

LINE 1625".\ARCEncoder\H264\ArcDeblock_MPA.sss"
       VIM               i7, i3, -MPO_SIZE_OF_FIXED_ENTRY                                   // leftAbove Above [ 1625] 
       VLD32_2           vr03,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-4)+4]    // luma0 Current [ 1626] 
       VLD32_4           vr03,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-3)+4]    // luma1 Current [ 1627] 
       VLD32_6           vr03,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-2)+4]    // luma2 Current [ 1628] 
       VLD32             vr08,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_STRIDE*(20-1)+4]    // luma3 Current [ 1629] 

       VLD16_7           vr02,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-2)+2] // chromaU0 Current [ 1631] 
       VLD16_1           vr03,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_U+PCB_RECONSTRUCTED_STRIDE*(12-1)+2] // chromaU1 Current [ 1632] 
       VLD16_2           vr08,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-2)+2] // chromaV0 Current [ 1633] 
       VLD16_3           vr08,[i2, MPO_PixelCoeffBuffer+PCB_RECONSTRUCTED_V+PCB_RECONSTRUCTED_STRIDE*(12-1)+2] // chromaV1 Current [ 1634] 

       VST32_2           vr03,[i7, MPO_Y_HorizPixelStore0+12]                               // luma0 leftAbove [ 1636] 
       VST32_4           vr03,[i7, MPO_Y_HorizPixelStore1+12]                               // luma1 leftAbove [ 1637] 
       VST32_6           vr03,[i7, MPO_Y_HorizPixelStore2+12]                               // luma2 leftAbove [ 1638] 
       VST32             vr08,[i7, MPO_Y_HorizPixelStore3+12]                               // luma3 leftAbove [ 1639] 

       VST16_7           vr02,[i7, MPO_U_HorizPixelStore0+6]                             // chromaU0 leftAbove [ 1641] 
       VST16_2           vr08,[i7, MPO_V_HorizPixelStore0+6]                             // chromaV0 leftAbove [ 1642] 
       VST16_1           vr03,[i7, MPO_U_HorizPixelStore1+6]                             // chromaU1 leftAbove [ 1643] 
       VST16_3           vr08,[i7, MPO_V_HorizPixelStore1+6]                             // chromaV1 leftAbove [ 1644] 
// End                                                                                                      // [ 1645] 


//exit --------                                                                                             // [ 1648] 
 vendrec r3                                                                                                 // [ 1648] 
 vrec r3                                                                                                    // [ 1648] 


  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz CopyToFixedAfterDeblockH264_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc CopyToFixedAfterDeblockH264                                                                      // [ 1651] 
















const int MACRO_IDX_SetCurrentMPC1 = 0;
const int MACRO_IDX_CopyToFixedAfterReconstructH264 = 1;
const int MACRO_IDX_DeblockMacroBlockSetup = 2;
const int MACRO_IDX_DeblockFetchPixelsFromNeigbours = 3;
const int MACRO_IDX_HorizontalDeblock = 4;
const int MACRO_IDX_VerticalDeblock = 5;
const int SIMD_IDX_DmaOutPixelData = 6;
const int MACRO_IDX_WriteDeblockedBlock = 7;
const int MACRO_IDX_WriteOutputBlock = 8;
const int MACRO_IDX_CallBackReleaseBuffer = 9;
const int MACRO_IDX_CopyToFixedAfterDeblockH264 = 10;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Deblock_Record(int sa, int SIMDnum)
{
    int *map = Deblock;

    SetSCQ(SIMDnum);

    map[MACRO_IDX_SetCurrentMPC1] = sa;
    sa = Record_SetCurrentMPC1(map);

    map[MACRO_IDX_CopyToFixedAfterReconstructH264] = sa;
    sa = Record_CopyToFixedAfterReconstructH264(map);

    map[MACRO_IDX_DeblockMacroBlockSetup] = sa;
    sa = Record_DeblockMacroBlockSetup(map);

    map[MACRO_IDX_DeblockFetchPixelsFromNeigbours] = sa;
    sa = Record_DeblockFetchPixelsFromNeigbours(map);

    map[MACRO_IDX_HorizontalDeblock] = sa;
    sa = Record_HorizontalDeblock(map);

    map[MACRO_IDX_VerticalDeblock] = sa;
    sa = Record_VerticalDeblock(map);

    map[SIMD_IDX_DmaOutPixelData] = sa;
    sa = Record_DmaOutPixelData(map);

    map[MACRO_IDX_WriteDeblockedBlock] = sa;
    sa = Record_WriteDeblockedBlock(map);

    map[MACRO_IDX_WriteOutputBlock] = sa;
    sa = Record_WriteOutputBlock(map);

    map[MACRO_IDX_CallBackReleaseBuffer] = sa;
    sa = Record_CallBackReleaseBuffer(map);

    map[MACRO_IDX_CopyToFixedAfterDeblockH264] = sa;
    sa = Record_CopyToFixedAfterDeblockH264(map);

    return sa;
}
