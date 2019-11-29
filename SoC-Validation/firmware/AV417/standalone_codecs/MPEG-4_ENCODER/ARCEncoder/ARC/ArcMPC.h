/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */

/* May contain code (c) British Telecommunications plc 2005        */



#ifndef ARC_MPC_H
#define ARC_MPC_H

#include "typdef.h"

// Max width of frame fixed buffer can support
#ifdef HD_SIM
#define MAX_FRAME_WIDTH_MB				(1280/16)
#else // HD_SIM
#define MAX_FRAME_WIDTH_MB				(720/16)
#endif // HD_SIM

// Macroblock Neighbour Flags
#define MBNF_NONZERO_COEFF5_SHIFT   0
#define MBNF_NONZERO_COEFF5_MASK    0x1
#define MBNF_NONZERO_COEFF10_SHIFT  0
#define MBNF_NONZERO_COEFF10_MASK   0x1
#define MBNF_NONZERO_COEFF7_SHIFT   1
#define MBNF_NONZERO_COEFF7_MASK    0x1
#define MBNF_NONZERO_COEFF11_SHIFT  1
#define MBNF_NONZERO_COEFF11_MASK   0x1
#define MBNF_NONZERO_COEFF13_SHIFT  2
#define MBNF_NONZERO_COEFF13_MASK   0x1
#define MBNF_NONZERO_COEFF14_SHIFT  2
#define MBNF_NONZERO_COEFF14_MASK   0x1
#define MBNF_NONZERO_COEFF15_SHIFT  3
#define MBNF_NONZERO_COEFF15_MASK   0x1
#define MBNF_VALID_SHIFT            4
#define MBNF_VALID_MASK             0x1
#define MBNF_VALID_ID_SHIFT         5
#define MBNF_VALID_ID_MASK          0x1
#define MBNF_INTRA_SHIFT            6
#define MBNF_INTRA_MASK             0x1
#define MBNF_CODE_SHIFT             7
#define MBNF_CODE_MASK              0x1

#define MBNF_NONZERO_COEFF5   1
#define MBNF_NONZERO_COEFF10  1
#define MBNF_NONZERO_COEFF7   2
#define MBNF_NONZERO_COEFF11  2
#define MBNF_NONZERO_COEFF13  4
#define MBNF_NONZERO_COEFF14  4
#define MBNF_NONZERO_COEFF15  8
#define MBNF_NONZERO_COEFF15  8
#define MBNF_VALID            16
#define MBNF_VALID_ID         32
#define MBNF_INTRA            64
#define MBNF_CODE             128


// Macroblock Coding Type (H.264)
#define MBCT_H264_UNKNOWN     0
#define MBCT_H264_INTRA16X16  1
#define MBCT_H264_INTRA4X4    2
#define MBCT_H264_INTRA8X8    3
#define MBCT_H264_I_PCM       4
#define MBCT_H264_INTER16X16  8
#define MBCT_H264_INTER16X8   16
#define MBCT_H264_INTER8X16   24
#define MBCT_H264_INTER8X8    32
#define MBCT_H264_SKIPPED     64
#define MBCT_H264_8x8_TRANSFORM 128

#define MBCT_H264_INTRA (MBCT_H264_INTRA16X16 |  MBCT_H264_INTRA8X8 |  MBCT_H264_INTRA4X4 | MBCT_H264_I_PCM)
#define MBCT_H264_INTER (MBCT_H264_INTER16X16 | MBCT_H264_INTER16X8 | MBCT_H264_INTER8X16 | MBCT_H264_INTER8X8)

// Macroblock Coding Type (MPEG4/H.263)
#define MBCT_MPEG4_UNKNOWN    0
#define MBCT_MPEG4_INTRA      1
#define MBCT_MPEG4_INTER16X16 2
#define MBCT_MPEG4_INTER8X8   4
#define MBCT_MPEG4_SKIPPED    8


// Macroblock Intra Mode
// IntraMode Luma (H.264)
#define MBIM_H264_Y_VERTICAL   0
#define MBIM_H264_Y_HORIZONTAL 1
#define MBIM_H264_Y_DC         2


// IntraMode Chroma (H.264)
#define MBIM_H264_C_DC         0
#define MBIM_H264_C_HORIZONTAL 1
#define MBIM_H264_C_VERTICAL   2


// IntraMode Luma (MPEG4/H.263)
#define MBIM_MPEG4_DC         0
#define MBIM_MPEG4_VERTICAL   1
#define MBIM_MPEG4_HORIZONTAL 2


// NonZeroCoeff (H.264)
#define NZC_H264_SB0    1
#define NZC_H264_SB1    2
#define NZC_H264_SB2    16
#define NZC_H264_SB3    32
#define NZC_H264_SB4    4
#define NZC_H264_SB5    8
#define NZC_H264_SB6    64
#define NZC_H264_SB7    128
#define NZC_H264_SB8    256
#define NZC_H264_SB9    512
#define NZC_H264_SB10   4096
#define NZC_H264_SB11   8192
#define NZC_H264_SB12   1024
#define NZC_H264_SB13   2048
#define NZC_H264_SB14   16384
#define NZC_H264_SB15   32768
#define NZC_H264_SB16   1
#define NZC_H264_SB17   2
#define NZC_H264_SB18   4
#define NZC_H264_SB19   8
#define NZC_H264_SB20   16
#define NZC_H264_SB21   32
#define NZC_H264_SB22   64
#define NZC_H264_SB23   128

#define NZC_H264_LUMA_B0 (NZC_H264_SB0 | NZC_H264_SB1 | NZC_H264_SB2 | NZC_H264_SB3)
#define NZC_H264_LUMA_B1 (NZC_H264_SB4 | NZC_H264_SB5 | NZC_H264_SB6 | NZC_H264_SB7)
#define NZC_H264_LUMA_B2 (NZC_H264_SB8 | NZC_H264_SB9 | NZC_H264_SB10 | NZC_H264_SB11)
#define NZC_H264_LUMA_B3 (NZC_H264_SB12 | NZC_H264_SB13 | NZC_H264_SB14 | NZC_H264_SB15)

#define NZC_H264_CHROMA_U_AC (NZC_H264_SB16 | NZC_H264_SB17 | NZC_H264_SB18 | NZC_H264_SB19)
#define NZC_H264_CHROMA_V_AC (NZC_H264_SB20 | NZC_H264_SB21 | NZC_H264_SB22 | NZC_H264_SB23)

#define NZC_H264_CHROMA_U_DC 1
#define NZC_H264_CHROMA_V_DC 2

// NonZeroCoeff (MPEG4/H.263)
#define NZC_MPEG4_DC_B0 1
#define NZC_MPEG4_DC_B1 2
#define NZC_MPEG4_DC_B2 4
#define NZC_MPEG4_DC_B3 8
#define NZC_MPEG4_DC_B4 16
#define NZC_MPEG4_DC_B5 32
#define NZC_MPEG4_AC_B0 64
#define NZC_MPEG4_AC_B1 128
#define NZC_MPEG4_AC_B2 256
#define NZC_MPEG4_AC_B3 512
#define NZC_MPEG4_AC_B4 1024
#define NZC_MPEG4_AC_B5 2048


// Pixel and Coefficient Buffer Strides in Bytes
#define PCB_CURRENT_STRIDE        16

#define PCB_REFERENCE_STRIDE      16

#define PCB_RESIDUAL_STRIDE       32

#define PCB_TRANSFORM_STRIDE      32

#define PCB_RECONSTRUCTED_STRIDE  24

#define PCB_DEBLOCK_STRIDE        24



// Macroblock Neighbouring Flags
struct NeighbourFlags_t
{
	u_int8 Flags;
	u_int8 QuantValue;

};


// Motion Vector
struct MV_t
{
	s_int8 mvx;
	s_int8 mvy;

};


// Macroblock Pixel and Control Common Class
class cMPCcommon
{
public:

	// constructor and deconstructor
	cMPCcommon ();
	~cMPCcommon ();

	// MB Information and Neighbour Flags Data Table
#define											MPO_LeftMB				0	+ 0
	NeighbourFlags_t LeftMB;
#define											MPO_AboveMB				0	+ 2
	NeighbourFlags_t AboveMB;
#define											MPO_AboveRightMB		0	+ 4
	NeighbourFlags_t AboveRightMB;
#define											MPO_AboveLeftMB			0	+ 6
	NeighbourFlags_t AboveLeftMB;

#define											MPO_NonZeroCoeffLuma	0	+ 8
	u_int16 NonZeroCoeffLuma;
#define											MPO_NonZeroCoeffChroma	0	+ 10
	u_int8 NonZeroCoeffChroma;

#define											MPO_UV_DC_NonZeroCoeff	0	+ 11
	u_int8 UV_DC_NonZeroCoeff;

#define											MPO_MBCodingType		0	+ 12
	u_int8 MBCodingType;
#define											MPO_IntraMode			0	+ 13
	u_int8 IntraMode;
#define INTRAMODE_LUMA_SHIFT 0
#define INTRAMODE_LUMA_MASK  0x3
#define INTRAMODE_CHROMA_SHIFT 4
#define INTRAMODE_CHROMA_MASK  0x3
#define											MPO_QuantValue			0	+ 14
	u_int8 QuantValue;
#define											MPO_CollectionID		0	+ 15
	u_int8 CollectionID;

	// Motion Vectors Data Table
#define											MPO_mv					16	+ 0
	MV_t mv[4];
#define											MPO_mvp					16	+ 8
	MV_t mvp[4];

	// MB Luma Intra Prediction Pixel/DC Coeff Store Data Table
	union
	{
#define											MPO_Y_IntraPredPixel	32	+ 0
		u_int8 Y_IntraPredPixel[16];
#define											MPO_Y_IntraPredDC_Coeff	32	+ 0
		int16 Y_IntraPredDC_Coeff[4];
	};

	// MB Chroma Intra Prediction Pixel/DC Coeff Store Data Table
	union
	{
#define											MPO_U_IntraPredPixel	48	+ 0
		u_int8 U_IntraPredPixel[8];
#define											MPO_UV_IntraPredDC_Coeff 48	+ 0
		int16 UV_IntraPredDC_Coeff[2];
	};
#define											MPO_V_IntraPredPixel	48	+ 8
	u_int8 V_IntraPredPixel[8];


	// MB Luma Horizontal Pixel Store Data Table
#define											MPO_Y_HorizPixelStore0	64	+ 0
	u_int8 Y_HorizPixelStore0[16];
#define											MPO_Y_HorizPixelStore1	64	+ 16
	u_int8 Y_HorizPixelStore1[16];
#define											MPO_Y_HorizPixelStore2	64	+ 32
	u_int8 Y_HorizPixelStore2[16];
#define											MPO_Y_HorizPixelStore3	64	+ 48
	u_int8 Y_HorizPixelStore3[16];

	// MB Chroma Horizontal Pixel Store Data Table
#define											MPO_U_HorizPixelStore0	128	+ 0
	u_int8 U_HorizPixelStore0[8];
#define											MPO_V_HorizPixelStore0	128	+ 8
	u_int8 V_HorizPixelStore0[8];
#define											MPO_U_HorizPixelStore1	128	+ 16
	u_int8 U_HorizPixelStore1[8];
#define											MPO_V_HorizPixelStore1	128	+ 24
	u_int8 V_HorizPixelStore1[8];

   // Macroblock Work Area (reclaimed from unused MB Chroma Horizontal Pixel Store Data Table)
#define 										MPO_WorkArea			160	+ 0
	union
	{
		unsigned char WorkAreaSpace[32];
		struct
		{
#define 										MPO_WorkArea_BestIntraLumaCost	160	+ 0
			u_int16 BestIntraLumaCost;
#define 										MPO_WorkArea_BestIntraLumaMode	160	+ 2
			unsigned char BestIntraLumaMode;
#define 										MPO_WorkArea_BestIntraChromaMode	160	+ 3
			unsigned char BestIntraChromaMode;
#define 										MPO_WorkArea_MvSkip					160	+ 4
			MV_t MvSkip;
            signed char SliceGroup;     // Maybe move to ActMPC_info in MrgMPC at later date
		} WorkArea;
	};
};


// Macroblock Pixel and Control Class for the Fixed Buffer
class cMPCfb : public cMPCcommon
{
public:

	// constructor and deconstructor
	cMPCfb ();
	~cMPCfb ();

	// Intra 4x4 Modes and Coefficient Counts Data Table
#define											MPO_Intra4x4ModeCoeffCountSB	192	+ 0
	u_int32 Intra4x4ModeCoeffCountSB[4]; // Intra 4x4 Mode SBs 10, 11, 14 and 15. Coeff Count SBs (8 nibbles) 10, 11, 14, 15, 18, 19, 22 and 23
	//int reservedIntra4x4ModeCoeffCountDataTable; // do not use
};
#define											MPO_SIZE_OF_FIXED_ENTRY			208


// Macroblock Pixel and Control Class for the Circular Buffer
class cMPCcb : public cMPCcommon
{
   public:

	   // constructor and deconstructor
      cMPCcb ();
      ~cMPCcb ();

      // MB Luma DC Coefficient Store Data Table
#define											MPO_Y_DC_CoeffStore0	192	+ 0
	   int16 Y_DC_CoeffStore0[8];
#define											MPO_Y_DC_CoeffStore1	208	+ 0
	   int16 Y_DC_CoeffStore1[8];

      // MB Chroma DC Coefficient Store Data Table
#define											MPO_U_DC_CoeffStore		224	+ 0
	   int16 U_DC_CoeffStore[4];
#define											MPO_V_DC_CoeffStore		224	+ 8
	   int16 V_DC_CoeffStore[4];

	   // Coefficient Counts Data Table
#define											MPO_CoeffCountSB		240	+ 0
      u_int32 CoeffCountSB[4]; // 128 bits
#define COEFF_MASK 0x1F

      // Intra 4x4 Modes Data Table
#define											MPO_Intra4x4ModeSB		256	+ 0
      u_int8 Intra4x4ModeSB[16];

      // Intra 4x4 Predictions Data Table
#define											MPO_Intra4x4PredSB		272	+ 0
      u_int8 Intra4x4PredSB[16];

	   // Pixel and Coefficient Buffers
#define											MPO_PixelCoeffBuffer	288	+ 0
	   u_int8 PixelCoeffBuffer[16*96];
// Pixel and Coefficient Buffer offsets from MPO_PixelCoeffBuffer
#define 										PCB_CURRENT_Y         0
#define 										PCB_CURRENT_U         (PCB_CURRENT_Y + PCB_CURRENT_STRIDE*16) //0x220
#define 										PCB_CURRENT_V         (PCB_CURRENT_U + PCB_CURRENT_STRIDE/2)

#define 										PCB_REFERENCE_Y       (PCB_CURRENT_Y + PCB_CURRENT_STRIDE*24) //0x2a0
#define 										PCB_REFERENCE_U       (PCB_REFERENCE_Y + PCB_REFERENCE_STRIDE*16)
#define 										PCB_REFERENCE_V       (PCB_REFERENCE_U + PCB_REFERENCE_STRIDE/2)

#define 										PCB_RESIDUAL_Y        (PCB_CURRENT_Y + PCB_CURRENT_STRIDE*48)   //0x420
#define 										PCB_RESIDUAL_U        (PCB_RESIDUAL_Y + PCB_RESIDUAL_STRIDE*16)	//0x620
#define 										PCB_RESIDUAL_V        (PCB_RESIDUAL_U + PCB_RESIDUAL_STRIDE/2)  // 0x630	

#define 										PCB_TRANSFORM_Y       PCB_RESIDUAL_Y
#define 										PCB_TRANSFORM_U       PCB_RESIDUAL_U
#define 										PCB_TRANSFORM_V       PCB_RESIDUAL_V

// 		offsets from MPO_PixelCoeffBuffer
#define 										PCB_RECONSTRUCTED_Y		0
#define 										PCB_RECONSTRUCTED_U   (PCB_RECONSTRUCTED_Y + PCB_RECONSTRUCTED_STRIDE*20)
#define 										PCB_RECONSTRUCTED_V   (PCB_RECONSTRUCTED_U + 12)

#define											MPO_SIZE_OF_CIRCULAR_BUF_ENTRY		1824
};



// Current / Reference buffers
//              PCB_CURRENT_STRIDE
//              /
//      |------+-------|
//              11111111
//      0123456789012345
// 0    0000000011111111
// 1    0000000011111111
// 2    0000000011111111
// 3    0000000011111111
// 4    0000000011111111
// 5    0000000011111111
// 6    0000000011111111
// 7    0000000011111111
// 8    2222222233333333
// 9    2222222233333333
//10    2222222233333333
//11    2222222233333333
//12    2222222233333333
//13    2222222233333333
//14    2222222233333333
//15    2222222233333333
//16    4444444455555555
//17    4444444455555555
//18    4444444455555555
//19    4444444455555555
//20    4444444455555555
//21    4444444455555555
//22    4444444455555555
//23    4444444455555555



// Reconstructed buffer
//                PCB_RECONSTRUCTED_STRIDE
//                  /
//      |----------+-----------|
//              1111111111222222
//      012345678901234567890123
// 0    *xxxxxxxAAAAAAAAAAAAAAAA		*	PCB_RECONSTRUCTED_Y
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
//20    #xxxxxxxxxxx@xxxxxxxxxxx		#	PCB_RECONSTRUCTED_U		@ PCB_RECONSTRUCTED_V
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

// MPC Class
class cMPC
{
   public:

	   // constructor and deconstructor
      cMPC (int picture_width);
      ~cMPC ();

      cMPCcb *GetLeftMB (cMPCcb *curr);
      cMPCfb *GetAboveMB (cMPCcb *curr);
      int AllocateMB (int stage, cMPCcb *curr);
      int FreeMB (int stage);


   private:

      // Fixed Buffer
      cMPCfb *FixedBuffer;
      // Circular Buffer
      cMPCcb *CircularBuffer;

      // Number of entries in the circular buffer
      int cbSize;
      // Positions in the circular buffer of the different stages
      // Position 0        = Head = Stage 1 = Motion Estimation
      // Position cbSize-1 = Tail = Stage 2 = Left MB for Deblock
      int Stage1Pos;
      int Stage2Pos;
      int Stage3Pos;
      int Stage4Pos;

      // Flag to indicate that the ME is waiting and therefore needs to be
      // kicked the next time a MB is freed
      int Stage1Waiting;
};



#endif // ARC_MPC_H
