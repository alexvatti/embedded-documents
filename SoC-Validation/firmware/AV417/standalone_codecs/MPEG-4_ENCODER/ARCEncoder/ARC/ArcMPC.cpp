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





#include <stdio.h>
#include "ArcMPC.h"



// cMPCcommon member functions
cMPCcommon::cMPCcommon ()
{
}

cMPCcommon::~cMPCcommon ()
{
}


// cMPCfb member functions
cMPCfb::cMPCfb ()
{
   for(int i=0;i<4;i++)
      Intra4x4ModeCoeffCountSB[i] = 0;
}

cMPCfb::~cMPCfb ()
{
}


// cMPCcb member functions
cMPCcb::cMPCcb ()
{
}

cMPCcb::~cMPCcb ()
{
}


// cMPC member functions
cMPC::cMPC (int picture_width)
{
   FixedBuffer=NULL;
   CircularBuffer=NULL;
   cbSize=0;
   Stage1Pos=-1;
   Stage2Pos=-1;
   Stage3Pos=-1;
   Stage4Pos=-1;
   Stage1Waiting=0;
}

cMPC::~cMPC ()
{
}
