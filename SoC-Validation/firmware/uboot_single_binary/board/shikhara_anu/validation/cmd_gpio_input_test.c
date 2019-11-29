/*
 * Copyright 2000-2009
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_irq.h>

void (*f2)(int,int);
void GPIO_INPUT_handler(int,int);

void GPIO_INPUT_handler(int a,int b)
{
   printf("Key is pressed\n");
   writel(0xff,(SHIKHARA_GPIO_BANK_C+0x41c));//clearing gpio's interrupt.
}
int do_gpio_input_test(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
       printf("Checking gpio of bank2 in input mode\n");
       writel(0x0,(SHIKHARA_GPIO_BANK_C+0x400));//gpio direction register is set to input
       writel(0x0,(SHIKHARA_GPIO_BANK_C+0x404));//gpio is set to in edge trigger mode
       writel(0xff,(SHIKHARA_GPIO_BANK_C+0x410));//enable the interrupt
       f2=GPIO_INPUT_handler;
       interrupt_handler_set(GPIO2_IRQ_NUM,f2);
       int_enable(GPIO2_IRQ_NUM); //enabling gpio interrupt number at gic level. 
	return(1);
}

U_BOOT_CMD(
	gpio_input_test,	CONFIG_SYS_MAXARGS,	1,	do_gpio_input_test,
	"print command description/usage",
	"\n"
	"\n"
	"\n"
	"checking gpio in input mode"
);

