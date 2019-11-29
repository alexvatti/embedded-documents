/* ------------------------------------------------------------------------ */
/* FILE:			H264IntraRecon.cpp										*/
/* DESCRIPTION:		H.264 Intra prediction reconstruction functions			*/
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

#include "H264Warnings.h"

#include "CH264Decoder.h"
#include "H264DecoderControl.h"
#include "H264DecoderTables.h"
#include "H264Tables.h"
#include "H264OffsetTables.h"
#include "H264Transform.h"

#if defined(AURORA_DMA)
#include "H264DecodedPictureBuffer.h"
#endif
/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::IntraRecon4x4									*/
/* DESCRIPTION:	Performs Intra reconstruction for the current macroblock

				The luminance 4x4 blocks must be processed in transmission
				order, that is:

				 0  1  4  5
				 2  3  6  7
				 8  9 12 13
				10 11 14 15

				The 4x4 block contains the pels a..p, and is predicted
				from neighbouring pels A, B, ... Q as shown below.

				Q A B C D E F G H
				I a b c d
				J e f g h
				K i j k l
				L m n o p

*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */



// Select most probable mode given above & left vals
unsigned char ProbSetTab[16][16] =
{       // Left
//    0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15
    { 2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  0, 0, 0, 0  }, // 0
    { 2, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0  }, // 1
    { 2, 0, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 0, 0, 0  }, // 2
    { 2, 0, 1, 2,  2, 2, 2, 2,  2, 2, 2, 2,  0, 0, 0, 0  }, // 3

    { 2, 0, 1, 2,  3, 3, 3, 3,  3, 3, 3, 3,  0, 0, 0, 0  }, // 4
    { 2, 0, 1, 2,  3, 4, 4, 4,  4, 4, 4, 4,  0, 0, 0, 0  }, // 5
    { 2, 0, 1, 2,  3, 4, 5, 5,  5, 5, 5, 5,  0, 0, 0, 0  }, // 6
    { 2, 0, 1, 2,  3, 4, 5, 6,  6, 6, 6, 6,  0, 0, 0, 0  }, // 7     Above

    { 2, 0, 1, 2,  3, 4, 5, 6,  7, 7, 7, 7,  0, 0, 0, 0  }, // 8
    { 2, 0, 1, 2,  3, 4, 5, 6,  7, 8, 8, 8,  0, 0, 0, 0  }, // 9
    { 2, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9, 9,  0, 0, 0, 0  }, // 10
    { 2, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9 ,10, 0, 0, 0, 0  }, // 11

    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  }, // 12
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  }, // 13
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  }, // 14
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  }, // 15
};

inline unsigned char ProbSet(int above, int left)
{
    return ProbSetTab[above][left];
}



/* -----------------------------------------------------------------------------
 * Interface to macros
 *
 * sdm_left_line_add        %i0
 * sdm_top_line_add         %i1
 * coef_buf_add             %i2
 * out_mcblk_add            %i3
 *
 * sdm_top_left_pel_add     %i5
 *
 *
 *
 */

_Asm void LumaIntra4x4PredSetup( unsigned int sdm_top_line_add,
                                            unsigned int sdm_left_line_add,
                                            unsigned int coef_buf_add,
                                            unsigned int out_mcblk_add,
                                            unsigned int  sdm_top_left_pel_add)
{
        % reg sdm_top_line_add, sdm_left_line_add, coef_buf_add, out_mcblk_add,sdm_top_left_pel_add

.off    percent_reg

    // load inp and output pointers
    vmovw       vr00, sdm_left_line_add,1       ;       // load i0 with sdm_top_line_add
    vmovw       vr00, sdm_top_line_add, 2       ;       // load i1 with sdm_top_line_add
    vmovw       vr00, coef_buf_add,     4       ;       // load i2 with coef_buf_add
    vmovw       vr00, out_mcblk_add,    8       ;       // load i3 with out_mcblk_add
    vmovw       vr00, sdm_top_left_pel_add, 32  ;       // load i5 with sdm_top_left_pel_add

.on     percent_reg
}

//IntraMacroTable[ (mode<< 3) + extrapolationReqd + (above_mode<< 1) + left_mode
e_macro_index Intra4x4MacroTable[] =
{
// 0
    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,
    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,    AM_Intra_Luma4x4VerPred,
//1
    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,
    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,    AM_Intra_Luma4x4HorPred,
//2
    AM_Intra_Luma4x4DCConst,    AM_Intra_Luma4x4DCHor,		AM_Intra_Luma4x4DCVer,		AM_Intra_Luma4x4DCVerAndHor,
    AM_Intra_Luma4x4DCConst,    AM_Intra_Luma4x4DCHor,		AM_Intra_Luma4x4DCVer,		AM_Intra_Luma4x4DCVerAndHor,
//3
	AM_Intra_Luma4x4DiagDownLeftWOExtrp, AM_Intra_Luma4x4DiagDownLeftWOExtrp, AM_Intra_Luma4x4DiagDownLeftWOExtrp, AM_Intra_Luma4x4DiagDownLeftWOExtrp,
	AM_Intra_Luma4x4DiagDownLeftWithExtrp,AM_Intra_Luma4x4DiagDownLeftWithExtrp,AM_Intra_Luma4x4DiagDownLeftWithExtrp,AM_Intra_Luma4x4DiagDownLeftWithExtrp,
//4
	AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,
	AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,AM_Intra_Luma4x4DiagDownRight,
//5
	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,
	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,	AM_Intra_Luma4x4VerRight,
//6
	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,
	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,	AM_Intra_Luma4x4HorDown,
//7
	AM_Intra_Luma4x4VerLeftWOExtrp,AM_Intra_Luma4x4VerLeftWOExtrp,AM_Intra_Luma4x4VerLeftWOExtrp,AM_Intra_Luma4x4VerLeftWOExtrp,
	AM_Intra_Luma4x4VerLeftWithExtrp,AM_Intra_Luma4x4VerLeftWithExtrp,AM_Intra_Luma4x4VerLeftWithExtrp,AM_Intra_Luma4x4VerLeftWithExtrp,
//8
	AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp,
	AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp,		AM_Intra_Luma4x4HorUp
};

short Intra4x4MacroAddrTable[72]; // Direct address of macros above

			// Above right block is available for raster_block_index 4, 6, 8, 10, 12, 14 and 9
			// RK: This is because transmission order is different from raster order
			// "x": denotes blocks which require extrapolation
			//	4	5	6			4	x	6	x
			//	8	9	10			8	9	10	x
			//	12	13	14			12	x	14	x

char TopExtrapolationReqdTab[] = {
	0,4,0,4, 0,4,0,4, 0,0,0,4, 0,4,0,4
};


void CH264Decoder::SIMDIntraRecon4x4(h264_decoder_control_t * control)
{
	int tx_block_index;
	int above_mode_plus_one = 0;
	int left_mode_plus_one = 0;

	SDM_Offset	sdm_y_out_buf;
	SDM_Offset	sdm_y_prev_out_buf;
	SDM_Offset	sdm_top_line_buf;

	SDM_Offset	sdm_top_left_pel_ptr;
	SDM_Offset	sdm_left_line_ptr;
	SDM_Offset	sdm_top_line_ptr;
	char mbFlags = control->Flags;

	int			extrapolationReqd;

	sdm_y_out_buf = m_sdm_buf_ofs + SDM_ofs_y_out_buf;
	sdm_y_prev_out_buf = m_prev_sdm_buf_ofs + SDM_ofs_y_out_buf;
	/* -
		do not use DMA for left pel since macroblocks within a
	      slice are in raster order - rahul
	sdm_y_prev_out_buf = Simdu->GetLumaWorkBuffer();
	*/
	sdm_top_line_buf = Simdu->GetLumaTopLineBuf();
	sdm_top_left_pel_ptr = sdm_top_line_buf;

	// perform DMA-IN if possible.... we do not check whether DMA is reqd
	// as we might end up consuming more time in checking this..
	// this sets the flags namely: performDmaIn, getSuffix, getPrefix

	char aboveFlags = 0;
	if(mbFlags & CTLF_AboveInSlice)
	{
		SDM_Offset    dstPtr;
		unsigned int    length = 16;
		int picX = control->PixelX;
		int picY = control->PixelY-1;
        int             arrayPositionOfCurrentPicture;

        arrayPositionOfCurrentPicture = m_DecodedPictureBuffer->GetArrayPositionOfCurrentPicture();
 		aboveFlags = control->GetAbove()->Flags;
		// set flag for prefix:
		if(aboveFlags & CTLF_Left)
		{
			length += 4;
			picX -= 4;
			dstPtr = sdm_top_line_buf;
		}
        else
        {
			dstPtr = sdm_top_line_buf + 4;
        }

        // set flag for suffix
		if(aboveFlags & CTLF_Right)
		{
			length += 4;
		}
//#ifdef INLOOP_DBLOCK
		if(m_InloopDeblock)
		{
			// get the topline for intra prediction
			_vr00 = _vmovw(AuroraH264Dec::m_top_intra_Y + picX, 0x1);       // %i0
		    _vr00 = _vmovw(dstPtr, 0x2);       // %i1
		    if(length == 24)
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE24);
		    }
		    else if(length == 20)
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE20);
		    }
		    else
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE16);
		    }
		    
		    
			if(aboveFlags & CTLF_Left)
			{
				//*(unsigned char*)((int)Simdu->GetSDMStartAdd() + dstPtr +3) = *top_left_intra_Y;
				_vr00 = _vmovw(AuroraH264Dec::m_top_left_intra, 0x1);       // %i0
			    _vr00 = _vmovw(dstPtr, 0x2);       // %i1
			    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
			}
		}
		else
		{
//#else
			Simdu->GetLumaRefBlockInSDM(arrayPositionOfCurrentPicture,
                       dstPtr,  picX, picY,
                        length, 1, length );

			Simdu->WaitForDMAIn();
		}
//#endif
	}

	

	// Find the luminance prediction mode for each of the 4x4 blocks
	for (tx_block_index=0; tx_block_index<16; tx_block_index++)
	{
		int most_probable_mode;
		int probability;
		int offsetWithinMcBlk;

		extrapolationReqd = 0;
		// Can decode in transmission scan order

		// Convert transmission order index to raster index, noting the table is self inverting
		UNSIGNED_SEPTET raster_block_index = h264_4x4_transmission_to_raster_table[tx_block_index];
		offsetWithinMcBlk = top_left_of_luma_4x4_block_raster_order_table[raster_block_index];

		///////////////////////////
		// Set above_mode_plus_one
		///////////////////////////

		if (raster_block_index < 4)
		{
			sdm_top_line_ptr = (sdm_top_line_buf + 4 + offsetWithinMcBlk);

			if (mbFlags & CTLF_AboveInSlice)
			{
				if (H264_INTRA_16X16 == control->GetAbove()->mtype)
				{
					above_mode_plus_one = 3; // Treat as DC prediction
				}
				else if (H264_INTRA_4X4 == control->GetAbove()->mtype)
				{
					above_mode_plus_one = control->GetAbove()->intra_prediction_mode[raster_block_index + 12] + 1;
				}
				else
				{
					// Treat as DC in usual case and treat as outside if constrained intra
					if (m_PictureParameterSet->constrained_intra_pred_flag)
					{
						// Treat as outside of picture
						above_mode_plus_one = 0;
					}
					else
					{
						above_mode_plus_one = 3;
					}
				}

				if (0 != above_mode_plus_one)
				{
					// calculate Q
					sdm_top_left_pel_ptr = (sdm_top_line_ptr-4);

					// calculate E, F, G, H
					if ((raster_block_index < 3))
					{
						// Above right block is available in above MB
						extrapolationReqd = 0;
					}
					else
					{
						extrapolationReqd = 4;
						if (((aboveFlags & CTLF_Right) &&
							(control->slice_id == control->GetAbove()->GetRight()->slice_id)) &&
							// Above right exists and is in same slice
						    ((H264_INTRA_BLKS & control->GetAbove()->GetRight()->mtype) ||
								(!m_PictureParameterSet->constrained_intra_pred_flag)) )
						{
							// Above right block is available in above right MB
							extrapolationReqd = 0;
						}
					}
				}
			}
            else
   			{
				// Above MB is outside of slice
				above_mode_plus_one = 0;
			}

		}
		else
		{

			// above 4x4 block is in this MB
			above_mode_plus_one = control->intra_prediction_mode[raster_block_index-4] + 1;

			sdm_top_line_ptr = (sdm_y_out_buf + offsetWithinMcBlk - 16);
			// A_ptr = (sdm_y_out_buf + offsetWithinMcBlk - 16);

			// Above right block is available for raster_block_index 4, 6, 8, 10, 12, 14 and 9
			// RK: This is because transmission order is different from raster order
			// "x": denotes blocks which require extrapolation
			//	4	5	6			4	x	6	x
			//	8	9	10			8	9	10	x
			//	12	13	14			12	x	14	x
			extrapolationReqd = TopExtrapolationReqdTab[raster_block_index];
			// for subBlkIdx>3 Q will be set by left-pel code
		}

		///////////////////////////
		// Set left_mode_plus_one
		///////////////////////////

		if (0 == (raster_block_index&3))
		{		
			// left 4x4 block is in left MB
			
            //right-most sub-block of the left macroblock (in the same row)
		
			// left 4x4 block is in left MB
			if ((mbFlags & CTLF_LeftInSlice) == 0)
			{
				// Left MB is outside of picture
				left_mode_plus_one = 0;
			}
			else
			{
				if (H264_INTRA_16X16 == control->GetLeft()->mtype)
				{
					left_mode_plus_one = 3; // Treat as DC prediction
				}
				else if (H264_INTRA_4X4 == control->GetLeft()->mtype)
				{
					left_mode_plus_one = control->GetLeft()->intra_prediction_mode[raster_block_index + 3] + 1;
				}
				else
				{
					// Treat as DC in usual case and treat as outside if constrained intra
					if (m_PictureParameterSet->constrained_intra_pred_flag)
					{
						// Treat as outside of picture
						left_mode_plus_one = 0;
					}
					else
					{
						left_mode_plus_one = 3;
					}
				}


				if (0 != left_mode_plus_one)
				{
					sdm_left_line_ptr = sdm_y_prev_out_buf+ offsetWithinMcBlk + 12;

					if(0 != raster_block_index)
					{
						//  Calculate Q
						sdm_top_left_pel_ptr = sdm_left_line_ptr - 16;
					}
				}
			}
		}
		else
		{
			// left 4x4 block is in this MB
			left_mode_plus_one = control->intra_prediction_mode[raster_block_index-1] + 1;
			sdm_left_line_ptr = sdm_y_out_buf + offsetWithinMcBlk - 4;

			// Calculate Q:
			if(raster_block_index > 4)
			{
				sdm_top_left_pel_ptr = sdm_left_line_ptr - 16;
				//Q = *(I_ptr-16);
			}
		}
		
		probability = control->intra_prediction_mode_probability[raster_block_index];
		
		// Now use above_mode_plus_one, left_mode_plus_one and probability to deduce the prediction mode
////		if ((0 == above_mode_plus_one) || (0 == left_mode_plus_one))
////		{
////			// Above or left is "outside", so most probable mode is mode 2 DC prediction
////			most_probable_mode = 2;
////		}
////		else
////		{
////			if (above_mode_plus_one < left_mode_plus_one)
////			{
////				most_probable_mode = above_mode_plus_one - 1;
////			}
////			else
////			{
////				most_probable_mode = left_mode_plus_one - 1;
////			}
////		}
////  Replace by table lookup
        most_probable_mode = ProbSet(  above_mode_plus_one, left_mode_plus_one); // Table version




		if (0 == probability)
		{
			probability = most_probable_mode;
		}
		else if (probability <= most_probable_mode)
		{
			probability--;
		}
		
		control->intra_prediction_mode[raster_block_index] = probability;

        LumaIntra4x4PredSetup(sdm_top_line_ptr, sdm_left_line_ptr,
        	m_y_data + offsetWithinMcBlk+offsetWithinMcBlk, sdm_y_out_buf + offsetWithinMcBlk
               , sdm_top_left_pel_ptr);

////		probability = (probability << 3) + extrapolationReqd;
////		if(left_mode_plus_one)
////		{
////			probability += 1;
////		}
////		if(above_mode_plus_one)
////		{
////			probability += 2;
////		}
////        Simdu->RecMgr.Run(	IntraMacroTable[probability] );

                    
        _vrun(
            Intra4x4MacroAddrTable[
                (probability<< 3) +
                extrapolationReqd +
                (_min(above_mode_plus_one,1) << 1) +
                _min(left_mode_plus_one,1)]
                    );


	}

	if(m_InloopDeblock)
    {
	    Simdu->MoveLumaMacroBlock2DBBuffer(sdm_y_out_buf );
    }
    else
    {
   	Simdu->MoveLumaMacroBlock2Pic(sdm_y_out_buf ,
    		(control->PixelX), (control->PixelY) );
    }

	////////////////////////////////////////////////////
	// Chrominance
	////////////////////////////////////////////////////

	SIMDIntraChromaRecon(control);
}





/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::IntraRecon16x16								*/
/* DESCRIPTION:	Performs Intra reconstruction for the current macroblock
				which has been coded in 16x16 mode.
*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::SIMDIntraRecon16x16(h264_decoder_control_t * control)
{
	// TO_DO: In bit error situations etc, the prediction type can be illegal and
	// try to access data that is not available. Need to address how to handle this,
	// while not making debugging harder!


	const h264_decoder_control_t * above_control = NULL;

	SDM_Offset sdm_y_out_buf = m_sdm_buf_ofs + SDM_ofs_y_out_buf;
	SDM_Offset sdm_y_prev_out_buf = m_prev_sdm_buf_ofs + SDM_ofs_y_out_buf;
	SDM_Offset sdm_top_line_ptr = Simdu->GetLumaTopLineBuf()+12;


	// DMA in the top line
	if (control->Flags & CTLF_AboveInSlice)
	{
		above_control = control->GetAbove();
		SDM_Offset dstPtr;
		unsigned int length;
		int picX = control->PixelX;
		int picY = control->PixelY-1;

        int             arrayPositionOfCurrentPicture;
        arrayPositionOfCurrentPicture = m_DecodedPictureBuffer->GetArrayPositionOfCurrentPicture();

		if( above_control->Flags & CTLF_Left)
		{
			picX -= 4;
			length = 20;
			dstPtr = sdm_top_line_ptr;
		}
		else
		{
			length = 16;
			dstPtr = sdm_top_line_ptr + 4;
		}
		
//#ifdef INLOOP_DBLOCK
		if(m_InloopDeblock)
		{
			// get the topline for intra prediction
			_vr00 = _vmovw(AuroraH264Dec::m_top_intra_Y + picX, 0x1);       // %i0
		    _vr00 = _vmovw(dstPtr, 0x2);       // %i1
		    if(length == 20)
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE20);
		    }
		    else
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE16);
		    }
		    
			if(above_control->Flags & CTLF_Left)
			{
				//*(unsigned char*)((int)Simdu->GetSDMStartAdd() + dstPtr +3) = *top_left_intra_Y;
				_vr00 = _vmovw(AuroraH264Dec::m_top_left_intra, 0x1);       // %i0
			    _vr00 = _vmovw(dstPtr, 0x2);       // %i1
			    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
			}
			
		}
		else
		{
//#else
			Simdu->GetLumaRefBlockInSDM(arrayPositionOfCurrentPicture,
                        dstPtr, picX, picY,
                        length, 1, length );

			Simdu->WaitForDMAIn();
		}
//#endif
	}
	

	int available = 0;
	if((control->mtype_intra_16x16 & 3) == 2)
	{
		// DC prediction
		enum
		{
			AboveAvailable = 1,
			LeftAvailable = 2,
		};

		if ((0 != above_control) /*&& (control->slice_id == above_control->slice_id)*/) // Already tested
		{
			if ((H264_INTRA_BLKS & above_control->mtype)||
				(!m_PictureParameterSet->constrained_intra_pred_flag))
			{
				available += AboveAvailable;
			}
		}

		if (control->Flags & CTLF_LeftInSlice)
		{
			if ((H264_INTRA_BLKS & control->GetLeft()->mtype) ||
				(!m_PictureParameterSet->constrained_intra_pred_flag))
			{
				available += LeftAvailable;
			}
		}
	}
	
	Simdu->Luma16x16IntraPred( sdm_top_line_ptr,
			(sdm_y_prev_out_buf + 12),
			m_y_data ,
			sdm_y_out_buf,
			control->mtype_intra_16x16 & 3, available );



	// copy data to picture
	if(m_InloopDeblock)
    {
	    Simdu->MoveLumaMacroBlock2DBBuffer(sdm_y_out_buf );
    }
    else
    {
    	Simdu->MoveLumaMacroBlock2Pic(sdm_y_out_buf ,
    		(control->PixelX), (control->PixelY) );
    }
    	
	////////////////////////////////////////////////////
	// Chrominance
	////////////////////////////////////////////////////

	SIMDIntraChromaRecon(control);
}



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::IntraChromaRecon								*/
/* DESCRIPTION:	Performs Intra reconstruction for the chroma of the current
				macroblock according to
				control->intra_chroma_prediction_mode
				If any part of a macroblock is intra, it is all predicted
				as intra.

				In the case of DC, the prediction is made from the sums of
				the neighbouring pels.
				A, B, C, and D are the 4x4 blocks,
				S0, S1, S2, S3 are the sums

			           S0     S1

				   S2   A      B

			       S3   C      D
*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */



void CH264Decoder::SIMDIntraChromaRecon(h264_decoder_control_t * control)
{
	SDM_Offset sdm_u_out_buf = m_sdm_buf_ofs + SDM_ofs_u_out_buf;
	SDM_Offset sdm_v_out_buf = m_sdm_buf_ofs + SDM_ofs_v_out_buf;

	SDM_Offset sdm_u_prev_out_buf =  m_prev_sdm_buf_ofs + SDM_ofs_u_out_buf;

	SDM_Offset sdm_u_top_line_buf = Simdu->GetChromaUTopLineBuf();
	SDM_Offset sdm_v_top_line_buf = Simdu->GetChromaVTopLineBuf();

	char mbFlags = control->Flags;
	

	// DMA in the top line
	if ( mbFlags & CTLF_AboveInSlice)
	{
		unsigned int sdmOffset;
		unsigned int length;
		int picX = control->PixelX >> 1;
		int picY = (control->PixelY >> 1)-1;

        int arrayPositionOfCurrentPicture = m_DecodedPictureBuffer->GetArrayPositionOfCurrentPicture();


		if( control->GetAbove()->Flags & CTLF_Left)
		{
			//
			sdmOffset = 4;
			length = 12;
			picX -= 4;
		}
		else
		{
			sdmOffset = 8;
			length = 8;
		}
//#ifdef INLOOP_DBLOCK
		if(m_InloopDeblock)
		{
		    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_U + picX, 0x1);       // %i0
		    _vr00 = _vmovw(sdm_u_top_line_buf + sdmOffset, 0x2);       // %i1
		    if(length == 12)
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE12);
		    }
		    else
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE8);
		    }
		    
		    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_V + picX, 0x1);       // %i0
		    _vr00 = _vmovw(sdm_v_top_line_buf + sdmOffset, 0x2);       // %i1
		    if(length == 12)
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE12);
		    }
		    else
		    {
		    	Simdu->RecMgr.Run(AM_COPYTOPLINE8);
		    }
		    
			//copy the topleft pixel 
			if( control->GetAbove()->Flags & CTLF_Left)
			{
				_vr00 = _vmovw(AuroraH264Dec::m_top_left_intra + 4, 0x1);       // %i0
			    _vr00 = _vmovw(sdm_u_top_line_buf + sdmOffset, 0x2);       // %i1
			    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
			    
				_vr00 = _vmovw(AuroraH264Dec::m_top_left_intra + 8, 0x1);       // %i0
			    _vr00 = _vmovw(sdm_v_top_line_buf + sdmOffset, 0x2);       // %i1
			    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
	
			}
		}
		else
		{
			
//#else
			// Get U
			Simdu->GetChromaURefBlockInSDM(arrayPositionOfCurrentPicture,
    				sdm_u_top_line_buf + sdmOffset,
                    picX, picY,
                    length, 1, length );

			// Get V
			Simdu->GetChromaVRefBlockInSDM(arrayPositionOfCurrentPicture,
    				sdm_v_top_line_buf + sdmOffset,
                    picX, picY,
                    length, 1, length);
			Simdu->WaitForDMAIn();
		}
//#endif      
	}


	
	{
		enum
		{
			LeftAvailable = 1,
			AboveAvailable = 2,
		};
		int availBlocks = 0;

		if( 0 == control->intra_chroma_prediction_mode)
		{

			if (mbFlags & CTLF_AboveInSlice)
			{
				if (( H264_INTRA_BLKS & control->GetAbove()->mtype) ||
					(!m_PictureParameterSet->constrained_intra_pred_flag))
				{
					availBlocks += AboveAvailable;
				}
			}

			if (mbFlags & CTLF_LeftInSlice)
			{
				if (( H264_INTRA_BLKS & control->GetLeft()->mtype) ||
					(!m_PictureParameterSet->constrained_intra_pred_flag))
				{
					availBlocks += LeftAvailable;
				}
			}
		}
		
		//  U & V
		Simdu->ChromaIntraPred( sdm_u_top_line_buf+4,
					(sdm_u_prev_out_buf + 4),
					m_u_data ,
					sdm_u_out_buf,
					control->intra_chroma_prediction_mode, availBlocks);

	}

	if(m_InloopDeblock)
    {
	    Simdu->MoveChromaUMacroBlock2DBBuffer( sdm_u_out_buf);
	
	    Simdu->MoveChromaVMacroBlock2DBBuffer( sdm_v_out_buf);
    }
    else
    {
    
        int    mcblk_hor_offset =  control->PixelX >> 1;
        int    mcblk_ver_offset =  control->PixelY >> 1;

    	Simdu->MoveChromaUMacroBlock2Pic( sdm_u_out_buf,
    		mcblk_hor_offset, mcblk_ver_offset );

    	Simdu->MoveChromaVMacroBlock2Pic( sdm_v_out_buf,
    		mcblk_hor_offset, mcblk_ver_offset );
    }
}




/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::IntraReconPCM									*/
/* DESCRIPTION:	Performs Intra reconstruction for the current macroblock
				which has been coded in PCM mode.
*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */
void CH264Decoder::SIMDIntraReconPCM(h264_decoder_control_t * control)
{
    int    mcblk_hor_offset =  control->PixelX;
    int    mcblk_ver_offset =  control->PixelY;

	SDM_Offset sdm_y_out_buf = m_sdm_buf_ofs + SDM_ofs_y_out_buf;
	SDM_Offset sdm_u_out_buf = m_sdm_buf_ofs + SDM_ofs_u_out_buf;
	SDM_Offset sdm_v_out_buf = m_sdm_buf_ofs + SDM_ofs_v_out_buf;

	// Luma
	Simdu->IntraReconPCMLuma( m_y_data, sdm_y_out_buf );

	if(m_InloopDeblock)
    {
	    Simdu->MoveLumaMacroBlock2DBBuffer(sdm_y_out_buf );
    }
    else
    {
    	Simdu->MoveLumaMacroBlock2Pic(sdm_y_out_buf ,
		    mcblk_hor_offset, mcblk_ver_offset );
    }

    mcblk_hor_offset = mcblk_hor_offset>>1;
    mcblk_ver_offset = mcblk_ver_offset>>1;


	// Chroma U
	Simdu->IntraReconPCMChroma( m_u_data, sdm_u_out_buf );

	if(m_InloopDeblock)
    {
	    Simdu->MoveChromaUMacroBlock2DBBuffer( sdm_u_out_buf);
    }
    else
    {
    	Simdu->MoveChromaUMacroBlock2Pic( sdm_u_out_buf,
    			mcblk_hor_offset, mcblk_ver_offset );
    }

	// Chroma V
	Simdu->IntraReconPCMChroma( m_v_data, sdm_v_out_buf );
	if(m_InloopDeblock)
    {
	    Simdu->MoveChromaVMacroBlock2DBBuffer( sdm_v_out_buf);
    }
    else
    {
    	Simdu->MoveChromaVMacroBlock2Pic( sdm_v_out_buf,
    			mcblk_hor_offset, mcblk_ver_offset );
    }

}
/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
