/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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

/* ------------------------------------------------------------------------ */
/* FILE:			vlc_api.h									            */
/* DESCRIPTION:		API definitions for H264 hardware accelerator 			*/
/* VERSION:			v1.00													*/
/* DATE:			16 August 2005											*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef	VLC_API_H
#define	VLC_API_H

#include "vlc_types.h"
#include "vlc_model.h"

#ifdef __cplusplus
extern "C" 
{
#endif 

#define BIT_PTR_MASK        0x0000001F
#define BIT_PTR_LEN         5
#define BUF_ITEM_MASK       0x00000FE0
#define DEC_ERROR           0x40000000
#define BITS_IN_BUF         0x80000000

/* flags for AUX_VLC_DMA_CTRL */ 
#define DMA_ACTIVE	    0x1
#define DMA_OFF		    0x0
#define DMA_ESCAPE_3	    0x6

/* flags for AUX_VLC_SETUP */
#define VLC_INT_CLEAR       0x2
#define VLC_INT_ENABLE      0x4
#define VLC_BYTE_SWAP       0x10
#define VLC_DEC_MASK        0x300

// decoder definition
#define VLC_DEC_EXP         0x200
#define VLC_DEC_CAVLC       0x200
    
#if defined(CAVLC_HW_PRESENT)

#if defined __ARC_LINUX__
extern void *baseVirtualAddress;
extern void *basePhysicalAddress;
#endif

extern void CAVLC_Reset();

__FORCE_INLINE__ uint32 vlc_lr(uint32 aux_addr)
{
    return _lr(aux_addr);
}

__FORCE_INLINE__ void vlc_sr(uint32 aux_addr, uint32 aux_data)
{
    _sr(aux_data, aux_addr);
}

__FORCE_INLINE__ uint32 VLC_Get_Offset(void)
{
    return (vlc_lr(AUX_VLC_IBUF_STATUS) & BIT_PTR_MASK);
}

__FORCE_INLINE__ uint32 VLC_Get_Bits(int nbits)
{
    return vlc_lr(AUX_VLC_GET_0BIT + nbits);
}

__FORCE_INLINE__ int VLC_Get_Bit(void)
{
    return vlc_lr(AUX_VLC_GET_1BIT);
}

#define DMA_ESCAPE_OPTION DMA_ESCAPE_3
#define VLC_BYTE_ORDER VLC_BYTE_SWAP

__FORCE_INLINE__ void vlc_start_dma(void)
{
    vlc_sr(AUX_VLC_DMA_CTRL, DMA_ESCAPE_OPTION | DMA_ACTIVE);
}

__FORCE_INLINE__ void vlc_stop_dma(void)
{
    vlc_sr(AUX_VLC_DMA_CTRL, DMA_ESCAPE_OPTION | DMA_OFF);
}

__FORCE_INLINE__ void VLC_Reset(void)
{
    vlc_stop_dma();
    vlc_sr(AUX_VLC_DMA_ESC, 0x000003); // Big endian
    vlc_sr(AUX_VLC_IBUF_STATUS, 0);
}

__FORCE_INLINE__ void VLC_Clear_Interrupt(void)
{
    // clear the interrupt and set the decoder to work in EXP-Golomb mode
    vlc_sr(AUX_VLC_SETUP, VLC_DEC_EXP | VLC_INT_CLEAR | VLC_BYTE_ORDER);
}

/* Start reading new NAL starting at address indata */
__FORCE_INLINE__ void VLC_Buf_Init(unsigned char *indata, int data_length)
{
#if defined __ARC_LINUX__ 
    //convert the virtual address to a physical one for DMA operations
    unsigned char *data = (indata - (unsigned char *)baseVirtualAddress) +
                                    (unsigned char *)basePhysicalAddress;
#else
    unsigned char *data = indata;
#endif
    
    VLC_Reset();
    CAVLC_Reset();

    int offset = (unsigned)data & 0x3;
    vlc_sr(AUX_VLC_DMA_PTR, (unsigned)data - offset);
    vlc_sr(AUX_VLC_DMA_END, (unsigned)data + data_length);
    vlc_start_dma();
    vlc_lr(AUX_VLC_GET_0BIT + 8*offset);
}

__FORCE_INLINE__ uint32 VLC_Get_Bits_Left(void)
{
    vlc_stop_dma();
    unsigned status = vlc_lr(AUX_VLC_IBUF_STATUS);
    unsigned buf_end = vlc_lr(AUX_VLC_DMA_END);
    unsigned buf_pos = vlc_lr(AUX_VLC_DMA_PTR);
    vlc_start_dma();

    return 8 * (buf_end - buf_pos) + (status & 0xE0) - (status & BIT_PTR_MASK);
}

__FORCE_INLINE__ void VLC_Sel_Dec(uint32 decoder)
{
    vlc_sr(AUX_VLC_SETUP, decoder | VLC_INT_CLEAR | VLC_BYTE_ORDER);
}

__FORCE_INLINE__ int VLC_Get_Dec_Error(void)
{
    return (vlc_lr(AUX_VLC_IBUF_STATUS) & DEC_ERROR);
}

#endif

#ifdef __cplusplus
}
#endif

#endif

