/*
 * H263/MPEG4 backend for ffmpeg encoder and decoder
 * Copyright (c) 2000,2001 Fabrice Bellard.
 * H263+ support.
 * Copyright (c) 2001 Juan J. Sierralta P.
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ac prediction encoding, b-frame support, error resilience, optimizations,
 * qpel decoding, gmc decoding, interlaced decoding,
 * by Michael Niedermayer <michaelni@gmx.at>
 */

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
/*                                                                 */


/**
 * @file h263.c
 * h263/mpeg4 codec.
 */
#include "defines.h"
//#define DEBUG
#include <memory.h>
#include <stdlib.h> 
#include "CMP4Encoder.h"
#include "SymbolByteBuffer.h"

#include "MP4EncoderControl.h"
#include "MP4Tables.h"
#include "ArcEntropyEncoderModel.h"
#include "ArcMP4SDMTables.h"
#include <arc/arc_avu.h>
#include "ArcEncoderMain_MPA.h"
extern unsigned short y_dc_aan_scale_table[32];
extern unsigned short c_dc_aan_scale_table[32];
extern unsigned short MP4RawBlockOffset[6];
extern unsigned short MP4DiffBlockOffset[6];
extern unsigned short MP4PredBlockOffset[6];
extern unsigned short MP4FinalBlockOffset[6];
extern unsigned int y_dc_scale_table_shift20[32];
extern unsigned int c_dc_scale_table_shift20[32];
extern const unsigned char y_dc_scale_table[32];
extern const unsigned char c_dc_scale_table[32];

#define INTRA_MCBPC_VLC_BITS 6
#define INTER_MCBPC_VLC_BITS 7
#define CBPY_VLC_BITS 6
#define MV_VLC_BITS 9
#define DC_VLC_BITS 9
#define SPRITE_TRAJ_VLC_BITS 6
#define MB_TYPE_B_VLC_BITS 4
#define TEX_VLC_BITS 9
#define H263_MBTYPE_B_VLC_BITS 6
#define CBPC_B_VLC_BITS 3
#define DCTELEM short
#define MAX_MV 32
#define NULL 0
#define AV_TIME_BASE 1000000
/* run length table */
#define MAX_RUN    64
#define MAX_LEVEL  64

/* these matrixes will be permuted for the idct */
const short ff_mpeg4_default_intra_matrix[64] = {
  8, 17, 18, 19, 21, 23, 25, 27,
 17, 18, 19, 21, 23, 25, 27, 28,
 20, 21, 22, 23, 24, 26, 28, 30,
 21, 22, 23, 24, 26, 28, 30, 32,
 22, 23, 24, 26, 28, 30, 32, 35,
 23, 24, 26, 28, 30, 32, 35, 38,
 25, 26, 28, 30, 32, 35, 38, 41,
 27, 28, 30, 32, 35, 38, 41, 45, 
};

const short ff_mpeg4_default_non_intra_matrix[64] = {
 16, 17, 18, 19, 20, 21, 22, 23,
 17, 18, 19, 20, 21, 22, 23, 24,
 18, 19, 20, 21, 22, 23, 24, 25,
 19, 20, 21, 22, 23, 24, 26, 27,
 20, 21, 22, 23, 25, 26, 27, 28,
 21, 22, 23, 24, 26, 27, 28, 30,
 22, 23, 24, 26, 27, 28, 30, 31,
 23, 24, 25, 27, 28, 30, 31, 33,
};

extern ArcEntropyEncoderModel gArcEntropyEncoderModel;

//#define UNI_MPEG4_ENC_INDEX(last,run,level) ((last)*128*64 + (run)*128 + (level))

#define UNI_MPEG4_ENC_INDEX(last,run,level) (((last)<<13) + ((run)<<7) + (level))
/* mpeg4
inter
max level: 24/6
max run: 53/63

intra
max level: 53/16
max run: 29/41
*/

extern int picture_number;
//extern h264_picture_type_t picture_type;
#define IS_3IV1 0

#    define FASTDIV(a,b)   ((a)/(b))
//#define abs(a) (a < 0 ? -a : a) // for some reason this is giving weird results
//#define ABS(x) (x >= 0 ? x : -x)

void CH264Encoder::h263_encode_init(int *coding_standard)
{
	int temp = 1;

	m_rounding_control = 0;
	
	
    switch(m_coding_standard){
    case MPEG4:
      //  control->fcode_tab= fcode_tab;
		m_last_time_base = 0;
        m_min_qcoeff= -2047;// JRMM made to be symmetric, should have little effect on output
        m_max_qcoeff=  2047;


		m_mpeg_quant = 1;
	
		m_time_increment_resolution = m_frame_rate;
		m_time_increment_bits = 0;

		m_h263_modified_quant = 0;  // Annex T
		m_h263_use_gob_headers = 0; 
		m_h263_aic = 0;				// Annex I
		m_h263_loop_filter = 0;		// Annex J
		m_h263_slice_structured = 0;// Annex K
		m_use_ac_pred = 0; // not implemented
		while (temp < m_time_increment_resolution)
		{
			m_time_increment_bits++;
			temp = temp+temp;// double it
		}
      break;

	case H263:
		
		m_mpeg_quant = 0;
        m_min_qcoeff= -127; // jrmm should be -128, but made 127 here to be symmetric
        m_max_qcoeff=  127;
		m_use_ac_pred = 0; // not implemented
		if(m_h263_aic || m_h263_loop_filter || m_h263_slice_structured || m_h263_modified_quant) 
		{
			*coding_standard = m_coding_standard = H263_PLUS;
			if(m_h263_modified_quant)
			{
				m_min_qcoeff= -2048;
				m_max_qcoeff=  2047;
			}
			else
			{
				m_min_qcoeff= -128;
				m_max_qcoeff=  127;
			}
			if(!m_h263_loop_filter)
			{
				m_use_4MV = 0;
				m_UseInter16x16Only = 1;
			}
			if(m_h263_slice_structured)
			{
				m_h263_use_gob_headers = 0;
			}
		}
		m_video_packets = 0;
 		break;
    case H263_PLUS:
		
		m_mpeg_quant = 0;
		
		m_use_ac_pred = 0; // not implemented
		if(m_h263_modified_quant)
		{
			m_min_qcoeff= -2048;
			m_max_qcoeff=  2047;
		}
		else
		{
			m_min_qcoeff= -128;
			m_max_qcoeff=  127;
		}
		if(!m_h263_loop_filter )
		{
			m_use_4MV = 0;
			m_UseInter16x16Only = 1;
		}
		if(m_h263_slice_structured)
		{
			m_h263_use_gob_headers = 0;
		}

        break;
        //Note for mpeg4 & h263 the dc-scale table will be set per frame as needed later

    //default: //nothing needed default table allready set in mpegvideo.c

    }

	// note that these  loads should be done at init, as they never change
    memcpy(&WorkAreaSDM->mpeg4_dct_dequant_scaling_table[0], ff_mpeg4_default_intra_matrix, 64 *sizeof(short));
    memcpy(&WorkAreaSDM->mpeg4_dct_dequant_scaling_table[1], ff_mpeg4_default_non_intra_matrix, 64 *sizeof(short));

	// this set of data corresponds to quantisation with aan scaling in, for DC term
    memcpy(&WorkAreaSDM->mpeg4_luma_aan_dc_scaler[0], y_dc_aan_scale_table, 32*sizeof(unsigned short));
    memcpy(&WorkAreaSDM->mpeg4_chroma_aan_dc_scaler[0], c_dc_aan_scale_table, 32*sizeof(unsigned short));
	// this set of data is for quantisation of predicted DC term, where AAN scaling is already taken out
    memcpy(&WorkAreaSDM->mpeg4_luma_dc_scaler[0], y_dc_scale_table_shift20, 32*sizeof(unsigned int));
    memcpy(&WorkAreaSDM->mpeg4_chroma_dc_scaler[0], c_dc_scale_table_shift20, 32*sizeof(unsigned int));
	// this set of data is for forward quantisation of DC Prediction term
    memcpy(&WorkAreaSDM->mpeg4_fwd_luma_dc_scaler[0], y_dc_scale_table, 32*sizeof(unsigned char));
    memcpy(&WorkAreaSDM->mpeg4_fwd_chroma_dc_scaler[0], c_dc_scale_table, 32*sizeof(unsigned char));
	// this set of data is for lookup of pixel data offset and residual data offset
    memcpy(&WorkAreaSDM->mpeg4_raw_pixel_offset[0], MP4RawBlockOffset ,6*sizeof(unsigned short));
    memcpy(&WorkAreaSDM->mpeg4_diff_pixel_offset[0], MP4DiffBlockOffset ,6*sizeof(unsigned short));
	   
    memcpy(&WorkAreaSDM->mpeg4_pred_pixel_offset[0], MP4PredBlockOffset ,6*sizeof(unsigned short));

    memcpy(&WorkAreaSDM->mpeg4_final_pixel_offset[0], MP4FinalBlockOffset ,6*sizeof(unsigned short));
	memcpy(&WorkAreaSDM->MotionEstOffset, ::software_me_offset, 16*sizeof(u_int16));
	memcpy(&WorkAreaSDM->LaneLookUp, ::software_me_lanes, 8*sizeof(u_int16));

#ifndef ARC_ENCODER_API
    flush_dcache();
#endif
    
    // Split the following transfer into 4 because of the limitation of the DMA to a max of 256 bytes per linear transfer
	/*SetupDma((unsigned int)(&WorkAreaSDM->mpeg4_DC_bits_plus_256[0]), (unsigned int)uni_DCtab_lum_len2 ,128*sizeof(unsigned char));
    _vrun(MACRO_SdmDmaIn);
	SetupDma((unsigned int)(&WorkAreaSDM->mpeg4_DC_bits_plus_256[128]), (unsigned int)(&uni_DCtab_lum_len2[128]) ,128*sizeof(unsigned char));
    _vrun(MACRO_SdmDmaIn);
	SetupDma((unsigned int)(&WorkAreaSDM->mpeg4_DC_bits_plus_256[256]), (unsigned int)(&uni_DCtab_lum_len2[256]) ,128*sizeof(unsigned char));
    _vrun(MACRO_SdmDmaIn);
	SetupDma((unsigned int)(&WorkAreaSDM->mpeg4_DC_bits_plus_256[384]), (unsigned int)(&uni_DCtab_lum_len2[384]) ,128*sizeof(unsigned char));
    _vrun(MACRO_SdmDmaIn);*/
}
