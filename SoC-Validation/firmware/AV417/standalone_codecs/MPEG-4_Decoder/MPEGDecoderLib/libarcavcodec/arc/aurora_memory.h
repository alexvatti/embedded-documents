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
 * aurora memory management
 */

#ifndef _AURORA_MEMORY_H_
#define _AURORA_MEMORY_H_


#define START_OF_STATUS_WORDS_FOR_DCT_BUFFERS       (512)

#define START_OF_DCT_CIRCULAR_BUFFERS               (1024)

#define START_OF_FIXED_SDM_BUFFERS                  (26*1024)

#define NUMBER_OF_DCT_CIRCULAR_BUFFERS              32

#define SIZE_OF_DCT_BUFFER                          (6*(2*8*8))

/******************** EXTERN VARIABLES ****************************/
extern unsigned int sdm_shifted_input_buffer;
extern unsigned int sdm_ver_filter_output_buffer;
extern unsigned int sdm_hor_filter_output_buffer;
extern unsigned int sdm_cpel_filter_output_buffer;

extern unsigned int sdm_luma_dma_in_buffer;
extern unsigned int sdm_chroma_u_dma_in_buffer;
extern unsigned int sdm_chroma_v_dma_in_buffer;

extern unsigned int sdm_luma_mcblk_out_buffer;
extern unsigned int sdm_chroma_u_mcblk_out_buffer;
extern unsigned int sdm_chroma_v_mcblk_out_buffer;

extern unsigned int last_dct_buffer_index;

extern void aurora_init_fixed_sdm_buffers(  unsigned short ** sdm_intra_matrix_ptr, 
                                    unsigned short ** sdm_inter_matrix_ptr,
                                    unsigned short ** sdm_chroma_intra_matrix_ptr,
                                    unsigned short ** sdm_chroma_inter_matrix_ptr);

/*************** Get Pre-allocated buffers ********************/
// DMA-IN buffers
inline unsigned int GetLumaDMAInBuffer_sdm()
{
    return sdm_luma_dma_in_buffer;
}

inline unsigned int GetChromaUDMAInBuffer_sdm()
{
    return sdm_chroma_u_dma_in_buffer;
}

inline unsigned int GetChromaVDMAInBuffer_sdm()
{
    return sdm_chroma_v_dma_in_buffer;
}

// macro-block output buffers
inline unsigned int GetLumaMcBlkOutBuffer_sdm()
{
    return sdm_luma_mcblk_out_buffer;
}

inline unsigned int GetChromaUMcBlkOutBuffer_sdm()
{
    return sdm_chroma_u_mcblk_out_buffer;
}

inline unsigned int GetChromaVMcBlkOutBuffer_sdm()
{
    return sdm_chroma_v_mcblk_out_buffer;
}

/******************** DCT CIRCULAR BUFFER ****************************/
_Asm void InlineClearDctBufferStatusWord(unsigned int status_word_addr)
{
    % reg status_word_addr
    vmovw   %vr00, status_word_addr, 2 ;       // Load r1 in i1 with index

    vxor    %vr01, %vr01, %vr01 ;               // clear a register

    vst16   %vr01, [%i1, 0] ;                   // store 16-bit status word at the specified location
}

inline void * GetDctBuffer()
{
    unsigned int timeOut = 1;
    unsigned short *status_word_buffer_ptr;
    status_word_buffer_ptr = (unsigned short *)(START_OF_STATUS_WORDS_FOR_DCT_BUFFERS + (unsigned int)SDM_Bytes);

    do
    {
        last_dct_buffer_index = (last_dct_buffer_index + 1) & 0x1f;     // this works with 32 buffers
        if( *(status_word_buffer_ptr + last_dct_buffer_index) == 0)
        {
            *(status_word_buffer_ptr + last_dct_buffer_index) = 1;
            return (void *)(START_OF_DCT_CIRCULAR_BUFFERS + 
                               (unsigned int)SDM_Bytes + (last_dct_buffer_index*SIZE_OF_DCT_BUFFER));
        }

    }while(++timeOut);

    // something is wrong.... make an attempt to recover
    // free all the buffers except the first buffer and return 1st buffer
    for(last_dct_buffer_index=1; last_dct_buffer_index<NUMBER_OF_DCT_CIRCULAR_BUFFERS;  last_dct_buffer_index++)
    {
        *(status_word_buffer_ptr + last_dct_buffer_index) = 0;
    }
    
    last_dct_buffer_index = 0;
    return (void *)(START_OF_DCT_CIRCULAR_BUFFERS + (unsigned int)SDM_Bytes);
}

// Free up the last allocated DCT Buffer
inline void FreeDctBuffer()
{
    unsigned int status_word_ofst= START_OF_STATUS_WORDS_FOR_DCT_BUFFERS + (2*last_dct_buffer_index);
    InlineClearDctBufferStatusWord(status_word_ofst);

}

// Free up all DCT buffers
inline void FreeAllDctBuffers()
{
    unsigned int buffer_index;
    unsigned int status_word_ofst;

    for(buffer_index=0; buffer_index<NUMBER_OF_DCT_CIRCULAR_BUFFERS;  buffer_index++)
    {
        status_word_ofst= START_OF_STATUS_WORDS_FOR_DCT_BUFFERS + (2*last_dct_buffer_index);
        InlineClearDctBufferStatusWord(status_word_ofst);
    }
}

inline int waitforDCTbufferFree()
{
  unsigned int timeOut = 0;
    volatile unsigned short *status_word_buffer_ptr;
    
    volatile unsigned short * status_word_ofst= (volatile unsigned short*)(START_OF_STATUS_WORDS_FOR_DCT_BUFFERS + (2*last_dct_buffer_index) + (unsigned int)SDM_Bytes);

    do
    {
        volatile short tmp = *(status_word_ofst);
        if( tmp == 0)
        {
            //*(status_word_buffer_ptr + last_dct_buffer_index) = 1;
            timeOut++;
            return timeOut;
        }

    }while(1);
    return -1;
}
#endif  //_AURORA_MEMORY_H_

