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

#ifndef	H264_PARAMETER_SETS_H
#define	H264_PARAMETER_SETS_H

//#define NUMBER_OF_SEQUENCE_PARAMETER_SETS 32
//#define NUMBER_OF_PICTURE_PARAMETER_SETS 128

///////////////////////////////////////////
// Definition of Sequence Parameter Set
///////////////////////////////////////////

#define MAXIMUM_REF_FRAMES_IN_PIC_CYCLE 256
#define MAXIMUM_NUMBER_OF_SLICE_GROUPS 4 // number of macroblock rows

typedef struct h264_vui_parameters_tag
{
	// The following parameters should be set by the application
	bool aspect_ratio_info_present_flag;
	int aspect_ratio_idc;
	int sar_width;
	int sar_height;

	bool overscan_info_present_flag;
	bool overscan_appropriate_flag;

	bool video_signal_type_present_flag;
	int video_format;
	bool video_full_range_flag;
	bool colour_description_present_flag;
	int colour_primaries;
	int transfer_characteristics;
	int matrix_coefficients;

	bool chroma_loc_info_present_flag;
	int chroma_sample_loc_type_top_field;
	int chroma_sample_loc_type_bottom_field;

	bool timing_info_present_flag;
	unsigned int num_units_in_tick;
	unsigned int time_scale;
	bool fixed_frame_rate_flag;

	// The following parameters are set by the encoder
	bool nal_hrd_parameters_present_flag;
	int nal_hrd_cpb_cnt_minus1;
	int nal_hrd_bit_rate_scale;
	int nal_hrd_cpb_size_scale;
	unsigned int nal_hrd_bit_rate_value_minus1[32];
	unsigned int nal_hrd_cpb_size_value_minus1[32];
	bool nal_hrd_cbr_flag[32];
	int nal_hrd_initial_cpb_removal_delay_length_minus1;
	int nal_hrd_cpb_removal_delay_length_minus1;
	int nal_hrd_dpb_output_delay_length_minus1;
	int nal_hrd_time_offset_length;

	bool vcl_hrd_parameters_present_flag;
	int vcl_hrd_cpb_cnt_minus1;
	int vcl_hrd_bit_rate_scale;
	int vcl_hrd_cpb_size_scale;
	unsigned int vcl_hrd_bit_rate_value_minus1[32];
	unsigned int vcl_hrd_cpb_size_value_minus1[32];
	bool vcl_hrd_cbr_flag[32];
	int vcl_hrd_initial_cpb_removal_delay_length_minus1;
	int vcl_hrd_cpb_removal_delay_length_minus1;
	int vcl_hrd_dpb_output_delay_length_minus1;
	int vcl_hrd_time_offset_length;

	bool low_delay_hrd_flag;

	bool pic_struct_present_flag;

	bool bitstream_restriction_flag;
	bool motion_vectors_over_pic_boundaries_flag;
	unsigned int max_bytes_per_pic_denom;
	unsigned int max_bits_per_mb_denom;
	int log2_max_mv_length_horizontal;
	int log2_max_mv_length_vertical;
	int num_reorder_frames;
	unsigned int max_dec_frame_buffering;

} h264_vui_parameters;

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
