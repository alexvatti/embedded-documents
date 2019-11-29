
#ifndef ArcDeblock_MPA_H
#define ArcDeblock_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcDeblock_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Deblock[12];
extern int Deblock_Record(int sa, int SIMDnum = 0);
// pubreg cIdx
#define REG_SetCurrentMPC1_cIdx	_vr00
#define MSK_SetCurrentMPC1_cIdx	1
#define SET_SetCurrentMPC1_cIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg Column
#define REG_SetCurrentMPC1_Column	_vr00
#define MSK_SetCurrentMPC1_Column	2
#define SET_SetCurrentMPC1_Column(a)	_vr00 = _vmovw(a, 2)

// pubreg leftIdx
#define REG_SetCurrentMPC1_leftIdx	_vr00
#define MSK_SetCurrentMPC1_leftIdx	4
#define SET_SetCurrentMPC1_leftIdx(a)	_vr00 = _vmovw(a, 4)

// pubreg chromaQIO
#define REG_DeblockMacroBlockSetup_chromaQIO	_vr00
#define MSK_DeblockMacroBlockSetup_chromaQIO	1
#define SET_DeblockMacroBlockSetup_chromaQIO(a)	_vr00 = _vmovw(a, 1)

// pubreg DeblockMode
#define REG_DeblockMacroBlockSetup_DeblockMode	_vr00
#define MSK_DeblockMacroBlockSetup_DeblockMode	2
#define SET_DeblockMacroBlockSetup_DeblockMode(a)	_vr00 = _vmovw(a, 2)

// pubreg BufIdx
#define REG_CallBackReleaseBuffer_BufIdx	_vr00
#define MSK_CallBackReleaseBuffer_BufIdx	1
#define SET_CallBackReleaseBuffer_BufIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_SetCurrentMPC1;
extern const int MACRO_IDX_CopyToFixedAfterReconstructH264;
extern const int MACRO_IDX_DeblockMacroBlockSetup;
extern const int MACRO_IDX_DeblockFetchPixelsFromNeigbours;
extern const int MACRO_IDX_HorizontalDeblock;
extern const int MACRO_IDX_VerticalDeblock;
extern const int SIMD_IDX_DmaOutPixelData;
extern const int MACRO_IDX_WriteDeblockedBlock;
extern const int MACRO_IDX_WriteOutputBlock;
extern const int MACRO_IDX_CallBackReleaseBuffer;
extern const int MACRO_IDX_CopyToFixedAfterDeblockH264;

#define MACRO_SetCurrentMPC1 Deblock[MACRO_IDX_SetCurrentMPC1]
#define MACRO_CopyToFixedAfterReconstructH264 Deblock[MACRO_IDX_CopyToFixedAfterReconstructH264]
#define MACRO_DeblockMacroBlockSetup Deblock[MACRO_IDX_DeblockMacroBlockSetup]
#define MACRO_DeblockFetchPixelsFromNeigbours Deblock[MACRO_IDX_DeblockFetchPixelsFromNeigbours]
#define MACRO_HorizontalDeblock Deblock[MACRO_IDX_HorizontalDeblock]
#define MACRO_VerticalDeblock Deblock[MACRO_IDX_VerticalDeblock]
#define SIMD_SUB_DmaOutPixelData Deblock[SIMD_IDX_DmaOutPixelData]
#define MACRO_WriteDeblockedBlock Deblock[MACRO_IDX_WriteDeblockedBlock]
#define MACRO_WriteOutputBlock Deblock[MACRO_IDX_WriteOutputBlock]
#define MACRO_CallBackReleaseBuffer Deblock[MACRO_IDX_CallBackReleaseBuffer]
#define MACRO_CopyToFixedAfterDeblockH264 Deblock[MACRO_IDX_CopyToFixedAfterDeblockH264]

#endif // ArcDeblock_MPA_H
