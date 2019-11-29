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
#include "MP4EncoderControl.h"
#include "ArcMP4SDMTables.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::FrameStartStatistics							*/
/* DESCRIPTION:	This performs initialisation for statistics.
				It resets all of the statistics. It should be called before 
				starting to encode a frame									*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::FrameStartStatistics(void)
{
	m_TotalIntra4x4Count = 0;
	m_TotalIntra16x16Count = 0;
	m_TotalInter16x16Count = 0;
	m_TotalInter16x8Count = 0;
	m_TotalInter8x16Count = 0;
	m_TotalInter8x8Count = 0;
	m_TotalSkippedCount = 0;
	m_TotalForcedSkipCount = 0;
	m_TotalSkip8x8Count = 0;
//	m_TotalDirectCount = 0;
	m_TotalQuantiser = 0;
	m_NumberOfQuantisers = 0;

	m_ThreadIntra4x4Count = 0;
	m_ThreadIntra16x16Count = 0;
	m_ThreadInter16x16Count = 0;
	m_ThreadInter16x8Count = 0;
	m_ThreadInter8x16Count = 0;
	m_ThreadInter8x8Count = 0;
	m_ThreadSkippedCount = 0;
	m_ThreadForcedSkipCount = 0;
	m_ThreadSkip8x8Count = 0;
	//m_ThreadDirectCount = 0;
	m_ThreadQuantiser = 0;
	m_ThreadNumberOfQuantisers = 0;


}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MacroblockStatistics							*/
/* DESCRIPTION:	This updates the statistics using information about the 
				given macroblock.											*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::MacroblockStatistics(int sIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[sIdx];

	switch (cb.MBCodingType)
	{
		case MBCT_MPEG4_INTRA : m_ThreadIntra16x16Count++;
			break;

		case MBCT_MPEG4_INTER16X16 : m_ThreadInter16x16Count++;
			break;

		case MBCT_MPEG4_INTER8X8 : 
			m_ThreadInter8x8Count++;
			break;

		case MBCT_MPEG4_SKIPPED : m_ThreadSkippedCount++;
			break;
	}

	m_ThreadQuantiser += cb.QuantValue;
	m_ThreadNumberOfQuantisers++;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::FrameEndStatistics							*/
/* DESCRIPTION:	This performs end of frame statistics functions
				It calculates the average quantiser for the frame, for 
				use in encoding the next frame.								*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Encoder::FrameEndStatistics(void)
{
	// Accumulate macroblock statistics
	m_TotalIntra4x4Count	+= m_ThreadIntra4x4Count;
	m_TotalIntra16x16Count	+= m_ThreadIntra16x16Count;
	m_TotalInter16x16Count	+= m_ThreadInter16x16Count;
	m_TotalInter16x8Count	+= m_ThreadInter16x8Count;
	m_TotalInter8x16Count	+= m_ThreadInter8x16Count;
	m_TotalInter8x8Count	+= m_ThreadInter8x8Count;
	m_TotalSkippedCount		+= m_ThreadSkippedCount;
	m_TotalForcedSkipCount  += m_ThreadForcedSkipCount;
	m_TotalSkip8x8Count     += m_ThreadSkip8x8Count;
	//m_TotalDirectCount		+= m_ThreadDirectCount;
	m_TotalQuantiser		+= m_ThreadQuantiser;
	m_NumberOfQuantisers	+= m_ThreadNumberOfQuantisers;
	
	if (0 != m_NumberOfQuantisers)
	{
		//m_PreviousFrameAverageQuantiserDouble = (double)(m_TotalQuantiser) / (double)(m_NumberOfQuantisers);
		//m_PreviousFrameAverageQuantiser = (unsigned int)((double)(m_TotalQuantiser) / (double)(m_NumberOfQuantisers) + 0.5);
		m_PreviousFrameAverageQuantiser = (m_TotalQuantiser*m_NumMBsRecip + (1<<19))>>20;
	}
	else
	{
		 // Not sure how this could ever occur, but prevent a crash
		//m_PreviousFrameAverageQuantiserDouble = 20;
		m_PreviousFrameAverageQuantiser = 8;
	}
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */

