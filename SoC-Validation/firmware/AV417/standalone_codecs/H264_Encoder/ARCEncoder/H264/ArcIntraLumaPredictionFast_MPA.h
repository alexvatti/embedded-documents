
#ifndef ArcIntraLumaPredictionFast_MPA_H
#define ArcIntraLumaPredictionFast_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcIntraLumaPredictionFast_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int IntraLumaPredictionFast[3];
extern int IntraLumaPredictionFast_Record(int sa, int SIMDnum = 0);
// pubreg intraValidFlag
#define REG_Global_intraValidFlag	_vr00
#define MSK_Global_intraValidFlag	1
#define SET_Global_intraValidFlag(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_GenerateIntra16x16PredictionFastAll;
extern const int MACRO_IDX_GenerateIntra16x16PredictionFastDC;
extern const int MACRO_IDX_GenerateIntra16x16PredictionFastVertical;

#define MACRO_GenerateIntra16x16PredictionFastAll IntraLumaPredictionFast[MACRO_IDX_GenerateIntra16x16PredictionFastAll]
#define MACRO_GenerateIntra16x16PredictionFastDC IntraLumaPredictionFast[MACRO_IDX_GenerateIntra16x16PredictionFastDC]
#define MACRO_GenerateIntra16x16PredictionFastVertical IntraLumaPredictionFast[MACRO_IDX_GenerateIntra16x16PredictionFastVertical]

#endif // ArcIntraLumaPredictionFast_MPA_H
