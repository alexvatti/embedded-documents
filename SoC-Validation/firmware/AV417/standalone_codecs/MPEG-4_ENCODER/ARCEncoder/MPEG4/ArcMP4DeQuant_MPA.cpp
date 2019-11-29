//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss'

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


//	showstalls
//	vec16	Scalars0 = vr00, Scalars1 = vr01

// MetaWare ouput ON                                                  // [   21] 
#define LINE #line                                                    // [   21] 
    

// ScanMP: Including file .\ARCEncoder\MPEG4\..\ARC\SIMD_ABI.ii.
// (c) Arc international 2007
// Creo vrapter ABI using MPC data structures
// To co-operate with ME & EE hardware

// Media processor ABI ---------------------------
//	vec16				Scalars0 = vr00
//	vec16				Scalars1 = vr01
//	**	i0 - i3			Scalar params/scratch
// p16    i4  CirBuf                                                  // [    9] 
// p16    i5  FixBuf                                                  // [   10] 
// p16    i6  CirLeft                                                 // [   11] 
//	**	i7				scratch
// 	**	i8 - i15 		Work - should be preserved
//	**	vr01 - vr04		params
//	**	vr05 - vr30		scratch
//vec16  vr62  VZERO                                                  // [   16] 
//vec16  vr31  VSTACK                                                 // [   17] 
//vec16  vr63  VFLAGS                                                 // [   18] 

// Media processor ABI end -----------------------


// -----------------------------------------------
// CirBuf, FixBuf , CirLeft are set by  "SetCurrentMPC" in ArcSDMTables_MPA.sss
// -----------------------------------------------


// ScanMP: End of included file ..\ARC\SIMD_ABI.ii.

	
	
#include ".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.h"


// Macro table
    int MP4DeQuant[8];
    int MP4DeQuant_Record(int sa, int SIMDnum);
	
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"    
    
//#include "../MPEG4/ArcIDCT_MPA.h"

extern int Idct[] ;
//------------------------------------------------------------------------------
// Name:          MP4DeQuant
// Purpose:       provide the dequantisation step for the 6 blocks of an MPEG4 MB 
// Arguments:     none
// Return Value:  void
//------------------------------------------------------------------------------
      
// [Macro] MP4DeQuant                                                 // [   45] 
_Asm int Record_MP4DeQuant(int *mac_tab)
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
MP4DeQuant_RecLoop:
  ld r1,[r17 ,4]
  vrec r1



	
// Begin                                                              // [   49] 
// p16    i0  blkInOffset                                             // [   50] // i0

// p16    i2  Intra                                                   // [   52] // i2 1 or 0
//vec16  vr02  row0                                                   // [   53] 
//vec16  vr03  row1                                                   // [   54] 
//vec16  vr04  row2                                                   // [   55] 
//vec16  vr05  row3                                                   // [   56] 
//vec16  vr06  row4                                                   // [   57] 
//vec16  vr07  row5                                                   // [   58] 
//vec16  vr08  row6                                                   // [   59] 
//vec16  vr09  row7                                                   // [   60] 
//vec16  vr10  scale0                                                 // [   61] 
//vec16  vr11  scale1                                                 // [   61] 
//vec16  vr12  scale2                                                 // [   61] 
//vec16  vr13  scale3                                                 // [   61] 
//vec16  vr14  scale4                                                 // [   61] 
//vec16  vr15  scale5                                                 // [   61] 
//vec16  vr16  scale6                                                 // [   61] 
//vec16  vr17  scale7                                                 // [   61] 
//vec16  vr18  sign0                                                  // [   62] 
//vec16  vr19  sign1                                                  // [   62] 
//vec16  vr20  sign2                                                  // [   62] 
//vec16  vr21  sign3                                                  // [   62] 
//vec16  vr22  sign4                                                  // [   62] 
//vec16  vr23  sign5                                                  // [   62] 
//vec16  vr24  sign6                                                  // [   62] 
//vec16  vr25  sign7                                                  // [   62] 



// p16    i1  QuantVal                                                // [   66] 
//vec16  vr26  QuantVec                                               // [   67] 
// p16    i10  temp                                                   // [   68] 
// p16    i7  cirPtr                                                  // [   69] 
// s16    vr27.4  mode                                                // [   70] 
//vec16  vr28  Inter                                                  // [   71] 
// p16    i3  dcterm                                                  // [   72] 
// p16    i8  block_no                                                // [   73] 
// p16    i9  blockdouble                                             // [   74] 
// s16    vr29.4  cbp                                                 // [   75] 
// p16    i11  zero                                                   // [   76] 
// p16    i12  hasAC                                                  // [   77] 

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0|cirPtr| CirL.|FixBuf|CirBuf|dcterm| Intra| Quan.| blkI.|
//|VR 1|      |      |      | hasAC|  zero|  temp| bloc.| bloc.|
//|VR 2|                        row0                           |
//|VR 3|                        row1                           |
//|VR 4|                        row2                           |
//|VR 5|                        row3                           |
//|VR 6|                        row4                           |
//|VR 7|                        row5                           |
//|VR 8|                        row6                           |
//|VR 9|                        row7                           |
//|VR10|                      scale0                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                       sign0                           |
//|VR19|                       sign1                           |
//|VR20|                       sign2                           |
//|VR21|                       sign3                           |
//|VR22|                       sign4                           |
//|VR23|                       sign5                           |
//|VR24|                       sign6                           |
//|VR25|                       sign7                           |
//|VR26|                    QuantVec                           |
//|VR27|      |      |      |      |      |  mode|      |      |
//|VR28|                       Inter                           |
//|VR29|      |      |      |      |      |   cbp|      |      |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

    
LINE 81".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VMOVW             vr01, 0, 8                              // zero [   81] 
       VMOVW             vr01, 128, 4                            // temp [   82] // offset for Inter scaling tables
   
       VLD8W_2           vr00,[i4, MPO_MBCodingType]     // Intra CirBuf [   84] 
       VAND.4            vr00, vr00, MBCT_MPEG4_INTRA     // Intra Intra [   85] 
       VLD16_2           vr29,[i4, MPO_NonZeroCoeffLuma]   // cbp CirBuf [   86] // load cbp
       VSNEW.4           vr00, vr62                       // Intra VZERO [   87] 
       VMOVW.4.S         vr01, 0                                 // temp [   88] // offset for INtra scaling tables
       VLD8W_1           vr00,[i4, MPO_QuantValue]    // QuantVal CirBuf [   89] 
       VMIVW.255         vr26, i1                   // QuantVec QuantVal [   90] 
       
       VIM               i7, i10, SDMOF_mpeg4_dct_dequant_scaling_table // cirPtr temp [   92] // inter

     // load quantization coefficients here as they are the same for all blocks
       VLD128            vr10,[i7, 0x00]                // scale0 cirPtr [   95] 
       VLD128            vr11,[i7, 0x10]                // scale1 cirPtr [   96] 
       VLD128            vr12,[i7, 0x20]                // scale2 cirPtr [   97] 
       VLD128            vr13,[i7, 0x30]                // scale3 cirPtr [   98] 
       VLD128            vr14,[i7, 0x40]                // scale4 cirPtr [   99] 
       VLD128            vr15,[i7, 0x50]                // scale5 cirPtr [  100] 
       VLD128            vr16,[i7, 0x60]                // scale6 cirPtr [  101] 
       VLD128            vr17,[i7, 0x70]                // scale7 cirPtr [  102] 
    
    // multiply by quant, to avoid this calculation for each of the 6 blocks
       VMULW             vr10, vr10, vr26      // scale0 scale0 QuantVec [  105] 
       VMULW             vr11, vr11, vr26      // scale1 scale1 QuantVec [  106] 
       VMULW             vr12, vr12, vr26      // scale2 scale2 QuantVec [  107] 
       VMULW             vr13, vr13, vr26      // scale3 scale3 QuantVec [  108] 
       VMULW             vr14, vr14, vr26      // scale4 scale4 QuantVec [  109] 
       VMULW             vr15, vr15, vr26      // scale5 scale5 QuantVec [  110] 
       VMULW             vr16, vr16, vr26      // scale6 scale6 QuantVec [  111] 
       VMULW             vr17, vr17, vr26      // scale7 scale7 QuantVec [  112] 
 
       VMOVW             vr01, 0, 1                          // block_no [  114] 

//DeQuantLoop --------                                                // [  116] 
 vendrec r3                                                           // [  116] 
 vrec r3                                                              // [  116] 
        VMR7W.2         vr01, vr01, vr01         // blockdouble block_no [  117] 
LINE 118".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VADDW.2           vr01, vr01, vr01 // blockdouble blockdouble blockdouble [  118] // needed for indexing short arrays

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0|cirPtr| CirL.|FixBuf|CirBuf|dcterm| Intra| Quan.| blkI.|
//|VR 1|      |      |      | hasAC|  zero|  temp| bloc.| bloc.|
//|VR 2|                        row0                           |
//|VR 3|                        row1                           |
//|VR 4|                        row2                           |
//|VR 5|                        row3                           |
//|VR 6|                        row4                           |
//|VR 7|                        row5                           |
//|VR 8|                        row6                           |
//|VR 9|                        row7                           |
//|VR10|                      scale0                           |
//|VR11|                      scale1                           |
//|VR12|                      scale2                           |
//|VR13|                      scale3                           |
//|VR14|                      scale4                           |
//|VR15|                      scale5                           |
//|VR16|                      scale6                           |
//|VR17|                      scale7                           |
//|VR18|                       sign0                           |
//|VR19|                       sign1                           |
//|VR20|                       sign2                           |
//|VR21|                       sign3                           |
//|VR22|                       sign4                           |
//|VR23|                       sign5                           |
//|VR24|                       sign6                           |
//|VR25|                       sign7                           |
//|VR26|                    QuantVec                           |
//|VR27|      |      |      |      |      |  mode|      |      |
//|VR28|                       Inter                           |
//|VR29|      |      |      |      |      |   cbp|      |      |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

//  now check if we need to dequant this block
// Begin                                                              // [  122] 

        VMR6W.4         vr27, vr01, vr01                // mode block_no [  124] 
LINE 125".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VIM               i10, i11, 2048                     // temp zero [  125] // temp = 1<<11
       VLSRVW.4          vr01, vr01, vr27              // temp temp mode [  126] // temp =  1<<(11-block_no)
       VAND.4            vr01, vr29, vr01               // temp cbp temp [  127] // see if relevant bit of cbp is set
        VMR6W.16        vr01, vr01, vr01                   // hasAC temp [  128] 
LINE 129".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VOR.4             vr01, vr01, vr00             // temp temp Intra [  129] // if cbp is set for this block or we have an Intra MB
// End                                                                // [  130] 
 // note that the Intra dc term has already been dequantised, but because of mismatch control, we still
 // have to do this

    // now we know we are either Intra mode or have AC coeffs (or both)

LINE 136".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VIM               i7, i9, SDMOF_mpeg4_diff_pixel_offset // cirPtr blockdouble [  136] 
       VLD16             vr00,[i7, 0]              // blkInOffset cirPtr [  137] 
       VIM               i7, i4, MPO_PixelCoeffBuffer   // cirPtr CirBuf [  138] 
       VADDW.128         vr00, vr00, i0     // cirPtr cirPtr blkInOffset [  139] 
    
       VLD128            vr02,[i7, 0]                     // row0 cirPtr [  141] // load 8 shorts from input
       VLD128            vr03,[i7, PCB_RESIDUAL_STRIDE]   // row1 cirPtr [  142] // load next 8 shorts from input
       VLD128            vr04,[i7, PCB_RESIDUAL_STRIDE*2] // row2 cirPtr [  143] // load next 8 shorts from input
       VLD128            vr05,[i7, PCB_RESIDUAL_STRIDE*3] // row3 cirPtr [  144] // load next 8 shorts from input
       VLD128            vr06,[i7, PCB_RESIDUAL_STRIDE*4] // row4 cirPtr [  145] // load next 8 shorts from input

 // skip to end of loop if dequant not required
 
       VJP!i10           r4                         // temp InterPredict [  149] // don't do anything if not INtra and no CBP
       VLD128            vr07,[i7, PCB_RESIDUAL_STRIDE*5] //DS row5 cirPtr [  150] // load next 8 shorts from input
       VLD128            vr08,[i7, PCB_RESIDUAL_STRIDE*6] //DS row6 cirPtr [  151] // load next 8 shorts from input
       VLD128            vr09,[i7, PCB_RESIDUAL_STRIDE*7] //DS row7 cirPtr [  152] // load next 8 shorts from input
    
    
       VJP!i12           r5                            // hasAC MisMatch [  155] // no AC coeffs and Intra mode (otherwise we would have gone to SkipBlock)
                          // so only need to do mismatch control
       VNOP                //DS_________________________________________ [  157] 
       VMOVW             vr28, 0, 255  //DS_______________________ Inter [  158] 
        vmvw.4          vr01, vr00  //DS_____________________ temp Intra [  159] 
    
LINE 161".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VJP.i10           r6                              // temp GoIntra [  161] 
       VNOP                //DS_________________________________________ [  162] 
       VNOP                //DS_________________________________________ [  163] 
       VNOP                //DS_________________________________________ [  164] 
    
       VJP               r7                                   // dequant [  166] 
       VMOVW             vr28, 1, 255  //DS_______________________ Inter [  167] 
       VNOP                //DS_________________________________________ [  168] 
       VNOP                //DS_________________________________________ [  169] 

    
//GoIntra --------                                                    // [  172] 
 vendrec r6                                                           // [  172] 
 vrec r6                                                              // [  172] 
LINE 173".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VMOVW             vr28, 0, 255                           // Inter [  173] 
 // save dcterm - already dealt with
       VIV.1             i3, vr02                         // dcterm row0 [  175] 
       VMOVW             vr28, 0, 255                           // Inter [  176] 
      
 
//dequant --------                                                    // [  179] 
 vendrec r7                                                           // [  179] 
 vrec r7                                                              // [  179] 

    // save sign of each element
LINE 182".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VSIGNW            vr18, vr02                        // sign0 row0 [  182] 
       VSIGNW            vr19, vr03                        // sign1 row1 [  183] 
       VSIGNW            vr20, vr04                        // sign2 row2 [  184] 
       VSIGNW            vr21, vr05                        // sign3 row3 [  185] 
       VSIGNW            vr22, vr06                        // sign4 row4 [  186] 
       VSIGNW            vr23, vr07                        // sign5 row5 [  187] 
       VSIGNW            vr24, vr08                        // sign6 row6 [  188] 
       VSIGNW            vr25, vr09                        // sign7 row7 [  189] 
    
    // make all coeffs positive
 // abs(coeff)
       VABSW             vr02, vr02                         // row0 row0 [  193] 
       VABSW             vr03, vr03                         // row1 row1 [  194] 
       VABSW             vr04, vr04                         // row2 row2 [  195] 
       VABSW             vr05, vr05                         // row3 row3 [  196] 
       VABSW             vr06, vr06                         // row4 row4 [  197] 
       VABSW             vr07, vr07                         // row5 row5 [  198] 
       VABSW             vr08, vr08                         // row6 row6 [  199] 
       VABSW             vr09, vr09                         // row7 row7 [  200] 
// mult by 2
// 2*(abs(coeff))
       VADDW             vr02, vr02, vr02              // row0 row0 row0 [  203] 
       VADDW             vr03, vr03, vr03              // row1 row1 row1 [  204] 
       VADDW             vr04, vr04, vr04              // row2 row2 row2 [  205] 
       VADDW             vr05, vr05, vr05              // row3 row3 row3 [  206] 
       VADDW             vr06, vr06, vr06              // row4 row4 row4 [  207] 
       VADDW             vr07, vr07, vr07              // row5 row5 row5 [  208] 
       VADDW             vr08, vr08, vr08              // row6 row6 row6 [  209] 
       VADDW             vr09, vr09, vr09              // row7 row7 row7 [  210] 
    
 // add 1 if Inter
 // 2*(abs(coeff)) + IsInter
       VADDW             vr02, vr02, vr28             // row0 row0 Inter [  214] 
       VADDW             vr03, vr03, vr28             // row1 row1 Inter [  215] 
       VADDW             vr04, vr04, vr28             // row2 row2 Inter [  216] 
       VADDW             vr05, vr05, vr28             // row3 row3 Inter [  217] 
       VADDW             vr06, vr06, vr28             // row4 row4 Inter [  218] 
       VADDW             vr07, vr07, vr28             // row5 row5 Inter [  219] 
       VADDW             vr08, vr08, vr28             // row6 row6 Inter [  220] 
       VADDW             vr09, vr09, vr28             // row7 row7 Inter [  221] 
    
// perform scaling and quantization on all coefficients 
// (2*(abs(coeff)) + IsInter)*quant*matrix  
       VMULW             vr02, vr02, vr10            // row0 row0 scale0 [  225] 
       VMULW             vr03, vr03, vr11            // row1 row1 scale1 [  226] 
       VMULW             vr04, vr04, vr12            // row2 row2 scale2 [  227] 
       VMULW             vr05, vr05, vr13            // row3 row3 scale3 [  228] 
       VMULW             vr06, vr06, vr14            // row4 row4 scale4 [  229] 
       VMULW             vr07, vr07, vr15            // row5 row5 scale5 [  230] 
       VMULW             vr08, vr08, vr16            // row6 row6 scale6 [  231] 
       VMULW             vr09, vr09, vr17            // row7 row7 scale7 [  232] 

	
// shift down by 4
//(2*(abs(coeff)) + IsInter)*quant*matrix >>4
       VASRW             vr02, vr02, 4                      // row0 row0 [  237] 
       VASRW             vr03, vr03, 4                      // row1 row1 [  238] 
       VASRW             vr04, vr04, 4                      // row2 row2 [  239] 
       VASRW             vr05, vr05, 4                      // row3 row3 [  240] 
       VASRW             vr06, vr06, 4                      // row4 row4 [  241] 
       VASRW             vr07, vr07, 4                      // row5 row5 [  242] 
       VASRW             vr08, vr08, 4                      // row6 row6 [  243] 
       VASRW             vr09, vr09, 4                      // row7 row7 [  244] 
	
	// reapply sign
       VMULW             vr02, vr02, vr18             // row0 row0 sign0 [  247] 
       VMULW             vr03, vr03, vr19             // row1 row1 sign1 [  248] 
       VMULW             vr04, vr04, vr20             // row2 row2 sign2 [  249] 
       VMULW             vr05, vr05, vr21             // row3 row3 sign3 [  250] 
       VMULW             vr06, vr06, vr22             // row4 row4 sign4 [  251] 
       VMULW             vr07, vr07, vr23             // row5 row5 sign5 [  252] 
       VMULW             vr08, vr08, vr24             // row6 row6 sign6 [  253] 
       VMULW             vr09, vr09, vr25             // row7 row7 sign7 [  254] 
	
   // if Intra mode, put back the saved dc term
       VSEQW             vr28, 0                                // Inter [  257] // Inter holds all 1 if Inter, all 0 if Intra
       VMIVW.1.S         vr02, i3                         // row0 dcterm [  258] 
    
//MisMatch --------                                                   // [  260] 
 vendrec r5                                                           // [  260] 
 vrec r5                                                              // [  260] 
  // mismatch control  using signs as working arrays
LINE 262".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VADDW             vr18, vr02, vr03             // sign0 row0 row1 [  262] 
       VADDW             vr19, vr04, vr05             // sign1 row2 row3 [  263] 
       VADDW             vr20, vr06, vr07             // sign2 row4 row5 [  264] 
       VADDW             vr21, vr08, vr09             // sign3 row6 row7 [  265] 
       VADDW             vr22, vr18, vr19           // sign4 sign0 sign1 [  266] 
       VADDW             vr23, vr20, vr21           // sign5 sign2 sign3 [  267] 
       VADDW             vr22, vr22, vr23           // sign4 sign4 sign5 [  268] 
       VXSUMW            vr23, vr22, 255                  // sign5 sign4 [  269] // every element contains sum of all coeffs
    
       VXOR              vr24, vr24, vr24           // sign6 sign6 sign6 [  271] // clear v15
    
    
     
    // now call IDCT for this block
  ld r8,[r17,0];                                     // VSTACK IDCTBlock [  276] 
       VJL               vr31, r8                                     // [  276] 
       VMOVW             vr24, 1, 128  //DS_______________________ sign6 [  277] // put a 1 in lane 7
       VBIC              vr18, vr24, vr23  //DS_______ sign0 sign6 sign5 [  278] // if lowest bit of sum not set
    
       VXOR              vr09, vr09, vr18  //DS_________ row7 row7 sign0 [  280] // twiddle lowest bit of highest freq coeff
    
       VJP.i2            r9                           // Intra SaveIntra [  282] 
    
//InterPredict --------  //DS___________________________________________ [  284] 
 vendrec r4  //DS_______________________________________________________ [  284] 
 vrec r4  //DS__________________________________________________________ [  284] 
    // calculate the address of the predicted data
LINE 286".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VIM               i10, i9, SDMOF_mpeg4_pred_pixel_offset //DS temp blockdouble [  286] // calc offset of lookup table
       VLD16             vr00,[i10, 0]  //DS___________ blkInOffset temp [  287] // read offset into MP0_PixelCoeffBuffer
       VIM               i10, i4, MPO_PixelCoeffBuffer  //DS temp CirBuf [  288] 
       VADDW.4           vr01, vr01, i0         // temp temp blkInOffset [  289] 
    // inter need to add prediction
    //load predicted data
       VLD64W            vr18,[i10, 0*16]                  // sign0 temp [  292] 
       VLD64W            vr19,[i10, 1*16]                  // sign1 temp [  293] 
       VLD64W            vr20,[i10, 2*16]                  // sign2 temp [  294] 
       VLD64W            vr21,[i10, 3*16]                  // sign3 temp [  295] 
       VLD64W            vr22,[i10, 4*16]                  // sign4 temp [  296] 
       VLD64W            vr23,[i10, 5*16]                  // sign5 temp [  297] 
       VLD64W            vr24,[i10, 6*16]                  // sign6 temp [  298] 
       VLD64W            vr25,[i10, 7*16]                  // sign7 temp [  299] 
    //add prediction to inverse DCT
       VADDW             vr02, vr02, vr18             // row0 row0 sign0 [  301] 
       VADDW             vr03, vr03, vr19             // row1 row1 sign1 [  302] 
       VADDW             vr04, vr04, vr20             // row2 row2 sign2 [  303] 
       VADDW             vr05, vr05, vr21             // row3 row3 sign3 [  304] 
       VADDW             vr06, vr06, vr22             // row4 row4 sign4 [  305] 
       VADDW             vr07, vr07, vr23             // row5 row5 sign5 [  306] 
       VADDW             vr08, vr08, vr24             // row6 row6 sign6 [  307] 
       VADDW             vr09, vr09, vr25             // row7 row7 sign7 [  308] 
    
//SaveIntra --------                                                  // [  310] 
 vendrec r9                                                           // [  310] 
 vrec r9                                                              // [  310] 
    // set up pointer for saving data
LINE 312".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VIM               i7, i9, SDMOF_mpeg4_final_pixel_offset // cirPtr blockdouble [  312] // calc offset of lookup table
       VLD16             vr00,[i7, 0]              // blkInOffset cirPtr [  313] // read offset into MP0_PixelCoeffBuffer
       VIM               i7, i4, MPO_PixelCoeffBuffer   // cirPtr CirBuf [  314] 
       VADDW.128         vr00, vr00, i0     // cirPtr cirPtr blkInOffset [  315] 
// save to byte and saturate
       VASRPWB           vr02, vr02, 0                      // row0 row0 [  317] 
       VASRPWB           vr03, vr03, 0                      // row1 row1 [  318] 
       VASRPWB           vr04, vr04, 0                      // row2 row2 [  319] 
       VASRPWB           vr05, vr05, 0                      // row3 row3 [  320] 
       VASRPWB           vr06, vr06, 0                      // row4 row4 [  321] 
       VASRPWB           vr07, vr07, 0                      // row5 row5 [  322] 
       VASRPWB           vr08, vr08, 0                      // row6 row6 [  323] 
       VASRPWB           vr09, vr09, 0                      // row7 row7 [  324] 
    
    // store quantized dct coefficients in memory 
       VST64             vr02,[i7, 0]                     // row0 cirPtr [  327] 
       VST64             vr03,[i7, 16]                    // row1 cirPtr [  328] 
       VST64             vr04,[i7, 32]                    // row2 cirPtr [  329] 
       VST64             vr05,[i7, 48]                    // row3 cirPtr [  330] 
       VST64             vr06,[i7, 64]                    // row4 cirPtr [  331] 
       VST64             vr07,[i7, 80]                    // row5 cirPtr [  332] 
       VST64             vr08,[i7, 96]                    // row6 cirPtr [  333] 
       VST64             vr09,[i7, 112]                   // row7 cirPtr [  334] 
       
    


//SkipBlock --------                                                  // [  339] 
 vendrec r10                                                          // [  339] 
 vrec r10                                                             // [  339] 
    
LINE 341".\ARCEncoder\MPEG4\ArcMP4DeQuant_MPA.sss"
       VIM               i8, i8, 1                  // block_no block_no [  341] 
    
    // check to see if we have done all blocks
       VIM               i10, i8, -6                    // temp block_no [  344] 
    
       VJP.i10           r3                          // temp DeQuantLoop [  346] 
       VNOP                //DS_________________________________________ [  347] 
       VNOP                //DS_________________________________________ [  348] 
       VNOP                //DS_________________________________________ [  349] 
	    
// End                                                                // [  351] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz MP4DeQuant_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc MP4DeQuant                                                 // [  352] 



const int MACRO_IDX_MP4DeQuant = 1;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int MP4DeQuant_Record(int sa, int SIMDnum)
{
    int *map = MP4DeQuant;

    SetSCQ(SIMDnum);

    map[0] = Idct[0];

    map[MACRO_IDX_MP4DeQuant] = sa;
    sa = Record_MP4DeQuant(map);

    return sa;
}
