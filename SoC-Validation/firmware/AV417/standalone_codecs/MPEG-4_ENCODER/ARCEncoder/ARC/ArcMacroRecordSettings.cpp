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

#include "ArcMacroRecordSettings.h"
#include "typdef.h"


// These vars are accessed from assembler 
// while recoding SIMD macros
//STRUCT MacrocSetting ---------------------


// channel auxiliary register addresses
u_int32 MacroSetting_ChannelCtrl;
u_int32 MacroSetting_ChannelStatus;

// channel command numbers (channel num no shift)
u_int32 MacroSetting_ChannelNum_MP01ToArc;
u_int32 MacroSetting_ChannelNum_MP00ToArc;

// channel commands (channel num shifted)
u_int32 MacroSetting_ChannelCmd_ArcToME;
u_int32 MacroSetting_ChannelCmd_ArcToEE;

u_int32 MacroSetting_ChannelCmd_METoArc;
u_int32 MacroSetting_ChannelCmd_EEToArc;


u_int32 MacroSetting_ChannelCmd_TimerToArc;
u_int32 MacroSetting_ChannelCmd_ArcToTimer;

// END STRUCT------------------------------


