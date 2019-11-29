/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *	Aneesh V <aneesh@ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	.text      :
	{
		_start = .;
		arch/arm/cpu/armv7/start.o	(.text*)
		*(.text*)
		*(.rodata*);
	}

	. = ALIGN(4);
	 
	text_end  = .;
 
        . = 0x5D5C8C00;
	.data : AT (text_end) {
		data_start = .;
		*(SORT_BY_ALIGNMENT(.data*)) 
        	 __image_copy_end = .;
       		 _end = .;
		data_end = .;

		}
        .bss :
	{
           __bss_start = .;
                *(.bss*)
                . = ALIGN(4);
                __bss_end__ = .;
	}

}
