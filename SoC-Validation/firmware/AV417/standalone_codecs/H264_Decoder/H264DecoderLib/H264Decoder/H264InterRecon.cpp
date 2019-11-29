/* ------------------------------------------------------------------------ */
/* FILE:            H264InterRecon.cpp                                        */
/* DESCRIPTION:        H.264 Inter prediction reconstruction functions            */
/* VERSION:            v2.00                                                    */
/* DATE:            29 June 2005                                            */
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
/*                                                                            */
/*                 (c) British Telecommunications plc                            */
/*                        All rights reserved 2005                            */
/*                            COPYRIGHT NOTICE                                */
/*                                                                            */
/*    British Telecommunications plc owns the sole copyright to this            */
/*    software. You may not (1) make a copy of this software except for the    */
/*    purposes of maintaining a single archive copy (2) derive works            */
/*    herefrom (3) distribute this work to others. This notice is provided    */
/*  for information and other rights may also apply.                        */
/*                                                                            */
/* ------------------------------------------------------------------------ */

#include "H264Warnings.h"

#include "CH264Decoder.h"
#include "H264DecoderControl.h"
#include "H264DecodedPictureBuffer.h"
#include "H264Tables.h"
#include "H264OffsetTables.h"
#include "H264Transform.h"

#ifdef I16K_D8K
#pragma Code("codesec3")
#endif

#ifdef _WIN32_WCE
#define assert(x)
#endif
/* ------------------------------------------------------------------------ */
/* FUNCTION:    CH264Decoder::InterRecon8x8                                    */
/* DESCRIPTION:    Performs Inter reconstruction 8x8 macroblocks                */
/* RETURNS:        True if successful else false.                                */
/* ------------------------------------------------------------------------ */
void CH264Decoder::SIMDInterRecon8x8(h264_decoder_control_t * control)
{
    unsigned int block_count_8x8;


	SDM_Offset sdm_y_out_buf = m_sdm_buf_ofs + SDM_ofs_y_out_buf;
	SDM_Offset sdm_u_out_buf = m_sdm_buf_ofs + SDM_ofs_u_out_buf;
	SDM_Offset sdm_v_out_buf = m_sdm_buf_ofs + SDM_ofs_v_out_buf;

    for (block_count_8x8=0; block_count_8x8<4; block_count_8x8++)
    {
        // Map the partition mode into a block shape and number of blocks.
        // If intra partition, will have to call some other function
        h264_block_shape_type_t block_shape;
        unsigned int number_of_blocks;
        unsigned int first_block_number = ((block_count_8x8 & 1) << 1) + ((block_count_8x8 & 2) << 2);
        
        int block_hor_size;
        int block_ver_size;

        unsigned int block_index;  // Indicates the index of the top left 4x4 block of the motion compensation block
        unsigned int block_number;  // Loop variable

        // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
        // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
        int for_hor_mv;
        int for_ver_mv;

        int integer_for_hor_vector;
        int integer_for_ver_vector;
        int fractional_for_hor_vector;
        int fractional_for_ver_vector;



        switch (control->partition_mode[block_count_8x8])
        {
            case H264_8X8_PARTITION :
                block_shape = H264_8X8;
                number_of_blocks = 1;
                block_hor_size = 8;
                block_ver_size = 8;
                break;

            case H264_8X4_PARTITION :
                block_shape = H264_8X4;
                number_of_blocks = 2;
                block_hor_size = 8;
                block_ver_size = 4;
                break;

            case H264_4X8_PARTITION :
                block_shape = H264_4X8;
                number_of_blocks = 2;
                block_hor_size = 4;
                block_ver_size = 8;
                break;

            case H264_4X4_PARTITION :
                block_shape = H264_4X4;
                number_of_blocks = 4;
                block_hor_size = 4;
                block_ver_size = 4;
                break;

            default :
                continue;  // Error. Continue to next partition
        }

        int fwd_ref_picture_array_position = control->mv_for_array_position[first_block_number];


        // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
        // Loop for each block that needs to be motion compensated
        for (block_number=0; block_number < number_of_blocks; block_number++)
        {
            block_index = first_block_number + h264_block_offset_table[block_shape][block_number];
            int wid = block_hor_size;
            int hgt = block_ver_size;

            for_hor_mv = control->MotionVecs[block_index].Comp.Hor;
            for_ver_mv = control->MotionVecs[block_index].Comp.Ver;

            integer_for_hor_vector = for_hor_mv >> 2;
            integer_for_ver_vector = for_ver_mv >> 2;

            fractional_for_hor_vector = for_hor_mv & 3;
            fractional_for_ver_vector = for_ver_mv & 3;

            // Calculate the extra pels needed
            if (fractional_for_hor_vector)
            {
                integer_for_hor_vector -= 2;
                wid += 5;
            }

            if (fractional_for_ver_vector)
            {
                integer_for_ver_vector -= 2;
                hgt += 5;
            }

            // Get the integer pel values from the reference picture
            // taking care with motion vectors that go off the edge of the picture

            Simdu->GetLumaRefBlockInSDM( fwd_ref_picture_array_position,
                    AuroraH264Dec::m_y_work_buf,
                    control->PixelX + 4 * (block_index & 3) + integer_for_hor_vector,
                    control->PixelY + (block_index & 0xc ) + integer_for_ver_vector,
                    wid, hgt,
                    LUMA_IN_OUT_BUF_X_SIZE );

            //////////////////////////////////////////////////////////////////////////////////////////////
            // LUMINANCE MOTION COMPENSATION
            //////////////////////////////////////////////////////////////////////////////////////////////

            if((0!=fractional_for_hor_vector) || (0!=fractional_for_ver_vector))
            {
                // Filter Luma
                Simdu->SIMDFilterLumaASM( AuroraH264Dec::m_y_work_buf,
                    fractional_for_hor_vector, fractional_for_ver_vector,
                     block_hor_size, block_ver_size);
            }

            // Now reconstruct Luma
            int sofs = 4 * (block_index & 3) + 64 * (block_index / 4);
            Simdu->SIMDReconLuma( m_y_data + sofs + sofs,
                sdm_y_out_buf + sofs,
                AuroraH264Dec::m_y_work_buf,    block_hor_size, block_ver_size );

            // CHROMA
            // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
            // which implies quarter pel precison for chroma

            wid = block_hor_size >> 1;
            hgt = block_ver_size >> 1;

            integer_for_hor_vector = for_hor_mv >> 3;
            integer_for_ver_vector = for_ver_mv >> 3;

            fractional_for_hor_vector = for_hor_mv & 7;
            fractional_for_ver_vector = for_ver_mv & 7;

            if(fractional_for_hor_vector)
            {
                wid += 1;
            }

            if(fractional_for_ver_vector)
            {
                hgt += 1;
            }
            // Ensure that the DMA-IN for LUMA is complete

            Simdu->GetChromaRefBlockInSDM( fwd_ref_picture_array_position,
                (control->PixelX >> 1) + 2 * (block_index & 3) + integer_for_hor_vector,
                (control->PixelY >> 1) + 2 * (block_index / 4) + integer_for_ver_vector,
                wid,hgt,
                CHROMA_IN_OUT_BUF_X_SIZE );



            //////////////////////////////////////////////////////////////////////////////////////////////
            // CHROMINANCE MOTION COMPENSATION
            //////////////////////////////////////////////////////////////////////////////////////////////


        	if((0!=fractional_for_hor_vector) || (0!=fractional_for_ver_vector))
        	{
                // Filter Chroma-U
                Simdu->SIMDFilterChromaASM( fractional_for_hor_vector, fractional_for_ver_vector,
                    block_hor_size/2, block_ver_size/2);
            }
            
        	sofs = ((block_index & 3) + 8 * (block_index >> 2))*2;
            // Reconstruct Chroma-U & V
            Simdu->SIMDReconChroma( m_u_data + sofs + sofs /* coeffs*/,
                    sdm_u_out_buf + sofs,
                    AuroraH264Dec::m_u_work_buf,    block_hor_size/2, block_ver_size/2 );
	
            
        }    // for each sub-block 4x4 4x8 8x4 8x8
    }        // for each block 8x8 8x16 16x8 16x16

    if(m_InloopDeblock)
    {
	    Simdu->MoveLumaMacroBlock2DBBuffer(sdm_y_out_buf );
	
	    
	    Simdu->MoveChromaUMacroBlock2DBBuffer( sdm_u_out_buf);
	
	    Simdu->MoveChromaVMacroBlock2DBBuffer( sdm_v_out_buf);
    }
    else
    {
    	int    mcblk_hor_offset =  control->PixelX;
	    int    mcblk_ver_offset =  control->PixelY;
	
	    Simdu->MoveLumaMacroBlock2Pic(sdm_y_out_buf , 
	        mcblk_hor_offset, mcblk_ver_offset );
	
	    mcblk_hor_offset = mcblk_hor_offset>>1;
	    mcblk_ver_offset = mcblk_ver_offset>>1;
	    
	    Simdu->MoveChromaUMacroBlock2Pic( sdm_u_out_buf, 
	        mcblk_hor_offset, mcblk_ver_offset );
	
	    Simdu->MoveChromaVMacroBlock2Pic( sdm_v_out_buf, 
	        mcblk_hor_offset, mcblk_ver_offset );
    }
    }



#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec1")
#endif



/* ------------------------------------------------------------------------ */
/* FUNCTION:    CH264Decoder::InterRecon                                    */
/* DESCRIPTION:    Performs Inter reconstruction for 16x16, 16x8 and 8x16 
                macroblocks                                                    */
/* RETURNS:        True if successful else false.                                */
/* ------------------------------------------------------------------------ */

void CH264Decoder::SIMDInterRecon(h264_decoder_control_t * control)
{
    h264_block_shape_type_t block_shape;
    unsigned int number_of_blocks;

    unsigned int block_index;  // Indicates the index of the top left 4x4 block of the motion compensation block
    unsigned int block_number;  // Loop counter

    // The integer vector is calculated so that the actual vector is between integer_vector and integer_vector + 1
    // regardless of whether the vector is positive or negative. 1 in this case means 1 integer unit.
    int for_hor_mv;
    int for_ver_mv;

    int integer_for_hor_vector;
    int integer_for_ver_vector;
    int fractional_for_hor_vector;
    int fractional_for_ver_vector;


    // To perform fractional pel interpolation, extra pels beyond the area of the motion compensation block are needed

    int fwd_ref_picture_array_position=0;

    int block_hor_size = h264_block_width_table[control->mtype];
    int block_ver_size = h264_block_height_table[control->mtype];


	SDM_Offset sdm_y_out_buf = m_sdm_buf_ofs + SDM_ofs_y_out_buf;
	SDM_Offset sdm_u_out_buf = m_sdm_buf_ofs + SDM_ofs_u_out_buf;
	SDM_Offset sdm_v_out_buf = m_sdm_buf_ofs + SDM_ofs_v_out_buf;



	block_shape = control->mtype;
	number_of_blocks = _min(block_shape +1,2);

    for (block_number=0; block_number < number_of_blocks; block_number++)
    {
        block_index = h264_block_offset_table[block_shape][block_number];

        fwd_ref_picture_array_position = control->mv_for_array_position[block_index];
        int wid = block_hor_size;
        int hgt = block_ver_size;
        for_hor_mv = control->MotionVecs[block_index].Comp.Hor;
        for_ver_mv = control->MotionVecs[block_index].Comp.Ver;

        // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
        integer_for_hor_vector = for_hor_mv >> 2;
        integer_for_ver_vector = for_ver_mv >> 2;

        fractional_for_hor_vector = for_hor_mv & 3;
        fractional_for_ver_vector = for_ver_mv & 3;

        // Calculate the extra pels needed
        if (fractional_for_hor_vector)
        {
            integer_for_hor_vector -= 2;
            wid += 5;
        }

        if (fractional_for_ver_vector)
        {
            integer_for_ver_vector -= 2;
            hgt += 5;
        }
        // Get the integer pel values from the reference picture
        // taking care with motion vectors that go off the edge of the picture

        Simdu->GetLumaRefBlockInSDM( fwd_ref_picture_array_position,
                AuroraH264Dec::m_y_work_buf,
                control->PixelX + 4 * (block_index & 3) + integer_for_hor_vector,
                control->PixelY + (block_index & 0xc) + integer_for_ver_vector,
                wid,
                hgt,
                LUMA_IN_OUT_BUF_X_SIZE );

        //////////////////////////////////////////////////////////////////////////////////////////////
        // LUMINANCE MOTION COMPENSATION
        //////////////////////////////////////////////////////////////////////////////////////////////
            
        if((0!=fractional_for_hor_vector) || (0!=fractional_for_ver_vector))
        {
            Simdu->SIMDFilterLumaASM( AuroraH264Dec::m_y_work_buf,
                fractional_for_hor_vector, fractional_for_ver_vector,
                block_hor_size, block_ver_size);
        }
        {
        	int t = ((block_index & 3) << 2 )+ 64 * (block_index / 4);
//        	int t = top_left_of_luma_4x4_block_raster_order_table[block_index];
	        Simdu->SIMDReconLuma( m_y_data + t*2,
	                sdm_y_out_buf + t,
	                AuroraH264Dec::m_y_work_buf, block_hor_size, block_ver_size );
        }

        // Calculate the integer and fractional parts of the motion vector assuming quarter pel precision
        // which implies quarter pel precison for chroma

        wid = block_hor_size >> 1;
        hgt = block_ver_size >> 1;

        integer_for_hor_vector = for_hor_mv >> 3;
        integer_for_ver_vector = for_ver_mv >> 3;

        fractional_for_hor_vector = for_hor_mv & 7;
        fractional_for_ver_vector = for_ver_mv & 7;

        if(fractional_for_hor_vector)
        {
            wid += 1;
        }

        if(fractional_for_ver_vector)
        {
            hgt += 1;
        }


        // Get the integer pel values from the reference picture
        // taking care with motion vectors that go off the edge of the picture
        Simdu->GetChromaRefBlockInSDM( fwd_ref_picture_array_position,
            (control->PixelX >> 1) + 2 * (block_index & 3) + integer_for_hor_vector ,
            (control->PixelY >> 1) + 2 * (block_index / 4) + integer_for_ver_vector ,
            wid,
            hgt,
            CHROMA_IN_OUT_BUF_X_SIZE );



        //////////////////////////////////////////////////////////////////////////////////////////////
        // CHROMINANCE MOTION COMPENSATION
        //////////////////////////////////////////////////////////////////////////////////////////////


    	if((0!=fractional_for_hor_vector) || (0!=fractional_for_ver_vector))
    	{
            Simdu->SIMDFilterChromaASM(fractional_for_hor_vector, fractional_for_ver_vector,
                block_hor_size/2, block_ver_size/2);
        }

		{
			int sofs = ( (block_index & 3) + 8 * (block_index / 4))*4;
			int dofs = ((block_index & 3) << 1) + (16 * (block_index >> 2));
	        Simdu->SIMDReconChroma( m_u_data + sofs /* coeffs*/,
	                sdm_u_out_buf + dofs,
	                AuroraH264Dec::m_u_work_buf, block_hor_size/2, block_ver_size/2 );
	
		}
    }
    if(m_InloopDeblock)
    {
	    Simdu->MoveLumaMacroBlock2DBBuffer(sdm_y_out_buf );
	
	    
	    Simdu->MoveChromaUMacroBlock2DBBuffer( sdm_u_out_buf);
	
	    Simdu->MoveChromaVMacroBlock2DBBuffer( sdm_v_out_buf);
    }
    else
    {
        int    mcblk_hor_offset =   control->PixelX;
        int    mcblk_ver_offset =   control->PixelY;

        Simdu->MoveLumaMacroBlock2Pic(sdm_y_out_buf , 
            mcblk_hor_offset, mcblk_ver_offset );

        mcblk_hor_offset = mcblk_hor_offset>>1;
        mcblk_ver_offset = mcblk_ver_offset>>1;
        
        Simdu->MoveChromaUMacroBlock2Pic( sdm_u_out_buf, 
            mcblk_hor_offset, mcblk_ver_offset );

        Simdu->MoveChromaVMacroBlock2Pic( sdm_v_out_buf, 
            mcblk_hor_offset, mcblk_ver_offset );
    }
}


/* ------------------------------------------------------------------------ */
/* END OF FILE                                                                */
/* ------------------------------------------------------------------------ */
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif


