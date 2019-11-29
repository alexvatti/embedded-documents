/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2006 ARC International (Unpublished)                  */
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

/* 
    memctl.h
*/

#if !defined __GNUC__
extern "C" 
{
#endif
void h264_init_memctl (unsigned long *,unsigned long *);
void h264_get_frame_index (unsigned long,int *);
typedef void (*callback) (unsigned long, unsigned long);
int  h264_set_callback (callback);
void h264_mem_setsize (unsigned long, unsigned long, int);
/* Get memory from the h264 buffer */
void *h264_mem_alloc (int,int);
/* determine the virtual to physical mapping */
unsigned long h264_get_physical_adrs (unsigned long);
unsigned long h264_get_virtual_adrs  (unsigned long);
/* free the used resources */
void h264_mem_freeall (void *);
#if !defined __GNUC__
}
#endif


