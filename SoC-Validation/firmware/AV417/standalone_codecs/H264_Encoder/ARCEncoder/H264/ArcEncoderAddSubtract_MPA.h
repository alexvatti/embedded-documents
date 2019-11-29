
#ifndef ArcEncoderAddSubtract_MPA_H
#define ArcEncoderAddSubtract_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcEncoderAddSubtract_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int EncoderAddSubtract[3];
extern int EncoderAddSubtract_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_ReconstructLuma16X16;
extern const int MACRO_IDX_ReconstructChroma;
extern const int MACRO_IDX_ReconstructSkipped;

#define MACRO_ReconstructLuma16X16 EncoderAddSubtract[MACRO_IDX_ReconstructLuma16X16]
#define MACRO_ReconstructChroma EncoderAddSubtract[MACRO_IDX_ReconstructChroma]
#define MACRO_ReconstructSkipped EncoderAddSubtract[MACRO_IDX_ReconstructSkipped]

#endif // ArcEncoderAddSubtract_MPA_H
