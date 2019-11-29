
#ifndef ArcSDMTables_MPA_H
#define ArcSDMTables_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\H264\ArcSDMTables_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int SDM_Buffers[8];
extern int SDM_Buffers_Record(int sa, int SIMDnum = 0);
// pubreg cIdx
#define REG_SetCurrentMPC0_cIdx	_vr00
#define MSK_SetCurrentMPC0_cIdx	1
#define SET_SetCurrentMPC0_cIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg Column
#define REG_SetCurrentMPC0_Column	_vr00
#define MSK_SetCurrentMPC0_Column	2
#define SET_SetCurrentMPC0_Column(a)	_vr00 = _vmovw(a, 2)

// pubreg leftIdx
#define REG_SetCurrentMPC0_leftIdx	_vr00
#define MSK_SetCurrentMPC0_leftIdx	4
#define SET_SetCurrentMPC0_leftIdx(a)	_vr00 = _vmovw(a, 4)

// pubreg From
#define REG_MemCopy64Byte_From	_vr00
#define MSK_MemCopy64Byte_From	1
#define SET_MemCopy64Byte_From(a)	_vr00 = _vmovw(a, 1)

// pubreg To
#define REG_MemCopy64Byte_To	_vr00
#define MSK_MemCopy64Byte_To	2
#define SET_MemCopy64Byte_To(a)	_vr00 = _vmovw(a, 2)

// pubreg Size
#define REG_MemCopy64Byte_Size	_vr00
#define MSK_MemCopy64Byte_Size	4
#define SET_MemCopy64Byte_Size(a)	_vr00 = _vmovw(a, 4)

// pubreg BufIdx
#define REG_CallBackStartME_BufIdx	_vr00
#define MSK_CallBackStartME_BufIdx	1
#define SET_CallBackStartME_BufIdx(a)	_vr00 = _vmovw(a, 1)

// pubreg BufIdx
#define REG_CallBackStartDeblock_BufIdx	_vr00
#define MSK_CallBackStartDeblock_BufIdx	1
#define SET_CallBackStartDeblock_BufIdx(a)	_vr00 = _vmovw(a, 1)

extern const int MACRO_IDX_SetCurrentMPC0;
extern const int MACRO_IDX_InitCurrentMPC;
extern const int MACRO_IDX_MemCopy64Byte;
extern const int MACRO_IDX_CallBackStartME;
extern const int MACRO_IDX_CallBackStartDeblock;

#define MACRO_SetCurrentMPC0 SDM_Buffers[MACRO_IDX_SetCurrentMPC0]
#define MACRO_InitCurrentMPC SDM_Buffers[MACRO_IDX_InitCurrentMPC]
#define MACRO_MemCopy64Byte SDM_Buffers[MACRO_IDX_MemCopy64Byte]
#define MACRO_CallBackStartME SDM_Buffers[MACRO_IDX_CallBackStartME]
#define MACRO_CallBackStartDeblock SDM_Buffers[MACRO_IDX_CallBackStartDeblock]

#endif // ArcSDMTables_MPA_H
