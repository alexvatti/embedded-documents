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
/* FILE:			ARCMedia.h												*/
/* DESCRIPTION:		Interface for the Aurora class.							*/
/* DATE:															        */
/* ------------------------------------------------------------------------ */

#if !defined(_ARC_MEDIA_H_)
#define _ARC_MEDIA_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <arc/arc_avu.h>


#define SDM_NUMBER_OF_Y_PELS_PER_BLOCK				256
#define SDM_NUMBER_OF_U_PELS_PER_BLOCK				 64
#define SDM_NUMBER_OF_V_PELS_PER_BLOCK				 64

// Offesets inside buffers
enum sdm_macro_blk_data
{
	SDM_ofs_y_data		=	0,
	SDM_ofs_u_data		=	SDM_ofs_y_data + SDM_NUMBER_OF_Y_PELS_PER_BLOCK*sizeof(short),
	SDM_ofs_v_data		=	SDM_ofs_u_data + SDM_NUMBER_OF_U_PELS_PER_BLOCK*sizeof(short),
	
	SDM_ofs_y_out_buf	=	SDM_ofs_v_data + SDM_NUMBER_OF_V_PELS_PER_BLOCK*sizeof(short),
	SDM_ofs_u_out_buf	=	SDM_ofs_y_out_buf + SDM_NUMBER_OF_Y_PELS_PER_BLOCK*sizeof(char),
	SDM_ofs_v_out_buf	=	SDM_ofs_u_out_buf + SDM_NUMBER_OF_U_PELS_PER_BLOCK*sizeof(char),
	
	SDM_ofs_status		=	SDM_ofs_v_out_buf + SDM_NUMBER_OF_V_PELS_PER_BLOCK*sizeof(char),

	SDM_ofs_size		=	SDM_ofs_status + 8 * sizeof(short) 	// Only 1 word used
};

//#ifdef INLOOP_DBLOCK
#define MAXIMUM_NUMBER_OF_MACROBUFFERS 7
/*#else
#define MAXIMUM_NUMBER_OF_MACROBUFFERS 10
#endif*/

typedef int SDM_Offset;

_Asm void InlineFreeSDMBuffer(unsigned int index)
{
    % reg index
    vmovw	%vr00, index, 2     ;	// Load r1 in i1 with index

	vxor %vr01, %vr01, %vr01    ;	// clear a register
	
	vst16 %vr01, [%i1, 0]     ;	    // store 16-bit status word at the specified location
}


class SDM_BufCtrl
{
	void *BaseSDM;
    // SDM addresses, staticaly allocated
	SDM_Offset LastBufferOffset;	//index of the last allocated buffer
	void PrintBufferLock();	// Time out looking for new buffer
public:
    enum sdm_offsets
    {
//#ifdef INLOOP_DBLOCK
    	bufferPtr_ofs      =   0x5CA0,
/*#else    	
        bufferPtr_ofs      =   0x1370,
#endif*/        
    };
	inline void SetSDM_BaseAddress(void *base) {BaseSDM = base;}
	inline void *GetSDM_BaseAddress() { return BaseSDM;}
	bool GetSDMBuffers();
	
	// returns -1 on failure
	inline SDM_Offset GetNextSDMBuffer()
	{	
	
#if _DEBUG
		unsigned int timeOut = 0;
		while(++timeOut)
#else
		while(1)
#endif
		{
			LastBufferOffset += SDM_ofs_size;
			if(LastBufferOffset >= bufferPtr_ofs+MAXIMUM_NUMBER_OF_MACROBUFFERS*SDM_ofs_size)
			{
				LastBufferOffset = bufferPtr_ofs;
			}
			if(*(short *)((int)BaseSDM + LastBufferOffset + SDM_ofs_status) == 0)
			{
				*(short *)((int)BaseSDM + LastBufferOffset + SDM_ofs_status) = 1;
				return LastBufferOffset;
			}
		}
	
		//Print all the status-words and return error
	    PrintBufferLock();
		return -1;
	}


	inline void FreeSDMBuffer(SDM_Offset bufOfs)
	{		
	    InlineFreeSDMBuffer(bufOfs + SDM_ofs_status);
	}
	
};




//class ARCMedia : public Aurora  
class ARCMedia
{

public:
	// Construction
//	static ARCMedia & GetInstance();					// Singleton
	virtual ~ARCMedia();
#if defined __ARC_MPLAYER__
	bool SetSDMStartAddr (void *);
#endif 
	inline unsigned int GetSDMStartAdd() {return (unsigned int)SDM_Buffers.GetSDM_BaseAddress();}

	SDM_BufCtrl  SDM_Buffers;
    

	//SDM_Offset MallocSDM(size_t BufferSize);   // Returns -1 for error
	//void FreeSDM(void *bufferPtr);

	//SDM_Offset GetFreePtrSDM();



        void SetDMAFrame(int offset, unsigned int baseAddr, unsigned int stride, unsigned int xDim, unsigned int yDim);
protected:
	ARCMedia();			// Create via inheritance or singleton

};

/*
inline void FlushDCache()
{
    _sr(1, 0x47);            // DC_IVDC
//    while(_lr(0x48) & 256) {};
    _sync();
}
*/

_Asm void FlushDCache()
{
        sr      1,      [0x47]
        sync
}

_Asm void Sync()
{
        sync
}

#endif // !defined(_ARC_MEDIA_H_)
