/* ------------------------------------------------------------------------ */
/* FILE:			H264EnumTypes.h											*/
/* DESCRIPTION:		BT H.264 enumerated and types header file				*/
/* VERSION:			v2.20													*/
/* DATE:			22 September 2005										*/
/* MODIFIED BY:	    ARC International (UK) Limited							*/
/* MODIFIED DATE:	31 July 2007											*/
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
/* 																			*/

/* May contain code (c) British Telecommunications plc 2005        			*/

#ifndef	ENUM_TYPES_H
#define	ENUM_TYPES_H

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
#ifndef	_WIN32_WCE
	#include <assert.h>
#endif
	#include <limits.h>
#endif

#ifndef __cplusplus
#define bool int
#define false 0
#define true 1
#endif 

// UNSIGNED_SEPTET is used for unsigned 7 bit values. It is defined as either
// a signed or unsigned char depending on the target processor. Signed char
// appears faster on the Pentium as there is a "load byte with sign extension"
// instruction. Unsigned char appears faster on the ARM as "load" compiles to
// less instructions than when signed.
#ifdef __arm
#define UNSIGNED_SEPTET unsigned char
#else
#define UNSIGNED_SEPTET signed char
#endif

#define PIXEL unsigned char

#define CODED_BYTE unsigned char

#define WORKING_ARRAY_SIZE 21
#define CHROMA_WORKING_ARRAY_SIZE 9 // only used for chroma which has a max partition size off (16 >> 1) + 1 (+1 extra pel for filter)
#define NUMBER_OF_VECTORS_PER_MACROBLOCK		 16
#define NUMBER_OF_Y_PELS_PER_BLOCK				256
#define NUMBER_OF_U_PELS_PER_BLOCK				 64
#define NUMBER_OF_V_PELS_PER_BLOCK				 64

#define NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS		 16
#define NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS	  4

#define MINIMUM_QUANTISER_INDEX					 12
#define MAXIMUM_QUANTISER_INDEX					 42
#define MINIMUM_QUANTISER_INDEX_MPEG4			 1
#define MAXIMUM_QUANTISER_INDEX_MPEG4			 31


#define MAXIMUM_NUMBER_OF_STORED_FRAMES			 2



#define NOT_REAL_REFERENCE_FRAME_THRESHOLD		125      // previous test was < 0, now test is > NOT_REAL_REFERENCE_FRAME_THRESHOLD
#define DIFFERENT_REFERENCE_FRAME				126      // was -2
#define REFERENCE_FRAME_NOT_AVAILABLE			127      // was -1

// The number of border pels needs to be 17 on the left and top 
// and 16 on the right and bottom - see MN working records of
// 30 October 2003. A vector of -16.5 horizontally does not lead
// to the same prediction as a vector of -16: the right most
// column of predicted pels would be different. Hence the border
//  area needs to include -16.5, and hence 17 border pels are needed.
// Note, when tested on 9 and 10 October 2003 with 16 border 
// pels all round (which is now known to be wrong), slower
// encoding is observed than when extra border pels were created.
// This is thought to have been due to 64kbyte pointer alignment


// changed to 18x18 because we allow -15.75 to +15.75 and we use a 6 tap filter,
// therefore 15 + 3 = 18
#define NUMBER_OF_LEFT_BORDER_PELS 18
#define NUMBER_OF_RIGHT_BORDER_PELS 18
#define NUMBER_OF_TOP_BORDER_PELS 18
#define NUMBER_OF_BOTTOM_BORDER_PELS 18

#ifndef MEDIAN
#define MEDIAN(a,b,c) ( ((a) > (b)) ? ( ((c) >= (a)) ? (a) : MAX((b), (c)) ) : ( ((c) >= (b)) ? (b) : MAX((a), (c)) ) )
#endif

#ifndef CLIP
#define CLIP(low, high, x) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))
#endif

// QUANT_DIV calculates q/6 for 0 <= q <= 51 without a division
#ifndef	QUANT_DIV
#define	QUANT_DIV(q)	((43 * q) >> 8)
#endif

// QUANT_MOD calculates q%6 for 0 <= q <= 51 without a division using the result of QUANT_DIV
#ifndef	QUANT_MOD
#define	QUANT_MOD(q, q_div)	(q - (6 * q_div))
#endif

#ifndef MIN_POSITIVE
#define MIN_POSITIVE(a,b)  ((((a)>=0) && ((b)>=0)) ? MIN(a,b) : MAX(a,b))
#endif

/*
--------------------------------------------------------------------------------
Type definitions.
--------------------------------------------------------------------------------
*/


typedef enum
{
	H264_BASELINE_PROFILE = 66,
	H264_MAIN_PROFILE = 77,
	H264_EXTENDED_PROFILE = 88,
	H264_HIGH_PROFILE = 100,
	H264_HIGH_10_PROFILE = 110,
	H264_HIGH_422_PROFILE = 122,
	H264_HIGH_444_PROFILE = 144,
	H264_UNKNOWN_PROFILE
}	h264_profile_type_t;

typedef enum
{
	H264_LOW_UNSPECIFIED_NAL_UNIT_TYPE,
	H264_CODED_SLICE_NO_DP,
	H264_CODED_DATA_PARTITION_A,
	H264_CODED_DATA_PARTITION_B,
	H264_CODED_DATA_PARTITION_C,
	H264_CODED_SLICE_IDR,
	H264_SEI_NAL_UNIT_TYPE,
	H264_SEQUENCE_PARAMETER_SET,
	H264_PICTURE_PARAMETER_SET,
	H264_PICTURE_DELIMITER,
	H264_END_OF_SEQUENCE,
	H264_END_OF_STREAM,
	H264_FILLER_DATA,
	H264_HIGH_RESERVED_NAL_UNIT_TYPE
}	h264_NAL_unit_type_t;

typedef enum
{
	H264_P_SLICE,
	H264_B_SLICE_NOT_BaseLine,
	H264_I_SLICE,
	H264_SP_SLICE_NOT_BaseLine,
	H264_SI_SLICE_NOT_BaseLine,
	H264_ALL_P_SLICE,
	H264_ALL_B_SLICE_NOT_BaseLine,
	H264_ALL_I_SLICE,
	H264_ALL_SP_SLICE_NOT_BaseLine,
	H264_ALL_SI_SLICE_NOT_BaseLine,
	H264_UNKNOWN_SLICE
}	h264_slice_type_t;


typedef enum
{
	H264_UNKNOWN_REGION,
	H264_FORWARD_REGION,
	H264_BACKWARD_REGION,
	H264_BIDIRECTIONAL_REGION,
	H264_INTRA_REGION,
	H264_DIRECT_REGION		// Has to be translated to H264_BIDIRECTIONAL before use
}	h264_8x8_partition_direction_type_t;

typedef enum
{
	H264_16X16,
	H264_16X8,
	H264_8X16,
	H264_8X8,
	H264_8X4,
	H264_4X8,
	H264_4X4,
	H264_UNKNOWN_SHAPE_TYPE
}	h264_block_shape_type_t;




typedef enum
{
	H264_LUMA_DC,
	H264_LUMA_AC,
	H264_CHROMA_DC,
	H264_CHROMA_AC,
	H264_LUMA_8X8
}	h264_block_t;



enum h264_mv_component_type
{
	H264_HORIZONTAL,
	H264_VERTICAL
};

typedef enum
{
	H264,
	MPEG4,
	H263,
	H263_PLUS
} CodingStandard_t;

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
