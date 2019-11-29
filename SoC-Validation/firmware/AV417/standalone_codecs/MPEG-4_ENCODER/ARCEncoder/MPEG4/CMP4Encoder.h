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


#ifndef	CH264_ENCODER_H
#define	CH264_ENCODER_H

#include	<stdio.h>
#include "picture_encoder.h"
#include "defines.h"

#ifdef ARC_ENCODER_API
#include <packet_queue.h>
#endif

/*
--------------------------------------------------------------------------------
Class declarations.
--------------------------------------------------------------------------------
*/

class CBTPicture;
class CSymbolByteBuffer;
class CH264Encoder;

#ifdef _WIN32
	class CEvent;
	class CWinThread;
#endif

/*
--------------------------------------------------------------------------------
#define definitions.
--------------------------------------------------------------------------------
*/

#define MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK 400
#define MAXIMUM_BITS_FOR_A_CODED_MACROBLOCK 3200

#define NUMBER_OF_LINE_MOTION_VECTORS 3
#define LUMA_WORKING_ARRAY_SIZE 21
/*
--------------------------------------------------------------------------------
Type definitions.
--------------------------------------------------------------------------------
*/

#include "EnumTypes.h"
struct working_array_t
{
//	int data[LUMA_WORKING_ARRAY_SIZE][LUMA_WORKING_ARRAY_SIZE];
	u_int16 data[CHROMA_WORKING_ARRAY_SIZE][CHROMA_WORKING_ARRAY_SIZE];
};

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include "MP4Encoder.h"
#include "MP4EncoderControl.h"
//#include "MP4ParameterSets.h"
#include "MP4EncodedPictureBuffer.h"
#include "defines.h"

#define ARC_MOTION // use 4 step motion search
#define COEFF_ELIM // provides simple coeff-elimination strategy
#define SSD_CHROMA_CHECK // use SSD for chroma veto
#define INTRA_NEIGH_BIAS // bias intra decision depending on neighbours
/*
--------------------------------------------------------------------------------
Class:	CH264Encoder
--------------------------------------------------------------------------------
*/
struct thread_parameters_t
{
	int thread_number;
	CH264Encoder * object;
};
/*
--------------------------------------------------------------------------------
Structure definitions
--------------------------------------------------------------------------------
*/

class CH264Encoder
{
public:
	CH264Encoder(void);
	~CH264Encoder(void);

	bool Create(void);
	void Destroy(void);

	bool ChangeSettings(h264_encoder_setup * setup_parameters);

	int	GetDebugLevel(void);
	int	SetDebugLevel(int debug_level);

	bool EncodePicture(CBTPicture* picture,
					   h264_picture_type_t picture_type,
					   bool store_for_reference,
					   int number_of_bits_in_buffer,
					   int target_bit_count,
					   h264_encoder_results** encoder_results,
#ifdef ARC_ENCODER_API
					   struct packet_queue_t* opq,
#endif
					   int buffer_length,
					   bool& more_to_encode,
					   bool& statistics_valid);

	void h263_encode_init(int *coding_standard);

	bool Reset(void);
/*
	int GetPreviousPictureQuantiser(void) 
	{
		return m_PreviousFrameAverageQuantiser;
	}
	int SetPreviousPictureQuantiser(int quantiser)
	{
		m_PreviousFrameAverageQuantiser = quantiser;
		return 0;
	}*/
		int m_time;
private:
	void SetReturnParameters(CBTPicture * decoded_picture, CBTPicture * source_picture, CBTPicture * output_picture);
	void ClearReturnParameters(void);

	bool IDRChangeSettings(h264_encoder_setup * setup_parameters,
									/* bool baseline_compatibility_flag,
									 bool main_compatibility_flag,
									 bool extended_compatibility_flag,
									 bool high_compatibility_flag,*/
									 int level_index);

	bool SetUpSequenceParameterSet(h264_encoder_setup * setup_parameters,
								   int sequence_parameter_set_id,
								   bool baseline_compatibility_flag,
								   bool main_compatibility_flag,
								   bool extended_compatibility_flag,
								   bool high_compatibility_flag,
								   int level_index,
								   int number_of_reference_pictures);


	//////////////////////
	// Setup parameters
	//////////////////////
public:

	h264_encoder_setup m_PreviousSetupParameters;	// Not initially set, retains a copy of the settings passed to ChangeSettings the previous time

	int m_NumberOfMBs;
	int m_NumMBsRecip;


	//bool m_IntraMacroblocks;					// Indicates whether Intra macroblocks should be present in non-intra pictures

	int m_IntraMBRefreshRate;			// The minimum rate of Intra refresh of macroblocks for QCIF in MBs/picture
	int m_IntraUpdateMBNumber;			// The number of the next macroblock to be intra refreshed
	int	m_IntraUpdateSliceGroupID;		// The slice group of the next macroblock to be intra refreshed

	int m_min_qcoeff;
	int m_max_qcoeff;
	int m_use_4MV;
	int m_h263_use_gob_headers;
	int m_h263_rows_per_gob;
	int m_use_ac_pred;

	int m_resync_mb_x;
	int m_resync_mb_y;

	bool m_CodeNextPictureAsIDR;				// Indicates next picture is to be coded as an IDR picture
	unsigned int m_IDRPictureSourceFN;			// Stores m_SourcePicture->m_fn of the last picture coded as IDR

	unsigned int m_LastTransmittedQuant;
	int	m_MBAdaptFreq;							// frequency of dynamic adaption in MBs
	int m_CycleBudgetMB;						// budgetted cycles per MB
	int m_EndCycles;							// number of cycles between last MB of one frame and first of next
	//////////////////////

	bool m_Created;
	int m_DebugLevel;
	bool m_IncompatibleSettings;				// Set when the settings build of code is not compatible with the user settings

//*****************************************************************************/
// Threading related declarations
//*****************************************************************************/

	bool m_ThreadInit;
	int m_CurrentCopySliceGroup;

	thread_parameters_t m_ThreadParameters;


//****************************************************************************
// Threads declarations for Windows
//****************************************************************************


//*****************************************************************************/
// End of Threading related declarations
//*****************************************************************************/



	h264_encoder_results m_H264EncoderResults;
	bool m_MoreToEncode;  // Set to true when encoding is not yet complete; set to false when picture has been encoded
	bool m_EncoderMainFrameStart;  // Indicates that the start of frame processing for the current frame is not yet done
	bool m_EncoderMainPictureStart;  // Indicates that the start of picture processing for the current picture is not yet done
	bool m_EncoderMainCodeMBs;	   // Indicates that the macroblocks for the current picture have not yet been coded
	

	int m_InputPictureNumber;			// Increments for each picture in input order
	
	// The encoding control list
	CH264EncoderControl * m_EncoderControl;


	// Main encoding member variables


	int m_NumberOfPictureParameterSets;

	int m_NALReferenceIDC;
	h264_NAL_unit_type_t m_NALUnitType;

	unsigned int m_FrameNumber;
	unsigned int m_CurrentFrameNumber;

	unsigned int m_FrameNumberOffset;
	int m_PictureOrderCount;
	int m_TopFieldOrderCount;

	int m_CurrentPictureOrderCount;
	int m_IDRPictureID;


	// Parameters relating to out of order source pictures (sometimes known as B pictures)

	bool m_StoreForReference;

	int * m_SourcePictureStoreCodingOrder;

	int m_IDR_POC_Offset;

	// Main encoding member functions
	void EncoderMainFrameStart(int number_of_bits_in_buffer, bool& more_to_encode, int target_bit_count);
	bool EncoderMainPictureStart(void);
	
	bool EncodePictureMainEncodeMacroblocks();

	bool EncodePictureMainMacroblock();

	bool EncodePictureMainPictureEnd(void);

	void AdjustSettings(int panic_level);

#ifdef ARC_ENCODER_API
    struct packet_queue_t* m_output_q;
    unsigned int m_error;
#endif

	// The following variables are used to create and use an average of the motion vectors of the previous
	// picture.  This is used in motion estimation simply as a candidate starting point for the motion search

public:
	// Reconstructed reference pictures
	CH264EncodedPictureBuffer * m_EncodedPictureBuffer;

    void MotionEstimateCalculateSkippedChromaCost();

	int WorstSkippedChromaCost;

 private:

	int h263_round_chroma(int x);
public:
  // Make Inter Prediction functions
	bool MakeInterChromaPrediction263(short cIdx);
	void ChooseInterOrIntra(int cIdx);
    void ARC_WriteOutMacroblock(CBTPicture* pic, int oIdx);

private:


	// Make Inter Prediction member variables
	working_array_t * m_ForwardWorkingArray;

public:
	void VLCEncodeFixedLengthCode(unsigned int length, unsigned int value);

private:

	bool MakeSkippedChromaPrediction(int cIdx);	// Used just to create a chroma prediction so that (initially at least)
							 
public:
	FILE * m_TraceFile;					// trace file generated if MAKE_SYMEJECT_TRACE_FILE is defined
	CSymbolByteBuffer * m_CurrentSymbolByteBuffer[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int m_FirstMacroblockInSlice[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int m_SliceID[MAXIMUM_NUMBER_OF_SLICE_GROUPS]; // The ID for the current slice in each thread
	int m_ThreadBufferStatus;
	int m_time_increment_resolution;
	int m_time_increment_bits;
	int m_last_time_base;
	int m_time_base;
	int m_frame_width_in_mbs;
	int m_frame_height_in_mbs;
	int m_mpeg_quant;
	int m_rounding_control;
	int m_OverallBufferStatus;
	h264_picture_type_t m_PictureType;
	h264_slice_type_t m_SliceType;
    bool m_StartNewPacket;
	int m_MaximumNumberOfBytes;
	int m_MaximumPacketDataSize;				// Measured in bytes; only relevant at present to standard bitstream format
	int m_h263_slice_structured;
	int m_umvplus;						// extended motion vector range as part of annex D
	int m_video_packets;
	int m_NumberOfMacroblocksInPicture;
	int m_use_hec;
	int m_coding_standard;				// should be MPEG4, H263 or H263_PLUS
	int m_frame_rate;
	int m_frame_rate_base;
	CBTPicture * m_SourcePicture;
	int m_h263_aic;
	int m_h263_loop_filter;
	int m_h263_modified_quant;
	CBTPicture * m_EncodedPicture;

	int m_UseInter16x16Only;			// does what it says
	int m_SkipSadThresholdMult;			// threshold on SAD to avoid doing motion estimation based on skip vector
	int m_Skip8x8ThresholdMult;			// threshold on 16x16 SAD to avoid doing 8x8 search
	int m_StepOneControl;				// variable to control detail in step one of 4 step search
	int m_SubPelControl;				// Variable to control precision of sub-pel search
	int m_PanicLevel;				// panic level used to control motion estimation params
	int m_MinPanicLevel;
	int m_DynamicControl;			// whether to use dynamic control or not values 0 1 or 2
	int m_LastMBCount;					// used to control ME and SIMD
	int m_StartProcessingTimer;			// used to control Channel Timer
	int m_LastHitCount;		// used to check veracity of count hit in channel timer


    int m_pad_ctrl;
    unsigned int m_bestInterCost;
	int m_PictureBitCount;
    unsigned char m_lastMBType;
    int m_time_since_last_intra_refresh;
    
private:
	// Symbol Eject member variables
	FILE * m_RateConTraceFile;			// trace file generated if MAKE_H264_RATECONTROL_TRACE_FILE is defined
	FILE * m_GeneralEncoderTraceFile;	// trace file generated if MAKE_ENCODER_TRACE_FILE is defined



	CSymbolByteBuffer * m_SymbolByteBufferList[MAXIMUM_NUMBER_OF_SLICE_GROUPS];


	bool m_TransmitSequenceParameterSet;
	
	// Rate control member variables
	bool m_FixedQuantiser;				// indicates whether a fixed quantiser or rate control is to be used
	int m_FixedQuantiserValue;			// indicates whether the fixed quantiser value, if rate control is not to be used
	int m_InputPicturePeriod;			// input picture period in 90 KHZ units
	int m_TargetPicturePeriod;			// target picture period in 90 KHZ units
	int m_TargetBitRate;				// target bit rate bits per second
	int m_IntraPictureFrequency;		// The frequency of regular intra pictures.  0 means just the first picture
										// NB value is constrained to a multiple of m_BPictureRatio+1

	bool m_RealTimeRateControl;			// For real-time encoding
	int m_RateControlMacroblockCount;	// Counts macroblocks processed by the rate controller. Needed for MBAFF and multi-threading

	int m_CodeAllPictures;				// indicates whether all source pictures should be coded, or whether some can be skipped by the rate control for fixed bit rate operation

	int m_TargetBitsForThisFrame;
	int m_TargetBitsForThisMB;
	int m_StartOfPictureBufferStatus;
	unsigned int m_LastInputPictureNumber;

	int m_AverageBitsPerMacroblock;  // m_AverageBitsPerFrame / m_NumberOfMBs
	int m_AverageBitsPerFrame;		// Bit Rate / Frame Rate
	int m_AverageBitsPerGOP;			// m_AverageBitsPerFrame * m_TotalNumberPerGOP
	int m_BitsLeftInGOP;				// Bits available for the remaining frames in the GOP
	int m_InitialQuant;

	int m_NumberPPerGOP;				// Number of P frames per GOP
	int m_TotalNumberPerGOP;			// Number of   frames per GOP

	int m_NumberPLeftInGOP;				// Number of P frames left in the GOP
	int m_TotalNumberLeftInGOP;			// Number of   frames left in the GOP

	int m_ReactionParameter; 
	int m_InvReactParX31;
	int m_ReactionMult;	
	int m_IVirtualBuffer;			// Virtual buffer level for Intra frames
	int m_PVirtualBuffer;			// Virtual buffer level for P frames

	// Store the slice type selected at frame start for use by the rate control.
	// Note that when picture level frame-field adaptive encoding is used, the top and bottom
	// fields can be coded with different slice types, hence it is important to remember the
	// type of frame the rate control thinks is being encoded.
	h264_slice_type_t m_RateControlFrameSliceType;

public:
	// Rate control member functions
	bool RateControlInitialisation(void);
	void RateControlDestroy(void);
	bool RateControlCodeNextFrame(unsigned int number_of_bits_in_buffer);
	void RateControlDroppedFrames(unsigned int dropped_frames);
	void RateControlFrameStart(unsigned int target_bit_count);
	void RateControlFrameEnd(void);
	void RateControlMacroblockEnd();
	int  RateControlQuantSelect();
	void MacroblockStatistics(int sIdx);

	void RateControlReduceBufferStatus(int number_of_bits_to_remove) 
	{ 
		m_OverallBufferStatus -= number_of_bits_to_remove; 
		if (m_OverallBufferStatus < 0) 
		{
			m_OverallBufferStatus = 0;
		}
	}

	void RateControlReduceThreadBufferStatus(int number_of_bits_to_remove)
	{
		m_ThreadBufferStatus -= number_of_bits_to_remove; 
		if (m_ThreadBufferStatus < 0) 
		{
			m_ThreadBufferStatus = 0;
		}
	}

private:
   // unsigned char uni_DCtab_lum_len2[512];
    void init_uni_dc_tab2(void);

    DWORD m_MacroblockTime;
	DWORD m_QPelTime;
	// Statistics member variables
	unsigned int m_TotalIntra4x4Count;
	unsigned int m_ThreadIntra4x4Count;
	
	unsigned int m_TotalIntra16x16Count;
	unsigned int m_ThreadIntra16x16Count;

	unsigned int m_TotalInter16x16Count;
	unsigned int m_ThreadInter16x16Count;

	unsigned int m_TotalInter16x8Count;
	unsigned int m_ThreadInter16x8Count;

	unsigned int m_TotalInter8x16Count;
	unsigned int m_ThreadInter8x16Count;

	unsigned int m_TotalInter8x8Count;
	unsigned int m_ThreadInter8x8Count;

	unsigned int m_TotalSkippedCount;
	unsigned int m_ThreadSkippedCount;

	unsigned int m_TotalForcedSkipCount;
	unsigned int m_ThreadForcedSkipCount;

	unsigned int m_TotalSkip8x8Count;
	unsigned int m_ThreadSkip8x8Count;
	//unsigned int m_TotalDirectCount;
	//unsigned int m_ThreadDirectCount;

	unsigned int m_CyclesInFrame;

	unsigned int m_TotalQuantiser;
	unsigned int m_ThreadQuantiser;

	unsigned int m_NumberOfQuantisers;
	unsigned int m_ThreadNumberOfQuantisers;

	unsigned int m_PreviousFrameAverageQuantiser;
	//unsigned int m_PreviousControlAverageQuantiser;
	//double m_PreviousFrameAverageQuantiserDouble;

	// Statistics member functions
	void FrameStartStatistics(void);
	void FrameEndStatistics(void);
};



extern CH264Encoder gCH264Encoder;
extern unsigned int h264_encoder_vector_code_length(int mvd);

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
