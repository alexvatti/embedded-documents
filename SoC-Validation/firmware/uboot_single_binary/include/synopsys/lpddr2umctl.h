/*
 * (C) Copyright 2011 Andes Technology Corp
 * Macpaul Lin <macpaul@andestech.com>
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
 */

/*LPDDR2 controller configuration information
 */

#ifndef __LPDDR2UMCTL_H
#define __LPDDR2UMCTL_H


#define REMAP_REG  		0xD4561098
#define DDR2_MSTR_REG  		0xD45A4000
#define DDR2_STAT_REG  		0xD45A4004
#define DDR2_MRCTRL0_REG  	0xD45A4010
#define DDR2_MRCTRL1_REG  	0xD45A4014
#define DDR2_PWRCTL_REG  	0xD45A4030
#define DDR2_PWRTMG_REG  	0xD45A4034
#define DDR2_INIT0_REG 		0xD45A40d0
#define DDR2_INIT1_REG  	0xD45A40d4
#define DDR2_INIT2_REG  	0xD45A40d8
#define DDR2_INIT3_REG  	0xD45A40dc
#define DDR2_INIT4_REG  	0xD45A40e0
#define DDR2_DRAMTMG0_REG  	0xD45A4100
#define DDR2_DRAMTMG1_REG  	0xD45A4104
#define DDR2_DRAMTMG2_REG  	0xD45A4108
#define DDR2_DRAMTMG3_REG  	0xD45A410c
#define DDR2_DRAMTMG4_REG  	0xD45A4110
#define DDR2_DRAMTMG5_REG  	0xD45A4114
#define DDR2_RFSHTMG_REG  	0xD45A4064

#define DDR2_MSTR_DATA  	0x01020000
#define DDR2_STAT_DATA  	0x00000000
#define DDR2_MRCTRL0_DATA  	0x00000010
#define DDR2_MRCTRL1_DATA  	0x00000000
#define DDR2_PWRCTL_DATA  	0x00000000
#define DDR2_PWRTMG_DATA  	0x00000010
#define DDR2_INIT0_DATA  	0x0002004E
#define DDR2_INIT1_DATA  	0x00000000
#define DDR2_INIT2_DATA  	0x00000000
#define DDR2_INIT3_DATA  	0x0A622BA0
#define DDR2_INIT4_DATA  	0x40003000
#define DDR2_DRAMTMG0_DATA  	0x19141B14
#define DDR2_DRAMTMG1_DATA  	0x00040916
#define DDR2_DRAMTMG2_DATA  	0x090A0511
#define DDR2_DRAMTMG3_DATA  	0x0000400C
#define DDR2_DRAMTMG4_DATA  	0x02030506
#define DDR2_DRAMTMG5_DATA  	0x01010404
#define DDR2_RFSHTMG_DATA  	0x0BB80034

#endif	/* __LPDDR2UMCTL_H */
