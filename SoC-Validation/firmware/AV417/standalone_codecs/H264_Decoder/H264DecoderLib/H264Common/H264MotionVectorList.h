/* ------------------------------------------------------------------------ */
/* FILE:			H264MotionVectorList.h									*/
/* DESCRIPTION:		BT H.264 motion vector list header file					*/
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

#ifndef	H264_MOTION_VECTOR_LIST_H
#define	H264_MOTION_VECTOR_LIST_H

#include "H264EnumTypes.h"
#ifdef __B_PICTURES__

//struct mv_type
//{
//	int hor[NUMBER_OF_VECTORS_PER_MACROBLOCK];
//	int ver[NUMBER_OF_VECTORS_PER_MACROBLOCK];
//	UNSIGNED_SEPTET mv_picture_index[NUMBER_OF_VECTORS_PER_MACROBLOCK];		// Picture index for forward multiple reference picture prediction
//	h264_8x8_partition_mode_type_t partition_mode[NUMBER_OF_LUMA_EIGHT_BY_EIGHT_BLOCKS];
//
//	int col_zero_flags;	// 16 single bit flags used for spatial direct mode
//
//	bool mb_aff;	// In case of MB-AFF, indicates whether field (true) or frame (false) mode was used
//
//	h264_mb_type_t mtype;
//};

//class CH264MotionVectorList 
//{
//public:
//	CH264MotionVectorList(void);
//	~CH264MotionVectorList(void);
//
//	bool IsCreated() 
//	{ 
//		return (0 != m_ListHeader);
//	}
//
//	bool SetSize(int number_of_macroblocks);
//
//	mv_type * GetFrameMacroblock(int macroblock_number);
//
//private:
//	bool CreateList();
//	void DeleteList();
//
//	mv_type * m_ListHeader;
//
//	int m_NumberOfMacroblocks;
//
//};


#endif
#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
