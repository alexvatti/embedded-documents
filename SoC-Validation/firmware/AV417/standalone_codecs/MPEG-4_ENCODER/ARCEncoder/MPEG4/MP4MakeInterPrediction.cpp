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
#include <assert.h>

#include "CMP4Encoder.h"
#include "MP4EncoderControl.h"
#include "MP4EncodedPictureBuffer.h"
#include "MP4Tables.h"
#include "ArcEEOffsetTables.h"
#include "ArcMP4SDMTables.h"
#include "ArcFrameBuffers.h"
#include <arc/arc_avu.h>
#include "ArcEncoderMain_MPA.h"
#include "ArcMP4ChromaFilter_MPA.h"
extern void (*HandleServiceCall)();


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MakeInterPrediction							*/
/* DESCRIPTION:	Makes Inter prediction for the current macroblock, putting 
				the result into y_prediction_data if make_skipped_prediction
				is false, and into m_skip_mb_y_prediction_data otherwise.
				The term block is used to refer to a 16x16, 16x8, 8x16 or 
				8x8 area, and the term sub-block for the sub-division of a
				block.
				It was originally based on CH264Decoder::InterRecon8x8 with
				the following differences (and many more!):

				-	The output is the prediction in y_prediction_data rather 
					than the clipped sum of residual and prediction in y_data.
*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::MakeInterChromaPrediction263(short cIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

	int integer_for_hor_vector;
	int integer_for_ver_vector;
	int fractional_for_hor_vector;
	int fractional_for_ver_vector;
	int chroma_for_hor_vector;
	int chroma_for_ver_vector;

	CBTPicture * forward_reference_picture = 0;

	h264_block_shape_type_t block_shape;
	int number_of_blocks;

	switch (cb.MBCodingType)
	{
		case MBCT_MPEG4_SKIPPED :
		case MBCT_MPEG4_INTER16X16 :
			block_shape = H264_16X16;
			number_of_blocks = 1;
			break;


		case MBCT_MPEG4_INTER8X8 :
			block_shape = H264_8X8;
			number_of_blocks = 4;
			break;

		default :
			assert(false);
			return false;  // Error.
	}
	
	forward_reference_picture = m_EncodedPictureBuffer->GetRefPicture();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// CHROMINANCE MOTION COMPENSATION
	//////////////////////////////////////////////////////////////////////////////////////////////

	if(number_of_blocks == 4)// MPEG4 and H263 same in this respect
	{
		int mv_hor, mv_ver;
      mv_hor = cb.mv[0].mvx + cb.mv[1].mvx + 
               cb.mv[2].mvx + cb.mv[3].mvx;
      mv_ver = cb.mv[0].mvy + cb.mv[1].mvy + 
               cb.mv[2].mvy + cb.mv[3].mvy;

		chroma_for_hor_vector = h263_round_chroma(mv_hor);
		chroma_for_ver_vector = h263_round_chroma(mv_ver);	
		integer_for_hor_vector = (chroma_for_hor_vector>>1); // truncates to - infinity
		integer_for_ver_vector = (chroma_for_ver_vector>>1); 
		fractional_for_hor_vector = chroma_for_hor_vector - (integer_for_hor_vector<<1);
		fractional_for_ver_vector = chroma_for_ver_vector - (integer_for_ver_vector<<1);
	}
	else // 16x16
	{
		if(m_coding_standard == MPEG4) 
		{
			chroma_for_hor_vector = h263_round_chroma(cb.mv[0].mvx<<2 );
			chroma_for_ver_vector = h263_round_chroma(cb.mv[0].mvy<<2 );
			integer_for_hor_vector = (chroma_for_hor_vector >>1);
			integer_for_ver_vector = (chroma_for_ver_vector >>1);
			fractional_for_hor_vector = chroma_for_hor_vector - (integer_for_hor_vector<<1);
			fractional_for_ver_vector = chroma_for_ver_vector - (integer_for_ver_vector<<1);
		}
		else // H263
		{

			chroma_for_hor_vector = (cb.mv[0].mvx % 4 == 0 ? cb.mv[0].mvx >> 1 : (cb.mv[0].mvx >> 1) | 1);
			chroma_for_ver_vector = (cb.mv[0].mvy % 4 == 0 ? cb.mv[0].mvy >> 1 : (cb.mv[0].mvy >> 1) | 1);

			integer_for_hor_vector = (chroma_for_hor_vector >>1);//(chroma_for_hor_vector< 0 ? (chroma_for_hor_vector -1)/2 : chroma_for_hor_vector/2 ); //truncate towards -infinity
			integer_for_ver_vector = (chroma_for_ver_vector >>1);//(chroma_for_ver_vector < 0 ? (chroma_for_ver_vector -1)/2 : chroma_for_ver_vector/2 ); //truncate towards -infinity
			
			fractional_for_hor_vector = chroma_for_hor_vector - (integer_for_hor_vector<<1);
			fractional_for_ver_vector = chroma_for_ver_vector - (integer_for_ver_vector<<1);

		}
	}
		
	// Get the integer pel values from the reference picture
	// taking care with motion vectors that go off the edge of the picture

    // DMA U component prediction into SDM chroma working area
    _vr00 = _vmovw(8*MgrMPC.ActMPC[cIdx].Column +  integer_for_hor_vector, 0x01); // prediction hor comp
    _vr00 = _vmovw(8*MgrMPC.ActMPC[cIdx].Row +  integer_for_ver_vector, 0x02); // prediction vert comp
    _vr00 = _vmovw(CHROMA_U_WORKING_ARRAY, 0x40); // SDM address where to store U prediction
    _vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
    _vr01 = _vmovw(0x0909, 0x01); // block size
    // point at the right reference frame
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_CopyChromaPredToSDM);    

    // DMA V component prediction into SDM chroma working area
    _vr00 = _vmovw(CHROMA_V_WORKING_ARRAY, 0x40); // SDM address where to store V prediction
    // point at the right reference frame
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    _vrun(MACRO_CopyChromaPredToSDM);    

    // Filter chroma U
    _ASM("vdmawait 2, 0x7f"); // wait for only 1 dma in and max dma outs left in fifos
    _vr00 = _vmovw((unsigned int)(&(WorkAreaSDM->ChromaWorkingArrayU[0][0])), 0x01);
    _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U])), 0x02);
    _vr00 = _vmovw(fractional_for_hor_vector, 0x04);
    _vr01 = _vmovw(fractional_for_ver_vector, 0x04);
    _vr00 = _vmovw(m_rounding_control, 0x08);
    _vrun(MACRO_FilterChromaPred);

    // Filter chroma V in place
    _ASM("vdmawait 1, 0x7f"); // wait for only 0 dma in and max dma outs left in fifos
    _vr00 = _vmovw((unsigned int)(&(WorkAreaSDM->ChromaWorkingArrayV[0][0])), 0x01);
    _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V])), 0x02);
    _vrun(MACRO_FilterChromaPred);

	return true;
}

 int CH264Encoder::h263_round_chroma(int x){
    if (x >= 0)
        return  (h263_chroma_roundtab[x & 0xf] + ((x >> 3) & ~1));
    else {
        x = -x;
        return -(h263_chroma_roundtab[x & 0xf] + ((x >> 3) & ~1));
    }
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MakeSkippedInterChromaPrediction				*/
/* DESCRIPTION:	Makes skipped prediction for chroma for block_number of
				block_shape of the current macroblock. The results are put
				into m_Data->m_u_prediction_data and
				m_Data->m_v_prediction_data.
				It is used to enable calculation of skipped chroma SSD.
*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::MakeSkippedChromaPrediction(int cIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];

    CBTPicture * forward_reference_picture = 0;

	{
		forward_reference_picture = m_EncodedPictureBuffer->GetRefPicture();

		if (0 == forward_reference_picture)
		{
			// Really should never occur. Give up as it seems to be a lost cause!
			assert(false);
			return false;
		}
	}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// CHROMINANCE MOTION COMPENSATION
		//////////////////////////////////////////////////////////////////////////////////////////////

    // DMA in U component skip prediction straight into MPC buffer in SDM
    _vr00 = _vmovw(8*MgrMPC.ActMPC[cIdx].Column, 0x01);
    _vr00 = _vmovw(8*MgrMPC.ActMPC[cIdx].Row, 0x02);
    _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U])), 0x40); // SDM addr
    _vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
    _vr01 = _vmovw(0x0808, 0x01);
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    _vrun(MACRO_CopyChromaPredToSDM);    

    // DMA in V component skip prediction straight into MPC buffer in SDM
    _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V])), 0x40);
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    _vrun(MACRO_CopyChromaPredToSDM);    

    _ASM("vdmawait 1, 0x7f"); // wait for only 0 dma in and max dma outs left in fifos

	return true;
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
