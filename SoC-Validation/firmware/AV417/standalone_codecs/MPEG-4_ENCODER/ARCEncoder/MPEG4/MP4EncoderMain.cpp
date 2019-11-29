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
#include	<assert.h>

#include "CMP4Encoder.h"
#include "MP4EncoderControl.h"
#include "MP4EncodedPictureBuffer.h"
#include "SymbolByteBuffer.h"
#include "picture_encoder.h"
#include "ArcEEOffsetTables.h"
#include "MP4Tables.h"
#include "MP4Transform.h"
#include "MP4EncoderTables.h"


#include "ArcFrameBuffers.h"
#include "ArcMotionEstimationModel.h"
#include "defines.h"
#include "ArcChannelRoutines.h"
#include "ArcMP4SDMTables.h"

#include "ArcSDMTables_MPA.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcFDCT_MPA.h"
#include "ArcIDCT_MPA.h"
#include "ArcMP4DeQuant_MPA.h"
#include "ArcH263DeQuant_MPA.h"
#include "ArcEncoderAddSubtract_MPA.h"
#include "ArcMp4IntraLumaPrediction_MPA.h"
extern void (*HandleServiceCall)();


#include "ArcTimer.h"


#ifdef ARC_DCT
extern void CreoFdct (short *, int stride, unsigned short *scaletable, int DCT_round, int cmax);
extern void IntraCreoFdct (unsigned char *in, short *block, int in_stride, int out_stride, unsigned short *scaletable, int DCT_round, int cmax);
extern void CreoIdct (short *, int stride);
extern   unsigned short aanscaletable[2][32][64];
extern unsigned short y_dc_aan_scale_table[32];
extern unsigned short c_dc_aan_scale_table[32];
extern unsigned short MP4RawBlockOffset[6];
extern unsigned short MP4DiffBlockOffset[6];
extern unsigned short MP4PredBlockOffset[6];
extern unsigned short MP4FinalBlockOffset[6];
#else // ARC_DCT
int Dct( unsigned char *blkIn, short *blkOut, int sIn, int sOut);
int Dct( short *blkIn, short *blkOut, int sIn, int sOut);
void idct(short *blk, int stride);
#endif // ARC_DCT

extern unsigned short y_dc_scale_table_shift15[32];
extern unsigned short c_dc_scale_table_shift15[32];
extern unsigned int y_dc_scale_table_shift20[32];
extern unsigned int c_dc_scale_table_shift20[32];

extern short ff_mpeg4_default_intra_matrix_shift15[64];
extern short ff_mpeg4_default_non_intra_matrix_shift15[64];
extern unsigned short mpeg4_quantiser_table_shift15[32];


extern const unsigned char y_dc_scale_table[32];
extern const unsigned char c_dc_scale_table[32];
extern  const short ff_mpeg4_default_intra_matrix[64];
extern  const short ff_mpeg4_default_non_intra_matrix[64];

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

//#include "MP4MotionVectorList.h"

#define MAXIMUM_COEFFICIENT_LEVEL 2047
//#define max(a,b) (a > b ? a : b)
//FILE * frame_field_file = fopen("frame_field.csv", "w");

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
#ifdef _DEBUG_VRAPTORISATION
	printf("\n\tFrame# %4d\n\n", m_InputPictureNumber);
#endif	
	m_InputPictureNumber++;

	// This is a key frame, so encode it now

	// Set the slice type by typecast - assumes that the order of both enum types is the same
	m_SliceType = (h264_slice_type_t)(m_PictureType);

	if (m_StoreForReference)
	{
		// Assume for now that all I pictures are IDR. Will also need to consider retransmitting parameter sets in case of IDR
		if (H264_I_PICTURE == m_PictureType)
		{
			m_NALReferenceIDC = 3;
			m_NALUnitType = H264_CODED_SLICE_IDR;
		}
		else
		{
			m_NALReferenceIDC = 2;
			m_NALUnitType = H264_CODED_SLICE_NO_DP;
		}
	}
	else
	{
		m_NALReferenceIDC = 0;
		m_NALUnitType = H264_CODED_SLICE_NO_DP;
	}




	m_LastInputPictureNumber = m_SourcePicture->m_fn;

	// Make Picture drop decision here
	if (!RateControlCodeNextFrame(number_of_bits_in_buffer))
	{
		RateControlDroppedFrames(1);
		more_to_encode = false;

		// In P picture only coding, ensure coded pictures are numbered in sequence
		//m_InputPictureNumber--;
		return;
	}

	FrameStartStatistics();

	// Start of picture rate control
	RateControlFrameStart(target_bit_count);

	//////////////////////////////////////////////////////////////////////////
	// Make frame/field coding decision
	//////////////////////////////////////////////////////////////////////////

	m_NumberOfMacroblocksInPicture =
		m_frame_width_in_mbs * m_frame_height_in_mbs;

    MgrMPC.CurrentEEMb.SetMB(m_NumberOfMacroblocksInPicture-1);



	//////////////////////////////////////////////////////////////////////////
	// Calculate Picture Order Count
	//////////////////////////////////////////////////////////////////////////

	if (H264_CODED_SLICE_IDR == m_NALUnitType)
	{
		m_FrameNumber = 0;
		m_FrameNumberOffset = 0;
		m_IDR_POC_Offset = m_SourcePicture->m_fn;
	}

	int picture_order_count;

		picture_order_count = 2 * (m_SourcePicture->m_fn - m_IDR_POC_Offset);

	// When encoding in frame mode only (H264_FRAME_CODING), content is assumed to be progressive
	// and top and bottom field POC are set to be the same. In all other cases top and bottom
	// field POC are different by one, with the order being given by m_TopFieldFirst. When this is true,
	// the top field is assumed to be output first, and have a POC one less than the bottom field POC.
	{
		m_TopFieldOrderCount = picture_order_count;
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
	// Initialise m_CurrentSymbolByteBuffer
	//////////////////////////////////////////////////////////////////////////

	int j;

	for (j = 0; j < 1; j++)
	{
		m_CurrentSymbolByteBuffer[j] = m_SymbolByteBufferList[j];
	}

		// Calculate the absolute frame number by adding the LSBs in m_FrameNumber to the
		// MSBs in m_VLDSliceHeaderPreviousFrameNumberOffset
		unsigned int absolute_frame_number;
			absolute_frame_number = m_FrameNumberOffset + m_FrameNumber;

		if ((!m_StoreForReference) && (absolute_frame_number > 0))
		{
			absolute_frame_number--;
		}

	if (!m_EncoderControl->SetSize(m_SourcePicture->m_hsize, m_SourcePicture->m_vsize))
	{
		assert(false);
		return false;
	}

    // This is used to indicate not assigned a slice ID yet
    Service_ME_SetCollectionID((u_int32) INT_MAX);

	m_EncodedPictureBuffer->GetPictureForEncoding(&m_EncodedPicture);

	assert(0 != m_EncodedPicture);


	m_EncodedPictureBuffer->SetEncodingPictureParameters(m_FrameNumber, m_PictureOrderCount,
														 m_TopFieldOrderCount);

	m_EncodedPicture->set_source_frame_size(m_SourcePicture->m_hsize, m_SourcePicture->m_vsize);

	unsigned char *ty, *tu, *tv;
	gArcFrameBuffers.GetOutputPic(&ty,&tu,&tv);
	m_EncodedPicture->set_yuv_buffers(ty,tu,tv);

	return true;
}


// Inter / Intra decission
void CH264Encoder::ChooseInterOrIntra(int cIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

    // How does the cost of the intra case compare with the cost of the inter case?
	int multiplier = 16;
#ifdef INTRA_NEIGH_BIAS
	if((cb.LeftMB.Flags &  MBNF_VALID) && (cb.LeftMB.Flags &  MBNF_INTRA) )
	{
		multiplier-=1;
	}
	if((cb.AboveMB.Flags &  MBNF_VALID) && (cb.AboveMB.Flags &  MBNF_INTRA) )
	{
		multiplier-=1;
	}
#endif //INTRA_NEIGH_BIAS

	if (m_bestInterCost > (multiplier * (unsigned int)cb.WorkArea.BestIntraLumaCost)>>4)
	{
		cb.MBCodingType = MBCT_MPEG4_INTRA;
	}
	
    if (MgrMPC.MacroBlockStartedCount < gCH264Encoder.m_NumberOfMacroblocksInPicture)
	{
		SET_CallBackStartME_BufIdx(0);
		_vrun(MACRO_CallBackStartME);
	}
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
		4, 6, 2, 4, 1, 3, 0, 2,
		1, 1, 0, 0,-2,-2,-4,-4
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


		int discrepancy = mb_number -target;
	
	
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


// Setup and kick off ME
CHANNEL_SERVICE Service_StartME(int dummy)
{
    int cbIdx = MgrMPC.IdxSetupME = MgrMPC.AllocMPC();      // Allocate new MPC circular buffer entry
                            // Will be freed at end of deblock

    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];             // Current MPC cirluar buffer entry
    
    Service_ME_SetCollectionID((u_int32) gCH264Encoder.m_SliceID[0]);

#ifdef ARC_EE_ENCODER
    gArcEntropyEncoderModel.SetMBNumber(MgrMPC.MacroBlockStartedCount);
#endif

    // If the bitstream buffer is nearly full then 
	int bytesused = (gCH264Encoder.m_CurrentSymbolByteBuffer[0]->GetBitCount() >> 3);

	if ((bytesused + (h263_maximum_mb_bits[gCH264Encoder.m_LastTransmittedQuant]>>2) >= (gCH264Encoder.m_MaximumNumberOfBytes))
		&& !MgrMPC.ActMPC[MgrMPC.CurrentEEMb.Index].BufferLimit && (bytesused >15))
	{
		MgrMPC.ActMPC[cbIdx].BufferLimit  = 1; // mark this MB as packet full
		MgrMPC.CurrentEEMb.Index = cbIdx;
		gCH264Encoder.m_SliceID[0] += MAXIMUM_NUMBER_OF_SLICE_GROUPS; // change slice number
		
	}
	if (gCH264Encoder.m_FirstMacroblockInSlice[0] == MgrMPC.MacroBlockStartedCount)
	{
		int quantValue = gCH264Encoder.RateControlQuantSelect();
		// This is the first macroblock in the slice, so code the slice header
		if(gCH264Encoder.m_coding_standard == MPEG4)  
		{
            Service_WriteMpeg4PictureHeader(quantValue);
		}
		else 
		{
            Service_WriteH263PictureHeader(quantValue);
		}
		gCH264Encoder.m_LastTransmittedQuant = quantValue;
	}
   /* if (gCH264Encoder.m_StartNewPacket)
    {
        gCH264Encoder.m_StartNewPacket = false;
		if(gCH264Encoder.m_coding_standard == MPEG4) 
		{
            Service_WriteMpeg4PacketHeader(MgrMPC.MacroBlockStartedCount, gCH264Encoder.m_LastTransmittedQuant);
		}
		else if(gCH264Encoder.m_h263_slice_structured)
		{
			Service_WriteH263GobHeader(gCH264Encoder.m_LastTransmittedQuant);
		}
	
		// Change the slice ID for the slice group
		gCH264Encoder.m_SliceID[0] += MAXIMUM_NUMBER_OF_SLICE_GROUPS;

        Service_ME_SetCollectionID((u_int32) gCH264Encoder.m_SliceID[0]);

		gCH264Encoder.m_FirstMacroblockInSlice[0] = MgrMPC.MacroBlockStartedCount;
	} // if packet size too big

	if ((gCH264Encoder.m_FirstMacroblockInSlice[0] == MgrMPC.MacroBlockStartedCount)
		|| (gCH264Encoder.m_h263_use_gob_headers && !MgrMPC.CurrentMeMB.Col && !(MgrMPC.CurrentMeMB.Row & (gCH264Encoder.m_h263_rows_per_gob-1)))
		)
	{
		// This is the first macroblock in the slice, so code the slice header
		unsigned int quantValue = gCH264Encoder.RateControlQuantSelect();
		//gCH264Encoder.m_BuffersToCopy++;

		if(gCH264Encoder.m_coding_standard == MPEG4)  
		{
			if(MgrMPC.MacroBlockStartedCount == 0) 
			{
                Service_WriteMpeg4PictureHeader(quantValue);
				gCH264Encoder.m_LastTransmittedQuant = quantValue;
			}
			else {
	            Service_WriteMpeg4PacketHeader(MgrMPC.MacroBlockStartedCount, gCH264Encoder.m_LastTransmittedQuant);
				gCH264Encoder.m_LastTransmittedQuant = quantValue;
			}
		}
		else // 263
		{
			if(MgrMPC.MacroBlockStartedCount == 0)
			{
                Service_WriteH263PictureHeader(quantValue);
				gCH264Encoder.m_LastTransmittedQuant = quantValue;
			}
			else if((MgrMPC.CurrentMeMB.Col == 0) && gCH264Encoder.m_h263_use_gob_headers && !(MgrMPC.CurrentMeMB.Row&(gCH264Encoder.m_h263_rows_per_gob-1)))
			{
                Service_WriteH263GobHeader(quantValue);
				gCH264Encoder.m_LastTransmittedQuant = quantValue;
				gCH264Encoder.m_SliceID[0] += MAXIMUM_NUMBER_OF_SLICE_GROUPS;
                Service_ME_SetCollectionID((u_int32) gCH264Encoder.m_SliceID[0]);
				gCH264Encoder.m_FirstMacroblockInSlice[0] = MgrMPC.MacroBlockStartedCount;
			}
		}
	}
*/


    ////////////////////////////////////////////////////////
	// Select Quantiser
	////////////////////////////////////////////////////////

    int luma_QP;
    luma_QP = gCH264Encoder.RateControlQuantSelect();
    cb.QuantValue = MgrMPC.ActMPC[MgrMPC.IdxSetupME].QuantValue = luma_QP;

    ////////////////////////////////////////////////////////
	// Copy source chroma data to MPC pixel buffer
    // Luma data will be transfered by the ME module
	////////////////////////////////////////////////////////
    _vr00 = _vmovw(MgrMPC.CurrentMeMB.Col, 0x01);
    _vr00 = _vmovw(MgrMPC.CurrentMeMB.Row, 0x02);
    _vrun(MACRO_CopyCurrentChromaToSDM);

    ////////////////////////////////////////////////////////
	// Kick off te ME
	////////////////////////////////////////////////////////
    int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
    cMPCcb &lcb = WorkAreaSDM->CircularBuffs[leftIdx];
    cMPCfb &fb = WorkAreaSDM->FixedBuffs[MgrMPC.CurrentMeMB.Col];
    cMPCfb &lfb = WorkAreaSDM->FixedBuffs[MgrMPC.CurrentMeMB.Col-1];
    MgrMPC.MacroBlockStartedCount++;

#ifdef _DEBUG_VRAPTORISATION
    printf("Service_StartME: MPC Index = %d\n", cbIdx);
#endif    
    
    MgrMPC.MeQueue.Push(cbIdx); // Queue up index going into ME
	if ((H264_I_SLICE != gCH264Encoder.m_SliceType) && (gCH264Encoder.m_time_since_last_intra_refresh != (gCH264Encoder.m_IntraMBRefreshRate-1)))
	{
        // Inter case
        Service_ME_InterIteration(&cb, &lcb, &lfb);
        gCH264Encoder.m_bestInterCost = 0x7fffffff;						// Inter very bad

    }
	else
	{ // Intra case
      	MgrMPC.ActMPC[cbIdx].MotionEstOk = true;	// Not needed for intra
      	MgrMPC.ActMPC[cbIdx].SkippedOk = true;	// Not needed

        // kick off me to fetch current and setup mpc for intra
        Service_ME_IntraIteration(&cb, &lcb, &lfb);
    }

    ////////////////////////////////////////////////////////
    // Increment Intra refresh interval
    ////////////////////////////////////////////////////////
	if((gCH264Encoder.m_time_since_last_intra_refresh == gCH264Encoder.m_IntraMBRefreshRate-1) ||(gCH264Encoder.m_IntraMBRefreshRate == 0))
	{
		gCH264Encoder.m_time_since_last_intra_refresh = 0;
	}
	else
	{
		gCH264Encoder.m_time_since_last_intra_refresh ++;
	}
}

// Called from SIMD via channel on completion of intra
CHANNEL_SERVICE Service_IntraComplete(int cIdx)
{
	MgrMPC.ActMPC[cIdx].IntraOk = true;
	int leftIdx = MgrMPC.ActMPC[cIdx].LeftIdx;
	int lastQuant;
	
#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_IntraComplete: MPC Index = %d\n", cIdx);
#endif
   	
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

	if( MgrMPC.ActMPC[cIdx].MB_number == gCH264Encoder.m_FirstMacroblockInSlice[0])
	{
		lastQuant = gCH264Encoder.m_LastTransmittedQuant;
	}
	else
	{
		lastQuant = MgrMPC.ActMPC[leftIdx].QuantValue;
	}
	int dquant = cb.QuantValue - lastQuant;
	if((gCH264Encoder.m_coding_standard >= MPEG4) && !gCH264Encoder.m_h263_modified_quant) // may need to restrict quantiser
	{
		if(dquant < -2)
		{
			dquant = -2; // limited
			cb.QuantValue = lastQuant + dquant;
		}
		if(dquant > 2)
		{
			dquant = 2; // limited
			cb.QuantValue = lastQuant + dquant;
		}
	}
	
    MgrMPC.ActMPC[cIdx].Dquant = dquant;
	MgrMPC.ActMPC[cIdx].QuantValue = cb.QuantValue;


	if ( MgrMPC.ActMPC[cIdx].MotionEstOk && 
		 MgrMPC.ActMPC[cIdx].SkippedOk )
	{
		if ((MBCT_MPEG4_INTER16X16 == cb.MBCodingType) ||
            (MBCT_MPEG4_INTER8X8 == cb.MBCodingType))
		{
			gCH264Encoder.ChooseInterOrIntra(cIdx);
		}

        // If we go for inter (skipped is dealt with in Service_CalculateSkippedChromaCost())
		if ((MBCT_MPEG4_INTER16X16 == cb.MBCodingType) ||
	            (MBCT_MPEG4_INTER8X8 == cb.MBCodingType))
        {
    	    gCH264Encoder.MakeInterChromaPrediction263(cIdx);

            // Y block
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_Y])), 0x01);           // current MB address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_Y])), 0x02);       // prediction address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_Y])), 0x04); // prediction error address
            _vrun(MACRO_MakeLumaPredictionError16X16);

            // U block
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_U])), 0x01);    // current U block address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U])), 0x02);  // U prediction address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_U])), 0x04);   // U prediction error address
            _vrun(MACRO_MakeChromaPredictionError);

            // V block
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_CURRENT_V])), 0x01);    // current V block address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V])), 0x02);  // V prediction address
            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_V])), 0x04);   // V prediction error address
            _vrun(MACRO_MakeChromaPredictionError);
        }

        // MPEG4 does not allow quant change with 4 MVs
        if((gCH264Encoder.m_coding_standard == MPEG4) && (cb.MBCodingType == MBCT_MPEG4_INTER8X8)
		    && (MgrMPC.ActMPC[cIdx].Dquant != 0))
	    {
            _vr00 = _vmovw(cIdx, 1);
	        _vr00 = _vmovw(MgrMPC.ActMPC[cIdx].LeftIdx, 2);
            _vr02 = _vmovw(MgrMPC.ActMPC[cIdx].Dquant, 1);
            _vrun(MACRO_RevertQuantValue);
		    MgrMPC.ActMPC[cIdx].Dquant = 0;
			MgrMPC.ActMPC[cIdx].QuantValue = cb.QuantValue ;
        }

		SET_CallBackDoFwdTransforms_BufIdx(cIdx);
		_vrun(MACRO_CallBackDoFwdTransforms);
	}

}



// Setup SIMD code for transforms
CHANNEL_SERVICE Service_DoFwdTransforms(int cbIdx)
{
	MgrMPC.IdxTransform = cbIdx;
	cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];
	int leftIdx = MgrMPC.ActMPC[cbIdx].LeftIdx;
	int lastQuant;
#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_DoFwdTransforms: MPC Index = %d\n", cbIdx);
#endif
   	
   	// -----------------------------------
    // Kick off ME for next MB so it'll be done in parallel with SIMD operations
    // of current block
	if ((H264_I_SLICE == gCH264Encoder.m_SliceType) &&	
		     (MgrMPC.MacroBlockStartedCount < gCH264Encoder.m_NumberOfMacroblocksInPicture) )
	{
		SET_CallBackStartME_BufIdx(0);
		_vrun(MACRO_CallBackStartME);
	}
    // ------------------------------------
	
	if( MgrMPC.ActMPC[cbIdx].MB_number == gCH264Encoder.m_FirstMacroblockInSlice[0])
	{
		lastQuant = gCH264Encoder.m_LastTransmittedQuant;
	}
	else
	{
		lastQuant = MgrMPC.ActMPC[leftIdx].QuantValue;
	}
	int dquant = cb.QuantValue - lastQuant;
	if((gCH264Encoder.m_coding_standard >= MPEG4) && !gCH264Encoder.m_h263_modified_quant) // may need to restrict quantiser
	{
		if(dquant < -2)
		{
			dquant = -2; // limited
			cb.QuantValue = lastQuant + dquant;
		}
		if(dquant > 2)
		{
			dquant = 2; // limited
			cb.QuantValue = lastQuant + dquant;
		}
	}
    
    MgrMPC.ActMPC[MgrMPC.IdxTransform].Dquant = dquant;
	MgrMPC.ActMPC[MgrMPC.IdxTransform].QuantValue = cb.QuantValue;
   
	if (cb.MBCodingType != MBCT_MPEG4_SKIPPED)
	{
		// DMA In quantisation table.
		// This can change on a MB basis.
		if ((cb.MBCodingType != gCH264Encoder.m_lastMBType) || 
				(((MgrMPC.CurrentMeMB.Row==0)|| (gCH264Encoder.m_h263_use_gob_headers) )&& (MgrMPC.CurrentMeMB.Col==0)) ||
				MgrMPC.ActMPC[cbIdx].Dquant || ( MgrMPC.ActMPC[cbIdx].MB_number == gCH264Encoder.m_FirstMacroblockInSlice[0]))
		{
			if(gCH264Encoder.m_coding_standard == MPEG4)
			{
				if (MBCT_MPEG4_INTRA == cb.MBCodingType)
				{
					SetupDmaQuant(cbIdx, SDMOF_mpeg4_dct_quant_scaling_table, (unsigned int)p_aanscaletables, 64*2);
					_vrun(MACRO_SdmDmaIn);
				}
				else
				{
					SetupDmaQuant(cbIdx, SDMOF_mpeg4_dct_quant_scaling_table + (64*2), (unsigned int)p_aanscaletables + 32*64*2, 64*2);
					_vrun(MACRO_SdmDmaIn);
				}
			}
			else // H263
			{
				if (MBCT_MPEG4_INTRA == cb.MBCodingType)
				{
					SetupDmaQuant(cbIdx, SDMOF_mpeg4_dct_quant_scaling_table, (unsigned int)p_aanscaletables263, 64*2);
					_vrun(MACRO_SdmDmaIn);
				}
				else
				{
					SetupDmaQuant(cbIdx, SDMOF_mpeg4_dct_quant_scaling_table + (64*2), (unsigned int)p_aanscaletables263 + 32*64*2, 64*2);
					_vrun(MACRO_SdmDmaIn);
				}
			}
		}

		// Store last MB's type
		gCH264Encoder.m_lastMBType = cb.MBCodingType;

		// setup pointers for the current, left and above macroblocks in the simd
		_vr00 = _vmovw(cbIdx, 1);
		_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].Column, 2);
		_vr00 = _vmovw(leftIdx, 4);
		_vrun(MACRO_SetCurrentMPC);

		// Kick off fwd DCT
		_vr00 = _vmovw((gCH264Encoder.m_coding_standard == MPEG4? 0x0000 : 0xFFFF), 8);
		_vrun(MACRO_FDCT);

		// Revert back to skipped if MBType=INTER16X16 and CBP==0 and MV=0 and DQuant=0
		SET_RevertToSkipped_cbIdx(cbIdx);
		SET_RevertToSkipped_dquant(MgrMPC.ActMPC[cbIdx].Dquant);
		SET_RevertToSkipped_LastTransQuant(lastQuant);//(gCH264Encoder.m_LastTransmittedQuant);
		_vrun(MACRO_RevertToSkipped);
		MgrMPC.ActMPC[cbIdx].QuantValue = cb.QuantValue;
		MgrMPC.ActMPC[cbIdx].Dquant = cb.QuantValue - lastQuant;
	}
	else
	{
		// For skipped MB's, there is no DCT, go straight into EE
        // Kick off ME for next MB to maximise parallelism
	    if (MgrMPC.MacroBlockStartedCount < gCH264Encoder.m_NumberOfMacroblocksInPicture)
		{
	    	SET_CallBackStartME_BufIdx(0);
	    	_vrun(MACRO_CallBackStartME);
		}
		SET_CallBackDoEntropyEncode_BufIdx(cbIdx);
		SET_CallBackDoEntropyEncode_dquant(0);
		SET_CallBackDoEntropyEncode_lastQuant(lastQuant);
		_vrun(MACRO_CallBackDoEntropyEncode);
		MgrMPC.ActMPC[cbIdx].QuantValue = lastQuant;
		MgrMPC.ActMPC[cbIdx].Dquant = 0;
	}
}


// Setup entropy encode
CHANNEL_SERVICE Service_DoEntropyEncode(int cbIdx, short dquant, short lastQuant)
{
	MgrMPC.IdxSetupEE = cbIdx;
    MgrMPC.ActMPC[cbIdx].FwdTransformsOk = true;
    MgrMPC.ActMPC[cbIdx].Dquant = dquant;
    gCH264Encoder.m_LastTransmittedQuant = lastQuant;
    MgrMPC.MacroBlockThroughSimdCount++;

#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_DoEntropyEncode: MPC Index = %d\n", cbIdx);
#endif


    MgrMPC.IdxReconstruct = cbIdx;    // Should be set by final channel call from transforms (Assumes overlap with EE)
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cbIdx];
    
	if ((MBCT_MPEG4_SKIPPED != cb.MBCodingType))
	{	
	    // setup pointers for the current, left and above macroblocks in the simd
		_vr00 = _vmovw(cbIdx, 1);
		_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].Column, 2);
		_vr00 = _vmovw(MgrMPC.ActMPC[cbIdx].LeftIdx, 4);
		_vrun(MACRO_SetCurrentMPC);
		if(gCH264Encoder.m_mpeg_quant)
		{
			//_vr00 = _vmovw((m_coding_standard == MPEG4? 0x0000 : 0xFFFF), 2);
			//nb 16, 32, 64 used for cirbuff, fixbuff etc
			_vrun(MACRO_MP4DeQuant);
		}
		else 
		{
			//nb 16, 32, 64 used for cirbuff, fixbuff etc
			_vrun(MACRO_H263DeQuant);
		}
	}
	else // skipped
	{
				// Because luma and chroma Xform blocks are contiguous in SDM, this macro
		// call will take care of both in one go
		_vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_REFERENCE_Y), 0x01);      // Y prediction block address in SDM
		_vr00 = _vmovw((unsigned int)(cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y), 0x02);  // Y reconstructed block address in SDM
		_vrun(MACRO_ReconstructSkipped);
	}

	
    if ((MgrMPC.MacroBlockThroughSimdCount==1) || MgrMPC.ActMPC[MgrMPC.IdxFromEE].EntropyEncodeOk)
    {
    	MgrMPC.EEQueue.Push(cbIdx);
    	Service_WriteMacroblockMP4(cbIdx, dquant);
    }
    else
    {
    	MgrMPC.ActMPC[cbIdx].EENotStarted = true;
    }

	
	SET_CallBackStartDeblock_BufIdx(cbIdx);
	_vrun(MACRO_CallBackStartDeblock);
	
	if (MgrMPC.MacroBlockThroughSimdCount == gCH264Encoder.m_NumberOfMacroblocksInPicture)
	{
		SET_CallBackStartDeblock_BufIdx(-1);
		_vrun(MACRO_CallBackStartDeblock);		
	}

}

CHANNEL_SERVICE Service_StartDeblock(int sIdx)
{
	if(sIdx >= 0 && sIdx < SDM_WorkArea::MAX_CIRCULAR_BUFFS)
	{
		MgrMPC.ActMPC[sIdx].InvTransformsOk = true;
	}		
	MgrMPC.MacroBlockThroughDblkCount++;


    if (sIdx >= 0)
        WorkAreaSDM->AddForDeblock(sIdx);	// TEMP Delay for SDM_WorkArea::DEBLOCK_Q_SIZE mb

	// Add temp delay
    short dbIdx = WorkAreaSDM->GetForDeblock();
    if(dbIdx >= 0)
	{
    	SET_CallBackWriteOutMB_BufIdx(dbIdx);
    	_vrun(MACRO_CallBackWriteOutMB);
	}

	
#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_StartDeblock: Pushed MPC Index = %d; Pulled MPC Idx = %d\n", sIdx, dbIdx);
#endif
   	
    // At the end of a MB iteration, kick off SIMD operation for the next MB.
    // For intra MB's, kick off the transforms etc.
    // For inter blocks, kick off intra cost calculation
    short nextIdx = sIdx + 1;
    if (nextIdx == MAX_CIRC_BUFFS)
    	nextIdx = 0;
    if ( MgrMPC.ActMPC[nextIdx].IsUsed &&
    	(MgrMPC.MacroBlockThroughDblkCount < gCH264Encoder.m_NumberOfMacroblocksInPicture) && 
    	 MgrMPC.ActMPC[nextIdx].LumaInpOk )
    {
        cMPCcb &cb = WorkAreaSDM->CircularBuffs[nextIdx];
        if (MBCT_MPEG4_INTRA == cb.MBCodingType)
        {
		    SET_CallBackDoFwdTransforms_BufIdx(nextIdx);
		    _vrun(MACRO_CallBackDoFwdTransforms);
        }
        else 
        {
	    // setup pointers for the current, left and above macroblocks in the simd
		    _vr00 = _vmovw(nextIdx, 1);
		    _vr00 = _vmovw(MgrMPC.ActMPC[nextIdx].Column, 2);
		    _vr00 = _vmovw(MgrMPC.ActMPC[nextIdx].LeftIdx, 4);
		    _vrun(MACRO_SetCurrentMPC);
    	    if(gCH264Encoder.m_coding_standard == MPEG4 ) 
    	    {
    		    _vrun(MACRO_IntraModeCostmp4);
    	    }
    	    else 
    	    {
    		    _vrun(MACRO_IntraModeCost263);
    	    }
		    SET_CallBackIntraComplete_BufIdx(nextIdx);
		    _vrun(MACRO_CallBackIntraComplete);
        }
    }
    
    if ((H264_I_SLICE != gCH264Encoder.m_SliceType) &&	
    	(MgrMPC.MacroBlockStartedCount < gCH264Encoder.m_NumberOfMacroblocksInPicture) && 
    	((gCH264Encoder.m_time_since_last_intra_refresh==0) && (gCH264Encoder.m_IntraMBRefreshRate!=0)) )
    {
		SET_CallBackStartME_BufIdx(0);
		_vrun(MACRO_CallBackStartME);    	
    }
}

CHANNEL_SERVICE Service_CallBackWriteOutMB(int sIdx)
{
#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_CallBackWriteOutMB: MPC Index = %d\n", sIdx);
#endif
   	
	gCH264Encoder.ARC_WriteOutMacroblock(gCH264Encoder.m_EncodedPicture, sIdx);
    WorkAreaSDM->CopyToFixedAfterReconstructMP4(sIdx);
    SET_CallBackReleaseBuffer_BufIdx(sIdx);
    _vrun(MACRO_CallBackReleaseBuffer);
}


CHANNEL_SERVICE Service_ReleaseBuffer(int sIdx)
{
#ifdef _DEBUG_VRAPTORISATION
   	printf("Service_ReleaseBuffer: MPC Index = %d\n", sIdx);
#endif
   	
	gCH264Encoder.MacroblockStatistics(sIdx);
    // Free MPC circular buffer entry
    MgrMPC.FreeMPC(sIdx);
    MgrMPC.MacroBlocksCompleteCount++;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ARC_WriteOutMacroblock					    */
/* DESCRIPTION:	Writes out a MB (luma + chroma) to the frame buffer.        */
/* RETURNS:		None.								                        */
/* ------------------------------------------------------------------------ */
void CH264Encoder::ARC_WriteOutMacroblock(CBTPicture* pic, int oIdx)
{
    cMPCcb &ob = WorkAreaSDM->CircularBuffs[oIdx];

    // Use DMA engine to output data
    // DMA out Y reconstructed macroblock from SDM to frame buffer
    _vr00 = _vmovw(16*MgrMPC.Deblock.Col, 0x01); // prediction hor comp
    _vr00 = _vmovw(16*MgrMPC.Deblock.Row, 0x02); // prediction vert comp
    _vr00 = _vmovw((unsigned int)(ob.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y), 0x40); // SDM address where to get data from
    _vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
    _vr01 = _vmovw(0x1010, 0x01); // block size
    // point at the right reference frame
    if (!gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_OutputMacroblock);    

    // DMA out U reconstructed macroblock from SDM to frame buffer
    _vr00 = _vmovw(8*MgrMPC.Deblock.Col, 0x01); // prediction hor comp
    _vr00 = _vmovw(8*MgrMPC.Deblock.Row, 0x02); // prediction vert comp
    _vr00 = _vmovw((unsigned int)(ob.PixelCoeffBuffer + PCB_CURRENT_U), 0x40); // SDM address where to get data from
    _vr01 = _vmovw(0x0808, 0x01); // block size
    // point at the right reference frame
    if (!gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_OutputMacroblock);    

    // DMA out V reconstructed macroblock from SDM to frame buffer
    _vr00 = _vmovw((unsigned int)(ob.PixelCoeffBuffer + PCB_CURRENT_V), 0x40); // SDM address where to get data from
    // point at the right reference frame
    if (!gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_OutputMacroblock);    

    // Move on to next column
    MgrMPC.Deblock.Col++;
    MgrMPC.Deblock.LumaOffset += 16;
    MgrMPC.Deblock.ChromaOffset += 8;

    // Move to next row ?
    if(MgrMPC.Deblock.Col == MgrMPC.Picture.Width)
    {
        MgrMPC.Deblock.Col = 0;
        MgrMPC.Deblock.Row++;
        MgrMPC.Deblock.LumaOffset += MgrMPC.Picture.Width*16*15;
        MgrMPC.Deblock.ChromaOffset += MgrMPC.Picture.Width*8*7;
    }
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::EncodePictureMainPictureEnd					*/
/* DESCRIPTION:	This performs end of picture operations for encoding the
				current picture.                                            */
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::EncodePictureMainPictureEnd(void)
{
	m_EncodedPicture->m_ts = m_SourcePicture->m_ts;

	unsigned char encoded_picture_array_position =
	m_EncodedPictureBuffer->StorePicture(m_StoreForReference);

	{
		////////////////////////////
		// Finished encoding frame
		////////////////////////////

		FrameEndStatistics();

		// Now output frames. There are two modes of operation.
		// When pictures are coded in order, one is output as one is encoded.
		// Otherwise the bumping method is used to output pictures

		{
			// No re-ordering of pictures, so output one now
			CBTPicture * output_picture = 0;
			m_EncodedPictureBuffer->OutputFrame(&output_picture);
			SetReturnParameters(m_EncodedPicture, m_SourcePicture, output_picture);
		}

		// Record information about this picture, to be used when encoding the next picture
		// First save info about current picture for use by switch stream encoders
		m_CurrentPictureOrderCount = m_PictureOrderCount;
		m_CurrentFrameNumber = m_FrameNumber;

		}

	m_EncodedPicture = 0;  // This is not needed, but may help with bug finding

	return true;
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
