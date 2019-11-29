/* ------------------------------------------------------------------------ */
/* FILE:			H264VLDHeaders.cpp										*/
/* DESCRIPTION:		H.264 header variable length decoder class				*/
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

#include "CH264Decoder.h"
#include "H264DecoderControl.h"
#include "H264DecodedPictureBuffer.h"
#include "H264Tables.h"
#include "H264DecoderTables.h"
#include "H264OffsetTables.h"

#include "cavlc_api.h"
#include "vlc_api.h"

#if defined _WIN32
	#include "StdAfx.h"
	#include "Aurora.h"
#else
	#include <stdio.h>
#endif	//_WIN32

	#include "ARCMedia.h"
	#include "AuroraH264Dec.h"



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeSequenceParameterSet					*/
/* DESCRIPTION:	Decodes a sequence parameter set.							*/
/* RETURNS:		True if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeSequenceParameterSet(int &byteCode)
{
//	bool error = false;
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	bool no_bits_left = false;
#endif
//	bool finished_parsing = false;

	m_IncompatibleBitstream = false;

	//	Loop while bits in buffer and not decoded a sequence parameter set.
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
			case H264_VLD_PROFILE_IDC :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_VLDSeqParamSetProfileLevel = VLC_Get_Bits(8) << 16;
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
				m_VLDSeqParamSetProfileLevel = byteCode << 16;
#endif
				m_VLDState = H264_VLD_LEVEL_AND_FLAGS;
				break;

			// --------------------------------------------------------------
			case H264_VLD_LEVEL_AND_FLAGS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_VLDSeqParamSetProfileLevel |= VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_VLDSeqParamSetProfileLevel |= byteCode;
#endif
				m_VLDState = H264_VLD_SEQ_PARAM_SET_ID;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEQ_PARAM_SET_ID :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE);
				m_SequenceParameterSet = m_ParameterSets->WriteSequenceParameterSet(CAVLC_Read_Symbol());
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet = m_ParameterSets->WriteSequenceParameterSet(byteCode);
#endif
//				if (0 == m_SequenceParameterSet)  {	error = true; break; }
				if (0 == m_SequenceParameterSet)  {	return false; }

				m_SequenceParameterSet->profile_idc = (m_VLDSeqParamSetProfileLevel >> 16) & 0xff;
				m_SequenceParameterSet->level_idc = m_VLDSeqParamSetProfileLevel & 0xff;

				// Map level_idc to level index
				m_SequenceParameterSet->level_index = 0;
				while (m_SequenceParameterSet->level_idc != 
							annex_a_level_table[m_SequenceParameterSet->level_index][0])
				{
					m_SequenceParameterSet->level_index++;

					if (m_SequenceParameterSet->level_index >= NUMBER_OF_ANNEX_A_LEVELS) break;
				}

				m_SequenceParameterSet->baseline_compatibility_flag =
					(H264_BASELINE_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(0x8000 == (m_VLDSeqParamSetProfileLevel & 0x8000));
#if defined __ARC_MAINPROFILE__
				m_SequenceParameterSet->main_compatibility_flag =
					(H264_MAIN_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(0x4000 == (m_VLDSeqParamSetProfileLevel & 0x4000));

				m_SequenceParameterSet->extended_compatibility_flag =
					(H264_EXTENDED_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(0x2000 == (m_VLDSeqParamSetProfileLevel & 0x2000));
#endif
				// Check that the bitstream is compatible with the build of software
				if (m_SequenceParameterSet->baseline_compatibility_flag)
				{
					// Can always decode in this case
				}
#if defined __ARC_MAINPROFILE__
				else if (m_SequenceParameterSet->main_compatibility_flag)
				{
					if (!m_SequenceParameterSet->extended_compatibility_flag)
					{
						// Main and not Extended, Need CABAC, and Interlace which is tested later
#ifndef __CABAC__
						m_IncompatibleBitstream = true;
#endif
					}
					// else Main and Extended, only depends on Interlace which is tested later
				}
				else
				{
					// Not Main
					if (m_SequenceParameterSet->extended_compatibility_flag)
					{
						// Extended and not Main, Need SP, and Interlace which is tested later
						m_IncompatibleBitstream = true;
					}
					else
					{
						// Not Main and Not Extended
						// FRExt bitstream. Can only decode if H264_HIGH_PROFILE profile and code included
						m_IncompatibleBitstream = true;
					}
				}
#else
                else
                {
                  m_IncompatibleBitstream = true;
                }
#endif
				if (m_IncompatibleBitstream)
				{
//					error = true;
//					break;
                    return false;
				}
#if defined __ARC_MAINPROFILE__
				if ((H264_HIGH_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(H264_HIGH_10_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(H264_HIGH_422_PROFILE == m_SequenceParameterSet->profile_idc) ||
					(H264_HIGH_444_PROFILE == m_SequenceParameterSet->profile_idc))
				{
					m_VLDState = H264_VLD_CHROMA_FORMAT_IDC;
				}
				else
#endif
				{
					m_SequenceParameterSet->chroma_format_idc = 1;
					m_VLDState = H264_VLD_LOG2_MAX_FRAME_NUMBER;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_LOG2_MAX_FRAME_NUMBER :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_PICTURE_ORDER_COUNT_TYPE;
				m_SequenceParameterSet->log2_max_frame_num_minus4 = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->log2_max_frame_num_minus4 = byteCode;
				m_VLDState = H264_VLD_PICTURE_ORDER_COUNT_TYPE;
#endif
				m_SequenceParameterSet->maximum_frame_number = 16 << m_SequenceParameterSet->log2_max_frame_num_minus4;
				break;

			// --------------------------------------------------------------
			case H264_VLD_PICTURE_ORDER_COUNT_TYPE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_SequenceParameterSet->pic_order_cnt_type = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->pic_order_cnt_type = byteCode;
#endif
				if (0 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					m_VLDState = H264_VLD_LOG2_MAX_POC_LSB;
				}
				else if (1 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					m_VLDState = H264_VLD_DELTA_PIC_ORDER_ZERO_FLAG;
				}
				else
				{
					m_VLDState = H264_VLD_NUM_REF_FRAMES;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_LOG2_MAX_POC_LSB :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_NUM_REF_FRAMES;
				m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4 = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4 = byteCode;
				m_VLDState = H264_VLD_NUM_REF_FRAMES;
#endif
				m_SequenceParameterSet->max_pic_order_cnt_lsb
					= 16 << m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4;

				break;

			// --------------------------------------------------------------
			case H264_VLD_DELTA_PIC_ORDER_ZERO_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->delta_pic_order_always_zero_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->delta_pic_order_always_zero_flag = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_OFFSET_FOR_NON_REF_PIC;
				break;

			// --------------------------------------------------------------
			case H264_VLD_OFFSET_FOR_NON_REF_PIC :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_OFFSET_TOP_BOTTOM_FIELD;
				m_SequenceParameterSet->offset_for_non_ref_pic = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->offset_for_non_ref_pic = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
				m_VLDState = H264_VLD_OFFSET_TOP_BOTTOM_FIELD;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_OFFSET_TOP_BOTTOM_FIELD :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_NUM_REF_IN_POC_CYCLE;
				m_SequenceParameterSet->offset_for_top_to_bottom_field = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->offset_for_top_to_bottom_field = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
				m_VLDState = H264_VLD_NUM_REF_IN_POC_CYCLE;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_IN_POC_CYCLE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle = 0;
				m_VLDStateCount = 0; // Used to count offset_for_ref_frame
				m_VLDState = H264_VLD_OFFSET_FOR_REF_FRAME;
				m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle = byteCode;
				m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle = 0;
				m_VLDStateCount = 0; // Used to count offset_for_ref_frame
				m_VLDState = H264_VLD_OFFSET_FOR_REF_FRAME;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_OFFSET_FOR_REF_FRAME :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_SequenceParameterSet->offset_for_ref_frame[m_VLDStateCount] = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->offset_for_ref_frame[m_VLDStateCount] = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
#endif
				m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle
					+= m_SequenceParameterSet->offset_for_ref_frame[m_VLDStateCount];

				m_VLDStateCount++;

				if (m_VLDStateCount >= m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle)
				{
					m_VLDState = H264_VLD_NUM_REF_FRAMES;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_FRAMES :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_REQUIRED_FN_UPDATE_FLAG;
				m_SequenceParameterSet->num_ref_frames = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->num_ref_frames = byteCode;
				m_VLDState = H264_VLD_REQUIRED_FN_UPDATE_FLAG;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_REQUIRED_FN_UPDATE_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->required_frame_num_update_behaviour_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->required_frame_num_update_behaviour_flag = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_PICTURE_WIDTH;
				break;

			// --------------------------------------------------------------
			case H264_VLD_PICTURE_WIDTH :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_PICTURE_HEIGHT;
				m_SequenceParameterSet->frame_width_in_mbs = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_width_in_mbs = byteCode + 1;
				m_VLDState = H264_VLD_PICTURE_HEIGHT;
#endif
				break;
 
			// --------------------------------------------------------------
			case H264_VLD_PICTURE_HEIGHT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_MBS_ONLY_FLAG;
				m_SequenceParameterSet->picture_height_in_map_units = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->picture_height_in_map_units = byteCode + 1;
				m_VLDState = H264_VLD_FRAME_MBS_ONLY_FLAG;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_MBS_ONLY_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->frame_mbs_only_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_mbs_only_flag = (byteCode == 1);
#endif
				// Check that the bitstream is compatible with the build of software
				// It will be at this stage unless frame_mbs_only_flag is false and Interlace not included
#ifndef __INTERLACE__
				if (!m_SequenceParameterSet->frame_mbs_only_flag)
				{
					m_IncompatibleBitstream = true;
//					error = true;
//					break;
                    return false;
				}
#endif
				m_SequenceParameterSet->frame_height_in_mbs = 
					(2 - m_SequenceParameterSet->frame_mbs_only_flag) * 
						m_SequenceParameterSet->picture_height_in_map_units;

				if (!m_SequenceParameterSet->frame_mbs_only_flag)
				{
					m_VLDState = H264_VLD_MB_ADAPTIVE_FRAME_FIELD_FLAG;
				}
				else
				{
					m_SequenceParameterSet->mb_adaptive_frame_field_flag = 0;
					m_VLDState = H264_VLD_DIRECT_8X8_INFERENCE_FLAG;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_MB_ADAPTIVE_FRAME_FIELD_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->mb_adaptive_frame_field_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->mb_adaptive_frame_field_flag = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_DIRECT_8X8_INFERENCE_FLAG;
				break;

			// --------------------------------------------------------------
			case H264_VLD_DIRECT_8X8_INFERENCE_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->direct_8x8_inference_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->direct_8x8_inference_flag = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_FRAME_CROPPING;
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROPPING :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->frame_cropping_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_flag = (byteCode == 1);
#endif
				if (m_SequenceParameterSet->frame_cropping_flag)
				{
					m_VLDState = H264_VLD_FRAME_CROP_LEFT_OFFSET;
				}
				else
				{
					m_VLDState = H264_VLD_VUI_PARAMETERS_PRESENT_FLAG;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_LEFT_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_RIGHT_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_left_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_left_offset = byteCode;
				m_VLDState = H264_VLD_FRAME_CROP_RIGHT_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_RIGHT_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_TOP_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_right_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_right_offset = byteCode;
				m_VLDState = H264_VLD_FRAME_CROP_TOP_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_TOP_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_BOTTOM_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_top_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_top_offset = byteCode;
				m_VLDState = H264_VLD_FRAME_CROP_BOTTOM_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_BOTTOM_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_PARAMETERS_PRESENT_FLAG;
				m_SequenceParameterSet->frame_cropping_rect_bottom_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_bottom_offset = byteCode;
				m_VLDState = H264_VLD_VUI_PARAMETERS_PRESENT_FLAG;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_PARAMETERS_PRESENT_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_seq_parameters_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_seq_parameters_flag = (byteCode == 1);
#endif
				if (m_SequenceParameterSet->vui_seq_parameters_flag)
				{
                    // the following state may be used in VLDDecodeVUI() to to start the VUI decoding
					m_VLDState = H264_VLD_VUI_ASPECT_RATIO_INFO_PRESENT;
                    return true;
				}
				else
				{
//					finished_parsing = true;
                    // the following state may be used in VLDDecodeVUI() to skip the VUI decoding
                    m_VLDState = H264_VLD_END_OF_SPS;
                    return true;
				}

				break;

			// --------------------------------------------------------------
			case H264_VLD_END_OF_SPS :
//				finished_parsing = true;
//				break;
                return true;

			// --------------------------------------------------------------
			default :
//				error = true;
//				break;
                return false;
		}
/*
		if (finished_parsing)
		{
			m_VLDState = H264_VLD_END_OF_SPS;
		}
*/
//		if (error || no_bits_left || finished_parsing || (H264_VLD_VUI_ASPECT_RATIO_INFO_PRESENT == m_VLDState))
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
		if (no_bits_left)
		{
			break;  // Break the while (true) loop
		}
#endif
	}

//	return !error;
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodePictureParameterSet					*/
/* DESCRIPTION:	Decodes a picture parameter set.							*/
/* RETURNS:		True if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodePictureParameterSet(int &byteCode)
{
//	bool error = false;
	bool no_bits_left = false;
//	bool finished_parsing = false;

	//	Loop while bits in buffer and not decoded a picture parameter set.
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
			case H264_VLD_PICTURE_PARAMETER_SET_ID :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_SEQUENCE_PARAMETER_SET_ID;
				m_PictureParameterSet = m_ParameterSets->WritePictureParameterSet(CAVLC_Read_Symbol());
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet = m_ParameterSets->WritePictureParameterSet(byteCode);
				m_VLDState = H264_VLD_SEQUENCE_PARAMETER_SET_ID;
#endif
				if (0 == m_PictureParameterSet) { no_bits_left = true; break; }
				break;

			// --------------------------------------------------------------
			case H264_VLD_SEQUENCE_PARAMETER_SET_ID :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_ENTROPY_CODING_MODE_FLAG;
				m_PictureParameterSet->sequence_parameter_set_id = CAVLC_Read_Symbol();
				m_SequenceParameterSet = m_ParameterSets->ReadSequenceParameterSet(m_PictureParameterSet->sequence_parameter_set_id);
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->sequence_parameter_set_id = byteCode;
				m_SequenceParameterSet = m_ParameterSets->ReadSequenceParameterSet(byteCode);
				m_VLDState = H264_VLD_ENTROPY_CODING_MODE_FLAG;
#endif
//				if (0 == m_SequenceParameterSet) { error = true; break; }
				if (0 == m_SequenceParameterSet) { return false; }
				break;

			// --------------------------------------------------------------
			case H264_VLD_ENTROPY_CODING_MODE_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_PictureParameterSet->CABAC_entropy_coding_mode = VLC_Get_Bit();
                if (m_PictureParameterSet->CABAC_entropy_coding_mode == 0)
                {
                    VLC_Sel_Dec(VLC_DEC_CAVLC);
                }
                else
                {
//                    VLC_Sel_Dec(VLC_DEC_CABAC); // to be implemented
                }
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->CABAC_entropy_coding_mode = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_PIC_ORDER_PRESENT_FLAG;
				break;

			// --------------------------------------------------------------
			case H264_VLD_PIC_ORDER_PRESENT_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_PictureParameterSet->pic_order_present_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->pic_order_present_flag = (byteCode == 1);
#endif
				m_VLDState = H264_VLD_NUM_SLICE_GROUPS;
				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_SLICE_GROUPS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
                m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				m_PictureParameterSet->num_slice_groups = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->num_slice_groups = byteCode + 1;
                m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
#endif
//				if (m_PictureParameterSet->num_slice_groups > MAXIMUM_NUMBER_OF_SLICE_GROUPS) {	error = true; break; }
				if (m_PictureParameterSet->num_slice_groups > MAXIMUM_NUMBER_OF_SLICE_GROUPS) {	return false; }

				if (m_PictureParameterSet->num_slice_groups > 1)
				{
					m_VLDState = H264_VLD_MB_SLICE_GROUP_MAP_TYPE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_MB_SLICE_GROUP_MAP_TYPE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				m_PictureParameterSet->slice_group_map_type = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_map_type = byteCode;
				m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
#endif
				if (0 == m_PictureParameterSet->slice_group_map_type)
				{
					m_VLDStateCount = 0;  // Counts slice groups

					m_VLDState = H264_VLD_SGT0_RUN_LENGTH;
				}
				else if (2 == m_PictureParameterSet->slice_group_map_type)
				{
					m_VLDStateCount = 0;  // Counts slice groups

					m_VLDState = H264_VLD_SGT2_TOP_LEFT;
				}
				else if ((3 == m_PictureParameterSet->slice_group_map_type) ||
						 (4 == m_PictureParameterSet->slice_group_map_type) ||
						 (5 == m_PictureParameterSet->slice_group_map_type))
				{
					m_VLDState = H264_VLD_SG_CHANGE_DIRECTION;
				}
				else if (6 == m_PictureParameterSet->slice_group_map_type)
				{
					m_VLDState = H264_VLD_SGT6_PIC_SIZE_IN_MAP_UNITS;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SGT0_RUN_LENGTH :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_PictureParameterSet->slice_group_run_length[m_VLDStateCount] = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_run_length[m_VLDStateCount] = byteCode + 1;
#endif
				m_VLDStateCount++;

				if (m_VLDStateCount == m_PictureParameterSet->num_slice_groups)
				{
					m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SGT2_TOP_LEFT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_SGT2_BOTTOM_RIGHT;
				m_PictureParameterSet->slice_group_top_left[m_VLDStateCount] = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_top_left[m_VLDStateCount] = byteCode;
				m_VLDState = H264_VLD_SGT2_BOTTOM_RIGHT;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_SGT2_BOTTOM_RIGHT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
                m_VLDState = H264_VLD_SGT2_TOP_LEFT;
				m_PictureParameterSet->slice_group_bottom_right[m_VLDStateCount] = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_bottom_right[m_VLDStateCount] = byteCode;
                m_VLDState = H264_VLD_SGT2_TOP_LEFT;
#endif
				m_VLDStateCount++;

				if (m_VLDStateCount == m_PictureParameterSet->num_slice_groups - 1)
				{
					m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SG_CHANGE_DIRECTION :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_PictureParameterSet->slice_group_change_direction_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_change_direction_flag = byteCode;
#endif
				m_VLDState = H264_VLD_SG_CHANGE_RATE;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SG_CHANGE_RATE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				m_PictureParameterSet->slice_group_change_rate = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_change_rate = byteCode + 1;
				m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_SGT6_PIC_SIZE_IN_MAP_UNITS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDStateCount = 0;  // count slice_group_id
				m_VLDState = H264_VLD_SGT6_SLICE_GROUP_ID;
				m_PictureParameterSet->slice_group_pic_size_in_map_units = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_pic_size_in_map_units = byteCode + 1;
				m_VLDStateCount = 0;  // count slice_group_id
				m_VLDState = H264_VLD_SGT6_SLICE_GROUP_ID;
#endif
				if (2 == m_PictureParameterSet->num_slice_groups)
				{
					m_SliceGroupIDCodeLength = 1;
				}
				else if (m_PictureParameterSet->num_slice_groups <= 4)
				{
					m_SliceGroupIDCodeLength = 2;
				}
				else
				{
					m_SliceGroupIDCodeLength = 3;
				}

				m_PictureParameterSet->slice_group_id =
					new UNSIGNED_SEPTET[m_PictureParameterSet->slice_group_pic_size_in_map_units];

				break;

			// --------------------------------------------------------------
			case H264_VLD_SGT6_SLICE_GROUP_ID :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                // m_SliceGroupIDCodeLength is 1, 2 or 3
				m_PictureParameterSet->slice_group_id[m_VLDStateCount] = (UNSIGNED_SEPTET) VLC_Get_Bits(m_SliceGroupIDCodeLength);
#else
				if (!ByteBufferDecodeFixedLengthCode(m_SliceGroupIDCodeLength, byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->slice_group_id[m_VLDStateCount] = (UNSIGNED_SEPTET) byteCode;
#endif
				m_VLDStateCount++;

				if (m_VLDStateCount == m_PictureParameterSet->slice_group_pic_size_in_map_units)
				{
					m_VLDState = H264_VLD_NUM_REF_IDX_L0_ACTIVE;
				}

				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_IDX_L0_ACTIVE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_NUM_REF_IDX_L1_ACTIVE;
				m_PictureParameterSet->num_ref_idx_l0_active = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->num_ref_idx_l0_active = byteCode + 1;
				m_VLDState = H264_VLD_NUM_REF_IDX_L1_ACTIVE;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_IDX_L1_ACTIVE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_WEIGHTED_PRED_AND_BIPRED;
				m_PictureParameterSet->num_ref_idx_l1_active = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->num_ref_idx_l1_active = byteCode + 1;
				m_VLDState = H264_VLD_WEIGHTED_PRED_AND_BIPRED;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_WEIGHTED_PRED_AND_BIPRED :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(3);
#else
				if (!ByteBufferDecodeFixedLengthCode(3, byteCode)) { no_bits_left = true; break; }
#endif
				m_PictureParameterSet->weighted_pred_flag = ((byteCode & 4) == 4);

				m_PictureParameterSet->weighted_bipred_idc = (byteCode & 0x3);

				assert(!m_PictureParameterSet->weighted_pred_flag);

				m_VLDState = H264_VLD_PIC_QP;
				break;

			// --------------------------------------------------------------
			case H264_VLD_PIC_QP :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_PIC_QS;
				m_PictureParameterSet->pic_init_qp = CAVLC_Read_Symbol() + 26;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->pic_init_qp = VLDDecodeMapCodeNumberToSignedGolomb(byteCode) + 26;
				m_VLDState = H264_VLD_PIC_QS;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_PIC_QS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_CHROMA_QP_OFFSET;
				m_PictureParameterSet->pic_init_qs = CAVLC_Read_Symbol() + 26;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->pic_init_qs = VLDDecodeMapCodeNumberToSignedGolomb(byteCode) + 26;
				m_VLDState = H264_VLD_CHROMA_QP_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_CHROMA_QP_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_PPS_THREE_SINGLE_BIT_FLAGS;
				m_PictureParameterSet->chroma_qp_index_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->chroma_qp_index_offset = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
				m_VLDState = H264_VLD_PPS_THREE_SINGLE_BIT_FLAGS;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_PPS_THREE_SINGLE_BIT_FLAGS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(3);
#else
				if (!ByteBufferDecodeFixedLengthCode(3, byteCode)) { no_bits_left = true; break; }
#endif
				m_PictureParameterSet->deblocking_filter_parameters_present_flag = ((byteCode & 4) == 4);

				m_PictureParameterSet->constrained_intra_pred_flag = ((byteCode & 2) == 2);

				m_PictureParameterSet->redundant_pic_cnt_present_flag = ((byteCode & 1) == 1);				

				{
					m_PictureParameterSet->transform_8x8_modeflag = false;
//					finished_parsing = true;
                    m_VLDState = H264_VLD_END_OF_PPS;
                    return true;
				}

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_LEFT_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_RIGHT_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_left_offset = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_left_offset = byteCode + 1;
				m_VLDState = H264_VLD_FRAME_CROP_RIGHT_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_RIGHT_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_TOP_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_right_offset = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_right_offset = byteCode + 1;
				m_VLDState = H264_VLD_FRAME_CROP_TOP_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_TOP_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_CROP_BOTTOM_OFFSET;
				m_SequenceParameterSet->frame_cropping_rect_top_offset = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_top_offset = byteCode + 1;
				m_VLDState = H264_VLD_FRAME_CROP_BOTTOM_OFFSET;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_FRAME_CROP_BOTTOM_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
//				finished_parsing = true;
                m_VLDState = H264_VLD_END_OF_PPS;
				m_SequenceParameterSet->frame_cropping_rect_bottom_offset = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->frame_cropping_rect_bottom_offset = byteCode + 1;
//				finished_parsing = true;
                m_VLDState = H264_VLD_END_OF_PPS;
#endif
//				break;
                return true;

			// --------------------------------------------------------------
			case H264_TRANSFORM_8X8_MODE_FLAG:
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_PictureParameterSet->transform_8x8_modeflag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->transform_8x8_modeflag = (byteCode == 1);
#endif
				m_VLDState = H264_PIC_SCALING_MATRIX_PRESENT_FLAG;
				break;

			// --------------------------------------------------------------
			case H264_PIC_SCALING_MATRIX_PRESENT_FLAG:

				assert(false);
				break;

			// --------------------------------------------------------------
			case H264_PIC_SCALING_MATRIX_LIST:
				assert(false);
				break;

				// --------------------------------------------------------------
			case H264_PIC_SCALING_LIST:
				assert(false);
				break;
				
			// --------------------------------------------------------------
			case H264_SECOND_CHROMA_QP_INDEX_OFFSET:
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
//				finished_parsing = true;
                m_VLDState = H264_VLD_END_OF_PPS;
				m_PictureParameterSet->second_chroma_qp_index_offset = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_PictureParameterSet->second_chroma_qp_index_offset = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
//				finished_parsing = true;
                m_VLDState = H264_VLD_END_OF_PPS;
#endif
                return true;
//				break;

			// --------------------------------------------------------------
			case H264_VLD_END_OF_PPS :
//				finished_parsing = true;
//				break;
                return true;

			default :
                return false;
//				error = true;
//				break;
		}
/*
		if (finished_parsing)
		{
			m_VLDState = H264_VLD_END_OF_PPS;
		}
*/
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
//		if (error || no_bits_left || finished_parsing)
		if (no_bits_left)
		{
			break;  // Break the while (true) loop
		}
#endif
	}

//	return !error;
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeSliceHeader							*/
/* DESCRIPTION:	Decodes a slice header.										*/
/* RETURNS:		True if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeSliceHeader(void)
{
//	bool error = false;
	bool no_bits_left = false;
	int byteCode;

	// Initialise m_PreviousMB to 0

#if defined __ARC_MAINPROFILE__
	m_PreviousMB = 0;
#endif

#ifdef __INTERLACE__
    if (m_SequenceParameterSet != NULL)
      m_IsAdaptiveFrame = m_SequenceParameterSet->mb_adaptive_frame_field_flag && !m_FieldPictureFlag;
    else
      m_IsAdaptiveFrame = false;
#endif

	//	Loop while bits in buffer and not decoded a slice header.
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
			case H264_VLD_FIRST_MB_IN_SLICE :
				m_VLDSliceHeaderFirstSliceOfPicture = false;
				m_VLDSliceHeaderPreviousSlicePictureOrderCount = m_PictureOrderCount;
				m_VLDSliceHeaderPreviousSliceFrameNumber = m_FrameNumber;

				m_VLDSliceHeaderPreviousPicOrderCntLsb = m_VLDSliceHeaderPicOrderCntLsb;
				m_VLDSliceHeaderPreviousDeltaPicOrderCountBottom = m_VLDSliceHeaderDeltaPicOrderCountBottom;
				m_VLDSliceHeaderPreviousDeltaPicOrderCount[0] = m_VLDSliceHeaderDeltaPicOrderCount[0];
				m_VLDSliceHeaderPreviousDeltaPicOrderCount[1] = m_VLDSliceHeaderDeltaPicOrderCount[1];

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
#endif
				if (!m_VLDSliceHeaderFirstSlice && (0 != m_NumberOfMacroblocksDecoded))
				{
					if ((m_VLDSliceHeaderPreviousSliceNALReferenceIDC != m_NALReferenceIDC) && ((0 == m_VLDSliceHeaderPreviousSliceNALReferenceIDC) || (0 == m_NALReferenceIDC)) ||
						((H264_CODED_SLICE_IDR == m_NALUnitType) && !m_VLDSliceHeaderPreviousSliceWasIDR))
					{
						// The previous picture was not completely decoded: perform error concealment and output the picture
						DecoderErrorConcealment();
					}
				}

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
#endif
				
				
					// Non MB-AFF Frame or field
				m_VLDSliceHeaderFirstMBInSlice = byteCode;


				m_VLDState = H264_VLD_SLICE_TYPE;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SLICE_TYPE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_PICTURE_PARAMETER_SET;
				m_SliceType = (h264_slice_type_t)(CAVLC_Read_Symbol());
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SliceType = (h264_slice_type_t)(byteCode);
				m_VLDState = H264_VLD_PICTURE_PARAMETER_SET;
#endif
				if ((m_SliceType >= H264_ALL_P_SLICE) && (m_SliceType <= H264_ALL_SI_SLICE))
				{
					// Map the _ALL slice types to the usual slice types
					// This simplifies logic elsewhere in the decoder
					m_SliceType = (h264_slice_type_t)((int)(m_SliceType) - 5);
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_PICTURE_PARAMETER_SET :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_FRAME_NUMBER;
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_VLDState = H264_VLD_FRAME_NUMBER;
#endif
				if (!m_VLDSliceHeaderFirstSlice &&
					(m_PictureParameterSetID != byteCode) &&
					(0 != m_NumberOfMacroblocksDecoded))
				{
					// The previous picture was not completely decoded: perform error concealment and output the picture
					DecoderErrorConcealment();
				}

				m_PictureParameterSetID = byteCode;
				m_PictureParameterSet = m_ParameterSets->ReadPictureParameterSet(m_PictureParameterSetID);
//				if (0 == m_PictureParameterSet)  {	error = true; break; }
				if (0 == m_PictureParameterSet)  {	return false; }

//				m_SequenceParameterSetID = m_PictureParameterSet->sequence_parameter_set_id;
				m_SequenceParameterSet = m_ParameterSets->ReadSequenceParameterSet(m_PictureParameterSet->sequence_parameter_set_id);
//				if (0 == m_SequenceParameterSet)  {	error = true; break; }
				if (0 == m_SequenceParameterSet)  {	return false; }

                if (m_VLDSliceHeaderFirstSlice)
                {
                    // First slice of an IDR picture
                    // Activate the sequence parameter set
                    if (m_SequenceParameterSet->vui_seq_parameters_flag)
                    {
                        if (m_SequenceParameterSet->vui_parameters->bitstream_restriction_flag)
                        {
                            m_MaxDecFrameBuffering = m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering;

                            // The only case in which bumping is not used is when num_reorder_frames is zero
                            // in which case it is known that pictures can be output as soon as they are decoded
                            // as there is no re-ordering
                            m_BumpingDecoder = (0 != m_SequenceParameterSet->vui_parameters->num_reorder_frames);
                            m_RemainingBumpingDelay = 0;
                        }
                        else
                        {
                            m_MaxDecFrameBuffering = annex_a_level_table[m_SequenceParameterSet->level_index][3] /
                                (384 * m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs);
                            if (m_MaxDecFrameBuffering > 16) m_MaxDecFrameBuffering = 16;

                            m_BumpingDecoder = true;
                        }
                    }
                    else
                    {
                        m_MaxDecFrameBuffering = annex_a_level_table[m_SequenceParameterSet->level_index][3] /
                            (384 * m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs);
                        if (m_MaxDecFrameBuffering > 16) m_MaxDecFrameBuffering = 16;

                        m_BumpingDecoder = true;
                    }
                }

				break;
				}

			// --------------------------------------------------------------
			case H264_VLD_FRAME_NUMBER :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                // m_SequenceParameterSet->log2_max_frame_num_minus4 is in the range of 0 to 12, inclusive
                byteCode = VLC_Get_Bits(m_SequenceParameterSet->log2_max_frame_num_minus4 + 4);
#else
				if (!ByteBufferDecodeFixedLengthCode(m_SequenceParameterSet->log2_max_frame_num_minus4 + 4, byteCode)) { no_bits_left = true; break; }
#endif
				if (!m_VLDSliceHeaderFirstSlice &&
					(m_VLDSliceHeaderPreviousSliceFrameNumber != (unsigned int)(byteCode)) &&
					(0 != m_NumberOfMacroblocksDecoded))
				{
					// The previous picture was not completely decoded: perform error concealment and output the picture
					DecoderErrorConcealment();
				}

				m_FrameNumber = byteCode;
				if (!m_SequenceParameterSet->frame_mbs_only_flag)
				{
					m_VLDState = H264_VLD_FIELD_PICTURE_FLAG;
				}
				else
				{
#if defined __ARC_MAINPROFILE__
					m_FieldPictureFlag = false;
					m_BottomFieldFlag = false;
#endif
#ifdef __INTERLACE__
                    m_IsAdaptiveFrame = m_SequenceParameterSet->mb_adaptive_frame_field_flag;
#endif
					m_VLDState = H264_VLD_IDR_PICTURE_ID;
				}
				break;

			// --------------------------------------------------------------
#if defined __ARC_MAINPROFILE__
			case H264_VLD_FIELD_PICTURE_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    byteCode = VLC_Get_Bit();
#else
					if (!ByteBufferDecodeFixedLengthCode(1)) { no_bits_left = true; break; }
#endif
					if (!m_VLDSliceHeaderFirstSlice &&
						(m_VLDSliceHeaderPreviousFieldPictureFlag != (byteCode == 1)) &&
						(0 != m_NumberOfMacroblocksDecoded))
					{
						// The previous picture was not completely decoded: perform error concealment and output the picture
						DecoderErrorConcealment();
					}

					m_FieldPictureFlag = (byteCode == 1);

					if (m_FieldPictureFlag)
					{
						m_VLDState = H264_VLD_BOTTOM_FIELD_FLAG;
#ifdef __INTERLACE__
                        m_IsAdaptiveFrame = false;
#endif
					}
					else
					{
						m_BottomFieldFlag = false;
						m_VLDState = H264_VLD_IDR_PICTURE_ID;
#ifdef __INTERLACE__
                        m_IsAdaptiveFrame = m_SequenceParameterSet->mb_adaptive_frame_field_flag;
#endif
					}
					break;
#endif

			// --------------------------------------------------------------
#if defined __ARC_MAINPROFILE__
			case H264_VLD_BOTTOM_FIELD_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    byteCode = VLC_Get_Bit();
#else
					if (!ByteBufferDecodeFixedLengthCode(1)) { no_bits_left = true; break; }
#endif
					if (!m_VLDSliceHeaderFirstSlice &&
						(m_VLDSliceHeaderPreviousBottomFieldFlag != (byteCode == 1)) &&
						(0 != m_NumberOfMacroblocksDecoded))
					{
						// The previous picture was not completely decoded: perform error concealment and output the picture
						DecoderErrorConcealment();
					}

					m_BottomFieldFlag = (byteCode == 1);
					m_VLDState = H264_VLD_IDR_PICTURE_ID;
					break;
#endif
			// --------------------------------------------------------------
			case H264_VLD_IDR_PICTURE_ID :
				m_VLDSliceHeaderIDRPictureID = 0;

				// The following should in due course be used for coded picture boundary detection
				if (H264_CODED_SLICE_IDR == m_NALUnitType)
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
				    m_VLDState = H264_VLD_POC_LSB;
                    byteCode = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				    m_VLDState = H264_VLD_POC_LSB;
#endif
					if (!m_VLDSliceHeaderFirstSlice &&
						((H264_CODED_SLICE_IDR == m_NALUnitType) && m_VLDSliceHeaderPreviousSliceWasIDR && (byteCode != m_VLDSliceHeaderPreviousSliceIDRPictureID)) &&
						(0 != m_NumberOfMacroblocksDecoded))
					{
						// The previous picture was not completely decoded: perform error concealment and output the picture
						DecoderErrorConcealment();
					}

					m_VLDSliceHeaderIDRPictureID = byteCode;

					m_VLDSliceHeaderPreviousFrameNumberOffset = 0;
				}
				else if (m_FrameNumber < m_VLDSliceHeaderPreviousSliceFrameNumber)
				{
					m_VLDSliceHeaderPreviousFrameNumberOffset += m_SequenceParameterSet->maximum_frame_number;
				    m_VLDState = H264_VLD_POC_LSB;
				}
                else {
				    m_VLDState = H264_VLD_POC_LSB;
                }

				break;

			// --------------------------------------------------------------
			case H264_VLD_POC_LSB :
				if (0 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					// Firstly determine whether the MSBs of m_PictureOrderCount should be stored 
					// as m_VLDSliceHeaderPreviousReferenceSlicePOC. This is done when this is 
					// the first slice after the last slice of a reference picture. That is, 
					// when previous slice was stored and this one is not, or 
					// both previous and this are stored but the frame numbers are different.
					// m_VLDSliceHeaderPreviousReferenceSlicePOC is set to zero at an IDR slice
					// and after decoding a picture with MMCO Reset

					if (H264_CODED_SLICE_IDR == m_NALUnitType)
					{
						m_VLDSliceHeaderPreviousReferenceSlicePOC = 0;
					}
					else if (((0 != m_VLDSliceHeaderPreviousSliceNALReferenceIDC) && (0 == m_NALReferenceIDC)) ||
							 ((0 != m_VLDSliceHeaderPreviousSliceNALReferenceIDC) && (0 != m_NALReferenceIDC) 
										&& (m_VLDSliceHeaderPreviousSliceFrameNumber != m_FrameNumber)))
					{
						m_VLDSliceHeaderPreviousReferenceSlicePOC = m_PictureOrderCount;
					}

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    // log2_max_pic_order_cnt_lsb_minus4 is in the range of 0 to 12 inclusive
					m_VLDSliceHeaderPicOrderCntLsb = VLC_Get_Bits(m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4+4);
#else
					if (!ByteBufferDecodeFixedLengthCode(m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4+4, byteCode)) { no_bits_left = true; break; }
					m_VLDSliceHeaderPicOrderCntLsb = byteCode;
#endif
					m_VLDState = H264_VLD_DELTA_POC_BOTTOM;
				}
				else
				{
					m_VLDState = H264_VLD_DELTA_POC_0;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_DELTA_POC_BOTTOM :
				{
				int prev_lsb, prev_msb, curr_msb;

				if (m_PictureParameterSet->pic_order_present_flag 
                    )
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_SE); 
				    m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
					m_VLDSliceHeaderDeltaPicOrderCountBottom = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDSliceHeaderDeltaPicOrderCountBottom = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
				    m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
#endif
				}
				else
				{
					m_VLDSliceHeaderDeltaPicOrderCountBottom = 0;
				    m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
				}

				if (!m_VLDSliceHeaderFirstSlice &&
					((m_VLDSliceHeaderPreviousSliceFrameNumber == m_FrameNumber) && ((m_VLDSliceHeaderPicOrderCntLsb != m_VLDSliceHeaderPreviousPicOrderCntLsb) || (m_VLDSliceHeaderDeltaPicOrderCountBottom != m_VLDSliceHeaderPreviousDeltaPicOrderCountBottom))) &&
					(0 != m_NumberOfMacroblocksDecoded))
				{
					// The previous picture was not completely decoded: perform error concealment and output the picture
					DecoderErrorConcealment();
				}
				
				prev_lsb = m_VLDSliceHeaderPreviousReferenceSlicePOC & 
								(m_SequenceParameterSet->max_pic_order_cnt_lsb - 1);

				prev_msb = m_VLDSliceHeaderPreviousReferenceSlicePOC - prev_lsb;

				curr_msb = prev_msb;

				if ((m_VLDSliceHeaderPicOrderCntLsb < prev_lsb) &&
					((prev_lsb - m_VLDSliceHeaderPicOrderCntLsb) >= (m_SequenceParameterSet->max_pic_order_cnt_lsb >> 1)))
				{
					curr_msb = prev_msb + m_SequenceParameterSet->max_pic_order_cnt_lsb;
				}
				else if ((m_VLDSliceHeaderPicOrderCntLsb > prev_lsb) &&
						 ((m_VLDSliceHeaderPicOrderCntLsb - prev_lsb) > (m_SequenceParameterSet->max_pic_order_cnt_lsb >> 1)))
				{
					curr_msb = prev_msb - m_SequenceParameterSet->max_pic_order_cnt_lsb;
				}

					m_TopFieldOrderCount = curr_msb + m_VLDSliceHeaderPicOrderCntLsb;
					m_BottomFieldOrderCount = m_TopFieldOrderCount + m_VLDSliceHeaderDeltaPicOrderCountBottom;

					m_PictureOrderCount = MIN(m_TopFieldOrderCount, m_BottomFieldOrderCount);
				m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
				break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DELTA_POC_0 :
				if (1 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					if (!m_SequenceParameterSet->delta_pic_order_always_zero_flag)
					{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                        UVLC_Set_Control(UVLC_SE); 
						m_VLDSliceHeaderDeltaPicOrderCount[0] = CAVLC_Read_Symbol();
#else
						if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
						m_VLDSliceHeaderDeltaPicOrderCount[0] = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
#endif
					}

					m_VLDState = H264_VLD_DELTA_POC_1;
				}
				else
				{
					m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_DELTA_POC_1 :
				if (1 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					unsigned int absolute_frame_number;
					int pic_order_cnt_cycle_cnt = 0;
					int frame_num_in_pic_order_cnt_cycle = 0;
					int expected_pic_order_cnt = 0;
					int i;

					if (!m_SequenceParameterSet->delta_pic_order_always_zero_flag)
					{
						if (m_PictureParameterSet->pic_order_present_flag 
#if defined __ARC_MAINPROFILE__
                            && !m_FieldPictureFlag)
#else
                            )
#endif
						{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                            UVLC_Set_Control(UVLC_SE); 
							m_VLDSliceHeaderDeltaPicOrderCount[1] = CAVLC_Read_Symbol();
#else
							if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
							m_VLDSliceHeaderDeltaPicOrderCount[1] = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
#endif
						}
						else
						{
							m_VLDSliceHeaderDeltaPicOrderCount[1] = 0;
						}
					}
					else
					{
						m_VLDSliceHeaderDeltaPicOrderCount[0] = 0;
						m_VLDSliceHeaderDeltaPicOrderCount[1] = 0;
					}

					if (!m_VLDSliceHeaderFirstSlice &&
						((m_VLDSliceHeaderPreviousSliceFrameNumber == m_FrameNumber) && ((m_VLDSliceHeaderDeltaPicOrderCount[0] != m_VLDSliceHeaderPreviousDeltaPicOrderCount[0]) || (m_VLDSliceHeaderDeltaPicOrderCount[1] != m_VLDSliceHeaderPreviousDeltaPicOrderCount[1]))) &&
						(0 != m_NumberOfMacroblocksDecoded))
					{
						// The previous picture was not completely decoded: perform error concealment and output the picture
						DecoderErrorConcealment();
					}

					// Calculate the absolute frame number by adding the LSBs in m_FrameNumber to the 
					// MSBs in m_VLDSliceHeaderPreviousFrameNumberOffset
					if (0 != m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle)
					{
						absolute_frame_number = m_VLDSliceHeaderPreviousFrameNumberOffset + m_FrameNumber;
					}
					else
					{
						absolute_frame_number = 0;
					}

					if ((0 == m_NALReferenceIDC) && (absolute_frame_number > 0))
					{
						absolute_frame_number--;
					}

					// Derive pic_order_cnt_cycle_cnt and frame_num_in_pic_order_cnt_cycle

					if (absolute_frame_number > 0)
					{
						pic_order_cnt_cycle_cnt = (absolute_frame_number - 1) /
									m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle;

						frame_num_in_pic_order_cnt_cycle = (absolute_frame_number - 1) %
									m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle;

						// expected_delta_per_pic_order_cnt_cycle has been derived and stored in the 
						// sequence parameter set

						// Calculate expected_pic_order_cnt

						expected_pic_order_cnt = pic_order_cnt_cycle_cnt *
								m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle;

						for (i = 0; i <= frame_num_in_pic_order_cnt_cycle; i++)
						{
							expected_pic_order_cnt += m_SequenceParameterSet->offset_for_ref_frame[i];
						}
					}

					if (0 == m_NALReferenceIDC)
					{
						expected_pic_order_cnt += m_SequenceParameterSet->offset_for_non_ref_pic;
					}

						m_TopFieldOrderCount = expected_pic_order_cnt + m_VLDSliceHeaderDeltaPicOrderCount[0];
						m_BottomFieldOrderCount = m_TopFieldOrderCount +
												m_SequenceParameterSet->offset_for_top_to_bottom_field + 
												m_VLDSliceHeaderDeltaPicOrderCount[1];

						m_PictureOrderCount = MIN(m_TopFieldOrderCount, m_BottomFieldOrderCount);
				}

				// if (2 == m_SequenceParameterSet->pic_order_cnt_type) POC can not be calculated until
				// after coded picture boundary detection

				m_VLDState = H264_VLD_REDUNDANT_PICTURE_COUNT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_REDUNDANT_PICTURE_COUNT :
				if (m_PictureParameterSet->redundant_pic_cnt_present_flag)
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
//					m_RedundantPictureCount = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
//					m_RedundantPictureCount = byteCode;
#endif
				}

				if (2 == m_SequenceParameterSet->pic_order_cnt_type)
				{
					// This is the POC type with no POC-specific information in the bitstream

					// Calculate the absolute frame number by adding the LSBs in m_FrameNumber to the 
					// MSBs in m_VLDSliceHeaderPreviousFrameNumberOffset

					m_PictureOrderCount = (m_VLDSliceHeaderPreviousFrameNumberOffset + m_FrameNumber) << 1;

					if (0 == m_NALReferenceIDC)
					{
						m_PictureOrderCount--;
					}
					m_TopFieldOrderCount = m_PictureOrderCount;
					m_BottomFieldOrderCount = m_PictureOrderCount;
				}

				// Start of picture detection
				if ((m_VLDSliceHeaderFirstSlice) ||
					(m_VLDSliceHeaderPreviousSliceFrameNumber != m_FrameNumber) ||
					((m_VLDSliceHeaderPreviousSliceNALReferenceIDC != m_NALReferenceIDC) && ((0 == m_VLDSliceHeaderPreviousSliceNALReferenceIDC) || (0 == m_NALReferenceIDC))) ||
					((H264_CODED_SLICE_IDR == m_NALUnitType) && m_VLDSliceHeaderPreviousSliceWasIDR && (m_VLDSliceHeaderIDRPictureID != m_VLDSliceHeaderPreviousSliceIDRPictureID)) ||
					((m_VLDSliceHeaderPreviousSliceFrameNumber == m_FrameNumber) && (0 == m_SequenceParameterSet->pic_order_cnt_type) && ((m_VLDSliceHeaderPicOrderCntLsb != m_VLDSliceHeaderPreviousPicOrderCntLsb) || (m_VLDSliceHeaderDeltaPicOrderCountBottom != m_VLDSliceHeaderPreviousDeltaPicOrderCountBottom))) ||
					((m_VLDSliceHeaderPreviousSliceFrameNumber == m_FrameNumber) && (1 == m_SequenceParameterSet->pic_order_cnt_type) && ((m_VLDSliceHeaderDeltaPicOrderCount[0] != m_VLDSliceHeaderPreviousDeltaPicOrderCount[0]) || (m_VLDSliceHeaderDeltaPicOrderCount[1] != m_VLDSliceHeaderPreviousDeltaPicOrderCount[1]))) ||
					((H264_CODED_SLICE_IDR == m_NALUnitType) && !m_VLDSliceHeaderPreviousSliceWasIDR)
				   )

				{
					if (!VLDDecodeStartOfPicture())
					{
						assert(false);
                        return false;
//						error = true;
//						break;
					}
				}
				else
				{
					if ((m_PacketLost) && (0 == m_VLDCurrentPicture))
					{
						// This is caused by a significant loss of consecutive NAL units such that the means
						// to detect frame loss fail. For example, the current frame number is the same
						// as the previous frame number because it has wrapped.
						if (!VLDDecodeStartOfPicture())
						{
							assert(false);
                            return false;
//							error = true;
//							break;
						}
					}
					else
					{
						m_CurrentSlice_ID++;
					}
				}

                // skip the state H264_VLD_DIRECT_SPATIAL_FLAG
				m_VLDState = H264_VLD_NUM_REF_IDX_OVERRIDE;
				break;


			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_IDX_OVERRIDE :
				if (H264_P_SLICE == m_SliceType)
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
					if (VLC_Get_Bit() == 1) // num_ref_idx_active_override_flag
#else
					if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
					if (byteCode == 1) // num_ref_idx_active_override_flag
#endif
					{
						m_VLDState = H264_VLD_NUM_REF_IDX_L0_MINUS1;
					}
					else
					{
						m_SliceNumList0RefPictures = m_PictureParameterSet->num_ref_idx_l0_active;
						m_VLDState = H264_VLD_REF_REORDERING_FLAG_L0;
					}
				}
				else
				{
					m_SliceNumList0RefPictures = 0;
					m_VLDState = H264_VLD_REF_REORDERING_FLAG_L0;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_NUM_REF_IDX_L0_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_REF_REORDERING_FLAG_L0;
				m_SliceNumList0RefPictures = CAVLC_Read_Symbol() + 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SliceNumList0RefPictures = byteCode + 1;
				m_VLDState = H264_VLD_REF_REORDERING_FLAG_L0;
#endif
				break;


			// --------------------------------------------------------------
			case H264_VLD_REF_REORDERING_FLAG_L0 :
				if ((H264_I_SLICE != m_SliceType) /*&& (H264_SI_SLICE != m_SliceType)*/)
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
					byteCode = VLC_Get_Bit();
#else
					if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
#endif
					{
							m_DecodedPictureBuffer->CreateFrameDefaultIndicesForPSlice();
					}
					if (1 == byteCode)
					{
						m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
					}
					else
					{
						m_DecodedPictureBuffer->ActOnNextRemappingInstruction(
							true, H264_RMPNI_END_OF_LOOP, 0);
                        m_VLDState = H264_VLD_PRED_WEIGHT_LUMA_DENOM;
					}
				}
				else
				{
					m_VLDState = H264_VLD_PRED_WEIGHT_LUMA_DENOM;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0 :
				{
				h264_RMPNI_type command_type;
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				command_type = (h264_RMPNI_type) CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				command_type = (h264_RMPNI_type) byteCode;
#endif
				if (H264_RMPNI_NEGATIVE_ADPN == command_type)
				{
					m_VLDState = H264_VLD_REF_REORDERING_NEG_DIFF_L0;
				}
				else if (H264_RMPNI_POSITIVE_ADPN == command_type)
				{
					m_VLDState = H264_VLD_REF_REORDERING_POS_DIFF_L0;
				}
				else if (H264_RMPNI_LPIR == command_type)
				{
					m_VLDState = H264_VLD_REF_REORDERING_LONG_TERM_L0;
				}
				else
				{
					// End of loop
					m_DecodedPictureBuffer->ActOnNextRemappingInstruction(
						true, H264_RMPNI_END_OF_LOOP, 0);
                        m_VLDState = H264_VLD_PRED_WEIGHT_LUMA_DENOM;
				}
				break;
				}

			// --------------------------------------------------------------
			case H264_VLD_REF_REORDERING_NEG_DIFF_L0 :
				// A negative difference of picture numbers
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
#endif
				m_DecodedPictureBuffer->ActOnNextRemappingInstruction(
					true, H264_RMPNI_NEGATIVE_ADPN, byteCode);

				break;

			// --------------------------------------------------------------
			case H264_VLD_REF_REORDERING_POS_DIFF_L0 :
				// A positive difference of picture numbers
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
#endif
				m_DecodedPictureBuffer->ActOnNextRemappingInstruction(
					true, H264_RMPNI_POSITIVE_ADPN, byteCode);

				break;

			// --------------------------------------------------------------
			case H264_VLD_REF_REORDERING_LONG_TERM_L0 :
				// A long term index
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_VLDState = H264_VLD_REF_REORDERING_OF_PIC_NUMS_IDC_L0;
#endif
				m_DecodedPictureBuffer->ActOnNextRemappingInstruction(
					true, H264_RMPNI_LPIR, byteCode);

				break;


			// --------------------------------------------------------------
			case H264_VLD_PRED_WEIGHT_LUMA_DENOM :
				{
					if (H264_CODED_SLICE_IDR == m_NALUnitType)
					{
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_IDR;
					}
					else
					{
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_ADAPTIVE_FLAG;
					}
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_PRED_WEIGHT_CHROMA_DENOM :

				assert(false);
				break;
				
			// --------------------------------------------------------------
			case H264_VLD_PRED_WEIGHT_LUMA_WEIGHT_FLAG_L0 :
				assert(false);
				break;

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_IDR :
				{
				bool flush_non_reference_pictures;
				bool store_as_a_long_term_picture;

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(2);
#else
				if (!ByteBufferDecodeFixedLengthCode(2, byteCode)) { no_bits_left = true; break; }
#endif
				flush_non_reference_pictures = (0 != (byteCode & 2));

				store_as_a_long_term_picture = (0 != (byteCode & 1));

				m_DecodedPictureBuffer->SetIDRParameters(true, flush_non_reference_pictures, 
														 store_as_a_long_term_picture);

				m_VLDState = H264_VLD_CABAC_INIT_IDC;
				break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_ADAPTIVE_FLAG :
				m_DecodedPictureBuffer->SetIDRParameters(false, false, false);

				if (0 == m_NALReferenceIDC)
				{
					// Set to sliding window mode so avoid having to create an MMCO end of loop command
					m_DecodedPictureBuffer->SetSlidingWindowBufferingMode(true);
					m_VLDState = H264_VLD_CABAC_INIT_IDC;
				}
				else
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
					if (0 == VLC_Get_Bit())
#else
					if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
					if (0 == byteCode)
#endif
					{
						// Sliding window
						m_DecodedPictureBuffer->SetSlidingWindowBufferingMode(true);
						m_VLDState = H264_VLD_CABAC_INIT_IDC;
					}
					else
					{
						// MMCO
						m_DecodedPictureBuffer->SetSlidingWindowBufferingMode(false);
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					}
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_MMCO :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
                byteCode = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
#endif
				switch ((h264_MMCO_type)(byteCode))
				{
					case H264_MMCO_END_OF_LOOP :
					{
						m_DecodedPictureBuffer->SetNextMMCOCommand(H264_MMCO_END_OF_LOOP);
						m_VLDState = H264_VLD_CABAC_INIT_IDC;
						break;
					}

					case H264_MMCO_MARK_SHORT_UNUSED :
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_SHORT_UNUSED;
						break;

					case H264_MMCO_MARK_LONG_UNUSED :
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_LONG_UNUSED;
						break;

					case H264_MMCO_ASSIGN_LONG_TERM :
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_ASSIGN_LONG_TERM_DPN;
						break;

					case H264_MMCO_MAX_LONG_INDEX :
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MAX_LONG_INDEX;
						break;

					case H264_MMCO_RESET :
					{
						m_DecodedPictureBuffer->SetNextMMCOCommand(H264_MMCO_RESET);
						break;
					}

					case H264_MMCO_STORE_AS_LONG_TERM :
						m_VLDState = H264_VLD_DEC_REF_PIC_MARK_STORE_AS_LONG_TERM;
						break;

					default :
                        return false;
//						error = true;
//						break;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_SHORT_UNUSED :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					byteCode = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
#endif
					m_DecodedPictureBuffer->SetNextMMCOCommand(H264_MMCO_MARK_SHORT_UNUSED,byteCode);

					break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_LONG_UNUSED :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					byteCode = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
#endif
					m_DecodedPictureBuffer->SetNextMMCOCommandLTI(H264_MMCO_MARK_LONG_UNUSED,(UNSIGNED_SEPTET)byteCode);

					break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_ASSIGN_LONG_TERM_DPN :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_ASSIGN_LONG_TERM_LTN;
					m_VLDSliceHeaderDiffPicNumMinus1 = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDSliceHeaderDiffPicNumMinus1 = byteCode;
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_ASSIGN_LONG_TERM_LTN;
#endif
					break;

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_ASSIGN_LONG_TERM_LTN :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					byteCode = (UNSIGNED_SEPTET)(CAVLC_Read_Symbol());
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
#endif
					m_DecodedPictureBuffer->SetNextMMCOCommand(H264_MMCO_ASSIGN_LONG_TERM,
                                                               m_VLDSliceHeaderDiffPicNumMinus1,
                                                               (UNSIGNED_SEPTET)(byteCode));

					break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_MAX_LONG_INDEX :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					byteCode = (UNSIGNED_SEPTET)(CAVLC_Read_Symbol());
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
#endif
				    m_DecodedPictureBuffer->SetNextMMCOCommandLTIP1(H264_MMCO_MAX_LONG_INDEX,(UNSIGNED_SEPTET)(byteCode));

					break;
				}

			// --------------------------------------------------------------
			case H264_VLD_DEC_REF_PIC_MARK_STORE_AS_LONG_TERM :
				{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
					byteCode = (UNSIGNED_SEPTET)(CAVLC_Read_Symbol());
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_VLDState = H264_VLD_DEC_REF_PIC_MARK_MMCO;
#endif
			        m_DecodedPictureBuffer->SetNextMMCOCommandLTI(H264_MMCO_STORE_AS_LONG_TERM,(UNSIGNED_SEPTET)(byteCode));

					break;
				}

			// --------------------------------------------------------------
			case H264_VLD_CABAC_INIT_IDC :
#ifdef __CABAC__
				if ((m_PictureParameterSet->CABAC_entropy_coding_mode) && 
					(H264_SI_SLICE != m_SliceType) &&
					(H264_I_SLICE != m_SliceType))
				{

					// Decode CABAC_INIT_IDC
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
				    m_StartOfSlice = true;
				    m_VLDEndOfSliceFlag = false;
					m_CABACInitIDC = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_CABACInitIDC = byteCode;
#endif
				}
				else
				{
					m_CABACInitIDC = 3;	// Just used for table indexing in this case
				    m_StartOfSlice = true;
				    m_VLDEndOfSliceFlag = false;
				}
#else
				if ((m_PictureParameterSet->CABAC_entropy_coding_mode) && 
					(H264_SI_SLICE != m_SliceType) &&
					(H264_I_SLICE != m_SliceType))
				{
					assert(false);
//					error = true;
					return false;
				}
#endif
		
				m_VLDState = H264_VLD_SLICE_QP_DELTA;
				break;

			// --------------------------------------------------------------
			case H264_VLD_SLICE_QP_DELTA :
				{
				int chroma_QP;
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_DISABLE_DEBLOCKING_IDC;
				m_VLDQuant = m_PictureParameterSet->pic_init_qp + CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_VLDQuant = m_PictureParameterSet->pic_init_qp + VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
				m_VLDState = H264_VLD_DISABLE_DEBLOCKING_IDC;
#endif
				m_VLDQuantDiv = QUANT_DIV(m_VLDQuant);
				m_VLDQuantMod = QUANT_MOD(m_VLDQuant, m_VLDQuantDiv);

				chroma_QP = h264_quantiser_chroma_mapping_table[
					CLIP_CHROMA_QUANT((int)(m_VLDQuant) + m_PictureParameterSet->chroma_qp_index_offset)];
				m_VLDQuantChromaDiv = QUANT_DIV(chroma_QP);
				m_VLDQuantChromaMod = QUANT_MOD(chroma_QP, m_VLDQuantChromaDiv);

				break;
				}

			// --------------------------------------------------------------
			// --------------------------------------------------------------
			case H264_VLD_DISABLE_DEBLOCKING_IDC :
				if (m_PictureParameterSet->deblocking_filter_parameters_present_flag)
				{
					// Deblocking filter parameters
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    UVLC_Set_Control(UVLC_UE); 
					m_DisableDeblockingFilterIDC = CAVLC_Read_Symbol();
#else
					if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
					m_DisableDeblockingFilterIDC = byteCode;
#endif
					if (1 != m_DisableDeblockingFilterIDC )
					{
						m_VLDState = H264_VLD_SLICE_ALPHA_OFFSET;
					}
					else
					{
						// Set to defaults
						m_SliceAlphaC0Offset = 0;
						m_SliceBetaOffset = 0;

						m_VLDState = H264_VLD_SLICE_GROUP_CHANGE_CYCLE;
					}
				}
				else
				{
					// No deblocking filter parameters present - so set to default
					m_DisableDeblockingFilterIDC = 0;
					m_SliceAlphaC0Offset = 0;
					m_SliceBetaOffset = 0;

					m_VLDState = H264_VLD_SLICE_GROUP_CHANGE_CYCLE;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_SLICE_ALPHA_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_SLICE_BETA_OFFSET;
				m_SliceAlphaC0Offset = CAVLC_Read_Symbol() << 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SliceAlphaC0Offset = VLDDecodeMapCodeNumberToSignedGolomb(byteCode) << 1;
				m_VLDState = H264_VLD_SLICE_BETA_OFFSET;
#endif
				break;


			// --------------------------------------------------------------
			case H264_VLD_SLICE_BETA_OFFSET :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_SE); 
				m_VLDState = H264_VLD_SLICE_GROUP_CHANGE_CYCLE;
				m_SliceBetaOffset = CAVLC_Read_Symbol() << 1;
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SliceBetaOffset = VLDDecodeMapCodeNumberToSignedGolomb(byteCode) << 1;
				m_VLDState = H264_VLD_SLICE_GROUP_CHANGE_CYCLE;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_SLICE_GROUP_CHANGE_CYCLE :
				if ((m_PictureParameterSet->num_slice_groups > 1) &&
					(m_PictureParameterSet->slice_group_map_type >= 3) &&
					(m_PictureParameterSet->slice_group_map_type <= 5))
				{
					int max_slice_group_change_cycle = 
							((m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs)
								/ m_PictureParameterSet->slice_group_change_rate);

					int slice_group_change_cycle_code_length = 0;
					int slice_group_change_cycle = 0;
					int number_of_macroblocks;

					while (max_slice_group_change_cycle > 0)
					{
						slice_group_change_cycle_code_length++;
						max_slice_group_change_cycle >>= 1;
					}

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                    // the value of slice_group_change_cycle is not defined in H264 Spec when slice_group_change_cycle_code_length = 0
                    // since VLC_Get_Bits(0) will return the next 32 bits from the HW buffer, so we only call it when slice_group_change_cycle_code_length
                    // is greater than 0 otherwise slice_group_change_cycle retains 0.
                    if (slice_group_change_cycle_code_length > 0) {
					    slice_group_change_cycle = VLC_Get_Bits(slice_group_change_cycle_code_length);
                    }
#else
					if (!ByteBufferDecodeFixedLengthCode(slice_group_change_cycle_code_length, byteCode)) { no_bits_left = true; break; }
					slice_group_change_cycle = byteCode;
#endif
					m_MapUnitsInSliceGroup0 = slice_group_change_cycle * m_PictureParameterSet->slice_group_change_rate;

					number_of_macroblocks = 
						m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs;

					m_MapUnitsInSliceGroup0 = MIN(m_MapUnitsInSliceGroup0, number_of_macroblocks);
				}

				// Slice header is now parsed. Get ready to parse slice data

				if (m_VLDSliceHeaderFirstSliceOfPicture)
				{
					if (!m_DecoderControl.SetSize(m_SequenceParameterSet->frame_width_in_mbs << 4, 
												   m_SequenceParameterSet->frame_height_in_mbs << 4))
					{
//						error = true;
//						break;
                        return false;
					}

#ifdef __INTERLACE__
					if (m_SequenceParameterSet->mb_adaptive_frame_field_flag)
					{
						if (!m_DecoderControl.InitialiseForMBAFF())
						{
							assert(false);
//							error = true;
//							break;
                            return false;
						}
					}
#endif

#if defined __ARC_MAINPROFILE__
					if (!m_FieldPictureFlag)
					{
#endif
						m_NumberOfMacroblocksInPicture = 
							m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs;
#if defined __ARC_MAINPROFILE__
					}
					else
					{
						m_NumberOfMacroblocksInPicture = 
							(m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs) >> 1;
					}
#endif
//					if (!VLDDecodeAllocateSliceGroups()) {	error = true; break; }
					if (!VLDDecodeAllocateSliceGroups()) {	return false; }
				}

				// Use m_VLDSliceHeaderFirstMBInSlice to set the control list to point at 
				// the appropriate first macroblock
				{
					h264_decoder_control_t * control = m_DecoderControl.GetMacroblock(m_VLDSliceHeaderFirstMBInSlice);
					
					m_DecoderControl.m_CurrentElement = control;

//					if (0 == control) {	error = true; break; }
					if (0 == control) {	return false; }

					// Get the slice group ID from the current macroblock
					m_VLDCurrentSliceGroup = control->slice_group;
				}

				// Set next state
				if ((H264_I_SLICE == m_SliceType) /*|| (H264_SI_SLICE == m_SliceType)*/)
				{
#ifdef __CABAC__
					if (m_IsAdaptiveFrame)
					{
						m_VLDState = H264_VLD_FIELD_MB_DECODING_FLAG;
					}
					else
#endif
					{
						m_VLDState = H264_VLD_MB_TYPE;
					}
				}
				else
				{
#ifdef __CABAC__
					if (m_PictureParameterSet->CABAC_entropy_coding_mode)
					{
						m_VLDState = H264_CABAC_SKIP_FLAG;
					}
					else
#endif
					{
						m_VLDState = H264_VLD_RUN;
					}
				}

				// Record information about this slice, to be used when decoding the next slice header
				m_VLDSliceHeaderFirstSlice = false;
				m_VLDSliceHeaderPreviousSliceNALReferenceIDC = m_NALReferenceIDC;
				m_VLDSliceHeaderPreviousSliceIDRPictureID = m_VLDSliceHeaderIDRPictureID;
				m_VLDSliceHeaderPreviousSliceWasIDR = (H264_CODED_SLICE_IDR == m_NALUnitType);
#if defined __ARC_MAINPROFILE__
				m_VLDSliceHeaderPreviousFieldPictureFlag = m_FieldPictureFlag;
				m_VLDSliceHeaderPreviousBottomFieldFlag = m_BottomFieldFlag;
#endif
//				break;
				return true;

			// --------------------------------------------------------------
			default :
                return false;
//				error = true;
//				break;
		}

#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
        // the states in the conditional expression are referred only in state H264_VLD_SLICE_GROUP_CHANGE_CYCLE.
//		if (error || no_bits_left || (H264_VLD_MB_TYPE == m_VLDState) || (H264_VLD_RUN == m_VLDState) ||
//		   (H264_VLD_FIELD_MB_DECODING_FLAG == m_VLDState) || (H264_CABAC_SKIP_FLAG == m_VLDState))
		if (no_bits_left)
		{
			break;  // Break the while (true) loop
		}
#endif
	}
//	m_PreviousSliceType = m_SliceType;

//	return !error;
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeStartOfPicture						*/
/* DESCRIPTION:	This is called when the start of a new picture is detected
				It initialises variables and ensures the decoded picture 
				buffer is dimensioned appropriately.						*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeStartOfPicture(void)
{
	h264_picture_type type;
	unsigned int previous_frame_number_plus1;

	m_VLDSliceHeaderFirstSliceOfPicture = true;
	m_CurrentSlice_ID = 0;

	// Set number of decoded buffer frames to m_MaxDecFrameBuffering plus one
	// The extra one is for decoding of the current frame before storing it.
#if defined __ARC_SHARED_BUFFER__
	if (!m_DecodedPictureBuffer->Modify(m_MaxDecFrameBuffering + 2, 
  	    m_SequenceParameterSet->num_ref_frames,
	    m_SequenceParameterSet->maximum_frame_number-1))
#else
	if (!m_DecodedPictureBuffer->Modify(m_MaxDecFrameBuffering + 1, 
  	    m_SequenceParameterSet->num_ref_frames,
	    m_SequenceParameterSet->maximum_frame_number-1))
#endif
	{
		assert(false);
		return false;
	}

	if(m_PictureParameterSet->num_slice_groups == 1)
		m_InloopDeblock = true;
	else
		m_InloopDeblock	= false;
	
	bool first_picture = false;

	if (!m_DecodingHasStarted)
	{
		first_picture = true;

		if (H264_CODED_SLICE_IDR != m_NALUnitType)
		{
			// If decoding is about to start and this is not an IDR slice,
			// need to make the decoded picture buffer look like it is full
			// of short term reference pictures
			int i;
			for (i=0; i<m_SequenceParameterSet->num_ref_frames; i++)
			{
				bool mmco_reset;
				CBTPicture * output_frame;

				m_DecodedPictureBuffer->GetPictureForDecoding(&m_VLDCurrentPicture, H264_FRAME_PICTURE);

				assert(0 != m_VLDCurrentPicture);

				m_VLDCurrentPicture->set_size(16 * m_SequenceParameterSet->frame_width_in_mbs,
                                                              16 * m_SequenceParameterSet->frame_height_in_mbs, m_MaxDecFrameBuffering);

				m_DecodedPictureBuffer->SetDecodingPictureParameters(
					m_FrameNumber - m_SequenceParameterSet->num_ref_frames + i, 0, 0, 0);

				m_DecodedPictureBuffer->StorePicture(true, &mmco_reset);
#ifdef __B_PICTURES__
				m_DecodedPictureBuffer->StoreConcealmentMotionVectors();
#endif // def __B_PICTURES__

				// Mark the dummy picture as not waiting for output by fooling the
				// decoded picture buffer into thinking we want to output a picture
				m_DecodedPictureBuffer->OutputFrame(&output_frame);
			}
		}

		m_DecodingHasStarted = true;
	}

	// Get a frame from the decoded picture buffer to use as the working picture buffer, 
	// to decode the current picture into in case of frame coding and in case of first 
	// field of a frame in field coding.
#if defined __ARC_MAINPROFILE__
	if (!m_FieldPictureFlag)
	{
#endif
		type = H264_FRAME_PICTURE;
#if defined __ARC_MAINPROFILE__
	}
	else if (!m_BottomFieldFlag)
	{
		type = H264_TOP_FIELD_PICTURE;
	}
	else
	{
		type = H264_BOTTOM_FIELD_PICTURE;
	}
#endif
	// Check whether there is a gap in frame numbers
	// If a packet has been lost, assume that all missing frames are due to loss.
	// Otherwise, if gaps in frame numbers are allowed according to the sequence
	// parameter set, then treat as gaps in frame number process
	// Otherwise assert. Not sure how this last case could occur.

	previous_frame_number_plus1 =
		((m_VLDSliceHeaderPreviousSliceFrameNumber+1)&(m_SequenceParameterSet->maximum_frame_number-1));

	// Expected behaviour is:
	//	 FN = PrevFN   if previous was not a reference frame
	//   FN = PrevFN+1 if previous was a reference frame
	// So check for this not being the case

	bool gap_in_frame_numbers;
	
	if ((H264_CODED_SLICE_IDR == m_NALUnitType) || (first_picture))
	{
		// A gap will be expected as frame number is set back to zero, but there is no significance to this
		// as far as behaviour for packet loss or gaps in frame number is concerned
		gap_in_frame_numbers = false;
	}
	else
	{
		// Frame or first field of a field pair
		gap_in_frame_numbers =
			((0 == m_VLDSliceHeaderPreviousSliceNALReferenceIDC) && 
					(m_VLDSliceHeaderPreviousSliceFrameNumber != m_FrameNumber)) ||
			((0 != m_VLDSliceHeaderPreviousSliceNALReferenceIDC) && 
					(previous_frame_number_plus1 != m_FrameNumber));
	}

	if (!gap_in_frame_numbers)
	{
		// Normal case. Select a picture to decode into
		m_DecodedPictureBuffer->GetPictureForDecoding(&m_VLDCurrentPicture, type);
	}
	else  // gap_in_frame_numbers
	{
		if (m_PacketLost)
		{
			VLDDecodeLostReferencePictureConcealment();

			// Select a picture to decode into after performing lost picture concealment
			m_DecodedPictureBuffer->GetPictureForDecoding(&m_VLDCurrentPicture, type);
		}
		else if (m_SequenceParameterSet->required_frame_num_update_behaviour_flag)
		{
			// Gaps in frame numbers process

			// Select a picture to decode into
			m_DecodedPictureBuffer->GetPictureForDecoding(&m_VLDCurrentPicture, type);

			int number_of_non_existing_frames = (m_FrameNumber + m_SequenceParameterSet->maximum_frame_number - 1
					- m_VLDSliceHeaderPreviousSliceFrameNumber) & (m_SequenceParameterSet->maximum_frame_number - 1);

			if (m_VLDSliceHeaderPreviousSliceFrameNumber == m_FrameNumber)
			{
				number_of_non_existing_frames = 0;
			}

			while (number_of_non_existing_frames > 0)
			{
				// Not sure what else needs to be done if not baseline - do motion vectors need to be stored for direct mode?
				// What about calculation of POC for default reference index ordering for B pictures?
				assert(m_SequenceParameterSet->baseline_compatibility_flag);

				// Bumping process
				CBTPicture * decoded_picture;

				m_DecodedPictureBuffer->NonExistingFrameMarking();

				do
				{
					m_DecodedPictureBuffer->OutputFrameBumpingNonExisting(&decoded_picture);

					if (0 != decoded_picture)
					{
						if (m_SequenceParameterSet->frame_cropping_flag)
						{
						 	decoded_picture->SetCroppingInformation(
								m_SequenceParameterSet->frame_cropping_rect_left_offset,
								m_SequenceParameterSet->frame_cropping_rect_right_offset,
								m_SequenceParameterSet->frame_cropping_rect_top_offset,
								m_SequenceParameterSet->frame_cropping_rect_bottom_offset);
							/*
							m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].crop(decoded_picture,
								m_SequenceParameterSet->frame_cropping_rect_left_offset,
								m_SequenceParameterSet->frame_cropping_rect_right_offset,
								m_SequenceParameterSet->frame_cropping_rect_top_offset,
								m_SequenceParameterSet->frame_cropping_rect_bottom_offset);

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

				m_DecodedPictureBuffer->NonExistingFrameStorage();

				number_of_non_existing_frames--;
			}
		}
		else
		{
			assert(false);
		}
	}

	if (0 == m_VLDCurrentPicture) 
	{
		assert(false);
		return false;
	}

	m_DecodedPictureBuffer->SetDecodingPictureParameters(m_FrameNumber,  m_PictureOrderCount, m_TopFieldOrderCount, m_BottomFieldOrderCount);

    int hsize = 16 * m_SequenceParameterSet->frame_width_in_mbs;
    int vsize = 16 * m_SequenceParameterSet->frame_height_in_mbs;



    // Allocate new buffer IF REQD
	m_VLDCurrentPicture->set_size(hsize, vsize,m_MaxDecFrameBuffering);

    // update the DMA Frame table
    {        
        // get instance of Aurora Class
//	    AuroraH264Dec &simdu = AuroraH264Dec::GetInstance();

        // setup the DMA table for the current frame
        // note the the array position for the current picture has been 
        // calculated earlier. refer GetPictureForDecoding()
        Simdu->SetDMAFrameTableEntry( m_DecodedPictureBuffer->GetArrayPositionOfCurrentPicture(),
                                m_VLDCurrentPicture->m_y,
                                m_VLDCurrentPicture->m_u,
                                m_VLDCurrentPicture->m_v,
                                m_VLDCurrentPicture->m_hsize,
                                m_VLDCurrentPicture->m_vsize );
    }

	if (H264_CODED_SLICE_IDR == m_NALUnitType)
	{
		if (!m_VLDSliceHeaderFirstSlice)
		{
			if (m_VLDSliceHeaderPreviousSliceWasIDR)
			{
				m_POCAccumulator += m_VLDSliceHeaderPreviousSlicePictureOrderCount     + m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle;
			}
			else
			{
				m_POCAccumulator += m_VLDSliceHeaderPreviousSlicePictureOrderCount + 1 + m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle;
			}
		}
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeLostReferencePictureConcealment		*/
/* DESCRIPTION:	This is called from VLDDecodeStartOfPicture when loss of 
				reference frames due to packet loss, rather than the gaps 
				in frame number process, has been detected.
				The following is done for each lost frame.
					- The value of POC is estimated.
					- The picture is set to be a copy of the previous 
						reference picture.
					- Zero motion vectors are stored (if not baseline).
					- The picture is stored for reference.
					- The picture is output.								*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::VLDDecodeLostReferencePictureConcealment(void)
{
	int number_of_lost_frames;
	unsigned int lost_picture_frame_number;
	int i;

	// Not sure how to support loss when field coding is active
	assert(m_SequenceParameterSet->frame_mbs_only_flag);

	///////////////////////////////////////////////////////////////////////
	// Calculate number of lost frames and the frame number of the first 
	// lost frame minus one: it is incremented before it is first used.
	///////////////////////////////////////////////////////////////////////

	if (0 == m_VLDSliceHeaderPreviousSliceNALReferenceIDC)
	{
		// Previous was not a reference picture
		number_of_lost_frames = (m_FrameNumber + m_SequenceParameterSet->maximum_frame_number
			- m_VLDSliceHeaderPreviousSliceFrameNumber) & (m_SequenceParameterSet->maximum_frame_number - 1);

		// Decrease by one if not previous not reference. Don't worry about wrap.
		lost_picture_frame_number = m_VLDSliceHeaderPreviousSliceFrameNumber - 1;
	}
	else
	{
		// Previous was a reference picture
		number_of_lost_frames = (m_FrameNumber + m_SequenceParameterSet->maximum_frame_number - 1
			- m_VLDSliceHeaderPreviousSliceFrameNumber) & (m_SequenceParameterSet->maximum_frame_number - 1);

		lost_picture_frame_number = m_VLDSliceHeaderPreviousSliceFrameNumber;
	}

	m_DecodedPictureBuffer->SetSlidingWindowBufferingMode(true);

	///////////////////////////////////////////////////////////////////////
	// Loop over each lost frame.
	///////////////////////////////////////////////////////////////////////

	for (i=0; i<number_of_lost_frames; i++)
	{
		int picture_order_count;
		int top_field_order_count;
		int bottom_field_order_count;
		int decoded_picture_array_position;
		bool mmco_reset;

		m_DecodedPictureBuffer->GetPictureForDecoding(&m_VLDCurrentPicture, H264_FRAME_PICTURE);

		assert(0 != m_VLDCurrentPicture);

		lost_picture_frame_number
			= (lost_picture_frame_number + 1) & (m_SequenceParameterSet->maximum_frame_number - 1);

		///////////////////////////////////////////////////////////////////////
		// Estimate a value for Picture Order Count
		///////////////////////////////////////////////////////////////////////

		if (0 == m_SequenceParameterSet->pic_order_cnt_type)
		{
			if (0 == m_NALReferenceIDC)
			{
				// Current picture is a non reference picture
				// Assume last lost reference has POC greater than current, and others are 
				// evenly spaced between this last lost POC and the last reference POC
				if (i == (number_of_lost_frames - 1))
				{
					picture_order_count = m_PictureOrderCount + 1;
				}
				else
				{
					int delta_reference_poc = 
						((m_PictureOrderCount + 1) - m_VLDSliceHeaderPreviousReferenceSlicePOC + 
								number_of_lost_frames - 1) / number_of_lost_frames;

					picture_order_count = (m_PictureOrderCount + 1) - (delta_reference_poc * (number_of_lost_frames - 1));
				}
			}
			else
			{
				// Current picture is a reference picture
				// Evenly spaced between the current (reference) POC and the last reference POC
				int delta_reference_poc = 
					(m_PictureOrderCount - m_VLDSliceHeaderPreviousReferenceSlicePOC + 
							number_of_lost_frames) / (number_of_lost_frames + 1);

				picture_order_count = m_PictureOrderCount - (delta_reference_poc * (number_of_lost_frames - 1));
			}

			top_field_order_count = picture_order_count;
			bottom_field_order_count = picture_order_count;
		}
		else if (1 == m_SequenceParameterSet->pic_order_cnt_type)
		{
			// Assume delta_pic_order_always_zero_flag is zero

			// Calculate the absolute frame number by adding the LSBs in m_FrameNumber to the 
			// MSBs in m_VLDSliceHeaderPreviousFrameNumberOffset
			int expected_pic_order_cnt = 0;
			if (0 != m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle)
			{
				unsigned int absolute_frame_number;
				int pic_order_cnt_cycle_cnt = 0;
				int frame_num_in_pic_order_cnt_cycle = 0;

				absolute_frame_number = m_VLDSliceHeaderPreviousFrameNumberOffset + lost_picture_frame_number;

				if (lost_picture_frame_number > m_FrameNumber)
				{
					// frame number has wrapped between this lost frame and the current one
					// m_VLDSliceHeaderPreviousFrameNumberOffset has been updated, so allow for this
					absolute_frame_number -= m_SequenceParameterSet->maximum_frame_number;
				}

				// Derive pic_order_cnt_cycle_cnt and frame_num_in_pic_order_cnt_cycle
				if (absolute_frame_number > 0)
				{
					int i2;

					pic_order_cnt_cycle_cnt = (absolute_frame_number - 1) /
								m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle;

					frame_num_in_pic_order_cnt_cycle = (absolute_frame_number - 1) %
								m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle;

					// expected_delta_per_pic_order_cnt_cycle has been derived and stored in the 
					// sequence parameter set

					// Calculate expected_pic_order_cnt

					expected_pic_order_cnt = pic_order_cnt_cycle_cnt *
							m_SequenceParameterSet->expected_delta_per_pic_order_cnt_cycle;

					for (i2 = 0; i2 <= frame_num_in_pic_order_cnt_cycle; i2++)
					{
						expected_pic_order_cnt += m_SequenceParameterSet->offset_for_ref_frame[i2];
					}
				}
			}

#if defined __ARC_MAINPROFILE__
			if (!m_FieldPictureFlag)
			{
#endif
				top_field_order_count = expected_pic_order_cnt;
				bottom_field_order_count = top_field_order_count +
										m_SequenceParameterSet->offset_for_top_to_bottom_field;

				picture_order_count = MIN(top_field_order_count, bottom_field_order_count);
#if defined __ARC_MAINPROFILE__
			}
			else if (!m_BottomFieldFlag)
			{
				top_field_order_count = expected_pic_order_cnt;
				picture_order_count = top_field_order_count;
			}
			else
			{
				bottom_field_order_count = top_field_order_count +
										m_SequenceParameterSet->offset_for_top_to_bottom_field;
				picture_order_count = bottom_field_order_count;
			}
#endif
		}
		else
		{
			// POC type 2
			picture_order_count = (m_VLDSliceHeaderPreviousFrameNumberOffset + lost_picture_frame_number) << 1;

			if (lost_picture_frame_number > m_FrameNumber)
			{
				// frame number has wrapped between this lost frame and the current one
				// m_VLDSliceHeaderPreviousFrameNumberOffset has been updated, so allow for this
				picture_order_count -= (m_SequenceParameterSet->maximum_frame_number << 1);
			}

			top_field_order_count = picture_order_count;
			bottom_field_order_count = picture_order_count;
		}

		m_DecodedPictureBuffer->SetDecodingPictureParameters(lost_picture_frame_number,
							picture_order_count, top_field_order_count, bottom_field_order_count);

		// "Decode" the lost frame by decoding it as a copy of the most recently decoded reference picture
		m_VLDCurrentPicture->copy(m_DecodedPictureBuffer->GetMostRecentReferencePicture());
#ifdef __B_PICTURES__
		if (!m_SequenceParameterSet->baseline_compatibility_flag)
		{
			m_DecodedPictureBuffer->StoreConcealmentMotionVectors();
		}
#endif // def __B_PICTURES__

		// Need to ensure that this picture is not stored as an IDR
		m_DecodedPictureBuffer->SetIDRParameters(false, false, false);

		decoded_picture_array_position
			= m_DecodedPictureBuffer->StorePicture(true, &mmco_reset);  // but ignore mmco_reset

		VLDOutputPicture();
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeAllocateSliceGroups					*/
/* DESCRIPTION:	This associates a slice group ID with each macroblock in 
				the picture. The allocation is done as in the picture 
				parameter set.												*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeAllocateSliceGroups(void)
{
	h264_decoder_control_t * control = m_DecoderControl.FirstCtrl();

	if (0 == m_PictureParameterSet) 
	{
		assert(false);
		return false;
	}

	// If only one slice group, then no allocation is necessary as the reset
	// value of the control list element is for slice_group to be zero
	if (1 == m_PictureParameterSet->num_slice_groups)
	{
		return true; 
	}

	if (0 == control) 
	{
		assert(false);
		return false;
	}

	if (0 == m_PictureParameterSet->slice_group_map_type)
	{
		int slice_group = 0;
		int macroblocks_left_in_slice_group = m_PictureParameterSet->slice_group_run_length[slice_group];

		// Loop over macroblocks
		while (0 != control)
		{
			control->slice_group = slice_group;

			macroblocks_left_in_slice_group--;

			if (0 == macroblocks_left_in_slice_group)
			{
				slice_group++;

				if (slice_group == m_PictureParameterSet->num_slice_groups) slice_group = 0;

				macroblocks_left_in_slice_group = m_PictureParameterSet->slice_group_run_length[slice_group];
			}

			control = control->next;
		}
	}
	else if (1 == m_PictureParameterSet->slice_group_map_type)
	{
		// Loop over macroblocks
		while (0 != control)
		{
			control->slice_group = ((control->PixelX>>4) + 
				(((control->PixelY>>4) * m_PictureParameterSet->num_slice_groups) >> 1))
					% m_PictureParameterSet->num_slice_groups;

			control = control->next;
		}
	}
	else if (2 == m_PictureParameterSet->slice_group_map_type)
	{
		// Foreground and left over slice groups.

		// First assign all macroblocks to the background slice group, then proceed 
		// through foreground slice groups from largest numbered to smallest numbered, 
		// assigning macroblocks to slice groups. A macroblock is assigned to the 
		// smallest slice group if it is in more than one foreground area.
		int slice_group;

		for (slice_group = m_PictureParameterSet->num_slice_groups-1; slice_group >= 0; slice_group--)
		{
			int top_left_x;
			int top_left_y;
			int bottom_right_x;
			int bottom_right_y;

			if (slice_group != m_PictureParameterSet->num_slice_groups-1)
			{
				// A foreground slice group
				top_left_y = m_PictureParameterSet->slice_group_top_left[slice_group] / m_DecoderControl.m_MacroblocksPerRow;
				top_left_x = m_PictureParameterSet->slice_group_top_left[slice_group] % m_DecoderControl.m_MacroblocksPerRow;
				bottom_right_y = m_PictureParameterSet->slice_group_bottom_right[slice_group] / m_DecoderControl.m_MacroblocksPerRow;
				bottom_right_x = m_PictureParameterSet->slice_group_bottom_right[slice_group] % m_DecoderControl.m_MacroblocksPerRow;
			}

			// Loop over macroblocks
			control = m_DecoderControl.FirstCtrl();
			while (0 != control)
			{
				if (slice_group == m_PictureParameterSet->num_slice_groups-1)
				{
					// Background slice group
					control->slice_group = slice_group;
				}
				else
				{
					// Foreground slice
					if (((control->PixelX>>4) >= top_left_x) && ((control->PixelX>>4) <= bottom_right_x) &&
						((control->PixelY>>4) >= top_left_y) && ((control->PixelY>>4) <= bottom_right_y))
					{
						// Macroblock is in this foreground area
						control->slice_group = slice_group;
					}
				}

				control = control->next;
			}
		}
	}
	else if (3 == m_PictureParameterSet->slice_group_map_type)
	{
		// Box out slice groups.

		int picture_width_in_macroblocks = m_DecoderControl.m_MacroblocksPerRow;
		int picture_height_in_macroblocks
			= m_DecoderControl.m_NumberOfMacroblocks / m_DecoderControl.m_MacroblocksPerRow;

		int x = picture_width_in_macroblocks;
		int y = picture_height_in_macroblocks;

		int left_bound, top_bound, right_bound, bottom_bound;
		int x_direction, y_direction;
		int map_unit_vacant;
		int mb_count;

		// First assign all macroblocks to the 'background' slice group, then proceed 
		// allocate the required number of macroblocks to the 'foreground' box according 
		// to the standard algorithm, starting at the centre of the picture and circling 
		// outwards anti-clockwise or clockwise according to the direction flag.
		
		// Assign all macroblocks to the 'background' slice group
		UNSIGNED_SEPTET *slice_group_map = new UNSIGNED_SEPTET[m_DecoderControl.m_NumberOfMacroblocks];

		if (0 == slice_group_map) 
		{
			assert(false);
			return false;
		}
		
		memset(slice_group_map, 1, m_DecoderControl.m_NumberOfMacroblocks*sizeof(UNSIGNED_SEPTET));

		x = (x - m_PictureParameterSet->slice_group_change_direction_flag) >> 1;
		y = (y - m_PictureParameterSet->slice_group_change_direction_flag) >> 1;

		left_bound = x;
		top_bound = y;
		right_bound = x;
		bottom_bound = y;

		x_direction = m_PictureParameterSet->slice_group_change_direction_flag - 1;
		y_direction = m_PictureParameterSet->slice_group_change_direction_flag;

		for (mb_count=0;
			 mb_count < m_MapUnitsInSliceGroup0;
			 mb_count += map_unit_vacant)
		{
			int mb_address = y * picture_width_in_macroblocks + x;

			if (1 == slice_group_map[mb_address])
			{
				map_unit_vacant = 1;
				slice_group_map[mb_address] = 0;
			}
			else
			{
				map_unit_vacant = 0;
			}

			if ((-1 == x_direction) && (x == left_bound))
			{
				left_bound = MAX(left_bound - 1, 0);
				x = left_bound;
				x_direction = 0;
				y_direction = 2 * m_PictureParameterSet->slice_group_change_direction_flag - 1;
			}
			else if ((1 == x_direction) && (x == right_bound))
			{
				right_bound = MIN(right_bound + 1, picture_width_in_macroblocks - 1);
				x = right_bound;
				x_direction = 0;
				y_direction = 1 - 2 * m_PictureParameterSet->slice_group_change_direction_flag;
			}
			else if ((-1 == y_direction) && (y == top_bound)) 
			{
				top_bound = MAX(top_bound - 1, 0);
				y = top_bound;
				x_direction = 1 - 2 * m_PictureParameterSet->slice_group_change_direction_flag;
				y_direction = 0;
			}
			else if ((1 == y_direction) && (y == bottom_bound))
			{
				bottom_bound = MIN( bottom_bound + 1, picture_height_in_macroblocks - 1);
				y = bottom_bound;
				x_direction = 2 * m_PictureParameterSet->slice_group_change_direction_flag - 1;
				y_direction = 0;
			}
			else
			{
				x += x_direction;
				y += y_direction;
			}
		}

		// All macroblocks are allocated to slice groups, now copy from the slice_group_map 
		// array into the control list
		
		control = m_DecoderControl.FirstCtrl();
		while (0 != control)
		{
			control->slice_group = slice_group_map[control->mb_number];

			control = control->next;
		}

		delete slice_group_map;
	}
	else if (4 == m_PictureParameterSet->slice_group_map_type)
	{
		// Raster scan slice groups.

		// Calculate the number of macroblocks in the upper left slice group, then loop
		// over all the macroblocks and set according to the count and the direction flag.

		int size_of_upper_left_slice_group;

		if (0 != m_PictureParameterSet->slice_group_change_direction_flag)
		{
			size_of_upper_left_slice_group =
				m_DecoderControl.m_NumberOfMacroblocks - m_MapUnitsInSliceGroup0;
		}
		else
		{
			size_of_upper_left_slice_group = m_MapUnitsInSliceGroup0;
		}

		control = m_DecoderControl.FirstCtrl();
		while (0 != control)
		{
			if (control->mb_number < size_of_upper_left_slice_group)
			{
				control->slice_group = m_PictureParameterSet->slice_group_change_direction_flag;
			}
			else
			{
				control->slice_group = 1 - m_PictureParameterSet->slice_group_change_direction_flag;
			}

			control = control->next;
		}
	}
	else if (5 == m_PictureParameterSet->slice_group_map_type)
	{
		// Wipe slice groups.

		// Calculate the number of macroblocks in the upper left slice group, then loop
		// over all the macroblocks and set according to the count and the direction flag.

		int size_of_upper_left_slice_group;
		int macroblocks_per_column =
			m_DecoderControl.m_NumberOfMacroblocks / m_DecoderControl.m_MacroblocksPerRow;

		if (0 != m_PictureParameterSet->slice_group_change_direction_flag)
		{
			size_of_upper_left_slice_group =
				m_DecoderControl.m_NumberOfMacroblocks - m_MapUnitsInSliceGroup0;
		}
		else
		{
			size_of_upper_left_slice_group = m_MapUnitsInSliceGroup0;
		}

		control = m_DecoderControl.FirstCtrl();

		while (0 != control)
		{
			int wipe_macroblock_number = (control->PixelY>>4) + ((control->PixelX>>4) * macroblocks_per_column);
			if (wipe_macroblock_number < size_of_upper_left_slice_group)
			{
				control->slice_group = m_PictureParameterSet->slice_group_change_direction_flag;
			}
			else
			{
				control->slice_group = 1 - m_PictureParameterSet->slice_group_change_direction_flag;
			}

			control = control->next;
		}
	}
	else if (6 == m_PictureParameterSet->slice_group_map_type)
	{
		// Explicit slice groups.
		control = m_DecoderControl.FirstCtrl();

		while (0 != control)
		{
			control->slice_group = m_PictureParameterSet->slice_group_id[control->mb_number];

			control = control->next;
		}
	}
	else
	{
		assert(false);
		return false;
	}

	return true;
}
/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
