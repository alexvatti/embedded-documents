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
/* FILE:			cavlc_api.h									            */
/* DESCRIPTION:		API definitions for H264 hardware accelerator 			*/
/* VERSION:			v1.00													*/
/* DATE:			16 August 2005											*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef	CAVLC_API_H
#define	CAVLC_API_H

#include"vlc_types.h"

#ifdef __cplusplus
extern "C" 
{
#endif 

#ifdef CAVLC_HW_PRESENT
#include "vlc_model.h"
#include "vlc_api.h"
#include "cavlc_model.h"

__FORCE_INLINE__ void UVLC_Set_Control(uint32 ctrlWord)
{
    cavlc_sr(AUX_UCAVLC_SETUP, ctrlWord);
}

__FORCE_INLINE__ void CAVLC_Set_Control(uint32 ctrlWord)
{
    cavlc_sr(AUX_UCAVLC_SETUP, ctrlWord);
}

__FORCE_INLINE__ void CAVLC_Reset()
{
    cavlc_sr(AUX_UCAVLC_SETUP, 0); 
    cavlc_sr(AUX_UCAVLC_STATE, 0); 
//    cavlc_sr(AUX_UVLC_I_STATE, 0); 
//    cavlc_sr(AUX_UVLC_SR_STATE, 0); 
}

__FORCE_INLINE__ uint32 CAVLC_Get_Symbol()
{
    return cavlc_lr(AUX_VLC_GET_SYMBOL);
}

__FORCE_INLINE__ uint32 CAVLC_Read_Symbol()
{
    return cavlc_lr(AUX_VLC_READ_SYMBOL);
}

__FORCE_INLINE__ uint32 CAVLC_Get_Zerosleft()
{
    return cavlc_lr(AUX_CAVLC_ZERO_LEFT);
}

__FORCE_INLINE__ void cavlc_sr(uint32 aux_addr, uint32 data)
{
    vlc_sr(aux_addr, data);
}

__FORCE_INLINE__ uint32 cavlc_lr(uint32 aux_addr)
{
    return  vlc_lr(aux_addr);
}

#endif

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
