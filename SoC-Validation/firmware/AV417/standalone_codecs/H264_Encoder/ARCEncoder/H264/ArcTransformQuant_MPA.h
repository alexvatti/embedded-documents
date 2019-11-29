
#ifndef ArcTransformQuant_MPA_H
#define ArcTransformQuant_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcTransformQuant_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Transform_Buffers[20];
extern int Transform_Buffers_Record(int sa, int SIMDnum = 0);
// pubreg SliceType
#define REG_SetupQuantValues_SliceType	_vr02
#define MSK_SetupQuantValues_SliceType	8
#define SET_SetupQuantValues_SliceType(a)	_vr02 = _vmovw(a, 8)

// pubreg ChromaQpIndexOffset
#define REG_SetupQuantValues_ChromaQpIndexOffset	_vr02
#define MSK_SetupQuantValues_ChromaQpIndexOffset	1
#define SET_SetupQuantValues_ChromaQpIndexOffset(a)	_vr02 = _vmovw(a, 1)

// pubreg QuantVal
#define REG_SetupQuantValues_QuantVal	_vr04
#define MSK_SetupQuantValues_QuantVal	1
#define SET_SetupQuantValues_QuantVal(a)	_vr04 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackDoFwdTransforms_BufIdx	_vr00
#define MSK_CallBackDoFwdTransforms_BufIdx	1
#define SET_CallBackDoFwdTransforms_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackDoEntropyEncode_BufIdx	_vr00
#define MSK_CallBackDoEntropyEncode_BufIdx	1
#define SET_CallBackDoEntropyEncode_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackDoInverseTransforms_BufIdx	_vr00
#define MSK_CallBackDoInverseTransforms_BufIdx	1
#define SET_CallBackDoInverseTransforms_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg LastTransQuant
#define REG_RevertToSkipped_LastTransQuant	_vr00
#define MSK_RevertToSkipped_LastTransQuant	1
#define SET_RevertToSkipped_LastTransQuant(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_SetupQuantValues;
extern const int MACRO_IDX_TransformQuantiseLumaDC;
extern const int SIMD_IDX_TransformQuantiseUVDC;
extern const int MACRO_IDX_TransformQuantiseChromaDC;
extern const int SIMD_IDX_Forward4x4TransformQuantise;
extern const int SIMD_IDX_ZeroBlocks;
extern const int MACRO_IDX_Forward4x4LumaTransformAndSmallCoeff;
extern const int MACRO_IDX_Forward4x4ChromaTransformAndSmallCoeff;
extern const int SIMD_IDX_InverseTransformLumaDC;
extern const int SIMD_IDX_Inverse4x4CoeffTransform;
extern const int MACRO_IDX_InverseQuantTransformCoeffs;
extern const int MACRO_IDX_InverseTransformDequantLumaCoeffs;
extern const int MACRO_IDX_InverseTransformDequantChromaCoeffs;
extern const int MACRO_IDX_CallBackDoFwdTransforms;
extern const int MACRO_IDX_CallBackDoEntropyEncode;
extern const int MACRO_IDX_CallBackDoInverseTransforms;
extern const int MACRO_IDX_RevertToSkipped;

#define MACRO_SetupQuantValues Transform_Buffers[MACRO_IDX_SetupQuantValues]
#define MACRO_TransformQuantiseLumaDC Transform_Buffers[MACRO_IDX_TransformQuantiseLumaDC]
#define SIMD_SUB_TransformQuantiseUVDC Transform_Buffers[SIMD_IDX_TransformQuantiseUVDC]
#define MACRO_TransformQuantiseChromaDC Transform_Buffers[MACRO_IDX_TransformQuantiseChromaDC]
#define SIMD_SUB_Forward4x4TransformQuantise Transform_Buffers[SIMD_IDX_Forward4x4TransformQuantise]
#define SIMD_SUB_ZeroBlocks Transform_Buffers[SIMD_IDX_ZeroBlocks]
#define MACRO_Forward4x4LumaTransformAndSmallCoeff Transform_Buffers[MACRO_IDX_Forward4x4LumaTransformAndSmallCoeff]
#define MACRO_Forward4x4ChromaTransformAndSmallCoeff Transform_Buffers[MACRO_IDX_Forward4x4ChromaTransformAndSmallCoeff]
#define SIMD_SUB_InverseTransformLumaDC Transform_Buffers[SIMD_IDX_InverseTransformLumaDC]
#define SIMD_SUB_Inverse4x4CoeffTransform Transform_Buffers[SIMD_IDX_Inverse4x4CoeffTransform]
#define MACRO_InverseQuantTransformCoeffs Transform_Buffers[MACRO_IDX_InverseQuantTransformCoeffs]
#define MACRO_InverseTransformDequantLumaCoeffs Transform_Buffers[MACRO_IDX_InverseTransformDequantLumaCoeffs]
#define MACRO_InverseTransformDequantChromaCoeffs Transform_Buffers[MACRO_IDX_InverseTransformDequantChromaCoeffs]
#define MACRO_CallBackDoFwdTransforms Transform_Buffers[MACRO_IDX_CallBackDoFwdTransforms]
#define MACRO_CallBackDoEntropyEncode Transform_Buffers[MACRO_IDX_CallBackDoEntropyEncode]
#define MACRO_CallBackDoInverseTransforms Transform_Buffers[MACRO_IDX_CallBackDoInverseTransforms]
#define MACRO_RevertToSkipped Transform_Buffers[MACRO_IDX_RevertToSkipped]

#endif // ArcTransformQuant_MPA_H
