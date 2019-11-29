/* ------------------------------------------------------------------------ */
/* FILE:			H264DecoderMotionVectors.cpp							*/
/* DESCRIPTION:		H.264 Decoder Motion Vector and Neighbouring			*/
/*					Locations functions										*/
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
#include "H264OffsetTables.h"
#include "H264Tables.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::GetNeighbouringLocationNonMBAFF				*/
/* DESCRIPTION:	This function performs the function in Table 6-3 of the
				standard, and uses the terminology of that table.
				xN and yN are an offset to a pel relative to the top left
				of the current macroblock, given by current_control. The
				function returns the macroblock containing the pel, given
				by neighbouring_control and the vertical offset, yW, of the
				pel relative to the top left of the macroblock it is within.
				maxWH is the width/height of the macroblock, i.e. 16 for
				luma and 8 for chroma.										*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */


inline  const h264_decoder_control_t *CH264Decoder::GetNeighbouringLocationNonMBAFFLuma(int xN, int yN, const h264_decoder_control_t * current_control) const
{
    if(xN >= 0)
    {
        if(xN < 16)
        {
            if(yN < 0)
            {
                return current_control->GetAbove()->Self;
            }
            else if(yN < 16)
            {
                return current_control;
            }
        }
        else if(yN < 0)
        {
            return current_control->GetAbove()->GetRight()->Self;
        }
    }
    else
    {
        if( yN < 0)
        {
            return current_control->GetLeft()->GetAbove()->Self;
        }
        else if( yN < 16)
        {
            return current_control->GetLeft()->Self;
        }
    }
    return NULL;
}

// Two parallel 16 bit adds
_Asm DuelAcc(int a , int b)
{
	%reg a,b
	addsdw a,a,b
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecodeDifferentialMotionVector				*/
/* DESCRIPTION:	This is called when a differential motion vector has been
				parsed. It calculates the absolute motion vector and
				inserts it into the arrays mv_for_hor/ver for all 4x4 
				blocks that it applies to.
				
				block_index indicates the 4x4 block index for the top left 
				4x4 block for which the current vector applies

				vector_number is the number of the current vector in raster 
				scan order, and so goes from 0 to 1 for 16x8 and 8x16 and 
				0 to 3 for 8x8. For 8x8 partitions, it should have the value
				it would have if the whole macroblock had been coded with 
				the same sub-block size. So in the bottom left 8x8 block,
				the first 8x4 sub-block should have vector_number 8.

				block_shape indicates the shape of the block that the 
				current vector represents: 16x16, 16x8, ..., 8x4, 4x4

				Prediction for the motion vector can be from the above 
				block B, the above right block C, the left block A, or the 
				median of the three. When C is not available, the vector 
				of the above left block D is used instead.
				A is the block containing the pel to the left of the top 
				left pel of the current block
				B is the block containing the pel above the top left pel of 
				the current block
				C is the block containing the pel above and to the right of 
				the top right pel of the current block
				D is the block containing the pel above and to the left of 
				the top left pel of the current block

					D  |  B        |  C
					--------------------
					A  |  Current  |
					   |           |
																			*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::VLDDecodeDifferentialMotionVector(h264_decoder_control_t * control,
										 int block_index, int vector_number,
										 h264_block_shape_type_t block_shape
                                                                                 )
{
	// The default reference frame is set to REFERENCE_FRAME_NOT_AVAILABLE meaning not available,
	// that is, outside of the picture or the current slice
	// The value of DIFFERENT_REFERENCE_FRAME is used to indicate the predictor is available but is intra, direct etc,
	// hence has different reference picture to current but is inside the picture.
	int ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE;
	int ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE;
	int ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE;
	mv_pair pred_A = 0;
	mv_pair pred_B = 0;
	mv_pair pred_C = 0;
	
	////////////////////////////////////////////////////////////////////////
	// Set the neighbouring controls
	////////////////////////////////////////////////////////////////////////

	int block_index_C = 0;
	bool above_right_not_available;
	int blockWidth = h264_block_width_table[block_shape];

	mv_pair absolute;

	int block_count;

	{
	    int yW;
		
		// Field or Non-MB-AFF Frame

		// Calculate the top left position (xC, yC) of the current block
		int xC = (block_index & 3) << 2;
		int yC = (block_index & 12);
        yW = (CalculateYW(yC-1,16) & 12);
                

		// Above Right MB
		const h264_decoder_control_t * above_right_control = 
				GetNeighbouringLocationNonMBAFFLuma(xC + blockWidth, yC - 1, control);
		
	
		// Determine the reference frame indices for blocks A, B and C
		// and set to REFERENCE_FRAME_NOT_AVAILABLE if not available and DIFFERENT_REFERENCE_FRAME if intra
		// And also get the prediction vectors if available
	
		////////////////////////////////////////////////////////////////////////
		// Left Block : A
		////////////////////////////////////////////////////////////////////////
	
		if ((block_index & 3) == 0)
		{
			// Left edge of MB, look to left MB
			if (control->Flags & CTLF_LeftInSlice)
			{
				if (!(control->GetLeft()->Flags & CTLF_IsIntraCoded))
				{
					ref_frame_A = control->GetLeft()->mv_for_remapped_index[block_index+3];

					if (DIFFERENT_REFERENCE_FRAME != ref_frame_A)
					{
						pred_A = control->GetLeft()->MotionVecs[block_index+3].Both;
					}
					
				}
				else
				{
					ref_frame_A = DIFFERENT_REFERENCE_FRAME;
				}
			}
			// else default: ref_frame_A = REFERENCE_FRAME_NOT_AVAILABLE; etc.
		}
		else
		{
			// Left Motion Vector is in this macroblock
			{
				ref_frame_A = control->mv_for_remapped_index[block_index-1];
	
				if (DIFFERENT_REFERENCE_FRAME != ref_frame_A)
				{
					pred_A = control->MotionVecs[block_index-1].Both;
				}
			}
		}
	
		////////////////////////////////////////////////////////////////////////
		// Above Block : B
		////////////////////////////////////////////////////////////////////////
	
		if (block_index < 4)
		{
	        // Above MB
			// Top edge of MB, look to above MB
			if( control->Flags & CTLF_AboveInSlice)
			{
				if (!(control->GetAbove()->Flags & CTLF_IsIntraCoded))
				{
					ref_frame_B = control->GetAbove()->mv_for_remapped_index[block_index+12];

					if (DIFFERENT_REFERENCE_FRAME != ref_frame_B)
					{
						pred_B = control->GetAbove()->MotionVecs[block_index+12].Both;
					}
				}
				else
				{
					ref_frame_B = DIFFERENT_REFERENCE_FRAME;
				}
			}
			// else default: ref_frame_B = REFERENCE_FRAME_NOT_AVAILABLE; etc.
		}
		else
		{
			// Above Motion Vector is in this macroblock
			{
				ref_frame_B = control->mv_for_remapped_index[block_index-4];
	
				if (DIFFERENT_REFERENCE_FRAME != ref_frame_B)
				{
					pred_B = control->MotionVecs[block_index-4].Both;
				}
			}
		}
	
		////////////////////////////////////////////////////////////////////////
		// Above Right Block : C or
		// Above  Left Block : D
		////////////////////////////////////////////////////////////////////////
	
		const h264_decoder_control_t * control_C = 0;
	
		// The following case applies when the above right is in the current MB
		// but is in a subsequent 8x8 partition and so is not available
		// 8x4 or 4x4 bottom right of a left hand 8x8 partition - above right not available
		above_right_not_available = 
			(((H264_4X4 == block_shape) || (H264_8X4 == block_shape)) &&
			(((block_index + (blockWidth >> 2)) & 7) == 6)) ||
			(0 == above_right_control) || (control->slice_id != above_right_control->slice_id);
	
		if (!above_right_not_available)
		{
			if (!(above_right_control->Flags & CTLF_IsIntraCoded))
			{
				control_C = above_right_control;
				block_index_C = (((xC + blockWidth) & 15) >> 2) + yW;
			}
			else
			{
				ref_frame_C = DIFFERENT_REFERENCE_FRAME;
			}
		}
		else
		{
	        // Above left MB
	        const h264_decoder_control_t * above_left_control = GetNeighbouringLocationNonMBAFFLuma(xC - 1, yC - 1, control);
	
			// Position C is not available so look at position D
			if ((0 != above_left_control) && (control->slice_id == above_left_control->slice_id))
			{
				if (!(above_left_control->Flags & CTLF_IsIntraCoded))
				{
					control_C = above_left_control;
					block_index_C = (((xC + 15) & 15) >> 2) + yW;
				}
				else
				{
					ref_frame_C = DIFFERENT_REFERENCE_FRAME;
				}
			}
			// else default: ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE; etc.
		}
		// else default: ref_frame_C = REFERENCE_FRAME_NOT_AVAILABLE; etc.
	
		if (0 != control_C)
		{
			{
				ref_frame_C = control_C->mv_for_remapped_index[block_index_C];
	
				if (DIFFERENT_REFERENCE_FRAME != ref_frame_C)
				{
					pred_C = control_C->MotionVecs[block_index_C].Both;
				}
			}
	
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// If only one predictor matches the current reference frame, then use it
	// Otherwise, for 16x8, 8x16, if reference frame matches, use single predictor
	// Else use median prediction
	///////////////////////////////////////////////////////////////////////////


	{
		UNSIGNED_SEPTET remapped_index = control->mv_for_remapped_index[block_index];		
		mv_pair pred = 0;

		if((ref_frame_A == remapped_index) &&
			     (ref_frame_B != remapped_index) &&
				 (ref_frame_C != remapped_index))
		{
			pred = pred_A;
		}
		else if ((ref_frame_A != remapped_index) &&
				 (ref_frame_B == remapped_index) &&
				 (ref_frame_C != remapped_index))
		{
			pred = pred_B;
		}
		else if ((ref_frame_A != remapped_index) &&
				 (ref_frame_B != remapped_index) &&
				 (ref_frame_C == remapped_index))
		{
			pred = pred_C;
		}
		else if ((H264_16X8 == block_shape) &&
				 (0 == vector_number) &&
				 (ref_frame_B == remapped_index))
		{
			// Top 16x8 block, use B if ref frame matches
			pred = pred_B;
		}
		else if ((H264_16X8 == block_shape) &&
				 (1 == vector_number) &&
				 (ref_frame_A == remapped_index))
		{
			// Bottom 16x8 block, use A if ref frame matches
			pred = pred_A;
		}
		else if ((H264_8X16 == block_shape) &&
				 (0 == vector_number) &&
				 (ref_frame_A == remapped_index))
		{
			// Left 8x16 block, use A if ref frame matches
			pred = pred_A;
		}
		else if ((H264_8X16 == block_shape) &&
				 (1 == vector_number) &&
				 (ref_frame_C == remapped_index))
		{
			// Right 8x16 block, use C if ref frame matches
			pred = pred_C;
		}
		else
		{
			// If D, B and C all are outside of the picture, use A
			// else use Median
			if ((REFERENCE_FRAME_NOT_AVAILABLE == ref_frame_B) && (REFERENCE_FRAME_NOT_AVAILABLE == ref_frame_C))
			{
				pred = pred_A;
			}
			else
			{
				// Use Median
				int pred_hor = MEDIAN( ((short)pred_A), ((short)pred_B), ((short)pred_C));
				int pred_ver = MEDIAN( ((int)pred_A >> 16), ((int)pred_B>>16), ((int)pred_C>>16));
				pred = (unsigned int)(pred_hor & 0xffff) + ((unsigned int)pred_ver << 16);
			}
		}

	///////////////////////////////////////////////////////////////////////////
	// Add the differential vector to the predictor to get the absolute vector
	// and copy into the motion vector array for each 4x4 block of the motion
	// compensation block.
	///////////////////////////////////////////////////////////////////////////

		absolute = m_MotionVec.Both;
		DuelAcc(absolute, pred);
	}
        

    switch (block_shape)
    {
    case H264_16X16 : 
            for (block_count=0; block_count<16; block_count++)
            {
                control->MotionVecs[block_count].Both = absolute;
            }
            break;

    case H264_16X8 :
            for (block_count=0; block_count<8; block_count++)
            {
                int bn = block_count+vector_number*8;
                control->MotionVecs[bn].Both = absolute;
            }
            break;

    case H264_8X16 :
            for (block_count=0; block_count<16; block_count += 4)
            {
                int bn = block_count+vector_number*2;
                control->MotionVecs[bn].Both = absolute;
                control->MotionVecs[bn+1].Both = absolute;
            }
            break;

    case H264_8X8 :
            block_count = ((vector_number & 1) << 1 ) + ((vector_number&2) << 2 );
            
            control->MotionVecs[block_count+0].Both = absolute;
           
            control->MotionVecs[block_count+1].Both = absolute;
           
            control->MotionVecs[block_count+4].Both = absolute;
           
            control->MotionVecs[block_count+5].Both = absolute;
           
            break;

    case H264_8X4 :
//                        block_count = ((vector_number &1) << 2) + ((vector_number &4) << 1) +(vector_number & 2);
            block_count = vector_number << 1;
            
            control->MotionVecs[block_count+0].Both = absolute;
           
            control->MotionVecs[block_count+1].Both = absolute;
            break;

    case H264_4X8 :
            block_count = ((vector_number & 4) << 1) + (vector_number & 3);
            
            control->MotionVecs[block_count+0].Both = absolute;
           
            control->MotionVecs[block_count+4].Both = absolute;
             break;

    case H264_4X4 :
            control->MotionVecs[vector_number].Both = absolute;
            break;

    default : // Never should occur
             break;
    }
//	return true;
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ResetIndicesAndVectors						*/
/* DESCRIPTION:	This is called to reset reference picture indices and motion
				vectors in B slices for 16x16, 16x8 and 8x16 inter 
				macroblocks.												*/
/* RETURNS:		void														*/
/* ------------------------------------------------------------------------ */
#ifdef __B_PICTURES__
void CH264Decoder::ResetIndicesAndVectors(h264_decoder_control_t * control)
{}
#endif // def __B_PICTURES__
/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::CalculateSkippedMotionVectors					*/
/* DESCRIPTION:	This is called to calculate the motion vectors for skipped 
				macroblocks in P and SP slices.
				Skipped macroblocks in inter pictures are coded as either
				   - having zero motion vector and simply being a copy of
				     the reference picture with index 0, and so can be
					 decoded as inter 16x16 with zero vector and zero
					 remapped reference picture index.
				   - having motion vector equal to the motion vector
				     predictor that the block would have had, if it had
					 been 16x16 coded
				
				The choice depends on the vectors of the above and left 
				blocks, A and B, as follows
				
				if ((Vector A not available) || (Vector B not available) ||
					(Vector A == 0 and Vector A has remapped index 0) ||
					(Vector B == 0 and Vector B has remapped index 0))
				{
					Zero vector is used: no action needed as vector
					will already be zero
				}
				else
				{
					Motion vector prediction is used
				}
				 
				In both cases the picture index must be set to the array
				position of the picture with remapped index of zero.

				So inverting the logic to test only for the motion vector
				prediction case

				if ((Vector A available) && (Vector B available) &&
					(Vector A != 0 or Vector A does not have reference index 0) &&
					(Vector B != 0 or Vector B does not have reference index 0))
																			*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::CalculateSkippedMotionVectors(h264_decoder_control_t * control)
{
	// The following array position is that of the picture with remapped index of zero
	// It has a short name to keep the condition text of managable length!
	UNSIGNED_SEPTET ap = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(0);

	AlignedIntMemSet(control->mv_for_array_position, ap, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
	AlignedIntMemClr(control->mv_for_remapped_index, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));

	////////////////////////////////////////////////////////////////////////
	// Set the neighbouring controls
	////////////////////////////////////////////////////////////////////////

	
	// Field or Non-MB-AFF Frame

	////////////////////////////////////////////////////////////////////////
	// Determine the skipped mode vector type: zero or 16x16 predictor
	////////////////////////////////////////////////////////////////////////

	if (( (control->Flags & CTLF_LeftInSlice+CTLF_AboveInSlice) == CTLF_LeftInSlice+CTLF_AboveInSlice ) &&
		((0 != control->GetLeft()->MotionVecs[3].Both) ||
			  (0 != control->GetLeft()->mv_for_remapped_index[3])) &&
		((0 != control->GetAbove()->MotionVecs[12].Both)  ||
			  (0 != control->GetAbove()->mv_for_remapped_index[12])))
	{
		m_MotionVec.Both = 0;
		VLDDecodeDifferentialMotionVector(control, 0, 0, H264_16X16 );
	}
	else
	{
		// set all vectors to zero
		AlignedIntMemClr(control->MotionVecs, NUMBER_OF_VECTORS_PER_MACROBLOCK*sizeof(int));
	}

	// Adjust the mtype as the vector and reference picture index are already correct.
	control->mtype = H264_INTER_16X16;

	// Set the quantiser as this will be needed by the loop filter
	control->quant = m_VLDQuant;
	control->partition_modes = 0; // H264_8X8_PARTITION ....
//	control->partition_mode[0] = H264_8X8_PARTITION;
//	control->partition_mode[1] = H264_8X8_PARTITION;
//	control->partition_mode[2] = H264_8X8_PARTITION;
//	control->partition_mode[3] = H264_8X8_PARTITION;

}


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
