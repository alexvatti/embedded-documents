/* ------------------------------------------------------------------------ */
/* FILE:			H264ParameterSets.cpp									*/
/* DESCRIPTION:		Parameter sets definitions for H.264					*/
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

#include "H264EnumTypes.h"
#include "H264Warnings.h"

#include "H264ParameterSets.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::CH264ParameterSets						*/
/* DESCRIPTION:	Constructor
				This class manages the storage of sequence and picture 
				parameter sets. It assumes maximum numbers of each type of
				parameter set, and assumes that a parameter set can be 
				stored in a fixed amount of memory.							*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

CH264ParameterSets::CH264ParameterSets(void)
{
	int i;

	for (i=0; i<NUMBER_OF_SEQUENCE_PARAMETER_SETS; i++)
	{
		m_SequenceParameterSets[i] = 0;
	}

	for (i=0; i<NUMBER_OF_PICTURE_PARAMETER_SETS; i++)
	{
		m_PictureParameterSets[i] = 0;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::~CH264ParameterSets						*/
/* DESCRIPTION:	Destructor													*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

CH264ParameterSets::~CH264ParameterSets(void)
{
	int i;

	for (i=0; i<NUMBER_OF_SEQUENCE_PARAMETER_SETS; i++)
	{
		if (m_SequenceParameterSets[i] != 0) DeleteSequenceParameterSet(i);
	}

	for (i=0; i<NUMBER_OF_PICTURE_PARAMETER_SETS; i++)
	{
		if (m_PictureParameterSets[i] != 0) DeletePictureParameterSet(i);
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::WriteSequenceParameterSet				*/
/* DESCRIPTION:	This returns a pointer to the given parameter set to allow 
				the calling function to write into that parameter set. If 
				memory has not already been allocated, then it is allocated. 
				In case of memory failure or an ID too large, zero is 
				returned.													*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

sequence_parameter_set_t * CH264ParameterSets::WriteSequenceParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_SEQUENCE_PARAMETER_SETS) return 0;

	if (0 == m_SequenceParameterSets[parameter_set_id])
	{
		m_SequenceParameterSets[parameter_set_id] = new sequence_parameter_set_t;

		// Ensure pointer to vui_parameters is zero
		m_SequenceParameterSets[parameter_set_id]->vui_parameters = 0;
	}

	return m_SequenceParameterSets[parameter_set_id];
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::WritePictureParameterSet				*/
/* DESCRIPTION:	This returns a pointer to the given parameter set to allow 
				the calling function to write into that parameter set. If 
				memory has not already been allocated, then it is allocated. 
				In case of memory failure or an ID too large, zero is 
				returned.													*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

picture_parameter_set_t * CH264ParameterSets::WritePictureParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_PICTURE_PARAMETER_SETS) return 0;

	if (0 == m_PictureParameterSets[parameter_set_id])
	{
		m_PictureParameterSets[parameter_set_id] = new picture_parameter_set_t;

		// Ensure pointer to slice_group_id is zero
		m_PictureParameterSets[parameter_set_id]->slice_group_id = 0;
	}

	return m_PictureParameterSets[parameter_set_id];
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::ReadSequenceParameterSet				*/
/* DESCRIPTION:	This returns a pointer to the given parameter set to allow 
				the calling function to read from that parameter set. If 
				memory has not already been allocated or an ID too large, 
				zero is returned.											*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

sequence_parameter_set_t * CH264ParameterSets::ReadSequenceParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_SEQUENCE_PARAMETER_SETS) return 0;

	return m_SequenceParameterSets[parameter_set_id];
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::ReadPictureParameterSet					*/
/* DESCRIPTION:	This returns a pointer to the given parameter set to allow 
				the calling function to read from that parameter set. If 
				memory has not already been allocated or an ID too large, 
				zero is returned.											*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

picture_parameter_set_t * CH264ParameterSets::ReadPictureParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_PICTURE_PARAMETER_SETS) return 0;

	return m_PictureParameterSets[parameter_set_id];
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::DeleteSequenceParameterSet				*/
/* DESCRIPTION:	This deletes the memory for the given parameter set if it
				has been allocated.											*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

void CH264ParameterSets::DeleteSequenceParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_SEQUENCE_PARAMETER_SETS) return;

	if (0 != m_SequenceParameterSets[parameter_set_id])
	{
		// Ensure vui_parameters is deleted if the pointer to it is non-zero
		if (0 != m_SequenceParameterSets[parameter_set_id]->vui_parameters)
		{
			delete m_SequenceParameterSets[parameter_set_id]->vui_parameters;
			m_SequenceParameterSets[parameter_set_id]->vui_parameters = 0;
		}

		delete m_SequenceParameterSets[parameter_set_id];
		m_SequenceParameterSets[parameter_set_id] = 0;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264ParameterSets::DeletePictureParameterSet				*/
/* DESCRIPTION:	This deletes the memory for the given parameter set if it
				has been allocated.											*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

void CH264ParameterSets::DeletePictureParameterSet(int parameter_set_id)
{
	if (parameter_set_id >= NUMBER_OF_PICTURE_PARAMETER_SETS) return;

	if (0 != m_PictureParameterSets[parameter_set_id])
	{
		// Ensure slice_group_id is deleted if the pointer to it is non-zero
		if (0 != m_PictureParameterSets[parameter_set_id]->slice_group_id)
		{
			delete m_PictureParameterSets[parameter_set_id]->slice_group_id;
			m_PictureParameterSets[parameter_set_id]->slice_group_id = 0;
		}

		delete m_PictureParameterSets[parameter_set_id];
		m_PictureParameterSets[parameter_set_id] = 0;
	}
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
