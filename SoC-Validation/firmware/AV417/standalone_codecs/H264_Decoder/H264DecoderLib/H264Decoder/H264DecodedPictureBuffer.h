/* ------------------------------------------------------------------------ */
/* FILE:			H264DecodedPictureBuffer.h								*/
/* DESCRIPTION:		Decoded Picture Buffer declarations for H.264			*/
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

#ifndef	H264_DECODED_PICTURE_BUFFER_H
#define	H264_DECODED_PICTURE_BUFFER_H

#include "H264MotionVectorList.h"
#include "H264SEIParameters.h"

class CBTPicture;

///////////////////////////////////////////
// Definition of #defines
///////////////////////////////////////////

#ifdef __INTERLACE__
#define MAXIMUM_NUMBER_OF_REMAPPED_INDICES		32
#else
#define MAXIMUM_NUMBER_OF_REMAPPED_INDICES		16
#endif

#define MAXIMUM_NUMBER_OF_MMCO_COMMANDS			(MAXIMUM_NUMBER_OF_REMAPPED_INDICES + 3)    // Is this right?

///////////////////////////////////////////
// Definition of MMCO_command_type
///////////////////////////////////////////

typedef struct 
{
	h264_MMCO_type type;

	int diff_pn_minus1;
	UNSIGNED_SEPTET long_term_index;
	UNSIGNED_SEPTET max_long_term_index_plus1;

} h264_MMCO_command_type;

/////////////////////////////////////////////////
// Definition of H264DecodedPictureBuffer Class
/////////////////////////////////////////////////

class CH264DecodedPictureBuffer
{
private:

	////////////////////////////////////////////////////////////////////////////////
	// The following arrays have index array_position
	////////////////////////////////////////////////////////////////////////////////

	CBTPicture * m_Picture[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	unsigned int m_TopFrameNumber[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	unsigned int m_BottomFrameNumber[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	int m_TopPictureOrderCount[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	int m_BottomPictureOrderCount[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	UNSIGNED_SEPTET m_TopLongTermIndex[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	UNSIGNED_SEPTET m_BottomLongTermIndex[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	bool m_TopStoredForReference[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	bool m_BottomStoredForReference[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	
	bool m_TopNonExisting[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	bool m_BottomNonExisting[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	
	bool m_TopWaitingToBeOutput[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	bool m_BottomWaitingToBeOutput[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	bool m_TopOutputBeforeIDR[MAXIMUM_NUMBER_OF_STORED_FRAMES];
	bool m_BottomOutputBeforeIDR[MAXIMUM_NUMBER_OF_STORED_FRAMES];

	UNSIGNED_SEPTET m_ArrayPositionToLowestRemappedIndexList0[2*MAXIMUM_NUMBER_OF_STORED_FRAMES];

	////////////////////////////////////////////////////////////////////////////////
	// The following arrays have index as default_index and remapped_index and outputs an array_position
	////////////////////////////////////////////////////////////////////////////////

	UNSIGNED_SEPTET m_DefaultIndexList0ToArrayPosition[2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES];

	UNSIGNED_SEPTET m_RemappedIndexList0ToArrayPosition[2*MAXIMUM_NUMBER_OF_REMAPPED_INDICES];

	////////////////////////////////////////////////////////////////////////////////
	// SEI Declarations
	////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////
	// Other Member Variable Declarations
	////////////////////////////////////////////////////////////////////////////////

	int m_NumberOfDecodedFrames;
	int m_NumberOfReferenceFrames;

	int m_MaximumLongTermIndexPlus1;

	bool m_SlidingWindowBufferingMode;  // True -> sliding window; false -> mmco mode

	unsigned int m_MaximumFrameNumberMinus1;
	unsigned int m_MaximumFieldNumberMinus1;
	bool m_CurrentFrameNumberContinuity;
	unsigned int m_CurrentFrameNumber;
	int m_CurrentPictureOrderCount;
	int m_CurrentTopPictureOrderCount;
	int m_CurrentBottomPictureOrderCount;

	bool m_IDR_pic_flag;
	bool m_FlushNonReferencePictures;
	bool m_StoreIDRAsLongTerm;

	UNSIGNED_SEPTET m_CurrentPictureLongTermIndex;
	bool m_MMCOReset;

	UNSIGNED_SEPTET m_NextRemappingIndex;
	unsigned int m_RemappingFrameNumberPredictor;
	unsigned int m_RemappingPicNumberPredictor;

	h264_MMCO_command_type m_MMCO_Commands[MAXIMUM_NUMBER_OF_MMCO_COMMANDS];

	int m_MMCO_WritePointer;

	int m_ArrayPositionOfMostRecentPicture;
	int m_ArrayPositionOfMostRecentReferencePicture;

	int m_ArrayPositionOfCurrentPicture;
	h264_picture_type m_PictureTypeOfCurrentPicture;

	////////////////////////////////////////////////////////////////////////////////
	// Member Function Declarations
	////////////////////////////////////////////////////////////////////////////////

	bool Create(int number_of_decoded_frames, 
				int number_of_reference_frames);

	void ProcessMMCOReset(void);

	void OperateSlidingWindow(void);
	void ActOnMMCOCommandsFrame(void);
	void ActOnMMCOCommandsField(void);

public:
	CH264DecodedPictureBuffer(void);
	~CH264DecodedPictureBuffer(void);

	bool Modify(int number_of_decoded_frames, 
				int number_of_reference_frames,
				unsigned int maximum_frame_number_minus1);

	void Destroy(void);

    inline void SetNextMMCOCommand (h264_MMCO_type type)
    {
    	if (m_MMCO_WritePointer >= MAXIMUM_NUMBER_OF_MMCO_COMMANDS) 
          return;
    	m_MMCO_Commands[m_MMCO_WritePointer++].type = type;
    }
    
    inline void SetNextMMCOCommand (h264_MMCO_type type,int diff)
    {
    	if (m_MMCO_WritePointer >= MAXIMUM_NUMBER_OF_MMCO_COMMANDS) 
          return;
    	m_MMCO_Commands[m_MMCO_WritePointer].type = type;
        m_MMCO_Commands[m_MMCO_WritePointer++].diff_pn_minus1 = diff;
    }

    inline void SetNextMMCOCommand (h264_MMCO_type type,int diff,UNSIGNED_SEPTET lti)
    {
        if (m_MMCO_WritePointer >= MAXIMUM_NUMBER_OF_MMCO_COMMANDS) 
          return;
        m_MMCO_Commands[m_MMCO_WritePointer].type = type;
        m_MMCO_Commands[m_MMCO_WritePointer].diff_pn_minus1 = diff;
        m_MMCO_Commands[m_MMCO_WritePointer++].long_term_index = lti;	
    }

    inline void SetNextMMCOCommandLTI(h264_MMCO_type type,UNSIGNED_SEPTET lti)
    {
    	if (m_MMCO_WritePointer >= MAXIMUM_NUMBER_OF_MMCO_COMMANDS) 
          return;
    	m_MMCO_Commands[m_MMCO_WritePointer].type = type;
        m_MMCO_Commands[m_MMCO_WritePointer++].long_term_index = lti;	
    }
    
    inline void SetNextMMCOCommandLTIP1(h264_MMCO_type type,UNSIGNED_SEPTET lti)
    {
    	if (m_MMCO_WritePointer >= MAXIMUM_NUMBER_OF_MMCO_COMMANDS) 
          return;
    	m_MMCO_Commands[m_MMCO_WritePointer].type = type;
        m_MMCO_Commands[m_MMCO_WritePointer++].max_long_term_index_plus1 = lti;	
    }

	inline void SetIDRParameters(bool IDR_pic_flag, bool flush_non_reference_pictures, bool store_IDR_as_long_term)
	{	
      m_IDR_pic_flag = IDR_pic_flag;
	  m_FlushNonReferencePictures = flush_non_reference_pictures;
	  m_StoreIDRAsLongTerm = store_IDR_as_long_term;
	}

	int GetNumberOfActiveReferenceFrames(void) const;

	inline void SetSlidingWindowBufferingMode(bool mode) { m_SlidingWindowBufferingMode = mode;}

	void NonExistingFrameMarking(void);
	void NonExistingFrameStorage(void);

	void GetPictureForDecoding(CBTPicture ** picture, h264_picture_type type);

	void SetDecodingPictureParameters(unsigned int frame_number, int picture_order_count,
									  int top_picture_order_count, int bottom_picture_order_count);

	int StorePicture(bool use_for_reference, bool* mmco_reset);


	int OutputFrame(CBTPicture ** output_frame);
	int OutputFrameBumping(CBTPicture ** output_frame);
    int OutputFrameBumpingNonExisting(CBTPicture ** output_frame);


	void CreateFrameDefaultIndicesForPSlice(void);



	void ActOnNextRemappingInstruction(bool remap_list0, h264_RMPNI_type type, int value)
	{
			ActOnNextFrameRemappingInstruction(remap_list0, type, value);
	}
	
	void ActOnNextFrameRemappingInstruction(bool remap_list0, h264_RMPNI_type type, int value);

#ifdef _DEBUG
// If debug, this is present in the .cpp file to allow the assert to be present
// If release, it is here without the assert

	UNSIGNED_SEPTET RemappedIndexList0ToArrayPosition(int remapped_index);
	UNSIGNED_SEPTET RemappedIndexList1ToArrayPosition(int remapped_index);
	UNSIGNED_SEPTET ArrayPositionToLowestRemappedIndexList0(int array_position);

#else

	inline UNSIGNED_SEPTET RemappedIndexList0ToArrayPosition(int remapped_index)
						{ return m_RemappedIndexList0ToArrayPosition[remapped_index]; }

	inline UNSIGNED_SEPTET ArrayPositionToLowestRemappedIndexList0(int array_position)
						{ return m_ArrayPositionToLowestRemappedIndexList0[array_position]; }
#endif

	CBTPicture * GetMostRecentPicture(void) { return GetPictureByArrayPosition(m_ArrayPositionOfMostRecentPicture); }
	CBTPicture * GetMostRecentReferencePicture(void) { return GetPictureByArrayPosition(m_ArrayPositionOfMostRecentReferencePicture); }

	CBTPicture * GetPictureByArrayPosition(int array_position) const
	{
			if (m_TopStoredForReference[array_position] || m_TopWaitingToBeOutput[array_position])
			{
				return m_Picture[array_position];
			}
			else
			{
				return 0;
			}
	}

	int GetFramePOCByArrayPosition(int frame_array_position)
		{ return MIN(m_TopPictureOrderCount[frame_array_position],m_BottomPictureOrderCount[frame_array_position]); }


	bool PictureIsLongTermByArrayPosition(int array_position) const;


	int GetArrayPositionOfCurrentPicture()
	{
		return m_ArrayPositionOfCurrentPicture;
	}

};


#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
