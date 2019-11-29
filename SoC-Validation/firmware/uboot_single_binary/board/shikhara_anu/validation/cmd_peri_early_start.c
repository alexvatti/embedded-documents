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
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#include <asm/arch/shikhara_i2c.h>
#endif
#include"../../../include/keyboard.h"
#include"../../../include/mmc.h"
#include"../../../include/nand.h"
#include"../../../include/can.h"

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SHIKHARA_PERI_DELAYSTART

#if defined(CONFIG_PS2KBD)
int do_kbd_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
         puts("PS/2 initialization\n");
         kbd_init();
	 return(0);
}
U_BOOT_CMD(
	kbd_start, CONFIG_SYS_MAXARGS, 1, do_kbd_start,
        "\n",
	"kbd is initializing"
);
#endif


#ifdef CONFIG_GENERIC_MMC
int do_mmc_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
         puts("MMC initialization:\n");
         mmc_initialize(gd->bd);

	 return(0);
}
U_BOOT_CMD(
	mmc_start, CONFIG_SYS_MAXARGS, 1, do_mmc_start,
        "\n",
	"mmc is initializing"
);
#endif

#if defined(CONFIG_CMD_NAND)
int do_nand_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
         puts("NAND: ");
         nand_init();
	 return(0);
}
U_BOOT_CMD(
	nand_start, CONFIG_SYS_MAXARGS, 1, do_nand_start,
        "\n",
	"smc nand is initializing"
);
#endif

#if defined(CONFIG_CMD_NOR)
int do_nor_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
         puts("SMC initialization with nor:\n");
         smc_init_nor();
	 return(0);
}
U_BOOT_CMD(
	nor_start, CONFIG_SYS_MAXARGS, 1, do_nor_start,
        "\n",
	"smc nor is initializing"
);
#endif
#if defined(CONFIG_CMD_SRAM)
int do_sram_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
         puts("SMC initialization with RAM:\n");
         smc_init_sram();
	 return(0);
}
U_BOOT_CMD(
	sram_start, CONFIG_SYS_MAXARGS, 1, do_sram_start,
        "\n",
	"smc sram is initializing"
);
#endif

#if !defined(CONFIG_SYS_NO_FLASH)
int do_flash_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
       ulong flash_size = flash_init();
       print_size(flash_size, "");

	return(0);
}
U_BOOT_CMD(
	flash_start, CONFIG_SYS_MAXARGS, 1, do_flash_start,
        "\n",
	"flash is initializing"
);
#endif

#ifdef CONFIG_CAN
int do_can_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
      printf("can is initializing\n");
      can_initialize(gd->bd);
	return(0);
}
U_BOOT_CMD(
	can_start, CONFIG_SYS_MAXARGS, 1, do_can_start,
        "\n",
	"can is initializing"
);
#endif

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
int do_i2c_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	printf("I2c is initializing\n");
        puts("I2C:   ");
#ifndef CONFIG_I2C_MULTI_BUS		
        i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#else     
        i2c_set_bus_num(I2C_BUS0);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
        i2c_set_bus_num(I2C_BUS1);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
        i2c_set_bus_num(I2C_BUS2);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
 #endif
	return(0);
}

U_BOOT_CMD(
	i2c_start, CONFIG_SYS_MAXARGS,	1, do_i2c_start,
        "\n",
	"I2c is initializing"
);
#endif

#ifdef CONFIG_PS2KBD
int do_kmi_start(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
      printf("kmi is initializing\n");
      kmi_init();
      return(0);
}

U_BOOT_CMD(
	kmi_start, CONFIG_SYS_MAXARGS,	1, do_kmi_start,
        "\n",
	"kmi initialization"
);
#endif

#endif //CONFIG_SHIKHARA_PERI_DELAYSTART
