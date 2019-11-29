/*
 * Copyright 2013 Moschip Semiconductor.
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <div64.h>
#include <linux/types.h>        /* for size_t */
#include <linux/stddef.h>       /* for NULL */
#include <asm/io.h>
#include <asm/arch-armv7/systimer.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/gpio.h>
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch-armv7/wdt.h>

#define BOOT_LED_BLINK_0               SHIKHARA_GPIO_PA0

static unsigned long long timestamp; 
static unsigned long long lastinc;
static struct systimer *dt0timer_base = (struct systimer *)SHIKHARA_DT0_BASE;
#ifdef CONFIG_CORE_LOWPOWER_MODE
static struct systimer *dt1timer_base = (struct systimer *)SHIKHARA_DT1_BASE;
#endif
void boot_led_blink_init(void);
void led_blink_handler(void);
void dttimer_handler(void);
extern void platform_reset_handler(void);
void (*dt1)(void);
int timer_power_init(void);

#ifndef CONFIG_BL1_BUILD
#ifndef CONFIG_DT_TEST

void led_blink_handler(void)
{
     gpio_set_value(BOOT_LED_BLINK_0,~(gpio_get_value(BOOT_LED_BLINK_0)));
     writel(0x10, &dt0timer_base->timer1intclr); /* status register bit is cleared by writing any value */
}

void boot_led_blink_init(void)
{
        /*
         * Setup counter1 of dt0
         */

        /*
           SYSTIMER_EN          ->   Enabling the timer1 module
           SYSTIMER_32BIT       ->   Selecting the 32 bit counter
           SYSTIMER_INT_DISABLE ->   Disabling the timer interrupt

        By default in timer0 control register the following are enabled
         1) wrapping mode
         2) prescalr vlaue selection 00 = 0 stages of prescale, clock is divided by 1
         3) Timer module is in free-running mode(auto reload mode)

        */
	u32 tcon;

         // Initialise DT-0 timer of counter 1
        writel(SYSTIMER_XLOAD, &dt0timer_base->timer1load); // loading counter value
	tcon =readl(&dt0timer_base->timer1control);
	tcon |=SYSTIMER_EN | SYSTIMER_IEN | SYSTIMER_32BIT | SYSTIMER_ONESHOT;
        writel(tcon,&dt0timer_base->timer1control);
	gpio_direction_output(BOOT_LED_BLINK_0,0); //setting the output direction for gpio pin 0
        int_enable(DT0_IRQ_NUM); // enabling the interrupt of GIC
        interrupt_handler_set(DT0_IRQ_NUM,led_blink_handler);
}
#endif

#ifdef CONFIG_CORE_LOWPOWER_MODE

void dttimer_handler(void)
{
    printf("DT1 timer's interrupt is fired of counter1\n");
    writel(0x10, &dt1timer_base->timer1intclr); // clearing status register
    writel(0x0,  &dt1timer_base->timer1control); // clearing the timer control register results default bits are set
    platform_reset_handler(); // restoring the context
}

int timer_power_init(void)
{
       u32 *timer_control,*timer_load,tcon;
       uchar  irq_num;
       timer_control=&dt1timer_base->timer1control;
       irq_num=(uchar)DT1_IRQ_NUM;
       timer_load = &dt1timer_base->timer1load;
       dt1=dttimer_handler;
        // Initialise DT timer
        writel(SYSTIMER_XLOAD,timer_load);
        // setting the mode into one shot
        tcon=readl(timer_control);
        tcon |=SYSTIMER_EN | SYSTIMER_IEN | SYSTIMER_32BIT | SYSTIMER_ONESHOT; // enabling the timer
        writel(tcon,timer_control);
        int_enable(irq_num);
        interrupt_handler_set(irq_num,dt1);
}
#endif

#endif //CONFIG_BL1_BUILD


/*
 * Start the timer
 */
int timer_init(void)
{
	/*
	 * Setup timer0 of dt0
	 */

	/* 
           SYSTIMER_EN          ->   Enabling the timer0 module
           SYSTIMER_32BIT       ->   Selecting the 32 bit counter
	   SYSTIMER_INT_DISABLE ->   Disabling the timer interrupt 

	By default in timer0 control register the following are enabled and its default value is 0x20
         1) wrapping mode
	 2) prescalar value selection 00 = 0 stages of prescale, clock is divided by 1
	 3) Timer module is in free-running mode(auto reload mode)
	 4) Interrupt bit is enabled
	*/
	u32 tcon;
	writel(SYSTIMER_MAXVAL,&dt0timer_base->timer0load);
	tcon = SYSTIMER_EN | SYSTIMER_32BIT | SYSTIMER_INT_DISABLE | SYSTIMER_FREERUNNING | SYSTIMER_PRESCALE_DIV1;
	writel(tcon,&dt0timer_base->timer0control); //enable counter0
	reset_timer_masked();

	return 0;

}

#define TICK_PER_TIME	((DT_CLOCK + CONFIG_SYS_HZ / 2) / CONFIG_SYS_HZ)
#define NS_PER_TICK	(1000000000 / DT_CLOCK) // Number of nano-seconds per tick

static inline unsigned long long tick_to_time(unsigned long long tick)
{
	do_div(tick, TICK_PER_TIME);
	return tick;
}

static inline unsigned long long time_to_tick(unsigned long long time)
{
	return time * TICK_PER_TIME;
}

static inline unsigned long long us_to_tick(unsigned long long us)
{
	unsigned long long tick = us * 1000;
	tick += NS_PER_TICK - 1;
	do_div(tick, NS_PER_TICK);
	return tick;
}

unsigned long long get_ticks(void)
{
	ulong now = ~readl(&dt0timer_base->timer0value);

	if (now >= lastinc)	/* normal mode (non roll) */
		/* move stamp forward with absolut diff ticks */
		timestamp += (now - lastinc);
	else			/* we have rollover of incrementer */
		timestamp += (0xFFFFFFFF - lastinc) + now;
	lastinc = now;
	return timestamp; //timestamp is the number of ticks since reset
}

/*
 * Delay x useconds AND preserve advance timstamp value
 *     assumes timer is ticking at 1 msec
 */
void __udelay(ulong usec)
{
	unsigned long long tmp;
	ulong tmo;

	tmo = us_to_tick(usec);
	tmp = get_ticks() + tmo;	/* get current timestamp */

	while (get_ticks() < tmp)	/* loop till event */
		 /*NOP*/;
}

ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

void reset_timer_masked(void)
{
	lastinc = ~readl(&dt0timer_base->timer0value);
	timestamp = 0;
}

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer_masked(void)
{
	return tick_to_time(get_ticks());
}
/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
        return CONFIG_SYS_HZ;
}




