/* ------------------------------------------------------------------------ */
/* FILE:			H264DecoderControl.h									*/
/* DESCRIPTION:		BT H.264 video decoder control header file.				*/
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



#ifndef _H264_DECODER_CONTROL_H_
#define _H264_DECODER_CONTROL_H_

#include "H264EnumTypes.h"

/*
--------------------------------------------------------------------------------
Class declarations.
--------------------------------------------------------------------------------
*/

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

// Max resloution is set here
// 720 x 576
const int MAX_RES_IN_MB_X =	45;
const int MAX_RES_IN_MB_Y =	36;
const int MAX_TOTAL_MB	= (MAX_RES_IN_MB_X+2)*(MAX_RES_IN_MB_Y+2);	// One block border arround edge

typedef unsigned int mv_pair;
union u_motion_vector
{
	struct mv_hv
	{
		short Hor;
		short Ver;
	} Comp;
	mv_pair Both;
};


enum h264_decoder_control_Flags_enum
{
	CTLF_Above 			= 0x00000001,
	CTLF_AboveInSlice	= 0x00000002,
	CTLF_Left			= 0x00000004,	
	CTLF_LeftInSlice	= 0x00000008,
	CTLF_Right			= 0x00000010,
//	CTLF_RightInSlice	= 0x00000020,
	CTLF_IsIntraCoded	= 0x00000040,	// indicates whether the macroblock is intra coded
};

struct h264_decoder_control_t
{
    short PixelX;
    short PixelY;
    int slice_id;
	char Flags;
    h264_mb_type_t mtype;							// interpreted value of mtype


	h264_decoder_control_t	*next;					// link to next control block
	h264_decoder_control_t	*Self;					// Either this or zero if outside pic
	int mb_number;


	int slice_group;

	int y_coeffs_present;							// Bit pattern that indictates which of the 4x4 luma blocks has non zero coefficients

	union
	{
		h264_8x8_partition_mode_type_t partition_mode[NUMBER_OF_LUMA_EIGHT_BY_EIGHT_BLOCKS];
		int partition_modes;		// version for fast clearing
	};
													// Partition modes for 8 x 8 sub partitions in P and B frames
													// Used for P and B frames to specify FORWARD, BACKWARD, etc.
	char quant;										// quantisation index
	int cbp;										// coded block pattern
    int mtype_intra_16x16;							// macroblock type integer, 0..23 for intra 16x16 sub modes

	char coded_block_flags;							// indicates whether the MB has been decoded and reconstructed. If not, then the MB can be considered lost and will be concealed


	char intra_chroma_prediction_mode;				// Intra chroma prediction mode


	char disable_deblocking_filter_idc;				// 0=normal, 1=no deblocking, 2=all except slice boundaries
	signed char slice_alpha_c0_offset;
	signed char slice_beta_offset;

    int:0;
													// Intra prediction modes for the 4x4 Intra mode
	UNSIGNED_SEPTET intra_prediction_mode[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];	

	union	// Same data as bytes or shorts
	{
		UNSIGNED_SEPTET mv_for_array_position[NUMBER_OF_VECTORS_PER_MACROBLOCK];	// Array position for forward multiple reference picture prediction
		unsigned short mv_for_array_position_SHORT[NUMBER_OF_VECTORS_PER_MACROBLOCK/2];	// Array position for forward multiple reference picture prediction
	};
	union	// Same data as bytes or shorts
	{
		UNSIGNED_SEPTET mv_for_remapped_index[NUMBER_OF_VECTORS_PER_MACROBLOCK];	// Remapped picture index for forward multiple reference picture prediction
		unsigned short mv_for_remapped_index_SHORT[NUMBER_OF_VECTORS_PER_MACROBLOCK/2];	// Remapped picture index for forward multiple reference picture prediction
	};
	
	union	// These two never used at same time
	{	
		u_motion_vector MotionVecs[NUMBER_OF_VECTORS_PER_MACROBLOCK]; // forward motion vectors
													// Intra prediction mode probabilities for the 4x4 Intra mode
		UNSIGNED_SEPTET intra_prediction_mode_probability[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];	
	};

				// These arrays store the number of non zero coefficients, and are used in 
				// CAVLC to determine the VLC table to be used for coeff_token
	union
	{
		UNSIGNED_SEPTET number_of_y_coefficients[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];
		int number_of_y_coefficients_INT[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS/4];		// version for fast clearing
	};
	union
	{
		UNSIGNED_SEPTET number_of_u_coefficients[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS];
		int number_of_u_coefficients_INT[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS/4];		// version for fast clearing
	};
	union
	{
		UNSIGNED_SEPTET number_of_v_coefficients[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS];
		int number_of_v_coefficients_INT[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS/4];		// version for fast clearing
	};


	inline const h264_decoder_control_t *GetLeft() const
	{
		return this-1;
	}
	inline const h264_decoder_control_t *GetRight() const
	{
		return this+1;
	}
	inline const h264_decoder_control_t *GetAbove() const
	{
		return( this - (MAX_RES_IN_MB_X+2));
	}
	inline h264_decoder_control_t *GetLeft_NC()
	{
		return this-1;
	}
	inline h264_decoder_control_t *GetRight_NC()
	{
		return this+1;
	}
	inline h264_decoder_control_t *GetAbove_NC()
	{
		return( this - (MAX_RES_IN_MB_X+2));
	}
};



#if 0
extern  void *      memset(void *__s, int __c, size_t __n);

#define AlignedIntMemClr(mem,  sz) {                            \
        if((int)mem & 3)                                        \
        {                                                       \
            printf("Align error %s %d\n",__FILE__,__LINE__);    \
        }                                                       \
        if(sz & 3)                                              \
        {                                                       \
            printf("Size error %s %d\n",__FILE__,__LINE__);     \
        }                                                       \
        memset(mem,0,sz);}

#else
inline void AlignedIntMemClr(void *mem,  const size_t sz)
{
    for(unsigned int i =0; i < (sz>>2); i++)
    {
        ((int *)mem)[i] = 0;
    }
}
#endif


#if 0
extern  void *      memset(void *__s, int __c, size_t __n);

#define AlignedIntMemSet(mem, val, sz) {                            \
        if((int)mem & 3)                                        \
        {                                                       \
            printf("Align error %s %d\n",__FILE__,__LINE__);    \
        }                                                       \
        if(sz & 3)                                              \
        {                                                       \
            printf("Size error %s %d\n",__FILE__,__LINE__);     \
        }                                                       \
        memset(mem,val,sz);}

#else

inline void AlignedIntMemSet(void *mem, const int val, const size_t sz)
{
//    int mv = val *0x01010101;
	int mv = val + (val << 8);
	mv = mv + (mv << 16);
    for(unsigned int i =0; i < (sz>>2); i++)
    {
        ((int *)mem)[i] = mv;
    }    
}
#endif




inline void ClearControl(h264_decoder_control_t *clearPtr)
{
    clearPtr->slice_id = INT_MAX;  // This is used to indicate not assigned a slice ID yet
	clearPtr->slice_group = 0;		// Not otherwise set in case of single slice group
	clearPtr->y_coeffs_present = 0;  // As blocks are found to have coeffs, individual bits are set
	clearPtr->Flags &= ~CTLF_IsIntraCoded;	// Only set if intra, probably a small fraction of MBs are intra
	clearPtr->coded_block_flags = false;
}


/* ------------------------------------------------------------------------ */
/* CLASS CH264DecoderControl												*/
/* ------------------------------------------------------------------------ */

class CH264DecoderControl
{
public:
	CH264DecoderControl(void);
	~CH264DecoderControl(void);

public:
	h264_decoder_control_t*	m_ControlList;
	h264_decoder_control_t*	m_CurrentElement;
	h264_decoder_control_t* m_ControlPtrs[MAX_TOTAL_MB];

	int m_hsize;				// horizontal picture size
	int m_vsize;				// vertical picture size

	int m_NumberOfMacroblocks;	// number of control blocks
	int m_MacroblocksPerRow;	// number of macroblocks per row of the picture
private:
#ifdef __INTERLACE__
	bool m_InitialisedForMBAFF;
#endif

public:
    bool SetSize(int hsize, int vsize);
	void Reset(void);

	inline h264_decoder_control_t*	SetAtStart(void)
    {
  	  m_CurrentElement = m_ControlList+MAX_RES_IN_MB_X+3;
	  return m_CurrentElement;
    }
    inline h264_decoder_control_t*	FirstCtrl() {return m_ControlList+MAX_RES_IN_MB_X+3;}
    
	inline h264_decoder_control_t*	GetNext(void)
    {
	  m_CurrentElement = m_CurrentElement->next;
	  return m_CurrentElement;
    }


	inline h264_decoder_control_t* GetMacroblock (int macroblock_number) 
    { 
      if ((macroblock_number >= 0) && (macroblock_number < m_NumberOfMacroblocks)) 
        return m_ControlPtrs[macroblock_number]; 
      else return 
        0; 
    }

private:
	bool CreateList(void);
	void DeleteList(void);

};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
