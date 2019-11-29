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

/* May contain code (c) British Telecommunications plc 2005        */

#ifndef ARCMACRORECORDSETTINGS_H_
#define ARCMACRORECORDSETTINGS_H_

#include "typdef.h"


// These vars are accessed from assembler 
// while recoding SIMD macros
//STRUCT MacrocSetting ---------------------


// channel auxiliary register addresses
extern u_int32 MacroSetting_ChannelCtrl;
extern u_int32 MacroSetting_ChannelStatus;

// channel command numbers (channel num no shift)
extern u_int32 MacroSetting_ChannelNum_MP01ToArc;
extern u_int32 MacroSetting_ChannelNum_MP00ToArc;

// channel commands (channel num shifted)
extern u_int32 MacroSetting_ChannelCmd_ArcToME;
extern u_int32 MacroSetting_ChannelCmd_ArcToEE;

extern u_int32 MacroSetting_ChannelCmd_METoArc;
extern u_int32 MacroSetting_ChannelCmd_EEToArc;

extern u_int32 MacroSetting_ChannelCmd_TimerToArc;
extern u_int32 MacroSetting_ChannelCmd_ArcToTimer;
//END STRUCT------------------------------



#endif /*ARCMACRORECORDSETTINGS_H_*/
