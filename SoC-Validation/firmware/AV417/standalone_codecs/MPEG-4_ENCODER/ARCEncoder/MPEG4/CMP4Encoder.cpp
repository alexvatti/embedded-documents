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
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */

#include "CMP4Encoder.h"
#include "MP4EncodedPictureBuffer.h"
#include "SymbolByteBuffer.h"
#include "picture_encoder.h"
#include "MP4Tables.h"
#include "MP4EncoderTables.h"
#include <assert.h>
#include <memory.h>
#include "math.h"
#include "ArcEntropyEncoderModel.h"
#include "defines.h"
#include "ArcChannelRoutines.h"

#include "ArcFrameBuffers.h"
#include "ArcMotionEstimationModel.h"
#include "ArcMP4SDMTables.h"
#include "ArcChannelRoutines.h"

#include "ArcEncoderMain_MPA.h"

#include "ArcTimer.h"

extern void (*HandleServiceCall)();

extern int picture_number;

#ifndef ARC_ENCODER_API
extern FILE *stats_file ;
#endif


#ifdef ENABLE_PERFORMANCE

static void Reset_Timer0(void);
static void Timer0_Val_To_Mem(void);
//------------------------------------------------------------------------------

_Asm void DisableInterrupts( )
{
  FLAG 0     ; Disable Interrupt
}

_Asm void EnableInterrupts( )
{
  FLAG 6     ; Enable Interrupt
}
/*
void Reset_Timer0(void)
{
	DisableInterrupts();
  _sr(3,              0x22);       // CONTROL0 = 2 -- NH
  _sr(0xffffffff,     0x23);       // LIMIT0 = -1
  _sr(0,              0x21);       // COUNT0 = 0
}
void Timer0_Val_To_Mem()
{
       unsigned int val_t = _lr(0x21);
       //(YUVBuffer.TimerTicks)[idx] = val_t;
       *(YUVBuffer.TimerTicks) = val_t;
       YUVBuffer.TimerTicks++;
       _sr(0x1,0x47);
#ifdef PRINT_FRAME_DATA
	   Service_DisableTimer();
	   printf("Ticks = %d, End_cycles %d, panic_lev = %d\n",val_t, gCH264Encoder.m_EndCycles, gCH264Encoder.m_PanicLevel);
	   Service_EnableTimer();
#endif // PRINT_FRAME_DATA
}
*/
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
       //(YUVBuffer.TimerTicks)[idx] = val_t;
       *(YUVBuffer.TimerTicks) = val_t;
       YUVBuffer.TimerTicks++;
#ifdef PRINT_FRAME_DATA
	   Service_DisableTimer();
	   printf("Ticks = %d, End_cycles %d, panic_lev = %d\n",val_t, gCH264Encoder.m_EndCycles, gCH264Encoder.m_PanicLevel);
	   Service_EnableTimer();
#endif // PRINT_FRAME_DATA
}

#endif

#if	defined MAKE_ENCODER_TRACE_FILE
#define		WRITE_STRING(Name)	{ if (0 != m_GeneralEncoderTraceFile) fprintf(m_GeneralEncoderTraceFile,"%s\n",(Name)); }
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)	{ if (0 != m_EncoderTraceFile) fprintf(m_GeneralEncoderTraceFile"%s %i\n",(Name), (Number)); }
#else
#define		WRITE_STRING(Name)
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)
#endif

// ==========================================================================
// One global instance (As only one HW block) ===============================
// ==========================================================================
CH264Encoder gCH264Encoder;
// ==========================================================================
#define MIN_MPEG4_PANIC_LEVEL 0
#define MIN_H263_PANIC_LEVEL  32 // don't include 8x8 settings for baseline H263

#define NUM_MPEG4_ME_PANIC_SETTINGS 64
const char init_mp4panics[11][6] = {
// subpel, step1, skipthresh, Inter16only, 8x8thresh, lutintoMEpanic
	0,		2,      32,			1,			32,			60, // level 0
	1,      2,      32,			1,			32,			56, // level 1
	1,      0,      32,			1,			32,			52, // level 2
	1,      0,      32,			1,			32,			48, // level 3
	1,      0,		28,			1,			32,			44, // level 4
	1,      0,      24,			1,			32,			40, // level 5
	1,      0,      20,			1,			32,			36, // level 6
	1,      0,      20,			0,			32,			30, // level 7
	1,      0,      20,			0,			20,			20, // level 8
	1,      0,      20,			0,			10,			10, // level 9
	1,      0,      20,			0,			0,			0	  // level 10


};

const char me_mp4control_lut[NUM_MPEG4_ME_PANIC_SETTINGS][5] =
{ // skip_8x8,  8x8, skip_sad, step1, subpel
	0,		0,	20,	0,	1,
	1,		0,	20,	0,	1,
	2,		0,	20,	0,	1,
	3,		0,	20,	0,	1,
	4,		0,	20,	0,	1,
	5,		0,	20,	0,	1,
	6,		0,	20,	0,	1,
	7,		0,	20,	0,	1,
	8,		0,	20,	0,	1,
	9,		0,	20,	0,	1,
	10,		0,	20,	0,	1,
	11,		0,	20,	0,	1,
	12,		0,	20,	0,	1,
	13,		0,	20,	0,	1,
	14,		0,	20,	0,	1,
	15,		0,	20,	0,	1,
	16,		0,	20,	0,	1,
	17,		0,	20,	0,	1,
	18,		0,	20,	0,	1,
	19,		0,	20,	0,	1,
	20,		0,	20,	0,	1,
	21,		0,	20,	0,	1,
	22,		0,	20,	0,	1,
	23,		0,	20,	0,	1,
	24,		0,	20,	0,	1,
	25,		0,	20,	0,	1,
	26,		0,	20,	0,	1,
	27,		0,	20,	0,	1,
	28,		0,	20,	0,	1,
	29,		0,	20,	0,	1,
	30,		0,	20,	0,	1,//30
	31,		0,	20,	0,	1,
	32,		0,	20,	0,	1,
	32,		1,	20,	0,	1,//33
	32,		1,	20,	0,	1,
	32,		1,	20,	0,	1,
	32,		1,	20,	0,	1,//36
	32,		1,	21,	0,	1,
	32,		1,	22,	0,	1,
	32,		1,	23,	0,	1,
	32,		1,	24,	0,	1,//40
	32,		1,	25,	0,	1,
	32,		1,	26,	0,	1,
	32,		1,	27,	0,	1,
	32,		1,	28,	0,	1,//44
	32,		1,	29,	0,	1,
	32,		1,	30,	0,	1,
	32,		1,	31,	0,	1,
	32,		1,	32,	0,	1,//48
	32,		1,	32,	0,	1,
	32,		1,	32,	0,	1,
	32,		1,	32,	0,	1,
	32,		1,	32,	1,	1,//52
	32,		1,	32,	1,	1,
	32,		1,	32,	1,	1,
	32,		1,	32,	1,	1,
	32,		1,	32,	2,	1,//56
	32,		1,	32,	2,	1,
	32,		1,	32,	2,	1,
	32,		1,	32,	2,	1,
	32,		1,	32,	2,	0,// 60
	32,		1,	32,	2,	0,
	32,		1,	32,	2,	0,
	32,		1,	32,	2,	0  // 63
};

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::CH264Encoder									*/
/* DESCRIPTION:	H.264 encoder constructor.									*/
/* ------------------------------------------------------------------------ */


CH264Encoder::CH264Encoder(void)
{
	m_Created = false;
	m_IncompatibleSettings = false;
	//m_IntraMacroblocks = false;
	m_NumberOfMBs = 0;
	m_ThreadInit = false;
	m_DebugLevel = 0;
	m_SourcePicture = 0;
	m_MoreToEncode = false;
	m_EncoderMainFrameStart = true;
	m_EncoderMainPictureStart = true;
	m_EncoderMainCodeMBs = false;
	m_PictureBitCount = 0;
	m_MaximumNumberOfBytes = 0;

	m_CodeAllPictures = false;

	m_SourcePictureStoreCodingOrder = 0;
	m_IDRPictureSourceFN = 0;

	m_EncoderControl = 0;
	m_EncodedPictureBuffer = 0;
	m_TraceFile = 0;
	m_RateConTraceFile = 0;
	m_GeneralEncoderTraceFile = 0;

	m_CurrentCopySliceGroup = 0;

	m_OverallBufferStatus = 0;

   int j;
	for (j=0; j<MAXIMUM_NUMBER_OF_SLICE_GROUPS ; j++)
	{
		m_SymbolByteBufferList[j] = 0;
		m_CurrentSymbolByteBuffer[j] = 0;
	}

	//m_BuffersToCopy = 0;
	m_ThreadBufferStatus = 0;

	m_ForwardWorkingArray = 0;
    m_StartNewPacket = false;
    m_time_since_last_intra_refresh = 0;

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
	if(m_coding_standard == MPEG4)
	{
		m_TransmitSequenceParameterSet = true;
	}
	else
	{
		m_TransmitSequenceParameterSet = false;
	}
	m_CodeNextPictureAsIDR = true;
	m_InputPictureNumber = 0;
	m_MaximumPacketDataSize = 0;

	m_EncoderControl = new CH264EncoderControl();
	if (0 == m_EncoderControl)
	{
		Destroy();
		assert(false);
		return false;
	}

    init_uni_dc_tab2();
    RateControlInitialisation();

	m_IDRPictureID = 0;

	m_Created = true;

#if	defined MAKE_ENCODER_TRACE_FILE
//	char file_name[30];
//	sprintf(file_name, "MP4GeneralEncoderTraceFile.txt");
	m_GeneralEncoderTraceFile = fopen("MP4GeneralEncoderTraceFile.txt","w");
#endif
	extern void aanscaletable_init(void);
	aanscaletable_init();
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

	if (!m_ThreadInit)
	{
		m_ForwardWorkingArray = new working_array_t;
	}
	m_frame_width_in_mbs = (setup_parameters->frame_width + 15) >> 4;;
	m_frame_height_in_mbs = (setup_parameters->frame_height + 15) >> 4;
	m_coding_standard = setup_parameters->coding_standard;

	m_h263_use_gob_headers = setup_parameters->use_gob_headers;
	m_h263_rows_per_gob = 1;
	if(m_frame_width_in_mbs * m_frame_height_in_mbs == 44*36) {
		m_h263_rows_per_gob = 2;
	}
	m_umvplus = setup_parameters->use_annex_D;
	m_h263_aic = setup_parameters->use_annex_I;
	m_h263_loop_filter = setup_parameters->use_annex_J;
	m_h263_slice_structured = setup_parameters->use_annex_K;
	m_h263_modified_quant = setup_parameters->use_annex_T;

	m_use_hec = setup_parameters->use_hec;
	m_video_packets = setup_parameters->use_video_packets;
	m_use_ac_pred = 0;

	if(setup_parameters->intra_refresh_rate != 0) {
		m_IntraMBRefreshRate = (m_frame_width_in_mbs*m_frame_height_in_mbs)/setup_parameters->intra_refresh_rate;
	}
	else m_IntraMBRefreshRate = 131;
	// check that coprime with number of MBs in frame, otherwise, we will refesh the same blocks each time
	// we are assuming that we are always encoding with qcif, cif, or 4cif frames or some derived size
	int qcif =(m_frame_width_in_mbs==11)&&(m_frame_height_in_mbs==9);
	while(((m_IntraMBRefreshRate/11)*11 == m_IntraMBRefreshRate) ||  //columns are always divisible by 11
		((m_IntraMBRefreshRate/3)*3 == m_IntraMBRefreshRate) ||		 // rows are always divisible by 3
		(!qcif &&((m_IntraMBRefreshRate/2)*2 == m_IntraMBRefreshRate))	)	 // cif and above are always divisible by 2
	{
		m_IntraMBRefreshRate--;
	}

	if(m_IntraMBRefreshRate > 132) // won't conform to standards
	{
		m_IntraMBRefreshRate = 131; // prime number, guaranteed to be coprime
	}
	m_IntraUpdateMBNumber = 0;
	m_IntraUpdateSliceGroupID = 0;


	m_SkipSadThresholdMult = MIN_SKIP_16X16_MP4;
	m_Skip8x8ThresholdMult = MIN_SKIP_8X8_MP4;
	m_StepOneControl = 0; // do full first step
	m_SubPelControl = 1; // do full sub-pel precision

	m_frame_rate = 90000/setup_parameters->target_picture_period;


	m_RealTimeRateControl = setup_parameters->real_time_rate_control;

	m_PanicLevel = 0;
	if (m_coding_standard == MPEG4){
		//m_UseInter16x16Only = 1;
		m_MinPanicLevel = MIN_MPEG4_PANIC_LEVEL;
		m_use_4MV = 1;
	}
	else { // H263
		m_UseInter16x16Only = 1;
		m_use_4MV = 0;
		m_MinPanicLevel = MIN_H263_PANIC_LEVEL;
	}
 






   m_pad_ctrl = 0;
  	if(m_coding_standard == MPEG4){	      // MPEG4
		m_pad_ctrl = 1;
		m_umvplus = 1;// as we are using fcode == 1, standard says in B.1.3 that 
						// effectively MVs must lie within [-16, 15.5] of predictor
  	}
	else if(m_coding_standard == H263)  // H263
	{
		m_pad_ctrl = 0;
		if(m_umvplus)m_pad_ctrl = 1;
	}
	else if (setup_parameters->use_annex_J)// H263plus
	{
		m_pad_ctrl = 1;  // MVs over boundaries             
		m_umvplus = 0;   // annex D not supported for H263plus
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	//  Create/Resize Symbol Buffers
	//
	///////////////////////////////////////////////////////////////////////////////

	// At the moment only creating one SymbolByteBuffer.  This will change!

	if ((m_MaximumPacketDataSize != setup_parameters->maximum_packet_size) || (!m_ThreadInit))
	{
		// Note this will be true the first time as m_MaximumCodedSliceSize is set up to be zero

		m_MaximumPacketDataSize = setup_parameters->maximum_packet_size;

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

//			if (!m_SymbolByteBufferList[j]->Create(m_MaximumPacketDataSize))

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
	
	MgrMPC.Init(m_frame_width_in_mbs, m_frame_height_in_mbs);

	m_NumberOfMBs = m_frame_width_in_mbs * m_frame_height_in_mbs;
	if(m_NumberOfMBs)m_NumMBsRecip = (1<<20)/m_NumberOfMBs;
	int number_of_mbs_per_second = (90000 * m_NumberOfMBs) / setup_parameters->target_picture_period;
	

	m_MBAdaptFreq = m_frame_width_in_mbs;
	if(m_MBAdaptFreq == 0)m_MBAdaptFreq = 9000; // i.e. not used
	m_CycleBudgetMB = setup_parameters->quality_level*NOMINAL_FREQ*200000/number_of_mbs_per_second;
		
	if(setup_parameters->quality_level == 0)m_CycleBudgetMB = NOMINAL_FREQ*100000/number_of_mbs_per_second;

	if(setup_parameters->quality_level == 10)m_CycleBudgetMB = m_CycleBudgetMB*3; // 1200MHz

	m_DynamicControl = setup_parameters->dynamic_control ;
	int index = setup_parameters->quality_level;
	if(m_DynamicControl) // 1 or 2 
	{
		m_EndCycles = 0; // may need to guess this !
		m_CycleBudgetMB = setup_parameters->quality_level*1000000/number_of_mbs_per_second;
		if(setup_parameters->quality_level == 0)m_CycleBudgetMB = 200000000/number_of_mbs_per_second;	

		if(m_NumberOfMBs < 352*576)index = 10; // less than 2CIF assume we can do everything
		else index = (setup_parameters->quality_level-200)/20 + 5;
	
		//to enable selection of initial settings
	}

	if((index > 10) || (index < 0)){
		// compatibility with normal golden model
		index = 10;
		
	}

	m_SubPelControl =			init_mp4panics[index][0];
	m_StepOneControl =			init_mp4panics[index][1];
	m_SkipSadThresholdMult =	init_mp4panics[index][2];
	m_UseInter16x16Only =		init_mp4panics[index][3];
	m_Skip8x8ThresholdMult =	init_mp4panics[index][4];
	

	if ((m_coding_standard == H263) && (setup_parameters->use_annex_J == 0))
		m_UseInter16x16Only =  1; // H263plus - but should check to see if J annex is on



	if(m_DynamicControl) 
	{
		m_PanicLevel = CLIP(m_MinPanicLevel, NUM_MPEG4_ME_PANIC_SETTINGS-1, init_mp4panics[index][5]);
	}
	


#ifdef _DEBUG	
	printf("dynamic control %d, m_PanicLevel %d, subpel %d, step1 %d, skipSad %d, inter16 %d, skip8 %d\n",m_DynamicControl, m_PanicLevel, m_SubPelControl, m_StepOneControl, m_SkipSadThresholdMult, m_UseInter16x16Only,m_Skip8x8ThresholdMult);
#endif	
	m_LastHitCount  = 3000;
	m_CodeAllPictures = setup_parameters->code_all_pictures; 
	m_FixedQuantiser = setup_parameters->fixed_quantiser;
	m_FixedQuantiserValue = setup_parameters->fixed_quantiser_value;
	m_FixedQuantiserValue = CLIP(1, 31, m_FixedQuantiserValue);
	if (m_FixedQuantiser)
	{
		m_LastTransmittedQuant = m_FixedQuantiserValue;

		//m_PreviousFrameAverageQuantiserDouble = m_FixedQuantiserValue;
		m_PreviousFrameAverageQuantiser = m_FixedQuantiserValue;
	}

	m_TargetBitRate = setup_parameters->target_bitrate;
	if(setup_parameters->fixed_quantiser && (setup_parameters->target_bitrate < 64000))
	{
		m_TargetBitRate = setup_parameters->target_bitrate = 64000*16/(setup_parameters->fixed_quantiser_value);
	}
	m_TargetPicturePeriod = setup_parameters->target_picture_period;
	m_InputPicturePeriod = setup_parameters->input_picture_period;

	m_ReactionMult = setup_parameters->reaction_multiplier;


	m_AverageBitsPerFrame = (((m_TargetBitRate >>8)* m_TargetPicturePeriod)/90000)<<8;

	if(m_NumberOfMBs > 100) {
		m_AverageBitsPerFrame -= (30*(m_ReactionMult));
	}

	m_AverageBitsPerMacroblock = (m_AverageBitsPerFrame*m_NumMBsRecip + (1<<19))>>20;//m_AverageBitsPerFrame / m_NumberOfMBs;

	m_InitialQuant = (int)((210.0/m_AverageBitsPerMacroblock) + 5.0);

	m_InitialQuant = CLIP(4, 15, m_InitialQuant);
	
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
	m_BitsLeftInGOP = 0;
	
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
		//double picture_size_factor = (double)(176*144)/(setup_parameters->frame_width * setup_parameters->frame_height);

		//m_PreviousFrameAverageQuantiserDouble = 
		//	1 + (50 * (5.348-log10(m_AverageBitsPerFrame * picture_size_factor))) / 3;
		m_PreviousFrameAverageQuantiser = setup_parameters->fixed_quantiser_value;
	}
	else
	{
	//	m_PreviousFrameAverageQuantiserDouble = setup_parameters->fixed_quantiser_value;
		m_PreviousFrameAverageQuantiser = m_InitialQuant;
	}

	// Clip to max and min values of quantiser just in case
	m_PreviousFrameAverageQuantiser = MAX(MINIMUM_QUANTISER_INDEX_MPEG4, MIN(MAXIMUM_QUANTISER_INDEX_MPEG4, (m_PreviousFrameAverageQuantiser)));
	//m_PreviousFrameAverageQuantiserDouble = m_PreviousFrameAverageQuantiser;

	//double h263_equivalent_q = 3 * pow(2.0, (m_PreviousFrameAverageQuantiserDouble-24.0) / 6.0);
	int h263_equivalent_q = m_PreviousFrameAverageQuantiser;


	m_ReactionParameter = (m_ReactionMult * m_TargetBitRate)>>5;

	if(m_ReactionParameter)m_InvReactParX31 = ((31<<24)/m_ReactionParameter);
	else m_InvReactParX31 = 10;

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
		= 384 * m_frame_width_in_mbs * m_frame_height_in_mbs * number_of_reference_pictures;
	
	int video_bit_rate = setup_parameters->target_bitrate;

	int level_index = 0;

	while (true)
	{
		if (/*(number_of_mbs_per_second <= annex_a_level_table[level_index][1]) &&*/
			(m_NumberOfMBs <= annex_x_level_table[level_index][1]) &&
			setup_parameters->target_picture_period >= annex_x_level_table[level_index][2] && 
			/*(decoded_picture_buffer_size <= annex_a_level_table[level_index][3]) &&*/
			(video_bit_rate <= annex_x_level_table[level_index][0]))
		{
			break;
		}

		level_index++;

		if (level_index >= NUMBER_OF_ANNEX_X_LEVELS) 
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
		(setup_parameters->frame_height != m_PreviousSetupParameters.frame_height)
		)
	{
		if (!IDRChangeSettings(setup_parameters,  level_index))
		{
			m_IncompatibleSettings = true;
			assert(false);
			return false;
		}
	}
	//update ME settings
	if (HwMe == 1)
	{
	    WriteToChannelME_Hw(CHANREG_ME_CONTROL1, (u_int32) (((m_rounding_control & ME_CONTROL1__ROUND_CTRL_MASK) << ME_CONTROL1__ROUND_CTRL_SHIFT) | 
									                     ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT) |		
	                                                     ((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT)) );

	    WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
	                                                  ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	                                                  ((m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
	                                                  ((m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
	                                                  ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	                                                  ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
													  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
													  ((m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
													  ));
	}
	else
	{
	    WriteToChannelME_Sw(CHANREG_ME_CONTROL1, (u_int32) (((m_rounding_control & ME_CONTROL1__ROUND_CTRL_MASK) << ME_CONTROL1__ROUND_CTRL_SHIFT) | 
                ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT) |		
                ((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT)) );

	    WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
             ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
             ((m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
             ((m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
             ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
             ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
			  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
			  ((m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
			  ));
		
	}

	m_PreviousSetupParameters = *setup_parameters;
    m_ThreadInit = true;

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
									/* bool baseline_compatibility_flag,
									 bool main_compatibility_flag,
									 bool extended_compatibility_flag,
									 bool high_compatibility_flag,*/
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
										setup_parameters->frame_height,
										true,
										false))
	{
		Destroy();
		assert(false);
		return false;
	}
	
	// Initialise the block offsets
	unsigned int hsize = ((setup_parameters->frame_width + 15) >> 4) << 4;
	unsigned int qhsize = hsize >> 2;


	
    // Ensure that the sequence parameter set and picture parameter sets are transmitted
	if(m_coding_standard == MPEG4)
	{
		m_TransmitSequenceParameterSet = true;
	}
	else
	{
		m_TransmitSequenceParameterSet = false;
	}

	m_CodeNextPictureAsIDR = true;

	return true;
}

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

	delete m_ForwardWorkingArray;
	m_ForwardWorkingArray = 0;



	m_ThreadInit = 0;


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

	//SymbolEjectDestroy();
	RateControlDestroy();
	extern void aanscaletable_uninit(void);
	aanscaletable_uninit();


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

	m_LastTransmittedQuant = 8;

	m_EncoderMainFrameStart = true;
	m_EncoderMainPictureStart = true;
	m_PictureBitCount = 0;

	m_PreviousFrameAverageQuantiser = 8;  // Make sure there is a valid value for the first picture
	//m_PreviousFrameAverageQuantiserDouble = 8;

	// Lots more work to do here to make it reset correctly?
	//m_h263_use_gob_headers = 0;

    m_StartNewPacket = false;

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
				operate efficiently.
																			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePicture(CBTPicture* picture,
								 h264_picture_type_t picture_type,
								 bool store_for_reference,
								 int number_of_bits_in_buffer,
								 int target_bit_count,
								 h264_encoder_results** encoder_results,
#ifdef ARC_ENCODER_API
					   struct packet_queue_t* opq,
#endif
								 int buffer_length,
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
		m_StoreForReference = store_for_reference;
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

	ClearReturnParameters();

	//	set address of the encoding parameters being returned to the calling function
	*encoder_results = &m_H264EncoderResults;

#ifdef ARC_ENCODER_API
	m_error = 0;
	assert(opq != 0);
	m_output_q = opq;
#endif

	while (true) // A single iteration only - break to exit function
	{
		if (m_TransmitSequenceParameterSet) // only true for MPEG4
		{
			m_CurrentSymbolByteBuffer[0] = m_SymbolByteBufferList[0];

			m_CurrentCopySliceGroup = 0;

            Service_WriteMpeg4VolHeader(0, 0);

			Service_GetChannelResponse();

            m_CurrentSymbolByteBuffer[0]->ResetBuffer();

           	m_TransmitSequenceParameterSet = false;

			m_CurrentSymbolByteBuffer[0] = m_SymbolByteBufferList[0];

			m_CurrentCopySliceGroup = 0;

            Service_WriteMpeg4UserData();

			Service_GetChannelResponse();

            m_CurrentSymbolByteBuffer[0]->ResetBuffer();



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
			finished_encoding_frame = true;
			m_CurrentSymbolByteBuffer[0]->ResetBuffer();
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


		if (!EncodePictureMainPictureEnd())
		{
			// Error condition
			return_value = false;
			assert(false);
			break;
		}

		// Finished encoding frame

		finished_encoding_frame = true;

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

	if (0 != m_SourcePicture)
	{
		m_H264EncoderResults.timestamp = m_SourcePicture->m_ts;
	}

	if (finished_encoding_frame)
	{
		m_EncoderMainFrameStart = true;
		m_EncoderMainPictureStart = true;
		m_PictureBitCount = 0;
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

	m_H264EncoderResults.intra_4x4_count = m_TotalIntra4x4Count;
	m_H264EncoderResults.intra_16x16_count = m_TotalIntra16x16Count;
	m_H264EncoderResults.inter_16x16_count = m_TotalInter16x16Count;
	m_H264EncoderResults.inter_16x8_count = m_TotalInter16x8Count;
	m_H264EncoderResults.inter_8x16_count = m_TotalInter8x16Count;
	m_H264EncoderResults.inter_8x8_count = m_TotalInter8x8Count;
	m_H264EncoderResults.skipped_count = m_TotalSkippedCount;
	m_H264EncoderResults.forced_skip_count = m_TotalForcedSkipCount;
	m_H264EncoderResults.skip_8x8_count = m_TotalSkip8x8Count;
//	m_H264EncoderResults.direct_count = m_TotalDirectCount;
	m_H264EncoderResults.cycles_in_frame  = m_CyclesInFrame;

	if (m_RealTimeRateControl)
	{
		m_H264EncoderResults.buffer_level = 0;
	}
	else
	{
		m_H264EncoderResults.buffer_level = (unsigned int)(m_OverallBufferStatus);
	}

	m_H264EncoderResults.timestamp = source_picture->m_ts;
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

/*
--------------------------------------------------------------------------------
Function GetDebugLevel
--------------------------------------------------------------------------------
*/

int		CH264Encoder::GetDebugLevel(void)
{
	return(m_DebugLevel);
}

/*
--------------------------------------------------------------------------------
Function SetDebugLevel.
--------------------------------------------------------------------------------
*/

int		CH264Encoder::SetDebugLevel(int debug_level)
{
	m_DebugLevel = debug_level;
	return(0);
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePictureMainEncodeMacroblocks			*/
/* DESCRIPTION:	Encodes all the macroblocks, or those for which the calling
				thread is responsible										*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePictureMainEncodeMacroblocks()
{
	int mb_number;
	m_SliceID[0] = 4;
	m_FirstMacroblockInSlice[0]= 0;

#ifdef ENABLE_PERFORMANCE
	Reset_Timer0();
#endif

   // setup ME reference and current locations in main memory
    Service_ME_PictureSetup();

   MgrMPC.Deblock.Init(1, 0);
   ////////////////////////////////////////////////////////////////////////////////////
	// Loop over macroblocks for encoding
	////////////////////////////////////////////////////////////////////////////////////

	MgrMPC.ResetTopLeft();      // Reset macro block pos
	mb_number = 0;


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


	SET_CallBackStartME_BufIdx(0);
	_vrun(MACRO_CallBackStartME);
	do
	{
		(HandleServiceCall)();
	}
	while((MgrMPC.MacroBlocksCompleteCount < m_NumberOfMacroblocksInPicture) || 
        ! (MgrMPC.ActMPC[MgrMPC.IdxSetupME].InvTransformsOk && MgrMPC.ActMPC[MgrMPC.IdxSetupME].EntropyEncodeOk)
        );
	




#ifdef ENABLE_PERFORMANCE
	Timer0_Val_To_Mem();
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


	m_PanicLevel = CLIP(m_MinPanicLevel, NUM_MPEG4_ME_PANIC_SETTINGS-1, m_PanicLevel + panic_level);
	m_Skip8x8ThresholdMult =	me_mp4control_lut[m_PanicLevel][0];
	m_UseInter16x16Only =		me_mp4control_lut[m_PanicLevel][1];
	m_SkipSadThresholdMult =	me_mp4control_lut[m_PanicLevel][2];
	m_StepOneControl =			me_mp4control_lut[m_PanicLevel][3];
	m_SubPelControl =			me_mp4control_lut[m_PanicLevel][4];

	if(!m_use_4MV)m_UseInter16x16Only =1;//for safety with H263
//set these values for the ME until we next visit this function

	if (gCH264Encoder.m_SliceType != H264_I_SLICE)
	{
		if (HwMe == 1)
		{
		    WriteToChannelME_Hw(CHANREG_ME_CONTROL1, (u_int32) (((m_rounding_control & ME_CONTROL1__ROUND_CTRL_MASK) << ME_CONTROL1__ROUND_CTRL_SHIFT) | 
										                     ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT) |		
		                                                     ((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT)) );
		    
			WriteToChannelME_Hw(CHANREG_ME_SETUP, (u_int32) (((m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
		                                                  ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
		                                                  ((m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
		                                                  ((m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
		                                                  ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
		                                                  ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
														  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
														  ((m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
														  ));
		}
		else
		{
		    WriteToChannelME_Sw(CHANREG_ME_CONTROL1, (u_int32) (((m_rounding_control & ME_CONTROL1__ROUND_CTRL_MASK) << ME_CONTROL1__ROUND_CTRL_SHIFT) | 
	                ((m_StepOneControl & ME_CONTROL1__STEP_ONE_CTRL_MASK) << ME_CONTROL1__STEP_ONE_CTRL_SHIFT) |		
	                ((m_SubPelControl & ME_CONTROL1__SUB_PEL_CTRL_MASK) << ME_CONTROL1__SUB_PEL_CTRL_SHIFT)) );
	
		    WriteToChannelME_Sw(CHANREG_ME_SETUP, (u_int32) (((m_UseInter16x16Only & ME_SETUP__ITER_TYPE_MASK) << ME_SETUP__ITER_TYPE_SHIFT) |
	             ((1 & ME_SETUP__BLK_PAT_EN_MASK) << ME_SETUP__BLK_PAT_EN_SHIFT) |
	             ((m_pad_ctrl  & ME_SETUP__PAD_CTRL_MASK)  << ME_SETUP__PAD_CTRL_SHIFT)  |
	             ((m_coding_standard & ME_SETUP__CODEC_MASK) << ME_SETUP__CODEC_SHIFT) |
	             ((1 & ME_SETUP__SYS_DMA_EN_MASK) << ME_SETUP__SYS_DMA_EN_SHIFT) |
	             ((2 & ME_SETUP__SKIP_ADJUST_MASK) << ME_SETUP__SKIP_ADJUST_SHIFT) |
				  ((1 & ME_SETUP__PMV_MASK) << ME_SETUP__PMV_SHIFT) |
				  ((m_umvplus & ME_SETUP__EMV_MASK) << ME_SETUP__EMV_SHIFT)
				  ));
		
		}
	}
}

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
