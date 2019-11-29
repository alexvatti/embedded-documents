/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007/8 ARC International (Unpublished)                  */
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


#include <string.h>
#include "ArcMPC.h"
#include "defines.h"
#include "ArcChannelRoutines.h"
#include "ArcMotionEstimationModel.h"
#include "EncoderControl.h"


#include <arc/arc_avu.h>
#ifdef MPEG4BUILD
#include "ArcMP4SDMTables.h"
#include "ArcLumaMP4SubPelFilter_MPA.h"
#else // MPEG4BUILD
#include "ArcSDMTables.h"
#include "ArcLumaH264SubPelFilter_MPA.h"
#include "ArcSoftwareQPelME_MPA.h"
#endif // MPEG4BUILD
#include "ArcSDMTables_MPA.h"
#include "ArcFrameBuffers.h"
#include "ArcEncoderMain_MPA.h"
#include "ArcSoftwareME_MPA.h"
#include "ArcSoftwareSubME_MPA.h"

#ifdef MPEG4BUILD
#include "CMP4Encoder.h"
#else
#include "CH264Encoder.h"
#endif // MPEG4BUILD

CHANNEL_SERVICE Service_DoLumaSubPelFilter(int cIdx);
/* ------------------------------------------------------------------------ */
/* FUNCTION:    Load_Current_MB_Luma(cMPCcb *me_curr_pointer, int intra)    */
/* DESCRIPTION: Loads in Data to be coded for this MB to the circular buffer*/
/*              Done in AV404V and AV402V only - hardware ME provides this  */
/*				in other variants 											*/
/* RETURNS:     void                                                        */
/* ------------------------------------------------------------------------ */
void ArcMotionEstimationModel::Load_Current_MB_Luma(cMPCcb *me_curr_pointer, int intra)
{
	
	

    _vr00 = _vmovw(16*MgrMPC.CurrentMeMB.Col, 0x01); // prediction hor comp
    _vr00 = _vmovw(16*MgrMPC.CurrentMeMB.Row, 0x02); // prediction vert comp
    _vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
    _vr01 = _vmovw(0x1010, 0x01); // block size
	_vr00 = _vmovw( (unsigned int)me_curr_pointer->PixelCoeffBuffer, 0x40); // SDM address where to store Y ref data

	_vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_CURR_INDEX), 0x02);

    _vrun(MACRO_CopyMBLumaToSDM);


    ReadNeighbourDataFromMPC();
   
#ifdef MPEG4BUILD
if(intra) 
	{
		WriteNeighbourDataToMPC(MBCT_MPEG4_INTRA);
	}
else
	{
		WriteNeighbourDataToMPC(MBCT_MPEG4_INTER16X16);
	}    
    
#else // MPEG4BUILD 
    if(intra) 
    {
    	WriteNeighbourDataToMPC(MBCT_H264_INTRA16X16);
    }
    else
    {
    	WriteNeighbourDataToMPC(MBCT_H264_INTER16X16);
    }
#endif // MPEG4BUILD   
    // need to increment the ME models indices at the end of every iteration
	IncrementMBIndices();
	
}
/* ------------------------------------------------------------------------ */
/* FUNCTION:    Software_ME_control(cMPCcb *me_curr_pointer)                */
/* DESCRIPTION: Calls SIMD Software Motion Estimation functions for variants*/
/*              AV404V and AV402V									        */
/* RETURNS:     void                                                        */
/* ------------------------------------------------------------------------ */
void ArcMotionEstimationModel::Software_ME_control(cMPCcb *me_curr_pointer)
{
	int predX, predY;

    u_int8 * mpcBytePtr=(u_int8 *) me_curr_pointer;
  
    int sIdx = MgrMPC.IdxSetupME;

	int col = MgrMPC.ActMPC[sIdx].Column;
	int leftIdx = MgrMPC.ActMPC[sIdx].LeftIdx;

;
// calculate skipped vectors
	GenerateSkippedMotionVectors(); // forms part of the hardware ME function

// calculate predicted vectors for Inter16x16 - we need it for RD calcs
	GeneratePredictedMotionVectors( 0, 0, H264_16X16,predX, predY);

  	me_curr_pointer->mvp[0].mvx =(s_int8) predX;
	me_curr_pointer->mvp[0].mvy =(s_int8) predY;

    
	_vr00 = _vmovw(sIdx, 1);
	_vr00 = _vmovw(col, 2);
	_vr00 = _vmovw(leftIdx, 4);

#ifdef MPEG4BUILD
	_vrun(MACRO_SetCurrentMPC); 
	int lambda  = MgrMPC.ActMPC[sIdx].QuantValue;
#else // MPEG4BUILD
	_vrun(MACRO_SetCurrentMPC0);
	int lambda = h264_encoder_rd_sad_lambda[MgrMPC.ActMPC[sIdx].QuantValue];
#endif // MPEG4BUILD
	
	_vrun(MACRO_Set_Skip_Large);// set large value for extra points minimisation
	// note that points tried here will take precedence over the result of the 
	// step search
	// not a lot of point trying skip and pred, as skip if it's not pred is going to be
	// zero, which is tried anyway in the step search.  Theonly difference is that the
	// RD costs will not be correct for anything other than pred, as the SAD_calc macro 
	// assumes a bit cost of 2

	// Use m_UseInter16x16Only flag for pred vec, as it is ignored when software ME
	// since only Inter16x16 is done anyway, yet it will still be set by dynamic encoding 
	// and the default will be for it to be off for CIF sequences
	
	//if(col == 2 && MgrMPC.ActMPC[sIdx].Row == 5){	
	//	printf("");
	//}
	
	
	
	if(predX|predY)
	{ // don't bother with (0,0) it will be covered anyway in the step search
		me_curr_pointer->mv[0].mvx = (s_int8)predX;  // for now
		me_curr_pointer->mv[0].mvy = (s_int8)predY;  // for now
		SET_CallBackDoLumaSubPelFilter_BufIdx(sIdx);
	
		Service_DoLumaSubPelFilter(sIdx);
		
		// I think the circ buffer values are trashed by the mpeg4/h263 half pel interp
		// so re-set them here
		_vr00 = _vmovw(sIdx, 1);
		_vr00 = _vmovw(col, 2);
		_vr00 = _vmovw(leftIdx, 4);

	#ifdef MPEG4BUILD
		_vrun(MACRO_SetCurrentMPC); 
	#else // MPEG4BUILD
		_vrun(MACRO_SetCurrentMPC0);
	#endif // MPEG4BUILD	
		
		_vr00 = _vmovw(lambda, 128); // needed for RateDist calc
		_vr00 = _vmovw(predX, 4);
		_vr00 = _vmovw(predY, 8);
		_vrun(MACRO_SAD_calc);
		// this saves the pred vec cost in vr30, not used elsewhere, until the 
		// save_vectors macro, where it is compared to the result of the step search
	}
		

	   
//DMA reference data into SDM
// we need 8 pixels either side of the current MB location as a start
    
	_vr00 = _vmovw(16*MgrMPC.CurrentMeMB.Col - 8 , 0x01); // prediction hor comp
	_vr00 = _vmovw(16*MgrMPC.CurrentMeMB.Row - 8 , 0x02); // prediction vert comp
	_vr00 = _vmovw(0, 0x80); // clear the top 16 bits of the SDM address
	_vr01 = _vmovw(0x2020, 0x01); // block size 32x32
	_vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x40); // SDM address where to store Y ref data
	if (gArcFrameBuffers.YUVPos)
	{
		_vr01 = _vmovw( (FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
	}
	else 
	{
		_vr01 = _vmovw( (FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
	}
	_vrun(MACRO_CopyMERefToSDM);




	//reset the vals needed for Rate Distortion
	_vr00 = _vmovw(predX, 1);
	_vr00 = _vmovw(predY, 2);
	_vr00 = _vmovw(lambda, 128);

	int match_pattern = 255; // this indicates which of the 8 sums to consider in minimisation
	int StepMult = 16; // the step multiplier in sub-pel units (1/4 pel for h264, 1/2 pel for MPEG4)
#ifdef MPEG4BUILD		
	if(gCH264Encoder.m_coding_standard == H263)
	{
		// H263 in baseline, does not allow Motion Vectors to be pointing outside the frame
		// set match_pattern to determine which of the 8 sads in each step to be included
		if(MgrMPC.ActMPC[sIdx].Column == 0)match_pattern &= 214; // left  11010110
		if(MgrMPC.ActMPC[sIdx].Row == 0)match_pattern &= 248;    // above 11111000		

		if(MgrMPC.ActMPC[sIdx].Column == (MgrMPC.Picture.Width-1))match_pattern &= 107; 
																 // right 01101011		
		if(MgrMPC.ActMPC[sIdx].Row == (MgrMPC.Picture.Hgt-1))match_pattern &= 31; 
																 // below 00011111		
	}
	StepMult = 8;
#endif // MPEG4BUILD	

	_vr00 = _vmovw(0, 12); // set vecs to zero
	_vr01 = _vmovw(StepMult*2, 16);

	_vr01 = _vmovw(16*MgrMPC.CurrentMeMB.Col - 8 , 0x40); // prediction hor comp
	_vr01 = _vmovw(16*MgrMPC.CurrentMeMB.Row - 8 , 0x80); // prediction vert comp

	//gCH264Encoder.m_StepOneControl=2;// for temporary testing
	
	if(gCH264Encoder.m_StepOneControl == 1){ // just horizontal +/- 8
		_vr00 = _vmovw(match_pattern& 24, 64); // 24 = 0x18 -i.e only 2 positions plus zero
		_vrun(MACRO_StepOne);
	}
	else if(gCH264Encoder.m_StepOneControl == 0){
		_vr00 = _vmovw(match_pattern , 64); // full first step (9 points, inc zero)
		_vrun(MACRO_FullStepOne);
	}
	else {
		_vr00 = _vmovw(match_pattern, 64); // just zero required 
		_vrun(MACRO_ZeroDisplacement);
	}
	
	// intraValid and skip are required to be written to the MPC in the ME function
	int skip = (0xff&PSliceSkippedHor) + ((0xff&PSliceSkippedVer)<<8);
	int intraValid = gArcMotionEstimationModel.leftMB.validID | (gArcMotionEstimationModel.aboveMB.validID<<1);
	_vr01 = _vmovw(intraValid , 64);
	_vr01 = _vmovw(skip, 128);
	_vr00 = _vmovw(match_pattern, 64); // overwrite CirLeft which we won't use
	_vr01 = _vmovw(StepMult, 16);
// now do 3-step search
	
	_vrun(MACRO_IntegerME);  // always do the 3 step integer search

	if(gCH264Encoder.m_SubPelControl> 0) {
		_vrun(MACRO_HalfPelME); // sometimes do the half pel search
	}
#ifndef MPEG4BUILD
	if(gCH264Encoder.m_SubPelControl > 1) {		
		_vrun(MACRO_QuarterPelSearch); // only do the quarterpel search (if required) for H264
	}
#endif // MPEG4BUILD		


	// see whether the initial point(s) turned
	// out to be better than the step search point
	// and save vectors to MPC structure.
	_vrun(MACRO_SaveVectors);
		

    // Sub-pel interpolation
    SET_CallBackDoLumaSubPelFilter_BufIdx(sIdx);
    _vrun(MACRO_CallBackDoLumaSubPelFilter);

    _vrun(MACRO_Generate_Soft_ME_Response);
}


#ifndef MPEG4BUILD
/* ------------------------------------------------------------------------ */
/* FUNCTION:    SIMDFilterLumaH264                                          */
/* DESCRIPTION: H.264 sub-pel sample generation using a 6-tap filter for    */
/*              half-pels and bilinear interpolation for quarter-pels       */
/* RETURNS:     void                                                        */
/* ------------------------------------------------------------------------ */
void SIMDFilterLumaH264(int fractional_hor_vector,
                        int fractional_ver_vector)
{
    int subPelPosition = (fractional_ver_vector<<2) + fractional_hor_vector;

    // calculate inter-prediction values
    switch( subPelPosition )
    {
    case 1: // 'a' position on diagram in the spec
        //perform horizontal filtering on all input rows
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vrun(MACRO_PerformInpHorFiltering);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN1- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(2, 0x08);                         // Discard 2 left-most columns
        _vrun(MACRO_QPelFiltering);                           // First 2 columns of ref buf (extra pels to the left) are not used

        break;


    case 2: // 'b' position on diagram in the spec
        //perform horizontal filtering and store result in output array
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);
        break;


    case 3: // 'c' position on diagram in the spec
        //perform horizontal filtering on all input rows
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vrun(MACRO_PerformInpHorFiltering);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN1- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(3, 0x08);                         // Discard 3 left-most columns
        _vrun(MACRO_QPelFiltering);                           // First 3 columns of ref buf (pels to the left) are not used

        break;

    case 4: // 'd' position on diagram in the spec
        //perform vertical filtering on block_hor_size at offset 0
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(2, 0x04);                         // Number of vertical bands the integer prediction is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 2*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN1-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(0, 0x08);                         // No discards
        _vrun(MACRO_QPelFiltering);

        break;


    case 5: // 'e' position on diagram in the spec
        //perform vertical filtering on only block_hor_size at offset 2
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        // calculate 'b' and store result in half pel buffer
        // discard the first 2 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 2*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        // Different Hor half pel samples are used for 5 & 13 but
        // this re-arrangement is done in hor-half pel calculation
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(2, 0x08);                         // Discard 2 left-most columns
        _vrun(MACRO_QPelFiltering);                           // First 2 columns of ref buf (extra pels to the left) are not used

        break;

    case 6: // 'f' position on diagram in the spec
        //perform vertical filtering on all columns
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringWORounding);

        // calculate 'j'
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN-  intermediate working bffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vrun(MACRO_CentrePelFilterWOInpRounding);

        // calculate 'b' and store result in half pel buffer
        // discard the first 2 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 2*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN1- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(0, 0x08);                         // No discards
        _vrun(MACRO_QPelFiltering); // d,f,n,q
        break;
    case 7: // 'g' position on diagram in the spec
        // perform vertical filtering on block_hor_size columns at offset 3
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        // calculate 'b' and store result in half pel buffer
        // discard the first 2 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 2*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        // Different Hor half pel samples are used for 7 & 15 but
        // this re-arrangement is done in hor-half pel calculation
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(3, 0x08);                         // Discard 3 left-most columns
        _vrun(MACRO_QPelFiltering);                      // First 3 columns of ref buf (pels to the left) are not used

        break;

    case 8: // 'h' position on diagram in the spec
        //perform vertical filtering and store result in output array
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vr00 = _vmovw(2, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        break;

    case 9: // 'i' position on diagram in the spec
        //perform vertical filtering on all columns
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringWORounding);

        // calculate 'j' and round the vertical half-pels
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN-  intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT1-prediction buffer; Rounded input
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x04);      // OUT2-intermediate working bffer; j samples
        _vrun(MACRO_CentrePelFilAndInpRounding);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN1- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(2, 0x08);                         // Discard 2 left-most columns
        _vrun(MACRO_QPelFiltering);                           // First 2 columns of ref buf (extra pels to the left) are not used

        break;
    case 10: // 'j' position on diagram in the spec
        //perform vertical filtering on all columns
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringWORounding);

        // calculate the final output 'j'
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN-  intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_CentrePelFilterWOInpRounding);

        break;
    case 11: // 'k' position on diagram in the spec
        //perform vertical filtering on all columns
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringWORounding);

        // calculate 'j' and round the vertical half-pels
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN-  intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT1-prediction buffer; Rounded input
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x04);      // OUT2-intermediate working bffer; j samples
        _vrun(MACRO_CentrePelFilAndInpRounding);

        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN1- prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // IN2- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(3, 0x08);                         // Discard 3 left-most columns
        _vrun(MACRO_QPelFiltering);                      // First 3 columns of ref buf (pels to the left) are not used

        break;
    case 12: // 'n' position on diagram in the spec
        //perform vertical filtering on block_hor_size at offset 0
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(2, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 3*ME_REFERENCE_ARRAY_STRIDE, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT-  prediction buffer
        _vr00 = _vmovw(0, 0x08);                         // No discards
        _vrun(MACRO_QPelFiltering); // d,f,n,q

        break;

    case 13: // 'p' position on diagram in the spec
        //perform vertical filtering on only block_hor_size at offset 2
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        // calculate 's' and store result in half pel buffer
        // discard the first 3 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 3*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        // Different Hor half pel samples are used for 5 & 13 but
        // this re-arrangement is done in hor-half pel calculation
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(2, 0x08);                         // Discard 2 left-most columns
        _vrun(MACRO_QPelFiltering);                      // First 2 columns of ref buf (extra pels to the left) are not used

        break;

    case 14: // 'q' position on diagram in the spec
        //perform vertical filtering on all columns
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringWORounding);

        // calculate 'j'
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN-  intermediate working bffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vrun(MACRO_CentrePelFilterWOInpRounding);

        // calculate 's' and store result in half pel buffer
        // discard the first 3 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 3*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(0, 0x08);                         // No discards
        _vrun(MACRO_QPelFiltering); // d,f,n,q
        break;

    case 15: // 'r' position on diagram in the spec
        // perform vertical filtering on block_hor_size columns at offset 3
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);    // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x02);      // OUT- intermediate working bffer
        _vr00 = _vmovw(3, 0x04);                         // Number of vertical bands the integer prediction area is divided to (width/8)
        _vrun(MACRO_VerticalFilteringAndRounding);

        // calculate 's' and store result in half pel buffer
        // discard the first 3 rows of the input
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY + 3*ME_REFERENCE_ARRAY_STRIDE, 0x01); // IN-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // OUT- prediction buffer
        _vrun(MACRO_PerformInpHorFiltering);

        // Different Hor half pel samples are used for 7 & 15 but
        // this re-arrangement is done in hor-half pel calculation
        _vr00 = _vmovw(SDM_ME_WORKING_ARRAY, 0x01);      // IN1- intermediate working bffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);    // IN2-  prediction buffer
        _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x04);    // OUT- prediction buffer
        _vr00 = _vmovw(3, 0x08);                         // Discard 3 left-most columns
        _vrun(MACRO_QPelFiltering);                           // First 3 columns of ref buf (pels to the left) are not used

        break;

    default:
        assert(false);
        break;
    }

    return;
}
#else
/* ------------------------------------------------------------------------ */
/* FUNCTION:    SIMDFilterLumaMP4                                           */
/* DESCRIPTION: Bilinear interpolation for half-pel sample generation for   */
/*              MPEG4/H263                                                  */
/* RETURNS:     void                                                        */
/* ------------------------------------------------------------------------ */
void SIMDFilterLumaMP4(int fractional_hor_vector,
                       int fractional_ver_vector, int rnd_ctrl)
{
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x02);
    _vr00 = _vmovw(fractional_hor_vector, 0x04);
    _vr01 = _vmovw(fractional_ver_vector, 0x04);
    _vr00 = _vmovw(rnd_ctrl, 0x08);
    _vrun(MACRO_FilterLumaPred);

    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8, 0x01);
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8, 0x02);
    _vrun(MACRO_FilterLumaPred);

    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8*ME_REFERENCE_ARRAY_STRIDE, 0x01);
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8*ME_REFERENCE_ARRAY_STRIDE, 0x02);
    _vrun(MACRO_FilterLumaPred);

    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8*ME_REFERENCE_ARRAY_STRIDE+8, 0x01);
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY+8*ME_REFERENCE_ARRAY_STRIDE+8, 0x02);
    _vrun(MACRO_FilterLumaPred);
}
#endif

CHANNEL_SERVICE Service_DoLumaSubPelFilter(int cIdx)
{

    int xSize;
    int ySize;
    int fractional_for_hor_vector;
    int fractional_for_ver_vector;
    int xOffset;
    int yOffset;
    cMPCcb &cb = WorkAreaSDM->CircularBuffs[cIdx];
    int mvx = cb.mv[0].mvx;
    int mvy = cb.mv[0].mvy;

    // ------------------------------------------------
    // DMA luma prediction into SDM ME reference area
    // Always get extended prediction area regardless
    // whether MV is fractional or not
    // ------------------------------------------------
#ifndef MPEG4BUILD //H.264

    xSize = 16;
    ySize = 16;

    fractional_for_hor_vector = mvx & 3;
    fractional_for_ver_vector = mvy & 3;

    xOffset = 16*MgrMPC.ActMPC[cIdx].Column + (mvx>>2);
    yOffset = 16*MgrMPC.ActMPC[cIdx].Row + (mvy>>2);
    if (fractional_for_hor_vector)
    {
        xOffset -= 2;
        xSize   += 5;
    }
    if (fractional_for_ver_vector)
    {
        yOffset -= 2;
        ySize   += 5;
    }
    _vr00 = _vmovw(xOffset, 0x01);                         // prediction hor comp. MVx will be added inside CopyLumaPredToSDM
    _vr00 = _vmovw(yOffset, 0x02);                         // prediction ver comp. MVy will be added inside CopyLumaPredToSDM
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x40);          // SDM address where to store U prediction
    _vr00 = _vmovw(0, 0x80);                               // clear the top 16 bits of the SDM address
    _vr01 = _vmovw((ySize<<8)|xSize, 0x01);                // block size

    // point at the right reference frame
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_CopyLumaPredToSDM);
    _ASM("vdmawait 1, 0x7f"); // wait for only 0 dma in and max dma outs left in fifos

    // Perform the filtering
    if (fractional_for_hor_vector || fractional_for_ver_vector)
        SIMDFilterLumaH264(fractional_for_hor_vector, fractional_for_ver_vector);

#else // MPEG4/H263

    xSize = 16;
    ySize = 16;
    fractional_for_hor_vector = mvx & 1;
    fractional_for_ver_vector = mvy & 1;
    xOffset = 16*MgrMPC.ActMPC[cIdx].Column + (mvx>>1);
    yOffset = 16*MgrMPC.ActMPC[cIdx].Row + (mvy>>1);
    if (fractional_for_hor_vector)
        xSize   += 1;
    if (fractional_for_ver_vector)
        ySize   += 1;
    _vr00 = _vmovw(xOffset, 0x01);                         // prediction hor comp. MVx will be added inside CopyLumaPredToSDM
    _vr00 = _vmovw(yOffset, 0x02);                         // prediction ver comp. MVy will be added inside CopyLumaPredToSDM
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x40);          // SDM address where to store U prediction
    _vr00 = _vmovw(0, 0x80);                               // clear the top 16 bits of the SDM address
    _vr01 = _vmovw((ySize<<8)|xSize, 0x01);                 // block size

    // point at the right reference frame
    if (gArcFrameBuffers.YUVPos)
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF1_INDEX), 0x02);
    else
        _vr01 = _vmovw((FRAME_TABLE_Y_INDEX + FRAME_TABLE_REF2_INDEX), 0x02);
    // perform DMA transfer
    _vrun(MACRO_CopyLumaPredToSDM);
    _ASM("vdmawait 1, 0x7f"); // wait for only 0 dma in and max dma outs left in fifos

    // Perform the filtering
    if (fractional_for_hor_vector || fractional_for_ver_vector)
        SIMDFilterLumaMP4(fractional_for_hor_vector, fractional_for_ver_vector, gCH264Encoder.m_rounding_control);

#endif
    
    // Store final prediction in prediction buffer in MPC
    _vr00 = _vmovw(SDM_ME_REFERENCE_ARRAY, 0x01);
    _vr00 = _vmovw((unsigned int)(&(cb.PixelCoeffBuffer[PCB_REFERENCE_Y])), 0x02);
    _vrun(MACRO_StoreFilteredLumaPred);
    
}

