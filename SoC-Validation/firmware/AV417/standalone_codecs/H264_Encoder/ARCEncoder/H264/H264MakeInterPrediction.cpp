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

/* May contain code (c) British Telecommunications plc 2005        */

/* ------------------------------------------------------------------------ */
/* FILE:			H264MakeInterPrediction.cpp								*/
/* DESCRIPTION:		H.264 Make Inter prediction functions					*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
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

#include <stdlib.h>
#include <assert.h>

#include <arc/arc_avu.h>

#include "H264EncoderControl.h"
#include "ArcSDMTables.h"
#include "ArcMPC.h"
#include "ArcSDMTables_MPA.h"
#include "ArcFrameBuffers.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcChromaFilter_MPA.h"



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MakeInterChromaPrediction							*/
/* DESCRIPTION:	Used to be called CH264Encoder::MakeInterPrediction
                  Makes Inter prediction for the current macroblock, putting
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

bool CH264Encoder::MakeInterChromaPrediction(int sIdx)
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[sIdx];

    int block_shift = 0;
	int number_of_blocks;
	bool block8wide = false;

	// The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
	// regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
	int integer_for_hor_vector;
	int integer_for_ver_vector;
	int fractional_for_hor_vector;
	int fractional_for_ver_vector;
	int any_fractional;

	// To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed
	// sub_block_hor/ver_size is dependant on block_shape, and when
	// block_shape==H264_8X8 it is then dependant on sub_block_shape (which
	// always =H264_8X8)
	int sub_block_size_chroma;
	int filterMacro = MACRO_FilterChromaPred8;


	switch (cb.MBCodingType)
	{
	case MBCT_H264_SKIPPED :
	case MBCT_H264_INTER16X16 :
		number_of_blocks = 1;
		block8wide = true;
		sub_block_size_chroma = 0x0808;
		break;

	case MBCT_H264_INTER16X8 :
		block8wide = true;
		filterMacro = MACRO_FilterChromaPred4;
		number_of_blocks = 2;
		block_shift = 1;
		sub_block_size_chroma = 0x0408;
		break;

	case MBCT_H264_INTER8X16 :
		number_of_blocks = 2;
		sub_block_size_chroma = 0x0804;
		break;

	case MBCT_H264_INTER8X8 :
		filterMacro = MACRO_FilterChromaPred4;
		number_of_blocks = 4;
		sub_block_size_chroma = 0x0404;
		break;

	default :
		assert(false);
		return false;  // Error.
	}

	CBTPicture * forward_reference_picture = m_EncodedPictureBuffer->GetRefPicture();

	// Loop for each block that needs to be motion compensated
	for (int block_number=0; block_number < number_of_blocks; block_number++)
	{
		// block_index indicates the index of the top left 4x4 block of the motion compensation block
	
		int block_index = block_number << block_shift;



	    // Map the partition mode into a block shape and number of sub-blocks.
        // 8x8 is the only partion mode

		//////////////////////////////////////////////////////////////////////////////////////////////
		// CHROMINANCE MOTION COMPENSATION
		//////////////////////////////////////////////////////////////////////////////////////////////

		// First u

		// Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
		// which implies quarter pel precison for chroma
		integer_for_hor_vector = cb.mv[block_index].mvx >> 3;
		integer_for_ver_vector = cb.mv[block_index].mvy >> 3;

		fractional_for_hor_vector = cb.mv[block_index].mvx & 7;
		fractional_for_ver_vector = cb.mv[block_index].mvy & 7;

		// generate flag to see if there are any fractional bits to the motion vector
		any_fractional = fractional_for_hor_vector | fractional_for_ver_vector;

		// Calculate the extra pels needed
		int blk_size = sub_block_size_chroma;
		
		if (0 != fractional_for_ver_vector)
		{
			blk_size += 0x0100;
		}
		
		if (0 != fractional_for_hor_vector)
		{
			blk_size += 0x0001;
		}
		
		int leftIdx = MgrMPC.ActMPC[sIdx].LeftIdx;
		int col = MgrMPC.ActMPC[sIdx].Column;
		_vr00 = _vmovw(sIdx, 1);
		_vr00 = _vmovw(col , 2);
		_vr00 = _vmovw(leftIdx, 4);
		_vrun(MACRO_SetCurrentMPC0);

		// DMA U component prediction into SDM chroma working area
        _vr00 = _vmovw(8*col + 4 * (block_index & 1) + integer_for_hor_vector, 0x01); // prediction hor comp
        _vr00 = _vmovw(8*MgrMPC.ActMPC[sIdx].Row + 4 * (block_index / 2) + integer_for_ver_vector, 0x02); // prediction vert comp
        _vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
        _vr01 = _vmovw(blk_size, 0x01); // block size

        
        // if there are any fractional components to the motion vector then we
        // need to dma to a working array and then perform a filtering operation.
        // however, if there aren't any fractional components then we can simply
        // dma into the final position in the sdm
        if (any_fractional)
        {
        	_vr00 = _vmovw(CHROMA_U_WORKING_ARRAY, 0x40); // SDM address where to store U prediction
        
	        // point at the right reference frame
	        if (gArcFrameBuffers.YUVPos)
	        {
	            _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
	            // perform DMA transfer
	            _vrun(MACRO_CopyChromaPredToSDM);
	
	            // DMA V component prediction into SDM chroma working area
	            _vr00 = _vmovw(CHROMA_V_WORKING_ARRAY, 0x40); // SDM address where to store V prediction
	            _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
	        }
	        else
	        {
	            _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
	            // perform DMA transfer
	            _vrun(MACRO_CopyChromaPredToSDM);
	
	            // DMA V component prediction into SDM chroma working area
	            _vr00 = _vmovw(CHROMA_V_WORKING_ARRAY, 0x40); // SDM address where to store V prediction
	            _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
	        }
	        _vrun(MACRO_CopyChromaPredToSDM);
	
	        // Filter chroma U
	        int offset=((block_index & 2) << 5) + ((block_index & 1) << 2);  // Offset to the start of the chroma sub-block
	        _vr00 = _vmovw((unsigned int)(&(WorkAreaSDM->ChromaWorkingArrayU[0][0])), 0x01); // SDM buffer containing the pred
	        _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U+offset])), 0x02); // SDM buffer where the filtered data is to be stored
	        _vr00 = _vmovw(fractional_for_hor_vector, 0x04); // sub-pel horizontally?
	        _vr01 = _vmovw(fractional_for_ver_vector, 0x04); // sub-pel vertically?
	        _vr00 = _vmovw(block8wide?1:0, 0x8); // sub-block hor size
	
	
	        // Wait for U DMA to finish
	        _ASM("vdmawait 2, 0x7f"); // wait for only 1 dma in and max dma outs left in fifos
	         _vrun(filterMacro);
	        
	
	        // Filter chroma V in place
	        _vr00 = _vmovw((unsigned int)(&(WorkAreaSDM->ChromaWorkingArrayV[0][0])), 0x01);
	        _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V+offset])), 0x02);
	        
	        // Wait for V DMA to finish
	        _ASM("vdmawait 1, 0x7f"); // wait for 0 dma ins and max dma outs left in fifos
	        _vrun(filterMacro);

        }
        else // no fractional
        {
        	int offset=((block_index & 2) << 5) + ((block_index & 1) << 2);  // Offset to the start of the chroma sub-block

	        _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_U+offset])), 0x40); // final SDM address where to store U prediction
        
	        // point at the right reference frame
	        if (gArcFrameBuffers.YUVPos)
	        {
	            _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
	            // perform DMA transfer
	            _vrun(MACRO_CopyChromaPredToSDM);
	
	            // DMA V component prediction into SDM chroma working area
	            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V+offset])), 0x40); // SDM address where to store V prediction
	            _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
	        }
	        else
	        {
	            _vr01 = _vmovw((FRAME_TABLE_U_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
	            // perform DMA transfer
	            _vrun(MACRO_CopyChromaPredToSDM);
	
	            // DMA V component prediction into SDM chroma working area
	            _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_V+offset])), 0x40); // SDM address where to store V prediction
	            _vr01 = _vmovw((FRAME_TABLE_V_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
	        }
	        _vrun(MACRO_CopyChromaPredToSDM);

	        // Wait for both U and V DMAs to finish
	        _ASM("vdmawait 1, 0x7f"); // wait for 0 dma ins and max dma outs left in fifos
        }
	        
	} // for (int block_number=0; block_number < number_of_blocks; block_number++)

	return true;
}
