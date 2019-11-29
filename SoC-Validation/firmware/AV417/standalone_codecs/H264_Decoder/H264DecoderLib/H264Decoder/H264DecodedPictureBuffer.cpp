/* ------------------------------------------------------------------------ */
/* FILE:			H264DecodedPictureBuffer.cpp							*/
/* DESCRIPTION:		Decoded Picture Buffer definitions for H.264			*/
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

#include "BTPicture.h"
#include "H264MotionVectorList.h"
#include "H264DecoderControl.h"

#include "H264DecodedPictureBuffer.h"

#include "H264OffsetTables.h"
#include "H264Tables.h"
#include "vbuf.h"

#define UNUSED_ARRAY_POSITION 127

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::CH264DecodedPictureBuffer		*/
/* DESCRIPTION:	Constructor
				This class manages the storage of reference pictures, and 
				non reference pictures awaiting output.
				It is intended for use in the decoder.						*/
/* ------------------------------------------------------------------------ */


CH264DecodedPictureBuffer::CH264DecodedPictureBuffer(void)
{
	m_NumberOfDecodedFrames = 0;
#if defined __ARC__
	//additional initialisation added to avoid runtime errors
	for (int i=0; i<MAXIMUM_NUMBER_OF_STORED_FRAMES; i++)
	{
	  m_TopWaitingToBeOutput[i] = false;
	  m_BottomWaitingToBeOutput[i] = false;	
	  m_ArrayPositionToLowestRemappedIndexList0[i*2] = UNUSED_ARRAY_POSITION;
	  m_ArrayPositionToLowestRemappedIndexList0[(i*2)+1] = UNUSED_ARRAY_POSITION;
//	  m_ArrayPositionToLowestRemappedIndexList1[i*2] = UNUSED_ARRAY_POSITION;
//	  m_ArrayPositionToLowestRemappedIndexList1[(i*2)+1] = UNUSED_ARRAY_POSITION;
	}
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::~CH264DecodedPictureBuffer		*/
/* DESCRIPTION:	Destructor													*/
/* ------------------------------------------------------------------------ */

CH264DecodedPictureBuffer::~CH264DecodedPictureBuffer(void)
{
	Destroy();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::Create							*/
/* DESCRIPTION:	Creation :
				This class is used for decoded picture buffering. 
				A number of pictures are stored within this class, and they 
				are managed using indices. This function allocates memory 
				for the requested number of decoded frames up to a maximum 
				of MAXIMUM_NUMBER_OF_STORED_FRAMES.
				number_of_reference_frames is no greater than this, and 
				indicates the maximum number of frames that can be use for 
				storing reference frames.									*/
/* RETURNS:		true if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264DecodedPictureBuffer::Create(int number_of_decoded_frames, 
									   int number_of_reference_frames)
{
	int i;
	m_NumberOfDecodedFrames = number_of_decoded_frames;
	m_NumberOfReferenceFrames = number_of_reference_frames;

	m_MaximumLongTermIndexPlus1 = 0;

	m_MMCO_WritePointer = 0;

	m_IDR_pic_flag = false;
	m_FlushNonReferencePictures = false;
	m_StoreIDRAsLongTerm = false;

	m_CurrentPictureLongTermIndex = UNUSED_ARRAY_POSITION;
	m_ArrayPositionOfCurrentPicture = 0;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		m_Picture[i] = new CBTPicture();

		if (0 == m_Picture[i])
		{
			// Memory allocation failure
			Destroy();
			return false;
		}

		m_TopWaitingToBeOutput[i] = false;
		m_BottomWaitingToBeOutput[i] = false;

		m_TopOutputBeforeIDR[i] = false;
		m_BottomOutputBeforeIDR[i] = false;
	}


	ProcessMMCOReset();

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::Modify							*/
/* DESCRIPTION:	Creation and Modification:
				This class is used for decoded picture buffering.
				A number of pictures are stored within this class, and they
				are managed using indices. This function allocates memory
				for the requested number of decoded frames up to a maximum 
				of MAXIMUM_NUMBER_OF_STORED_FRAMES.
				number_of_reference_frames is no greater than this, and 
				indicates the maximum number of frames that can be use for 
				storing reference frames.
				If no memory has been previously allocated, the Create 
				function is called.
				If a reduction in memory is indicated, no memory is freed as 
				this would require shifting of data, and currently the 
				trade-off of having to spend processing time and software 
				development time on this does not balance the reward of 
				reduction in instantaneous memory usage.
				If an increase in memory is needed, it is allocated. If 
				the allocation fails, all memory is deleted and the class 
				is destroyed.												*/
/* RETURNS:		true if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264DecodedPictureBuffer::Modify(int number_of_decoded_frames,
									   int number_of_reference_frames,
									   unsigned int maximum_frame_number_minus1)
{
	int i;
	m_MaximumFrameNumberMinus1 = maximum_frame_number_minus1;
	m_MaximumFieldNumberMinus1 = 2 * m_MaximumFrameNumberMinus1 + 1;

	// Clip the input parameters to the maximum allowed
	if (number_of_decoded_frames > MAXIMUM_NUMBER_OF_STORED_FRAMES)
	{
		number_of_decoded_frames = MAXIMUM_NUMBER_OF_STORED_FRAMES;
	}

	if (number_of_reference_frames > number_of_decoded_frames)
	{
		number_of_reference_frames = number_of_decoded_frames;
	}

	if (0 == m_NumberOfDecodedFrames)
	{
		// This indicates it is not yet created, so just call create
		return Create(number_of_decoded_frames, number_of_reference_frames);
	}

	if (number_of_decoded_frames <= m_NumberOfDecodedFrames)
	{
		// Don't change the memory allocations or m_NumberOfDecodedFrames as
		// this would require shifting data around while the only benefit 
		// would be a reduction in current memory usage.
		// But do change m_NumberOfReferenceFrames as this is needed to operate
		// the sliding window correctly. There should be no side effects as
		// the current picture should be an IDR picture.
		m_NumberOfReferenceFrames = number_of_reference_frames;

		if (m_NumberOfReferenceFrames > m_NumberOfDecodedFrames)
		{
			m_NumberOfReferenceFrames = m_NumberOfDecodedFrames;
		}

		return true;
	}

	// Extra memory must be allocated
          
	for (i=m_NumberOfDecodedFrames; i<number_of_decoded_frames; i++)
	{
		m_Picture[i] = new CBTPicture();

		if (0 == m_Picture[i])
		{
			// Memory allocation failure
			Destroy();
			return false;
		}

		m_TopWaitingToBeOutput[i] = false;
		m_BottomWaitingToBeOutput[i] = false;

		m_TopOutputBeforeIDR[i] = false;
		m_BottomOutputBeforeIDR[i] = false;
	}

	m_NumberOfDecodedFrames = number_of_decoded_frames;
	m_NumberOfReferenceFrames = number_of_reference_frames;

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::Destroy							*/
/* DESCRIPTION:	Deletes all picture memories, and sets their pointers to 
				zero. Sets the number of reference frames to zero.			*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::Destroy(void)
{
	int i;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if (0 != m_Picture[i])
		{
			delete m_Picture[i];
			m_Picture[i] = 0;
		}
	}

	m_NumberOfDecodedFrames = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ProcessMMCOReset					*/
/* DESCRIPTION:	This carries out functions associated with MMCO reset.
				All reference pictures are marked as not used for reference */
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::ProcessMMCOReset(void)
{
	int i;
	// Frame Number is considered to be zero after a reset, hence there is
	// discontinuity in the series of its values
	m_CurrentFrameNumberContinuity = false;
	m_CurrentFrameNumber = 0;
//	m_CurrentPictureOrderCount = 0;
//	m_CurrentTopPictureOrderCount = 0;
//	m_CurrentBottomPictureOrderCount = 0;
	m_CurrentTopPictureOrderCount -= m_CurrentPictureOrderCount;
	m_CurrentBottomPictureOrderCount -= m_CurrentPictureOrderCount;
	m_CurrentPictureOrderCount = 0;

	m_MaximumLongTermIndexPlus1 = 0;
	m_ArrayPositionOfMostRecentPicture = 0;
	m_ArrayPositionOfMostRecentReferencePicture = 0;

	for (i=0; i<MAXIMUM_NUMBER_OF_STORED_FRAMES; i++)
	{
		m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
		m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;

		m_TopStoredForReference[i] = false;
		m_BottomStoredForReference[i] = false;

		m_TopNonExisting[i] = false;
		m_BottomNonExisting[i] = false;

		if (m_FlushNonReferencePictures)
		{
			m_TopWaitingToBeOutput[i] = false;
			m_BottomWaitingToBeOutput[i] = false;

			m_TopOutputBeforeIDR[i] = false;
			m_BottomOutputBeforeIDR[i] = false;
		}
		else
		{
			if (m_TopWaitingToBeOutput[i])
			{
				m_TopOutputBeforeIDR[i] = true;
			}
			if (m_BottomWaitingToBeOutput[i])
			{
				m_BottomOutputBeforeIDR[i] = true;
			}
		}
	}
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::GetNumberOfActiveReferenceFrames	*/
/* DESCRIPTION:	This returns the number of reference frames that are 
				currently stored in the buffer, that is, the number for 
				which m_StoredForReference is true							*/
/* RETURNS:		The number of frames that are stored for reference.			*/
/* ------------------------------------------------------------------------ */

int CH264DecodedPictureBuffer::GetNumberOfActiveReferenceFrames(void) const
{
	int i;
	int number_of_active_reference_frames = 0;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if (m_TopStoredForReference[i] && m_BottomStoredForReference[i])
		{
			number_of_active_reference_frames++;
		}
	}

	return number_of_active_reference_frames;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::GetNumberOfActiveReferenceFields	*/
/* DESCRIPTION:	This returns the number of reference fields that are 
				currently stored in the buffer, that is, the number for 
				which m_StoredForReference is true							*/
/* RETURNS:		The number of fields that are stored for reference.			*/
/* ------------------------------------------------------------------------ */

#ifdef __INTERLACE__
int CH264DecodedPictureBuffer::GetNumberOfActiveReferenceFields(void) const
{
	int i;
	int number_of_active_reference_frames = 0;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if (m_TopStoredForReference[i])
		{
			number_of_active_reference_frames++;
		}
		if (m_BottomStoredForReference[i])
		{
			number_of_active_reference_frames++;
		}
	}

	return number_of_active_reference_frames;
}
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::OperateSlidingWindow				*/
/* DESCRIPTION:	This performs the sliding window operation.
				If the number of frames stored for reference equals the 
				number of reference frames, the oldest frame stored for 
				reference, but not as a long term frame, is marked as not 
				stored for reference. If it is marked as non-existing, it 
				is changed to no longer indicate it is non-existing. Note,
				this is part of the gaps in frame numbers process, and a 
				frame marked as non-existing, or changed back from 
				non-existing, has never and will never actually exist!		*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::OperateSlidingWindow(void)
{
	int i;
	int number_of_active_reference_frames = 0;

	int array_position_of_oldest_reference_frame = 0;

	unsigned int age_of_oldest_reference_frame = 0;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if (m_TopStoredForReference[i] || m_BottomStoredForReference[i])
		{
			number_of_active_reference_frames++;

			if ((UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]) && 
				(UNUSED_ARRAY_POSITION == m_BottomLongTermIndex[i]))
			{
				// Not a long term reference frame
				unsigned int age_of_reference_frame;

				// If both fields are stored in the same array position, they shall have the same frame number
				if (m_TopStoredForReference[i] && m_BottomStoredForReference[i])
				{
					assert(m_TopFrameNumber[i] == m_BottomFrameNumber[i]);
				}

				if (m_TopStoredForReference[i])
				{
					age_of_reference_frame = (m_MaximumFrameNumberMinus1 + 1 + 
						m_CurrentFrameNumber - m_TopFrameNumber[i]) & m_MaximumFrameNumberMinus1;
				}
				else
				{
					age_of_reference_frame = (m_MaximumFrameNumberMinus1 + 1 + 
						m_CurrentFrameNumber - m_BottomFrameNumber[i]) & m_MaximumFrameNumberMinus1;
				}

				if (age_of_reference_frame > age_of_oldest_reference_frame)
				{
					age_of_oldest_reference_frame = age_of_reference_frame;
					array_position_of_oldest_reference_frame = i;
				}
			}
		}
	}

	if (number_of_active_reference_frames == m_NumberOfReferenceFrames)
	{
		// Operate the sliding window
		m_TopStoredForReference[array_position_of_oldest_reference_frame] = false;
		m_BottomStoredForReference[array_position_of_oldest_reference_frame] = false;

		m_TopNonExisting[array_position_of_oldest_reference_frame] = false;
		m_BottomNonExisting[array_position_of_oldest_reference_frame] = false;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::NonExistingFrameMarking			*/
/* DESCRIPTION:	This operates the first part of the gaps in frame number 
				procedure for a single non existing frame. The sliding 
				window procedure is operated to make room for it.			*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::NonExistingFrameMarking(void)
{
	OperateSlidingWindow();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::NonExistingFrameStorage			*/
/* DESCRIPTION:	This operates the second part of the gaps in frame number 
				procedure for a single non existing frame. These are added
				to the decoded picture buffer, and marked as stored for 
				reference and non existing.									*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::NonExistingFrameStorage(void)
{
	int i;
	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if (!m_TopStoredForReference[i] && !m_BottomStoredForReference[i] && 
			!m_TopWaitingToBeOutput[i] && !m_BottomWaitingToBeOutput[i] &&
			(i != m_ArrayPositionOfCurrentPicture))
		{
			// Found a suitable array position
			m_TopStoredForReference[i] = true;
			m_BottomStoredForReference[i] = true;
			
			m_TopNonExisting[i] = true;
			m_BottomNonExisting[i] = true;

			m_CurrentFrameNumber++;
			m_TopFrameNumber[i] = m_CurrentFrameNumber;
			m_BottomFrameNumber[i] = m_CurrentFrameNumber;

			break;
		}
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ActOnMMCOCommandsFrame			*/
/* DESCRIPTION:	This returns performs the actions associated with the stored 
				set of MMCO commands. It is called after a frame 
				has been decoded.											*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::ActOnMMCOCommandsFrame(void)
{
	int MMCO_read_pointer = 0;
	h264_MMCO_command_type command;

	assert(m_PictureTypeOfCurrentPicture == H264_FRAME_PICTURE);

	if (0 == m_MMCO_WritePointer) return;  // No commands to process

	while (MMCO_read_pointer < m_MMCO_WritePointer)
	{
		command = m_MMCO_Commands[MMCO_read_pointer];

		switch (command.type)
		{
			case H264_MMCO_END_OF_LOOP :
				break;

			case H264_MMCO_MARK_SHORT_UNUSED	:
			{
				int i;
				unsigned int frame_number = (m_MaximumFrameNumberMinus1 + m_CurrentFrameNumber - 
												command.diff_pn_minus1) & m_MaximumFrameNumberMinus1;

				// Now find this frame
				bool found_picture_to_remove = false;
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if ((frame_number == m_TopFrameNumber[i]) &&
						(UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]) &&
						(m_TopStoredForReference[i]))
					{
						// Found it!
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopNonExisting[i] = false;
						m_BottomNonExisting[i] = false;

						found_picture_to_remove = true;
						break;
					}
				}

				if (!found_picture_to_remove)
				{
					// This is either an error condition, or the result of packet loss or gradual decoder refresh
					// Best solution appears to be to invoke the sliding window so that if the buffer is 
					// full a picture is removed, and nothing is removed otherwise
					OperateSlidingWindow();
				}
				break;
			}

			case H264_MMCO_MARK_LONG_UNUSED :
			{
				int i;
				bool found_picture_to_remove = false;
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if (command.long_term_index == m_TopLongTermIndex[i])
					{
						// Found it!
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopNonExisting[i] = false;
						m_BottomNonExisting[i] = false;

						m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;

						found_picture_to_remove = true;
						break;
					}
				}

				if (!found_picture_to_remove)
				{
					// This is either an error condition, or the result of packet loss or gradual decoder refresh
					// Best solution appears to be to invoke the sliding window so that if the buffer is 
					// full a picture is removed, and nothing is removed otherwise
					OperateSlidingWindow();
				}
				break;
			}

			case H264_MMCO_ASSIGN_LONG_TERM :
			{
				unsigned int frame_number = (m_MaximumFrameNumberMinus1 + m_CurrentFrameNumber - 
												command.diff_pn_minus1) & m_MaximumFrameNumberMinus1;

				int i, array_position;

				// Now find this frame
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if ((frame_number == m_TopFrameNumber[i]) &&
						(m_TopStoredForReference[i]) &&
						(UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]))
					{
						break;
					}
				}

				if (i == m_NumberOfDecodedFrames)
				{
					// This means we did not find the frame
					// This is an error condition - could be due to errors/loss in the bitstream
					// Ignore this MMCO command
					assert(false);
					break;
				}

				array_position = i;

				// Determine whether this long term index has already been used
				// If so, mark the frame with this long term index as not stored for reference
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if (m_TopLongTermIndex[i] == command.long_term_index)
					{
						// Mark the frame as unused
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopNonExisting[i] = false;
						m_BottomNonExisting[i] = false;

						m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						break;
					}
				}

				// Now map the frame
				m_TopLongTermIndex[array_position] = command.long_term_index;
				m_BottomLongTermIndex[array_position] = command.long_term_index;

				break;
			}

			case H264_MMCO_MAX_LONG_INDEX :
				if (command.max_long_term_index_plus1 < m_MaximumLongTermIndexPlus1)
				{
					int i;
					// Frames that have long term index greater than or equal to 
					// command.max_long_term_index_plus1 must be marked as unused for reference
					for (i=0; i<m_NumberOfDecodedFrames; i++)
					{
						if ((UNUSED_ARRAY_POSITION != m_TopLongTermIndex[i]) &&
							(m_TopLongTermIndex[i] >= command.max_long_term_index_plus1))
						{
							// Mark the frame as unused
							m_TopStoredForReference[i] = false;
							m_BottomStoredForReference[i] = false;

							m_TopNonExisting[i] = false;
							m_BottomNonExisting[i] = false;

							m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
							m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						}
					}
				}

				m_MaximumLongTermIndexPlus1 = command.max_long_term_index_plus1;
				break;

			case H264_MMCO_RESET :
				m_MMCOReset = true;
				ProcessMMCOReset();
				break;

			case H264_MMCO_STORE_AS_LONG_TERM :
			{
				int i;
				// Determine whether this long term index has already been used
				// If so, mark the frame with this long term index as not stored for reference
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if (command.long_term_index == m_TopLongTermIndex[i])
					{
						// Found it!
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopNonExisting[i] = false;
						m_BottomNonExisting[i] = false;

						m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						break;
					}
				}
				m_CurrentPictureLongTermIndex = command.long_term_index;
				break;
			}

			default :
				// Unknown MMCO Command. Igore it.
				break;
		}

		if (H264_MMCO_END_OF_LOOP == command.type)
		{
			break;
		}

		MMCO_read_pointer++;
	}

	m_MMCO_WritePointer = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ActOnMMCOCommandsField			*/
/* DESCRIPTION:	This returns performs the actions associated with the stored 
				set of MMCO commands. It is called after a field 
				has been decoded.											*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::ActOnMMCOCommandsField(void)
{
	int MMCO_read_pointer = 0;
	h264_MMCO_command_type command;

	assert((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) ||
		   (H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture));

	if (0 == m_MMCO_WritePointer) return;  // No commands to process

	while (MMCO_read_pointer < m_MMCO_WritePointer)
	{
		command = m_MMCO_Commands[MMCO_read_pointer];

		switch (command.type)
		{
			case H264_MMCO_END_OF_LOOP :
				break;

			case H264_MMCO_MARK_SHORT_UNUSED	:
			{
				int i;
				int pic_num = (int)(2 * m_CurrentFrameNumber + 1) - command.diff_pn_minus1 - 1;

				bool same_parity = (1 == (pic_num & 1));

				unsigned int frame_number = (m_MaximumFrameNumberMinus1 + 1 + (pic_num >> 1)) & m_MaximumFrameNumberMinus1;

				unsigned int * frame_number_array;
				bool * stored_for_reference_array;
				UNSIGNED_SEPTET * long_term_index_array;

				if (((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && same_parity) ||
					((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && !same_parity))
				{
					frame_number_array = m_TopFrameNumber;
					stored_for_reference_array = m_TopStoredForReference;
					long_term_index_array = m_TopLongTermIndex;
				}
				else
				{
					frame_number_array = m_BottomFrameNumber;
					stored_for_reference_array = m_BottomStoredForReference;
					long_term_index_array = m_BottomLongTermIndex;
				}

				// Now find this field
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if ((frame_number == frame_number_array[i]) &&
						(stored_for_reference_array[i]) &&
						(UNUSED_ARRAY_POSITION == long_term_index_array[i]))
					{
						// Found it!
						stored_for_reference_array[i] = false;
						break;
					}
				}
				break;
			}

			case H264_MMCO_MARK_LONG_UNUSED :
			{
				int i;
				bool same_parity = (1 == (command.long_term_index & 1));
				
				unsigned int * frame_number_array;
				bool * stored_for_reference_array;
				UNSIGNED_SEPTET * long_term_index_array;

				if (((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && same_parity) ||
					((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && !same_parity))
				{
					frame_number_array = m_TopFrameNumber;
					stored_for_reference_array = m_TopStoredForReference;
					long_term_index_array = m_TopLongTermIndex;
				}
				else
				{
					frame_number_array = m_BottomFrameNumber;
					stored_for_reference_array = m_BottomStoredForReference;
					long_term_index_array = m_BottomLongTermIndex;
				}

				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if ((command.long_term_index >> 1) == long_term_index_array[i])
					{
						// Found it!
						stored_for_reference_array[i] = false;
						long_term_index_array[i] = UNUSED_ARRAY_POSITION;
						break;
					}
				}
				break;
			}

			case H264_MMCO_ASSIGN_LONG_TERM :
			{
				int i, array_position;
				int pic_num = (int)(2 * m_CurrentFrameNumber + 1) - command.diff_pn_minus1 - 1;

				bool same_parity = (1 == (pic_num & 1));

				unsigned int frame_number = (m_MaximumFrameNumberMinus1 + 1 + (pic_num >> 1)) & m_MaximumFrameNumberMinus1;

				unsigned int * frame_number_array;
				bool * stored_for_reference_array;
				UNSIGNED_SEPTET * long_term_index_array;

				if (((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && same_parity) ||
					((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && !same_parity))
				{
					frame_number_array = m_TopFrameNumber;
					stored_for_reference_array = m_TopStoredForReference;
					long_term_index_array = m_TopLongTermIndex;
				}
				else
				{
					frame_number_array = m_BottomFrameNumber;
					stored_for_reference_array = m_BottomStoredForReference;
					long_term_index_array = m_BottomLongTermIndex;
				}

				// Now find this field
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if ((frame_number == frame_number_array[i]) &&
						(stored_for_reference_array[i]) &&
						(UNUSED_ARRAY_POSITION == long_term_index_array[i]))
					{
						break;
					}
				}

				if (i == m_NumberOfDecodedFrames)
				{
					// This means we did not find the frame
					// This is an error condition - could be due to errors/loss in the bitstream
					// Ignore this MMCO command
					assert(false);
					break;
				}

				array_position = i;

				// Determine whether this long term index has already been used
				// Frame number must be different to the current one, otherwise it
				// is the field paired with the current one
				// If so, mark both fields with this long term index as not stored for reference
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if (((m_TopLongTermIndex[i] == command.long_term_index) && (frame_number != m_TopFrameNumber[i])) ||
						((m_BottomLongTermIndex[i] == command.long_term_index) && (frame_number != m_BottomFrameNumber[i])))
					{
						// Mark both fields as unused
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						break;
					}
				}

				// Now map the field
				long_term_index_array[array_position] = command.long_term_index;
				break;
			}

			case H264_MMCO_MAX_LONG_INDEX :
				if (command.max_long_term_index_plus1 < m_MaximumLongTermIndexPlus1)
				{
					int i;
					// Fields that have long term index greater than or equal to 
					// command.max_long_term_index_plus1 must be marked as unused for reference
					for (i=0; i<m_NumberOfDecodedFrames; i++)
					{
						if ((UNUSED_ARRAY_POSITION != m_TopLongTermIndex[i]) &&
							(m_TopLongTermIndex[i] >= command.max_long_term_index_plus1))
						{
							// Mark the field as unused
							m_TopStoredForReference[i] = false;
							m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						}

						if ((UNUSED_ARRAY_POSITION != m_BottomLongTermIndex[i]) &&
							(m_BottomLongTermIndex[i] >= command.max_long_term_index_plus1))
						{
							// Mark the field as unused
							m_BottomStoredForReference[i] = false;
							m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						}
					}
				}

				m_MaximumLongTermIndexPlus1 = command.max_long_term_index_plus1;
				break;

			case H264_MMCO_RESET :
				ProcessMMCOReset();
				break;

			case H264_MMCO_STORE_AS_LONG_TERM :
			{
				int i;
				// Determine whether this long term index has already been used
				// If so, mark the fields with this long term index as not stored for reference
				for (i=0; i<m_NumberOfDecodedFrames; i++)
				{
					if (((m_TopLongTermIndex[i] == command.long_term_index) && (m_CurrentFrameNumber != m_TopFrameNumber[i])) ||
						((m_BottomLongTermIndex[i] == command.long_term_index) && (m_CurrentFrameNumber != m_BottomFrameNumber[i])))
					{
						// Mark both fields as unused
						m_TopStoredForReference[i] = false;
						m_BottomStoredForReference[i] = false;

						m_TopLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						m_BottomLongTermIndex[i] = UNUSED_ARRAY_POSITION;
						break;
					}
				}
				m_CurrentPictureLongTermIndex = command.long_term_index;
				break;
			}

			default :
				// Unknown MMCO Command. Ignore it.
				assert(false);
				break;
		}

		if (H264_MMCO_END_OF_LOOP == command.type)
		{
			break;
		}

		MMCO_read_pointer++;
	}
	m_MMCO_WritePointer = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::GetPictureForDecoding			*/
/* DESCRIPTION:	This is used to get a pointer to a frame in the decoded
				picture buffer that can be used to decode the current
				picture into.
				Normally, there is at least one frame marked as
				not used for reference and not waiting to be output, in
				which case one such frame is selected as the destination
				for decoding the current frame.
				Otherwise, I think it is an error condition and picture
				is set to zero.												*/
/* RETURNS:		A CBTPicture** into which the calling function can write the
				decoded frame. This pointer is guaranteed to be correct
				only until the next call to StorePicture, which is used to
				indicate that decoding of the picture has been completed.
				The return value is void.									*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::GetPictureForDecoding(CBTPicture ** picture, h264_picture_type type)
{
	int i;

	*picture = 0; // Error condition if this is not changed

			// Find a frame that is not used for reference and not waiting for output
			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if (!m_TopStoredForReference[i] && !m_BottomStoredForReference[i] &&
					!m_TopWaitingToBeOutput[i] && !m_BottomWaitingToBeOutput[i])
				{
					// Found a suitable array position
#if defined __ARC_SHARED_BUFFER__
					if (!m_Picture[i]->IsBeingDisplayed())
					{
#endif
					*picture= m_Picture[i];
					m_ArrayPositionOfCurrentPicture = i;
					break;
#if defined __ARC_SHARED_BUFFER__
					}
#endif
				}
			}

	m_PictureTypeOfCurrentPicture = type;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::SetDecodingPictureParameters		*/
/* DESCRIPTION:	The value of frame number for the current frame is set.
				It is needed for subsequent reference frame remapping and 
				MMCO operations.
				The value of Picture Order Count for the current picture 
				is stored, depending on the picture type. In the decoder, 
				this is needed for subsequent default reference index 
				ordering.													*/
/* RETURNS:		The return value is void.									*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::SetDecodingPictureParameters(unsigned int frame_number, int picture_order_count,
															 int top_picture_order_count, int bottom_picture_order_count)
{
	m_CurrentFrameNumber = frame_number;
	m_CurrentFrameNumberContinuity = true;

	m_CurrentPictureOrderCount = picture_order_count;

#ifdef __INTERLACE__
	switch (m_PictureTypeOfCurrentPicture)
	{
		case H264_FRAME_PICTURE :
#endif
			// Find a frame that is not used for reference and not waiting for output
			m_CurrentTopPictureOrderCount = top_picture_order_count;
			m_CurrentBottomPictureOrderCount = bottom_picture_order_count;

#ifdef __INTERLACE__
			break;

		case H264_TOP_FIELD_PICTURE :
			m_CurrentTopPictureOrderCount = top_picture_order_count;
			m_CurrentBottomPictureOrderCount = INT_MAX; // Not set
			break;

		case H264_BOTTOM_FIELD_PICTURE :
			m_CurrentTopPictureOrderCount = INT_MAX;  // Not set
			m_CurrentBottomPictureOrderCount = bottom_picture_order_count;

			break;
	}
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::StorePicture						*/
/* DESCRIPTION:	This is used to store the current picture in the decoded 
				picture buffer. The picture previously returned by 
				GetPictureForDecoding is stored according to the parameters
				Firstly , the contents of the buffer are updated as 
				appropriate, either by sliding window method or the 
				adaptive (MMCO) method.
				Next, the frame is found in the decoded picture buffer 
				and is marked appropriately according to parameters set 
				during decoding.											*/
/* RETURNS:		The return value is the array position in which the picture 
				is stored. This can be used for subsequent storing of the 
				motion vectors for use in direct mode.						*/
/* ------------------------------------------------------------------------ */

int CH264DecodedPictureBuffer::StorePicture(bool use_for_reference, bool* mmco_reset)
{
	m_CurrentPictureLongTermIndex = UNUSED_ARRAY_POSITION;  // UNUSED_ARRAY_POSITION unless set be MMCO commands
	m_MMCOReset = false;

	if (use_for_reference)
	{
		if (m_IDR_pic_flag)
		{
			ProcessMMCOReset();
		}
		else if (m_SlidingWindowBufferingMode)
		{
			// Only operate sliding window on the first field of a field pair and on a frame
			bool second_field_of_reference_pair = 
				((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) &&
				 (m_BottomFrameNumber[m_ArrayPositionOfMostRecentPicture] == m_CurrentFrameNumber) && 
				 (m_BottomStoredForReference[m_ArrayPositionOfMostRecentPicture])) ||
				((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) &&
				 (m_TopFrameNumber[m_ArrayPositionOfMostRecentPicture] == m_CurrentFrameNumber) && 
				 (m_TopStoredForReference[m_ArrayPositionOfMostRecentPicture]));

			if (!second_field_of_reference_pair)
			{
				OperateSlidingWindow();
			}
		}
		else
		{
			if (H264_FRAME_PICTURE == m_PictureTypeOfCurrentPicture)
			{
				ActOnMMCOCommandsFrame();
			}
			else
			{
				ActOnMMCOCommandsField();
			}
		}
	}

	*mmco_reset = m_MMCOReset;

			m_TopFrameNumber[m_ArrayPositionOfCurrentPicture] = m_CurrentFrameNumber;
			m_BottomFrameNumber[m_ArrayPositionOfCurrentPicture] = m_CurrentFrameNumber;

			m_TopPictureOrderCount[m_ArrayPositionOfCurrentPicture] = m_CurrentTopPictureOrderCount;
			m_BottomPictureOrderCount[m_ArrayPositionOfCurrentPicture] = m_CurrentBottomPictureOrderCount;
			
			m_TopLongTermIndex[m_ArrayPositionOfCurrentPicture] = m_CurrentPictureLongTermIndex;
			m_BottomLongTermIndex[m_ArrayPositionOfCurrentPicture] = m_CurrentPictureLongTermIndex;
			
			m_TopStoredForReference[m_ArrayPositionOfCurrentPicture] = use_for_reference;
			m_BottomStoredForReference[m_ArrayPositionOfCurrentPicture] = use_for_reference;
			
			m_TopWaitingToBeOutput[m_ArrayPositionOfCurrentPicture] = true;
			m_BottomWaitingToBeOutput[m_ArrayPositionOfCurrentPicture] = true;

			m_ArrayPositionOfMostRecentPicture = m_ArrayPositionOfCurrentPicture;

			if (use_for_reference)
			{
				m_ArrayPositionOfMostRecentReferencePicture = m_ArrayPositionOfCurrentPicture;
			}

			if ((m_IDR_pic_flag) && m_StoreIDRAsLongTerm)
			{
				m_TopLongTermIndex[m_ArrayPositionOfCurrentPicture] = 0;
				m_BottomLongTermIndex[m_ArrayPositionOfCurrentPicture] = 0;
				m_MaximumLongTermIndexPlus1 = 1;
			}

	return m_ArrayPositionOfCurrentPicture;
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::OutputFrame						*/
/* DESCRIPTION:	This is sets output_frame to point to the next frame to
				be output and zero if there is no such frame. The next 
				frame to be output is the one marked as waiting for output 
				with the lowest value of frame order count. It is then 
				marked as not waiting for output.
				frames around an IDR or MMCO Reset frame have frame 
				order counts on different scales. Those before the reseting 
				point must be output before the one at the reset point and 
				those decoded afterwards. The ones to be output earlier are 
				indicated by m_OutputBeforeIDR[] being true.				*/
/* RETURNS:		The array position of the output frame.						*/
/* ------------------------------------------------------------------------ */

int CH264DecodedPictureBuffer::OutputFrame(CBTPicture ** output_frame)
{
	int lowest_picture_order_count = INT_MAX;
	int output_location = UNUSED_ARRAY_POSITION;
	int i;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		if ((m_TopOutputBeforeIDR[i]) && (m_TopWaitingToBeOutput[i]) && 
			(m_TopPictureOrderCount[i] < lowest_picture_order_count))
		{
			lowest_picture_order_count = m_TopPictureOrderCount[i];
			output_location = i;
		}
	}

	if (UNUSED_ARRAY_POSITION == output_location)
	{
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((m_TopWaitingToBeOutput[i]) && 
				(m_TopPictureOrderCount[i] < lowest_picture_order_count))
			{
				lowest_picture_order_count = m_TopPictureOrderCount[i];
				output_location = i;
			}
		}
	}

	if (output_location != UNUSED_ARRAY_POSITION)
	{
		*output_frame = m_Picture[output_location];

		m_TopWaitingToBeOutput[output_location] = false;
		m_BottomWaitingToBeOutput[output_location] = false;
		m_TopOutputBeforeIDR[output_location] = false;
		m_BottomOutputBeforeIDR[output_location] = false;
	}
	else
	{
		*output_frame = 0;
	}

	return output_location;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::OutputFrameBumping
				CH264DecodedPictureBuffer::OutputFrameBumpingNonExisting	*/
/* DESCRIPTION:	This outputs frames according to the bumping process.
				The basic idea is that a frame is only bumped out when
				there is no longer any space left for storage of frames in
				the decoded picture buffer. But it may be necessary to
				bump out more than one frame at any point in time, so this
				function should be called until output_frame is zero.
				This is sets output_frame to point to the next frame to
				be output and zero if there is no such frame. The next
				frame to be output is the one marked as waiting for output
				with the lowest value of frame order count. It is then
				marked as not waiting for output.
				frames around an IDR or MMCO Reset frame have frame
				order counts on different scales. Those before the reseting
				point must be output before the one at the reset point and
				those decoded afterwards. The ones to be output earlier are
				indicated by m_OutputBeforeIDR[] being true.				*/
/* RETURNS:		The array position of the output frame.						*/
/* ------------------------------------------------------------------------ */

int CH264DecodedPictureBuffer::OutputFrameBumping(CBTPicture ** output_frame)
{
  int i;
  int number_of_frames_stored = 0;
  bool output_before_IDR = false;
  // Count the number of frame stores in use to determine whether bumping is necessary
  for (i=0; i<m_NumberOfDecodedFrames; i++)
  {
    if (m_TopStoredForReference[i] || m_BottomStoredForReference[i] || m_TopWaitingToBeOutput[i] || m_BottomWaitingToBeOutput[i])
    {
      number_of_frames_stored++;
    }
    if (m_TopOutputBeforeIDR[i] || m_BottomOutputBeforeIDR[i])
    {
      output_before_IDR = true;
      break;
    }
  }
  //TODO check if -2/-1 should be -1/-0
#if defined __ARC_SHARED_BUFFER__
  if ((number_of_frames_stored < m_NumberOfDecodedFrames - 2) && !output_before_IDR)
#else
  if ((number_of_frames_stored < m_NumberOfDecodedFrames - 1) && !output_before_IDR)
#endif
  {
    // No need to bump
    *output_frame = 0;
    return UNUSED_ARRAY_POSITION;
  }
  else
  {
    // Find the appropriate frame to bump
    return OutputFrame(output_frame);
  }
}

// This variant is called in the case of decoding non-existing frames
// It has an additional test on m_ArrayPositionOfCurrentPicture, which has already been
// allocated for decoding the current frame into, and so must be included in the count
// of frames that are stored
int CH264DecodedPictureBuffer::OutputFrameBumpingNonExisting(CBTPicture ** output_frame)
{
  int i;
 int number_of_frames_stored = 0;
  bool output_before_IDR = false;
  // Count the number of frame stores in use to determine whether bumping is necessary
  // Note the test on m_ArrayPositionOfCurrentPicture is needed for the case of non-existing frames
  for (i=0; i<m_NumberOfDecodedFrames; i++)
  {
    if (m_TopStoredForReference[i] || m_BottomStoredForReference[i] ||
        m_TopWaitingToBeOutput[i] || m_BottomWaitingToBeOutput[i] || (i == m_ArrayPositionOfCurrentPicture))
    {
      number_of_frames_stored++;
    }
    if (m_TopOutputBeforeIDR[i] || m_BottomOutputBeforeIDR[i])
    {
      output_before_IDR = true;
      break;
    }
  }
  if ((number_of_frames_stored < m_NumberOfDecodedFrames) && !output_before_IDR)
  {
    // No need to bump
    *output_frame = 0;
    return UNUSED_ARRAY_POSITION;
  }
  else
  {
    // Find the appropriate frame to bump
    return OutputFrame(output_frame);
  }
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::CreateFrameDefaultIndicesForPSlice	*/
/* DESCRIPTION:	This calculates default indices for the reference
				frames in P and SP slices. The default order is the short
				term frames starting from the most recent and working
				towards to the oldest, followed by the long term frames
				starting from the one with long term index 0.
				It resets the remapped index order to UNUSED_ARRAY_POSITION.
				So ActOnNextRemappingInstruction must be called at least
				once to create a set of remapped indices.					*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::CreateFrameDefaultIndicesForPSlice(void)
{
	int i;
	int default_index;

	// Set default indices to unused (needed for consideration of long term frames)
	// calculate frame number differences, and count short term reference frames

	unsigned int picture_number_difference[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int number_of_short_term_reference_frames = 0;
	int next_long_term_index;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		m_DefaultIndexList0ToArrayPosition[i] = UNUSED_ARRAY_POSITION;

//		m_DefaultIndexList1ToArrayPosition[i] = UNUSED_ARRAY_POSITION;

		m_RemappedIndexList0ToArrayPosition[i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[i] = UNUSED_ARRAY_POSITION;
		
//		m_RemappedIndexList1ToArrayPosition[i] = UNUSED_ARRAY_POSITION;
//		m_ArrayPositionToLowestRemappedIndexList1[i] = UNUSED_ARRAY_POSITION;

		if (m_TopStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]))
		{
			// Frame is a short term reference frame
			number_of_short_term_reference_frames++;

			picture_number_difference[i] = (m_MaximumFrameNumberMinus1 + 1 + m_CurrentFrameNumber 
												- m_TopFrameNumber[i]) & m_MaximumFrameNumberMinus1;
		}
		else
		{
			picture_number_difference[i] = UINT_MAX;
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the short term reference pictures
	////////////////////////////////////////////////////////////////////////

	for (default_index=0; default_index<number_of_short_term_reference_frames; default_index++)
	{
		unsigned int lowest_picture_number_difference = UINT_MAX;
		int array_position = 0;

		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (picture_number_difference[i] < lowest_picture_number_difference)
			{
				lowest_picture_number_difference = picture_number_difference[i];
				array_position = i;
			}
		}

		m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)array_position;

		picture_number_difference[array_position] = UINT_MAX;  // Ensure it is not selected next time
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the long term reference pictures
	////////////////////////////////////////////////////////////////////////

	next_long_term_index = 0;
	do
	{
		int array_position = UNUSED_ARRAY_POSITION;

		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (next_long_term_index == m_TopLongTermIndex[i])
			{
				array_position = i;
				break;
			}
		}

		if (UNUSED_ARRAY_POSITION != array_position)
		{
			m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)array_position;

			default_index++;
		}

		next_long_term_index++;

	} while (next_long_term_index < m_MaximumLongTermIndexPlus1);

	////////////////////////////////////////////////////////////////////////
	// Initialise Parameters for Remapping of indices
	////////////////////////////////////////////////////////////////////////

	m_NextRemappingIndex = 0;
	m_RemappingFrameNumberPredictor = m_CurrentFrameNumber;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::CreateFieldDefaultIndicesForPSlice	*/
/* DESCRIPTION:	This calculates default indices for the reference
				fields in P and SP slices. The default order is the short 
				term fields starting from the most recent and working 
				towards to the oldest, followed by the long term fields 
				starting from the one with long term index 0.
				It resets the remapped index order to UNUSED_ARRAY_POSITION.
				So ActOnNextRemappingInstruction must be called at least 
				once to create a set of remapped indices.					*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

#ifdef __INTERLACE__

void CH264DecodedPictureBuffer::CreateFieldDefaultIndicesForPSlice(void)
{
	int i;

	// Set default indices to unused (needed for consideration of long term frames)
	// calculate frame number differences, and count short term reference frames

	int picture_number_difference[2*MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int number_of_short_term_reference_fields = 0;

	int default_index;
	int field_parity;
	int lowest_picture_number_difference;
	int selected_array_position;
	int next_long_term_index;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		m_DefaultIndexList0ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_DefaultIndexList0ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;

		m_DefaultIndexList1ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_DefaultIndexList1ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;

		m_RemappedIndexList0ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_RemappedIndexList0ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[2*i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[2*i+1] = UNUSED_ARRAY_POSITION;
		
		m_RemappedIndexList1ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_RemappedIndexList1ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList1[2*i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList1[2*i+1] = UNUSED_ARRAY_POSITION;

		if (m_TopStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]))
		{
			// Top field is a short term reference field
			number_of_short_term_reference_fields++;

			picture_number_difference[2*i] = (m_MaximumFrameNumberMinus1 + 1 + m_CurrentFrameNumber 
												- m_TopFrameNumber[i]) & m_MaximumFrameNumberMinus1;
		}
		else
		{
			picture_number_difference[2*i] = INT_MAX;
		}

		if (m_BottomStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_BottomLongTermIndex[i]))
		{
			// Bottom field is a short term reference field
			number_of_short_term_reference_fields++;

			picture_number_difference[2*i+1] = (m_MaximumFrameNumberMinus1 + 1 + m_CurrentFrameNumber 
												- m_BottomFrameNumber[i]) & m_MaximumFrameNumberMinus1;
		}
		else
		{
			picture_number_difference[2*i+1] = INT_MAX;
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the short term reference pictures
	////////////////////////////////////////////////////////////////////////

	if (H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture)
	{
		field_parity = 0;
	}
	else
	{
		field_parity = 1;
	}

	for (default_index=0; default_index<number_of_short_term_reference_fields; /* no increment here */)
	{
		// Firstly look for same parity fields, select one with lowest picture number difference
		lowest_picture_number_difference = INT_MAX;
		selected_array_position = UNUSED_ARRAY_POSITION;

		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (picture_number_difference[2*i+field_parity] < lowest_picture_number_difference)
			{
				lowest_picture_number_difference = picture_number_difference[2*i+field_parity];
				selected_array_position = i;
			}
		}

		if (UNUSED_ARRAY_POSITION != selected_array_position)
		{
			m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+field_parity);

			// Ensure it is not selected next time
			picture_number_difference[2*selected_array_position+field_parity] = INT_MAX;

			default_index++;
		}

		// Secondly look for opposite parity fields, select one with lowest picture number difference
		lowest_picture_number_difference = INT_MAX;
		selected_array_position = UNUSED_ARRAY_POSITION;

		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (picture_number_difference[2*i+1-field_parity] < lowest_picture_number_difference)
			{
				lowest_picture_number_difference = picture_number_difference[2*i+1-field_parity];
				selected_array_position = i;
			}
		}

		if (UNUSED_ARRAY_POSITION != selected_array_position)
		{
			m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+1-field_parity);

			// Ensure it is not selected next time
			picture_number_difference[2*selected_array_position+1-field_parity] = INT_MAX;

			default_index++;
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the long term reference pictures
	////////////////////////////////////////////////////////////////////////

	next_long_term_index = 0;
	do
	{
		selected_array_position = UNUSED_ARRAY_POSITION;

		// Firstly look for same parity fields
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_TopLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				default_index++;
				break;
			}
			if ((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_BottomLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				default_index++;
				break;
			}
		}

		// Secondly look for opposite parity fields
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_TopLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				default_index++;
				break;
			}
			if ((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_BottomLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				default_index++;
				break;
			}
		}

		next_long_term_index++;

	} while (next_long_term_index < m_MaximumLongTermIndexPlus1);

	////////////////////////////////////////////////////////////////////////
	// Initialise Parameters for Remapping of indices
	////////////////////////////////////////////////////////////////////////

	m_NextRemappingIndex = 0;
	m_RemappingFrameNumberPredictor = m_CurrentFrameNumber;
	m_RemappingPicNumberPredictor = 2 * m_CurrentFrameNumber + 1;
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::CreateFrameDefaultIndicesForBSlice	*/
/* DESCRIPTION:	This calculates default indices for the reference
				frames in B slices. The default order for both lists is the 
				short term frames, ordered as below, differently for each 
				list, followed by the long term frames starting from the 
				one with long term index 0.
				The default order for short term frames in list 0 is
				starting from the one with largest POC less than the current 
				POC and progressing to one with the smallest POC, then the 
				one with the smallest POC greater than the current POC,
				progressing to the one with the largest POC.
				The default order for short term frames in list 1 is
				starting from the one with the smallest POC greater than 
				the current POC, progressing to the one with the largest 
				POC, then the one with the largest POC less than the current 
				POC and progressing to one with the smallest POC.
				This is illustrated below, with C being current, R reference

				list 0    2   1   0       3   4   5
				          R   R   R   C   R   R   R
				list 1    5   4   3       0   1   2

				If the two lists are the same, the first two elements in list
				1 are swapped.
				It resets the remapped index order to UNUSED_ARRAY_POSITION.
				So ActOnNextRemappingInstruction must be called at least 
				once to create a set of remapped indices.					*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */
#ifdef __B_PICTURES__
void CH264DecodedPictureBuffer::CreateFrameDefaultIndicesForBSlice(void)
{
	int i;

	// Set default indices to unused (needed for consideration of long term frames)
	// calculate frame number differences, and count short term reference frames

	int poc_difference[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int number_of_short_term_reference_frames = 0;

	// Count short term reference frames in the future, that is, ones for which poc_difference is negative
	int number_of_future_reference_frames = 0;
	int default_index;
	int next_long_term_index;

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		m_DefaultIndexList0ToArrayPosition[i] = UNUSED_ARRAY_POSITION;

		m_DefaultIndexList1ToArrayPosition[i] = UNUSED_ARRAY_POSITION;

		m_RemappedIndexList0ToArrayPosition[i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[i] = UNUSED_ARRAY_POSITION;
		
		m_RemappedIndexList1ToArrayPosition[i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList1[i] = UNUSED_ARRAY_POSITION;

		if (m_TopStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]))
		{
			// Frame is a short term reference frame
			number_of_short_term_reference_frames++;

			poc_difference[i] = m_CurrentPictureOrderCount - m_TopPictureOrderCount[i];

			if (poc_difference[i] < 0) number_of_future_reference_frames++;
		}
		else
		{
			poc_difference[i] = 0;
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the future short term reference pictures
	/////////////////////////////////////////////////////////////////////////////

	if (number_of_future_reference_frames > 0)
	{
		int next_list0_index = number_of_short_term_reference_frames;
		int next_list1_index = number_of_future_reference_frames;

		do
		{
			int lowest_poc_difference = INT_MAX;
			int array_position = 0;

			next_list0_index--;
			next_list1_index--;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference[i] < lowest_poc_difference) &&
					(0 != poc_difference[i]))
				{
					lowest_poc_difference = poc_difference[i];
					array_position = i;
				}
			}

			m_DefaultIndexList0ToArrayPosition[next_list0_index] = (UNSIGNED_SEPTET)array_position;
			m_DefaultIndexList1ToArrayPosition[next_list1_index] = (UNSIGNED_SEPTET)array_position;

			poc_difference[array_position] = 0;  // Ensure it is not selected next time

		} while (0 != next_list1_index);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the past short term reference pictures
	/////////////////////////////////////////////////////////////////////////////

	if (number_of_short_term_reference_frames > number_of_future_reference_frames)
	{
		int next_list0_index = 0;
		int next_list1_index = number_of_future_reference_frames;

		do
		{
			int lowest_poc_difference = INT_MAX;
			int array_position = 0;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference[i] < lowest_poc_difference) &&
					(0 != poc_difference[i]))
				{
					lowest_poc_difference = poc_difference[i];
					array_position = i;
				}
			}

			m_DefaultIndexList0ToArrayPosition[next_list0_index] = (UNSIGNED_SEPTET)array_position;
			m_DefaultIndexList1ToArrayPosition[next_list1_index] = (UNSIGNED_SEPTET)array_position;

			poc_difference[array_position] = 0;  // Ensure it is not selected next time

			next_list0_index++;
			next_list1_index++;

		} while (next_list1_index < number_of_short_term_reference_frames);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Swap the first two elements in list 1 if both lists are the same
	/////////////////////////////////////////////////////////////////////////////

	if (number_of_short_term_reference_frames > 1)
	{
		if (m_DefaultIndexList0ToArrayPosition[0] == m_DefaultIndexList1ToArrayPosition[0])
		{
			m_DefaultIndexList1ToArrayPosition[0] = m_DefaultIndexList0ToArrayPosition[1];
			m_DefaultIndexList1ToArrayPosition[1] = m_DefaultIndexList0ToArrayPosition[0];
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the long term reference pictures
	////////////////////////////////////////////////////////////////////////

	default_index = number_of_short_term_reference_frames;
	next_long_term_index = 0;
	do
	{
		int array_position = UNUSED_ARRAY_POSITION;

		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (next_long_term_index == m_TopLongTermIndex[i])
			{
				array_position = i;
				break;
			}
		}

		if (UNUSED_ARRAY_POSITION != array_position)
		{
			m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)array_position;
			m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)array_position;

			default_index++;
		}

		next_long_term_index++;

	} while (next_long_term_index < m_MaximumLongTermIndexPlus1);

	////////////////////////////////////////////////////////////////////////
	// Initialise Parameters for Remapping of indices
	////////////////////////////////////////////////////////////////////////

	m_NextRemappingIndex = 0;
	m_RemappingFrameNumberPredictor = m_CurrentFrameNumber;
}
#endif // def __B_PICTURES__

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::CreateFieldDefaultIndicesForBSlice	*/
/* DESCRIPTION:	This calculates default indices for the reference
				fields in B slices. The default order for both lists is the 
				short term fields, ordered as below, differently for each 
				list, followed by the long term fields starting from the 
				one with long term index 0.
				The default order for short term fields in list 0 is
				starting from the one with largest POC less than the current 
				POC and progressing to one with the smallest POC, then the 
				one with the smallest POC greater than the current POC,
				progressing to the one with the largest POC.
				The default order for short term fields in list 1 is
				starting from the one with the smallest POC greater than 
				the current POC, progressing to the one with the largest 
				POC, then the one with the largest POC less than the current 
				POC and progressing to one with the smallest POC.
				This is illustrated below, with C being current, R reference

				list 0    2   1   0       3   4   5
				          R   R   R   C   R   R   R
				list 1    5   4   3       0   1   2

				If the two lists are the same, the first two elements in list
				1 are swapped.
				It resets the remapped index order to UNUSED_ARRAY_POSITION.
				So ActOnNextRemappingInstruction must be called at least 
				once to create a set of remapped indices.					*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

#ifdef __INTERLACE__

void CH264DecodedPictureBuffer::CreateFieldDefaultIndicesForBSlice(void)
{
	int i;

	// Set default indices to unused (needed for consideration of long term frames)
	// calculate frame number differences, and count short term reference frames

	// In the following, the first index is for each field, with zero indicating 
	// same parity and 1 opposite parity
	int poc_difference_list0[2][MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int poc_difference_list1[2][MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int number_of_short_term_reference_fields[2] = {0, 0};

	// Count short term reference fields in the future, that is, ones for which poc_difference is negative
	int number_of_future_reference_fields[2] = {0, 0};

	int default_index;
	bool added_same_parity_to_list;
	bool added_opposite_parity_to_list;
	bool same_parity_first_part;
	bool opposite_parity_first_part;

	int next_long_term_index;

	int field_parity;

	if (H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture)
	{
		field_parity = 0;
	}
	else
	{
		field_parity = 1;
	}

	for (i=0; i<m_NumberOfDecodedFrames; i++)
	{
		m_DefaultIndexList0ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_DefaultIndexList0ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;

		m_DefaultIndexList1ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_DefaultIndexList1ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;

		m_RemappedIndexList0ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_RemappedIndexList0ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[2*i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList0[2*i+1] = UNUSED_ARRAY_POSITION;
		
		m_RemappedIndexList1ToArrayPosition[2*i] = UNUSED_ARRAY_POSITION;
		m_RemappedIndexList1ToArrayPosition[2*i+1] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList1[2*i] = UNUSED_ARRAY_POSITION;
		m_ArrayPositionToLowestRemappedIndexList1[2*i+1] = UNUSED_ARRAY_POSITION;

		if (m_TopStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]))
		{
			// Frame is a short term reference frame
			number_of_short_term_reference_fields[field_parity]++;

			poc_difference_list0[field_parity][i] = m_CurrentPictureOrderCount - m_TopPictureOrderCount[i];
			poc_difference_list1[field_parity][i] = poc_difference_list0[field_parity][i];

			if (poc_difference_list1[field_parity][i] >= 0)
			{
				poc_difference_list1[field_parity][i]++;
			}

			if (poc_difference_list0[field_parity][i] < 0) 
			{
				number_of_future_reference_fields[field_parity]++;
			}
			else
			{
				// Need to add 1 to past reference frame case to account for situation where the POC of a bottom
				// frame is the same as that for a top frame
				poc_difference_list0[field_parity][i]++;
			}
		}
		else
		{
			poc_difference_list0[field_parity][i] = 0;
			poc_difference_list1[field_parity][i] = 0;
		}

		if (m_BottomStoredForReference[i] && (UNUSED_ARRAY_POSITION == m_BottomLongTermIndex[i]))
		{
			// Frame is a short term reference frame
			number_of_short_term_reference_fields[1-field_parity]++;

			poc_difference_list0[1-field_parity][i] = m_CurrentPictureOrderCount - m_BottomPictureOrderCount[i];
			poc_difference_list1[1-field_parity][i] = poc_difference_list0[1-field_parity][i];

			if (poc_difference_list1[1-field_parity][i] >= 0)
			{
				poc_difference_list1[1-field_parity][i]++;
			}

			if (poc_difference_list0[1-field_parity][i] < 0) 
			{
				number_of_future_reference_fields[1-field_parity]++;
			}
			else
			{
				poc_difference_list0[1-field_parity][i]++;
			}
		}
		else
		{
			poc_difference_list0[1-field_parity][i] = 0;
			poc_difference_list1[1-field_parity][i] = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Create the list 0 default order amongst the future short term reference fields
	//////////////////////////////////////////////////////////////////////////////////

	default_index = 0;
	same_parity_first_part = true;
	opposite_parity_first_part = true;

	do
	{
		added_same_parity_to_list = false;
		added_opposite_parity_to_list = false;

		/////////////////////////////////////////////////////////////////////////////
		// Firstly look for same parity fields
		/////////////////////////////////////////////////////////////////////////////
		
		if (same_parity_first_part)
		{
			// Look for largest POC less than the current POC, i.e. the smallest positive POC difference
		
			int lowest_poc_difference = INT_MAX;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list0[0][i] < lowest_poc_difference) &&
					(poc_difference_list0[0][i] > 0))
				{
					lowest_poc_difference = poc_difference_list0[0][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+field_parity);
				poc_difference_list0[0][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_same_parity_to_list = true;
			}
			else
			{
				// Did not find one, look for pictures with higher POC than current
				same_parity_first_part = false;
			}
		}

		if (!same_parity_first_part && !added_same_parity_to_list)
		{
			// Not already allocated an index on this iteration

			// Look for smallest POC greater than the current POC, i.e. the largest negative POC difference (nearest to zero)

			int highest_poc_difference = INT_MIN;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list0[0][i] > highest_poc_difference) &&
					(poc_difference_list0[0][i] < 0))
				{
					highest_poc_difference = poc_difference_list0[0][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+field_parity);
				poc_difference_list0[0][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_same_parity_to_list = true;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// Secondly look for opposite parity fields
		/////////////////////////////////////////////////////////////////////////////
		
		if (opposite_parity_first_part)
		{
			// Look for largest POC less than the current POC, i.e. the smallest positive POC difference
		
			int lowest_poc_difference = INT_MAX;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list0[1][i] < lowest_poc_difference) &&
					(poc_difference_list0[1][i] > 0))
				{
					lowest_poc_difference = poc_difference_list0[1][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+1-field_parity);
				poc_difference_list0[1][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_opposite_parity_to_list = true;
			}
			else
			{
				// Did not find one, look for pictures with higher POC than current
				opposite_parity_first_part = false;
			}
		}

		if (!opposite_parity_first_part && !added_opposite_parity_to_list)
		{
			// Not already allocated an index on this iteration

			// Look for smallest POC greater than the current POC, i.e. the largest negative POC difference (nearest to zero)

			int highest_poc_difference = INT_MIN;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list0[1][i] > highest_poc_difference) &&
					(poc_difference_list0[1][i] < 0))
				{
					highest_poc_difference = poc_difference_list0[1][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+1-field_parity);
				poc_difference_list0[1][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_opposite_parity_to_list = true;
			}
		}
			
	} while (added_same_parity_to_list || added_opposite_parity_to_list);

	//////////////////////////////////////////////////////////////////////////////////
	// Create the list 1 default order amongst the future short term reference fields
	//////////////////////////////////////////////////////////////////////////////////

	default_index = 0;
	same_parity_first_part = true;
	opposite_parity_first_part = true;

	do
	{
		added_same_parity_to_list = false;
		added_opposite_parity_to_list = false;

		/////////////////////////////////////////////////////////////////////////////
		// Firstly look for same parity fields
		/////////////////////////////////////////////////////////////////////////////
		
		if (same_parity_first_part)
		{
			// Look for smallest POC greater than the current POC, i.e. the largest negative POC difference (nearest to zero)

			int highest_poc_difference = INT_MIN;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list1[0][i] > highest_poc_difference) &&
					(poc_difference_list1[0][i] < 0))
				{
					highest_poc_difference = poc_difference_list1[0][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+field_parity);
				poc_difference_list1[0][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_same_parity_to_list = true;
			}
			else
			{
				// Did not find one, look for pictures with higher POC than current
				same_parity_first_part = false;
			}
		}

		if (!same_parity_first_part && !added_same_parity_to_list)
		{
			// Not already allocated an index on this iteration

			// Look for largest POC less than the current POC, i.e. the smallest positive POC difference

			int lowest_poc_difference = INT_MAX;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list1[0][i] < lowest_poc_difference) &&
					(poc_difference_list1[0][i] > 0))
				{
					lowest_poc_difference = poc_difference_list1[0][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+field_parity);
				poc_difference_list1[0][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_same_parity_to_list = true;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// Secondly look for opposite parity fields
		/////////////////////////////////////////////////////////////////////////////
		
		if (opposite_parity_first_part)
		{
			// Look for smallest POC greater than the current POC, i.e. the largest negative POC difference (nearest to zero)

			int highest_poc_difference = INT_MIN;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list1[1][i] > highest_poc_difference) &&
					(poc_difference_list1[1][i] < 0))
				{
					highest_poc_difference = poc_difference_list1[1][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+1-field_parity);
				poc_difference_list1[1][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_opposite_parity_to_list = true;
			}
			else
			{
				// Did not find one, look for pictures with higher POC than current
				opposite_parity_first_part = false;
			}
		}

		if (!opposite_parity_first_part && !added_opposite_parity_to_list)
		{
			// Not already allocated an index on this iteration

			// Look for largest POC less than the current POC, i.e. the smallest positive POC difference

			int lowest_poc_difference = INT_MAX;
			int selected_array_position = UNUSED_ARRAY_POSITION;

			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((poc_difference_list1[1][i] < lowest_poc_difference) &&
					(poc_difference_list1[1][i] > 0))
				{
					lowest_poc_difference = poc_difference_list1[1][i];
					selected_array_position = i;
				}
			}

			if (UNUSED_ARRAY_POSITION != selected_array_position)
			{
				// Found one
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*selected_array_position+1-field_parity);
				poc_difference_list1[1][selected_array_position] = 0;  // Ensure it is not selected next time
				default_index++;
				added_opposite_parity_to_list = true;
			}
		}
			
	} while (added_same_parity_to_list || added_opposite_parity_to_list);

	/////////////////////////////////////////////////////////////////////////////
	// Swap the first two elements in list 1 if both lists are the same
	/////////////////////////////////////////////////////////////////////////////

	if ((number_of_short_term_reference_fields[0] + number_of_short_term_reference_fields[1]) > 1)
	{
		if (m_DefaultIndexList0ToArrayPosition[0] == m_DefaultIndexList1ToArrayPosition[0])
		{
			m_DefaultIndexList1ToArrayPosition[0] = m_DefaultIndexList0ToArrayPosition[1];
			m_DefaultIndexList1ToArrayPosition[1] = m_DefaultIndexList0ToArrayPosition[0];
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Create the default order amongst the long term reference pictures
	////////////////////////////////////////////////////////////////////////

	default_index = number_of_short_term_reference_fields[0] + number_of_short_term_reference_fields[1];
	next_long_term_index = 0;

	do
	{
		// Firstly look for same parity fields
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_TopLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				default_index++;
				break;
			}
			if ((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_BottomLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				default_index++;
				break;
			}
		}

		// Secondly look for opposite parity fields
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((H264_BOTTOM_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_TopLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i);
				default_index++;
				break;
			}
			if ((H264_TOP_FIELD_PICTURE == m_PictureTypeOfCurrentPicture) && (next_long_term_index == m_BottomLongTermIndex[i]))
			{
				m_DefaultIndexList0ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				m_DefaultIndexList1ToArrayPosition[default_index] = (UNSIGNED_SEPTET)(2*i+1);
				default_index++;
				break;
			}
		}

		next_long_term_index++;

	} while (next_long_term_index < m_MaximumLongTermIndexPlus1);

	////////////////////////////////////////////////////////////////////////
	// Initialise Parameters for Remapping of indices
	////////////////////////////////////////////////////////////////////////

	m_NextRemappingIndex = 0;
	m_RemappingFrameNumberPredictor = m_CurrentFrameNumber;
	m_RemappingPicNumberPredictor = 2 * m_CurrentFrameNumber + 1;
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ActOnNextFrameRemappingInstruction	*/
/* DESCRIPTION:	This acts on the given remapping instruction to continue 
				the building up of the list of remapped indices.
				The frame number predictor is stored in 
				m_RemappingFrameNumberPredictor, and the remapped index 
				that the command applies to is m_NextRemappingIndex.
				It is called in the decoder after processing each 
				remapping instruction. When no remapping instructions are
				received, it should be called once with type equal to 
				H264_RMPNI_END_OF_LOOP.
				remap_list0 true : apply command to list 0, else to list 1
				The type is one of: H264_RMPNI_NEGATIVE_ADPN, 
				H264_RMPNI_POSITIVE_ADPN, H264_RMPNI_LPIR and 
				H264_RMPNI_END_OF_LOOP.
				On H264_RMPNI_END_OF_LOOP, all unassigned remapped indices are 
				assigned in default order.									*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264DecodedPictureBuffer::ActOnNextFrameRemappingInstruction(bool remap_list0, 
																   h264_RMPNI_type type,
																   int value)
{
	////////////////////////////////////////////////////////////////////////
	// Find array position for the frame that the command applies to
	////////////////////////////////////////////////////////////////////////

	int i;
	int array_position = UNUSED_ARRAY_POSITION;

	if (m_NextRemappingIndex > MAXIMUM_NUMBER_OF_REMAPPED_INDICES) return;

	if ((H264_RMPNI_NEGATIVE_ADPN == type) || (H264_RMPNI_POSITIVE_ADPN == type))
	{
		unsigned int frame_number;

		if (H264_RMPNI_NEGATIVE_ADPN == type)
		{
			frame_number = (m_MaximumFrameNumberMinus1 + m_RemappingFrameNumberPredictor - value)
									& m_MaximumFrameNumberMinus1;
		}
		else
		{
			// H264_RMPNI_POSITIVE_ADPN == type
			frame_number = (m_RemappingFrameNumberPredictor + value + 1) & m_MaximumFrameNumberMinus1;
		}

		m_RemappingFrameNumberPredictor = frame_number;

		// Look for this picture number;
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if ((m_TopStoredForReference[i] && 
				(UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]) &&
				frame_number == m_TopFrameNumber[i]))
			{
				array_position = i;
				break;
			}
		}
	}
	else if (H264_RMPNI_LPIR == type)
	{
		// Look for this long term index;
		for (i=0; i<m_NumberOfDecodedFrames; i++)
		{
			if (m_TopLongTermIndex[i] == value)
			{
				array_position = i;
				break;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Use the array position to remap the next index
	////////////////////////////////////////////////////////////////////////

	if ((H264_RMPNI_NEGATIVE_ADPN == type) || (H264_RMPNI_POSITIVE_ADPN == type) || (H264_RMPNI_LPIR == type))
	{
		if (UNUSED_ARRAY_POSITION == array_position)
		{
			// Error condition: could not find a frame that corresponds to the remapping instruction
			return;
		}

		if (remap_list0)
		{
			m_RemappedIndexList0ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;

			if ((UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList0[array_position]) ||
				(m_NextRemappingIndex < m_ArrayPositionToLowestRemappedIndexList0[array_position]))
			{
				m_ArrayPositionToLowestRemappedIndexList0[array_position] = m_NextRemappingIndex;
			}
		}
//		else
//		{
//			m_RemappedIndexList1ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;
//
//			if ((UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList1[array_position]) ||
//				(m_NextRemappingIndex < m_ArrayPositionToLowestRemappedIndexList1[array_position]))
//			{
//				m_ArrayPositionToLowestRemappedIndexList1[array_position] = m_NextRemappingIndex;
//			}
//		}

		m_NextRemappingIndex++;
	}
	else if (H264_RMPNI_END_OF_LOOP == type)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// Complete the remapped index array by copying the order from the default list
		/////////////////////////////////////////////////////////////////////////////////

		int default_index = 0;

		if (remap_list0)
		{
			while (true)
			{
				array_position = m_DefaultIndexList0ToArrayPosition[default_index];

				if (UNUSED_ARRAY_POSITION == array_position)
				{
					// All default indices have been considered
					break;
				}

				assert((array_position >= 0) && (array_position < m_NumberOfDecodedFrames));

				if (UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList0[array_position])
				{
					// This array position has not already appeared in the remapped list, so insert it
					m_RemappedIndexList0ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;
					m_ArrayPositionToLowestRemappedIndexList0[array_position] = m_NextRemappingIndex;

					m_NextRemappingIndex++;
				}

				default_index++;

				if ((m_NextRemappingIndex >= MAXIMUM_NUMBER_OF_REMAPPED_INDICES) ||
					(default_index >= MAXIMUM_NUMBER_OF_STORED_FRAMES))
				{
					// Finished !
					break;
				}
			}
		}
//		else
//		{
//			while (true)
//			{
//				array_position = m_DefaultIndexList1ToArrayPosition[default_index];
//
//				if (UNUSED_ARRAY_POSITION == array_position)
//				{
//					// All default indices have been considered
//					break;
//				}
//
//				assert((array_position >= 0) && (array_position < m_NumberOfDecodedFrames));
//
//				if (UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList1[array_position])
//				{
//					// This array position has not already appeared in the remapped list, so insert it
//					m_RemappedIndexList1ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;
//					m_ArrayPositionToLowestRemappedIndexList1[array_position] = m_NextRemappingIndex;
//
//					m_NextRemappingIndex++;
//				}
//
//				default_index++;
//
//				if ((m_NextRemappingIndex >= MAXIMUM_NUMBER_OF_REMAPPED_INDICES) ||
//					(default_index >= MAXIMUM_NUMBER_OF_STORED_FRAMES))
//				{
//					// Finished !
//					break;
//				}
//			}
//		}

		// Reset for start of next list - especially at end of list 0 for list 1
		m_NextRemappingIndex = 0;
		m_RemappingFrameNumberPredictor = m_CurrentFrameNumber;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ActOnNextFieldRemappingInstruction	*/
/* DESCRIPTION:	This acts on the given remapping instruction to continue 
				the building up of the list of remapped indices.
				The frame number predictor is stored in 
				m_RemappingFrameNumberPredictor, and the remapped index 
				that the command applies to is m_NextRemappingIndex.
				It is called in the decoder after processing each 
				remapping instruction. When no remapping instructions are
				received, it should be called once with type equal to 
				H264_RMPNI_END_OF_LOOP.
				remap_list0 true : apply command to list 0, else to list 1
				The type is one of: H264_RMPNI_NEGATIVE_ADPN, 
				H264_RMPNI_POSITIVE_ADPN, H264_RMPNI_LPIR and 
				H264_RMPNI_END_OF_LOOP.
				On H264_RMPNI_END_OF_LOOP, all unassigned remapped indices are 
				assigned in default order.									*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

#ifdef __INTERLACE__

void CH264DecodedPictureBuffer::ActOnNextFieldRemappingInstruction(bool remap_list0, 
																   h264_RMPNI_type type,
																   int value)
{
	////////////////////////////////////////////////////////////////////////
	// Find array position for the frame that the command applies to
	////////////////////////////////////////////////////////////////////////

	int i;
	int array_position = UNUSED_ARRAY_POSITION;
	unsigned int frame_number;

	if (m_NextRemappingIndex > 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES) return;

	if ((H264_RMPNI_NEGATIVE_ADPN == type) || (H264_RMPNI_POSITIVE_ADPN == type))
	{
		unsigned int pic_num_no_wrap;

		if (H264_RMPNI_NEGATIVE_ADPN == type)
		{
			pic_num_no_wrap = (m_MaximumFieldNumberMinus1 + m_RemappingPicNumberPredictor - value)
									& m_MaximumFieldNumberMinus1;
		}
		else
		{
			// H264_RMPNI_POSITIVE_ADPN == type
			pic_num_no_wrap = (m_RemappingPicNumberPredictor + value + 1) & m_MaximumFieldNumberMinus1;
		}

		m_RemappingPicNumberPredictor = pic_num_no_wrap;

		frame_number = pic_num_no_wrap >> 1;

		if (((0 == (pic_num_no_wrap & 1)) && (m_PictureTypeOfCurrentPicture == H264_BOTTOM_FIELD_PICTURE)) ||
			((1 == (pic_num_no_wrap & 1)) && (m_PictureTypeOfCurrentPicture == H264_TOP_FIELD_PICTURE)))
		{
			// Look for this frame number in top fields
			int i;
			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((m_TopStoredForReference[i] && 
					(UNUSED_ARRAY_POSITION == m_TopLongTermIndex[i]) &&
					frame_number == m_TopFrameNumber[i]))
				{
					array_position = 2 * i;
					break;
				}
			}
		}
		else
		{
			// Look for this frame number in bottom fields
			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if ((m_BottomStoredForReference[i] && 
					(UNUSED_ARRAY_POSITION == m_BottomLongTermIndex[i]) &&
					frame_number == m_BottomFrameNumber[i]))
				{
					array_position = 2 * i + 1;
					break;
				}
			}
		}
	}
	else if (H264_RMPNI_LPIR == type)
	{
		if (((0 == (value & 1)) && (m_PictureTypeOfCurrentPicture == H264_BOTTOM_FIELD_PICTURE)) ||
			((1 == (value & 1)) && (m_PictureTypeOfCurrentPicture == H264_TOP_FIELD_PICTURE)))
		{
			// Look for this long term index;
			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if (m_TopLongTermIndex[i] == (value>>1))
				{
					array_position = 2 * i;
					break;
				}
			}
		}
		else
		{
			// Look for this long term index;
			for (i=0; i<m_NumberOfDecodedFrames; i++)
			{
				if (m_BottomLongTermIndex[i] == (value>>1))
				{
					array_position = 2 * i + 1;
					break;
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Use the array position to remap the next index
	////////////////////////////////////////////////////////////////////////

	if ((H264_RMPNI_NEGATIVE_ADPN == type) || (H264_RMPNI_POSITIVE_ADPN == type) || (H264_RMPNI_LPIR == type))
	{
		if (UNUSED_ARRAY_POSITION == array_position)
		{
			// Error condition: could not find a frame that corresponds to the remapping instruction
			return;
		}

		if (remap_list0)
		{
			m_RemappedIndexList0ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;

			if ((UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList0[array_position]) ||
				(m_NextRemappingIndex < m_ArrayPositionToLowestRemappedIndexList0[array_position]))
			{
				m_ArrayPositionToLowestRemappedIndexList0[array_position] = m_NextRemappingIndex;
			}
		}
		else
		{
			m_RemappedIndexList1ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)array_position;

			if ((UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList1[array_position]) ||
				(m_NextRemappingIndex < m_ArrayPositionToLowestRemappedIndexList1[array_position]))
			{
				m_ArrayPositionToLowestRemappedIndexList1[array_position] = m_NextRemappingIndex;
			}
		}

		m_NextRemappingIndex++;
	}
	else if (H264_RMPNI_END_OF_LOOP == type)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// Complete the remapped index array by copying the order from the default list
		/////////////////////////////////////////////////////////////////////////////////

		int default_index = 0;
		int array_position;

		if (remap_list0)
		{
			while (true)
			{
				array_position = m_DefaultIndexList0ToArrayPosition[default_index];

				if (UNUSED_ARRAY_POSITION == array_position)
				{
					// All default indices have been considered
					break;
				}

				assert((array_position >= 0) && (array_position < 2*m_NumberOfDecodedFrames));

				if (UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList0[array_position])
				{
					// This array position has not already appeared in the remapped list, so insert it
					m_RemappedIndexList0ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)(array_position);
					m_ArrayPositionToLowestRemappedIndexList0[array_position] = m_NextRemappingIndex;

					m_NextRemappingIndex++;
				}

				default_index++;

				if ((m_NextRemappingIndex >= 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES) ||
					(default_index >= 2*MAXIMUM_NUMBER_OF_STORED_FRAMES))
				{
					// Finished !
					break;
				}
			}
		}
		else
		{
			while (true)
			{
				array_position = m_DefaultIndexList1ToArrayPosition[default_index];

				if (UNUSED_ARRAY_POSITION == array_position)
				{
					// All default indices have been considered
					break;
				}

				assert((array_position >= 0) && (array_position < 2*m_NumberOfDecodedFrames));

				if (UNUSED_ARRAY_POSITION == m_ArrayPositionToLowestRemappedIndexList1[array_position])
				{
					// This array position has not already appeared in the remapped list, so insert it
					m_RemappedIndexList1ToArrayPosition[m_NextRemappingIndex] = (UNSIGNED_SEPTET)(array_position);
					m_ArrayPositionToLowestRemappedIndexList1[array_position] = m_NextRemappingIndex;

					m_NextRemappingIndex++;
				}

				default_index++;

				if ((m_NextRemappingIndex >= 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES) ||
					(default_index >= 2*MAXIMUM_NUMBER_OF_STORED_FRAMES))
				{
					// Finished !
					break;
				}
			}
		}

		// Reset for start of next list - especially at end of list 0 for list 1
		m_NextRemappingIndex = 0;
	        m_RemappingPicNumberPredictor = 2 * m_CurrentFrameNumber + 1;

	}
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::RemappedIndexList0ToArrayPosition*/
/* DESCRIPTION:	This returns the array position of the picture corresponding
				to the given remapped index.								*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

#ifdef _DEBUG
// If debug, this is present here to allow the assert to be present
// If release, it is in the .h file without the assert

UNSIGNED_SEPTET CH264DecodedPictureBuffer::RemappedIndexList0ToArrayPosition(int remapped_index)
{
	assert((remapped_index >= 0) && (remapped_index < 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES));
	assert(UNUSED_ARRAY_POSITION != m_RemappedIndexList0ToArrayPosition[remapped_index]);

	return m_RemappedIndexList0ToArrayPosition[remapped_index];
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::RemappedIndexList1ToArrayPosition*/
/* DESCRIPTION:	This returns the array position of the picture corresponding
				to the given remapped index.								*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

#ifdef _DEBUG
// If debug, this is present here to allow the assert to be present
// If release, it is in the .h file without the assert

UNSIGNED_SEPTET CH264DecodedPictureBuffer::RemappedIndexList1ToArrayPosition(int remapped_index)
{
	assert((remapped_index >= 0) && (remapped_index < 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES));
	assert(UNUSED_ARRAY_POSITION != m_RemappedIndexList1ToArrayPosition[remapped_index]);

	return m_RemappedIndexList1ToArrayPosition[remapped_index];
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::ArrayPositionToLowestRemappedIndexList0*/
/* DESCRIPTION:	This returns the lowest remapped index from list 0 that is
				assoicated with the picture at the given array position.	*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

#ifdef _DEBUG
// If debug, this is present here to allow the assert to be present
// If release, it is in the .h file without the assert

UNSIGNED_SEPTET CH264DecodedPictureBuffer::ArrayPositionToLowestRemappedIndexList0(int array_position)
{
	assert((array_position >= 0) && (array_position < 2*MAXIMUM_NUMBER_OF_STORED_FRAMES));
	assert(UNUSED_ARRAY_POSITION != m_ArrayPositionToLowestRemappedIndexList0[array_position]);

	return m_ArrayPositionToLowestRemappedIndexList0[array_position];
}

#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::PictureIsLongTermByArrayPosition	*/
/* DESCRIPTION:	This returns whether the specified picture is stored as a 
				long term reference picture.								*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

bool CH264DecodedPictureBuffer::PictureIsLongTermByArrayPosition(int array_position) const
{
	assert((array_position >= 0) && (array_position < 2*MAXIMUM_NUMBER_OF_STORED_FRAMES));

		return ((array_position < MAXIMUM_NUMBER_OF_STORED_FRAMES) && 
				(m_TopStoredForReference[array_position]) &&
				(UNUSED_ARRAY_POSITION != m_TopLongTermIndex[array_position]));
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264DecodedPictureBuffer::PictureIsLongTermByRemappedIndexList1*/
/* DESCRIPTION:	This returns whether the specified picture is stored as a 
				long term reference picture.								*/
/* RETURNS:		As above.													*/
/* ------------------------------------------------------------------------ */

//bool CH264DecodedPictureBuffer::PictureIsLongTermByRemappedIndexList1(int remapped_index) const
//{
//	assert((remapped_index >= 0) && (remapped_index < 2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES));
//	int array_position = m_RemappedIndexList1ToArrayPosition[remapped_index];
//
//	if (UNUSED_ARRAY_POSITION != array_position)
//	{
//		return PictureIsLongTermByArrayPosition(array_position);
//	}
//	else
//	{
//		// Error condition
//		return false;
//	}
//}

//* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

