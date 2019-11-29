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

#include "MP4Warnings.h"
#include "MP4MemoryMonitor.h"

#include "MP4Transform.h"

#include <assert.h>

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse4x4DCTransform										*/
/* DESCRIPTION:	Performs the inverse 4x4 DC transform - for DC coefficients 
				of 16x16 Intra macroblocks. Uses Hadamard transform.
				The output data overwrites the input data, which must be in 
				an array of 16 ints

				The transform matrix is:
				1   1   1    1
				1   1  -1   -1
				1  -1  -1    1
				1  -1   1   -1
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Inverse4x4DCTransform(int* data)
{
	int * io_data = data;

	int temp0, temp1, temp2, temp3;  // intermediate values
	int i;

	// Horizontal inverse transform first - put result into output
	for(i=0; i<4; i++)
	{
		temp0 = *(io_data  ) + *(io_data+2);
		temp2 = *(io_data  ) - *(io_data+2);
		temp1 = *(io_data+1) + *(io_data+3);
		temp3 = *(io_data+1) - *(io_data+3);

		*(io_data  ) = temp0 + temp1;
		*(io_data+1) = temp2 + temp3;
		*(io_data+2) = temp2 - temp3;
		*(io_data+3) = temp0 - temp1;

		io_data += 4;
	}

	// Vertical inverse transform second
	for(i=0; i<4; i++)
	{
		io_data = data + i;

		temp0 = *(io_data   ) + *(io_data+ 8);
		temp2 = *(io_data   ) - *(io_data+ 8);
		temp1 = *(io_data+ 4) + *(io_data+12);
		temp3 = *(io_data+ 4) - *(io_data+12);

		*(io_data   ) = temp0 + temp1;
		*(io_data+ 4) = temp2 + temp3;
		*(io_data+ 8) = temp2 - temp3;
		*(io_data+12) = temp0 - temp1;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse4x4Transform											*/
/* DESCRIPTION:	Performs the inverse 4x4 transform.
				The output data overwrites the input data.
				Line length field indicates the pointer offsets that 
				this function should use to get from one pel to the 
				immediately below one. For example, when the data being 
				transformed is within a 16x16 block, the value should be 16.
				The resulting pels are added to 32 and right shifted six.

				The transform matrix is:
				1    1    1    1/2
				1   1/2  -1    -1
				1  -1/2  -1     1
				1   -1    1   -1/2
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Inverse4x4Transform(int* data, unsigned int line_length)
{
	int * in_data = data;
	int * out_data = data;

	int temp0, temp1, temp2, temp3;  // intermediate values
	int i;

	// Horizontal inverse transform first - put result into output
	for(i=0; i<4; i++)
	{
		temp0 =  *(in_data  )       +  *(in_data+2);
		temp1 =  *(in_data  )       -  *(in_data+2);
		temp2 = (*(in_data+1) >> 1) -  *(in_data+3);
		temp3 =  *(in_data+1)       + (*(in_data+3) >> 1);

		*(out_data  ) = temp0 + temp3;
		*(out_data+1) = temp1 + temp2;
		*(out_data+2) = temp1 - temp2;
		*(out_data+3) = temp0 - temp3;

		in_data += line_length;
		out_data += line_length;
	}

	// Vertical inverse transform second
	for(i=0; i<4; i++)
	{
		in_data = data + i;
		out_data = in_data;

		temp0 =  *in_data       +  *(in_data+(line_length<<1))    + 32;
		temp1 =  *in_data       -  *(in_data+(line_length<<1))    + 32;

		in_data += line_length;

		temp2 = (*in_data >> 1) -  *(in_data+(line_length<<1));
		temp3 =  *in_data       + (*(in_data+(line_length<<1)) >> 1);

		*out_data = (temp0 + temp3) >> 6;
		out_data += line_length;

		*out_data = (temp1 + temp2) >> 6;
		out_data += line_length;
		
		*out_data = (temp1 - temp2) >> 6;
		out_data += line_length;
		
		*out_data = (temp0 - temp3) >> 6;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse4x4TransformDCOnly									*/
/* DESCRIPTION:	Performs the inverse 4x4 transform when only the DC 
				coefficient is present.
				The output data overwrites the input data.
				Line length field indicates the pointer offsets that 
				this function should use to get from one pel to the 
				immediately below one. For example, when the data being 
				transformed is within a 16x16 block, the value should be 16.
				The output is equal to:
					(input DC coeff + 32) >> 6.

*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Inverse4x4TransformDCOnly(int* data, unsigned int line_length)
{
	int output = ((*data) + 32) >> 6;

	*(data+0) = output;
	*(data+1) = output;
	*(data+2) = output;
	*(data+3) = output;

	data += line_length;

	*(data+0) = output;
	*(data+1) = output;
	*(data+2) = output;
	*(data+3) = output;

	data += line_length;

	*(data+0) = output;
	*(data+1) = output;
	*(data+2) = output;
	*(data+3) = output;

	data += line_length;

	*(data+0) = output;
	*(data+1) = output;
	*(data+2) = output;
	*(data+3) = output;
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Inverse2x2Transform											*/
/* DESCRIPTION:	Performs the inverse 2x2 transform for chroma.
				Output data overwrites input data.

				The transform matrix is:
				      1  1
				      1 -1
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Inverse2x2Transform(int* data)
{
	int temp0, temp1, temp2, temp3;  // intermediate values

	temp0 = *(data  ) + *(data+1);
	temp1 = *(data  ) - *(data+1);
	temp2 = *(data+2) + *(data+3);
	temp3 = *(data+2) - *(data+3);

	*(data  ) = (temp0 + temp2);
	*(data+1) = (temp1 + temp3);
	*(data+2) = (temp0 - temp2);
	*(data+3) = (temp1 - temp3);
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Forward4x4DCTransform										*/
/* DESCRIPTION:	Performs the forward 4x4 DC transform - for DC coefficients 
				of 16x16 Intra macroblocks. Uses Hadamard transform.
				The output data overwrites the input data, which must be in 
				an array of 16 ints
				The document states rounding is used, but the reference code 
				uses division with truncation. The implmentation is the same 
				as the reference code.

				The transform matrix is:
				1   1   1    1
				1   1  -1   -1
				1  -1  -1    1
				1  -1   1   -1
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Forward4x4DCTransform(int* data)
{
	int * io_data = data;

	int temp0, temp1, temp2, temp3;  // intermediate values
	int i;

	// Horizontal inverse transform first - put result into output
	for(i=0; i<4; i++)
	{
		temp0 = *(io_data  ) + *(io_data+2);
		temp2 = *(io_data  ) - *(io_data+2);
		temp1 = *(io_data+1) + *(io_data+3);
		temp3 = *(io_data+1) - *(io_data+3);

		*(io_data  ) = temp0 + temp1;
		*(io_data+1) = temp2 + temp3;
		*(io_data+2) = temp2 - temp3;
		*(io_data+3) = temp0 - temp1;

		io_data += 4;
	}

	// Vertical inverse transform second
	for(i=0; i<4; i++)
	{
		io_data = data + i;

		temp0 = *(io_data   ) + *(io_data+ 8);
		temp2 = *(io_data   ) - *(io_data+ 8);
		temp1 = *(io_data+ 4) + *(io_data+12);
		temp3 = *(io_data+ 4) - *(io_data+12);

		*(io_data   ) = (temp0 + temp1) >> 1;
		*(io_data+ 4) = (temp2 + temp3) >> 1;
		*(io_data+ 8) = (temp2 - temp3) >> 1;
		*(io_data+12) = (temp0 - temp1) >> 1;
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Forward4x4Transform											*/
/* DESCRIPTION:	Performs the forward 4x4 transform.
				The output data overwrites the input data.
				Line length field indicates the pointer offsets that 
				this function should use to get from one pel to the 
				immediately below one. For example, when the data being 
				transformed is within a 16x16 block, the value should be 16.

				The transform matrix is:
				1    1    1    1
				2    1   -1   -2
				1   -1   -1    1
				1   -2    2   -1
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Forward4x4Transform(int* data, unsigned int line_length)
{
	int * in_data = data;
	int * out_data = data;

	int temp0, temp1, temp2, temp3;  // intermediate values
	int i;

	// Horizontal transform first - put result into output
	for(i=0; i<4; i++)
	{
		temp0 = *(in_data  ) + *(in_data+3);
		temp1 = *(in_data  ) - *(in_data+3);
		temp2 = *(in_data+1) + *(in_data+2);
		temp3 = *(in_data+1) - *(in_data+2);

		*(out_data  ) = temp0 + temp2;
		*(out_data+1) = (temp1 << 1) + temp3;
		*(out_data+2) = temp0 - temp2;
		*(out_data+3) = temp1 - (temp3 << 1);

		in_data += line_length;
		out_data += line_length;
	}

	// Vertical transform second
	for(i=0; i<4; i++)
	{
		in_data = data + i;
		out_data = in_data;

		temp0 = (*in_data) + *(in_data+(3*line_length));
		temp1 = (*in_data) - *(in_data+(3*line_length));
		in_data += line_length;
		temp2 = (*in_data) + *(in_data+line_length);
		temp3 = (*in_data) - *(in_data+line_length);

		*out_data = temp0 + temp2;
		out_data += line_length;

		*out_data = (temp1 << 1) + temp3;
		out_data += line_length;
		
		*out_data = temp0 - temp2;
		out_data += line_length;
		
		*out_data = temp1 - (temp3 << 1);
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Forward4x4TransformSeparate									*/
/* DESCRIPTION:	Performs the forward 4x4 transform.
				The output data is separate from the input data.
				Line length field indicates the pointer offsets that 
				this function should use to get from one pel to the 
				immediately below one. For example, when the data being 
				transformed is within a 16x16 block, the value should be 16.

				The transform matrix is:
				1    1    1    1
				2    1   -1   -2
				1   -1   -1    1
				1   -2    2   -1
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Forward4x4TransformSeparate(int* in_data_ptr, int* out_data_ptr, unsigned int line_length)
{
	int * in_data = in_data_ptr;
	int * out_data = out_data_ptr;

	int temp0, temp1, temp2, temp3;  // intermediate values
	int i;

	// Horizontal transform first - put result into output
	for(i=0; i<4; i++)
	{
		temp0 = *(in_data  ) + *(in_data+3);
		temp1 = *(in_data  ) - *(in_data+3);
		temp2 = *(in_data+1) + *(in_data+2);
		temp3 = *(in_data+1) - *(in_data+2);

		*(out_data  ) = temp0 + temp2;
		*(out_data+1) = (temp1 << 1) + temp3;
		*(out_data+2) = temp0 - temp2;
		*(out_data+3) = temp1 - (temp3 << 1);

		in_data += line_length;
		out_data += line_length;
	}

	// Vertical transform second
	for(i=0; i<4; i++)
	{
		in_data = out_data_ptr + i;
		out_data = in_data;

		temp0 = (*in_data) + *(in_data+(3*line_length));
		temp1 = (*in_data) - *(in_data+(3*line_length));
		in_data += line_length;
		temp2 = (*in_data) + *(in_data+line_length);
		temp3 = (*in_data) - *(in_data+line_length);

		*out_data = temp0 + temp2;
		out_data += line_length;

		*out_data = (temp1 << 1) + temp3;
		out_data += line_length;
		
		*out_data = temp0 - temp2;
		out_data += line_length;
		
		*out_data = temp1 - (temp3 << 1);
	}
}

/* ------------------------------------------------------------------------ */
/* FUNCTION:	Forward2x2Transform											*/
/* DESCRIPTION:	Performs the forward 2x2 transform for chroma.
				Output data overwrites input data.

				The transform matrix is:
				1  1
				1 -1

				for input:
				0 1
				2 3
*/
/* RETURNS:		Void.														*/
/* ------------------------------------------------------------------------ */

void Forward2x2Transform(int* data)
{
	int temp0, temp1, temp2, temp3;  // intermediate values

	temp0 = *(data  ) + *(data+1);
	temp1 = *(data  ) - *(data+1);
	temp2 = *(data+2) + *(data+3);
	temp3 = *(data+2) - *(data+3);

	*(data  ) = (temp0 + temp2);
	*(data+1) = (temp1 + temp3);
	*(data+2) = (temp0 - temp2);
	*(data+3) = (temp1 - temp3);
}

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
