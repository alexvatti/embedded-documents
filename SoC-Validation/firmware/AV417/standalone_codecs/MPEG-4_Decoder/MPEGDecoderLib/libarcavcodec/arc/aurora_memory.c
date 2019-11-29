/*
 * Copyright (c) 2005 ARC International (UK) Ltd
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
 */

/*
 * Memory Management for Aurora
 */

#include <stdlib.h>
#include "../dsputil.h"
#include "aurora_asm.h" /* for START_OF_FIXED_SDM */
#include "aurora_defines.h"
#include "aurora_memory.h"
#include "aurora_dma.h"

/*****************************************************

SDM BUFFER MANAGEMENT
0k===================================
                 ^
                 |
                 |
                 |
             DMA TABLE
         = DMA_TABLE_LENGTH
            = 512 = 0.5K
                 |
                 |
                 |
                \/
0.5k-----------------------------512b
                 ^
                 |
         (Status words etc)
                0.5k             
                 |
                \/
1k-------------------------------1024b
                 ^
                 |
                 |
                 |
                 |
         (circular buffer)
((NUMBER_OF_DCT_CIRCULAR_BUFFERS*SIZE_OF_DCT_BUFFER)
    =32*768=24576=24*1024= 24k  )     
                 |
                 |
                 |
                 |
                \/
25k-----------------------------25600b
       FREE(1k)-- for future use
26K-----------------------------26624b
                 ^
                 |
                 |
                 |
                 |
    (DMA-IN + MACRO-BLOCK OUTPUT)
    =3*((32*17)+(32*17))= 3264
             = 
                 |
                 |
                 |
                 |
                \/
29.1875K............................29888b
                 ^
                 |
                 |
                 |
                 |
    (INTERMEDIATE_QPEL_BUFFERS + DE-QUANT BUFFERS)
  =(4*32*17) + (4*2*64)= 2176 + 512 = 2688 = 2.625k
                 |
                 |
                 |
                 |
                \/
31.8125K---------------------------32576b
              !!FREE!! (192b)
32K===================================

*****************************************************/

// SDM buffers for prediction shouls always have width/stride 32 to match output buffers
// SDM buffer height is picked as 17 for max block size 16x16
#define SIZE_OF_SDM_DMA_IN_BUFFER              (32*17)

#define SIZE_OF_SDM_MCBLK_OUT_BUFFER           (32*17)

#define SIZE_OF_SDM_QPEL_INTERMEDIATE_BUFFER   (32*17)

// De-quant buffers should have size of short[64]
#define SIZE_OF_SDM_DEQUANT_BUFFER             (2*8*8)

/************** SDM BUFFER START ADDRS***********************/
#define START_OF_SDM_DMA_IN_BUFFERS         START_OF_FIXED_SDM_BUFFERS

#define START_OF_SDM_MCBLK_OUT_BUFFERS      (START_OF_SDM_DMA_IN_BUFFERS + 3*SIZE_OF_SDM_DMA_IN_BUFFER)

#define START_OF_SDM_QPEL_BUFFERS           (START_OF_SDM_MCBLK_OUT_BUFFERS + 3*SIZE_OF_SDM_MCBLK_OUT_BUFFER)

#define START_OF_SDM_DEQUANT_BUFFERS        (START_OF_SDM_QPEL_BUFFERS + 4*SIZE_OF_SDM_QPEL_INTERMEDIATE_BUFFER)

/* Virtual pointer to the SDM. */
unsigned char *SDM_Bytes;

/* The virtual address of the start of the frame buffer. */
void * Frame_Buffer_Virtual_Address = NULL;

/* The physical address of the start of the frame buffer. */
void * Frame_Buffer_Physical_Address = NULL;

unsigned int sdm_luma_dma_in_buffer;
unsigned int sdm_chroma_u_dma_in_buffer;
unsigned int sdm_chroma_v_dma_in_buffer;

unsigned int sdm_luma_mcblk_out_buffer;
unsigned int sdm_chroma_u_mcblk_out_buffer;
unsigned int sdm_chroma_v_mcblk_out_buffer;

unsigned int sdm_shifted_input_buffer;
unsigned int sdm_ver_filter_output_buffer;
unsigned int sdm_hor_filter_output_buffer;
unsigned int sdm_cpel_filter_output_buffer;

unsigned int sdm_intra_matrix_buffer;
unsigned int sdm_inter_matrix_buffer;
unsigned int sdm_chroma_intra_matrix_buffer;
unsigned int sdm_chroma_inter_matrix_buffer;

unsigned int last_dct_buffer_index;      //index of the last allocated dct buffer

void aurora_init_fixed_sdm_buffers(  unsigned short ** sdm_intra_matrix_ptr, 
                                    unsigned short ** sdm_inter_matrix_ptr,
                                    unsigned short ** sdm_chroma_intra_matrix_ptr,
                                    unsigned short ** sdm_chroma_inter_matrix_ptr)
{

#ifdef __ARC_STANDALONE_
    // Initialise the SDM base pointer
    SDM_Bytes = (unsigned char *)SDM_BASE;
#endif

    sdm_luma_dma_in_buffer          = START_OF_SDM_DMA_IN_BUFFERS;
    sdm_chroma_u_dma_in_buffer      = START_OF_SDM_DMA_IN_BUFFERS + SIZE_OF_SDM_DMA_IN_BUFFER;
    sdm_chroma_v_dma_in_buffer      = START_OF_SDM_DMA_IN_BUFFERS + 2*SIZE_OF_SDM_DMA_IN_BUFFER;

    sdm_luma_mcblk_out_buffer       = START_OF_SDM_MCBLK_OUT_BUFFERS;
    sdm_chroma_u_mcblk_out_buffer   = START_OF_SDM_MCBLK_OUT_BUFFERS + SIZE_OF_SDM_MCBLK_OUT_BUFFER;
    sdm_chroma_v_mcblk_out_buffer   = START_OF_SDM_MCBLK_OUT_BUFFERS + 2*SIZE_OF_SDM_MCBLK_OUT_BUFFER;
    
    sdm_shifted_input_buffer        = START_OF_SDM_QPEL_BUFFERS;
    sdm_ver_filter_output_buffer    = START_OF_SDM_QPEL_BUFFERS + SIZE_OF_SDM_QPEL_INTERMEDIATE_BUFFER;
    sdm_hor_filter_output_buffer    = START_OF_SDM_QPEL_BUFFERS + 2*SIZE_OF_SDM_QPEL_INTERMEDIATE_BUFFER;
    sdm_cpel_filter_output_buffer   = START_OF_SDM_QPEL_BUFFERS + 3*SIZE_OF_SDM_QPEL_INTERMEDIATE_BUFFER;

    sdm_intra_matrix_buffer         = START_OF_SDM_DEQUANT_BUFFERS;
    sdm_inter_matrix_buffer         = START_OF_SDM_DEQUANT_BUFFERS + SIZE_OF_SDM_DEQUANT_BUFFER;
    sdm_chroma_intra_matrix_buffer  = START_OF_SDM_DEQUANT_BUFFERS + 2*SIZE_OF_SDM_DEQUANT_BUFFER;
    sdm_chroma_inter_matrix_buffer  = START_OF_SDM_DEQUANT_BUFFERS + 3*SIZE_OF_SDM_DEQUANT_BUFFER;

    *sdm_intra_matrix_ptr = (unsigned short *) ((unsigned int)sdm_intra_matrix_buffer + (unsigned int)(SDM_Bytes));
    *sdm_inter_matrix_ptr = (unsigned short *) ((unsigned int)sdm_inter_matrix_buffer + (unsigned int)(SDM_Bytes));

    *sdm_chroma_intra_matrix_ptr = (unsigned short *) ((unsigned int)sdm_chroma_intra_matrix_buffer + (unsigned int)(SDM_Bytes));
    *sdm_chroma_inter_matrix_ptr = (unsigned short *) ((unsigned int)sdm_chroma_inter_matrix_buffer + (unsigned int)(SDM_Bytes));


    /********* initialize the DCT circular buffers  ***********/    
    // clear the status words; free the buffers
    for(last_dct_buffer_index=0; last_dct_buffer_index<NUMBER_OF_DCT_CIRCULAR_BUFFERS; last_dct_buffer_index++)
    {
        unsigned short *status_word = (unsigned short *)(START_OF_STATUS_WORDS_FOR_DCT_BUFFERS + (unsigned int)SDM_Bytes
                                                            + (2*last_dct_buffer_index));
        *status_word = 0;
    }
    // last_dct_buffer_index = NUMBER_OF_DCT_CIRCULAR_BUFFERS;

    {
        unsigned char * base_addr = (unsigned char *)(START_OF_DCT_CIRCULAR_BUFFERS + (unsigned int)SDM_Bytes);
        memset( base_addr, 0, (NUMBER_OF_DCT_CIRCULAR_BUFFERS * SIZE_OF_DCT_BUFFER));
    }
}
