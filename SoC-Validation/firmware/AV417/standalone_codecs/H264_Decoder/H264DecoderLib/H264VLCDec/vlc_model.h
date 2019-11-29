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
/* FILE:			vlc_model.h									            */
/* DESCRIPTION:		The C model for H264 hardware accelerator 			*/
/* VERSION:			v1.00													*/
/* DATE:			16 August 2005											*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef	VLC_MODEL_H
#define	VLC_MODEL_H

#include"vlc_types.h"

#ifdef __cplusplus
extern "C" 
{
#endif 

// Auxiliary Registers

//      Register Name         Aux Address  Mode   Description
//      -------------         -----------  ----   ----------------------------------------------------------------------------
#define AUX_VLC_BUF_IDX       0x500      // R     Bitstream Buffer Pointer for buffer read
#define AUX_VLC_READ_BUF      0x501      // R     Bitstream Buffer Read register
#define AUX_VLC_BUF_IN        0x503      // W     Bitstream Buffer Write Register
#define AUX_VLC_BUF_FREE      0x504      // 
#define AUX_VLC_IBUF_STATUS   0x505      // R/W   Bitstream Buffer Status. Write to change bit offset and Buffer tail pointer.
#define AUX_VLC_SETUP         0x506      // R/W   Setup register: Decoder select and interrupt ctrl
#define AUX_VLC_BITS          0x507      // R     Returns the next 32bits from the bitstream
#define AUX_VLC_TABLE         0x508      // R/W   Table select
#define AUX_VLC_GET_SYMBOL    0x509      // R     Read decoded Symbol and update offset
#define AUX_VLC_READ_SYMBOL   0x50A      // R     Read decoded Symbol only
// Reserved -- AUX_VLC_GET_LENGTH    0x50B
// Reserved -- AUX_VLC_READ_LENGTH   0x50C
// Reserved -- AUX_VLC_ADD_OFFSET    0x50D
// Reserved -- AUX_VLC_GET_BIT       0x50E
#define AUX_UCAVLC_SETUP      0x510      // R/W   UVLC and CAVLC Decode Setup Register
#define AUX_UCAVLC_STATE      0x511      // R/W   UVLC and CAVLC Decode Intermedate State Values Registers
#define AUX_CAVLC_ZERO_LEFT   0x512      // R/W   CAVLC Zero Run decode Zero Left Register. (Must be 00 00 03)
// Reserved -- AUX_UVLC_SYMBOL       0x513
// Reserved -- AUX_UVLC_I_STATE      0x514
// Reserved -- AUX_UVLC_SR_STATE     0x515

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

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
