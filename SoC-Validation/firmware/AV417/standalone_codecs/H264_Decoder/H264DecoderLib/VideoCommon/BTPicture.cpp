/* ------------------------------------------------------------------------ */
/* FILE:			BTPicture.h												*/
/* DESCRIPTION:		BT Picture Class										*/
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
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec4")
#endif

#ifndef __SYMBIAN32__
	#include <string.h>  // For memset
#endif

#ifdef H264_MEMORY_MONITOR
#include "..\H264Common\H264Warnings.h"
#include "..\H264Common\H264MemoryMonitor.h"
#endif

#include	"BTPicture.h"

#include "memctl.h"

#ifdef AURORA_DMA
#include "ARCMedia.h"
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture													*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CBTPicture::CBTPicture	(void)
{
	m_ts		= 0;
	m_hsize		= 0;
	m_vsize		= 0;
	m_size		= 0;
	m_y			= 0;
	m_u			= 0;
	m_v			= 0;
	m_data		= 0;
	//cropped image support
	m_hSizeOutput = 0;
	m_vSizeOutput = 0;
	//m_yOutput = 0;		/* pointer to Y pels */
	//m_uOutput = 0;		/* pointer to U pels */
	//m_vOutput = 0;		/* pointer to V pels */
	m_yStride = 0;
	m_uStride = 0;
	m_vStride = 0;

#if defined __ARC_SHARED_BUFFER__
	beingDisplayed = false;
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture													*/
/* DESCRIPTION:	copy constructor											*/
/* ------------------------------------------------------------------------ */

CBTPicture::CBTPicture	(const CBTPicture& other)
{
	m_ts		= 0;
	m_hsize		= 0;
	m_vsize		= 0;
	m_size		= 0;
	m_y			= 0;
	m_u			= 0;
	m_v			= 0;
	m_data		= 0;
	//cropped image support
	m_hSizeOutput = 0;
	m_vSizeOutput = 0;
	//m_yOutput = 0;		/* pointer to Y pels */
	//m_uOutput = 0;		/* pointer to U pels */
	//m_vOutput = 0;		/* pointer to V pels */
	m_yStride = 0;
	m_uStride = 0;
	m_vStride = 0;

	copy(other);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	~CBTPicture													*/
/* DESCRIPTION:	destructor													*/
/* ------------------------------------------------------------------------ */

CBTPicture::~CBTPicture	(void)
{
#if !defined __ARC_LINUX__ && !defined __ARC_SHARED_BUFFER__
  h264_mem_freeall((void *)m_data);
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture::set_size											*/
/* DESCRIPTION:	sets size of picture										*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CBTPicture::set_size	(long hsize, long vsize, int numBuffers)
{
 	if (m_data != 0)
	{
		if ((m_hsize != hsize)  || (m_vsize != vsize))
		{
			h264_mem_freeall((void *)m_data);
			m_data = 0;
		}
	}

	if (m_data == 0)
	{
		pel_t	*aligned_frame;
		pel_t	*next_aligned_add;
		m_size  = (hsize*vsize*3)/2;
		m_hsize	= hsize;
		m_vsize	= vsize;

		//get 256 byte aligned data
        	h264_mem_setsize(hsize,vsize,numBuffers);
		m_data = (pel_t *)h264_mem_alloc( sizeof(pel_t)*(255 + m_size) ,256);
		aligned_frame = m_data;
		m_y		= aligned_frame;
		m_u		= &aligned_frame[m_hsize*m_vsize];

		next_aligned_add = m_u + (m_hsize*m_vsize) / 4; //(pel_t *)((((unsigned int)(&aligned_frame[(m_hsize*m_vsize*5)/4])+255)>>8)<<8);

		m_v			= next_aligned_add;
		//m_v		= &aligned_frame[(m_hsize*m_vsize*5)/4];
#if defined AURORA_DMA && !defined __ARC_LINUX__
        	FlushDCache();
#endif
	}

	//cropped image support
	m_hSizeOutput = m_hsize;
	m_vSizeOutput = m_vsize;
	//m_yOutput = m_y;		/* pointer to Y pels */
	//m_uOutput = m_u;		/* pointer to U pels */
	//m_vOutput = m_v;		/* pointer to V pels */
	m_yStride = m_hsize;
	m_uStride = m_hsize / 2;
	m_vStride = m_hsize / 2;
	m_xOffset = 0;
	m_yOffset = 0;

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture::copy												*/
/* DESCRIPTION:	copies picture												*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

void   CBTPicture::copy (const CBTPicture &src)
{
	set_size(src.m_hsize,src.m_vsize,0);

	memcpy(m_data,src.m_data,src.m_size);

	m_fn	= src.m_fn;
	m_ts	= src.m_ts;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture::copy												*/
/* DESCRIPTION:	copies picture												*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

void   CBTPicture::copy (CBTPicture* src)
{
	set_size(src->m_hsize,src->m_vsize,0);

	memcpy(m_data,src->m_data,src->m_size);

	m_fn	= src->m_fn;
	m_ts	= src->m_ts;
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture::copy												*/
/* DESCRIPTION:	copies picture												*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

void CBTPicture::copy (long hsize, long vsize, pel_t* y, pel_t* u, pel_t* v)
{
	long size = hsize*vsize;

	set_size(hsize,vsize,0);

	memcpy(m_y,y,size);
	memcpy(m_u,u,size/4);
	memcpy(m_v,v,size/4);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CBTPicture::crop												*/
/* DESCRIPTION:	Copies part of the given picture into the member picture.
				frame_crop_left_offset, frame_crop_right_offset,
				frame_crop_top_offset, and frame_crop_bottom_offset specify
				the samples of a frame within a rectangle as follows.
				The output picture contains luma samples with horizontal
				coordinates from 2 * frame_crop_left_offset to
				m_hsize - (2 * frame_crop_right_offset + 1) and vertical
				coordinates from 2 * frame_crop_top_offset to
				m_vsize - (2 * frame_crop_bottom_offset + 1), inclusive.	*/
/* RETURNS:		none														*/
/* ------------------------------------------------------------------------ */

void CBTPicture::crop(CBTPicture* src,
					unsigned int frame_cropping_rect_left_offset,
					unsigned int frame_cropping_rect_right_offset,
					unsigned int frame_cropping_rect_top_offset,
					unsigned int frame_cropping_rect_bottom_offset)
{
	long hsize = src->m_hsize - ((frame_cropping_rect_left_offset + frame_cropping_rect_right_offset) << 1);
	long vsize = src->m_vsize - ((frame_cropping_rect_top_offset + frame_cropping_rect_bottom_offset) << 1);

	long uv_hsize = hsize >> 1;
	long uv_vsize = vsize >> 1;

	set_size(hsize, vsize,0);

	long y;

	pel_t* y_out_ptr = m_y;
	pel_t* y_in_ptr = src->m_y + src->m_hsize * (frame_cropping_rect_top_offset << 1);

	pel_t* u_out_ptr = m_u;
	pel_t* u_in_ptr = src->m_u + ((src->m_hsize * frame_cropping_rect_top_offset) >> 1);

	pel_t* v_out_ptr = m_v;
	pel_t* v_in_ptr = src->m_v + ((src->m_hsize * frame_cropping_rect_top_offset) >> 1);

	// Copy luma
	for (y = 0; y < vsize; y++)
	{
		memcpy(y_out_ptr, y_in_ptr + (frame_cropping_rect_left_offset << 1), hsize);

		y_out_ptr += hsize;

		y_in_ptr += src->m_hsize;
	}

	// Copy chroma
	for (y = 0; y < uv_vsize; y++)
	{
		memcpy(u_out_ptr, u_in_ptr + frame_cropping_rect_left_offset, uv_hsize);
		memcpy(v_out_ptr, v_in_ptr + frame_cropping_rect_left_offset, uv_hsize);

		u_out_ptr += uv_hsize;
		v_out_ptr += uv_hsize;

		u_in_ptr += (src->m_hsize >> 1);
		v_in_ptr += (src->m_hsize >> 1);
	}

	m_fn	= src->m_fn;
	m_ts	= src->m_ts;
}

void CBTPicture::SetCroppingInformation(unsigned int frame_cropping_rect_left_offset,
					unsigned int frame_cropping_rect_right_offset,
					unsigned int frame_cropping_rect_top_offset,
					unsigned int frame_cropping_rect_bottom_offset)
{
	m_hSizeOutput = m_hsize - ((frame_cropping_rect_left_offset + frame_cropping_rect_right_offset) << 1);
	m_vSizeOutput = m_vsize - ((frame_cropping_rect_top_offset + frame_cropping_rect_bottom_offset) << 1);
	/*
	m_yOutput = m_y + m_hsize * (frame_cropping_rect_top_offset << 1) + (frame_cropping_rect_left_offset << 1);
	m_uOutput = m_u + ((m_hsize * frame_cropping_rect_top_offset) >> 1) + frame_cropping_rect_left_offset;
	m_vOutput = m_v + ((m_hsize * frame_cropping_rect_top_offset) >> 1) + frame_cropping_rect_left_offset;
	*/
	m_xOffset = frame_cropping_rect_left_offset << 1;
	m_yOffset = frame_cropping_rect_top_offset << 1;
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif