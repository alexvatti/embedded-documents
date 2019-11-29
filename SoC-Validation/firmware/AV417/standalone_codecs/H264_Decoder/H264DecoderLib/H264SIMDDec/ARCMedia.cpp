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

// ARCMedia.cpp : Defines the entry point for the console application.
//

#if defined _WIN32
	#include "StdAfx.h"
#else
	#include <stdio.h>
	#include <assert.h>
	#include <stream.h>
#endif
//#include "Aurora.h"
#include "ARCMedia.h"

#define _CYCLIC_ALLOCATION_

#define SDM_SIZE (1024*32)


#define AUX_SDM_BASE 0xA6

_Asm unsigned int GetSDMBase(void)
{
    lr %r0, [AUX_SDM_BASE ]
    nop
    nop
    nop
}




ARCMedia::ARCMedia()
{

//	cout << "Arc SIMD Test bench class (ARCMedia) instance created" << endl;
	SDM_Buffers.SetSDM_BaseAddress((void *)GetSDMBase());

	//fprintf(stderr,"SDM BASE ADDRESS = 0x%X \n", (unsigned int)SDM_StartAddr);
	//fflush(stdout);

	//use the next 16byte aligned address
	//SDM_NextFreeMemPtr = (void *)((( (unsigned int)SDM_StartAddr + 0xf)>>4)<<4);

    if((int)SDM_Buffers.GetSDM_BaseAddress() & 0xf)
    {
        printf("Error BaseSDM 0x%08X not aligned\n",SDM_Buffers.GetSDM_BaseAddress());
    }
//    FreePtrSDM = 0;
}

ARCMedia::~ARCMedia()
{

}



//ARCMedia & ARCMedia::GetInstance()
//{
//    assert(0);
//	static ARCMedia instance;
//	return instance;
//}


#if defined __ARC_MPLAYER__
bool ARCMedia::SetSDMStartAddr (void *baseSDM)
{
  if (baseSDM == NULL)
    return (false);
  SDM_Buffers.SetSDM_BaseAddress(baseSDM);
//  SDM_NextFreeMemPtr = (void *)((( (unsigned int)BaseSDM + 0xf)>>4)<<4);
  fprintf(stderr,"\nSetting BaseSDM to 0x%.8X",baseSDM);
  return (true);
}
#endif

//// Returns -1 for error
//SDM_Offset ARCMedia::MallocSDM(size_t reqBufferSize)
//{
//	SDM_Offset retOffset = -1; // Default error
//	unsigned int actualBufferSize = 0;
//
//	// always allocate buffer in multiple of 16 bytes
//	actualBufferSize = (reqBufferSize+0x0f) & ~0x0f;
//
//	//Compute the current buffer size
//
//	// If (requested_buffer_size + current_buffer_size) > SDM_Size return NULL
//	if( (FreePtrSDM + actualBufferSize) < SDM_SIZE )
//	{
//		retOffset = FreePtrSDM;
//		FreePtrSDM += actualBufferSize;
//	}
//    else
//    {
//    	fprintf(stderr,"\nBufferSize requested = 0x%x, available = 0x%x\n, SDM_StartAddr= 0x%x",
//    		reqBufferSize, SDM_SIZE-FreePtrSDM, BaseSDM);
//    	fflush(stderr);
//    }
//
//	// return pointer to next free buffer and incremenet
//    printf(" Offset 0x%04X Req 0x%04X Actual 0x%04X\n", retOffset,reqBufferSize,actualBufferSize);
//	return retOffset;
//}

//void ARCMedia::FreeSDM(void *bufferPtr)
//{
//	//Free is not supported so just assert
//	assert(false);
//	return;
//}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	AuroraH264Dec::						*/
/* DESCRIPTION:	Gets a pointer to new  buffer						*/
/* RETURNS:		Pointer to a  buffer								*/
/* ------------------------------------------------------------------------ */
_Asm void ClearStatusWord(unsigned int sdm_add)
{
	% reg  sdm_add 
.off	percent_reg

    vmovw	vr00, sdm_add, 2	;	// load i1 with r1
    vsubw   vr01, vr01, vr01    ;   // clear vr01

    vst128  vr01, [i1, 0]       ;   // store vr01
.on	percent_reg

}


bool SDM_BufCtrl::GetSDMBuffers()
{
//    printf("GetSDMBuffers\n");
	int index;

	LastBufferOffset = bufferPtr_ofs;;

	//clear the status word
    //ClearStatusWord( (unsigned int)statusWord - (unsigned int)SDM_StartAddr );
    short *statusWords = (short *)((int)BaseSDM + bufferPtr_ofs + SDM_ofs_status);
	for(index=0; index<MAXIMUM_NUMBER_OF_MACROBUFFERS; index++)
	{
		* statusWords = 0;
		statusWords += SDM_ofs_size/sizeof(short);
	}
	return true;
}


void SDM_BufCtrl::PrintBufferLock()
{
	
	fprintf(stderr,"Timeout in GetNextSDMBuffer\n");
//    for(int index=0; index< MAXIMUM_NUMBER_OF_MACROBUFFERS; index++)
//        fprintf(stderr,"0x%X  ", StatusWords[index]);
    
    fprintf(stderr,"\n");
}



//void ARCMedia::FreeAllSDMBuffers(SDM_BUF_CNTL_T * cntlPtr)
//{
//	FreeSDM(cntlPtr->BufferPtr);
//	cntlPtr->BufferPtr = NULL;
//
//	FreeSDM(cntlPtr->StatusWord);
//	cntlPtr->StatusWord = NULL;
//
//}

//SDM_Offset ARCMedia::GetFreePtrSDM()
//{
//	return FreePtrSDM;
//}


_Asm AsmSetDMAFrame(int offset, unsigned int baseAddr, unsigned int stride, unsigned int xDim, unsigned int yDim)
{
    %reg offset, baseAddr, stride, xDim, yDim
    
    vmovw       %vr00,offset,0x10       // i4

    vmovw       %vr01, baseAddr,0x1
    lsr         baseAddr, baseAddr,16
    vmovw       %vr01, baseAddr,0x2
    
    vmovw       %vr01, stride,0x4
    lsr         stride, stride,16
    vmovw       %vr01, stride,0x8

    vmovw       %vr01, xDim,0x10
    lsr         xDim, xDim,16
    vmovw       %vr01, xDim,0x20

    vmovw       %vr01, yDim,0x40
    lsr         yDim, yDim,16
    vmovw       %vr01, yDim,0x80

    vst128       %vr01,[%i4,0]
}


void ARCMedia::SetDMAFrame(int offset, unsigned int baseAddr, unsigned int stride, unsigned int xDim, unsigned int yDim)
{
    AsmSetDMAFrame(offset, baseAddr, stride, xDim-1, yDim-1);
}



