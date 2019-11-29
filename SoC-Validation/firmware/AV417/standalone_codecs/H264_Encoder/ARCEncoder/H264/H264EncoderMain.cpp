
/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderMain.cpp										*/
/* DESCRIPTION:		H.264 encoder main encoding function					*/
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


#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>
#include	<assert.h>

#include "CH264Encoder.h"
#include "H264EncoderControl.h"
#include "H264EncodedPictureBuffer.h"
#include "SymbolByteBuffer.h"
#include "picture_encoder.h"
#include "H264OffsetTables.h"
#include "H264Tables.h"
#include "H264Transform.h"
#include "H264EncoderTables.h"

#include "ArcFrameBuffers.h"
#include "ArcMotionEstimationModel.h"
#include "defines.h"



#include "ArcSDMTables.h"
#include "ArcChannelRoutines.h"

#include <arc/arc_avu.h>
#include "ArcSDMTables_MPA.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcTransformQuant_MPA.h"
#include "H264MotionEstimation_MPA.h"
#include "ArcIntraLumaPrediction_MPA.h"
#include "ArcIntraChromaPrediction_MPA.h"
#include "ArcDeblock_MPA.h"
extern void (*HandleServiceCall)();

#include "VRaptorInfo.h"

#include "ArcTimer.h"

#ifdef _DEBUG
#define MAKE_ENCODER_TRACE_FILE
#endif

#if	defined MAKE_ENCODER_TRACE_FILE
#define		WRITE_STRING(Name)	{ if (0 != m_GeneralEncoderTraceFile) fprintf(m_GeneralEncoderTraceFile, "%s\n",(Name)); }
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)	{ if (0 != m_GeneralEncoderTraceFile) fprintf(m_GeneralEncoderTraceFile,"%s %i\n",(Name), (Number)); }
#else
#define		WRITE_STRING(Name)
#define		WRITE_STRING_NUMBER_PAIR(Name, Number)
#endif


extern int gIntraValidFlag;


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncoderMainFrameStart							*/
/* DESCRIPTION:	This performs start of frame operations for encoding the
				current frame. This includes deciding whether to code or
				drop the frame.
				number_of_bits_in_buffer is zero for non-real-time encoding
				and is the number of bits in the actual buffer for
				real-time encoding. It is used in the rate control to
				adjust the buffer level.
				more_to_encode is set to false if the picture is dropped
				else to true.												*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::EncoderMainFrameStart(int number_of_bits_in_buffer,
										 bool& more_to_encode, int target_bit_count)
{
	more_to_encode = true;

	//////////////////////////////////////////////////////////////////////////
	// Coding order of frames
	//
	// There are two states, one in which in-coming pictures are stored for
	// later out of order encoding, and on in which out of order pictures
	// are retrieved from the store and are encoded.
	// The switch between the first and second states occurs when a key
	// frame is input to the encoder, that is, one with hierarchy number
	// equal to zero.
	// The switch from the second to the first state occurs when all stored
	// pictures have been encoded.
	//////////////////////////////////////////////////////////////////////////

	m_SourcePicture->m_fn = m_InputPictureNumber;
	m_InputPictureNumber++;

	// This is a key frame, so encode it now

	// Set the slice type by typecast - assumes that the order of both enum types is the same
	m_SliceType = (h264_slice_type_t)(m_PictureType);

	m_LastInputPictureNumber = m_SourcePicture->m_fn;

	// Make Picture drop decision here
	if (!RateControlCodeNextFrame(number_of_bits_in_buffer))
	{
		RateControlDroppedFrames(1);
		more_to_encode = false;

		// In P picture only coding, ensure coded pictures are numbered in sequence
		m_InputPictureNumber--;
		return;
	}


	FrameStartStatistics();

	// Start of picture rate control
	RateControlFrameStart(target_bit_count);

	//////////////////////////////////////////////////////////////////////////
	// Make frame/field coding decision
	//////////////////////////////////////////////////////////////////////////

	m_NumberOfMacroblocksInPicture =
		m_SequenceParameterSet->frame_width_in_mbs * m_SequenceParameterSet->frame_height_in_mbs;

    MgrMPC.CurrentEEMb.SetMB(m_NumberOfMacroblocksInPicture-1);
	
	//////////////////////////////////////////////////////////////////////////
	// Calculate Picture Order Count
	//////////////////////////////////////////////////////////////////////////
    if (H264_I_PICTURE == m_PictureType)
	{
		m_FrameNumber = 0;
		m_FrameNumberOffset = 0;
		m_IDR_POC_Offset = m_SourcePicture->m_fn;
	}


	// When encoding in frame mode only (H264_FRAME_CODING), content is assumed to be progressive
	// and top and bottom field POC are set to be the same. In all other cases top and bottom
	// field POC are different by one, with the order being given by m_TopFieldFirst. When this is true,
	// the top field is assumed to be output first, and have a POC one less than the bottom field POC.
	if ((0 == m_SequenceParameterSet->pic_order_cnt_type) || (1 == m_SequenceParameterSet->pic_order_cnt_type))
	{
		m_TopFieldOrderCount = 2 * (m_SourcePicture->m_fn - m_IDR_POC_Offset);
	}
	else
	{
		m_TopFieldOrderCount = (m_FrameNumberOffset + m_FrameNumber) << 1;
	}

}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncoderMainPictureStart						*/
/* DESCRIPTION:	This performs start of picture operations for encoding the
				current picture. This includes deciding whether to code or
				drop the picture, copying into the control list, motion
				estimation and making mode decisions, and coding the
				picture header.
				time_in_ms is zero for non-real-time encoding and is the
				actual time in milliseconds for real-time encoding. It is
				used in the rate control to adjust the buffer level.
				number_of_bits_in_buffer is zero for non-real-time encoding
				and is the number of bits in the actual buffer for
				real-time encoding. It is used in the rate control to
				adjust the buffer level.
				more_to_encode is set to false if the picture is dropped
				else to true.												*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncoderMainPictureStart(void)
{
	//////////////////////////////////////////////////////////////////////////
	// Set Picture Order Count
	//////////////////////////////////////////////////////////////////////////

	m_PictureOrderCount = m_TopFieldOrderCount;

	//////////////////////////////////////////////////////////////////////////
	// Select a picture parameter set
	//////////////////////////////////////////////////////////////////////////

	if (1 == m_NumberOfPictureParameterSets)
	{
		m_PictureParameterSetID = 0;
	}
	else
	{
		if (H264_P_SLICE == m_SliceType)
		{
			m_PictureParameterSetID = 1;
		}
		else
		{
			m_PictureParameterSetID = 0;
		}
	}

	m_PictureParameterSet = m_ParameterSets->ReadPictureParameterSet(m_PictureParameterSetID);
	assert(0 != m_PictureParameterSet);

	//////////////////////////////////////////////////////////////////////////
	// Initialise m_CurrentSymbolByteBuffer
	//////////////////////////////////////////////////////////////////////////

	int j;

	for (j = 0; j < m_PictureParameterSet->num_slice_groups; j++)
	{
		m_CurrentSymbolByteBuffer[j] = m_SymbolByteBufferList[j];
	}

	//////////////////////////////////////////////////////////////////////////
	// Calculate POC related information
	//////////////////////////////////////////////////////////////////////////

	if (0 == m_SequenceParameterSet->pic_order_cnt_type)
	{
		m_PicOrderCountLSB = m_TopFieldOrderCount & (m_SequenceParameterSet->max_pic_order_cnt_lsb - 1);
		}
	else if (1 == m_SequenceParameterSet->pic_order_cnt_type)
	{
		// Calculate the absolute frame number by adding the LSBs in m_FrameNumber to the
		// MSBs in m_VLDSliceHeaderPreviousFrameNumberOffset
		unsigned int absolute_frame_number;
		if (0 != m_SequenceParameterSet->num_ref_frames_in_pic_order_cnt_cycle)
		{
			absolute_frame_number = m_FrameNumberOffset + m_FrameNumber;
		}
		else
		{
			absolute_frame_number = 0;
		}


		// Derive pic_order_cnt_cycle_cnt and frame_num_in_pic_order_cnt_cycle
		unsigned int pic_order_cnt_cycle_cnt = 0;
		unsigned int frame_num_in_pic_order_cnt_cycle = 0;
		int expected_pic_order_cnt = 0;

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

			for (unsigned int i = 0; i <= frame_num_in_pic_order_cnt_cycle; i++)
			{
				expected_pic_order_cnt += m_SequenceParameterSet->offset_for_ref_frame[i];
			}
		}


		m_DeltaPicOrderCount = m_TopFieldOrderCount - expected_pic_order_cnt;


		// If delta[0] is non zero, flag must indicate not always zero to enable delta[0] to be sent
		assert((0 == m_DeltaPicOrderCount) || (!m_SequenceParameterSet->delta_pic_order_always_zero_flag));

	}

	if (!m_EncoderControl->SetSize(m_SourcePicture->m_hsize, m_SourcePicture->m_vsize))
	{
		assert(false);
		return false;
	}


	if (!EncodePictureMainAllocateSliceGroups())
	{
		assert(false);
		return false;
	}


	m_EncodedPictureBuffer->GetPictureForEncoding(&m_EncodedPicture);

	assert(0 != m_EncodedPicture);


	m_EncodedPicture->set_source_frame_size(m_SourcePicture->m_hsize, m_SourcePicture->m_vsize);

   unsigned char *ty, *tu, *tv;

   gArcFrameBuffers.GetOutputPic(&ty,&tu,&tv);

	m_EncodedPicture->set_yuv_buffers(ty,tu,tv);

	// Create Remapped indices and calculate number of reference pictures in each list
	if (H264_I_SLICE == m_SliceType)
	{
		m_SliceNumList0RefPictures = 0;
	}
	else
	{
		// P or SP slice
		m_SliceNumList0RefPictures = 1;
	}
	

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePictureMainAllocateSliceGroups			*/
/* DESCRIPTION:	This associates a slice group ID with each macroblock in
				the picture. The allocation is done as in the picture
				parameter set.												*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePictureMainAllocateSliceGroups(void)
{
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

    int numBlocks = m_NumberOfMacroblocksInPicture;//(m_EncoderControl->m_hsize * m_EncoderControl->m_vsize)>>8;
	
	if ((m_PictureParameterSet->slice_group_map_type >= 3) &&
		(m_PictureParameterSet->slice_group_map_type <= 5))
	{
		m_SliceGroupChangeCycle = m_InputPictureNumber %			// Can be any value so long as it's the same within a picture.  Want it to change, but only for testing
			((numBlocks / m_PictureParameterSet->slice_group_change_rate) + 1);
	}


	if (0 == m_PictureParameterSet->slice_group_map_type)
	{
		// Interleaved slice groups. Loop over macroblocks
		int slice_group = 0;
		int macroblocks_left_in_slice_group = m_PictureParameterSet->slice_group_run_length[slice_group];

		// Loop over macroblocks
        int mbNum = 0;
		while (mbNum < numBlocks)
		{
            MgrMPC.Picture.SliceGroupMap[mbNum] = slice_group;

			macroblocks_left_in_slice_group--;

			if (0 == macroblocks_left_in_slice_group)
			{
				slice_group++;

				if (slice_group == m_PictureParameterSet->num_slice_groups) slice_group = 0;

				macroblocks_left_in_slice_group = m_PictureParameterSet->slice_group_run_length[slice_group];
			}

            mbNum++;
		}
	}
	else if (1 == m_PictureParameterSet->slice_group_map_type)
	{
		// Dispersed slice groups. Loop over macroblocks
		int col = 0;
		int row = 0;
        int mbNum = 0;
		while (mbNum < numBlocks)
		{
			MgrMPC.Picture.SliceGroupMap[mbNum] = (col +
				((row * m_PictureParameterSet->num_slice_groups) >> 1))
					% m_PictureParameterSet->num_slice_groups;

			if(col++ == m_EncoderControl->m_MacroblocksPerRow)
			{
				col = 0;
				row++;
		}
            mbNum++;
	}
	}
	else if (2 == m_PictureParameterSet->slice_group_map_type)
	{
		// Foreground and left over slice groups.

		// First assign all macroblocks to the background slice group, then proceed
		// through foreground slice groups from largest numbered to smallest numbered,
		// assigning macroblocks to slice groups. A macroblock is assigned to the
		// smallest slice group if it is in more than one foreground area.

		int top_left_x = 0;
		int top_left_y = 0;
		int bottom_right_x = 0;
		int bottom_right_y = 0;

		for (int slice_group = m_PictureParameterSet->num_slice_groups-1; slice_group >= 0; slice_group--)
		{
			if (slice_group != m_PictureParameterSet->num_slice_groups-1)
			{
				// A foreground slice group
				top_left_y = m_PictureParameterSet->slice_group_top_left[slice_group] / m_EncoderControl->m_MacroblocksPerRow;
				top_left_x = m_PictureParameterSet->slice_group_top_left[slice_group] % m_EncoderControl->m_MacroblocksPerRow;
				bottom_right_y = m_PictureParameterSet->slice_group_bottom_right[slice_group] / m_EncoderControl->m_MacroblocksPerRow;
				bottom_right_x = m_PictureParameterSet->slice_group_bottom_right[slice_group] % m_EncoderControl->m_MacroblocksPerRow;
			}

			// Loop over macroblocks
			int col = 0;
			int row = 0;
            int mbNum = 0;
			while (mbNum < numBlocks)
			{
				if (slice_group == m_PictureParameterSet->num_slice_groups-1)
				{
					// Background slice group
					MgrMPC.Picture.SliceGroupMap[mbNum] = slice_group;
				}
				else
				{
					// Foreground slice
					if ((col >= top_left_x) && (col <= bottom_right_x) &&
						(row >= top_left_y) && (row <= bottom_right_y))
					{
						// Macroblock is in this foreground area
						MgrMPC.Picture.SliceGroupMap[mbNum] = slice_group;
					}
				}

				if(col++ == m_EncoderControl->m_MacroblocksPerRow)
				{
					col = 0;
					row++;
				}
                mbNum++;
			}
		}
	}
	else if (3 == m_PictureParameterSet->slice_group_map_type)
	{
		// Box out slice groups.


		}
	else if (4 == m_PictureParameterSet->slice_group_map_type)
	{
		// Raster scan slice groups.

		// Calculate the number of macroblocks in the upper left slice group, then loop
		// over all the macroblocks and set according to the count and the direction flag.

		int macroblocks_in_slice_group0 =
			m_PictureParameterSet->slice_group_change_rate * m_SliceGroupChangeCycle;

		int size_of_upper_left_slice_group;

		if (0 != m_PictureParameterSet->slice_group_change_direction_flag)
		{
			size_of_upper_left_slice_group =
				m_EncoderControl->m_NumberOfMacroblocks - macroblocks_in_slice_group0;
		}
		else
		{
			size_of_upper_left_slice_group = macroblocks_in_slice_group0;
		}

		int numMB = 0;
		while (numMB < numBlocks)
		{
			if (numMB < size_of_upper_left_slice_group)
			{
				MgrMPC.Picture.SliceGroupMap[numMB] = m_PictureParameterSet->slice_group_change_direction_flag;
			}
			else
			{
				MgrMPC.Picture.SliceGroupMap[numMB] = 1 - m_PictureParameterSet->slice_group_change_direction_flag;
			}

			numMB++;
		}
	}
	else if (5 == m_PictureParameterSet->slice_group_map_type)
	{
		// Wipe slice groups.

		// Calculate the number of macroblocks in the upper left slice group, then loop
		// over all the macroblocks and set according to the count and the direction flag.

		int macroblocks_in_slice_group0 =
			m_PictureParameterSet->slice_group_change_rate * m_SliceGroupChangeCycle;

		int size_of_upper_left_slice_group;

		if (0 != m_PictureParameterSet->slice_group_change_direction_flag)
		{
			size_of_upper_left_slice_group =
				m_EncoderControl->m_NumberOfMacroblocks - macroblocks_in_slice_group0;
		}
		else
		{
			size_of_upper_left_slice_group = macroblocks_in_slice_group0;
		}


		int macroblocks_per_column =
			m_EncoderControl->m_NumberOfMacroblocks / m_EncoderControl->m_MacroblocksPerRow;

        int col = 0;
        int row = 0;
        int mbNum = 0;
		while (mbNum < numBlocks)
		{
			int wipe_macroblock_number = row + (col * macroblocks_per_column);
			if (wipe_macroblock_number < size_of_upper_left_slice_group)
			{
				MgrMPC.Picture.SliceGroupMap[mbNum] = m_PictureParameterSet->slice_group_change_direction_flag;
			}
			else
			{
				MgrMPC.Picture.SliceGroupMap[mbNum] = 1 - m_PictureParameterSet->slice_group_change_direction_flag;
			}

			if(col++ == m_EncoderControl->m_MacroblocksPerRow)
			{
				col = 0;
				row++;
		}
            mbNum++;
	}
	}
	else if (6 == m_PictureParameterSet->slice_group_map_type)
	{
		// Explicit slice groups.
		int numMB = 0;
		while (numMB < numBlocks)
		{
			MgrMPC.Picture.SliceGroupMap[numMB] = m_PictureParameterSet->slice_group_id[numMB];

			numMB++;
		}
	}
	else
	{
		assert(false);
		return false;
	}

	return true;
}


// Called from SIMD via channel on completion of intra
// sIdx		: Circular buffer index
CHANNEL_SERVICE Service_IntraComplete(int sIdx)
{
	MgrMPC.ActMPC[sIdx].IntraOk = true;
	if (MgrMPC.ActMPC[sIdx].MotionEstOk &&
	    MgrMPC.ActMPC[sIdx].SkippedOk)
	{
		cMPCcb &cb = WorkAreaSDM->CircularBuffs[sIdx];
		u_int8 mbct = cb.MBCodingType;

		if (mbct & (MBCT_H264_INTER + MBCT_H264_SKIPPED))
		{
			gCH264Encoder.ChooseInterOrIntra(sIdx);
		}
		else
		{
			if (mbct == MBCT_H264_INTRA16X16) // i.e. not skipped
			{
				gCH264Encoder.GetIntraLumaChromaPredictionAndError(sIdx);

				Service_StartME(0);

				SET_CallBackDoFwdTransforms_BufIdx(sIdx);
				_vrun(MACRO_CallBackDoFwdTransforms);
			}
		}
	}

}


// Service_UseInter 	Called from SIMD code via channel
//sIdx		: Circular buffer index
CHANNEL_SERVICE Service_UseInter(int sIdx)
{
    int col = MgrMPC.ActMPC[sIdx].Column;
    int leftIdx = MgrMPC.ActMPC[sIdx].LeftIdx;
    
	_vr00 = _vmovw(sIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
    // generates inter luma and chroma residuals
    _vrun(MACRO_MakeLumaChromaPredictionInterError);

	Service_StartME(0);

    SET_CallBackDoFwdTransforms_BufIdx(sIdx);
	_vrun(MACRO_CallBackDoFwdTransforms);
}



//Service_UseIntra 
// Called from SIMD code via channel
//sIdx		: Circular buffer index
CHANNEL_SERVICE Service_UseIntra(int sIdx)
{
	gCH264Encoder.GetIntraLumaChromaPredictionAndError(sIdx);

	Service_StartME(0);

	SET_CallBackDoFwdTransforms_BufIdx(sIdx);
	_vrun(MACRO_CallBackDoFwdTransforms);
}


// Set SIMD register with cost
_Asm void SetChooseInterOrIntra_bestInterCost(int cost)
{
	%reg cost;
	// REG_ChooseInterOrIntra_bestInterCost assumes VR02
	vmov.MSK_ChooseInterOrIntra_bestInterCost %VR02, cost;
}




// ChooseInterOrIntra
//Called from SIMD code via channel
// Inter / Intra decission
//cbIdx		: Circular buffer index
void CH264Encoder::ChooseInterOrIntra(int cbIdx)
{
   //printf("ie%d\n", cbIdx);
    int col = MgrMPC.ActMPC[cbIdx].Column;
    int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
    
	_vr00 = _vmovw(cbIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
//	SET_ChooseInterOrIntra_bestInterCost(bestInterCost);
	SetChooseInterOrIntra_bestInterCost(bestInterCost);
	SET_ChooseInterOrIntra_cIdx(cbIdx);
	_vrun(MACRO_ChooseInterOrIntra);				// Calls back Service_UseInter or Service_UseIntra
    
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	Service_InitTimer											*/
/* DESCRIPTION:	This function initialises the channel timer with a limit of n cycles
				and the address of a function to execute when the limit is reached.
				It is called at the start of processing of a frame and
				a couple of initialisations are also performed here			*/
/* RETURNS:		Void														*/
/* ------------------------------------------------------------------------ */
CHANNEL_SERVICE Service_InitTimer(int n)
{
	 unsigned long CTimerCtrlReg = _lr(AUXREG_TIMER_CTIMER_CTRL);
	 CTimerCtrlReg &= 0xfffffffe;
	 _sr(CTimerCtrlReg, AUXREG_TIMER_CTIMER_CTRL);// clear enable bit

	 _sr(0, AUXREG_TIMER_CTIMER_COUNT); //  set to 0
	 _sr(n, AUXREG_TIMER_CTIMER_LIMIT); // set to frequency decided  before = n
	 _sr(0, AUXREG_TIMER_CTIMER_HIT);// set to 0
	 _sr(0, AUXREG_TIMER_CTIMER_MSG0_CMD);//set to 0
	 _sr(63, AUXREG_TIMER_CTIMER_MSG1_CMD);

	// AUXREG_CTIMER_MSG1_DATA to function address
	 
	unsigned int addr_val = (unsigned int)Service_TimerLimitResponse;
	_sr(addr_val, AUXREG_TIMER_CTIMER_MSG1_DATA);

	gCH264Encoder.m_LastMBCount = 0;
	gCH264Encoder.m_StartProcessingTimer = 0;
	
#ifdef TIMER_OUTPUT
	printf("initTimer to %d cycles \n", n);
#endif // TIMER_OUTPUT
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:	Service_DisableTimer										*/
/* DESCRIPTION:	This function Disables the channel timer so that it is no	*/
/*				longer incremented. Called to avoid counting writing delays	*/
/* RETURNS:		Void														*/
/* ------------------------------------------------------------------------ */
CHANNEL_SERVICE Service_DisableTimer()
{
	 unsigned long CTimerCtrlReg = _lr(AUXREG_TIMER_CTIMER_CTRL);
	 CTimerCtrlReg &= 0xfffffffe;
	 _sr(CTimerCtrlReg, AUXREG_TIMER_CTIMER_CTRL);// clear enable bit
}
/* -----------------------------------------------------------------------  */
/* FUNCTION:	Service_EnableTimer											*/
/* DESCRIPTION:	This function re-Enables the channel timer so that it is 	*/
/*				incremented again - usually after a write					*/
/* RETURNS:		Void														*/
/* ------------------------------------------------------------------------ */
CHANNEL_SERVICE Service_EnableTimer()
{

	 _sr(0xd, AUXREG_TIMER_CTIMER_CTRL);
	// CTIMER_CTRL.ResetAtLimit to 1
	// CTIMER_CTRL.SendTicks to 1
	// CTIMER_CTRL.Enable to 1
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:	Service_StopTimer											*/
/* DESCRIPTION:	This function Disables the channel timer so that it is no	*/
/*				longer incremented.	It also resets the timer and the hit    */
/*				count to zero												*/
/* RETURNS:		Void														*/
/* ------------------------------------------------------------------------ */
CHANNEL_SERVICE Service_StopTimer()
{
#ifdef TIMER_OUTPUT
	if((gCH264Encoder.m_LastHitCount) * gCH264Encoder.m_MBAdaptFreq <=gCH264Encoder.m_LastMBCount )printf("frame completed in time-excess %d MBs\n",gCH264Encoder.m_LastMBCount- (gCH264Encoder.m_LastHitCount) * gCH264Encoder.m_MBAdaptFreq);
	else printf("frame failed budget deficiency %d MBs\n",gCH264Encoder.m_LastMBCount- (gCH264Encoder.m_LastHitCount) * gCH264Encoder.m_MBAdaptFreq);
#endif // TIMER_OUTPUT

	 unsigned long CTimerCtrlReg = _lr(AUXREG_TIMER_CTIMER_CTRL);
	 CTimerCtrlReg &= 0xfffffffe;
	 _sr(CTimerCtrlReg, AUXREG_TIMER_CTIMER_CTRL);// clear enable bit

	 _sr(0, AUXREG_TIMER_CTIMER_COUNT); //  set to 0
	 _sr(0, AUXREG_TIMER_CTIMER_HIT);// set to 0
	 _sr(0, AUXREG_TIMER_CTIMER_MSG0_CMD);//set to 0	
	 gCH264Encoder.m_StartProcessingTimer = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Service_TimerLimitResponse									*/
/* DESCRIPTION:	This function provides the main control of Dynamic encoding.*/
/*				It is called when the channel timer hits the previously set */
/*				limit.	There are some checks to make sure that it is		*/
/*				relevant to this frame and then the budgeted number of      */
/*				is calculated from the cycles used and the actual number    */
/*				is calculated from the mgrMPC to determine whether we are   */
/*				processing too slowly or too quickly.  This is done from the*/
/*				start of the frame, from the last call of this function and */
/*				uses whether we are in the first half of the frame and      */
/*				if any frame discrepancy can be made up in the remainder of */
/*				the frame.  These values are combined together and a delta  */
/*				panic value is derived.  This is added to the absolute panic*/
/*				value (in AdjustSettings) and used to set the coding tools.  */
/* RETURNS:		Void														*/
/* ------------------------------------------------------------------------ */

CHANNEL_SERVICE Service_TimerLimitResponse(unsigned int reg0)
{
	


	int hit_count;
	int comp, this_panic;
	int frame_win, slice_win, first_half, discrep_test;

	const int Panic_Levels[16] = {
		6, 4, 4, 2, 3, 1, 2, 0,
		2,-1, 0, 0,-4,-6,-2,-3  
	};

	// initialise to any value not refered to in Panic_Levels
	static int oldPanic = 0xff; 


	hit_count = reg0;//_lr(AUXREG_TIMER_CTIMER_HIT);

	if(!gCH264Encoder.m_StartProcessingTimer && (hit_count < gCH264Encoder.m_LastHitCount))gCH264Encoder.m_StartProcessingTimer = 1;

	if(gCH264Encoder.m_StartProcessingTimer) {
		gCH264Encoder.m_LastHitCount = hit_count;
		int mb_number = (MgrMPC.MacroBlocksCompleteCount + MgrMPC.MacroBlockStartedCount+1)>>1;

		int target = gCH264Encoder.m_MBAdaptFreq*hit_count;// mbs we should have used so far in frame
	
		int current_target = gCH264Encoder.m_MBAdaptFreq;// mbs we should have used since we last looked

		int discrepancy =  target - mb_number;
		
		int max_savings = ((gCH264Encoder.m_NumberOfMBs - mb_number))>>1; // assuming 50% savings per mb

		frame_win = (mb_number > target);
		slice_win = ((mb_number - gCH264Encoder.m_LastMBCount) > current_target);
		first_half = (mb_number + mb_number < gCH264Encoder.m_NumberOfMBs);
		discrep_test = (discrepancy < max_savings);

		comp = (frame_win<<3) + (slice_win<<2) + (first_half<<1) + discrep_test;

		this_panic = Panic_Levels[comp];

		// only adjust settings if dynamic encoding and they have changed since
		// last time
		if(gCH264Encoder.m_DynamicControl && this_panic != oldPanic) 
		{
			gCH264Encoder.AdjustSettings(this_panic);
			oldPanic = this_panic;
#ifdef TIMER_OUTPUT
			printf("mb_num %d (%d-%d), hitcount %d target %d, this_panic = %d (comp %d), new Panic = %d\n",mb_number,MgrMPC.MacroBlocksCompleteCount , MgrMPC.MacroBlockStartedCount,hit_count,target, this_panic, comp, gCH264Encoder.m_PanicLevel);
#endif // TIMER_OUTPUT
		}
		gCH264Encoder.m_LastMBCount = mb_number;
	}
}





// Setup SIMD code for transforms called from MACRO_CallBackDoFwdTransforms
//cbIdx		:	Circular buffer index
CHANNEL_SERVICE Service_DoFwdTransforms(int cbIdx)
{

	int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
	_vr00 = _vmovw(cbIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].Column, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
//	MgrMPC.IdxTransform = cbIdx;
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];
	u_int8 bType =  cb.MBCodingType;
	// setup pointers for the current, left and above macroblocks in the simd
    SET_SetupQuantValues_ChromaQpIndexOffset(gCH264Encoder.m_PictureParameterSet->chroma_qp_index_offset);
    //SET_SetupQuantValues_SliceType(gCH264Encoder.m_SliceType);    
    SET_SetupQuantValues_QuantVal(MgrMPC.ActMPC[cbIdx].QuantValue);
    _vrun(MACRO_SetupQuantValues); 
    
    if((bType & MBCT_H264_SKIPPED) == 0)
    {
		_vr01 = _vmovw(0, 1);     
		if (MBCT_H264_INTRA16X16 == bType)
			_vr01 = _vmovw(17, 1);
		
	    _vrun(MACRO_Forward4x4LumaTransformAndSmallCoeff);
	
		
		if (MBCT_H264_INTRA16X16 == bType)
		{
			_vrun(MACRO_TransformQuantiseLumaDC);		// Luminance DC transform and quantise
		}
		_vrun(MACRO_Forward4x4ChromaTransformAndSmallCoeff);
	    _vrun(MACRO_TransformQuantiseChromaDC);
    }

    SET_RevertToSkipped_LastTransQuant(gCH264Encoder.m_LastTransmittedQuant[cb.WorkArea.SliceGroup]);
    _vrun(MACRO_RevertToSkipped);
	
	MgrMPC.ActMPC[cbIdx].FwdTransformsOk = true;
	
	SET_CallBackDoInverseTransforms_BufIdx(cbIdx);
	_vrun(MACRO_CallBackDoInverseTransforms);
	
	SET_CallBackDoEntropyEncode_BufIdx(cbIdx);
	_vrun(MACRO_CallBackDoEntropyEncode);

}



// Called from MACRO_CallBackDoInverseTransforms
//cbIdx		:	Circular buffer index
CHANNEL_SERVICE Service_InverseTransforms(int cbIdx)
{
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];
	int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
	_vr00 = _vmovw(cbIdx, 1);
	_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].Column, 2);
	_vr00 = _vmovw(leftIdx, 4);
	_vrun(MACRO_SetCurrentMPC0);
	if (MBCT_H264_SKIPPED != (127 & cb.MBCodingType))
	{
		 _vr00 = _vmovw(0, 1);
		 if (MBCT_H264_INTRA16X16 == cb.MBCodingType)
			 _vr00 = _vmovw(17, 1);
		 
		_vrun(MACRO_InverseTransformDequantLumaCoeffs);
        _vrun(MACRO_InverseTransformDequantChromaCoeffs);
        
        //MgrMPC.IdxReconstruct = cbIdx;
        gCH264Encoder.ReconstructLuma16X16(cbIdx);
        gCH264Encoder.ReconstructChroma(cbIdx);
	}
	else
	{
        //MgrMPC.IdxReconstruct = cbIdx;
		gCH264Encoder.ReconstructSkipped(cbIdx);
	}
	MgrMPC.ActMPC[cbIdx].InvTransformsOk = true;
     
	// _vrun(MACRO_CopyToFixedAfterReconstructH264);
	
	if(MgrMPC.IntraStatus.Idx != NO_INTRA_SCHED &&
	   cbIdx == MgrMPC.ActMPC[MgrMPC.IntraStatus.Idx].LeftIdx)
	   // only start intra on globally scheduled index if the globally scheduled
	   // index's left neighbour matches the current (this) index which has just
	   // finished inverse transformation
	{
		DoIntraPrediction(MgrMPC.IntraStatus.ValidBits , MgrMPC.IntraStatus.Idx);
		MgrMPC.IntraStatus.Idx = NO_INTRA_SCHED;
	}
	SET_CallBackStartDeblock_BufIdx(cbIdx);
	_vrun(MACRO_CallBackStartDeblock);
	gCH264Encoder.MacroblockStatistics(cbIdx);
	
}



//Service_StartME
// Setup Motion estimator
// Can be called from SIMD or Arc code
// Dummy 	:	not used 	(debug tracking index) 
CHANNEL_SERVICE Service_StartME(int dummy)
{
	static int time_since_last_intra_refresh = 0;
	
	if(MgrMPC.MacroBlockStartedCount == gCH264Encoder.m_NumberOfMacroblocksInPicture)
	{
		return;
	}
	
	///////////////////////////////////////////////////////
    // Will be freed at end of deblock
    int cbIdx = MgrMPC.IdxSetupME = MgrMPC.AllocMPC();      // Allocate new MPC circular buffer entry
    if(cbIdx < 0 )
    {
    	MgrMPC.RestartCount++;
    	return;		// No free buffer
    }    
	
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];             // Current MPC cirluar buffer entry
	
	// set current slice group
	signed char currSliceGroup = MgrMPC.Picture.SliceGroupMap[MgrMPC.MacroBlockStartedCount];
	
	cb.WorkArea.SliceGroup = currSliceGroup;
	
    if (HwMe == 1)
    {
        WriteToChannelME_Hw(CHANREG_ME_COLLECTION_ID, gCH264Encoder.m_SliceID[currSliceGroup]);
    }
    else
    {
        WriteToChannelME_Sw(CHANREG_ME_COLLECTION_ID, gCH264Encoder.m_SliceID[currSliceGroup]);
    }

	gCH264Encoder.m_CurrentSliceGroup = currSliceGroup;
	
	gCH264Encoder.m_lastquant = gCH264Encoder.m_LastTransmittedQuant[currSliceGroup];

	// - 2*MAXIMUM_SIZE_OF_A_CODED_MACROBLOCK
	int bytesused = (gCH264Encoder.m_CurrentSymbolByteBuffer[gCH264Encoder.m_CurrentSliceGroup]->GetBitCount() >> 3);
	if (((bytesused+ h264_maximum_mb_bits[gCH264Encoder.m_lastquant]) >= (gCH264Encoder.m_MaximumNumberOfBytes))
		&& !MgrMPC.ActMPC[MgrMPC.CurrentEEMb.Index].BufferLimit && (bytesused > 15)) // at least one MB in slice besides header

	{

		MgrMPC.ActMPC[cbIdx].BufferLimit  = 1;
		MgrMPC.CurrentEEMb.Index = cbIdx;
		gCH264Encoder.m_SliceID[gCH264Encoder.m_CurrentSliceGroup] = ((gCH264Encoder.m_SliceID[gCH264Encoder.m_CurrentSliceGroup] + gCH264Encoder.m_NumberOfSliceGroupsForPPictures) &0xFF);
        //Service_ME_SetCollectionID((u_int32) gCH264Encoder.m_SliceID[gCH264Encoder.m_CurrentSliceGroup]);
 
	}


	if (gCH264Encoder.m_FirstMacroblockInSlice[cb.WorkArea.SliceGroup] == MgrMPC.MacroBlockStartedCount)
	{
		// This is the first macroblock in the slice, so code the slice header
		Service_WriteSliceHeader(MgrMPC.MacroBlockStartedCount, MgrMPC.MacroBlockStartedCount);
	}
	
    int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
    cMPCcb &lcb = WorkAreaSDM->CircularBuffs[leftIdx];
    cMPCfb &fb = WorkAreaSDM->FixedBuffs[MgrMPC.CurrentMeMB.Col];
    cMPCfb &lfb = WorkAreaSDM->FixedBuffs[MgrMPC.CurrentMeMB.Col-1];
    
    // setup quantiser
    cb.QuantValue = MgrMPC.ActMPC[cbIdx].QuantValue = gCH264Encoder.RateControlQuantSelect();

    // setup pointers for the current, left and above macroblocks in the simd
    SET_SetCurrentMPC0_cIdx(cbIdx);
    SET_SetCurrentMPC0_Column(MgrMPC.CurrentMeMB.Col);
    SET_SetCurrentMPC0_leftIdx(leftIdx);
    _vrun(MACRO_SetCurrentMPC0);

    // perform input chroma dmas
    _vr00 = _vmovw(MgrMPC.CurrentMeMB.Col, 0x01);
    _vr00 = _vmovw(MgrMPC.CurrentMeMB.Row, 0x02);
    _vrun(MACRO_CopyCurrentChromaToSDM);

    
    MgrMPC.MacroBlockStartedCount++;

    
	if ((H264_I_SLICE != gCH264Encoder.m_SliceType)&& (time_since_last_intra_refresh != gCH264Encoder.m_IntraMBRefreshRate-1))
	{	// P_SLICE
		MgrMPC.IntraStatus.Enable = !gCH264Encoder.m_NoIntraInPSlice;
        MgrMPC.MeQueue.Push(MgrMPC.IdxSetupME); // Queue up index going into ME
		
        // kick off me for inter prediction
        gCH264Encoder.bestInterCost = 0x7fffffff;						// Inter very bad
        Service_ME_InterIteration(&cb, &lcb, &lfb);

	}	// P_SLICE
	else
	{	// I SLICE

		MgrMPC.IntraStatus.Enable = true;
      
      	MgrMPC.ActMPC[MgrMPC.IdxSetupME].MotionEstOk = true;	// Not needed for intra
      	MgrMPC.ActMPC[MgrMPC.IdxSetupME].SkippedOk = true;	// Not needed
      	
        MgrMPC.MeQueue.Push(MgrMPC.IdxSetupME); // Queue up index going into ME

        // kick off me to fetch current and setup mpc for intra
        Service_ME_IntraIteration(&cb, &lcb, &lfb);

	}	// I SLICE


	if((time_since_last_intra_refresh == gCH264Encoder.m_IntraMBRefreshRate-1) ||(gCH264Encoder.m_IntraMBRefreshRate == 0))
	{
		time_since_last_intra_refresh = 0;
	}
	else
	{
		time_since_last_intra_refresh ++;
	}

}







/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePictureMainPictureEnd					*/
/* DESCRIPTION:	This performs end of picture operations for encoding the
				current picture. This includes putting the encoded picture
				into the reference buffer, loop filtering and making
				upsampled pictures for future motion estimation.			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePictureMainPictureEnd(void)
{
		////////////////////////////
		// Finished encoding frame
		////////////////////////////

		FrameEndStatistics();

		// Now output frames. There are two modes of operation.
		// When pictures are coded in order, one is output as one is encoded.
		// Otherwise the bumping method is used to output pictures

			// No re-ordering of pictures, so output one now
			CBTPicture * output_picture = 0;
			m_EncodedPictureBuffer->OutputFrame(&output_picture);
			SetReturnParameters(m_EncodedPicture, m_SourcePicture, output_picture);

		// Record information about this picture, to be used when encoding the next picture
		// First save info about current picture for use by switch stream encoders
        if (H264_I_PICTURE == m_PictureType)
		{
			m_IDRPictureID = (m_IDRPictureID + 1) & 3;  // 3 is our hard coded choice for efficiency

			m_FrameNumber = 1;
			m_FrameNumberOffset = 0;
		}
		else
		{
			m_FrameNumber++;
			if (m_FrameNumber > m_SequenceParameterSet->maximum_frame_number-1)
			{
				m_FrameNumber = 0;
				m_FrameNumberOffset += m_SequenceParameterSet->maximum_frame_number;
			}
		}


	m_EncodedPicture = 0;  // This is not needed, but may help with bug finding

	return true;
}



/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
