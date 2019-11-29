//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcIDCT_MPA.sss'

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

// MetaWare ouput ON                                                  // [   22] 
#define LINE #line                                                    // [   22] 
    

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

	
	
#include ".\ARCEncoder\MPEG4\ArcIDCT_MPA.h"


// Macro table
    int Idct[8];
    int Idct_Record(int sa, int SIMDnum);
    
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"    
//------------------------------------------------------------------------------
// Name:          IDCTBlock
// Purpose:       provide the Inverse DCT for an individual 8x8 block in situ 
// Arguments:     8 rows of words in vector registers 2 to 9 inclusive
// Return Value:  void
//------------------------------------------------------------------------------
      
// [Subroutine] IDCTBlock                                             // [   43] // in place IDCT
_Asm int Record_IDCTBlock(int *mac_tab)
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

// parameters
//vec16  vr02  row0                                                   // [   45] 
//vec16  vr03  row1                                                   // [   46] 
//vec16  vr04  row2                                                   // [   47] 
//vec16  vr05  row3                                                   // [   48] 
//vec16  vr06  row4                                                   // [   49] 
//vec16  vr07  row5                                                   // [   50] 
//vec16  vr08  row6                                                   // [   51] 
//vec16  vr09  row7                                                   // [   52] 



//vec16  vr22  work8                                                  // [   56] 
//vec16  vr23  work9                                                  // [   57] 
//vec16  vr24  work10                                                 // [   58] 
//vec16  vr25  work11                                                 // [   59] 
 // working vecs, named for compatibility with C code
LINE 61".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VBMULW            vr02, vr02, 16                     // row0 row0 [   61] 
       VBMULW            vr03, vr03, 16                     // row1 row1 [   62] 
       VBMULW            vr04, vr04, 16                     // row2 row2 [   63] 
       VBMULW            vr05, vr05, 16                     // row3 row3 [   64] 
       VBMULW            vr06, vr06, 16                     // row4 row4 [   65] 
       VBMULW            vr07, vr07, 16                     // row5 row5 [   66] 
       VBMULW            vr08, vr08, 16                     // row6 row6 [   67] 
       VBMULW            vr09, vr09, 16                     // row7 row7 [   68] 
    // transpose the matrix
       VEXCH1            vr02, vr03                         // row0 row1 [   70] 
       VEXCH1            vr04, vr05                         // row2 row3 [   71] 
       VEXCH1            vr06, vr07                         // row4 row5 [   72] 
       VEXCH1            vr08, vr09                         // row6 row7 [   73] 
       VEXCH2            vr02, vr04                         // row0 row2 [   74] 
       VEXCH2            vr03, vr05                         // row1 row3 [   75] 
       VEXCH2            vr06, vr08                         // row4 row6 [   76] 
       VEXCH2            vr07, vr09                         // row5 row7 [   77] 
       VEXCH4            vr02, vr06                         // row0 row4 [   78] 
       VEXCH4            vr03, vr07                         // row1 row5 [   79] 
       VEXCH4            vr04, vr08                         // row2 row6 [   80] 
       VEXCH4            vr05, vr09                         // row3 row7 [   81] 


       VBMULFW           vr02, vr02, 23170                  // row0 row0 [   84] 
       VBMULFW           vr22, vr03, 23170                 // work8 row1 [   85] 
       VBMULFW           vr03, vr06, 23170                  // row1 row4 [   86] 
       VBMULFW           vr06, vr09, 23170                  // row4 row7 [   87] 


       VBMULFW           vr09, vr04, 12540                  // row7 row2 [   90] 
       VADDSUW           vr02, vr03                         // row0 row1 [   91] 
       VADDSUW           vr22, vr06                        // work8 row4 [   92] 

 
       VBMULFW           vr23, vr08, 30274                 // work9 row6 [   95] 
       VBMULFW           vr24, vr04, 30274                // work10 row2 [   96] 
       VBMULFW           vr08, vr08, 12540                  // row6 row6 [   97] 
       VSUBW             vr04, vr09, vr23             // row2 row7 work9 [   98] 
       VADDSUW           vr22, vr05                        // work8 row3 [   99] 
       VADDSUW           vr06, vr07                         // row4 row5 [  100] 


       VBMULFW           vr23, vr05, 32138                 // work9 row3 [  103] 


       VBMULFW           vr09, vr05, 6393                   // row7 row3 [  106] 
       VADDW             vr05, vr24, vr08            // row3 work10 row6 [  107] 
       VADDSUW           vr03, vr04                         // row1 row2 [  108] 
       VADDSUW           vr02, vr05                         // row0 row3 [  109] 
       VBMULFW           vr24, vr07, 6393                 // work10 row5 [  110] 
       VBMULFW           vr08, vr07, 32138                  // row6 row5 [  111] 


       VBMULFW           vr25, vr06, 27246                // work11 row4 [  114] 
       VSUBW             vr07, vr23, vr24           // row5 work9 work10 [  115] 
       VADDW             vr08, vr09, vr08              // row6 row7 row6 [  116] 


       VBMULFW           vr23, vr22, 18205                // work9 work8 [  119] 
       VBMULFW           vr09, vr06, 18205                  // row7 row4 [  120] 
       VBMULFW           vr22, vr22, 27246                // work8 work8 [  121] 

       VADDSUW           vr03, vr08                         // row1 row6 [  123] 
       VSUBW             vr06, vr25, vr23           // row4 work11 work9 [  124] 
       VADDW             vr09, vr09, vr22             // row7 row7 work8 [  125] 
       VADDSUW           vr04, vr07                         // row2 row5 [  126] 
       VADDSUW           vr02, vr09                         // row0 row7 [  127] 
       VADDSUW           vr05, vr06                         // row3 row4 [  128] 

       VEXCH1            vr02, vr03                         // row0 row1 [  130] 
       VEXCH1            vr04, vr05                         // row2 row3 [  131] 
       VEXCH1            vr06, vr07                         // row4 row5 [  132] 
       VEXCH1            vr08, vr09                         // row6 row7 [  133] 
       VEXCH2            vr02, vr04                         // row0 row2 [  134] 
       VEXCH2            vr03, vr05                         // row1 row3 [  135] 
       VEXCH2            vr06, vr08                         // row4 row6 [  136] 
       VEXCH2            vr07, vr09                         // row5 row7 [  137] 
       VEXCH4            vr02, vr06                         // row0 row4 [  138] 
       VEXCH4            vr03, vr07                         // row1 row5 [  139] 
       VEXCH4            vr04, vr08                         // row2 row6 [  140] 
       VEXCH4            vr05, vr09                         // row3 row7 [  141] 

 
       VBMULFW           vr02, vr02, 23170                  // row0 row0 [  144] 
       VBMULFW           vr22, vr03, 23170                 // work8 row1 [  145] 
       VBMULFW           vr03, vr06, 23170                  // row1 row4 [  146] 
       VBMULFW           vr06, vr09, 23170                  // row4 row7 [  147] 


       VBMULFW           vr09, vr04, 12540                  // row7 row2 [  150] 
       VADDSUW           vr02, vr03                         // row0 row1 [  151] 
       VADDSUW           vr22, vr06                        // work8 row4 [  152] 


       VBMULFW           vr23, vr08, 30274                 // work9 row6 [  155] 
       VBMULFW           vr24, vr04, 30274                // work10 row2 [  156] 
       VBMULFW           vr08, vr08, 12540                  // row6 row6 [  157] 
       VSUBW             vr04, vr09, vr23             // row2 row7 work9 [  158] 
       VADDSUW           vr22, vr05                        // work8 row3 [  159] 
       VADDSUW           vr06, vr07                         // row4 row5 [  160] 


       VBMULFW           vr23, vr05, 32138                 // work9 row3 [  163] 


       VBMULFW           vr09, vr05, 6393                   // row7 row3 [  166] 
       VADDW             vr05, vr24, vr08            // row3 work10 row6 [  167] 
       VADDSUW           vr03, vr04                         // row1 row2 [  168] 
       VADDSUW           vr02, vr05                         // row0 row3 [  169] 
       VBMULFW           vr24, vr07, 6393                 // work10 row5 [  170] 
       VBMULFW           vr08, vr07, 32138                  // row6 row5 [  171] 

 
       VBMULFW           vr25, vr06, 27246                // work11 row4 [  174] 
       VSUBW             vr07, vr23, vr24           // row5 work9 work10 [  175] 
       VADDW             vr08, vr09, vr08              // row6 row7 row6 [  176] 


       VBMULFW           vr23, vr22, 18205                // work9 work8 [  179] 
       VBMULFW           vr09, vr06, 18205                  // row7 row4 [  180] 
       VBMULFW           vr22, vr22, 27246                // work8 work8 [  181] 
       VADDSUW           vr03, vr08                         // row1 row6 [  182] 
       VSUBW             vr06, vr25, vr23           // row4 work11 work9 [  183] 
       VADDW             vr09, vr09, vr22             // row7 row7 work8 [  184] 

       VADDSUW           vr04, vr07                         // row2 row5 [  186] 
       VADDSUW           vr02, vr09                         // row0 row7 [  187] 
       VADDSUW           vr05, vr06                         // row3 row4 [  188] 

       VASRSRW           vr02, vr02, 6                      // row0 row0 [  190] 
       VASRSRW           vr03, vr03, 6                      // row1 row1 [  191] 
       VASRSRW           vr04, vr04, 6                      // row2 row2 [  192] 
       VASRSRW           vr05, vr05, 6                      // row3 row3 [  193] 
       VASRSRW           vr06, vr06, 6                      // row4 row4 [  194] 
       VASRSRW           vr07, vr07, 6                      // row5 row5 [  195] 
       VASRSRW           vr08, vr08, 6                      // row6 row6 [  196] 
       VASRSRW           vr09, vr09, 6                      // row7 row7 [  197] 
  
   // clip to -256
       VMAXW             vr02, vr02, -256                   // row0 row0 [  200] 
       VMAXW             vr03, vr03, -256                   // row1 row1 [  201] 
       VMAXW             vr04, vr04, -256                   // row2 row2 [  202] 
       VMAXW             vr05, vr05, -256                   // row3 row3 [  203] 
       VMAXW             vr06, vr06, -256                   // row4 row4 [  204] 
       VMAXW             vr07, vr07, -256                   // row5 row5 [  205] 
       VMAXW             vr08, vr08, -256                   // row6 row6 [  206] 
       VMAXW             vr09, vr09, -256                   // row7 row7 [  207] 
   // clip to 255
       VMINW             vr02, vr02, 255                    // row0 row0 [  209] 
       VMINW             vr03, vr03, 255                    // row1 row1 [  210] 
       VMINW             vr04, vr04, 255                    // row2 row2 [  211] 
       VMINW             vr05, vr05, 255                    // row3 row3 [  212] 
       VMINW             vr06, vr06, 255                    // row4 row4 [  213] 
   
     // return to caller
       VJB               vr31, 0                               // VSTACK [  216] 

    // delay slots start
// ****** Warning : Unmarked delay slot found at line number 219 *****
LINE 219".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VMINW             vr07, vr07, 255  //DS________________ row5 row5 [  219] 
// ****** Warning : Unmarked delay slot found at line number 220 *****
LINE 220".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VMINW             vr08, vr08, 255  //DS________________ row6 row6 [  220] 
// ****** Warning : Unmarked delay slot found at line number 221 *****
LINE 221".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VMINW             vr09, vr09, 255  //DS________________ row7 row7 [  221] 
    // delay slots end  
    
  vstoprec r0 // [Subroutine]
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc IDCTBlock                                                  // [  224] 



//------------------------------------------------------------------------------
// Name:          IDCT
// Purpose:       provide the Inverse DCT for the entire MB, avoiding blocks with no coefficients
// Arguments:     requires 8 rows of input word data in vector registers 2 to 9
// Return Value:  void
//------------------------------------------------------------------------------
    
// [Macro] IDCT                                                       // [  233] 
_Asm int Record_IDCT(int *mac_tab)
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
IDCT_RecLoop:
  ld r1,[r17 ,4]
  vrec r1

	
// Begin                                                              // [  235] 

// p16    i0  blkInOffset                                             // [  237] 
// p16    i1  block_no                                                // [  238] 

//vec16  vr02  row0                                                   // [  240] 
//vec16  vr03  row1                                                   // [  241] 
//vec16  vr04  row2                                                   // [  242] 
//vec16  vr05  row3                                                   // [  243] 
//vec16  vr06  row4                                                   // [  244] 
//vec16  vr07  row5                                                   // [  245] 
//vec16  vr08  row6                                                   // [  246] 
//vec16  vr09  row7                                                   // [  247] 
// p16    i2  cirPtr                                                  // [  248] 
// s16    vr10.4  blockdouble                                         // [  249] 
// p16    i3  temp                                                    // [  250] 

// p16    i7  ptr                                                     // [  252] 
//vec16  vr11  work8                                                  // [  253] // working vecs, named for compatibility with C code
//vec16  vr12  work9                                                  // [  253] 
//vec16  vr13  work10                                                 // [  253] 
//vec16  vr14  work11                                                 // [  253] 

//s16     shiftval : temp
// s16    vr10.8  Intra                                               // [  256] 
// s16    vr15.8  cbp                                                 // [  257] 
// p16    i8  zero                                                    // [  258] 
// s16    vr16.8  tempblk                                             // [  259] 

// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0|   ptr| CirL.|FixBuf|CirBuf|  temp|cirPtr| bloc.| blkI.|
//|VR 1|      |      |      |      |      |      |      |  zero|
//|VR 2|                        row0                           |
//|VR 3|                        row1                           |
//|VR 4|                        row2                           |
//|VR 5|                        row3                           |
//|VR 6|                        row4                           |
//|VR 7|                        row5                           |
//|VR 8|                        row6                           |
//|VR 9|                        row7                           |
//|VR10|      |      |      |      | Intra| bloc.|      |      |
//|VR11|                       work8                           |
//|VR12|                       work9                           |
//|VR13|                      work10                           |
//|VR14|                      work11                           |
//|VR15|      |      |      |      |   cbp|      |      |      |
//|VR16|      |      |      |      | temp.|      |      |      |
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+


LINE 263".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VMOVW             vr01, 0, 1                              // zero [  263] 
    // set Intra flag
       VLD8W_3           vr10,[i4, MPO_MBCodingType]     // Intra CirBuf [  265] 
       VAND.8            vr10, vr10, MBCT_MPEG4_INTRA     // Intra Intra [  266] 
    // load cbp
       VLD16_3           vr15,[i4, MPO_NonZeroCoeffLuma]   // cbp CirBuf [  268] // load cbp
 
   
    // initialise block_no to first block
       VMOVW             vr00, 0, 2                          // block_no [  272] 
  // main block loop starts here
//BlockLoop --------                                                  // [  274] 
 vendrec r3                                                           // [  274] 
 vrec r3                                                              // [  274] 
        VMR7W.4         vr10, vr00, vr00         // blockdouble block_no [  275] 
LINE 276".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VADDW.4           vr10, vr10, vr10 // blockdouble blockdouble blockdouble [  276] // needed for indexing short arrays

// work out if block is not coded and not Intra, if so we can skip all this

        VMR6W.8         vr16, vr00, vr00             // tempblk block_no [  280] 

LINE 282".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VIM               i3, i8, 2048                       // temp zero [  282] // temp = 1<<11
       VLSRVW.8          vr00, vr00, vr16           // temp temp tempblk [  283] // temp =  1<<(11-block_no)
       VAND.8            vr00, vr15, vr00               // temp cbp temp [  284] // see if relevant bit of cbp is set
    
       VOR.8             vr00, vr00, vr10             // temp temp Intra [  286] // if cbp is set for this block or we have an Intra MB
    
       VIM               i2, i8, SDMOF_mpeg4_diff_pixel_offset // cirPtr zero [  288] 
       VADDW.4           vr00, vr00, vr10   // cirPtr cirPtr blockdouble [  289] 
       VLD16             vr00,[i2, 0]              // blkInOffset cirPtr [  290] // read offset for this block in MP0_PixelCoeffBuffer

  
       VIM               i2, i4, MPO_PixelCoeffBuffer   // cirPtr CirBuf [  293] 
       VADDW.4           vr00, vr00, i0     // cirPtr cirPtr blkInOffset [  294] // set base address for this blocks data
    
    
       VJP!i3            r4                            // temp SkipBlock [  297] 
// ****** Warning : Unmarked delay slot found at line number 298 *****
LINE 298".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  298] 
// ****** Warning : Unmarked delay slot found at line number 299 *****
LINE 299".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  299] 
// ****** Warning : Unmarked delay slot found at line number 300 *****
LINE 300".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  300] 
   
    // load input data
       VLD128            vr02,[i2, 0x00]                  // row0 cirPtr [  303] 
       VLD128            vr03,[i2, 0x20]                  // row1 cirPtr [  304] 
       VLD128            vr04,[i2, 0x40]                  // row2 cirPtr [  305] 
       VLD128            vr05,[i2, 0x60]                  // row3 cirPtr [  306] 
       VLD128            vr06,[i2, 0x80]                  // row4 cirPtr [  307] 
       VLD128            vr07,[i2, 0xA0]                  // row5 cirPtr [  308] 
       VLD128            vr08,[i2, 0xC0]                  // row6 cirPtr [  309] 
       VLD128            vr09,[i2, 0xE0]                  // row7 cirPtr [  310] 
    // Convert input coefficients to 12Q4 format and transpose to column fmt.
    // This may eventually be done in the dequantization step
    //
    
  ld r1,[r17,0];                                     // VSTACK IDCTBlock [  315] 
       VJL               vr31, r1                                     // [  315] 
// ****** Warning : Unmarked delay slot found at line number 316 *****
LINE 316".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  316] 
// ****** Warning : Unmarked delay slot found at line number 317 *****
LINE 317".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  317] 
// ****** Warning : Unmarked delay slot found at line number 318 *****
LINE 318".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  318] 
 
//

// store back in circular buffer
       VST128            vr02,[i2, 0x00]                  // row0 cirPtr [  323] 
       VST128            vr03,[i2, 0x20]                  // row1 cirPtr [  324] 
       VST128            vr04,[i2, 0x40]                  // row2 cirPtr [  325] 
       VST128            vr05,[i2, 0x60]                  // row3 cirPtr [  326] 
       VST128            vr06,[i2, 0x80]                  // row4 cirPtr [  327] 
       VST128            vr07,[i2, 0xA0]                  // row5 cirPtr [  328] 
       VST128            vr08,[i2, 0xC0]                  // row6 cirPtr [  329] 
       VST128            vr09,[i2, 0xE0]                  // row7 cirPtr [  330] 


       VJP               r5                                  // EndBlock [  333] // avoid the skip bit
// ****** Warning : Unmarked delay slot found at line number 334 *****
LINE 334".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  334] 
// ****** Warning : Unmarked delay slot found at line number 335 *****
LINE 335".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  335] 
// ****** Warning : Unmarked delay slot found at line number 336 *****
LINE 336".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  336] 
//DC_Only --------                                                    // [  337] 
 vendrec r6                                                           // [  337] 
 vrec r6                                                              // [  337] 
// if only DC terms, then all outputs will be the same value, so 
// don't bother doing the  inverse transform

 // end of Main Block Loop
//SkipBlock --------                                                  // [  342] 
 vendrec r4                                                           // [  342] 
 vrec r4                                                              // [  342] 

LINE 344".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VST128            vr62,[i2, 0xE0]                 // VZERO cirPtr [  344] 
            // in case mpeg4 and last coeff's value was toggled, this can probably be removed later
            // after the dequant function is skipped when cbp is zero
 
//EndBlock --------                                                   // [  348] 
 vendrec r5                                                           // [  348] 
 vrec r5                                                              // [  348] 
LINE 349".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VIM               i1, i1, 1                  // block_no block_no [  349] // increment block counter
    
// check to see if we have done all blocks
       VIM               i3, i1, -6                     // temp block_no [  352] // check to see if all 6 blocks processed
    
       VJP.i3            r3                            // temp BlockLoop [  354] // loop back if block_no < 6
// ****** Warning : Unmarked delay slot found at line number 355 *****
LINE 355".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  355] 
// ****** Warning : Unmarked delay slot found at line number 356 *****
LINE 356".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  356] 
// ****** Warning : Unmarked delay slot found at line number 357 *****
LINE 357".\ARCEncoder\MPEG4\ArcIDCT_MPA.sss"
       VNOP                //DS_________________________________________ [  357] 
    
    
// End                                                                // [  360] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz IDCT_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc IDCT                                                       // [  361] 





const int SIMD_IDX_IDCTBlock = 0;
const int MACRO_IDX_IDCT = 1;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int Idct_Record(int sa, int SIMDnum)
{
    int *map = Idct;

    SetSCQ(SIMDnum);

    map[SIMD_IDX_IDCTBlock] = sa;
    sa = Record_IDCTBlock(map);

    map[MACRO_IDX_IDCT] = sa;
    sa = Record_IDCT(map);

    return sa;
}
