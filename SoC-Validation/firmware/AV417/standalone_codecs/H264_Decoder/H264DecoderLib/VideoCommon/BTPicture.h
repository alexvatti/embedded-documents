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

/* ------------------------------------------------------------------------ */
/* CLASS CBTPicture															*/
/* ------------------------------------------------------------------------ */

class CBTPicture
{
#if defined __ARC_SHARED_BUFFER__
protected:
	bool			beingDisplayed;
#endif
public:
	CBTPicture		(void);
	CBTPicture		(const CBTPicture& other);
	~CBTPicture		(void);


	void			set_size	(long hsize, long vsize, int numBuffers);
	
	void			copy		(const CBTPicture &src);
	void			copy		(CBTPicture* src);
	void			copy		(long hsize, long vsize, pel_t* y, pel_t* u, pel_t* v);

	void			crop(CBTPicture* src,
						 unsigned int frame_cropping_rect_left_offset,
						 unsigned int frame_cropping_rect_right_offset,
						 unsigned int frame_cropping_rect_top_offset,
						 unsigned int frame_cropping_rect_bottom_offset);
	void			SetCroppingInformation(unsigned int frame_cropping_rect_left_offset,
						 unsigned int frame_cropping_rect_right_offset,
						 unsigned int frame_cropping_rect_top_offset,
						 unsigned int frame_cropping_rect_bottom_offset);

#if defined __ARC_SHARED_BUFFER__
	inline void 		SetBeingDisplayed (bool d)	{ beingDisplayed = d; }
	inline bool		IsBeingDisplayed  (void)	{ return (beingDisplayed); }
#endif

public:
	framenum_t		m_fn;			/* frame number */
	timestamp_t		m_ts;			/* timestamp of picture in 90KHZ units */

	long			m_hsize;		/* horizontal picture size */
	long			m_vsize;		/* vertical picture size */

	pel_t*			m_y;			/* pointer to Y pels */
	pel_t*			m_u;			/* pointer to U pels */
	pel_t*			m_v;			/* pointer to V pels */
        //additional metrics required to support cropping
	long			m_hSizeOutput;		/* horizontal size of displayed image */
	long			m_vSizeOutput;		/* vertical size of displayed image */
	int			m_xOffset;		/* x offset of start of displayed image */
	int			m_yOffset;		/* y offset of start of displayed image */
	//pel_t*			m_yOutput;		/* pointer to Y pels */
	//pel_t*			m_uOutput;		/* pointer to U pels */
	//pel_t*			m_vOutput;		/* pointer to V pels */
	int			m_yStride;		/* stride of displayed image */
	int			m_uStride;
	int			m_vStride;


public:
	pel_t*			m_data;			/* pointer to top of allocated memory */
	long			m_size;			/* size of allocated memory */
};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
