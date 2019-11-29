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

class CSymbolByteBuffer
{
public:
	CSymbolByteBuffer(void);
	~CSymbolByteBuffer(void);

	bool Create(int size);

	bool WriteToBuffer(int code, int length);

	bool WriteSingleByte(unsigned char value);	// Typically used to set the NAL unit header

	void SaveWriteStatus(void);
	void RestoreWriteStatus(void);

	void CreateArchive(void);
	void RestoreArchive(void);

	bool Overflowed()
	{
		return m_Overflowed;
	}

	int GetBitCount(void)
	{
		return m_BitCount;
	}

	int GetLength()
	{
		return m_WritePointer - m_BufferPointer;
	}
	bool CopyAndEmpty(unsigned char * to_ptr, int length);

	CSymbolByteBuffer * m_Next;

private:
	bool m_Created;
	bool m_Overflowed;

	unsigned char * m_BufferPointer;		// Points to beginning of the buffer or 0 if no buffer.
	int	m_BufferSize;

	int	m_BitCount;							// Total number of bits in the buffer
	int	m_ZeroByteCount;
	unsigned char * m_WritePointer;			// Points to the next byte to be written
	unsigned int m_ShiftRegisterLength;
	unsigned int m_ShiftRegisterStore;

	int	m_SavedBitCount;
	int	m_SavedZeroByteCount;
	unsigned char * m_SavedWritePointer;
	unsigned int m_SavedShiftRegisterLength;
	unsigned int m_SavedShiftRegisterStore;
	bool m_SavedOverflowed;

	unsigned char * m_ArchivedBufferPointer;// Points to beginning of the archive buffer

	int	m_ArchivedBitCount;
	int	m_ArchivedZeroByteCount;
	unsigned char * m_ArchivedWritePointer;
	unsigned int m_ArchivedShiftRegisterLength;
	unsigned int m_ArchivedShiftRegisterStore;
};

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
