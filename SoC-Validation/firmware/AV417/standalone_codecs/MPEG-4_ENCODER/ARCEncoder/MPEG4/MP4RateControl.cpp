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

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "CMP4Encoder.h"
#include "MP4EncodedPictureBuffer.h"
#include "math.h"

#define PICTURE_DROP_THRESHOLD				3	// drop picture if buffer is above m_AverageBitsPerFrame * PICTURE_DROP_THRESHOLD



//#ifdef _DEBUG
//#define MAKE_H264_RATECONTROL_TRACE_FILE
//#endif

#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
#define		WRITE_NAME_VALUE_PAIR(Name, Value)	{ if (0 != m_RateConTraceFile) fprintf(m_RateConTraceFile,"%s : %f\n",(Name),(Value)); }
#else
#define		WRITE_NAME_VALUE_PAIR(Name, Value)
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlInitialisation						*/
/* DESCRIPTION:	This performs initialisation for rate control.				*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::RateControlInitialisation(void)
{
	m_LastInputPictureNumber = 0;

#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
	m_RateConTraceFile = fopen("H264RateControlTraceFile.csv","w");
#endif

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlDestroy							*/
/* DESCRIPTION:	This destroys the rate control functionality.
				Currently this only deals with closing the trace file.		*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::RateControlDestroy(void)
{
#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
	if (0 != m_RateConTraceFile) fclose(m_RateConTraceFile);
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlCodeNextFrame						*/
/* DESCRIPTION:	makes decision whether to code next frame
				number_of_bits_in_buffer is zero for non-real-time encoding 
				and is the number of bits in the actual buffer for 
				real-time encoding. It is used in the rate control to 
				adjust the buffer level.									*/
/* RETURNS:		true if frame is to be coded								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::RateControlCodeNextFrame(unsigned int number_of_bits_in_buffer)
{
	// Note that m_RateControlFrameSliceType has not been set to m_SliceType yet
	/*if ((H264_ALL_I_SLICE == m_SliceType) ||
		(H264_I_SLICE == m_SliceType))
	{
		m_RateControlIntraPictureCorrection = 1.4;
	}
	else
	{
		if (m_RateControlIntraPictureCorrection > 1)
		{
			m_RateControlIntraPictureCorrection -= 0.1;
		}
	}*/
	// Code all frames if fixed quantiser is specified
	if (m_FixedQuantiser) return true;

	// Get current buffer status - method depends on whether real-time or non-real-time encoding
	int buffer_status;

	if (m_RealTimeRateControl)
	{
		buffer_status = (number_of_bits_in_buffer);

		// For real time encoding, put this value of buffer level into symbol buffer status
		m_OverallBufferStatus = buffer_status;
	}
	else
	{
		// non-real-time control
		buffer_status = m_OverallBufferStatus;
	}

	// Code picture if buffer is below threshold 
	if (!m_CodeAllPictures)
	{
		int limit = (m_ReactionMult*m_AverageBitsPerFrame)>>1;
		return (buffer_status < limit);
	}
	else
	{
		return true;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlDroppedFrames						*/
/* DESCRIPTION:	Informs rate control that frames have been missed			*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::RateControlDroppedFrames(unsigned int dropped_frames)
{
	if (dropped_frames > 0)
	{
		RateControlReduceBufferStatus(m_AverageBitsPerFrame * dropped_frames);
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlFrameStart							*/
/* DESCRIPTION:	informs rate control that this is start of new frame.
				When target_bit_count is zero (normal case), the initial 
				target number of bits per frame are set to the average 
				number of bits per frame.
				When target_bit_count is non zero (FASTNETS case), the given 
				value is used as the target number of bits per frame.		*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::RateControlFrameStart(unsigned int target_bit_count)
{
	m_RateControlMacroblockCount = 0;

	m_RateControlFrameSliceType = m_SliceType;

	if (0 == target_bit_count)
	{
		if (0 == m_TotalNumberLeftInGOP)
		{
			// Start of GOP, reset statistics
			m_NumberPLeftInGOP = m_NumberPPerGOP;
			m_TotalNumberLeftInGOP = m_TotalNumberPerGOP;
			if ((H264_I_SLICE != m_RateControlFrameSliceType) || (m_NumberOfMBs < 792))
			{
				m_BitsLeftInGOP += m_AverageBitsPerGOP;
			}
			if(m_BitsLeftInGOP > (1<<24))m_BitsLeftInGOP = (1<<24);
			else if(m_BitsLeftInGOP < -(1<<24))m_BitsLeftInGOP = -(1<<24);
		}

		if (H264_I_SLICE == m_RateControlFrameSliceType)
		{
				
			if(m_NumberOfMBs < 792)
			{
				m_TargetBitsForThisFrame = (((m_BitsLeftInGOP)*23)>>5);
			}
			else 
			{
				m_TargetBitsForThisFrame = (((m_BitsLeftInGOP+m_AverageBitsPerGOP)*23)>>5);
			}
			// jrmm - 23/32 is an approximation to 8/11 (assuming m_NumberPLeftInGOP is 1)
			// and Pcomplexity was originally 60*TargetRate/115 and Icomplexity was 160*TargetRate/115
			// hence, (1 + (1*Pcomplexity)/Icomplexity)) = 1 + 60/160 = 11/8
		}
		else
		{
			
			m_TargetBitsForThisFrame = m_BitsLeftInGOP;
			
		}
		if(m_TargetBitsForThisFrame < 0)
		{
			if (H264_I_SLICE == m_RateControlFrameSliceType)
			{ // weight the Iframe bits to P frames at 8:1				
				if(m_IntraPictureFrequency == 1) m_TargetBitsForThisFrame = m_AverageBitsPerFrame;
				else m_TargetBitsForThisFrame = m_AverageBitsPerFrame<<1;
			}
			else 
			{
				m_TargetBitsForThisFrame = m_AverageBitsPerFrame>>1;
			}
		}
		int minimum_target = m_AverageBitsPerFrame >>3;
		if (m_TargetBitsForThisFrame < minimum_target)
		{
			m_TargetBitsForThisFrame = minimum_target;
		}
		int maximum_target = m_AverageBitsPerFrame<<3;
		if (m_TargetBitsForThisFrame > maximum_target)
		{
			m_TargetBitsForThisFrame = maximum_target;
		}

		if (m_OverallBufferStatus + m_TargetBitsForThisFrame <  m_AverageBitsPerFrame + (m_AverageBitsPerFrame>>1))
		{
			// Underflow likely, increase target bits
			m_TargetBitsForThisFrame = ( m_AverageBitsPerFrame + (m_AverageBitsPerFrame>>1)) - m_OverallBufferStatus;
		}
	}
	else
	{
		m_TargetBitsForThisFrame = target_bit_count;
	}
	m_TargetBitsForThisMB = (m_TargetBitsForThisFrame*m_NumMBsRecip + (1<<19))>>20;
	m_StartOfPictureBufferStatus = m_OverallBufferStatus;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlFrameEnd							*/
/* DESCRIPTION:	Performs end of frame rate control functions, such as 
				calculating by how much the target bits per frame was 
				missed.														*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::RateControlFrameEnd(void)
{
	// Assume that FrameEndStatistics has already been called for this frame

	if (H264_I_SLICE == m_RateControlFrameSliceType)
	{

		m_IVirtualBuffer += m_PictureBitCount - m_TargetBitsForThisFrame;
		if(m_NumberOfMBs >792) 
		{
			m_PVirtualBuffer += m_PictureBitCount - m_TargetBitsForThisFrame;
		}
	}
	else
	{

		m_PVirtualBuffer += m_PictureBitCount - m_TargetBitsForThisFrame;
		m_NumberPLeftInGOP--;
	}

	m_TotalNumberLeftInGOP--;

	m_BitsLeftInGOP -= m_PictureBitCount;

#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
	if (0 != m_RateConTraceFile)
	{
		fprintf(m_RateConTraceFile,"%d,%d,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f\n",
			m_PictureOrderCount, m_RateControlFrameSliceType, m_PreviousFrameAverageQuantiserDouble,
			m_TargetBitsForThisFrame, m_PictureBitCount, m_BitsLeftInGOP,
			m_IComplexity, m_PComplexity, m_BComplexity,
			m_IVirtualBuffer, m_PVirtualBuffer, m_BVirtualBuffer);
	}
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlMacroblockEnd						*/
/* DESCRIPTION:	Performs end of macroblock rate control functions, such as 
				removing bits from the buffer status.						*/
/* RETURNS:		none.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::RateControlMacroblockEnd()
{
//	WRITE_NAME_VALUE_PAIR("RateControlMacroblockEnd: m_AverageBitsPerMacroblock = ",m_AverageBitsPerMacroblock)
	RateControlReduceThreadBufferStatus(m_AverageBitsPerMacroblock);
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::RateControlQuantSelect						*/
/* DESCRIPTION:	Selects quantiser to use								 	*/
/* RETURNS:		int: the selected quantiser index.							*/
/* ------------------------------------------------------------------------ */

int CH264Encoder::RateControlQuantSelect()
{
	int new_quant;

	if (m_FixedQuantiser)
	{
		int new_quant = m_FixedQuantiserValue;

		if (new_quant < MINIMUM_QUANTISER_INDEX_MPEG4)
		{
			new_quant = MINIMUM_QUANTISER_INDEX_MPEG4;
		}
		else if (new_quant > MAXIMUM_QUANTISER_INDEX_MPEG4)
		{
			new_quant = MAXIMUM_QUANTISER_INDEX_MPEG4;
		}

		m_RateControlMacroblockCount++;

		return new_quant;
	}

	// Rate control (rather than fixed QP)

	int buffer_status = 0;
	buffer_status += m_ThreadBufferStatus;
	

	int virtual_buffer_level;
	//double reaction_parameter;

	if (H264_I_SLICE == m_RateControlFrameSliceType)
	{
		virtual_buffer_level = m_IVirtualBuffer;
		
	}
	else
	{
		if(m_NumberOfMBs < 792) 
		{
			virtual_buffer_level = m_PVirtualBuffer;
		}
		else 
		{
			virtual_buffer_level = (m_PVirtualBuffer + buffer_status)>>1;
		
		}
	}

	// Note the following will have problems with MBAFF and with multi-thread encoding
	virtual_buffer_level +=
		(buffer_status + (m_RateControlMacroblockCount * m_AverageBitsPerMacroblock) - m_StartOfPictureBufferStatus) -
		(m_TargetBitsForThisMB * m_RateControlMacroblockCount );
	// saturate if we are severely undershooting or overshooting
	if(virtual_buffer_level < -(1<<24) )
	{
		virtual_buffer_level = -(1<<24);
	}
	else if(virtual_buffer_level > (1<<24))
	{
		virtual_buffer_level = (1<<24);
	}	
	if ((H264_I_SLICE == m_RateControlFrameSliceType) ||  (m_PVirtualBuffer == m_IVirtualBuffer) || (m_NumberOfMBs < 150))
	{
		if(virtual_buffer_level > m_ReactionParameter) new_quant = 31;
		else new_quant = (m_InvReactParX31* virtual_buffer_level + (1<<23))>>24;
	}
	else 
	{
		if(buffer_status > m_ReactionParameter) new_quant = 31;	
		else new_quant =  (m_InvReactParX31* buffer_status  + (1<<23))>>24;
	}



#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
	fprintf(m_RateConTraceFile,"%d,",new_quant);
#endif

	// Force quantiser into range
	if (new_quant < MINIMUM_QUANTISER_INDEX_MPEG4)
	{
		new_quant = MINIMUM_QUANTISER_INDEX_MPEG4;
	}
	else if (new_quant > MAXIMUM_QUANTISER_INDEX_MPEG4)
	{
		new_quant = MAXIMUM_QUANTISER_INDEX_MPEG4;
	}

	m_RateControlMacroblockCount++;

	return(new_quant);
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

