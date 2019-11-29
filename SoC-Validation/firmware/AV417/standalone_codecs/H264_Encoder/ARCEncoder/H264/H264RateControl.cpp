/* ------------------------------------------------------------------------ */
/* FILE:			H264RateControl.cpp										*/
/* DESCRIPTION:		H.264 rate control functions							*/
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

#include	<stdlib.h>
#include	<memory.h>
#include	<limits.h>

#include "CH264Encoder.h"
#include "H264EncodedPictureBuffer.h"

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
int h263_equivalent_lut[52] = {
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
 2, 2, 2, 3, 3, 3, 4, 4, 5, 5,
 6, 7, 8, 8,10,11,12,13,15,17,
19,21,24,27,30,31,31,31,31,31,
31,31
};/*
int h264_equivalent_lut[32] = {
 0,14,20,24,26,28,30,31,32,34,
34,35,36,37,37,38,38,39,40,40,
40,41,41,42,42,42,43,43,43,44,
44,44  
};*/
int h264_equivalent_lut2[64] = {
12, 13, 15, 17, 20, 22, 24, 25, 26, 27, 28, 29, 30, 30, 31, 31, 32, 33, 33, 34,
34, 35, 35, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 40, 40, 40, 40, 40,
40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 44, 44, 44, 
44, 44, 44, 44
};
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

	// Code all frames if fixed quantiser is specified
	if (m_FixedQuantiser) return true;

	// Get current buffer status - method depends on whether real-time or non-real-time encoding
	int buffer_status;

	if (m_RealTimeRateControl)
	{
		//buffer_status = (number_of_bits_in_buffer);

		// For real time encoding, put this value of buffer level into symbol buffer status
		//m_OverallBufferStatus = buffer_status;
		if(m_OverallBufferStatus < 0) 
			m_OverallBufferStatus = 0;	
		buffer_status = m_OverallBufferStatus;
	
	}
	else
	{
		// non-real-time control
		buffer_status = m_OverallBufferStatus;
	}

	// Code picture if buffer is below threshold 
	if (!m_CodeAllPictures)
	{
		int limit = (m_ReactionMult*m_AverageBitsPerFrame)>>1;//(m_AverageBitsPerFrame * PICTURE_DROP_THRESHOLD)
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


			if ((H264_I_SLICE != m_RateControlFrameSliceType) || m_SmallPictures)
			{
				m_BitsLeftInGOP += m_AverageBitsPerGOP;
			}

			if(m_BitsLeftInGOP > (1<<24))m_BitsLeftInGOP = (1<<24);
			else if(m_BitsLeftInGOP < -(1<<24))m_BitsLeftInGOP = -(1<<24);
		}

		if (H264_I_SLICE == m_RateControlFrameSliceType)
		{

			if(m_SmallPictures) 
			{

				m_TargetBitsForThisFrame = (((m_BitsLeftInGOP)*23)>>5);

				// jrmm - 23/32 is an approximation to 8/11 (assuming m_NumberPLeftInGOP is 1)
			}
			else 
			{			// compensate for not adding in m_AverageBitsPerGOP above
				m_TargetBitsForThisFrame = (((m_BitsLeftInGOP+m_AverageBitsPerGOP)*23)>>5);

			}
		
		}
		else
		{
			m_TargetBitsForThisFrame = m_BitsLeftInGOP;

		}

		if(m_TargetBitsForThisFrame < 0)
		{
			if (H264_I_SLICE == m_RateControlFrameSliceType)
			{ 
				if(m_IntraPictureFrequency == 1) m_TargetBitsForThisFrame = m_AverageBitsPerFrame;
				else m_TargetBitsForThisFrame = m_AverageBitsPerFrame<<1;
			}
			else 
			{
				m_TargetBitsForThisFrame = m_AverageBitsPerFrame>>1;
			}
		}


		int minimum_target = (m_AverageBitsPerFrame >>3);
		if (m_TargetBitsForThisFrame < minimum_target)
		{
			m_TargetBitsForThisFrame = minimum_target;
		}
		int maximum_target =( m_AverageBitsPerFrame<<3);
		if (m_TargetBitsForThisFrame > maximum_target)
		{
			m_TargetBitsForThisFrame = maximum_target;
		}

		if (m_OverallBufferStatus + m_TargetBitsForThisFrame <  m_AverageBitsPerFrame + (m_AverageBitsPerFrame>>1))
		{
			// Underflow likely, increase target bits
			m_TargetBitsForThisFrame = (( m_AverageBitsPerFrame + (m_AverageBitsPerFrame>>1)) - m_OverallBufferStatus);
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
	int h263_equivalent_q = h263_equivalent_lut[m_PreviousFrameAverageQuantiser];//3 * pow(2.0, (m_PreviousFrameAverageQuantiserDouble-24.0) / 6.0);
	int complexity = h263_equivalent_q * m_PictureBitCount;

	if (H264_I_SLICE == m_RateControlFrameSliceType)
	{
		
		m_IVirtualBuffer += m_PictureBitCount - m_TargetBitsForThisFrame;

		// update PVirtual_buffer as well - otherwise at the start its still based on data that 
		// has nothing to do with content of the sequence
		if(!m_SmallPictures) 
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
	if (m_FixedQuantiser)
	{
		int new_quant = m_FixedQuantiserValue;

		if (new_quant < MINIMUM_QUANTISER_INDEX)
		{
			new_quant = MINIMUM_QUANTISER_INDEX;
		}
		else if (new_quant > MAXIMUM_QUANTISER_INDEX)
		{
			new_quant = MAXIMUM_QUANTISER_INDEX;
		}

		m_RateControlMacroblockCount++;

		return new_quant;
	}

	// Rate control (rather than fixed QP)

	int buffer_status = 0;
	buffer_status += m_ThreadBufferStatus;
	

	int virtual_buffer_level;
	//double reaction_parameter;
	int new_quant_263;

	if (H264_I_SLICE == m_RateControlFrameSliceType)
	{
		virtual_buffer_level = m_IVirtualBuffer;
		//reaction_parameter = m_IReactionParameter;
	}
	else
	{

		if(m_SmallPictures) // i.e. not SD
		{
			virtual_buffer_level = m_PVirtualBuffer;
		}
		else
		{
			//virtual_buffer_level = buffer_status;
			virtual_buffer_level = (m_PVirtualBuffer + buffer_status)>>1;
		}

	}
	
	// Note the following will have problems with MBAFF and with multi-thread encoding
	virtual_buffer_level +=
		(buffer_status + (m_RateControlMacroblockCount * m_AverageBitsPerMacroblock) - m_StartOfPictureBufferStatus) -
				(m_TargetBitsForThisMB * m_RateControlMacroblockCount);

	// saturate if we are severely undershooting or overshooting
	if(virtual_buffer_level < -(1<<24) )
	{
		virtual_buffer_level = -(1<<24);
	}
	else if(virtual_buffer_level > (1<<24))
	{
		virtual_buffer_level = (1<<24);
	}

	//this bit worked ok except the first frame is too finely coded and we overshoot the required bit-rate
	if ((H264_I_SLICE == m_RateControlFrameSliceType) ||  (m_PVirtualBuffer == m_IVirtualBuffer) || m_SmallPictures)
	{
		if(virtual_buffer_level > m_ReactionParameter) new_quant_263 = 63;
		else if(virtual_buffer_level <= 0)new_quant_263 = 0;// might be so negative that following line underflows
		else new_quant_263 = (m_InvReactParX31* virtual_buffer_level*2 + (1<<23))>>24;
	}
	else 
	{
		if(buffer_status > m_ReactionParameter) new_quant_263 = 63;	
		else if(buffer_status <= 0) new_quant_263 = 0; // might be so negative that following line underflows
		else new_quant_263 =  (m_InvReactParX31* buffer_status*2  + (1<<23))>>24;
	}
	if(new_quant_263 > 63)new_quant_263 = 63; // maximum - we have overflowed in the calcs
	else if(new_quant_263 < 0) new_quant_263 = 0;




	// Convert : QP264 = 24 + 6 * log2(QP263/3)
	//new_quant_double = 14.49022 + 8.65617 * log(new_quant_double);  // This is natural log

	// Round to nearest integer

	int new_quant = h264_equivalent_lut2[new_quant_263];

#if	defined MAKE_H264_RATECONTROL_TRACE_FILE
	fprintf(m_RateConTraceFile,"%d,",new_quant);
#endif

	// Force quantiser into range
	if (new_quant < MINIMUM_QUANTISER_INDEX)
	{
		new_quant = MINIMUM_QUANTISER_INDEX;
	}
	else if (new_quant > MAXIMUM_QUANTISER_INDEX)
	{
		new_quant = MAXIMUM_QUANTISER_INDEX;
	}

	m_RateControlMacroblockCount++;

	return(new_quant);
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

