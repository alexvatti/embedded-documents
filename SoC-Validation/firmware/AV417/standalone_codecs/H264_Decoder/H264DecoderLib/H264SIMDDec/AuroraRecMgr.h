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

#ifndef _AURORARECMGR_H_
#define _AURORARECMGR_H_


enum chroma_block_indx
{
	 CHROMA_BLK_IDX_2x2 = 0,
	 CHROMA_BLK_IDX_2x4	= 1,
	 CHROMA_BLK_IDX_4x2	= 4,
	 CHROMA_BLK_IDX_4x4	= 5,
	 CHROMA_BLK_IDX_4x8	= 6,
	 CHROMA_BLK_IDX_8x4	= 9,
	 CHROMA_BLK_IDX_8x8	= 10
};

// Indexes for Aurora Macros
enum e_macro_index
{
    // Deblock
    AM_DB_HorzontalFilterAligned,        // /   Do not reorder  \   //
    AM_DB_HorzontalFilter,              //  \   Do not reorder  /  //
    AM_DB_NormalVerticalFilter,
    AM_DB_StrongVerticalFilter,
    AM_DB_EdgeChromaHorizontal,
    AM_DB_EdgeChromaHorizontalStrong,
    AM_DB_EdgeChromaVertical,
    AM_DB_EdgeChromaVerticalStrong,
    AM_DB_MoveForNextBlockY,
    AM_DB_MoveForNextBlockU,
    AM_DB_MoveForNextBlockV,
    AM_DB_VerticalFilter2,
    
    // Transform Macros
    AM_Xform_Luma,
    AM_Xform_Chroma,

	// InterPred Macros
	AM_Inter_FilterLuma,
	AM_Inter_Luma8x8VerFilterAndRound,
	AM_Inter_Luma4x8VerFilterAndRound,
	AM_Inter_Luma8x8VerFilterWORounding,
	AM_Inter_Luma4x8VerFilterWORounding,

	AM_Inter_LumaHorFiltering_HBlkSz_4_8,
	AM_Inter_LumaHorFiltering_HBlkSz_16,

	AM_Inter_CPelFilterWOInpRnd_HBlkSz_4,
	AM_Inter_CPelFilterWOInpRnd_HBlkSz_8,
	AM_Inter_CPelFilterWOInpRnd_HBlkSz_16,
	AM_Inter_CPelFilterAndInpRnd_HBlkSz_4,
	AM_Inter_CPelFilterAndInpRnd_HBlkSz_8,
	AM_Inter_CPelFilterAndInpRnd_HBlkSz_16,

	AM_Inter_LumaQPelFiltering_HBlkSz_4_8,
	AM_Inter_LumaQPelFiltering_HBlkSz_16,
	AM_Inter_LumaShft2QPelFil_HBlkSz_4,
	AM_Inter_LumaShft2QPelFil_HBlkSz_8,
	AM_Inter_LumaShft2QPelFil_HBlkSz_16,
	AM_Inter_LumaShft3QPelFil_HBlkSz_4,
	AM_Inter_LumaShft3QPelFil_HBlkSz_8,
	AM_Inter_LumaShft3QPelFil_HBlkSz_16,

	AM_Inter_FilterChroma,
	AM_Inter_FilterChromaBlkSz_Nx2,
	AM_Inter_FilterChromaBlkSz_Nx4,
	AM_Inter_FilterChromaBlkSz_4x8,
	AM_Inter_FilterChromaBlkSz_8x4,
	AM_Inter_FilterChromaBlkSz_8x8,

	// Luma Reconstruction
	AM_Inter_ReconLuma_HBlkSz_4,
	AM_Inter_ReconLuma_HBlkSz_8,
	AM_Inter_ReconLuma_HBlkSz_16,

	// Chroma Reconstruction
	AM_Inter_ChromaReconBlkSz_2x2,
	AM_Inter_ChromaReconBlkSz_2x4,
	AM_Inter_ChromaReconBlkSz_4x2,
	AM_Inter_ChromaReconBlkSz_4x4,
	AM_Inter_ChromaReconBlkSz_4x8,
	AM_Inter_ChromaReconBlkSz_8x4,
	AM_Inter_ChromaReconBlkSz_8x8,
	
	// IntraPred Macros
	AM_Intra_Luma16x16,
	AM_Intra_Luma16x8VerPred,
	AM_Intra_Luma16x4HorPred,
	AM_Intra_Luma16DCVerMask,
	AM_Intra_Luma16DCHorMask,
	AM_Intra_Luma16DCVerAndHorMask,
	AM_Intra_Luma16x8DCPred,
	AM_Intra_Luma16x16CalcPlaneModeParams,
	AM_Intra_Luma16x4PlanarPred,

	AM_Intra_Luma4x4,
	AM_Intra_Luma4x4VerPred,
	AM_Intra_Luma4x4HorPred,
	AM_Intra_Luma4x4DCVer,
	AM_Intra_Luma4x4DCHor,
	AM_Intra_Luma4x4DCVerAndHor,
	AM_Intra_Luma4x4DCConst,
	AM_Intra_Luma4x4DiagDownLeftWOExtrp,
	AM_Intra_Luma4x4DiagDownLeftWithExtrp,
	AM_Intra_Luma4x4VerLeftWOExtrp,
	AM_Intra_Luma4x4VerLeftWithExtrp,
	AM_Intra_Luma4x4DiagDownRight,   
	AM_Intra_Luma4x4VerRight,
	AM_Intra_Luma4x4HorDown, 
	AM_Intra_Luma4x4HorUp,

	
	AM_Intra_Chroma,
	AM_Intra_ChromaDCConstMask,
	AM_Intra_ChromaDCVerMask,
	AM_Intra_ChromaDCHorMask,
	AM_Intra_ChromaDCVerAndHorMask,
	AM_Intra_ChromaDCPred,
	AM_Intra_Chroma8x4HorPred,
	AM_Intra_ChromaVerPredMask,
	AM_Intra_Chroma8x8PlanarPred,

	// IntraPCM Macros
	AM_IntraPCM_Luma,
	AM_IntraPCM_Chroma,

	// Misc Macros
	AM_SDM_Mem_Clear,
	
	// DecodeSliceData
	
	
	// I PCM Macros
	AM_PCM_IntraReconPCMLuma16x8,
	AM_PCM_IntraReconPCMChroma,
//#ifdef INLOOP_DBLOCK
	// Macros used for inloop deblocking
	AM_COPYTOPLINE16,
	AM_COPYTOPLINE20,
	AM_COPYTOPLINE24,
	AM_COPYTOPLINE8,
	AM_COPYTOPLINE4,
	AM_COPYTOPLINE12,
	AM_COPY16x16_OFF_24_720,
	AM_COPY16x4_OFF_24_720,
	AM_COPY8x8_OFF_16_360,
	AM_COPY8x2_OFF_16_360,
	AM_COPY8x2_OFF_360_16_DBINPUT,
	AM_COPY16X4_OFF_720_24_DBINPUT,
	AM_COPY16X16_OFF_16_24_DBINPUT,
	AM_COPY8X8_OFF_8_16_DBINPUT
//#endif
};



class AuroraRecMgr
{
    int MacroAddr[256];
    int NextAddr;
public:

	short ChromaBlockMacros[11];


	AuroraRecMgr();
	/*virtual */ ~AuroraRecMgr();
        
    inline void Run(e_macro_index n) const
    {
        _vrun(MacroAddr[n]);
    }
        
    inline int GetMacroAddr(e_macro_index n) const
    {
        return MacroAddr[n];
    }
        
    inline void Record(int idx, int end)
    {
        MacroAddr[idx] = NextAddr;
        NextAddr = end+1;
    }
        
    void RecordDeblockMacros();

	void RecordXformMacros();
	void RecordInterPredMacros();
	void RecordLumaInterReconMacros();
	void RecordChromaInterReconMacros();
	void RecordChromaInterPredMacros();
	void RecordChromaIntraPredMacros();
	void RecordLuma16x16IntraPredMacros();
	void RecordLuma4x4IntraPredMacros();
	void RecordLumaInterPredVerFilterMacros();
	void RecordLumaInterPredHorFilterMacros();
	void RecordLumaCtrPelFilteringMacros();
	void RecordLumaInterPredQPelFilterMacros();
	void RecordSDMMemClearMacro();
	void RecordDecodeSliceDataMacros();
	void RecordIntraPCM_Macros();
//#ifdef INLOOP_DBLOCK
	void RecordInloopDBMacros();
//#endif	
};

#endif //_AURORARECMGR_H_
