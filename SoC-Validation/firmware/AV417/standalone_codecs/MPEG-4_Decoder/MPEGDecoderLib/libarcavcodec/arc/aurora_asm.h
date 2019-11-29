/*
 * Copyright (c) 2005 ARC International (UK) Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * aurora_asm.h
 *
 * Nigel Topham, April 2005.
 */

#ifndef _AURORA_ASM_H_
#define _AURORA_ASM_H_
/* Virtual pointer to the start of the SDM. */
extern unsigned char * SDM_Bytes;

/* The virtual address of the start of the frame buffer. */
extern void * Frame_Buffer_Virtual_Address;

/* The physical address of the start of the frame buffer. */
extern void * Frame_Buffer_Physical_Address;

/******************** ADDR MANP MACROS **********************/
/* Converts a virtual pointer to a byte offset within the SDM.
 * Behaviour is undefined for pointers outwith the SDM. */
#define VIRTUAL_TO_SDM(ADDR)                          \
    ((int)(ADDR)-(int)SDM_Bytes)

/* Converts a virtual pointer to a physical address (in terms of the entire
 * memory space) within the frame buffer. Behaviour is undefined for
 * virtual pointers that do not correspond to the frame buffer. */
#define VIRTUAL_TO_FB(ADDR)                          \
    ((int)(ADDR)-(int)Frame_Buffer_Virtual_Address+(int)Frame_Buffer_Physical_Address)

#endif  //#define _AURORA_ASM_H_
