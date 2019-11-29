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
#include <asm/arch/shikhara_gic.h>
#include <asm/arch/shikhara_irq.h>

#define SHARED_MEM (volatile unsigned int *)(0x0D100000)

/* System Controller Register */
#define PMU_CONFIG_ADDR ((volatile unsigned int *)(SHIKHARA_SYS_CTRL_BASEADDR + 0x0CC))
#define SOFT_REGISTER_4 ((volatile unsigned int *)(SHIKHARA_SYS_CTRL_BASEADDR + 0x054))

/* IPC Registers */
#define IPC_REG0    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x000)) /* Dummy Register */
#define IPC_REG1    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x004)) /* Dummy Register */
#define IPC_REG2    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x008)) /* Dummy Register */
#define IPC_REG3    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x00C)) /* VIDEO Application start                                                                                                    * Address Register
                                                                        */
#define IPC_REG4    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x010)) /* ARC IPC Interrupt
                                                                        * generation register
                                                                        */
#define IPC_REG5    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x014)) /* STATUS Update Register                                                                         */
#define IPC_REG6    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x018)) /* Dummy Register */
#define IPC_REG7    ((volatile unsigned int *)(IPC_BASE_ADDR + 0x01C)) /* ARM IPC Interrupt
                                                                        * generation register
                                                                        */

void (*ipc_handler)(int,int);
void ipc_av417_handler(int,int);
unsigned int irq_check=1;

int do_arcipc (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   unsigned int irq_num;
   unsigned int vss_power,avss_reset;
   irq_num =(unsigned int)IPC_AV417;
   int_enable(irq_num);                       /* Enable the ARC interrupt line */
   ipc_handler = ipc_av417_handler;
   interrupt_handler_set(irq_num,ipc_handler); /* Setting handler for IPC Communication */
   #ifdef CONFIG_SHIKHARA_ASIC
   vss_power = readl(PMU_CONFIG_ADDR);         /* Default value of this reg is 0x0000003F
                                                * by default video subsystem is powered down
                                                * inorder to power on, set the 3rd bit in
                                                  pmu_config_addr to zero */
   vss_power=(vss_power & ~(1<<3));
   writel(vss_power,PMU_CONFIG_ADDR);
   #endif
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
   printf("interruptarc \n");
   writel(0x0000FFFF,IPC_REG4);                /* Generate Interrupt to ARC */
   writel(0x00000001,SHARED_MEM);
   return 0;
  }

void ipc_av417_handler(int a,int b)
{
  unsigned int memory_check;
  puts("in arm irq handler\n");
  memory_check = readl(SHARED_MEM);
  if(memory_check == 0x00000001)
  {
   puts(" ipc test failed \n");
  }else {
  puts("ipc test success \n");
  }
  irq_check=0;
  writel(0x0,IPC_REG7);
}

/***************************************************/

U_BOOT_CMD(
	arcipc,CONFIG_SYS_MAXARGS,0,do_arcipc,
	"ipc communication","arcipc");
