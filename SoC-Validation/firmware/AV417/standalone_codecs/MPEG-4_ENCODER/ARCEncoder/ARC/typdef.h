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

#ifndef _TYPDEF_H_
#define _TYPDEF_H_

/* ------------------------------------------------------------------------ */
/* INTEGER TYPES															*/
/* ------------------------------------------------------------------------ */

typedef short				int16;
typedef unsigned char		u_int8;
typedef signed char			s_int8;
typedef unsigned short		u_int16;

typedef int					int32;
typedef unsigned  int		u_int32;
typedef unsigned long long	u_int64;

/* ------------------------------------------------------------------------ */
/* 128 bit TYPES															*/
/* ------------------------------------------------------------------------ */

struct qword
{
	union
	{
		u_int8	Bytes[16];
		int16	Sint16[8];
		int32	Sint32[4];
	};
};

typedef unsigned char		pel_t;



#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
