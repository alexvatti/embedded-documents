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



#include "stdlib.h"
#include "VRaptorInfo.h"
#include <string.h>
#include "ArcChannelRoutines.h"
#include "typdef.h"
#include "assert.h"
#include "ArcMacroRecordSettings.h"


// global instaniation
VRaptorInfo VRaptorDevs;
int SCQ1; // Is 1 when second SIMD unit exists
int HwMe; // 1 = Hardware ME is present, 0 = ME must be performed in software
int HwChTm; // 1 = Hardware channel TIMER is present, 0 = dynamic encoding cannot be used
int HwVlc; // 1 = Hardware VLC is present, 0 = VLC must be performed in software
char ProductName[7];


//******************************************************************************
// Name:          VRaptorInfo::VRaptorInfo
// Purpose:       Constructor
// Arguments:     void
// Return Value:  
//******************************************************************************
VRaptorInfo::VRaptorInfo(void)
{
    Channels = NULL;
}


//******************************************************************************
// Name:          VRaptorInfo::~VRaptorInfo
// Purpose:       Deconstructor
// Arguments:     void
// Return Value:  
//******************************************************************************
VRaptorInfo::~VRaptorInfo(void)
{
    // delete channel connectivity information
    if (Channels != NULL) delete [] Channels;
}

#ifdef __ARC_LINUX__
extern "C" unsigned get_ccr(int idx);
extern "C" unsigned get_vconfig(const char *name);
extern "C" unsigned get_aux_base(const char *name);
extern "C" unsigned arc_num_channels(void);
extern "C" unsigned get_vlc_build(void);
#endif


//******************************************************************************
// Name:          VRaptorInfo::GetDeviceAddr
// Purpose:       Scans the devices in the auxiliary register space looking for
//                a particular device. It then returns the address of this
//                device
// Arguments:     devAddr = pointer to where to write the address of the found
//                          device to
//                devName = name of the device we're looking for
// Return Value:  bool = true if we found the device otherwise false
//******************************************************************************
#ifndef __ARC_LINUX__
bool VRaptorInfo::GetDeviceAddr (u_int32 & devAddr, const char * devName)
{
    u_int32 baseAddr = AUXREG_DOMAIN_START;
    u_int32 vconfig;
    u_int8 n_tstr;
    u_int8 n_istr;
    u_int8 n_aux;

    u_int8 namestrLen;
    u_int32 namestrAddr;
    u_int32 namestrWord = 0; // initialisation to stop compiler complaing about possible reference before set
    int namestrCount;
    char * namestrPtr;
    
    #define MAX_NAMESTR_LEN 256
    char namestr[MAX_NAMESTR_LEN];

    int byteNum;
    
    bool found = false;
    
    int devCount = 1;
    
    // loop searching for the device
    while (!found)
    {
        // get vconfig info for a device
        vconfig = _lr(baseAddr);
        n_tstr = (vconfig >> VCONFIG__N_TSTR_SHIFT) & VCONFIG__N_TSTR_MASK;
        n_istr = (vconfig >> VCONFIG__N_ISTR_SHIFT) & VCONFIG__N_ISTR_MASK;
        n_aux = (vconfig >> VCONFIG__N_AUX_SHIFT) & VCONFIG__N_AUX_MASK;
    
        // length in words
        namestrLen = n_tstr + n_istr;
        namestrAddr = baseAddr + 1; // namestr follows vconfig
        namestrCount = 0;
        namestrPtr = namestr;
        
        // build the device name a word at a time
        while (namestrCount < namestrLen)
        {
            byteNum = namestrCount & 0x3;
        
            if (byteNum == 0)
            {
                namestrWord = _lr(namestrAddr++);
            }

#pragma Off(Behaved)            
            *namestrPtr++ = *(((char *) &namestrWord) + byteNum);
#pragma On(Behaved)            
            
            namestrCount++;
        }

        // add termination for string compare
        *namestrPtr = '\0';
    
        // need to allow for the address of the channel device to be found and
        // returned too
        if (strcmp(devName, namestr)==0)
        {
            // device found
            found = true;
        }
        else if (devCount >= Creo.NumMods)
        {
            // have tried all devices without finding the one we want
        	return(false);
        }
        else
        {
            // goto next device
            baseAddr += n_aux;
            devCount++;
        }
    }

    // write back the address of the device found
    devAddr = baseAddr;
    return true;
}
#endif

//******************************************************************************
// Name:          VRaptorInfo::FindChannelsDevice
// Purpose:       Scans for the channels device and then extracts the list of
//                channels and the associated IDs. It also sets up some of the
//                global addresses
// Arguments:     void
// Return Value:  bool = true if we found the channels information otherwise false
//******************************************************************************
bool VRaptorInfo::FindChannelsDevice (void)
{
    u_int32 chanDevAddr;
    u_int32 chanCCR;

#ifndef __ARC_LINUX__
    // find channels device
    if (!GetDeviceAddr(chanDevAddr, CHANNEL_NAMESTR))
    {
        ENC_PRINTF(("VRaptorInfo: Error - Unable to find %s (channels) device\n", CHANNEL_NAMESTR));
        assert(false);
        return(false);
    }

    // get total number of channels
    Creo.NumChannels = (_lr(chanDevAddr + CHANNEL_BUILD_ADDR) >> CHANNEL_BUILD__NCHANNELS_SHIFT) & CHANNEL_BUILD__NCHANNELS_MASK;
#else 
    Creo.NumChannels = arc_num_channels();
#endif

    
    Channels = new ChannelCCR[Creo.NumChannels];
    
    // loop through all channels and build a list of which devices they connect
    for (int chanNum=0; chanNum<Creo.NumChannels; chanNum++)
    {
#ifndef __ARC_LINUX__
	chanCCR=_lr(chanDevAddr + CHANNEL_CCR_ADDR + chanNum);
#else
        chanCCR= get_ccr(chanNum);
#endif

        Channels[chanNum].ReceiverID = (chanCCR >> CHANNEL_CCR__RECEIVER_ID_SHIFT) & CHANNEL_CCR__RECEIVER_ID_MASK;
        Channels[chanNum].SenderID = (chanCCR >> CHANNEL_CCR__SENDER_ID_SHIFT) & CHANNEL_CCR__SENDER_ID_MASK;
	//ENC_PRINTF("Channel %d from %d to %d\n", chanNum, Channels[chanNum].SenderID, Channels[chanNum].ReceiverID);
    }

    // setup global channel addresses
    MacroSetting_ChannelCtrl = chanDevAddr + CHANNEL_CTRL_ADDR;
    MacroSetting_ChannelStatus = chanDevAddr + CHANNEL_STATUS_ADDR;

    return true;
}


//******************************************************************************
// Name:          VRaptorInfo::FindDevice
// Purpose:       Scans for a device and then extracts the mod_id used for
//                identifying channel connectivity
// Arguments:     void
// Return Value:  bool = true if we found the device information otherwise false
//******************************************************************************
#ifndef __ARC_LINUX__
bool VRaptorInfo::FindDevice(Device & devRef, const char * devName)
{
    // find the device
    if (!GetDeviceAddr(devRef.AuxBase, devName))
    {
        ENC_PRINTF(("VRaptorInfo: Unable to find device - %s\n", devName));
        devRef.ModID = -1;
        return false;
    }
    
    ENC_PRINTF(("VRaptorInfo: Found device - %s\n", devName));

    // get mod_id, used for identifying channel connectivity
    devRef.ModID = (_lr(devRef.AuxBase) >> VCONFIG__MOD_ID_SHIFT) & VCONFIG__MOD_ID_MASK;

    return true;
}
#else
bool VRaptorInfo::FindDevice(Device & devRef, const char * devName)
{
    // find the device
    unsigned vconfig = get_vconfig(devName);
    
    // get mod_id, used for identifying channel connectivity
    devRef.ModID = (vconfig >> VCONFIG__MOD_ID_SHIFT) & VCONFIG__MOD_ID_MASK;
    devRef.AuxBase = get_aux_base(devName);

    return ((devRef.ModID == 0xff) || (devRef.AuxBase == 0xffffffff)) ? false : true;
}
#endif


//******************************************************************************
// Name:          VRaptorInfo::FindChannel
// Purpose:       Scans for a device and then extracts the mod_id used for
//                identifying channel connectivity
// Arguments:     channel = a channel number/command variable to update
//                sendDevModID = mod_id of the sending device
//                recvDecModID = mod_id of the receiving device
//                shift = whether to shift the channel number found before
//                        assigning to the channel argument
// Return Value:  bool = true if we found the channel connectivity otherwise false
//******************************************************************************
bool VRaptorInfo::FindChannel(u_int32 & channel, int sendDevModID, int recvDecModID, int shift)
{
    bool found = false;
    u_int8 chanNum = 0;

    if(sendDevModID < 0 || recvDecModID < 0)
    {
    	ENC_PRINTF(("VRaptorInfo: Note - Channel for device Id -1 requested (-1 returned)\n"));
    	channel = -1;
    	return false;
    }
    
    // loop through all channel connectivity
    while (!found)
    {
        // check the sender and reciever mod_ids
        if (Channels[chanNum].SenderID==sendDevModID &&
            Channels[chanNum].ReceiverID==recvDecModID)
        {
            if (shift)
            {
                // shift channel num here so we only have to 'or' in the channel reg later
                channel = (chanNum & CHAN_CMD__CHAN_NUM_MASK) << CHAN_CMD__CHAN_NUM_SHIFT;
            }
            else
            {
                // don't shift shift channel num
                channel = chanNum;
            }
            
            found = true;
        }
        else
        {
            // increament channel iterator
            chanNum++;

            // check to see if we've tried all channels
            if (chanNum >= Creo.NumChannels)
            {
                ENC_PRINTF(("VRaptorInfo: Error - Unable to find channel connectivity for sender (%d) and receiver (%d)\n", sendDevModID, recvDecModID)); 
                assert(false);
                return false;
            }
        }
    }

    return true;
}


//******************************************************************************
// Name:          VRaptorInfo::FindDomainInfo
// Purpose:       Scans for a device and then extracts the mod_id used for
//                identifying channel connectivity
// Arguments:     void
// Return Value:  bool = true if this arc is the domain master otherwise false
//******************************************************************************
bool VRaptorInfo::FindDomainInfo(void)
{
    // read this arc's build configuration register
#ifndef __ARC_LINUX__
    u_int32 vraptor_bcr = _lr(VRAPTOR_BCR_ADDR);
#else
    extern unsigned g_vraptor_bcr;
    u_int32 vraptor_bcr = g_vraptor_bcr; // need to get from API
#endif
    
    // determain if we're the domain master
    u_int8 vraptor_version = (vraptor_bcr >> VRAPTOR_BCR__VRAPTOR_VER_SHIFT) & VRAPTOR_BCR__VRAPTOR_VER_MASK;

    if (vraptor_version == VRBCR_VRVER_DOMAIN_SLAVE)
    {
        ENC_PRINTF(("VRaptorInfo: Error - This Arc isn't the domain master and so has no channel\n"));
        ENC_PRINTF(("    auxiliary register space and therefore can't perform channel connectivity.\n"));
        assert(false);
        return false;
    }

    // setup domain master's mod_id for channel connectivity
    ArcMaster.ModID = ARC_DOMAIN_MASTER_MOD_ID;
    
    // get the number of devices in the auxiliary register space of the domain master
    Creo.NumMods = (vraptor_bcr >> VRAPTOR_BCR__N_MOD_SHIFT) & VRAPTOR_BCR__N_MOD_MASK;

    return true;
}



//******************************************************************************
// Name:          VRaptorInfo::Init
// Purpose:       Scans for a device and then extracts the mod_id used for
//                identifying channel connectivity
// Arguments:     void
// Return Value:  true if initialisation was successful, otherwise false
//******************************************************************************
bool VRaptorInfo::Init(void)
{
    // set domain information
    FindDomainInfo();

    // get channel connectivity
    FindChannelsDevice();

    // get device mod_ids for channel connectivity
    // find simds
    if (FindDevice(MediaProc0, "MP0"))
   	{
        SCQ1 = 0; // 1 simd
   	}
    else
    {
        ENC_PRINTF(("VRaptorInfo: Error - Cannot find SIMD MP0\n"));
        ENC_PRINTF(("VRaptorInfo: Exiting due unsupported hardware configuration\n"));
        return(false);
    }

    if (FindDevice(MediaProc1, "MP1"))
   	{
        SCQ1 = 1; // 2 simds
   	}

    // find entropy encoder
    if (!FindDevice(EntropyEncoder, "EE"))
    {
        ENC_PRINTF(("VRaptorInfo: Error - Cannot find EE\n"));
        ENC_PRINTF(("VRaptorInfo: Exiting due unsupported hardware configuration\n"));
        return(false);
    }

    // find motion estimator
    if (FindDevice(MotionEstimator, "ME"))
    {
        HwMe = 1;
    }
    else
    {
        HwMe = 0;

        // ME is in software so set base addresses to zero
        VRaptorDevs.MotionEstimator.AuxBase = 0;
        MacroSetting_ChannelCmd_ArcToME = 0;
    }

    // find channel timer
    if (FindDevice(Timer, "TIMER"))
    {
    	HwChTm = 1;
    }
    else
    {
    	HwChTm = 0;
    }

    // find variable length decoder
#ifndef __ARC_LINUX__
    unsigned char vlcBuild_VersionNumber = _lr(0x0fc) & 0xff;
#else
    unsigned char vlcBuild_VersionNumber = get_vlc_build() & 0xff;
#endif

    if (vlcBuild_VersionNumber == 1)
    {
        ENC_PRINTF(("VRaptorInfo: Found device - VLC\n"));
    	HwVlc = 1;
    }
    else
    {
        ENC_PRINTF(("VRaptorInfo: Unable to find device - VLC\n"));
        HwVlc = 0;
    }


    // infer the arc video subsystem product name
    if (SCQ1 == 1)
    {
        strcpy(ProductName, "AV417V");
    }
    else
    {
    	if (HwMe == 1)
    	{
            if (HwVlc == 1)
            {
                strcpy(ProductName, "AV407V");
            }
            else
            {
                strcpy(ProductName, "AV406V");
            }
    	}
    	else
    	{
            if (HwVlc == 1)
            {
                strcpy(ProductName, "AV404V");
            }
            else
            {
                strcpy(ProductName, "AV402V");
            }
        }
    }

    ENC_PRINTF(("VRaptorInfo: Hardware configuration infers product name is %s\n", ProductName));


    // interogate the channel connectivity we want and expect. this will change
    // for each vraptor architecture 
    // setup channel numbers
    #define FINDCHAN_NOSHIFT 0

    FindChannel(MacroSetting_ChannelNum_MP00ToArc, MediaProc0.ModID, ArcMaster.ModID, FINDCHAN_NOSHIFT);
    if (SCQ1)
    {
    	FindChannel(MacroSetting_ChannelNum_MP01ToArc, MediaProc1.ModID, ArcMaster.ModID, FINDCHAN_NOSHIFT);
    }
    else
    {
    	MacroSetting_ChannelNum_MP01ToArc = MacroSetting_ChannelNum_MP00ToArc;	// All on SIMD 0
    }

    // setup channel commands (i.e. channel numbers shifted)
    #define FINDCHAN_SHIFT 1
    if (HwMe == 1)
    {
        FindChannel(MacroSetting_ChannelCmd_ArcToME, ArcMaster.ModID, MotionEstimator.ModID, FINDCHAN_SHIFT);
        FindChannel(MacroSetting_ChannelCmd_METoArc, MotionEstimator.ModID, ArcMaster.ModID, FINDCHAN_SHIFT);
    }

    FindChannel(MacroSetting_ChannelCmd_ArcToEE, ArcMaster.ModID, EntropyEncoder.ModID, FINDCHAN_SHIFT);
    FindChannel(MacroSetting_ChannelCmd_EEToArc, EntropyEncoder.ModID, ArcMaster.ModID, FINDCHAN_SHIFT);

    FindChannel(MacroSetting_ChannelCmd_TimerToArc, Timer.ModID, ArcMaster.ModID, FINDCHAN_SHIFT);

    // enable the channels 
    Initialise_Channels();

    // setup the sdm base address
#ifndef __ARC_LINUX__
    SDM_Base = (char *)_lr(MediaProc0.AuxBase+0xa);
#else
    extern char *sdm_virt_mapping;
    SDM_Base = sdm_virt_mapping;
#endif

    // everything initialisated successfully
    return(true);
}
