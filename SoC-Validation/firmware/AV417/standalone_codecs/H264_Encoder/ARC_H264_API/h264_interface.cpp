/*                                                       -*-C++-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2006-2007 ARC International (Unpublished)             *
 * All Rights Reserved.                                            *
 *                                                                 *
 * This document, material and/or software contains confidential   *
 * and proprietary information of ARC International and is         *
 * protected by copyright, trade secret and other state, federal,  *
 * and international laws, and may be embodied in patents issued   *
 * or pending.  Its receipt or possession does not convey any      *
 * rights to use, reproduce, disclose its contents, or to          *
 * manufacture, or sell anything it may describe.  Reverse         *
 * engineering is prohibited, and reproduction, disclosure or use  *
 * without specific written authorization of ARC International is  *
 * strictly forbidden.  ARC and the ARC logotype are trademarks of *
 * ARC International.                                              *
 *                                                                 *
 *                                                                 *
 * h264_interface.cpp                                              *
 *                                                                 *
 * ARC H264 Encoder API implementation.                            *
 *                                                                 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <h264_interface.h>
#include <packet_queue.h>
#include <arc_encoder_malloc.h>

#include <BTPicture.h>
#include <ArcFrameBuffers.h>
#include <typdef.h>
#include <ArcMPC.h>
#include <ArcSDMTables.h>
#include <CH264Encoder.h>

//#ifndef MODEL_PC_VERSION
#include <ctype.h>
#include <arc/arc_simd.h>
//#endif // __ARC_
//#define _VERBOSE_

#include "VRaptorInfo.h"
#include "defines.h"
#ifdef HARDWARE_SIMD
#include "ArcSDMTables_MPA.h"
#include "ArcDeblock_MPA.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcTransformQuant_MPA.h"
#include "ArcIntraLumaPrediction_MPA.h"
#include "ArcIntraLumaPredictionFast_MPA.h"
#include "ArcIntraChromaPrediction_MPA.h"
#include "ArcChromaFilter_MPA.h"
#include "ArcEncoderAddSubtract_MPA.h"
#include "H264MotionEstimation_MPA.h"
#include "ArcChannel_MPA.h"
#include "ArcSoftwareME_MPA.h"
#include "ArcSoftwareSubME_MPA.h"
#include "ArcSoftwareQPelME_MPA.h"
#include "ArcLumaH264SubPelFilter_MPA.h"
#include "H264LoopFilter.h"
#endif

#include "ArcChannelRoutines.h"

/*----------------------------------------------------------------------------------*/

static struct packet_queue_t opq;

static u32 encHandle = 0;
static u16 instances = 0;
static bool inited = false;
static StreamDescriptor* strDesc = 0;
static bool collectingStats = false;
static bool outputDecoded = false;
static bool verbose = false;
static u32 packet_count = 0;
static int frame_skip;
static int skipped;

static int btHandle;
static H264SetupParams* setup_parameters = 0;
static h264_encoder_setup* encoder_setup = 0;

#define TO_BOOL(x) (x != 0 ? true : false)

#define STATS_BUF_LEN 320
#define MAX_RTP_PACKET_SIZE 100000

static int maximum_packet_size = MAX_RTP_PACKET_SIZE;

extern ArcFrameBuffers gArcFrameBuffers;

/*----------------------------------------------------------------------------------*/

static void print_encoder_setup(h264_encoder_setup* setup)
{
	API_LOG_STDERR("frame_width %d\n",setup->frame_width);
	API_LOG_STDERR("frame_height %d\n",setup->frame_height);
	API_LOG_STDERR("fixed_quantiser %d\n",(int) setup->fixed_quantiser);
	API_LOG_STDERR("fixed_quantiser_value %d\n",setup->fixed_quantiser_value);
	API_LOG_STDERR("code_all_pictures %d\n",(int) setup->code_all_pictures);
	API_LOG_STDERR("target_bitrate %d\n",setup->target_bitrate);
	API_LOG_STDERR("target_picture_period %d\n",setup->target_picture_period);
	API_LOG_STDERR("input_picture_period %d\n",setup->input_picture_period);
	API_LOG_STDERR("intra_refresh_rate %d\n",setup->intra_refresh_rate);
	API_LOG_STDERR("intra_picture_frequency %d\n",setup->intra_picture_frequency);
	API_LOG_STDERR("real_time_rate_control %d\n",(int) setup->real_time_rate_control);
	API_LOG_STDERR("disable_deblocking_filter_idc %d\n",setup->disable_deblocking_filter_idc);
	API_LOG_STDERR("chroma_qp_index_offset %d\n",(int) setup->chroma_qp_index_offset);
	API_LOG_STDERR("constrained_intra_pred_flag %d\n",(int) setup->constrained_intra_pred_flag);
	API_LOG_STDERR("maximum_packet_size %d\n",setup->maximum_packet_size);
	API_LOG_STDERR("number_of_slice_groups_for_P_pictures %d\n",setup->number_of_slice_groups_for_P_pictures);
	API_LOG_STDERR("reaction_multiplier %d\n",setup->reaction_multiplier);
	API_LOG_STDERR("quality_level %d\n",setup->quality_level);
	API_LOG_STDERR("dynamic_control %d\n",setup->dynamic_control);

	if (setup->vui_parameters)
	{
	    API_LOG_STDERR("vui_parameters->aspect_ratio_info_present_flag %d\n", (int) setup->vui_parameters->aspect_ratio_info_present_flag);
	    API_LOG_STDERR("vui_parameters->aspect_ratio_idc %d\n", setup->vui_parameters->aspect_ratio_idc);
	    API_LOG_STDERR("vui_parameters->sar_width %d\n", setup->vui_parameters->sar_width);
	    API_LOG_STDERR("vui_parameters->sar_height %d\n", setup->vui_parameters->sar_height);

	    API_LOG_STDERR("vui_parameters->overscan_info_present_flag %d\n", (int) setup->vui_parameters->overscan_info_present_flag);
	    API_LOG_STDERR("vui_parameters->overscan_appropriate_flag %d\n", (int) setup->vui_parameters->overscan_appropriate_flag);

	    API_LOG_STDERR("vui_parameters->video_signal_type_present_flag %d\n", (int) setup->vui_parameters->video_signal_type_present_flag);
	    API_LOG_STDERR("vui_parameters->video_format %d\n", setup->vui_parameters->video_format);
	    API_LOG_STDERR("vui_parameters->video_full_range_flag %d\n", (int) setup->vui_parameters->video_full_range_flag);
	    API_LOG_STDERR("vui_parameters->colour_description_present_flag %d\n", (int) setup->vui_parameters->colour_description_present_flag);
	    API_LOG_STDERR("vui_parameters->colour_primaries %d\n", setup->vui_parameters->colour_primaries);
	    API_LOG_STDERR("vui_parameters->transfer_characteristics %d\n", setup->vui_parameters->transfer_characteristics);
	    API_LOG_STDERR("vui_parameters->matrix_coefficients %d\n", setup->vui_parameters->matrix_coefficients);

	    API_LOG_STDERR("vui_parameters->chroma_loc_info_present_flag %d\n", (int) setup->vui_parameters->chroma_loc_info_present_flag);
	    API_LOG_STDERR("vui_parameters->chroma_sample_loc_type_top_field %d\n", setup->vui_parameters->chroma_sample_loc_type_top_field);
	    API_LOG_STDERR("vui_parameters->chroma_sample_loc_type_bottom_field %d\n", setup->vui_parameters->chroma_sample_loc_type_bottom_field);

	    API_LOG_STDERR("vui_parameters->timing_info_present_flag %d\n", (int) setup->vui_parameters->timing_info_present_flag);
	    API_LOG_STDERR("vui_parameters->num_units_in_tick %d\n", setup->vui_parameters->num_units_in_tick);
	    API_LOG_STDERR("vui_parameters->time_scale %d\n", setup->vui_parameters->time_scale);
	    API_LOG_STDERR("vui_parameters->fixed_frame_rate_flag %d\n", (int) setup->vui_parameters->fixed_frame_rate_flag);

	    API_LOG_STDERR("vui_parameters->nal_hrd_parameters_present_flag %d\n", (int) setup->vui_parameters->nal_hrd_parameters_present_flag);
	    API_LOG_STDERR("vui_parameters->nal_hrd_cpb_cnt_minus1 %d\n", setup->vui_parameters->nal_hrd_cpb_cnt_minus1);
	    API_LOG_STDERR("vui_parameters->nal_hrd_bit_rate_scale %d\n", setup->vui_parameters->nal_hrd_bit_rate_scale);
	    API_LOG_STDERR("vui_parameters->nal_hrd_cpb_size_scale %d\n", setup->vui_parameters->nal_hrd_cpb_size_scale);
	    API_LOG_STDERR("vui_parameters->nal_hrd_initial_cpb_removal_delay_length_minus1 %d\n", setup->vui_parameters->nal_hrd_initial_cpb_removal_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->nal_hrd_cpb_removal_delay_length_minus1 %d\n", setup->vui_parameters->nal_hrd_cpb_removal_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->nal_hrd_dpb_output_delay_length_minus1 %d\n", setup->vui_parameters->nal_hrd_dpb_output_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->nal_hrd_time_offset_length %d\n", setup->vui_parameters->nal_hrd_time_offset_length);

	    API_LOG_STDERR("vui_parameters->vcl_hrd_parameters_present_flag %d\n", (int) setup->vui_parameters->vcl_hrd_parameters_present_flag);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_cpb_cnt_minus1 %d\n", setup->vui_parameters->vcl_hrd_cpb_cnt_minus1);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_bit_rate_scale %d\n", setup->vui_parameters->vcl_hrd_bit_rate_scale);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_cpb_size_scale %d\n", setup->vui_parameters->vcl_hrd_cpb_size_scale);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_initial_cpb_removal_delay_length_minus1 %d\n", setup->vui_parameters->vcl_hrd_initial_cpb_removal_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_cpb_removal_delay_length_minus1 %d\n", setup->vui_parameters->vcl_hrd_cpb_removal_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_dpb_output_delay_length_minus1 %d\n", setup->vui_parameters->vcl_hrd_dpb_output_delay_length_minus1);
	    API_LOG_STDERR("vui_parameters->vcl_hrd_time_offset_length %d\n", setup->vui_parameters->vcl_hrd_time_offset_length);

	    API_LOG_STDERR("vui_parameters->low_delay_hrd_flag %d\n", (int) setup->vui_parameters->low_delay_hrd_flag);
	    API_LOG_STDERR("vui_parameters->pic_struct_present_flag %d\n", (int) setup->vui_parameters->pic_struct_present_flag);

	    API_LOG_STDERR("vui_parameters->bitstream_restriction_flag %d\n", (int) setup->vui_parameters->bitstream_restriction_flag);
	    API_LOG_STDERR("vui_parameters->motion_vectors_over_pic_boundaries_flag %d\n", (int) setup->vui_parameters->motion_vectors_over_pic_boundaries_flag);
	    API_LOG_STDERR("vui_parameters->max_bytes_per_pic_denom %d\n", setup->vui_parameters->max_bytes_per_pic_denom);
	    API_LOG_STDERR("vui_parameters->max_bits_per_mb_denom %d\n", setup->vui_parameters->max_bits_per_mb_denom);
	    API_LOG_STDERR("vui_parameters->log2_max_mv_length_horizontal %d\n", setup->vui_parameters->log2_max_mv_length_horizontal);
	    API_LOG_STDERR("vui_parameters->log2_max_mv_length_vertical %d\n", setup->vui_parameters->log2_max_mv_length_vertical);
	    API_LOG_STDERR("vui_parameters->num_reorder_frames %d\n", setup->vui_parameters->num_reorder_frames);
	    API_LOG_STDERR("vui_parameters->max_dec_frame_buffering %d\n", setup->vui_parameters->max_dec_frame_buffering);
	}
}

/* Convert API setup params structure into encoder structure */

static h264_encoder_setup* convert_to_native(H264SetupParams* inParams)
{
  h264_encoder_setup* outSettings = NULL;

  if ((inParams != 0) && ((outSettings = new h264_encoder_setup) != 0))
  {
    memset(outSettings,0,sizeof(h264_encoder_setup));

    // defaults
    if (inParams->reaction_multiplier == 0)
      inParams->reaction_multiplier = 30;
    if (inParams->maximum_packet_size == 0)
      inParams->maximum_packet_size = MAX_RTP_PACKET_SIZE;

    outSettings->frame_width = inParams->frame_width;
    outSettings->frame_height = inParams->frame_height;
    outSettings->fixed_quantiser = TO_BOOL(inParams->fixed_quantiser);
    outSettings->fixed_quantiser_value = inParams->fixed_quantiser_value;
    outSettings->code_all_pictures = TO_BOOL(inParams->code_all_pictures);
    outSettings->target_bitrate = inParams->target_bitrate;
    outSettings->target_picture_period = inParams->target_picture_period;
    outSettings->input_picture_period = inParams->input_picture_period;
    outSettings->intra_refresh_rate = inParams->intra_refresh_rate;
    outSettings->intra_picture_frequency = inParams->intra_picture_frequency;
    outSettings->real_time_rate_control = TO_BOOL(inParams->real_time_rate_control);
    outSettings->chroma_qp_index_offset = inParams->chroma_qp_index_offset;
    outSettings->constrained_intra_pred_flag = TO_BOOL(inParams->constrained_intra_pred_flg);
    outSettings->number_of_slice_groups_for_P_pictures = inParams->num_slice_grps_P_pictures;
    outSettings->disable_deblocking_filter_idc = inParams->disable_deblocking_filter_idc;
    maximum_packet_size = outSettings->maximum_packet_size = inParams->maximum_packet_size;
    outSettings->reaction_multiplier = inParams->reaction_multiplier;
    outSettings->quality_level = inParams->quality_level;
    outSettings->dynamic_control = inParams->dynamic_control;

#ifndef NO_VUI
    if ((outSettings->vui_parameters = (h264_vui_parameters*) malloc(sizeof (h264_vui_parameters))) != 0)
    {
      if (inParams->vui_parameters)
        memcpy(outSettings->vui_parameters,inParams->vui_parameters,sizeof(h264_vui_parameters));
      else
      {
        outSettings->vui_parameters->nal_hrd_parameters_present_flag = false;  // Parameters not set
        outSettings->vui_parameters->vcl_hrd_parameters_present_flag = false;  // Parameters not set
      }
    }

    outSettings->vui_parameters->low_delay_hrd_flag = !outSettings->code_all_pictures;
    outSettings->vui_parameters->fixed_frame_rate_flag = outSettings->code_all_pictures;
    outSettings->vui_parameters->pic_struct_present_flag = false;
#endif

  }

  return outSettings;
}

/*----------------------------------------------------------------------------------*/

static encoder_interface* h264_create_encoder(u32 encoder_type,
					      u32 handle)
{
	encoder_interface* newif = NULL;
	if ((encoder_type == ENC_TYPE_H264) && (encHandle == 0))
	{
		if ((newif = (encoder_interface*) malloc(sizeof(encoder_interface))) != NULL)
		{
			newif->init = h264_enc_init;
			newif->uninit = h264_enc_uninit;
			newif->encode = h264_enc_encode;
			newif->control = h264_enc_control;
			newif->handle = handle;
			encHandle = handle; // One instance only right now
		}
	}

	return newif;
}

/*----------------------------------------------------------------------------------*/

static encoder_basic_context* h264_create_context(u32 encoder_type, u32 handle)
{
    encoder_basic_context* context = 0;

    if (encoder_type == ENC_TYPE_H264)
    {
	if ((context = malloc(sizeof(encoder_basic_context))) != 0)
	{
	    context->handle = handle;
	    context->expif = h264_create_encoder(ENC_TYPE_H264,handle);
	    context->impif = get_core_import_interface();
	}
    }

    return context;
}

/*----------------------------------------------------------------------------------*/

static s32 h264_release_encoder(u32 encoder_type,
			   u32 handle)
{
	if ((encoder_type == ENC_TYPE_H264) && (encHandle == handle))
	{
		encHandle = 0;
		return ENC_ERR_NONE;
	}
	return ENC_ERR_NOTINIT;
}

/*----------------------------------------------------------------------------------*/

static s32 change_settings(EncArgument* extraData)
{
    H264SetupParams* inParams;

    if ((! extraData) || (! CHECK_ARGTYPE(extraData, ENC_ARGTYPE_H264SETUP)))
	return ENC_ERR_BADARG;

    CAST_ENCARGUMENT(inParams, extraData, H264SetupParams*);
    if ((encoder_setup = convert_to_native(inParams)) == NULL)
	return ENC_ERR_OOM;

    if (!encoder_setup->fixed_quantiser)
	encoder_setup->fixed_quantiser_value = 0;

    if(!gArcFrameBuffers.SetSize(inParams->frame_width, inParams->frame_height))
    {
	gArcFrameBuffers.DeleteInternalPictureBuffs();
	return ENC_ERR_OOM;
    }

    if (!gCH264Encoder.ChangeSettings(encoder_setup))
    {
	return ENC_ERR_FAILED;
    }

    frame_skip = (inParams->target_picture_period/inParams->input_picture_period) - 1;
    if (frame_skip < 0) frame_skip = 0;
    skipped = frame_skip; // always code first frame

    if (verbose) print_encoder_setup(encoder_setup);
    setup_parameters = inParams;

    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static const char stats_header[] =
	"\n Frm                                          Inter  Intra  Inter  Inter  Inter  Inter         Force  Average\n Num FC    Y_SNR    U_SNR    V_SNR   Bit cnt    4x4  16x16  16x16   16x8   8x16    8x8  skppd  skppd  quant\n\n";

static s32 get_stats(H264EncodingParams* encParams, EncPacket* packet)
{
#ifdef STATISTICS
  double y_snr, u_snr, v_snr;
  CPicture * encoded_picture_ptr = (CPicture *)(encParams->results->pEncodedPicture);
  CPicture * source_picture_ptr = (CPicture *)(encParams->results->pSourcePicture);

  if (encoded_picture_ptr && source_picture_ptr)
  {
    encoded_picture_ptr->yuv_snr(*source_picture_ptr, y_snr, u_snr, v_snr);

    memset(packet->stats_buf,0,packet->stats_buf_len);
    sprintf(packet->stats_buf,/*packet->stats_buf_len,*/"%4d, %1d, %7.2f, %7.2f, %7.2f, %8d, %5d, %5d, %5d, %5d, %5d, %5d, %5d, %5d, %7.4f",
	    source_picture_ptr->m_fn, encParams->results->field_coding,
	   y_snr, u_snr, v_snr, encParams->results->bit_count,
	   encParams->results->intra_4x4_count, encParams->results->intra_16x16_count,
	   encParams->results->inter_16x16_count, encParams->results->inter_16x8_count,
	   encParams->results->inter_8x16_count, encParams->results->inter_8x8_count,
	   encParams->results->skipped_count, encParams->results->forced_skip_count,
	   encParams->results->average_quant);

    encParams->cumulative_stats->num_pics_encoded++;
    encParams->cumulative_stats->total_bits += encParams->results->bit_count + 32;
    encParams->cumulative_stats->y_snr += y_snr;
    encParams->cumulative_stats->u_snr += u_snr;
    encParams->cumulative_stats->v_snr += v_snr;
    encParams->cumulative_stats->num_MB_force_skipped += encParams->results->forced_skip_count;
    encParams->cumulative_stats->num_8x8_skipped += encParams->results->skip_8x8_count;
  }
#endif

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static s32 generate_cumulative_stats(EncArgument* extraData)
{
#ifdef STATISTICS
	H264EncodingParams* encParams;
	H264EncoderStats* cumulative_stats;

	if ((! extraData) || (! CHECK_ARGTYPE(extraData, ENC_ARGTYPE_H264ENCPARAMS)))
		    return ENC_ERR_BADARG;

	CAST_ENCARGUMENT(encParams, extraData, H264EncodingParams*);

	if (encParams->cumulative_stats->stats_buf)
	    free(encParams->cumulative_stats->stats_buf);
	encParams->cumulative_stats->stats_buf = (char*) malloc(STATS_BUF_LEN);
	encParams->cumulative_stats->stats_buf_len = STATS_BUF_LEN;
	cumulative_stats = encParams->cumulative_stats;

	sprintf(cumulative_stats->stats_buf,
			"Finished: Elapsed time %d,  /*File reading time %d,*/ Frames per second  %7.4f\n",
			cumulative_stats->tot_elapsed_ticks, /*file_reading_time,*/ (double)(1000*cumulative_stats->num_pics_encoded)/(cumulative_stats->tot_elapsed_ticks));
	sprintf(encParams->cumulative_stats->stats_buf,
			"%d Pictures Encoded, Bit rate (at %5.2fHz) %7.4f\n",
			cumulative_stats->num_pics_encoded, (double)90000.0/(double)setup_parameters->target_picture_period,
			(double)(cumulative_stats->total_bits) * 90000.0/(double)setup_parameters->target_picture_period/(double)(cumulative_stats->num_pics_encoded));
	sprintf(encParams->cumulative_stats->stats_buf,
			"Bits/Picture %d,  AVG-Y-SNR=%7.4f, AVG-U-SNR=%7.4f, AVG-V-SNR=%7.4f,  %d MBs force skipped, %d skipped_8x8\n",
			cumulative_stats->total_bits/cumulative_stats->num_pics_encoded,
			cumulative_stats->y_snr/cumulative_stats->num_pics_encoded,
			cumulative_stats->u_snr/cumulative_stats->num_pics_encoded,
			cumulative_stats->v_snr/cumulative_stats->num_pics_encoded,
			cumulative_stats->num_MB_force_skipped,
			cumulative_stats->num_8x8_skipped);

#endif
  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Init */
s32 h264_enc_init     (u32 handle,
		       u32 codingType,
		       StreamDescriptor* streamInfo,
		       EncArgument* extraData)
{
  s32 tmpres;
  int recAddr;
  int scq0;

  if (inited)
    return ENC_ERR_NOMOREINSTANCES;

  if (codingType != ENC_TYPE_H264)
    return ENC_ERR_CODINGTYPE;

  packet_count = 0;

  // Find vraptor devices
  if (VRaptorDevs.Init() == false) return ENC_ERR_FAILED;
  // Init SDM local memory
  SDM_WorkArea::Init_SDM_WorkArea();

#ifdef HARDWARE_SIMD
// Record Media processor SIMD macros
  recAddr = 0;

  recAddr = SDM_Buffers_Record(recAddr,0);
  recAddr = Transform_Buffers_Record(recAddr,0);
  recAddr = EncoderMain_Record(recAddr,0);
  recAddr = IntraLumaPrediction_Record(recAddr,0);
  recAddr = IntraLumaPredictionFast_Record(recAddr, 0);
  recAddr = IntraChromaPrediction_Record(recAddr,0);
  recAddr = FilterChroma_Record(recAddr,0);
  recAddr = EncoderAddSubtract_Record(recAddr,0);
  recAddr = H264MotionEstimation_Record(recAddr,0);

  if (!HwMe)
  {
      recAddr = ArcChannel_Record(recAddr, 0);
      recAddr = Software_ME_Record(recAddr,0);
      recAddr = Software_SubME_Record(recAddr,0);
      recAddr = Software_QPelME_Record(recAddr,0);
      recAddr = LumaSubPelFilter_Record(recAddr,0);
  }

  SetSCQ(SCQ1);

  if(SCQ1)
  {
      ENC_PRINTF(("SIMD0 SCM size required = 0x%x instructions (%dKBytes)\n", recAddr, ((recAddr*7)+1023)/1024));
      recAddr = 0;
  }

  recAddr = Deblock_Record(recAddr,SCQ1);

  ENC_PRINTF(("SIMD%d SCM size required = 0x%x instructions (%dKBytes)\n", SCQ1, recAddr, ((recAddr*7)+1023)/1024));

  scq0 = 0;
  SetSCQ(scq0);
#endif

  // Select concat unit as the packer
  Initialise_Entropy_Encoder(4);

  strDesc = streamInfo;

#ifdef HARDWARE_EE
  MgrMPC.CurrentEEMb.Init();

  // setup me
  Service_ME_Initialise();
  Service_ME_SetBranchAddresses();
#endif

  if ((tmpres = change_settings(extraData)) != ENC_ERR_NONE)
    return tmpres;

  encHandle = handle;
  inited = true;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Uninit */
s32 h264_enc_uninit   (u32 handle)
{
  if (inited == false)
    return ENC_ERR_NOTINIT;
  if (handle != encHandle)
    return ENC_ERR_BADHANDLE;

  //  f_h264_encoder_destruct(btHandle); // FIXME - does error here leave i/f inited?
  gArcFrameBuffers.DeleteInternalPictureBuffs();

  btHandle = 0;
  encHandle = 0;
  inited = false;
  if (encoder_setup) free(encoder_setup);

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Encode */
s32 h264_enc_encode   (u32 handle,
		       RawFrame** inputData,
		       u32* numInFrames,
		       EncPacket** outputDest,
		       u32* numOutPackets,
		       u32* status,
		       EncArgument* extraData)
{
  bool tmpres = false;
  H264EncodingParams* encParams;
  CBTPicture picture;
  int i;
  int bitsUsed;
  u32 start_ticks, end_ticks;
  bool more_to_encode;
  h264_picture_type_t picture_type;
  bool statistics_valid;
  u32 limit, processed;
  s32 result;
  int first_packet;

  // Has no effect unless CACHE_CONTROL is #define'd
  flush_dcache();

  picture.m_hsize = strDesc->width;
  picture.m_vsize = strDesc->height;

  if (inited == false)
    return ENC_ERR_NOTINIT;
  if (handle != encHandle)
    return ENC_ERR_BADHANDLE;

  if ((! extraData) || (! CHECK_ARGTYPE(extraData, ENC_ARGTYPE_H264ENCPARAMS)))
    return ENC_ERR_BADARG;

  CAST_ENCARGUMENT(encParams, extraData, H264EncodingParams*);
  limit = *numOutPackets;
  *numOutPackets = 0;

  encParams->results = 0;
  processed = 0;

  if (verbose) API_LOG_STDERR("Will process %d frames this time, encoding 1 of %d\n",*numInFrames,frame_skip);

  for (i = 0; i < (int) *numInFrames; i++)
  {
    if (! inputData[i])
	continue;

    if ((frame_skip) && (skipped++ < frame_skip))
	continue;
    skipped = 0;

    processed++;
    picture.m_fn = inputData[i]->info.frame_num;
    picture.m_ts = inputData[i]->info.capture_time;
    picture.m_y = inputData[i]->data.Y_base;
    picture.m_u = inputData[i]->data.U_base;
    picture.m_v = inputData[i]->data.V_base;

    picture.m_data = inputData[i]->data.Y_base;
    picture.m_vsize = inputData[i]->info.strDesc->height;
    picture.m_hsize = inputData[i]->info.strDesc->width;
    picture.m_size = (picture.m_hsize * picture.m_vsize * 3)/2;

    if (verbose)
	API_LOG_STDERR("Input frame %dx%d, size %d Virt: Y 0x%08x U 0x%08x V 0x%08x Phys: Y 0x%08x U 0x%08x V 0x%08x\n",
		picture.m_hsize,picture.m_vsize,picture.m_size,picture.m_y,picture.m_u,picture.m_v,
		inputData[i]->data.Y_phys,inputData[i]->data.U_phys,inputData[i]->data.V_phys);
    gArcFrameBuffers.SetInputYUV( inputData[i]->data.Y_base,inputData[i]->data.U_base,inputData[i]->data.V_base,
				  inputData[i]->data.Y_phys,inputData[i]->data.U_phys,inputData[i]->data.V_phys);
    more_to_encode = true;
    statistics_valid = false;
    picture_type = H264_P_PICTURE;

    if (0 == inputData[i]->info.frame_num)
    {
      // First I frame
      picture_type = H264_I_PICTURE;
    }
    else if (setup_parameters->intra_picture_frequency &&
	     (inputData[i]->info.frame_num % setup_parameters->intra_picture_frequency == 0))
    {
             // Regular Intra frame
      picture_type = H264_I_PICTURE;
    }

    while (more_to_encode == true)
    {
      reset_packet_queue(&opq);
      first_packet = *numOutPackets;
      bitsUsed = maximum_packet_size - 4;

      if (verbose)
	  API_LOG_STDERR("Before encode %d, tmpres = %d, more_to_encode = %d, %d packets available\n",inputData[i]->info.frame_num,tmpres,more_to_encode,opq.num_entries);

#ifdef STATISTICS
      if (collectingStats)
      {
	get_current_time(&start_ticks);
      }
#endif

#ifdef USE_ARC_TIMERS
      Reset_Timer0 ();
#endif

      tmpres = gCH264Encoder.EncodePicture(&picture,
					   picture_type,
					   0,
					   0,
					   &encParams->results,
					   &opq,
					   bitsUsed,
					   more_to_encode,
					   statistics_valid);

#ifdef USE_ARC_TIMERS
      Timer0_Val_To_Mem ();
#endif

#ifdef STATISTICS
      if (collectingStats)
      {
	get_current_time(&end_ticks);
      }
#endif

      if (verbose)
	  API_LOG_STDERR("Back from encode, tmpres = %d, more_to_encode = %d, %d packets available, numOutPackets %d\n",
			   tmpres,more_to_encode,opq.num_entries, *numOutPackets);

      if (tmpres == false)
      {
	  return gCH264Encoder.m_error;
      }

      result = enqueue_output_packets(&opq,
				      outputDest,
				      numOutPackets,
				      limit,
				      &packet_count);

#ifdef STATISTICS
      encParams->statistics_valid = statistics_valid;
      if (collectingStats && statistics_valid)
      {
    	  encParams->cumulative_stats->tot_elapsed_ticks += (end_ticks - start_ticks);
          outputDest[first_packet]->stats_buf = malloc(STATS_BUF_LEN);
          outputDest[first_packet]->stats_buf[0] = '\0';
          outputDest[first_packet]->stats_buf_len = STATS_BUF_LEN;
	  get_stats(encParams,outputDest[first_packet]);
    	  API_LOG_STDERR("%s\n",outputDest[first_packet]->stats_buf);
      }
#else
      API_LOG_STDERR("%d\n",picture.m_fn);
      encParams->statistics_valid = false;
#endif

      if (result != ENC_ERR_NONE)
	return result;

#ifdef OUTPUT_DEBUG_INFO
      if (outputDecoded)
      {
	for (i = 0 ; ((i < encParams->results->number_of_output_pictures) && (i < encParams->max_decoded_frames)); i++)
	{
	  CBTPicture* thispic = (CBTPicture*) encParams->results->pOutputPicture[i];
	  encParams->decoded_frames[i].data.Y_base = thispic->m_y; // FIXME these may be coming back as physical addresses
	  encParams->decoded_frames[i].data.U_base = thispic->m_u;
	  encParams->decoded_frames[i].data.V_base = thispic->m_v;
	}
      }
#endif
    }
  }

  *numInFrames = processed;
  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Control */
s32 h264_enc_control  (u32 handle,
		       u32 functionCode,
		       EncArgument* extraData)
{
  ArgCharstar* arg;
  ArgFundamental *fund;

  switch (functionCode)
  {
    case ENC_FN_H264_SETPARAMS:
	if (inited == false)
	    return ENC_ERR_NOTINIT;
	if (handle != encHandle)
	    return ENC_ERR_BADHANDLE;

	return change_settings(extraData);

    case ENC_FN_H264_CUMULATIVESTATS:
#ifdef STATISTICS
	if (inited == false)
	    return ENC_ERR_NOTINIT;
	if (handle != encHandle)
	    return ENC_ERR_BADHANDLE;

	if (collectingStats) return generate_cumulative_stats(extraData);
	else
#endif
	    return ENC_ERR_NOSTATS;

    case ENC_FN_COLL_STATS:
	if (inited == false)
	    return ENC_ERR_NOTINIT;
	if (handle != encHandle)
	    return ENC_ERR_BADHANDLE;

	return encoder_bool_arg(extraData, 1, &collectingStats);

    case ENC_FN_OUTPUTDEBUG:
	if (inited == false)
	    return ENC_ERR_NOTINIT;
	if (handle != encHandle)
	    return ENC_ERR_BADHANDLE;

	return encoder_bool_arg(extraData, 1, &outputDecoded);

  case ENC_FN_SET_COLL_STATS:
#ifdef STATISTICS
      if (inited == false)
	  return ENC_ERR_NOTINIT;
      if (handle != encHandle)
	  return ENC_ERR_BADHANDLE;

      if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_FUNDAMENTAL)))
	  return ENC_ERR_BADARG;
      CAST_ENCARGUMENT (fund, extraData, ArgFundamental *);
      collectingStats = fund->array[0].boolval;
      return ENC_ERR_NONE;
#else
      return ENC_ERR_NOFUNC;
#endif

    case ENC_FN_SET_OUTPUTDEBUG:
#ifdef OUTPUT_DEBUG_INFO
	if (inited == false)
	    return ENC_ERR_NOTINIT;
	if (handle != encHandle)
	    return ENC_ERR_BADHANDLE;

	if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_FUNDAMENTAL)))
	    return ENC_ERR_BADARG;
	CAST_ENCARGUMENT (fund, extraData, ArgFundamental *);
	outputDecoded = fund->array[0].boolval;
	return ENC_ERR_NONE;
#else
      return ENC_ERR_NOFUNC;
#endif

   case ENC_FN_H264_GETSTATSHDR:
       if ((! extraData) || (! CHECK_ARGTYPE(extraData, ENC_ARGTYPE_CHARSTAR)))
	   return ENC_ERR_BADARG;
       CAST_ENCARGUMENT(arg, extraData, ArgCharstar*);
       arg->value = (char*) stats_header;
       break;

    default:
	break;
  }

  return default_enc_control (0 /*selfptr*/,
			      handle,
			      functionCode,
			      extraData);
}

/*----------------------------------------------------------------------------------*/

static EncArgument* h264_create_argument(u32 typecode)
{
	switch (typecode)
	{
	     case ENC_ARGTYPE_H264SETUP:
		     {
			     H264SetupParams* tmpptr;
			     if ((tmpptr = (H264SetupParams*) malloc(sizeof(H264SetupParams))) != NULL)
			     {
				     if ((tmpptr->vui_parameters = (h264_vui_parameters*) malloc(sizeof(h264_vui_parameters))) != NULL)
				     {
					     SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_H264SETUP, sizeof(H264SetupParams));
					     tmpptr->typeinfo.magic = MGMT_IF_ALLOC_MAGIC;
					     return &(tmpptr->typeinfo);
				     }
				     else
					     free(tmpptr);
			     }
		     }
		     break;

	     case ENC_ARGTYPE_H264ENCPARAMS:
		     {
			     H264EncodingParams* tmpptr;
			     if ((tmpptr = (H264EncodingParams*) malloc(sizeof(H264EncodingParams))) != NULL)
			     {
				     SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_H264ENCPARAMS, sizeof(H264EncodingParams));
				     tmpptr->typeinfo.magic = MGMT_IF_ALLOC_MAGIC;
				     return &(tmpptr->typeinfo);
			     }
		     }
		     break;

	     case ENC_ARGTYPE_H264STATS:
		     {
			     H264EncoderStats* tmpptr;
			     if ((tmpptr = (H264EncoderStats*) malloc(sizeof(H264EncoderStats))) != NULL)
			     {
				     SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_H264STATS, sizeof(H264EncoderStats));
				     tmpptr->typeinfo.magic = MGMT_IF_ALLOC_MAGIC;
				     return &(tmpptr->typeinfo);
			     }
		     }
		     break;

	     default:
		     break;
	}
	return NULL;
}

/*----------------------------------------------------------------------------------*/

static s32 h264_destroy_argument(u32 typecode,
			    EncArgument* victim)
{
	if ((victim == NULL) || (victim->magic != MGMT_IF_ALLOC_MAGIC))
		return ENC_ERR_BADARG;

	victim->magic = 0;

	switch (typecode)
	{
	     case ENC_ARGTYPE_H264SETUP:
		     {
			     H264SetupParams* tmpptr;
			     CAST_ENCARGUMENT(tmpptr, victim, H264SetupParams*);
			     tmpptr->typeinfo.magic = 0;
			     free(tmpptr->vui_parameters);
			     free(tmpptr);
		     }
		     break;

	     case ENC_ARGTYPE_H264ENCPARAMS:
	     case ENC_ARGTYPE_H264STATS:
		     free(victim);
		     break;

	     default:
		 return ENC_ERR_FAILED;
	}

	return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 init_h264_mgmt_if()
{
	if ((register_encoder_factory_fns(h264_create_encoder,
					 h264_release_encoder) == ENC_ERR_NONE)
	    && (register_argument_factory_fns(h264_create_argument,
					      h264_destroy_argument) == ENC_ERR_NONE))
		return ENC_ERR_NONE;

	return ENC_ERR_FAILED ;
}

/*----------------------------------------------------------------------------------*/

s32 uninit_h264_mgmt_if()
{
	if ((remove_encoder_factory_fns(h264_create_encoder,
					h264_release_encoder) == ENC_ERR_NONE)
	    && (remove_argument_factory_fns(h264_create_argument,
					    h264_destroy_argument) == ENC_ERR_NONE))
		return ENC_ERR_NONE;

	return ENC_ERR_FAILED ;
}

/*----------------------------------------------------------------------------------*/


