/*
 * (C) Copyright 2001
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

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>

#define APPF_INITIALIZE 0
#define POWERDOWN_CPU   1

extern int appf_entry_point(int function ,int cstate, int rstate, int flags );

extern int appf_boottime_init(void);
int i;
int do_lowpower (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	         appf_boottime_init();                     // calling the first function of lowpower mode code.
                 appf_entry_point(APPF_INITIALIZE,0,0,0);  // Intializing the functions of lowpower mode  code.
                  for(i=0;i<=10;i++)
                  {
                   if(i == 5)
                   appf_entry_point(POWERDOWN_CPU,0,0,0); // Power down the cluster the parameters are as follows
                   printf("%d\n",i);
                  }
                  
                 /* cstate:The power state that this CPU should enter.1 For powerdown 0 for powerup
                    rstate:The power state that the cluster should enter, ignored if there are other CPUs still running.
                    flags:Bitmask of extra context items to save/restore (currently only supported flag is SAVE_VFP).*/

	return 0;
}

U_BOOT_CMD(
	mos_powerdown_cputest ,    2,    1,     do_lowpower,
	"Make the processor to lowpower mode for some time",
	"N\n"
);
