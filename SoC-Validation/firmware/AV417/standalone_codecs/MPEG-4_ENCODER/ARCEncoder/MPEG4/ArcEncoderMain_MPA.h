
#ifndef ArcEncoderMain_MPA_H
#define ArcEncoderMain_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcEncoderMain_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int EncoderMain[15];
extern int EncoderMain_Record(int sa, int SIMDnum = 0);
// pubreg cbIdx
#define REG_RevertToSkipped_cbIdx	_vr00
#define MSK_RevertToSkipped_cbIdx	1
#define SET_RevertToSkipped_cbIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg dquant
#define REG_RevertToSkipped_dquant	_vr00
#define MSK_RevertToSkipped_dquant	2
#define SET_RevertToSkipped_dquant(a)	_vr00 = _vmovw(a, 2)

// pubreg LastTransQuant
#define REG_RevertToSkipped_LastTransQuant	_vr00
#define MSK_RevertToSkipped_LastTransQuant	4
#define SET_RevertToSkipped_LastTransQuant(a)	_vr00 = _vmovw(a, 4)

// pubreg BufIdx
#define REG_CallBackStartME_BufIdx	_vr00
#define MSK_CallBackStartME_BufIdx	1
#define SET_CallBackStartME_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackIntraComplete_BufIdx	_vr00
#define MSK_CallBackIntraComplete_BufIdx	1
#define SET_CallBackIntraComplete_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackDoFwdTransforms_BufIdx	_vr00
#define MSK_CallBackDoFwdTransforms_BufIdx	1
#define SET_CallBackDoFwdTransforms_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackDoEntropyEncode_BufIdx	_vr00
#define MSK_CallBackDoEntropyEncode_BufIdx	1
#define SET_CallBackDoEntropyEncode_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg dquant
#define REG_CallBackDoEntropyEncode_dquant	_vr00
#define MSK_CallBackDoEntropyEncode_dquant	2
#define SET_CallBackDoEntropyEncode_dquant(a)	_vr00 = _vmovw(a, 2)

// pubreg lastQuant
#define REG_CallBackDoEntropyEncode_lastQuant	_vr00
#define MSK_CallBackDoEntropyEncode_lastQuant	4
#define SET_CallBackDoEntropyEncode_lastQuant(a)	_vr00 = _vmovw(a, 4)

// pubreg BufIdx
#define REG_CallBackStartDeblock_BufIdx	_vr00
#define MSK_CallBackStartDeblock_BufIdx	1
#define SET_CallBackStartDeblock_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackReleaseBuffer_BufIdx	_vr00
#define MSK_CallBackReleaseBuffer_BufIdx	1
#define SET_CallBackReleaseBuffer_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackWriteOutMB_BufIdx	_vr00
#define MSK_CallBackWriteOutMB_BufIdx	1
#define SET_CallBackWriteOutMB_BufIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_CopyCurrentChromaToSDM;
extern const int MACRO_IDX_CopyChromaPredToSDM;
extern const int MACRO_IDX_OutputMacroblock;
extern const int MACRO_IDX_SdmDmaIn;
extern const int MACRO_IDX_RevertToSkipped;
extern const int MACRO_IDX_CopyMERefToSDM;
extern const int MACRO_IDX_CopyMBLumaToSDM;
extern const int MACRO_IDX_CallBackStartME;
extern const int MACRO_IDX_CallBackIntraComplete;
extern const int MACRO_IDX_CallBackDoFwdTransforms;
extern const int MACRO_IDX_CallBackDoEntropyEncode;
extern const int MACRO_IDX_CallBackStartDeblock;
extern const int MACRO_IDX_CallBackReleaseBuffer;
extern const int MACRO_IDX_CallBackWriteOutMB;

#define MACRO_CopyCurrentChromaToSDM EncoderMain[MACRO_IDX_CopyCurrentChromaToSDM]
#define MACRO_CopyChromaPredToSDM EncoderMain[MACRO_IDX_CopyChromaPredToSDM]
#define MACRO_OutputMacroblock EncoderMain[MACRO_IDX_OutputMacroblock]
#define MACRO_SdmDmaIn EncoderMain[MACRO_IDX_SdmDmaIn]
#define MACRO_RevertToSkipped EncoderMain[MACRO_IDX_RevertToSkipped]
#define MACRO_CopyMERefToSDM EncoderMain[MACRO_IDX_CopyMERefToSDM]
#define MACRO_CopyMBLumaToSDM EncoderMain[MACRO_IDX_CopyMBLumaToSDM]
#define MACRO_CallBackStartME EncoderMain[MACRO_IDX_CallBackStartME]
#define MACRO_CallBackIntraComplete EncoderMain[MACRO_IDX_CallBackIntraComplete]
#define MACRO_CallBackDoFwdTransforms EncoderMain[MACRO_IDX_CallBackDoFwdTransforms]
#define MACRO_CallBackDoEntropyEncode EncoderMain[MACRO_IDX_CallBackDoEntropyEncode]
#define MACRO_CallBackStartDeblock EncoderMain[MACRO_IDX_CallBackStartDeblock]
#define MACRO_CallBackReleaseBuffer EncoderMain[MACRO_IDX_CallBackReleaseBuffer]
#define MACRO_CallBackWriteOutMB EncoderMain[MACRO_IDX_CallBackWriteOutMB]

#endif // ArcEncoderMain_MPA_H
