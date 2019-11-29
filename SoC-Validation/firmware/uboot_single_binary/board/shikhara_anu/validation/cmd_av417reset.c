/* Copyright 2005
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
#define  PMU_CONFIG_ADDR ((volatile unsigned int *)(SHIKHARA_SYS_CTRL_BASEADDR + 0x0CC))
#define SOFT_REGISTER_4  ((volatile unsigned int *)(SHIKHARA_SYS_CTRL_BASEADDR + 0x054))

int do_av417reset (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   unsigned int avss_reset;

   /* Resetting ARC processor */
   avss_reset=readl(SOFT_REGISTER_4);   
   if((avss_reset&1)==0)
    {
      avss_reset=avss_reset|1;
      writel(avss_reset,SOFT_REGISTER_4);
      printf("system already got reset: %x\n",avss_reset);
    }
   printf("Releasing Reset\n");
   avss_reset=(avss_reset & ~(1<<0));
   writel(avss_reset,SOFT_REGISTER_4); 
   return 0;
}


int do_av417enable (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   unsigned int vss_power;
   unsigned int avss_reset;

   vss_power = readl(PMU_CONFIG_ADDR); /* Default value of this reg is 0x0000003F
                                        * by default video subsystem is powered down
                                        * inorder to power on, set the 3rd bit in pmu_config_addr to zero */
   vss_power=(vss_power & ~(1<<3));
   writel(vss_power,PMU_CONFIG_ADDR);
   printf("Enabled Power to the AV417 Video subsystem\n");

   return 0;
}


/***************************************************/

U_BOOT_CMD(
	av417reset,CONFIG_SYS_MAXARGS,0,do_av417reset,
	"To Reset AV417 video subsystem","av417reset");


U_BOOT_CMD(
	av417enable,CONFIG_SYS_MAXARGS,0,do_av417enable,
	"Power-up AV417 video subsystem","av417enable");
