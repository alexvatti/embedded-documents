
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
 * arc_encoder.h                                                   *
 *                                                                 *
 * ARC Video Encoder API definitions.                              *
 *                                                                 */

#include <arc_encoder.h>
#include <arc_encoder_malloc.h>
#include <assert.h>
#include <packet_queue.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ARC_LINUX__
#include <linux_support.h>
#else
#include <standalone_support.h>
#endif

/*----------------------------------------------------------------------------------*/

static EncPacket packets[MAX_OUTPUT_PACKETS];

u32 LastRecon;
static MemBlkDesc* freedescptr[2];

/*----------------------------------------------------------------------------------*/

u32 arc_encoder_api_version = API_VERSION;

/*----------------------------------------------------------------------------------*/

static encoder_import_interface* impif = 0;     // default import interface

/*----------------------------------------------------------------------------------*/

static int alignments[] = { 4,4,8,16,32,64,128 };
#define ALIGNLIMIT (ENC_MEM_ALIGN_128 >> ENC_MEM_ALIGNSHIFT)

int get_alignment(u32 align)
{
    align &= ENC_MEM_ALIGNMASK;
    align >>= ENC_MEM_ALIGNSHIFT;
    if (align > ALIGNLIMIT) return 0; // We're doomed

    return alignments[align];
}

/*----------------------------------------------------------------------------------*/

encoder_import_interface* get_core_import_interface()
{
  return impif;
}

/*----------------------------------------------------------------------------------*/

u32 get_current_time (u32* dest)
{
  return time (dest);
}

/*----------------------------------------------------------------------------------*/

u32 ticks_to_ts (u32 ticks)
{
  return ticks;
}

/*----------------------------------------------------------------------------------*/

#define CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index) \
  if ((! extraData) || (! CHECK_ARGTYPE(extraData, ENC_ARGTYPE_FUNDAMENTAL))) \
    return ENC_ERR_BADARG; \
  CAST_ENCARGUMENT(fundarg, extraData, ArgFundamental*); \
  if (fundarg->num_entries < (index + 1)) \
    return ENC_ERR_BADARG;

/*----------------------------------------------------------------------------------*/

s32 encoder_bool_arg (EncArgument* extraData, 
		      int index, 
		      bool* target)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)

  *target = fundarg->array[index].boolval;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 set_encoder_bool_arg (EncArgument* extraData, 
			 int index, 
			 bool value)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)
  
  fundarg->array[index].boolval = value;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 encoder_u32_arg (EncArgument* extraData, 
		     int index, 
		     u32* target)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)

  *target = fundarg->array[index].longval;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 set_encoder_u32_arg (EncArgument* extraData, 
			 int index, 
			 u32 value)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)
  
  fundarg->array[index].longval = value;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 encoder_u16_arg (EncArgument* extraData, 
		     int index, 
		     u16* target)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)

  *target = fundarg->array[index].shortval;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 set_encoder_u16_arg (EncArgument* extraData, 
			 int index, 
			 u16 value)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)
  
  fundarg->array[index].shortval = value;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 encoder_u8_arg (EncArgument* extraData, 
		     int index, 
		     u8* target)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)

  *target = fundarg->array[index].charval;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 set_encoder_u8_arg (EncArgument* extraData, 
			 int index, 
			 u8 value)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)
  
  fundarg->array[index].charval = value;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 encoder_ptr_arg (EncArgument* extraData, 
		     int index, 
		     void** target)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)

  *target = fundarg->array[index].ptr;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 set_encoder_ptr_arg (EncArgument* extraData, 
			 int index, 
			 void* value)
{
  ArgFundamental *fundarg;
  CAST_AND_CHECK_ARG_FUNDAMENTAL(extraData,fundarg,index)
  
  fundarg->array[index].ptr = value;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 encoder_charstar_arg (EncArgument* extraData, 
			  char **target)
{
  ArgCharstar *arg;
  if ((!extraData) || (!CHECK_ARGTYPE (extraData, ENC_ARGTYPE_CHARSTAR)))
    return ENC_ERR_BADARG;
  CAST_ENCARGUMENT (arg, extraData, ArgCharstar *);

  target = &(arg->value);

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 default_enc_control (encoder_interface* this,
			 u32 handle,
			 u32 function_code,
			 EncArgument* extra_data)
{
  switch (function_code)
  {
    case ENC_FN_IFVERSION:
      return set_encoder_u32_arg(extra_data, 0, API_VERSION);

    case ENC_FN_SET_IMPIF:
      if (encoder_ptr_arg(extra_data, 0, (void **)&impif) == ENC_ERR_NONE)
	if (impif != 0) return  ENC_ERR_NONE;
      return ENC_ERR_BADARG;

    case ENC_FN_GET_IMPIF:
      return set_encoder_ptr_arg(extra_data, 0, (void **)&impif);

    case ENC_FN_SET_BLOCK:
      return ENC_ERR_NOFUNC;

    case ENC_FN_GET_BLOCK:
      return set_encoder_u32_arg(extra_data, 0, ENC_BLOCKING);

    case ENC_FN_ADDMEM:
#ifdef __ARC_LINUX__
    {
	MemBlkDesc *memblkdesc;
	if ((!extra_data) || (!CHECK_ARGTYPE (extra_data, ENC_ARGTYPE_MEMBLKDESC)))
	    return ENC_ERR_BADARG;
	CAST_ENCARGUMENT (memblkdesc, extra_data, MemBlkDesc *);
	return addmemory_linux(memblkdesc);
    }
#endif
    break;

    case ENC_FN_NUM_FRAMEBUFS:
#ifdef __ARC_LINUX__
    {
	u32 res = num_framebufs_linux();
	return set_encoder_u32_arg(extra_data, 0, res);
    }
#endif
    break;


    default:
      break;
  }

  return ENC_ERR_NOFUNC;
}

/*----------------------------------------------------------------------------------*/

s32 arc_encoder_attention(encoder_basic_context* contextptr, u32 cause, u32 details)
{
    if (contextptr->impif->attention)
	contextptr->impif->attention(contextptr->expif,cause);

    switch(cause)
    {
        case ARC_ENC_ATTN_RECON:
	    LastRecon = details;
	    break;

        default:
	    return ENC_ERR_NOFUNC;
    }

    return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

void* arc_encoder_api_malloc(u32 size,
			     u32 flags,
			     unsigned char** phys,
			     u32* tag)
{
  void* tmpptr;
  MemBlkDesc* descptr;

  assert(impif != 0);
  assert(impif->allocate != 0);

  if (impif->allocate(0 /*encoder_if*/, size, flags, 1, &descptr) == ENC_ERR_NONE)
  {
    assert(descptr != 0);
    tmpptr = descptr->virt;
    *phys = (unsigned char*) descptr->phys;
    if (tag) 
	*tag = descptr->tag;

    return tmpptr;
  }

/*   API_LOG_STDERR("impif->allocate failed\n"); */
  return (void*) 0;
}

/*----------------------------------------------------------------------------------*/

void arc_encoder_api_free(void* ptr, 
			  u32 flags,
			  u32* tag)
{
  assert(impif != 0);
  assert(impif->release != 0);

  if (!ptr)
  {
      API_LOG_STDERR("Attempt to free a null pointer\n");
      return;
  }

  SETUP_ENCARGPTR(freedescptr[0], ENC_ARGTYPE_MEMBLKDESC, sizeof(MemBlkDesc));
  freedescptr[0]->flags = flags;
  if (tag)
      freedescptr[0]->tag = *tag;
  else
      freedescptr[0]->tag = 0;
  
  freedescptr[0]->virt = ptr;
    
  freedescptr[1] = (MemBlkDesc*) 0;
  impif->release(0 /*encoder_if*/, freedescptr);
}

/*----------------------------------------------------------------------------------*/

EncPacket* getFreePacket()
{
  int i, lastfree;
  lastfree =  -1;
  for (i = 0; i < MAX_OUTPUT_PACKETS; i++)
  {
    if (packets[i].unused)
    {
	lastfree = i;
	if (packets[i].data)
	{
	    return_buffer(packets[i].data);
	}
	packets[i].data = 0;
	if (packets[i].stats_buf)
	    free(packets[i].stats_buf);
	packets[i].stats_buf = 0;
	packets[i].stats_buf_len = 0;
    }
  }
  return (lastfree >= 0) ? &(packets[lastfree]) : 0;
}

/*----------------------------------------------------------------------------------*/

s32 enqueue_output_packets(struct packet_queue_t* opq, 
			   EncPacket** outputDest,
			   u32* numOutPackets,
			   u32 limit,
			   u32* packet_count)
{
  int i;
  EncPacket* outPacket;
  
  for (i = 0; i < opq->num_entries; i++)
  {
    if ((outPacket = getFreePacket()) == 0)
    {
      API_LOG_STDERR("Out of free packets\n");
      return ENC_ERR_OUTOFOPS;
    }
    outPacket->unused = false;
    outPacket->packet_no = (*packet_count)++;
    outPacket->data = opq->entries[i].packet;
    outPacket->packet_len = opq->entries[i].length;
    outputDest[*numOutPackets] = outPacket;
    (*numOutPackets)++;
    if (*numOutPackets >= limit)
    {
      API_LOG_STDERR("Output packet array is full %d >= %d\n",*numOutPackets,limit);
      return ENC_ERR_OUTOFOPS;
    }	
  }

  return ENC_ERR_NONE;  
}

/*----------------------------------------------------------------------------------*/

s32 init_arc_encoder_api()
{
    int i;
#ifdef __ARC_LINUX__
  init_linux_support();
  impif = &linux_impif;
#else
  init_standalone_support();
  impif = &standalone_impif;
#endif

  for (i = 0; i < MAX_OUTPUT_PACKETS; i++)
  {
    packets[i].unused = true;
    packets[i].data = (unsigned char*) 0;
    packets[i].packet_len = 0;
    packets[i].stats_buf = 0;
    packets[i].stats_buf_len = 0;
  }

  if ((freedescptr[0] = (MemBlkDesc*) malloc(sizeof(MemBlkDesc))) != 0)
      return ENC_ERR_FAILED;
  freedescptr[1] = (MemBlkDesc*) 0;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 uninit_arc_encoder_api()
{
  int i;

  for (i = 0; i < MAX_OUTPUT_PACKETS; i++)
  {
    packets[i].unused = false;
    free(packets[i].stats_buf);
  }

  free(freedescptr[0]);

#ifdef __ARC_LINUX__
  deinit_linux_support();
#else
  deinit_standalone_support();
#endif

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

