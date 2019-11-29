
#ifndef ArcEncoderMain_MPA_H
#define ArcEncoderMain_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcEncoderMain_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int EncoderMain[5];
extern int EncoderMain_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_CopyCurrentChromaToSDM;
extern const int MACRO_IDX_CopyChromaPredToSDM;
extern const int MACRO_IDX_CopyMERefToSDM;
extern const int MACRO_IDX_CopyMBLumaToSDM;

#define MACRO_CopyCurrentChromaToSDM EncoderMain[MACRO_IDX_CopyCurrentChromaToSDM]
#define MACRO_CopyChromaPredToSDM EncoderMain[MACRO_IDX_CopyChromaPredToSDM]
#define MACRO_CopyMERefToSDM EncoderMain[MACRO_IDX_CopyMERefToSDM]
#define MACRO_CopyMBLumaToSDM EncoderMain[MACRO_IDX_CopyMBLumaToSDM]

#endif // ArcEncoderMain_MPA_H
