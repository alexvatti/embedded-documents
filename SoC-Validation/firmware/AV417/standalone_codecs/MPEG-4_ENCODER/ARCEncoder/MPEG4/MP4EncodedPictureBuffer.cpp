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

#include "BTPicture.h"

#include "MP4EncoderControl.h"

#include "MP4EncodedPictureBuffer.h"

#include "ArcEEOffsetTables.h"

//#include "ArcMotionEstimation.h"

#define UNUSED_ARRAY_POSITION 127

#define FORCE_TO_END_OF_LIST_USAGE (-10)

/*
The purpose of FORCE_TO_END_OF_LIST_USAGE is as follows.
When B pictures are present in the encoded bitstream, it is best not to use
all of the reference pictures available when coding a P picture, as one of
these will no longer be available when the next B picture is coded, and will
make direct mode unavailable when the co-located macroblock has used this
now-missing reference.
In the absence of adaptive memory management, the picture to avoid using
as a reference for the P picture would always be the one at the end of the
default index list, and so could be avoided by effecting reducing the number
of reference frames by one (and fields by two).
But with adaptive memory management, the picture to avoid using as a
reference for the P picture could be anywhere in the default list. It will
be identified during the PrepareForStoringFrame stage, typically in creating
the MMCO commands. The strategy is therefore at this stage to set the usage
to a large negative value to force the picture to the end of the remapped
index list, and to avoid searching it as a reference by again reducing the
number of reference pictures used, and searching them in remapped order.
The number of number of reference pictures used is calculated when creating
the remapped indices, making use of usage figures.
*/

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::CH264EncodedPictureBuffer		*/
/* DESCRIPTION:	Constructor
				This class manages the storage of reference pictures, and 
				non reference pictures awaiting output.
				It is intended for use in the encoder.                      */
/* ------------------------------------------------------------------------ */

CH264EncodedPictureBuffer::CH264EncodedPictureBuffer(void)
{
	m_NumberOfCreatedFrames = 0;
	m_NumberOfNormalFrames = 0;
	m_NumberOfReferenceFrames = 0;

	m_hsize = 0;
	m_vsize = 0;
	m_FrameAreaIncludingBorderPels = 0;

	
	m_StoreReferenceFrames = false;
	m_StoreReferenceFields = false;

	for (int i=0; i<MAXIMUM_NUMBER_OF_STORED_FRAMES; i++)
	{
		m_Picture[i] = 0;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::~CH264EncodedPictureBuffer		*/
/* DESCRIPTION:	Destructor                                                  */
/* ------------------------------------------------------------------------ */

CH264EncodedPictureBuffer::~CH264EncodedPictureBuffer(void)
{
	Destroy();
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::Create							*/
/* DESCRIPTION:	Creation :
				This class is used for encoded picture buffering. 
				A number of pictures are stored within this class, and they 
				are managed using indices. This function allocates memory 
				for the requested number of decoded frames up to a maximum 
				of MAXIMUM_NUMBER_OF_STORED_FRAMES.                         */
/* RETURNS:		true if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264EncodedPictureBuffer::Create(void)
{


	m_IDR_pic_flag = false;
	m_FlushNonReferencePictures = false;
	m_StoreIDRAsLongTerm = false;
	m_ReferencesWithNonZeroGopHierarchyNumber = false;


	m_SendMLIP1InNextSliceHeader = false;


	m_ArrayPositionOfCurrentPicture = 0;
	m_OutputPictureCount = 0;

	// Calculate m_MaximumFrameNumberMinus1.
	// Emphasis is on efficiency and with little thought of error resilience
	if (m_ExponentialSlidingWindow)
	{
		m_MaximumFrameNumberMinus1 = (1 << (m_NumberOfReferenceFrames - 0)) - 1;
		if (m_MaximumFrameNumberMinus1 < 15) m_MaximumFrameNumberMinus1 = 15;
	}
	else
	{
		if ((m_NumberOfReferenceFrames - 0) > 7)
		{
			m_MaximumFrameNumberMinus1 = 31;
		}
		else
		{
			m_MaximumFrameNumberMinus1 = 15;
		}
	}
	m_MaximumFieldNumberMinus1 = 2 * m_MaximumFrameNumberMinus1 + 1;

	for (int i=0; i<2; i++)
	{
		m_Picture[i] = new CBTPicture();

		if (0 == m_Picture[i])
		{
			// Memory allocation failure
			Destroy();
			assert(false);
			return false;
		}
		
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::Modify							*/
/* DESCRIPTION:	Creation and Modification:
				This class is used for encoded picture buffering. 
				A number of pictures are stored within this class, and they 
				are managed using indices. This function allocates memory 
				for the requested number of decoded frames up to a maximum 
				of MAXIMUM_NUMBER_OF_STORED_FRAMES.
				number_of_normal_frames is no greater than this, and 
				indicates the maximum number of frames that can be use for 
				storing reference frames and the curently encoded picture.
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
				is destroyed.
				When weighted prediction is used, the lower
				number_of_normal_frames frames are used as normal and the
				upper number_of_normal_frames are used for weighted 
				equivalents to the reference pictures, with a spare slot
				corresponding to the currently decoded picture. This
				simplifies mapping between array position of normal
				reference picture and weighted reference picture.
				number_of_long_term_frames indicates how many frames can be 
				stored as long term frames. The last stored frame before a 
				scene change indication is marked as a long term frame
				exponential_sliding_window indicates whether the short term 
				reference pictures are to be managed using an exponential 
				sliding window.
				frame_width indicates source frame width in pels
				frame_height indicates source frame height in pels
				store_reference_frames and store_reference_fields indicate
				whether reference data is to be stored, in quarter pel 
				arrays, as either frame data, field data or both (both 
				false implies intra only encoding).                         */
/* RETURNS:		true if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264EncodedPictureBuffer::Modify(int number_of_normal_frames, 
									   int number_of_reference_frames,
									   bool exponential_sliding_window,
									   int frame_width, int frame_height,
									   bool store_reference_frames,
									   bool store_reference_fields)
{
	if ((0 == number_of_normal_frames) || (0 == number_of_reference_frames)) 
	{
		assert(false);
		return false;
	}

	// Clip the input parameters to the maximum allowed
	if (number_of_normal_frames > MAXIMUM_NUMBER_OF_STORED_FRAMES)
	{
		number_of_normal_frames = MAXIMUM_NUMBER_OF_STORED_FRAMES;
	}

	if (number_of_reference_frames >= number_of_normal_frames)
	{
		number_of_reference_frames = number_of_normal_frames - 1;
	}

	int number_of_created_frames = number_of_normal_frames;



	m_ExponentialSlidingWindow = exponential_sliding_window;

	m_hsize = frame_width;
	m_vsize = frame_height;

	m_StoreReferenceFrames = store_reference_frames;
	m_StoreReferenceFields = store_reference_fields;

	m_FrameAreaIncludingBorderPels = (m_hsize + NUMBER_OF_LEFT_BORDER_PELS + NUMBER_OF_RIGHT_BORDER_PELS) *
									 (m_vsize + NUMBER_OF_TOP_BORDER_PELS + NUMBER_OF_BOTTOM_BORDER_PELS);



	for (int quarter_pel_position=0; quarter_pel_position<16; quarter_pel_position++)
	{
		m_QuarterPelFrameArrayOffset[quarter_pel_position] = quarter_pel_position * m_FrameAreaIncludingBorderPels;


	}

	if (0 == m_NumberOfCreatedFrames)
	{
		// This indicates it is not yet created, so just call create
		m_NumberOfCreatedFrames = number_of_created_frames;
		m_NumberOfNormalFrames = number_of_normal_frames;
		m_NumberOfReferenceFrames = number_of_reference_frames;

		return Create();
	}

	if (number_of_created_frames <= m_NumberOfCreatedFrames)
	{
		// Don't change the memory allocations or m_NumberOfCreatedFrames as
		// this would require shifting data around while the only benefit 
		// would be a reduction in current memory usage.
		// But do change m_NumberOfReferenceFrames as this is needed to operate
		// the sliding window correctly. There should be no side effects as
		// the current picture should be an IDR picture.
		m_NumberOfReferenceFrames = number_of_reference_frames;
		m_NumberOfNormalFrames = number_of_normal_frames;

		if (m_NumberOfReferenceFrames >= m_NumberOfNormalFrames)
		{
			m_NumberOfReferenceFrames = m_NumberOfNormalFrames - 1;
		}

		return true;
	}

	// Extra memory must be allocated

	m_SendMLIP1InNextSliceHeader = false;

	// Calculate m_MaximumFrameNumberMinus1.
	// Emphasis is on efficiency and with little thought of error resilience
	if (m_ExponentialSlidingWindow)
	{
		m_MaximumFrameNumberMinus1 = (1 << m_NumberOfReferenceFrames ) - 1;
		if (m_MaximumFrameNumberMinus1 < 15) m_MaximumFrameNumberMinus1 = 15;
	}
	else
	{
		if (m_NumberOfReferenceFrames  > 7)
		{
			m_MaximumFrameNumberMinus1 = 31;
		}
		else
		{
			m_MaximumFrameNumberMinus1 = 15;
		}
	}
	m_MaximumFieldNumberMinus1 = 2 * m_MaximumFrameNumberMinus1 + 1;

	for (int i=m_NumberOfCreatedFrames; i<number_of_created_frames; i++)
	{
		m_Picture[i] = new CBTPicture();

		if (0 == m_Picture[i])
		{
			// Memory allocation failure
			Destroy();
			assert(false);
			return false;
		}

	}

	m_NumberOfCreatedFrames = number_of_created_frames;
	m_NumberOfNormalFrames = number_of_normal_frames;
	m_NumberOfReferenceFrames = number_of_reference_frames;

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::Destroy							*/
/* DESCRIPTION:	Deletes all picture memories, and sets their pointers to 
				zero. Sets the number of reference frames to zero.          */
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264EncodedPictureBuffer::Destroy(void)
{
	for (int i=0; i<m_NumberOfCreatedFrames; i++)
	{
		if (0 != m_Picture[i])
		{
			delete m_Picture[i];

			m_Picture[i] = 0;
		}	
	}

	m_NumberOfCreatedFrames = 0;
	m_NumberOfNormalFrames = 0;
	m_NumberOfReferenceFrames = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::ProcessMMCOReset					*/
/* DESCRIPTION:	This carries out functions associated with MMCO reset.
				All reference pictures are marked as not used for reference */
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264EncodedPictureBuffer::ProcessMMCOReset(void)
{
	// Frame Number is considered to be zero after a reset, hence there is
	// discontinuity in the series of its values
	m_CurrentFrameNumber = 0;
	m_CurrentTopPictureOrderCount -= m_CurrentPictureOrderCount;
	m_CurrentPictureOrderCount = 0;

	m_ArrayPositionOfMostRecentReferencePicture = 0;
	m_ArrayPositionOfMostRecentKeyReferencePicture = 0;

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::SetEncodingPictureParameters		*/
/* DESCRIPTION:	The value of frame number for the current frame is set.
				It is needed for subsequent reference frame remapping and 
				MMCO operations.
				The value of Picture Order Count for the current picture 
				is stored, depending on the picture type. In the encoder, 
				this is needed for subsequent default reference index 
				ordering.													*/
/* RETURNS:		The return value is void.									*/
/* ------------------------------------------------------------------------ */

void CH264EncodedPictureBuffer::SetEncodingPictureParameters(unsigned int frame_number, int picture_order_count,
															 int top_picture_order_count)
{
	m_CurrentFrameNumber = frame_number;

	m_CurrentPictureOrderCount = picture_order_count;

	// Find a frame that is not used for reference and not waiting for output
	m_CurrentTopPictureOrderCount = top_picture_order_count;

}






/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::GetPictureForEncoding			*/
/* DESCRIPTION:	This is used to get a pointer to a frame in the encoded 
				picture buffer that can be used to encode the current 
				picture into.
				m_PictureTypeOfCurrentPicture MUST be set before calling
				this function. It is set by calling 
				SetPictureTypeOfCurrentPicture.
				Normally, there is at least one frame marked as 
				not used for reference and not waiting to be output, in 
				which case one such frame is selected as the destination 
				for decoding the current frame.
				Otherwise, I think it is an error condition. For now, 
				choose to use a frame that is not used for reference, and 
				if there is no such one, to use array position zero.
				Better techniques could be developed in due course.
				gop_hierarchy_number indicates the gop hierarchy number for
				the current picture. This is used to ensure that a picture
				can only be encoded using references that have a lower
				than or equal gop_hierarchy_number.						*/
/* RETURNS:		A CBTPicture** into which the calling function can write 
				the decoded frame. This pointer is guaranteed to be correct 
				only until the next call to StoreFrame, which is used to 
				indicate that encoding of the picture has been completed.
				The return value is void.									*/
/* ------------------------------------------------------------------------ */

void CH264EncodedPictureBuffer::GetPictureForEncoding(CBTPicture ** picture)
{
	m_ArrayPositionOfCurrentPicture = 1-m_ArrayPositionOfCurrentPicture;
    *picture= m_Picture[m_ArrayPositionOfCurrentPicture];

	assert(0 != *picture);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::StorePicture						*/
/* DESCRIPTION:	This is used to store the current picture in the encoded 
				picture buffer. The picture previously returned by 
				GetPictureForEncoding is stored according to the parameters
				Firstly , the contents of the buffer are updated as 
				appropriate, either by sliding window method or the 
				adaptive (MMCO) method.
				Next, the picture is found in the encoded picture buffer 
				and is marked appropriately according to parameters set 
				during encoding.											*/
/* RETURNS:		The return value is the array position in which the picture 
				is stored. This can be used for subsequent storing of the 
				motion vectors for use in direct mode.						*/
/* ------------------------------------------------------------------------ */

int CH264EncodedPictureBuffer::StorePicture(bool use_for_reference)
{

	if (use_for_reference)
	{
		if (m_IDR_pic_flag)
		{
			ProcessMMCOReset();
		}

		 m_ArrayPositionOfMostRecentReferencePicture = m_ArrayPositionOfCurrentPicture;

	    if (0 == m_CurrentGopHierarchyNumber)
	    {
		    m_ArrayPositionOfMostRecentKeyReferencePicture = m_ArrayPositionOfCurrentPicture;
	    }
	}
						
	return m_ArrayPositionOfCurrentPicture;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264EncodedPictureBuffer::OutputFrame						*/
/* DESCRIPTION:	This is sets output_frame to point to the next frame to
				be output and zero if there is no such frame. The next 
				frame to be output is the one marked as waiting for output 
				with the lowest value of frame order count. It is then 
				marked as not waiting for output.
				Frames around an IDR or MMCO Reset picture have frame 
				order counts on different scales. Those before the reseting 
				point must be output before the one at the reset point and 
				those encoded afterwards. The ones to be output earlier are 
				indicated by m_OutputBeforeIDR[] being true.				*/
/* RETURNS:		The array position of the output frame.						*/
/* ------------------------------------------------------------------------ */

int CH264EncodedPictureBuffer::OutputFrame(CBTPicture ** output_frame)
{
	int output_location = m_ArrayPositionOfCurrentPicture;

	if (output_location != UNUSED_ARRAY_POSITION)
	{
		*output_frame = m_Picture[output_location];
		(*output_frame)->m_fn = m_OutputPictureCount;
		m_OutputPictureCount++;

	}
	else
	{
		*output_frame = 0;
	}

	return output_location;
}








/* ------------------------------------------------------------------------ */







/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

