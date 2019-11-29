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
 * arc_h264.c
 *
 * December 2005.
 */
#ifdef I16K_D8K
#pragma Code("codesec3")
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdint.h>


#include "H264Decoder.h"
#include "arc_codecs.h"


typedef struct tag_framestore {
  int numFrames;
  int nextFrame;
  void **pictures;
  struct tag_framestore *next;
} FrameStore;

/******************************************************************************
   Function prototypes
******************************************************************************/
//functions used to construct the interface
static int h264_control 		(int, void *);
static int h264_set_buffer_addresses 	(unsigned long, unsigned long *,unsigned long *);
static int h264_init 			(int, char *, int, int, int, int);
static int h264_decode 			(char *,int,int,int *);
static int h264_get_frame_data		(arc_frame_data *);
static int h264_uninit 			(void);

extern int imageWidth;
extern int imageHeight;

/******************************************************************************
   Function pointer interface to the wrapper
******************************************************************************/

arc_codec_functions h264_functions = {
  &h264_init,
  &h264_set_buffer_addresses,
  &h264_control,
  &h264_decode,
  &h264_get_frame_data,
  &h264_uninit,
  NULL, //set_sizes
  NULL  //read_callback
};

/******************************************************************************
   Global/static data
******************************************************************************/

static int handle = -1;
static unsigned long SDMBaseAddress = 0;
static unsigned long *virtualAddresses = NULL;
static unsigned long *physicalAddresses = NULL;
static void *lastPicture = NULL;
static int resyncStream = 0;
static FrameStore *frameStore = NULL;
static FrameStore *frameStoreTail = NULL;
static int numFramesToBeDisplayed = 0;

#if !defined __ARC_SHARED_BUFFER__
//temporary buffer used for transport, if not using the shared buffer model
static char frameData[1000000];
#endif

/******************************************************************************
   H.264 decoder control
******************************************************************************/

// to set/get/query special features/parameters
static int h264_control (int cmd, void* arg)
{
  if (cmd == VDCTRL_RESYNC_STREAM)
  {
    resyncStream = 1;
    //clear the waiting frame display store
    while (frameStore != NULL)
    {
      frameStoreTail = frameStore->next;
      if (frameStore->pictures != NULL)
      {
#if defined __ARC_SHARED_BUFFER__
        int i;
        for (i=0; i<frameStore->numFrames; i++)
          f_h264_clear_being_displayed (frameStore->pictures[i]);
#endif	  
        free(frameStore->pictures);
      }
      free (frameStore);
      frameStore = frameStoreTail;
      numFramesToBeDisplayed = 0;
    }
    return CONTROL_TRUE;
  }
  return CONTROL_UNKNOWN;
}

/******************************************************************************
   H.264 decoder initialisation functions
******************************************************************************/

//set the buffer addresses to be used by the codec
static int h264_set_buffer_addresses (unsigned long SDMBase, unsigned long *vAddresses,unsigned long *pAddresses)
{
  if (vAddresses != NULL)
  {
    virtualAddresses = vAddresses;
    if (pAddresses != NULL)
      physicalAddresses = pAddresses;
    h264_init_memctl(vAddresses,pAddresses);
  }
  if (SDMBaseAddress == 0)
  {
    SDMBaseAddress = SDMBase;
#if !defined __BENCHMARK__
      fprintf(stderr,"\nSDM has been initialised, mapping to %p.\n", SDMBaseAddress);
#endif
  }
  return (1);
}


// Initialise the codec, using only the type and width/height information
int h264_init (int type,char *header_data,int header_size,int header_offset, int w, int h)
{
  //check the type
  if (type != ARC_H264)
  {
#if !defined __BENCHMARK__
    fprintf(stderr,"\nInvalid codec type %u",type);
#endif
    return (0);
  }

#if defined __ARC_LINUX__
  handle = f_h264_decoder_construct((void *)SDMBaseAddress);
#else
  handle = f_h264_decoder_construct();
#endif
  if (handle == -1)
    return (0);
  return 1;
}


/******************************************************************************
   H.264 decoder de-initialisation
******************************************************************************/

// uninit driver
static int h264_uninit (void)
{
  if (handle != -1)
    f_h264_decoder_destruct(handle);
  //clean up frame stores
  while (frameStore != NULL)
  {
    frameStoreTail = frameStore->next;
    if (frameStore->pictures != NULL)
      free(frameStore->pictures);
    free (frameStore);
    frameStore = frameStoreTail;
    numFramesToBeDisplayed = 0;
  }
  return (1);
}

/******************************************************************************
   H.264 decoder decode functions
******************************************************************************/

static int verb = 0;

// decode a frame
static int h264_decode (char *data,int len,int offset,int *num_frames)
{
  int result;
  h264_decoder_parameters *decParams = NULL;
#if 0
  if (len >= 4)
  {
    unsigned char *udata = (unsigned char *)data;
    fprintf (stderr,"\nLen %X: Data %.2X %.2X %.2X %.2X : %.2X %.2X %.2X %.2X",len,udata[0],udata[1],udata[2],udata[3],
             udata[len-4],udata[len-3],udata[len-2],udata[len-1]);
  }
#endif
  //check the unit type
  if (resyncStream)
  {
    /*
    if (len >= 4)
    {
      unsigned char *udata = (unsigned char *)data;
      fprintf (stderr,"\nLen %X: Data %.2X %.2X %.2X %.2X : %.2X %.2X %.2X %.2X",len,udata[0],udata[1],udata[2],udata[3],
               udata[len-4],udata[len-3],udata[len-2],udata[len-1]);
    }
    else
      fprintf(stderr,"\nLen %X",len);
    */
    if ((data[0] & 0x1F) == 5)
    {
      result = f_h264_decoder_decode(handle,data,len,resyncStream,1,1,0,0,&decParams);
      resyncStream = 0; //only required for a single frame
      *num_frames = decParams->bDisplayFrame;
    }
    else
    {
      *num_frames = 0;
      result = 1;
    }
  }
  else
  {
    result = f_h264_decoder_decode(handle,data,len,0,1,1,0,0,&decParams);
    *num_frames = decParams->bDisplayFrame;
  }
  //fprintf(stderr,"\nNum frames %u",decParams->bDisplayFrame);
  //save the returned information
  if (*num_frames != 0)
  {
    int i;
    FrameStore *fs = (FrameStore *)malloc(sizeof(FrameStore));
    if (fs == NULL)
    {
      fprintf(stderr,"\nMalloc failure, h264_decode");
      return (0);
    }
    fs->numFrames = *num_frames;
    fs->nextFrame = 0;
    fs->next = NULL;
    fs->pictures = (void **)malloc(sizeof(void *) * fs->numFrames);
    if (fs->pictures == NULL)
    {
      fprintf(stderr,"\nMalloc failure, h264_decode, pictures");
      return (0);
    }
    for (i=0; i<*num_frames; i++)
      fs->pictures[i] = (void *)decParams->pPicture[i];
    if (frameStore == NULL)
      frameStore = fs;
    else
      frameStoreTail->next = fs;
    frameStoreTail = fs;
    numFramesToBeDisplayed += *num_frames;
  }
  //*num_frames = numFramesToBeDisplayed;
  return (result);
}

static int h264_get_frame_data (arc_frame_data *frameData)
{
  void *picture = NULL;
  //determine the picture to output
  if (frameStore == NULL)
  {
#if !defined __BENCHMARK__
    fprintf(stderr,"\nInvalid call to get_frame_data - no data to get");
#endif
    return (0);
  }
  //check if all the pictures in the current frame have been shown
  if (frameStore->nextFrame == frameStore->numFrames)
  {
    FrameStore *tmp = frameStore->next;
    if (frameStore->pictures != NULL)
      free(frameStore->pictures);
    free(frameStore);
    frameStore = tmp;    
    if (tmp == NULL)
    {
      frameStoreTail = NULL;
#if !defined __BENCHMARK__
      fprintf(stderr,"\nInvalid call to get_frame_data - no data to get");
#endif
      return (0);
    }
  }
  picture = frameStore->pictures[frameStore->nextFrame++];
#if defined __ARC_SHARED_BUFFER__
  f_h264_get_data(lastPicture,picture,frameData);
#else
  f_h264_copy_data(picture,frameData);
#endif
  lastPicture = picture;
  numFramesToBeDisplayed--;
  return (1);
}

#ifdef I16K_D8K
#pragma Code()
#endif