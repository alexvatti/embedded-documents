
#ifndef ArcSoftwareSubME_MPA_H
#define ArcSoftwareSubME_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareSubME_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Software_SubME[5];
extern int Software_SubME_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_HalfPelME;
extern const int MACRO_IDX_SAD_calc;
extern const int MACRO_IDX_Set_Skip_Large;

#define MACRO_HalfPelME Software_SubME[MACRO_IDX_HalfPelME]
#define MACRO_SAD_calc Software_SubME[MACRO_IDX_SAD_calc]
#define MACRO_Set_Skip_Large Software_SubME[MACRO_IDX_Set_Skip_Large]

#endif // ArcSoftwareSubME_MPA_H
