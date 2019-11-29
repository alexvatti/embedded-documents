/* ------------------------------------------------------------------------ */
/* FILE:			H264VLD.cpp												*/
/* DESCRIPTION:		H.264 variable length decoder class						*/
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

#ifdef __SYMBIAN32__
#include "stdafx.h"
#else
#include <string.h>  // For memset
#endif


#include "H264Warnings.h"

#include "CH264Decoder.h"
#include "H264DecoderControl.h"
#include "H264DecoderTables.h"
#include "H264DecodedPictureBuffer.h"
#include "H264VLDTables.h"
#include "H264OffsetTables.h"
#include "H264Tables.h"
#include "H264Transform.h"

#include "cavlc_api.h"

#include "vlc_api.h"

#ifdef FRAME_TIMES
extern void SetATime();
#endif

#if defined __GNUC__
#define _min(x,y)  (((x)>(y))?(y):(x))
#endif

#if defined __BENCHMARK__

#include "arc_benchmark.h"
extern short enable_benchmark;
extern FILE *benchmark_file;

extern "C" { 
  void stop_BPR();
  void run_BPR();
  void reset_BPR_Counters();
  void capture_BPR_benchmark_data();
}

#endif





// This is as clean as it can be made, but is still not pleasant.
// To allow the function ByteBufferDecodeUniversalVLC to be inline
// and hence run fast, it must be defined in the file in which it 
// is used, that is, it must be defined in this file. However, it 
// is nice to keep it with all the other ByteBuffer functions in
// H264ByteBuffer.cpp.
// So INCLUDED_IN_H264_VLD is defined and H264ByteBuffer.cpp is included,
// the result being only ByteBufferDecodeUniversalVLC is included here.

// At present all of this code is included in H264ByteBuffer.cpp
//#define INCLUDED_IN_H264_VLD
//#include	"H264ByteBuffer.cpp"





#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
#else
static const h264_vld_state_t NextStateTab[] = {H264_VLD_TCOEFF_LUMA_DC_END_OF_BLOCK,	//H264_LUMA_DC
                        H264_VLD_TCOEFF_LUMA_END_OF_BLOCK,								//H264_LUMA_AC
                        H264_VLD_TCOEFF_CHROMA_DC_END_OF_BLOCK,							//H264_CHROMA_DC
                        H264_VLD_TCOEFF_CHROMA_AC_END_OF_BLOCK,							//H264_CHROMA_AC
                        H264_VLD_LOST,H264_VLD_LOST,H264_VLD_LOST,H264_VLD_LOST,H264_VLD_LOST,H264_VLD_LOST};
#endif

char cbp_tab[] = {0, 0, 0, 0, 16, 16, 16, 16, 32, 32, 32, 32, 15, 15, 15, 15, 31, 31, 31, 31, 47, 47, 47, 47, 15, 15, 15, 15, 31, 31, 31, 31};


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDInitialise									*/
/* DESCRIPTION:	Initialises variables for the variable length decoding 		*/
/* state machine.															*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */
#ifdef I32K_D32K
#pragma Code("codesec1")
#endif
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec3")
#endif
#ifdef I16K_D8K
#pragma Code("codesec1")
#endif

bool CH264Decoder::VLDInitialise(void)
{
    m_VLDState = H264_VLD_LOST;
    m_VLDStateCount = 0;
    m_VLDCurrentPartitionMode = H264_UNKNOWN_PARTITION_MODE;
    
    m_FrameNumber = 0;
    m_PictureOrderCount = 0;
    
    m_VLDSliceHeaderFirstSlice = true;
    m_VLDSliceHeaderPreviousSliceNALReferenceIDC = 0;
    m_VLDSliceHeaderPreviousSliceIDRPictureID = 0;
    m_VLDSliceHeaderPreviousSliceWasIDR = false;
    m_VLDSliceHeaderPreviousReferenceSlicePOC = 0;
    m_VLDSliceHeaderPreviousFrameNumberOffset = 0;
    
    m_VLDQuant = 0;
    m_VLDQuantDiv = 0;
    m_VLDQuantMod = 0;
    m_VLDQuantChromaDiv = 0;
    m_VLDQuantChromaMod = 0;
    m_MotionVec.Both = 0;

    m_PictureParameterSet = 0;
    m_NumberOfMacroblocksDecoded = 0;
    m_VLDCurrentSliceGroup = 0;
    m_DisableDeblockingFilterIDC = 0;
    m_SliceAlphaC0Offset = 0;
    m_SliceBetaOffset = 0;
    
    
    
    // Initialise the VLC HW accelerator
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
    VLC_Reset();
    CAVLC_Reset();
#if !defined(ENABLE_INTERRUPT)
    // clear any pending interrupts in case the VLC/CAVLC decoder is activated after the use of legacy 
    // decoders which may cause problems. This is done in VLC_Reset() when interrupt is enabled.
    // this call also sets the decoder to EXP-Golomb decoder.
    VLC_Clear_Interrupt();
#endif
#endif

    return true;
}




// Copy tables into SDM
void CH264Decoder::InitSimdTables()
{
}


void AuroraRecMgr::RecordDecodeSliceDataMacros()
{
//    Record(AM_DS_, (NextAddr));
//    Record(AM_DS_, (NextAddr));
}


// I do so hate to have to do this, but otherwise every other line is a #if
// Can't be inline as needs to do a real return
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
// Only read one vector
#define READ8x8MV_O																\
        UVLC_Set_Control(UVLC_SE);												\
        m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();								\
        m_MotionVec.Comp.Ver = CAVLC_Read_Symbol()
#if 1	// ----- SHOULD BE FASTER BUT IS SLOWER most times
// Read first vector of more than 1
#define READ8x8MV_F																\
        UVLC_Set_Control(UVLC_SE);												\
        m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();								\
        m_MotionVec.Comp.Ver = CAVLC_Get_Symbol()
// Read middle vector of 3 or more
#define READ8x8MV_M																\
        m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();								\
        m_MotionVec.Comp.Ver = CAVLC_Get_Symbol()
// Read last vector
#define READ8x8MV_L																\
        m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();								\
        m_MotionVec.Comp.Ver = CAVLC_Read_Symbol()
#else	// -----
#define READ8x8MV_F READ8x8MV_O
#define READ8x8MV_M READ8x8MV_O
#define READ8x8MV_L READ8x8MV_O
#endif	// -----
#else
#define READ8x8MV_O																\
        if (!ByteBufferDecodeUniversalVLC(byteCode))							\
        	return;																\
        m_MotionVec.Comp.Hor = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);	\
        if (!ByteBufferDecodeUniversalVLC(byteCode))							\
        	return;																\
        m_MotionVec.Comp.Ver = VLDDecodeMapCodeNumberToSignedGolomb(byteCode)
// Only one version when no HW
#define READ8x8MV_F READ8x8MV_O
#define READ8x8MV_M READ8x8MV_O
#define READ8x8MV_L READ8x8MV_O
#endif


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDecode										*/
/* DESCRIPTION:	Operates the variable length decoding state machine.
The function first calls a function to decode a syntax 
element - the actual function called depending on whether
the SEs are CAVLC or CABAC encoded. Then a state machine 
acts on the decoded result, including changing the state. 
In some states no bits are decoded.							*/
/* RETURNS:		True if bits are still to be decoded, false if all are 
consumed.													*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::DecodeSliceData(int byteCode, h264_vld_state_t stateVLD)
{
    h264_decoder_control_t * control;
    
//    m_VLDFinishedDecodingPicture = false;
    int stateCount = 0;
    control = m_DecoderControl.m_CurrentElement;
    bool coefInBlk = false;
  	int blockNum = 0;
  	
    //	Loop while bits in buffer and not decoded a complete picture.
    while (true)
    {
        //assert(!m_PictureParameterSet->CABAC_entropy_coding_mode);
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Act on the variable length code that has been decoded. Or in the case of not having had to
        // decode one,  act as appropriate for the state.
        //
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        
        switch (stateVLD)
        {
        case H264_VLD_RUN:  
            {
			    int run;
			
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
			    UVLC_Set_Control(UVLC_UE);
			    run = CAVLC_Read_Symbol();
			    

#else
			    if (!ByteBufferDecodeUniversalVLC(run)) return;
#endif
			
			    			
			    while (run > 0)
			    {
			        // Decode the skipped macroblocks
			        control = m_DecoderControl.m_CurrentElement;
			        
			        control->mtype = H264_SKIPPED;
			        control->slice_id = m_CurrentSlice_ID;
			        InitMBFlags(control);
			        
			        AlignedIntMemClr(control->number_of_y_coefficients,
			        			NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS +
			        			NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS +
			        			NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS);
			        
			        
			        if (!VLDEndOfMacroblock(control, stateVLD))
			        	return;
			        
			        run--;
			    }
			    
			    
			    
			    stateVLD = H264_VLD_MB_TYPE;
			               
                break;
            }
            



        case H264_VLD_MB_TYPE:  
            {
        		control = m_DecoderControl.m_CurrentElement;

#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
			    UVLC_Set_Control(UVLC_UE);
#else
				if (!ByteBufferDecodeUniversalVLC(byteCode)) return;
#endif

			    control->quant = m_VLDQuant;
			    control->slice_id = m_CurrentSlice_ID;
			    InitMBFlags(control);
			    
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
			    byteCode = CAVLC_Read_Symbol();
#else
				// HW version does this as it goes/ when required
                AlignedIntMemClr(m_y_dc_data, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));
			    AlignedIntMemClr(m_u_dc_data, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));
			    AlignedIntMemClr(m_v_dc_data, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));
#endif
			                
			
			    if (H264_I_SLICE == m_SliceType)
			    {
			        control->Flags |= CTLF_IsIntraCoded;
			        control->partition_modes = 0; // H264_8X8_PARTITION,H264_8X8_PARTITION,H264_8X8_PARTITION,H264_8X8_PARTITION                    
			        
			//                    AlignedIntMemSet(control->mv_for_remapped_index, DIFFERENT_REFERENCE_FRAME, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			
			        if (0 == byteCode)
			        {
			            control->mtype = H264_INTRA_4X4;
			            
			            // 4x4 mode followed by eight Intra Prediction mode code words
			            
			            stateVLD = H264_VLD_INTRA_PRED_MODE_FLAG;
			        }
			        else if (25 == byteCode)
			        {
			            control->mtype = H264_I_PCM;
			            
			            stateVLD = H264_VLD_I_PCM_BYTE_ALIGNMENT;
			        }
			        else
			        {
			            control->mtype = H264_INTRA_16X16;
			            control->mtype_intra_16x16 = byteCode - 1;
			            
			            // Create a cbp from the 16x16 type
			            // The AC value in the code table indicates all or no luma blocks,
			            // hence results in a contribution to cbp of either 0 or 15.
			            // The nc value contributes 0, 16, or 32 as normal.
			            			            
			            control->cbp = cbp_tab[control->mtype_intra_16x16];
			            
			            // One of the 16x16 modes
			            stateVLD = H264_VLD_INTRA_CHROMA_PRED_MODE;
			        }
			    }
				else if (H264_P_SLICE == m_SliceType)
			    {
			        
			        if (byteCode < 3)
			        {
			            // Mode is 16x16 (0), 16x8 (1) or 8x16 (2)
			            control->mtype = (h264_mb_type_t) byteCode;
			            control->partition_modes = 0;
			            
			            if(control->mtype == H264_INTER_16X16)
			            {
			           		m_VLDList0MVCount = 1;
			            }
			            else
			            {
			           	 	m_VLDList0MVCount = 2;
			            }
			            
			            {
			                
			                UNSIGNED_SEPTET array_position;
			
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						    if ((m_SliceNumList0RefPictures <= 1)/* || 
						        (H264_INTER_8X8_ZERO_REF == m_DecoderControl.m_CurrentElement->mtype)*/) 
						    {
						        // Nothing to do if m_SliceNumList0RefPictures < 2 or 8x8 zero ref
						        array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(0);
			                    AlignedIntMemSet(control->mv_for_array_position, array_position, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			                    AlignedIntMemClr(control->mv_for_remapped_index, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
						    }
						    else
#endif                
			                if (H264_INTER_16X16 == control->mtype)
			                {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						        if(2 == m_SliceNumList0RefPictures)
						        {
						            // Read one bit and invert it
						            UVLC_Set_Control(UVLC_TE | UVLC_TE_RANGE_EQ1);
						        }
						        else
						        {
						            UVLC_Set_Control(UVLC_TE);
						        }
						        byteCode = CAVLC_Read_Symbol();
#else
				                if (!DecodeCAVLCList0RefIndex(byteCode))
				                	return;
#endif                    
				                array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
			                    AlignedIntMemSet(control->mv_for_array_position, array_position, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			                    AlignedIntMemSet(control->mv_for_remapped_index, byteCode, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			                }
			                else if (H264_INTER_16X8 == control->mtype)
			                {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						        if(2 == m_SliceNumList0RefPictures)
						        {
						            // Read one bit and invert it
						            UVLC_Set_Control(UVLC_TE | UVLC_TE_RANGE_EQ1);
						        }
						        else
						        {
						            UVLC_Set_Control(UVLC_TE);
						        }
						        byteCode = CAVLC_Get_Symbol();
#else
				                if (!DecodeCAVLCList0RefIndex(byteCode))
				                	return;
#endif                    
				                array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
			                    AlignedIntMemSet(control->mv_for_array_position, array_position, 8*sizeof(UNSIGNED_SEPTET));
			                    AlignedIntMemSet(control->mv_for_remapped_index, byteCode, 8*sizeof(UNSIGNED_SEPTET));
			                        
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						        byteCode = CAVLC_Read_Symbol();
#else
			                	if (!DecodeCAVLCList0RefIndex(byteCode))
			                		return;
#endif                    
			                	array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
			                    AlignedIntMemSet(&control->mv_for_array_position[8], array_position, 8*sizeof(UNSIGNED_SEPTET));
			                    AlignedIntMemSet(&control->mv_for_remapped_index[8], byteCode, 8*sizeof(UNSIGNED_SEPTET));
			                }
			                else if (H264_INTER_8X16 == control->mtype)
			                {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						        if(2 == m_SliceNumList0RefPictures)
						        {
						            // Read one bit and invert it
						            UVLC_Set_Control(UVLC_TE | UVLC_TE_RANGE_EQ1);
						        }
						        else
						        {
						            UVLC_Set_Control(UVLC_TE);
						        }
						        byteCode = CAVLC_Get_Symbol();
#else
			                	if (!DecodeCAVLCList0RefIndex(byteCode))
			                		return;
#endif                    
			                	array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
			                    unsigned short value = ((unsigned short)array_position << 8) | array_position;
			                    control->mv_for_array_position_SHORT[0] = value;
			                    control->mv_for_array_position_SHORT[2] = value;
			                    control->mv_for_array_position_SHORT[4] = value;
			                    control->mv_for_array_position_SHORT[6] = value;
			                    
			                    value = ((unsigned short)byteCode << 8) | ((unsigned short)byteCode & 0x00FF);
			                    control->mv_for_remapped_index_SHORT[0] = value;
			                    control->mv_for_remapped_index_SHORT[2] = value;
			                    control->mv_for_remapped_index_SHORT[4] = value;
			                    control->mv_for_remapped_index_SHORT[6] = value;
			                        
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						        byteCode = CAVLC_Read_Symbol();
#else
			                	if (!DecodeCAVLCList0RefIndex(byteCode))
			                		return;
#endif                    
			                	array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
			                    value = ((unsigned short)array_position << 8) | array_position;
			                    control->mv_for_array_position_SHORT[1] = value;
			                    control->mv_for_array_position_SHORT[3] = value;
			                    control->mv_for_array_position_SHORT[5] = value;
			                    control->mv_for_array_position_SHORT[7] = value;
			                    
			                    value = ((unsigned short)byteCode << 8) | ((unsigned short)byteCode & 0x00FF);
			                    control->mv_for_remapped_index_SHORT[1] = value;
			                    control->mv_for_remapped_index_SHORT[3] = value;
			                    control->mv_for_remapped_index_SHORT[5] = value;
			                    control->mv_for_remapped_index_SHORT[7] = value;
			                }
			                else
			                {
			                    return;
			                }
			                if (m_VLDList0MVCount == 0)
			                {
			                    return;
			                }
			            }
		//        state  H264_VLD_MVD_FORWARD ---------------  
		                {
		
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
		                    UVLC_Set_Control(UVLC_SE);
		                    
	                        if(m_VLDList0MVCount == 1)
	                        {
			                    m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();
			                    m_MotionVec.Comp.Ver = CAVLC_Read_Symbol();
			                    
		                        VLDDecodeDifferentialMotionVector(control, 
		                                h264_block_offset_table[(h264_block_shape_type_t)(control->mtype)][0],
		                                0, (h264_block_shape_type_t) (control->mtype));
		                    }
	                        else
	                        {
			                    m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();
			                    m_MotionVec.Comp.Ver = CAVLC_Get_Symbol();
			                    
		                        VLDDecodeDifferentialMotionVector(control, 
		                                h264_block_offset_table[(h264_block_shape_type_t)(control->mtype)][0],
		                                0, (h264_block_shape_type_t) (control->mtype));
	                                
		                    	m_MotionVec.Comp.Hor = CAVLC_Get_Symbol();
			                    m_MotionVec.Comp.Ver = CAVLC_Read_Symbol();
			                    VLDDecodeDifferentialMotionVector(control, 
			                            h264_block_offset_table[(h264_block_shape_type_t)(control->mtype)][1],
			                            1, (h264_block_shape_type_t) (control->mtype));
		                    }
		                    stateVLD = H264_VLD_CBP;
		                    break;
		                    
#else
		                    int VLDStateCount = 0;
		                    while(true)
		                    {
		                        if (!ByteBufferDecodeUniversalVLC(byteCode)) return;
		                        // VLDStateCount is used to count the motion vector components
		                        if (0 == (VLDStateCount & 1))
		                        {
		                            m_MotionVec.Comp.Hor = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
		                        }
		                        else
		                        {
		                            m_MotionVec.Comp.Ver = VLDDecodeMapCodeNumberToSignedGolomb(byteCode);
		                            
		                            VLDDecodeDifferentialMotionVector(control, 
		                                    h264_block_offset_table[(h264_block_shape_type_t)(control->mtype)][VLDStateCount >> 1],
		                                    VLDStateCount >> 1, (h264_block_shape_type_t) (control->mtype));
		                        }
		                        
		                        // Check whether all vectors have been decoded
		                        
		                        if ((m_VLDList0MVCount<<1) == (UNSIGNED_SEPTET)++VLDStateCount)
		                        {
		                            stateVLD = H264_VLD_CBP;
		                            break;
		                        }
		                    } // end of while-loop
		                    break;
#endif                    
		                }
			
			        }
			        else if (5 > byteCode)
			        {
				        if (3 == byteCode)
				        {
				            // Mode is 8x8_multi_ref
				            control->mtype = H264_INTER_8X8_MULTI_REF;
				        }
				        else
				        {
				            // Mode is 8x8_zero_ref
				            UNSIGNED_SEPTET array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(0);
				            control->mtype =  H264_INTER_8X8_ZERO_REF;
				            			            
				            // Set the picture index to the array position of the picture 
				            // with remapped index of zero.
				            AlignedIntMemSet(control->mv_for_array_position, array_position, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));				            
				        }


		
		//            case H264_VLD_8X8_REGION_TYPE:
						{
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						    UVLC_Set_Control(UVLC_UE);
						    control->partition_mode[0] = CAVLC_Get_Symbol();
						    control->partition_mode[1] = CAVLC_Get_Symbol();
						    control->partition_mode[2] = CAVLC_Get_Symbol();
						    control->partition_mode[3] = CAVLC_Read_Symbol();
#else
							for(int i = 0; i < 4; i++)
							{
							    if (!ByteBufferDecodeUniversalVLC(byteCode))
							    	return;
							    control->partition_mode[i] = (h264_8x8_partition_mode_type_t)(byteCode);
							}
#endif
					        // Read the four region types - now go back and parse the next stuff from the first region types
					        
					        // First of all calculate direct motion vectors if any of the 8x8 partitions are direct
					            
					            // Loop around to see if we need a list 0 index
					            
					        m_VLDCurrentPartitionMode = control->partition_mode[0];
		
						}
						
		//            case H264_VLD_8X8_LIST0_REF_INDEX:
			            {
			                int vld88c = 0;
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
						    if ((m_SliceNumList0RefPictures <= 1) || 
						        (H264_INTER_8X8_ZERO_REF == control->mtype)) 
						    {
						        // Nothing to do if m_SliceNumList0RefPictures < 2 or 8x8 zero ref
						        UNSIGNED_SEPTET array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(0);
			                    AlignedIntMemSet(control->mv_for_array_position, array_position, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			                    AlignedIntMemClr(control->mv_for_remapped_index, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
						    }
						    else
#endif                
			                {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
							    if(2 == m_SliceNumList0RefPictures)
							    {
							        // Read one bit and invert it
							        UVLC_Set_Control(UVLC_TE | UVLC_TE_RANGE_EQ1);
							    }
							    else
							    {
							        UVLC_Set_Control(UVLC_TE);
							    }
							    byteCode = CAVLC_Get_Symbol();
#else
			                    if (!DecodeCAVLCList0RefIndex(byteCode)) return;
#endif
			                    UNSIGNED_SEPTET array_position;
			                    
			                    array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
								control->mv_for_array_position[0+0] = array_position;
								control->mv_for_array_position[0+1] = array_position;
								control->mv_for_array_position[0+4] = array_position;
								control->mv_for_array_position[0+5] = array_position;
								control->mv_for_remapped_index[0+0] = byteCode;
								control->mv_for_remapped_index[0+1] = byteCode;
								control->mv_for_remapped_index[0+4] = byteCode;
								control->mv_for_remapped_index[0+5] = byteCode;
		
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
							    byteCode = CAVLC_Get_Symbol();
#else
			                    if (!DecodeCAVLCList0RefIndex(byteCode)) return;
#endif
			                    // 2 == unsigned int first_block_number = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
			                    	                    
			                    array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
								control->mv_for_array_position[2+0] = array_position;
								control->mv_for_array_position[2+1] = array_position;
								control->mv_for_array_position[2+4] = array_position;
								control->mv_for_array_position[2+5] = array_position;
								control->mv_for_remapped_index[2+0] = byteCode;
								control->mv_for_remapped_index[2+1] = byteCode;
								control->mv_for_remapped_index[2+4] = byteCode;
								control->mv_for_remapped_index[2+5] = byteCode;
		
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
							    byteCode = CAVLC_Get_Symbol();
#else
			                    if (!DecodeCAVLCList0RefIndex(byteCode)) return;
#endif
			                    // 8 == unsigned int first_block_number = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
			                    	                    
			                    array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
								control->mv_for_array_position[8+0] = array_position;
								control->mv_for_array_position[8+1] = array_position;
								control->mv_for_array_position[8+4] = array_position;
								control->mv_for_array_position[8+5] = array_position;
								control->mv_for_remapped_index[8+0] = byteCode;
								control->mv_for_remapped_index[8+1] = byteCode;
								control->mv_for_remapped_index[8+4] = byteCode;
								control->mv_for_remapped_index[8+5] = byteCode;
		
		
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
							    byteCode = CAVLC_Read_Symbol();
#else
			                    if (!DecodeCAVLCList0RefIndex(byteCode)) return;
#endif
			                    // 10 == unsigned int first_block_number = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
			                    	                    
			                    array_position = m_DecodedPictureBuffer->RemappedIndexList0ToArrayPosition(byteCode);
								control->mv_for_array_position[10+0] = array_position;
								control->mv_for_array_position[10+1] = array_position;
								control->mv_for_array_position[10+4] = array_position;
								control->mv_for_array_position[10+5] = array_position;
								control->mv_for_remapped_index[10+0] = byteCode;
								control->mv_for_remapped_index[10+1] = byteCode;
								control->mv_for_remapped_index[10+4] = byteCode;
								control->mv_for_remapped_index[10+5] = byteCode;
			
		                        // Read all reference frames, now go on to motion vectors
		                        vld88c = 0;
		                        
		                        // Find an 8x8 block with forward motion vectors (there must be at least one!)
		                		m_VLDCurrentPartitionMode = control->partition_mode[0];
		//                        stateVLD = H264_VLD_8X8_FORWARD_MVD;
				                    
			            	}
		// Case runs through ...................................................................                
		
		//            case H264_VLD_8X8_FORWARD_MVD:  
							while(true)
							{
								
								switch(m_VLDCurrentPartitionMode)
								{
								case H264_8X8_PARTITION:
									READ8x8MV_O;
									VLDDecodeDifferentialMotionVector(
											control,
											((vld88c & 1) << 1) + ((vld88c & 2) << 2),
											vld88c,
											H264_8X8);
									break;
									
								case H264_8X4_PARTITION:
									{
										int blockIndex = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
										int vectorNumber = (vld88c & 1) + ((vld88c & 2) << 1);
										READ8x8MV_F;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex,
												vectorNumber,
												H264_8X4);
				
										READ8x8MV_L;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex + 4,
												vectorNumber + 2,
												H264_8X4);
									}
									break;
									
								case H264_4X8_PARTITION:
									{
										int blockIndex = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
										int vectorNumber = vld88c << 1;
										READ8x8MV_F;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex,
												vectorNumber,
												H264_4X8);
				
										READ8x8MV_L;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex + 1,
												vectorNumber + 1,
												H264_4X8);
									}
									break;
									
								case H264_4X4_PARTITION:
									{
										int blockIndex = ((vld88c & 1) << 1) + ((vld88c & 2) << 2);
										READ8x8MV_F;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex,
												blockIndex,
												H264_4X4);
										READ8x8MV_M;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex+1,
												blockIndex+1,
												H264_4X4);
										READ8x8MV_M;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex+4,
												blockIndex+4,
												H264_4X4);
										READ8x8MV_L;
										VLDDecodeDifferentialMotionVector(
												control,
												blockIndex+5,
												blockIndex+5,
												H264_4X4);
									}
									break;
								}
								
			                    if (4 == ++vld88c)
			                    {
			                        // All partitions now considered, move on to other syntax
			                        stateVLD = H264_VLD_CBP;
			                        break;
			                    }
								m_VLDCurrentPartitionMode = control->partition_mode[vld88c];
							}
			            }
						break;

			        }
			        else if (5 == byteCode) // Slight remap of internals symbols H264_INTRA_4X4
			        {
			            control->mtype = H264_INTRA_4X4;
			            
			            control->Flags |= CTLF_IsIntraCoded;
			            control->partition_modes = 0;
			            AlignedIntMemSet(control->mv_for_remapped_index, DIFFERENT_REFERENCE_FRAME, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			            
			            {
			                // 4x4 mode followed by eight Intra Prediction mode code words
			                stateVLD = H264_VLD_INTRA_PRED_MODE_FLAG;
			            }
			        }
			        else if (30 == byteCode)
			        {
			            control->mtype = H264_I_PCM;
			            control->partition_modes = 0;
			                                    
			            AlignedIntMemSet(control->mv_for_array_position, DIFFERENT_REFERENCE_FRAME, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			            AlignedIntMemSet(control->mv_for_remapped_index, DIFFERENT_REFERENCE_FRAME, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			            
			            stateVLD = H264_VLD_I_PCM_BYTE_ALIGNMENT;
			        }
			        else
			        {
			            control->mtype = H264_INTRA_16X16;
			            control->mtype_intra_16x16 = byteCode - 6;
			            
			            control->Flags |= CTLF_IsIntraCoded;
						control->partition_modes = 0;                        
			            
			            AlignedIntMemSet(control->mv_for_remapped_index, DIFFERENT_REFERENCE_FRAME, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
			            
			            // Create a cbp from the 16x16 type
			            // The AC value in the code table indicates all or no luma blocks,
			            // hence results in a contribution to cbp of either 0 or 15.
			            // The nc value contributes 0, 16, or 32 as normal.
			//                        control->cbp = 0;
			//                        if (control->mtype_intra_16x16 > 11) control->cbp += 15;
			//                        
			//                        control->cbp += ((control->mtype_intra_16x16 >> 2) % 3) << 4;
			            control->cbp = cbp_tab[control->mtype_intra_16x16];
			            // One of the 16x16 modes
			            stateVLD = H264_VLD_INTRA_CHROMA_PRED_MODE;
			        }
				}
			    else
			    {
			        return;
			    }

                stateCount = 0;
                break;
            }
            
            // --------------------------------------------------------------

            
        case H264_VLD_INTRA_PRED_MODE_FLAG: 
            {
           	
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><> //----------------------- 

//	 0,  1,  4,  5,  2,  3,  6,  7,  8,  9, 12, 13,   10, 11, 14, 15
				UNSIGNED_SEPTET *ipmp = control->intra_prediction_mode_probability;
				UVLC_Set_Control(I1A3);
				// Unrolled loop
                // 0
            	ipmp[0] = CAVLC_Get_Symbol();
                // 1
            	ipmp[1] = CAVLC_Get_Symbol();
                // 2
            	ipmp[4] = CAVLC_Get_Symbol();
                // 3
            	ipmp[5] = CAVLC_Get_Symbol();
                // 4
            	ipmp[2] = CAVLC_Get_Symbol();
                // 5
            	ipmp[3] = CAVLC_Get_Symbol();
                // 6
            	ipmp[6] = CAVLC_Get_Symbol();
                // 7
            	ipmp[7] = CAVLC_Get_Symbol();
                // 8
            	ipmp[8] = CAVLC_Get_Symbol();
                // 9
            	ipmp[9] = CAVLC_Get_Symbol();
                // 10
            	ipmp[12] = CAVLC_Get_Symbol();
                // 11
            	ipmp[13] = CAVLC_Get_Symbol();
                // 12
            	ipmp[10] = CAVLC_Get_Symbol();
                // 13
            	ipmp[11] = CAVLC_Get_Symbol();
                // 14
            	ipmp[14] = CAVLC_Get_Symbol();
                // 15
            	ipmp[15] = CAVLC_Get_Symbol();


#else //-----------------------
            	AlignedIntMemClr(control->intra_prediction_mode_probability, 16);
                for(int stCnt = 0; stCnt < 16; stCnt++)
                {
                    ByteBufferDecodeFixedLengthCode(1, byteCode);
                    
                    
					if( !byteCode)
                    {
                        // Not most probable mode, read extra bits to determine probability
                        ByteBufferDecodeFixedLengthCode(3, byteCode);  
                        control->intra_prediction_mode_probability[h264_4x4_transmission_to_raster_table[stCnt]]
                            = (UNSIGNED_SEPTET)(1 + byteCode);
                    }
                }
#endif //-----------------------            	
            	
            }

// D A N G E R   T H E S E   C A S E S   R U N   T H R O U G H            

        case H264_VLD_INTRA_CHROMA_PRED_MODE:   
            {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                UVLC_Set_Control(UVLC_UE);
                if (H264_INTRA_16X16 == control->mtype)
                {
                    stateVLD = H264_VLD_DQUANT;
                }
                else
                {
                    stateVLD = H264_VLD_CBP;
                }
                control->intra_chroma_prediction_mode = CAVLC_Read_Symbol();
                break;
#else
                if (!ByteBufferDecodeUniversalVLC(byteCode)) return;
                control->intra_chroma_prediction_mode = byteCode;
                if (H264_INTRA_16X16 == control->mtype)
                {
                    stateVLD = H264_VLD_DQUANT;
                }
                else
                {
                    stateVLD = H264_VLD_CBP;
                }
                break;
#endif
            }
            
                
                // --------------------------------------------------------------
            case H264_VLD_CBP:  
                {
                    {
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
                        if (!ByteBufferDecodeUniversalVLC(byteCode)) return;
#endif
                        if ((H264_INTRA_4X4 == control->mtype) || (H264_S_INTRA_4X4 == control->mtype))
                        {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                            UVLC_Set_Control(CAVLC_BLK_INTRA_4X4 | UVLC_ME);
#else
                            control->cbp = h264_decoder_intra_cbp_table[byteCode];
#endif
                        }
                        else
                        {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                            UVLC_Set_Control(CAVLC_BLK_INTER | UVLC_ME);
#else
                            control->cbp = h264_decoder_inter_cbp_table[byteCode];
#endif
                        }
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                        control->cbp = CAVLC_Read_Symbol();
#endif
                    }
                    
                    // DQUANT is present if CBP != 0 (or MB is 16x16 Intra coded, in which case CBP is not explicitly coded)
                    if (0 != control->cbp)
                    {
                    	stateVLD = H264_VLD_DQUANT;
                    }
                    else
                    {
                        // Nothing more to decode for the macroblock
                        AlignedIntMemClr(control->number_of_y_coefficients, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                        AlignedIntMemClr(control->number_of_u_coefficients, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                        AlignedIntMemClr(control->number_of_v_coefficients, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                        
                        if (!VLDEndOfMacroblock(control, stateVLD))
                        	return;
                    	break;
                    }
                    
                }
                
 // Case runs through . . . . . . . . . . . . . . . . . . . . . . . .
 
            case H264_VLD_DQUANT:   
                {
                    // Store code number in m_VLDPrevQPDelta for CABAC
                    {
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                        UVLC_Set_Control(UVLC_SE);
                        byteCode = CAVLC_Read_Symbol();
#else
                        if (!ByteBufferDecodeUniversalVLC(byteCode)) return;
#endif
                    }
                    // If byteCode == 0, no change to m_VLDQuant
                    if (0 != byteCode)
                    {
                        UNSIGNED_SEPTET chroma_QP;
                        
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                        m_VLDQuant = (52 + m_VLDQuant + byteCode) % 52;
#else
                        m_VLDQuant = (52 + m_VLDQuant + VLDDecodeMapCodeNumberToSignedGolomb(byteCode)) % 52;
#endif
                        m_VLDQuantDiv = QUANT_DIV(m_VLDQuant);
                        m_VLDQuantMod = QUANT_MOD(m_VLDQuant, m_VLDQuantDiv);
                        
                        chroma_QP = h264_quantiser_chroma_mapping_table[
                            CLIP_CHROMA_QUANT((int)(m_VLDQuant) + m_PictureParameterSet->chroma_qp_index_offset)];
                        m_VLDQuantChromaDiv = QUANT_DIV(chroma_QP);
                        m_VLDQuantChromaMod = QUANT_MOD(chroma_QP, m_VLDQuantChromaDiv);
                        
                    }
                    
                    control->quant = m_VLDQuant;
                    
                    if (H264_INTRA_16X16 == control->mtype)
                    {
                        stateVLD = H264_VLD_TCOEFF_LUMA_DC_START;
                    }
                    else
                    {
                        int cbp = control->cbp;
                        if ((cbp & 15) != 0)
                        {
                            stateVLD = H264_VLD_TCOEFF_LUMA_START;
                        }
                        else // No luma data
                        {
                            AlignedIntMemClr(control->number_of_y_coefficients, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                            if ((cbp >> 4) != 0)
                            {
                                // Chroma DC is present
                                stateVLD = H264_VLD_TCOEFF_CHROMA_DC_START;
                            }
                            else
                            {
                                AlignedIntMemClr(control->number_of_u_coefficients, 2 * NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                                
                                if (!VLDEndOfMacroblock(control, stateVLD)) return;
                            }
                        }
                    }
                    break;
                }
                
                // --------------------------------------------------------------
            // --------------------------------------------------------------
            case H264_VLD_TCOEFF_LUMA_DC_START: 
                {
                    // Initialise variables for decoding the DC luma for the 4x4 blocks
                    
                    blockNum = 0;
                    coefInBlk = false;
                    
                    m_VLDBlockSize = 16;
                    // Determine the VLC table to use for decoding coeff token
                    m_VLDCoeffTokenTable = VLDDetermineVLCTableForLumaCoeffToken0(control);

#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>  
                	DecodeCAVLCCoefficient(control);
#else
                    m_VLDBlockType = H264_LUMA_DC;
                    stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;
                    break;
                }

            case H264_VLD_TCOEFF_LUMA_DC_END_OF_BLOCK:  		// No VLC HW
                {
#endif
                    // Perform inverse run-level scanning for the block
                    if (m_VLDTotalCoefficients > 0)
                    {
                    	AlignedIntMemClr(m_y_dc_data, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));
                    	
                        short level;
                        int total_run = 0;
                        int run_level_pair;
                        
                        // Loop over all non zero levels
                        for (run_level_pair=0; run_level_pair<m_VLDTotalCoefficients; run_level_pair++)
                        {
                            level = m_VLDDecodedLevelData[run_level_pair];
                            total_run += m_VLDDecodedRunData[run_level_pair];
                            
                            //if (total_run > 15) total_run = 15; // This will not normally occur but may in an error environment
                            total_run &= 15;
                            // Insert coeff into DC block at correct position - Inverse quantisation is done later
                            m_y_dc_data[frame_mb_zig_zag_luma_dc_table[total_run]] = level;
                            
                            total_run++;
                        }
                    // Perform inverse quantisation and inverse transform for the block
                                            
                        
                        // Perform inverse 4x4 transform, inverse quantise and scale and insert in top left of 4x4 blocks
                        Inverse4x4DCTransform(m_y_dc_data);
                        
                        
                        const char qtabVal = h264_frame_scan_inverse_quantiser_table[m_VLDQuantMod][0];
                        
                        
                        
                        
                        int i;
                        short *yData = (short*)(m_y_data+Simdu->GetSDMStartAdd());
						int yCoefPres = control->y_coeffs_present;
                        if (m_VLDQuantDiv >= 2)
                        {
                        	short q = qtabVal << (m_VLDQuantDiv - 2);
                            for (i=0; i<16; i++)
                            {
                                // i.e. QP >= 12
                                short t = m_y_dc_data[i] * q;
                                yData[top_left_of_luma_4x4_block_raster_order_table[i]] = t;
                            
                                if (0 != t)
                                {
                                    yCoefPres |= (1 << i);
                                }
                        	}
                    	}
                    	else
                    	{
                    		short q = 1 << (1 - m_VLDQuantDiv);
                    		short r = 2 - m_VLDQuantDiv;
                            for (i=0; i<16; i++)
                            {
                                short t = (m_y_dc_data[i] * qtabVal + q) >> r;
                             	yData[top_left_of_luma_4x4_block_raster_order_table[i]] = t;
                                if (0 != t)
                                {
                                    yCoefPres |= (1 << i);
                                }
                      		}
                    	}
                        control->y_coeffs_present = yCoefPres;
                    }
                    
                    
                    if ((control->cbp & 15) != 0)
                    {
                        stateVLD = H264_VLD_TCOEFF_LUMA_START;
                    }
                    else
                    {
                        // Luma AC is not present, so do the inverse transforms here
                        int i;
                        AlignedIntMemClr(control->number_of_y_coefficients, NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                        short *yData = (short*)(m_y_data+Simdu->GetSDMStartAdd());
                        for (i=0; i<16; i++)
                        {
                        	short *yPtr = yData + top_left_of_luma_4x4_block_bitstream_order_table[i];
                            if (0 != *yPtr)
                            {
                                Inverse4x4TransformDCOnly(yPtr, 16);
                            }
                        }
                        
                        if ((control->cbp >> 4) != 0)
                        {
                            // Chroma DC is present
                            stateVLD = H264_VLD_TCOEFF_CHROMA_DC_START;
                        }
                        else
                        {
                            AlignedIntMemClr(control->number_of_u_coefficients, 2* NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                            

                            if (!VLDEndOfMacroblock(control, stateVLD)) return;
                        }
                    }
                    break;
                }
                
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_LUMA_START:    
                {
                    int i;
                    // Initialise variables for decoding the luminance for the 4x4 blocks
                    // First block number is 0, 4, 8 or 12 depending on cbp
                    
                    blockNum = 0;
                    for (i=0; i<4; i++)
                    {
                        if (control->cbp & (1<< i) ) break; // found non zero cbp bit
                        
                        // This 8x8 block has no coeffs, so increase block number by 4
#ifndef CAVLC_HW_PRESENT
                        control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]  ] = 0;
                        control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]+1] = 0;
                        control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]+4] = 0;
                        control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]+5] = 0;
#else
						control->number_of_y_coefficients_INT[blockNum >> 2 ] = 0;
#endif                        
                        
                        blockNum += 4;
                    }
                    
                    if (H264_INTRA_16X16 == control->mtype)
                    {
                        short *yData = (short*)(m_y_data+Simdu->GetSDMStartAdd());
                        coefInBlk = (yData[top_left_of_luma_4x4_block_bitstream_order_table[blockNum]] != 0);
                    }
                    else
                    {
                        coefInBlk = false;
                    }
                    
                    m_VLDBlockSize = 16 - (H264_INTRA_16X16 == control->mtype);
                    
                    // Determine the VLC table to use for decoding coeff token
                    m_VLDCoeffTokenTable = VLDDetermineVLCTableForLumaCoeffToken(control, blockNum);

#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                    control->number_of_y_coefficients[blockNum] = DecodeCAVLCCoefficient(control);
                    stateVLD = H264_VLD_TCOEFF_LUMA_END_OF_BLOCK;
//                    break;
                }
#else
                    m_VLDBlockType = H264_LUMA_AC;
                    stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;                    
                    break;
                }
                
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_LUMA_END_OF_BLOCK: 		// No VLC HW
#endif
                do
                {
                    // Perform inverse quantisation for the block
                    if (m_VLDTotalCoefficients > 0)
                    {
                        short level;
                        int total_run = 0;
                        
                        
                        coefInBlk = true;
                        
                        control->y_coeffs_present |= h264_4x4_transmission_to_raster_mask_table[blockNum];
                        
                        if (H264_INTRA_16X16 == control->mtype)
                        {
                            total_run = 1;  // Start at 1 if intra 16x16
                        }
                        
                        
                        const char *h264_inverse_quantiser_table = h264_frame_scan_inverse_quantiser_table[m_VLDQuantMod];
                        
                        // Loop over all non zero levels
                        int run_level_pair;
						if(m_VLDTotalCoefficients)
                        {
                            short *yData = (short*)(m_y_data+Simdu->GetSDMStartAdd())
                            		+top_left_of_luma_4x4_block_bitstream_order_table[blockNum];
                            for (run_level_pair=0; run_level_pair<m_VLDTotalCoefficients; run_level_pair++)
                            {
                                total_run += m_VLDDecodedRunData[run_level_pair];
                                level = m_VLDDecodedLevelData[run_level_pair];
                                
                                //if(total_run>15) total_run = 15;
                                total_run &= 15;
                                
                                // Insert coeff into block at correct position and perform inverse quantisation
                                    // Ordinary inverse quantisation
                                yData[transposed_frame_mb_zig_zag_luma_table[total_run]]
                                        = level * h264_inverse_quantiser_table[total_run] << m_VLDQuantDiv;
                               
                                total_run++;
                            }
                        }
                            
                    }
                    // Perform inverse transform if coefficients have been decoded and not SP block
                    // Move to next block
                    blockNum++;
                    
                    // Is the block part of one transmitted according to cbp?
                    // Create 8x8 block number by shifting right 2 bits, then make into
                    // a cbp mask by shifting 1 left the given number of times.
                    // Hences masks will be created in the order 1,1,1,1,2,2,2,2,4,...
                    
                    while (16 != blockNum)
                    {
                        unsigned int mask = 1 << (blockNum >> 2);
                        
                        if (0 != (control->cbp & mask))
                        {
                            break;
                        }
                        else
                        {
                            // This 8x8 block has no coeffs, so increase block number by 4
#ifndef CAVLC_HW_PRESENT                            
                            unsigned short *sptr = (unsigned short *)&control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]];
                            sptr[0] = 0;
                            sptr[2] = 0;
#else
							control->number_of_y_coefficients_INT[blockNum >> 2 ] = 0;
#endif
                            blockNum += 4;
                        }
                    }
                    
                    if (blockNum > 15)
                    {
                        // perform 4x4 transform on each block
                        Simdu->Inverse2DLumaTransform( m_y_data);
                        if( control->cbp & 0xf0)
                        {
                            // Chroma is present
                            stateVLD = H264_VLD_TCOEFF_CHROMA_DC_START;
                            break;
                        }
                        else
                        {
                            AlignedIntMemClr(control->number_of_u_coefficients,2*NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));

                            if (!VLDEndOfMacroblock(control, stateVLD))
                            	return;
                        }
                    }
                    else
                    {
                        // The next state is to decode coeff token
                        
                        // Determine the VLC table to use for decoding coeff token
                        m_VLDCoeffTokenTable = VLDDetermineVLCTableForLumaCoeffToken(control, blockNum);
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                        control->number_of_y_coefficients[blockNum] = DecodeCAVLCCoefficient(control);
#else
                        stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;
#endif                        
                    }
                    
                }while(stateVLD == H264_VLD_TCOEFF_LUMA_END_OF_BLOCK );
                break;
            
            // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CHROMA_DC_START:   
                
                    // Initialise variables for decoding the DC chroma for the 4x4 blocks
                    // First block number is 0. Block number is either 0 (for U chroma) to 1 (for V Chroma)
                    
                    
                    m_VLDBlockSize = 4;
                    
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
 				// Danger case runs through      . . . . . . . . .         

#else
                    m_VLDBlockType = H264_CHROMA_DC;
                    blockNum = 0;
                    coefInBlk = false;
                    stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;
                    break;

            case H264_VLD_TCOEFF_CHROMA_DC_END_OF_BLOCK: 		// No VLC HW
#endif
            
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
			{
			    AlignedIntMemClr(m_u_dc_data, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));
			    AlignedIntMemClr(m_v_dc_data, NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(short));

                const char * h264_inverse_quantiser_table
                   			 = h264_frame_scan_inverse_quantiser_table[m_VLDQuantChromaMod];
				for(blockNum = 0; blockNum < 2; blockNum++)
                {
                    DecodeCAVLCCoefficient_Choma_DC(control);
                    // Perform inverse run-level scanning for the block
                    short *chromaDC = m_u_dc_data + blockNum*4;
                    if (m_VLDTotalCoefficients > 0)
                    {
                        short level;
                        int total_run = 0;
                        int run_level_pair;
                        
                        
                        // Loop over all non zero levels
                        for (run_level_pair=0; run_level_pair<m_VLDTotalCoefficients; run_level_pair++)
                        {
                            total_run += m_VLDDecodedRunData[run_level_pair];
                            level = m_VLDDecodedLevelData[run_level_pair];
                            
                            //if (total_run > 3) total_run = 3; // This will not normally occur but may in an error environment
                            total_run &= 3;
                            // This inverse quantisation is from the software rather than the document
                                // Ordinary inverse quantisation but done after the inverse two by two transform
                            chromaDC[total_run] = level;
                            
                            total_run++; // Move on to next coefficient
                        }
                        // End of U DC coefficients
                        
                        
                        // Perform inverse 2x2 transform
                        Inverse2x2Transform(chromaDC);
                        int iqvalue = h264_inverse_quantiser_table[0];
                        // Perform inverse quantisation and insert into top left corners of m_u_data
                        short *cData = (short*)(m_u_data+Simdu->GetSDMStartAdd()+(blockNum << 7));
                        if (m_VLDQuantChromaDiv >= 1)
                        {
                            int mul = iqvalue << (m_VLDQuantChromaDiv - 1);
                            cData[ 0] = chromaDC[0] * mul;
                            cData[ 4] = chromaDC[1] * mul;
                            cData[32] = chromaDC[2] * mul;
                            cData[36] = chromaDC[3] * mul;
                        }
                        else
                        {
                            cData[ 0] = (chromaDC[0] * iqvalue) >> 1;
                            cData[ 4] = (chromaDC[1] * iqvalue) >> 1;
                            cData[32] = (chromaDC[2] * iqvalue) >> 1;
                            cData[36] = (chromaDC[3] * iqvalue) >> 1;
                        }
                    }
                    // else no DC coeffs, hence nothing do do as m_u_data is initialised to zero
                        
                }

                if ((control->cbp >> 4) != 2)
                {
                    int block,index;
                    // Chroma AC is not present, so perform the inverse transforms here
                    short *uData = (short*)(m_u_data+Simdu->GetSDMStartAdd());
                    short *vData = (short*)(m_v_data+Simdu->GetSDMStartAdd());
                    for (block=0; block<4; block++)
                    {
                        index = top_left_of_chroma_4x4_block_table[block];
                        if (0 != uData[index])
                            Inverse4x4TransformDCOnly(&uData[index], 8);
                        if (0 != vData[index])
                            Inverse4x4TransformDCOnly(&vData[index], 8);
                    }
                    
       				AlignedIntMemClr(control->number_of_u_coefficients,2 * NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                    
                    if (!VLDEndOfMacroblock(control, stateVLD))
                    	return;
                    	
                	break;
                }
            }
#else   
                {
                    // Perform inverse run-level scanning for the block
                    const char * h264_inverse_quantiser_table
                   			 = h264_frame_scan_inverse_quantiser_table[m_VLDQuantChromaMod];
                    
                    if (m_VLDTotalCoefficients > 0)
                    {
                        short level;
                        int total_run = 0;
                        int run_level_pair;
                        
                        coefInBlk = true;
                        
                        // Loop over all non zero levels
                        for (run_level_pair=0; run_level_pair<m_VLDTotalCoefficients; run_level_pair++)
                        {
                            level = m_VLDDecodedLevelData[run_level_pair];
                            total_run += m_VLDDecodedRunData[run_level_pair];
                            
                            //if (total_run > 3) total_run = 3; // This will not normally occur but may in an error environment
                            total_run &= 3;
                            // This inverse quantisation is from the software rather than the document
                            {
                                // Ordinary inverse quantisation but done after the inverse two by two transform
                                if (0 == blockNum)
                                {
                                    m_u_dc_data[total_run] = level;
                                }
                                else
                                {
                                    m_v_dc_data[total_run] = level;
                                }
                            }
                            
                            total_run++; // Move on to next coefficient
                        }
                    }
                    if (0 == blockNum)
                    {
                        // End of U DC coefficients
                        
                        {
                            if (coefInBlk)
                            {
                                
                                // Perform inverse 2x2 transform
                                Inverse2x2Transform(m_u_dc_data);
                                int iqvalue = h264_inverse_quantiser_table[0];
                                // Perform inverse quantisation and insert into top left corners of m_u_data
                                short *uData = (short*)(m_u_data+Simdu->GetSDMStartAdd());
                                if (m_VLDQuantChromaDiv >= 1)
                                {
                                    int mul = iqvalue << (m_VLDQuantChromaDiv - 1);
                                    uData[ 0] = m_u_dc_data[0] * mul;
                                    uData[ 4] = m_u_dc_data[1] * mul;
                                    uData[32] = m_u_dc_data[2] * mul;
                                    uData[36] = m_u_dc_data[3] * mul;
                                }
                                else
                                {
                                    uData[ 0] = (m_u_dc_data[0] * iqvalue) >> 1;
                                    uData[ 4] = (m_u_dc_data[1] * iqvalue) >> 1;
                                    uData[32] = (m_u_dc_data[2] * iqvalue) >> 1;
                                    uData[36] = (m_u_dc_data[3] * iqvalue) >> 1;
                                }
                            }
                            // else no DC coeffs, hence nothing do do as m_u_data is initialised to zero
                        }
                        
                        blockNum = 1;
                        coefInBlk = false;
                        stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;
                    }
                    else
                    {
                        // End of V DC coefficients hence end of DC chroma
                        if (coefInBlk)
                        {
                            
                            // Perform inverse 2x2 transform
                            Inverse2x2Transform(m_v_dc_data);
                            int iqvalue = h264_inverse_quantiser_table[0];
                            short *vData = (short*)(m_v_data+Simdu->GetSDMStartAdd());
                            if (m_VLDQuantChromaDiv >= 1)
                            {
                                int mul = iqvalue << (m_VLDQuantChromaDiv - 1);
                                vData[ 0] = m_v_dc_data[0] * mul;
                                vData[ 4] = m_v_dc_data[1] * mul;
                                vData[32] = m_v_dc_data[2] * mul;
                                vData[36] = m_v_dc_data[3] * mul;
                            }
                            else
                            {
                                vData[ 0] = (m_v_dc_data[0] * iqvalue) >> 1;
                                vData[ 4] = (m_v_dc_data[1] * iqvalue) >> 1;
                                vData[32] = (m_v_dc_data[2] * iqvalue) >> 1;
                                vData[36] = (m_v_dc_data[3] * iqvalue) >> 1;
                            }
                        }
                        // else no DC coeffs, hence nothing do do as m_v_data is initialised to zero
                        
                        if ((control->cbp >> 4) == 2)
                        {
                            // Chroma AC is present
                            stateVLD = H264_VLD_TCOEFF_CHROMA_AC_START;
                        }
                        else
                        {
                            int block,index;
                            // Chroma AC is not present, so perform the inverse transforms here
                            short *uData = (short*)(m_u_data+Simdu->GetSDMStartAdd());
                            short *vData = (short*)(m_v_data+Simdu->GetSDMStartAdd());
                            for (block=0; block<4; block++)
                            {
                                index = top_left_of_chroma_4x4_block_table[block];
                                if (0 != uData[index])
                                    Inverse4x4TransformDCOnly(&uData[index], 8);
                                if (0 != vData[index])
                                    Inverse4x4TransformDCOnly(&vData[index], 8);
                            }
                            
               				AlignedIntMemClr(control->number_of_u_coefficients,2 * NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS*sizeof(UNSIGNED_SEPTET));
                            
                            if (!VLDEndOfMacroblock(control, stateVLD)) return;
                        }
                    }
                    break;
            }
#endif   
            
            // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CHROMA_AC_START:   
                {
                    // Initialise variables for decoding the AC chroma for the 4x4 blocks
                    // First block number is 0. U blocks are 0..3, and V blocks are 4..7
                    
                    blockNum = 0;
                    coefInBlk = (0 != ((short*)(m_u_data+Simdu->GetSDMStartAdd()))
                    						[top_left_of_chroma_4x4_block_table[0]]);
                    
                    m_VLDBlockSize = 15;
                    
                    // Determine the VLC table to use for decoding coeff token
                    m_VLDCoeffTokenTable = VLDDetermineVLCTableForChromaCoeffToken(control, blockNum);
                    
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
					control->number_of_u_coefficients[blockNum] = DecodeCAVLCCoefficient(control);
	// no break case runs through    . . . . . . .
                }
#else                    
                    m_VLDBlockType = H264_CHROMA_AC;
                    stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;                    
                    break;
                }
                
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CHROMA_AC_END_OF_BLOCK:    		// No VLC HW
#endif
				while(1)
                {
                    short *uData = (short*)(m_u_data+Simdu->GetSDMStartAdd());
                    const char * h264_inverse_quantiser_table = h264_frame_scan_inverse_quantiser_table[m_VLDQuantChromaMod];

                    // Perform inverse run-level scanning for the block
                    if (m_VLDTotalCoefficients > 0)
                    {
                        short level;
                        int total_run = 1;  // DC has already been decoded
                        int run_level_pair;
                        
                         // Loop over all non zero levels
                     	short * dp = uData + top_left_of_chroma_4x4_block_table[blockNum];
                        for (run_level_pair=0; run_level_pair<m_VLDTotalCoefficients; run_level_pair++)
                        {
                            total_run += m_VLDDecodedRunData[run_level_pair];
                            // This will not normally occur but may in an error environment
                            total_run &= 15;
                            level = m_VLDDecodedLevelData[run_level_pair] * h264_inverse_quantiser_table[total_run] << m_VLDQuantChromaDiv;
                            // Ordinary inverse quantisation
                            // Insert coeff into block at correct position and perform inverse quantisation
                            dp[transposed_frame_mb_zig_zag_chroma_table[total_run]] = level;
                            total_run++; // Move on to next coefficient
                        }
                    }
                    
                    // Move to next block
                    ++blockNum;                    
                    
                    if (blockNum > 7)
                    {
                        // apply inverse transform for u & v
                        Simdu->Inverse2DChromaTransform( m_u_data );
                        if (!VLDEndOfMacroblock(control, stateVLD)) 
                        	return;
                        break;
                    }
                    else
                    {
                        
                        // The next state is to decode coeff token
                        // Determine the VLC table to use for decoding coeff token
                        m_VLDCoeffTokenTable = VLDDetermineVLCTableForChromaCoeffToken(control, blockNum);
                        
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                        control->number_of_u_coefficients[blockNum] = DecodeCAVLCCoefficient(control);
#else
                        stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN;
                        break;
#endif
                    }
//                  break;
                }
                break;

#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>  

#else
                // --------------------------------------------------------------
			case H264_VLD_TCOEFF_CAVLC_COEFFICIENT_TOKEN:		// No VLC HW
                {
                    if (H264_CHROMA_DC == m_VLDBlockType)
                    {
                        ByteBufferDecodeCoeffToken(coeff_token_vld_chroma, byteCode);
                        m_VLDTotalCoefficients = coeff_token_value_chroma[byteCode][0];
                        m_VLDTrailingOnes = coeff_token_value_chroma[byteCode][1];
                    }
                    else
                    {
                        switch (m_VLDCoeffTokenTable)
                        {
                        case 0 :    
                            {                        
                                ByteBufferDecodeCoeffToken(coeff_token_vld_table0, byteCode);
                                m_VLDTotalCoefficients = coeff_token_value_table0[byteCode][0];
                                m_VLDTrailingOnes = coeff_token_value_table0[byteCode][1];
                                break;
                            }
                        case 1 :    
                            {                        
                                ByteBufferDecodeCoeffToken(coeff_token_vld_table1, byteCode);
                                m_VLDTotalCoefficients = coeff_token_value_table1[byteCode][0];
                                m_VLDTrailingOnes = coeff_token_value_table1[byteCode][1];
                                break;
                            }
                        case 2 :    
                            {
                                ByteBufferDecodeCoeffToken(coeff_token_vld_table2, byteCode);
                                m_VLDTotalCoefficients = coeff_token_value_table2[byteCode][0];
                                m_VLDTrailingOnes = coeff_token_value_table2[byteCode][1];
                                break;
                            }
                        case 3 :    
                            {
                                ByteBufferDecodeFixedLengthCode(6, byteCode);
                                if (3 == byteCode)
                                {
                                    m_VLDTotalCoefficients = 0;
                                    m_VLDTrailingOnes = 0;
                                }
                                else
                                {
                                    m_VLDTotalCoefficients = 1 + (byteCode >> 2);
                                    m_VLDTrailingOnes = byteCode & 3;
                                }
                                break;
                            }
                        }
                    }
                    
                    m_VLDOtherCoefficients = m_VLDTotalCoefficients - m_VLDTrailingOnes;
                    m_VLDIndexIntoLevelArray = m_VLDTotalCoefficients;  // It will be decremented before being used as an index
                    
                    m_FirstCoefficient = true;
                    
                    if (H264_LUMA_AC == m_VLDBlockType)
                    {
                        control->number_of_y_coefficients[h264_4x4_transmission_to_raster_table[blockNum]] = (UNSIGNED_SEPTET)(m_VLDTotalCoefficients);
                    }
                    
                    else if (H264_CHROMA_AC == m_VLDBlockType)
                    {
                        if (blockNum < 4)
                        {
                            control->number_of_u_coefficients[blockNum] = (UNSIGNED_SEPTET)(m_VLDTotalCoefficients);
                        }
                        else
                        {
                            control->number_of_v_coefficients[blockNum-4] = (UNSIGNED_SEPTET)(m_VLDTotalCoefficients);
                        }
                    }
                    
                    if ((m_VLDTotalCoefficients > 10) && (m_VLDTrailingOnes < 3))
                    {
                        m_VLDLevelTable = 1;
                    }
                    else
                    {
                        m_VLDLevelTable = 0;
                    }
                    
                    if (m_VLDTrailingOnes > 0)
                    {
                        stateVLD = H264_VLD_TCOEFF_CAVLC_TRAILING_ONES_SIGN;
                    }
                    else if (m_VLDTotalCoefficients > 0)
                    {
                        stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_LEVEL;
                    }
                    else
                    {
                        // No non-zero cofficients
                        stateVLD = NextStateTab[m_VLDBlockType];
                    }
                    break;
            }
            // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CAVLC_TRAILING_ONES_SIGN:  		// No VLC HW
                {
                    ByteBufferDecodeFixedLengthCode(m_VLDTrailingOnes, byteCode);
                    
                    int i;
                    unsigned int mask = 1 << m_VLDTrailingOnes;
                    
                    for (i=0; i<m_VLDTrailingOnes; i++)
                    {
                        m_VLDIndexIntoLevelArray--;
                        mask >>= 1;
                        if (0 == (byteCode & mask))
                        {
                            m_VLDDecodedLevelData[m_VLDIndexIntoLevelArray] = 1;
                        }
                        else
                        {
                            m_VLDDecodedLevelData[m_VLDIndexIntoLevelArray] = -1;
                        }
                    }
                    
                    if (m_VLDOtherCoefficients > 0)
                    {
                        stateVLD = H264_VLD_TCOEFF_CAVLC_COEFFICIENT_LEVEL;
                    }
                    else
                    {
                        // Note total zeros must be present as the maximum number of trailing ones is three
                        stateVLD = H264_VLD_TCOEFF_CAVLC_TOTAL_ZEROS;
                    }
                    break;
                }
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CAVLC_COEFFICIENT_LEVEL:   		// No VLC HW
                {
                    while(true)
                    {
                        ByteBufferDecodeLevelVLC(m_VLDLevelTable, byteCode);
                        int byteBufferCodeNumber = byteCode;
                        int level = (byteBufferCodeNumber + 2) >> 1;
                        
                        if ((m_FirstCoefficient) && (m_VLDTrailingOnes < 3))
                        {
                            level++;
                        }
                        
                        if ((byteBufferCodeNumber & 1) != 0)
                        {
                            m_VLDDecodedLevelData[--m_VLDIndexIntoLevelArray] = -level;
                        }
                        else
                        {
                            m_VLDDecodedLevelData[--m_VLDIndexIntoLevelArray] = level;
                        }
                        if (--m_VLDOtherCoefficients > 0)
                        {
                            // Calculate the VLC table to use for the next level
                            if ((level > level_table_threshold[m_VLDLevelTable]) && (m_VLDLevelTable < 6))
                            {
                                m_VLDLevelTable++;
                            }
                            if ((m_FirstCoefficient) && (level > 3))
                            {
                                m_VLDLevelTable = 2;
                            }
                            m_FirstCoefficient = false;
                        }
                        else if (m_VLDTotalCoefficients < m_VLDBlockSize)
                        {
                            stateVLD = H264_VLD_TCOEFF_CAVLC_TOTAL_ZEROS;
                            break;
                        }
                        else
                        {
                            // This occurs when all coefficients are present
                            // Set all runs to be zero
                            AlignedIntMemClr(m_VLDDecodedRunData, 16*sizeof(UNSIGNED_SEPTET));
                            stateVLD = NextStateTab[m_VLDBlockType];
                            break; // exit the loop
                        } // end of if (OtherCoeffCnt > 0)
                    } // end of while-loop
                    break;
                }
                
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CAVLC_TOTAL_ZEROS: 		// No VLC HW
                {
                    if (H264_CHROMA_DC == m_VLDBlockType)
                    {
                        ByteBufferDecodeTotalZerosDCVLC(byteCode);
                    }
                    else
                    {
                        assert(m_VLDTotalCoefficients > 0);
                        assert(m_VLDTotalCoefficients < 16);
                        ByteBufferDecodeTotalZerosVLC((const UNSIGNED_SEPTET*)total_zeros_coeff_table_ptrs[m_VLDTotalCoefficients],
                            total_zeros_coeff_table_code_length[m_VLDTotalCoefficients], byteCode);
                    }
                    // All coefficients are decoded, so reset level index for associating zeros with levels
                    m_VLDIndexIntoLevelArray = m_VLDTotalCoefficients;  // It will be decremented before being used as an index
                    
                    m_VLDZerosLeft = byteCode;
                    
                    if ((m_VLDTotalCoefficients > 1) && (m_VLDZerosLeft > 0))
                    {
                        stateVLD = H264_VLD_TCOEFF_CAVLC_RUN_BEFORE;
                    }
                    else
                    {
                        if (m_VLDZerosLeft > 0)
                        {
                            // m_VLDIndexIntoLevelArray must be 1, meaning we are about to index position 0
                            // Only one coefficient left, so m_VLDZerosLeft is its associated run
                            m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)(m_VLDZerosLeft);
                        }
                        else
                        {
                            // There are no zeros left, and m_VLDIndexIntoLevelArray levels left
                            // Set the corresponding run data to zero
                            memset(m_VLDDecodedRunData,0, m_VLDIndexIntoLevelArray*sizeof(UNSIGNED_SEPTET));
                        }
                        
                        stateVLD = NextStateTab[m_VLDBlockType];
                    }
                    
                    break;
                }
                
                // --------------------------------------------------------------
            case H264_VLD_TCOEFF_CAVLC_RUN_BEFORE:  		// No VLC HW
                {
                    while(true)
                    {
                        if (m_VLDZerosLeft > 6)
                        {
                            ByteBufferDecodeRunBeforeTable7VLC(byteCode);
                        }
                        else
                        {
                            ByteBufferDecodeRunBeforeVLC(byteCode);
                        }
                        m_VLDIndexIntoLevelArray--;
                        m_VLDDecodedRunData[m_VLDIndexIntoLevelArray] = (UNSIGNED_SEPTET)(byteCode);
                        
                        m_VLDZerosLeft -= byteCode;
                        
                        if ((m_VLDIndexIntoLevelArray > 1) && (m_VLDZerosLeft > 0))
                        {
                            // No change of state
                        }
                        else
                        {
                            if (m_VLDZerosLeft > 0)
                            {
                                // m_VLDIndexIntoLevelArray must be 1, meaning we are about to index position 0
                                // Only one coefficient left, so m_VLDZerosLeft is its associated run
                                m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)(m_VLDZerosLeft);
                            }
                            else
                            {
                                // There are no zeros left, and m_VLDIndexIntoLevelArray levels left
                                // Set the corresponding run data to zero
                                memset(m_VLDDecodedRunData,0, m_VLDIndexIntoLevelArray*sizeof(UNSIGNED_SEPTET));
                            }
                            
                            stateVLD = NextStateTab[m_VLDBlockType];
                            break; // exit the loop
                        } // end of if ((m_VLDIndexIntoLevelArray > 1) && (m_VLDZerosLeft > 0))
                    } // end of while-loop
                    
                    
                    break;
                }
#endif // defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>

              
                // --------------------------------------------------------------
            case H264_VLD_I_PCM_BYTE_ALIGNMENT: 
                {
                    ByteBufferByteAlign(); 
                    
                    control->Flags |= CTLF_IsIntraCoded;
                    control->quant = 0;
                    AlignedIntMemSet(control->number_of_y_coefficients, 16, 
                        (NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS + 2* NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS) *sizeof(UNSIGNED_SEPTET));
                    
                    stateCount = 0;
                }
                
//            case H264_VLD_I_PCM_Y_DATA: 
            	while(1)
                {
                    short *yData = (short*)(m_y_data+Simdu->GetSDMStartAdd());
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                    yData[stateCount] = VLC_Get_Bits(8);
#else
                    ByteBufferDecodeFixedLengthCode(8, byteCode);
                    yData[stateCount] = byteCode;
#endif
                    
                    if (NUMBER_OF_Y_PELS_PER_BLOCK == ++stateCount)
                    {
//                        stateVLD = H264_VLD_I_PCM_U_DATA;
                        stateCount = 0;
                    	break;
                    }
                }
                
// Case runs through .. ... . . . .. . . . .. .. . . .. 
//            case H264_VLD_I_PCM_U_DATA:
				while(1)
                {
                    short *uData = (short*)(m_u_data+Simdu->GetSDMStartAdd());
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                    uData[stateCount] = VLC_Get_Bits(8);
#else
                    ByteBufferDecodeFixedLengthCode(8, byteCode);
                    uData[stateCount] = byteCode;
#endif
                    if (NUMBER_OF_U_PELS_PER_BLOCK == ++stateCount)
                    {
//                        stateVLD = H264_VLD_I_PCM_V_DATA;
                        stateCount = 0;
                    break;
                    }
                }
                
//     Case runs through . . . . ... . . . .. . ... .. . . . . . .
//            case H264_VLD_I_PCM_V_DATA: 
				while(1)
                {
                    short *vData = (short*)(m_v_data+Simdu->GetSDMStartAdd());
#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
                    vData[stateCount] = VLC_Get_Bits(8);
#else
                    ByteBufferDecodeFixedLengthCode(8, byteCode);
                    vData[stateCount] = byteCode;
#endif
                    if (NUMBER_OF_V_PELS_PER_BLOCK == ++stateCount)
                    {
                        if (!VLDEndOfMacroblock(control, stateVLD)) 
                        	return;
                        
                    	break;
                    }
                }
                break;
                
                
                // --------------------------------------------------------------
            default:
            	return;
                // --------------------------------------------------------------
        }

    }
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDetermineVLCTableForLumaCoeffToken			*/
/* DESCRIPTION:	This determines the VLC table that is to be used for 
decoding the next luma coeff token. The parameter is the 
block number in decoding order, and is 0 for intra DC. 
The VLC table depends on the number of non zero 
coefficients in the above and left 4x4 blocks if they are 
in the same slice as the current block. 
Intra DC is treated as if it were the top left AC 
4x4 block.
The function sets the value of m_VLDCoeffTokenTable.		*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

static char h264_coeff_tokens[] = {0,0,1,1,2,2,2,2,3 ,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};

#ifdef CAVLC_HW_PRESENT

// Block to the left of current block when working in stream order
const UNSIGNED_SEPTET scan_left_of_table[16] =
{
	5,	0,	7,	2,
	1,	4,	3,	6,
	13,	8,	15,	10,
	9,	12,	11,	14
};

// Block above the current block when working in stream order
const UNSIGNED_SEPTET scan_above_of_table[16] =
{
	10,	11,	0,	1,
	14, 15, 4,	5,
	2,	3,	8,	9,
	6,	7,	12,	13
};

//{
//	 0,  1,  4,  5,
//	 2,  3,  6,  7,
//	 8,  9, 12, 13,
//	10, 11, 14, 15
//};

// Version working in stream order
inline int CH264Decoder::VLDDetermineVLCTableForLumaCoeffToken(h264_decoder_control_t * control, int bn) const
{
    int n;

	char mbFlags = control->Flags;
    if( (bn & 10) != 0 )
    {
        // Above block is in current macroblock
        n = control->number_of_y_coefficients[scan_above_of_table[bn]];
        if (0 == (bn & 5))
        {
            // Left block is in left macroblock
            if (mbFlags & CTLF_LeftInSlice)
            {
                int nl = control->GetLeft()->number_of_y_coefficients[bn + 5];
      			n = (n + nl + 1) >> 1;
            }
        }
        else
        {
            // Left block is in current macroblock
            int nl = control->number_of_y_coefficients[scan_left_of_table[bn]];
            n = (n + nl + 1) >> 1;
        }
    }
    else
    {	// Top line of MB
        if (bn )
        {
            // Above block is in above macroblock               
            if (mbFlags & CTLF_AboveInSlice)
            {
                n = control->GetAbove()->number_of_y_coefficients[bn + 10];
                int nl = control->number_of_y_coefficients[scan_left_of_table[bn]];
                n = (n + nl + 1) >> 1;                
            }
            else
            {
         	   n = control->number_of_y_coefficients[scan_left_of_table[bn]];
            }
        }
        else
        {
            // Above block is in above macroblock               
            if( (mbFlags & CTLF_AboveInSlice + CTLF_LeftInSlice) == CTLF_AboveInSlice + CTLF_LeftInSlice)
            {
                n = control->GetAbove()->number_of_y_coefficients[bn + 10];
                int nl = control->GetLeft()->number_of_y_coefficients[bn + 5];
                n = (n + nl + 1) >> 1;
            }
            // Left block is in left macroblock
            else if (mbFlags & CTLF_LeftInSlice)
            {
                n = control->GetLeft()->number_of_y_coefficients[bn + 5];
            }
            else if(mbFlags & CTLF_AboveInSlice)
            {
            	n = control->GetAbove()->number_of_y_coefficients[bn + 10];
            }
            else
            {
                n = 0;
            }
        }
    }
    
    if(n > 7)
    	return 3;
    return (int)h264_coeff_tokens[n];
}

#else	// CAVLC_HW_PRESENT

// No hardware support version
// Working in raster scan coords
inline int CH264Decoder::VLDDetermineVLCTableForLumaCoeffToken(h264_decoder_control_t * control, int blockNum) const
{
    // The following were defined as char but this caused problems on symbian where 
    // the initialisation to -1 caused initialisation to 255.
    int n = -1;  // The value produced by combining nu and nl
    int raster_block_number = h264_4x4_transmission_to_raster_table[blockNum];
	char mbFlags = control->Flags;
    
    if (raster_block_number < 4)
    {
        // Above block is in above macroblock
        if (mbFlags & CTLF_AboveInSlice)
        {
            n = control->GetAbove()->number_of_y_coefficients[raster_block_number + 12];
        }
    }
    else
    {
        // Above block is in current macroblock
        n = control->number_of_y_coefficients[raster_block_number-4];
    }
    
    if (0 == (raster_block_number & 3))
    {
        // Left block is in left macroblock
        
        if (mbFlags & CTLF_LeftInSlice)
        {
            int nl = control->GetLeft()->number_of_y_coefficients[raster_block_number + 3];
            if (n < 0)
                n = nl;
            else
                n = (n + nl + 1) >> 1;
        }
        else if (n < 0)
            n = 0;
    }
    else
    {
        // Left block is in current macroblock
        int nl = control->number_of_y_coefficients[raster_block_number-1];
        if (n < 0)
            n = nl;
        else
            n = (n + nl + 1) >> 1;
    }
//    printf("blockNum %d \t raster_block_number %d\t n %d\n", blockNum,raster_block_number, n);
    return (int)h264_coeff_tokens[MIN(n,8)];
}

#endif	// CAVLC_HW_PRESENT


inline int CH264Decoder::VLDDetermineVLCTableForLumaCoeffToken0(h264_decoder_control_t * control) const
{
    // The following were defined as char but this caused problems on symbian where 
    // the initialisation to -1 caused initialisation to 255.
    char mbFlags = control->Flags;

	if (mbFlags & CTLF_AboveInSlice)
	{
		if (mbFlags & CTLF_LeftInSlice)
		{
#ifndef CAVLC_HW_PRESENT
	        return (int)h264_coeff_tokens[ (control->GetAbove()->number_of_y_coefficients[12] +
	        	 control->GetLeft()->number_of_y_coefficients[3] +
	        	 1 ) >> 1];
#else
	        return (int)h264_coeff_tokens[ (control->GetAbove()->number_of_y_coefficients[10] +
	        	 control->GetLeft()->number_of_y_coefficients[5] +
	        	 1 ) >> 1];
#endif
		}
#ifndef CAVLC_HW_PRESENT
        return (int)h264_coeff_tokens[control->GetAbove()->number_of_y_coefficients[12]];
#else
        return (int)h264_coeff_tokens[control->GetAbove()->number_of_y_coefficients[10]];
#endif
	}
	else
	{
		if (mbFlags & CTLF_LeftInSlice)
		{
#ifndef CAVLC_HW_PRESENT
	        return (int)h264_coeff_tokens[ control->GetLeft()->number_of_y_coefficients[3] ];
#else
	        return (int)h264_coeff_tokens[ control->GetLeft()->number_of_y_coefficients[5] ];
#endif
		}
		return 0;
	}
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDDetermineVLCTableForChromaCoeffToken		*/
/* DESCRIPTION:	This determines the VLC table that is to be used for 
decoding the next chroma coeff token. The parameter is the
block number in decoding order, 0..3 for u, and 4..7 for v.
The VLC table depends on the number of non zero 
coefficients in the above and left 4x4 blocks if they are 
in the same slice as the current block. 
The function sets the value of m_VLDCoeffTokenTable.		*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

inline int CH264Decoder::VLDDetermineVLCTableForChromaCoeffToken(h264_decoder_control_t * control, int bn) const
{
    // Determine the number of coefficients in the above and left 4x4 blocks and use -1 to 
    // indicate block is not available
    // The following were defined as char but this caused problems on symbian where 
    // the initialisation to -1 caused initialisation to 255.
    int n = -1;  // The value produced by combining nu and nl
    int nl = -1;
        
    if ((bn & 3) < 2)
    {
        // Above block is in above macroblock
        
         // Field or Non-MB-AFF Frame        
        if (control->Flags & CTLF_AboveInSlice)
        {
			n = control->GetAbove()->number_of_u_coefficients[bn + 2];
        }
    }
    else
    {
        // Above block is in current macroblock
		n = control->number_of_u_coefficients[bn-2];
    }
    
    if (0 == (bn & 1))
    {
        // Left block is in left macroblock
        
        // Field or Non-MB-AFF Frame
        
        if (control->Flags & CTLF_LeftInSlice)
        {
            nl = control->GetLeft()->number_of_u_coefficients[bn + 1];
            if (n < 0)
                n = nl;
            else
                n = (n + nl + 1) >> 1;
        }
        else if (n < 0)
            n = 0;
    }
    else
    {
        // Left block is in current macroblock
        nl = control->number_of_u_coefficients[bn-1];
        if (n < 0)
            n = nl;
        else
            n = (n + nl + 1) >> 1;
    }
    if(n > 7)
    	return 3;
    return (int)h264_coeff_tokens[n];
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDEndOfMacroblock							*/
/* DESCRIPTION:	This is called at the end of parsing a macroblock.
By this time, the inverse transform will have been 
performed on the residual data received, but no prediction 
has been added.
The prediction is added to reconstruct the macroblock.
The control pointer is advanced, and a check for end of 
picture is made.
The control to reconstruct is passed as a parameter
The bool set_next says whether the function should move to 
the next control in the control list						*/
/* RETURNS:		True if success Else False													*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::VLDEndOfMacroblock(h264_decoder_control_t * control , h264_vld_state_t &stateVLD)
{    

#if defined(CAVLC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
    if (VLC_Get_Dec_Error() > 0)
    {
//    	printf("VLC error %08x\n",Q_DEBUG);
    	return false;
    }
#endif
    
    // Mark the macroblock as having been decoded and reconstructed
    control->coded_block_flags = true;
    control->disable_deblocking_filter_idc = m_DisableDeblockingFilterIDC;
    control->slice_alpha_c0_offset = m_SliceAlphaC0Offset;
    control->slice_beta_offset = m_SliceBetaOffset;
 

    m_NumberOfMacroblocksDecoded++;

    switch(control->mtype)
    {
    	case H264_SKIPPED:
	        // Set cbp to zero - needed for CABAC decoding - not set otherwise for skipped case
	        control->cbp = 0;
	        CalculateSkippedMotionVectors(control);
	        
//>>> case runs through <<<<    	
        case H264_INTER_16X16:
        case H264_INTER_16X8:
        case H264_INTER_8X16:
            SIMDInterRecon(control);
            break;
        
        case H264_INTER_8X8_MULTI_REF:
        case H264_INTER_8X8_ZERO_REF:
            SIMDInterRecon8x8(control);
            break;
            
        case H264_INTRA_4X4:
            SIMDIntraRecon4x4(control);
            break;
            
        case H264_INTRA_16X16:
            SIMDIntraRecon16x16(control);
            break;
            
        case H264_I_PCM:
            SIMDIntraReconPCM(control);
            break;
            
        default :
            return false;
    }
    
//#ifdef INLOOP_DBLOCK
    if(m_InloopDeblock)
    {
	    //strore bottom row of luma pixels before deblocking
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_Y + control->PixelX + 12, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_left_intra, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
	    
	    
	    _vr00 = _vmovw(m_sdm_buf_ofs + SDM_ofs_y_out_buf + 16*15, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_Y + control->PixelX, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE16);
	    //  strore bottom row of U component pixels before deblocking
	    int pic_x = control->PixelX >> 1;
	    
	    //  store the top left chroma pixel before overwriting top row pixels. This pixel is required 
	    // for intra prediction
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_U + pic_x + 4, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_left_intra + 4, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
	    
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_V + pic_x + 4, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_left_intra + 8, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE4);
	    
	    _vr00 = _vmovw(m_sdm_buf_ofs + SDM_ofs_u_out_buf + 8*7, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_U + pic_x, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE8);
	    
	    _vr00 = _vmovw(m_sdm_buf_ofs + SDM_ofs_v_out_buf + 8*7, 0x1);       // %i0
	    _vr00 = _vmovw(AuroraH264Dec::m_top_intra_V + pic_x, 0x2);       // %i1
	    Simdu->RecMgr.Run(AM_COPYTOPLINE8);
	    
	    Simdu->SetupDeblockDMA(m_VLDCurrentPicture->m_y, m_VLDCurrentPicture->m_u, m_VLDCurrentPicture->m_v, m_VLDCurrentPicture->m_hsize, m_VLDCurrentPicture->m_vsize);
	    Simdu->copy_top_left_Deblock_data(control->PixelX, control->PixelY);
	    DeblockMB(m_VLDCurrentPicture, control);
	    {
	    	bool dma_macroblock = ((m_NumberOfMacroblocksDecoded -1) > 0 && control->PixelY > 0);
	    	Simdu->CopyAndDMA_outDeblock(control->PixelX, control->PixelY, dma_macroblock );
	    	Simdu->WaitForDMAOut();
	    }
    }    
//#endif
     
    
    //get new macroblock buffer
    {
        Simdu->SDMMemClear(m_prev_sdm_buf_ofs);	// Now frees buffer
        Simdu->SDM_Buffers.FreeSDMBuffer( m_prev_sdm_buf_ofs);
        m_prev_sdm_buf_ofs = m_sdm_buf_ofs;
        
        //allocate new buffers
        m_sdm_buf_ofs = Simdu->SDM_Buffers.GetNextSDMBuffer();
#ifdef _DEBUG
        if(m_sdm_buf_index < 0)
        {
            printf("!!!Error in getting New SDM buffer!!!\n");
            fflush(stdout);
            m_simd_error = H264_DECODER_SIMD_BUFFER_ERROR;
            return false;
        }
#endif
        m_y_data = m_sdm_buf_ofs;
        m_u_data = m_y_data + SDM_ofs_u_data;
        m_v_data = m_y_data + SDM_ofs_v_data;
        
    }
    
    
    
   // Set m_PreviousMB
    control = control->next;
    while (control)
    {        
        if (m_VLDCurrentSliceGroup == control->slice_group)
        	break;
    	control = control->next;
    }
    m_DecoderControl.m_CurrentElement = control;
    
    if (m_NumberOfMacroblocksDecoded == m_NumberOfMacroblocksInPicture)
    {
//#ifdef INLOOP_DBLOCK
    	if(m_InloopDeblock)
    	{
    		Simdu->DMA_out_Last_MB_Row(m_DecoderControl.m_MacroblocksPerRow,m_DecoderControl.m_vsize - 16);
    		Simdu->WaitForDMAOut();
    	}
//#endif    	
//        m_VLDFinishedDecodingPicture = true;
        VLDEndOfPicture(0 != m_NALReferenceIDC);
        
        // Discard any byte alignment bits at the end of the picture
        ByteBufferByteAlign();
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
        m_ByteBufferZeroByteCount = 0;
#endif            
        stateVLD = H264_VLD_LOST;
        return false;			// Out of while loop

    }
    
    if (0 == control)
    {
        // This is the end of a slice group
        
        // Force search for next start code
        stateVLD = H264_VLD_LOST;
        return false;
    }
    

    if (H264_I_SLICE == m_SliceType)
    {
        stateVLD = H264_VLD_MB_TYPE;
    }
    else
    {
        stateVLD = H264_VLD_RUN;
    }
    
    return true;
    
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDEndOfPicture								*/
/* DESCRIPTION:	This is called at the end of parsing a picture.
Memory is associated with the picture in the decoded 
picture buffer and it is copied from the macroblock 
structure into the decoded picture buffer. The picture is 
then loop filtered.
If the picture is stored for reference and the bitstream 
may contain B slices, the motion vectors of the picture are 
stored.
stored_for_reference indicates whether the picture is to 
be stored in the decoded picture buffer as a reference
picture.													*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::VLDEndOfPicture(bool stored_for_reference)
{
    int decoded_picture_array_position;
    bool mmco_reset;
    
//    AuroraH264Dec::WaitForSIMDFree();
    
    // Loop filter the picture
    
    // Non-MB-AFF frame
    //inloop db: Abhijit
//#ifndef INLOOP_DBLOCK
//#if 0
    if(!m_InloopDeblock)
    	LoopFilterNonMBAFFFrame(m_VLDCurrentPicture);
//#endif    
//#endif
    
#ifdef FRAME_TIMES
    SetATime();
#endif

#if defined __BENCHMARK__
#if defined __BENCHMARK_WITHOUT_HOSTLIB__
    // *benchmark += Get_Timer0();
    // benchmark ++;
    // *benchmark = 0;
    // Reset_Timer0();
#else
    stop_BPR();

    /* extract benchmark data */
    if ( enable_benchmark == 1 ) { 
      capture_BPR_benchmark_data();
    }
    reset_BPR_Counters();
    run_BPR();
#endif
#endif

    m_VLDCurrentPicture->m_ts = m_CurrentTimestamp;
    
    m_OutputPictureNumber = m_PictureOrderCount + m_POCAccumulator;
   
    m_VLDCurrentPicture->m_fn = m_OutputPictureNumber;
    
    decoded_picture_array_position
        = m_DecodedPictureBuffer->StorePicture(stored_for_reference, &mmco_reset);
    
    
    m_VLDCurrentPicture = 0;  // This is not needed, but may help with bug finding
    
    if (mmco_reset)
    {
        m_POCAccumulator += m_PictureOrderCount;		// Accumulation for IDR picture type is done in VLDDecodeSliceHeader
        m_PictureOrderCount = 0;
        m_FrameNumber = 0;
        
        m_VLDSliceHeaderPreviousReferenceSlicePOC = 0;
        m_VLDSliceHeaderPreviousFrameNumberOffset = 0;
        m_VLDSliceHeaderPreviousSliceFrameNumber = 0;
        m_VLDSliceHeaderPreviousSlicePictureOrderCount = 0;
    }
    
    // Reset count of number of macroblocks decoded
    m_NumberOfMacroblocksDecoded = 0;
    
    // Now point decoded_picture to the picture that is to be output
//    if (m_VLDFinishedDecodingPicture)
//    {        
        VLDOutputPicture();        
//    }
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::VLDOutputPicture								*/
/* DESCRIPTION:	This is called to try to output a decoded picture from the 
decoded picture buffer.										*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::VLDOutputPicture(void)
{
    int array_position;  // For the output picture
    
    if (!m_BumpingDecoder)
    {
        // Original mode of operation
        if (m_RemainingBumpingDelay > 0)
        {
            // Still in the initial period of decoding but not outputting pictures
            m_RemainingBumpingDelay--;
        }
        else
        {
            CBTPicture * decoded_picture;
            
            array_position = m_DecodedPictureBuffer->OutputFrame(&decoded_picture);
            
            if (0 != decoded_picture)
            {
                if (m_SequenceParameterSet->frame_cropping_flag)
                {
					decoded_picture->SetCroppingInformation(
						m_SequenceParameterSet->frame_cropping_rect_left_offset,
						m_SequenceParameterSet->frame_cropping_rect_right_offset,
						m_SequenceParameterSet->frame_cropping_rect_top_offset,
						m_SequenceParameterSet->frame_cropping_rect_bottom_offset);
					/*
					m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].crop(decoded_picture,
						m_SequenceParameterSet->frame_cropping_rect_left_offset,
						m_SequenceParameterSet->frame_cropping_rect_right_offset,
						m_SequenceParameterSet->frame_cropping_rect_top_offset,
						m_SequenceParameterSet->frame_cropping_rect_bottom_offset);

					m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].m_ts = decoded_picture->m_ts;
					m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame].m_fn = decoded_picture->m_fn;

					SetReturnParameters(&m_CroppedOutputPicture[m_H264DecoderParameters.bDisplayFrame]);
					*/
					SetReturnParameters(decoded_picture);
                }
                else
                {
                    SetReturnParameters(decoded_picture);
                }
            }
        }
    }
    else
    {
        // Bumping mode of operation
        CBTPicture * decoded_picture;
        
        do
        {
            array_position = m_DecodedPictureBuffer->OutputFrameBumping(&decoded_picture);
            
            if (0 != decoded_picture)
            {
                if (m_SequenceParameterSet->frame_cropping_flag)
                {
					decoded_picture->SetCroppingInformation(
						m_SequenceParameterSet->frame_cropping_rect_left_offset,
						m_SequenceParameterSet->frame_cropping_rect_right_offset,
						m_SequenceParameterSet->frame_cropping_rect_top_offset,
						m_SequenceParameterSet->frame_cropping_rect_bottom_offset);

					SetReturnParameters(decoded_picture);
                }
                else
                {
                    SetReturnParameters(decoded_picture);
                }
            }
        } while (0 != decoded_picture);
    }
}






#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>

#else
inline bool CH264Decoder::DecodeCAVLCList0RefIndex(int & byteCode)
{
    if ((m_SliceNumList0RefPictures <= 1) || 
        (H264_INTER_8X8_ZERO_REF == m_DecoderControl.m_CurrentElement->mtype)) 
    {
        // Nothing to do if m_SliceNumList0RefPictures < 2 or 8x8 zero ref
        byteCode = 0;
    }
    else if (2 == m_SliceNumList0RefPictures)
    {
        ByteBufferDecodeFixedLengthCode(1, byteCode);
        // Invert the codeword: a one bit is codeword 0
        byteCode = 1 - byteCode;
    }
    else // (m_SliceNumList0RefPictures > 2)
    {
        if (!ByteBufferDecodeUniversalVLC(byteCode)) return false;
    }
    return true;
}
#endif




#if defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>

inline UNSIGNED_SEPTET CH264Decoder::DecodeCAVLCCoefficient(h264_decoder_control_t * control)
{
    // decode coefficient token in word buffer
    CAVLC_Set_Control( (m_VLDCoeffTokenTable << 8) |((CAVLC_OTHER_BLOCK << 4) | CAVLC_COEFF_TOKEN));
    
    int coeffToken = CAVLC_Get_Symbol();
        
    int tCoef = m_VLDTotalCoefficients = coeffToken >> 2;

    if(tCoef)
	{
		int count = coeffToken & 0x3;	// trailing Ones Cnt
	    int ila = tCoef;  // It will be decremented before being used as an index
	    
	    if (count > 0)	// trailing Ones Cnt
	    {
		    int trailingOneSigns = VLC_Get_Bits(count)<< 1;
			for(int i = count-1; i >=0; i--)
			{
				m_VLDDecodedLevelData[--ila] = 1 - ((trailingOneSigns >> i) & 2);
			}
	    }
	
    	count = tCoef - count;	// Other coef

		if(count > 0)
		{
		    CAVLC_Set_Control(CAVLC_COEFF_LEVEL);
		    
		    while (--count > 0)
		    {
		        m_VLDDecodedLevelData[--ila] = CAVLC_Get_Symbol();
		    }
		    
		    m_VLDDecodedLevelData[--ila] = CAVLC_Get_Symbol();
    	}
	    
	    if (tCoef < m_VLDBlockSize)
	    {
			CAVLC_Set_Control( (CAVLC_LUMA4X4 << 4) | CAVLC_TOTAL_ZERO);

		    count = CAVLC_Get_Symbol();	// Total zeros
		    
		    if ((tCoef > 1) && (count > 0))
		    {
		        // decode number of zeros (run) before each non-zero coefficient
		        CAVLC_Set_Control(CAVLC_RUN_BEFORE);
		        for(int i=tCoef-1; i>0; i--)
		        {
		            m_VLDDecodedRunData[i] = (UNSIGNED_SEPTET)(CAVLC_Get_Symbol());
		        }

		        m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)CAVLC_Get_Zerosleft();		        
		    }
		    else
		    {
		        if (count > 0)
		        {
		            // m_VLDTotalCoefficients must be 1, meaning we are about to index position 0
		            // Only one coefficient left, so m_VLDZerosLeft is its associated run
		            m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)(count);
		        }
		        else
		        {
		            // There are no zeros left
		            // Set the corresponding run data to zero
		            for(int i = 0; i < tCoef; i++)
		            {
		            	m_VLDDecodedRunData[i] = 0;
		            }
		        }
		    }
	    }
	    else
	    {
	        // This occurs when all coefficients are present
	        // Set all runs to be zero
	        AlignedIntMemClr(m_VLDDecodedRunData, 16*sizeof(UNSIGNED_SEPTET));
	    }
	}
	return tCoef;
}



inline UNSIGNED_SEPTET CH264Decoder::DecodeCAVLCCoefficient_Choma_DC(h264_decoder_control_t * control)
{
    // decode coefficient token in word buffer
	CAVLC_Set_Control( (m_VLDCoeffTokenTable << 8) |((CAVLC_CHROMA_DC << 4) | CAVLC_COEFF_TOKEN));
   	
    int coeffToken = CAVLC_Get_Symbol();
        
    int tCoef = m_VLDTotalCoefficients = coeffToken >> 2;

    if(tCoef)
	{
		int count = coeffToken & 0x3;	// trailing Ones Cnt
	    int ila = tCoef;  // It will be decremented before being used as an index
	    
	    if (count > 0)	// trailing Ones Cnt
	    {
		    int trailingOneSigns = VLC_Get_Bits(count)<< 1;
			for(int i = count-1; i >=0; i--)
			{
				m_VLDDecodedLevelData[--ila] = 1 - ((trailingOneSigns >> i) & 2);
			}
	    }
	
    	count = tCoef - count;	// Other coef

		if(count > 0)
		{
		    CAVLC_Set_Control(CAVLC_COEFF_LEVEL);
		    
		    while (--count > 0)
		    {
		        m_VLDDecodedLevelData[--ila] = CAVLC_Get_Symbol();
		    }
		    
		    m_VLDDecodedLevelData[--ila] = CAVLC_Get_Symbol();
    	}
	    
	    if (tCoef < m_VLDBlockSize)
	    {
	        //        blockType = CAVLC_CHROMA_DC;
		    // decode total number of zeros before the last non-zero coefficient in word buffer
		    CAVLC_Set_Control( (CAVLC_CHROMA_DC2X2 << 4) | CAVLC_TOTAL_ZERO);

		    count = CAVLC_Get_Symbol();	// Total zeros
		    
		    if ((tCoef > 1) && (count > 0))
		    {
		        // decode number of zeros (run) before each non-zero coefficient
		        CAVLC_Set_Control(CAVLC_RUN_BEFORE);
		        for(int i=tCoef-1; i>0; i--)
		        {
		            m_VLDDecodedRunData[i] = (UNSIGNED_SEPTET)(CAVLC_Get_Symbol());
		        }
		        
		        m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)CAVLC_Get_Zerosleft();
		    }
		    else
		    {
		        if (count > 0)
		        {
		            // m_VLDTotalCoefficients must be 1, meaning we are about to index position 0
		            // Only one coefficient left, so m_VLDZerosLeft is its associated run
		            m_VLDDecodedRunData[0] = (UNSIGNED_SEPTET)(count);
		        }
		        else
		        {
		            // There are no zeros left
		            // Set the corresponding run data to zero
		            for(int i = 0; i < tCoef; i++)
		            {
		            	m_VLDDecodedRunData[i] = 0;
		            }
		        }
		    }
	    }
	    else
	    {
	        // This occurs when all coefficients are present
	        // Set all runs to be zero
	        AlignedIntMemClr(m_VLDDecodedRunData, 16*sizeof(UNSIGNED_SEPTET));
	    }
	}
	return tCoef;
}


#if defined(I32K_D32K) || defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif

#ifdef I16K_D8K
#pragma Code()
#endif

#endif // defined(CAVLC_HW_PRESENT) // <><><><><><><><><><><><><><><><><>
