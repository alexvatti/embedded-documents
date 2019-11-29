
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
 * management_if.c                                                 *
 *                                                                 *
 * ARC Video Encoder management API implementation                 *
 *                                                                 */

/*----------------------------------------------------------------------------------*/

#include <arc_encoder.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

struct encoder_fn_desc
{
  encoder_factory_fn add;
  encoder_release_fn release;
  struct encoder_fn_desc *next;
};

struct argument_fn_desc
{
  argument_factory_fn create;
  argument_destructor_fn destroy;
  struct argument_fn_desc *next;
};

static struct encoder_fn_desc *encoder_factories = NULL;
static struct argument_fn_desc *argument_factories = NULL;

/*----------------------------------------------------------------------------------*/

encoder_interface* create_encoder (u32 encoder_type, 
				   u32 handle)
{
  struct encoder_fn_desc *descptr;
  encoder_factory_fn fnptr;
  encoder_interface *tmpif;

  for (descptr = encoder_factories; descptr; descptr = descptr->next)
  {
      if ((fnptr = descptr->add) != NULL)
	  if ((tmpif = fnptr (encoder_type, handle)) != NULL)
	      return tmpif;
  }

  return NULL;
}

/*----------------------------------------------------------------------------------*/

s32 release_encoder (u32 encoder_type, 
		     u32 handle)
{
  struct encoder_fn_desc *descptr;
  encoder_release_fn fnptr;
  s32 tmpres;

  for (descptr = encoder_factories; descptr; descptr = descptr->next)
  {
    if ((fnptr = descptr->release) != NULL)
      if ((tmpres = fnptr (encoder_type, handle)) == ENC_ERR_NONE)
	return tmpres;
  }

  return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/

s32 register_encoder_factory_fns (encoder_factory_fn ctor,
				  encoder_release_fn dtor)
{
  struct encoder_fn_desc *descptr;

  if ((descptr =
       (struct encoder_fn_desc *) malloc (sizeof (struct encoder_fn_desc))) == NULL)
    return ENC_ERR_OOM;

  descptr->add = ctor;
  descptr->release = dtor;
  descptr->next = encoder_factories;
  encoder_factories = descptr;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 remove_encoder_factory_fns (encoder_factory_fn ctor, 
				encoder_release_fn dtor)
{
  struct encoder_fn_desc *descptr, *prev;

  for (descptr = encoder_factories, prev = NULL; descptr;
       descptr = descptr->next)
  {
    if ((descptr->add == ctor) && (descptr->release == dtor))
    {
      if (prev != NULL)
	prev->next = descptr->next;
      else
	encoder_factories = descptr->next;
      free (descptr);
      return ENC_ERR_NONE;
    }
    prev = descptr;
  }

  return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/

static EncArgument* core_create_argument(u32 typecode)
{
	switch (typecode)
	{
	    case ENC_ARGTYPE_FUNDAMENTAL:
 	        {
		    ArgFundamental* tmpptr;
		    if ((tmpptr = (ArgFundamental*) malloc(sizeof(ArgFundamental))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_FUNDAMENTAL, sizeof(ArgFundamental));
			tmpptr->num_entries = 0;
			tmpptr->array=0;
			return &(tmpptr->typeinfo);
		    }
		}
		break;

	    case ENC_ARGTYPE_MEMBLKDESC:
 	        {
		    MemBlkDesc* tmpptr;
		    if ((tmpptr = (MemBlkDesc*) malloc(sizeof(MemBlkDesc))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_MEMBLKDESC, sizeof(MemBlkDesc));
			memset(tmpptr,0,sizeof(MemBlkDesc));
			return &(tmpptr->typeinfo);
		    }
		}
		break;
	    case ENC_ARGTYPE_CHARSTAR:
 	        {
		    ArgCharstar* tmpptr;
		    if ((tmpptr = (ArgCharstar*) malloc(sizeof(ArgCharstar))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_CHARSTAR, sizeof(ArgCharstar));
			memset(tmpptr,0,sizeof(ArgCharstar));
			return &(tmpptr->typeinfo);
		    }
		}
		break;
	    case ENC_ARGTYPE_STREAMDESC:
 	        {
		    StreamDescriptor* tmpptr;
		    if ((tmpptr = (StreamDescriptor*) malloc(sizeof(StreamDescriptor))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_STREAMDESC, sizeof(StreamDescriptor));
			memset(tmpptr,0,sizeof(StreamDescriptor));
			return &(tmpptr->typeinfo);
		    }
		}
		break;
	    case ENC_ARGTYPE_FRAMEDESC:
 	        {
		    FrameDescriptor* tmpptr;
		    if ((tmpptr = (FrameDescriptor*) malloc(sizeof(FrameDescriptor))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_FRAMEDESC, sizeof(FrameDescriptor));
			memset(tmpptr,0,sizeof(FrameDescriptor));
			return &(tmpptr->typeinfo);
		    }
		}
		break;
	    case ENC_ARGTYPE_RAWFRAME:
 	        {
		    RawFrame* tmpptr;
		    if ((tmpptr = (RawFrame*) malloc(sizeof(RawFrame))) != NULL)
		    {
			memset(tmpptr,0,sizeof(RawFrame));
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_RAWFRAME, sizeof(RawFrame));
			return &(tmpptr->typeinfo);
		    }
		}
		break;

	    case ENC_ARGTYPE_ENCPACKET:
 	        {
		    EncPacket* tmpptr;
		    if ((tmpptr = (EncPacket*) malloc(sizeof(EncPacket))) != NULL)
		    {
			SETUP_ENCARGPTR(tmpptr,ENC_ARGTYPE_ENCPACKET, sizeof(EncPacket));
			memset(tmpptr,0,sizeof(EncPacket));
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

static s32 core_destroy_argument(u32 typecode,
				 EncArgument* victim)
{
	if ((victim == NULL) || (victim->magic != MGMT_IF_ALLOC_MAGIC))
		return ENC_ERR_BADARG;

	victim->magic = 0;

	switch (typecode)
	{
	    case ENC_ARGTYPE_FUNDAMENTAL:
	    case ENC_ARGTYPE_MEMBLKDESC:
	    case ENC_ARGTYPE_CHARSTAR:
	    case ENC_ARGTYPE_STREAMDESC:
	    case ENC_ARGTYPE_FRAMEDESC:
	    case ENC_ARGTYPE_RAWFRAME:
	    case ENC_ARGTYPE_ENCPACKET:
		free(victim);
		break;

	    default:
		return ENC_ERR_FAILED;
	}

	return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

EncArgument* create_argument (u32 typecode)
{
  struct argument_fn_desc *descptr;
  argument_factory_fn fnptr;
  EncArgument *tmparg;

  for (descptr = argument_factories; descptr; descptr = descptr->next)
  {
    if ((fnptr = descptr->create) != NULL)
      if ((tmparg = fnptr (typecode)) != NULL)
	return tmparg;
  }

  return core_create_argument(typecode);
}

/*----------------------------------------------------------------------------------*/

s32 destroy_argument (EncArgument * victim)
{
    struct argument_fn_desc *descptr;
    argument_destructor_fn fnptr;
    s32 tmpres;
    u32 typecode;

    if (! victim) return ENC_ERR_FAILED;

    typecode = victim->type_id;

    for (descptr = argument_factories; descptr; descptr = descptr->next)
    {
	if ((fnptr = descptr->destroy) != NULL)
	    if ((tmpres = fnptr (typecode, victim)) == ENC_ERR_NONE)
		return tmpres;
    }

    return core_destroy_argument(typecode,victim);
}

/*----------------------------------------------------------------------------------*/

s32 register_argument_factory_fns (argument_factory_fn ctor,
				   argument_destructor_fn dtor)
{
  struct argument_fn_desc *descptr;

  if ((descptr =
       (struct argument_fn_desc *) malloc (sizeof (struct argument_fn_desc))) == NULL)
    return ENC_ERR_OOM;

  descptr->create = ctor;
  descptr->destroy = dtor;
  descptr->next = argument_factories;
  argument_factories = descptr;

  return ENC_ERR_NONE;
}

/*----------------------------------------------------------------------------------*/

s32 remove_argument_factory_fns (argument_factory_fn ctor,
				 argument_destructor_fn dtor)
{
  struct argument_fn_desc *descptr, *prev;

  for (descptr = argument_factories, prev = NULL; 
       descptr;
       descptr = descptr->next)
  {
    if ((descptr->create == ctor) && (descptr->destroy == dtor))
    {
      if (prev != NULL)
	prev->next = descptr->next;
      else
	argument_factories = descptr->next;
      free (descptr);
      return ENC_ERR_NONE;
    }
    prev = descptr;
  }

  return ENC_ERR_FAILED;
}

/*----------------------------------------------------------------------------------*/

