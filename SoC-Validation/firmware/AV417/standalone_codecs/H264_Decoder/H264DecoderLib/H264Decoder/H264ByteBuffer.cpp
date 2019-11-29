/* ------------------------------------------------------------------------ */
/* FILE:			H264ByteBuffer.cpp										*/
/* DESCRIPTION:		BT H.264 Byte Buffer source file.						*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
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

/*
--------------------------------------------------------------------------------
Include files.
--------------------------------------------------------------------------------
*/

#include "H264Warnings.h"

#include "CH264Decoder.h"
#include "H264VLDTables.h"
#include "H264DecoderTables.h"

#include "vlc_api.h"
#include "cavlc_api.h"

// This is as clean as it can be made, but is still not pleasant.
// To allow the frequently called functions, such as 
// ByteBufferFillShiftRegister and ByteBufferDecodeUniversalVLC to be inline
// and hence run fast, it must be defined in the file in which it 
// is used, that is, it must be defined in H264VLD.cpp. However, it 
// is nice to keep it with all the other ByteBuffer functions here.
// So when INCLUDED_IN_H264_VLD is defined, i.e. in H264VLD.cpp, only 
// ByteBufferDecodeUniversalVLC is included, and otherwise (here) all
// other functions are included.

// Note. At present all of this code is included in this file
#ifndef INCLUDED_IN_H264_VLD
#endif
#ifdef INCLUDED_IN_H264_VLD
#endif

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferInitialise							*/
/* DESCRIPTION:	Initialises variables for the byte buffer.
				The byte buffer is used for receiving in-coming compressed 
				bits and decoding them using the universal VLC table.
				The shift register is used to perform bit manipulations, 
				being filled with bytes when it is empty.
				The read position indicates the address of the next byte in 
				the byte buffer to be read, while m_ByteBufferEnd points 
				to the first byte after the last byte in the byte buffer 
				(not the end of allocated memory) - so trying to read a 
				byte when m_ByteBufferReadPosition = m_ByteBufferEnd is 
				not possible as the buffer is empty.						*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */
/*
bool  CH264Decoder::ByteBufferInitialise(void)
{
#if defined(CAVLC_HW_PRESENT) || defined(CABAC_HW_PRESENT) || defined(CAVLC_MODEL_ON)
    VLC_Reset();
    CAVLC_Reset();
#else
	m_ByteBufferEnd = 0;
	m_ByteBufferReadPosition = 0;

	m_ByteBufferBitsInRegister = 0;
	m_ByteBufferShiftRegister = 0;

	m_ByteBufferZeroByteCount = 0;

	m_ByteBufferState = 0;
	m_ByteBufferFlagLength = 0;
	m_ByteBufferDataValue = 0;
	m_ByteBufferDataLength = 0;
	m_ByteBufferCodeNumber = UINT_MAX;
#endif

	return true;
}
*/
/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDestory								*/
/* DESCRIPTION:	Deallocates memory for the byte buffer.						*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */

//inlined...




/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferSetPointers							*/
/* DESCRIPTION:	This records pointers to the start and end of the 
				compressed data.											*/
/* RETURNS:		True if successful else false.								*/
/* ------------------------------------------------------------------------ */
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code("codesec4")
#endif

void CH264Decoder::ByteBufferSetPointers(CODED_BYTE * data, int data_length,
										 bool start_of_NAL_unit, int &byteCode)
{
#if !defined(CAVLC_HW_PRESENT) 
	if (start_of_NAL_unit)
	{
		// If start of a new NAL Unit, flush out any leftovers from the previous one
		ByteBufferFlush(byteCode);

		m_NALReferenceIDC = (*data >> 5) & 3;
		m_NALUnitType = (h264_NAL_unit_type_t)((*data) & 31);

		m_ByteBufferEnd = data + data_length;
		m_ByteBufferReadPosition = data + 1;
	}
	else
	{
		m_ByteBufferEnd = data + data_length;
		m_ByteBufferReadPosition = data;
	}

#else

	if (start_of_NAL_unit)
	{
		// If start of a new NAL Unit, flush out any leftovers from the previous one
		ByteBufferFlush(byteCode);

		m_NALReferenceIDC = (*data >> 5) & 3;
		m_NALUnitType = (h264_NAL_unit_type_t)((*data) & 31);

#if defined(CAVLC_HW_PRESENT)
        VLC_Buf_Init(data+1, data_length-1);
#endif
	}
	else
	{
#if defined(CAVLC_HW_PRESENT) 
        VLC_Buf_Init(data, data_length);
#endif
	}
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferReset								*/
/* DESCRIPTION:	Resets the byte buffer.										*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

//inlined...




/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferFlush								*/
/* DESCRIPTION:	Flushes all bits out of the byte buffer. This is useful for 
				disposing of rbsp_trailing_bits.							*/
/* RETURNS:		void.														*/
/* ------------------------------------------------------------------------ */

void CH264Decoder::ByteBufferFlush(int &ret)
{
#if defined(CAVLC_HW_PRESENT) 
    VLC_Reset();
    CAVLC_Reset();
#else
    m_ByteBufferEnd = 0;
	m_ByteBufferReadPosition = 0;

	m_ByteBufferBitsInRegister = 0;
	m_ByteBufferShiftRegister = 0;

	m_ByteBufferZeroByteCount = 0;

	m_ByteBufferState = 0;
	m_ByteBufferFlagLength = 0;
	m_ByteBufferDataValue = 0;
	m_ByteBufferDataLength = 0;
	ret = UINT_MAX;
#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferByteAlign							*/
/* DESCRIPTION:	This removes 0 to seven bits from the byte buffer to 
				achieve byte alignment. This may be needed for example at 
				the end of a picture.										*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

//inlined...



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferBitsLeft							*/
/* DESCRIPTION:	Returns the total number of bits left in the byte buffer	*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

//inlined...



/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferNonTrailingBitsLeft					*/
/* DESCRIPTION:	Firstly checks the value of m_ByteBufferBitsInRegister
				If less then 24 it calls ByteBufferFillShiftRegister.
				If there are still bits left in the buffer it simply
				returns the value of m_ByteBufferBitsInRegister, as 
				although this is not a correct value it should be good enough.
				Otherwise it finds the final 1 bit in the shift register
				and returns m_ByteBufferBitsInRegister - 1 - number of 
				trailing zeroes.											*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

int CH264Decoder::ByteBufferNonTrailingBitsLeft(void)
{
// Note: This function is called when this codec is used to decode high profile bitstream
//       Currently, this function is not converted to use the word buffer maintained in HW

#if !defined(CAVLC_HW_PRESENT)

	unsigned int mask;
	int trailing_bit_count = 0;

	if (m_ByteBufferBitsInRegister < 24)
	{
		ByteBufferFillShiftRegister();
	}
	if (0 != (m_ByteBufferEnd - m_ByteBufferReadPosition))
	{
		return m_ByteBufferBitsInRegister;
	}

	// Where's the last 1?
	mask = 0x01 << (32 - m_ByteBufferBitsInRegister);
	while(true)
	{
		if (m_ByteBufferShiftRegister & mask)
		{
			// found a 1
			trailing_bit_count++;
			break;
		}
		else
		{
			trailing_bit_count++;
			mask <<= 1;
		}
		assert(trailing_bit_count < 10);
	}
	return (m_ByteBufferBitsInRegister - trailing_bit_count);

#else

    // TO BE IMPLEMENTED FOR HIGH PROFILE USING API FUNCTIONS
    return 1;

#endif
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferFillShiftRegister					*/
/* DESCRIPTION:	Copies as many bytes as possible from the byte buffer into
				the shift register.
				Assumes that the shift register contains less than 25 bits 
				when the function is called.
				This function is also responsible for byte stuffing removal.
				Whenever 0x000003 occurs in a NAL unit, the 0x03 byte is 
				discarded. Assumes that the first byte of a NAL (NAL header 
				byte) is not zero, as this byte is NOT distinguished from 
				other bytes.												*/
/* RETURNS:		True if one or more bytes was added to the shift regiser, 
				else false, indicating that the byte buffer has been 
				emptied.													*/
/* ------------------------------------------------------------------------ */

#if !defined(CAVLC_HW_PRESENT)

inline bool CH264Decoder::ByteBufferFillShiftRegister(void)
{
	bool return_value = false;

	if (m_ByteBufferBitsInRegister <= 24)
	{
		while (true)
		{
			if (m_ByteBufferReadPosition == m_ByteBufferEnd)
			{
				// Byte Buffer is empty
                if (m_ByteBufferBitsInRegister > 0) {
                    return_value = true;
                }
				break;
			}

			if ((2 == m_ByteBufferZeroByteCount) && (3 == (*m_ByteBufferReadPosition)))
			{
				// The byte *m_ByteBufferReadPosition is a stuffing byte, so discard it
				m_ByteBufferReadPosition++;
				m_ByteBufferZeroByteCount = 0;
			}
			else
			{
				// Update count of how many preceding bytes are zero
				if (0 == (*m_ByteBufferReadPosition))
				{
					m_ByteBufferZeroByteCount++;
				}
				else
				{
					m_ByteBufferZeroByteCount = 0;
				}

				return_value = true;
				m_ByteBufferShiftRegister |= (*m_ByteBufferReadPosition) << (24 - m_ByteBufferBitsInRegister);
				m_ByteBufferReadPosition++;
				m_ByteBufferBitsInRegister += 8;

				if (m_ByteBufferBitsInRegister > 24)
				{
					// Shift register as full as this function can make it
					break;
				}
			}
		}
	}
    else {
        return true;
    }

	return return_value;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferLongStartCode						*/
/* DESCRIPTION:	Decodes data in the shift register until it runs out of 
				data and returns false, or finds a long start code and 
				returns true.
				The long start code is 0x 00 00 00 01
				The variable m_ByteBufferDataLength counts the number
				of data bits.
				The variable m_ByteBufferDataLength contains the number
				of bits in the long start code (>=32) when the function 
				returns true.												*/
/* RETURNS:		True if a complete long start code has been decoded, and 
				false if there are insufficient bits to do so. No 
				other outcomes are possible									*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferLongStartCode(int & byteCode)
{
// Note: This function is called when this codec is used to decode high profile bitstream
//       Currently, this function is not converted to use the word buffer maintained in HW

	byteCode = UINT_MAX; // Reset the codeword as no longer valid

	while (true)
	{
		// Check that there is a bit in the shift register
		if (0 == m_ByteBufferBitsInRegister)
		{
			// Shift register empty - try to fill it
			if (!ByteBufferFillShiftRegister())
			{
				// Byte Buffer is empty too - so have run out of bits
				return false;
			}
		}

		if (0 == (m_ByteBufferShiftRegister >> 31))
		{
			m_ByteBufferShiftRegister <<= 1;
			m_ByteBufferBitsInRegister--;
			m_ByteBufferDataLength++;
		}
		else
		{
			m_ByteBufferShiftRegister <<= 1;
			m_ByteBufferBitsInRegister--;
			m_ByteBufferDataLength++;

			// Current bit is a '1'. If length >= 31 then this is the end of the start code, else reset count to 0
			if (m_ByteBufferDataLength >= 31)
			{
				// This is the end of the codeword

				// Calculate codeword and reset the value and count fields
				m_ByteBufferDataValue = 0;
				m_ByteBufferDataLength = 0;

				m_ByteBufferState = 0;
				m_ByteBufferFlagLength = 0;

				return true;
			}
			else
			{
				m_ByteBufferDataLength = 0;
			}
		}
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeUniversalVLC					*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer as a 
				universal VLC code.
				Codes are formed by a series of flag bits, the last of 
				which is a "1" bit, followed by data bits, the number of 
				which is equal to the number of "0" flag bits. Hence the 
				code table looks as follows.
								1
							0	1	x
						0	0	1	x	x
					0	0	0	1	x	x	x

				The variable m_ByteBufferState indicates whether the 
				next bit to be decoded is a flag (0) or data bit (1).
				The variable m_ByteBufferFlagLength counts the number of 
				"0" flag bits.
				The variable m_ByteBufferDataValue accumulates the data 
				bits, at the low end of the unsigned int, with the LSB being 
				the most recently decoded data bit.
				The variable m_ByteBufferDataLength counts the number
				of data bits.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				number when decoding is complete.							*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code. No 
				other outcomes are possible as the table is complete - 
				there are no invalid code words.							*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeUniversalVLC(int & byteCode)
{
	if (0 == m_ByteBufferState)
	{
		// Count the number of zero bits in the prefix
		int shift_count = 0;

		// Decoding the prefix
		while (0 == m_ByteBufferShiftRegister)
		{
			// All the bits are zero
			m_ByteBufferFlagLength += m_ByteBufferBitsInRegister;
			m_ByteBufferBitsInRegister = 0;
			m_ByteBufferShiftRegister = 0;

			// Shift register empty - try to fill it
			if (!ByteBufferFillShiftRegister())
			{
				// Byte Buffer is empty too - so have run out of bits
				return false;
			}
		}

		while (1 != (m_ByteBufferShiftRegister>>31))
		{
			m_ByteBufferShiftRegister <<= 1;
			shift_count++;
		}

		m_ByteBufferFlagLength += shift_count;

		// Remove the prefix from the shift register including the one bit
		m_ByteBufferShiftRegister <<= 1;
		m_ByteBufferBitsInRegister -= (shift_count + 1);

		// End of prefix flags - change state.
		m_ByteBufferState = 1;

		m_ByteBufferDataLength = m_ByteBufferFlagLength;
	}

	if (1 == m_ByteBufferState)
	{
		// Decoding the suffix

		if (m_ByteBufferBitsInRegister < m_ByteBufferDataLength)
		{
			// Not enough bits for the suffix so use these bits then load some more
			if (m_ByteBufferBitsInRegister > 0)
			{
				m_ByteBufferDataValue = (m_ByteBufferDataValue << m_ByteBufferBitsInRegister) +
					(m_ByteBufferShiftRegister >> (32 - m_ByteBufferBitsInRegister));

				m_ByteBufferDataLength -= m_ByteBufferBitsInRegister;
				m_ByteBufferBitsInRegister = 0;
				m_ByteBufferShiftRegister = 0;
			}

			ByteBufferFillShiftRegister();

			if (m_ByteBufferBitsInRegister < m_ByteBufferDataLength)
			{
				// Not enough bits yet
				return false;
			}
		}

		if (0 != m_ByteBufferDataLength)
		{
			m_ByteBufferDataValue = (m_ByteBufferDataValue << m_ByteBufferDataLength) +
				(m_ByteBufferShiftRegister >> (32 - m_ByteBufferDataLength));

			m_ByteBufferShiftRegister <<= m_ByteBufferDataLength;
			m_ByteBufferBitsInRegister -= m_ByteBufferDataLength;
		}


		// Calculate codeword and reset the value and count fields
		byteCode = (1 << m_ByteBufferFlagLength) + m_ByteBufferDataValue - 1;

		m_ByteBufferDataValue = 0;
		m_ByteBufferDataLength = 0;

		m_ByteBufferState = 0;
		m_ByteBufferFlagLength = 0;
	}

#ifdef VLC_DEBUG
    Dump_Symbol(m_ByteBufferCodeNumber, "UVLC");
#endif

	return true;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeFixedLengthCode				*/
/* DESCRIPTION:	Decodes data in the shift register as a fixed length code 
				with the given length.										*/
/* RETURNS:		True if a complete long start code has been decoded, and 
				false if there are insufficient bits to do so. No 
				other outcomes are possible									*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeFixedLengthCode(int code_length, int & ret)
{
	if (m_ByteBufferBitsInRegister < code_length)
	{
		// Not enough bits so fill the shift register
		ByteBufferFillShiftRegister();

		if (m_ByteBufferBitsInRegister < code_length)
		{
			// Not enough bits, so return false
			return false;
		}
	}

	ret = (m_ByteBufferShiftRegister >> (32-code_length));

	m_ByteBufferShiftRegister <<= code_length;
	m_ByteBufferBitsInRegister -= code_length;

#ifdef VLC_DEBUG
    Dump_Symbol(m_ByteBufferCodeNumber, "FLC");
#endif

	return true;
}

#endif // !defined(CAVLC_HW_PRESENT) 

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeCoeffToken					*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer 
				according to the table given. The table is a complete table
				of int[][2] that is used to lookup bitstrings that have 
				number_of_bits bits.
				This type of table differs from others as long code words 
				are present in it. Four bits at a time are looked up in 
				the table and if more bits are needed, the lookup iterates,
				until the whole codeword has been looked up.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

#if !defined(CAVLC_HW_PRESENT)

bool CH264Decoder::ByteBufferDecodeCoeffToken(const UNSIGNED_SEPTET code_table[][2], int & byteCode)
{
	unsigned int next_4_bits;
	unsigned char * table_ptr;
	int iteration_bit_count = 4;

  	// Fill the shift register as full as possible if less than 16 bits in it
	if (m_ByteBufferBitsInRegister < 16)
	{
		ByteBufferFillShiftRegister();
	}

	next_4_bits = m_ByteBufferShiftRegister >> 28;

	table_ptr = (unsigned char *)(&(code_table[next_4_bits][0]));

	while (true)
	{
		unsigned char code_length = *(table_ptr + 1);

		if (H264_UNF_TABLE != code_length)
		{
			// Got to the end of the codeword, but still need to check we have enough bits
			if (m_ByteBufferBitsInRegister >= code_length)
			{
				byteCode = *table_ptr;
				m_ByteBufferShiftRegister <<= code_length;
				m_ByteBufferBitsInRegister -= code_length;
				return true;
			}

			// Not enough bits yet
			return false;
		}
		else
		{
			if (iteration_bit_count > m_ByteBufferBitsInRegister)
			{
				// Not enough bits yet
				return false;
			}

			// Need to iterate to look at another 4 bits and look them up in the code table
			iteration_bit_count += 4;

			next_4_bits = (m_ByteBufferShiftRegister >> (32 - iteration_bit_count)) & 0xf;
			table_ptr = (unsigned char *)(&(code_table[((*table_ptr) << 4)+next_4_bits][0]));
		}
	}
}


/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeTotalZerosVLC					*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer 
				according to the table given. The table is a complete table
				of int[][2] that is used to lookup bitstrings that have 
				number_of_bits bits.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeTotalZerosVLC(const UNSIGNED_SEPTET * vlc_table, int number_of_bits, int & byteCode)
{
	unsigned int top_n_bits;
	UNSIGNED_SEPTET * table_ptr;
	UNSIGNED_SEPTET code_length;

  	// Fill the shift register as full as possible if less than top_n_bits bits in it
	if (m_ByteBufferBitsInRegister < number_of_bits)
	{
		ByteBufferFillShiftRegister();
	}

	top_n_bits = m_ByteBufferShiftRegister >> (32 - number_of_bits);
	table_ptr = (UNSIGNED_SEPTET*)(vlc_table + (top_n_bits<<1));
	code_length = *(table_ptr + 1);

	if (m_ByteBufferBitsInRegister >= code_length)
	{
		byteCode = *table_ptr;
		m_ByteBufferShiftRegister <<= code_length;
		m_ByteBufferBitsInRegister -= code_length;
		return true;
	}

	// Not enough bits yet
	return false;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeTotalZerosDCVLC				*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer as a 
				total number of dc chroma (non-trailing) zero coefficients.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeTotalZerosDCVLC(int & byteCode)
{
	unsigned int top_three_bits;
	int * table_ptr;
	int code_length;

  	// Fill the shift register as full as possible if less than three bits in it
	if (m_ByteBufferBitsInRegister < 3)
	{
		ByteBufferFillShiftRegister();
	}

	top_three_bits = m_ByteBufferShiftRegister >> 29;
	table_ptr = (int*)(&(total_zeros_dc_table[m_VLDTotalCoefficients-1][top_three_bits][0]));
	code_length = *(table_ptr + 1);

	if (m_ByteBufferBitsInRegister >= code_length)
	{
		byteCode = *table_ptr;
		m_ByteBufferShiftRegister <<= code_length;
		m_ByteBufferBitsInRegister -= code_length;
		return true;
	}

	// Not enough bits yet
	return false;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeRunBeforeVLC					*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer as a 
				run before VLC code. This is used if the number of zeros
				left is less than or equal to 6. All codewords in these
				cases are three bits or less.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeRunBeforeVLC(int & byteCode)
{
	unsigned int top_three_bits;
	int * table_ptr;
	int code_length;

  	// Fill the shift register as full as possible if less than three bits in it
	if (m_ByteBufferBitsInRegister < 3)
	{
		ByteBufferFillShiftRegister();
	}

	top_three_bits = m_ByteBufferShiftRegister >> 29;
	table_ptr = (int*)(&(run_before_table[m_VLDZerosLeft-1][top_three_bits][0]));
	code_length = *(table_ptr + 1);

	if (m_ByteBufferBitsInRegister >= code_length)
	{
		byteCode = *table_ptr;
		m_ByteBufferShiftRegister <<= code_length;
		m_ByteBufferBitsInRegister -= code_length;
		return true;
	}

	// Not enough bits yet
	return false;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeRunBeforeTable7VLC			*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer as a 
				run before VLC code. This is used if the number of zeros
				left is greater than 6. The code table is as below.

					{7,3},    //	0,	111
					{6,3},    //	1,	110
					{5,3},    //	2,	101
					{4,3},    //	3,	100
					{3,3},    //	4,	011
					{2,3},    //	5,	010
					{1,3},    //	6,	001
					{1,4},    //	7,	0001
					{1,5},    //	8,	00001
					{1,6},    //	9,	000001
					{1,7},    //	10,	0000001
					{1,8},    //	11,	00000001
					{1,9},    //	12,	000000001
					{1,10},   //	13,	0000000001
					{1,11},   //	14,	00000000001

				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeRunBeforeTable7VLC(int & byteCode)
{
	unsigned int top_three_bits;

  	// Fill the shift register as full as possible if less than 11 bits in it
	if (m_ByteBufferBitsInRegister < 11)
	{
		ByteBufferFillShiftRegister();
	}

	top_three_bits = m_ByteBufferShiftRegister >> 29;

	if (0 != top_three_bits)
	{
		// If enough bits, one of the three bit codewords
		if (m_ByteBufferBitsInRegister >= 3)
		{
			byteCode = 7 - top_three_bits;
			m_ByteBufferShiftRegister <<= 3;
			m_ByteBufferBitsInRegister -= 3;
			return true;
		}
	}
	else
	{
		// One of the longer codewords
		if (0 != m_ByteBufferShiftRegister)
		{
			int shift_count = 1;  // This will contain the length of the codeword
			unsigned int shift_register = m_ByteBufferShiftRegister;

			// Count the leading zero bits in the shift register
			while (1 != (shift_register>>31))
			{
				shift_register <<= 1;
				shift_count++;
			}

			// Remove the prefix from the shift register including the one bit
			m_ByteBufferShiftRegister <<= shift_count;
			m_ByteBufferBitsInRegister -= (shift_count);
			byteCode = shift_count + 3;

			return true;
		}
	}

	// Not enough bits yet
	return false;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	CH264Decoder::ByteBufferDecodeLevelVLC						*/
/* DESCRIPTION:	Decodes data in the shift register and byte buffer as a 
				coefficient level VLC code from the given table.
				Codes are formed by a series of flag bits, the last of 
				which is a "1" bit, followed by data bits, the number of 
				which is often equal to the table number. Hence table 2 
				looks as follows.
								1   x   x
							0	1	x   x
						0	0	1	x	x
					0	0	0	1	x	x

				Table 0 has two exceptions. The code with 14 zeros and a 1 
				has a further 4 bits. In all tables the code with 15 zeros 
				and a 1 has a further 12 bits.
				The variable m_ByteBufferState indicates whether the 
				next bit to be decoded is a flag (0) or data bit (1).
				The variable m_ByteBufferFlagLength counts the number of 
				"0" flag bits.
				The variable m_ByteBufferDataValue accumulates the data 
				bits, at the low end of the unsigned int, with the LSB being 
				the most recently decoded data bit.
				The variable m_ByteBufferDataLength counts the number
				of data bits.
				The variable m_ByteBufferCodeNumber contains the decoded code 
				word when decoding is complete.								*/
/* RETURNS:		True if a complete code has been decoded, and false if 
				there are insufficient bits to decode a complete code.		*/
/* ------------------------------------------------------------------------ */

bool CH264Decoder::ByteBufferDecodeLevelVLC(int table_number, int & byteCode)
{
	if (0 == m_ByteBufferState)
	{
		// Decoding the prefix
		if (0 == m_ByteBufferShiftRegister)
		{
			// Flag bit is not in the shift register so load more bits
			ByteBufferFillShiftRegister();

			if (0 == m_ByteBufferShiftRegister)
			{
				// Not enough bits yet
				return false;
			}
		}

		// Count the number of zero bits in the prefix
		m_ByteBufferFlagLength = 0;

		while (1 != (m_ByteBufferShiftRegister>>31))
		{
			m_ByteBufferShiftRegister <<= 1;
			m_ByteBufferFlagLength++;
		}

		// Remove the prefix from the shift register including the one bit
		m_ByteBufferShiftRegister <<= 1;
		m_ByteBufferBitsInRegister -= (m_ByteBufferFlagLength + 1);

		// End of prefix flags - change state.
		m_ByteBufferState = 1;

		// Calculate the number of data bits, and store in m_ByteBufferDataLength
		if ((0 == table_number) && (14 == m_ByteBufferFlagLength))
		{
			m_ByteBufferDataLength = 4;
		}
		else if (15 == m_ByteBufferFlagLength)
		{
//	-	When level_prefix is equal to 15, levelSuffixSize is set equal to 12.
			m_ByteBufferDataLength = 12;
		}
		else
		{
			m_ByteBufferDataLength = table_number;
		}
	}

	if (1 == m_ByteBufferState)
	{
		int data_value;
		
		// Decoding the suffix

		if (m_ByteBufferBitsInRegister < m_ByteBufferDataLength)
		{
			// Not enough bits for the suffix so load some more bits
			ByteBufferFillShiftRegister();

			if (m_ByteBufferBitsInRegister < m_ByteBufferDataLength)
			{
				// Not enough bits yet
				return false;
			}
		}

		if (0 == m_ByteBufferDataLength)
		{
			// Shift right by 32 is undefined
			data_value = 0;
		}
		else
		{
			data_value = (m_ByteBufferShiftRegister >> (32 - m_ByteBufferDataLength));

			m_ByteBufferShiftRegister <<= m_ByteBufferDataLength;
			m_ByteBufferBitsInRegister -= m_ByteBufferDataLength;
		}

		// table_number = suffix_length
		// m_ByteBufferFlagLength = level_prefix
		// m_ByteBufferDataLength = levelSuffixSize
		// data_value = level_suffix ???
		// m_ByteBufferCodeNumber = level_code

		// This is the end of the codeword

		// Calculate codeword and level, and reset the value and count fields
		if (0 == table_number)	// ie suffix_length == 0
		{
			if (m_ByteBufferFlagLength < 14)	// no suffix
			{
				byteCode = m_ByteBufferFlagLength;
			}
			else if (14 == m_ByteBufferFlagLength)
			{
				byteCode = 14 + data_value;
			}
			else
			{
				byteCode = 30 + data_value;
			}
		}
		else
		{
			// -	A variable levelCode is set equal to (level_prefix << suffix_length) + level_suffix.
			// -	When level_prefix is equal to 15 and suffix_length is equal to 0, levelCode is incremented by 15.
			if (m_ByteBufferFlagLength < 15)
			{
				byteCode = (m_ByteBufferFlagLength << table_number) + data_value;
			}

			else
			{		
				byteCode = (15 << table_number) + data_value;
			}
		}

		// End of suffix, change state
		m_ByteBufferState = 0;

		// Reset variables
		m_ByteBufferDataValue = 0;
		m_ByteBufferDataLength = 0;

		m_ByteBufferFlagLength = 0;
	}

	return true;
}

#endif // !defined(CAVLC_HW_PRESENT)
#if defined(I16K_D16K) || defined(I8K_D8K)
#pragma Code()
#endif
/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
 

