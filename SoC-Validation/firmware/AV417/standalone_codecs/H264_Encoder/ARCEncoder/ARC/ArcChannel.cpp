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




#ifdef _DEBUG
#include <assert.h>
#endif

#include "defines.h"
#include "ArcChannelRoutines.h"

#include <arc/arc_avu.h>

#include "ArcMacroRecordSettings.h"

static void enable_channel(int chan_num)
{
#ifndef __ARC_LINUX__
    // First we need to make sure that the channels are diabled before we can
    // change anything
    _sr(0,  MacroSetting_ChannelCtrl);

    // select channel
    _sr(chan_num << 1, MacroSetting_ChannelCtrl); // (val,aux_addr)

    // enable it
    _sr(1, MacroSetting_ChannelStatus);

    // Finally we can enable all channels
    _sr(0x8000, MacroSetting_ChannelCtrl); // (val,aux_addr)
#else
    extern "C" int arc_enable_channel(int chan_num, int is_enable);

    arc_enable_channel(chan_num, 1);
#endif
}


// Channel specific Initialization command go here
void Initialise_Channels(void)
{
    enable_channel(MacroSetting_ChannelCmd_ArcToEE >> 8);
    enable_channel(MacroSetting_ChannelCmd_EEToArc >> 8);

    enable_channel(MacroSetting_ChannelCmd_ArcToME >> 8);
    enable_channel(MacroSetting_ChannelCmd_METoArc >> 8);

    enable_channel(MacroSetting_ChannelNum_MP00ToArc);
    enable_channel(MacroSetting_ChannelNum_MP01ToArc);

    enable_channel(MacroSetting_ChannelCmd_TimerToArc >> 8);

}



// the get channel response function can't take any arguments and must only contain
// the _chop and _chex for the hardware or ISS build otehrwise the an additional level
// of frame pointer setup is done which causes the wrong frame table to be restored
// when the actual channel response function (called by _chex) reutrns using the
// branch link register of this getchannelresponse function to the level above.

CHANNEL_SERVICE Service_GetChannelResponse(void)
{
    _chop();
    _chex();
    _chex();
    _chex();
    _chex();
    _chex();
}


