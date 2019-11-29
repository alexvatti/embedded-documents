/* ------------------------------------------------------------------------ */
/* FILE:			H264VLDSei.cpp											*/
/* DESCRIPTION:		H.264 SEI variable length decoder class					*/
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

#include "H264Warnings.h"

#include "CH264Decoder.h"
#include "H264EnumTypes.h"
#include "H264Tables.h"
#include "H264SEIParameters.h"

#include "vlc_api.h"
#include "cavlc_api.h"

#ifdef __ARC_MPLAYER__
extern unsigned long g_NALsize;
extern unsigned long g_NALLenDec;
#endif

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_pic_struct_NumClockTS_table					*/
/* DESCRIPTION:	The table for mapping pic_struct to NumClockTS.				*/
/* ------------------------------------------------------------------------ */

const unsigned char h264_decoder_pic_struct_NumClockTS_table[16] =
	{  1,  1, 1, 2, 2, 3, 3, 2, 3, 0, 0, 0, 0, 0, 0, 0 };


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeSEIMessage							*/
/* DESCRIPTION:	Decodes an SEI message.										*/
/* RETURNS:		True if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeSEIMessage(int &byteCode)
{
	bool error = false;
	bool no_bits_left = false;
	bool finished_parsing = false;
	sequence_parameter_set_t * sequence_parameter_set = m_SequenceParameterSet;

#ifdef __ARC_MPLAYER__
	if (g_NALLenDec) 
	{
		m_SEIPayloadSize = g_NALsize;
		m_VLDState = H264_VLD_SEI_PAYLOAD_DATA;
	}
#endif

	//	Loop while bits in buffer and not decoded an SEI message.
	while (true)
	{
		///////////////////////////////////////////////////////////////////////
		// State machine
		//
		// Try to decode a variable length code
		//
		// If there are not enough bits in the bit buffer, no_bits_left is 
		// set to true, and the function returns.
		///////////////////////////////////////////////////////////////////////

		switch (m_VLDState)
		{
			// --------------------------------------------------------------
			case H264_VLD_SEI_PAYLOAD_TYPE :
#ifdef __ARC_MPLAYER__
				if (g_NALLenDec) finished_parsing = true;
#endif

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
#endif
				if (128 == byteCode)
				{
					// This is the RBSP trailing byte
					finished_parsing = true;
				}
				else
				{
					m_SEIPayloadType = byteCode;
					m_VLDState = H264_VLD_SEI_PAYLOAD_SIZE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEI_PAYLOAD_SIZE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SEIPayloadSize = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
				m_SEIPayloadSize = byteCode;
#endif
				m_VLDState = H264_VLD_SEI_PAYLOAD_DATA;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEI_PAYLOAD_DATA :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
#endif
				m_SEIPayloadSize--;

				if (0 == m_SEIPayloadSize)
				{
					// Finished this SEI message but there may be more in this NAL unit
					m_VLDState = H264_VLD_SEI_PAYLOAD_TYPE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEI_END_OF_SEI :
				finished_parsing = true;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEI_BYTE_ALIGNMENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
#endif
				assert(1 == byteCode);

				ByteBufferByteAlign();

				// Finished this SEI message but there may be more in this NAL unit
				m_VLDState = H264_VLD_SEI_PAYLOAD_TYPE;
				break;

			// --------------------------------------------------------------
			default :
				error = true;
				break;
		}

		if (finished_parsing)
		{
			m_VLDState = H264_VLD_SEI_END_OF_SEI;
		}

		if (error || no_bits_left || finished_parsing)
		{
			break;  // Break the while (true) loop
		}
	}

	return !error;
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
