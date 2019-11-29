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
#include <asm/arch/shikhara_clock.h>

#ifdef CONFIG_SHIKHARA_ZYNQBASED_FPGA /*To satisy QEMU comilation*/

int do_report(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
       float z;
       long long unsigned int x;
       long long unsigned int y;
       x=get_ticks();
       memcpy((CONFIG_ZYNQ_DDR_BASE+0x1C0000),(CONFIG_ZYNQ_DDR_BASE), 0x100000);
       y=get_ticks();
       z=(y-x)/(DT_CLOCK *10000000);
       printf("%f is the time to move a  data from zynq to zynq in sec\n",z);

        x=get_ticks();
	memcpy((CONFIG_V7FPGA_DDR_BASE+0x5000000),(CONFIG_V7FPGA_DDR_BASE+0x1000000), 0x100000);
        y=get_ticks();
	z=(y-x)/(DT_CLOCK *10000000);
        printf("%f is the time to move a  data from v7 to v7 in sec\n",z);


        x=get_ticks();
	memcpy(CONFIG_V7FPGA_DDR_BASE,CONFIG_ZYNQ_DDR_BASE, 0x100000);
        y=get_ticks();
	z=(y-x)/(DT_CLOCK *10000000);
       printf("%f is the time to move a  data from zynq to v7 in sec\n",z);

	
	return 0;
}

U_BOOT_CMD(
	mem_report, CONFIG_SYS_MAXARGS,	1,do_report,
	"print command description/usage",
	"\n"
	"	- print brief description of all commands\n"
	""
	"Gives the memory report"
);
#endif
