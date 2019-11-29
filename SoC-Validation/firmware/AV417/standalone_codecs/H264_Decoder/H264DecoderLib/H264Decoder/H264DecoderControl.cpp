/* ------------------------------------------------------------------------ */
/* FILE:			H264DecoderControl.cpp									*/
/* DESCRIPTION:		H.264 decoder control class								*/
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

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
	#include <string.h>  // For memset
#endif

#include "H264Warnings.h"

#include "H264DecoderControl.h"
#include "BTPicture.h"
#include "H264OffsetTables.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::CH264DecoderControl					*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CH264DecoderControl::CH264DecoderControl(void)
{
	m_ControlList = 0;
	m_CurrentElement = 0;

	m_hsize = 0;
	m_vsize = 0;
	m_NumberOfMacroblocks = 0;

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::~CH264DecoderControl					*/
/* DESCRIPTION:	destructor													*/
/* ------------------------------------------------------------------------ */

CH264DecoderControl::~CH264DecoderControl(void)
{
	if (0 != m_ControlList) DeleteList();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::SetSize								*/
/* DESCRIPTION:	set control list size, and allocate memory for list if 
				different to current size.									*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264DecoderControl::SetSize(int hsize, int vsize)
{
	if ((m_hsize != hsize) || (m_vsize != vsize) || (0 == m_ControlList))
	{
		if (0 != m_ControlList) DeleteList();	

		m_hsize = hsize;
		m_vsize	= vsize;

		return CreateList();
	}
	else
	{
		// Just reset the list
		Reset(); 			
	}
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::Reset									*/
/* DESCRIPTION:	resets control list parameters								*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264DecoderControl::Reset(void)
{
	h264_decoder_control_t* control = m_CurrentElement = m_ControlList+ MAX_RES_IN_MB_X+3;

	while (0 != control)
	{
		ClearControl(control);
		control = control->next;
	}
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::DeleteList								*/
/* DESCRIPTION:	deletes control list										*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264DecoderControl::DeleteList(void)
{
	if (0 != m_ControlList)
	{
		delete [] m_ControlList;
		m_ControlList = 0;
	}

	m_CurrentElement = 0;

	m_hsize = 0;
	m_vsize = 0;
	m_NumberOfMacroblocks = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecoderControl::CreateList								*/
/* DESCRIPTION:	creates control list										*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264DecoderControl::CreateList(void)
{
	h264_decoder_control_t*	control = 0;
	// determine the number of macroblocks
	m_NumberOfMacroblocks = (m_hsize/16) * (m_vsize/16);

	m_MacroblocksPerRow = m_hsize/16;
	
	if( m_hsize/16 > MAX_RES_IN_MB_X || m_vsize/16 > MAX_RES_IN_MB_Y)
	{
		return false;	// Stream res too big
	}

	// Create control list in one block
	m_ControlList = new h264_decoder_control_t[MAX_TOTAL_MB];
	if (0 == m_ControlList)
		return false; // Memory allocation failure
		
	int mbNum = 0;
	control = m_ControlList;
	int x,y;
	h264_decoder_control_t **nextPtrLast = NULL;
	for( y = -1; y <= MAX_RES_IN_MB_Y; y++)
	{
		for(x = -1; x <= MAX_RES_IN_MB_X; x++)
		{
			control->PixelX = x << 4;
			control->PixelY = y << 4;
			control->Flags = 0;
			control->slice_id = INT_MAX;
			if(x >= 0 && y >= 0 && x < m_hsize/16 && y < m_vsize/16)
			{
				if(nextPtrLast)
				{	// Set last blocks next pointer to point at this block
					*nextPtrLast = control;
				}
				m_ControlPtrs[mbNum] = control;
				control->mb_number = mbNum++;
				nextPtrLast = &control->next;	// So this blocks Next can be set later
				
				if(x)
				{
					control->Flags |= CTLF_Left;
				}
				
				if(y)
				{
					control->Flags |= CTLF_Above;
				}
				
				if(x < (m_hsize/16)-1)
				{
					control->Flags |= CTLF_Right;
				}
				control->Self = control;
			}
			else
			{
				control->next = NULL;
				control->Self = NULL;
			}
		
			control++;			
		}
	}
	if(nextPtrLast)
	{
		*nextPtrLast = NULL;
	}
	
	// reset coding parameters
	Reset();

	return true;
}


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
