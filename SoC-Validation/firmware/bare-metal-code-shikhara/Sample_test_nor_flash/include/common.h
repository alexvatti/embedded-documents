/*
 * (C) Copyright 2004
 * Texas Instruments
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __COMMON_H_
#define __COMMON_H_	1


#define __raw_writeb(v,a)               (*(volatile unsigned char *)(a) = (v))
#define __raw_writew(v,a)               (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)               (*(volatile unsigned int *)(a) = (v))

#define __raw_readb(a)                  (*(volatile unsigned char *)(a))
#define __raw_readw(a)                  (*(volatile unsigned short *)(a))
#define __raw_readl(a)                  (*(volatile unsigned int *)(a)) 

#define readl __raw_readl
#define readb __raw_readb
#define readw __raw_readw

#define writel __raw_writel
#define writeb __raw_writeb
#define writew __raw_writew


typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;
                        
typedef signed int s32;
typedef unsigned int u32;

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

typedef         __u8            uint8_t;
typedef         __u16           uint16_t;
typedef         __u32           uint32_t;

typedef int int32_t;
typedef unsigned long ulong;


void udelay(unsigned long us);
/*xmodem protocol constants*/
/* Values magic to the protocol */
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define BSP 0x08
#define NAK 0x15
#define CAN 0x18
//#define EOF 0x1A 

#define XMODEM_CHAR_TIMEOUT            2000   /* 2 seconds */
#define XMODEM_MAX_RETRIES             20
#define XMODEM_CAN_COUNT                3     /* Wait for 3 CAN before quitting */

#define modem_xmodem 1

#define xmodem_access   -1
#define xyzModem_noZmodem -2
#define xmodem_timeout  -3
#define xmodem_eof      -4
#define xmodem_cancel   -5
#define xmodem_frame    -6
#define xmodem_cksum    -7
#define xmodem_sequence -8

#define CFG_PRINTF             1
#define CFG_PBSIZE              256

#ifdef	CFG_PRINTF
#define printf(fmt,args...)	serial_printf (fmt ,##args)
#define getc() serial_getc()
#else
#define printf(fmt,args...)
#define getc() ' '
#endif	/* CFG_PRINTF */

#endif	/* __COMMON_H_ */
