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

#include "CMP4Encoder.h"
#include "ArcMPC.h"

#ifndef _H264_ENCODER_CONTROL_H_
#define _H264_ENCODER_CONTROL_H_


//#define USE_SAD // definition to use SAD (SOAD) calculations rather than SSD 
// calculations (sums of squared differences).  The main change, once absolute values
// are employed is to change the lambda weightings for Intra/Inter selection

/*
--------------------------------------------------------------------------------
Class declarations.
--------------------------------------------------------------------------------
*/

class CBTPicture;

/* ------------------------------------------------------------------------ */
/* DECLARATIONS																*/
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* CLASS CH264EncoderControl												*/
/* ------------------------------------------------------------------------ */

class CH264EncoderControl
{
public:
	CH264EncoderControl(void);
	~CH264EncoderControl(void);

	bool SetSize(int hsize, int vsize);

private:
	bool CreateList(void);
	void DeleteList(void);

public:
	int m_hsize;				// horizontal picture size
	int m_vsize;				// vertical picture size

	int m_NumberOfMacroblocks;	// number of control blocks
	int m_MacroblocksPerRow;	// number of macroblocks per row of the picture
	

private:
};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
