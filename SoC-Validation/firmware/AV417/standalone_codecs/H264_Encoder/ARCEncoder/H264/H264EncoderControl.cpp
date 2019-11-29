/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderControl.cpp									*/
/* DESCRIPTION:		H.264 encoder control class								*/
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

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "H264EncoderControl.h"
#include "picture_encoder.h"
#include "H264OffsetTables.h"
#include "ArcChannelRoutines.h"
#include "defines.h"


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncoderControl::CH264EncoderControl					*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CH264EncoderControl::CH264EncoderControl(void)
{
	m_hsize = 0;
	m_vsize = 0;
	m_NumberOfMacroblocks = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncoderControl::~CH264EncoderControl					*/
/* DESCRIPTION:	destructor													*/
/* ------------------------------------------------------------------------ */

CH264EncoderControl::~CH264EncoderControl(void)
{
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncoderControl::SetSize								*/
/* DESCRIPTION:	set control list size, and allocate memory for list if 
				different to current size.									*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264EncoderControl::SetSize(int hsize, int vsize)
{
	if ((m_hsize != hsize) || (m_vsize != vsize) )
	{

		m_hsize = hsize;
		m_vsize	= vsize;
	}

	return true;
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncoderControl::DeleteList								*/
/* DESCRIPTION:	deletes control list										*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264EncoderControl::DeleteList(void)
{
	m_hsize = 0;
	m_vsize = 0;
	m_NumberOfMacroblocks = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncoderControl::CreateList								*/
/* DESCRIPTION:	creates control list										*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264EncoderControl::CreateList(void)
{
	// determine the number of macroblocks
	m_NumberOfMacroblocks = (m_hsize/16) * (m_vsize/16);

	m_MacroblocksPerRow = m_hsize/16;

	return true;
}


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
