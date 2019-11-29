/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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


#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "ArcMPC.h"
#include "ArcSDMTables.h"
#include "EnumTypes.h"
#include "H264OffsetTables.h"
#include "VRaptorInfo.h"
#include "SymbolByteBuffer.h"
#include "H264EncoderControl.h"
#include "ArcChannelRoutines.h"
#include "H264Tables.h"
#include "ArcTimer.h"
#ifndef HARDWARE_EE
	#include "ArcEntropyEncoderModel.h"
#endif

#include "ArcMotionEstimationModel.h"


#include "ArcSDMTables.h"
#include "ArcChannelRoutines.h"
#ifdef HARDWARE_EE
#include <arc/arc_avu.h>
#endif
#include "ArcMacroRecordSettings.h"

#ifndef ARC_ENCODER_API
extern FILE *bitstream_file;
#else
#include <arc_encoder_malloc.h>

static CSymbolByteBuffer startCodeBuffer;
#endif

#define m_TraceFile gCH264Encoder.m_TraceFile
unsigned char UUID[16] = {0x1b,0x69,0xa3,0x50,0x13,0x32,0x11,0xdc,0x83,0x14,0x08,0x00,0x20,0x0c,0x9a,0x66};


void Initialise_Entropy_Encoder(unsigned int packer_id)
{
  // Set CODEC_CFG.CODEC_SEL to H264
  WriteToAuxRegEE((VRaptorDevs.EntropyEncoder.AuxBase + 0x3), 0);
  
  // Set the residual data offset MPC_Y_OFFSET
  WriteToAuxRegEE((VRaptorDevs.EntropyEncoder.AuxBase + 0x23),(MPO_PixelCoeffBuffer + PCB_RESIDUAL_Y));

  // Select the packer or concat unit
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R0),packer_id);

  // Enable EMU control
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R16),0x1);

  // Reset the buffer
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R6),0x1);


#ifdef ARC_ENCODER_API
  startCodeBuffer.Create(STARTCODELEN);
#endif
}


// Write the Sequence parameter using channel commands
CHANNEL_SERVICE	Service_WriteSequenceHeader(void)
{
  // Reset EE
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R6,0x1);

  // Set the address to write the packed bits
#ifdef ARC_ENCODER_API
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_PhysBufferPointer);
#else
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer);
#endif

  // Set the SequenceHeader response function address
  unsigned int addr_val = (unsigned int)Service_SequenceHeaderResponse;
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),addr_val);

  sequence_parameter_set_t * sequence_param_set = gCH264Encoder.m_SequenceParameterSet;

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,sequence_param_set->profile_idc);
  WRITE_ENCODED_SYMBOL("profile_idc", sequence_param_set->profile_idc)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->baseline_compatibility_flag);
  WRITE_ENCODED_SYMBOL("baseline_compatibility_flag", sequence_param_set->baseline_compatibility_flag)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->main_compatibility_flag);
  WRITE_ENCODED_SYMBOL("main_compatibility_flag", sequence_param_set->main_compatibility_flag)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->extended_compatibility_flag);
  WRITE_ENCODED_SYMBOL("extended_compatibility_flag", sequence_param_set->extended_compatibility_flag)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_5BIT,0);
  WRITE_ENCODED_SYMBOL("5 reserved zero bits", 0)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,sequence_param_set->level_idc);
  WRITE_ENCODED_SYMBOL("level_idc", sequence_param_set->level_idc)


  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,gCH264Encoder.m_SequenceParameterSetID);
  WRITE_ENCODED_SYMBOL("SequenceParameterSetID", gCH264Encoder.m_SequenceParameterSetID)

  // Check consistency of log2_max_frame_num_minus4 from maximum_frame_number
  if ((unsigned int)(16 << sequence_param_set->log2_max_frame_num_minus4) != sequence_param_set->maximum_frame_number)
  {
	  // This is inconsistent. Many options available to handle this. Choose to assume that
	  // log2_max_frame_num_minus4 is correct and fix maximum_frame_number
	  sequence_param_set->maximum_frame_number = 16 << sequence_param_set->log2_max_frame_num_minus4;
  }

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->log2_max_frame_num_minus4);
  WRITE_ENCODED_SYMBOL("log2_max_frame_num_minus4", sequence_param_set->log2_max_frame_num_minus4)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->pic_order_cnt_type);
  WRITE_ENCODED_SYMBOL("pic_order_cnt_type", sequence_param_set->pic_order_cnt_type)

  if (0 == sequence_param_set->pic_order_cnt_type)
  {
	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->log2_max_pic_order_cnt_lsb_minus4);
	  WRITE_ENCODED_SYMBOL("log2_max_pic_order_cnt_lsb_minus4", sequence_param_set->log2_max_pic_order_cnt_lsb_minus4)
  }
  else if (1 == sequence_param_set->pic_order_cnt_type)
  {
	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->delta_pic_order_always_zero_flag);
	  WRITE_ENCODED_SYMBOL("delta_pic_order_always_zero_flag", sequence_param_set->delta_pic_order_always_zero_flag)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R3,sequence_param_set->offset_for_non_ref_pic);
	  WRITE_ENCODED_SYMBOL("offset_for_non_ref_pic", sequence_param_set->offset_for_non_ref_pic)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R3,sequence_param_set->offset_for_top_to_bottom_field);
	  WRITE_ENCODED_SYMBOL("offset_for_top_to_bottom_field", sequence_param_set->offset_for_top_to_bottom_field)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->num_ref_frames_in_pic_order_cnt_cycle);
	  WRITE_ENCODED_SYMBOL("num_ref_frames_in_pic_order_cnt_cycle", sequence_param_set->num_ref_frames_in_pic_order_cnt_cycle)

	  for (int i = 0; i < sequence_param_set->num_ref_frames_in_pic_order_cnt_cycle; i++)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R3,sequence_param_set->offset_for_ref_frame[i]);
		  WRITE_ENCODED_SYMBOL("offset_for_ref_frame[i]", sequence_param_set->offset_for_ref_frame[i])
	  }
  }

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->num_ref_frames);
  WRITE_ENCODED_SYMBOL("num_ref_frames", sequence_param_set->num_ref_frames)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->required_frame_num_update_behaviour_flag);
  WRITE_ENCODED_SYMBOL("required_frame_num_update_behaviour_flag", sequence_param_set->required_frame_num_update_behaviour_flag)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->frame_width_in_mbs - 1);
  WRITE_ENCODED_SYMBOL("frame_width_in_mbs_minus1", sequence_param_set->frame_width_in_mbs - 1)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->picture_height_in_map_units - 1);
  WRITE_ENCODED_SYMBOL("picture_height_in_map_units_minus1", sequence_param_set->picture_height_in_map_units - 1)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->frame_mbs_only_flag);
  WRITE_ENCODED_SYMBOL("frame_mbs_only_flag", sequence_param_set->frame_mbs_only_flag)

  if (!sequence_param_set->frame_mbs_only_flag)
  {
	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->mb_adaptive_frame_field_flag);
	  WRITE_ENCODED_SYMBOL("mb_adaptive_frame_field_flag", sequence_param_set->mb_adaptive_frame_field_flag)
  }

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->direct_8x8_inference_flag);
  WRITE_ENCODED_SYMBOL("direct_8x8_inference_flag", sequence_param_set->direct_8x8_inference_flag)

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->frame_cropping_flag);
  WRITE_ENCODED_SYMBOL("frame_cropping_flag", sequence_param_set->frame_cropping_flag)

  if (sequence_param_set->frame_cropping_flag)
  {
	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->frame_cropping_rect_left_offset - 1);
	  WRITE_ENCODED_SYMBOL("frame_cropping_rect_left_offset", sequence_param_set->frame_cropping_rect_left_offset - 1)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->frame_cropping_rect_right_offset - 1);
	  WRITE_ENCODED_SYMBOL("frame_cropping_rect_right_offset", sequence_param_set->frame_cropping_rect_right_offset - 1)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->frame_cropping_rect_top_offset - 1);
	  WRITE_ENCODED_SYMBOL("frame_cropping_rect_top_offset", sequence_param_set->frame_cropping_rect_top_offset - 1)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,sequence_param_set->frame_cropping_rect_bottom_offset - 1);
	  WRITE_ENCODED_SYMBOL("frame_cropping_rect_bottom_offset", sequence_param_set->frame_cropping_rect_bottom_offset - 1)
  }

  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,sequence_param_set->vui_seq_parameters_flag);
  WRITE_ENCODED_SYMBOL("vui_seq_parameters_flag", sequence_param_set->vui_seq_parameters_flag)

  if (sequence_param_set->vui_seq_parameters_flag)
  {
	  // VUI Sequence Parameters
	  assert(0 != sequence_param_set->vui_parameters);

	  h264_vui_parameters * vui = sequence_param_set->vui_parameters;

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->aspect_ratio_info_present_flag);
	  WRITE_ENCODED_SYMBOL("aspect_ratio_info_present_flag", vui->aspect_ratio_info_present_flag)

	  if (sequence_param_set->vui_parameters->aspect_ratio_info_present_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,vui->aspect_ratio_idc);
		  WRITE_ENCODED_SYMBOL("aspect_ratio_idc", vui->aspect_ratio_idc)

		  if (255 == vui->aspect_ratio_idc)  // 255==Extended_SAR
		  {
			  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->sar_width);
			  WRITE_ENCODED_SYMBOL("sar_width", vui->sar_width)
			  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->sar_height);
			  WRITE_ENCODED_SYMBOL("sar_height", vui->sar_height)
		  }
	  }

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->overscan_info_present_flag);
	  WRITE_ENCODED_SYMBOL("overscan_info_present_flag", vui->overscan_info_present_flag)

	  if (vui->overscan_info_present_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->overscan_appropriate_flag);
		  WRITE_ENCODED_SYMBOL("overscan_appropriate_flag", vui->overscan_appropriate_flag)
	  }

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->video_signal_type_present_flag);
	  WRITE_ENCODED_SYMBOL("video_signal_type_present_flag", vui->video_signal_type_present_flag)

	  if (vui->video_signal_type_present_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_3BIT,vui->video_format);
		  WRITE_ENCODED_SYMBOL("video_format", vui->video_format)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->video_full_range_flag);
		  WRITE_ENCODED_SYMBOL("video_full_range_flag", vui->video_full_range_flag)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->colour_description_present_flag);
		  WRITE_ENCODED_SYMBOL("colour_description_present_flag", vui->colour_description_present_flag)

		  if (vui->colour_description_present_flag)
		  {
			  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,vui->colour_primaries);
			  WRITE_ENCODED_SYMBOL("colour_primaries", vui->colour_primaries)
			  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,vui->transfer_characteristics);
			  WRITE_ENCODED_SYMBOL("transfer_characteristics", vui->transfer_characteristics)
			  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,vui->matrix_coefficients);
			  WRITE_ENCODED_SYMBOL("matrix_coefficients", vui->matrix_coefficients)
		  }
	  }

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->chroma_loc_info_present_flag);
	  WRITE_ENCODED_SYMBOL("chroma_loc_info_present_flag", vui->chroma_loc_info_present_flag)

	  if (vui->chroma_loc_info_present_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->chroma_sample_loc_type_top_field);
		  WRITE_ENCODED_SYMBOL("chroma_sample_loc_type_top_field", vui->chroma_sample_loc_type_top_field)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->chroma_sample_loc_type_bottom_field);
		  WRITE_ENCODED_SYMBOL("chroma_sample_loc_type_bottom_field", vui->chroma_sample_loc_type_bottom_field)
	  }

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->timing_info_present_flag);
	  WRITE_ENCODED_SYMBOL("timing_info_present_flag", vui->timing_info_present_flag)

	  if (vui->timing_info_present_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->num_units_in_tick >> 16);
		  WRITE_ENCODED_SYMBOL("num_units_in_tick (ms 16 bits)", vui->num_units_in_tick >> 16)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->num_units_in_tick & 0xffff);
		  WRITE_ENCODED_SYMBOL("num_units_in_tick (ls 16 bits)", vui->num_units_in_tick & 0xffff)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->time_scale >> 16);
		  WRITE_ENCODED_SYMBOL("time_scale (ms 16 bits)", vui->time_scale >> 16)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_16BIT,vui->time_scale & 0xffff);
		  WRITE_ENCODED_SYMBOL("time_scale (ls 16 bits)", vui->time_scale & 0xffff)
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->fixed_frame_rate_flag);
		  WRITE_ENCODED_SYMBOL("fixed_frame_rate_flag", vui->fixed_frame_rate_flag)
	  }

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,0);
	  WRITE_ENCODED_SYMBOL("nal_hrd_parameters_present_flag", 0)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,0);
	  WRITE_ENCODED_SYMBOL("vcl_hrd_parameters_present_flag", 0)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->pic_struct_present_flag);
	  WRITE_ENCODED_SYMBOL("pic_struct_present_flag", vui->pic_struct_present_flag)

	  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->bitstream_restriction_flag);
	  WRITE_ENCODED_SYMBOL("bitstream_restriction_flag", vui->bitstream_restriction_flag)

	  if (vui->bitstream_restriction_flag)
	  {
		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,vui->motion_vectors_over_pic_boundaries_flag);
		  WRITE_ENCODED_SYMBOL("motion_vectors_over_pic_boundaries_flag", vui->motion_vectors_over_pic_boundaries_flag)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->max_bytes_per_pic_denom);
		  WRITE_ENCODED_SYMBOL("max_bytes_per_pic_denom", vui->max_bytes_per_pic_denom)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->max_bits_per_mb_denom);
		  WRITE_ENCODED_SYMBOL("max_bits_per_mb_denom", vui->max_bits_per_mb_denom)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->log2_max_mv_length_horizontal);
		  WRITE_ENCODED_SYMBOL("log2_max_mv_length_horizontal", vui->log2_max_mv_length_horizontal)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->log2_max_mv_length_vertical);
		  WRITE_ENCODED_SYMBOL("log2_max_mv_length_vertical", vui->log2_max_mv_length_vertical)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->num_reorder_frames);
		  WRITE_ENCODED_SYMBOL("num_reorder_frames", vui->num_reorder_frames)

		  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R2,vui->max_dec_frame_buffering);
		  WRITE_ENCODED_SYMBOL("max_dec_frame_buffering", vui->max_dec_frame_buffering)
	  }
  }

  WriteRBSPTrailingBits();

}

CHANNEL_SERVICE	Service_SequenceHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
  if(!reg2)
    return;

  MgrMPC.CurrentEEMb.StartCode[4] = (3 << 5) + H264_SEQUENCE_PARAMETER_SET;
  WriteBitStreamData(reg2);
  gCH264Encoder.m_OverallBufferStatus = gCH264Encoder.m_ThreadBufferStatus += (reg2 << 3 );
  //gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount(reg2 << 3);
	gCH264Encoder.m_PictureBitCount += (reg2 << 3) + 8; // 8 for the NalRefIDC
  WRITE_ENCODED_SYMBOL("PICTURE BIT CNT: ", gCH264Encoder.m_PictureBitCount)

  gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->ResetBuffer();
  // TODO Kick of picture parameter encoding from here
}
CHANNEL_SERVICE	Service_WriteUserData(void)
{
  // Reset EE
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R6,0x1);

  // Set the address to write the packed bits
#ifdef ARC_ENCODER_API
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_PhysBufferPointer);
#else
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer);
#endif

  // Set the SequenceHeader response function address
  unsigned int addr_val = (unsigned int)Service_UserDataResponse;
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),addr_val);

    WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 5);
	WRITE_ENCODED_SYMBOL("PAYLOAD_TYPE_BYTE", 5)
    WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 16+22);
	WRITE_ENCODED_SYMBOL("PAYLOAD_SIZE_BYTE", 16+22)
	for(int i = 0; i < 16; i++)
	{
		WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT,UUID[i]);
		WRITE_ENCODED_SYMBOL("UUID", UUID[i])
	}

	// loop to insert the product name into the bitstream
	for (int loop=0; loop<6; loop++)
	{
        WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, ProductName[loop]);
        WRITE_ENCODED_SYMBOL("PAYLOAD", ProductName[loop]);
	}

	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, '-');
	WRITE_ENCODED_SYMBOL("PAYLOAD", '-')
	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 'S');
	WRITE_ENCODED_SYMBOL("PAYLOAD", 'S')
	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 'v');
	WRITE_ENCODED_SYMBOL("PAYLOAD", 'v')

	char version[255];
	if (HwMe == 1)
	{
		sprintf(version,"%2d.%02d",SOFTWARE_MAJOR_VERSION, SOFTWARE_MINOR_VERSION);
	}
	else {
		sprintf(version,"%2d.%02d",MESOFTWARE_MAJOR_VERSION, MESOFTWARE_MINOR_VERSION);
	}
	
	for(int i = 0 ; i < 5; i ++) {
		WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, version[i]);
		WRITE_ENCODED_SYMBOL("PAYLOAD", version[i])
	}
	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, '-');
	WRITE_ENCODED_SYMBOL("PAYLOAD", '-')
	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 'H');
	WRITE_ENCODED_SYMBOL("PAYLOAD", 'H')
	WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, 'v');
	WRITE_ENCODED_SYMBOL("PAYLOAD", 'v')


	sprintf(version,"%2d.%02d",HARDWARE_MAJOR_VERSION, HARDWARE_MINOR_VERSION);
	for(int i = 0 ; i < 5; i ++) {
		WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_8BIT, version[i]);
		WRITE_ENCODED_SYMBOL("PAYLOAD", version[i])
	}

	WriteRBSPTrailingBits();
}

CHANNEL_SERVICE	Service_UserDataResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
  if(!reg2)
    return;

  MgrMPC.CurrentEEMb.StartCode[4] = H264_SEI_NAL_UNIT_TYPE;
  WriteBitStreamData(reg2);
  gCH264Encoder.m_OverallBufferStatus = gCH264Encoder.m_ThreadBufferStatus += (reg2 << 3 );
  //gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount(reg2 << 3);
	gCH264Encoder.m_PictureBitCount += (reg2 << 3) + 8; // 8 for the NalRefIDC
  WRITE_ENCODED_SYMBOL("SEI BIT CNT: ", gCH264Encoder.m_PictureBitCount)
 

  gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->ResetBuffer();


}
// Channel service routine to write the Picture parameter set
CHANNEL_SERVICE	Service_WritePictureParameter(unsigned char picture_parameter_set_id)
{

  picture_parameter_set_t * picture_parameter_set
		= gCH264Encoder.m_ParameterSets->ReadPictureParameterSet(picture_parameter_set_id);

  // Reset EE
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R6,0x1);

  // Set the address to write the packed bits
#ifdef ARC_ENCODER_API
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_PhysBufferPointer);
#else
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer);
#endif

   // Set the SequenceHeader response function address
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),(unsigned int)Service_PicParameterHeaderResponse);



	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set_id);
	WRITE_ENCODED_SYMBOL("picture_parameter_set_id", picture_parameter_set_id)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->sequence_parameter_set_id);
	WRITE_ENCODED_SYMBOL("sequence_parameter_set_id", picture_parameter_set->sequence_parameter_set_id)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->CABAC_entropy_coding_mode);
	WRITE_ENCODED_SYMBOL("CABAC_entropy_coding_mode", picture_parameter_set->CABAC_entropy_coding_mode)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->pic_order_present_flag);
	WRITE_ENCODED_SYMBOL("pic_order_present_flag", picture_parameter_set->pic_order_present_flag)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->num_slice_groups-1);
	WRITE_ENCODED_SYMBOL("num_slice_groups minus 1", picture_parameter_set->num_slice_groups-1)

	if (picture_parameter_set->num_slice_groups > 1)
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->slice_group_map_type);
		WRITE_ENCODED_SYMBOL("mb_allocation_map_type", picture_parameter_set->slice_group_map_type)

		if (0 == picture_parameter_set->slice_group_map_type)
		{
			for (int slice_group=0; slice_group<picture_parameter_set->num_slice_groups ; slice_group++)
			{
				WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->slice_group_run_length[slice_group]-1);
				WRITE_ENCODED_SYMBOL("run_length_minus1", picture_parameter_set->slice_group_run_length[slice_group]-1)
			}
		}
		else if (2 == picture_parameter_set->slice_group_map_type)
		{
			for (int slice_group=0; slice_group<picture_parameter_set->num_slice_groups-1; slice_group++)
			{
				WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->slice_group_top_left[slice_group]);
				WRITE_ENCODED_SYMBOL("top_left", picture_parameter_set->slice_group_top_left[slice_group])

				WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->slice_group_bottom_right[slice_group]);
				WRITE_ENCODED_SYMBOL("bottom_right", picture_parameter_set->slice_group_bottom_right[slice_group])
			}
		}
		else if ((picture_parameter_set->slice_group_map_type >= 3) &&
				 (picture_parameter_set->slice_group_map_type <= 5))
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->slice_group_change_direction_flag);
			WRITE_ENCODED_SYMBOL("slice_group_change_direction_flag", picture_parameter_set->slice_group_change_direction_flag)

			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->slice_group_change_rate-1);
			WRITE_ENCODED_SYMBOL("slice_group_change_rate", picture_parameter_set->slice_group_change_rate-1)

			int max_slice_group_change_cycle =
					((gCH264Encoder.m_SequenceParameterSet->frame_width_in_mbs * gCH264Encoder.m_SequenceParameterSet->frame_height_in_mbs)
						/ picture_parameter_set->slice_group_change_rate);

			gCH264Encoder.m_SliceGroupChangeCycleCodeLength = 0;
			while (max_slice_group_change_cycle > 0)
			{
				gCH264Encoder.m_SliceGroupChangeCycleCodeLength++;
				max_slice_group_change_cycle >>= 1;
			}
		}
		else if (6 == picture_parameter_set->slice_group_map_type)
		{
			unsigned int pic_size_in_map_units =
				gCH264Encoder.m_SequenceParameterSet->frame_width_in_mbs * gCH264Encoder.m_SequenceParameterSet->frame_height_in_mbs;

			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),pic_size_in_map_units-1);
			WRITE_ENCODED_SYMBOL("pic_size_in_map_units_minus_1", pic_size_in_map_units-1)

			unsigned int slice_group_id_code_length;
			if (2 == picture_parameter_set->num_slice_groups)
			{
				slice_group_id_code_length = 1;
			}
			else if (picture_parameter_set->num_slice_groups <= 4)
			{
				slice_group_id_code_length = 2;
			}
			else
			{
				slice_group_id_code_length = 3;
			}

			for (unsigned int i=0; i<pic_size_in_map_units; i++)
			{
				WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |(CH_ADD_BIT_OFFSET + slice_group_id_code_length)),picture_parameter_set->slice_group_id[i]);
				WRITE_ENCODED_SYMBOL("slice_group_id", picture_parameter_set->slice_group_id[i])
			}
		}
	}

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->num_ref_idx_l0_active - 1);
	WRITE_ENCODED_SYMBOL("num_ref_idx_l0_active_minus1", picture_parameter_set->num_ref_idx_l0_active - 1)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),picture_parameter_set->num_ref_idx_l1_active - 1);
	WRITE_ENCODED_SYMBOL("num_ref_idx_l1_active_minus1", picture_parameter_set->num_ref_idx_l1_active - 1)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->weighted_pred_flag);
	WRITE_ENCODED_SYMBOL("weighted_pred_flag", picture_parameter_set->weighted_pred_flag)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_2BIT),picture_parameter_set->weighted_bipred_idc);
	WRITE_ENCODED_SYMBOL("weighted_bipred_idc", picture_parameter_set->weighted_bipred_idc)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),(int)(picture_parameter_set->pic_init_qp)-26);
	WRITE_ENCODED_SYMBOL("pic_init_qp_minus26", (int)(picture_parameter_set->pic_init_qp)-26)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),(int)(picture_parameter_set->pic_init_qs)-26);
	WRITE_ENCODED_SYMBOL("pic_init_qs_minus26", (int)(picture_parameter_set->pic_init_qs)-26)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),picture_parameter_set->chroma_qp_index_offset);
	WRITE_ENCODED_SYMBOL("chroma_qp_index_offset", picture_parameter_set->chroma_qp_index_offset)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->deblocking_filter_parameters_present_flag);
	WRITE_ENCODED_SYMBOL("deblocking_filter_parameters_present_flag", picture_parameter_set->deblocking_filter_parameters_present_flag)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->constrained_intra_pred_flag);
	WRITE_ENCODED_SYMBOL("constrained_intra_pred_flag", picture_parameter_set->constrained_intra_pred_flag)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),picture_parameter_set->redundant_pic_cnt_present_flag);
	WRITE_ENCODED_SYMBOL("redundant_pic_cnt_present_flag", picture_parameter_set->redundant_pic_cnt_present_flag)

	WriteRBSPTrailingBits();
}



CHANNEL_SERVICE Service_PicParameterHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
   if(!reg2)
    return;

  MgrMPC.CurrentEEMb.StartCode[4] = (3 << 5) + H264_PICTURE_PARAMETER_SET;

  WriteBitStreamData(reg2);

  gCH264Encoder.m_OverallBufferStatus = gCH264Encoder.m_ThreadBufferStatus += (reg2 << 3 );
	gCH264Encoder.m_PictureBitCount += (reg2 << 3) + 8; // 8 for the NalRefIDC
  WRITE_ENCODED_SYMBOL("PICTURE BIT CNT: ", gCH264Encoder.m_PictureBitCount)

  //gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount(reg2 << 3);
  gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->ResetBuffer();
}


// Service routine to write the slice header
CHANNEL_SERVICE Service_WriteSliceHeader(unsigned int first_mb_in_slice, int mbNum)
{
  MgrMPC.CurrentEEMb.SliceNotFinished++;
 
  // Reset EE
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R6,0x1);

  // Set the address to write the packed bits
#ifdef ARC_ENCODER_API
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_PhysBufferPointer);
#else
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R14),(unsigned int)gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer);
#endif

   // Set the CH_CALL response function address
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R23),(unsigned int)Service_SliceHeaderResponse);

  // Set the CH_FL_ADDR response function address
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R22),(unsigned int)Service_SliceEndResponse);

  // Set the CH_MBE_ADDR response function address
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R21),(unsigned int)Service_MacroBlockEndResponse);


  // Set the slice type
  WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R1),(H264_P_SLICE == gCH264Encoder.m_SliceType)? 1:0 );
	WRITE_ENCODED_SYMBOL("Source Picture Frame number", gCH264Encoder.m_SourcePicture->m_fn)
	WRITE_ENCODED_SYMBOL("Picture order count", gCH264Encoder.m_PictureOrderCount)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),first_mb_in_slice);
	WRITE_ENCODED_SYMBOL("First MB in Slice", first_mb_in_slice)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_SliceType);
	WRITE_ENCODED_SYMBOL("Slice Type", gCH264Encoder.m_SliceType)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_PictureParameterSetID);
	WRITE_ENCODED_SYMBOL("Picture Parameter Set ID", gCH264Encoder.m_PictureParameterSetID)

	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |(CH_ADD_BIT_OFFSET + gCH264Encoder.m_SequenceParameterSet->log2_max_frame_num_minus4 + 4)),gCH264Encoder.m_FrameNumber);
	WRITE_ENCODED_SYMBOL("Frame Number", gCH264Encoder.m_FrameNumber)

	if (!gCH264Encoder.m_SequenceParameterSet->frame_mbs_only_flag)
	{
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);
			WRITE_ENCODED_SYMBOL("Frame Picture", 0)
		}
	}
    if (H264_I_PICTURE == gCH264Encoder.m_PictureType)
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_IDRPictureID);
		WRITE_ENCODED_SYMBOL("IDR Picture ID", gCH264Encoder.m_IDRPictureID)
	}

	if (0 == gCH264Encoder.m_SequenceParameterSet->pic_order_cnt_type)
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |(CH_ADD_BIT_OFFSET + gCH264Encoder.m_SequenceParameterSet->log2_max_pic_order_cnt_lsb_minus4+4)),gCH264Encoder.m_PicOrderCountLSB);
		WRITE_ENCODED_SYMBOL("pic_order_cnt_lsb", gCH264Encoder.m_PicOrderCountLSB)

		if (gCH264Encoder.m_PictureParameterSet->pic_order_present_flag)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),0);
			WRITE_ENCODED_SYMBOL("m_DeltaPicOrderCountBottom", 0)
		}
	}

	if ((1 == gCH264Encoder.m_SequenceParameterSet->pic_order_cnt_type) &&
		 !gCH264Encoder.m_SequenceParameterSet->delta_pic_order_always_zero_flag)
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),gCH264Encoder.m_DeltaPicOrderCount);
		WRITE_ENCODED_SYMBOL("m_DeltaPicOrderCount", gCH264Encoder.m_DeltaPicOrderCount)

		if (gCH264Encoder.m_PictureParameterSet->pic_order_present_flag
			)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),0);
			WRITE_ENCODED_SYMBOL("m_DeltaPicOrderCount[1]", 0)
		}
	}

	if (gCH264Encoder.m_PictureParameterSet->redundant_pic_cnt_present_flag)
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_RedundantPictureCount);
		WRITE_ENCODED_SYMBOL("RedundantPictureCount", gCH264Encoder.m_RedundantPictureCount)
	}



	if (H264_P_SLICE == gCH264Encoder.m_SliceType)

	{
		//if ((H264_P_SLICE == m_SliceType) &&
		if(gCH264Encoder.m_PictureParameterSet->num_ref_idx_l0_active != gCH264Encoder.m_SliceNumList0RefPictures)//)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),1);
			WRITE_ENCODED_SYMBOL("num_ref_idx_active_override_flag", 1)

			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_SliceNumList0RefPictures-1);
			WRITE_ENCODED_SYMBOL("num_ref_idx_l0_active_minus1", gCH264Encoder.m_SliceNumList0RefPictures-1)
		}
		else
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);
			WRITE_ENCODED_SYMBOL("num_ref_idx_active_override_flag", 0)
		}
	}

	if (H264_I_SLICE != gCH264Encoder.m_SliceType)
	{
		// No remapping needed
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);
		WRITE_ENCODED_SYMBOL("ref_idx_reordering_flag_l0", 0)
	}

  WriteDecodedPictureBufferManagement();

    int slice_qp_delta = gCH264Encoder.m_LastTransmittedQuant[MgrMPC.Picture.SliceGroupMap[mbNum]] - gCH264Encoder.m_PictureParameterSet->pic_init_qp;
	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),slice_qp_delta);
	WRITE_ENCODED_SYMBOL("slice_qp_delta", slice_qp_delta)

	if (gCH264Encoder.m_PictureParameterSet->deblocking_filter_parameters_present_flag)
	{
		// Deblocking filter parameters
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R2),gCH264Encoder.m_DisableDeblockingFilterIDC);
		WRITE_ENCODED_SYMBOL("disable_deblocking_filter_idc", gCH264Encoder.m_DisableDeblockingFilterIDC)

		if (1 != gCH264Encoder.m_DisableDeblockingFilterIDC)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),0 >> 1);		// SliceAlphaC0Offset
			WRITE_ENCODED_SYMBOL("slice_alpha_c0_offset_div2", 0 >> 1)

			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R3),/*gCH264Encoder.m_SliceBetaOffset*/0 >> 1);
			WRITE_ENCODED_SYMBOL("slice_beta_offset_div2", /*gCH264Encoder.m_SliceBetaOffset*/ 0 >> 1)
		}
	}

	if ((gCH264Encoder.m_PictureParameterSet->num_slice_groups > 1) &&
		(gCH264Encoder.m_PictureParameterSet->slice_group_map_type >= 3) &&
		(gCH264Encoder.m_PictureParameterSet->slice_group_map_type <= 5))
	{
		WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |(CH_ADD_BIT_OFFSET + gCH264Encoder.m_SliceGroupChangeCycleCodeLength)),gCH264Encoder.m_SliceGroupChangeCycle);
		WRITE_ENCODED_SYMBOL("slice_group_change_cycle", gCH264Encoder.m_SliceGroupChangeCycle)
	}
	 
	// Issue a CH_Call command to get the bits encoded
	WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R25),0x1);

}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	WriteDecodedPictureBufferManagement		*/
/* DESCRIPTION:	This writes the decoded picture buffer management (mmco)
				symbols to the symbol buffer.
				IDR_pic_flag is true if the decoded_picture_management() is
				part of a slice header of an IDR picture or if the
				decoded_picture_management() is part of a decoded picture
				management repetition SEI message and the
				original_idr_flag is 1. Otherwise it shall be false.		*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void WriteDecodedPictureBufferManagement(void)
{
	bool flush_non_reference_pictures, store_IDR_as_long_term;

	gCH264Encoder.m_EncodedPictureBuffer->GetIDRParameters(flush_non_reference_pictures,
											 store_IDR_as_long_term);
    if (H264_I_PICTURE == gCH264Encoder.m_PictureType)
	{
		if (flush_non_reference_pictures)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),1);	// Flush non stored pictures
			WRITE_ENCODED_SYMBOL("no_output_of_prior_pics_flag", 1)
		}
		else
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);	// Do not flush non stored pictures
			WRITE_ENCODED_SYMBOL("no_output_of_prior_pics_flag", 0)
		}

		if (store_IDR_as_long_term)
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),1);	// Store this picture as a long term reference picture
			WRITE_ENCODED_SYMBOL("long_term_reference_flag", 1)
		}
		else
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);	// Store this picture as a short term reference picture
			WRITE_ENCODED_SYMBOL("long_term_reference_flag", 0)
		}
	}
	else
	{
		{
			WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |CH_ADD_1BIT),0);	// Sliding Window
			WRITE_ENCODED_SYMBOL("dec_pic_buffering_mode", 0)
		}

	}
}


// Response function for CH_CALL command from the slice header routine
CHANNEL_SERVICE	Service_SliceHeaderResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
    gCH264Encoder.m_PictureBitCount += 8; // For NAL REF IDC
    gCH264Encoder.m_ThreadBufferStatus += reg1;
    gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount(reg1);
    gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->SetEmuByteCnt(reg2);

    unsigned char idc = 3;
    unsigned char nal_type = H264_CODED_SLICE_IDR;

    if (H264_I_PICTURE != gCH264Encoder.m_PictureType)
    {
	    idc = 2;
	    nal_type = H264_CODED_SLICE_NO_DP;
    }
    
   MgrMPC.CurrentEEMb.StartCode[4] = (idc << 5) + nal_type;
#ifndef ENABLE_PERFORMANCE
   WriteBitStreamData(0);
#endif

   if(gCH264Encoder.m_DynamicControl == 1)
   {
		Service_EnableTimer();
   }
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ARCSymbolEjectMacroblock							*/
/* DESCRIPTION:	This writes the current macroblock to the symbol buffer.	*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

CHANNEL_SERVICE	Service_WriteMacroblock(int cirIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cirIdx];
    int leftIdx = MgrMPC.ActMPC[cirIdx].LeftIdx;
    cMPCcb &lcb = WorkAreaSDM->CircularBuffs[leftIdx];
    cMPCfb &fb = WorkAreaSDM->FixedBuffs[MgrMPC.ActMPC[cirIdx].Column];
    int last_mb = 0;
    if(MgrMPC.CurrentEEMb.MbNum == MgrMPC.CurrentEEMb.LstMb)
    {
    	last_mb = 0x08;
    }
    MgrMPC.EEQueue.Push(cirIdx);

   // Set the current,top and left MPC structures
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R17),(unsigned int)&cb);


   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R18), (unsigned int)&fb);

   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R19),(unsigned int)&(lcb));
 

   // set current slice group
   signed char currSliceGroup = cb.WorkArea.SliceGroup;
   
   // DQuant is in the range -26 to 25
   //int differential_quant = ((int)(cb.QuantValue - gCH264Encoder.m_LastTransmittedQuant[cb.WorkArea.SliceGroup] + 78) % 52) - 26;

   static const char DiffQuantMod52[103] = // -51 to 51
   {
      1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
      0,
      1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1
   };
   
   int differential_quant = DiffQuantMod52[(cb.QuantValue - gCH264Encoder.m_LastTransmittedQuant[currSliceGroup] + 51)];

   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R15),differential_quant);


   // Kick off macroblock encoding
   WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R6),(0x04 | last_mb));

   if((MBCT_H264_SKIPPED == cb.MBCodingType)
     ||(!(cb.NonZeroCoeffLuma
         || cb.NonZeroCoeffChroma
         || cb.UV_DC_NonZeroCoeff) && cb.MBCodingType != MBCT_H264_INTRA16X16))
   {
     gCH264Encoder.m_lastquant = cb.QuantValue = gCH264Encoder.m_LastTransmittedQuant[currSliceGroup];
     return;
   }

	/////////////////////////////////////////////////////////
	// Store last DQuant
	/////////////////////////////////////////////////////////

	gCH264Encoder.m_lastquant = gCH264Encoder.m_LastTransmittedQuant[currSliceGroup] = cb.QuantValue;
}



// Called via channel from EE
//	reg0	:	Unit sel
//	reg1 	: 	Macroblock bit count
//	reg2	: 	concatination byte count for slice
CHANNEL_SERVICE	Service_SliceEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{

  if(gCH264Encoder.m_DynamicControl == 1) 
  {
	Service_DisableTimer();
  }
  gCH264Encoder.m_ThreadBufferStatus += reg1;
  gCH264Encoder.m_PictureBitCount += (reg2 << 3);

  WRITE_ENCODED_SYMBOL("PICTURE BIT CNT: ", gCH264Encoder.m_PictureBitCount)
  // gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount((reg1));
 // gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->SetEmuByteCnt(reg2);

	

#ifndef ARC_ENCODER_API
  #ifndef ENABLE_PERFORMANCE
  	fwrite(gCH264Encoder.m_NextBufferToCopy->m_BufferPointer, sizeof(unsigned char), reg2, bitstream_file);
  	fflush(bitstream_file);
  	flush_dcache();
  #endif
  	
  #ifdef ENABLE_FILE_WRITE
		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], MgrMPC.CurrentEEMb.StartCode,5*sizeof(unsigned char));
		YUVBuffer.OutputPtr += 5;
		//memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer, reg2*sizeof(unsigned char));
	
		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_NextBufferToCopy->m_BufferPointer, reg2*sizeof(unsigned char));
		YUVBuffer.OutputPtr += reg2;// -5 ??
		flush_dcache();
  #endif
	
#else
  gCH264Encoder.m_NextBufferToCopy->m_amount += reg2;
  gCH264Encoder.m_NextBufferToCopy->QueueBuffer();
#endif
  gCH264Encoder.m_NextBufferToCopy->ResetBuffer();
  if ((0 != gCH264Encoder.m_NextBufferToCopy->m_Next))
  {
	  gCH264Encoder.m_NextBufferToCopy = gCH264Encoder.m_NextBufferToCopy->m_Next;
  }
  
 /* if(MgrMPC.CurrentEEMb.MbNum <= MgrMPC.CurrentEEMb.LstMb)
  {
	  gCH264Encoder.m_NextBufferToCopy = gCH264Encoder.m_NextBufferToCopy->m_Next;
  }*/

  MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk = true;
  MgrMPC.CurrentEEMb.SliceStarted = 0;
  
  MgrMPC.CurrentEEMb.SliceNotFinished--;
}

CHANNEL_SERVICE	Service_MacroBlockEndResponse(unsigned int reg0,unsigned int reg1,unsigned int reg2)
{
   MgrMPC.IdxFromEE = MgrMPC.EEQueue.Pull();
   gCH264Encoder.m_ThreadBufferStatus += reg1;
   gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->AddBitCount(reg1);
   gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->SetEmuByteCnt(reg2);

   gCH264Encoder.RateControlMacroblockEnd();
   WRITE_ENCODED_SYMBOL_FLOAT("Thread Buffer Status: ", gCH264Encoder.m_ThreadBufferStatus)
  if(CheckForSliceEnd(gCH264Encoder.m_CurrentSliceGroup))
  {
    // Clear the MB_BIT_COUNT
    WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE |R20),0);
    // Flush if any skip count
    if(MgrMPC.CurrentEEMb.SliceStarted)
    {
        WriteToChannelEE((MacroSetting_ChannelCmd_ArcToEE | R24),0);
    }
	WriteRBSPTrailingBits();

	
	if(MgrMPC.CurrentEEMb.MbNum == MgrMPC.CurrentEEMb.LstMb) {// last mb in picture
		if(gCH264Encoder.m_DynamicControl)Service_StopTimer();
		if(gCH264Encoder.m_DynamicControl == 2)
		{
			Service_InitTimer(0x7FFFFFFF); // set to large number
			Service_EnableTimer(); // in order to start it and record cycles between frames		
		}
	}
  }
  else
  {
	  MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk = true;
  }
  MgrMPC.CurrentEEMb.MbNum++;

  if(MgrMPC.CurrentEEMb.WrSliceHdr)
  {
	  Service_WriteSliceHeader(MgrMPC.CurrentEEMb.MbNum, MgrMPC.CurrentEEMb.MbNum);
	  MgrMPC.CurrentEEMb.WrSliceHdr = 0;
  }


}

// Check to see if a new slice is to be formed.
// New slice will be created if it is the last macroblock of the picture
// or the symbol byte buffer has reached it's threshold.
// returns true if new slice has to be created

// FMO MODE
// In FMO mode 4 slice groups will be supported.
// Each bitpacker will be used to pack differnt slice groups.
// Packer ID will be the same as the Slice group ID. Packer0 will be
// used to pack macroblocks from slice group 0. This is needed as the
// response from EE H/W on completion of macroblock encoding returns the packer ID.
// TODO
// Currently only the concat unit is used and only one slice group
// control->mpc.WorkArea.SliceGroup = 0
int CheckForSliceEnd(unsigned int unit_sel)
{
    // Last macroblock in the slice
    if(MgrMPC.CurrentEEMb.MbNum == MgrMPC.CurrentEEMb.LstMb)
    {
    	MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit = 0;
    	return 1;
	}
    else if (MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit)
    {

    	    MgrMPC.ActMPC[MgrMPC.IdxFromEE].BufferLimit = 0;
    		if (gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->m_Next !=0)
			{
				gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]
					= gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->m_Next;
			}
			else
			{
				// Need to allocate a new byte buffer
				gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->m_Next = new CSymbolByteBuffer;
				if (0 == gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->m_Next)
				{
					assert(false);
					return 0;
				}

				gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]
					= gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->m_Next;

				gCH264Encoder.m_CurrentSymbolByteBuffer[unit_sel]->Create(gCH264Encoder.m_MaximumPacketDataSize+MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK);


			}


		  MgrMPC.CurrentEEMb.SliceStarted = 1;
		  MgrMPC.CurrentEEMb.WrSliceHdr = 1;

          return 1;
		}
  return 0;
}


void WriteRBSPTrailingBits(void)
{
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | CH_ADD_1BIT,1);
	WRITE_ENCODED_SYMBOL("RBSP Stop Bit", 1)

	// Pack to byte
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R7,0);

  // Flush the current buffer
  WriteToChannelEE(MacroSetting_ChannelCmd_ArcToEE | R6,0x2);
}


void WriteBitStreamData(unsigned int amount)
{
#ifndef ARC_ENCODER_API
    #ifndef ENABLE_PERFORMANCE
		fwrite(MgrMPC.CurrentEEMb.StartCode, sizeof(unsigned char), 5, bitstream_file);
		fwrite(gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer, sizeof(unsigned char), amount, bitstream_file);
		fflush(bitstream_file);
		flush_dcache();
    #endif
	
    #ifdef ENABLE_FILE_WRITE
		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], MgrMPC.CurrentEEMb.StartCode,5*sizeof(unsigned char));
		YUVBuffer.OutputPtr += 5;
		memcpy(&YUVBuffer.OutputStream[YUVBuffer.OutputPtr], gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_BufferPointer, amount*sizeof(unsigned char));
		YUVBuffer.OutputPtr += amount;
		flush_dcache();
	#endif    
#else
    memcpy(startCodeBuffer.m_BufferPointer,MgrMPC.CurrentEEMb.StartCode,STARTCODELEN);
    startCodeBuffer.m_amount += STARTCODELEN;
    startCodeBuffer.QueueBuffer();
    startCodeBuffer.ResetBuffer();
    if (amount)
    {
      gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->m_amount += amount;
      gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->QueueBuffer();
    }
#endif
}
