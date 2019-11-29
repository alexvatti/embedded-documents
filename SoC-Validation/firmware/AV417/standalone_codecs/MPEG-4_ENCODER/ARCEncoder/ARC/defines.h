/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */

/* May contain code (c) British Telecommunications plc 2005        */

#ifndef DEFINES_H
#define DEFINES_H

//#define ENC_PRINTF(x) (printf x)
#define ENC_PRINTF(x)

#if (defined(_DEBUG) && !defined(HARDWARE_EE) && !defined(ARC_ISS_EE_MODEL)) 
#define MAKE_H264_SYMEJECT_TRACE_FILE 1
#endif

#if	defined MAKE_H264_SYMEJECT_TRACE_FILE
#define		WRITE_ENCODED_SYMBOL(Name, Value)	{ if (0 != m_TraceFile) {fprintf(m_TraceFile,"%s : %d\n",(Name),(Value)); fflush(m_TraceFile);}}
#define		WRITE_ENCODED_SYMBOL_FLOAT(Name, Value)	{ if (0 != m_TraceFile) {fprintf(m_TraceFile,"%s : %f\n",(Name),(Value)); fflush(m_TraceFile);}}
#else
#define		WRITE_ENCODED_SYMBOL(Name, Value)
#define		WRITE_ENCODED_SYMBOL_FLOAT(Name, Value)
#endif

#if	defined MAKE_H264_SYMEJECT_TRACE_FILE
#define		WRITE_ENCODED_PAIR(Name, Value1, Value2)	{ if (0 != m_TraceFile) fprintf(m_TraceFile,"%s : %d : %d\n",(Name),(Value1),(Value2)); }
#else
#define		WRITE_ENCODED_PAIR(Name, Value1, Value2)
#endif

#define STARTCODELEN 5  // '0001' + 1 extra
//this stuff relates to user data
#define SOFTWARE_MAJOR_VERSION	1 // 0-99
#define SOFTWARE_MINOR_VERSION  0 // 0-99 5k
#define MESOFTWARE_MAJOR_VERSION	1 // 0-99
#define MESOFTWARE_MINOR_VERSION  1 // 0-99 5k
#define HARDWARE_MAJOR_VERSION	1
#define HARDWARE_MINOR_VERSION  0 //hardconfig to be defined
#define VERSION_DATE	"15th August 2007" // for "c" application wrapper
// end of user data


//#define STATISTICS // print out the stats to console and file - using doubles
//#define TIMER_OUTPUT // print out details from the Channel Timer

#define NOMINAL_FREQ 200 // MegaHerz

#define MIN_SKIP_8X8		36
#define MAX_SKIP_8X8		64
#define MIN_SKIP_16X16		24
#define MAX_SKIP_16X16		40

#define MIN_SKIP_8X8_MP4	0
#define MAX_SKIP_8X8_MP4	32
#define MIN_SKIP_16X16_MP4	20
#define MAX_SKIP_16X16_MP4	32
// EE Auxiliary register IDs
enum EEAuxRegIdx
{
   EE_VCONFIG,                   // R	VRaptor Configuration Register
   EE_NAMESTR0,                  //	R	Name String Register
   EE_BUILD,                     //	R	EE Build Configuration Register.
   CODEC_CFG,                    // R/W	Codec Selection and Configuration
   UNIT_SEL,                     // R/W	Unit Selection. Selects which packer or 
                                 // concatenation unit to write to.
   SLICE_TYPE,                   // R/W	Slice Type. Only for H.264
   ADD_UEV,                      // W	Add Unsigned Exp-Golomb value
   ADD_SEV,                      // W	Add Signed Exp-Golomb value
   ADD_MEV_INTRA,                // W	Add Mapped Exp-Golomb value using Intra 4x4/8x8 table
   ADD_MEV_INTER,                // W	Add Mapped Exp-Golomb value using Inter table
   BUFFER_CTRL,                  // R/W	Buffer Control Register
   PACK_TO_BYTE,                 // W	Pack to Byte Aligned Boundary
   CONCAT_SEL,                   // R/W	Concatenation unit input packer unit selection
   CONCAT_DI_ADR,                // R/W	Concatenation unit input DMA address.
   P0_BUFF,                      // R/W   p0_BUF,p1_BUF .... Also includes the concat buff
                                 // R/W	Concatenation unit packing register 
                                 // buffer content.
   P1_BUFF,
   P2_BUFF,
   P3_BUFF,
   CONCAT_BUF,
   EMU_BUFF,	                  // R/W	Emulation Buffer Content
   P0_CUR_ADDR,                  // R/W   4 Packer current addresses, includes concat address
                                 // R/W	Concatenation unit Current DMA out address
   P1_CUR_ADDR,
   P2_CUR_ADDR,
   P3_CUR_ADDR,
   CONCAT_CUR_ADDR,
   P0_STATUS,                    // R/W	Packer Valid and Flush status includes CN status
   P1_STATUS,
   P2_STATUS,
   P3_STATUS,                    // R/W	Concatenation unit Valid and Flush status
   CONCAT_STAUS,
   DELTA_QUANT,                  // R/W	Delta Quantizer Value to encode.
   EMU_CTRL,                     // R/W	Emulation Byte Prevention Logic Control.
   CURR_MPC_PTR,                 // R/W	Current MPC Pointer in SDM
   TOP_MPC_PTR,                  // R/W	Top (B) Neighbor MPC Pointer in SDM
   LEFT_MPC_PTR,                 // R/W	Left (A) Neighbor MPC Pointer in SDM
   MPC_Y_OFFSET,                 // R/W	Address Offset of Luma Transformed Coefficient in MPC
   STATUS_CTRL,                  // R/W	EE Status and Control
   MB_BIT_CNT,                   // R/W	Macroblock Bit Count.
   SKIP_CNT,                     // R/W	Skip MB count.
   CONCAT_BYTE_CNT,              // R/W	Concatenation unit Byte Count.
   EMU_BYTE_CNT,                 // R/W	Emulation Byte Count
   CH_MBE_ADR,                   // R/W	Macroblock Encoded Channel Branch Address
   CH_FL_ADR,                    // R/W	Packer or Concatenator Flushed Channel Branch Address
   CH_CALL_ADR,                  // R/W	Call Back Channel Branch Address

   ADD_SKIPRUN,              	   // R/W	Add Skip Run Value
   CH_CALL,	                     // R/W	Channel Call Back Register
   RESTORE,	                     // R/W	Restore saved context
   BUF_STORE,	                  // R/W	Context Saved Buffer Content
   CURR_ADDR_STORE,	            // R/W	Context Saved Current DMA Out Address
   STATUS_STORE,	               // R/W	Context Saved Buffer Status 
   EMU_BUFF_STORE,	            // R/W	Context Saved Emulation Buffer Content
   MB_BIT_CNT_STORE,             // R/W	Context Saved Macroblock Bit Count.
   SKIP_CNT_STORE,               //	R/W	Context Saved Skip MB count.
   CN_BYTE_CNT_STORE,            //	R/W	Context Saved Concatenation Byte Count.
   EMU_BYTE_CNT_STORE,           //	R/W	Context Saved Emulation Byte Count.
   ADD_BITS,                     // For all the add bit registers, 1 to 31
   EE_DEBUG = 87
};

#define EE_DEBUG_LOC 56
#define ADD_BIT_OFF 54
enum EEAddBit
{
   ADD_1BIT = 55,
   ADD_2BIT,ADD_3BIT,ADD_4BIT,ADD_5BIT,ADD_6BIT,ADD_7BIT,ADD_8BIT,ADD_9BIT,ADD_10BIT,
   ADD_11BIT,ADD_12BIT,ADD_13BIT,ADD_14BIT,ADD_15BIT,ADD_16BIT,ADD_17BIT,ADD_18BIT,ADD_19BIT,
   ADD_20BIT,ADD_21BIT,ADD_22BIT,ADD_23BIT,ADD_24BIT,ADD_25BIT,ADD_26BIT,ADD_27BIT,ADD_28BIT,
   ADD_29BIT,ADD_30BIT,ADD_31BIT,ADD_32BIT
};

enum EEChInRegs
{
   R0,
   R1,
   R2,
   R3,
   R4,
   R5,
   R6,
   R7,
   R8,
   R9,
   R10,
   R11,
   R12,
   R13,
   R14,
   R15,
   R16,
   R17,
   R18,
   R19,
   R20,
   R21,
   R22,
   R23,
   R24,
   R25,
   R26,
   R27,
   R31 = 31
};

#define CH_ADD_BIT_OFFSET 30

enum EEChAddBits
{
  CH_ADD_1BIT = 31,
  CH_ADD_2BIT,CH_ADD_3BIT,CH_ADD_4BIT,CH_ADD_5BIT,CH_ADD_6BIT,CH_ADD_7BIT,
  CH_ADD_8BIT,CH_ADD_9BIT,CH_ADD_10BIT,CH_ADD_11BIT,CH_ADD_12BIT,CH_ADD_13BIT,
  CH_ADD_14BIT,CH_ADD_15BIT,CH_ADD_16BIT,CH_ADD_17BIT,CH_ADD_18BIT,CH_ADD_19BIT,
  CH_ADD_20BIT,CH_ADD_21BIT,CH_ADD_22BIT,CH_ADD_23BIT,CH_ADD_24BIT,CH_ADD_25BIT,
  CH_ADD_26BIT,CH_ADD_27BIT,CH_ADD_28BIT,
  CH_ADD_29BIT,CH_ADD_30BIT,CH_ADD_31BIT,CH_ADD_32BIT
};

#ifndef HARDWARE_EE
	#define ARC_EE_ENCODER
#endif

// channel specific stuff
#define CHANNEL_CMD_MASK 0xFF

// IDs of registers which the EE module will
// be used while sending response
enum RegIds
{
   Reg_R0,
   Reg_R1,
   Reg_R2,
   Reg_R63 = 63
};

struct RegData
{
   bool valid;
   unsigned int RegID;
   unsigned int RegValue;
};

// Outward channel response 
struct Response
{
    // Delay cycles used to delay response, It will be set according to the type of
    // response
    // Once all delay count is zero then only the actual command will
    // executed and response generated.
    // Needed for debug halting
    unsigned int DelayCycles;
    // At any given time EE can modify upto 4 registers, ME can modify 3, but SIMD
    // can modify 5
    #define MAX_NUM_REGS_TO_RETURN 5
    struct RegData RegisterData[MAX_NUM_REGS_TO_RETURN];
};

#define SDM_BASE_ADDRESS 0x00f00000

//#define DUMP_DEBUG
// dump debugging info to a file
#ifdef DUMP_DEBUG // Dump = General information and control

#define DUMP_DEBUG0 // Dump 0 = Prediction and Residual
#define DUMP_DEBUG1 // Dump 1 = Luma Transform
#define DUMP_DEBUG2 // Dump 2 = Luma Quantise
#define DUMP_DEBUG3 // Dump 3 = Luma Small Coefficient Removal
#define DUMP_DEBUG4 // Dump 4 = Luma DC Transform
#define DUMP_DEBUG5 // Dump 5 = Luma DC Quantise
#define DUMP_DEBUG6 // Dump 6 = Chroma U Transform
#define DUMP_DEBUG7 // Dump 7 = Chroma U Quantise
#define DUMP_DEBUG8 // Dump 8 = Chroma V Transform
#define DUMP_DEBUG9 // Dump 9 = Chroma V Quantise
#define DUMP_DEBUG10 // Dump 10 = Chroma Small Coefficient Removal
#define DUMP_DEBUG11 // Dump 11 = Chroma U DC Transform
#define DUMP_DEBUG12 // Dump 12 = Chroma U DC Quantise
#define DUMP_DEBUG13 // Dump 13 = Chroma V DC Transform
#define DUMP_DEBUG14 // Dump 14 = Chroma V DC Quantise
#define DUMP_DEBUG15 // Dump 15 = Luma Inverse DC Transform
#define DUMP_DEBUG16 // Dump 16 = Luma Inverse DC Quantise & Inverse AC Quantise
#define DUMP_DEBUG17 // Dump 17 = Luma Inverse Transform
#define DUMP_DEBUG18 // Dump 18 = Chroma U Inverse DC Transform
#define DUMP_DEBUG19 // Dump 19 = Chroma V Inverse DC Transform
#define DUMP_DEBUG20 // Dump 20 = Chroma U Inverse DC Quantise (& Inverse AC Quantise)
#define DUMP_DEBUG21 // Dump 21 = Chroma U Inverse Transform
#define DUMP_DEBUG22 // Dump 22 = Chroma V Inverse DC Quantise & (Inverse AC Quantise)
#define DUMP_DEBUG23 // Dump 23 = Chroma V Inverse Transform
#define DUMP_DEBUG24 // Dump 24 = Reconstructed
#define DUMP_DEBUG25 // Dump 25 = Encoded Picture

#include <stdio.h>
extern FILE * dump_debug_fp;

#include "typdef.h"
extern void DumpDebug_String (const char * str);
extern void DumpDebug_Value (const char * str, u_int8 val);
extern void DumpDebug_Value (const char * str, u_int16 val);
extern void DumpDebug_Value (const char * str, unsigned int val);
extern void DumpDebug_Value (const char * str, int val);
extern void DumpDebug_Value (const char * str, double val);
extern void DumpDebug_Block (const char * str, u_int8 * block, int blockCount, int maxX, int maxY, int stride);
extern void DumpDebug_Block (const char * str, int16 * block, int blockCount, int maxX, int maxY, int stride);
#endif // DUMP_DEBUG


typedef void (*fn_response)(unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned int param5);


enum END_OF_MB_STATUS
{
    END_OF_PICT,
    END_OF_SLICE,
    IN_SLICE
};

extern const short TopCoeffCountSBBitPos[8];
extern const short CurrCoeffCountSBBitPos[8];


#ifndef ABS
#define ABS(x) (((x) < 0) ? (-(x)) : (x))
#endif

#ifndef	MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef	MIN
#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifdef __ARC_ 

#define ABS_TABLE(x) (((x) < 0) ? (-(x)) : (x))
#define SD_TABLE(x) ((x)*(x))
#define RECON_CLIP(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))
#define CLIP_CHROMA_QUANT(x) MAX(MIN((x),51),0)
#define CLIP_FOR_FILTER(x) MAX(MIN((x),36),0)

#else // __ARC_

inline unsigned int ABS_TABLE(int x)
{ 
    return ABS(x);
}

inline unsigned int SD_TABLE(int x)
{
    return x*x;
}

inline int RECON_CLIP(int x) 
{
    if(x < 0)
        return 0;
    if(x > 255)
        return 255;
    return x;
}

inline char CLIP_CHROMA_QUANT(char x)
{
    return MAX(MIN(x,51),0);
}

inline char CLIP_FOR_FILTER(char x)
{
    return MAX(MIN(x,36),0);
}

#endif // __ARC_

#ifndef ARC_ENCODER_API
/* flush data-cache is a privilaged instruction */
#ifdef HARDWARE_SIMD
	#define flush_dcache() _sr(0x1, 0x47)
#else
	#define flush_dcache()
#endif
#endif

#ifdef ENABLE_PERFORMANCE

//#define ENABLE_FRAME_PROFILING		// Used in case of each frame being read in memory
#define ENABLE_FILE_WRITE
#define PRINT_FRAME_DATA
// Global YUV input buffer, only used for performance evaluation
// This is where the debugger will load the input YUV data, bypassing the cache.
// In this buffer the data has to be loaded starting from 64bit aligned address
#define TIMER_TICK_MEM 0x19000000
#define YUV_BUFF_SIZE 52428800

#ifdef ENABLE_BVCI_COUNTERS
#define BVCI_COUNT_MEM (TIMER_TICK_MEM + 0x10000)
struct bvci_data {
  unsigned int  vacyccnt;
  unsigned int  idlecyccnt;
  unsigned int  pktcnt;
  unsigned int  sumlat;
  unsigned int  lat;
};
#endif
struct buff{
	//#ifndef ENABLE_FRAME_PROFILING
    	//unsigned char alignment1[32];        
    	//unsigned int InputBuffer[YUV_BUFF_SIZE/4];
    	//unsigned char alignment2[32]; 
    	//unsigned int TimerTks[10];
    	//unsigned long int FileSz;
   //#endif
	int	OutputPtr;
#ifdef ENABLE_FILE_WRITE
	unsigned char OutputStream[9000000];
#endif // ENABLE_FILE_WRITE
    unsigned int *TimerTicks;
#ifdef ENABLE_BVCI_COUNTERS
    struct bvci_data *BVCICounts;
#endif
    };
extern struct buff YUVBuffer;
_Asm Halt_Processor(void) 
{
    flag 1
    nop
    nop
    nop
    nop
}
#endif
#endif // DEFINES_H
