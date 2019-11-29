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
#include "MP4Warnings.h"
#include "MP4MemoryMonitor.h"
#include "MP4EncoderControl.h"
#include "ArcEEOffsetTables.h"
#include "MP4Tables.h"
#include "ArcMP4SDMTables.h"
#ifdef HARDWARE_SIMD
#include <arc/arc_avu.h>
#include "ArcEncoderAddSubtract_MPA.h"
extern void (*HandleServiceCall)();
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MakeChromaPredictionError						*/
/* DESCRIPTION:	This subtracts the prediction (m_x_prediction_data) from 
				the source data (m_x_data) to make the prediction error 
				(m_x_prediction_error_data).
				Operates on the current macroblock chroma data.				*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::MakeChromaPredictionError()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxFromME];

	int row;
    u_int8 *source_ptr;
    u_int8 *prediction_ptr;
	short *prediction_error_ptr;

	source_ptr = &(cb.PixelCoeffBuffer[PCB_CURRENT_U]);
	prediction_ptr = &(cb.PixelCoeffBuffer[PCB_REFERENCE_U]);
	prediction_error_ptr = reinterpret_cast<short *>(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_U]));
	for (row=0; row<8; row++)
	{
		*(prediction_error_ptr     ) = *(source_ptr     ) - *(prediction_ptr     );
		*(prediction_error_ptr +  1) = *(source_ptr +  1) - *(prediction_ptr +  1);
		*(prediction_error_ptr +  2) = *(source_ptr +  2) - *(prediction_ptr +  2);
		*(prediction_error_ptr +  3) = *(source_ptr +  3) - *(prediction_ptr +  3);

		*(prediction_error_ptr +  4) = *(source_ptr +  4) - *(prediction_ptr +  4);
		*(prediction_error_ptr +  5) = *(source_ptr +  5) - *(prediction_ptr +  5);
		*(prediction_error_ptr +  6) = *(source_ptr +  6) - *(prediction_ptr +  6);
		*(prediction_error_ptr +  7) = *(source_ptr +  7) - *(prediction_ptr +  7);

		source_ptr += PCB_CURRENT_STRIDE;
		prediction_ptr += PCB_REFERENCE_STRIDE;
		prediction_error_ptr += PCB_RESIDUAL_STRIDE/2;
	}

	source_ptr = &(cb.PixelCoeffBuffer[PCB_CURRENT_V]);
	prediction_ptr = &(cb.PixelCoeffBuffer[PCB_REFERENCE_V]);
	prediction_error_ptr = reinterpret_cast<short *>(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_V]));

	for (row=0; row<8; row++)
	{
		*(prediction_error_ptr     ) = *(source_ptr     ) - *(prediction_ptr     );
		*(prediction_error_ptr +  1) = *(source_ptr +  1) - *(prediction_ptr +  1);
		*(prediction_error_ptr +  2) = *(source_ptr +  2) - *(prediction_ptr +  2);
		*(prediction_error_ptr +  3) = *(source_ptr +  3) - *(prediction_ptr +  3);

		*(prediction_error_ptr +  4) = *(source_ptr +  4) - *(prediction_ptr +  4);
		*(prediction_error_ptr +  5) = *(source_ptr +  5) - *(prediction_ptr +  5);
		*(prediction_error_ptr +  6) = *(source_ptr +  6) - *(prediction_ptr +  6);
		*(prediction_error_ptr +  7) = *(source_ptr +  7) - *(prediction_ptr +  7);

		source_ptr += PCB_CURRENT_STRIDE;
		prediction_ptr += PCB_REFERENCE_STRIDE;
		prediction_error_ptr += PCB_RESIDUAL_STRIDE/2;
	}

    return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ReconstructLuma16X16							*/
/* DESCRIPTION:	This adds the prediction to the coded prediction error, 
				clips, and reconstructs into the encoded picture.
				Operates on the current 16x16 macroblock luma data.			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::ARC_ReconstructLuma16X16()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxReconstruct];

	u_int8 * mpcRecon;
	int16 * prediction_error_ptr;
	u_int8 * prediction_ptr;

   // luma
	mpcRecon = cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y;
	
	prediction_error_ptr = reinterpret_cast<int16 *> (cb.PixelCoeffBuffer + PCB_RESIDUAL_Y);
	prediction_ptr = cb.PixelCoeffBuffer + PCB_REFERENCE_Y;

	for (int row=0; row<16; row++)
	{
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr     ) + *(prediction_error_ptr     ));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  1) + *(prediction_error_ptr +  1));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  2) + *(prediction_error_ptr +  2));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  3) + *(prediction_error_ptr +  3));

		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  4) + *(prediction_error_ptr +  4));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  5) + *(prediction_error_ptr +  5));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  6) + *(prediction_error_ptr +  6));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  7) + *(prediction_error_ptr +  7));

		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  8) + *(prediction_error_ptr +  8));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  9) + *(prediction_error_ptr +  9));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 10) + *(prediction_error_ptr + 10));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 11) + *(prediction_error_ptr + 11));

		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 12) + *(prediction_error_ptr + 12));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 13) + *(prediction_error_ptr + 13));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 14) + *(prediction_error_ptr + 14));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr + 15) + *(prediction_error_ptr + 15));

		prediction_error_ptr += 16;
		prediction_ptr += 16;
	}

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::ARC_ReconstructChroma								*/
/* DESCRIPTION:	This adds the prediction to the coded prediction error, 
				clips, and reconstructs into the encoded picture.
				Operates on the current macroblock chroma data.				*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

bool CH264Encoder::ARC_ReconstructChroma()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxReconstruct];

	u_int8 * mpcRecon;

	int16 * prediction_error_ptr;
	u_int8 * prediction_ptr;

   // u component
	mpcRecon = cb.PixelCoeffBuffer + PCB_CURRENT_U;

	prediction_error_ptr = reinterpret_cast<int16 *> (cb.PixelCoeffBuffer + PCB_TRANSFORM_U);
	prediction_ptr = cb.PixelCoeffBuffer + PCB_REFERENCE_U;

	for (int row=0; row<8; row++)
	{
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr     ) + *(prediction_error_ptr     ));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  1) + *(prediction_error_ptr +  1));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  2) + *(prediction_error_ptr +  2));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  3) + *(prediction_error_ptr +  3));

		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  4) + *(prediction_error_ptr +  4));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  5) + *(prediction_error_ptr +  5));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  6) + *(prediction_error_ptr +  6));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  7) + *(prediction_error_ptr +  7));
		mpcRecon += 8;

		prediction_error_ptr += 16; // n.b. PCB_TRANSFORM_STRIDE is in bytes and prediction_error_ptr is an u_int16
		prediction_ptr += PCB_REFERENCE_STRIDE;
	}


   // v component
	mpcRecon = cb.PixelCoeffBuffer + PCB_CURRENT_V;

	prediction_error_ptr = reinterpret_cast<int16 *> (cb.PixelCoeffBuffer + PCB_TRANSFORM_V);
	prediction_ptr = cb.PixelCoeffBuffer + PCB_REFERENCE_V;

	for (int row=0; row<8; row++)
	{
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr     ) + *(prediction_error_ptr     ));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  1) + *(prediction_error_ptr +  1));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  2) + *(prediction_error_ptr +  2));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  3) + *(prediction_error_ptr +  3));

		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  4) + *(prediction_error_ptr +  4));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  5) + *(prediction_error_ptr +  5));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  6) + *(prediction_error_ptr +  6));
		*mpcRecon++ = RECON_CLIP(*(prediction_ptr +  7) + *(prediction_error_ptr +  7));
		mpcRecon += 8;

		prediction_error_ptr += 16; // n.b. PCB_TRANSFORM_STRIDE is in bytes and prediction_error_ptr is an u_int16
		prediction_ptr += PCB_REFERENCE_STRIDE;
	}

    return true;
}

// Copies transform area of MPC to reconstructed area of MPC
void CH264Encoder::ARC_ReconstructIntra()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxReconstruct];

    u_int8 * dptr;
	short * sptr;

	// luma (encoded picture + pixel coeff buffer)
	dptr = cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y;
	sptr = reinterpret_cast<short *>(&(cb.PixelCoeffBuffer[PCB_TRANSFORM_Y]));


	for(int y = 0; y < 16; y++)		// x,y source space coords
	{
		for(int x = 0; x < 16; x++)
		{
			*dptr++ = RECON_CLIP(*sptr); sptr++;
		}
	}


	// chroma u + v (encoded picture)
	for(int y = 0; y < 8; y++)
	{
      for (int chroma_component=0; chroma_component<2; chroma_component++)
      {
		   for(int x = 0; x < 8; x++)
		   {
			   *dptr++ = RECON_CLIP(*sptr); sptr++;
		   }
      }
	}

 }


// Copies reference area of MPC to reconstructed area of MPC (overlapping copy) stride 16 to stride 24
void CH264Encoder::ARC_ReconstructSkipped()
{
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxReconstruct];

	u_int8 * dptr;
	u_int8 * sptr;

	// luma (encoded picture + pixel coeff buffer)
	dptr = cb.PixelCoeffBuffer + PCB_RECONSTRUCTED_Y;
	sptr = cb.PixelCoeffBuffer + PCB_REFERENCE_Y;


	for(int y = 0; y < 16; y++)		// x,y source space coords
	{
		for(int x = 0; x < 16; x++)
			*dptr++ = *sptr++;
	}


	// chroma u + v (encoded picture)
	for(int y = 0; y < 8; y++)
	{
        for (int chroma_component=0; chroma_component<2; chroma_component++)
        {
		    for(int x = 0; x < 8; x++)
			    *dptr++ = *sptr++;
        }
	}
 }


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
