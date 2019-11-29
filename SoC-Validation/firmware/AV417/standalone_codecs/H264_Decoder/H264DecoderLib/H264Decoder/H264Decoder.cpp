/* ------------------------------------------------------------------------ */
/* FILE:			CH264Decoder.h											*/
/* DESCRIPTION:		BT H.264 video decoder source file.						*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include <string.h>

#include "memctl.h"
#include "vbuf.h"
#include "arc_codecs.h"

#include "H264Warnings.h"

#include "H264Decoder.h"
#include "CH264Decoder.h"
#include "picture_encoder.h"






//the one and only decoder object
static CH264Decoder decoder;
//static CH264Decoder *decoder = NULL;
//Wrapper function to return the image size of the supplied CPicture object
int EXPORTED f_h264_get_image_size (void *frame, int *w, int *h)
{
  CPicture *picture = (CPicture *)frame;
  *w = picture->m_hsize;
  *h = picture->m_vsize;
  return (0);
}

//Data copy function to extract the buffers from the supplied CPicture object to the set of arrays
int EXPORTED f_h264_copy_data (void *frame, arc_frame_data *frameData)
{
  CPicture *picture = (CPicture *)frame;
  //NOTE, currently this does a copy of the pixels to the allocated buffer in MPLayer, this should
  //be replaced with a re-allocation, based on integration yet to be done
  //fprintf(stderr,"\nStride %u %u %u",stride[0],stride[1],stride[2]);
  //fprintf(stderr,"\nOffsets %u %u %u %u",*x,*y,*w,*h);
  frameData->stride[0] = picture->m_yStride;
  frameData->stride[1] = picture->m_uStride;
  frameData->stride[2] = picture->m_vStride;
  long numpels = (picture->m_hsize * picture->m_vsize);
  memcpy(frameData->planes[0],picture->m_y,numpels);
  memcpy(frameData->planes[1],picture->m_u,numpels/4);
  memcpy(frameData->planes[2],picture->m_v,numpels/4);
#if defined AURORA_DMA && !defined __ARC_LINUX__
        	FlushDCache();
#endif
  
  frameData->xOffset = picture->m_xOffset;
  frameData->yOffset = picture->m_yOffset;
  frameData->width = picture->m_hSizeOutput;
  frameData->height = picture->m_vSizeOutput;
  //fprintf(stderr,"\nOffsets %u %u %u %u",*x,*y,*w,*h);
  //fprintf(stderr," Output Frame %u (%u,%u)",picture->m_index,picture->m_hsize,picture->m_vsize);
  return (0);
}


#if defined __ARC_SHARED_BUFFER__
int EXPORTED f_h264_get_data (void *oldframe,void *frame, arc_frame_data *frameData)
{
  CPicture *picture = (CPicture *)frame;
  CPicture *oldPicture = (CPicture *)oldframe;
  frameData->stride[0] = picture->m_yStride;
  frameData->stride[1] = picture->m_uStride;
  frameData->stride[2] = picture->m_vStride;
  frameData->planes[0] = picture->m_y;
  frameData->planes[1] = picture->m_u;
  frameData->planes[2] = picture->m_v;
  frameData->xOffset = picture->m_xOffset;
  frameData->yOffset = picture->m_yOffset;
  frameData->width = picture->m_hSizeOutput;
  frameData->height = picture->m_vSizeOutput;
  h264_get_frame_index((unsigned long)(picture->m_y),&frameData->index);
  vbuf_send(frameData->index);
  vbuf_free(frameData->index);
  // zero buffer pointers as ownership has been returned to codec
  picture->m_data = NULL;
  picture->m_y = NULL;
  picture->m_u = NULL;
  picture->m_v = NULL;
  if (oldPicture != NULL)
    oldPicture->SetBeingDisplayed(false);
  return (0);
}

void EXPORTED f_h264_clear_being_displayed (void *frame)
{
  CPicture *picture = (CPicture *)frame;
  if (picture != NULL)
    picture->SetBeingDisplayed(false);
}

#endif




/*
--------------------------------------------------------------------------------
Function f_h264_decoder_construct.
--------------------------------------------------------------------------------
*/

#if defined __ARC_LINUX__
int EXPORTED	f_h264_decoder_construct(void *SDMBase)
#else
int EXPORTED	f_h264_decoder_construct(void)
#endif
{
	int			result;


    //decoder = new CH264Decoder();
	/*	Open the decoder. */
	result	=	decoder.Create();

	if (result < 0)
	{
		return(H264_DECODER_VLD_MALLOC_ERROR);
	}

	/* Initialize Aurora */
#if defined __ARC_LINUX__
	if( !decoder.InitAurora(SDMBase) )
#else
	if( !decoder.InitAurora() )
#endif
	{
		return H264_DECODER_SIMD_INIT_ERROR;
	}

	return(0);
}

/*
--------------------------------------------------------------------------------
Function f_h264_decoder_destruct.
--------------------------------------------------------------------------------
*/

int EXPORTED	f_h264_decoder_destruct(int iHandle)
{
	decoder.Destroy();
    //delete decoder;

	h264_mem_freeall(NULL);

	return H264_DECODER_OK;
}

/*
--------------------------------------------------------------------------------
Function f_h264_decoder_decode.
--------------------------------------------------------------------------------
*/

int EXPORTED	f_h264_decoder_decode(	int iHandle, unsigned char* pBytes,
									    int iNumBytes, int bDataLoss,
										int bStartOfNALUnit, int bEndOfNALUnit, 
										int bDiscardIfDisposable,
										unsigned int time_stamp,
										h264_decoder_parameters **ppParameters)
{
  int i,ret;
  ret = decoder.Decode(pBytes, iNumBytes, bDataLoss != 0, bStartOfNALUnit != 0,
							bEndOfNALUnit != 0,
							bDiscardIfDisposable != 0, time_stamp, ppParameters);
#if defined __ARC_SHARED_BUFFER__
  for (i=0;i<(*ppParameters)->bDisplayFrame;i++)
  {
	int idx = -1;
    ((CBTPicture *)((*ppParameters)->pPicture[i]))->SetBeingDisplayed(true);
	h264_get_frame_index((unsigned long)((CBTPicture *)((*ppParameters)->pPicture[i]))->m_y, &idx);
  }
#endif
  return (ret);
}
/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
