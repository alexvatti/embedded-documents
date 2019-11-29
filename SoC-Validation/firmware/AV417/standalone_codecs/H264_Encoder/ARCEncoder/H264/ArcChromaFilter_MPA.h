
#ifndef ArcChromaFilter_MPA_H
#define ArcChromaFilter_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcChromaFilter_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int FilterChroma[2];
extern int FilterChroma_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_FilterChromaPred8;
extern const int MACRO_IDX_FilterChromaPred4;

#define MACRO_FilterChromaPred8 FilterChroma[MACRO_IDX_FilterChromaPred8]
#define MACRO_FilterChromaPred4 FilterChroma[MACRO_IDX_FilterChromaPred4]

#endif // ArcChromaFilter_MPA_H
