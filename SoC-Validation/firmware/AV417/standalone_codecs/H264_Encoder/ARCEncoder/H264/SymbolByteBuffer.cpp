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
/* FILE:			SymbolByteBuffer.cpp									*/
/* DESCRIPTION:		Symbol byte buffer class								*/
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

#include	"SymbolByteBuffer.h"
#include	<assert.h>
#include	<string.h>

#ifdef ARC_ENCODER_API
#include <arc_encoder_malloc.h>
#include <packet_queue.h>
#include <CH264Encoder.h>
#include <h264_interface.h>
#include <ArcFrameBuffers.h>
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CSymbolByteBuffer											*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CSymbolByteBuffer::CSymbolByteBuffer(void)
{
	m_Created = false;
	
	m_BufferPointer = 0;
	m_BufferPtrAlocated = 0;
#ifdef ARC_ENCODER_API
	m_PhysBufferPointer = 0;
	m_amount = 0;
	m_Size = 0;
	for (int i = 0; i < BUF_STORE_LEN; i++)
	{
	    buffers[i] = 0;
	    phys_buffers[i] = 0;
	}
	numbufs = 0;
#endif

	m_BitCount = 0;
  m_EmuByteCnt = 0;
	
	m_Next = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	~CSymbolByteBuffer											*/
/* DESCRIPTION:	destructor													*/
/* ------------------------------------------------------------------------ */

CSymbolByteBuffer::~CSymbolByteBuffer	(void)
{
#ifdef ARC_ENCODER_API
	for (int i = 0; i < numbufs; i++)
	{
	    arc_encoder_api_free(buffers[i],ENC_MEM_OPSTREAM|ENC_MEM_DMA_TGT,0);
	    buffers[i] = 0;
	    phys_buffers[i] = 0;
	}
	numbufs = 0;
	m_PhysBufferPointer = 0;
	m_Size = 0;
#else
    if (0 != m_BufferPtrAlocated)
	{
		delete[] m_BufferPtrAlocated;
		m_BufferPtrAlocated = 0;
	}
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Create														*/
/* DESCRIPTION:	creates the byte symbol buffer (allocates storage)			*/
/* ------------------------------------------------------------------------ */

bool CSymbolByteBuffer::Create(int size)
{
	if (m_Created)
	{
		assert(false);
		return false;
	}

	if (0 == size)
	{
		assert(false);
		return false;
	}
#ifdef ARC_ENCODER_API
	m_Size = size ;
	SetupDMABuffer();
#else
	m_BufferPtrAlocated = new unsigned char[size + 128];
	m_BufferPointer  = m_BufferPtrAlocated + 64;
	if (0 == m_BufferPtrAlocated)
	{
		assert(false);
		return false;
	}
	flush_dcache();
#endif

	m_Created = true;
	m_Next = 0;

	return true;
}


void CSymbolByteBuffer::ResetBuffer(void)
{
	m_BitCount = 0;
	m_EmuByteCnt = 0;
#ifdef ARC_ENCODER_API
	m_amount = 0;
#endif
}

#ifdef ARC_ENCODER_API
void CSymbolByteBuffer::QueueBuffer(void)
{
    arc_encoder_enqueue_data(m_Buffer,m_amount,gCH264Encoder.m_output_q);
    SetupDMABuffer();
}

void CSymbolByteBuffer::SetupDMABuffer()
{
    m_Buffer = 0;
    m_amount = 0;
    for (int i = 0; i < numbufs; i++)
    {
	if (is_buffer_free(buffers[i]))
	{
	    m_Buffer = buffers[i];
	    m_PhysBufferPointer = phys_buffers[i];
	    break;
	}
    }
    
    if ((! m_Buffer)
	&& (numbufs < BUF_STORE_LEN) 
	&& (m_Buffer = arc_encoder_api_malloc(m_Size,ENC_MEM_OPSTREAM|ENC_MEM_DC_PAD|ENC_MEM_DMA_TGT,&m_PhysBufferPointer,0)))
    {
	buffers[numbufs] = m_Buffer;
	phys_buffers[numbufs] = m_PhysBufferPointer;
	return_buffer(m_Buffer);
	numbufs++;
    }
	
    if (! m_Buffer)
    {
	// We're in trouble - we can't allocate any more memory. Reuse memory
        // to allow the channels to clear, and indicate that the stream's corrupt.
	gCH264Encoder.m_error = ENC_ERR_H264_OOM;
	if (! numbufs)
	{
	    // We are in real trouble.
	    m_Buffer = gArcFrameBuffers.OutputY;
	    m_PhysBufferPointer = gArcFrameBuffers.PhysOPY;
	}
	else
	{
	    m_Buffer = buffers[0];
	    m_PhysBufferPointer = phys_buffers[0];
	}
    }
    
    assert(m_Buffer != 0);
    m_BufferPointer = m_Buffer; 
}
#endif
