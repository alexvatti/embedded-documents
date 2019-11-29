//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss'

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
	
#include ".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.h"


// Macro table
    int H263DeQuant[3];
    int H263DeQuant_Record(int sa, int SIMDnum);
	
    
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"    

//#include "../MPEG4/ArcIDCT_MPA.h"

extern int Idct[] ;
//------------------------------------------------------------------------------
// Name:          H263DeQuant
// Purpose:       provide the dequantisation step for the 6 blocks of an H263 MB 
// Arguments:     none
// Return Value:  void
//------------------------------------------------------------------------------
    
// [Macro] H263DeQuant                                                // [   44] 
_Asm int Record_H263DeQuant(int *mac_tab)
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
H263DeQuant_RecLoop:
  ld r1,[r17 ,4]
  vrec r1



	
// Begin                                                              // [   48] 

// p16    i1  blkInOffset                                             // [   50] 
// p16    i0  Intra                                                   // [   51] 


//vec16  vr02  row0                                                   // [   54] 
//vec16  vr03  row1                                                   // [   55] 
//vec16  vr04  row2                                                   // [   56] 
//vec16  vr05  row3                                                   // [   57] 
//vec16  vr06  row4                                                   // [   58] 
//vec16  vr07  row5                                                   // [   59] 
//vec16  vr08  row6                                                   // [   60] 
//vec16  vr09  row7                                                   // [   61] 

//vec16  vr10  sign0                                                  // [   63] 
//vec16  vr11  sign1                                                  // [   63] 
//vec16  vr12  sign2                                                  // [   63] 
//vec16  vr13  sign3                                                  // [   63] 
//vec16  vr14  sign4                                                  // [   63] 
//vec16  vr15  sign5                                                  // [   63] 
//vec16  vr16  sign6                                                  // [   63] 
//vec16  vr17  sign7                                                  // [   63] 


// p16    i2  QuantVal                                                // [   66] 
//vec16  vr18  QuantVec                                               // [   67] 
// p16    i3  temp                                                    // [   68] 
// p16    i7  cirPtr                                                  // [   69] 
//vec16  vr19  QuantEven                                              // [   70] 


// p16    i8  dcterm                                                  // [   73] // must be same lane as Intra
// p16    i9  block_no                                                // [   74] 
// p16    i10  blockdouble                                            // [   75] 
// s16    vr20.1  cbp                                                 // [   76] 
// s16    vr21.1  mode                                                // [   77] 
// p16    i11  zero                                                   // [   78] 
// p16    i12  hasAC                                                  // [   79] 

LINE 81".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VMOVW             vr01, 0, 8                              // zero [   81] 

// determine if Intra block
       VLD8W             vr00,[i4, MPO_MBCodingType]     // Intra CirBuf [   84] 
       VAND.1            vr00, vr00, MBCT_MPEG4_INTRA     // Intra Intra [   85] 
       VSEQW.1           vr00, 1                                // Intra [   86] // Sticky bit set in Lane 1 if Intra
// get the quantiser    
       VLD8W_2           vr00,[i4, MPO_QuantValue]    // QuantVal CirBuf [   88] 
       VMIVW.255         vr18, i2                   // QuantVec QuantVal [   89] 
// calculate the QuantEven vector    
       VAND              vr19, vr18, 1             // QuantEven QuantVec [   91] // set to 1 if odd, 0 if even
       VRSUBW            vr19, vr19, 1            // QuantEven QuantEven [   92] // subtract from 1: set to 1 if even, 0 if odd
          
       VSUBW             vr19, vr18, vr19 // QuantEven QuantVec QuantEven [   94] // quant - quanteven
    
       VADDW             vr18, vr18, vr18  // QuantVec QuantVec QuantVec [   96] // double QuantVec for use in mults
 // load cbp   
       VLD16             vr20,[i4, MPO_NonZeroCoeffLuma]   // cbp CirBuf [   98] 

 // initialise block counter   
       VMOVW             vr01, 0, 2                          // block_no [  101] 

//DeQuant263Loop --------                                             // [  103] 
 vendrec r3                                                           // [  103] 
 vrec r3                                                              // [  103] 
        VMR7W.4         vr01, vr01, vr01         // blockdouble block_no [  104] 
LINE 105".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VADDW.4           vr01, vr01, vr01 // blockdouble blockdouble blockdouble [  105] // needed for indexing short arrays

        VMR1W.1         vr21, vr01, vr01                // mode block_no [  107] 
LINE 108".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VIM               i8, i11, 2048                    // dcterm zero [  108] // dcterm = 1<<11
       VLSRVW.1          vr01, vr01, vr21          // dcterm dcterm mode [  109] // dcterm =  1<<(11-block_no)
       VAND.1            vr01, vr20, vr01           // dcterm cbp dcterm [  110] // see if relevant bit of cbp is set
        VMR4W.16        vr01, vr01, vr01                 // hasAC dcterm [  111] // save in hasAC
LINE 112".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VOR.1             vr01, vr01, vr00         // dcterm dcterm Intra [  112] // if cbp is set for this block OR we have an Intra MB
// REGMAP 
//+============================================================+
//|REG |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
//+============================================================+
//|VR 0|cirPtr| CirL.|FixBuf|CirBuf|  temp| Quan.| blkI.| Intra|
//|VR 1|      |      |      | hasAC|  zero| bloc.| bloc.|dcterm|
//|VR 2|                        row0                           |
//|VR 3|                        row1                           |
//|VR 4|                        row2                           |
//|VR 5|                        row3                           |
//|VR 6|                        row4                           |
//|VR 7|                        row5                           |
//|VR 8|                        row6                           |
//|VR 9|                        row7                           |
//|VR10|                       sign0                           |
//|VR11|                       sign1                           |
//|VR12|                       sign2                           |
//|VR13|                       sign3                           |
//|VR14|                       sign4                           |
//|VR15|                       sign5                           |
//|VR16|                       sign6                           |
//|VR17|                       sign7                           |
//|VR18|                    QuantVec                           |
//|VR19|                   QuantEven                           |
//|VR20|      |      |      |      |      |      |      |   cbp|
//|VR21|      |      |      |      |      |      |      |  mode|
//|............................................................|
//|VR31|                      VSTACK                           |
//|............................................................|
//|VR62|                       VZERO                           |
//|VR63|                      VFLAGS                           |
//+------------------------------------------------------------+

   // get address of residual data
LINE 115".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VIM               i7, i10, SDMOF_mpeg4_diff_pixel_offset // cirPtr blockdouble [  115] // calc offset of lookup table
       VLD16_1           vr00,[i7, 0]              // blkInOffset cirPtr [  116] // read offset into MP0_PixelCoeffBuffer
       VIM               i7, i4, MPO_PixelCoeffBuffer   // cirPtr CirBuf [  117] 
       VADDW.128         vr00, vr00, i1     // cirPtr cirPtr blkInOffset [  118] 

    // load the residual data
       VLD128            vr02,[i7, 0]                     // row0 cirPtr [  121] // load 8 shorts from input - first row of transform data
       VLD128            vr03,[i7, PCB_RESIDUAL_STRIDE]   // row1 cirPtr [  122] // load next 8 shorts from input
       VLD128            vr04,[i7, PCB_RESIDUAL_STRIDE*2] // row2 cirPtr [  123] // load next 8 shorts from input
       VLD128            vr05,[i7, PCB_RESIDUAL_STRIDE*3] // row3 cirPtr [  124] // load next 8 shorts from input
       VLD128            vr06,[i7, PCB_RESIDUAL_STRIDE*4] // row4 cirPtr [  125] // load next 8 shorts from input
   
  
   
       VJP!i8            r4                       // dcterm InterPredict [  129] // not Intra and no coeffs => skip the dequant & IDCT process
       VLD128            vr07,[i7, PCB_RESIDUAL_STRIDE*5] //DS row5 cirPtr [  130] // load next 8 shorts from input
       VLD128            vr08,[i7, PCB_RESIDUAL_STRIDE*6] //DS row6 cirPtr [  131] // load next 8 shorts from input
       VLD128            vr09,[i7, PCB_RESIDUAL_STRIDE*7] //DS row7 cirPtr [  132] // load next 8 shorts from input  
    

 
       VIV.1.S           i8, vr02                         // dcterm row0 [  136] // save dcterm - if intra (may not have AC coeffs)
       VMULW.1.S         vr01, vr01, 8                  // dcterm dcterm [  137] // mult by 8 - dequant for DC term
  //  vst16        'dcterm, [cirPtr,    0] // store in case main loop skipped due to cbp zero
  
 // if no AC coeffs, skip the main loop
 //   vjp!hasAC    .End
 //   vnop
//    vnop
 //   vnop
 
//dequant --------                                                    // [  146] 
 vendrec r5                                                           // [  146] 
 vrec r5                                                              // [  146] 


    // save sign of each element
LINE 150".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VSIGNW            vr10, vr02                        // sign0 row0 [  150] 
       VSIGNW            vr11, vr03                        // sign1 row1 [  151] 
       VSIGNW            vr12, vr04                        // sign2 row2 [  152] 
       VSIGNW            vr13, vr05                        // sign3 row3 [  153] 
       VSIGNW            vr14, vr06                        // sign4 row4 [  154] 
       VSIGNW            vr15, vr07                        // sign5 row5 [  155] 
       VSIGNW            vr16, vr08                        // sign6 row6 [  156] 
       VSIGNW            vr17, vr09                        // sign7 row7 [  157] 
    
    // make all coeffs positive
 // abs(coeff)
       VABSW             vr02, vr02                         // row0 row0 [  161] 
       VABSW             vr03, vr03                         // row1 row1 [  162] 
       VABSW             vr04, vr04                         // row2 row2 [  163] 
       VABSW             vr05, vr05                         // row3 row3 [  164] 
       VABSW             vr06, vr06                         // row4 row4 [  165] 
       VABSW             vr07, vr07                         // row5 row5 [  166] 
       VABSW             vr08, vr08                         // row6 row6 [  167] 
       VABSW             vr09, vr09                         // row7 row7 [  168] 


// multiply by 2*quant
// (2*(abs(coeff)) )*quant  
       VMULW             vr02, vr02, vr18          // row0 row0 QuantVec [  173] 
       VMULW             vr03, vr03, vr18          // row1 row1 QuantVec [  174] 
       VMULW             vr04, vr04, vr18          // row2 row2 QuantVec [  175] 
       VMULW             vr05, vr05, vr18          // row3 row3 QuantVec [  176] 
       VMULW             vr06, vr06, vr18          // row4 row4 QuantVec [  177] 
       VMULW             vr07, vr07, vr18          // row5 row5 QuantVec [  178] 
       VMULW             vr08, vr08, vr18          // row6 row6 QuantVec [  179] 
       VMULW             vr09, vr09, vr18          // row7 row7 QuantVec [  180] 
     
	
// add (quant - QuantEven)
//(2*(abs(coeff)) + 1)*quant - QuantEven
       VADDW             vr02, vr02, vr19         // row0 row0 QuantEven [  185] 
       VADDW             vr03, vr03, vr19         // row1 row1 QuantEven [  186] 
       VADDW             vr04, vr04, vr19         // row2 row2 QuantEven [  187] 
       VADDW             vr05, vr05, vr19         // row3 row3 QuantEven [  188] 
       VADDW             vr06, vr06, vr19         // row4 row4 QuantEven [  189] 
       VADDW             vr07, vr07, vr19         // row5 row5 QuantEven [  190] 
       VADDW             vr08, vr08, vr19         // row6 row6 QuantEven [  191] 
       VADDW             vr09, vr09, vr19         // row7 row7 QuantEven [  192] 
	
	// reapply sign
       VMULW             vr02, vr02, vr10             // row0 row0 sign0 [  195] 
       VMULW             vr03, vr03, vr11             // row1 row1 sign1 [  196] 
       VMULW             vr04, vr04, vr12             // row2 row2 sign2 [  197] 
       VMULW             vr05, vr05, vr13             // row3 row3 sign3 [  198] 
       VMULW             vr06, vr06, vr14             // row4 row4 sign4 [  199] 
       VMULW             vr07, vr07, vr15             // row5 row5 sign5 [  200] 
    
    
    // now call IDCT for this block
  ld r6,[r17,0];                                     // VSTACK IDCTBlock [  204] 
       VJL               vr31, r6                                     // [  204] 
       VMULW             vr08, vr08, vr16  //DS_________ row6 row6 sign6 [  205] 
       VMULW             vr09, vr09, vr17  //DS_________ row7 row7 sign7 [  206] 
    
    // if Intra mode, put back the saved dc term - sticky was already set for Intra
       VMIVW.1.S         vr02, i8  //DS_____________________ row0 dcterm [  209] 
    
       VJP.i0            r7                           // Intra SaveIntra [  211] 
    
//InterPredict --------  //DS___________________________________________ [  213] 
 vendrec r4  //DS_______________________________________________________ [  213] 
 vrec r4  //DS__________________________________________________________ [  213] 
    // calculate the address of the predicted data
LINE 215".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VIM               i3, i10, SDMOF_mpeg4_pred_pixel_offset //DS temp blockdouble [  215] // calc offset of lookup table
       VLD16_1           vr00,[i3, 0]  //DS____________ blkInOffset temp [  216] // read offset into MP0_PixelCoeffBuffer
       VIM               i3, i4, MPO_PixelCoeffBuffer  //DS_ temp CirBuf [  217] 
       VADDW.8           vr00, vr00, i1         // temp temp blkInOffset [  218] 
    // inter need to add prediction
    //load predicted data
       VLD64W            vr10,[i3, 0*16]                   // sign0 temp [  221] 
       VLD64W            vr11,[i3, 1*16]                   // sign1 temp [  222] 
       VLD64W            vr12,[i3, 2*16]                   // sign2 temp [  223] 
       VLD64W            vr13,[i3, 3*16]                   // sign3 temp [  224] 
       VLD64W            vr14,[i3, 4*16]                   // sign4 temp [  225] 
       VLD64W            vr15,[i3, 5*16]                   // sign5 temp [  226] 
       VLD64W            vr16,[i3, 6*16]                   // sign6 temp [  227] 
       VLD64W            vr17,[i3, 7*16]                   // sign7 temp [  228] 
    //add prediction to inverse DCT
       VADDW             vr02, vr02, vr10             // row0 row0 sign0 [  230] 
       VADDW             vr03, vr03, vr11             // row1 row1 sign1 [  231] 
       VADDW             vr04, vr04, vr12             // row2 row2 sign2 [  232] 
       VADDW             vr05, vr05, vr13             // row3 row3 sign3 [  233] 
       VADDW             vr06, vr06, vr14             // row4 row4 sign4 [  234] 
       VADDW             vr07, vr07, vr15             // row5 row5 sign5 [  235] 
       VADDW             vr08, vr08, vr16             // row6 row6 sign6 [  236] 
       VADDW             vr09, vr09, vr17             // row7 row7 sign7 [  237] 
    
//SaveIntra --------                                                  // [  239] 
 vendrec r7                                                           // [  239] 
 vrec r7                                                              // [  239] 

    // set up pointer for saving data
LINE 242".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VIM               i7, i10, SDMOF_mpeg4_final_pixel_offset // cirPtr blockdouble [  242] // calc offset of lookup table
       VLD16_1           vr00,[i7, 0]              // blkInOffset cirPtr [  243] // read offset into MP0_PixelCoeffBuffer
       VIM               i7, i4, MPO_PixelCoeffBuffer   // cirPtr CirBuf [  244] 
       VADDW.128         vr00, vr00, i1     // cirPtr cirPtr blkInOffset [  245] 
// save to byte and saturate
       VASRPWB           vr02, vr02, 0                      // row0 row0 [  247] 
       VASRPWB           vr03, vr03, 0                      // row1 row1 [  248] 
       VASRPWB           vr04, vr04, 0                      // row2 row2 [  249] 
       VASRPWB           vr05, vr05, 0                      // row3 row3 [  250] 
       VASRPWB           vr06, vr06, 0                      // row4 row4 [  251] 
       VASRPWB           vr07, vr07, 0                      // row5 row5 [  252] 
       VASRPWB           vr08, vr08, 0                      // row6 row6 [  253] 
       VASRPWB           vr09, vr09, 0                      // row7 row7 [  254] 
    
    // store quantized dct coefficients in memory 
       VST64             vr02,[i7, 0]                     // row0 cirPtr [  257] 
       VST64             vr03,[i7, 16]                    // row1 cirPtr [  258] 
       VST64             vr04,[i7, 32]                    // row2 cirPtr [  259] 
       VST64             vr05,[i7, 48]                    // row3 cirPtr [  260] 
       VST64             vr06,[i7, 64]                    // row4 cirPtr [  261] 
       VST64             vr07,[i7, 80]                    // row5 cirPtr [  262] 
       VST64             vr08,[i7, 96]                    // row6 cirPtr [  263] 
       VST64             vr09,[i7, 112]                   // row7 cirPtr [  264] 
    
//End --------                                                        // [  266] 
 vendrec r8                                                           // [  266] 
 vrec r8                                                              // [  266] 
	
LINE 268".\ARCEncoder\MPEG4\ArcH263DeQuant_MPA.sss"
       VIM               i9, i9, 1                  // block_no block_no [  268] 
    
    // check to see if we have done all blocks
       VIM               i3, i9, -6                     // temp block_no [  271] 
    
       VJP.i3            r3                       // temp DeQuant263Loop [  273] 
       VNOP                //DS_________________________________________ [  274] 
       VNOP                //DS_________________________________________ [  275] 
       VNOP                //DS_________________________________________ [  276] 
    
// End                                                                // [  278] 
  vendrec r1 ; [Macro]
  sub.f r2,r2,1
  bnz H263DeQuant_RecLoop
  add r0,r1,1
  pop  r17
  pop  r16
  pop  r15
  pop  r14
  pop  r13
.on percent_reg
}
// Endfunc H263DeQuant                                                // [  279] 



const int MACRO_IDX_H263DeQuant = 1;

_Asm void SetSCQ(int q)
{
   %reg q
   scq	0,q
}

// Record macros
int H263DeQuant_Record(int sa, int SIMDnum)
{
    int *map = H263DeQuant;

    SetSCQ(SIMDnum);

    map[0] = Idct[0];

    map[MACRO_IDX_H263DeQuant] = sa;
    sa = Record_H263DeQuant(map);

    return sa;
}
