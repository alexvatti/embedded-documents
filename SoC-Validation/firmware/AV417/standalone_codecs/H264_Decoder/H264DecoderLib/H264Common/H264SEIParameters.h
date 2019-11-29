/* ------------------------------------------------------------------------ */
/* FILE:			H264SEIParameters.h										*/
/* DESCRIPTION:		SEI parameter declarations for H.264					*/
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

#ifndef	H264_SEI_PARAMETERS_H
#define	H264_SEI_PARAMETERS_H

///////////////////////////////////////////
// Definition of SEI Parameters
///////////////////////////////////////////

typedef struct h264_sei_parameters_tag
{
	bool buffering_period_sei_present;
	unsigned int seq_parameter_set_id;
	unsigned int nal_cpb_cnt;
	unsigned int nal_initial_cpb_removal_delay[32];
	unsigned int nal_initial_cpb_removal_delay_offset[32];
	unsigned int vcl_cpb_cnt;
	unsigned int vcl_initial_cpb_removal_delay[32];
	unsigned int vcl_initial_cpb_removal_delay_offset[32];

	bool picture_timing_sei_present;
	bool cpb_dpb_delays_present;
	unsigned int cpb_removal_delay;
	unsigned int dpb_output_delay;
	bool pic_struct_present;
	unsigned int pic_struct;
	unsigned int num_clock_TS;
	bool clock_timestamp_flag[3];
	unsigned int ct_type[3];
	bool nuit_field_based_flag[3];
	unsigned int counting_type[3];
	bool full_timestamp_flag[3];
	bool discontinuity_flag[3];
	bool cnt_dropped_flag[3];
	unsigned int n_frames[3];
	unsigned int seconds_value[3];
	unsigned int minutes_value[3];
	unsigned int hours_value[3];
	int time_offset[3];

	bool recovery_point_sei_present;
	unsigned int recovery_frame_cnt;
	bool exact_match_flag;
	bool broken_link_flag;
	unsigned int changing_slice_group_idc;

} h264_sei_parameters;



#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
