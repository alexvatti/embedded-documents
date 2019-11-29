/* ------------------------------------------------------------------------ */
/* FILE:			H264LoopFilter.h										*/
/* DESCRIPTION:		H.264 Loop Filter functions								*/
/* VERSION:			v2.20													*/
/* DATE:			22 September 2005										*/
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

#ifndef __SYMBIAN32__

#include <stdlib.h>
#ifndef	_WIN32_WCE
#include "assert.h"
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//
// This file is used in both the H.264 encoder and the H.264 decoder 
//
// It is included in the source files H264EncoderLoopFilter.cpp and 
// H264DecoderLoopFilter.cpp, preceded by a definition of either
// ENCODER_LOOP_FILTER or DECODER_LOOP_FILTER
//
// The only differences between the encoder and decoder are that it is a 
// member function of the respective encoder and decoder objects and uses the 
// encoder and decoder control lists. In both cases the functionaility is 
// exactly the same
//
//////////////////////////////////////////////////////////////////////////////

#define CONTROL_LIST_TYPE h264_encoder_control_t

#include	"CH264Encoder.h"


#include "ArcSDMTables.h"
#include "H264Tables.h"

#include	"H264OffsetTables.h"
#include	"BTPicture.h"



/* ------------------------------------------------------------------------ */
/* FUNCTION:	DeblockAndOutput						
 DESCRIPTION:	Performs loop filtering on the whole Non-MB-AFF frame.
				Must be called after complete frame has been 
				encoded/decoded.
				When Q is appended to a variable, it indicates a parameter
				of the current block or macroblock
				When P is appended to a variable, it indicates a parameter
				of the neighbouring block or macroblock						*/

/* ------------------------------------------------------------------------ */
void DeblockAndOutput(CBTPicture * picture, int dbIdx);



// Output pixels to frame buffer from reconstructed MPC , performing no deblock
void OutputOnly(CBTPicture * picture, int dbIdx);



// H264 deblock ------------------------------------------------------------------
// Does deblock on reconstructed MPC for 1 macroblock & outputs resulting pixels to frame buffer
// Must be delayed by 2 or more macroblock or will corupt data being used by current
// Free MPC circular buffer entry
void DoH264DeblockFilter(CBTPicture * picture, int dbIdx);


/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
