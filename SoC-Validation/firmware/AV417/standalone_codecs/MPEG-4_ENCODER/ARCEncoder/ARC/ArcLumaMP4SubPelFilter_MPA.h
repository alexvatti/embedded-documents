
#ifndef ArcLumaMP4SubPelFilter_MPA_H
#define ArcLumaMP4SubPelFilter_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcLumaMP4SubPelFilter_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int LumaSubPelFilter[4];
extern int LumaSubPelFilter_Record(int sa, int SIMDnum = 0);
// pubreg BufIdx
#define REG_CallBackDoLumaSubPelFilter_BufIdx	_vr00
#define MSK_CallBackDoLumaSubPelFilter_BufIdx	1
#define SET_CallBackDoLumaSubPelFilter_BufIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_CopyLumaPredToSDM;
extern const int MACRO_IDX_CallBackDoLumaSubPelFilter;
extern const int MACRO_IDX_FilterLumaPred;
extern const int MACRO_IDX_StoreFilteredLumaPred;

#define MACRO_CopyLumaPredToSDM LumaSubPelFilter[MACRO_IDX_CopyLumaPredToSDM]
#define MACRO_CallBackDoLumaSubPelFilter LumaSubPelFilter[MACRO_IDX_CallBackDoLumaSubPelFilter]
#define MACRO_FilterLumaPred LumaSubPelFilter[MACRO_IDX_FilterLumaPred]
#define MACRO_StoreFilteredLumaPred LumaSubPelFilter[MACRO_IDX_StoreFilteredLumaPred]

#endif // ArcLumaMP4SubPelFilter_MPA_H
