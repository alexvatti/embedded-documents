/*
 * Watchdog driver for the FTWDT010 Watch Dog Driver
 *
 * (c) Copyright 2004 Faraday Technology Corp. (www.faraday-tech.com)
 * Based on sa1100_wdt.c by Oleg Drokin <green@crimea.edu>
 * Based on SoftDog driver by Alan Cox <alan@redhat.com>
 *
 * Copyright (C) 2011 Andes Technology Corporation
 * Macpaul Lin, Andes Technology Corporation <macpaul@andestech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 27/11/2004 Initial release, Faraday.
 * 12/01/2011 Port to u-boot, Macpaul Lin.
 */

#include <asm/arch/shikhara_map.h>
#ifndef __SP805_H
#define __SP805_H
/* Watchdog timer (SP805) register base address */

#ifndef __ASSEMBLY__

struct sp805_wdt {
       u32 wdogload;           /* 0x000 */
       u32 wdogvalue;
       u32 wdogcontrol;
       u32 wdogintclr;
       u32 wdogris;
       u32 wdogmis;
       u32 res1[0x2F9];
       u32 wdoglock;           /* 0xC00 */
       u32 res2[0xBE];
       u32 wdogitcr;           /* 0xF00 */
       u32 wdogitop;
       u32 res3[0x35];
       u32 wdogperiphid0;      /* 0xFE0 */
       u32 wdogperiphid1;
       u32 wdogperiphid2;
       u32 wdogperiphid3;
       u32 wdogpcellid0;
       u32 wdogpcellid1;
       u32 wdogpcellid2;
       u32 wdogpcellid3;
};

#endif 
 
 
#define SP805_WDLOAD(x)		((x) & 0xffffffff)

#define SP805_WDCR_RST		(1 << 1)
#define SP805_WDCR_INTR		(1 << 0)

/*
 * Variable timeout should be set in ms.
 * (CONFIG_SYS_CLK_FREQ/1000) equals 1 ms.
 * WDLOAD = timeout * TIMEOUT_FACTOR.
 */
#define SP805_TIMEOUT_FACTOR		(CONFIG_SYS_CLK_FREQ / 1000) /* 1 ms */

void sp805_wdt_reset(void);
void sp805_wdt_disable(void);

#endif /* __SP805_H */
