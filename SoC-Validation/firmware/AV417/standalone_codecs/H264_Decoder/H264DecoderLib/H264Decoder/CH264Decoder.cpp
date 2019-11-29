/* ------------------------------------------------------------------------ */
/* FILE:			CH264Decoder.cpp										*/
/* DESCRIPTION:		BT H.264 video decoder source file.						*/
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

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
	#include <string.h>  // For memset
#endif

#include "H264Warnings.h"

//#include "picture_encoder.h"
#include "CH264Decoder.h"
#include "H264DecodedPictureBuffer.h"

#include "H264MotionVectorList.h"
#include "H264Tables.h"

#ifdef __cplusplus
extern "C" 
{
#endif
 
#include "vlc_api.h"

#ifdef __cplusplus
}
#endif 

/*
--------------------------------------------------------------------------------
Function	:	CH264Decoder::CH264Decoder.
Description	:	Constructor.
--------------------------------------------------------------------------------
*/

CH264Decoder::CH264Decoder(void)
{
//	InitialiseMemberVariables();

//	ByteBufferInitialise();
//	VLDInitialise();
}


// get pointer to macro-block buffer
#if defined __ARC_LINUX__
bool CH264Decoder::InitAurora(void *SDMBase)
#else
bool CH264Decoder::InitAurora(void)
#endif
{
	int bufferIndex;
    
    Simdu = new AuroraH264Dec();

#if defined __ARC_LINUX__
	//change the base address of the SDM to the mmap'd location given by Linux
	if (!Simdu->SetSDMStartAddr(SDMBase))
    {
	  fprintf(stderr,"\nFailed to set SDM Base address");
	  return false;
	}
#endif
    // Initialize SIMD memory for sub-pel filtering
    if( !Simdu->InitSIMDMemory())
    {
        printf("Error in allocating SIMD memory for sub-pel interpolation\n");
        return false;
    }
	// allocate SDM memory for all macroblocks
	if( !Simdu->SDM_Buffers.GetSDMBuffers())
	{
		fflush(stdout);
		printf("tried allocating %d bytes in SDM\n", SDM_ofs_size);
		printf("Error in Allocating Memory for SDM Macroblock Buffers\n");
		fflush(stdout);
		return false;
	}

	//clear all the macroblock buffers
	for(bufferIndex=0; bufferIndex<MAXIMUM_NUMBER_OF_MACROBUFFERS; bufferIndex++)
	{
		Simdu->SDMMemClear( SDM_BufCtrl::bufferPtr_ofs +  SDM_ofs_size * bufferIndex);
	}
    
    
    // Get prev buffer (Just so we can set if free later)
    m_prev_sdm_buf_ofs  = Simdu->SDM_Buffers.GetNextSDMBuffer();
	// get new macroblock buffer
	m_sdm_buf_ofs = Simdu->SDM_Buffers.GetNextSDMBuffer();
    if(m_sdm_buf_ofs== -1)
    {
        printf("GetNextSDMBuffer returned %d\n", m_sdm_buf_ofs);
        return false;
    }

	m_y_data = m_sdm_buf_ofs;
	m_u_data = m_y_data + SDM_ofs_u_data;
	m_v_data = m_y_data + SDM_ofs_v_data;
	

    
//    // set previous macro-block buffer index to "-1"
//    m_prev_sdm_buf_index = -1;
    
//	InitSimdTables();
	
	return true;
}


/*
--------------------------------------------------------------------------------
Function	:	CH264Decoder::~CH264Decoder.
Description	:	Constructor.
--------------------------------------------------------------------------------
*/

CH264Decoder::~CH264Decoder(void)
{
	Destroy();
}

/*
--------------------------------------------------------------------------------
Function	:	CH264Decoder::InitialiseMemberVariables.
Description	:	initialises all member variables.
--------------------------------------------------------------------------------
*/

void CH264Decoder::InitialiseMemberVariables(void)
{
	ClearReturnParameters();

	m_DecodingHasStarted = false;
	m_IncompatibleBitstream = false;


	m_OutputPictureNumber = 0;
	m_RemainingBumpingDelay = 0;

	m_VLDCurrentPicture = 0;

	m_POCAccumulator = 0;

	m_VLDState = H264_VLD_LOST;

	m_BumpingDecoder = true;


    m_PacketLost = false;

	// No decoding of coded video data can be performed until parameter sets are valid
	m_SequenceParameterSet = 0;
	m_PictureParameterSet = 0;



	m_simd_error = 0;

}

/*
--------------------------------------------------------------------------------
Function	:	CH264Decoder::Create.
Description	:	Creates the video decoder.
--------------------------------------------------------------------------------
*/

bool CH264Decoder::Create(void)
{
	m_FrameNumber = UINT_MAX;

	InitialiseMemberVariables();


	/* store this handle */
//	m_ThisHandle = handle;

	if (!VLDInitialise())
	{
		Destroy();
		return false;
	}

	m_DecodedPictureBuffer = new CH264DecodedPictureBuffer();
	if (0 == m_DecodedPictureBuffer)
	{
		Destroy();
		return false;
	}

	m_ParameterSets = new CH264ParameterSets();
	if (0 == m_ParameterSets)
	{
		Destroy();
		return false;
	}

	// Set the parameter set IDs to invalid values, as these will be set in the slice header.
	// Changes to the parameter set ID need to be identified.
//	m_SequenceParameterSetID = 255;
	m_PictureParameterSetID = 255;


	return true;
}

/*
--------------------------------------------------------------------------------
Function	:	CH264Decoder::Destroy.
Description	:	Destroys the video decoder.
--------------------------------------------------------------------------------
*/

bool CH264Decoder::Destroy(void)
{

//	ByteBufferDestroy();

	if (0 != m_DecodedPictureBuffer)
	{
		delete m_DecodedPictureBuffer;
		m_DecodedPictureBuffer = 0;
	}

	if (0 != m_ParameterSets)
	{
		delete m_ParameterSets;
		m_ParameterSets = 0;
	}


	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::Decode										*/
/* DESCRIPTION:	This decodes H.264 compressed data. The data must be from 
				one NAL unit, but does not need to be a whole NAL Unit.
				When used to decode the Annex B byte stream format, the 
				start codes must be removed before calling this function.
				The function returns when all of the bytes have been 
				processed, or, for example in the case of a parsing error, 
				when no more decoding can be done.
				buffer : pointer to the H.264 data
				buffer_length : the number of H.264 bytes.
				data_loss : set to true to indicate loss of compressed data 
				  since the last time this function was called. Could be set 
				  by checking RTP sequence number. Could be used by decoder 
				  to invoke error concealment.
				start_of_NAL_unit : must be set to true if the data 
				  represents the first data of a NAL unit, false otherwise.
				discard_if_disposable : when set to true the data is not 
				  decoded, but instead discarded, if the current NAL unit 
				  is disposable, i.e. NAL_ref_idc is zero.
				timestamp is the RTP timestamp associated with the data. 
				  It is stored and returned in the decoded output picture. 
				  Note that due to reordering of pictures, the picture 
				  retuned may not be the one decoded from this data.
				decoder_parameters : structure providing information about 
				  the current decoding status.								*/
/* RETURNS:		int: H264_DECODER_OK or H264_DECODER_INCOMPATIBLE_BITSTREAM.*/
/* ------------------------------------------------------------------------ */


int CH264Decoder::Decode(CODED_BYTE* buffer, int buffer_length, bool data_loss, 
						  bool start_of_NAL_unit, bool end_of_NAL_unit, bool discard_if_disposable,
						  unsigned int time_stamp, h264_decoder_parameters** decoder_parameters)
{
	bool error = false;

	int byteCode;
	
    // Need to retain true state regardless of current flag
    // Flag is reset after decoding slice data, but is retained in other cases, such as parameter sets
	m_PacketLost |= data_loss;

    ClearReturnParameters();

	//	set address of the decoding parameters being returned to the calling function
	*decoder_parameters = &m_H264DecoderParameters;

	if (buffer_length <= 0)
	{
		return H264_DECODER_OK;
	}

	// 'Load' the new data
	ByteBufferSetPointers(buffer, buffer_length, start_of_NAL_unit, byteCode);


	if (start_of_NAL_unit)
	{
		// Set state
		switch (m_NALUnitType)
		{
			case H264_SEQUENCE_PARAMETER_SET :
				m_VLDState = H264_VLD_PROFILE_IDC;
				break;

			case H264_PICTURE_PARAMETER_SET :
				m_VLDState = H264_VLD_PICTURE_PARAMETER_SET_ID;
				break;

			case H264_CODED_SLICE_NO_DP :
			case H264_CODED_SLICE_IDR :
				m_VLDState = H264_VLD_FIRST_MB_IN_SLICE;
				break;

			case H264_SEI_NAL_UNIT_TYPE :
				m_VLDState = H264_VLD_SEI_PAYLOAD_TYPE;
				break;

			case H264_PICTURE_DELIMITER :
				m_VLDState = H264_VLD_END_OF_NAL_UNIT;
				break;

			case H264_END_OF_SEQUENCE :
				m_VLDState = H264_VLD_END_OF_NAL_UNIT;
				break;

			case H264_END_OF_STREAM :
				m_VLDState = H264_VLD_END_OF_NAL_UNIT;
				break;

			case H264_FILLER_DATA :
				m_VLDState = H264_VLD_END_OF_NAL_UNIT;
				break;

			default :  // This is data partitioning and the reserved NAL unit types
				m_VLDState = H264_VLD_LOST;
				break;
		}
	
	}

	if ((0 == m_NALReferenceIDC) && discard_if_disposable)
	{
		// Disposable - so dispose!
		return H264_DECODER_OK;
	}

	if (H264_VLD_LOST == m_VLDState)
	{
		// There is no means to resync within a NAL Unit
		ByteBufferFlush(byteCode);
		m_H264DecoderParameters.bVideoSync = 0;
		return H264_DECODER_OK;
	}

	m_CurrentTimestamp = time_stamp;

	switch (m_NALUnitType)
	{
		case H264_SEQUENCE_PARAMETER_SET :
			if (m_VLDState < H264_VLD_VUI_ASPECT_RATIO_INFO_PRESENT)
			{
				error = !VLDDecodeSequenceParameterSet(byteCode);
			}
			if (error)
			{
				break;
			}

			if ((m_VLDState >= H264_VLD_VUI_ASPECT_RATIO_INFO_PRESENT) && (!m_IncompatibleBitstream))
			{
				error = !VLDDecodeVUI();
			}
			if (error)
			{
				break;
			}

			m_H264DecoderParameters.pSequenceParameters = (void*)(m_SequenceParameterSet);

			break;

		case H264_PICTURE_PARAMETER_SET :
			if (!m_IncompatibleBitstream)
			{
				error = !VLDDecodePictureParameterSet(byteCode);
			}
			break;

		case H264_CODED_SLICE_NO_DP :
		case H264_CODED_SLICE_IDR :
			if ((0 == m_SequenceParameterSet) || (0 == m_PictureParameterSet))
			{
				// Can not decode data yet
				break;
			}

			if ((m_VLDState < H264_VLD_RUN) && (!m_IncompatibleBitstream))
			{
				error = !VLDDecodeSliceHeader();
			}
			if (error)
			{
				break;
			}

			if ((m_VLDState >= H264_VLD_RUN) && !m_IncompatibleBitstream)
			{
                DecodeSliceData(byteCode, m_VLDState);

				if(m_simd_error != H264_DECODER_OK)
				{
					return m_simd_error;
				}
			}
			break;

		case H264_SEI_NAL_UNIT_TYPE :
			if (!m_IncompatibleBitstream)
			{
				error = !VLDDecodeSEIMessage(byteCode);
			}
			break;

		case H264_PICTURE_DELIMITER :
			ByteBufferFlush(byteCode);
			break;

		case H264_END_OF_SEQUENCE :
			ByteBufferFlush(byteCode);
			break;

		case H264_END_OF_STREAM :
			{
				CBTPicture * decoded_picture;

				do
				{
					m_DecodedPictureBuffer->OutputFrame(&decoded_picture);

					if (0 != decoded_picture)
					{
						decoded_picture->m_fn = m_OutputPictureNumber;
						m_OutputPictureNumber++;

						if (m_SequenceParameterSet->frame_cropping_flag)
						{
							decoded_picture->SetCroppingInformation(
								m_SequenceParameterSet->frame_cropping_rect_left_offset,
								m_SequenceParameterSet->frame_cropping_rect_right_offset,
								m_SequenceParameterSet->frame_cropping_rect_top_offset,
								m_SequenceParameterSet->frame_cropping_rect_bottom_offset);
/*							m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].crop(decoded_picture,
								m_SequenceParameterSet->frame_cropping_rect_left_offset,
								m_SequenceParameterSet->frame_cropping_rect_right_offset,
								m_SequenceParameterSet->frame_cropping_rect_top_offset,
								m_SequenceParameterSet->frame_cropping_rect_bottom_offset);

							m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].m_ts = decoded_picture->m_ts;
							m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].m_fn = decoded_picture->m_fn;

							SetReturnParameters(&m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame]);
*/
							SetReturnParameters(decoded_picture);
						}
						else
						{
							SetReturnParameters(decoded_picture);
						}
					}

				} while (0 != decoded_picture);

				ByteBufferFlush(byteCode);

				break;
			}

		case H264_FILLER_DATA :
			ByteBufferFlush(byteCode);
			break;

		default :  // This is data partitioning and the reserved NAL unit types
			ByteBufferFlush(byteCode);
			break;
	}

    if ((H264_CODED_SLICE_NO_DP == m_NALUnitType) || (H264_CODED_SLICE_IDR == m_NALUnitType))
    {
      m_PacketLost = false; // Retain state in all other cases
    }

	if (error)
	{
		m_VLDState = H264_VLD_LOST;
		m_H264DecoderParameters.bVideoSync = 0;
	}

	if (m_IncompatibleBitstream)
	{
		return H264_DECODER_INCOMPATIBLE_BITSTREAM;
	}
	else
	{
		return H264_DECODER_OK;
	}
}

/*
--------------------------------------------------------------------------------
Function SetReturnParameters.
--------------------------------------------------------------------------------
*/

void CH264Decoder::SetReturnParameters(CBTPicture * decoded_picture)
{
	m_H264DecoderParameters.pPicture[m_H264DecoderParameters.bDisplayFrame] = (void *)(decoded_picture);

	m_H264DecoderParameters.ErrorNumber = H264_DECODER_OK;
	m_H264DecoderParameters.HorSize = m_SequenceParameterSet->frame_width_in_mbs << 4;
	m_H264DecoderParameters.VerSize = m_SequenceParameterSet->frame_height_in_mbs << 4;

	m_H264DecoderParameters.bVideoSync = 1;

	m_H264DecoderParameters.bDisplayFrame++;

	m_H264DecoderParameters.bDiscardedData = 0;
	m_H264DecoderParameters.Spare = 0;

	m_H264DecoderParameters.pSequenceParameters = (void*)(m_SequenceParameterSet);
}

/*
--------------------------------------------------------------------------------
Function ClearReturnParameters.
--------------------------------------------------------------------------------
*/

void CH264Decoder::ClearReturnParameters(void)
{
	memset(&m_H264DecoderParameters, 0, sizeof(h264_decoder_parameters));

	m_H264DecoderParameters.ErrorNumber = H264_DECODER_OK;
	m_H264DecoderParameters.bVideoSync = 1;

	m_H264DecoderParameters.pSequenceParameters = 0;//(void*)(m_SequenceParameterSet);
}

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
