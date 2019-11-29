/* ------------------------------------------------------------------------ */
/* FILE:			CH264Encoder.cpp										*/
/* DESCRIPTION:		BT H.264 video encoder source file.						*/
/* VERSION:			v2.20													*/
/* DATE:			22 September 2005										*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */
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


/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include "CH264Encoder.h"
#include "H264EncodedPictureBuffer.h"
#include "SymbolByteBuffer.h"
#include "picture_encoder.h"
#include "H264ParameterSets.h"
#include "H264Tables.h"
#include <assert.h>
#include <memory.h>
#include "math.h"
#include "ArcFrameBuffers.h"
#include "VRaptorInfo.h"
#ifndef HARDWARE_ME
#include "ArcMotionEstimationModel.h"
#endif

#include "ArcSDMTables.h"
#include "H264LoopFilter.h"

#include "defines.h"
#ifndef HARDWARE_EE
   #include "ArcEntropyEncoderModel.h"
#endif



#include "ArcChannelRoutines.h"
#include "ArcMacroRecordSettings.h"


#ifdef _DEBUG
//#define MAKE_ENCODER_TRACE_FILE
#endif

#include "ArcSDMTables.h"

#include <arc/arc_avu.h>
#include "ArcSDMTables_MPA.h"
#include "ArcTimer.h"

extern void (*HandleServiceCall)();


#ifndef ARC_ENCODER_API
extern FILE *stats_file ;
#endif


// ==========================================================================
// One global instance (As only one HW block) ===============================
// ==========================================================================
CH264Encoder gCH264Encoder;
// ==========================================================================


#ifdef ENABLE_PERFORMANCE

static void Reset_Timer0(void);
//static void Timer0_Val_To_Mem(void);
//------------------------------------------------------------------------------

_Asm void DisableInterrupts( )
{
  FLAG 0     ; Disable Interrupt
}

_Asm void EnableInterrupts( )
{
  FLAG 6     ; Enable Interrupt
}

void Reset_Timer0(void)
{
	//DisableInterrupts();
  _sr(2,              0x22);       // CONTROL0 = 2 -- NH
 _sr(0xffffffff,     0x23);       // LIMIT0 = -1
  _sr(0,              0x21);       // COUNT0 = 0
}


void Timer0_Val_To_Mem(void)
{
       unsigned int val_t = _lr(0x21);
       *(YUVBuffer.TimerTicks) = val_t;
       YUVBuffer.TimerTicks++;
//#ifdef PRINT_FRAME_DATA
	   if(gCH264Encoder.m_DynamicControl)Service_DisableTimer();
	   printf("Ticks = %d, End_cycles %d, panic_lev = %d\n",val_t, gCH264Encoder.m_EndCycles, gCH264Encoder.m_PanicLevel);
	   if(gCH264Encoder.m_DynamicControl)Service_EnableTimer();
//#endif // PRINT_FRAME_DATA
}

#ifdef ENABLE_BVCI_COUNTERS
void Reset_BVCI_profiler(void)
{
  *(volatile unsigned int *)0xfed004 = 2;   // Clear the BVCI counts (but not the min/max latency)
}

void BVCI_profiler_val_to_mem(void)
{
  int i;
  *(volatile unsigned int *)0xfed004 = 0x1;  // Halt the counters
  
  // Loop over the data capture nodes and store the data in the buffer
  for(i=0;i<=7;i++) {
    *(volatile unsigned int *)0xfed014 = i;   // Point to node
    YUVBuffer.BVCICounts->vacyccnt    = *(volatile unsigned int *)0xfed018;  // valid/ack cycle count
    YUVBuffer.BVCICounts->idlecyccnt  = *(volatile unsigned int *)0xfed01c;  // idle cycle count
    YUVBuffer.BVCICounts->pktcnt      = *(volatile unsigned int *)0xfed020;  // packet count
    YUVBuffer.BVCICounts->sumlat      = *(volatile unsigned int *)0xfed024;  // sum of latencies
    YUVBuffer.BVCICounts->lat         = *(volatile unsigned int *)0xfed028;  // min/max latencies
    YUVBuffer.BVCICounts++;
  }
}
#endif


#endif

#if	defined MAKE_ENCODER_TRACE_FILE
#define		WRITE_STRING(Name)	{ if (0 != m_GeneralEncoderTraceFile) fprintf(m_GeneralEncoderTraceFile,"%s\n",(Name)); }
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)	{ if (0 != m_EncoderTraceFile) fprintf(m_GeneralEncoderTraceFile"%s %i\n",(Name), (Number)); }
#else
#define		WRITE_STRING(Name)
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)
#endif
extern  int h263_equivalent_lut[52];
extern  int h264_equivalent_lut[32];
// temp lut for testing purposes
const char init_panics[12][11] =
{
//  subpel, step1, skipthresh, use16only, skip8x8, 16x8etc, NoIntra, DConly, Allintra, whiten, paniclevel
	
0,			2,			40,			1,		64,			0,		1,		1,			0,		0,		82,	// level 0
1,			2,			40,			1,		64,			0,		1,		1,			0,		0,		78,	// level 1
2,			2,			40,			1,		64,			0,		1,		1,			0,		0,		74,	// level 2
2,			0,			40,			1,		64,			0,		1,		1,			0,		0,		65,	// level 3
2,			0,			24,			1,		64,			0,		1,		1,			0,		0,		60,	// level 4
2,			0,			24,			1,		64,			0,		0,		1,			0,		0,		57,	// level 5
2,			0,			24,			0,		64,			0,		0,		1,			0,		0,		41,	// level 6
2,			0,			24,			0,		36,			0,		0,		1,			0,		0,		12,	// level 7
2,			0,			24,			0,		36,			1,		0,		1,			0,		0,		8,	// level 8
2,			0,			24,			0,		36,			1,		0,		0,			1,		0,		8,	// level 9
2,			0,			24,			0,		36,			1,		0,		0,			1,		1,		4,	// this used for default when < SD
2,			0,			24,			0,	    36,			0,	    0,		0,		    1,		1,		0	// this used for default when SD
};
// end



#define NUM_H264_PANIC_SETTINGS (86-4)
const char dynamic_control_lut[NUM_H264_PANIC_SETTINGS][10] =

//skip8x8	16x8	!8x8	skipSAD	step1	subpel	whiten	dconly	vertonly noIntraInP
{								
36,	1,	0,	24,	0,	2,	1,	0,	0,	0,
36,	1,	0,	24,	0,	2,	1,	0,	0,	0,
36,	1,	0,	24,	0,	2,	1,	0,	0,	0,
36,	1,	0,	24,	0,	2,	1,	0,	0,	0,
36,	1,	0,	24,	0,	2,	1,	1,	0,	0,
36,	1,	0,	24,	0,	2,	1,	1,	0,	0,
36,	1,	0,	24,	0,	2,	1,	1,	0,	0,
36,	1,	0,	24,	0,	2,	1,	1,	0,	0,
36,	1,	0,	24,	0,	2,	0,	0,	0,	0,
36,	1,	0,	24,	0,	2,	0,	0,	0,	0,
36,	1,	0,	24,	0,	2,	0,	0,	0,	0,
36,	1,	0,	24,	0,	2,	0,	0,	0,	0,
36,	0,	0,	24,	0,	2,	0,	0,	0,	0,
37,	0,	0,	24,	0,	2,	0,	0,	0,	0,
38,	0,	0,	24,	0,	2,	0,	0,	0,	0,
39,	0,	0,	24,	0,	2,	0,	0,	0,	0,
40,	0,	0,	24,	0,	2,	0,	0,	0,	0,
41,	0,	0,	24,	0,	2,	0,	0,	0,	0,
42,	0,	0,	24,	0,	2,	0,	0,	0,	0,
43,	0,	0,	24,	0,	2,	0,	0,	0,	0,
44,	0,	0,	24,	0,	2,	0,	0,	0,	0,
45,	0,	0,	24,	0,	2,	0,	0,	0,	0,
46,	0,	0,	24,	0,	2,	0,	0,	0,	0,
47,	0,	0,	24,	0,	2,	0,	0,	0,	0,
48,	0,	0,	24,	0,	2,	0,	0,	0,	0,
49,	0,	0,	24,	0,	2,	0,	0,	0,	0,
50,	0,	0,	24,	0,	2,	0,	0,	0,	0,
51,	0,	0,	24,	0,	2,	0,	0,	0,	0,
52,	0,	0,	24,	0,	2,	0,	0,	0,	0,
53,	0,	0,	24,	0,	2,	0,	0,	0,	0,
54,	0,	0,	24,	0,	2,	0,	0,	0,	0,
55,	0,	0,	24,	0,	2,	0,	0,	0,	0,
56,	0,	0,	24,	0,	2,	0,	0,	0,	0,
57,	0,	0,	24,	0,	2,	0,	0,	0,	0,
58,	0,	0,	24,	0,	2,	0,	0,	0,	0,
59,	0,	0,	24,	0,	2,	0,	0,	0,	0,
60,	0,	0,	24,	0,	2,	0,	0,	0,	0,
61,	0,	0,	24,	0,	2,	0,	0,	0,	0,
62,	0,	0,	24,	0,	2,	0,	0,	0,	0,
63,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	0,	24,	0,	2,	0,	0,	0,	0,
64,	0,	1,	24,	0,	2,	0,	1,	0,	0,
64,	0,	1,	25,	0,	2,	0,	1,	0,	0,
64,	0,	1,	26,	0,	2,	0,	1,	0,	0,
64,	0,	1,	27,	0,	2,	0,	1,	0,	0,
64,	0,	1,	28,	0,	2,	0,	1,	0,	0,
64,	0,	1,	29,	0,	2,	0,	1,	0,	0,
64,	0,	1,	30,	0,	2,	0,	1,	0,	0,
64,	0,	1,	31,	0,	2,	0,	1,	0,	0,
64,	0,	1,	32,	0,	2,	0,	1,	0,	0,
64,	0,	1,	33,	0,	2,	0,	1,	0,	0,
64,	0,	1,	34,	0,	2,	0,	1,	0,	0,
64,	0,	1,	35,	0,	2,	0,	1,	0,	0,
64,	0,	0,	24,	0,	2,	0,	1,	0,	0,
64,	0,	0,	24,	0,	2,	0,	1,	0,	0,
64,	0,	0,	24,	0,	2,	0,	1,	0,	0,
64,	0,	0,	24,	0,	2,	0,	1,	0,	0,
64,	0,	1,	36,	0,	2,	0,	1,	0,	0,
64,	0,	1,	37,	0,	2,	0,	1,	0,	0,
64,	0,	1,	38,	0,	2,	0,	1,	0,	0,
64,	0,	1,	39,	0,	2,	0,	1,	0,	0,
64,	0,	1,	40,	0,	2,	0,	1,	0,	0,
64,	0,	1,	40,	1,	2,	0,	1,	0,	0,
64,	0,	1,	40,	1,	2,	0,	1,	0,	0,
64,	0,	1,	40,	1,	2,	0,	1,	0,	0,
64,	0,	1,	40,	1,	2,	0,	1,	0,	0,
64,	0,	1,	40,	2,	2,	0,	1,	0,	0,
64,	0,	1,	40,	2,	2,	0,	1,	0,	0,
64,	0,	1,	40,	2,	2,	0,	1,	0,	0,
64,	0,	1,	40,	2,	2,	0,	1,	0,	0,
64,	0,	1,	40,	2,	1,	0,	1,	0,	0,
64,	0,	1,	40,	2,	1,	0,	1,	0,	0,
64,	0,	1,	40,	2,	1,	0,	1,	0,	0,
64,	0,	1,	40,	2,	1,	0,	1,	0,	0,
64,	0,	1,	40,	2,	0,	0,	1,	0,	0,
64,	0,	1,	40,	2,	0,	0,	1,	0,	0,
64,	0,	1,	40,	2,	0,	0,	1,	0,	0,
64,	0,	1,	40,	2,	0,	0,	1,	0,	0/*,
64,	0,	1,	40,	2,	0,	0,	1,	0,	1,
64,	0,	1,	40,	2,	0,	0,	1,	0,	1,
64,	0,	1,	40,	2,	0,	0,	1,	0,	1,
64,	0,	1,	40,	2,	0,	0,	1,	0,	1,
*/



};

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::CH264Encoder									*/
/* DESCRIPTION:	H.264 encoder constructor.									*/
/* ------------------------------------------------------------------------ */

CH264Encoder::CH264Encoder(void)
{
	m_Created = false;
	m_IncompatibleSettings = false;
	m_NumberOfMBs = 0;
	m_ThreadInit = false;
	m_SourcePicture = 0;
	m_MoreToEncode = false;
	m_PictureBitCount = 0;
	m_MaximumNumberOfBytes = 0;

	m_CodeAllPictures = false;


	m_SourcePictureStoreCodingOrder = 0;

	m_ParameterSets = 0;
	m_EncoderControl = 0;
	m_EncodedPictureBuffer = 0;
	m_TraceFile = 0;
	m_RateConTraceFile = 0;
	m_GeneralEncoderTraceFile = 0;

	m_CurrentCopySliceGroup = 0;

	m_OverallBufferStatus = 0;

	//m_PreviousXAverage = 0;
	//m_PreviousYAverage = 0;


	int j;
	for (j=0; j<MAXIMUM_NUMBER_OF_SLICE_GROUPS ; j++)
	{
		m_SymbolByteBufferList[j] = 0;
		m_CurrentSymbolByteBuffer[j] = 0;
	}

	m_ThreadBufferStatus = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::~CH264Encoder									*/
/* DESCRIPTION:	H.264 encoder destructor.									*/
/* ------------------------------------------------------------------------ */

CH264Encoder::~CH264Encoder(void)
{
	Destroy();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::Create										*/
/* DESCRIPTION:	Creates the H.264 encoder. Allocates memory and initialises
				variables. ChangeSettings must be called before encoding
				can start.													*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::Create(void)
{
	if (m_Created) return false;

	ClearReturnParameters();
	if (!Reset())
	{
		assert(false);
		return false;
	}

	// Current assumption is that parameter sets are transmitted once at the beginning of a stream
	// More options will need to be supported in due course
	m_RetransmitParameterSets = false;
	m_TransmitSequenceParameterSet = true;
	m_NextPictureParameterSetToTransmit = 0;
	m_CodeNextPictureAsIDR = true;
	m_InputPictureNumber = 0;
	m_MaximumPacketDataSize = 0;

	m_ParameterSets = new CH264ParameterSets();
	if (0 == m_ParameterSets)
	{
		Destroy();
		assert(false);
		return false;
	}

	m_EncoderControl = new CH264EncoderControl();
	if (0 == m_EncoderControl)
	{
		Destroy();
		assert(false);
		return false;
	}

	#if	defined(MAKE_H264_SYMEJECT_TRACE_FILE)
	  m_TraceFile = fopen("H264EncodedTraceFile.txt","w");
      gArcEntropyEncoderModel.SetTraceFile(m_TraceFile);
   #endif

	RateControlInitialisation();

	m_IDRPictureID = 0;

	m_Created = true;

#if	defined MAKE_ENCODER_TRACE_FILE
	char file_name[30];
	sprintf(file_name, "H264GeneralEncoderTraceFile.txt");
	m_GeneralEncoderTraceFile = fopen(file_name,"w");
#endif

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ChangeSettings								*/
/* DESCRIPTION:	This is called to change the encoding parameters.
				Must be called before encoding can begin.
				This can only be called before or between encoding pictures:
				it should not be called in the middle of encoding a picture.
				Determines how many parameters have changed and makes the
				minimal changes - only codes an IDR if it has to.			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::ChangeSettings(h264_encoder_setup * setup_parameters)
{
	if (!m_Created)
	{
		Create();
	}

	m_IncompatibleSettings = false;

	///////////////////////////////////////////////////////////////////////////////
	//
	//  General Settings
	//
	///////////////////////////////////////////////////////////////////////////////


	int j;

	int frame_width_in_mbs = (setup_parameters->frame_width + 15) >> 4;
	int frame_height_in_mbs = (setup_parameters->frame_height + 15) >> 4;

	m_NumberOfMBs = frame_width_in_mbs * frame_height_in_mbs;

	m_IntraMBRefreshRate = setup_parameters->intra_refresh_rate;

	if(setup_parameters->intra_refresh_rate != 0) {
		m_IntraMBRefreshRate = m_NumberOfMBs/setup_parameters->intra_refresh_rate;
	// check that coprime with number of MBs in frame, otherwise, we will refesh the same blocks each time
	// we are assuming that we are always encoding with qcif, cif, or 4cif frames or some derived size

		while(((m_IntraMBRefreshRate/11)*11 == m_IntraMBRefreshRate) ||  //columns are always divisible by 11
			((m_IntraMBRefreshRate/3)*3 == m_IntraMBRefreshRate) ||		 // rows are always divisible by 3
			((m_IntraMBRefreshRate/2)*2 == m_IntraMBRefreshRate))		 // cif and above are always divisible by 2
		{
			m_IntraMBRefreshRate--;
		}
	}




	m_IntraUpdateMBNumber = 0;
	m_IntraUpdateSliceGroupID = 0;

	m_SkipSadThresholdMult =  MIN_SKIP_16X16;
	m_Skip8x8ThresholdMult = MIN_SKIP_8X8;
	m_StepOneControl = 0; // do full first step
	m_SubPelControl = 2; // do full sub-pel precision

	m_SmallPictures = m_UseAllInterModes = setup_parameters->frame_height*setup_parameters->frame_width < 352*288*2 ? 1 : 0;
	m_UseInter16x16Only = 0; // default, will get set in catchup operations
	m_UseIntraVertPredOnly = 0;
	m_UseIntraDCpredOnly = 0; // ditto
	m_NoIntraInPSlice = 0;
	m_PanicLevel = 0;

	m_UseWhitening = 1;
	m_RealTimeRateControl = setup_parameters->real_time_rate_control;

	m_NumberOfSliceGroupsForPPictures = 1;//setup_parameters->number_of_slice_groups_for_P_pictures;

	if (m_NumberOfSliceGroupsForPPictures < 1)
	{
		m_NumberOfSliceGroupsForPPictures = 1;
	}
	else if (m_NumberOfSliceGroupsForPPictures > 4)
	{
		m_NumberOfSliceGroupsForPPictures = 4;
	}

	if (1 == m_NumberOfSliceGroupsForPPictures)
	{
		m_NumberOfPictureParameterSets = 1;
	}
	else
	{
		// One PPS for the I and B pictures, and another for the P and SP pictures.
		m_NumberOfPictureParameterSets = 2;
	}




	///////////////////////////////////////////////////////////////////////////////
	//
	//  Create/Resize Symbol Buffers
	//
	///////////////////////////////////////////////////////////////////////////////

	// At the moment only creating one SymbolByteBuffer.  This will change!

	if ((m_MaximumPacketDataSize != (setup_parameters->maximum_packet_size - 12)) || (!m_ThreadInit))
	{
		// Note this will be true the first time as m_MaximumCodedSliceSize is set up to be zero

        // Allow for RTP header but no CSRC (12 bytes) and assume no aggregate packets
		m_MaximumPacketDataSize = setup_parameters->maximum_packet_size - 12;

		//for (j=0; j<MAXIMUM_NUMBER_OF_SLICE_GROUPS ; j++)
		for(j = 0; j < m_NumberOfSliceGroupsForPPictures; j++)
		{
			CSymbolByteBuffer * next = m_SymbolByteBufferList[j];
			m_SymbolByteBufferList[j] = 0;
			while (0 != next)
			{
				CSymbolByteBuffer * current = next;
				next = current->m_Next;
				delete current;
			}
		}

		for (j=0; j<MAXIMUM_NUMBER_OF_SLICE_GROUPS ; j++)
		{
			m_SymbolByteBufferList[j] = new CSymbolByteBuffer;
			if (0 == m_SymbolByteBufferList[j])
			{
				Destroy();
				assert(false);
				return false;
			}

			if (!m_SymbolByteBufferList[j]->Create(m_MaximumPacketDataSize+MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK))
			{
				Destroy();
				assert(false);
				return false;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	//  Create/Resize B Picture Buffers
	//
	///////////////////////////////////////////////////////////////////////////////


	if(m_NumberOfMBs)m_NumMBsRecip = (1<<20)/m_NumberOfMBs;
	int number_of_mbs_per_second = (90000 * m_NumberOfMBs) / setup_parameters->target_picture_period;
	m_CodeAllPictures = setup_parameters->code_all_pictures;
	m_FixedQuantiser = setup_parameters->fixed_quantiser;

	m_MBAdaptFreq = frame_width_in_mbs;
	if(m_MBAdaptFreq == 0)m_MBAdaptFreq = 9000; // i.e. not used

	m_DynamicControl = setup_parameters->dynamic_control ;

	// check to make sure that the required hardware for dynamic encoding is
	// available if required
	if (m_DynamicControl == 1 && HwChTm == 0)
	{
        ENC_PRINTF(("CH264Encoder: Error - Dynamic control enabled and unable to find channel TIMER"));
        ENC_PRINTF(("CH264Encoder: Exiting due unsupported hardware configuration"));
        assert(false);
        return false;
	}
	
	int index = setup_parameters->quality_level;
	if(m_DynamicControl) // 1 or 2 
	{
		m_EndCycles = 0; // may need to guess this !
		m_CycleBudgetMB = setup_parameters->quality_level*1000000/number_of_mbs_per_second;
		if(setup_parameters->quality_level == 0)m_CycleBudgetMB = 200000000/number_of_mbs_per_second;	
		index = (setup_parameters->quality_level-200)/20 + 5;
		//to enable selection of initial settings
	}


	// this section is utilising quality_level for the purposes of testing various switches and their effects on timing

	if((index > 10) || (index < 0)){
		// compatibility with normal golden model
		if(m_SmallPictures) index = 10;
		else index = 11;
	}

	m_SubPelControl =			init_panics[index][0];
	m_StepOneControl =			init_panics[index][1];
	m_SkipSadThresholdMult =	init_panics[index][2];
	m_UseInter16x16Only =		init_panics[index][3];
	m_Skip8x8ThresholdMult =	init_panics[index][4];
	m_UseAllInterModes =		init_panics[index][5];
	m_NoIntraInPSlice =			init_panics[index][6]==1;		
	m_UseIntraDCpredOnly =		init_panics[index][7]==1;		
	m_UseIntraVertPredOnly= 0;
	if (HwMe == 1) {
		m_UseWhitening =			init_panics[index][9]==1;
	}
	else {
		m_UseWhitening = 0;
	}

	if(m_DynamicControl) 
	{
		m_PanicLevel = init_panics[index][10];
		if(m_SmallPictures)m_PanicLevel = m_PanicLevel>>2; // reduce panic level significantly
	}

	m_LastHitCount  = 3000;
	m_FixedQuantiserValue = setup_parameters->fixed_quantiser_value;
	m_FixedQuantiserValue = CLIP(6, 51, m_FixedQuantiserValue);
	if (m_FixedQuantiser)
	{
		for (int slice_group=0; slice_group<MAXIMUM_NUMBER_OF_SLICE_GROUPS; slice_group++)
		{
			m_LastTransmittedQuant[slice_group] = m_FixedQuantiserValue;
		}

		//m_PreviousFrameAverageQuantiserDouble = m_FixedQuantiserValue;
		m_PreviousFrameAverageQuantiser = m_FixedQuantiserValue;
	}

	m_TargetBitRate = setup_parameters->target_bitrate;
	m_TargetPicturePeriod = setup_parameters->target_picture_period;
	m_InputPicturePeriod = setup_parameters->input_picture_period;


	m_ReactionMult = setup_parameters->reaction_multiplier;


	if(m_SmallPictures)
	{
		int fps = 90000/m_TargetPicturePeriod;

		m_AverageBitsPerFrame = (((m_TargetBitRate >>8)* m_TargetPicturePeriod)/90000)<<8;
	}
	else
	{

		int fix;
		fix = (30*(m_ReactionMult));

		m_AverageBitsPerFrame = ((((m_TargetBitRate >>8)* m_TargetPicturePeriod)/90000)<<8)-fix;

	}
																		// initial value as target bits per picture. Otherwise
																			// non-real time bitrate is incorrect if source frame rate
																			// differs from target frame rateTRJ 29/9/2003
	m_AverageBitsPerMacroblock = ((m_AverageBitsPerFrame)*m_NumMBsRecip + (1<<19))>>20;//m_AverageBitsPerFrame / m_NumberOfMBs;

	m_InitialQuant = (int)((210.0/m_AverageBitsPerMacroblock) + 25.0);
	m_InitialQuant = MIN(m_InitialQuant, 36);
	m_InitialQuant += (m_NumberOfMBs < 396 ? -2 : 0);
	if(m_CodeAllPictures) // then not bothered about realtime issues
	{
		m_InitialQuant = m_InitialQuant > 30 ? 30 : m_InitialQuant;
	}

	m_lastquant = m_InitialQuant;
	m_TargetBitsForThisFrame = m_AverageBitsPerFrame;

	m_IntraPictureFrequency = setup_parameters->intra_picture_frequency;


	// Set up GOP parameters
	if (0 == m_IntraPictureFrequency)
	{
		// No regular intras
		m_NumberPPerGOP = 1;

		m_TotalNumberPerGOP = m_NumberPPerGOP;
	}
	else
	{
		// Regular intras
		m_NumberPPerGOP = m_IntraPictureFrequency - 1;

		m_TotalNumberPerGOP = 1 + m_NumberPPerGOP;
	}

	m_NumberPLeftInGOP = m_NumberPPerGOP;
	m_TotalNumberLeftInGOP = 0;



	m_AverageBitsPerGOP = m_AverageBitsPerFrame * m_TotalNumberPerGOP;

	// Initialise m_BitsLeftInGOP to a value that is reasonable for the buffer level at the end of ech GOP
	m_BitsLeftInGOP = 0;//m_AverageBitsPerFrame;

	// Try to set the initial quantiser to a sensible value
	// In the original rate control, this is stored in m_PreviousFrameAverageQuantiser and m_PreviousFrameAverageQuantiserDouble.
	// Continue to set these parameters at present.
	// These are also used to set the initial virtual buffer levels as these directly relate to initial quantisers
	// Setting the initial complexities is an outstanding issue requiring further study.

	// Using the following calculation (based on 6 measurements at different values of QP for
	// Mother and daughter, 100 frames, 16x16 full search, 1 ref pic)
	// Initial Q = (5.348 - Log10(bits per pic)) * 50/3
	// Add 1 to the initial quantiser to make some allowance for the fact that the first picture is intra

	// To scale the calculation for non-QCIF pictures we should recognise the fact that the above calculation is based on
	// measurements taken using QCIF pictures, and scale the number of bits per picture by the ratio
	// (size of QCIF picture)/(size of current picture) where size is measured in pixels.

	if (setup_parameters->fixed_quantiser)
	{

		m_PreviousFrameAverageQuantiser = setup_parameters->fixed_quantiser_value;
	}
	else
	{
		m_PreviousFrameAverageQuantiser = m_InitialQuant;

	}

	// Clip to max and min values of quantiser just in case
	m_PreviousFrameAverageQuantiser = MAX(MINIMUM_QUANTISER_INDEX, MIN(MAXIMUM_QUANTISER_INDEX, (m_PreviousFrameAverageQuantiser)));

int h263_equivalent_q = h263_equivalent_lut[m_PreviousFrameAverageQuantiser];//3 * pow(2.0, (m_PreviousFrameAverageQuantiserDouble-24.0) / 6.0);



	m_ReactionParameter = (m_ReactionMult * m_TargetBitRate)>>5;

	if(m_ReactionParameter)m_InvReactParX31 = ((31<<24)/m_ReactionParameter);
	else m_InvReactParX31 = 10;// no real reason for this value


	m_IVirtualBuffer = (h263_equivalent_q *  m_ReactionParameter) >>5; // jrmm 32 is pretty similar to 31, isn't it ?
	m_PVirtualBuffer = (h263_equivalent_q *  m_ReactionParameter) >>5;




	///////////////////////////////////////////////////////////////////////////////
	//
	//  Determine whether an IDR picture needs to be coded
	//
	//  Yes if:		next picture is to be coded as IDR anyway (true first time)
	//				picture width or height changed
	//				number of reference pictures has increased (no action if decreased)
	//				profile or level or compatibility flags changed
	//
	///////////////////////////////////////////////////////////////////////////////

	int number_of_reference_pictures = 1;

	// Calculate Annex A level

	int decoded_picture_buffer_size
		= 384 * frame_width_in_mbs * frame_height_in_mbs * number_of_reference_pictures;

	int video_bit_rate = setup_parameters->target_bitrate;

	int level_index = 0;

	while (true)
	{
		if ((number_of_mbs_per_second <= annex_a_level_table[level_index][1]) &&
			(m_NumberOfMBs <= annex_a_level_table[level_index][2]) &&
			(decoded_picture_buffer_size <= annex_a_level_table[level_index][3]) &&
			(video_bit_rate <= annex_a_level_table[level_index][4]))
		{
			break;
		}

		level_index++;

		if (level_index >= NUMBER_OF_ANNEX_A_LEVELS)
		{
			m_IncompatibleSettings = true;
			assert(false);
			return false;
		}
	}


	// Calculate profile compatibility flags
	bool baseline_compatibility_flag = true;

	// The following assumes a that >1 Picture parameter set indicates the use of FMO
	bool main_compatibility_flag = !(m_NumberOfPictureParameterSets > 1);

	bool extended_compatibility_flag = true;

	bool high_compatibility_flag = main_compatibility_flag;

	// Compare current and previous settings to see if an IDR is needed

	if ((m_CodeNextPictureAsIDR) ||  // This is true the first time this function is called
		(setup_parameters->frame_width != m_PreviousSetupParameters.frame_width) ||
		(setup_parameters->frame_height != m_PreviousSetupParameters.frame_height) ||
		(baseline_compatibility_flag != m_SequenceParameterSet->baseline_compatibility_flag) ||
		(main_compatibility_flag != m_SequenceParameterSet->main_compatibility_flag))
	{
		if (!IDRChangeSettings(setup_parameters, baseline_compatibility_flag,
								 main_compatibility_flag, extended_compatibility_flag, high_compatibility_flag, level_index))
		{
			m_IncompatibleSettings = true;
			assert(false);
			return false;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	//  Determine whether a picture parameter sets need to be recreated and transmitted
	//
	//  Yes if any of the following have changed:
	//				deblocking
	//				chroma qp offset
	//				constrained intra
	//				slice groups
	//
	///////////////////////////////////////////////////////////////////////////////

	if ((setup_parameters->disable_deblocking_filter_idc != m_PreviousSetupParameters.disable_deblocking_filter_idc) ||
		(setup_parameters->chroma_qp_index_offset != m_PreviousSetupParameters.chroma_qp_index_offset) ||
		(setup_parameters->constrained_intra_pred_flag != m_PreviousSetupParameters.constrained_intra_pred_flag) ||
		(setup_parameters->number_of_slice_groups_for_P_pictures != m_PreviousSetupParameters.number_of_slice_groups_for_P_pictures))
	{
		for (unsigned char picture_parameter_set_id = 0;
						   picture_parameter_set_id < m_NumberOfPictureParameterSets;
						   picture_parameter_set_id++)
		{
		   if (!SetUpPictureParameterSet(picture_parameter_set_id,
									number_of_reference_pictures,
									setup_parameters->disable_deblocking_filter_idc,
									0,
									0,
									setup_parameters->chroma_qp_index_offset,
									setup_parameters->constrained_intra_pred_flag,
									setup_parameters->number_of_slice_groups_for_P_pictures,
									false))
		   {
			   m_IncompatibleSettings = true;
			   assert(false);
			   return false;
		   }
		}

		// Ensure that the sequence parameter set and picture parameter sets are transmitted
		m_NextPictureParameterSetToTransmit = 0;
	}

	m_PreviousSetupParameters = *setup_parameters;
    m_ThreadInit = true;

	Reset();


	
	// now we've changed the settings we need to update the me 
    if (HwMe == 1)
    {
        if(m_UseAllInterModes) // allow Inter16x8 and Inter8x16 as well as usual Inter16x16 and Inter8x8
        {
            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((2 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
    												      ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
        else if(!m_UseInter16x16Only) // only allow Inter16x16 and Inter8x8
        {
            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
    											          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
        else  // only 16x16
        {
    
            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
    													  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
    
        WriteToChannelME_Hw(CHANREG_ME_CONTROL1, (u_int32) (((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT) |
                                                         ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT)));
    }
    else
    {
        if(m_UseAllInterModes) // allow Inter16x8 and Inter8x16 as well as usual Inter16x16 and Inter8x8
        {
            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((2 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
        else if(!m_UseInter16x16Only) // only allow Inter16x16 and Inter8x8
        {
            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
        else  // only 16x16
        {
    
            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
        }
    
        WriteToChannelME_Sw(CHANREG_ME_CONTROL1, (u_int32) (((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT) |
                                                         ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT)));
    }


	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::IDRChangeSettings								*/
/* DESCRIPTION:	This is called to change the encoding parameters when an
				IDR picture must be encoded to achieve the change.
				This resizes the reference picture buffer if necessary
				and creates new sequence amd picture parameter sets and
				ensures they are transmitted.								*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::IDRChangeSettings(h264_encoder_setup * setup_parameters,
									 bool baseline_compatibility_flag,
									 bool main_compatibility_flag,
									 bool extended_compatibility_flag,
									 bool high_compatibility_flag,
									 int level_index)
{
	unsigned int number_of_normal_frames;
	unsigned int number_of_reference_pictures;


	number_of_reference_pictures = 1;

	number_of_normal_frames = number_of_reference_pictures + 1;

	if (0 != m_EncodedPictureBuffer)
	{
		delete m_EncodedPictureBuffer;
		m_EncodedPictureBuffer = 0;
	}

	m_EncodedPictureBuffer = new CH264EncodedPictureBuffer();
	if (0 == m_EncodedPictureBuffer)
	{
		Destroy();
		assert(false);
		return false;
	}

	if (!m_EncodedPictureBuffer->Modify(number_of_normal_frames, number_of_reference_pictures,
										false, // exponential_sliding_window - not supported at present
										setup_parameters->frame_width,
										setup_parameters->frame_height))
	{
		Destroy();
		assert(false);
		return false;
	}

	m_SequenceParameterSetID = 0;

	SetUpSequenceParameterSet(setup_parameters,
							  m_SequenceParameterSetID,
							  baseline_compatibility_flag,
							  main_compatibility_flag,
							  extended_compatibility_flag,
							  high_compatibility_flag,
							  level_index,
							  number_of_reference_pictures);

	// Initialise the block offsets
	unsigned int hsize = ((setup_parameters->frame_width + 15) >> 4) << 4;
	unsigned int qhsize = hsize >> 2;

	int block_number;

	for (block_number=0 ; block_number<16; block_number++)
	{
		m_frame_y_4x4_block_offset[block_number]  = ((block_number & 3) << 2) + ( hsize * (block_number & 12));
		m_frame_uv_4x4_block_offset[block_number] = ((block_number & 3) << 1) + (qhsize * (block_number & 12));

	}

	for (unsigned char picture_parameter_set_id = 0;
					   picture_parameter_set_id < m_NumberOfPictureParameterSets;
					   picture_parameter_set_id++)
	{
	   SetUpPictureParameterSet(picture_parameter_set_id,
								number_of_reference_pictures,
								setup_parameters->disable_deblocking_filter_idc,
								0,
								0,
								setup_parameters->chroma_qp_index_offset,
								setup_parameters->constrained_intra_pred_flag,
								setup_parameters->number_of_slice_groups_for_P_pictures,
								false);
	}

	// Ensure that the sequence parameter set and picture parameter sets are transmitted
	m_TransmitSequenceParameterSet = true;
	m_NextPictureParameterSetToTransmit = 0;

	m_CodeNextPictureAsIDR = true;

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::SetUpSequenceParameterSet						*/
/* DESCRIPTION:	This is called to set up the given sequence parameter set.	*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::SetUpSequenceParameterSet(h264_encoder_setup * setup_parameters,
											 int sequence_parameter_set_id,
											 bool baseline_compatibility_flag,
											 bool main_compatibility_flag,
											 bool extended_compatibility_flag,
											 bool high_compatibility_flag,
											 int level_index,
											 int number_of_reference_pictures)
{
	m_SequenceParameterSet = m_ParameterSets->ReadSequenceParameterSet(sequence_parameter_set_id);
	if (0 == m_SequenceParameterSet)
	{
		m_SequenceParameterSet = m_ParameterSets->WriteSequenceParameterSet(sequence_parameter_set_id);
	}

	if (0 == m_SequenceParameterSet)
	{
		assert(false);
		return false;
	}

	m_SequenceParameterSet->level_index = level_index;
	m_SequenceParameterSet->level_idc = annex_a_level_table[level_index][0];

	m_SequenceParameterSet->maximum_frame_number =  m_EncodedPictureBuffer->GetMaximumFrameNumberMinus1() + 1;

	m_SequenceParameterSet->baseline_compatibility_flag = baseline_compatibility_flag;
	m_SequenceParameterSet->main_compatibility_flag = main_compatibility_flag;
	m_SequenceParameterSet->extended_compatibility_flag = extended_compatibility_flag;

	if (m_SequenceParameterSet->baseline_compatibility_flag)
	{
		m_SequenceParameterSet->profile_idc = H264_BASELINE_PROFILE;
	}
	else if (m_SequenceParameterSet->extended_compatibility_flag)
	{
		m_SequenceParameterSet->profile_idc = H264_EXTENDED_PROFILE;
	}
	else if (m_SequenceParameterSet->main_compatibility_flag)
	{
		m_SequenceParameterSet->profile_idc = H264_MAIN_PROFILE;
	}
	else if (high_compatibility_flag)
	{
		m_SequenceParameterSet->profile_idc = H264_HIGH_PROFILE;
	}
	else
	{
		assert(false);
	}

	int max_frame_number = (m_SequenceParameterSet->maximum_frame_number >> 4) - 1;
	m_SequenceParameterSet->log2_max_frame_num_minus4 = 0;
	while (max_frame_number > 0)
	{
		m_SequenceParameterSet->log2_max_frame_num_minus4++;
		max_frame_number >>= 1;
	}

	m_SequenceParameterSet->pic_order_cnt_type = 1;//setup_parameters->pic_order_cnt_type;
	assert((m_SequenceParameterSet->pic_order_cnt_type >= 0) && (m_SequenceParameterSet->pic_order_cnt_type <= 2));
	assert((2 != m_SequenceParameterSet->pic_order_cnt_type) || (0 == 0));

	// Which block of code below is run depends on the line above
	if (0 == m_SequenceParameterSet->pic_order_cnt_type)
	{
		// Need to set the number of bits used according to how many frames can be re-ordered
		int number_of_bits = 4;
		int max_value = 6 * (0+1);  // 2 per picture and three gops to be safe

		while (max_value > (1 << number_of_bits))
		{
			number_of_bits++;
		}

		m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4 = number_of_bits - 4;
		m_SequenceParameterSet->max_pic_order_cnt_lsb = 1 << number_of_bits;
		m_SequenceParameterSet->delta_pic_order_always_zero_flag = false;
		m_SequenceParameterSet->offset_for_non_ref_pic = 0;
		m_SequenceParameterSet->offset_for_top_to_bottom_field = 0;
		m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle = 0;

		m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle = 0;

		memset(m_SequenceParameterSet->offset_for_ref_frame, 0, MAXIMUM_REF_FRAMES_IN_PIC_CYCLE * sizeof(int));
	}
	else if (1 == m_SequenceParameterSet->pic_order_cnt_type)
	{
		{
			// No B pictures, or one between each reference. No slice header overhead
			m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4 = 0;
			m_SequenceParameterSet->max_pic_order_cnt_lsb = 16;
			m_SequenceParameterSet->delta_pic_order_always_zero_flag = (true);
			m_SequenceParameterSet->offset_for_non_ref_pic = -2;

			// When encoding in frame mode only (H264_FRAME_CODING), content is assumed to be progressive
			// and top and bottom field POC are set to be the same. In all other cases top and bottom
			// field POC are different by one, with the order being given by m_TopFieldFirst. When this is true,
			// the top field is assumed to be output first, and have a POC one less than the bottom field POC.
			{
				m_SequenceParameterSet->offset_for_top_to_bottom_field = 0;
			}

			m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle = 1;

			m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle = 2 * (0 + 1);

			memset(m_SequenceParameterSet->offset_for_ref_frame, 0, MAXIMUM_REF_FRAMES_IN_PIC_CYCLE * sizeof(int));

			m_SequenceParameterSet->offset_for_ref_frame[0] = 2 * (0 + 1);
		}

	}
	else // (2 == m_SequenceParameterSet->pic_order_cnt_type)
	{
		m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4 = 0;
		m_SequenceParameterSet->max_pic_order_cnt_lsb = 16;
		m_SequenceParameterSet->delta_pic_order_always_zero_flag = false;
		m_SequenceParameterSet->offset_for_non_ref_pic = 0;
		m_SequenceParameterSet->offset_for_top_to_bottom_field = 0;
		m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle = 0;

		m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle = 0;

		memset(m_SequenceParameterSet->offset_for_ref_frame, 0, MAXIMUM_REF_FRAMES_IN_PIC_CYCLE * sizeof(int));
	}

	m_SequenceParameterSet->num_ref_frames = number_of_reference_pictures;
	m_SequenceParameterSet->required_frame_num_update_behaviour_flag = false;

	m_SequenceParameterSet->frame_mbs_only_flag = (true);
	m_SequenceParameterSet->mb_adaptive_frame_field_flag = false;

	m_SequenceParameterSet->frame_width_in_mbs = (setup_parameters->frame_width + 15) >> 4;;
	m_SequenceParameterSet->frame_height_in_mbs = (setup_parameters->frame_height + 15) >> 4;
	MgrMPC.Init(m_SequenceParameterSet->frame_width_in_mbs, m_SequenceParameterSet->frame_height_in_mbs);

	if (0 == m_SequenceParameterSet->frame_mbs_only_flag)
	{
		m_SequenceParameterSet->picture_height_in_map_units = m_SequenceParameterSet->frame_height_in_mbs >> 1;
	}
	else
	{
		m_SequenceParameterSet->picture_height_in_map_units = m_SequenceParameterSet->frame_height_in_mbs;
	}

	m_SequenceParameterSet->direct_8x8_inference_flag = true;

	if ((0 != (setup_parameters->frame_width & 15)) || (0 != (setup_parameters->frame_height & 15)))
	{
		// Assume in this case that the active picture area will be put in the top left of the coded picture
		// NOTE: CopyPictureToControl does not handle this case
		assert(false);  // This functionality is not fully developed and tested.
		m_SequenceParameterSet->frame_cropping_flag = true;
		m_SequenceParameterSet->frame_cropping_rect_left_offset = 0;
		m_SequenceParameterSet->frame_cropping_rect_right_offset = 16 - (setup_parameters->frame_width & 15);
		m_SequenceParameterSet->frame_cropping_rect_top_offset = 0;
		m_SequenceParameterSet->frame_cropping_rect_bottom_offset = 16 - (setup_parameters->frame_height & 15);
	}
	else
	{
		m_SequenceParameterSet->frame_cropping_flag = false;
		m_SequenceParameterSet->frame_cropping_rect_left_offset = 0;
		m_SequenceParameterSet->frame_cropping_rect_right_offset = 0;
		m_SequenceParameterSet->frame_cropping_rect_top_offset = 0;
		m_SequenceParameterSet->frame_cropping_rect_bottom_offset = 0;
	}

	if (0 == m_SequenceParameterSet->vui_parameters)
	{
		m_SequenceParameterSet->vui_parameters = new h264_vui_parameters;
	}

	if (0 == m_SequenceParameterSet->vui_parameters)
	{
		// Should not occur
		m_SequenceParameterSet->vui_seq_parameters_flag = false;
	}
	else
	{
		m_SequenceParameterSet->vui_seq_parameters_flag = true;

		if (0 == setup_parameters->vui_parameters)
		{
			// No VUI parameters in the setup parameters, so use defaults
			m_SequenceParameterSet->vui_parameters->aspect_ratio_info_present_flag = false;
			m_SequenceParameterSet->vui_parameters->overscan_info_present_flag = false;
			m_SequenceParameterSet->vui_parameters->video_signal_type_present_flag = false;
			m_SequenceParameterSet->vui_parameters->chroma_loc_info_present_flag = false;
			m_SequenceParameterSet->vui_parameters->timing_info_present_flag = false;
		}
		else
		{
			// Copy from VUI in setup parameters
			memcpy(m_SequenceParameterSet->vui_parameters, setup_parameters->vui_parameters, sizeof(h264_vui_parameters));
		}

		// Set some VUI parameters here
		m_SequenceParameterSet->vui_parameters->nal_hrd_parameters_present_flag = false;  // Parameters not set
		m_SequenceParameterSet->vui_parameters->vcl_hrd_parameters_present_flag = false;  // Parameters not set
		m_SequenceParameterSet->vui_parameters->low_delay_hrd_flag = !m_CodeAllPictures;
		m_SequenceParameterSet->vui_parameters->pic_struct_present_flag = false;  // No picture timing SEI messages at all
		m_SequenceParameterSet->vui_parameters->bitstream_restriction_flag = true;
		m_SequenceParameterSet->vui_parameters->motion_vectors_over_pic_boundaries_flag = true;
		m_SequenceParameterSet->vui_parameters->max_bytes_per_pic_denom = 0;  // Unspecified
		m_SequenceParameterSet->vui_parameters->max_bits_per_mb_denom = 0;  // Unspecified
		m_SequenceParameterSet->vui_parameters->log2_max_mv_length_horizontal = 13;
		m_SequenceParameterSet->vui_parameters->log2_max_mv_length_vertical = annex_a_level_table[m_SequenceParameterSet->level_index][7];
		m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering = m_SequenceParameterSet->num_ref_frames;

		{
			// No re-ordering so set num_reorder_frames to 0
			m_SequenceParameterSet->vui_parameters->num_reorder_frames = 0;
		}

	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::SetUpPictureParameterSet						*/
/* DESCRIPTION:	This is called to set up the given picture parameter set
				Normally only one is used, but two are used when there is
				more than one slice group (in P and SP pictures).
				One PPS is used for I, SI and B pictures, for which there
				is only one slice group. The other PPS is used for P and SP
				pictures. The definition and number of slice groups depends
				on number_of_slice_groups_for_P_pictures:
				2: map type 0: odd MB rows in one SG and even in the other.
				3: map type 0: SG0 = MB rows 0,3,6,; SG1 = MB rows 1, 4, 7,
				4: map type 1: simple pattern: 0 1 2 3, next row 2 3 0 1 ...*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::SetUpPictureParameterSet(int picture_parameter_set_id,
											int number_of_reference_pictures,
											int disable_deblocking_filter_idc,
											int slice_alpha_c0_offset_div2,
											int slice_beta_offset_div2,
											int chroma_qp_index_offset,
											bool constrained_intra_pred_flag,
											int number_of_slice_groups_for_P_pictures,
											bool CABAC)
{
	picture_parameter_set_t * picture_parameter_set
		= m_ParameterSets->ReadPictureParameterSet(picture_parameter_set_id);

	if (0 == picture_parameter_set)
	{
		picture_parameter_set = m_ParameterSets->WritePictureParameterSet(picture_parameter_set_id);
	}

	if (0 == picture_parameter_set)
	{
		assert(false);
		return false;
	}

	picture_parameter_set->sequence_parameter_set_id = 0;
	picture_parameter_set->CABAC_entropy_coding_mode = CABAC;

	// When encoding in frame mode only (H264_FRAME_CODING), content is assumed to be progressive
	// and top and bottom field POC are set to be the same. In all other cases top and bottom
	// field POC are different by one, with the order being given by m_TopFieldFirst. When this is true,
	// the top field is assumed to be output first, and have a POC one less than the bottom field POC.
	picture_parameter_set->pic_order_present_flag = false;

	// The slice group parameters are hard coded
	if (0 == picture_parameter_set_id)
	{
		// One slice group
		picture_parameter_set->num_slice_groups = 1;
	}
	else
	{
		// number_of_slice_groups_for_P_pictures slice groups
		switch (number_of_slice_groups_for_P_pictures)
		{
			case 2 :
				// slice_group_map_type 0 : Interleaved slice groups, by MB row
				picture_parameter_set->num_slice_groups = 2;
				picture_parameter_set->slice_group_map_type = 0;

				picture_parameter_set->slice_group_run_length[0] = m_SequenceParameterSet->frame_width_in_mbs;
				picture_parameter_set->slice_group_run_length[1] = m_SequenceParameterSet->frame_width_in_mbs;
				break;

			case 3 :
				// slice_group_map_type 0 : Interleaved slice groups, by MB row
				picture_parameter_set->num_slice_groups = 3;
				picture_parameter_set->slice_group_map_type = 0;

				picture_parameter_set->slice_group_run_length[0] = m_SequenceParameterSet->frame_width_in_mbs;
				picture_parameter_set->slice_group_run_length[1] = m_SequenceParameterSet->frame_width_in_mbs;
				picture_parameter_set->slice_group_run_length[2] = m_SequenceParameterSet->frame_width_in_mbs;
				break;

			case 4 :
				// slice_group_map_type 1 : Dispersed slice groups
				picture_parameter_set->num_slice_groups = 4;
				picture_parameter_set->slice_group_map_type = 1;
				break;

			default :
				// should not occur
				picture_parameter_set->num_slice_groups = 1;
				break;
		}
	}

	{
		// No B pictures
		picture_parameter_set->num_ref_idx_l0_active = number_of_reference_pictures;
		picture_parameter_set->num_ref_idx_l1_active = 1;	// Can't signal 0 pictures in L1 - value sent is num_ref_idx_l1_active minus 1
															// - can't send -1 in unsigned Golomb code.
	}

	{
		picture_parameter_set->weighted_pred_flag = false;
		picture_parameter_set->weighted_bipred_idc = 0;
	}

	if (m_FixedQuantiser)
	{
		picture_parameter_set->pic_init_qp = m_FixedQuantiserValue;
		picture_parameter_set->pic_init_qs = 26;
	}
	else
	{
		picture_parameter_set->pic_init_qp = 26;
		picture_parameter_set->pic_init_qs = 26;
	}

	picture_parameter_set->chroma_qp_index_offset = CLIP(-12,12,chroma_qp_index_offset);

	picture_parameter_set->deblocking_filter_parameters_present_flag =
		((0 != disable_deblocking_filter_idc) ||
		 (0 != slice_alpha_c0_offset_div2) ||
		 (0 != slice_beta_offset_div2));

	m_DisableDeblockingFilterIDC = disable_deblocking_filter_idc;

	picture_parameter_set->constrained_intra_pred_flag = constrained_intra_pred_flag;

	picture_parameter_set->redundant_pic_cnt_present_flag = false;

	return true;
}

/*
--------------------------------------------------------------------------------
Function	:	CH264Encoder::Destroy.
Description	:	Destroys the video encoder.
--------------------------------------------------------------------------------
*/

void CH264Encoder::Destroy(void)
{
	m_Created = false;


	int j;
	for (j=0; j<MAXIMUM_NUMBER_OF_SLICE_GROUPS ; j++)
	{
		CSymbolByteBuffer * next = m_SymbolByteBufferList[j];
		m_SymbolByteBufferList[j] = 0;
		while (0 != next)
		{
			CSymbolByteBuffer * current = next;
			next = current->m_Next;
			delete current;
		}
	}


	m_ThreadInit = 0;

	if (0 != m_ParameterSets)
	{
		delete m_ParameterSets;
		m_ParameterSets = 0;
	}

	if (0 != m_EncoderControl)
	{
		delete m_EncoderControl;
		m_EncoderControl = 0;
	}

	if (0 != m_EncodedPictureBuffer)
	{
		delete m_EncodedPictureBuffer;
		m_EncodedPictureBuffer = 0;
	}



	if (0 != m_SourcePictureStoreCodingOrder)
	{
		delete m_SourcePictureStoreCodingOrder;

		m_SourcePictureStoreCodingOrder = 0;
	}

#if	defined MAKE_ENCODER_TRACE_FILE
	if (0 != m_GeneralEncoderTraceFile) fclose(m_GeneralEncoderTraceFile);
#endif

	#if	defined MAKE_H264_SYMEJECT_TRACE_FILE
	  if (0 != m_TraceFile) fclose(m_TraceFile);
  #endif

	RateControlDestroy();

}

/*
--------------------------------------------------------------------------------
Function	:	CH264Encoder::Reset
Description	:	resets the video encoder.
--------------------------------------------------------------------------------
*/

bool CH264Encoder::Reset(void)
{
	m_FrameNumber = 0;
	m_FrameNumberOffset = 0;
	m_PictureOrderCount = 0;
	m_IDR_POC_Offset = 0;

	for (unsigned char slice_group=0; slice_group<MAXIMUM_NUMBER_OF_SLICE_GROUPS; slice_group++)
	{
		m_LastTransmittedQuant[slice_group] = 32;
	}

	m_PictureBitCount = 0;

	m_PreviousFrameAverageQuantiser = m_InitialQuant;


	return true;
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePicture									*/
/* DESCRIPTION:	This encodes the given picture according to the standard
				bitstream format of NAL Units.
				The current assumption is that parameter sets are transmitted
				once at the beginning of a stream. More options will need to
				be supported in due course.
				picture_type specifies whether the picture is to be coded
				with all P slices, all B slices, all I slices, all SP
				slices or all SI slices.
				gop_hierarchy_number indicates the hierarchy (layer) that
				the picture is to be in: 0 represents the "key" frames,
				higher numbers represent higher temporal resolutions.
				store_for_reference indicates whether the picture is to
				be used as a reference picture.
				delta_qp indicates the change to make to the quantiser
				index for this picture.
				number_of_bits_in_buffer indicates the number of bits in
				the bit buffer. This is used in real-time encoding, and
				can be zero otherwise.
				encoder_results provides results of encoding, including a
				pointer to the reconstructed picture.
				Up to buffer_length encoded bytes are placed in buffer.
				If this is sufficent for encoding the remainder of the
				picture, more_to_encode is false, otherwise it is true and
				the function must be called again with another byte buffer,
				until more_to_encode is false.
				The buffer length must be sufficient to contain the
				largest possible coded macroblock, which is currently
				estimated to be 400 bytes.
				On subsequent calls to encode the same picture, the
				parameters picture and number_of_bits_in_buffer are not used.
				When more_to_encode is false it means that the encoder needs
				another source picture in order to continue
				Encoder_results only contains valid values
				when statistics_valid is true.

				The function returns when a NAL unit has being written into
				the given buffer. The first byte of this buffer contains the
				NAL unit header: the forbidden bit equal to 0,
				nal_storage_idc equal to 0 or 3 according to whether the
				information is disposable or not (1 and 2 are intended for
				use by us with data partitioning). and the nal_unit_type

				The frame number of the source picture is modified to be
				sequential starting from 0 for coded pictures (rather
				than dropped ones). This allows picture order count to
				operate efficiently.										*/

/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePicture(CBTPicture* picture,
								 h264_picture_type_t picture_type,
								 int number_of_bits_in_buffer,
								 int target_bit_count,
								 h264_encoder_results** encoder_results,
#ifdef ARC_ENCODER_API
								 struct packet_queue_t* opq,
#endif
								 int& buffer_length,
								 bool& more_to_encode, bool& statistics_valid)
{
	if (!m_Created)
	{
		assert(false);
		return false;
	}

	if (m_IncompatibleSettings)
	{
		assert(false);
		return false;
	}
	if((m_DynamicControl == 2)&&(picture->m_fn)) 
	{
		Service_EnableTimer();
	}

	{
		// Copy the parameters into the member variables
		// Otherwise, when coding a stored frame, they will be read back from the store
		// Need to ensure that when two or more slices are produced for a stored picture
		// that the value read back from the store is not overwritten here on the second
		// and subsequent slices.
		m_PictureType = picture_type;
	}

	m_MaximumNumberOfBytes = buffer_length-1;
	if (m_MaximumNumberOfBytes > m_MaximumPacketDataSize-1)
	{
		m_MaximumNumberOfBytes = m_MaximumPacketDataSize-1;
	}

	if (m_MaximumNumberOfBytes < MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK)
	{
		assert(false);
		return false; // No guarantee that the packet is big enough for a coded macroblock
	}

	bool return_value = true;  // Function returns OK unless this is set to false
	bool finished_encoding_frame = false;
	more_to_encode = true;

#ifdef ARC_ENCODER_API
	m_error = 0;
	assert(opq != 0);
	m_output_q = opq;
#endif

	ClearReturnParameters();

	//	set address of the encoding parameters being returned to the calling function
	*encoder_results = &m_H264EncoderResults;

	while (true) // A single iteration only - break to exit function
	{
		////////////////////////////////////////////////////////////////////////////////////
		// Sequence Parameter Set
		////////////////////////////////////////////////////////////////////////////////////

		if (m_TransmitSequenceParameterSet)
		{
			m_CurrentSymbolByteBuffer[0] = m_SymbolByteBufferList[0];
			m_CurrentSliceGroup = 0;

			m_NextBufferToCopy = m_SymbolByteBufferList[0];
			m_CurrentCopySliceGroup = 0;

			m_ThreadBufferStatus = m_OverallBufferStatus;

			Service_WriteSequenceHeader();

			m_TransmitSequenceParameterSet = false;
      // In the outer loop currently Service_GetChannelResponse
      // is called multiple times. This needs to be change and will be called
      // only once. Should be done when all the EE functions are
      // called in order from the response functions and the main outer
      // loop is changed
#ifndef HARDWARE_EE
			Service_GetChannelResponseModel();
#else
			Service_GetChannelResponse();
#endif

			m_CurrentSymbolByteBuffer[0] = m_SymbolByteBufferList[0];
			m_CurrentSliceGroup = 0;

			m_NextBufferToCopy = m_SymbolByteBufferList[0];
			m_CurrentCopySliceGroup = 0;

			m_ThreadBufferStatus = m_OverallBufferStatus;

			Service_WriteUserData();
			Service_GetChannelResponse();

			break;

		}

		////////////////////////////////////////////////////////////////////////////////////
		// Picture Parameter Set
		////////////////////////////////////////////////////////////////////////////////////

		if (m_NextPictureParameterSetToTransmit < m_NumberOfPictureParameterSets)
		{
			m_CurrentSymbolByteBuffer[0] = m_SymbolByteBufferList[0];
			m_CurrentSliceGroup = 0;

			m_NextBufferToCopy = m_SymbolByteBufferList[0];
			m_CurrentCopySliceGroup = 0;

			//m_NALReferenceIDC = 3;
			//m_NALUnitType = H264_PICTURE_PARAMETER_SET;

			m_ThreadBufferStatus = m_OverallBufferStatus;

			Service_WritePictureParameter(m_NextPictureParameterSetToTransmit);

			m_NextPictureParameterSetToTransmit++;

			Service_GetChannelResponse();
			break;
		}

		////////////////////////////////////////////////////////////////////////////////////
		// Frame Start
		////////////////////////////////////////////////////////////////////////////////////

			m_SourcePicture = picture;

			EncoderMainFrameStart(number_of_bits_in_buffer, more_to_encode, target_bit_count);

			if (!more_to_encode)
			{
				// Frame has been dropped or has been stored for later encoding
				buffer_length = 0;
				finished_encoding_frame = true;

				break;
			}


		////////////////////////////////////////////////////////////////////////////////////
		// Picture Start
		////////////////////////////////////////////////////////////////////////////////////

			if (!EncoderMainPictureStart())
			{
				// Error condition
				assert(false);
				return_value = false;
				break;
			}

			// Distribute m_OverallBufferStatus across m_ThreadBufferStatus
			m_ThreadBufferStatus = m_OverallBufferStatus;


			// Encode Macroblocks
			m_NextBufferToCopy = m_SymbolByteBufferList[0];

			m_CurrentCopySliceGroup = 0;


			if (!EncodePictureMainEncodeMacroblocks())
			{
				// Error condition
				assert(false);
				return_value = false;
				break;
			}

			// Total up the buffer counts, restore m_OverallBufferStatus
		m_OverallBufferStatus = m_ThreadBufferStatus;


		////////////////////////////////////////////////////////////////////////////////////
		// Picture End
		////////////////////////////////////////////////////////////////////////////////////

			// Just packetised the last slice

			if (!EncodePictureMainPictureEnd())
			{
				// Error condition
				return_value = false;
				assert(false);
				break;
			}


				// Finished encoding frame

				// Assumes top field is coded first
				finished_encoding_frame = true;

				// Was the frame a re-ordered frame or do we have any re-ordered frames outstanding?
					m_H264EncoderResults.source_picture_finally_encoded = false;

					more_to_encode = false;

		break;
	}

#ifdef ARC_ENCODER_API
	if (m_error != 0)
	    return_value = false;
#endif

	if (return_value)
	{
		statistics_valid = finished_encoding_frame;

		if (statistics_valid)
		{
			m_H264EncoderResults.bit_count = m_PictureBitCount;

			RateControlFrameEnd();
		}
	}
	else
	{
		// In case of error, consider the picture to be complete, i.e. give up on it!
		more_to_encode = false;
		statistics_valid = false;
	}


	if (finished_encoding_frame)
	{
		m_PictureBitCount = 0;

		if (m_RetransmitParameterSets)
		{
			m_TransmitSequenceParameterSet = true;
			m_NextPictureParameterSetToTransmit = 0;

			m_RetransmitParameterSets = false;
		}
		if(m_DynamicControl == 2) 
		{
			Service_DisableTimer(); // to avoid counting frame reading from file
		}
	}


	return return_value;
}

/*
--------------------------------------------------------------------------------
Function SetReturnParameters.
--------------------------------------------------------------------------------
*/

void CH264Encoder::SetReturnParameters(CBTPicture * encoded_picture, CBTPicture * source_picture,
									   CBTPicture * output_picture)
{
	if (!m_Created) return;

  m_H264EncoderResults.intra_16x16_count = m_TotalIntra16x16Count;
	m_H264EncoderResults.inter_16x16_count = m_TotalInter16x16Count;
	m_H264EncoderResults.inter_16x8_count = m_TotalInter16x8Count;
	m_H264EncoderResults.inter_8x16_count = m_TotalInter8x16Count;
	m_H264EncoderResults.inter_8x8_count = m_TotalInter8x8Count;
	m_H264EncoderResults.skipped_count = m_TotalSkippedCount;
	m_H264EncoderResults.forced_skip_count = m_TotalForcedSkipCount;
	m_H264EncoderResults.skip_8x8_count = m_TotalSkip8x8Count;
	m_H264EncoderResults.direct_count = m_TotalDirectCount;
	m_H264EncoderResults.max_ISlice_mb_bits = m_MaxISliceMBBits;
	m_H264EncoderResults.max_PSlice_mb_bits = m_MaxPSliceMBBits;
	m_H264EncoderResults.cycles_in_frame  = m_CyclesInFrame;


	if (m_RealTimeRateControl)
	{
		m_H264EncoderResults.buffer_level = 0;
	}
	else
	{
		m_H264EncoderResults.buffer_level = (unsigned int)(m_OverallBufferStatus);
	}

#ifdef STATISTICS
	if (0 != m_NumberOfQuantisers)
	{
		m_H264EncoderResults.average_quant = (double)(m_TotalQuantiser) / (double)(m_NumberOfQuantisers);
	}
	else
	{
		m_H264EncoderResults.average_quant = 0; // Not sure how this could ever occur, but prevent a crash
	}
#endif // STATISTICS
	m_H264EncoderResults.pFrameY = encoded_picture->m_y;
	m_H264EncoderResults.pFrameU = encoded_picture->m_u;
	m_H264EncoderResults.pFrameV = encoded_picture->m_v;
	m_H264EncoderResults.pEncodedPicture = (void *)(encoded_picture);
	m_H264EncoderResults.pSourcePicture = (void *)(source_picture);
	m_H264EncoderResults.pOutputPicture[m_H264EncoderResults.number_of_output_pictures] = (void *)(output_picture);

	m_H264EncoderResults.field_coding = false;
	if (0 != output_picture)
	{
		m_H264EncoderResults.number_of_output_pictures++;
	}
}

/*
--------------------------------------------------------------------------------
Function ClearReturnParameters.
--------------------------------------------------------------------------------
*/

void CH264Encoder::ClearReturnParameters(void)
{
	memset(&m_H264EncoderResults, 0, sizeof(h264_encoder_results));
}






/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePictureMainEncodeMacroblocks			*/
/* DESCRIPTION:	Encodes all the macroblocks, or those for which the calling
				thread is responsible										*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePictureMainEncodeMacroblocks()
{
	// Set slice id
	
	int mb_number = 0;
	// set slicegroup data for 1 slice group per P frame
	int sliceGroup = MgrMPC.Picture.SliceGroupMap[0];
	m_SliceID[sliceGroup] = (m_NumberOfSliceGroupsForPPictures) ;
	m_SkippedMacroblockCount[sliceGroup] = 0;
	m_FirstMacroblockInSlice[sliceGroup] = 0;
	m_LastMacroblockInSlice[sliceGroup] = m_NumberOfMacroblocksInPicture-1;

	#ifdef ENABLE_PERFORMANCE
	Reset_Timer0();
        #ifdef ENABLE_BVCI_COUNTERS
        Reset_BVCI_profiler();
        #endif
	#endif

    // setup me current and strides and resolutions at start of every stream
    Service_ME_StreamSetup();
    // setup me reference at start of every picture
    Service_ME_PictureSetup();

	if(m_DynamicControl)
	{
		if(m_DynamicControl == 2) 
		{
			if (m_SourcePicture->m_fn) // don't call for first frame
			{
				m_EndCycles = _lr(AUXREG_TIMER_CTIMER_COUNT); // number of cycles since end of last frame MBs	
				Service_StopTimer();
			}
			int temp = (((m_EndCycles>>4)*m_NumMBsRecip + (1<<19))>>20)<<4;
			if(temp > m_CycleBudgetMB) temp = m_CycleBudgetMB>>1;
			int new_limit = m_MBAdaptFreq*(m_CycleBudgetMB - temp);
			Service_InitTimer(new_limit);
			Service_EnableTimer(); // enable now to include all
		}
		else // m_DynamicControl ==1
		{
			Service_InitTimer(m_MBAdaptFreq*m_CycleBudgetMB);
			//enable this at first slice header
		}
	}

#ifdef DUMP_DEBUG
   DumpDebug_String("");
   DumpDebug_String("################################################################################");
   DumpDebug_Value("Picture", m_FrameNumberOffset + m_FrameNumber);
#endif // DUMP_DEBUG

   MgrMPC.Deblock.Init(
	   m_DisableDeblockingFilterIDC,						// Mode
	   m_PictureParameterSet->chroma_qp_index_offset		// chroma thingy
	   );

	////////////////////////////////////////////////////////////////////////////////////
	// Loop over macroblocks for encoding
	////////////////////////////////////////////////////////////////////////////////////

	MgrMPC.ResetTopLeft();      // Reset macro block pos
	mb_number = 0;

	
	
	SET_CallBackStartME_BufIdx(0);

	// Starting point for power analysis measurments
	_ASM("PowerAnalysisPictureStart::");

	_vrun(MACRO_CallBackStartME);
	do
	{
		(HandleServiceCall)();
	}
	while((MgrMPC.MacroBlocksCompleteCount < m_NumberOfMacroblocksInPicture)|| MgrMPC.CurrentEEMb.SliceNotFinished);


	// End point for power analysis measurments
	_ASM("PowerAnalysisPictureEnd::");



	
	#ifdef ENABLE_PERFORMANCE
		Timer0_Val_To_Mem();
        #ifdef ENABLE_BVCI_COUNTERS
        BVCI_profiler_val_to_mem();
        #endif
	#endif


	
	


	return true;
}





void CH264Encoder::AdjustSettings(int panic_level)
{
// order of switching
// 1. increase 8x8 thresh to max
// 2. switch off 16x8 and 8x16
// 3. switch off 8x8
// 4. increase skip thresh to max
// 5. increase stepOneControl (0 ->2)
// 6. reduce sub pel control (2 -> 0)

	m_PanicLevel = CLIP(0, NUM_H264_PANIC_SETTINGS-1, m_PanicLevel + panic_level);

	m_Skip8x8ThresholdMult =	dynamic_control_lut[m_PanicLevel][0];
	m_UseAllInterModes =		dynamic_control_lut[m_PanicLevel][1];
	m_UseInter16x16Only =		dynamic_control_lut[m_PanicLevel][2];
	m_SkipSadThresholdMult =	dynamic_control_lut[m_PanicLevel][3];
	m_StepOneControl =			dynamic_control_lut[m_PanicLevel][4];
	m_SubPelControl =			dynamic_control_lut[m_PanicLevel][5];
	m_UseWhitening       =		(dynamic_control_lut[m_PanicLevel][6]==1);
	m_UseIntraDCpredOnly =		(dynamic_control_lut[m_PanicLevel][7]==1);
	m_UseIntraVertPredOnly =	(dynamic_control_lut[m_PanicLevel][8]==1);
	m_NoIntraInPSlice =			(dynamic_control_lut[m_PanicLevel][9]==1);


	// now we've changed the settings we need to update the me 
	if (gCH264Encoder.m_SliceType != H264_I_SLICE)
	{
	    if (HwMe == 1)
	    {
	        if(m_UseAllInterModes) // allow Inter16x8 and Inter8x16 as well as usual Inter16x16 and Inter8x8
	        {
	            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((2 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
	                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	    												      ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	        else if(!m_UseInter16x16Only) // only allow Inter16x16 and Inter8x8
	        {
	            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	    											          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	        else  // only 16x16
	        {
	    
	            WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
	                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	    													  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	    
	        WriteToChannelME_Hw(CHANREG_ME_CONTROL1, (u_int32) (((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT) |
	                                                         ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT)));
	    }
	    else
	    {
	        if(m_UseAllInterModes) // allow Inter16x8 and Inter8x16 as well as usual Inter16x16 and Inter8x8
	        {
	            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((2 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
	                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	        else if(!m_UseInter16x16Only) // only allow Inter16x16 and Inter8x8
	        {
	            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	        else  // only 16x16
	        {
	    
	            WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((1 & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT)   |
	                                                          ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                          ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT)));
	        }
	    
	        WriteToChannelME_Sw(CHANREG_ME_CONTROL1, (u_int32) (((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT) |
	                                                         ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT)));
	    }
	}

	if( m_UseIntraVertPredOnly && (H264_I_SLICE == m_SliceType)) 
	{
		// don't allow vertonly in I frames - get streaky effects
		m_UseIntraVertPredOnly = 0;
		m_UseIntraDCpredOnly = 1;
	}

}



/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
