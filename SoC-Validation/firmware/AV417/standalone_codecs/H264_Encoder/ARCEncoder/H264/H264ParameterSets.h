/* ------------------------------------------------------------------------ */
/* FILE:			H264ParameterSets.h										*/
/* DESCRIPTION:		Parameter Sets declarations for H.264					*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
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

#ifndef	H264_PARAMETER_SETS_H
#define	H264_PARAMETER_SETS_H

#define NUMBER_OF_SEQUENCE_PARAMETER_SETS 32
#define NUMBER_OF_PICTURE_PARAMETER_SETS 128

///////////////////////////////////////////
// Definition of Sequence Parameter Set
///////////////////////////////////////////

#define MAXIMUM_REF_FRAMES_IN_PIC_CYCLE 256
#define MAXIMUM_NUMBER_OF_SLICE_GROUPS 8

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

typedef struct 
{
	int profile_idc;
	int level_idc;
	int level_index;

	bool baseline_compatibility_flag;
	bool main_compatibility_flag;
	bool extended_compatibility_flag;
//	bool high_compatibility_flag;

	int chroma_format_idc;
	int bit_depth_luma_minus_8;
	int bit_depth_chroma_minus_8;
	bool qpprime_y_zero_transform_bypass_flag;
	bool seq_scaling_matrix_present_flag;

	unsigned int maximum_frame_number;
	int log2_max_frame_num_minus4;

	int pic_order_cnt_type;
	int max_pic_order_cnt_lsb;
	int log2_max_pic_order_cnt_lsb_minus4;
	bool delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int num_ref_frames_in_pic_order_cnt_cycle;
	int offset_for_ref_frame[MAXIMUM_REF_FRAMES_IN_PIC_CYCLE];
	int expected_delta_per_pic_order_cnt_cycle;

	int num_ref_frames;
	bool required_frame_num_update_behaviour_flag;
	int picture_height_in_map_units;
	int frame_width_in_mbs;
	int frame_height_in_mbs;

	bool frame_mbs_only_flag;
	bool mb_adaptive_frame_field_flag;
	bool direct_8x8_inference_flag;

	bool frame_cropping_flag;
	int frame_cropping_rect_left_offset;
	int frame_cropping_rect_right_offset;
	int frame_cropping_rect_top_offset;
	int frame_cropping_rect_bottom_offset;

	bool vui_seq_parameters_flag;
	h264_vui_parameters * vui_parameters; // If non-zero, pointer to VUI parameters

} sequence_parameter_set_t;

///////////////////////////////////////////
// Definition of Picture Parameter Set
///////////////////////////////////////////
//	bool direct_temporal_constrained_flag;

typedef struct 
{
	int sequence_parameter_set_id;

	bool CABAC_entropy_coding_mode;
	bool pic_order_present_flag;

	int num_slice_groups;
	int slice_group_map_type;
	int slice_group_run_length[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int slice_group_top_left[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int slice_group_bottom_right[MAXIMUM_NUMBER_OF_SLICE_GROUPS];
	int slice_group_change_direction_flag;
	int slice_group_change_rate;
	int slice_group_pic_size_in_map_units;
	signed char * slice_group_id;

	int num_ref_idx_l0_active;
	int num_ref_idx_l1_active;

	bool weighted_pred_flag;
	int weighted_bipred_idc;

	int pic_init_qp;
	int pic_init_qs;

	int chroma_qp_index_offset;
	bool deblocking_filter_parameters_present_flag;
	bool constrained_intra_pred_flag;
	bool redundant_pic_cnt_present_flag;
	bool transform_8x8_modeflag;
	bool pic_scaling_matrix_present_flag;
	int second_chroma_qp_index_offset;

} picture_parameter_set_t;

///////////////////////////////////////////
// Definition of CH264ParameterSets Class
///////////////////////////////////////////

#ifdef __cplusplus

class CH264ParameterSets
{
public:
	CH264ParameterSets(void);
	~CH264ParameterSets(void);

	sequence_parameter_set_t * WriteSequenceParameterSet(int parameter_set_id);
	picture_parameter_set_t * WritePictureParameterSet(int parameter_set_id);

	sequence_parameter_set_t * ReadSequenceParameterSet(int parameter_set_id);
	picture_parameter_set_t * ReadPictureParameterSet(int parameter_set_id);

	void DeleteSequenceParameterSet(int parameter_set_id);
	void DeletePictureParameterSet(int parameter_set_id);

private:
	sequence_parameter_set_t * m_SequenceParameterSets[NUMBER_OF_SEQUENCE_PARAMETER_SETS];
	picture_parameter_set_t * m_PictureParameterSets[NUMBER_OF_PICTURE_PARAMETER_SETS];
};

#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
