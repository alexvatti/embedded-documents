
#ifndef ArcMP4ChromaFilter_MPA_H
#define ArcMP4ChromaFilter_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcMP4ChromaFilter_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int FilterChroma[1];
extern int FilterChroma_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_FilterChromaPred;

#define MACRO_FilterChromaPred FilterChroma[MACRO_IDX_FilterChromaPred]

#endif // ArcMP4ChromaFilter_MPA_H
