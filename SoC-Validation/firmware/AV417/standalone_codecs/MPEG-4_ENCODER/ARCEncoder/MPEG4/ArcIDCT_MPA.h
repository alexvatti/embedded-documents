
#ifndef ArcIDCT_MPA_H
#define ArcIDCT_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcIDCT_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Idct[8];
extern int Idct_Record(int sa, int SIMDnum = 0);
extern const int SIMD_IDX_IDCTBlock;
extern const int MACRO_IDX_IDCT;

#define SIMD_SUB_IDCTBlock Idct[SIMD_IDX_IDCTBlock]
#define MACRO_IDCT Idct[MACRO_IDX_IDCT]

#endif // ArcIDCT_MPA_H
