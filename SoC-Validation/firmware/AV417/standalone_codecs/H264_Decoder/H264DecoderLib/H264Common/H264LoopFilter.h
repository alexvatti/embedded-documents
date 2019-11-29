/* ------------------------------------------------------------------------ */
/* FILE:			H264LoopFilter.h										*/
/* DESCRIPTION:		H.264 Loop Filter functions								*/
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

//#ifdef ENCODER_LOOP_FILTER
//#define CH264Decoder CH264Encoder
//#define CONTROL_LIST_TYPE h264_encoder_control_t
//#include	"CH264Encoder.h"
//#include	"H264EncoderControl.h"
//#endif

//#ifdef DECODER_LOOP_FILTER

//#define CLASS_NAME CH264Decoder
#define CONTROL_LIST_TYPE h264_decoder_control_t

#include	"CH264Decoder.h"
#include	"H264DecoderControl.h"
//#endif

//////////////////////////////////////////////////////////////////////////////

#include	"H264Tables.h"
#include	"H264OffsetTables.h"
#include	"BTPicture.h"

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#else
	#include <string.h>  // For memset
#endif

//#define ALPHABETAZERO_TEST   // Turns on a BT non-optimisation                                


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::LoopFilterNonMBAFFFrame							*/
/* DESCRIPTION:	Performs loop filtering on the whole Non-MB-AFF frame.
				Must be called after complete frame has been 
				encoded/decoded.
				When Q is appended to a variable, it indicates a parameter
				of the current block or macroblock
				When P is appended to a variable, it indicates a parameter
				of the neighbouring block or macroblock
*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */


// Baseline only code
const UNSIGNED_SEPTET CLIP_BETA_TABLE[] =
{
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,     // 15 zeros
          0,
          2,  2,  2,  3,  3,  3,  3,  4,  4,
          4,  6,  6,  7,  7,  8,  8,  9,  9,
         10, 10, 11, 11, 12, 12, 13, 13, 14,
         14, 15, 15, 16, 16, 17, 17, 18, 18,
         
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 

         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18, 
         18,18,18,18, 18,18,18,18, 18,18,18,18, 18,18,18,18
};


const UNSIGNED_SEPTET clip_chroma_quant_mapping_table[76] =
{
//      -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1   0   1
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
//        2   3   4   5   6   7   8   9  10  11  12  13  14  15
          2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
//       16  17  18  19  20  21  22  23  24  25  26  27  28  29
         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
//       30  31  32  33  34  35  36  37  38  37  40  41  42  43
         29, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37,
//       44  45  46  47  48  49  50  51  52  53  54  55  56  57
         37, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
//       58  59  60  61  62  63
         39, 39, 39, 39, 39, 39
};



//#include <set>
//
//std::set<int> Usedx;
//static int frNum = 0;
//int CLIP_CHROMA_MAPPED_QUANT(int x) 
//{
//	std::set<int>::const_iterator f = Usedx.find(x);
//	if(f == Usedx.end())
//	{
//		Usedx.insert(x);
//		printf("------------------   First use of %d\n",x);
//		printf("Old %d  new %d \n", h264_quantiser_chroma_mapping_table[CLIP_CHROMA_QUANT(x)],clip_chroma_quant_mapping_table[x+12]);
//	}
//	return clip_chroma_quant_mapping_table[x+12];
//}


#define CLIP_CHROMA_MAPPED_QUANT(x) clip_chroma_quant_mapping_table[x+12]




#ifdef AURORA_DEBLOCK //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Block offsets in SDM buffer (Horizonatl edge)
// ((blockNum & 3) << 2) + (blockNum & 12)*24 + 4 +24*4
const unsigned short HBlockLumaOffsets[16] = 
{
    ((0 & 3) << 2) + (0 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((1 & 3) << 2) + (1 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((2 & 3) << 2) + (2 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((3 & 3) << 2) + (3 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((4 & 3) << 2) + (4 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((5 & 3) << 2) + (5 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((6 & 3) << 2) + (6 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((7 & 3) << 2) + (7 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((8 & 3) << 2) + (8 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((9 & 3) << 2) + (9 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((10 & 3) << 2) + (10 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((11 & 3) << 2) + (11 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((12 & 3) << 2) + (12 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((13 & 3) << 2) + (13 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((14 & 3) << 2) + (14 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf,
    ((15 & 3) << 2) + (15 & 12)*24 + 4 +24*4+AuroraH264Dec::m_y_work_buf
};

// Block offsets in SDM buffer (Vertical edge)
// ((blockNum & 3) << 2) + (blockNum & 12)*24 + 8 + m_y_work_buf;
const unsigned short VBlockLumaOffsets[16] = 
{
    ((0 & 3) << 2) + (0 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((1 & 3) << 2) + (1 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((2 & 3) << 2) + (2 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((3 & 3) << 2) + (3 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((4 & 3) << 2) + (4 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((5 & 3) << 2) + (5 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((6 & 3) << 2) + (6 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((7 & 3) << 2) + (7 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((8 & 3) << 2) + (8 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((9 & 3) << 2) + (9 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((10 & 3) << 2) + (10 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((11 & 3) << 2) + (11 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((12 & 3) << 2) + (12 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((13 & 3) << 2) + (13 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((14 & 3) << 2) + (14 & 12)*24 +8+AuroraH264Dec::m_y_work_buf,
    ((15 & 3) << 2) + (15 & 12)*24 +8+AuroraH264Dec::m_y_work_buf
};



const unsigned short HBlockChromaOffsets[16] = 
{
    40+((0 & 3) << 1) + ((0 & 12) << 3),
    40+((1 & 3) << 1) + ((1 & 12) << 3),
    40+((2 & 3) << 1) + ((2 & 12) << 3),
    40+((3 & 3) << 1) + ((3 & 12) << 3),
    40+((4 & 3) << 1) + ((4 & 12) << 3),
    40+((5 & 3) << 1) + ((5 & 12) << 3),
    40+((6 & 3) << 1) + ((6 & 12) << 3),
    40+((7 & 3) << 1) + ((7 & 12) << 3),
    40+((8 & 3) << 1) + ((8 & 12) << 3),
    40+((9 & 3) << 1) + ((9 & 12) << 3),
    40+((10 & 3) << 1) + ((10 & 12) << 3),
    40+((11 & 3) << 1) + ((11 & 12) << 3),
    40+((12 & 3) << 1) + ((12 & 12) << 3),
    40+((13 & 3) << 1) + ((13 & 12) << 3),
    40+((14 & 3) << 1) + ((14 & 12) << 3),
    40+((15 & 3) << 1) + ((15 & 12) << 3)
};

inline unsigned short CalcUVOffset(int bn)
{
    return HBlockChromaOffsets[bn];
}


//#ifdef INLOOP_DBLOCK
void CH264Decoder::DeblockMB(CBTPicture * picture, h264_decoder_control_t * control)
{
	//CONTROL_LIST_TYPE const * control = GetControlTop();
	CONTROL_LIST_TYPE const * neighbouring_control;

//	printf("Frame %d\n",++frNum);


	unsigned int block_number;
	unsigned int neighbouring_block_number;
	
	const UNSIGNED_SEPTET *strengthTab;
	
	unsigned int luma_hsize = picture->m_hsize;
	unsigned int chroma_hsize = luma_hsize >> 1;
	
	UNSIGNED_SEPTET index_a_luma[3];
	UNSIGNED_SEPTET b_luma[3];
	UNSIGNED_SEPTET index_a_chroma[3];
	UNSIGNED_SEPTET b_chroma[3];
	
	
	
	//Simdu->SetupDeblockDMA(picture->m_y, picture->m_u, picture->m_v, picture->m_hsize, picture->m_vsize);
	
	
	
    //while (0 != control)
    //{
        if (1 != control->disable_deblocking_filter_idc)
		{
            // Deblocking for this macroblock
    		char mbFlags = control->Flags;
            //         FlushDCache();
            //Simdu->DMA_inDeblock(control->PixelX, control->PixelY);
            
            // Calculate the luma and chroma a and b indices for centre, left and top edges
            
            // Centre edges, depends only on this macroblock's quant
            {
                UNSIGNED_SEPTET average_chroma_quant 
                    = CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset);
                
                index_a_luma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(control->quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[control->quant + control->slice_beta_offset ];
                
                index_a_chroma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Left edges, depends on this macroblock's and the left macroblock's quant
            
            if (mbFlags & CTLF_Left)
            {
            	neighbouring_control = control->GetLeft();
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Top edges, depends on this macroblock's and the above macroblock's quant
            
            
            if (mbFlags & CTLF_Above)
            {
            	neighbouring_control = control->GetAbove();
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
                    
            }
            
   /////////////////////////////////////////////////////////////////////////
   /////////// Horizontal filtering ////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
            bool qskp;
            
            qskp = ((mbFlags & CTLF_Left ) == 0)
                ||( 2 == control->disable_deblocking_filter_idc 
                && (mbFlags & CTLF_LeftInSlice ) == 0);
            
            strengthTab = CLIP_TABLE[2]; //  = 3;
            unsigned int edgeOffset;
            int index_a;
            
            int alpha;
            int c0;
// Left edge intra case
            // Logic for intra case is not as complicated
            if(! qskp && ( (mbFlags & CTLF_IsIntraCoded) || (control->GetLeft()->Flags & CTLF_IsIntraCoded )))
            {
                
                // Luma
                edgeOffset = 4 +24*4 + AuroraH264Dec::m_y_work_buf;
                index_a = index_a_luma[H264_LEFT_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
                c0 = CLIP_TABLE[3-1][index_a];
                
                _vr00 = _vmovw(edgeOffset-96,2);
                _vr01 = _vmovw(c0, 0x1);
                _vr01 = _vmovw(1, 0x2);         // Strong filter
                _vr01 = _vmovw(b_luma[H264_LEFT_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilter);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
                // Chroma
        
                // Two iterations of filtering
                _vr01 = _vmovw(ALPHA_TABLE[index_a_chroma[H264_LEFT_EDGE]], 0xff);
                _vr02 = _vmovw(b_chroma[H264_LEFT_EDGE], 0xff);
                
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaHorizontalStrong);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+32+8-4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+32+8-4, 0x20);     // i5
                _vrun(macro);
                _vrun(macro);
                                
                qskp = true; // Edge done     
            }
            
// Centre edges intra cases
            if(mbFlags & CTLF_IsIntraCoded)
            {
                index_a = index_a_luma[H264_CENTRE_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
                c0 = CLIP_TABLE[3-1][index_a];
                _vr01 = _vmovw(c0, 0x1);
                _vr01 = _vmovw(0, 0x2);         // Strong filter off
                _vr01 = _vmovw(b_luma[H264_CENTRE_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                
                edgeOffset = 4 +24*4 + AuroraH264Dec::m_y_work_buf;
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilter);

                int macroA = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilterAligned);
                edgeOffset += 4-96;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                
                edgeOffset += 4;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
                edgeOffset += 4;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);                
           
                index_a = index_a_chroma[H264_CENTRE_EDGE];
                alpha = ALPHA_TABLE[index_a];
                int c = CLIP_TABLE[3-1][index_a] + 1;
        
                _vr01 = _vmovw(alpha, 0xff);
                _vr02 = _vmovw(b_chroma[H264_CENTRE_EDGE], 0xff);
                _vr20 = _vmovw(c, 0xff);
                
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+32+8+4-4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+32+8+4-4, 0x20);     // i5
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaHorizontal);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
            }
            else
            {
// Edges not intra
                int coefMask = control->y_coeffs_present;
                coefMask |= (coefMask & 0x7777) << 1;
                if(mbFlags & CTLF_Left)
                {
                    coefMask |= (control->GetLeft()->y_coeffs_present & 0x8888) >> 3; 
                }

                
                // First column of blocks
                if( ! qskp )
                {
                    neighbouring_control = control->GetLeft();
                    for (block_number=0; block_number<16; block_number+= 4)
                    {
                        // Calculate Strength
                        // Check whether macroblock or left macroblock is intra coded
                        if( coefMask & (1 << block_number) )
                        {
                            // Luma block or neighbouring block has coefficients
                            strengthTab = CLIP_TABLE[1]; //   = 2;
                        }
                        else
                        {
                            neighbouring_block_number = block_number + 3;
                            if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[neighbouring_block_number]) ||
                                (ABS(control->MotionVecs[block_number].Comp.Hor - neighbouring_control->MotionVecs[neighbouring_block_number].Comp.Hor) >= 4) ||
                                (ABS(control->MotionVecs[block_number].Comp.Ver - neighbouring_control->MotionVecs[neighbouring_block_number].Comp.Ver) >= 4) )
                            {
                                strengthTab = CLIP_TABLE[0]; //   = 1;
                            }
                            else
                            {
                                // Nothing to do so skip to next block
                                continue;
                            }
                        }
                         // Do horizontal filtering
                        // y edge
    #ifdef ALPHABETAZERO_TEST                                
                        if ((index_a_luma[H264_LEFT_EDGE] > 0) || (b_luma[H264_LEFT_EDGE] > 0))
    #endif                                        
                        {
                            Simdu->LoopFilterEdgeLumaHorzontal(block_number, strengthTab, 
                                index_a_luma[H264_LEFT_EDGE],
                                b_luma[H264_LEFT_EDGE]);
                        }
                        
                        if (((block_number & 1) == 0)
    #ifdef ALPHABETAZERO_TEST                                
                            &&  ((index_a_chroma[H264_LEFT_EDGE] > 0) || (b_chroma[H264_LEFT_EDGE] > 0))
    #endif                                        
                            )
                        {
                            // Filter every second edge
                            int ofs = CalcUVOffset(block_number);
                            int idxA = index_a_chroma[H264_LEFT_EDGE];
                            int beta = b_chroma[H264_LEFT_EDGE];
                            
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxA, beta);
                        }                        
                    }       // END OF: for (block_number
                }
                
                {
                    UNSIGNED_SEPTET ial = index_a_luma[H264_CENTRE_EDGE];
                    UNSIGNED_SEPTET bl = b_luma[H264_CENTRE_EDGE];
                    int idxAC = index_a_chroma[H264_CENTRE_EDGE];
                    int betaC = b_chroma[H264_CENTRE_EDGE];
                    
                    // Loop over remaining blocks in raster scan
                    for (block_number= 1; block_number<16; block_number++)
                    {
                        ////////////////////////////////////////////////////////////////
                        // Determine Horizontal Neighbouring Block
                        ////////////////////////////////////////////////////////////////
                        
                        if ((block_number & 3) == 0)
                        {
                            continue;
                        }
                        // Neighbouring block is in current macroblock 
                        // Calculate Strength										   /
                        if( coefMask & (1 << block_number) )
                        {
                            // Luma block or neighbouring block has coefficients
                            strengthTab = CLIP_TABLE[1]; //   = 2;
                        }
                        else
                        {
                            if (  (control->mv_for_array_position[block_number] != control->mv_for_array_position[block_number - 1]) ||
                                (ABS(control->MotionVecs[block_number].Comp.Hor - control->MotionVecs[block_number - 1].Comp.Hor) >= 4) ||
                                (ABS(control->MotionVecs[block_number].Comp.Ver - control->MotionVecs[block_number - 1].Comp.Ver) >= 4) )
                            {
                                strengthTab = CLIP_TABLE[0]; //   = 1;
                            }
                            else
                            {
                                // Nothing to do so skip to next block
                                continue;
                            }
                            
                            
                        }
                         // Do horizontal filtering
                         
                        // y edge
    #ifdef ALPHABETAZERO_TEST                                
                        if (ial > 0) || (bl > 0))
    #endif                                        
                        {
                            Simdu->LoopFilterEdgeLumaHorzontal(block_number, strengthTab, ial, bl);
                        }
                        
                        if (((block_number & 1) == 0)
    #ifdef ALPHABETAZERO_TEST                                
                            &&  ((index_a_chroma[H264_CENTRE_EDGE] > 0) || (b_chroma[H264_CENTRE_EDGE] > 0))
    #endif                                        
                            )
                        {
                            // Filter every second edge
                            int ofs = CalcUVOffset(block_number);
                            
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxAC, betaC);
                        }
                        
                    }	// END OF: for (block_number
                }
            }	// END OF: else(control->block_coded_intra)


            
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Vertical   filtering ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


            
            qskp = (mbFlags & CTLF_Above) == 0
                || (2 == control->disable_deblocking_filter_idc
                && (mbFlags & CTLF_AboveInSlice) == 0);

            strengthTab = CLIP_TABLE[2]; //   = 3;
// Top edge intra case
            // Logic for intra case is not as complicated
            // Top line of blocks
            if( !qskp && ( (mbFlags & CTLF_IsIntraCoded) ||(control->GetAbove()->Flags & CTLF_IsIntraCoded )))
            {
                
                // Luma
                edgeOffset = 8 + AuroraH264Dec::m_y_work_buf;
                index_a = index_a_luma[H264_TOP_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
//                c0 = CLIP_TABLE[strength-1][index_a];               

                _vr00 = _vmovw(edgeOffset,2);
//                _vr01 = _vmovw(1, 0x1);			// Safe c0
                _vr01 = _vmovw(1, 0x2);         // Strong filter
                _vr01 = _vmovw(b_luma[H264_TOP_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_VerticalFilter2);
                _vrun(macro);
                _vrun(macro);

                
                // Chroma
        
                // Two iterations of filtering
                _vr01 = _vmovw(ALPHA_TABLE[index_a_chroma[H264_TOP_EDGE]], 0xff);
                _vr02 = _vmovw(b_chroma[H264_TOP_EDGE], 0xff);

                edgeOffset = CalcUVOffset(0);

                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+8, 0x20);     // i5
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVerticalStrong);
                _vrun(macro);
                
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+8, 0x20);     // i5
                _vrun(macro);

                qskp = true; // Edge done     
            }

            // Other 3 lines of blocks intra
            if( mbFlags & CTLF_IsIntraCoded)
            {
                strengthTab = CLIP_TABLE[2]; //   = 3;                    
                    // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                if ((index_a_luma[H264_CENTRE_EDGE] > 0) || (b_luma[H264_CENTRE_EDGE] > 0))
#endif                                        
                {
                    int idxa = index_a_luma[H264_CENTRE_EDGE];
                    alpha = ALPHA_TABLE[idxa];
                    int c0 = CLIP_TABLE[3-1][idxa];
                
                    int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_NormalVerticalFilter);
                    _vr01 = _vmovzw( alpha, 0xff);//     alpha 
                    _vr02 = _vmovzw( b_luma[H264_CENTRE_EDGE], 0xff);//     beta 
                    _vr03 = _vmovzw( c0, 0xff    )    ;//     c0 
                       
                    _vr00 = _vmovzw(8 + 24*4 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*4 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    
                    _vr00 = _vmovzw(8 + 24*8 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*8 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    
                    _vr00 = _vmovzw(8 + 24*12 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*12 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);

                }
                // Internal chroma edge
                const int ofs = CalcUVOffset(8)-32;
                int idxA = index_a_chroma[H264_CENTRE_EDGE];
                // Two iterations of filtering
        
                _vr01 = _vmovzw(ALPHA_TABLE[idxA], 0xff);               // Alpha
                _vr02 = _vmovzw(b_chroma[H264_CENTRE_EDGE], 0xff);      // Beta
                _vr20 = _vmovzw(CLIP_TABLE[3-1][idxA] + 1, 0xff);    // C
                _vr22 = _vmovzw(-1, 0xff);      // Lane enable mask
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs, 0x20);     // i5
                _vrun(macro);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs+4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs+4, 0x20);     // i5
                _vrun(macro);
            }
            else
            {
                int coefMask = control->y_coeffs_present;
                coefMask |= coefMask << 4;
                if(mbFlags & CTLF_Above)
                {
                    coefMask |= control->GetAbove()->y_coeffs_present >> 12; 
                }
                

                // Loop over blocks in top line of blocks
                const UNSIGNED_SEPTET *strengthTab2;
                neighbouring_control = control->GetAbove();
                for (block_number =( qskp<< 2); block_number<4; block_number += 2)
                {
                    // Calculate Strength

                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << block_number) )
                    {
                        strengthTab = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[block_number + 12]) ||
                            (ABS(control->MotionVecs[block_number].Comp.Hor - neighbouring_control->MotionVecs[block_number + 12].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[block_number].Comp.Ver - neighbouring_control->MotionVecs[block_number + 12].Comp.Ver) >= 4) )
                        {
                            strengthTab = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab = NULL;
                        }
                    }
                    
                    const int bn2 = block_number+1;
                    if ( coefMask & (1 << bn2) )
                    {
                        strengthTab2 = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[bn2] != neighbouring_control->mv_for_array_position[bn2 + 12]) ||
                            (ABS(control->MotionVecs[bn2].Comp.Hor - neighbouring_control->MotionVecs[bn2 + 12].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[bn2].Comp.Ver - neighbouring_control->MotionVecs[bn2 + 12].Comp.Ver) >= 4) )
                        {
                            strengthTab2 = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab2 = NULL;
                        }
                    }
                    
                    if(strengthTab == NULL && strengthTab2 == NULL)
                    {
                        continue;
                    }
                    
                    // Vertical Filtering
                     // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[H264_TOP_EDGE] > 0) || (b_luma[H264_TOP_EDGE] > 0))
#endif                                        
                    {
                        const unsigned short edgeOffset = VBlockLumaOffsets[block_number];
                    
                        int alpha = ALPHA_TABLE[index_a_luma[H264_TOP_EDGE]];
                        _vr00 = _vmovw(edgeOffset, 2);
                        _vr01 = _vmovzw( 0, 0xff);//     alpha
                    
                        if(strengthTab)
                        {
                            _vr01 = _vmovw( alpha, 0x0f);//     alpha low x 4
                            _vr03 = _vmovw( strengthTab[index_a_luma[H264_TOP_EDGE]], 0x0f )    ;//     c0 low x 4
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw( alpha, 0xf0);//     alpha hi x 4
                            _vr03 = _vmovw( strengthTab2[index_a_luma[H264_TOP_EDGE]], 0xf0 )    ;//     c0 hi x 4
                        }

                        _vr02 = _vmovw( b_luma[H264_TOP_EDGE], 0xff);//     beta x8
                           
                        Simdu->RecMgr.Run(AM_DB_NormalVerticalFilter);
                    }
                    
#ifdef ALPHABETAZERO_TEST                                
                     if ( ((index_a_chroma[H264_TOP_EDGE] > 0) || (b_chroma[H264_TOP_EDGE] > 0)))
#endif       
                    {
                        // Filter every second edge
                        int ofs = CalcUVOffset(block_number);
                        int idxAC = index_a_chroma[H264_TOP_EDGE];
                        int betaC = b_chroma[H264_TOP_EDGE];
                        // u,v edge
//                        Simdu->LoopFilterEdgeChromaVertical(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxA, beta);
                        unsigned char alpha = ALPHA_TABLE[idxAC];
                        // Two iterations of filtering
                
                        _vr01 = _vmovzw(0, 0xff);
                        _vr02 = _vmovzw(betaC, 0xff);
                
                        if(strengthTab)
                        {
                            _vr01 = _vmovw(alpha, 0x33);
                            _vr20 = _vmovw(strengthTab[idxAC] + 1, 0x33);
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw(alpha, 0xcc);
                            _vr20 = _vmovw(strengthTab2[idxAC] + 1, 0xcc);
                        }
                        int macroc = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                        _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs-32, 0x10);     // i4
                        _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs-32, 0x20);     // i5
                        _vrun(macroc);
                    }
                }       // END OF: for (block_number

               // Loop over blocks in remaining 3 lines
               
                for (block_number = 4; block_number<16; block_number += 2)
                {
                    ////////////////////////////////////////////////////////////////
                    // Calculate Strength  1                                                                            /
                    ////////////////////////////////////////////////////////////////
                    
                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << block_number) )
                    {
                        strengthTab = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                    // Neighbouring block is in current macroblock 
                     //   neighbouring_block_number = block_number - 4;
                        if (  (control->mv_for_array_position[block_number] != control->mv_for_array_position[block_number - 4]) ||
                            (ABS(control->MotionVecs[block_number].Comp.Hor - control->MotionVecs[block_number - 4].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[block_number].Comp.Ver - control->MotionVecs[block_number - 4].Comp.Ver) >= 4) )
                        {
                            strengthTab = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab = NULL;
                        }
                    }
                   ////////////////////////////////////////////////////////////////
                    // Calculate Strength   2                                                                           /
                    ////////////////////////////////////////////////////////////////
                    const int bn2 = block_number+1;
                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << bn2) )
                    {
                        strengthTab2 = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                    // Neighbouring block is in current macroblock 
                     //   neighbouring_block_number = block_number - 4;
                        if (  (control->mv_for_array_position[bn2] != control->mv_for_array_position[bn2 - 4]) ||
                            (ABS(control->MotionVecs[bn2].Comp.Hor - control->MotionVecs[bn2 - 4].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[bn2].Comp.Ver - control->MotionVecs[bn2 - 4].Comp.Ver) >= 4) )
                        {
                            strengthTab2 = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab2 = NULL;
                        }
                    }
                    
                    if(strengthTab == NULL && strengthTab2 == NULL)
                    {
                        continue;
                    }
                    // Vertical Filtering
                     // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[H264_CENTRE_EDGE] > 0) || (b_luma[H264_CENTRE_EDGE] > 0))
#endif                                        
                    {
                        const unsigned short edgeOffset = VBlockLumaOffsets[block_number];
                    
                        int alpha = ALPHA_TABLE[index_a_luma[H264_CENTRE_EDGE]];
                        _vr00 = _vmovw(edgeOffset, 2);
                        _vr01 = _vmovzw( 0, 0xff);//     alpha
                    
                        if(strengthTab)
                        {
                            _vr01 = _vmovw( alpha, 0x0f);//     alpha low x 4
                            _vr03 = _vmovw( strengthTab[index_a_luma[H264_CENTRE_EDGE]], 0x0f )    ;//     c0 low x 4
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw( alpha, 0xf0);//     alpha hi x 4
                            _vr03 = _vmovw( strengthTab2[index_a_luma[H264_CENTRE_EDGE]], 0xf0 )    ;//     c0 hi x 4
                        }

                        _vr02 = _vmovw( b_luma[H264_CENTRE_EDGE], 0xff);//     beta x8
                           
                        Simdu->RecMgr.Run(AM_DB_NormalVerticalFilter);
                    }
                    
                    if (((block_number & 4) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        && ((index_a_chroma[H264_CENTRE_EDGE] > 0) || (b_chroma[H264_CENTRE_EDGE] > 0))
#endif
                        )
                    {
                        // Filter every second edge
                        int ofs = CalcUVOffset(block_number);
                        int idxAC = index_a_chroma[H264_CENTRE_EDGE];
                        int betaC = b_chroma[H264_CENTRE_EDGE];
                         // u,v edge
                        unsigned char alpha = ALPHA_TABLE[idxAC];
                        // Two iterations of filtering
                
                        _vr01 = _vmovzw(0, 0xff);
                        _vr02 = _vmovzw(betaC, 0xff);
                
                        if(strengthTab)
                        {
                            _vr01 = _vmovw(alpha, 0x33);
                            _vr20 = _vmovw(strengthTab[idxAC] + 1, 0x33);
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw(alpha, 0xcc);
                            _vr20 = _vmovw(strengthTab2[idxAC] + 1, 0xcc);
                        }
                        int macroc = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                        _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs-32, 0x10);     // i4
                        _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs-32, 0x20);     // i5
                        _vrun(macroc);
                    }
                }       // END OF: for (block_number
                
                
            }   // END OF: else ( control->block_coded_intra)


            //      FlushDCache();
            //Simdu->DMA_outDeblock(control->PixelX, control->PixelY);
            
            
        }  // END OF: (1 != control->disable_deblocking_filter_idc)
                
        //control = control->next;
    //}
}
//#else
//#if 0
void CH264Decoder::LoopFilterNonMBAFFFrame(CBTPicture * picture)
{
	CONTROL_LIST_TYPE const * control = GetControlTop();
	CONTROL_LIST_TYPE const * neighbouring_control;

//	printf("Frame %d\n",++frNum);


	unsigned int block_number;
	unsigned int neighbouring_block_number;
	
	const UNSIGNED_SEPTET *strengthTab;
	
	unsigned int luma_hsize = picture->m_hsize;
	unsigned int chroma_hsize = luma_hsize >> 1;
	
	UNSIGNED_SEPTET index_a_luma[3];
	UNSIGNED_SEPTET b_luma[3];
	UNSIGNED_SEPTET index_a_chroma[3];
	UNSIGNED_SEPTET b_chroma[3];
	
	
	
	Simdu->SetupDeblockDMA(picture->m_y, picture->m_u, picture->m_v, picture->m_hsize, picture->m_vsize);
	
	
	
    while (0 != control)
    {
        if (1 != control->disable_deblocking_filter_idc)
        {
            // Deblocking for this macroblock
    		char mbFlags = control->Flags;
            //         FlushDCache();
            Simdu->DMA_inDeblock(control->PixelX, control->PixelY);
            
            // Calculate the luma and chroma a and b indices for centre, left and top edges
            
            // Centre edges, depends only on this macroblock's quant
            {
                UNSIGNED_SEPTET average_chroma_quant 
                    = CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset);
                
                index_a_luma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(control->quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[control->quant + control->slice_beta_offset ];
                
                index_a_chroma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Left edges, depends on this macroblock's and the left macroblock's quant
            
            if (mbFlags & CTLF_Left)
            {
            	neighbouring_control = control->GetLeft();
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Top edges, depends on this macroblock's and the above macroblock's quant
            
            
            if (mbFlags & CTLF_Above)
            {
            	neighbouring_control = control->GetAbove();
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
                    
            }
            
   /////////////////////////////////////////////////////////////////////////
   /////////// Horizontal filtering ////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
            bool qskp;
            
            qskp = ((mbFlags & CTLF_Left ) == 0)
                ||( 2 == control->disable_deblocking_filter_idc 
                && (mbFlags & CTLF_LeftInSlice ) == 0);
            
            strengthTab = CLIP_TABLE[2]; //  = 3;
            unsigned int edgeOffset;
            int index_a;
            
            int alpha;
            int c0;
// Left edge intra case
            // Logic for intra case is not as complicated
            if(! qskp && ( (mbFlags & CTLF_IsIntraCoded) || (control->GetLeft()->Flags & CTLF_IsIntraCoded )))
            {
                
                // Luma
                edgeOffset = 4 +24*4 + AuroraH264Dec::m_y_work_buf;
                index_a = index_a_luma[H264_LEFT_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
                c0 = CLIP_TABLE[3-1][index_a];
                
                _vr00 = _vmovw(edgeOffset-96,2);
                _vr01 = _vmovw(c0, 0x1);
                _vr01 = _vmovw(1, 0x2);         // Strong filter
                _vr01 = _vmovw(b_luma[H264_LEFT_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilter);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
                // Chroma
        
                // Two iterations of filtering
                _vr01 = _vmovw(ALPHA_TABLE[index_a_chroma[H264_LEFT_EDGE]], 0xff);
                _vr02 = _vmovw(b_chroma[H264_LEFT_EDGE], 0xff);
                
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaHorizontalStrong);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+32+8-4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+32+8-4, 0x20);     // i5
                _vrun(macro);
                _vrun(macro);
                                
                qskp = true; // Edge done     
            }
            
// Centre edges intra cases
            if(mbFlags & CTLF_IsIntraCoded)
            {
                index_a = index_a_luma[H264_CENTRE_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
                c0 = CLIP_TABLE[3-1][index_a];
                _vr01 = _vmovw(c0, 0x1);
                _vr01 = _vmovw(0, 0x2);         // Strong filter off
                _vr01 = _vmovw(b_luma[H264_CENTRE_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                
                edgeOffset = 4 +24*4 + AuroraH264Dec::m_y_work_buf;
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilter);

                int macroA = Simdu->RecMgr.GetMacroAddr(AM_DB_HorzontalFilterAligned);
                edgeOffset += 4-96;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                
                edgeOffset += 4;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
                edgeOffset += 4;
                _vr00 = _vmovw(edgeOffset,2);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);
                _vrun(macroA);                
           
                index_a = index_a_chroma[H264_CENTRE_EDGE];
                alpha = ALPHA_TABLE[index_a];
                int c = CLIP_TABLE[3-1][index_a] + 1;
        
                _vr01 = _vmovw(alpha, 0xff);
                _vr02 = _vmovw(b_chroma[H264_CENTRE_EDGE], 0xff);
                _vr20 = _vmovw(c, 0xff);
                
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+32+8+4-4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+32+8+4-4, 0x20);     // i5
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaHorizontal);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                _vrun(macro);
                
            }
            else
            {
// Edges not intra
                int coefMask = control->y_coeffs_present;
                coefMask |= (coefMask & 0x7777) << 1;
                if(mbFlags & CTLF_Left)
                {
                    coefMask |= (control->GetLeft()->y_coeffs_present & 0x8888) >> 3; 
                }

                
                // First column of blocks
                if( ! qskp )
                {
                    neighbouring_control = control->GetLeft();
                    for (block_number=0; block_number<16; block_number+= 4)
                    {
                        // Calculate Strength
                        // Check whether macroblock or left macroblock is intra coded
                        if( coefMask & (1 << block_number) )
                        {
                            // Luma block or neighbouring block has coefficients
                            strengthTab = CLIP_TABLE[1]; //   = 2;
                        }
                        else
                        {
                            neighbouring_block_number = block_number + 3;
                            if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[neighbouring_block_number]) ||
                                (ABS(control->MotionVecs[block_number].Comp.Hor - neighbouring_control->MotionVecs[neighbouring_block_number].Comp.Hor) >= 4) ||
                                (ABS(control->MotionVecs[block_number].Comp.Ver - neighbouring_control->MotionVecs[neighbouring_block_number].Comp.Ver) >= 4) )
                            {
                                strengthTab = CLIP_TABLE[0]; //   = 1;
                            }
                            else
                            {
                                // Nothing to do so skip to next block
                                continue;
                            }
                        }
                         // Do horizontal filtering
                        // y edge
    #ifdef ALPHABETAZERO_TEST                                
                        if ((index_a_luma[H264_LEFT_EDGE] > 0) || (b_luma[H264_LEFT_EDGE] > 0))
    #endif                                        
                        {
                            Simdu->LoopFilterEdgeLumaHorzontal(block_number, strengthTab, 
                                index_a_luma[H264_LEFT_EDGE],
                                b_luma[H264_LEFT_EDGE]);
                        }
                        
                        if (((block_number & 1) == 0)
    #ifdef ALPHABETAZERO_TEST                                
                            &&  ((index_a_chroma[H264_LEFT_EDGE] > 0) || (b_chroma[H264_LEFT_EDGE] > 0))
    #endif                                        
                            )
                        {
                            // Filter every second edge
                            int ofs = CalcUVOffset(block_number);
                            int idxA = index_a_chroma[H264_LEFT_EDGE];
                            int beta = b_chroma[H264_LEFT_EDGE];
                            
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxA, beta);
                        }                        
                    }       // END OF: for (block_number
                }
                
                {
                    UNSIGNED_SEPTET ial = index_a_luma[H264_CENTRE_EDGE];
                    UNSIGNED_SEPTET bl = b_luma[H264_CENTRE_EDGE];
                    int idxAC = index_a_chroma[H264_CENTRE_EDGE];
                    int betaC = b_chroma[H264_CENTRE_EDGE];
                    
                    // Loop over remaining blocks in raster scan
                    for (block_number= 1; block_number<16; block_number++)
                    {
                        ////////////////////////////////////////////////////////////////
                        // Determine Horizontal Neighbouring Block
                        ////////////////////////////////////////////////////////////////
                        
                        if ((block_number & 3) == 0)
                        {
                            continue;
                        }
                        // Neighbouring block is in current macroblock 
                        // Calculate Strength										   /
                        if( coefMask & (1 << block_number) )
                        {
                            // Luma block or neighbouring block has coefficients
                            strengthTab = CLIP_TABLE[1]; //   = 2;
                        }
                        else
                        {
                            if (  (control->mv_for_array_position[block_number] != control->mv_for_array_position[block_number - 1]) ||
                                (ABS(control->MotionVecs[block_number].Comp.Hor - control->MotionVecs[block_number - 1].Comp.Hor) >= 4) ||
                                (ABS(control->MotionVecs[block_number].Comp.Ver - control->MotionVecs[block_number - 1].Comp.Ver) >= 4) )
                            {
                                strengthTab = CLIP_TABLE[0]; //   = 1;
                            }
                            else
                            {
                                // Nothing to do so skip to next block
                                continue;
                            }
                            
                            
                        }
                         // Do horizontal filtering
                         
                        // y edge
    #ifdef ALPHABETAZERO_TEST                                
                        if (ial > 0) || (bl > 0))
    #endif                                        
                        {
                            Simdu->LoopFilterEdgeLumaHorzontal(block_number, strengthTab, ial, bl);
                        }
                        
                        if (((block_number & 1) == 0)
    #ifdef ALPHABETAZERO_TEST                                
                            &&  ((index_a_chroma[H264_CENTRE_EDGE] > 0) || (b_chroma[H264_CENTRE_EDGE] > 0))
    #endif                                        
                            )
                        {
                            // Filter every second edge
                            int ofs = CalcUVOffset(block_number);
                            
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxAC, betaC);
                        }
                        
                    }	// END OF: for (block_number
                }
            }	// END OF: else(control->block_coded_intra)


            
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Vertical   filtering ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


            
            qskp = (mbFlags & CTLF_Above) == 0
                || (2 == control->disable_deblocking_filter_idc
                && (mbFlags & CTLF_AboveInSlice) == 0);

            strengthTab = CLIP_TABLE[2]; //   = 3;
// Top edge intra case
            // Logic for intra case is not as complicated
            // Top line of blocks
            if( !qskp && ( (mbFlags & CTLF_IsIntraCoded) ||(control->GetAbove()->Flags & CTLF_IsIntraCoded )))
            {
                
                // Luma
                edgeOffset = 8 + AuroraH264Dec::m_y_work_buf;
                index_a = index_a_luma[H264_TOP_EDGE];
                
                alpha = ALPHA_TABLE[index_a];
//                c0 = CLIP_TABLE[strength-1][index_a];               

                _vr00 = _vmovw(edgeOffset,2);
//                _vr01 = _vmovw(1, 0x1);			// Safe c0
                _vr01 = _vmovw(1, 0x2);         // Strong filter
                _vr01 = _vmovw(b_luma[H264_TOP_EDGE], 0x4);
                _vr01 = _vmovw(alpha, 0x8);
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_VerticalFilter2);
                _vrun(macro);
                _vrun(macro);

                
                // Chroma
        
                // Two iterations of filtering
                _vr01 = _vmovw(ALPHA_TABLE[index_a_chroma[H264_TOP_EDGE]], 0xff);
                _vr02 = _vmovw(b_chroma[H264_TOP_EDGE], 0xff);

                edgeOffset = CalcUVOffset(0);

                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+8, 0x20);     // i5
                macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVerticalStrong);
                _vrun(macro);
                
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+8, 0x20);     // i5
                _vrun(macro);

                qskp = true; // Edge done     
            }

            // Other 3 lines of blocks intra
            if( mbFlags & CTLF_IsIntraCoded)
            {
                strengthTab = CLIP_TABLE[2]; //   = 3;                    
                    // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                if ((index_a_luma[H264_CENTRE_EDGE] > 0) || (b_luma[H264_CENTRE_EDGE] > 0))
#endif                                        
                {
                    int idxa = index_a_luma[H264_CENTRE_EDGE];
                    alpha = ALPHA_TABLE[idxa];
                    int c0 = CLIP_TABLE[3-1][idxa];
                
                    int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_NormalVerticalFilter);
                    _vr01 = _vmovzw( alpha, 0xff);//     alpha 
                    _vr02 = _vmovzw( b_luma[H264_CENTRE_EDGE], 0xff);//     beta 
                    _vr03 = _vmovzw( c0, 0xff    )    ;//     c0 
                       
                    _vr00 = _vmovzw(8 + 24*4 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*4 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    
                    _vr00 = _vmovzw(8 + 24*8 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*8 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    
                    _vr00 = _vmovzw(8 + 24*12 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);
                    _vr00 = _vmovzw(16 + 24*12 + AuroraH264Dec::m_y_work_buf, 2);
                    _vrun(macro);

                }
                // Internal chroma edge
                const int ofs = CalcUVOffset(8)-32;
                int idxA = index_a_chroma[H264_CENTRE_EDGE];
                // Two iterations of filtering
        
                _vr01 = _vmovzw(ALPHA_TABLE[idxA], 0xff);               // Alpha
                _vr02 = _vmovzw(b_chroma[H264_CENTRE_EDGE], 0xff);      // Beta
                _vr20 = _vmovzw(CLIP_TABLE[3-1][idxA] + 1, 0xff);    // C
                _vr22 = _vmovzw(-1, 0xff);      // Lane enable mask
                int macro = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs, 0x20);     // i5
                _vrun(macro);
                _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs+4, 0x10);     // i4
                _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs+4, 0x20);     // i5
                _vrun(macro);
            }
            else
            {
                int coefMask = control->y_coeffs_present;
                coefMask |= coefMask << 4;
                if(mbFlags & CTLF_Above)
                {
                    coefMask |= control->GetAbove()->y_coeffs_present >> 12; 
                }
                

                // Loop over blocks in top line of blocks
                const UNSIGNED_SEPTET *strengthTab2;
                neighbouring_control = control->GetAbove();
                for (block_number =( qskp<< 2); block_number<4; block_number += 2)
                {
                    // Calculate Strength

                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << block_number) )
                    {
                        strengthTab = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[block_number + 12]) ||
                            (ABS(control->MotionVecs[block_number].Comp.Hor - neighbouring_control->MotionVecs[block_number + 12].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[block_number].Comp.Ver - neighbouring_control->MotionVecs[block_number + 12].Comp.Ver) >= 4) )
                        {
                            strengthTab = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab = NULL;
                        }
                    }
                    
                    const int bn2 = block_number+1;
                    if ( coefMask & (1 << bn2) )
                    {
                        strengthTab2 = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[bn2] != neighbouring_control->mv_for_array_position[bn2 + 12]) ||
                            (ABS(control->MotionVecs[bn2].Comp.Hor - neighbouring_control->MotionVecs[bn2 + 12].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[bn2].Comp.Ver - neighbouring_control->MotionVecs[bn2 + 12].Comp.Ver) >= 4) )
                        {
                            strengthTab2 = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab2 = NULL;
                        }
                    }
                    
                    if(strengthTab == NULL && strengthTab2 == NULL)
                    {
                        continue;
                    }
                    
                    // Vertical Filtering
                     // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[H264_TOP_EDGE] > 0) || (b_luma[H264_TOP_EDGE] > 0))
#endif                                        
                    {
                        const unsigned short edgeOffset = VBlockLumaOffsets[block_number];
                    
                        int alpha = ALPHA_TABLE[index_a_luma[H264_TOP_EDGE]];
                        _vr00 = _vmovw(edgeOffset, 2);
                        _vr01 = _vmovzw( 0, 0xff);//     alpha
                    
                        if(strengthTab)
                        {
                            _vr01 = _vmovw( alpha, 0x0f);//     alpha low x 4
                            _vr03 = _vmovw( strengthTab[index_a_luma[H264_TOP_EDGE]], 0x0f )    ;//     c0 low x 4
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw( alpha, 0xf0);//     alpha hi x 4
                            _vr03 = _vmovw( strengthTab2[index_a_luma[H264_TOP_EDGE]], 0xf0 )    ;//     c0 hi x 4
                        }

                        _vr02 = _vmovw( b_luma[H264_TOP_EDGE], 0xff);//     beta x8
                           
                        Simdu->RecMgr.Run(AM_DB_NormalVerticalFilter);
                    }
                    
#ifdef ALPHABETAZERO_TEST                                
                     if ( ((index_a_chroma[H264_TOP_EDGE] > 0) || (b_chroma[H264_TOP_EDGE] > 0)))
#endif       
                    {
                        // Filter every second edge
                        int ofs = CalcUVOffset(block_number);
                        int idxAC = index_a_chroma[H264_TOP_EDGE];
                        int betaC = b_chroma[H264_TOP_EDGE];
                        // u,v edge
//                        Simdu->LoopFilterEdgeChromaVertical(AuroraH264Dec::m_u_work_buf+ofs, AuroraH264Dec::m_v_work_buf+ofs, strengthTab, idxA, beta);
                        unsigned char alpha = ALPHA_TABLE[idxAC];
                        // Two iterations of filtering
                
                        _vr01 = _vmovzw(0, 0xff);
                        _vr02 = _vmovzw(betaC, 0xff);
                
                        if(strengthTab)
                        {
                            _vr01 = _vmovw(alpha, 0x33);
                            _vr20 = _vmovw(strengthTab[idxAC] + 1, 0x33);
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw(alpha, 0xcc);
                            _vr20 = _vmovw(strengthTab2[idxAC] + 1, 0xcc);
                        }
                        int macroc = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                        _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs-32, 0x10);     // i4
                        _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs-32, 0x20);     // i5
                        _vrun(macroc);
                    }
                }       // END OF: for (block_number

               // Loop over blocks in remaining 3 lines
               
                for (block_number = 4; block_number<16; block_number += 2)
                {
                    ////////////////////////////////////////////////////////////////
                    // Calculate Strength  1                                                                            /
                    ////////////////////////////////////////////////////////////////
                    
                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << block_number) )
                    {
                        strengthTab = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                    // Neighbouring block is in current macroblock 
                     //   neighbouring_block_number = block_number - 4;
                        if (  (control->mv_for_array_position[block_number] != control->mv_for_array_position[block_number - 4]) ||
                            (ABS(control->MotionVecs[block_number].Comp.Hor - control->MotionVecs[block_number - 4].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[block_number].Comp.Ver - control->MotionVecs[block_number - 4].Comp.Ver) >= 4) )
                        {
                            strengthTab = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab = NULL;
                        }
                    }
                   ////////////////////////////////////////////////////////////////
                    // Calculate Strength   2                                                                           /
                    ////////////////////////////////////////////////////////////////
                    const int bn2 = block_number+1;
                    // Check whether macroblock or left macroblock is intra coded
                    if ( coefMask & (1 << bn2) )
                    {
                        strengthTab2 = CLIP_TABLE[1]; //   = 2;    // Luma block or neighbouring block has coefficients
                    }
                    else
                    {
                    // Neighbouring block is in current macroblock 
                     //   neighbouring_block_number = block_number - 4;
                        if (  (control->mv_for_array_position[bn2] != control->mv_for_array_position[bn2 - 4]) ||
                            (ABS(control->MotionVecs[bn2].Comp.Hor - control->MotionVecs[bn2 - 4].Comp.Hor) >= 4) ||
                            (ABS(control->MotionVecs[bn2].Comp.Ver - control->MotionVecs[bn2 - 4].Comp.Ver) >= 4) )
                        {
                            strengthTab2 = CLIP_TABLE[0]; //   = 1;
                        }
                        else
                        {
                            strengthTab2 = NULL;
                        }
                    }
                    
                    if(strengthTab == NULL && strengthTab2 == NULL)
                    {
                        continue;
                    }
                    // Vertical Filtering
                     // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[H264_CENTRE_EDGE] > 0) || (b_luma[H264_CENTRE_EDGE] > 0))
#endif                                        
                    {
                        const unsigned short edgeOffset = VBlockLumaOffsets[block_number];
                    
                        int alpha = ALPHA_TABLE[index_a_luma[H264_CENTRE_EDGE]];
                        _vr00 = _vmovw(edgeOffset, 2);
                        _vr01 = _vmovzw( 0, 0xff);//     alpha
                    
                        if(strengthTab)
                        {
                            _vr01 = _vmovw( alpha, 0x0f);//     alpha low x 4
                            _vr03 = _vmovw( strengthTab[index_a_luma[H264_CENTRE_EDGE]], 0x0f )    ;//     c0 low x 4
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw( alpha, 0xf0);//     alpha hi x 4
                            _vr03 = _vmovw( strengthTab2[index_a_luma[H264_CENTRE_EDGE]], 0xf0 )    ;//     c0 hi x 4
                        }

                        _vr02 = _vmovw( b_luma[H264_CENTRE_EDGE], 0xff);//     beta x8
                           
                        Simdu->RecMgr.Run(AM_DB_NormalVerticalFilter);
                    }
                    
                    if (((block_number & 4) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        && ((index_a_chroma[H264_CENTRE_EDGE] > 0) || (b_chroma[H264_CENTRE_EDGE] > 0))
#endif
                        )
                    {
                        // Filter every second edge
                        int ofs = CalcUVOffset(block_number);
                        int idxAC = index_a_chroma[H264_CENTRE_EDGE];
                        int betaC = b_chroma[H264_CENTRE_EDGE];
                         // u,v edge
                        unsigned char alpha = ALPHA_TABLE[idxAC];
                        // Two iterations of filtering
                
                        _vr01 = _vmovzw(0, 0xff);
                        _vr02 = _vmovzw(betaC, 0xff);
                
                        if(strengthTab)
                        {
                            _vr01 = _vmovw(alpha, 0x33);
                            _vr20 = _vmovw(strengthTab[idxAC] + 1, 0x33);
                        }
                        if(strengthTab2)
                        {
                            _vr01 = _vmovw(alpha, 0xcc);
                            _vr20 = _vmovw(strengthTab2[idxAC] + 1, 0xcc);
                        }
                        int macroc = Simdu->RecMgr.GetMacroAddr(AM_DB_EdgeChromaVertical);
                        _vr00 = _vmovw(AuroraH264Dec::m_u_work_buf+ofs-32, 0x10);     // i4
                        _vr00 = _vmovw(AuroraH264Dec::m_v_work_buf+ofs-32, 0x20);     // i5
                        _vrun(macroc);
                    }
                }       // END OF: for (block_number
                
                
            }   // END OF: else ( control->block_coded_intra)


            //      FlushDCache();
            Simdu->DMA_outDeblock(control->PixelX, control->PixelY);
            
            
        }  // END OF: (1 != control->disable_deblocking_filter_idc)
                
        control = control->next;
    }
}
//#endif
//#endif

#else  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void CH264Decoder::LoopFilterNonMBAFFFrame(CBTPicture * picture)
{
	CONTROL_LIST_TYPE const * control = GetControlTop();
	CONTROL_LIST_TYPE const * neighbouring_control;

//	printf("Frame %d\n",++frNum);


	unsigned int block_number;
	unsigned int neighbouring_block_number;
#ifndef AURORA_DEBLOCK
	PIXEL * top_left_of_4x4_block;
#endif
	int macroblock_edge;
	bool strong_filtering;
	
	unsigned int strength;
	
	unsigned int luma_hsize = picture->m_hsize;
	unsigned int chroma_hsize = luma_hsize >> 1;
	
	UNSIGNED_SEPTET index_a_luma[3];
	UNSIGNED_SEPTET b_luma[3];
	UNSIGNED_SEPTET index_a_chroma[3];
	UNSIGNED_SEPTET b_chroma[3];
	
#ifdef AURORA_DEBLOCK
	
	AuroraH264Dec &simdu = AuroraH264Dec::GetInstance();
	
	Simdu->SetupDeblockDMA(picture->m_y, picture->m_u, picture->m_v, picture->m_hsize, picture->m_vsize);
	
#endif
	
	
	while (0 != control)
    {
        if (1 != control->disable_deblocking_filter_idc)
        {
            // Deblocking for this macroblock
#ifdef AURORA_DEBLOCK
            //         FlushDCache();
            Simdu->DMA_inDeblock(control->mb_col*16-4, control->mb_row*16-4);
#endif
            
            // Calculate the luma and chroma a and b indices for centre, left and top edges
            
            // Centre edges, depends only on this macroblock's quant
            {
                UNSIGNED_SEPTET average_chroma_quant 
                    = CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset);
                
                index_a_luma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(control->quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[control->quant + control->slice_beta_offset ];
                
                index_a_chroma[H264_CENTRE_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_CENTRE_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Left edges, depends on this macroblock's and the left macroblock's quant
            neighbouring_control = control->left_MB;
            
            if (0 != neighbouring_control)
            {
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_LEFT_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_LEFT_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            // Top edges, depends on this macroblock's and the above macroblock's quant
            
            neighbouring_control = control->above_MB;
            
            if (0 != neighbouring_control)
            {
                int average_luma_quant = (control->quant + neighbouring_control->quant + 1) >> 1;
                
                int average_chroma_quant = 
                    ( CLIP_CHROMA_MAPPED_QUANT(control->quant + m_PictureParameterSet->chroma_qp_index_offset) +
                    CLIP_CHROMA_MAPPED_QUANT(neighbouring_control->quant + m_PictureParameterSet->chroma_qp_index_offset) + 1) >> 1;
                
                index_a_luma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_luma_quant + control->slice_alpha_c0_offset - 15);
                
                b_luma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_luma_quant + control->slice_beta_offset];
                
                index_a_chroma[H264_TOP_EDGE]
                    = CLIP_FOR_FILTER(average_chroma_quant + control->slice_alpha_c0_offset - 15);
                
                b_chroma[H264_TOP_EDGE]
                    = CLIP_BETA_TABLE[average_chroma_quant + control->slice_beta_offset];
            }
            
            ////////////////////////////////////////////////////////////////
            // Horizontal filtering when direction == 0
            ////////////////////////////////////////////////////////////////
            bool qskp;
            
            qskp = control->left_MB == NULL 
                ||( 2 == control->disable_deblocking_filter_idc 
                && control->left_MB->slice_id != control->slice_id);
            
            if(control->block_coded_intra)
            {
                strength = 3;
                for (block_number=0; block_number<16; block_number++)
                {
                    if ((block_number & 3) == 0)
                    {
                        
                        if (qskp)
                        {
                            // Neighbouring block is in different slice and no 
                            // filtering over slice boundaries - so skip to next block
                            continue;
                        }
                        neighbouring_control = control->left_MB;
                        neighbouring_block_number = block_number + 3;
                        macroblock_edge = H264_LEFT_EDGE;
                        strong_filtering = true;
                    }
                    else
                    {
                        // Neighbouring block is in current macroblock 
                        macroblock_edge = H264_CENTRE_EDGE;
                        neighbouring_control = control;
                        neighbouring_block_number = block_number - 1;
                        strong_filtering = false;
                    }
                    
                    
#ifdef AURORA_DEBLOCK
                    Simdu->LoopFilterEdgeLumaHorzontal(block_number, strength, strong_filtering, 
                        index_a_luma[macroblock_edge],
                        b_luma[macroblock_edge]);
                    
#else
                    top_left_of_4x4_block = picture->m_y + control->frame_y_offset + m_frame_y_4x4_block_offset[block_number];
                    
                    LoopFilterEdgeLuma(top_left_of_4x4_block, 1, luma_hsize, strength, strong_filtering, 
                        index_a_luma[macroblock_edge],
                        b_luma[macroblock_edge]);
#endif
                    
                    
                    
                    if (((block_number & 1) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        &&
                        ((index_a_chroma[macroblock_edge] > 0) ||
                        (b_chroma[macroblock_edge] > 0))
#endif                                        
                        )
                    {
                        // Filter every second edge
#ifdef AURORA_DEBLOCK
                        int ofs = Simdu->CalcUVOffset(block_number);
#else
                        int ofs = control->frame_uv_offset + m_frame_uv_4x4_block_offset[block_number];
#endif
                        int idxA = index_a_chroma[macroblock_edge];
                        int beta = b_chroma[macroblock_edge];
                        if(strong_filtering)
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontalStrong(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaHorizontalStrong(picture->m_u+ofs, chroma_hsize, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaHorizontalStrong(picture->m_v+ofs, chroma_hsize, idxA, beta);
#endif                                                             
                        }
                        else
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, strength, idxA, beta);
#else
                            // u edge
                            LoopFilterEdgeChromaHorizontal(picture->m_u+ofs, chroma_hsize, strength, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaHorizontal(picture->m_v+ofs, chroma_hsize, strength, idxA, beta);
#endif                                                            
                        }
                    }
                    
                }
            }
            else
            {
                // Loop over blocks in raster scan
                for (block_number=0; block_number<16; block_number++)
                {
                    ////////////////////////////////////////////////////////////////
                    // Determine Horizontal Neighbouring Block
                    ////////////////////////////////////////////////////////////////
                    
                    if ((block_number & 3) == 0)
                    {
                        
                        if (qskp)
                        {
                            // Neighbouring block is in different slice and no 
                            // filtering over slice boundaries - so skip to next block
                            continue;
                        }
                        neighbouring_control = control->left_MB;
                        neighbouring_block_number = block_number + 3;
                        macroblock_edge = H264_LEFT_EDGE;
                    }
                    else
                    {
                        // Neighbouring block is in current macroblock 
                        macroblock_edge = H264_CENTRE_EDGE;
                        neighbouring_control = control;
                        neighbouring_block_number = block_number - 1;
                    }
                    
                    ////////////////////////////////////////////////////////////////
                    // Calculate Strength										   /
                    ////////////////////////////////////////////////////////////////
                    
                    // Check whether macroblock or left macroblock is intra coded
                    
                    
                    
                    if (neighbouring_control->block_coded_intra)
                    {
                        // Note this is not quite as in the document as the variable strong_filtering 
                        // is used to indicate what the document refers to as strength == 4
                        strength = 3;  
                        strong_filtering = macroblock_edge != H264_CENTRE_EDGE; // Implies strong filtering is possible but not guaranteed
                    }
                    else if ( (control->y_coeffs_present & (1 << block_number)) ||
                        (neighbouring_control->y_coeffs_present & ( 1 << neighbouring_block_number)) )
                    {
                        // Luma block or neighbouring block has coefficients
                        strength = 2;
                        strong_filtering = false;
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[neighbouring_block_number]) ||
                            (ABS(control->mv_for_hor[block_number] - neighbouring_control->mv_for_hor[neighbouring_block_number]) >= 4) ||
                            (ABS(control->mv_for_ver[block_number] - neighbouring_control->mv_for_ver[neighbouring_block_number]) >= 4) )
                        {
                            strength = 1;
                            strong_filtering = false;
                        }
                        
                        else
                        {
                            // Nothing to do so skip to next block
                            continue;
                        }
                        
                    }
                    
                    
                    ////////////////////////////////////////////////////////////////
                    // Horizontal Filtering
                    ////////////////////////////////////////////////////////////////
                    
                    // y edge
#ifdef ALPHABETAZERO_TEST                                
                    if ((index_a_luma[macroblock_edge] > 0) ||
                        (b_luma[macroblock_edge] > 0))
#endif                                        
                    {
#ifdef AURORA_DEBLOCK
                        Simdu->LoopFilterEdgeLumaHorzontal(block_number, strength, strong_filtering, 
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
                        
#else
                        top_left_of_4x4_block = picture->m_y + control->frame_y_offset + m_frame_y_4x4_block_offset[block_number];
                        
                        LoopFilterEdgeLuma(top_left_of_4x4_block, 1, luma_hsize, strength, strong_filtering, 
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
#endif
                    }
                    
                    
                    if (((block_number & 1) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        &&
                        ((index_a_chroma[macroblock_edge] > 0) ||
                        (b_chroma[macroblock_edge] > 0))
#endif                                        
                        )
                    {
                        // Filter every second edge
#ifdef AURORA_DEBLOCK
                        int ofs = Simdu->CalcUVOffset(block_number);
#else
                        int ofs = control->frame_uv_offset + m_frame_uv_4x4_block_offset[block_number];
#endif
                        int idxA = index_a_chroma[macroblock_edge];
                        int beta = b_chroma[macroblock_edge];
                        if(strong_filtering)
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontalStrong(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaHorizontalStrong(picture->m_u+ofs, chroma_hsize, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaHorizontalStrong(picture->m_v+ofs, chroma_hsize, idxA, beta);
#endif                                                             
                        }
                        else
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaHorizontal(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, strength, idxA, beta);
#else
                            // u edge
                            LoopFilterEdgeChromaHorizontal(picture->m_u+ofs, chroma_hsize, strength, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaHorizontal(picture->m_v+ofs, chroma_hsize, strength, idxA, beta);
#endif                                                            
                        }
                    }
                    
                }	// END OF: for (block_number
            }	// END OF: else(control->block_coded_intra)


            
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Vertical   filtering when direction == 1/////////////////////
/////////////////////////////////////////////////////////////////////////////////////


            
            qskp = control->above_MB == NULL
                || (2 == control->disable_deblocking_filter_idc
                && control->above_MB->slice_id != control->slice_id);
            
            
            if( control->block_coded_intra)
            {
                strength = 3;
                for (block_number =( qskp<< 2); block_number<16; block_number++)
                {
                    ////////////////////////////////////////////////////////////////
                    // Determine Vertical Neighbouring Block
                    ////////////////////////////////////////////////////////////////
                    
                    if ((block_number & 12) == 0)
                    {
                        macroblock_edge = H264_TOP_EDGE;
                        neighbouring_control = control->above_MB;
                        neighbouring_block_number = block_number + 12;
                        strong_filtering = true;
                    }
                    else
                    {
                        // Neighbouring block is in current macroblock 
                        macroblock_edge = H264_CENTRE_EDGE;
                        neighbouring_control = control;
                        neighbouring_block_number = block_number - 4;
                        strong_filtering = false;
                    }
                    
                                      
                    
                    ////////////////////////////////////////////////////////////////
                    // Vertical Filtering
                    ////////////////////////////////////////////////////////////////
                    
                    // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[macroblock_edge] > 0) ||
                        (b_luma[macroblock_edge] > 0))
#endif                                        
                    {
#ifdef AURORA_DEBLOCK
                        Simdu->LoopFilterEdgeLumaVertical(block_number, strength, strong_filtering,
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
#else                                                    
                        top_left_of_4x4_block = picture->m_y + control->frame_y_offset + m_frame_y_4x4_block_offset[block_number];
                        
                        LoopFilterEdgeLuma(top_left_of_4x4_block, luma_hsize, 1, strength, strong_filtering,
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
#endif
                    }
                    
                    
                    if (((block_number & 4) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        &&
                        ((index_a_chroma[macroblock_edge] > 0) || 
                        (b_chroma[macroblock_edge] > 0))
#endif
                        )
                    {
                        // Filter every second edge
#ifdef AURORA_DEBLOCK
                        int ofs = Simdu->CalcUVOffset(block_number);
#else
                        int ofs = control->frame_uv_offset + m_frame_uv_4x4_block_offset[block_number];
#endif
                        int idxA = index_a_chroma[macroblock_edge];
                        int beta = b_chroma[macroblock_edge];
                        
                        if(strong_filtering)
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaVerticalStrong(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaVerticalStrong(picture->m_u+ofs, chroma_hsize, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaVerticalStrong(picture->m_v+ofs, chroma_hsize, idxA, beta);
#endif                                                                 
                        }
                        else
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaVertical(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, strength, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaVertical(picture->m_u+ofs, chroma_hsize, strength, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaVertical(picture->m_v+ofs, chroma_hsize, strength, idxA, beta);
#endif                                                                
                        }
                    }
                    
                }       // END OF: for (block_number
            }
            else
            {
                
                // Loop over blocks in raster scan
                for (block_number =( qskp<< 2); block_number<16; block_number++)
                {
                    ////////////////////////////////////////////////////////////////
                    // Determine Vertical Neighbouring Block
                    ////////////////////////////////////////////////////////////////
                    
                    if ((block_number & 12) == 0)
                    {
                        macroblock_edge = H264_TOP_EDGE;
                        neighbouring_control = control->above_MB;
                        neighbouring_block_number = block_number + 12;
                    }
                    else
                    {
                        // Neighbouring block is in current macroblock 
                        macroblock_edge = H264_CENTRE_EDGE;
                        neighbouring_control = control;
                        neighbouring_block_number = block_number - 4;
                    }
                    
                    ////////////////////////////////////////////////////////////////
                    // Calculate Strength                                                                              /
                    ////////////////////////////////////////////////////////////////
                    
                    // Check whether macroblock or left macroblock is intra coded
                    
                    
                    
                    if ( neighbouring_control->block_coded_intra)
                    {
                        // Note this is not quite as in the document as the variable strong_filtering 
                        // is used to indicate what the document refers to as strength == 4
                        strength = 3;  
                        strong_filtering = macroblock_edge != H264_CENTRE_EDGE; // Implies strong filtering is possible but not guaranteed
                    }
                    else if ( (control->y_coeffs_present & (1 << block_number)) ||
                        (neighbouring_control->y_coeffs_present & ( 1 << neighbouring_block_number)) )
                    {
                        // Luma block or neighbouring block has coefficients
                        strength = 2;
                        strong_filtering = false;
                    }
                    else
                    {
                        if (  (control->mv_for_array_position[block_number] != neighbouring_control->mv_for_array_position[neighbouring_block_number]) ||
                            (ABS(control->mv_for_hor[block_number] - neighbouring_control->mv_for_hor[neighbouring_block_number]) >= 4) ||
                            (ABS(control->mv_for_ver[block_number] - neighbouring_control->mv_for_ver[neighbouring_block_number]) >= 4) )
                        {
                            strength = 1;
                            strong_filtering = false;
                        }
                        
                        else
                        {
                            // Nothing to do so skip to next block
                            continue;
                        }
                        
                    }
                    
                    
                    ////////////////////////////////////////////////////////////////
                    // Vertical Filtering
                    ////////////////////////////////////////////////////////////////
                    
                    // y edge
#ifdef ALPHABETAZERO_TEST                                                               
                    if ((index_a_luma[macroblock_edge] > 0) ||
                        (b_luma[macroblock_edge] > 0))
#endif                                        
                    {
#ifdef AURORA_DEBLOCK
                        Simdu->LoopFilterEdgeLumaVertical(block_number, strength, strong_filtering,
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
#else                                                    
                        top_left_of_4x4_block = picture->m_y + control->frame_y_offset + m_frame_y_4x4_block_offset[block_number];
                        
                        LoopFilterEdgeLuma(top_left_of_4x4_block, luma_hsize, 1, strength, strong_filtering,
                            index_a_luma[macroblock_edge],
                            b_luma[macroblock_edge]);
#endif
                    }
                    
                    
                    if (((block_number & 4) == 0)
#ifdef ALPHABETAZERO_TEST                                
                        &&
                        ((index_a_chroma[macroblock_edge] > 0) || 
                        (b_chroma[macroblock_edge] > 0))
#endif
                        )
                    {
                        // Filter every second edge
#ifdef AURORA_DEBLOCK
                        int ofs = Simdu->CalcUVOffset(block_number);
#else
                        int ofs = control->frame_uv_offset + m_frame_uv_4x4_block_offset[block_number];
#endif
                        int idxA = index_a_chroma[macroblock_edge];
                        int beta = b_chroma[macroblock_edge];
                        
                        if(strong_filtering)
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaVerticalStrong(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaVerticalStrong(picture->m_u+ofs, chroma_hsize, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaVerticalStrong(picture->m_v+ofs, chroma_hsize, idxA, beta);
#endif                                                                 
                        }
                        else
                        {
#ifdef AURORA_DEBLOCK
                            // u,v edge
                            Simdu->LoopFilterEdgeChromaVertical(AuroraH264Dec::sdm_u_work_buf+ofs, AuroraH264Dec::sdm_v_work_buf+ofs, strength, idxA, beta);
#else                                                            
                            // u edge
                            LoopFilterEdgeChromaVertical(picture->m_u+ofs, chroma_hsize, strength, idxA, beta);
                            // v edge
                            LoopFilterEdgeChromaVertical(picture->m_v+ofs, chroma_hsize, strength, idxA, beta);
#endif                                                                
                        }
                    }
                    
                }       // END OF: for (block_number
            }   // END OF: else ( control->block_coded_intra)
            
            
#ifdef AURORA_DEBLOCK
            //      FlushDCache();
            Simdu->DMA_outDeblock(control->mb_col*16-4, control->mb_row*16-4);
#endif
            
            
        }  // END OF: (1 != control->disable_deblocking_filter_idc)
        
        control = control->next;
    }
}

#endif //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::LoopFilterField									*/
/* DESCRIPTION:	Performs loop filtering on the whole field.
				Must be called after complete field has been 
				encoded/decoded.
				When Q is appended to a variable, it indicates a parameter
				of the current block or macroblock
				When P is appended to a variable, it indicates a parameter
				of the neighbouring block or macroblock
*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::LoopFilterMBAFFFrame							*/
/* DESCRIPTION:	Performs loop filtering on the whole MB-AFF frame.
				Must be called after complete frame has been 
				encoded/decoded.
				When Q is appended to a variable, it indicates a parameter
				of the current block or macroblock
				When P is appended to a variable, it indicates a parameter
				of the neighbouring block or macroblock
*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::LoopFilterEdgeLuma/Chroma						*/
/* DESCRIPTION:	Performs loop filtering on a single edge.
				Q and q indicate a parameter/pel of the current block
				P and p indicate a parameter/pel of the neighbouring block

				Variables used within the function may appear to have names 
				that are not self explanatory. However, they are such, when 
				read in conjunction with the standard.
				
				The filtered pels are labelled:
					p3 p2 p1 p0 q0 q1 q2 q3
				data points to q1
				data_inc is the increment that should be applied to 
				the in_data array to get from p1 to p2 etc.
				data_inc_next is the increment that should be applied to 
				the data array to get from p1 to the p1 in the next 
				row/column etc.
				strength give the strength of the edge - it should be > 0
				strong_filtering indicates whether stronger (intra) filtering 
				is possible.
				index_a and index_b are the indices into the alpha, beta 
				and c0 tables.

*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

#ifndef AURORA_DEBLOCK


#define p3 (int)(*(data-4*data_inc))
#define p2 (int)(*(data-3*data_inc))
#define p1 (int)(*(data-2*data_inc))
#define p0 (int)(*(data-data_inc))
#define q0 (int)(*(data))
#define q1 (int)(*(data+data_inc))
#define q2 (int)(*(data+2*data_inc))
#define q3 (int)(*(data+3*data_inc))

#define P2 (*(data-3*data_inc))
#define P1 (*(data-2*data_inc))
#define P0 (*(data-data_inc))
#define Q0 (*(data))
#define Q1 (*(data+data_inc))
#define Q2 (*(data+2*data_inc))


inline void CH264Decoder::LoopFilterEdgeLuma(PIXEL * data, int data_inc,
										   int data_inc_next, int strength,
										   bool strong_filtering, int index_a, 
										   int beta)
{
	// macro_input are a temporary variables used as the argument to the CLIP macro
	// to avoid the need to evaluate the variable multiple times on expansion of the macro.
	int macro_input;

	int alpha = ALPHA_TABLE[index_a];
//	UNSIGNED_SEPTET beta = BETA_TABLE[index_b];
	int c0 = CLIP_TABLE[strength-1][index_a];
	int filter_count;

	for (filter_count=0; filter_count<4; filter_count++)
	{
		macro_input = ABS_PEL_DIFF(q0 - p0);

		if ( (macro_input < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
		{
			// Filter the edge
			if (strong_filtering)
			{
				// Strong filtering for some intra macroblock boundaries
				bool ap_less_than_beta = (ABS_PEL_DIFF(p2 - p0) < beta);
				bool aq_less_than_beta = (ABS_PEL_DIFF(q2 - q0) < beta);

				// Copy some original pels first
				int op1 = p1;
				int op0 = p0;
				int oq0 = q0;
				int oq1 = q1;

				bool small_gap = (macro_input < ((alpha >> 2) + 2));

				if (ap_less_than_beta && small_gap)
				{
					P0 = (PIXEL)((p2 + oq1 + ((op1 + op0 + oq0) << 1) + 4) >> 3);

					P1 = (PIXEL)((p2 + op1 + op0 + oq0 + 2) >> 2);

					P2 = (PIXEL)((p2 + op1 + op0 + oq0 + ((p3 + p2) << 1) + 4) >> 3);
				}
				else
				{
					P0 = (PIXEL)((op0 + oq1 + (op1 << 1) + 2) >> 2);
				}

				if (aq_less_than_beta && small_gap)
				{
					Q0 = (PIXEL)((op1 + q2 + ((op0 + oq0 + oq1) << 1) + 4) >> 3);

					Q1 = (PIXEL)((op0 + oq0 + oq1 + q2 + 2) >> 2);

					Q2 = (PIXEL)((q2 + oq1 + oq0 + op0 + ((q3 + q2) << 1) + 4) >> 3);
				}
				else
				{
					Q0 = (PIXEL)((oq0 + op1 + (oq1 << 1) + 2) >> 2);
				}
			}
			else
			{
				// Ordinary filtering

				bool ap_less_than_beta = (ABS_PEL_DIFF(p2 - p0) < beta);

				bool aq_less_than_beta = (ABS_PEL_DIFF(q2 - q0) < beta);

				int c = c0 + ap_less_than_beta + aq_less_than_beta;

				int p0_q0_rs1, delta;

				macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

				p0_q0_rs1 = (p0 + q0 + 1) >> 1;

				delta = CLIP(-c, c, macro_input);

				P0 = RECON_CLIP(p0 + delta);

				Q0 = RECON_CLIP(q0 - delta);

				if (ap_less_than_beta)
				{
					macro_input = (p2 + p0_q0_rs1 - (p1 << 1)) >> 1;

					P1 = (PIXEL)(p1 + CLIP(-c0, c0, macro_input));
				}

				if (aq_less_than_beta)
				{
					macro_input = (q2 + p0_q0_rs1 - (q1 << 1)) >> 1;

					Q1 = (PIXEL)(q1 + CLIP(-c0, c0, macro_input));
				}
			}
		}
		data += data_inc_next;
	}
}

#else





inline void AuroraH264Dec::LoopFilterEdgeLumaHorzontal(int blockNum,
                                        const unsigned char * strength,
                              //          bool strong_filtering,
                                        int index_a,
                                        int beta)
{
//        unsigned int edgeOffset = ((blockNum & 3) << 2) + (blockNum & 12)*24 + 4 +24*4 + m_y_work_buf;
        unsigned short edgeOffset = HBlockLumaOffsets[blockNum];

        int alpha = ALPHA_TABLE[index_a];
        int c0 = strength[index_a];

		e_macro_index macro = (e_macro_index)( AM_DB_HorzontalFilter - (blockNum & 1));
        _vr00 = _vmovw(edgeOffset-96,2);
        _vr01 = _vmovw(c0, 0x1);
        _vr01 = _vmovw(0, 0x2);
        _vr01 = _vmovw(beta, 0x4);
        _vr01 = _vmovw(alpha, 0x8);

		RecMgr.Run(macro);

}



//void AuroraH264Dec::LoopFilterEdgeLumaVertical(int blockNum,
//                                        const unsigned char * strength,
//                               //         bool strong_filtering,
//                                        int index_a,
//                                        int beta)
//{
//    unsigned int edgeOffset = VBlockLumaOffsets[blockNum];
//
//    int alpha = ALPHA_TABLE[index_a];
//    int c0 = strength[index_a];
//
//    if(edgeOffset & 7)
//    {
//        _vr00 = _vmovw(edgeOffset& ~7, 2);
//        _vr01 = _vmovzw( alpha, 0xf0);//     alpha hi x 4
//    }
//    else
//    {
//        _vr00 = _vmovw(edgeOffset, 2);
//        _vr01 = _vmovzw( alpha, 0x0f);//     alpha low x 4
//    }
//    _vr02 = _vmovw( beta, 0xff);//     beta low x4
//    _vr03 = _vmovw( c0, 0xff    )    ;//     c0 low x 4
//       
//    RecMgr.Run(AM_DB_NormalVerticalFilter);
//        
//}


#endif





/* ------------------------------------------------------------------------ */
#ifndef AURORA_DEBLOCK

inline void CH264Decoder::LoopFilterEdgeChroma(PIXEL * data, int data_inc,
											 int data_inc_next, int strength,
											 bool strong_filtering, int index_a,
											 int beta)
{
	unsigned char alpha = ALPHA_TABLE[index_a];
//	UNSIGNED_SEPTET beta = BETA_TABLE[index_b];

	// Two iterations of filtering

	// Start of first iteration
	if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
	{
		// Filter the edge
		if (strong_filtering)
		{
			// Strong filtering for some intra macroblock boundaries

			P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);

			Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
		}
		else
		{
			// Ordinary filtering

			int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

			int c = CLIP_TABLE[strength-1][index_a] + 1;

			int delta = CLIP(-c, c, macro_input);

			P0 = RECON_CLIP(p0 + delta);

			Q0 = RECON_CLIP(q0 - delta);
		}
	}
	// End of first iteration

	data += data_inc_next;

	// Start of second iteration
	if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
	{
		// Filter the edge
		if (strong_filtering)
		{
			// Strong filtering for some intra macroblock boundaries

			P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);

			Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
		}
		else
		{
			// Ordinary filtering

			int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

			int c = CLIP_TABLE[strength-1][index_a] + 1;

			int delta = CLIP(-c, c, macro_input);

			P0 = RECON_CLIP(p0 + delta);

			Q0 = RECON_CLIP(q0 - delta);
		}
	}
}
#endif

//------------------------------------------------------------------------------


#ifndef AURORA_DEBLOCK

inline void CH264Decoder::LoopFilterEdgeChromaHorizontal(PIXEL * data,
                                                                                         int data_inc_next, int strength,
                                                                                         int index_a,
                                                                                         int beta)
{
        unsigned char alpha = ALPHA_TABLE[index_a];
//        UNSIGNED_SEPTET beta = BETA_TABLE[index_b];
        const int data_inc = 1;
        int c = CLIP_TABLE[strength-1][index_a] + 1;

        // Two iterations of filtering

        // Start of first iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Ordinary filtering

                int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

                int delta = CLIP(-c, c, macro_input);

                P0 = RECON_CLIP(p0 + delta);

                Q0 = RECON_CLIP(q0 - delta);
        }
        // End of first iteration

        data += data_inc_next;

        // Start of second iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Ordinary filtering

                int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;


                int delta = CLIP(-c, c, macro_input);

                P0 = RECON_CLIP(p0 + delta);

                Q0 = RECON_CLIP(q0 - delta);
        }
}

#else


inline void AuroraH264Dec::LoopFilterEdgeChromaHorizontal(int data,  int data1, const unsigned char * strength, int index_a, int beta)
{
        unsigned char alpha = ALPHA_TABLE[index_a];
        const int data_inc = 1;
        int c = strength[index_a] + 1;

        _vr01 = _vmovw(alpha, 0xff);
        _vr02 = _vmovw(beta, 0xff);
        _vr20 = _vmovw(c, 0xff);
        
        _vr00 = _vmovw((int)data-4, 0x10);     // i4
        _vr00 = _vmovw((int)data1-4, 0x20);     // i5
        RecMgr.Run(AM_DB_EdgeChromaHorizontal);
}



#endif




//------------------------------------------------------------------------------




#ifndef AURORA_DEBLOCK



inline void CH264Decoder::LoopFilterEdgeChromaHorizontalStrong(PIXEL * data,
                                                               int data_inc_next,
                                                               int index_a,
                                                               int beta)
{
        unsigned char alpha = ALPHA_TABLE[index_a];
//        UNSIGNED_SEPTET beta = BETA_TABLE[index_b];
        const int data_inc = 1;

        // Two iterations of filtering

        // Start of first iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Strong filtering for some intra macroblock boundaries

                P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);
                Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
         }
        // End of first iteration

        data += data_inc_next;

        // Start of second iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Strong filtering for some intra macroblock boundaries

                P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);
                Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
        }
}

#else

//
//inline void AuroraH264Dec::LoopFilterEdgeChromaHorizontalStrong(int data, int data1, int index_a, int beta)
//{
//        unsigned char alpha = ALPHA_TABLE[index_a];
//        const int data_inc = 1;
//        int data_inc_next = 16;
//
//        // Two iterations of filtering
//        _vr01 = _vmovw(alpha, 0xff);
//        _vr02 = _vmovw(beta, 0xff);
//        
//        _vr00 = _vmovw((int)data-4, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaHorizontalStrong);
//        
//        _vr00 = _vmovw((int)data1-4, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaHorizontalStrong);
//}
//


#endif






//------------------------------------------------------------------------------




#ifndef AURORA_DEBLOCK


inline void CH264Decoder::LoopFilterEdgeChromaVertical(PIXEL * data, int data_inc, int strength,
                                                                                int index_a,
                                                                                int beta)
{
        unsigned char alpha = ALPHA_TABLE[index_a];
//        UNSIGNED_SEPTET beta = BETA_TABLE[index_b];
        const int data_inc_next = 1;
        int c = CLIP_TABLE[strength-1][index_a] + 1;
        // Two iterations of filtering

        // Start of first iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Ordinary filtering

                int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

                int delta = CLIP(-c, c, macro_input);

                P0 = RECON_CLIP(p0 + delta);

                Q0 = RECON_CLIP(q0 - delta);
        }
        // End of first iteration

        data += data_inc_next;

        // Start of second iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Ordinary filtering

                int macro_input = (((q0-p0)<<2) + (p1-q1) + 4) >> 3;

                int delta = CLIP(-c, c, macro_input);

                P0 = RECON_CLIP(p0 + delta);

                Q0 = RECON_CLIP(q0 - delta);
        }
}


#else

//inline void AuroraH264Dec::LoopFilterEdgeChromaVertical(int data, int data1, const unsigned char * strength, int index_a, int beta)
//{
//        unsigned char alpha = ALPHA_TABLE[index_a];
//        int c = strength[index_a] + 1;
//        // Two iterations of filtering
//
//        _vr01 = _vmovzw(0, 0xff);
//        _vr02 = _vmovzw(beta, 0xff);
//        _vr20 = _vmovzw(c, 0xff);
//
//        if((data & 3) != 0)
//        {
//            data = data & ~3;
//            data1 = data1 & ~3;
//            _vr01 = _vmovw(alpha, 0x0c);
//        }
//        else
//        {
//            _vr01 = _vmovw(alpha, 0x03);
//        }
//        _vr00 = _vmovw(data-32, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaVertical);
//
//        _vr00 = _vmovw(data1-32, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaVertical);
//}



#endif




//------------------------------------------------------------------------------



#ifndef AURORA_DEBLOCK


inline void CH264Decoder::LoopFilterEdgeChromaVerticalStrong(PIXEL * data, int data_inc,
                                                                                int index_a,
                                                                                int beta)
{
        unsigned char alpha = ALPHA_TABLE[index_a];
//        UNSIGNED_SEPTET beta = BETA_TABLE[index_b];
        const int data_inc_next = 1;
        // Two iterations of filtering

        // Start of first iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Strong filtering for some intra macroblock boundaries

                P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);

                Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
        }
        // End of first iteration

        data += data_inc_next;

        // Start of second iteration
        if ( (ABS_PEL_DIFF(q0 - p0) < alpha) && (ABS_PEL_DIFF(p1 - p0) < beta) && (ABS_PEL_DIFF(q1 - q0) < beta) )
        {
                // Filter the edge
                        // Strong filtering for some intra macroblock boundaries

                P0 = (PIXEL)((p0 + q1 + (p1 << 1) + 2) >> 2);

                Q0 = (PIXEL)((q0 + p1 + (q1 << 1) + 2) >> 2);
        }
}


#else



//inline void AuroraH264Dec::LoopFilterEdgeChromaVerticalStrong(int data, int data1, int index_a, int beta)
//{
//        unsigned char alpha = ALPHA_TABLE[index_a];
//        // Two iterations of filtering
//
//        _vr01 = _vmovzw(alpha, 0xff);
//        _vr02 = _vmovzw(beta, 0xff);
//
//        if((data & 2) != 0)
//        {
//            data = data & ~3;
//            data1 = data1 & ~3;
//            _vr22 = _vmovzw(-1, 0x0c);
//        }
//        else
//        {
//            _vr22 = _vmovzw(-1, 0x03); 
//        }
//        _vr00 = _vmovw((int)data-2*16, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaVerticalStrong);
//
//        _vr00 = _vmovw((int)data1-2*16, 0x20);     // i5
//        RecMgr.Run(AM_DB_EdgeChromaVerticalStrong);
//
//
//}



#endif




//------------------------------------------------------------------------------




/* ------------------------------------------------------------------------ */


#ifdef AURORA_DEBLOCK

_Asm void ASM_vdirun(int ftiSz, int x, int y, int dest, int stride)
{
        %con ftiSz; reg x ,y; con dest, stride
        
        vdiwr           %dr0,dest                                 ;//    SDM destination addr
        vdiwr           %dr1,stride                               ;//    SDM Stride
        vdiwr           %dr2,ftiSz                                ;//    idx 0, blk_ver_size<<8 || block_hor_size
//        vdowr           %dr6,2                                    ;//    Wait for DMA out
        vdirun          x,y
        
        
}


// DMA in block as setup above
void AuroraH264Dec::DMA_inDeblock(int x, int y)
{
/*#ifdef INLOOP_DBLOCK
	int chr_x = x >> 1;
    
	//Y
	//top 16 x 4 pixel block
	if(y != 0)
	{
		int dblk_Y_data = (m_top_dblock_Y + x + (720 * 12));
		int curr_dblk_data = (m_y_work_buf + 8);
		for (int i=0; i <4; i++)
		{
			_vr00 = _vmovw(dblk_Y_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE16);
		    
			curr_dblk_data += 24;
			dblk_Y_data += 720;
		}
		
	}
	
	//	left 4x16 pixel block
	if(x != 0)
	{
		int dblk_Y_data = m_top_dblock_Y + x - 4;
		int curr_dblk_data = m_y_work_buf + (24 * 4) + 4;
		_vr00 = _vmovw(dblk_Y_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY16X4_OFF_720_24_DBINPUT);
	}
	
	// U
	int chr_x_pos = x >> 1;
	int chr_y_pos = y >> 1;
	
	//	top 8 x 2 pixel block
	if(y != 0)
	{
		int dblk_U_data = m_top_dblock_U + chr_x_pos + (6 * 360);
		int curr_dblk_U_data = m_u_work_buf+8;
		for (int i=0; i <2; i++)
		{
			_vr00 = _vmovw(dblk_U_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_U_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE8);
			curr_dblk_U_data += 16;
			dblk_U_data += 360;
		}
		
		
	}
	
	//	left 2x8 pixel block
	if(x != 0)
	{
		int dblk_U_data =  m_top_dblock_U + chr_x_pos - 4;
		int curr_dblk_U_data =  m_u_work_buf+(16 * 2)+6;
		
		_vr00 = _vmovw(dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_360_16_DBINPUT);
	}
	
	// V
	//	top 8 x 2 pixel block
	if(y != 0)
	{
		int dblk_V_data = m_top_dblock_V + chr_x_pos + (6 * 360);
		int curr_dblk_V_data = m_v_work_buf+8;
		for (int i=0; i <2; i++)
		{
			_vr00 = _vmovw(dblk_V_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_V_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE8);
			curr_dblk_V_data += 16;
			dblk_V_data += 360;
		}
	}
	
	//	left 2x8 pixel block
	if(x != 0)
	{
		int dblk_V_data =  m_top_dblock_V + chr_x_pos - 4;
		int curr_dblk_V_data =  m_v_work_buf+(16 * 2)+6;
		
		_vr00 = _vmovw(dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_360_16_DBINPUT);
	}
	

#else*/
        _vr00 = _vmovw(m_y_work_buf+24, 0x1);       // %i0
        _vr00 = _vmovw(m_u_work_buf+16, 0x2);       // %i1
        _vr00 = _vmovw(m_v_work_buf+16, 0x4);       // %i2
        // Y
        RecMgr.Run(AM_DB_MoveForNextBlockY);
        ASM_vdirun(0x001410, x, y-4, m_y_work_buf+8, 24);
        // U
        RecMgr.Run(AM_DB_MoveForNextBlockU);
        ASM_vdirun(0x010a08, x/2, y/2-2, m_u_work_buf+8, 16);
        // V
        RecMgr.Run(AM_DB_MoveForNextBlockV);
        ASM_vdirun(0x020a08, x/2, y/2-2, m_v_work_buf+8, 16);
//#endif   
}

void AuroraH264Dec::copy_top_left_Deblock_data(int x, int y)
{
	int chr_x = x >> 1;
    
	//Y
	//top 16 x 4 pixel block
	if(y != 0)
	{
		int dblk_Y_data = (m_top_dblock_Y + x + (720 * 12));
		int curr_dblk_data = (m_y_work_buf + 8);
		for (int i=0; i <4; i++)
		{
			_vr00 = _vmovw(dblk_Y_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE16);
		    
			curr_dblk_data += 24;
			dblk_Y_data += 720;
		}
		
	}
	
	//	left 4x16 pixel block
	if(x != 0)
	{
		int dblk_Y_data = m_top_dblock_Y + x - 4;
		int curr_dblk_data = m_y_work_buf + (24 * 4) + 4;
		_vr00 = _vmovw(dblk_Y_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY16X4_OFF_720_24_DBINPUT);
	}
	
	// U
	int chr_x_pos = x >> 1;
	int chr_y_pos = y >> 1;
	
	//	top 8 x 2 pixel block
	if(y != 0)
	{
		int dblk_U_data = m_top_dblock_U + chr_x_pos + (6 * 360);
		int curr_dblk_U_data = m_u_work_buf+8;
		for (int i=0; i <2; i++)
		{
			_vr00 = _vmovw(dblk_U_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_U_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE8);
			curr_dblk_U_data += 16;
			dblk_U_data += 360;
		}
		
		
	}
	
	//	left 2x8 pixel block
	if(x != 0)
	{
		int dblk_U_data =  m_top_dblock_U + chr_x_pos - 4;
		int curr_dblk_U_data =  m_u_work_buf+(16 * 2)+6;
		
		_vr00 = _vmovw(dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_360_16_DBINPUT);
	}
	
	// V
	//	top 8 x 2 pixel block
	if(y != 0)
	{
		int dblk_V_data = m_top_dblock_V + chr_x_pos + (6 * 360);
		int curr_dblk_V_data = m_v_work_buf+8;
		for (int i=0; i <2; i++)
		{
			_vr00 = _vmovw(dblk_V_data, 0x1);       // %i0
		    _vr00 = _vmovw(curr_dblk_V_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE8);
			curr_dblk_V_data += 16;
			dblk_V_data += 360;
		}
	}
	
	//	left 2x8 pixel block
	if(x != 0)
	{
		int dblk_V_data =  m_top_dblock_V + chr_x_pos - 4;
		int curr_dblk_V_data =  m_v_work_buf+(16 * 2)+6;
		
		_vr00 = _vmovw(dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(curr_dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_360_16_DBINPUT);
	}
}


_Asm void ASM_dmaOutDeblock(int src, int ftiSz, int x, int y, int stride)
{
        %reg src,ftiSz,x,y,stride
//        vdiwr           %dr6,2          ;//    Clip mode - NON INTERLACE(block on DMA in not complete)

        vdowr           %dr0, src                                ;//    SDM source addr
        vdowr           %dr1, stride                               ;//    SDM Stride
        vdowr           %dr2, ftiSz                                ;//    idx 0, blk_ver_size<<8 || block_hor_size
        vdorun          x,y                                     ;    
}

//#ifdef INLOOP_DBLOCK
void AuroraH264Dec::CopyAndDMA_outDeblock(int x, int y, bool dma_macroblock)
{
	int chr_x = x >> 1;
	int chr_y = y >> 1;
	
	if(dma_macroblock)
	{
		//copy updated bottom pixels before DMA out
		int  curr_dblk_data = m_y_work_buf + (8 + 1*24);
		int dblk_Y_data = m_top_dblock_Y + x + (13 * 720);
		for (int i = 0; i<3 ; i ++)
		{
			_vr00 = _vmovw(curr_dblk_data, 0x1);       // %i0
		    _vr00 = _vmovw(dblk_Y_data, 0x2);       // %i1
		    RecMgr.Run(AM_COPYTOPLINE16);
			curr_dblk_data += 24;
			dblk_Y_data += 720;
		}
		
		
		int curr_dblk_U_data = m_u_work_buf + (8 + 1*16);
		int curr_dblk_V_data = m_v_work_buf + (8 + 1*16);
		
		int dblk_U_data = m_top_dblock_U + chr_x + (7 * 360);
		int dblk_V_data = m_top_dblock_V + chr_x + (7 * 360);
		
		_vr00 = _vmovw(curr_dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPYTOPLINE8);

	    _vr00 = _vmovw(curr_dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPYTOPLINE8);

		//DMA out Macroblock after deblocking
		ASM_dmaOutDeblock(m_top_dblock_Y + x, 0x001010, x, y - 16, 720);
        ASM_dmaOutDeblock(m_top_dblock_U + chr_x, 0x010808, chr_x, chr_y - 8, 360);
        _vdiwr(_dr6, 2);
        ASM_dmaOutDeblock(m_top_dblock_V + chr_x, 0x020808, chr_x, chr_y - 8, 360);
	}
	if(x == 0)
	{
		//copy macroblock data to deblocking luma buffer
		int curr_dblk_data = m_y_work_buf + (8 + 4*24);
		int dblk_Y_data = m_top_dblock_Y + x;
		_vr00 = _vmovw(curr_dblk_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_Y_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY16x16_OFF_24_720);
		
	    //	  copy macroblock data to deblocking chroma buffer
	    int curr_dblk_U_data = m_u_work_buf + (8 + 2*16);
	    int curr_dblk_V_data = m_v_work_buf + (8 + 2*16);
	    int dblk_U_data = m_top_dblock_U + chr_x;
	    int dblk_V_data = m_top_dblock_V + chr_x;
	    
	    _vr00 = _vmovw(curr_dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x8_OFF_16_360);
	    
	    _vr00 = _vmovw(curr_dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x8_OFF_16_360);
		
	}
	else
	{
		//copy macroblock data to deblocking luma buffer
		int curr_dblk_data = m_y_work_buf + (4 + 4*24);
		int dblk_Y_data = m_top_dblock_Y + x - 4;
		_vr00 = _vmovw(curr_dblk_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_Y_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY16x4_OFF_24_720);
	    
	    curr_dblk_data += 4;
	    dblk_Y_data += 4;
	    _vr00 = _vmovw(curr_dblk_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_Y_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY16x16_OFF_24_720);
	    
	    //copy macroblock data to deblocking chroma buffer
		
	    int curr_dblk_U_data = m_u_work_buf + (4 + 2*16);
		int dblk_U_data =  m_top_dblock_U + chr_x - 2;
		_vr00 = _vmovw(curr_dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_16_360);

	    curr_dblk_U_data += 4;
		dblk_U_data +=  2;
		_vr00 = _vmovw(curr_dblk_U_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_U_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x8_OFF_16_360);

	    
	    int curr_dblk_V_data = m_v_work_buf + (4 + 2*16);
	    int  dblk_V_data =  m_top_dblock_V + chr_x - 2;
	    _vr00 = _vmovw(curr_dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x2_OFF_16_360);
	    
	    curr_dblk_V_data += 4;
		dblk_V_data +=  2;
		_vr00 = _vmovw(curr_dblk_V_data, 0x1);       // %i0
	    _vr00 = _vmovw(dblk_V_data, 0x2);       // %i1
	    RecMgr.Run(AM_COPY8x8_OFF_16_360);
	}
	
}

//Simdu->DMA_out_Last_MB_Row(control->m_MacroblocksPerRow,control->PixelY);
void AuroraH264Dec::DMA_out_Last_MB_Row(int mbs_in_row, int y_pos)
{
	int chr_y = y_pos >> 1;
	for(int mb_num = 0; mb_num < mbs_in_row; mb_num++)
	{
		int x = mb_num * 16;
		int chr_x = mb_num * 8;
//		DMA out Macroblock after deblocking
		ASM_dmaOutDeblock(m_top_dblock_Y + x, 0x001010, x, y_pos, 720);
        ASM_dmaOutDeblock(m_top_dblock_U + chr_x, 0x010808, chr_x, chr_y, 360);
        _vdiwr(_dr6, 2);
        ASM_dmaOutDeblock(m_top_dblock_V + chr_x, 0x020808, chr_x, chr_y, 360);
	}
}

//#endif

// DMA out block as setup above
void AuroraH264Dec::DMA_outDeblock(int x, int y)
{
    if( x < 4)
    {
        if( y < 4)
        {       // Top corner of pic 16x16 luma & 8x8 chroma
            ASM_dmaOutDeblock(m_y_work_buf + 8 + 4*24, 0x001010, 0, 0, 24);
            ASM_dmaOutDeblock(m_u_work_buf + 8 + 2*16, 0x010808, 0, 0, 16);
            _vdiwr(_dr6, 2);
            ASM_dmaOutDeblock(m_v_work_buf + 8 + 2*16, 0x020808, 0, 0, 16);
        }
        else
        {   // 19x16 luma & 9x8 chroma
            ASM_dmaOutDeblock(m_y_work_buf + 8 + 1*24, 0x001310, 0, y-3, 24);
            ASM_dmaOutDeblock(m_u_work_buf + 8 + 1*16, 0x010908, 0, y/2-1, 16);
            _vdiwr(_dr6, 2);
            ASM_dmaOutDeblock(m_v_work_buf + 8 + 1*16, 0x020908, 0, y/2-1, 16);
        }
    }
    else
    {
         if( y < 4)
        {   // 16x19 luma & 8x9 chroma
            ASM_dmaOutDeblock(m_y_work_buf + 5 + 4*24, 0x001013, x-3, 0, 24);
            ASM_dmaOutDeblock(m_u_work_buf + 7 + 2*16, 0x010809, x/2-1, 0, 16);
            _vdiwr(_dr6, 2);
            ASM_dmaOutDeblock(m_v_work_buf + 7 + 2*16, 0x020809, x/2-1, 0, 16);
        }
        else
        {   // 19x19 luma & 9x9 chroma
            ASM_dmaOutDeblock(m_y_work_buf + 5 + 1*24, 0x001313, x-3, y-3, 24);
            ASM_dmaOutDeblock(m_u_work_buf + 7 + 1*16, 0x010909, x/2-1, y/2-1, 16);
            _vdiwr(_dr6, 2);
            ASM_dmaOutDeblock(m_v_work_buf + 7 + 1*16, 0x020909, x/2-1, y/2-1, 16);
        }
    }
}



#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
