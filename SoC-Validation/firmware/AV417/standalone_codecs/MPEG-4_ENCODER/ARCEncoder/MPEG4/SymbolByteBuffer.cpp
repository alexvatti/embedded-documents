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
/* May contain code (c) British Telecommunications plc 2005        */

#include	"SymbolByteBuffer.h"
#include	<assert.h>
#include	<string.h>
#include    "defines.h"

#ifdef ARC_ENCODER_API
#include <arc_encoder_malloc.h>
#include <packet_queue.h>
#include <CMP4Encoder.h>
#include <mpeg4_interface.h>
#include <ArcFrameBuffers.h>
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CSymbolByteBuffer											*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CSymbolByteBuffer::CSymbolByteBuffer(void)
{
	m_Created = false;

	m_Overflowed = false;
	
	m_BufferPointer = 0;
	m_BufferPtrAlocated = 0;
	m_BufferSize = 0;

	m_BitCount = 0;
    m_EmuByteCnt = 0;
    m_ByteCount  = 0;
	m_ZeroByteCount = 0;
	m_WritePointer = 0;
	m_ShiftRegisterLength = 0;
	m_ShiftRegisterStore = 0;
	m_ShiftRegisterLength = 0;
	
	m_Next = 0;

#ifdef ARC_ENCODER_API
	m_PhysBufferPointer = 0;
	m_Amount = 0;
	m_Size = 0;
	for (int i = 0; i < BUF_STORE_LEN; i++)
	{
	    buffers[i] = 0;
	    phys_buffers[i] = 0;
	}
	numbufs = 0;
#endif
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
	m_BufferPointer = 0;
	m_BufferSize = 0;
	m_Size = 0;
#else
	if (0 != m_BufferPtrAlocated)
	{
		delete[] m_BufferPtrAlocated;
		m_BufferPtrAlocated = 0;
		m_BufferPointer = 0;
		m_BufferSize = 0;
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
	m_Size = size;
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

	m_WritePointer = m_BufferPointer;
    m_BufferSize = size;

	m_Created = true;
	m_Next = 0;

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	WriteSingleByte												*/
/* DESCRIPTION:	Writes a single byte to the buffer. Typically used to set 
				the NAL unit type											*/
/* ------------------------------------------------------------------------ */

bool CSymbolByteBuffer::WriteSingleByte(unsigned char value)
{
	assert(m_WritePointer == m_BufferPointer);		// Only expect to write the first byte with this function
	*m_WritePointer++ = value;
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	WriteToBuffer												*/
/* DESCRIPTION:	Writes a symbol to the buffer								*/
/* ------------------------------------------------------------------------ */

bool CSymbolByteBuffer::WriteToBuffer(int input_code, int input_length)
{
	int code = input_code;
	int length = input_length;
	unsigned char next_byte;
	int shift;
	
	if (length > 24)	// Have to treat as two separate symbols in effect
	{
		int top_code ;
		top_code = input_code >> 24;
		int top_length;
		top_length = input_length - 24;
		code = input_code & 0x00ffffff;
		length = 24;

		shift   = 32 - (unsigned int)(top_length) - m_ShiftRegisterLength;
		m_ShiftRegisterLength += (unsigned int)(top_length);
		m_ShiftRegisterStore |= (top_code << shift);
	
		
		while (m_ShiftRegisterLength >= 8)
		{
			if (m_WritePointer-m_BufferPointer >= m_BufferSize)
			{
				m_Overflowed = true;
				break;		// Don't write the symbol
							// We've overflowed the buffer - but continue accepting input. The idea here
							// is that we maintain the bit count - at the end of encoding the macroblock
							// we will discover that the limit has been broken
							// and re-encode the macroblock in PCM mode
							// However, we do want to set overflow indicators
			}
			assert(m_WritePointer-m_BufferPointer < m_BufferSize);
			next_byte = (unsigned char)(m_ShiftRegisterStore >> 24);

			// Byte stuffing
		/*	if ((2 == m_ZeroByteCount) && (0 == (next_byte & 252)))
			{
				// Insert an extra 0x03 byte
				*m_WritePointer++ = 3;
				m_ZeroByteCount = 0;
				m_BitCount += 8;
			}*/
			if (0 == next_byte)
			{
				m_ZeroByteCount++;
			}
			else
			{
				m_ZeroByteCount = 0;
			}		

			*m_WritePointer++ = next_byte;

			m_ShiftRegisterStore  = m_ShiftRegisterStore << 8;
			m_ShiftRegisterLength -= 8;	 
		}
	}

	shift   = 32 - (unsigned int)(length) - m_ShiftRegisterLength;
	m_ShiftRegisterLength += (unsigned int)(length);
	m_ShiftRegisterStore |= (code << shift);

    while (m_ShiftRegisterLength >= 8)
    {
		if (m_WritePointer-m_BufferPointer >= m_BufferSize)
		{
			m_Overflowed = true;
			break;		// Don't write the symbol
						// We've overflowed the buffer - but continue accepting input. The idea here
						// is that we maintain the bit count - at the end of encoding the macroblock
						// we will discover that the limit has been broken
						// and re-encode the macroblock in PCM mode
						// However, we do want to set overflow indicators
		}
		assert(m_WritePointer-m_BufferPointer < m_BufferSize);
		next_byte = (unsigned char)(m_ShiftRegisterStore >> 24);

		// Byte stuffing
	/*	if ((2 == m_ZeroByteCount) && (0 == (next_byte & 252)))
		{
			// Insert an extra 0x03 byte
			*m_WritePointer++ = 3;
			m_ZeroByteCount = 0;
			m_BitCount += 8;
		}*/
		if (0 == next_byte)
		{
			m_ZeroByteCount++;
		}
		else
		{
			m_ZeroByteCount = 0;
		}		

		*m_WritePointer++ = next_byte;

        m_ShiftRegisterStore  = m_ShiftRegisterStore << 8;
		m_ShiftRegisterLength -= 8;	 
    }

	m_BitCount += input_length;
	//assert((m_BitCount - (m_WritePointer-1 - m_BufferPointer)*8) < 8); // -1 is because of NAL unit header
	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CopyAndEmpty												*/
/* DESCRIPTION:	Copies the contents of the buffer to the supplied memory
				and resets the pointers etc - effectively emptying the 
				buffer.  Note that it also empties the shift register etc	*/
/* ------------------------------------------------------------------------ */

bool CSymbolByteBuffer::CopyAndEmpty(unsigned char * to_ptr, int length)
{
	if (!m_Created)
	{
		assert(false);
		return false;
	}
	if (m_Overflowed)
	{
		assert(false);
		return false;
	}
	if (0 == to_ptr)
	{
		assert(false);
		return false;
	}
	if (0 == length)
	{
	//	assert(false);
		return false;
	}
	if (length < (m_WritePointer - m_BufferPointer))
	{
		assert(false);
		return false;
	}
	//assert((m_BitCount+7)>>3 == (m_WritePointer - m_BufferPointer));	// -1 if because of NAL unit header JRMM removed

	memcpy(to_ptr, m_BufferPointer, length);

	m_WritePointer = m_BufferPointer;
	m_BitCount = 0;
	m_ShiftRegisterLength = 0;
	m_ShiftRegisterStore = 0; 
	m_ZeroByteCount = 0;

	return true;
}

void CSymbolByteBuffer::ResetBuffer()
{
#ifdef ARC_ENCODER_API
	arc_encoder_enqueue_data(m_Buffer, 
							 m_Amount, 
							 gCH264Encoder.m_output_q);
	SetupDMABuffer();
#endif
	m_WritePointer = m_BufferPointer;
	m_BitCount = 0;
	m_ShiftRegisterLength = 0;
	m_ShiftRegisterStore = 0; 
	m_ZeroByteCount = 0;
}   

#ifdef ARC_ENCODER_API
void CSymbolByteBuffer::SetupDMABuffer()
{
    m_Buffer = 0;
    m_Amount = 0;
    // Try and reuse an existing buffer
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
	gCH264Encoder.m_error = ENC_ERR_MPEG4_OOM;
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
