
#ifndef ArcChannel_MPA_H
#define ArcChannel_MPA_H

//ScanMp
//Scanning file: '.\ARCEncoder\ARC\ArcChannel_MPA.sss'

// MetaWare ouput ON
// Macro table
extern int ArcChannel[3];
extern int ArcChannel_Record(int sa, int SIMDnum = 0);
// pubreg intraValidFlag
#define REG_Generate_ME_SDMResponse_intraValidFlag	_vr00
#define MSK_Generate_ME_SDMResponse_intraValidFlag	1
#define SET_Generate_ME_SDMResponse_intraValidFlag(a)	_vr00 = _vmovw(a, 1)

// pubreg skipMv
#define REG_Generate_ME_SDMResponse_skipMv	_vr00
#define MSK_Generate_ME_SDMResponse_skipMv	2
#define SET_Generate_ME_SDMResponse_skipMv(a)	_vr00 = _vmovw(a, 2)

// pubreg cost
#define REG_Generate_ME_SkipResponse_cost	_vr00
#define MSK_Generate_ME_SkipResponse_cost	1
#define SET_Generate_ME_SkipResponse_cost(a)	_vr00 = _vmovw(a, 1)

// pubreg blkPattern
#define REG_Generate_ME_SkipResponse_blkPattern	_vr00
#define MSK_Generate_ME_SkipResponse_blkPattern	2
#define SET_Generate_ME_SkipResponse_blkPattern(a)	_vr00 = _vmovw(a, 2)

// pubreg cost
#define REG_Generate_ME_InterResponse_cost	_vr00
#define MSK_Generate_ME_InterResponse_cost	1
#define SET_Generate_ME_InterResponse_cost(a)	_vr00 = _vmovw(a, 1)

// pubreg blkPattern
#define REG_Generate_ME_InterResponse_blkPattern	_vr00
#define MSK_Generate_ME_InterResponse_blkPattern	2
#define SET_Generate_ME_InterResponse_blkPattern(a)	_vr00 = _vmovw(a, 2)

extern const int MACRO_IDX_Generate_ME_SDMResponse;
extern const int MACRO_IDX_Generate_ME_SkipResponse;
extern const int MACRO_IDX_Generate_ME_InterResponse;

#define MACRO_Generate_ME_SDMResponse ArcChannel[MACRO_IDX_Generate_ME_SDMResponse]
#define MACRO_Generate_ME_SkipResponse ArcChannel[MACRO_IDX_Generate_ME_SkipResponse]
#define MACRO_Generate_ME_InterResponse ArcChannel[MACRO_IDX_Generate_ME_InterResponse]

#endif // ArcChannel_MPA_H
