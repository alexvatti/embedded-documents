
/*                                                         -*-C-*- *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION                        *
 * Copyright 2006 ARC International (Unpublished)                  *
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
 * h264_interface.h                                                *
 *                                                                 *
 * ARC H264 Encoder API definitions.                               *
 *                                                                 */

#ifndef H264_ENC_IF_H
#define H264_ENC_IF_H

#ifdef __cplusplus
extern "C" {
#endif	

#ifdef __ARC_LINUX__
#include <time.h>
#endif

#include <arc_encoder.h>
#include <H264Encoder.h>

/*----------------------------------------------------------------------------------*/

/* exported API functions */
s32 h264_enc_init     (u32,                     /* handle */
		       u32,                     /* coding_type */
		       StreamDescriptor*,       /* stream info */
		       EncArgument*);           /* Encoder-specific extra data */

/* Uninit */
s32 h264_enc_uninit   (u32);                    /* handle */

/* Encode */
s32 h264_enc_encode   (u32,                     /* handle */
		       RawFrame**,              /* input data */
		       u32*,                    /* No of input frames available (in) / consumed (out) */
		       EncPacket**,             /* output destination */
		       u32*,                    /* no of output packets available (in) / produced (out) */
		       u32*,                    /* flags/status */
		       EncArgument*);           /* Encoder-specific extra data */

/* Control */
s32 h264_enc_control  (u32,                     /* handle */
		       u32,                     /* function code */
		       EncArgument*);           /* Encoder-specific extra data */


/*----------------------------------------------------------------------------------*/

/* H264 encoding types */
#define ENC_TYPE_H264 1001

/*----------------------------------------------------------------------------------*/

/* H264 specific control functions */
#define ENC_FN_H264_SETPARAMS        1001
#define ENC_FN_H264_GETSTATS         1002
#define ENC_FN_H264_GETSTATSHDR      1003
#define ENC_FN_H264_CUMULATIVESTATS  1004

/*----------------------------------------------------------------------------------*/

/* H264 specific error codes */
#define ENC_ERR_H264_OOM        1001

/*----------------------------------------------------------------------------------*/

/* H264 data types */
typedef struct {
  IS_ARGTYPE;
  u32          frame_width;
  u32          frame_height;
  bool         fixed_quantiser;
  u32          fixed_quantiser_value;
  u32          target_bitrate;
  u32          intra_picture_frequency;
  u32          disable_deblocking_filter_idc;
  bool         code_all_pictures;
  u32          target_picture_period;
  u32          input_picture_period;
  u32          intra_refresh_rate;
  bool         real_time_rate_control;
  s8           chroma_qp_index_offset;
  bool         constrained_intra_pred_flg;
  u32          num_slice_grps_P_pictures;
  u32 	       maximum_packet_size;
  u32 	       reaction_multiplier;
  u32 	       quality_level;
  u32          dynamic_control;  
  h264_vui_parameters* vui_parameters;
} H264SetupParams; /* ENC_ARGTYPE_H264SETUP */

typedef struct {
  IS_ARGTYPE;
  u32          tot_elapsed_ticks;
  u32          num_pics_encoded;
  u32          total_bits;
  double       y_snr;
  double       u_snr;
  double       v_snr;
  u32          num_MB_force_skipped;
  u32          num_8x8_skipped;
  char*        stats_buf;
  u32          stats_buf_len;
} H264EncoderStats;  /* ENC_ARGTYPE_H264STATS */

typedef struct {
  IS_ARGTYPE;
  h264_encoder_results* results;
  RawFrame*    decoded_frames;
  u32          max_decoded_frames;
  bool         statistics_valid;
  H264EncoderStats* cumulative_stats;
} H264EncodingParams; /* ENC_ARGTYPE_H264ENCPARAMS */

/*----------------------------------------------------------------------------------*/

/* Argument type codes: H264 1000->1999 */
#define ENC_ARGTYPE_H264SETUP       1000
#define ENC_ARGTYPE_H264ENCPARAMS   1001
#define ENC_ARGTYPE_H264STATS       1002

/*----------------------------------------------------------------------------------*/

/* Management interface setup */

s32 init_h264_mgmt_if();
s32 uninit_h264_mgmt_if();

/*----------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* H264_ENC_IF_H */
