
#ifndef ArcIntraLumaPrediction_MPA_H
#define ArcIntraLumaPrediction_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraLumaPrediction_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int IntraLumaPrediction[6];
extern int IntraLumaPrediction_Record(int sa, int SIMDnum = 0);
// pubreg intraValidFlag
#define REG_Global_intraValidFlag	_vr00
#define MSK_Global_intraValidFlag	1
#define SET_Global_intraValidFlag(a)	_vr00 = _vmovw(a, 1)

// pubreg gUseIntraLumaDCPredOnly
#define REG_Global_gUseIntraLumaDCPredOnly	_vr05
#define MSK_Global_gUseIntraLumaDCPredOnly	4
#define SET_Global_gUseIntraLumaDCPredOnly(a)	_vr05 = _vmovw(a, 4)

// pubreg gUseWhitening
#define REG_Global_gUseWhitening	_vr05
#define MSK_Global_gUseWhitening	8
#define SET_Global_gUseWhitening(a)	_vr05 = _vmovw(a, 8)

// pubreg constIntraPred
#define REG_ConstrainIntraValidFlag_constIntraPred	_vr01
#define MSK_ConstrainIntraValidFlag_constIntraPred	1
#define SET_ConstrainIntraValidFlag_constIntraPred(a)	_vr01 = _vmovw(a, 1)

extern const int MACRO_IDX_ConstrainIntraValidFlag;
extern const int SIMD_IDX_CalculateDC;
extern const int SIMD_IDX_Calculate4x4SADs;
extern const int SIMD_IDX_WhitenResidual_CalculateSAD;
extern const int SIMD_IDX_GenerateIntra16x16Residual;
extern const int MACRO_IDX_GenerateIntra16x16Prediction;

#define MACRO_ConstrainIntraValidFlag IntraLumaPrediction[MACRO_IDX_ConstrainIntraValidFlag]
#define SIMD_SUB_CalculateDC IntraLumaPrediction[SIMD_IDX_CalculateDC]
#define SIMD_SUB_Calculate4x4SADs IntraLumaPrediction[SIMD_IDX_Calculate4x4SADs]
#define SIMD_SUB_WhitenResidual_CalculateSAD IntraLumaPrediction[SIMD_IDX_WhitenResidual_CalculateSAD]
#define SIMD_SUB_GenerateIntra16x16Residual IntraLumaPrediction[SIMD_IDX_GenerateIntra16x16Residual]
#define MACRO_GenerateIntra16x16Prediction IntraLumaPrediction[MACRO_IDX_GenerateIntra16x16Prediction]

#endif // ArcIntraLumaPrediction_MPA_H
