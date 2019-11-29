
#ifndef ArcIntraChromaPrediction_MPA_H
#define ArcIntraChromaPrediction_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraChromaPrediction_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int IntraChromaPrediction[3];
extern int IntraChromaPrediction_Record(int sa, int SIMDnum = 0);
// pubreg gUseIntraChromaDCPredOnly
#define REG_Global_gUseIntraChromaDCPredOnly	_vr06
#define MSK_Global_gUseIntraChromaDCPredOnly	2
#define SET_Global_gUseIntraChromaDCPredOnly(a)	_vr06 = _vmovw(a, 2)

// pubreg sIdx
#define REG_SetIntraModeMPC_sIdx	_vr00
#define MSK_SetIntraModeMPC_sIdx	8
#define SET_SetIntraModeMPC_sIdx(a)	_vr00 = _vmovw(a, 8)

extern const int SIMD_IDX_GenerateIntra16x8Residual_Calculate16x8SAD;
extern const int MACRO_IDX_GenerateIntraChromaPrediction;
extern const int MACRO_IDX_SetIntraModeMPC;

#define SIMD_SUB_GenerateIntra16x8Residual_Calculate16x8SAD IntraChromaPrediction[SIMD_IDX_GenerateIntra16x8Residual_Calculate16x8SAD]
#define MACRO_GenerateIntraChromaPrediction IntraChromaPrediction[MACRO_IDX_GenerateIntraChromaPrediction]
#define MACRO_SetIntraModeMPC IntraChromaPrediction[MACRO_IDX_SetIntraModeMPC]

#endif // ArcIntraChromaPrediction_MPA_H
