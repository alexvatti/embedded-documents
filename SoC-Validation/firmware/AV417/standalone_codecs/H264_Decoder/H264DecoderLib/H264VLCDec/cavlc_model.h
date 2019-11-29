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
/* FILE:			cavlc_model.h									            */
/* DESCRIPTION:		The C model for H264 hardware accelerator 			*/
/* VERSION:			v1.00													*/
/* DATE:			16 August 2005											*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef	CAVLC_MODEL_H
#define	CAVLC_MODEL_H

#include"vlc_types.h"

#ifdef __cplusplus
extern "C" 
{
#endif 

uint32 cavlc_lr(uint32 aux_addr);
void cavlc_reset();

void cavlc_sr(uint32 aux_addr, uint32 aux_data);

// CAVLC control structure
typedef struct {
    unsigned symbolType:3;
    unsigned reserved1:1;
    unsigned blockType:2;
    unsigned reserved2:2;
    unsigned tokenType:3;
    unsigned reserved3:1;
    unsigned elemType:2;
    unsigned maxRange:1;
    unsigned blkPattern:1;
    unsigned reserved4:16;
} cavlc_ctrl_t;

// Token types used to select lookup tables for coefficient token decoding
#define TOKEN_VLD0          0
#define TOKEN_VLD1          1
#define TOKEN_VLD2          2
#define TOKEN_VLD3          3
#define TOKEN_CHROMA_DC     4

// Symbol types for CAVLC decoding
#define CAVLC_COEFF_TOKEN   0   
#define CAVLC_TRAILING_ONE  5    
#define CAVLC_COEFF_LEVEL   1   
#define CAVLC_TOTAL_ZERO    2  
#define CAVLC_RUN_BEFORE    3  
#define UVLC                4
/*
  [13:12] ELEM_TYPE, This field defines the element type to be decoded for UVLC
                       "00" : ue(v)
                       "01" : se(v)
                       "10" : me(v)
                       "11" : te(v)
*/
#define UE                  0x0
#define SE                  0x1
#define ME                  0x2
#define TE                  0x3
#define I1A3				0x5
#define UVLC_UE             0x0004
#define UVLC_SE             0x1004
#define UVLC_ME             0x2004
#define UVLC_TE             0x3004

#define CAVLC_BLK_INTRA_4X4 0x0000
#define CAVLC_BLK_INTER     0x8000

#define CAVLC_CHROMA_DC     0
#define CAVLC_CHROMA_DC2X2  1
#define CAVLC_LUMA4X4       2
#define CAVLC_OTHER_BLOCK   5

#define UVLC_TE_RANGE_EQ1   0x4000

// Auxiliary Registers

//      Register Name         Aux Address  Mode   Description
//      -------------         -----------  ----   ----------------------------------------------------------------------------
#define AUX_UCAVLC_SETUP      0x510      // R/W   UVLC and CAVLC Decode Setup Register
#define AUX_UCAVLC_STATE      0x511      // R/W   UVLC and CAVLC Decode Intermedate State Values Registers
#define AUX_CAVLC_ZERO_LEFT   0x512      // R     CAVLC Zero Run decode Zero Left Register
// Reserved -- AUX_UVLC_SYMBOL       0x513      // W     UVLC register for restoring SYMBOL decode result
// Reserved -- AUX_UVLC_I_STATE      0x514      // R/W   UVLC Internal states, for context saving/restoring
// Reserved -- AUX_UVLC_SR_STATE     0x515      // R/W   UVLC Internal Sihft register state, for context saving/restoring


#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
