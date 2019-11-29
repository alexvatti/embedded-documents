
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
/*                                                                 */



#include "VRaptorInfo.h"



#define AUXREG_TIMER_VCONFIG                 (VRaptorDevs.Timer.AuxBase + 0)
#define AUXREG_TIMER_NAMESTR                 (VRaptorDevs.Timer.AuxBase + 1)
#define AUXREG_TIMER_NAMESTR1                (VRaptorDevs.Timer.AuxBase + 2)
#define AUXREG_TIMER_CTIMER_CTRL	         (VRaptorDevs.Timer.AuxBase + 3)
#define AUXREG_TIMER_CTIMER_COUNT	         (VRaptorDevs.Timer.AuxBase + 4)
#define AUXREG_TIMER_CTIMER_HIT		         (VRaptorDevs.Timer.AuxBase + 5)
#define AUXREG_TIMER_CTIMER_LIMIT            (VRaptorDevs.Timer.AuxBase + 6)
#define AUXREG_TIMER_CTIMER_MSG0_CMD         (VRaptorDevs.Timer.AuxBase + 7)
#define AUXREG_TIMER_CTIMER_MSG1_CMD         (VRaptorDevs.Timer.AuxBase + 8)
#define AUXREG_TIMER_CTIMER_MSG1_DATA        (VRaptorDevs.Timer.AuxBase + 9)