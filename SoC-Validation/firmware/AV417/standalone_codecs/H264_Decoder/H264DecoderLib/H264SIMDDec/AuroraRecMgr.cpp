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

#ifdef _WIN32
#include "StdAfx.h"
#else
#include <stdio.h>
#endif

//#include "Aurora.h"
#include "ARCMedia.h"
#include "AuroraH264Dec.h"








AuroraRecMgr::AuroraRecMgr()
{
    NextAddr = 0;
    RecordDeblockMacros();

    RecordXformMacros();
    RecordLumaInterReconMacros();
    RecordChromaInterReconMacros();
    RecordChromaInterPredMacros();
    RecordChromaIntraPredMacros();
    RecordLuma16x16IntraPredMacros();
    RecordLuma4x4IntraPredMacros();
    RecordLumaInterPredVerFilterMacros();
    RecordLumaInterPredHorFilterMacros();
    RecordLumaCtrPelFilteringMacros();
    RecordLumaInterPredQPelFilterMacros();

    RecordSDMMemClearMacro();
	RecordDecodeSliceDataMacros();
	RecordIntraPCM_Macros();
//#ifdef INLOOP_DBLOCK
	RecordInloopDBMacros();
//#endif	
}

AuroraRecMgr::~AuroraRecMgr()
{
}
