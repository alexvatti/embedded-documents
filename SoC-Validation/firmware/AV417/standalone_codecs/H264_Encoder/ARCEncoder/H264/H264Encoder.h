/* ------------------------------------------------------------------------ */
/* FILE:			H264Encoder.h											*/
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

#ifndef	H264_ENCODER_H
#define	H264_ENCODER_H

#ifdef __cplusplus
extern "C" 
{
#endif 


#define MAXIMUM_NUMBER_OF_SLICE_GROUPS 8

#include "H264ParameterSets.h"
#include "defines.h"

#ifdef __cplusplus
class CBTPicture;
class CSymbolBuffer;
#endif

/*
--------------------------------------------------------------------------------
Definitions.
--------------------------------------------------------------------------------
*/

#define REAL_TIME_RATE_CONTROL		42
#define NON_REAL_TIME_RATE_CONTROL	43

#define		NO_PIC_SIZE_CHANGE						0
#define		PIC_SIZE_CHANGE							1

#define		MAX_NO_H264_ENCODERS					128

#define		MAX_ERR_STR_LEN							80	

#define		H264_ENCODER_OK							0
#define		H264_ENCODER_ICO_MALLOC_ERROR			-1
#define		H264_ENCODER_VLD_MALLOC_ERROR			-2
#define		H264_ENCODER_SELF_TEST_ERROR			-3
#define		H264_ENCODER_MAX_NO_EXCEEDED			-4
#define		H264_ENCODER_INVALID_HANDLE				-5
#define		H264_ENCODER_STRING_LUT_RANGE_ERROR		-6

#define		H264_ENCODER_MAJOR_REV_NO				1
#define		H264_ENCODER_MINOR_REV_NO				0


#define		H264_ENCODER_VA_STRING_LUT_RANGE_ERROR	3


#define		H264_ENCODER_SF_STRING_LUT_RANGE_ERROR	8

#define		H264_ENCODER_MAX_OUTPUT_PICTURES        16


typedef enum
{
	H264_P_PICTURE,
	H264_B_PICTURE_NotBaseline,
	H264_I_PICTURE,
	H264_SP_PICTURE_NotBaseline,
	H264_SI_PICTURE_NotBaseline
}	h264_picture_type_t;


typedef struct h264_encoder_setup_tag
{
	int frame_width;					// indicates source frame width in pels
	int frame_height;					// indicates source frame height in pels

	bool fixed_quantiser;				// indicates whether a fixed quantiser or rate control is to be used
	int fixed_quantiser_value;			// indicates whether the fixed quantiser value, if rate control is not to be used
	bool code_all_pictures;				// indicates whether all source pictures should be coded, or whether some can be skipped by the rate control for fixed bit rate operation
	int target_bitrate;					// target bit rate bits per second
	int target_picture_period;			// target picture period in 90 KHZ units
	int input_picture_period;			// input picture period in 90 KHZ units
	int intra_refresh_rate;				// The minimum rate of Intra refresh for QCIF in MBs/picture
	int intra_picture_frequency;		// The rate of regular intra pictures (0 = just the first picture is intra, apart
										// from scene changes.  NB This values is constrained to be a multiple of
										// b_picture_ratio+1

	int quality_level;					// target processor clock rate
	int dynamic_control;				// whether to use dynamic control
	bool real_time_rate_control;		// For real-time encoding

	//int pic_order_cnt_type;				// Indicates the picture order count type to use:
										// 0=fixed length codes, flexible but not the most efficient
										// 1=predicted, efficient if a regular coding structure, e.g. BBP, is used
										// 2=implicit, very efficient but can only be used when pictures are coded in output
										//   order and at most one non-reference picture is between each pair of reference pictures

	int disable_deblocking_filter_idc;	// 0=normal, 1=no deblocking, 2=all except slice boundaries
	
	signed char chroma_qp_index_offset;
	bool constrained_intra_pred_flag;

	int maximum_packet_size;			// Measured in bytes; only relevant at present to standard bitstream format
	int reaction_multiplier;
	

	int number_of_slice_groups_for_P_pictures;	// FMO can be used in P pictures for error resilience



	h264_vui_parameters * vui_parameters; // If non-zero, pointer to VUI parameters



} h264_encoder_setup;


typedef struct	h264_encoder_results_tag
{
	int bit_count;
	bool source_picture_finally_encoded;
	bool field_coding;
	int intra_4x4_count;
	int intra_16x16_count;
	int inter_16x16_count;
	int inter_16x8_count;
	int inter_8x16_count;
	int inter_8x8_count;
	int skipped_count;
	int forced_skip_count;
	int skip_8x8_count;
	int max_ISlice_mb_bits;
	int max_PSlice_mb_bits;
	int cycles_in_frame;
	int direct_count;
	int buffer_level;  // Only set for non-real-time encoding
#ifdef STATISTICS
	double average_quant;
#endif // STATISTICS
	int number_of_output_pictures;

	unsigned char  *	pFrameY;
	unsigned char  *	pFrameU;
	unsigned char  *	pFrameV;
	void			  * pEncodedPicture;
	void			  * pSourcePicture;
	void			  * pOutputPicture[H264_ENCODER_MAX_OUTPUT_PICTURES];

}	h264_encoder_results;


#ifdef __cplusplus
}
#endif

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/

