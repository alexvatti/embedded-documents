/* ------------------------------------------------------------------------ */
/* FILE:			H264EncodedPictureBuffer.h								*/
/* DESCRIPTION:		Decoded Picture Buffer declarations for H.264			*/
/* VERSION:			v2.20													*/
/* DATE:			22 September 2005										*/
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
	int m_CurrentGopHierarchyNumber;

	bool m_IDR_pic_flag;
	bool m_FlushNonReferencePictures;
	bool m_StoreIDRAsLongTerm;



	int m_ArrayPositionOfCurrentPicture;
 

	////////////////////////////////////////////////////////////////////////////////
	// Encoder Specific Member Variable Declarations
	////////////////////////////////////////////////////////////////////////////////

	int m_hsize; // horizontal picture size
	int m_vsize; // vertical picture size

	
	bool m_ExponentialSlidingWindow;
	bool m_ReferencesWithNonZeroGopHierarchyNumber;

	int m_OutputPictureCount;

	bool m_SendMLIP1InNextSliceHeader;

	////////////////////////////////////////////////////////////////////////////////
	// Member Function Declarations
	////////////////////////////////////////////////////////////////////////////////

	bool Create(void);



public:
	CH264EncodedPictureBuffer(void);
	~CH264EncodedPictureBuffer(void);

	bool Modify(int number_of_normal_frames, 
				int number_of_reference_frames,
				bool exponential_sliding_window,
				int frame_width, int frame_height);

	void Destroy(void);


	int GetMaximumFrameNumberMinus1(void) { return m_MaximumFrameNumberMinus1; }


	void PrepareForStoringFrame(bool IDR_pic_flag, bool reset_buffer, bool flush_non_reference_pictures,
								bool store_IDR_as_long_term, bool p_with_b_direct_problem);

	void GetIDRParameters(bool& flush_non_reference_pictures, bool& store_IDR_as_long_term)
		{	
			flush_non_reference_pictures = m_FlushNonReferencePictures;
			store_IDR_as_long_term = m_StoreIDRAsLongTerm;
		}
	void GetPictureForEncoding(CBTPicture ** picture);


	int OutputFrame(CBTPicture ** output_picture);
	int OutputFrameBumping(CBTPicture ** output_frame);

	inline CBTPicture * GetRefPicture()
	{
        return m_Picture[ 1 - m_ArrayPositionOfCurrentPicture];
	}



};


#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
