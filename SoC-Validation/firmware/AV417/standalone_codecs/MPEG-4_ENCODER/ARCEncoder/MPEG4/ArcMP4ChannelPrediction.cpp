/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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


#include <stdio.h>
#include <string.h>
#include "ArcMPC.h"
#include "ArcMP4SDMTables.h"
#include "EnumTypes.h"
#include "VRaptorInfo.h"
#include "ArcChannelRoutines.h"

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Encoder::MakeLumaPredictionError16X16					*/
/* DESCRIPTION:	This subtracts the prediction (m_x_prediction_data) from 
				the source data (m_x_data) to make the prediction error 
				(m_x_prediction_error_data).
				Operates on the current 16x16 macroblock luma data			*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

//CHANNEL_SERVICE Service_MakeLumaPredictionError16X16()
//{
//    cMPCcb &cb = WorkAreaSDM->CircularBuffs[MgrMPC.IdxFromME];
//
//	int row;
//    u_int8 *source_ptr;
//    u_int8 *prediction_ptr;
//	short *prediction_error_ptr;
//	source_ptr = &(cb.PixelCoeffBuffer[PCB_CURRENT_Y]);
//	prediction_ptr = &(cb.PixelCoeffBuffer[PCB_REFERENCE_Y]);
//	prediction_error_ptr = reinterpret_cast<short *>(&(cb.PixelCoeffBuffer[PCB_RESIDUAL_Y]));
//	for (row=0; row<16; row++)
//	{
//		*(prediction_error_ptr     ) = *(source_ptr     ) - *(prediction_ptr     );
//		*(prediction_error_ptr +  1) = *(source_ptr +  1) - *(prediction_ptr +  1);
//		*(prediction_error_ptr +  2) = *(source_ptr +  2) - *(prediction_ptr +  2);
//		*(prediction_error_ptr +  3) = *(source_ptr +  3) - *(prediction_ptr +  3);
//
//		*(prediction_error_ptr +  4) = *(source_ptr +  4) - *(prediction_ptr +  4);
//		*(prediction_error_ptr +  5) = *(source_ptr +  5) - *(prediction_ptr +  5);
//		*(prediction_error_ptr +  6) = *(source_ptr +  6) - *(prediction_ptr +  6);
//		*(prediction_error_ptr +  7) = *(source_ptr +  7) - *(prediction_ptr +  7);
//
//		*(prediction_error_ptr +  8) = *(source_ptr +  8) - *(prediction_ptr +  8);
//		*(prediction_error_ptr +  9) = *(source_ptr +  9) - *(prediction_ptr +  9);
//		*(prediction_error_ptr + 10) = *(source_ptr + 10) - *(prediction_ptr + 10);
//		*(prediction_error_ptr + 11) = *(source_ptr + 11) - *(prediction_ptr + 11);
//
//		*(prediction_error_ptr + 12) = *(source_ptr + 12) - *(prediction_ptr + 12);
//		*(prediction_error_ptr + 13) = *(source_ptr + 13) - *(prediction_ptr + 13);
//		*(prediction_error_ptr + 14) = *(source_ptr + 14) - *(prediction_ptr + 14);
//		*(prediction_error_ptr + 15) = *(source_ptr + 15) - *(prediction_ptr + 15);
//
//		source_ptr += PCB_CURRENT_STRIDE;
//		prediction_ptr += PCB_REFERENCE_STRIDE;
//		prediction_error_ptr += PCB_RESIDUAL_STRIDE/2;
//	}
//
//}
//


