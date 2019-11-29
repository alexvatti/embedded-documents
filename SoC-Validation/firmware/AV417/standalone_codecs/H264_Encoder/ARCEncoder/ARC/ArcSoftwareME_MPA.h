
#ifndef ArcSoftwareME_MPA_H
#define ArcSoftwareME_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareME_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Software_ME[10];
extern int Software_ME_Record(int sa, int SIMDnum = 0);
// pubreg IntraValid
#define REG_Generate_Soft_ME_Response_IntraValid	_vr01
#define MSK_Generate_Soft_ME_Response_IntraValid	64
#define SET_Generate_Soft_ME_Response_IntraValid(a)	_vr01 = _vmovw(a, 64)

// pubreg SkipVec
#define REG_Generate_Soft_ME_Response_SkipVec	_vr01
#define MSK_Generate_Soft_ME_Response_SkipVec	128
#define SET_Generate_Soft_ME_Response_SkipVec(a)	_vr01 = _vmovw(a, 128)

// pubreg BestCost
#define REG_Generate_Soft_ME_Response_BestCost	_vr01
#define MSK_Generate_Soft_ME_Response_BestCost	1
#define SET_Generate_Soft_ME_Response_BestCost(a)	_vr01 = _vmovw(a, 1)

extern const int MACRO_IDX_Generate_Soft_ME_Response;
extern const int SIMD_IDX_endofIter;
extern const int SIMD_IDX_initRDvals;
extern const int MACRO_IDX_IntegerME;
extern const int MACRO_IDX_SaveVectors;
extern const int MACRO_IDX_ZeroDisplacement;
extern const int MACRO_IDX_StepOne;
extern const int MACRO_IDX_FullStepOne;

#define MACRO_Generate_Soft_ME_Response Software_ME[MACRO_IDX_Generate_Soft_ME_Response]
#define SIMD_SUB_endofIter Software_ME[SIMD_IDX_endofIter]
#define SIMD_SUB_initRDvals Software_ME[SIMD_IDX_initRDvals]
#define MACRO_IntegerME Software_ME[MACRO_IDX_IntegerME]
#define MACRO_SaveVectors Software_ME[MACRO_IDX_SaveVectors]
#define MACRO_ZeroDisplacement Software_ME[MACRO_IDX_ZeroDisplacement]
#define MACRO_StepOne Software_ME[MACRO_IDX_StepOne]
#define MACRO_FullStepOne Software_ME[MACRO_IDX_FullStepOne]

#endif // ArcSoftwareME_MPA_H
