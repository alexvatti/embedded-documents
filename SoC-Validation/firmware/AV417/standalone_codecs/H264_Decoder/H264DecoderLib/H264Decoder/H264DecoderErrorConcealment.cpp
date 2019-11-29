/* ------------------------------------------------------------------------ */
/* FILE:			H264DecoderErrorConcealment.cpp							*/
/* DESCRIPTION:		H.264 Decoder Error Concealment functions				*/
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
#include "H264Tables.h"

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_concealment_preference_table					*/
/* DESCRIPTION:	The table of preferences of motion vectors to use for
				concealment of each 8x8 region of a lost macroblock.
				The positions are identified as shown below

				 1  |  2  |  3  |  4
				---------------------
				    |           |
				 5  |     |     |  6
				    |           |
				-----  -  -  -  -----
				    |           |
				 7  |     |     |  8
				    |           |
				---------------------
				 9  |  10 |  11 |  12
																			*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_decoder_concealment_preference_table[4][12] =
{
	{  1,  2,  5,  3,  4,  6,  7,  9, 10,  8, 11, 12 },
	{  4,  3,  6,  2,  1,  5,  8, 12, 11,  7, 10,  9 },
	{  9, 10,  7, 11, 12,  8,  5,  1,  2,  6,  3,  4 },
	{ 12, 11,  8, 10,  9,  7,  6,  4,  3,  5,  2,  1 }
};

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_concealment_vector_index_table					*/
/* DESCRIPTION:	The table of motion vector indices for each of the 
				candidate concealment positiions.							*/
/* ------------------------------------------------------------------------ */

const UNSIGNED_SEPTET h264_decoder_concealment_vector_index_table[12] =

	// 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
	{ 15, 12, 15, 12,  3,  0, 11,  8,  3,  0,  3,  0 };

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_concealment_hor_control_offset_table			*/
/* DESCRIPTION:	The table of horizontal control list element offsets for 
				each of the candidate concealment positiions.				*/
/* ------------------------------------------------------------------------ */

const signed char h264_decoder_concealment_hor_control_offset_table[12] =

	// 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
	{ -1,  0,  0,  1, -1,  1, -1,  1, -1,  0,  0,  1 };

/* ------------------------------------------------------------------------ */
/* NAME:		h264_decoder_concealment_ver_control_offset_table			*/
/* DESCRIPTION:	The table of vertical control list element offsets for 
				each of the candidate concealment positiions.				*/
/* ------------------------------------------------------------------------ */

const signed char h264_decoder_concealment_ver_control_offset_table[12] =

	// 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
	{ -1, -1, -1, -1,  0,  0,  0,  0,  1,  1,  1,  1 };

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::DecoderErrorConcealment						*/
/* DESCRIPTION:	This function should be called when a slice header is being 
				decoded and it is realised that it belongs to a new picture
				and the picture previously being decoded has noit been fully
				decoded. It sets motion vectors and reference picture array
				positions for macroblocks that have not been decoded yet, 
				and then decodes them. It then calls the VLDEndOfPicture
				function to perform the usual functions, inclduing loop
				filtering and consideration of output.						*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
void CH264Decoder::DecoderErrorConcealment(void)
{
	h264_decoder_control_t* control;

//	m_VLDFinishedDecodingPicture = true;


    {
        // clear the current coeff buffer in case it was partially filled

//    	SDM_MACRO_BLK_DATA *macroBlkDataPtr;
//        
//    	macroBlkDataPtr = (SDM_MACRO_BLK_DATA *) Simdu->SDM_Buffers.BufferPtr;
//    	Simdu->SDMMemClear( (short *)macroBlkDataPtr[m_sdm_buf_index].sdm_m_y_data );

    	Simdu->SDMMemClear(m_sdm_buf_ofs);
    }
	control = m_DecoderControl.SetAtStart();

	do
	{
		if (! control->coded_block_flags )
		{
			// Macroblock has not been decoded so conceal it
			control->disable_deblocking_filter_idc = m_DisableDeblockingFilterIDC;
			control->slice_alpha_c0_offset = m_SliceAlphaC0Offset;
			control->slice_beta_offset = m_SliceBetaOffset;

			control->cbp = 0;

			if (0 == m_DecodedPictureBuffer->GetNumberOfActiveReferenceFrames())
			{
				// No reference frames available, so have to use intra concealment
				control->mtype = H264_INTRA_16X16;
				control->mtype_intra_16x16 = 2;  // DC is always available
				control->Flags |= CTLF_IsIntraCoded;

				// Set the quantiser as this will be needed by the loop filter
				control->quant = m_VLDQuant;
				control->partition_modes = 0; // H264_8X8_PARTITION....
//				control->partition_mode[0] = H264_8X8_PARTITION;
//				control->partition_mode[1] = H264_8X8_PARTITION;
//				control->partition_mode[2] = H264_8X8_PARTITION;
//				control->partition_mode[3] = H264_8X8_PARTITION;



        		SIMDIntraRecon16x16(control);
			}
			else
			{
				UNSIGNED_SEPTET ap = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(0);
				u_motion_vector mv_candidate[3];
				int block_8x8;

				AlignedIntMemSet(control->mv_for_array_position, ap, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
				AlignedIntMemClr(control->mv_for_remapped_index, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));

				control->mtype = H264_INTER_8X8_MULTI_REF;

				for (block_8x8 = 0; block_8x8<4; block_8x8++)
				{
					// Calculate a concealment vector for the 8x8 block by looking for up to three candidate vectors
					int number_of_candidate_vectors = 0;
					int next_candidate_vector = 0;

					// Use zero vector if there is no candidate
					short mv_hor = 0;
					short mv_ver = 0;
					int first_block;

					do
					{
						int position = h264_decoder_concealment_preference_table[block_8x8][next_candidate_vector];

						int candidate_mb_number = 
								control->mb_number + 
								h264_decoder_concealment_hor_control_offset_table[position] +
								(h264_decoder_concealment_ver_control_offset_table[position]
									* m_DecoderControl.m_MacroblocksPerRow);

						h264_decoder_control_t* candidate_control = m_DecoderControl.GetMacroblock(candidate_mb_number);

						if ((0 != candidate_control) && (INT_MAX != candidate_control->slice_id))
						{
							mv_candidate[number_of_candidate_vectors].Both
								= candidate_control->MotionVecs[h264_decoder_concealment_vector_index_table[position]].Both;

							number_of_candidate_vectors++;
						}

						next_candidate_vector++;

					} while ((number_of_candidate_vectors < 3) && (next_candidate_vector < 12));

					if (0 != number_of_candidate_vectors)
					{
						// Ensure there are three candidates by copying the first
						while (number_of_candidate_vectors < 3)
						{
							mv_candidate[number_of_candidate_vectors].Both = mv_candidate[0].Both;

							number_of_candidate_vectors++;
						}

						mv_hor = MEDIAN(mv_candidate[0].Comp.Hor, mv_candidate[1].Comp.Hor, mv_candidate[2].Comp.Hor);
						mv_ver = MEDIAN(mv_candidate[0].Comp.Ver, mv_candidate[1].Comp.Ver, mv_candidate[2].Comp.Ver);
					}

					first_block = h264_8x8_block_index_to_raster_index_table[block_8x8];

					control->MotionVecs[first_block    ].Comp.Hor = mv_hor;
					control->MotionVecs[first_block + 1].Comp.Hor = mv_hor;
					control->MotionVecs[first_block + 4].Comp.Hor = mv_hor;
					control->MotionVecs[first_block + 5].Comp.Hor = mv_hor;

					control->MotionVecs[first_block    ].Comp.Ver = mv_ver;
					control->MotionVecs[first_block + 1].Comp.Ver = mv_ver;
					control->MotionVecs[first_block + 4].Comp.Ver = mv_ver;
					control->MotionVecs[first_block + 5].Comp.Ver = mv_ver;
				}

				// Set the quantiser as this will be needed by the loop filter
				control->quant = m_VLDQuant;
				control->partition_modes = 0; // H264_8X8_PARTITION .....
//				control->partition_mode[0] = H264_8X8_PARTITION;
//				control->partition_mode[1] = H264_8X8_PARTITION;
//				control->partition_mode[2] = H264_8X8_PARTITION;
//				control->partition_mode[3] = H264_8X8_PARTITION;

				SIMDInterRecon8x8(control);
			}
            {
            // End of macroblock decoding
            // free the last macroblock and allocate next mcblock

//        		SDM_MACRO_BLK_DATA *macroBlkDataPtr;
//                
//        		macroBlkDataPtr = (SDM_MACRO_BLK_DATA *) Simdu->SDM_Buffers.BufferPtr;
//
//        		//free the prev buffer
////        		if(m_prev_sdm_buf_index >= 0)
////                {
//        			Simdu->SDMMemClear( (short *)macroBlkDataPtr[m_prev_sdm_buf_index].sdm_m_y_data );
//        		    Simdu->SDM_Buffers.FreeSDMBuffer(m_prev_sdm_buf_index);
////                }
    			Simdu->SDMMemClear(m_sdm_buf_ofs);	//Now clears & frees
            	Simdu->SDM_Buffers.FreeSDMBuffer(m_prev_sdm_buf_ofs);

                m_prev_sdm_buf_ofs = m_sdm_buf_ofs;

        		//allocate new buffers 
        		m_sdm_buf_ofs = Simdu->SDM_Buffers.GetNextSDMBuffer();
                if(m_sdm_buf_ofs < 0)
                {
                    printf("!!!Error in getting New SDM buffer!!!\n");
                    fflush(stdout);
        			m_simd_error = H264_DECODER_SIMD_BUFFER_ERROR;
                    return;
                }

				m_y_data = m_sdm_buf_ofs;
				m_u_data = m_y_data + SDM_ofs_u_data;
				m_v_data = m_y_data + SDM_ofs_v_data;


            }
		}

		control = control = m_DecoderControl.GetNext();
	} while ((0 != control) && (control->mb_number < m_NumberOfMacroblocksInPicture));  // Second test is for fields


	VLDEndOfPicture(0 != m_VLDSliceHeaderPreviousSliceNALReferenceIDC);
}
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
