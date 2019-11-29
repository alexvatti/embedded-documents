/* ------------------------------------------------------------------------ */
/* FILE:			CH264Decoder.h											*/
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

#ifndef	CH264_DECODER_H
#define	CH264_DECODER_H

extern "C" {
#include <stdio.h>
#include <stdlib.h>
}
//#endif

/*
--------------------------------------------------------------------------------
#define definitions.
--------------------------------------------------------------------------------
*/

#define MAXIMUM_NUMBER_OF_SLICE_GROUPS 8

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include "BTPicture.h"
#include "H264EnumTypes.h"

#include "H264Decoder.h"
#include "H264DecoderControl.h"
#include "H264ParameterSets.h"
#include "H264MotionVectorList.h"
#include "H264SEIParameters.h"
#include "H264DecodedPictureBuffer.h"


#if defined _WIN32
	#include "StdAfx.h"
	#include "Aurora.h"
#else
	#include <stdio.h>
#endif	//_WIN32

	#include "ARCMedia.h"
	#include "AuroraH264Dec.h"

#if defined(CAVLC_HW_PRESENT)
#include "vlc_api.h"
#endif

/*
--------------------------------------------------------------------------------
Class declarations.
--------------------------------------------------------------------------------
*/

class CBTPicture;
class CH264DecodedPictureBuffer;
class CH264MotionVectorList;

/*
--------------------------------------------------------------------------------
Type definitions.
--------------------------------------------------------------------------------
*/

struct working_array_t
{
	short data[WORKING_ARRAY_SIZE][WORKING_ARRAY_SIZE];
};

/*
--------------------------------------------------------------------------------
Class:	CH264Decoder
--------------------------------------------------------------------------------
*/

class	CH264Decoder
{
    AuroraH264Dec * Simdu;
    
public:
	
	CH264Decoder(void);
	~CH264Decoder(void);

	bool Create(void);
#if defined __ARC_LINUX__
	bool InitAurora(void *);
#else
	bool InitAurora(void);
#endif
	bool Destroy(void);

	int Decode(CODED_BYTE* buffer, int buffer_length, bool data_loss,
				bool start_of_NAL_unit, bool end_of_NAL_unit,
				bool discard_if_disposable, unsigned int time_stamp,
				h264_decoder_parameters** decoder_parameters);

private:
	void InitialiseMemberVariables(void);
	void SetReturnParameters(CBTPicture * decoded_picture);
	void ClearReturnParameters(void);

private:
	// For the ARMulator, it is most efficient to have the most often used member variables
	// at the top of the list of member variables. This is because it loads and stores member
	// variable values by adding an offset to the 'this' pointer. If the offset is less than 256,
	// it can do it in two instructions (load the offset, load the mmber variable using the offset),
	// otherwise it typically needs four instructions to calculate the address amd load the value.

	//////////////////////////////////////////////////////////////////////////////////////
	// Byte buffer member variables
	//////////////////////////////////////////////////////////////////////////////////////

#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	CODED_BYTE * m_ByteBufferEnd;			// Pointer to byte after last byte in buffer
	CODED_BYTE * m_ByteBufferReadPosition;	// Pointer to next byte to be read from buffer

	int m_ByteBufferBitsInRegister;				// Number of bits currently left in Shift Register
	unsigned int m_ByteBufferShiftRegister;		// The shift register used 
	int m_ByteBufferZeroByteCount;				// Counts the number of preceding bytes that were zero, for byte stuffing removal

    //////////////////////////////////////////////////////////////////////////////////////
    // Byte buffer UVLC decoding member variables
    //////////////////////////////////////////////////////////////////////////////////////

    int m_ByteBufferState;
    int m_ByteBufferFlagLength;
    int m_ByteBufferDataValue;
    int m_ByteBufferDataLength;
#endif
//    int m_ByteBufferCodeNumber;

	//////////////////////////////////////////////////////////////////////////////////////
	// CABAC decoding member variables
	//////////////////////////////////////////////////////////////////////////////////////

	
    //////////////////////////////////////////////////////////////////////////////////////
	// Variable length decoding member variables
	//////////////////////////////////////////////////////////////////////////////////////

	h264_vld_state_t m_VLDState;
	int m_VLDStateCount;
    h264_8x8_partition_mode_type_t m_VLDCurrentPartitionMode;

	CBTPicture * m_VLDCurrentPicture;

	h264_slice_type_t m_SliceType;

	// m_CurrentSlice_ID if the slice ID of the currently being decoded slice 
	// It is not from the bitstream but is generated locally to label each macroblock with the 
	// ID of a slice so that the decoding process can determine whether a macroblock is in the 
	// same slice as another
	int m_CurrentSlice_ID;	

	//bool m_DirectSpatialMotionVectorPredFlag;
	int m_SliceNumList0RefPictures;

	int m_DisableDeblockingFilterIDC;
	int m_SliceAlphaC0Offset;
	int m_SliceBetaOffset;

	int m_OutputPictureNumber;			// Increments for each picture in output order
	int m_RemainingBumpingDelay;
	int m_CurrentTimestamp;
	bool m_BumpingDecoder;
	int m_MaxDecFrameBuffering;

	bool m_NALHRDParameters;
	
	int m_VLDList0MVCount;

	int m_VLDQuant;
	int m_VLDQuantDiv;
	int m_VLDQuantMod;
	int m_VLDQuantChromaDiv;
	int m_VLDQuantChromaMod;

    int m_VLDTotalCoefficients;
	int m_VLDBlockSize;  // 16 except for H264_CHROMA_DC when it is 4

	int m_VLDCoeffTokenTable;
#if !defined(CAVLC_HW_PRESENT)
	h264_block_t m_VLDBlockType;
    int m_VLDIndexIntoLevelArray;
	bool m_FirstCoefficient;
	int m_VLDZerosLeft;
    int m_VLDLevelTable;
    int m_VLDTrailingOnes;
    int m_VLDOtherCoefficients;
#endif
	u_motion_vector m_MotionVec;
	
	int m_NumberOfMacroblocksDecoded;
	bool m_PacketLost;
	bool m_DecodingHasStarted;
	bool m_IncompatibleBitstream;  // Set when the build of code is not (expected to be) compatible with the bitstream
	
	// Reconstructed reference pictures
	CH264DecodedPictureBuffer * m_DecodedPictureBuffer;

//-----------------------------------------------
    int m_UnderFlowBuffer[40];					// Handle VLC error cases
	short m_VLDDecodedLevelData[16];			// Stores levels in the order they are decoded, i.e. zig-zag scan order
	UNSIGNED_SEPTET m_VLDDecodedRunData[16];	// Stores runs of preceding zeros associated with each level
//-----------------------------------------------

	int m_NALReferenceIDC;
	h264_NAL_unit_type_t m_NALUnitType;

	unsigned int m_FrameNumber;
	int m_PictureOrderCount;
	int m_POCAccumulator;
	int m_TopFieldOrderCount;
	int m_BottomFieldOrderCount;

	int m_NumberOfMacroblocksInPicture;

	//int m_RedundantPictureCount;

	int m_SEIPayloadType;
	int m_SEIPayloadSize;

	//////////////////////////////////////////////////////////////////////////////////////
	// Parameter set and API parameters
	//////////////////////////////////////////////////////////////////////////////////////

	CH264ParameterSets * m_ParameterSets;

	//int m_SequenceParameterSetID;
	int m_PictureParameterSetID;

	sequence_parameter_set_t * m_SequenceParameterSet;
	picture_parameter_set_t * m_PictureParameterSet;

	int m_VLDSeqParamSetProfileLevel;

	h264_decoder_parameters m_H264DecoderParameters;


	//////////////////////////////////////////////////////////////////////////////////////
	// Decoding arrays
	//////////////////////////////////////////////////////////////////////////////////////
	
	working_array_t m_ForwardWorkingArray;

	int:0;
	short m_y_dc_data[NUMBER_OF_LUMA_FOUR_BY_FOUR_BLOCKS];	// Y DC coefficient data
	short m_u_dc_data[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS];	// U DC coefficient data
	short m_v_dc_data[NUMBER_OF_CHROMA_FOUR_BY_FOUR_BLOCKS];	// V DC coefficient data

	short m_simd_error;	// should be initialized to 0 even if not using SIMD extension

	SDM_Offset m_sdm_buf_ofs;
	SDM_Offset m_prev_sdm_buf_ofs;


	SDM_Offset m_y_data;			// Y macroblock data
	SDM_Offset m_u_data;			// U macroblock data
	SDM_Offset m_v_data;			// V macroblock data




	//////////////////////////////////////////////////////////////////////////////////////
	// Slice Header decoding state variables
	//////////////////////////////////////////////////////////////////////////////////////

	int m_VLDCurrentSliceGroup;  // The slice group of the slice currently being decoded
	bool m_VLDSliceHeaderFirstSliceOfPicture;
	int m_VLDSliceHeaderPreviousSlicePictureOrderCount;
	unsigned int m_VLDSliceHeaderPreviousSliceFrameNumber;

	int m_VLDSliceHeaderPicOrderCntLsb;
	int m_VLDSliceHeaderDeltaPicOrderCountBottom;
	int m_VLDSliceHeaderDeltaPicOrderCount[2];
	int m_VLDSliceHeaderPreviousPicOrderCntLsb;
	int m_VLDSliceHeaderPreviousDeltaPicOrderCountBottom;
	int m_VLDSliceHeaderPreviousDeltaPicOrderCount[2];

	int m_VLDSliceHeaderFirstMBInSlice;
	int m_VLDSliceHeaderIDRPictureID;
	int m_VLDSliceHeaderDiffPicNumMinus1;

	bool m_VLDSliceHeaderFirstSlice;
	// The following group of variables should only be used in decoding the slice header
	// At the end of decoding a slice header, they are updated and so apply to the current slice
	bool m_VLDSliceHeaderPreviousSliceWasIDR;
	int m_VLDSliceHeaderPreviousSliceNALReferenceIDC;
	int m_VLDSliceHeaderPreviousSliceIDRPictureID;
	int m_VLDSliceHeaderPreviousReferenceSlicePOC;
	unsigned int m_VLDSliceHeaderPreviousFrameNumberOffset;


	int m_MapUnitsInSliceGroup0;
	int m_SliceGroupIDCodeLength;

	bool m_InloopDeblock;


	//////////////////////////////////////////////////////////////////////////////////////
	// The decoding control list
	//////////////////////////////////////////////////////////////////////////////////////

	CH264DecoderControl m_DecoderControl;

	h264_decoder_control_t * GetControlList(void) const  { return m_DecoderControl.m_CurrentElement; }
	h264_decoder_control_t * GetControlTop(void) const   { return m_DecoderControl.m_ControlList+MAX_RES_IN_MB_X+3; }


	//////////////////////////////////////////////////////////////////////////////////////
	// Byte buffer member functions
	//////////////////////////////////////////////////////////////////////////////////////

//	bool ByteBufferInitialise(void);
//    inline bool ByteBufferDestroy(void) { return ByteBufferInitialise(); }
	void ByteBufferSetPointers(CODED_BYTE * data, int data_length, bool start_of_NAL_unit, int &byteCode);
    inline void ByteBufferReset(int &ret) { ByteBufferFlush(ret); }
	void ByteBufferFlush(int &ret);
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	bool ByteBufferFillShiftRegister(void);

	//////////////////////////////////////////////////////////////////////////////////////
	// Byte buffer decoding member functions
	//////////////////////////////////////////////////////////////////////////////////////

	bool ByteBufferLongStartCode(int & byteCode);
	bool ByteBufferDecodeUniversalVLC(int & byteCode);
	bool ByteBufferDecodeFixedLengthCode(int code_length, int &ret);
#endif
#if !defined(CAVLC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	bool ByteBufferDecodeCoeffToken(const UNSIGNED_SEPTET code_table[][2],int & byteCode);
	bool ByteBufferDecodeTotalZerosVLC(const UNSIGNED_SEPTET * vlc_table, int number_of_bits, int & byteCode);
	bool ByteBufferDecodeTotalZerosDCVLC(int & byteCode);
	bool ByteBufferDecodeRunBeforeVLC(int & byteCode);
	bool ByteBufferDecodeRunBeforeTable7VLC(int & byteCode);
	bool ByteBufferDecodeLevelVLC(int table_number, int & byteCode);
#endif
    inline void ByteBufferByteAlign(void)
    {
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	    int number_of_bits_to_discard = m_ByteBufferBitsInRegister & 7;
	    m_ByteBufferBitsInRegister &= 0xfffffff8;
	    m_ByteBufferShiftRegister <<= number_of_bits_to_discard;
#else
        int word_buffer_number_of_bits_to_discard = (8 - VLC_Get_Offset() & 0x7) % 8;

        VLC_Get_Bits(word_buffer_number_of_bits_to_discard);
#endif
    }
    inline int ByteBufferBitsLeft(void) const
    {
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
	    return ((m_ByteBufferEnd - m_ByteBufferReadPosition)*8 + m_ByteBufferBitsInRegister);
#else
        return VLC_Get_Bits_Left();
#endif
    }
	int  ByteBufferNonTrailingBitsLeft();


#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
// functions to accelerate CAVLC entropy decoding
    inline UNSIGNED_SEPTET DecodeCAVLCCoefficient(h264_decoder_control_t * control);
    inline UNSIGNED_SEPTET DecodeCAVLCCoefficient_Choma_DC(h264_decoder_control_t * control);
// end of CAVLC acceleration function declarations
#endif

#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)

#else
    inline bool DecodeCAVLCList0RefIndex(int &byteCode);
#endif

	//////////////////////////////////////////////////////////////////////////////////////
	// Variable length decoding member functions
	//////////////////////////////////////////////////////////////////////////////////////

	bool VLDInitialise(void);
#if !defined(CAVLC_HW_PRESENT) && !defined(CABAC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)
    inline int VLDDecodeMapCodeNumberToSignedGolomb(int byteCode) const
    {
       	if (0 == (byteCode & 1))
       	{
      	  return -(int)(byteCode >> 1);
     	}
       	else
       	{
       	  return (int)((byteCode + 1) >> 1);
    	}
    }
#endif
	bool VLDDecodeSequenceParameterSet(int &byteCode);
	bool VLDDecodeVUI(void);
	bool VLDDecodePictureParameterSet(int &byteCode);
	bool VLDDecodeSEIMessage(int & byteCode);
	bool VLDDecodeSliceHeader(void);
	bool VLDDecodeStartOfPicture(void);
	void VLDDecodeLostReferencePictureConcealment(void);
	bool VLDDecodeAllocateSliceGroups(void);

	void DecodeSliceData(int byteCode, h264_vld_state_t stateVLD);
	
	int VLDDetermineVLCTableForLumaCoeffToken(h264_decoder_control_t * control, int bn) const;
    int VLDDetermineVLCTableForLumaCoeffToken0(h264_decoder_control_t * control) const;
	int VLDDetermineVLCTableForChromaCoeffToken(h264_decoder_control_t * control, int bn) const;
	void VLDDecodeDifferentialMotionVector(h264_decoder_control_t * control,
										   int block_index, int vector_number,
										   h264_block_shape_type_t block_shape
                                                                                   );

	bool VLDEndOfMacroblock(h264_decoder_control_t * control, h264_vld_state_t &stateVLD);
	void VLDEndOfPicture(bool stored_for_reference);
	void VLDOutputPicture(void);
	void DecoderErrorConcealment(void);

    inline int CalculateYW (int yN, int maxWH) const { return ((yN + maxWH) & (maxWH-1)); }
	const h264_decoder_control_t *GetNeighbouringLocationNonMBAFFLuma(int xN, int yN, const h264_decoder_control_t * current_control) const;




	//////////////////////////////////////////////////////////////////////////////////////
	// Intra reconstruction functions
	//////////////////////////////////////////////////////////////////////////////////////


	void SIMDIntraRecon4x4(h264_decoder_control_t * control);
	void SIMDIntraRecon16x16(h264_decoder_control_t * control);
	void SIMDIntraChromaRecon(h264_decoder_control_t * control);
	void SIMDIntraReconPCM(h264_decoder_control_t * control);

	void InitSimdTables();	// Copy tables into SDM
	//////////////////////////////////////////////////////////////////////////////////////
	// Inter reconstruction functions
	//////////////////////////////////////////////////////////////////////////////////////
	
	void SIMDInterRecon(h264_decoder_control_t * control);
	void SIMDInterRecon8x8(h264_decoder_control_t * control);
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Loop filter functions
	//////////////////////////////////////////////////////////////////////////////////////
//#ifdef INLOOP_DBLOCK 
	void DeblockMB(CBTPicture * picture, h264_decoder_control_t * control);
//#else
//#if 0 
	void LoopFilterNonMBAFFFrame(CBTPicture * picture);
//#endif 
//#endif

	void LoopFilterEdgeLuma(PIXEL * data, int data_inc, int data_inc_next, int strength,
							bool strong_filtering, int index_a, int index_b);

	void LoopFilterEdgeChroma(PIXEL * data, int data_inc, int data_inc_next, int strength,
							  bool strong_filtering, int index_a, int index_b);

        void LoopFilterEdgeChromaHorizontal(PIXEL * data, int data_inc_next, int strength,
                                                          int index_a, int index_b);
                                                          
        void LoopFilterEdgeChromaHorizontalStrong(PIXEL * data, int data_inc_next,
                                                               int index_a,
                                                               int index_b);

        void LoopFilterEdgeChromaVerticalStrong(PIXEL * data, int data_inc,
                                                                 int index_a,
                                                                 int index_b);

	//////////////////////////////////////////////////////////////////////////////////////
	// Motion vector functions
	//////////////////////////////////////////////////////////////////////////////////////


	void CalculateSkippedMotionVectors(h264_decoder_control_t * control);

	inline void InitMBFlags(h264_decoder_control_t * control)
	{
		char flg = control->Flags & CTLF_Above+CTLF_Left+CTLF_Right;
		int sliceId = control->slice_id;
		if(flg & CTLF_Above)
		{
			if(control->GetAbove()->slice_id == sliceId)
				flg |= CTLF_AboveInSlice;
		}
		if(flg & CTLF_Left)
		{
			if(control->GetLeft()->slice_id == sliceId)
				flg |= CTLF_LeftInSlice;
		}
//		if(flg & CTLF_Right)
//		{
//			if(control->GetRight()->slice_id == sliceId)
//				flg |= CTLF_RightInSlice;
//		}
		control->Flags = flg;
	}
	
};




#endif


/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
