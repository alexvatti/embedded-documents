/*
 * (C) Copyright 2010 Linaro
 * Matt Waddel, <matt.waddel@linaro.org>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _SYSTIMER_H_
#define _SYSTIMER_H_

/* AMBA timer register base address */

/* SYSTIMER_RELOAD value required for a 1sec with presclar value is 1 is as follows

   TIMCLK = 50MHZ 

TimerXLoad =  (Interval x TIMCLKFREQ)/(TIMCLKENXDIV x PRESCALEDIV)

Assuming no prescaler, hence PRESCALEDIV=1;It can be prtogrammed to 16, 256. 
Let TIMCLKENXDIV is always 1 means the counter is decremented on every TIMCLK edge

TimerXLoad =  (1sec * 50*10^6)/(1 *1) = 50^6= 0x2FAF080

*/
#define SYSTIMER_MAXVAL		0xFFFFFFFF  // TimerXLoad Maximun value 
#define SYSTIMER_XLOAD		0x2FAF080  // TimerXLoad value for 1sec interval 
#define SYSTIMER_EN		(1 << 7)   //  Enable timer module
#define SYSTIMER_32BIT		(1 << 1)   //  select 32 bit counter
#define SYSTIMER_INT_DISABLE    (0 << 5)   //  Disable interrupt bit
#define SYSTIMER_IEN            (1 << 5)   //   Enable interrupt bit
#define SYSTIMER_ONESHOT        (1 << 0)   //   select one shot mode
#define SYSTIMER_FREERUNNING    (0 << 0)   //   select one shot mode
#define SYSTIMER_PRESCALE_DIV1	(0 << 2)   //  0 stages of prescale, clock is divided by 1
#define SYSTIMER_PRESCALE_DIV16	(1 << 2)   //  4 stages of prescale, clock is divided by 16
#define SYSTIMER_PRESCALE_DIV256 (2 << 2)  //   8 stages of prescale, clock is divided by 256

struct systimer {
	u32 timer0load;		/* 0x00 */
	u32 timer0value;
	u32 timer0control;
	u32 timer0intclr;
	u32 timer0ris;
	u32 timer0mis;
	u32 timer0bgload;
	u32 dummy;
	u32 timer1load;		/* 0x20 */
	u32 timer1value;
	u32 timer1control;
	u32 timer1intclr;
	u32 timer1ris;
	u32 timer1mis;
	u32 timer1bgload;
};
#endif /* _SYSTIMER_H_ */
