/*
 * Copyright (c) 2005 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * arc_mpeg.c
 *
 * December 2005.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#ifndef __GNUC__
#define inline _Inline
#endif

#include "libarcavcodec/arc/aurora_asm.h"
#include "libarcavcodec/bswap.h"
#include "arc_codecs.h"
#include "libarcavcodec/avcodec.h"

unsigned int avcodec_pix_fmt_to_codec_tag(enum PixelFormat fmt);
unsigned char isYUV422 = 0;

/******************************************************************************
   FFMPEG specific interface structure
******************************************************************************/

typedef struct {
  AVCodecContext *avctx;
  AVFrame *pic;
  float last_aspect;
  int do_slices;
  int do_dr1;
  int vo_inited;
  int best_csp;
  int b_age;
  int ip_age[2];
  int qp_stat[32];
  double qp_sum;
  double inv_qp_sum;
  int ip_count;
  int b_count;
} vd_ffmpeg_ctx;

/******************************************************************************
   Function prototypes
******************************************************************************/
//functions used to construct the interface
static int mpg_control 			(int, void *);
static int mpg_set_buffer_addresses 	(unsigned long, unsigned long *,unsigned long *);
static int mpg_init 			(int, char *, int, int, int, int);
static int mpg_decode 			(char *,int,int,int *);
static int mpg_get_frame_data		(arc_frame_data *);
static int mpg_uninit 			(void);
//(really) internal functions
static int get_buffer(AVCodecContext *avctx, AVFrame *pic);
static void release_buffer(AVCodecContext *avctx, AVFrame *pic);

/******************************************************************************
   Function pointer interface to the wrapper
******************************************************************************/

arc_codec_functions mpg_functions = {
  &mpg_init,
  &mpg_set_buffer_addresses,
  &mpg_control,
  &mpg_decode,
  &mpg_get_frame_data,
  &mpg_uninit,
  NULL, //set_sizes
  NULL  //read_callback
};

/******************************************************************************
   Global/static data
******************************************************************************/

extern void *Frame_Buffer_Virtual_Address;
extern void *Frame_Buffer_Physical_Address;
extern int vd_use_slices;

//default setup
static int lavc_param_workaround_bugs= FF_BUG_AUTODETECT;
static int lavc_param_error_resilience=2;
static int lavc_param_error_concealment=3;
static int lavc_param_gray=0;
static int lavc_param_vstats=0;
static int lavc_param_idct_algo=0;
static int lavc_param_debug=0;
static int lavc_param_vismv=0;
static int lavc_param_skip_top=0;
static int lavc_param_skip_bottom=0;
static int lavc_param_fast=0;
static int lavc_param_lowres=0;
static char *lavc_param_lowres_str=NULL;

static int avcodec_inited=0;
static void *context = 0;
static char *decoder_names[5] = {"mpegvideo","mpeg2video","mpeg4","h263","mjpeg"};
static unsigned int formats[5] = {0x10000001,0x10000002,0x10000004,0x33363248,0x47504A4D};
static int width = 0;
static int height = 0;
static unsigned char *planes [3] = {NULL,NULL,NULL};
static int stride [3] = {0,0,0};
//buffer management
static int current_flags = 0;
unsigned long *virtualAddresses = NULL;
unsigned long *physicalAddresses = NULL;

static int fb_addr_to_index(void *vaddr)
{
    int i;
    for (i=0; virtualAddresses[i] != (unsigned)vaddr; i++);

    return i;
}

/******************************************************************************
   MPEG decoder control
******************************************************************************/

// to set/get/query special features/parameters
static int mpg_control (int cmd, void* arg)
{
  vd_ffmpeg_ctx *ctx = context;
  AVCodecContext *avctx = ctx->avctx;
  switch(cmd)
  {
    case VDCTRL_QUERY_FORMAT:
    {
      int format = *((int*)arg);
      if (format == ctx->best_csp)
        return CONTROL_TRUE;//supported
      // possible conversions:
      switch( format )
      {
        case IMGFMT_YV12:
        case IMGFMT_IYUV:
        case IMGFMT_I420:
        // "converted" using pointer/stride modification
        if (avctx->pix_fmt==PIX_FMT_YUV420P)
          return CONTROL_TRUE;// u/v swap
        if (avctx->pix_fmt==PIX_FMT_YUV422P && !ctx->do_dr1)
          return CONTROL_TRUE;// half stride
        break;
      }
      return CONTROL_FALSE;
    }
    case VDCTRL_RESYNC_STREAM:
      avcodec_flush_buffers(avctx);
      return CONTROL_TRUE;
  }
  return CONTROL_UNKNOWN;
}

/******************************************************************************
   MPEG decoder initialisation functions
******************************************************************************/

//set the buffer addresses to be used by the codec
static int mpg_set_buffer_addresses (unsigned long SDMBase, unsigned long *vAddresses,unsigned long *pAddresses)
{
  if (SDM_Bytes == NULL)
  {
    SDM_Bytes = (void *)SDMBase;
    //fprintf(stderr,"\nSDM has been initialised, mapping %p to %p.\n", SDM_BASE,SDM_Bytes);
  }
  if (vAddresses != NULL)
  {
    virtualAddresses = vAddresses;
    Frame_Buffer_Virtual_Address = (void *)(vAddresses[0]);
  }
  if (pAddresses != NULL)
  {
    physicalAddresses = pAddresses;
    Frame_Buffer_Physical_Address = (void *)(pAddresses[0]);
  }
  else if (vAddresses != NULL)
    Frame_Buffer_Physical_Address = (void *)(vAddresses[0]);
  return (1);
}


// Initialise the codec, using only the type and width/height information
int mpg_init (int type,char *header_data,int header_size,int header_offset, int w, int h)
{
  AVCodecContext *avctx;
  vd_ffmpeg_ctx *ctx;
  AVCodec *lavc_codec;
  int lowres_w=0;
  int do_vis_debug= lavc_param_vismv || (lavc_param_debug&(FF_DEBUG_VIS_MB_TYPE|FF_DEBUG_VIS_QP));

  //check the type
  if ((type < ARC_MPEG1) || (type > ARC_MJPEG))
  {
    //fprintf(stderr,"\nInvalid codec type %u",type);
    return (0);
  }
  //normalise the type back to MPEG1 as a base
  type -= ARC_MPEG1;

  //check the SDM has been set
  if (SDM_Bytes == 0)
  {
    //fprintf(stderr,"\nInitialised SDM, call set_virtual_addresses first");
    return (0);
  }
 //set the stride for this image (assuming YUV420)
  stride[0] = w;
  stride[1] = w/2;
  stride[2] = w/2;
  //initialise avcodec library
  if (!avcodec_inited)
  {
    avcodec_init();
    avcodec_register_all();
    avcodec_inited=1;
  }
  //create the local context
  ctx = context = malloc(sizeof(vd_ffmpeg_ctx));
  if (!ctx)
    return(0);
  memset(ctx, 0, sizeof(vd_ffmpeg_ctx));
  //get a pointer to the correct codec
  lavc_codec = (AVCodec *)avcodec_find_decoder_by_name(decoder_names[type]);
  if(!lavc_codec)
  {
    //mp_msg(MSGT_DECVIDEO,MSGL_ERR,MSGTR_MissingLAVCcodec,sh->codec->dll);
    //fprintf(stderr,"\nFaild to load codec");
    return 0;
  }

  if (vd_use_slices && (lavc_codec->capabilities&CODEC_CAP_DRAW_HORIZ_BAND) && !do_vis_debug)
    ctx->do_slices=1;

  if (lavc_codec->capabilities & CODEC_CAP_DR1 && !do_vis_debug && lavc_codec->id != CODEC_ID_H264)
    ctx->do_dr1=1;
  ctx->b_age= ctx->ip_age[0]= ctx->ip_age[1]= 256*256*256*64;
  ctx->ip_count= ctx->b_count= 0;
  ctx->best_csp=IMGFMT_YV12;


  ctx->pic = avcodec_alloc_frame();
  ctx->avctx = avcodec_alloc_context();
  avctx = ctx->avctx;

  if(ctx->do_dr1)
  {
    avctx->flags|= CODEC_FLAG_EMU_EDGE;
    avctx->get_buffer= get_buffer;
    avctx->release_buffer= release_buffer;
    avctx->reget_buffer= get_buffer;
  }

  avctx->width = w;
  avctx->height= h;
  width = w;
  height = h;

  avctx->workaround_bugs= lavc_param_workaround_bugs;
  avctx->error_resilience= lavc_param_error_resilience;
  if (lavc_param_gray)
    avctx->flags|= CODEC_FLAG_GRAY;
  avctx->flags2 |= lavc_param_fast;
  avctx->codec_tag= formats[type];
  avctx->stream_codec_tag = 0; //0x64697678;
  avctx->idct_algo = lavc_param_idct_algo;
  avctx->error_concealment= lavc_param_error_concealment;
  avctx->debug = lavc_param_debug;
  avctx->debug_mv = lavc_param_vismv;
  avctx->skip_top   = lavc_param_skip_top;
  avctx->skip_bottom= lavc_param_skip_bottom;

  if (lavc_param_lowres_str != NULL)
  {
    sscanf(lavc_param_lowres_str, "%d,%d", &lavc_param_lowres, &lowres_w);
    if (lavc_param_lowres < 1 || lavc_param_lowres > 16 || (lowres_w > 0 && avctx->width < lowres_w))
      lavc_param_lowres = 0;
    avctx->lowres = lavc_param_lowres;
  }
  // open it
  if (avcodec_open(avctx, lavc_codec) < 0)
  {
   // fprintf(stderr,"\nError, can't open codec");
    return 0;
  }
  ctx->last_aspect = -3;
  ctx->vo_inited=0;
  return 1; //mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_YV12);
}


/******************************************************************************
   MPEG decoder de-initialisation
******************************************************************************/

// uninit driver
static int mpg_uninit (void)
{
  vd_ffmpeg_ctx *ctx = context;
  AVCodecContext *avctx = ctx->avctx;
  if (avcodec_close(avctx) < 0)
  	{
  	}
    //fprintf(stderr,"\nError, can't close codec");
  av_freep(&avctx->extradata);
  av_freep(&avctx->palctrl);
  av_freep(&avctx->slice_offset);
  av_freep(&avctx);
  av_freep(&ctx->pic);
  if (ctx)
    free(ctx);
 return (1);
}

/******************************************************************************
   MPEG decoder internal buffer callback functions
******************************************************************************/

static int get_buffer (AVCodecContext *avctx, AVFrame *pic)
{
  int flags = MP_IMGFLAG_ACCEPT_STRIDE | MP_IMGFLAG_PREFER_ALIGNED_STRIDE;
  vd_ffmpeg_ctx *ctx = context;
  int type= MP_IMGTYPE_IPB;
  int width= avctx->width;
  int height= avctx->height;
  int align=15;

  if ((avctx->pix_fmt == PIX_FMT_YUV422P) || (avctx->pix_fmt == PIX_FMT_YUVJ422P))
    isYUV422 = 1;

  //test for the MPEG-4 callback requirement
  if (virtualAddresses == NULL)
  {
    if (mpg_functions.set_sizes != (fp_set_sizes)0)
      mpg_functions.set_sizes(width,height,5);
    stride[0] = width;
    stride[1] = width/2;
    stride[2] = width/2;
  }

  if (avctx->pix_fmt == PIX_FMT_YUV410P)
    align=63; //yes seriously, its really needed (16x16 chroma blocks in SVQ1 -> 64x64)


  if (ctx->vo_inited == 0)
  {
    float last_aspect = av_q2d(avctx->sample_aspect_ratio) * avctx->width / avctx->height;
    ctx->last_aspect = last_aspect;
    ctx->vo_inited = 1;
  }

  if (pic->buffer_hints)
  {
    //mp_msg(MSGT_DECVIDEO,MSGL_DBG2, "Buffer hints: %u\n", pic->buffer_hints);
    type = MP_IMGTYPE_TEMP;
    if (pic->buffer_hints & FF_BUFFER_HINTS_READABLE)
      flags |= MP_IMGFLAG_READABLE;
    if (pic->buffer_hints & FF_BUFFER_HINTS_PRESERVE)
    {
      type = MP_IMGTYPE_STATIC;
      flags |= MP_IMGFLAG_PRESERVE;
    }
    if (pic->buffer_hints & FF_BUFFER_HINTS_REUSABLE)
    {
      type = MP_IMGTYPE_STATIC;
      flags |= MP_IMGFLAG_PRESERVE;
    }
    flags |= (!avctx->hurry_up && ctx->do_slices) ? MP_IMGFLAG_DRAW_CALLBACK:0;
    //mp_msg(MSGT_DECVIDEO,MSGL_DBG2, type == MP_IMGTYPE_STATIC ? "using STATIC\n" : "using TEMP\n");
  }
  else
  {
    if (!pic->reference)
    {
      ctx->b_count++;
      flags |= (!avctx->hurry_up && ctx->do_slices) ? MP_IMGFLAG_DRAW_CALLBACK:0;
    }
    else
    {
      ctx->ip_count++;
      flags |= MP_IMGFLAG_PRESERVE|MP_IMGFLAG_READABLE | (ctx->do_slices ? MP_IMGFLAG_DRAW_CALLBACK : 0);
    }
    if (ctx->b_count>1 || ctx->ip_count>2)
    {
      //fprintf(stderr,"DR1 failure\n");
      ctx->do_dr1=0; //FIXME
      avctx->get_buffer= avcodec_default_get_buffer;
      return avctx->get_buffer(avctx, pic);
    }
    if (avctx->has_b_frames)
      type= MP_IMGTYPE_IPB;
    else
      type= MP_IMGTYPE_IP;
  }

  //mpi = mpcodecs_get_image(sh,type, flags,(width+align)&(~align), (height+align)&(~align));

  avctx->draw_horiz_band= NULL;

  int buf = vbuf_alloc();


  pic->data[0] = (unsigned char *)virtualAddresses[buf];
  pic->data[1] = (unsigned char *)(virtualAddresses[buf] + (width * height));
  if (isYUV422)
    pic->data[2] = (unsigned char *)(pic->data[1] + (width * height) / 2);
  else
    pic->data[2] = (unsigned char *)(pic->data[1] + (width * height) / 4);

  /* Note, some (many) codecs in libavcodec must have stride1==stride2 && no changes between frames
   * lavc will check that and die with an error message, if its not true
  */
  pic->linesize[0] = stride[0];
  pic->linesize[1] = stride[1];
  pic->linesize[2] = stride[2];

  pic->opaque = (void *)flags;
  if (pic->reference)
  {
    pic->age= ctx->ip_age[0];
    ctx->ip_age[0]= ctx->ip_age[1]+1;
    ctx->ip_age[1]= 1;
    ctx->b_age++;
  }
  else
  {
    pic->age= ctx->b_age;
    ctx->ip_age[0]++;
    ctx->ip_age[1]++;
    ctx->b_age=1;
  }
  pic->type= FF_BUFFER_TYPE_USER;
  return 0;
}

static void release_buffer (struct AVCodecContext *avctx, AVFrame *pic)
{
  int flags = (int)pic->opaque;
  //sh_video_t * sh = avctx->opaque;
  vd_ffmpeg_ctx *ctx = context;
  int i;

  vbuf_free(fb_addr_to_index(pic->data[0]));

  if (ctx->ip_count <= 2 && ctx->b_count<=1)
  {
    if (flags & MP_IMGFLAG_PRESERVE)
      ctx->ip_count--;
    else
      ctx->b_count--;
  }
  if (pic->type!=FF_BUFFER_TYPE_USER)
  {
      avcodec_default_release_buffer(avctx, pic);
      return;
  }

  for(i=0; i<4; i++)
    pic->data[i] = NULL;
}


/******************************************************************************
   MPEG decoder decode functions
******************************************************************************/

// decode a frame
static int mpg_decode (char * data,int len,int offset,int *num_frames)
{
  int got_picture=0;
  int ret;
  vd_ffmpeg_ctx *ctx = context;
  AVFrame *pic= ctx->pic;
  AVCodecContext *avctx = ctx->avctx;
  int dr1= ctx->do_dr1;
  unsigned char *buf = (unsigned char *)data;
  *num_frames = 0;
#if 0
  fprintf(stderr,"\nLen: 0x%X, Data:",len);
  for (ret=0; ret<4; ret++)
    fprintf(stderr," %.2X",(unsigned char)(buf[ret]));
  fprintf(stderr," :");
  for (ret=0; ret<4; ret++)
    fprintf(stderr," %.2X",(unsigned char)(buf[len - 4 + ret]));
  buf = NULL;
#endif
  //if (len<=0)
  //  return (1); // skipped frame
  if (!dr1)
    avctx->draw_horiz_band = NULL;
  avctx->hurry_up = 0;

  ret = avcodec_decode_video(avctx, pic,&got_picture, data, len);

  dr1 = ctx->do_dr1;
  if (ret<0)
  {
	  //fprintf(stderr,"\nError while decoding frame!\n");
    //mp_msg(MSGT_DECVIDEO,MSGL_WARN, "Error while decoding frame!\n");
    return (0);
  }

  if(!got_picture)
    return 1;	// skipped image

  planes[0] = pic->data[0];
  planes[1] = pic->data[1];
  planes[2] = pic->data[2];
  stride[0] = pic->linesize[0];
  stride[1] = pic->linesize[1];
  stride[2] = pic->linesize[2];
  current_flags = (int)pic->opaque;
  if (!planes[0])
    return (0);
  vbuf_send(fb_addr_to_index(planes[0]));

  *num_frames = 1;
  return (1);
}

static int mpg_get_frame_data (arc_frame_data *frameData)
{
  frameData->stride[0] = stride[0];
  frameData->stride[1] = stride[1];
  frameData->stride[2] = stride[2];
  frameData->planes[0] = planes[0];
  frameData->planes[1] = planes[1];
  frameData->planes[2] = planes[2];
  frameData->xOffset = 0;
  frameData->yOffset = 0;
  frameData->width = width;
  frameData->height = height;
  frameData->index = 0;
  
  frameData->index = fb_addr_to_index(planes[0]);

  return (current_flags);
}

unsigned int avcodec_pix_fmt_to_codec_tag(enum PixelFormat fmt)
{
    return 0;
}
