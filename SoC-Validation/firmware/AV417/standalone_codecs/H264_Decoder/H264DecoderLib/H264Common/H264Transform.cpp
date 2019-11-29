/* ------------------------------------------------------------------------ */
/* FILE:			H264Transform.cpp										*/
/* DESCRIPTION:		Forward and inverse transform definitions for H.264		*/
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

#include "H264Warnings.h"

#include "H264Transform.h"

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

_Asm void Inverse4x4DCTransform_ASM()
{
	mov	%r2,%r0
	mov	%r9,%r0
	; ZD LOOP:
	; loop flattened by unrolling 21 instructions 3 time
	ldw	%r8,[%r9] 
	ldw	%r1,[%r9,4] 	;
	ldw	%r0,[%r9,6] 
	ldw	%r5,[%r9,2] 	;
	add	%r7,%r8,%r1
	sub	%r8,%r8,%r1
	add	%r11,%r0,%r5
	sub	%r5,%r5,%r0
	add	%r10,%r11,%r7
	add	%r0,%r5,%r8
	sub	%r8,%r8,%r5
	sub	%r7,%r7,%r11
	stw	%r10,[%r9] 
	stw	%r0,[%r9,2] 
	stw	%r8,[%r9,4] 
	stw	%r7,[%r9,6] 

	ldw	%r8,[%r9,0+8] 
	ldw	%r1,[%r9,4+8] 	;
	ldw	%r0,[%r9,6+8] 
	ldw	%r5,[%r9,2+8] 	;
	add	%r7,%r8,%r1
	sub	%r8,%r8,%r1
	add	%r11,%r0,%r5
	sub	%r5,%r5,%r0
	add	%r10,%r11,%r7
	add	%r0,%r5,%r8
	sub	%r8,%r8,%r5
	sub	%r7,%r7,%r11
	stw	%r10,[%r9,0+8] 
	stw	%r0,[%r9,2+8] 
	stw	%r8,[%r9,4+8] 
	stw	%r7,[%r9,6+8] 

	ldw	%r8,[%r9,0+16] 
	ldw	%r1,[%r9,4+16] 	;
	ldw	%r0,[%r9,6+16] 
	ldw	%r5,[%r9,2+16] 	;
	add	%r7,%r8,%r1
	sub	%r8,%r8,%r1
	add	%r11,%r0,%r5
	sub	%r5,%r5,%r0
	add	%r10,%r11,%r7
	add	%r0,%r5,%r8
	sub	%r8,%r8,%r5
	sub	%r7,%r7,%r11
	stw	%r10,[%r9,0+16] 
	stw	%r0,[%r9,2+16] 
	stw	%r8,[%r9,4+16] 
	stw	%r7,[%r9,6+16] 

	ldw	%r8,[%r9,0+24] 
	ldw	%r1,[%r9,4+24] 	;
	ldw	%r0,[%r9,6+24] 
	ldw	%r5,[%r9,2+24] 	;
	add	%r7,%r8,%r1
	sub	%r8,%r8,%r1
	add	%r11,%r0,%r5
	sub	%r5,%r5,%r0
	add	%r10,%r11,%r7
	add	%r0,%r5,%r8
	sub	%r8,%r8,%r5
	sub	%r7,%r7,%r11
	stw	%r10,[%r9,0+24] 
	stw	%r0,[%r9,2+24] 
	stw	%r8,[%r9,4+24] 
	stw	%r7,[%r9,6+24] 



	nop_s	;
	ld	%r6,[%r2] 
	ld	%r8,[%r2,16] 	;
	addsdw	%r4,%r6,%r8
	subsdw	%r6,%r6,%r8
	ld	%r7,[%r2,24] 
	ld_s	%r3,[%r2,8] 	;
	addsdw	%r11,%r7,%r3
	subsdw	%r3,%r3,%r7
	addsdw	%r1,%r11,%r4
	addsdw	%r5,%r3,%r6
	subsdw	%r6,%r6,%r3
	subsdw	%r4,%r4,%r11
	st_s	%r1,[%r2] 
	st	%r5,[%r2,8] 
	st	%r6,[%r2,16] 
	st	%r4,[%r2,24] 

	ld	%r6,[%r2,0+4] 
	ld	%r8,[%r2,16+4] 	;
	addsdw	%r4,%r6,%r8
	subsdw	%r6,%r6,%r8
	ld	%r7,[%r2,24+4] 
	ld_s	%r3,[%r2,8+4] 	;
	addsdw	%r11,%r7,%r3
	subsdw	%r3,%r3,%r7
	addsdw	%r1,%r11,%r4
	addsdw	%r5,%r3,%r6
	subsdw	%r6,%r6,%r3
	subsdw	%r4,%r4,%r11
	st_s	%r1,[%r2,0+4] 
	st	%r5,[%r2,8+4] 
	st	%r6,[%r2,16+4] 
	st	%r4,[%r2,24+4] 
}


void Inverse4x4DCTransform(short* data)
{
#if 1
	Inverse4x4DCTransform_ASM();
#else
	short * io_data = data;

	short temp0, temp1, temp2, temp3;  // intermediate values
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
#endif
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
//
//void Inverse4x4Transform(short* data, unsigned int line_length)
//{
//	short * in_data = data;
//	short * out_data = data;
//
//	short temp0, temp1, temp2, temp3;  // intermediate values
//	int i;
//
//	 Horizontal inverse transform first - put result into output
//	for(i=0; i<4; i++)
//	{
//		temp0 =  *(in_data  )       +  *(in_data+2);
//		temp1 =  *(in_data  )       -  *(in_data+2);
//		temp2 = (*(in_data+1) >> 1) -  *(in_data+3);
//		temp3 =  *(in_data+1)       + (*(in_data+3) >> 1);
//
//		*(out_data  ) = temp0 + temp3;
//		*(out_data+1) = temp1 + temp2;
//		*(out_data+2) = temp1 - temp2;
//		*(out_data+3) = temp0 - temp3;
//
//		in_data += line_length;
//		out_data += line_length;
//	}
//
//	 Vertical inverse transform second
//	for(i=0; i<4; i++)
//	{
//		in_data = data + i;
//		out_data = in_data;
//
//		temp0 =  *in_data       +  *(in_data+(line_length<<1))    + 32;
//		temp1 =  *in_data       -  *(in_data+(line_length<<1))    + 32;
//
//		in_data += line_length;
//
//		temp2 = (*in_data >> 1) -  *(in_data+(line_length<<1));
//		temp3 =  *in_data       + (*(in_data+(line_length<<1)) >> 1);
//
//		*out_data = (temp0 + temp3) >> 6;
//		out_data += line_length;
//
//		*out_data = (temp1 + temp2) >> 6;
//		out_data += line_length;
//		
//		*out_data = (temp1 - temp2) >> 6;
//		out_data += line_length;
//		
//		*out_data = (temp0 - temp3) >> 6;
//	}
//}
//

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

_Asm void Inverse2x2Transform_ASM()
{
	ldw.x	%r3,[%r0] 
	ldw.x	%r8,[%r0,2] 	;
	add	%r2,%r3,%r8
	sub	%r3,%r3,%r8
	ldw.x	%r7,[%r0,6] 
	ldw_s.x	%r1,[%r0,4] 	;
	add	%r6,%r7,%r1
	sub	%r1,%r1,%r7
	add	%r4,%r6,%r2
	add	%r5,%r1,%r3
	sub	%r2,%r2,%r6
	stw	%r4,[%r0] 
	stw	%r5,[%r0,2] 
	stw_s	%r2,[%r0,4] 
	sub_s	%r3,%r3,%r1
	stw	%r3,[%r0,6] 	;
}


void Inverse2x2Transform(short* data)
{
#if 1
	Inverse2x2Transform_ASM();
#else
	short temp0, temp1, temp2, temp3;  // intermediate values

	temp0 = *(data  ) + *(data+1);
	temp1 = *(data  ) - *(data+1);
	temp2 = *(data+2) + *(data+3);
	temp3 = *(data+2) - *(data+3);

	*(data  ) = (temp0 + temp2);
	*(data+1) = (temp1 + temp3);
	*(data+2) = (temp0 - temp2);
	*(data+3) = (temp1 - temp3);
#endif
}




/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
