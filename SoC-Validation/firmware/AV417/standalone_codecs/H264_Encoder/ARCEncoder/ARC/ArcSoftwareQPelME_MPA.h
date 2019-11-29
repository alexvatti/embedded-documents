
#ifndef ArcSoftwareQPelME_MPA_H
#define ArcSoftwareQPelME_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcSoftwareQPelME_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Software_QPelME[8];
extern int Software_QPelME_Record(int sa, int SIMDnum = 0);
extern const int SIMD_IDX_QuartPelME_half_half;
extern const int SIMD_IDX_QuartPelME_half_int;
extern const int SIMD_IDX_QuartPelME_int_half;
extern const int SIMD_IDX_QuartPelME_int_int;
extern const int MACRO_IDX_QuarterPelSearch;

#define SIMD_SUB_QuartPelME_half_half Software_QPelME[SIMD_IDX_QuartPelME_half_half]
#define SIMD_SUB_QuartPelME_half_int Software_QPelME[SIMD_IDX_QuartPelME_half_int]
#define SIMD_SUB_QuartPelME_int_half Software_QPelME[SIMD_IDX_QuartPelME_int_half]
#define SIMD_SUB_QuartPelME_int_int Software_QPelME[SIMD_IDX_QuartPelME_int_int]
#define MACRO_QuarterPelSearch Software_QPelME[MACRO_IDX_QuarterPelSearch]

#endif // ArcSoftwareQPelME_MPA_H
