
#ifndef ArcMp4IntraLumaPrediction_MPA_H
#define ArcMp4IntraLumaPrediction_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcMp4IntraLumaPrediction_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int IntraMP4LumaPrediction[6];
extern int IntraMP4LumaPrediction_Record(int sa, int SIMDnum = 0);
extern const int SIMD_IDX_CalculateDC8;
extern const int MACRO_IDX_IntraModeCostmp4;
extern const int MACRO_IDX_IntraModeCost263;

#define SIMD_SUB_CalculateDC8 IntraMP4LumaPrediction[SIMD_IDX_CalculateDC8]
#define MACRO_IntraModeCostmp4 IntraMP4LumaPrediction[MACRO_IDX_IntraModeCostmp4]
#define MACRO_IntraModeCost263 IntraMP4LumaPrediction[MACRO_IDX_IntraModeCost263]

#endif // ArcMp4IntraLumaPrediction_MPA_H
