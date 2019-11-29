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
/* May contain code (c) British Telecommunications plc 2005        */

#ifndef	H264_ENCODED_PICTURE_BUFFER_H
#define	H264_ENCODED_PICTURE_BUFFER_H

///////////////////////////////////////////
// Definition of #defines
///////////////////////////////////////////


#define MAXIMUM_NUMBER_OF_REMAPPED_INDICES		32

/////////////////////////////////////////////////
// Definition of H264EncodedPictureBuffer Class
/////////////////////////////////////////////////

class CH264EncodedPictureBuffer
{
public:
	
	////////////////////////////////////////////////////////////////////////////////
	// The following arrays have index array_position
	////////////////////////////////////////////////////////////////////////////////

	CBTPicture * m_Picture[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	////////////////////////////////////////////////////////////////////////////////
	// Other Member Variable Declarations
	////////////////////////////////////////////////////////////////////////////////

	int m_NumberOfCreatedFrames;   // The number allocated in memory
	int m_NumberOfNormalFrames;    // The number of frames allocated less the weighted frames, is 
								   // m_NumberOfCreatedFrames when weighting is not used and is 
								   // m_NumberOfCreatedFrames/2 when weighting is used.
	int m_NumberOfReferenceFrames; // The number that can be used as reference frames


	int m_MaximumFrameNumberMinus1;
	int m_MaximumFieldNumberMinus1;
	unsigned int m_CurrentFrameNumber;
	int m_CurrentPictureOrderCount;
	int m_CurrentTopPictureOrderCount;
	int m_CurrentGopHierarchyNumber;

	bool m_IDR_pic_flag;
	bool m_FlushNonReferencePictures;
	bool m_StoreIDRAsLongTerm;


	int m_ArrayPositionOfMostRecentReferencePicture;
	int m_ArrayPositionOfMostRecentKeyReferencePicture;  // Most recent reference with GopHierarchyNumber == 0

	int m_ArrayPositionOfCurrentPicture;
 

	////////////////////////////////////////////////////////////////////////////////
	// Encoder Specific Member Variable Declarations
	////////////////////////////////////////////////////////////////////////////////

	int m_hsize; // horizontal picture size
	int m_vsize; // vertical picture size

	bool m_StoreReferenceFrames;
	bool m_StoreReferenceFields;

	int m_FrameAreaIncludingBorderPels;
	int m_QuarterPelFrameArrayOffset[16];
	
	bool m_ExponentialSlidingWindow;
	bool m_ReferencesWithNonZeroGopHierarchyNumber;

	int m_OutputPictureCount;

	bool m_SendMLIP1InNextSliceHeader;

	////////////////////////////////////////////////////////////////////////////////
	// Member Function Declarations
	////////////////////////////////////////////////////////////////////////////////

	bool Create(void);

	void ProcessMMCOReset(void);



public:
	CH264EncodedPictureBuffer(void);
	~CH264EncodedPictureBuffer(void);

	bool Modify(int number_of_normal_frames, 
				int number_of_reference_frames,
				bool exponential_sliding_window,
				int frame_width, int frame_height,
				bool store_reference_frames,
				bool store_reference_fields);

	void Destroy(void);


	int GetMaximumFrameNumberMinus1(void) { return m_MaximumFrameNumberMinus1; }

	void SetEncodingPictureParameters(unsigned int frame_number, int picture_order_count,
									  int top_picture_order_count);


	void PrepareForStoringFrame(bool IDR_pic_flag, bool reset_buffer, bool flush_non_reference_pictures,
								bool store_IDR_as_long_term, bool p_with_b_direct_problem);



	int GetNumberOfReferenceFrames()
	{
		return m_NumberOfReferenceFrames;
	}

	int GetNumberOfNormalFrames()
	{
		return m_NumberOfNormalFrames;
	}


	void GetIDRParameters(bool& flush_non_reference_pictures, bool& store_IDR_as_long_term)
		{	
			flush_non_reference_pictures = m_FlushNonReferencePictures;
			store_IDR_as_long_term = m_StoreIDRAsLongTerm;
		}
	void GetPictureForEncoding(CBTPicture ** picture);


	int StorePicture(bool use_for_reference);

	unsigned char GetArrayPositionOfMostRecentReferencePicture(void) { return m_ArrayPositionOfMostRecentReferencePicture; }

	unsigned char GetArrayPositionOfMostRecentKeyReferencePicture(void) { return m_ArrayPositionOfMostRecentKeyReferencePicture; }

	int OutputFrame(CBTPicture ** output_picture);
	int OutputFrameBumping(CBTPicture ** output_frame);

	CBTPicture * GetRefPicture()
	{
        return m_Picture[ 1 - m_ArrayPositionOfCurrentPicture];
	}



};


#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
