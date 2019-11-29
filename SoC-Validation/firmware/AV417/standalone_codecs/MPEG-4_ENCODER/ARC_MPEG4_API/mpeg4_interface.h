
/*                                                         -*-C-*- *
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
 * mpeg4_interface.h                                               *
 *                                                                 *
 * ARC MPEG4 Encoder API definitions.                              *
 *                                                                 */

#ifndef MPEG4_ENC_IF_H
#define MPEG4_ENC_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __ARC_LINUX__
#include <time.h>
#endif

#include <arc_encoder.h>
#include <MP4Encoder.h>

/*----------------------------------------------------------------------------------*/

/* exported API functions */
  s32 mpeg4_enc_init (u32,	/* handle */
		     u32,	/* coding_type */
		     StreamDescriptor *,	/* stream info */
		     EncArgument *);	/* Encoder-specific extra data */

/* Uninit */
  s32 mpeg4_enc_uninit (u32);	/* handle */

/* Encode */
  s32 mpeg4_enc_encode (u32,	/* handle */
		       RawFrame **,	/* input data */
		       u32 *,	/* No of input frames available (in) / consumed (out) */
		       EncPacket **,	/* output destination */
		       u32 *,	/* no of output packets available (in) / produced (out) */
		       u32 *,	/* flags/status */
		       EncArgument *);	/* Encoder-specific extra data */

/* Control */
  s32 mpeg4_enc_control (u32,	/* handle */
			u32,	/* function code */
			EncArgument *);	/* Encoder-specific extra data */


/*----------------------------------------------------------------------------------*/

/* MPEG4 encoding types */
#define ENC_TYPE_MPEG4 2001

/*----------------------------------------------------------------------------------*/

/* MPEG4 specific control functions */
#define ENC_FN_MPEG4_SETPARAMS        2001
#define ENC_FN_MPEG4_GETSTATS         2002
#define ENC_FN_MPEG4_GETSTATSHDR      2003
#define ENC_FN_MPEG4_CUMULATIVESTATS  2004

/*----------------------------------------------------------------------------------*/

/* MPEG4 specific error codes */

/*----------------------------------------------------------------------------------*/

/* MPEG4 data types */
  typedef struct
  {
      IS_ARGTYPE;
      u32 frame_width;
      u32 frame_height;
      u8 coding_standard;
      bool use_annex_D;
      bool use_annex_J;
      bool use_annex_K;
      bool use_annex_I;
      bool use_annex_T;
      bool use_4MVs;
      bool use_video_packets;
      bool use_hec;
      bool use_gob_headers;
      bool fixed_quantiser;
      u32 fixed_quantiser_value;
      bool code_all_pictures;
      u32 target_bitrate;
      u32 target_picture_period;
      u32 input_picture_period;
      u32 intra_refresh_rate;
      u32 intra_picture_frequency;
      u32 quality_level;
      u32 dynamic_control;
      bool real_time_rate_control;
      u32 maximum_packet_size;
      u32 reaction_multiplier;

      h264_vui_parameters* vui_parameters;
  } MPEG4SetupParams;		/* ENC_ARGTYPE_MPEG4SETUP */

  typedef struct
  {
    IS_ARGTYPE;
    u32 tot_elapsed_ticks;
    u32 num_pics_encoded;
    u32 total_bits;
    double y_snr;
    double u_snr;
    double v_snr;
    u32 num_MB_force_skipped;
    u32 num_8x8_skipped;
    char *stats_buf;
    u32 stats_buf_len;
  } MPEG4EncoderStats;		/* ENC_ARGTYPE_MPEG4STATS */

  typedef struct
  {
    IS_ARGTYPE;
    h264_encoder_results *results;
    RawFrame *decoded_frames;
    u32 max_decoded_frames;
    bool statistics_valid;
    MPEG4EncoderStats *cumulative_stats;
  } MPEG4EncodingParams;		/* ENC_ARGTYPE_MPEG4ENCPARAMS */

/*----------------------------------------------------------------------------------*/

/* Argument type codes: MPEG4 2000->2999 */
#define ENC_ARGTYPE_MPEG4SETUP       2000
#define ENC_ARGTYPE_MPEG4ENCPARAMS   2001
#define ENC_ARGTYPE_MPEG4STATS       2002

/*----------------------------------------------------------------------------------*/

/* MPEG4 specific error codes */
#define ENC_ERR_MPEG4_OOM        2001

/*----------------------------------------------------------------------------------*/
/* Management interface setup */

  s32 init_mpeg4_mgmt_if ();
  s32 uninit_mpeg4_mgmt_if ();

/*----------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif				/* MPEG4_ENC_IF_H */
