
#ifndef ArcSDMTables_MPA_H
#define ArcSDMTables_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\MPEG4\ArcSDMTables_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int SDM_Buffers[5];
extern int SDM_Buffers_Record(int sa, int SIMDnum = 0);
extern const int MACRO_IDX_SetCurrentMPC;
extern const int MACRO_IDX_InitCurrentMPC;
extern const int MACRO_IDX_SetQuantValue;
extern const int MACRO_IDX_RevertQuantValue;
extern const int MACRO_IDX_CopyToFixedAfterReconstructMP4;

#define MACRO_SetCurrentMPC SDM_Buffers[MACRO_IDX_SetCurrentMPC]
#define MACRO_InitCurrentMPC SDM_Buffers[MACRO_IDX_InitCurrentMPC]
#define MACRO_SetQuantValue SDM_Buffers[MACRO_IDX_SetQuantValue]
#define MACRO_RevertQuantValue SDM_Buffers[MACRO_IDX_RevertQuantValue]
#define MACRO_CopyToFixedAfterReconstructMP4 SDM_Buffers[MACRO_IDX_CopyToFixedAfterReconstructMP4]

#endif // ArcSDMTables_MPA_H
