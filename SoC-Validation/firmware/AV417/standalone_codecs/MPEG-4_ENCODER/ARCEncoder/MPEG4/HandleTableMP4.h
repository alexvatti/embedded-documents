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

#ifndef	HNDTAB_H
#define	HNDTAB_H

#ifdef __cplusplus
extern "C" 
{
#endif 

/*
--------------------------------------------------------------------------------
Definitions.
--------------------------------------------------------------------------------
*/

#define		MAX_NUMBER_OF_H264_HANDLES			256

/*
--------------------------------------------------------------------------------
Prototypes.
--------------------------------------------------------------------------------
*/

int		f_h264_hndtab_add(void* object);
int		f_h264_hndtab_remove(int handle);
void*	f_h264_hndtab_get_object(int handle);

#ifdef __cplusplus
}
#endif

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
