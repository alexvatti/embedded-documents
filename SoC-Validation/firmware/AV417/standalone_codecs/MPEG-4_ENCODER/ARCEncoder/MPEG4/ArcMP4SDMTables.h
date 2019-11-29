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


#ifndef INCLUDED_ARC_SDM_TABLES
#define INCLUDED_ARC_SDM_TABLES

#ifdef _DEBUG
#include <stdio.h>
#endif

#include "ArcMPC.h"



#define ABC0_C0 0
#define ABC0_BETA 2
#define ABC0_ALPHA  3

#define MAX_CIRC_BUFFS 6
// this define is also in ArcFrameBuffers.h
#define CHROMA_WORKING_ARRAY_STRIDE 16
#define ME_REFERENCE_ARRAY_STRIDE	32

extern struct SDM_WorkArea
{
	static const int MAX_CIRCULAR_BUFFS = 6;
	static void Init_SDM_WorkArea();

//Frame tables for DMA
// this first define is also in ArcFrameBuffers.h
#define 											SDMOF_FrameTabDMA									0x0000
	qword	FrameTabDMA[16];			// 0x0100 bytes
// working arrays
#define												SDMOF_IntraPredictionResidsLuma						0x0100
	int16 IntraPredictionResidsLuma[256]; // 0x0200
#define                                             CHROMA_U_WORKING_ARRAY                              SDMOF_IntraPredictionResidsLuma + 0x200
   unsigned char ChromaWorkingArrayU[9][16];
#define                                             CHROMA_V_WORKING_ARRAY                              CHROMA_U_WORKING_ARRAY + 16*9
   unsigned char ChromaWorkingArrayV[9][16];
// block offset arrays
#define												SDMOF_mpeg4_raw_pixel_offset						CHROMA_V_WORKING_ARRAY + 16*9
   unsigned short mpeg4_raw_pixel_offset[6];
#define												SDMOF_mpeg4_diff_pixel_offset						SDMOF_mpeg4_raw_pixel_offset + 12
   unsigned short mpeg4_diff_pixel_offset[6];
#define												SDMOF_mpeg4_pred_pixel_offset						SDMOF_mpeg4_diff_pixel_offset + 12
   unsigned short mpeg4_pred_pixel_offset[6];
#define												SDMOF_mpeg4_final_pixel_offset						SDMOF_mpeg4_pred_pixel_offset + 12
   unsigned short mpeg4_final_pixel_offset[6];

// Macro block data structures
#define												SDMOF_FixedBuffs									SDMOF_mpeg4_final_pixel_offset + 12
   cMPCfb FixedBuffs[MAX_FRAME_WIDTH_MB]; // MAX_FRAME_WIDTH_MB*MPO_SIZE_OF_FIXED_ENTRY
#define												SDMOF_CircularBuffs									(SDMOF_FixedBuffs+(MAX_FRAME_WIDTH_MB*MPO_SIZE_OF_FIXED_ENTRY))
   cMPCcb CircularBuffs[MAX_CIRCULAR_BUFFS]; // MAX_CIRCULAR_BUFFS*MPO_SIZE_OF_CIRCULAR_BUF_ENTRY


//these bits only needed for mpeg4  = 0x4c0
#define												SDMOF_mpeg4_dct_quant_scaling_table					(SDMOF_CircularBuffs + (MAX_CIRC_BUFFS * MPO_SIZE_OF_CIRCULAR_BUF_ENTRY))
    unsigned short mpeg4_dct_quant_scaling_table[2][64]; // 0x0100
#define												SDMOF_mpeg4_dct_dequant_scaling_table				(SDMOF_mpeg4_dct_quant_scaling_table+0x100)
	unsigned short mpeg4_dct_dequant_scaling_table[2][64]; // 0x0100
#define												SDMOF_mpeg4_aan_luma_dc_scaler						(SDMOF_mpeg4_dct_dequant_scaling_table+0x100)
	unsigned short mpeg4_luma_aan_dc_scaler[32];	// 0x40
#define												SDMOF_mpeg4_aan_chroma_dc_scaler					(SDMOF_mpeg4_aan_luma_dc_scaler+0x40)
	unsigned short mpeg4_chroma_aan_dc_scaler[32];	// 0x40
#define												SDMOF_mpeg4_fwd_luma_dc_scaler						(SDMOF_mpeg4_aan_chroma_dc_scaler+0x40)
	unsigned char mpeg4_fwd_luma_dc_scaler[32];		// 0x20
#define												SDMOF_mpeg4_fwd_chroma_dc_scaler					(SDMOF_mpeg4_fwd_luma_dc_scaler+0x20)
	unsigned char mpeg4_fwd_chroma_dc_scaler[32];	// 0x20
#define												SDMOF_mpeg4_luma_dc_scaler							(SDMOF_mpeg4_fwd_chroma_dc_scaler+0x20)
	unsigned int mpeg4_luma_dc_scaler[32];			// 0x80
#define												SDMOF_mpeg4_chroma_dc_scaler						(SDMOF_mpeg4_luma_dc_scaler+0x80)
	unsigned int mpeg4_chroma_dc_scaler[32];		// 0x80
#define												SDMOF_mpeg4_DC_bits_plus_256						(SDMOF_mpeg4_chroma_dc_scaler +0x80)
   unsigned char	mpeg4_DC_bits_plus_256[512];	// 0x200
  
#define											SDM_ME_REFERENCE_ARRAY	(SDMOF_mpeg4_DC_bits_plus_256 + 512)
	u_int8 MotionEstReferenceData[ME_REFERENCE_ARRAY_STRIDE*ME_REFERENCE_ARRAY_STRIDE];

#define											SDM_ME_OFFSET_TABLE (SDM_ME_REFERENCE_ARRAY + ME_REFERENCE_ARRAY_STRIDE * ME_REFERENCE_ARRAY_STRIDE)
	u_int16 MotionEstOffset[2][8];
#define											SDM_LANE_LOOKUP (SDM_ME_OFFSET_TABLE + 32)
	u_int16 LaneLookUp[8];

    void CopyToFixedAfterReconstructMP4(int cirIdx);

// Temp section start -----------------------------------------------
// Data that will be transfered as part of circular buffer pipeline
	const static int DEBLOCK_Q_SIZE = 2;
	void AddForDeblock(int cIdxDb);
	int GetForDeblock();
	int16 DeblockQ[DEBLOCK_Q_SIZE];


// Temp section end -------------------------------------------------

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
	// Debug state of MB in pipeline
	enum MPC_State { InME, InIntra, InSelect, InTransform, InQuant, InSmallCoef, InIQuant, InITrans, InDeblock};

	typedef unsigned int CBuf_Idx;	// Index in circular buffer

protected:
	CBuf_Idx AllocCirBufIdx;

public:
	// Block used/free etc
	struct ActMPC_info
	{
		bool IsUsed;		// Is not free
		bool LumaInpOk;		// ME has returned input block
		bool IntraOk;		// Intra cost calculation complete
		bool MotionEstOk;	// Motion estimation done or not needed
		bool SkippedOk;		// Skipped chroma cost done or not needed
		bool FwdTransformsOk;	// Forward Transforms done
		bool EntropyEncodeOk;	// done or not needed
		bool InvTransformsOk;   // ...
		bool EENotStarted;		// Set by Service_DoEntropyEncode() when it kicks off EE
		int Column;			// Macro block column
		int Row;			// Macroblock row
		int MB_number;		// Macroblock index number
		int QuantValue;		// Quant value	(May change in life of MB)
		int Dquant; 		// Quant differential
		int BufferLimit;
		unsigned char MBType; // Macroblock type
		CBuf_Idx LeftIdx;		// Index of left macroblock in circular buffer
		MPC_State State;	// Should be debug only
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
		int QuantValue;
		CBuf_Idx CirBufIdx; 
	}CurrentMeMB;

	SimpQueue<CBuf_Idx, 4> MeQueue;	                    // Queue of macroblock currently in ME (or channel around it)
	SimpQueue<CBuf_Idx, 4> EEQueue;                     // Queue of macroblock currently in EE (or channel around it)

    CBuf_Idx IdxSetupME;								// MPC index of mocroblock going into ME
	CBuf_Idx IdxFromME;									// Index of macroblock for results coming out of ME
    CBuf_Idx IdxTransform;                              // MacroBlock index in transforms stage
    CBuf_Idx IdxReconstruct;                            // MacroBlock index in reconstruction stage
    CBuf_Idx IdxSetupEE;                                // Macroblock going into EE
    CBuf_Idx IdxFromEE;                                 // Macroblock in stream write out stage

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
    int Row;
    int Col;
    int MbNum;
    int LstMb;
    char StartCode[5];
    int SliceStarted;
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
	  Row = Col = 0;
      LstMb = last_mb;
	  Index = 0;
      SliceStarted = 0;	// Hack delete
      WrSliceHdr= 0;
    }
  } CurrentEEMb;

	// Sets picture size in units of MB
	void Init(short wid, short hgt);


	// Step on to next macroblcok for prediction
	void NextCurrentMacroblock();

    void SetupMacroBlock();

	// Status of MPCs free/used etc
	ActMPC_info ActMPC[SDM_WorkArea::MAX_CIRCULAR_BUFFS];

	MPC_Manager();
	~MPC_Manager();

	int AllocMPC();					// Returns index of free MPC in circular buffer/ActMPC or -1 for error
	void FreeMPC(int idx);

	int MacroBlockStartedCount;
	int MacroBlocksCompleteCount;
	int MacroBlockThroughSimdCount;
	int MacroBlockThroughDblkCount;

};

extern MPC_Manager MgrMPC;



extern  const short ff_mpeg4_default_intra_matrix[64];
extern  const short ff_mpeg4_default_non_intra_matrix[64];
extern unsigned char uni_DCtab_lum_len2[512];
extern unsigned short y_dc_aan_scale_table[32];
extern unsigned short c_dc_aan_scale_table[32];
extern unsigned short MP4RawBlockOffset[6];
extern unsigned short MP4DiffBlockOffset[6];
extern unsigned short MP4PredBlockOffset[6];
extern unsigned short MP4FinalBlockOffset[6];
extern const unsigned char y_dc_scale_table[32];
extern const unsigned char c_dc_scale_table[32];
extern unsigned int y_dc_scale_table_shift20[32];
extern unsigned int c_dc_scale_table_shift20[32];
extern const short software_me_lanes[8];
extern const short software_me_offset[2][8];
_Asm void ASM_SetupDma(unsigned int sdmAddr, unsigned int extMemAddr, unsigned int blkSize)
{
    %reg sdmAddr, extMemAddr, blkSize
.off percent_reg
    vmov.3      vr00, sdmAddr
    vmov.12     vr00, extMemAddr
    vmov.48     vr00, blkSize
.on percent_reg
}

inline SetupDma(unsigned int sdmAddr, unsigned int extMemAddr, unsigned int size)
{
    ASM_SetupDma(sdmAddr, extMemAddr, (31<<16) | (1<<8) | (size & 0xff));
}

_Asm void ASM_SetupDmaQuant(unsigned int cbIdx, unsigned int sdmAddr, unsigned int extMemAddr, unsigned int blkSize)
{
    %reg cbIdx, sdmAddr, extMemAddr, blkSize
.off percent_reg
    vmov.3      vr00, sdmAddr
    vmov.12     vr00, extMemAddr
    vmovw.1     vr01, cbIdx
    vmulw.1     vr01, vr01, MPO_SIZE_OF_CIRCULAR_BUF_ENTRY   // cIdx'leftIdx cIdx'leftIdx [   73] 
    vim         i8, i8, SDMOF_CircularBuffs                  // CirBuf cIdx [   74] 
    vmov.12     vr03, 0
    vld8w_2     vr03, [i8, MPO_QuantValue]
    vmulw.4     vr03, vr03, 128
    vadd.12     vr00, vr00, vr03
    vmov.48     vr00, blkSize
.on percent_reg
}

inline SetupDmaQuant(unsigned int cbIdx, unsigned int sdmAddr, unsigned int extMemAddr, unsigned int size)
{
    ASM_SetupDmaQuant(cbIdx, sdmAddr, extMemAddr, (31<<16) | (1<<8) | (size & 0xff));
}

#endif
