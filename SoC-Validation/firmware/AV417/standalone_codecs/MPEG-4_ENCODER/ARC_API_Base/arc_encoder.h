
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
 * arc_encoder.h                                                   *
 *                                                                 *
 * ARC Video Encoder API definitions.                              *
 *                                                                 */


#ifndef ARC_ENCODER_H
#define ARC_ENCODER_H

#ifdef __cplusplus
extern "C" 
{
#endif 

#include <constants.h>
#include <packet_queue.h>

/*----------------------------------------------------------------------------------*/

#if defined(HAVE_STDIO)
 #if defined(__HIGHC__)
  #define API_LOG_STDERR(arglist...) fprintf(stderr,arglist)
  #define API_LOG(arglist...) printf(arglist)
 #else
  #define API_LOG_STDERR(...) fprintf(stderr,__VA_ARGS__)
  #define API_LOG(...) printf(__VA_ARGS__)
 #endif
#else
 #define API_LOG_STDERR(arglist...)
 #define API_LOG(arglist...)
#endif

/*----------------------------------------------------------------------------------*/

/* Fundamental typedefs */
typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned long u32;
typedef long s32;

#if !defined(__cplusplus) && (!defined bool)
typedef unsigned char bool;
#define true (unsigned char) 1
#define false (unsigned char) 0
#endif

#ifdef __ARC_LINUX__
#define UNCACHED_PTR(ptr) unsigned char* ptr
#else
#define UNCACHED_PTR(ptr) unsigned char* ptr
#endif
#define CACHED_PTR(ptr) unsigned char* ptr

/*----------------------------------------------------------------------------------*/

/* Encoder-specific data types */

typedef struct
{
  u32 type_id;
  u32 size;
  u32 magic;
  void *owner;
} EncArgument;

#define IS_ARGTYPE EncArgument typeinfo

typedef union
{
  bool boolval;
  u8 charval;
  u16 shortval;
  u32 longval;
  void* ptr;
} AnyFundamental;

typedef struct
{
  IS_ARGTYPE;
  u32 num_entries;
  AnyFundamental *array;
} ArgFundamental;

typedef struct
{
  IS_ARGTYPE;
  char *value;
  u32 length;
} ArgCharstar;

#define CHECK_ARGTYPE(argptr, typecode) (argptr->type_id == typecode)

/* Argument type codes: generic 0->999 */
#define ENC_ARGTYPE_FUNDAMENTAL 1
#define ENC_ARGTYPE_MEMBLKDESC  2
#define ENC_ARGTYPE_CHARSTAR    3
#define ENC_ARGTYPE_STREAMDESC  4
#define ENC_ARGTYPE_FRAMEDESC   5
#define ENC_ARGTYPE_RAWFRAME    6
#define ENC_ARGTYPE_ENCPACKET   7


/*----------------------------------------------------------------------------------*/

#define SETUP_ENCARGUMENT(Arg, Typecode, Argsize) \
  Arg.typeinfo.type_id = Typecode;\
  Arg.typeinfo.size = Argsize;\
  Arg.typeinfo.owner = &Arg;\
  Arg.typeinfo.magic = 0;

#define SETUP_ENCARGPTR(Argptr, Typecode, Argsize) \
  Argptr->typeinfo.type_id = Typecode;\
  Argptr->typeinfo.size = Argsize;\
  Argptr->typeinfo.owner = Argptr;\
  Argptr->typeinfo.magic = 0;

#define CAST_ENCARGUMENT(Destptr, Srcptr, Destptrtype)   Destptr = (Destptrtype) (Srcptr->owner);
#define ENC_ARG(Arg) Arg.typeinfo
#define ENC_PTRARG(Argptr) Argptr->typeinfo

#define MGMT_IF_ALLOC_MAGIC 0xf15443ad	/* Mgmt interface uses this to identify data items it's allocated */

/*----------------------------------------------------------------------------------*/

/* API data structures */

typedef struct
{
  UNCACHED_PTR (Y_base);
  UNCACHED_PTR (U_base);
  UNCACHED_PTR (V_base);
  unsigned char* Y_phys;
  unsigned char* U_phys;
  unsigned char* V_phys;
} YUVData;

typedef struct
{
  IS_ARGTYPE;
  u16 width;
  u16 height;
  u32 frames;
  u32 current_time;
  u32 current_frame;
  u32 bitrate;
  bool interlaced;
} StreamDescriptor;

typedef struct
{
  IS_ARGTYPE;
  u32 frame_num;
  u32 capture_time;
  StreamDescriptor *strDesc;
} FrameDescriptor;

typedef struct
{
  IS_ARGTYPE;
  YUVData data;
  FrameDescriptor info;
  bool unused;
  u32 tag;
} RawFrame;

typedef struct
{
  IS_ARGTYPE;
  u32 packet_no;
  UNCACHED_PTR (data);
  u32 packet_len;
  u32 packet_type_code;
  FrameDescriptor info;
  bool unused;
  u8 num_unused_bits_first;
  u8 num_unused_bits_last;
  RawFrame *decoded_picture;
  char *stats_buf;
  u32 stats_buf_len;
} EncPacket;

typedef struct
{
  IS_ARGTYPE;
  void *phys;
  void *virt;
  u32 size;
  u32 flags;
  u32 tag;
} MemBlkDesc;

/*----------------------------------------------------------------------------------*/

/* Exported interface function typedefs */

/* Init */
typedef s32 (*fp_enc_init) (u32,	/* handle */
			    u32,	/* coding_type */
			    StreamDescriptor *,	/* stream info */
			    EncArgument *);	/* Encoder-specific extra data */

/* Uninit */
typedef s32 (*fp_enc_uninit) (u32);	/* handle */

/* Encode */
typedef s32 (*fp_enc_encode) (u32,	/* handle */
			      RawFrame **,	/* input data */
			      u32 *,	/* No of input frames (in) / num consumed (out) */
			      EncPacket **,	/* output destination */
			      u32 *,	/* size of output array (in) / no of output packets (out) */
			      u32 *,	/* flags/status */
			      EncArgument *);	/* Encoder-specific extra data */

/* Control */
typedef s32 (*fp_enc_control) (u32,	/* handle */
			       u32,	/* function code */
			       EncArgument *);	/* Encoder-specific extra data */


/* Interface structure */
typedef struct
{
  fp_enc_init     init;
  fp_enc_uninit   uninit;
  fp_enc_encode   encode;
  fp_enc_control  control;
  u32             handle;
} encoder_interface;

/*----------------------------------------------------------------------------------*/

/* Imported interface function typedefs */

/* Frames ready */
typedef s32 (*fp_enc_frames_ready) (encoder_interface *,	/* ptr to calling encoder */
				    EncPacket **,	/* encoded data */
				    u32);	/* packet count */

/* Data needed */
typedef s32 (*fp_enc_data_needed) (encoder_interface *,	/* ptr to calling encoder */
				   RawFrame **,	/* dest for input data */
				   u32 *);	/* frame count dest */

/* Allocate */
typedef s32 (*fp_enc_allocate) (encoder_interface *,	/* ptr to calling encoder */
				u32,	/* size */
				u32,	/* flags */
				u32,    /* numblocks */
				MemBlkDesc **);	/* destination */

/* Release */
typedef s32 (*fp_enc_release) (encoder_interface *,	/* ptr to calling encoder */
			       MemBlkDesc **);	/* blocks */

/* Attention */
typedef s32 (*fp_enc_attention) (encoder_interface *,	/* ptr to calling encoder */
				 u32);	/* Cause code */


/* Interface structure */
typedef struct
{
  fp_enc_frames_ready   frames_ready;
  fp_enc_data_needed    data_needed;
  fp_enc_allocate       allocate;
  fp_enc_release        release;
  fp_enc_attention      attention;
} encoder_import_interface;

encoder_import_interface* get_core_import_interface();

/*----------------------------------------------------------------------------------*/

/* This'll come into play more as we deal with multi-encoder usages */

typedef struct
{
    IS_ARGTYPE;
    u32 handle;
    encoder_interface *expif;
    encoder_import_interface *impif;
} encoder_basic_context;

#define ENC_ARGTYPE_ENC_CONTEXT 5

/*----------------------------------------------------------------------------------*/

/* Management interface */

/* --- Interface factory --- */
encoder_interface *create_encoder (u32,	  /* encoder type */
				   u32);  /* handle */
s32 release_encoder               (u32,	  /* encoder type */
		                   u32);  /* handle */

/* Factory plugin routine prototypes */
typedef encoder_interface *(*encoder_factory_fn) (u32,	/* encoder_type */
						  u32);	/* handle */
typedef s32                (*encoder_release_fn) (u32,	/* encoder_type */
                                                  u32); /* handle */
/* Add/remove plugin routines */
s32 register_encoder_factory_fns (encoder_factory_fn, 
                                  encoder_release_fn);
s32 remove_encoder_factory_fns   (encoder_factory_fn, 
                                  encoder_release_fn);

/* --- Argument factory --- */
EncArgument *create_argument  (u32);	        /* typecode */
s32          destroy_argument (EncArgument *);	/* victim */
/* Factory plugin routines */
typedef EncArgument *(*argument_factory_fn)    (u32);
typedef s32          (*argument_destructor_fn) (u32, 
                                                EncArgument *);
/* Add/remove plugin routines */
s32 register_argument_factory_fns (argument_factory_fn,
				   argument_destructor_fn);
s32 remove_argument_factory_fns   (argument_factory_fn, 
                                   argument_destructor_fn);

/*----------------------------------------------------------------------------------*/

/* Core API functions */

s32 init_arc_encoder_api();
s32 uninit_arc_encoder_api();
EncPacket* getFreePacket();
s32 enqueue_output_packets(struct packet_queue_t*, 
			   EncPacket**,
			   u32*,
			   u32,
			   u32*);
s32 arc_encoder_attention(encoder_basic_context*,
			  u32,
			  u32);

/*----------------------------------------------------------------------------------*/

/* Control function codes */
/* Generic */
#define ENC_FN_IFVERSION        1
#define ENC_FN_SET_IMPIF        2
#define ENC_FN_GET_IMPIF        3
#define ENC_FN_SET_BLOCK        4
#define ENC_FN_GET_BLOCK        5
#define ENC_FN_ENCSETTINGS      6
#define ENC_FN_ADDMEM           7
#define ENC_FN_COLL_STATS       8
#define ENC_FN_OUTPUTDEBUG      9
#define ENC_FN_SET_COLL_STATS  10
#define ENC_FN_SET_OUTPUTDEBUG 11
#define ENC_FN_NUM_FRAMEBUFS   12 // Linux-specific - get number of framebuffers

/*----------------------------------------------------------------------------------*/

/* Attention notification codes */
/* Generic */
#define ARC_ENC_ATTN_RECON 1

/*----------------------------------------------------------------------------------*/

/* Error codes */
/* Generic */
#define ENC_ERR_NONE              0
#define ENC_ERR_FAILED           -1
#define ENC_ERR_NOTINIT          -2
#define ENC_ERR_NOFUNC           -3
#define ENC_ERR_BAD_IMPIF        -4
#define ENC_ERR_CODINGTYPE       -5
#define ENC_ERR_OOM              -6
#define ENC_ERR_BADARG           -7
#define ENC_ERR_NOMOREINSTANCES  -8
#define ENC_ERR_BADHANDLE        -9
#define ENC_ERR_NOSTATS         -10
#define ENC_ERR_EOF             -11
#define ENC_ERR_OUTOFOPS        -12

/*----------------------------------------------------------------------------------*/

/* API states */

#define ENC_BLOCKING 1

/*----------------------------------------------------------------------------------*/

/* Argument errors */

/* Error source determination */
#define NO_ERROR(code) (code == ENC_ERR_NONE)
#define ENCODER_ERROR(code) (code > ENC_ERR_NONE)
#define WRAPPER_ERROR(code) (code < ENC_ERR_NONE)

/*----------------------------------------------------------------------------------*/

/* Drudge functions */

/* Wrapper round platform time fns */
u32 get_current_time (u32* dest);
u32 ticks_to_ts (u32 ticks);

/* WOrk out memory alignment from api_malloc flags */
int get_alignment(u32 align);

/* Common control fns */
s32 default_enc_control (encoder_interface*,
			 u32,
			 u32,
			 EncArgument*);

/* Fundamental argument handling */
s32 encoder_bool_arg (EncArgument*, 
		      int, 
		      bool*);
s32 set_encoder_bool_arg (EncArgument*, 
			 int, 
			 bool);
s32 encoder_u32_arg (EncArgument*, 
		     int, 
		     u32*);
s32 set_encoder_u32_arg (EncArgument*, 
			 int, 
			 u32);
s32 encoder_u16_arg (EncArgument*, 
		     int, 
		     u16*);
s32 set_encoder_u16_arg (EncArgument*, 
			 int, 
			 u16);
s32 encoder_u8_arg (EncArgument*, 
		     int, 
		     u8*);
s32 set_encoder_u8_arg (EncArgument*, 
			 int, 
			 u8);
s32 encoder_ptr_arg (EncArgument*, 
		     int, 
		     void**);
s32 set_encoder_ptr_arg (EncArgument*, 
			 int, 
			 void*);
s32 encoder_charstar_arg (EncArgument*, 
			  char**);

/*----------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif 

#endif /* ARC_ENCODER_H */
