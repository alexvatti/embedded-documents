/* ------------------------------------------------------------------------ */
/* FILE:			H264VLDVui.cpp											*/
/* DESCRIPTION:		H.264 VUI variable length decoder class					*/
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

#include "cavlc_api.h"
#include "vlc_api.h"

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_sar_width/height_tables						*/
/* DESCRIPTION:	The table for aspect_ratio_idc to sample aspect ratio.		*/
/* ------------------------------------------------------------------------ */

const unsigned char h264_decoder_sar_width_table[14] =
	{  1,  1, 12, 10, 16, 40, 24, 20, 32, 80, 18, 15, 64, 160 };

const unsigned char h264_decoder_sar_height_table[14] =
	{  1,  1, 11, 11, 11, 33, 11, 11, 11, 33, 11, 11, 3, 99 };

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeVUI									*/
/* DESCRIPTION:	Decodes a sequence parameter set VUI information.			*/
/* RETURNS:		True if OK, else false.										*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDDecodeVUI(void)
{
	bool error = false;
	bool no_bits_left = false;
	bool finished_parsing = false;
	int byteCode;

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
			case H264_VLD_VUI_ASPECT_RATIO_INFO_PRESENT :
				if (0 == m_SequenceParameterSet->vui_parameters)
				{
					m_SequenceParameterSet->vui_parameters = new h264_vui_parameters;

					assert(0 != m_SequenceParameterSet->vui_parameters);
				}

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
    				m_SequenceParameterSet->vui_parameters->aspect_ratio_info_present_flag = true;
					m_VLDState = H264_VLD_VUI_ASPECT_RATIO_IDC;
				}
				else
				{
    				m_SequenceParameterSet->vui_parameters->aspect_ratio_info_present_flag = false;
					// Our default - none in standard
					m_SequenceParameterSet->vui_parameters->sar_width = 1;
					m_SequenceParameterSet->vui_parameters->sar_height = 1;

					m_VLDState = H264_VLD_VUI_OVERSCAN_INFO_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->aspect_ratio_info_present_flag = (1 == byteCode);

				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_ASPECT_RATIO_IDC;
				}
				else
				{
					// Our default - none in standard
					m_SequenceParameterSet->vui_parameters->sar_width = 1;
					m_SequenceParameterSet->vui_parameters->sar_height = 1;

					m_VLDState = H264_VLD_VUI_OVERSCAN_INFO_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_ASPECT_RATIO_IDC :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                byteCode = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
#endif

				m_SequenceParameterSet->vui_parameters->aspect_ratio_idc = byteCode;

				if (255 == byteCode)  // 255==Extended_SAR
				{
					m_VLDState = H264_VLD_VUI_SAR_WIDTH;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->sar_width = h264_decoder_sar_width_table[byteCode];
					m_SequenceParameterSet->vui_parameters->sar_height = h264_decoder_sar_height_table[byteCode];

					m_VLDState = H264_VLD_VUI_OVERSCAN_INFO_PRESENT;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_SAR_WIDTH :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->sar_width = VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->sar_width = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_SAR_HEIGHT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_SAR_HEIGHT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->sar_height = VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->sar_height = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_OVERSCAN_INFO_PRESENT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_OVERSCAN_INFO_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->overscan_info_present_flag = true;
					m_VLDState = H264_VLD_VUI_OVERSCAN_FLAG;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->overscan_info_present_flag = false;
					m_SequenceParameterSet->vui_parameters->overscan_appropriate_flag = false; // Our default - none in standard
					m_VLDState = H264_VLD_VUI_VIDEO_SIGNAL_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->overscan_info_present_flag = (1 == byteCode);

				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_OVERSCAN_FLAG;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->overscan_appropriate_flag = false; // Our default - none in standard
					m_VLDState = H264_VLD_VUI_VIDEO_SIGNAL_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_OVERSCAN_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->overscan_appropriate_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->overscan_appropriate_flag = (1 == byteCode);
#endif
				m_VLDState = H264_VLD_VUI_VIDEO_SIGNAL_PRESENT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_VIDEO_SIGNAL_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->video_signal_type_present_flag = true;
					m_VLDState = H264_VLD_VUI_VIDEO_FORMAT;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->video_signal_type_present_flag = false;
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->video_format = 5;
					m_SequenceParameterSet->vui_parameters->video_full_range_flag = 0;
					m_SequenceParameterSet->vui_parameters->colour_primaries = 2;
					m_SequenceParameterSet->vui_parameters->transfer_characteristics = 2;
					m_SequenceParameterSet->vui_parameters->matrix_coefficients = 2;

					m_VLDState = H264_VLD_VUI_CHROMA_LOC_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }

				m_SequenceParameterSet->vui_parameters->video_signal_type_present_flag = (1 == byteCode);

				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_VIDEO_FORMAT;
				}
				else
				{
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->video_format = 5;
					m_SequenceParameterSet->vui_parameters->video_full_range_flag = 0;
					m_SequenceParameterSet->vui_parameters->colour_primaries = 2;
					m_SequenceParameterSet->vui_parameters->transfer_characteristics = 2;
					m_SequenceParameterSet->vui_parameters->matrix_coefficients = 2;

					m_VLDState = H264_VLD_VUI_CHROMA_LOC_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_VIDEO_FORMAT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->video_format = VLC_Get_Bits(3);
#else
				if (!ByteBufferDecodeFixedLengthCode(3, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->video_format = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_VIDEO_FULL_RANGE;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_VIDEO_FULL_RANGE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->video_full_range_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->video_full_range_flag = (1 == byteCode);
#endif

				m_VLDState = H264_VLD_VUI_COLOUR_DESC_FLAG;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_COLOUR_DESC_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->colour_description_present_flag = true;
					m_VLDState = H264_VLD_VUI_COLOUR_PRIMARIES;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->colour_description_present_flag = false;
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->colour_primaries = 2;
					m_SequenceParameterSet->vui_parameters->transfer_characteristics = 2;
					m_SequenceParameterSet->vui_parameters->matrix_coefficients = 2;

					m_VLDState = H264_VLD_VUI_CHROMA_LOC_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }

				m_SequenceParameterSet->vui_parameters->colour_description_present_flag = (1 == byteCode);

				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_COLOUR_PRIMARIES;
				}
				else
				{
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->colour_primaries = 2;
					m_SequenceParameterSet->vui_parameters->transfer_characteristics = 2;
					m_SequenceParameterSet->vui_parameters->matrix_coefficients = 2;

					m_VLDState = H264_VLD_VUI_CHROMA_LOC_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_COLOUR_PRIMARIES :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->colour_primaries = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->colour_primaries = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_TRANSFER_CHAR;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_TRANSFER_CHAR :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->transfer_characteristics = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->transfer_characteristics = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_MATRIX_COEFFICIENTS;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_MATRIX_COEFFICIENTS :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->matrix_coefficients = VLC_Get_Bits(8);
#else
				if (!ByteBufferDecodeFixedLengthCode(8, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->matrix_coefficients = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_CHROMA_LOC_PRESENT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CHROMA_LOC_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->chroma_loc_info_present_flag = true;
					m_VLDState = H264_VLD_VUI_CHROMA_LOC_TOP_FIELD;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->chroma_loc_info_present_flag = false;
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_top_field = 0;
					m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_bottom_field = 0;

					m_VLDState = H264_VLD_VUI_TIMING_INFO_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->chroma_loc_info_present_flag = (1 == byteCode);
				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_CHROMA_LOC_TOP_FIELD;
				}
				else
				{
					// Set defaults as in the standard
					m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_top_field = 0;
					m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_bottom_field = 0;

					m_VLDState = H264_VLD_VUI_TIMING_INFO_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CHROMA_LOC_TOP_FIELD :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE);
				m_VLDState = H264_VLD_VUI_CHROMA_LOC_BOTTOM_FIELD;
				m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_top_field = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_top_field = byteCode;
				m_VLDState = H264_VLD_VUI_CHROMA_LOC_BOTTOM_FIELD;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CHROMA_LOC_BOTTOM_FIELD :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE);
				m_VLDState = H264_VLD_VUI_TIMING_INFO_PRESENT;
				m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_bottom_field = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->chroma_sample_loc_type_bottom_field = byteCode;
				m_VLDState = H264_VLD_VUI_TIMING_INFO_PRESENT;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_TIMING_INFO_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->timing_info_present_flag = true;
					m_VLDState = H264_VLD_VUI_NUM_UNITS_IN_TICK_MS16;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->timing_info_present_flag = false;
					// Set defaults - none in standards
					m_SequenceParameterSet->vui_parameters->num_units_in_tick = 0;
					m_SequenceParameterSet->vui_parameters->time_scale = 0;
					m_SequenceParameterSet->vui_parameters->fixed_frame_rate_flag = false;

					m_VLDState = H264_VLD_VUI_NAL_HRD_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->timing_info_present_flag = (1 == byteCode);
				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_NUM_UNITS_IN_TICK_MS16;
				}
				else
				{
					// Set defaults - none in standards
					m_SequenceParameterSet->vui_parameters->num_units_in_tick = 0;
					m_SequenceParameterSet->vui_parameters->time_scale = 0;
					m_SequenceParameterSet->vui_parameters->fixed_frame_rate_flag = false;

					m_VLDState = H264_VLD_VUI_NAL_HRD_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_NUM_UNITS_IN_TICK_MS16 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->num_units_in_tick = VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->num_units_in_tick = byteCode;
#endif

				m_VLDState = H264_VLD_VUI_NUM_UNITS_IN_TICK_LS16;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_NUM_UNITS_IN_TICK_LS16 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->num_units_in_tick = 
					(m_SequenceParameterSet->vui_parameters->num_units_in_tick << 16) + VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->num_units_in_tick = 
					(m_SequenceParameterSet->vui_parameters->num_units_in_tick << 16) + byteCode;
#endif
				m_VLDState = H264_VLD_VUI_TIME_SCALE_MS16;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_TIME_SCALE_MS16 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->time_scale = VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->time_scale = byteCode;
#endif
				m_VLDState = H264_VLD_VUI_TIME_SCALE_LS16;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_TIME_SCALE_LS16 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->time_scale = 
					(m_SequenceParameterSet->vui_parameters->time_scale << 16) + VLC_Get_Bits(16);
#else
				if (!ByteBufferDecodeFixedLengthCode(16, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->time_scale = 
					(m_SequenceParameterSet->vui_parameters->time_scale << 16) + byteCode;
#endif
				m_VLDState = H264_VLD_VUI_FIXED_FRAME_RATE_FLAG;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_FIXED_FRAME_RATE_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->fixed_frame_rate_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->fixed_frame_rate_flag = (1 == byteCode);
#endif
				m_VLDState = H264_VLD_VUI_NAL_HRD_PRESENT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_NAL_HRD_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->nal_hrd_parameters_present_flag = true;
					m_NALHRDParameters = true;  // Indicate NAL parameters
					m_VLDState = H264_VLD_VUI_CBP_CNT_MINUS1;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->nal_hrd_parameters_present_flag = false;
					m_NALHRDParameters = false;
					m_VLDState = H264_VLD_VUI_VCL_HRD_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->nal_hrd_parameters_present_flag = (1 == byteCode);
				if (1 == byteCode)
				{
					m_NALHRDParameters = true;  // Indicate NAL parameters
					m_VLDState = H264_VLD_VUI_CBP_CNT_MINUS1;
				}
				else
				{
					m_NALHRDParameters = false;
					m_VLDState = H264_VLD_VUI_VCL_HRD_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_VCL_HRD_PRESENT :
				m_SequenceParameterSet->vui_parameters->low_delay_hrd_flag = 0; // Our default - none in standard

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->vcl_hrd_parameters_present_flag = true;
					m_NALHRDParameters = false;  // Indicate VCL parameters
					m_VLDState = H264_VLD_VUI_CBP_CNT_MINUS1;
				}
				else if (m_NALHRDParameters)
				{
				    m_SequenceParameterSet->vui_parameters->vcl_hrd_parameters_present_flag = false;
					m_VLDState = H264_VLD_VUI_LOW_DELAY_HRD_FLAG;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->vcl_hrd_parameters_present_flag = false;
					m_VLDState = H264_VLD_VUI_PIC_STRUCT_PRESENT;
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->vcl_hrd_parameters_present_flag = (1 == byteCode);
				if (1 == byteCode)
				{
					m_NALHRDParameters = false;  // Indicate VCL parameters
					m_VLDState = H264_VLD_VUI_CBP_CNT_MINUS1;
				}
				else if (m_NALHRDParameters)
				{
					m_VLDState = H264_VLD_VUI_LOW_DELAY_HRD_FLAG;
				}
				else
				{
					m_VLDState = H264_VLD_VUI_PIC_STRUCT_PRESENT;
				}
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_LOW_DELAY_HRD_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->low_delay_hrd_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->low_delay_hrd_flag = (1 == byteCode);
#endif
				m_VLDState = H264_VLD_VUI_PIC_STRUCT_PRESENT;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_PIC_STRUCT_PRESENT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->pic_struct_present_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->pic_struct_present_flag = (1 == byteCode);
#endif
				m_VLDState = H264_VLD_VUI_BITSTREAM_RESTRICTION;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_BITSTREAM_RESTRICTION :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (1 == VLC_Get_Bit())
				{
				    m_SequenceParameterSet->vui_parameters->bitstream_restriction_flag = true;
					m_VLDState = H264_VLD_VUI_MV_OFF_PICTURE;
				}
				else
				{
				    m_SequenceParameterSet->vui_parameters->bitstream_restriction_flag = false;
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->bitstream_restriction_flag = (1 == byteCode);
				if (1 == byteCode)
				{
					m_VLDState = H264_VLD_VUI_MV_OFF_PICTURE;
				}
				else
				{
#endif
					// Set defaults
					m_SequenceParameterSet->vui_parameters->motion_vectors_over_pic_boundaries_flag = true;
					m_SequenceParameterSet->vui_parameters->max_bytes_per_pic_denom = 2;
					m_SequenceParameterSet->vui_parameters->max_bits_per_mb_denom = 1;
					m_SequenceParameterSet->vui_parameters->log2_max_mv_length_horizontal = 16;
					m_SequenceParameterSet->vui_parameters->log2_max_mv_length_vertical = 16;

					m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering = 
						annex_a_level_table[m_SequenceParameterSet->level_index][3] /
							(384 * m_SequenceParameterSet->frame_width_in_mbs * 
								m_SequenceParameterSet->frame_height_in_mbs);

					if (m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering > 16)
					{
						m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering = 16;
					}

					m_SequenceParameterSet->vui_parameters->num_reorder_frames
						= m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering;

					finished_parsing = true;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_MV_OFF_PICTURE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				m_SequenceParameterSet->vui_parameters->motion_vectors_over_pic_boundaries_flag = VLC_Get_Bit();
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->motion_vectors_over_pic_boundaries_flag = (1 == byteCode);
#endif
				m_VLDState = H264_VLD_VUI_MAX_BYTES_PER_PIC_DENOM;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_MAX_BYTES_PER_PIC_DENOM :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_MAX_BITS_PER_MB_DENOM;
				m_SequenceParameterSet->vui_parameters->max_bytes_per_pic_denom = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->max_bytes_per_pic_denom = byteCode;
				m_VLDState = H264_VLD_VUI_MAX_BITS_PER_MB_DENOM;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_MAX_BITS_PER_MB_DENOM :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_LOG2_MAX_MV_HORIZ;
				m_SequenceParameterSet->vui_parameters->max_bits_per_mb_denom = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->max_bits_per_mb_denom = byteCode;
				m_VLDState = H264_VLD_VUI_LOG2_MAX_MV_HORIZ;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_LOG2_MAX_MV_HORIZ :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_LOG2_MAX_MV_VERT;
				m_SequenceParameterSet->vui_parameters->log2_max_mv_length_horizontal = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->log2_max_mv_length_horizontal = byteCode;
				m_VLDState = H264_VLD_VUI_LOG2_MAX_MV_VERT;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_LOG2_MAX_MV_VERT :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_NUM_REORDER_FRAMES;
				m_SequenceParameterSet->vui_parameters->log2_max_mv_length_vertical = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->log2_max_mv_length_vertical = byteCode;
				m_VLDState = H264_VLD_VUI_NUM_REORDER_FRAMES;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_NUM_REORDER_FRAMES :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_MAX_DEC_FRAME_BUFFERING;
				m_SequenceParameterSet->vui_parameters->num_reorder_frames = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->num_reorder_frames = byteCode;
				m_VLDState = H264_VLD_VUI_MAX_DEC_FRAME_BUFFERING;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_MAX_DEC_FRAME_BUFFERING :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				finished_parsing = true;
				m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering = CAVLC_Read_Symbol();
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				m_SequenceParameterSet->vui_parameters->max_dec_frame_buffering = byteCode;
				finished_parsing = true;
#endif
				break;

			// --------------------------------------------------------------
			// HRD Parameters parsing
			//
			// m_NALHRDParameters indicates NAL parameters if true
			// else VCL parameters
			//
			// --------------------------------------------------------------

			case H264_VLD_VUI_CBP_CNT_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDStateCount = 0;
				m_VLDState = H264_VLD_VUI_BIT_RATE_SCALE;
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_cnt_minus1 = CAVLC_Read_Symbol();
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_cnt_minus1 = CAVLC_Read_Symbol();
				}
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_cnt_minus1 = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_cnt_minus1 = byteCode;
				}
				m_VLDStateCount = 0;
				m_VLDState = H264_VLD_VUI_BIT_RATE_SCALE;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_BIT_RATE_SCALE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_bit_rate_scale = VLC_Get_Bits(4);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_bit_rate_scale = VLC_Get_Bits(4);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(4, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_bit_rate_scale = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_bit_rate_scale = byteCode;
				}
#endif

				m_VLDState = H264_VLD_VUI_CBP_SIZE_SCALE;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CBP_SIZE_SCALE :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_size_scale = VLC_Get_Bits(4);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_size_scale = VLC_Get_Bits(4);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(4, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_size_scale = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_size_scale = byteCode;
				}
#endif

				m_VLDState = H264_VLD_VUI_BIT_RATE_VALUE_MINUS1;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_BIT_RATE_VALUE_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_CBP_SIZE_VALUE_MINUS1;
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_bit_rate_value_minus1[m_VLDStateCount] = CAVLC_Read_Symbol();
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_bit_rate_value_minus1[m_VLDStateCount] = CAVLC_Read_Symbol();
				}
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_bit_rate_value_minus1[m_VLDStateCount] = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_bit_rate_value_minus1[m_VLDStateCount] = byteCode;
				}
				m_VLDState = H264_VLD_VUI_CBP_SIZE_VALUE_MINUS1;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CBP_SIZE_VALUE_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
                UVLC_Set_Control(UVLC_UE); 
				m_VLDState = H264_VLD_VUI_CBR_FLAG;
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_size_value_minus1[m_VLDStateCount] = CAVLC_Read_Symbol();
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_size_value_minus1[m_VLDStateCount] = CAVLC_Read_Symbol();
				}
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_size_value_minus1[m_VLDStateCount] = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_size_value_minus1[m_VLDStateCount] = byteCode;
				}
				m_VLDState = H264_VLD_VUI_CBR_FLAG;
#endif
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CBR_FLAG :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cbr_flag[m_VLDStateCount] = VLC_Get_Bit();
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cbr_flag[m_VLDStateCount] = VLC_Get_Bit();
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(1, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cbr_flag[m_VLDStateCount] = (1 == byteCode);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cbr_flag[m_VLDStateCount] = (1 == byteCode);
				}
#endif
				m_VLDStateCount++;

				if (( m_NALHRDParameters && (m_VLDStateCount > m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_cnt_minus1)) ||
					(!m_NALHRDParameters && (m_VLDStateCount > m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_cnt_minus1)))
				{
					m_VLDState = H264_VLD_VUI_INIT_CPB_DELAY;
				}
				else
				{
					m_VLDState = H264_VLD_VUI_BIT_RATE_VALUE_MINUS1;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_INIT_CPB_DELAY :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_initial_cpb_removal_delay_length_minus1 = VLC_Get_Bits(5);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_initial_cpb_removal_delay_length_minus1 = VLC_Get_Bits(5);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(5, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_initial_cpb_removal_delay_length_minus1 = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_initial_cpb_removal_delay_length_minus1 = byteCode;
				}
#endif
				m_VLDState = H264_VLD_VUI_CPB_REMOVAL_DELAY_MINUS1;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_CPB_REMOVAL_DELAY_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_removal_delay_length_minus1 = VLC_Get_Bits(5);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_removal_delay_length_minus1 = VLC_Get_Bits(5);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(5, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_cpb_removal_delay_length_minus1 = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_cpb_removal_delay_length_minus1 = byteCode;
				}
#endif
				m_VLDState = H264_VLD_VUI_DPB_OUTPUT_DELAY_MINUS1;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_DPB_OUTPUT_DELAY_MINUS1 :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_dpb_output_delay_length_minus1 = VLC_Get_Bits(5);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_dpb_output_delay_length_minus1 = VLC_Get_Bits(5);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(5, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_dpb_output_delay_length_minus1 = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_dpb_output_delay_length_minus1 = byteCode;
				}
#endif
				m_VLDState = H264_VLD_VUI_TIME_OFFSET_LENGTH;
				break;

			// --------------------------------------------------------------
			case H264_VLD_VUI_TIME_OFFSET_LENGTH :
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_time_offset_length = VLC_Get_Bits(5);
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_time_offset_length = VLC_Get_Bits(5);
				}
#else
				if (!ByteBufferDecodeFixedLengthCode(5, byteCode)) { no_bits_left = true; break; }
				if (m_NALHRDParameters)
				{
					m_SequenceParameterSet->vui_parameters->nal_hrd_time_offset_length = byteCode;
				}
				else
				{
					m_SequenceParameterSet->vui_parameters->vcl_hrd_time_offset_length = byteCode;
				}
#endif

				if (m_NALHRDParameters)
				{
					m_VLDState = H264_VLD_VUI_VCL_HRD_PRESENT;
				}
				else
				{
					m_VLDState = H264_VLD_VUI_LOW_DELAY_HRD_FLAG;
				}
				break;

			// --------------------------------------------------------------
			case H264_VLD_END_OF_SPS :
				finished_parsing = true;
				break;

			// --------------------------------------------------------------
			default :
				error = true;
				break;
		}

		if (finished_parsing)
		{
			m_VLDState = H264_VLD_END_OF_SPS;
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
