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

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CSymbolByteBuffer											*/
/* DESCRIPTION:	constructor													*/
/* ------------------------------------------------------------------------ */

CSymbolByteBuffer::CSymbolByteBuffer(void)
{
	m_Created = false;

	m_Overflowed = false;
	
	m_BufferPointer = 0;
	m_BufferSize = 0;

	m_BitCount = 0;
	m_ZeroByteCount = 0;
	m_WritePointer = 0;
	m_ShiftRegisterLength = 0;
	m_ShiftRegisterStore = 0;

	m_SavedBitCount = 0;
	m_SavedZeroByteCount = 0;
	m_SavedWritePointer = 0;
	m_ShiftRegisterLength = 0;
	m_SavedShiftRegisterStore = 0;
	m_SavedOverflowed = false;
	
	m_ArchivedBufferPointer = 0;

	m_ArchivedBitCount = 0;
	m_ArchivedZeroByteCount = 0;
	m_ArchivedWritePointer = 0;
	m_ArchivedShiftRegisterLength = 0;
	m_ArchivedShiftRegisterStore = 0;
	
	m_Next = 0;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	~CSymbolByteBuffer											*/
/* DESCRIPTION:	destructor													*/
/* ------------------------------------------------------------------------ */

CSymbolByteBuffer::~CSymbolByteBuffer	(void)
{
	if (0 != m_BufferPointer)
	{
		delete m_BufferPointer;
		m_BufferPointer = 0;
		m_BufferSize = 0;
	}

	if (0 != m_ArchivedBufferPointer)
	{
		delete m_ArchivedBufferPointer;
		m_ArchivedBufferPointer = 0;
	}
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
	 
	m_BufferPointer = new unsigned char[size];
	if (0 == m_BufferPointer)
	{
		assert(false);
		return false;
	}

	m_WritePointer = m_BufferPointer;

	m_BufferSize = size;

	m_ArchivedBufferPointer = new unsigned char[size];
	if (0 == m_ArchivedBufferPointer)
	{
		delete m_BufferPointer;
		m_BufferPointer = 0;

		assert(false);
		return false;
	}

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
			if ((2 == m_ZeroByteCount) && (0 == (next_byte & 252)))
			{
				// Insert an extra 0x03 byte
				*m_WritePointer++ = 3;
				m_ZeroByteCount = 0;
				m_BitCount += 8;
			}
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
		if ((2 == m_ZeroByteCount) && (0 == (next_byte & 252)))
		{
			// Insert an extra 0x03 byte
			*m_WritePointer++ = 3;
			m_ZeroByteCount = 0;
			m_BitCount += 8;
		}
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
/* FUNCTION:	SaveWriteStatus												*/
/* DESCRIPTION:	Saves various variables to enable restoration of the buffer
				to the point when this function was last called				*/
/* ------------------------------------------------------------------------ */

void CSymbolByteBuffer::SaveWriteStatus()
{
	m_SavedBitCount = m_BitCount;
	m_SavedZeroByteCount = m_ZeroByteCount;
	m_SavedWritePointer = m_WritePointer;
	m_SavedShiftRegisterLength = m_ShiftRegisterLength;
	m_SavedShiftRegisterStore = m_ShiftRegisterStore;
	m_SavedOverflowed = m_Overflowed;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RestoreWriteStatus											*/
/* DESCRIPTION:	Saves various variables to enable restoration of the buffer
				to the point when this function was last called				*/
/* ------------------------------------------------------------------------ */

void CSymbolByteBuffer::RestoreWriteStatus()
{
	m_BitCount = m_SavedBitCount;
	m_ZeroByteCount = m_SavedZeroByteCount;
	m_WritePointer = m_SavedWritePointer;
	m_ShiftRegisterLength = m_SavedShiftRegisterLength;
	m_ShiftRegisterStore = m_SavedShiftRegisterStore;
	m_Overflowed = m_SavedOverflowed;
	assert(false == m_Overflowed);	// Don't expect m_Overflowed to be true after 
									// RestoreWriteStatus, but it could be 
									// if SymbolByteBuffer not correctly used
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CreateArchive												*/
/* DESCRIPTION:	Creates an archive of the data added to the buffer since 
				the last save point.
				The aim is to allow multiple tries of encoding a macroblock,
				with the save point indicating the buffer status before 
				starting to encode the macroblock, and the archive storing 
				the result of the most effective encoding so far. So as 
				each attempt is made, if it is best so far, it is archived.
				The buffer is then restored and anotehr attempt made. When
				all attempts have been completed, the archive is restored
				so as to restore the bits from the most effective encoding.	*/
/* ------------------------------------------------------------------------ */

void CSymbolByteBuffer::CreateArchive()
{
	m_ArchivedBitCount = m_BitCount;
	m_ArchivedZeroByteCount = m_ZeroByteCount;
	m_ArchivedWritePointer = m_WritePointer;
	m_ArchivedShiftRegisterLength = m_ShiftRegisterLength;
	m_ArchivedShiftRegisterStore = m_ShiftRegisterStore;

	memcpy(m_ArchivedBufferPointer, m_SavedWritePointer, m_WritePointer - m_SavedWritePointer);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	RestoreArchive												*/
/* DESCRIPTION:	Restores an archive of the data added to the buffer since 
				the last save point.										*/
/* ------------------------------------------------------------------------ */

void CSymbolByteBuffer::RestoreArchive()
{
	memcpy(m_SavedWritePointer, m_ArchivedBufferPointer, m_ArchivedWritePointer - m_SavedWritePointer);
	
	m_BitCount = m_ArchivedBitCount;
	m_ZeroByteCount = m_ArchivedZeroByteCount;
	m_WritePointer = m_ArchivedWritePointer;
	m_ShiftRegisterLength = m_ArchivedShiftRegisterLength;
	m_ShiftRegisterStore = m_ArchivedShiftRegisterStore;
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
		assert(false);
		return false;
	}
	if (length < (m_WritePointer - m_BufferPointer))
	{
		assert(false);
		return false;
	}
	assert(m_BitCount == (m_WritePointer-1 - m_BufferPointer)*8);	// -1 if because of NAL unit header

	memcpy(to_ptr, m_BufferPointer, m_WritePointer - m_BufferPointer);

	m_WritePointer = m_BufferPointer;
	m_BitCount = 0;
	m_ShiftRegisterLength = 0;
	m_SavedShiftRegisterLength = 0;
	m_ShiftRegisterStore = 0; 
	m_SavedShiftRegisterStore = 0;
	m_SavedWritePointer = 0;
	m_SavedBitCount = 0;
	m_ZeroByteCount = 0;

	return true;
}