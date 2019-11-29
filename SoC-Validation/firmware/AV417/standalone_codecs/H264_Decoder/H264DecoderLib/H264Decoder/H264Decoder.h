/* ------------------------------------------------------------------------ */
/* FILE:			H264Decoder.h											*/
/* DESCRIPTION:		BT H.264 video decoder header file.						*/
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

#ifndef	H264_DECODER_H
#define	H264_DECODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "arc_codecs.h"

/*
--------------------------------------------------------------------------------
Definition for exported functions.
--------------------------------------------------------------------------------
*/

#ifdef WIN32
	#define EXPORTED	__declspec(dllexport)
#else
	#define EXPORTED
#endif

/*
--------------------------------------------------------------------------------
Definitions.
--------------------------------------------------------------------------------
*/

#define     H264_VERSION_JM6

#define		H264_DECODER_OK							 0
#define		H264_DECODER_VLD_MALLOC_ERROR			-1
#define		H264_DECODER_MAX_NO_EXCEEDED			-2
#define		H264_DECODER_INVALID_HANDLE				-3
#define		H264_DECODER_INCOMPATIBLE_BITSTREAM		-4
#define		H264_DECODER_SIMD_INIT_ERROR			-5
#define		H264_DECODER_SIMD_BUFFER_ERROR			-6
#define		H264_DECODER_MAX_ERROR_MINUS_ONE		-7

#define		H264_DECODER_MAX_OUTPUT_PICTURES        16

// The following is a bodge to allow the API layer to compile when using JM2.0 H.264 code which does not define h264_bitstream_type_t
#define H264_DECODER_H_HAS_BITSTREAM_TYPE

typedef struct	h264_decoder_parameters_tag
{
	int					ErrorNumber;		/*	byte  0	..  3	*/
	long				HorSize;			/*	byte  4 ..  7	*/
	long				VerSize;			/*	byte  8 .. 11	*/

	unsigned char		bVideoSync;			/*	byte 12			*/
	unsigned char		bDisplayFrame;		/*	byte 13			*/

	unsigned char		bDiscardedData;		/*	byte 14			*/
	unsigned char		Spare;				/*	byte 15			*/

	void			  * pPicture[H264_DECODER_MAX_OUTPUT_PICTURES];

	void              * pSequenceParameters;

	void              * pTopSEIParameters[H264_DECODER_MAX_OUTPUT_PICTURES];
	void              * pBottomSEIParameters[H264_DECODER_MAX_OUTPUT_PICTURES];

}	h264_decoder_parameters;

/*
--------------------------------------------------------------------------------
Prototypes.
--------------------------------------------------------------------------------
*/

#if defined __ARC_LINUX__
int EXPORTED			f_h264_decoder_construct(void *SDMBase);
#else
int EXPORTED			f_h264_decoder_construct(void);
#endif

int EXPORTED			f_h264_decoder_destruct(int iHandle);

int EXPORTED			f_h264_decoder_decode(	int iHandle, unsigned char* pBytes,
												int iNumBytes, int bDataLoss,
												int bStartOfNALUnit, int bEndOfNALUnit,
												int bDiscardIfDisposable,
												unsigned int time_stamp,
												h264_decoder_parameters **ppParameters);

int EXPORTED			f_h264_copy_data(void *frame,arc_frame_data *);
int EXPORTED			f_h264_get_image_size 	(void *,int *, int *);
#if defined __ARC_SHARED_BUFFER__
int EXPORTED 			f_h264_get_data (void *,void *,arc_frame_data *);
void EXPORTED                   f_h264_clear_being_displayed (void *);
#endif

#ifdef __cplusplus
}
#endif

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/

