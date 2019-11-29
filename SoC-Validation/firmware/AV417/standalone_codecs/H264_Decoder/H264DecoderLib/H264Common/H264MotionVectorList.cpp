/* ------------------------------------------------------------------------ */
/* FILE:			H264MotionVectorList.cpp								*/
/* DESCRIPTION:		BT H.264 motion vector list source file.				*/
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
#ifdef __B_PICTURES__


#include "H264Warnings.h"
#include "H264MemoryMonitor.h"

#include "H264MotionVectorList.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::CH264MotionVectorList				*/
/* DESCRIPTION:	Constructor													*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

CH264MotionVectorList::CH264MotionVectorList(void)
{
	m_NumberOfMacroblocks = 0;
	m_ListHeader = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::~CH264MotionVectorList				*/
/* DESCRIPTION:	Destructor													*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

CH264MotionVectorList::~CH264MotionVectorList(void)
{
	DeleteList();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::Create								*/
/* DESCRIPTION:	Allocates the motion vector list							*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

bool CH264MotionVectorList::SetSize(int number_of_macroblocks)
{
	if ((number_of_macroblocks != m_NumberOfMacroblocks) || (0 == m_ListHeader))
	{
		if (0 != m_ListHeader) 
		{
			DeleteList();
		}

		m_NumberOfMacroblocks = number_of_macroblocks;

		return CreateList();
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::CreateList							*/
/* DESCRIPTION:	Creates motion vector list									*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264MotionVectorList::CreateList()
{
	m_ListHeader = new mv_type[m_NumberOfMacroblocks];

	if (0 == m_ListHeader)
	{
		assert(false);
		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::DeleteList							*/
/* DESCRIPTION:	Creates motion vector list									*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264MotionVectorList::DeleteList()
{
	if (0 == m_ListHeader)
	{
		return;
	}

	delete [] m_ListHeader;
	m_ListHeader =  0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264MotionVectorList::GetFrameMacroblock					*/
/* DESCRIPTION:	Gets given element											*/
/* RETURNS:		given if OK, else zero										*/
/* ------------------------------------------------------------------------ */

mv_type * CH264MotionVectorList::GetFrameMacroblock(int macroblock_number)
{
	if ((0 == m_ListHeader) || (macroblock_number >= m_NumberOfMacroblocks))
	{
		return 0;
	}

	return &m_ListHeader[macroblock_number];
}


#endif


