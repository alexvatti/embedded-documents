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




#ifndef VRAPTER_INFO_H
#define VRAPTER_INFO_H



#include "typdef.h"

extern int SCQ1;// Is 1 when second SIMD unit exists
extern int HwMe; // 1 = Hardware ME is present, 0 = ME must be performed in software
extern int HwChTm; // 1 = Hardware channel TIMER is present, 0 = dynamic encoding cannot be used
extern int HwVlc; // 1 = Hardware VLC is present, 0 = VLC must be performed in software
extern char ProductName[7];

_Asm void SetSCQ(int q)
{
	%reg q
	scq	0,q
}



// arc register addresses
#define VRAPTOR_BCR_ADDR 0xf9

// vraptor build configuration register definition
#define VRAPTOR_BCR__VRAPTOR_VER_SHIFT 0
#define VRAPTOR_BCR__VRAPTOR_VER_MASK  0xff
#define VRAPTOR_BCR__N_MOD_SHIFT 8
#define VRAPTOR_BCR__N_MOD_MASK  0xff

// vraptor version field values of vraptor build configuration register 
#define VRBCR_VRVER_DOMAIN_SLAVE  0
#define VRBCR_VRVER_DOMAIN_MASTER 1

// arc mod_ids
#define ARC_DOMAIN_MASTER_MOD_ID 0

// start of all auxiliary registers in the current domain
#define AUXREG_DOMAIN_START  0x1000

// vconfig auxiliary register definition
#define VCONFIG__VERSION_SHIFT 0
#define VCONFIG__VERSION_MASK  0xff
#define VCONFIG__N_TSTR_SHIFT  8
#define VCONFIG__N_TSTR_MASK   0x1f
#define VCONFIG__N_ISTR_SHIFT  13
#define VCONFIG__N_ISTR_MASK   0x7
#define VCONFIG__MOD_ID_SHIFT  16
#define VCONFIG__MOD_ID_MASK   0xff
#define VCONFIG__N_AUX_SHIFT   24
#define VCONFIG__N_AUX_MASK    0xff

// namestr for the channels device 
#define CHANNEL_NAMESTR "CHAN"

// address offsets from channel device base address
#define CHANNEL_VCONFIG_ADDR 0x0
#define CHANNEL_NAMESTR_ADDR 0x1
#define CHANNEL_BUILD_ADDR   0x2
#define CHANNEL_CTRL_ADDR    0x3
#define CHANNEL_STATUS_ADDR  0x4
#define CHANNEL_DATA_ADDR    0x5
#define CHANNEL_CCR_ADDR     0x6

// channel build auxiliary register definition
#define CHANNEL_BUILD__NCHANNELS_SHIFT 8
#define CHANNEL_BUILD__NCHANNELS_MASK  0xff

// channel ccr auxiliary register definition
#define CHANNEL_CCR__RECEIVER_ID_SHIFT 0
#define CHANNEL_CCR__RECEIVER_ID_MASK  0xff
#define CHANNEL_CCR__SENDER_ID_SHIFT   8
#define CHANNEL_CCR__SENDER_ID_MASK    0xff

// channel command chsd definition
#define CHAN_CMD__CHAN_REG_SHIFT 0
#define CHAN_CMD__CHAN_REG_MASK  0xff
#define CHAN_CMD__CHAN_NUM_SHIFT 8
#define CHAN_CMD__CHAN_NUM_MASK  0xff



//******************************************************************************
// Name:          VRaptorInfo
// Purpose:       Initialise the VRaptor platform
//******************************************************************************
class VRaptorInfo
{
public:

    // public variables

    struct Domain
    {
        u_int8 NumMods;
        u_int8 NumChannels;
    };
    
    Domain Creo;
    

    struct Device
	{
		u_int32 AuxBase;
        int ModID;
	};

    Device ArcMaster;
	Device MotionEstimator;
	Device EntropyEncoder;
	Device MediaProc0;
	Device MediaProc1;
	Device Timer;

    
    struct ChannelCCR
    {
        u_int8 SenderID;
        u_int8 ReceiverID;
    };

    ChannelCCR * Channels;
    
	char *SDM_Base;


    // public functions

	VRaptorInfo(void);
	~VRaptorInfo(void);

    bool GetDeviceAddr (u_int32 & devAddr, const char * devName);
    bool FindChannelsDevice (void);
	bool FindDevice(Device & devRef, const char * devName);
	bool FindChannel(u_int32 & channel, int sendDevModID, int recvDecModID, int shift);
	bool FindDomainInfo(void);

	bool Init(void);

};



extern VRaptorInfo VRaptorDevs;



#endif // VRAPTER_INFO_H
