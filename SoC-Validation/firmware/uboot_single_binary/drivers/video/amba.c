/*
 * Driver for AMBA PrimeCell CLCD
 *
 * Copyright (C) 2009 Alessandro Rubini
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <lcd.h>
#include <amba_clcd.h>

#define PALETTE1            1
void lcd_cursor_init();
void show_lcd_cursor(short x, short y);

/* To set the Hardware palette Register
 * Each palette entry is made from 8bits of R,G and B data (24bits).
 * The 256-entry/16-bit-wide palette RAM is structured as 128 * 32 bits. 
 * So each word contains two palette entries. The 32 bits are as follows: 
 * MSB |I|BBBBB|GGGGG|RRRRR|I|BBBBB|GGGGG|RRRRR| LSB.
 * Since the image palette contains 8 bits of RGB the lower three bits of 
 * each color are removed so that they fit in 16 bit wide color entries.
 */
void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
 {
 struct clcd_config *config;
 struct clcd_registers *regs;

 config = (struct  clcd_config *)(panel_info1->priv);
 regs = config->address;

                int hw_reg = ((regno * 2) & ~3);

                u32 val, mask, newval;

                newval = (((blue & 0xff)>> 3)<<10) | (((green & 0xff)>>3)<< 5) | ((red & 0xff)>>3);
		
		/* Check of BigEndian Byte Order*/
                if (config->cntl & CNTL_BEBO)
                        regno ^= 1;
                if (regno & 1) {
                        newval <<= 16;
                        mask = 0x0000ffff;
                } else {
                        mask = 0xffff0000;
                }

                val = readl(regs->palette + hw_reg/4) & mask;
                writel(val | newval, (regs->palette + hw_reg/4));
}
	 	 

#ifdef CONFIG_SHIKHARA
void shikhara_pl111_lcd_init(void *lcdbase)
#else
void lcd_ctrl_init(void *lcdbase)
#endif
{
        struct clcd_config *config;
        struct clcd_registers *regs;
        u32 cntl;

        config = (struct  clcd_config *)(panel_info1->priv);
        regs = config->address;
	/*  Disable CLCD by setting low, Power and Enable bits of Control Register*/
        cntl = config->cntl & ~CNTL_LCDPWR & ~CNTL_LCDEN;
        writel(cntl, &regs->cntl);

	/* Horizontal resolution HSYNC Length, HFP, HBP */
        writel(config->tim0, &regs->tim0);

	/* Vertical resolution, VSYNC Length, VFP, VBP*/
        writel(config->tim1, &regs->tim1);

	/* CLCD clock Divisor value, Invert Pixel clock, Invert horizontal synchronization, Invert vertical synchronization */
        writel(config->tim2, &regs->tim2);

	/* Line-end signal delay 0, LCD Line end enable signal disable*/
        writel(config->tim3, &regs->tim3);

	/* Configure Framebuffer address to the CLCD controller*/
        writel((u32)lcdbase, &regs->ubas);
        
	/*  Finally Enable CLCD by setting High, Power and Enable bits of Control Register*/
        writel(cntl | CNTL_LCDPWR | CNTL_LCDEN, &regs->cntl);

printf("CLCD is configured for resolution %d x %d and %d pixeldepth\n", panel_info1->vl_col, panel_info1->vl_row, panel_info1->vl_bpix);
}
	 
void lcd_cursor_init()
{
        int i;
	struct clcd_registers *regs;
        struct clcd_config *config;
        
        config = (struct  clcd_config *)(panel_info1->priv);    
        regs = config->address;

	/*CLCD cursor registers*/
	for(i=0;i<255;i++)
        {
           #ifdef PALETTE1
                 writel(0x55555555,((regs->crsrimg)+i));
           #else
                 writel(0x0,((regs->crsrimg)+i));
           #endif
        }

        writel(0x01,&regs->crsrctrl);
     
	/*cursor size 64x64*/
        writel(0x1,&regs->crsrcnfg);
        
	/*0x00ffffff for white color*/
        writel(0x00ffffff,&regs->crsrplte0);

        /*0x00ff0000 for blue color*/ 
        writel(0x00ff0000,&regs->crsrplte1);
       
	/*clip cursor 44x44 then cursor size 20x20*/
        writel(0x2c2c,&regs->crsrclip);
}

void show_lcd_cursor(short x, short y)
{
        struct clcd_registers *regs;
        struct clcd_config *config;

        config = (struct  clcd_config *)(panel_info1->priv);
        regs = config->address;
        unsigned long posxy=0x0;

        posxy = ((y<<16) | x);

        writel(posxy,&regs->crsrxy);

}


#ifndef CONFIG_SHIKHARA
/* This is trivial, and copied from atmel_lcdfb.c */
ulong calc_fbsize(void)
{
	return ((panel_info.vl_col * panel_info.vl_row *
		NBITS(panel_info.vl_bpix)) / 8) + PAGE_SIZE;
}
#endif

