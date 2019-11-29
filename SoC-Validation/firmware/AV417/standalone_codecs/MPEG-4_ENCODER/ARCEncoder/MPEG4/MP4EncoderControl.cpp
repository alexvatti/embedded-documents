/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */
/*                                                                 */
/* May contain code (c) British Telecommunications plc 2005        */

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "MP4EncoderControl.h"
#include "picture_encoder.h"
#include "ArcEEOffsetTables.h"
#include "ArcMotionEstimationModel.h"
#include "ArcChannelRoutines.h"

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
	if ((m_hsize != hsize) || (m_vsize != vsize))
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