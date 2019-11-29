
#ifndef ArcLumaH264SubPelFilter_MPA_H
#define ArcLumaH264SubPelFilter_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcLumaH264SubPelFilter_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int LumaSubPelFilter[9];
extern int LumaSubPelFilter_Record(int sa, int SIMDnum = 0);
// pubreg BufIdx
#define REG_CallBackDoLumaSubPelFilter_BufIdx	_vr00
#define MSK_CallBackDoLumaSubPelFilter_BufIdx	1
#define SET_CallBackDoLumaSubPelFilter_BufIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_CopyLumaPredToSDM;
extern const int MACRO_IDX_CallBackDoLumaSubPelFilter;
extern const int MACRO_IDX_PerformInpHorFiltering;
extern const int MACRO_IDX_QPelFiltering;
extern const int MACRO_IDX_VerticalFilteringAndRounding;
extern const int MACRO_IDX_VerticalFilteringWORounding;
extern const int MACRO_IDX_CentrePelFilterWOInpRounding;
extern const int MACRO_IDX_CentrePelFilAndInpRounding;
extern const int MACRO_IDX_StoreFilteredLumaPred;

#define MACRO_CopyLumaPredToSDM LumaSubPelFilter[MACRO_IDX_CopyLumaPredToSDM]
#define MACRO_CallBackDoLumaSubPelFilter LumaSubPelFilter[MACRO_IDX_CallBackDoLumaSubPelFilter]
#define MACRO_PerformInpHorFiltering LumaSubPelFilter[MACRO_IDX_PerformInpHorFiltering]
#define MACRO_QPelFiltering LumaSubPelFilter[MACRO_IDX_QPelFiltering]
#define MACRO_VerticalFilteringAndRounding LumaSubPelFilter[MACRO_IDX_VerticalFilteringAndRounding]
#define MACRO_VerticalFilteringWORounding LumaSubPelFilter[MACRO_IDX_VerticalFilteringWORounding]
#define MACRO_CentrePelFilterWOInpRounding LumaSubPelFilter[MACRO_IDX_CentrePelFilterWOInpRounding]
#define MACRO_CentrePelFilAndInpRounding LumaSubPelFilter[MACRO_IDX_CentrePelFilAndInpRounding]
#define MACRO_StoreFilteredLumaPred LumaSubPelFilter[MACRO_IDX_StoreFilteredLumaPred]

#endif // ArcLumaH264SubPelFilter_MPA_H
