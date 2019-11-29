
#ifndef ArcFDCT_MPA_H
#define ArcFDCT_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcFDCT_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int Fdct[8];
extern int Fdct_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_FDCT;

#define MACRO_FDCT Fdct[MACRO_IDX_FDCT]

#endif // ArcFDCT_MPA_H
