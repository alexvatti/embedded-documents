/* ------------------------------------------------------------------------ */
/* FILE:			BTPicture.h												*/
/* DESCRIPTION:		BT Picture Class										*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
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

#ifndef _BT_PICTURE_H_
#define _BT_PICTURE_H_

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#endif

/* ------------------------------------------------------------------------ */
/* TYPE DEFINITIONS															*/
/* ------------------------------------------------------------------------ */

typedef unsigned long int	timestamp_t;
typedef unsigned long int	framenum_t;
typedef unsigned char		pel_t;

typedef struct BTPicture_tag {
	framenum_t		m_fn;			/* frame number */
	timestamp_t		m_ts;			/* timestamp of picture in 90KHZ units */
	long			m_hsize;		/* horizontal picture size */
	long			m_vsize;		/* vertical picture size */
	pel_t*			m_y;			/* pointer to Y pels */
	pel_t*			m_u;			/* pointer to U pels */
	pel_t*			m_v;			/* pointer to V pels */
    pel_t*			m_data;			/* pointer to top of allocated memory */
	long			m_size;			/* size of allocated memory */	
} BTPicture;

#if defined(__cplusplus)

/* ------------------------------------------------------------------------ */
/* CLASS CBTPicture															*/
/* ------------------------------------------------------------------------ */

class CBTPicture : public BTPicture
{
public:
	CBTPicture		(void);
	CBTPicture		(const CBTPicture& other);
	~CBTPicture		(void);

    void set_yuv_buffers(pel_t *y= 0,pel_t *u=0,pel_t *v=0);
    void set_source_frame_size(long hsize, long vsize);

	void			set_size	(long hsize, long vsize);
	
	void			copy		(const CBTPicture &src);
	void			copy		(CBTPicture* src);
	void			copy		(long hsize, long vsize, pel_t* y, pel_t* u, pel_t* v);

	void			crop(CBTPicture* src,
						 unsigned int frame_cropping_rect_left_offset,
						 unsigned int frame_cropping_rect_right_offset,
						 unsigned int frame_cropping_rect_top_offset,
						 unsigned int frame_cropping_rect_bottom_offset);
};

#endif // __cpluspluz
#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
