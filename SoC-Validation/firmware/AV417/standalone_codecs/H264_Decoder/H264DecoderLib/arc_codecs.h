/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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


#if !defined ARC_CODECS
#define ARC_CODECS

#define ARC_H264  	1
#define ARC_MPEG1 	2
#define ARC_MPEG2 	3
#define ARC_MPEG4 	4
#define ARC_H263RAW     5
#define ARC_MJPEG       6
#define ARC_VC1 	7
#define ARC_RCV		8
#define ARC_WVC1	9
#define ARC_WMVA       10

#define CONTROL_OK 1
#define CONTROL_TRUE 1
#define CONTROL_FALSE 0
#define CONTROL_UNKNOWN -1
#define CONTROL_ERROR -2
#define CONTROL_NA -3

#define VDCTRL_QUERY_FORMAT 3 		/* test for availabilty of a format */
#define VDCTRL_QUERY_MAX_PP_LEVEL 4 	/* test for postprocessing support (max level) */
#define VDCTRL_SET_PP_LEVEL 5 		/* set postprocessing level */
#define VDCTRL_SET_EQUALIZER 6 		/* set color options (brightness,contrast etc) */
#define VDCTRL_GET_EQUALIZER 7 		/* get color options (brightness,contrast etc) */
#define VDCTRL_RESYNC_STREAM 8 		/* seeking */
#define VDFLAGS_DROPFRAME 3

// codec doesn't support any form of direct rendering - it has own buffer
// allocation. so we just export its buffer pointers:
#define MP_IMGTYPE_EXPORT 0
// codec requires a static WO buffer, but it does only partial updates later:
#define MP_IMGTYPE_STATIC 1
// codec just needs some WO memory, where it writes/copies the whole frame to:
#define MP_IMGTYPE_TEMP 2
// I+P type, requires 2+ independent static R/W buffers
#define MP_IMGTYPE_IP 3
// I+P+B type, requires 2+ independent static R/W and 1+ temp WO buffers
#define MP_IMGTYPE_IPB 4

#define MP_MAX_PLANES	4

// set if buffer content shouldn't be modified:
#define MP_IMGFLAG_PRESERVE 0x01
// set if buffer content will be READ for next frame's MC: (I/P mpeg frames)
#define MP_IMGFLAG_READABLE 0x02

// stride should be aligned to MB boundary:     [for buffer allocation]
#define MP_IMGFLAG_PREFER_ALIGNED_STRIDE 0x8
// codec accept any stride (>=width):
#define MP_IMGFLAG_ACCEPT_STRIDE 0x10

#define MP_IMGFLAG_DRAW_CALLBACK 0x1000

#define IMGFMT_YV12 0x32315659
#define IMGFMT_I420 0x30323449
#define IMGFMT_IYUV 0x56555949

typedef struct {
  unsigned char 	       *planes[3];
  int 				stride[3];
  int 				xOffset;
  int 				yOffset;
  int				width;
  int 				height;
  int				index;
} arc_frame_data;

typedef int (*fp_control) 			(int,void *);
typedef int (*fp_init) 				(int,char *,int,int,int,int);
typedef int (*fp_set_buffer_addresses) 		(unsigned long,unsigned long *,unsigned long *);
typedef int (*fp_uninit) 			(void);
typedef int (*fp_decode) 			(char *,int,int,int *);
typedef int (*fp_get_frame_data)		(arc_frame_data *);
typedef int (*fp_set_sizes) 			(unsigned long,unsigned long,int);
typedef int (*fp_read_callback) 		(int,unsigned char *,int);


typedef struct {
  fp_init 			init;
  fp_set_buffer_addresses	set_buffer_addresses;
  fp_control 			control;
  fp_decode			decode;
  fp_get_frame_data		get_frame_data;
  fp_uninit 			uninit;
  fp_set_sizes			set_sizes;
  fp_read_callback		read_callback;
} arc_codec_functions;


#endif

