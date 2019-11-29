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

/* ------------------------------------------------------------------------ */
/* FILE:			CH264Encoder.h											*/
/* DESCRIPTION:		BT H.264 video encoder header file.						*/
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

/*
--------------------------------------------------------------------------------
#define definitions.
--------------------------------------------------------------------------------
*/

#define MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK 300 // JRMM changed from 400 as we don't support IPCM
//#define MAXIMUM_BITS_FOR_A_CODED_MACROBLOCK 3200

#define NUMBER_OF_LINE_MOTION_VECTORS 3

/*
--------------------------------------------------------------------------------
Type definitions.
--------------------------------------------------------------------------------
*/

#include "EnumTypes.h"

struct working_array_t
{
	u_int16 data[CHROMA_WORKING_ARRAY_SIZE][CHROMA_WORKING_ARRAY_SIZE];
};

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include "H264Encoder.h"
#include "H264EncoderControl.h"
#include "H264ParameterSets.h"
#include "H264EncodedPictureBuffer.h"



/*
--------------------------------------------------------------------------------
Class:	CH264Encoder
--------------------------------------------------------------------------------
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

	bool EncodePicture(CBTPicture* picture,
					   h264_picture_type_t picture_type,
					   int number_of_bits_in_buffer,
					   int target_bit_count,
					   h264_encoder_results** encoder_results,
#ifdef ARC_ENCODER_API
					   struct packet_queue_t* opq,
#endif
					   int& buffer_length,
					   bool& more_to_encode,
					   bool& statistics_valid);



	bool Reset(void);

	void SetReturnParameters(CBTPicture * decoded_picture, CBTPicture * source_picture, CBTPicture * output_picture);
	void ClearReturnParameters(void);

	bool IDRChangeSettings(h264_encoder_setup * setup_parameters,
									 bool baseline_compatibility_flag,
									 bool main_compatibility_flag,
									 bool extended_compatibility_flag,
									 bool high_compatibility_flag,
									 int level_index);

	bool SetUpSequenceParameterSet(h264_encoder_setup * setup_parameters,
								   int sequence_parameter_set_id,
								   bool baseline_compatibility_flag,
								   bool main_compatibility_flag,
								   bool extended_compatibility_flag,
								   bool high_compatibility_flag,
								   int level_index,
								   int number_of_reference_pictures);

	bool SetUpPictureParameterSet(int picture_parameter_set_id,
								  int number_of_reference_pictures,
								  int disable_deblocking_filter_idc,
								  int slice_alpha_c0_offset_div2,
								  int slice_beta_offset_div2,
								  int chroma_qp_index_offset,
								  bool constrained_intra_pred_flag,
								  int number_of_slice_groups_for_P_pictures,
								  bool CABAC);



	//////////////////////
	// Setup parameters
	//////////////////////
public:

	h264_encoder_setup m_PreviousSetupParameters;	// Not initially set, retains a copy of the settings passed to ChangeSettings the previous time

	int m_NumberOfMBs;
	int m_NumMBsRecip;

	//bool m_IntraMacroblocks;					// Indicates whether Intra macroblocks should be present in non-intra pictures

	int m_IntraMBRefreshRate;			// [Not implemented at this time] The minimum rate of Intra refresh of macroblocks for QCIF in MBs/picture
	int m_IntraUpdateMBNumber;			// The number of the next macroblock to be intra refreshed
	int	m_IntraUpdateSliceGroupID;		// The slice group of the next macroblock to be intra refreshed

	// Search type is one of DIAMOND_SEARCH_FULL_PEL, DIAMOND_SEARCH_SUB_PEL, FULL_SEARCH_FULL_PEL and FULL_SEARCH_SUB_PEL,
	// taking the values 2300, 2301, 2302 and 2303
	// These are values are taken from motest.h, but have been given different names, and slightly different meanings here
	// to keep the Eurescom interface the same
	int m_UseAllInterModes;				// flag on whether to use 16x8 and 8x16 modes
	int m_SmallPictures;				// flag if picture is smaller than 2CIF
	int m_UseInter16x16Only;			// does what it says
	bool m_NoIntraInPSlice;				// don't consider intra mode in a P slice
	bool m_UseIntraDCpredOnly;			// only DC prediction considered, should be mutually exclusive with m_UseIntraVertPredOnly 
	bool m_UseIntraVertPredOnly;		// only vertical prediction (to avoid dependency) should be mutually exclusive with m_UseIntraDCpredOnly
	bool m_UseWhitening;				// use whitened (by DC) residual for cost comparison on Intra Modes
	int m_SkipSadThresholdMult;			// threshold on SAD to avoid doing motion estimation based on skip vector
	int m_Skip8x8ThresholdMult;			// threshold on 16x16 SAD to avoid doing 8x8 search
	int m_StepOneControl;				// variable to control detail in step one of 4 step search
	int m_SubPelControl;				// Variable to control precision of sub-pel search
	int m_PanicLevel;				// panic level used to control motion estimation params
	int m_DynamicControl;			// whether to use dynamic control or not
	int m_LastMBCount;					// used to control ME and SIMD
	int m_StartProcessingTimer;			// used to control Channel Timer
	int m_LastHitCount;		// used to check veracity of count hit in channel timer
	bool m_CodeNextPictureAsIDR;				// Indicates next picture is to be coded as an IDR picture


	int m_MaximumPacketDataSize;				// Measured in bytes; only relevant at present to standard bitstream format


	int m_MaximumNumberOfBytes;
	int	m_lastquant;

	int	m_MBAdaptFreq;					// frequency of adaptation using dynamic control
	int m_CycleBudgetMB;				// budgetted number of cycles per macroblock at desired frequency and picture params
	int m_EndCycles;					// number of cycles used in between frame processing times

	//////////////////////

	bool m_Created;
	bool m_IncompatibleSettings;				// Set when the settings build of code is not compatible with the user settings

//*****************************************************************************/
// Threading related declarations
//*****************************************************************************/

	bool m_ThreadInit;
	int m_CurrentCopySliceGroup;



	h264_encoder_results m_H264EncoderResults;
	CBTPicture * m_SourcePicture;
	bool m_MoreToEncode;  // Set to true when encoding is not yet complete; set to false when picture has been encoded

	int m_PictureBitCount;

	int m_InputPictureNumber;			// Increments for each picture in input order

	// The encoding control list
	CH264EncoderControl * m_EncoderControl;

    // Reconstructed reference pictures
	CH264EncodedPictureBuffer * m_EncodedPictureBuffer;

	// Main encoding member variables

	CBTPicture * m_EncodedPicture;

	CH264ParameterSets * m_ParameterSets;

	int m_SequenceParameterSetID;
	int m_PictureParameterSetID;

	int m_NumberOfPictureParameterSets;

	sequence_parameter_set_t * m_SequenceParameterSet;
	picture_parameter_set_t * m_PictureParameterSet;

	unsigned int m_FrameNumber;

	unsigned int m_FrameNumberOffset;
	int m_PictureOrderCount;
	int m_TopFieldOrderCount;
	int m_NumberOfMacroblocksInPicture;

	int m_IDRPictureID;


	int m_NumberOfSliceGroupsForPPictures;

	// Parameters relating to out of order source pictures (sometimes known as B pictures)

	h264_picture_type_t m_PictureType;

	int * m_SourcePictureStoreCodingOrder;

	h264_slice_type_t m_SliceType;

	int m_IDR_POC_Offset;
	int m_PicOrderCountLSB;
	int m_DeltaPicOrderCount;
	int m_RedundantPictureCount;

	int m_SliceNumList0RefPictures;

	int m_DisableDeblockingFilterIDC;
	//int m_SliceBetaOffset;

	int m_SliceGroupChangeCycle;
	int m_SliceGroupChangeCycleCodeLength;

	int m_frame_y_4x4_block_offset[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];  // Offset of top left of 4x4 block relative to top left of first 4x4 block
	int m_frame_uv_4x4_block_offset[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS]; // Offset of top left of 4x4 block relative to top left of first 4x4 block

    // inter cost returned from ME channel response and used in encoder main by intra/inter decision
    unsigned int bestInterCost;

#ifdef ARC_ENCODER_API
    struct packet_queue_t* m_output_q;
    unsigned int m_error;
#endif

private:

	// Main encoding member functions
	void EncoderMainFrameStart(int number_of_bits_in_buffer, bool& more_to_encode, int target_bit_count);
	bool EncoderMainPictureStart(void);
	bool EncodePictureMainAllocateSliceGroups(void);

	bool EncodePictureMainEncodeMacroblocks();

	void EncodePictureMainMacroblock();

	// Will need some work to put this back in Q range is limited so should never happen
//	bool EncodePictureMainMacroblockPCM(h264_encoder_control_t* control);

	bool EncodePictureMainPictureEnd(void);

	// Intra Mode Selection member functions
public:
	void AdjustSettings(int panic_level);
	void ChooseInterOrIntra(int cbIdx);
	void IntraModeSelection(int intraValidFlag);

private:
	void ConsiderIntraChromaMode(int intraValidFlag, bool m_UseIntraDCpredOnly, bool m_UseIntraVertPredOnly);

	int ARCcoeff_elim(int16 *coeffs, int, int);

   void CopyCurrentChromaToSDM (unsigned char * current, unsigned char * sdm, int picWidth, int picHeight, int mbX, int mbY);
   void CopyReferenceChromaToSDM (working_array_t * reference, unsigned char * sdm, int xSize, int ySize);

	// The following variables are used to create and use an average of the motion vectors of the previous
	// picture.  This is used in motion estimation simply as a candidate starting point for the motion search

public:


	int WorstSkippedChromaCost;


    void MotionEstimateCalculateSkippedChromaCost(int sIdx);


public:
  // Make Inter Prediction functions
	bool MakeInterChromaPrediction(int sIdx);

private:

	void GenerateIntraChromaPrediction(int intra_chroma_mode,
									   u_int8 * prediction, bool above_available, bool left_available);


private:

	void GetIntegerPelReferenceBlockFromFrame(PIXEL * ref_picture_data,
				working_array_t * working_array,
				int ref_hor_size, int ref_ver_size,
				int ref_hor_offset, int ref_ver_offset,
				int block_hor_size, int block_ver_size);



   void FilterChromaReferenceBlock(working_array_t * working_array,
											     int fractional_hor_vector,
											     int fractional_ver_vector,
											     int block_hor_size,
											     int block_ver_size);

public:
	// Add and subtract member functions

   bool MakeChromaPredictionInterError();

	void GetIntraLumaChromaPredictionAndError(int sIdx);


	bool ReconstructLuma16X16(int idx);
	bool ReconstructChroma(int idx);
	void ReconstructSkipped(int idx);

	// Symbol Eject member variables
	FILE * m_TraceFile;					// trace file generated if MAKE_SYMEJECT_TRACE_FILE is defined
	FILE * m_RateConTraceFile;			// trace file generated if MAKE_H264_RATECONTROL_TRACE_FILE is defined
	FILE * m_GeneralEncoderTraceFile;	// trace file generated if MAKE_ENCODER_TRACE_FILE is defined

//	int m_NextSlice_ID;	// The next slice ID to be allocated to a slice group

	int m_SliceID[MAXIMUM_NUMBER_OF_SLICE_GROUPS]; // The ID for the current slice in each thread
	int m_FirstMacroblockInSlice[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int m_LastMacroblockInSlice[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	CSymbolByteBuffer * m_SymbolByteBufferList[MAXIMUM_NUMBER_OF_SLICE_GROUPS];

	int m_TotalBuffersToCopy;
	CSymbolByteBuffer * m_NextBufferToCopy;

	unsigned int m_LastTransmittedQuant[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	unsigned int m_SkippedMacroblockCount[MAXIMUM_NUMBER_OF_SLICE_GROUPS];

	CSymbolByteBuffer * m_CurrentSymbolByteBuffer[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int m_CurrentSliceGroup;

	bool m_RetransmitParameterSets;
	bool m_TransmitSequenceParameterSet;
	unsigned char m_NextPictureParameterSetToTransmit;



public:
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
	int m_ThreadBufferStatus;
	int m_OverallBufferStatus;
	//double m_RateControlIntraPictureCorrection;	// Used to adjust picture drop decisions after intra pictures
	int m_AverageBitsPerMacroblock;  // m_AverageBitsPerFrame / m_NumberOfMBs
	int m_AverageBitsPerFrame;		// Bit Rate / Frame Rate
	int m_AverageBitsPerGOP;			// m_AverageBitsPerFrame * m_TotalNumberPerGOP
	int m_BitsLeftInGOP;				// Bits available for the remaining frames in the GOP

	int m_InitialQuant;

	int m_NumberPPerGOP;				// Number of P frames per GOP
	int m_TotalNumberPerGOP;			// Number of   frames per GOP

	int m_NumberPLeftInGOP;				// Number of P frames left in the GOP
	int m_TotalNumberLeftInGOP;			// Number of   frames left in the GOP

	//double m_IComplexity;				// Complexity of I frames
	//double m_PComplexity;				// Complexity of P frames


	//int m_IReactionParameter;		// I Frame Rate control reaction parameter r = 2 * n * bit rate / picture rate
	//int m_PReactionParameter;		// P Frame Rate control reaction parameter r = 2 * n * bit rate / picture rate
	int m_ReactionParameter; 
	int m_InvReactParX31;
	int m_ReactionMult;					// Reaction control multiplier of average frame bits 
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
	unsigned int Calculate16x16SSD(int16 * prediction_error);

private:

	unsigned int CalculateChromaSSD(unsigned char* current, int* previous,
									 unsigned int current_yinc, unsigned int previous_yinc);

	// Statistics member variables
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
	unsigned int m_TotalDirectCount;
	unsigned int m_ThreadDirectCount;

	unsigned int m_CyclesInFrame;

	unsigned int m_TotalQuantiser;
	unsigned int m_ThreadQuantiser;

	unsigned int m_NumberOfQuantisers;
	unsigned int m_ThreadNumberOfQuantisers;
	unsigned int m_MaxPSliceMBBits;
	unsigned int m_MaxISliceMBBits;
	//unsigned int m_bit_cnt;
	unsigned int m_PreviousFrameAverageQuantiser;
	//double m_PreviousFrameAverageQuantiserDouble;

	// Statistics member functions
public:
	void FrameStartStatistics(void);
	void MacroblockStatistics(int sIdx);
	void FrameEndStatistics(void);


	void dump_state();
};



extern CH264Encoder gCH264Encoder;

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
