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

#ifndef	H264_ENCODER_H
#define	H264_ENCODER_H

#ifdef __cplusplus
extern "C" 
{
#endif 

/*
--------------------------------------------------------------------------------
Definition for exported functions.
--------------------------------------------------------------------------------
*/
#if defined D_UNIX || defined __ARC_
//#include	<redefs.h>
#define LPSTR		char *
#define FAR
typedef signed long DWORD;
#endif

// EXPORTED removed to start compatablity with channel calling
#ifdef WIN32
#include	<windows.h>
#endif // WIN32

//#define MAXIMUM_NUMBER_OF_SLICE_GROUPS 576>>4 // number of macroblock rows

#include "MP4ParameterSets.h"
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
	int coding_standard;
	int use_annex_D;
	int use_annex_J;
	int use_annex_K;
	int use_annex_I;
	int use_annex_T;
	int use_4MVs;
	int use_video_packets;
	int use_hec;
	int use_gob_headers;
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


	int quality_level;					// target processor clock rate will be based on this
	int dynamic_control;				// whether to use dynamic control
	bool real_time_rate_control;		// For real-time encoding


	int maximum_packet_size;			// Measured in bytes; only relevant at present to standard bitstream format
	int reaction_multiplier;
	

	
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
	int cycles_in_frame;
	int direct_count;
	int buffer_level;  // Only set for non-real-time encoding

	unsigned int timestamp;
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

