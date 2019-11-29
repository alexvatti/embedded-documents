/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */


#ifndef INCLUDED_ARC_SDM_TABLES
#define INCLUDED_ARC_SDM_TABLES

//#ifdef _DEBUG
#include <stdio.h>
//#endif

#include "ArcMPC.h"



#define ABC0_C0 0
#define ABC0_BETA 2
#define ABC0_ALPHA  3

//Temp
struct h264_encoder_control_tag;

#define MAX_CIRC_BUFFS 8
// this define is also in ArcFrameBuffers.h
#define CHROMA_WORKING_ARRAY_STRIDE 16
#define ME_REFERENCE_ARRAY_STRIDE   32
#define ME_WORKING_ARRAY_STRIDE     48

extern struct SDM_WorkArea
{
	static void Init_SDM_WorkArea();

//Frame tables for DMA
// this define is also in ArcFrameBuffers.h
#define 											SDMOF_FrameTabDMA									0x0000
	qword	FrameTabDMA[16];			// 0x0100 bytes

	// Intra prediction for modes 0-2
#define												SDMOF_IntraPredictionLuma							0x0100
	u_int8 IntraPredictionLuma[3][256]; // 0x0300 bytes
#define												SDMOF_IntraPredictionChroma							0x0400
	u_int8 IntraPredictionChroma[3][128]; // 0x0180
	// Intra prediction residues for modes 0-2
#define												SDMOF_IntraPredictionResidsLuma						0x0580
	int16 IntraPredictionResidsLuma[3][256]; // 0x0600
#define												SDMOF_IntraPredictionResidsChroma					0x0b80
	int16 IntraPredictionResidsChroma[3][128]; // 0x0300

#define												SDMOF_AlphaBetaCoTable								0x0e80
	unsigned char AlphaBetaCoTable[3][37][4];	// 0x01bc

#define                                             CHROMA_QUANT_TABLE                                  0x103c
    short * ChromaQuantTable;
    //unsigned char Padding_0[4];					// 0x0004
#define												SDMOF_h264_frame_scan_quantiser_table				0x1040
	short h264_frame_scan_quantiser_table[6][16]; // 0x00c0

#define												SDMOF_h264_frame_scan_inverse_quantiser_table		0x1100
	short h264_frame_scan_inverse_quantiser_table[6][16]; // 0x00c0

#define												SDMOF_h264_4x4_transmission_to_raster_table			0x11c0
	unsigned char h264_4x4_transmission_to_raster_table[16]; // 0x0010
#define												SDMOF_h264_quantiser_chroma_mapping_tableClipping	0x11d0
    unsigned char h264_quantiser_chroma_mapping_tableClipping[16+52+16]; // 0x0054

#define                                             LUMA_INVERSE_QUANT_TABLE                            0x1224
    short *LumaInverseQuantTable;
#define                                             LUMA_QUANT_TABLE                                    LUMA_INVERSE_QUANT_TABLE + 4
    short *LumaQuantTable;
#define                                             CHROMA_INVERSE_QUANT_TABLE                          LUMA_QUANT_TABLE + 4
    short * ChromaInverseQuantTable;
    //unsigned char Padding_1[12];					// 0x000c
#define                                             SDMOF_h264_quantiser_chroma_mapping_table			0x1230
    signed char h264_quantiser_chroma_mapping_table[52]; //0x34

#define                                             LUMA_SHIFT_NORMAL                                   0x1230 + 0x34
    unsigned short LumaShiftNormal;

#define                                             CHROMA_SHIFT                                        LUMA_SHIFT_NORMAL + 0x2
    unsigned short ChromaShift;

#define                                             LUMA_F                                              CHROMA_SHIFT + 0x2
    unsigned short LumaF;
    unsigned short LumaPadding;

#define                                             CHROMA_F                                            LUMA_F +0x4
    unsigned short ChromaF;
    unsigned short ChromaPadding;
    //
	//unsigned char Padding_2[4];					// 0x0004

	// Macro block data structures
#define												SDMOF_FixedBuffs									0x1270
   cMPCfb FixedBuffs[MAX_FRAME_WIDTH_MB]; // MAX_FRAME_WIDTH_MB*MPO_SIZE_OF_FIXED_ENTRY
#define												SDMOF_CircularBuffs									(0x1270+(MAX_FRAME_WIDTH_MB*MPO_SIZE_OF_FIXED_ENTRY))
   cMPCcb CircularBuffs[MAX_CIRC_BUFFS]; // MAX_CIRC_BUFFS*MPO_SIZE_OF_CIRCULAR_BUF_ENTRY

#define                                             LUMA_CHROMA_F                                       ((0x1270+(MAX_FRAME_WIDTH_MB*MPO_SIZE_OF_FIXED_ENTRY)) + (MAX_CIRC_BUFFS * MPO_SIZE_OF_CIRCULAR_BUF_ENTRY))
   unsigned short h264_LumaChromaFForIntraOrISlice[8];
   unsigned short Alignment_LumaChromaFForIntraOrISlice[8];

#define                                             CHROMA_U_WORKING_ARRAY                                (LUMA_CHROMA_F+32)
   unsigned char ChromaWorkingArrayU[9][16];
#define                                             CHROMA_V_WORKING_ARRAY                                CHROMA_U_WORKING_ARRAY + 16*9
   unsigned char ChromaWorkingArrayV[9][16];



	unsigned char padding8[8];

#define                                             LUMA_DIV                                           CHROMA_V_WORKING_ARRAY + 16*9 + 8
   int16 LumaDiv;

#define                                             CHROMA_DIV                                            LUMA_DIV + 2
   int16 ChromaDiv;


   // WhitePredError needs to be aligned to 128 bits
   #define                                             SDMOF_reservedAlignWhitePredErrorTo128                                (CHROMA_DIV + 0x2)
   u_int8 reservedAlignWhitePredErrorTo128[4]; // 0x4

   #define                                             SDMOF_WhitePredError                                (SDMOF_reservedAlignWhitePredErrorTo128 + 0x4)
    int16 WhitePredError[256]; // 0x200

#define                                             SDMOF_h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32		(SDMOF_WhitePredError + 0x200)
    u_int8 h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32[52]; // 0x34

#define                                             SDMOF_h264_encoder_rd_sad_lambda                    (SDMOF_h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32 + 0x34)
    u_int8 h264_encoder_rd_sad_lambda[52]; // 0x34
    
    u_int8 AlignmentTransformCoeffBuffer[8];
    
#define 										TRANSFORM_Y        (SDMOF_h264_encoder_rd_sad_lambda + 0x34 + 0x8)
#define 										TRANSFORM_U        (TRANSFORM_Y + 32*16)
#define 										TRANSFORM_V        (TRANSFORM_U + 32/2)

    u_int8 TransformCoeffBuffer[768];

#define											SDM_ME_REFERENCE_ARRAY	(TRANSFORM_Y + 768)
	u_int8 MotionEstReferenceData[ME_REFERENCE_ARRAY_STRIDE*ME_REFERENCE_ARRAY_STRIDE];

#define                                         SDM_ME_WORKING_ARRAY  (SDM_ME_REFERENCE_ARRAY + ME_REFERENCE_ARRAY_STRIDE * ME_REFERENCE_ARRAY_STRIDE)
    u_int8 MotionEstWorkingData[ME_REFERENCE_ARRAY_STRIDE*ME_REFERENCE_ARRAY_STRIDE];

#define											SDM_ME_OFFSET_TABLE (SDM_ME_WORKING_ARRAY + ME_REFERENCE_ARRAY_STRIDE * ME_REFERENCE_ARRAY_STRIDE)
	u_int16 MotionEstOffset[2][8];
#define											SDM_LANE_LOOKUP (SDM_ME_OFFSET_TABLE + 32)
	u_int16 LaneLookUp[8];

	void CopyToFixedAfterReconstructH264(int cirIdx);
	void CopyToFixedAfterDeblockH264(int cirIdx);



} * WorkAreaSDM;


// Simple in place queue
// T = type
// SIZE = Max storage   
template<typename T, int SIZE>
class SimpQueue
{
	T Buffer[SIZE];
	T *Front;
	T *Back;
public:
	SimpQueue()
	{
		Front = Buffer+1;
		Back = Buffer;
	}
	

	inline void Push(T val)
	{
		*Front++ = val;
		if( Front == Buffer+SIZE)
		{
			Front = Buffer;
		}
#ifdef _DEBUG
		if( Front == Back)
		{
			printf("+++++ ERROR queue Full ++++++++\n");
		}
#endif		
	}
	
	inline T Pull()
	{
		Back++;
		if( Back == Buffer+SIZE)
		{
			Back = Buffer;
		}
#ifdef _DEBUG
		if( Front == Back)
		{
			printf("+++++ ERROR queue under run ++++++++\n");	
		}
#endif
		return *Back;
	}
};


// Buffer management for the MPC data structures in the SDM
class MPC_Manager
{
public:
	typedef int CBuf_Idx;	// Index in circular buffer
protected:
	CBuf_Idx AllocCirBufIdx;
public:

	// Block used/free etc
	struct ActMPC_info
	{
		bool IsUsed;		// Is not free
		bool LumaInpOk;		// ME has returned input block
		bool ChromaInpOk;	// Chroma input DMA started
		bool IntraOk;		// Intra done
		bool MotionEstOk;	// Motion estimation done or not needed
		bool SkippedOk;		// Skipped chroma cost done or not needed
		bool FwdTransformsOk;	// Forward Transforms done
		bool EntropyEncodeOk;	// done or not needed
		bool InvTransformsOk;   // ...
		bool DeblockOk;
		int Column;			// Macro block column
		int Row;			// Macroblock row
		int QuantValue;		// Quant value	(May change in life of MB)
		int BufferLimit;
		CBuf_Idx LeftIdx;		// Index of left macroblock in circular buffer
		struct
		{
			short Mode;
			short ChromaQ_IdxOffset;
			int LumaOffset;
		} Deblock;
	};


	struct Picture_suff
	{
		// In units of macrolocks
		short Width;
		short Hgt;
        signed char *SliceGroupMap;
	} Picture;

// Info on current macro block (ME intra )
	struct MacroBlock_CurrentlyInME_info
	{
		int Col;
		int Row;
	}CurrentMeMB;
	
    int CountInME;										// Counting in & out of ME (ME look ahead depth)
	SimpQueue<CBuf_Idx, 4> MeQueue;	                    // Queue of macroblock currently in ME (or channel around it)
	SimpQueue<CBuf_Idx, 4> EEQueue;                     // Queue of macroblock currently in EE (or channel around it)

    CBuf_Idx IdxSetupME;								// MPC index of mocroblock going into ME
	CBuf_Idx IdxFromME;									// Index of macroblock for results coming out of ME
    //CBuf_Idx IdxTransform;                              // MacroBlock index in transforms stage
    //CBuf_Idx IdxReconstruct;                            // MacroBlock index in reconstruction stage
    CBuf_Idx IdxSetupEE;                                // Macroblock going into EE
    CBuf_Idx IdxFromEE;                                 // Macroblock in stream write out stage
    
#define NO_INTRA_SCHED	0xffff
    // Used to delay start of intra until left MB reconstructed
    struct Pending_Intra
    {
    	CBuf_Idx	Idx;								// NO_INTRA_SCHED = Intra already scheduled or not required
    	int	ValidBits;									// Above & left valide for intra
    	bool Enable;									// Intra enabled for frame
    } IntraStatus;

    void ResetTopLeft();

// Deblock vars -- (Maybe moved to SDM) ----
	struct Deblock_stuff
	{
		short Col;
		short Row;
		short Mode;
		short ChromaQ_IdxOffset;
		int LumaOffset;
		int ChromaOffset;

		void Init(short mode, short chromaQ_IdxOffset)	// Reset deblock for start of frame
		{
			Row = 0;
			Col = 0;
			Mode = mode;
			ChromaQ_IdxOffset = chromaQ_IdxOffset;
			LumaOffset = 0;
			ChromaOffset = 0;
		}
	} Deblock;

  // VLC specific data struxtures
  struct Macroblock_CurrentlyInEE_Info
  {
    int MbNum;
    int LstMb;
    char StartCode[5];
    int SliceStarted;
    int SliceNotFinished;
    int Index;
    int WrSliceHdr;
    void Init(void)
    {
      for(int i=0;i<3;i++)
        StartCode[i] = 0;
      StartCode[3] = 1;
    }
    void SetMB(int last_mb)
    {
      MbNum = 0;
      LstMb = last_mb;
      SliceStarted = 0;	
      Index = 0;
      WrSliceHdr= 0;
      SliceNotFinished = 0;
    }
  } CurrentEEMb;

	// Sets picture size in units of MB
	void Init(short wid, short hgt);


	// Status of MPCs free/used etc
	ActMPC_info ActMPC[MAX_CIRC_BUFFS];

	MPC_Manager();
	~MPC_Manager();

	int AllocMPC();					// Returns index of free MPC in circular buffer/ActMPC or -1 for error
	void FreeMPC(int idx);
	

		int MacroBlockStartedCount;
		int MacroBlocksCompleteCount;
		int RestartCount;				// ME starts that failed due to lack of buffers
};

extern MPC_Manager MgrMPC;


// moved here from H264EncoderTables
extern const unsigned char h264_encoder_rd_sad_lambda[52];
extern const unsigned char h264_encoder_8X8_skip_threshold[52];
extern const unsigned char h264_encoder_8X8_skip_threshold_multMIN_SKIP_16X16_div32[52];


#endif
