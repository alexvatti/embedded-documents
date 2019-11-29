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
 * This is an implementation of the API for the Aurora
 * optimised VLC bitstream decoder.
 * 
 */

#define VLC_BUILD             0x078     // R     Build Configuration register for VLC module
#define AUX_VLC_BUF_IDX       0x500     // R     Bitstream Buffer Pointer for buffer read
#define AUX_VLC_READ_BUF      0x501     // R     Bitstream Buffer Read register
#define AUX_VLC_BUF_IN        0x503     // W     Bitstream Buffer Write Register
#define AUX_VLC_BUF_FREE      0x504     // R     Bitstream Buffer Number of free items space.
#define AUX_VLC_IBUF_STATUS   0x505     // R/W   Bitstream Buffer Status. Write to change bit offset and Buffer tail pointer.
#define AUX_VLC_SETUP         0x506     // R/W   Setup register: Decoder select and interrupt ctrl 
#define AUX_VLC_BITS          0x507     // R     Returns the next 32bits from the bitstream
#define AUX_VLC_TABLE         0x508     // R/W   Table select
#define AUX_VLC_GET_SYMBOL    0x509     // R     Read decoded Symbol and update offset
#define AUX_VLC_READ_SYMBOL   0x50A     // R     Read decoded Symbol only

#define AUX_UCAVLC_SETUP      0x510      // R/W   UVLC and CAVLC Decode Setup Register
#define AUX_UCAVLC_STATE      0x511      // R/W   UVLC and CAVLC Decode Intermedate State Values Registers
#define AUX_CAVLC_ZERO_LEFT   0x512      // R/W   CAVLC Zero Run decode Zero Left Register.
#define AUX_UVLC_I_STATE      0x514      // R/W   UVLC Internal states, for context saving/restoring.

#define AUX_VLC_DMA_PTR       0x51C
#define AUX_VLC_DMA_END       0x51D
#define AUX_VLC_DMA_ESC       0x51E
#define AUX_VLC_DMA_CTRL      0x51F

#define AUX_VLC_GET_0BIT      0x520      // R     read and consume 0 bit from bit stream
#define AUX_VLC_GET_1BIT      0x521      // R     read and consume 1 bit from bit stream
#define AUX_VLC_GET_2BIT      0x522      // R     read and consume 2 bit from bit stream
#define AUX_VLC_GET_3BIT      0x523      // R     read and consume 3 bit from bit stream
#define AUX_VLC_GET_4BIT      0x524      // R     read and consume 4 bit from bit stream
#define AUX_VLC_GET_5BIT      0x525      // R     read and consume 5 bit from bit stream
#define AUX_VLC_GET_6BIT      0x526      // R     read and consume 6 bit from bit stream
#define AUX_VLC_GET_7BIT      0x527      // R     read and consume 7 bit from bit stream
#define AUX_VLC_GET_8BIT      0x528      // R     read and consume 8 bit from bit stream
#define AUX_VLC_GET_9BIT      0x529      // R     read and consume 9 bit from bit stream
#define AUX_VLC_GET_10BIT     0x52A      // R     read and consume 10 bit from bit stream
#define AUX_VLC_GET_11BIT     0x52B      // R     read and consume 11 bit from bit stream
#define AUX_VLC_GET_12BIT     0x52C      // R     read and consume 12 bit from bit stream
#define AUX_VLC_GET_13BIT     0x52D      // R     read and consume 13 bit from bit stream
#define AUX_VLC_GET_14BIT     0x52E      // R     read and consume 14 bit from bit stream
#define AUX_VLC_GET_15BIT     0x52F      // R     read and consume 15 bit from bit stream
#define AUX_VLC_GET_16BIT     0x530      // R     read and consume 16 bit from bit stream
#define AUX_VLC_GET_17BIT     0x531      // R     read and consume 17 bit from bit stream
#define AUX_VLC_GET_18BIT     0x532      // R     read and consume 18 bit from bit stream
#define AUX_VLC_GET_19BIT     0x533      // R     read and consume 19 bit from bit stream
#define AUX_VLC_GET_20BIT     0x534      // R     read and consume 20 bit from bit stream
#define AUX_VLC_GET_21BIT     0x535      // R     read and consume 21 bit from bit stream
#define AUX_VLC_GET_22BIT     0x536      // R     read and consume 22 bit from bit stream
#define AUX_VLC_GET_23BIT     0x537      // R     read and consume 23 bit from bit stream
#define AUX_VLC_GET_24BIT     0x538      // R     read and consume 24 bit from bit stream
#define AUX_VLC_GET_25BIT     0x539      // R     read and consume 25 bit from bit stream
#define AUX_VLC_GET_26BIT     0x53A      // R     read and consume 26 bit from bit stream
#define AUX_VLC_GET_27BIT     0x53B      // R     read and consume 27 bit from bit stream
#define AUX_VLC_GET_28BIT     0x53C      // R     read and consume 28 bit from bit stream
#define AUX_VLC_GET_29BIT     0x53D      // R     read and consume 29 bit from bit stream
#define AUX_VLC_GET_30BIT     0x53E      // R     read and consume 30 bit from bit stream
#define AUX_VLC_GET_31BIT     0x53F      // R     read and consume 31 bit from bit stream


#define ARC_VLC_TABLE_RL_MPEG1 1
#define ARC_VLC_TABLE_RL_MPEG2 2
#define ARC_VLC_TABLE_RL_INTRA 3
#define ARC_VLC_TABLE_RL_INTER 4
#define ARC_VLC_TABLE_RVLC_RL_INTRA 5
#define ARC_VLC_TABLE_RVLC_RL_INTER 6
#define ARC_VLC_TABLE_RL_INTRA_AIC 7
#define ARC_VLC_TABLE_MB_ADDR_INC 8
#define ARC_VLC_TABLE_DC_LUM 9
#define ARC_VLC_TABLE_DC_CHROMA 10
#define ARC_VLC_TABLE_MB_PAT 11
#define ARC_VLC_TABLE_MB_MOTION 12
#define ARC_VLC_TABLE_MB_PTYPE 13
#define ARC_VLC_TABLE_MB_BTYPE 14
#define ARC_VLC_TABLE_MP4_DC_LUM 15
#define ARC_VLC_TABLE_MP4_DC_CHROM 16
#define ARC_VLC_TABLE_SPRITE 17
#define ARC_VLC_TABLE_INTRA_MCBPC 18
#define ARC_VLC_TABLE_INTER_MCBPC 19
#define ARC_VLC_TABLE_CBPY 20
#define ARC_VLC_TABLE_MB_TYPE_B 21
#define ARC_VLC_TABLE_H263_MBTYPE_B 22
#define ARC_VLC_TABLE_CBPC_B 23
#define ARC_VLC_TABLE_MV 24

#ifdef __HIGHC__
#  define vlc_sr(aux_addr, aux_data) _sr(aux_data, aux_addr)
#  define vlc_lr(aux_addr) _lr(aux_addr)
#else

void vlc_reset ();
void vlc_sr (int aux_addr, unsigned int aux_data);
unsigned int vlc_lr (int aux_addr);

#endif

