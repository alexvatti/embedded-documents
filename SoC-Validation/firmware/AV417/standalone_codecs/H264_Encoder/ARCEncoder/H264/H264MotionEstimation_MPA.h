
#ifndef H264MotionEstimation_MPA_H
#define H264MotionEstimation_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\H264MotionEstimation_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int H264MotionEstimation[8];
extern int H264MotionEstimation_Record(int sa, int SIMDnum = 0);
// pubreg bestInterCost
#define REG_ChooseInterOrIntra_bestInterCost	_vr02
#define MSK_ChooseInterOrIntra_bestInterCost	3
#define SET_ChooseInterOrIntra_bestInterCost(a)	_vr02 = _vmov(a, 3)

// pubreg cIdx
#define REG_ChooseInterOrIntra_cIdx	_vr00
#define MSK_ChooseInterOrIntra_cIdx	1
#define SET_ChooseInterOrIntra_cIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_CalculateSkippedChromaCost;
extern const int MACRO_IDX_MakeLumaChromaPredictionInterError;
extern const int MACRO_IDX_ChooseInterOrIntra;

#define MACRO_CalculateSkippedChromaCost H264MotionEstimation[MACRO_IDX_CalculateSkippedChromaCost]
#define MACRO_MakeLumaChromaPredictionInterError H264MotionEstimation[MACRO_IDX_MakeLumaChromaPredictionInterError]
#define MACRO_ChooseInterOrIntra H264MotionEstimation[MACRO_IDX_ChooseInterOrIntra]

#endif // H264MotionEstimation_MPA_H
