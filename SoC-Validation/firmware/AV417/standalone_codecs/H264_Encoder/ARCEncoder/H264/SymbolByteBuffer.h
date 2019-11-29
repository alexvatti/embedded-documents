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

/* ------------------------------------------------------------------------ */
/* FILE:			BTSymbomByteBuffer.h									*/
/* DESCRIPTION:		BT Class for writing symbols to byte buffers, reading,  */
/*					saving and restoring status, copying etc				*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef _BT_SYMBOL_BYTE_BUFFER_H_
#define _BT_SYMBOL_BYTE_BUFFER_H_

#ifdef __SYMBIAN32__
	#include "stdafx.h"
#endif

#include "defines.h"

#define BUF_STORE_LEN 256 // Number of physical buffers that can be held by instances of this class

class CSymbolByteBuffer
{
public:
	CSymbolByteBuffer(void);
	~CSymbolByteBuffer(void);

	bool Create(int size);

	int GetBitCount(void)
	{
    return (m_BitCount + (m_EmuByteCnt << 3));
	}

  void AddBitCount(int val)
	{
		m_BitCount += val;
	}
   
  void SetEmuByteCnt(int val)
	{
		m_EmuByteCnt = val;
	}

  int GetEmuBitCount(void)
  {
    return m_EmuByteCnt << 3;
  }


	int GetLength()
	{
    return ((m_BitCount >> 3) + m_EmuByteCnt);
	}

  void ResetBuffer(void);

  bool IsCreated() { return m_Created; }

	CSymbolByteBuffer * m_Next;

private:
	bool m_Created;
	int m_Size;

public:
	unsigned char * m_BufferPointer;		// Points to beginning of the buffer or 0 if no buffer.

private:
    unsigned char *m_BufferPtrAlocated;
	int	m_BitCount;							// Total number of bits in the buffer
  int   m_EmuByteCnt;                 // Bytes inserted by emulation prevention logic


#ifdef ARC_ENCODER_API
public:
    unsigned char * m_Buffer;		            // Points to buffer as allocated by API.
	unsigned char * m_PhysBufferPointer;		// Points to physical beginning of the buffer or 0 if no buffer.
	int m_amount;
	void QueueBuffer(void);

	unsigned char* buffers[BUF_STORE_LEN];
	unsigned char* phys_buffers[BUF_STORE_LEN];
	int numbufs;

private:
	void SetupDMABuffer();
#endif
};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
