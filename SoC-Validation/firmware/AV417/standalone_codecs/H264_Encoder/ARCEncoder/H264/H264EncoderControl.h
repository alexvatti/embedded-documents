/* ------------------------------------------------------------------------ */
/* FILE:			H264EncoderControl.h									*/
/* DESCRIPTION:		BT H.264 video encoder control header file.				*/
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

#include "CH264Encoder.h"

#ifndef _H264_ENCODER_CONTROL_H_
#define _H264_ENCODER_CONTROL_H_

/*
--------------------------------------------------------------------------------
Class declarations.
--------------------------------------------------------------------------------
*/

class CBTPicture;

/* ------------------------------------------------------------------------ */
/* DEFINES																	*/
/* ------------------------------------------------------------------------ */

// These definitions moved to H264Common/H264EnumTypes.h

/* ------------------------------------------------------------------------ */
/* DECLARATIONS																*/
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* TYPE DEFINITIONS															*/
/* ------------------------------------------------------------------------ */

//typedef struct h264_encoder_control_tag
//{
//	h264_encoder_control_tag	*left_MB;				// ptr to macroblock left of this one
//	h264_encoder_control_tag	*right_MB;				// ptr to macroblock right of this one
//	h264_encoder_control_tag	*above_MB;				// ptr to macroblock above this one
//	h264_encoder_control_tag	*above_right_MB;		// ptr to macroblock above and right of this one
//
//	h264_encoder_control_tag	*next;					// link to next control block
//
//	cMPCfb AboveMpc;
//   // pointer to temporary MPC data structures, used for change over
//   cMPCcb mpc;
//
//} h264_encoder_control_t;

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
