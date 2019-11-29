/*
 * Register definitions for the AMBA CLCD logic cell.
 *
 * derived from David A Rusling, although rearranged as a C structure
 *     linux/include/asm-arm/hardware/amba_clcd.h -- Integrator LCD panel.
 *
 * Copyright (C) 2001 ARM Limited
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

/*
 * CLCD Controller Internal Register addresses
 */

struct clcd_registers {
	u32 tim0;	/* 0x00 */
	u32 tim1;
	u32 tim2;
	u32 tim3;
	u32 ubas;	/* 0x10 */
	u32 lbas;
#if !defined(CONFIG_ARCH_VERSATILE) && !defined(CONFIG_ARCH_REALVIEW) && !defined(CONFIG_SHIKHARA)
	u32 ienb;
	u32 cntl;
#else /* Someone rearranged these two registers on the Versatile */
	u32 cntl;
	u32 ienb;
#endif
	u32 stat;	/* 0x20 */
	u32 intr;
	u32 ucur;
	u32 lcur;
	u32 unused[0x74];	/* 0x030..0x1ff */
	u32 palette[0x80];	/* 0x200..0x3ff */
#ifdef CONFIG_SHIKHARA
	u32 unused1[0x100];      /*0x400..0x7ff*/
	u32 crsrimg[0x100];      /*0x800..0xbff*/
	u32 crsrctrl; 
	u32 crsrcnfg;
	u32 crsrplte0;
	u32 crsrplte1;
	u32 crsrxy;
	u32 crsrclip;
#endif
};

/* Bit definition for TIM2 */
#define TIM2_CLKSEL		(1 << 5)
#define TIM2_IVS		(1 << 11)
#define TIM2_IHS		(1 << 12)
#define TIM2_IPC		(1 << 13)
#define TIM2_IOE		(1 << 14)
#define TIM2_BCD		(1 << 26)

/* Bit definitions for control register */
#define CNTL_LCDEN		(1 << 0)
#define CNTL_LCDBPP1		(0 << 1)
#define CNTL_LCDBPP2		(1 << 1)
#define CNTL_LCDBPP4		(2 << 1)
#define CNTL_LCDBPP8		(3 << 1)
#define CNTL_LCDBPP16		(4 << 1)
#define CNTL_LCDBPP16_565	(6 << 1)
#define CNTL_LCDBPP24		(5 << 1)
#define CNTL_LCDBW		(1 << 4)
#define CNTL_LCDTFT		(1 << 5)
#define CNTL_LCDMONO8		(1 << 6)
#define CNTL_LCDDUAL		(1 << 7)
#define CNTL_BGR		(1 << 8)
#define CNTL_BEBO		(1 << 9)
#define CNTL_BEPO		(1 << 10)
#define CNTL_LCDPWR		(1 << 11)
#define CNTL_LCDVCOMP(x)	((x) << 12)
#define CNTL_LDMAFIFOTIME	(1 << 15)
#define CNTL_WATERMARK		(1 << 16)

#define CLCD_LED            0       // Line-end signal delay period
#define CLCD_LEE            0       // Line-end signal enable

#define PALETTE_WORDS       128     // Number of words in palette

#if defined(SHIKHARA_CONFIG_CORETILE)

// Low resolution 600 x 480
#define VGA_0_FREQ      23750000  //in Hz
// Medium resolution 800 x 600
#define VGA_1_FREQ      38250000  // in Hz
// High resolution 1024 x 768
#define VGA_2_FREQ      63500000  // in Hz
// Super High resolution 1280 x 1024
#define VGA_3_FREQ     109000000  // in Hz
// Ultra High resolution 1600 x 1200
#define VGA_4_FREQ     161000000  // in Hz
// 1080p resolution 1920 x 1080
#define VGA_5_FREQ     173000000 // in Hz

#endif

/* u-boot specific: information passed by the board file */
struct clcd_config {
	struct clcd_registers *address;
	u32			tim0;
	u32			tim1;
	u32			tim2;
	u32			tim3;
	u32			cntl;

#ifndef CONFIG_SHIKHARA
	unsigned long		pixclock;
#endif

};
