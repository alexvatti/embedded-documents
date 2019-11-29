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
 * mpeg4_interface.cpp                                             *
 *                                                                 *
 * ARC MPEG4 Encoder API implementation.                           *
 *                                                                 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mpeg4_interface.h>
#include <packet_queue.h>
#include <arc_encoder_malloc.h>

#include <BTPicture.h>
#include <ArcFrameBuffers.h>
#include <typdef.h>
#include <ArcMPC.h>
#include <CMP4Encoder.h>
#include <ArcMP4SDMTables.h>
#include <MP4Encoder.h>
#include <picture_encoder.h>
#include <VRaptorInfo.h>
#include <defines.h>
#include <ArcChannelRoutines.h>

#ifdef HARDWARE_SIMD
#include <ArcSDMTables_MPA.h>
#include <ArcEncoderMain_MPA.h>
#include <ArcEncoderAddSubtract_MPA.h>
#include <ArcMP4ChromaFilter_MPA.h>
#include <ArcFDCT_MPA.h>
#include <ArcIDCT_MPA.h>
#include <ArcMP4DeQuant_MPA.h>
#include <ArcH263DeQuant_MPA.h>
#include <ArcMp4IntraLumaPrediction_MPA.h>
#include "ArcChannel_MPA.h"
#include "ArcSoftwareME_MPA.h"
#include "ArcSoftwareSubME_MPA.h"
#include "ArcLumaMP4SubPelFilter_MPA.h"
#endif //HARDWARE_SIMD


/*----------------------------------------------------------------------------------*/

#if defined(ARC_DCT) && !defined(HARDWARE_SIMD)
extern void CreoInitQuantTables (int standard);
#endif // ARC_DCT

/*----------------------------------------------------------------------------------*/

static struct packet_queue_t opq;

static u32 encHandle = 0;
static bool inited = false;
static StreamDescriptor *strDesc = 0;
static bool collectingStats = false;
static bool outputDecoded = false;
static bool verbose = false;
static u32 packet_count = 0;
static int frame_skip;
static int skipped;

static int btHandle;
extern CH264Encoder gCH264Encoder;
static MPEG4SetupParams* setup_parameters = 0;
static h264_encoder_setup* encoder_setup = 0;

#define TO_BOOL(x) (x != 0 ? true : false)

#define PACKETSIZE 400000

#define STATS_BUF_LEN 120

extern ArcFrameBuffers gArcFrameBuffers;

int picture_number = 0;

/*----------------------------------------------------------------------------------*/

static void print_encoder_setup (h264_encoder_setup * setup)
{
  API_LOG_STDERR( "frame_width %d\n", setup->frame_width);
  API_LOG_STDERR( "frame_height %d\n", setup->frame_height);
  API_LOG_STDERR( "fixed_quantiser %d\n", (int) setup->fixed_quantiser);
  API_LOG_STDERR( "fixed_quantiser_value %d\n", setup->fixed_quantiser_value);
  API_LOG_STDERR( "code_all_pictures %d\n", (int) setup->code_all_pictures);
  API_LOG_STDERR( "target_bitrate %d\n", setup->target_bitrate);
  API_LOG_STDERR( "target_picture_period %d\n", setup->target_picture_period);
  API_LOG_STDERR( "input_picture_period %d\n", setup->input_picture_period);
  API_LOG_STDERR( "intra_refresh_rate %d\n", setup->intra_refresh_rate);
  API_LOG_STDERR( "intra_picture_frequency %d\n", setup->intra_picture_frequency);
  API_LOG_STDERR( "real_time_rate_control %d\n", (int) setup->real_time_rate_control);
  API_LOG_STDERR( "maximum_packet_size %d\n", setup->maximum_packet_size);
  API_LOG_STDERR( "coding_standard %d\n", setup->coding_standard);
  API_LOG_STDERR( "use_annex_D %d\n", setup->use_annex_D);
  API_LOG_STDERR( "use_annex_J %d\n", setup->use_annex_J);
  API_LOG_STDERR( "use_annex_K %d\n", setup->use_annex_K);
  API_LOG_STDERR( "use_annex_I %d\n", setup->use_annex_I);
  API_LOG_STDERR( "use_annex_T %d\n", setup->use_annex_T);
  API_LOG_STDERR( "use_4MVs %d\n", setup->use_4MVs);
  API_LOG_STDERR( "use_video_packets %d\n", setup->use_video_packets);
  API_LOG_STDERR( "use_hec %d\n", setup->use_hec);
  API_LOG_STDERR( "use_gob_headers %d\n", setup->use_gob_headers);
  API_LOG_STDERR( "real_time_rate_control %d\n", setup->real_time_rate_control);
  API_LOG_STDERR( "reaction_multiplier %d\n", setup->reaction_multiplier);
  API_LOG_STDERR( "quality_level %d\n",  setup->quality_level);
}

/* Convert API setup params structure into encoder structure */

static h264_encoder_setup* convert_to_native (MPEG4SetupParams* inParams)
{
  h264_encoder_setup *outSettings = NULL;

  if ((inParams) && (outSettings = new h264_encoder_setup))
  {
    memset(outSettings,0,sizeof(h264_encoder_setup));

    if (inParams->reaction_multiplier == 0)
      inParams->reaction_multiplier = 30;
    if (inParams->quality_level == -1)
	inParams->quality_level = 10;


    // Initialise settings to defaults
    outSettings->use_gob_headers = 0;
    outSettings->use_hec = 0;

    outSettings->frame_width = inParams->frame_width;
    outSettings->frame_height = inParams->frame_height;
    outSettings->fixed_quantiser = TO_BOOL (inParams->fixed_quantiser);
    outSettings->fixed_quantiser_value = inParams->fixed_quantiser_value;
    outSettings->code_all_pictures = TO_BOOL (inParams->code_all_pictures);
    outSettings->target_bitrate = inParams->target_bitrate;
    outSettings->target_picture_period = inParams->target_picture_period;
    outSettings->input_picture_period = inParams->input_picture_period;
    outSettings->intra_refresh_rate = inParams->intra_refresh_rate;
    outSettings->intra_picture_frequency = inParams->intra_picture_frequency;
    outSettings->real_time_rate_control = TO_BOOL (inParams->real_time_rate_control);
    outSettings->quality_level = inParams->quality_level;
    outSettings->dynamic_control = inParams->dynamic_control;
    outSettings->reaction_multiplier = inParams->reaction_multiplier;

    outSettings->coding_standard = inParams->coding_standard;
    outSettings->use_annex_D = inParams->use_annex_D;
    outSettings->use_annex_J = inParams->use_annex_J;
    outSettings->use_annex_K = inParams->use_annex_K;
    outSettings->use_annex_I = inParams->use_annex_I;
    outSettings->use_annex_T = inParams->use_annex_T;
    outSettings->use_4MVs = inParams->use_4MVs;
    outSettings->use_video_packets = inParams->use_video_packets;
    // Force outSettings.intra_picture_frequency to be a multiple of (0 + 1)
    outSettings->intra_picture_frequency
      = (outSettings->intra_picture_frequency / (0 + 1)) * (0 + 1);

    if ((inParams->use_annex_K) || (inParams->use_video_packets))
	outSettings->maximum_packet_size = inParams->maximum_packet_size;
    else
	outSettings->maximum_packet_size = PACKETSIZE-12;

    if (outSettings->vui_parameters = (h264_vui_parameters*) malloc(sizeof (h264_vui_parameters)))
    {
      if (inParams->vui_parameters)
        memcpy(outSettings->vui_parameters,inParams->vui_parameters,sizeof(h264_vui_parameters));
    }

    outSettings->vui_parameters->fixed_frame_rate_flag = outSettings->code_all_pictures;  // Allow frame dropping

    outSettings->vui_parameters->nal_hrd_parameters_present_flag = false;  // Parameters not set
    outSettings->vui_parameters->vcl_hrd_parameters_present_flag = false;  // Parameters not set

    outSettings->vui_parameters->low_delay_hrd_flag = !outSettings->code_all_pictures;
    outSettings->vui_parameters->pic_struct_present_flag = false;
  }

  return outSettings;
}

/*----------------------------------------------------------------------------------*/

static encoder_interface* mpeg4_create_encoder (u32 encoder_type, u32 handle)
{
  encoder_interface *newif = NULL;
  if ((encoder_type == ENC_TYPE_MPEG4) && (encHandle == 0))
  {
    if ((newif =
	 (encoder_interface *) malloc (sizeof (encoder_interface))) != NULL)
    {
      newif->init = mpeg4_enc_init;
      newif->uninit = mpeg4_enc_uninit;
      newif->encode = mpeg4_enc_encode;
      newif->control = mpeg4_enc_control;
      newif->handle = handle;
      encHandle = handle;	// One instance only right now
    }
  }

  return newif;
}

/*----------------------------------------------------------------------------------*/

static encoder_basic_context* mpeg4_create_context(u32 encoder_type, u32 handle)
{
    encoder_basic_context* context = 0;

    if (encoder_type == ENC_TYPE_MPEG4)
    {
	if (context = malloc(sizeof(encoder_basic_context)))
	{
	    context->handle = handle;
	    context->expif = mpeg4_create_encoder(ENC_TYPE_MPEG4,handle);
	    context->impif = get_core_import_interface();
	}
    }

    return context;
}

/*----------------------------------------------------------------------------------*/

static s32 mpeg4_release_encoder (u32 encoder_type, u32 handle)
{
  if ((encoder_type == ENC_TYPE_MPEG4) && (encHandle == handle))
  {
    encHandle = 0;
    return ENC_ERR_NONE;
  }
  return ENC_ERR_NOTINIT;
}

/*----------------------------------------------------------------------------------*/

static s32 change_settings (EncArgument * extraData)
{
  MPEG4SetupParams *inParams;

  if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_MPEG4SETUP)))
    return ENC_ERR_BADARG;

  CAST_ENCARGUMENT (inParams, extraData, MPEG4SetupParams *);
  if ((encoder_setup = convert_to_native(inParams)) == NULL)
    return ENC_ERR_OOM;

  if(!gArcFrameBuffers.SetSize(inParams->frame_width, inParams->frame_height))
  {
    gArcFrameBuffers.DeleteInternalPictureBuffs();
    return ENC_ERR_OOM;
  }

  if (verbose)
    print_encoder_setup(encoder_setup);

  if (!gCH264Encoder.ChangeSettings(encoder_setup))
  {
    return ENC_ERR_FAILED;
  }
  gCH264Encoder.h263_encode_init(&encoder_setup->coding_standard);

  frame_skip = (inParams->target_picture_period/inParams->input_picture_period) - 1;
  if (frame_skip < 0) frame_skip = 0;
  skipped = frame_skip; // always code first frame

  setup_parameters = inParams;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

static const char stats_header[] =
  "\n Frm                                          Inter  Intra  Inter  Inter  Inter  Inter         Force  Average\n Num FC    Y_SNR    U_SNR    V_SNR   Bit cnt    4x4  16x16  16x16   16x8   8x16    8x8  skppd  skppd  quant\n\n";

static s32 get_stats (MPEG4EncodingParams * encParams, EncPacket * packet)
{
#ifdef STATISTICS
  double y_snr, u_snr, v_snr;
  CPicture * encoded_picture_ptr = (CPicture *)(encParams->results->pEncodedPicture);
  CPicture * source_picture_ptr = (CPicture *)(encParams->results->pSourcePicture);

  if (encoded_picture_ptr && source_picture_ptr)
  {
    encoded_picture_ptr->yuv_snr(*source_picture_ptr, y_snr, u_snr, v_snr);

    memset(packet->stats_buf,0,packet->stats_buf_len);
    sprintf(packet->stats_buf,
	     "%4d, %1d, %7.2f, %7.2f, %7.2f, %8d, %5d, %5d, %5d, %5d, %5d, %5d, %5d, %5d",
	     source_picture_ptr->m_fn, encParams->results->field_coding,
	     y_snr, u_snr, v_snr, encParams->results->bit_count,
	     encParams->results->intra_4x4_count,
	     encParams->results->intra_16x16_count,
	     encParams->results->inter_16x16_count,
	     encParams->results->inter_16x8_count,
	     encParams->results->inter_8x16_count,
	     encParams->results->inter_8x8_count,
	     encParams->results->skipped_count,
	     encParams->results->forced_skip_count);


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

static s32 generate_cumulative_stats (EncArgument * extraData)
{
#ifdef STATISTICS
  MPEG4EncodingParams *encParams;
  MPEG4EncoderStats *cumulative_stats;
  int offset;

  if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_MPEG4ENCPARAMS)))
    return ENC_ERR_BADARG;

  CAST_ENCARGUMENT (encParams, extraData, MPEG4EncodingParams *);

  if (encParams->cumulative_stats->stats_buf)
      free(encParams->cumulative_stats->stats_buf);
  encParams->cumulative_stats->stats_buf = (char*) malloc(120);
  encParams->cumulative_stats->stats_buf_len = 120;
  cumulative_stats = encParams->cumulative_stats;

  offset = sprintf(cumulative_stats->stats_buf,
		    "Finished: Elapsed time %d,  Frames per second  %7.4f\n",
		    cumulative_stats->tot_elapsed_ticks,
		    (double) (1000 * cumulative_stats->num_pics_encoded) /
		    (cumulative_stats->tot_elapsed_ticks));
  offset += sprintf(encParams->cumulative_stats->stats_buf + offset,
	     "%u Pictures Encoded, Bit rate (at %5.2fHz) %7.4f\n",
	     cumulative_stats->num_pics_encoded,
	     (double) 90000.0 /
	     (double) setup_parameters->target_picture_period,
	     (double) (cumulative_stats->total_bits) * 90000.0 /
	     (double) setup_parameters->target_picture_period /
	     (double) (cumulative_stats->num_pics_encoded));
  sprintf(encParams->cumulative_stats->stats_buf + offset,
	   "Bits/Picture %d,  AVG-Y-SNR=%7.4f, AVG-U-SNR=%7.4f, AVG-V-SNR=%7.4f,  %d MBs force skipped, %d skipped_8x8\n",
	   cumulative_stats->total_bits/cumulative_stats->num_pics_encoded,
	   cumulative_stats->y_snr / cumulative_stats->num_pics_encoded,
	   cumulative_stats->u_snr / cumulative_stats->num_pics_encoded,
	   cumulative_stats->v_snr / cumulative_stats->num_pics_encoded,
	   cumulative_stats->num_MB_force_skipped,
	   cumulative_stats->num_8x8_skipped);
#endif

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Init */
s32 mpeg4_enc_init (u32 handle,
	       u32 codingType,
	       StreamDescriptor * streamInfo, EncArgument * extraData)
{
  s32 tmpres;
  int i;
  int recAddr = 0;

  packet_count = 0;

  if (inited)
    return ENC_ERR_NOMOREINSTANCES;

  if (codingType != ENC_TYPE_MPEG4)
    return ENC_ERR_CODINGTYPE;

  strDesc = streamInfo;

  // Find vraptor devices
  if (VRaptorDevs.Init() == false) return ENC_ERR_FAILED;
  // Init SDM local memory
  SDM_WorkArea::Init_SDM_WorkArea();

#ifdef HARDWARE_SIMD
//       Record Media processor SIMD macros
  recAddr = SDM_Buffers_Record(recAddr, 0);
  recAddr = EncoderMain_Record(recAddr, 0);
  recAddr = EncoderAddSubtract_Record(recAddr, 0);
  recAddr = FilterChroma_Record(recAddr, 0);
  recAddr = Fdct_Record(recAddr, 0);
  recAddr = Idct_Record(recAddr, 0);
  recAddr = MP4DeQuant_Record(recAddr, 0);
  recAddr = H263DeQuant_Record(recAddr, 0);
  recAddr = IntraMP4LumaPrediction_Record(recAddr, 0);

  if (!HwMe)
  {
    recAddr = ArcChannel_Record(recAddr, 0);
    recAddr = Software_ME_Record(recAddr,0);
    recAddr = Software_SubME_Record(recAddr,0);
    recAddr = LumaSubPelFilter_Record(recAddr,0);
  }

  ENC_PRINTF(("SIMD0 SCM size required = 0x%x instructions (%dKBytes)\n", recAddr, ((recAddr*7)+1023)/1024));
#endif

  if ((tmpres = change_settings (extraData)) != ENC_ERR_NONE)
    return tmpres;

#if defined(ARC_DCT) && !defined(HARDWARE_SIMD)
  CreoInitQuantTables (encoder_setup->coding_standard);
#endif // ARC_DCT

  // setup me
  Service_ME_Initialise();
  Service_ME_SetBranchAddresses();

  // Initialise EE
  Initialise_Entropy_Encoder(4);

  encHandle = handle;
  inited = true;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Uninit */
s32 mpeg4_enc_uninit (u32 handle)
{
  int i;

  if (inited == false)
    return ENC_ERR_NOTINIT;
  if (handle != encHandle)
    return ENC_ERR_BADHANDLE;

  btHandle = 0;
  encHandle = 0;
  inited = false;
  if (encoder_setup) free(encoder_setup);

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Encode */
s32 mpeg4_enc_encode (u32 handle,
		      RawFrame** inputData,
		      u32* numInFrames,
		      EncPacket** outputDest,
		      u32* numOutPackets,
		      u32* status,
		      EncArgument* extraData)
{
  s32 tmpres;
  MPEG4EncodingParams *encParams;
  EncPacket *outPacket;
  CBTPicture picture;
  int i;
  int bitsUsed;
  u32 start_ticks, end_ticks;
  bool more_to_encode;
  h264_picture_type_t picture_type;
  bool statistics_valid;
  bool store_for_ref;
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

  if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_MPEG4ENCPARAMS)))
    return ENC_ERR_BADARG;

  CAST_ENCARGUMENT (encParams, extraData, MPEG4EncodingParams *);
  limit = *numOutPackets;
  *numOutPackets = 0;
  processed = 0;

  encParams->results = 0;

  for (i = 0; i < (int) *numInFrames; i++)
  {
    if (!inputData[i])
      continue;

    if ((frame_skip) && (skipped++ < frame_skip))
	continue;
    skipped = 0;

    processed++;
    picture.m_fn = inputData[i]->info.frame_num;
    picture.m_ts = ticks_to_ts (inputData[i]->info.capture_time);
    picture.m_y = inputData[i]->data.Y_base;
    picture.m_u = inputData[i]->data.U_base;
    picture.m_v = inputData[i]->data.V_base;

    picture.m_data = inputData[i]->data.Y_base;
    picture.m_vsize = inputData[i]->info.strDesc->height;
    picture.m_hsize = inputData[i]->info.strDesc->width;
    picture.m_size = (picture.m_hsize * picture.m_vsize * 3) / 2;

    if (verbose)
      API_LOG_STDERR(
	       "Input frame %dx%d, size %d Y at 0x%08x U at 0x%08x V at 0x%08x\n",
	       picture.m_hsize, picture.m_vsize, picture.m_size, picture.m_y,
	       picture.m_u, picture.m_v);
    gArcFrameBuffers.SetInputYUV( inputData[i]->data.Y_base,inputData[i]->data.U_base,inputData[i]->data.V_base,
				  inputData[i]->data.Y_phys,inputData[i]->data.U_phys,inputData[i]->data.V_phys);

    more_to_encode = true;
    statistics_valid = true;
    picture_type = H264_P_PICTURE;
    store_for_ref = true;

    if (0 == picture.m_fn)
    {
      // First I frame
      picture_type = H264_I_PICTURE;
      store_for_ref = true;
    }
    else if ((0 != setup_parameters->intra_picture_frequency) &&
	     (0 == (picture.m_fn % setup_parameters->intra_picture_frequency)))
    {
      // Regular Intra frame
      picture_type = H264_I_PICTURE;
      store_for_ref = true;
    }

    while (more_to_encode == true)
    {
      bitsUsed = (encoder_setup->maximum_packet_size+12)-4;
      first_packet = *numOutPackets;

      if (collectingStats)
      {
	get_current_time (&start_ticks);
      }

    if (0 == picture.m_fn)
      gCH264Encoder.m_time = 0;
    else
      gCH264Encoder.m_time += encoder_setup->input_picture_period;

      if (verbose)
	API_LOG_STDERR("About to encode frame number %d\n",picture.m_fn);

#ifdef USE_ARC_TIMERS
      Reset_Timer0 ();
#endif

      tmpres = gCH264Encoder.EncodePicture(&picture,
				      picture_type,
				      store_for_ref,
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

      if (tmpres == false)
      {
	  return gCH264Encoder.m_error;
      }

      if (collectingStats)
      {
	get_current_time(&start_ticks);
      }

      if (verbose)
	API_LOG_STDERR("%d packets produced, %d bytes used, more to encode = %d\n", opq.num_entries, bitsUsed, more_to_encode);

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

      reset_packet_queue(&opq);

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
    }
  }

  *numInFrames = processed;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

/* Control */
s32 mpeg4_enc_control (u32 handle, u32 functionCode, EncArgument * extraData)
{
  ArgCharstar *arg;
  ArgFundamental *fund;

  switch (functionCode)
  {
  case ENC_FN_MPEG4_SETPARAMS:
      if (inited == false)
	  return ENC_ERR_NOTINIT;
      if (handle != encHandle)
	  return ENC_ERR_BADHANDLE;

      return change_settings (extraData);

  case ENC_FN_MPEG4_CUMULATIVESTATS:
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

      return encoder_bool_arg (extraData, 1, &collectingStats);

  case ENC_FN_OUTPUTDEBUG:
      return encoder_bool_arg (extraData, 1, &outputDecoded);

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

  case ENC_FN_MPEG4_GETSTATSHDR:
      if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_CHARSTAR)))
	  return ENC_ERR_BADARG;
      CAST_ENCARGUMENT (arg, extraData, ArgCharstar *);
      arg->value = (char *) stats_header;
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

static EncArgument * mpeg4_create_argument (u32 typecode)
{
  switch (typecode)
  {
    case ENC_ARGTYPE_MPEG4SETUP:
    {
      MPEG4SetupParams *tmpptr;
      if ((tmpptr =
	   (MPEG4SetupParams *) malloc (sizeof (MPEG4SetupParams))) != NULL)
      {
	SETUP_ENCARGPTR (tmpptr, ENC_ARGTYPE_MPEG4SETUP,
			 sizeof (MPEG4SetupParams));
	tmpptr->typeinfo.magic = MGMT_IF_ALLOC_MAGIC;
	return &(tmpptr->typeinfo);
      }
    }
    break;

    case ENC_ARGTYPE_MPEG4ENCPARAMS:
    {
      MPEG4EncodingParams *tmpptr;
      if ((tmpptr =
	   (MPEG4EncodingParams *) malloc (sizeof (MPEG4EncodingParams))) !=
	  NULL)
      {
	SETUP_ENCARGPTR (tmpptr, ENC_ARGTYPE_MPEG4ENCPARAMS,
			 sizeof (MPEG4EncodingParams));
	tmpptr->typeinfo.magic = MGMT_IF_ALLOC_MAGIC;
	return &(tmpptr->typeinfo);
      }
    }
    break;

    case ENC_ARGTYPE_MPEG4STATS:
    {
      MPEG4EncoderStats *tmpptr;
      if ((tmpptr =
	   (MPEG4EncoderStats *) malloc (sizeof (MPEG4EncoderStats))) != NULL)
      {
	SETUP_ENCARGPTR (tmpptr, ENC_ARGTYPE_MPEG4STATS,
			 sizeof (MPEG4EncoderStats));
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

static s32 mpeg4_destroy_argument (u32 typecode, EncArgument * victim)
{
  if ((victim == NULL) || (victim->magic != MGMT_IF_ALLOC_MAGIC))
    return ENC_ERR_BADARG;

  switch (typecode)
  {
      case ENC_ARGTYPE_MPEG4SETUP:
      case ENC_ARGTYPE_MPEG4ENCPARAMS:
      case ENC_ARGTYPE_MPEG4STATS:
	  free(victim);
	  break;

      default:
          return ENC_ERR_FAILED;
  }

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 init_mpeg4_mgmt_if ()
{
  if ((register_encoder_factory_fns (mpeg4_create_encoder,
				     mpeg4_release_encoder) == ENC_ERR_NONE)
      && (register_argument_factory_fns (mpeg4_create_argument,
					 mpeg4_destroy_argument) ==
	  ENC_ERR_NONE))
    return ENC_ERR_NONE;

  return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/

s32 uninit_mpeg4_mgmt_if ()
{
  if ((remove_encoder_factory_fns (mpeg4_create_encoder,
				   mpeg4_release_encoder) == ENC_ERR_NONE)
      && (remove_argument_factory_fns (mpeg4_create_argument,
				       mpeg4_destroy_argument) ==
	  ENC_ERR_NONE))
    return ENC_ERR_NONE;

  return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/
