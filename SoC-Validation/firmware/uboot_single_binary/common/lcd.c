/*
 * Common LCD routines for supported CPUs
 *
 * (C) Copyright 2001-2002
 * Wolfgang Denk, DENX Software Engineering -- wd@denx.de
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

/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

/* #define DEBUG */

#include <config.h>
#include <common.h>
#include <command.h>
#include <stdarg.h>
#include <linux/types.h>
#include <stdio_dev.h>
#if defined(CONFIG_POST)
#include <post.h>
#endif
#include <lcd.h>
#include <watchdog.h>
#include <amba_clcd.h>
#ifdef CONFIG_CMD_FLYER
#include <flyer.h>
#endif

DECLARE_GLOBAL_DATA_PTR;
extern void lcd_enable(void);

#if defined(CONFIG_CPU_PXA25X) || defined(CONFIG_CPU_PXA27X) || \
	defined(CONFIG_CPU_MONAHANS)
#define CONFIG_CPU_PXA
#include <asm/byteorder.h>
#endif

#if defined(CONFIG_MPC823)
#include <lcdvideo.h>
#endif

#if defined(CONFIG_ATMEL_LCD)
#include <atmel_lcdc.h>
#endif

/************************************************************************/
/* ** FONT DATA								*/
/************************************************************************/
#include <video_font.h>		/* Get font data, width and height	*/
#include <video_font_data.h>

/************************************************************************/
/* ** LOGO DATA								*/
/************************************************************************/
#ifdef CONFIG_LCD_LOGO
# include <bmp_logo.h>		/* Get logo data, width and height	*/
# include <bmp_logo_data.h>
# if (CONSOLE_COLOR_WHITE >= BMP_LOGO_OFFSET) && (LCD_BPP != LCD_COLOR16) && (LCD_BPP !=LCD_COLOR24)
#  error Default Color Map overlaps with Logo Color Map
# endif
#endif

DECLARE_GLOBAL_DATA_PTR;

ulong lcd_setmem (ulong addr);

static void lcd_drawchars (ushort x, ushort y, uchar *str, int count);
extern inline void lcd_puts_xy (ushort x, ushort y, uchar *s);
static inline void lcd_putc_xy (ushort x, ushort y, uchar  c);

static int lcd_init (void *lcdbase);

static void *lcd_logo (void);

static int lcd_getbgcolor (void);
static void lcd_setfgcolor (int color);
static void lcd_setbgcolor (int color);

/*Commands function prototypes*/
void fill_screen(unsigned color);
void lcd_draw_rectangle(int x,int y,int width,int height, unsigned int color);
void show_flyer(void);

char lcd_is_enabled = 0;

/* These variables are required by lcd.c -- although it sets them by itself */
int lcd_color_fg;
int lcd_color_bg;
void *lcd_console_address;
short console_col;
short console_row;

BGRA lcd_color_24_fg = {.Blue = 0xff, .Green = 0xff, .Red = 0xff}, lcd_color_24_bg = {.Blue = 0, .Green = 0, .Red = 0};

/* Horizontal Line Length for PL111 in Shikhara */
int lcd_line_length;

/* Horizontal Line Length for DSI*/
#ifdef CONFIG_DSI
int lcd_line_length2;
#endif

/* Horizontal Line Length for DSI*/
#ifdef CONFIG_HDMI
int lcd_line_length3;
#endif

#ifdef	NOT_USED_SO_FAR
static void lcd_getcolreg (ushort regno,
				ushort *red, ushort *green, ushort *blue);
static int lcd_getfgcolor (void);
#endif	/* NOT_USED_SO_FAR */

/************************************************************************/

/*----------------------------------------------------------------------*/

static void console_scrollup (void)
{
#if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)
	/* Copy up rows ignoring the first one */
	memcpy (CONSOLE_ROW_FIRST, CONSOLE_ROW_SECOND, CONSOLE_SCROLL_SIZE);

	/* Clear the last one */
	memset (CONSOLE_ROW_LAST, COLOR_MASK(lcd_color_bg), CONSOLE_ROW_SIZE);
#endif
}

/*----------------------------------------------------------------------*/

static inline void console_back (void)
{
	if (--console_col < 0) {
		console_col = CONSOLE_COLS-1 ;
		if (--console_row < 0) {
			console_row = 0;
		}
	}

	lcd_putc_xy (console_col * VIDEO_FONT_WIDTH,
		     console_row * VIDEO_FONT_HEIGHT,
		     ' ');
}

/*----------------------------------------------------------------------*/

static inline void console_newline (void)
{
#ifdef CONFIG_PL111
	++console_row;
	console_col = 0;

	/* Check if we need to scroll the terminal */
	if (console_row >= CONSOLE_ROWS) {
		/* Scroll everything up */
		console_scrollup () ;
		--console_row;
	}
#endif
}

/*----------------------------------------------------------------------*/

void lcd_putc (const char c)
{
#if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)
	if (!lcd_is_enabled) {
		serial_putc(c);
		return;
	}

	switch (c) {
	case '\r':	console_col = 0;
			return;

	case '\n':	console_newline();
			return;

	case '\t':	/* Tab (8 chars alignment) */
			console_col +=  8;
			console_col &= ~7;

			if (console_col >= CONSOLE_COLS) {
				console_newline();
			}
			return;

	case '\b':	console_back();
			return;

	default:	lcd_putc_xy (console_col * VIDEO_FONT_WIDTH,
				     console_row * VIDEO_FONT_HEIGHT,
				     c);
			if (++console_col >= CONSOLE_COLS) {
				console_newline();
			}
			return;
	}
	/* NOTREACHED */
#endif /* if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111) */
}

/*----------------------------------------------------------------------*/

void lcd_puts (const char *s)
{
	if (!lcd_is_enabled) {
		serial_puts (s);
		return;
	}

	while (*s) {
		lcd_putc (*s++);
	}
}

/*----------------------------------------------------------------------*/

void lcd_printf(const char *fmt, ...)
{
	va_list args;
	char buf[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	lcd_puts(buf);
}

/************************************************************************/
/* ** Low-Level Graphics Routines					*/
/************************************************************************/

static void lcd_drawchars (ushort x, ushort y, uchar *str, int count)
{

#if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)
	uchar *dest;
	ushort row;

#if LCD_BPP == LCD_MONOCHROME
	ushort off  = x * (1 << LCD_BPP) % 8;
#endif

	dest = (uchar *)(lcd_base + y * lcd_line_length + x * (1 << LCD_BPP) / 8);

	for (row=0;  row < VIDEO_FONT_HEIGHT;  ++row, dest += lcd_line_length)  {
		uchar *s = str;
		int i;
#if LCD_BPP == LCD_COLOR16
		ushort *d = (ushort *)dest;
#elif LCD_BPP == LCD_COLOR24
		uint *d = (uint *)dest; 
		uchar *dptr = dest;
#else
		uchar *d = dest;
#endif

#if LCD_BPP == LCD_MONOCHROME
		uchar rest = *d & -(1 << (8-off));
		uchar sym;
#endif
		for (i=0; i<count; ++i) {
			uchar c, bits;

			c = *s++;
			bits = video_fontdata[c * VIDEO_FONT_HEIGHT + row];

#if LCD_BPP == LCD_MONOCHROME
			sym  = (COLOR_MASK(lcd_color_fg) & bits) |
			       (COLOR_MASK(lcd_color_bg) & ~bits);

			*d++ = rest | (sym >> off);
			rest = sym << (8-off);
#elif LCD_BPP == LCD_COLOR8
			for (c=0; c<8; ++c) {
				*d++ = (bits & 0x80) ? lcd_color_fg : lcd_color_bg;
				bits <<= 1;
			}
#elif LCD_BPP == LCD_COLOR16
			for (c=0; c<8; ++c) {
				*d++ = (bits & 0x80) ? lcd_color_fg : lcd_color_bg;
				bits <<= 1;
			}

#endif
		}
#if LCD_BPP == LCD_MONOCHROME
		*d  = rest | (*d & ((1 << (8-off)) - 1));
#endif
	}

#endif /* #if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)*/
}

/*----------------------------------------------------------------------*/

#ifdef CONFIG_SHIKHARA
/* Function shikhara_lcd_drawchars_24_bit() is implemented to draw 24-bit chararcters for PL111, DSI and HDMI 
 * This will work only with one flag should enabled among them. */
void shikhara_lcd_drawchars_24_bit(ushort x, ushort y, uchar *str, int count)
{
        uchar *dest;
        ushort row;

#ifdef CONFIG_PL111
        dest = (uchar *)(lcd_base + y * lcd_line_length + x * (1 << LCD_BPP) / 8);
        for (row=0;  row < VIDEO_FONT_HEIGHT;  ++row, dest += lcd_line_length)
#endif
#ifdef CONFIG_DSI
        dest = (uchar *)(lcd_base2 + y * lcd_line_length2 + x * (panel_info2->vl_bpix) / 8);
        for (row=0;  row < VIDEO_FONT_HEIGHT;  ++row, dest += lcd_line_length2)
#endif
#ifdef CONFIG_HDMI
        dest = (uchar *)(lcd_base3 + y * lcd_line_length3 + x * (panel_info3->vl_bpix) / 8);
        for (row=0;  row < VIDEO_FONT_HEIGHT;  ++row, dest += lcd_line_length3)
#endif

        {
                uchar *s = str;
                int i;
                uchar *dptr = dest;
                for (i=0; i<count; ++i)
                {
                        uchar c, bits;
                        c = *s++;
                        bits = video_fontdata[c * VIDEO_FONT_HEIGHT + row];
                        for (c=0; c<8; ++c)
                        {
                                *dptr =  (bits & 0x80) ? lcd_color_24_fg.Blue : lcd_color_24_bg.Blue;
                                dptr++;
                                *dptr =  (bits & 0x80) ? lcd_color_24_fg.Green : lcd_color_24_bg.Green;
                                dptr++;
                                *dptr =  (bits & 0x80) ? lcd_color_24_fg.Red : lcd_color_24_bg.Red;
                                dptr++;
/* LCD controller (PL-111) will take an extra byte for 24-bits. It will consider 32-bit values as one pixel information  */
#ifdef CONFIG_PL111
                                dptr++;
#endif
                                bits <<= 1;
                        }
               }
        }

}
#endif //End of #ifdef CONFIG_SHIKHARA



inline void lcd_puts_xy (ushort x, ushort y, uchar *s)
{
#if defined(CONFIG_LCD_LOGO) && !defined(CONFIG_LCD_INFO_BELOW_LOGO)
#if LCD_BPP == LCD_COLOR24
        shikhara_lcd_drawchars_24_bit(x, y+BMP_LOGO_HEIGHT, s, strlen ((char *)s))
#else
        lcd_drawchars (x, y+BMP_LOGO_HEIGHT, s, strlen ((char *)s));
#endif
#else
#if LCD_BPP == LCD_COLOR24
        shikhara_lcd_drawchars_24_bit(x, y, s, strlen ((char *)s));
#else
        lcd_drawchars (x, y, s, strlen ((char *)s));
#endif

#endif
}

/*----------------------------------------------------------------------*/

static inline void lcd_putc_xy (ushort x, ushort y, uchar c)
{
#if defined(CONFIG_LCD_LOGO) && !defined(CONFIG_LCD_INFO_BELOW_LOGO)
#if LCD_BPP == LCD_COLOR24
        shikhara_lcd_drawchars_24_bit(x, y+BMP_LOGO_HEIGHT, &c, 1);
#else
        lcd_drawchars (x, y+BMP_LOGO_HEIGHT, &c, 1);
#endif
#else
#if LCD_BPP == LCD_COLOR24
        shikhara_lcd_drawchars_24_bit(x, y, &c, 1);
#else
        lcd_drawchars (x, y, &c, 1);
#endif
#endif
}

/************************************************************************/
/**  Small utility to check that you got the colours right		*/
/************************************************************************/
#ifdef LCD_TEST_PATTERN

#define	N_BLK_VERT	2
#define	N_BLK_HOR	3

static int test_colors[N_BLK_HOR*N_BLK_VERT] = {
	CONSOLE_COLOR_RED,	CONSOLE_COLOR_GREEN,	CONSOLE_COLOR_YELLOW,
	CONSOLE_COLOR_BLUE,	CONSOLE_COLOR_MAGENTA,	CONSOLE_COLOR_CYAN,
};

static void test_pattern (void)
{
	ushort v_max  = panel_info.vl_row;
	ushort h_max  = panel_info.vl_col;
	ushort v_step = (v_max + N_BLK_VERT - 1) / N_BLK_VERT;
	ushort h_step = (h_max + N_BLK_HOR  - 1) / N_BLK_HOR;
	ushort v, h;
	uchar *pix = (uchar *)lcd_base;

	printf ("[LCD] Test Pattern: %d x %d [%d x %d]\n",
		h_max, v_max, h_step, v_step);

	/* WARNING: Code silently assumes 8bit/pixel */
	for (v=0; v<v_max; ++v) {
		uchar iy = v / v_step;
		for (h=0; h<h_max; ++h) {
			uchar ix = N_BLK_HOR * iy + (h/h_step);
			*pix++ = test_colors[ix];
		}
	}
}
#endif /* LCD_TEST_PATTERN */


/* Function: 
 * drv_lcd_init() ---- GENERIC Initialization Routines.					
 */

int drv_lcd_init (void)
{
	struct stdio_dev lcddev;
	int rc;

#if defined(CONFIG_DSI)

	lcd_base2 = (void *)(gd->fb_base2);        
	lcd_line_length2 = (panel_info2->vl_col * panel_info2->vl_bpix) / 8;
	mipi_init (lcd_base2, panel_info2);

#endif

#if defined(CONFIG_HDMI)

	lcd_base3 = (void *)(gd->fb_base3);
	lcd_line_length3 = (panel_info3->vl_col * panel_info3->vl_bpix) / 8;
	hdmi_init(lcd_base3, panel_info3);

#endif

#ifdef CONFIG_PL111

        lcd_base = (void *)(gd->fb_base);

/* CLCD PL111 leave 1 byte extra in framebuffer when configured for 24bpp*/
        if(panel_info1->vl_bpix == 24)
                lcd_line_length = (panel_info1->vl_col *  32) / 8;
        else
                lcd_line_length = (panel_info1->vl_col *  panel_info1->vl_bpix) / 8;

        lcd_init (lcd_base);
#else 
/* To Enable and clear DSI and HDMI LCD Panel (if CLCD is not Enable) */
	lcd_clear();
	lcd_enable();
#endif

#ifndef CONFIG_SHIKHARA
	lcd_line_length = (panel_info.vl_col * NBITS (panel_info.vl_bpix)) / 8;
#endif

	/* Device initialization */
	memset (&lcddev, 0, sizeof (lcddev));

	strcpy (lcddev.name, "lcd");
	lcddev.ext   = 0;			/* No extensions */
	lcddev.flags = DEV_FLAGS_OUTPUT;	/* Output only */
	lcddev.putc  = lcd_putc;		/* 'putc' function */
	lcddev.puts  = lcd_puts;		/* 'puts' function */

	rc = stdio_register (&lcddev);

	return (rc == 0) ? 1 : rc;
}

/*----------------------------------------------------------------------*/
static
int do_lcd_clear(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	lcd_clear();
	return 0;
}

/* Function:
 *  lcd_clear() clears the CLCD, initilize the hardware palette with some basic
 *  colors and shows chip dependent logo.
 *  This function gets called at the time of CLCD initilization and also when 
 *  'cls' command issued.
 */
void lcd_clear(void)
{
#if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)
#if LCD_BPP == LCD_MONOCHROME
	/* Setting the palette */
	lcd_initcolregs();
	
#elif (LCD_BPP == LCD_COLOR8)  
	/* Setting the palette, Default colors */
	lcd_setcolreg  (CONSOLE_COLOR_BLACK,       0,    0,    0);
	lcd_setcolreg  (CONSOLE_COLOR_RED,	0xFF,    0,    0);
	lcd_setcolreg  (CONSOLE_COLOR_GREEN,       0, 0xFF,    0);
	lcd_setcolreg  (CONSOLE_COLOR_YELLOW,	0xFF, 0xFF,    0);
	lcd_setcolreg  (CONSOLE_COLOR_BLUE,        0,    0, 0xFF);
	lcd_setcolreg  (CONSOLE_COLOR_MAGENTA,	0xFF,    0, 0xFF);
	lcd_setcolreg  (CONSOLE_COLOR_CYAN,	   0, 0xFF, 0xFF);
	lcd_setcolreg  (CONSOLE_COLOR_GREY,	0xAA, 0xAA, 0xAA);
	lcd_setcolreg  (CONSOLE_COLOR_WHITE,	0xFF, 0xFF, 0xFF);
#endif

#ifndef CONFIG_SYS_WHITE_ON_BLACK
	lcd_setfgcolor (CONSOLE_COLOR_BLACK);
	lcd_setbgcolor (CONSOLE_COLOR_WHITE);
#else
	lcd_setfgcolor (CONSOLE_COLOR_WHITE);
	lcd_setbgcolor (CONSOLE_COLOR_BLACK);
#endif	/* CONFIG_SYS_WHITE_ON_BLACK */

#ifdef	LCD_TEST_PATTERN
	test_pattern();
#else
	/* set framebuffer to background color */
	memset ((char *)lcd_base, COLOR_MASK(lcd_getbgcolor()), lcd_line_length*panel_info1->vl_row);

#endif /* LCD_TEST_PATTERN */

	/* Paint the logo and retrieve LCD base address */
	debug ("[LCD] Drawing the logo...\n");
	lcd_console_address = lcd_logo ();

	console_col = 0;
	console_row = 0;

#endif /* #if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111) */

#ifdef CONFIG_DSI
        memset ((char *)lcd_base2, 0xffffff ,lcd_line_length2*panel_info2->vl_row);
#endif

#ifdef CONFIG_HDMI
        memset ((char *)lcd_base3, 0xffffff, lcd_line_length3*panel_info3->vl_row);
#endif

}

U_BOOT_CMD(
	cls,	1,	1,	do_lcd_clear,
	"clear screen",
	""
);

/*----------------------------------------------------------------------*/

static int lcd_init (void *lcdbase)
{
	lcd_ctrl_init(lcdbase);
	lcd_is_enabled = 1;
	lcd_clear();
	lcd_enable();

	/* Initialize the console */
	console_col = 0;
#ifdef CONFIG_LCD_INFO_BELOW_LOGO
	console_row = 7 + BMP_LOGO_HEIGHT / VIDEO_FONT_HEIGHT;
#else
	console_row = 1;	/* leave 1 blank line below logo */
#endif

	return 0;
}


/************************************************************************/
/* ** ROM capable initialization part - needed to reserve FB memory	*/
/************************************************************************/
/*
 * This is called early in the system initialization to grab memory
 * for the LCD controller.
 * Returns new address for monitor, after reserving LCD buffer memory
 *
 * Note that this is running from ROM, so no write access to global data.
 */
ulong lcd_setmem (ulong addr)
{
	ulong size;

#if defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111) 

        int line_length = (panel_info1->vl_col *  panel_info1->vl_bpix) / 8;
	size = line_length * panel_info1->vl_row;

#elif !defined(CONFIG_SHIKHARA)	
	int line_length = (panel_info.vl_col * NBITS (panel_info.vl_bpix)) / 8;
	size = line_length * panel_info.vl_row;
	
	debug ("LCD panel info: %d x %d, %d bit/pix\n",	panel_info.vl_col, panel_info.vl_row, NBITS (panel_info.vl_bpix));
#endif

	/* Round up to nearest full page */
	size = (size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

	/* Allocate pages for the frame buffer. */
	addr -= size;

	debug ("Reserving %ldk for LCD Framebuffer at: %08lx\n", size>>10, addr);

	return (addr);
}

/*----------------------------------------------------------------------*/

static void lcd_setfgcolor (int color)
{
	lcd_color_fg = color;
}

/*----------------------------------------------------------------------*/

static void lcd_setbgcolor (int color)
{
	lcd_color_bg = color;
}

/*----------------------------------------------------------------------*/

#ifdef	NOT_USED_SO_FAR
static int lcd_getfgcolor (void)
{
	return lcd_color_fg;
}
#endif	/* NOT_USED_SO_FAR */

/*----------------------------------------------------------------------*/

static int lcd_getbgcolor (void)
{
	return lcd_color_bg;
}

/*----------------------------------------------------------------------*/

/* Function:
 * bitmap_plot() --- Chipset depending Bitmap / Logo stuff...                         
 * Function initilizes Hardware palette with respect to Chipset depending Bitmap 
 * and draws logo from 'bmp_logo_data.h'. 
 * Parameter:
 * x -- x Coordinate on console
 * y -- y Coordinate on console
 */
#ifdef CONFIG_LCD_LOGO
void bitmap_plot (int x, int y)
{
#ifdef CONFIG_ATMEL_LCD
	uint *cmap;
#else
	ushort *cmap;
#endif
	ushort i, j;
	uchar *bmap;
	uchar *fb;
	ushort *fb16;
#if defined(CONFIG_CPU_PXA)
	struct pxafb_info *fbi = &panel_info->pxa;
#elif defined(CONFIG_MPC823)
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;
	volatile cpm8xx_t *cp = &(immr->im_cpm);
#endif

	debug ("Logo: width %d  height %d  colors %d  cmap %d\n",
		BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, BMP_LOGO_COLORS,
		(int)(sizeof(bmp_logo_palette)/(sizeof(ushort))));

	bmap = &bmp_logo_bitmap[0];
	fb   = (uchar *)(lcd_base + y * lcd_line_length + x);

#ifdef CONFIG_SHIKHARA && defined(CONFIG_PL111)
		if (panel_info1->vl_bpix < 12) 
#else
		if (NBITS(panel_info.vl_bpix) < 12) 
#endif
		{

		/* Leave room for default color map */
#if defined(CONFIG_CPU_PXA)
		cmap = (ushort *)fbi->palette;
#elif defined(CONFIG_MPC823)
		cmap = (ushort *)&(cp->lcd_cmap[BMP_LOGO_OFFSET*sizeof(ushort)]);
#elif defined(CONFIG_ATMEL_LCD)
		cmap = (uint *) (panel_info->mmio + ATMEL_LCDC_LUT(0));

#elif defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)                                                   
// cmap is now pointing to the hardware palette memory of CLCD 
  		struct clcd_config *config;                                      
                struct clcd_registers *regs;                                     
                                                                                  
                config = (struct  clcd_config *)(panel_info1->priv);                
                regs = config->address;                                           
                cmap = (ushort *)regs->palette; 
#else
		/*
		 * default case: generic system with no cmap (most likely 16bpp)
		 * We set cmap to the source palette, so no change is done.
		 * This avoids even more ifdef in the next stanza
		 */
		cmap = bmp_logo_palette;
#endif

		WATCHDOG_RESET();

		/* Set color map */
		for (i=0; i<(sizeof(bmp_logo_palette)/(sizeof(ushort))); ++i) {
			ushort colreg = bmp_logo_palette[i];
#ifdef CONFIG_ATMEL_LCD
			uint lut_entry;
#ifdef CONFIG_ATMEL_LCD_BGR555
			lut_entry = ((colreg & 0x000F) << 11) |
				    ((colreg & 0x00F0) <<  2) |
				    ((colreg & 0x0F00) >>  7);
#else /* CONFIG_ATMEL_LCD_RGB565 */
			lut_entry = ((colreg & 0x000F) << 1) |
				    ((colreg & 0x00F0) << 3) |
				    ((colreg & 0x0F00) << 4);
#endif
			*(cmap + BMP_LOGO_OFFSET) = lut_entry;
			cmap++;
#else /* !CONFIG_ATMEL_LCD */
#ifdef  CONFIG_SYS_INVERT_COLORS
			*cmap++ = 0xffff - colreg;
 			
#else		

/*BMP_LOGO_OFFSET is added to cmap to avoid chip dependent bitmap logo color entries 
 *overlapping with default color entries in palette. 	
 */
			*(cmap + BMP_LOGO_OFFSET) = colreg;                       
  			 cmap++; 
#endif
#endif /* CONFIG_ATMEL_LCD */
		}

		WATCHDOG_RESET();

		for (i=0; i<BMP_LOGO_HEIGHT; ++i) {
			memcpy (fb, bmap, BMP_LOGO_WIDTH);
			bmap += BMP_LOGO_WIDTH;
#ifdef CONFIG_SHIKHARA && defined(CONFIG_PL111)
			fb   += panel_info1->vl_col;
#else		
			fb   += panel_info.vl_col;
#endif
		}
	}
	else { /* true color mode */
		u16 col16;
		fb16 = (ushort *)(lcd_base + y * lcd_line_length + x);
		for (i=0; i<BMP_LOGO_HEIGHT; ++i) {
			for (j=0; j<BMP_LOGO_WIDTH; j++) {
				col16 = bmp_logo_palette[(bmap[j]-16)];
				fb16[j] =
					((col16 & 0x000F) << 1) |
					((col16 & 0x00F0) << 3) |
					((col16 & 0x0F00) << 4);
				}
			bmap += BMP_LOGO_WIDTH;
#ifdef CONFIG_SHIKHARA && defined(CONFIG_PL111)
			fb16 += panel_info1->vl_col;
#else
			fb16 += panel_info.vl_col;
#endif
		}
	}

	WATCHDOG_RESET();
}
#endif /* CONFIG_LCD_LOGO */

/*----------------------------------------------------------------------*/
#if defined(CONFIG_CMD_BMP) || defined(CONFIG_SPLASH_SCREEN)

#ifdef CONFIG_SPLASH_SCREEN_ALIGN
#define BMP_ALIGN_CENTER	0x7FFF
#endif

/* Function:
 * lcd_display_bitmap() ---- Show BMP Image 
 * Display the BMP file located at address bmp_image(Only uncompressed).
 * Parameter:
 * bmp_image -- RAM address where BMP Image stored 
 * x -- x Coordinate 
 * y -- y Coordinate
 * return: 0 (Success), 1 (Error)  
 */

int lcd_display_bitmap(ulong bmp_image, int x, int y)
{
#if !defined(CONFIG_MCC200)
	ushort *cmap = NULL;
#endif
	ushort *cmap_base = NULL;
	ushort i, j;
	bmp_image_t *bmp=(bmp_image_t *)bmp_image;
	uchar *bmap, *bmap2,*bmap3 ;
	ushort padded_line,padded_line2,padded_line3;
	unsigned long width, height,width2, height2,width3, height3, byte_width;
	int nbytes;

#ifdef CONFIG_SHIKHARA 

#ifdef CONFIG_PL111
	unsigned long pwidth = panel_info1->vl_col;
#endif
#ifdef CONFIG_DSI
	unsigned long pwidth2 = panel_info2->vl_col;
#endif
#ifdef CONFIG_HDMI
	unsigned long pwidth3 = panel_info3->vl_col;
#endif

#else	
	pwidth = panel_info.vl_col;
#endif /* CONFIG_SHIKHARA */
	
	unsigned colors, bpix,bpix2 ,bpix3, bmp_bpix;
#if defined(CONFIG_CPU_PXA)
	struct pxafb_info *fbi = &panel_info.pxa;
#elif defined(CONFIG_MPC823)
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;
	volatile cpm8xx_t *cp = &(immr->im_cpm);
#endif

	if (!((bmp->header.signature[0]=='B') &&
		(bmp->header.signature[1]=='M'))) {
		printf ("Error: no valid bmp image at %lx\n", bmp_image);
		return 1;
	}

         width = width2 = width3 = le32_to_cpu (bmp->header.width);
         height = height2 = height3 = le32_to_cpu (bmp->header.height);
	 bmp_bpix = le16_to_cpu(bmp->header.bit_count);
	 colors = 1 << bmp_bpix;

#ifdef CONFIG_SHIKHARA 
#ifdef CONFIG_PL111
	bpix = panel_info1->vl_bpix;
#endif
#ifdef CONFIG_DSI
	bpix2 = panel_info2->vl_bpix;
#endif
#ifdef CONFIG_HDMI
	bpix3 = panel_info3->vl_bpix;
#endif

#else	
	bpix = NBITS(panel_info.vl_bpix);
#endif /* CONFIG_SHIKHARA */

#ifdef CONFIG_PL111
	if ((bpix != 1) && (bpix != 8) && (bpix != 16) && (bpix != 24))  {
		printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
			bpix, bmp_bpix);
		return 1;
	}

	/* We support displaying 8bpp BMPs on 16bpp LCDs */
	if (bpix != bmp_bpix && (bmp_bpix != 8 || bmp_bpix != 16 || bmp_bpix != 24)) {
		printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
			bpix,
			le16_to_cpu(bmp->header.bit_count));
		return 1;
	}
#endif

#ifdef CONFIG_DSI
        if ((bpix2 != 1) && (bpix2 != 8) && (bpix2 != 16) && (bpix2 != 24))  {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix2, bmp_bpix);
                return 1;
        }

        /* We support displaying 8bpp BMPs on 16bpp LCDs */
        if (bpix2 != bmp_bpix && (bmp_bpix != 8 || bmp_bpix != 16 || bmp_bpix != 24)) {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix2,
                        le16_to_cpu(bmp->header.bit_count));
                return 1;
        }
#endif

#ifdef CONFIG_HDMI
	if ((bpix3 != 1) && (bpix3 != 8) && (bpix3 != 16) && (bpix3 != 24))  {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix3, bmp_bpix);
                return 1;
        }

        /* We support displaying 8bpp BMPs on 16bpp LCDs */
        if (bpix3 != bmp_bpix && (bmp_bpix != 8 || bmp_bpix != 16 || bmp_bpix != 24)) {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix3,
                        le16_to_cpu(bmp->header.bit_count));
                return 1;
        }
#endif
	
	debug ("Display-bmp: %d x %d  with %d colors\n",
		(int)width, (int)height, (int)colors);

#if !defined(CONFIG_MCC200)
	/* MCC200 LCD doesn't need CMAP, supports 1bpp b&w only */
	if (bmp_bpix == 8) {
#if defined(CONFIG_CPU_PXA)
		cmap = (ushort *)fbi->palette;
#elif defined(CONFIG_MPC823)
		cmap = (ushort *)&(cp->lcd_cmap[255*sizeof(ushort)]);
#elif !defined(CONFIG_ATMEL_LCD) && !defined(CONFIG_SHIKHARA)  
		cmap = panel_info->cmap;
#endif

		cmap_base = cmap;

		/* Set color map */
		for (i=0; i<colors; ++i) {
			bmp_color_table_entry_t cte = bmp->color_table[i];
#if !defined(CONFIG_ATMEL_LCD) && !defined(CONFIG_SHIKHARA)
			ushort colreg =
				( ((cte.red)   << 8) & 0xf800) |
				( ((cte.green) << 3) & 0x07e0) |
				( ((cte.blue)  >> 3) & 0x001f) ;
#ifdef CONFIG_SYS_INVERT_COLORS
			*cmap = 0xffff - colreg;
#else
			*cmap = colreg;
#endif
#if defined(CONFIG_MPC823)
			cmap--;
#else
			cmap++;
#endif
#elif defined(CONFIG_PL111)
			lcd_setcolreg(i, cte.red, cte.green, cte.blue);
#endif
		}
	}
#endif

	/*
	 *  BMP format for Monochrome assumes that the state of a
	 * pixel is described on a per Bit basis, not per Byte.
	 *  So, in case of Monochrome BMP we should align widths
	 * on a byte boundary and convert them from Bit to Byte
	 * units.
	 *  Probably, PXA250 and MPC823 process 1bpp BMP images in
	 * their own ways, so make the converting to be MCC200
	 * specific.
	 */
#if defined(CONFIG_MCC200)
	if (bpix==1)
	{
		width = ((width + 7) & ~7) >> 3;
		x     = ((x + 7) & ~7) >> 3;
		pwidth= ((pwidth + 7) & ~7) >> 3;
	}
#endif

#ifdef CONFIG_PL111
	if(bpix == 8){
                nbytes = 1;
        } else if(bpix == 16){
                nbytes = 2;
        } else if(bpix == 24){
                nbytes = 3;
        } else{
                printf("Error: %d bit/pixel mode", bpix);
                return 1;
        }

	/* padded line being calculated at byte level.(ex. 8bpp-1byte, 16bpp-2bpp, 24bpp-3byte)*/
	padded_line = ((width * nbytes) & 0x3) ? (((width * nbytes) & ~0x3)+4) : (width * nbytes);
#endif

#ifdef CONFIG_DSI
	if(bpix2 == 8){
                nbytes = 1;
        } else if(bpix2 == 16){
                nbytes = 2;
        } else if(bpix2 == 24){
                nbytes = 3;
        } else{
                printf("Error: %d bit/pixel mode", bpix2);
                return 1;
        }

        /* padded line being calculated at byte level.(ex. 8bpp-1byte, 16bpp-2bpp, 24bpp-3byte)*/
        padded_line2 = ((width2 * nbytes) & 0x3) ? (((width2 * nbytes) & ~0x3)+4) : (width2 * nbytes);
#endif

#ifdef CONFIG_HDMI
	if(bpix3 == 8){
                nbytes = 1;
        } else if(bpix3 == 16){
                nbytes = 2;
        } else if(bpix3 == 24){
                nbytes = 3;
        } else{
                printf("Error: %d bit/pixel mode", bpix3);
                return 1;
        }

        /* padded line being calculated at byte level.(ex. 8bpp-1byte, 16bpp-2bpp, 24bpp-3byte)*/
        padded_line3 = ((width3 * nbytes) & 0x3) ? (((width3 * nbytes) & ~0x3)+4) : (width3 * nbytes);
#endif
		
#ifdef CONFIG_SPLASH_SCREEN_ALIGN
	if (x == BMP_ALIGN_CENTER)
		x = max(0, (pwidth - width) / 2);
	else if (x < 0)
		x = max(0, pwidth - width + x + 1);

	if (y == BMP_ALIGN_CENTER)
		y = max(0, (panel_info1->vl_row - height) / 2);
	else if (y < 0)
		y = max(0, panel_info1->vl_row - height + y + 1);
#endif /* CONFIG_SPLASH_SCREEN_ALIGN */

#ifdef CONFIG_PL111
	if ((x + width)>pwidth)
                width = pwidth - x;
	if ((y + height)>panel_info1->vl_row)
		height = panel_info1->vl_row - y;

	/* CLCD Framebuffer calculation*/
	uchar *fb   = (uchar *) (lcd_base+(y + height - 1) * lcd_line_length + x * bpix / 8);
#endif
#ifdef CONFIG_DSI
	if ((x + width2)>pwidth2)
                width2 = pwidth2 - x;
	if ((y + height2)>panel_info2->vl_row)
                height2 = panel_info2->vl_row - y;

	/* MIPI-DSI Framebuffer calculation*/
	uchar *fb2   = (uchar *) (lcd_base2+(y + height2 - 1) * lcd_line_length2 + x * bpix2 / 8);
#endif
#ifdef CONFIG_HDMI
	if ((x + width3)>pwidth3)
                width3 = pwidth3 - x;
	if ((y + height3)>panel_info3->vl_row)
                height3 = panel_info3->vl_row - y;

	/*HDMI Framebuffer calculation*/
	uchar *fb3   = (uchar *) (lcd_base3+(y + height3 - 1) * lcd_line_length3 + x * bpix3 / 8);

#endif

// Setting up bmap where Pixel Data starts in BMP image file.
	bmap = bmap2 = bmap3 = (uchar *)bmp + le32_to_cpu (bmp->header.data_offset);

	switch (bmp_bpix) {
	case 1: /* pass through */
	case 8:
		if (bpix != 16)
			byte_width = width;
		else
			byte_width = width * 2;

		for (i = 0; i < height; ++i) {
			WATCHDOG_RESET();
			for (j = 0; j < width; j++) {
				if (bpix != 16) {
#if defined(CONFIG_CPU_PXA) || defined(CONFIG_ATMEL_LCD)  || defined(CONFIG_SHIKHARA)
#ifdef CONFIG_PL111
					
					*(fb++) = *(bmap++);
#endif
#if defined(CONFIG_MPC823) || defined(CONFIG_MCC200)
					*(fb++) = 255 - *(bmap++);
#endif
#endif
				} else {
#if defined(CONFIG_PL111)
					*(uint16_t *)fb = cmap_base[*(bmap++)];
					fb += sizeof(uint16_t) / sizeof(*fb);
#endif		
				}
			}
			bmap += (width - padded_line);
#ifdef CONFIG_PL111
			fb   -= (byte_width + lcd_line_length);
#endif
		}
		break;

 	case 16:
		for (i = 0; i < height; ++i) {
 			WATCHDOG_RESET();
 			for (j = 0; j < width; j++) {
#if defined(CONFIG_ATMEL_LCD_BGR555)
		
		*(fb++) = ((bmap[0] & 0x1f) << 2) |(bmap[1] & 0x03);
        	*(fb++) = (bmap[0] & 0xe0) | ((bmap[1] & 0x7c) >> 2);
	        bmap += 2;
#endif

#if !defined(CONFIG_ATMEL_LCD_BGR555) && defined(CONFIG_SHIKHARA) && defined(CONFIG_PL111)

		*(fb++) = *(bmap++);
        	*(fb++) = *(bmap++);
#endif
 	}
 	
	bmap += (padded_line - (width * 2));

#ifdef CONFIG_PL111
 	fb -= (width * 2 + lcd_line_length);
#endif
}
	break;

      case 24:
                for (i = 0; i < height; ++i) {

#ifdef CONFIG_PL111
                        for (j = 0; j < width; j++) {


/* BMP files has pixels value stored (default) as BGR, Little Endian format
 * and CLCD controller is also programmed to read pixel data in little endian format 
 * (shikhara_board.c - clcd_parameter_init()).
 */
                *(fb++) = bmap[0];
                *(fb++) = bmap[1];
                *(fb++) = bmap[2];
                fb++;  // For 24-bit it takes 4 bytes, 1 extra byte is empty.

                bmap += 3;
           }

#endif

#ifdef CONFIG_DSI
/*
Assumeing my SHikhara DSI controller Bridge is confiured as Little Endian, same as BGR of BMP file.
*/
				
		memcpy((void*)fb2, (void*)bmap2, lcd_line_length2);
                fb2+=lcd_line_length2;
                bmap2 +=lcd_line_length2;

#endif
#ifdef CONFIG_HDMI
/*
Assumeing my SHikhara HDMI controller Bridge is confiured as Little Endian, same as BGR of BMP file.
*/

		memcpy((void*)fb3, (void*)bmap3, lcd_line_length3);
                fb3+=lcd_line_length3;
                bmap3 +=lcd_line_length3;

#endif

#ifdef CONFIG_PL111             
                 bmap += (padded_line - (width * 3));
		 fb -= (width * 4 + lcd_line_length);
#endif

#ifdef CONFIG_DSI
		 bmap2 += (padded_line2 - (width2 * 3));
                 fb2 -= (width2 * 3 + lcd_line_length2);
#endif
#ifdef CONFIG_HDMI
		 bmap3 += (padded_line3 - (width3 * 3));
                 fb3 -= (width3 * 3 + lcd_line_length3);
#endif

        }
	break;

	default:
		break;
	};

	return (0);
}

#endif


/* Function:
 * lcd_logo() ---- Draw chip dependent logo 
 * Function draws logo and the board info to the CLCD as 'cls' command 
 * issued. This gets called also at the time of CLCD initilization.
 */

static void *lcd_logo (void)
{
#ifdef CONFIG_SPLASH_SCREEN
	char *s;
	ulong addr;
	static int do_splash = 1;

	if (do_splash && (s = getenv("splashimage")) != NULL) {
		int x = 0, y = 0;
		do_splash = 0;

		addr = simple_strtoul (s, NULL, 16);
#ifdef CONFIG_SPLASH_SCREEN_ALIGN
		if ((s = getenv ("splashpos")) != NULL) {
			if (s[0] == 'm')
				x = BMP_ALIGN_CENTER;
			else
				x = simple_strtol (s, NULL, 0);

			if ((s = strchr (s + 1, ',')) != NULL) {
				if (s[1] == 'm')
					y = BMP_ALIGN_CENTER;
				else
					y = simple_strtol (s + 1, NULL, 0);
			}
		}
#endif /* CONFIG_SPLASH_SCREEN_ALIGN */

#ifdef CONFIG_VIDEO_BMP_GZIP
		bmp_image_t *bmp = (bmp_image_t *)addr;
		unsigned long len;

		if (!((bmp->header.signature[0]=='B') &&
		      (bmp->header.signature[1]=='M'))) {
			addr = (ulong)gunzip_bmp(addr, &len);
		}
#endif

		if (lcd_display_bitmap (addr, x, y) == 0) {
			return ((void *)lcd_base);
		}
	}
#endif /* CONFIG_SPLASH_SCREEN */

#ifdef CONFIG_LCD_LOGO
	bitmap_plot (0, 0);
#endif /* CONFIG_LCD_LOGO */

#ifdef CONFIG_LCD_INFO
	console_col = LCD_INFO_X / VIDEO_FONT_WIDTH;
	console_row = LCD_INFO_Y / VIDEO_FONT_HEIGHT;
	lcd_show_board_info();
#endif /* CONFIG_LCD_INFO */

#if defined(CONFIG_LCD_LOGO) && !defined(CONFIG_LCD_INFO_BELOW_LOGO)
	return ((void *)((ulong)lcd_base + BMP_LOGO_HEIGHT * lcd_line_length));
#else
	return ((void *)lcd_base);
#endif /* CONFIG_LCD_LOGO && !CONFIG_LCD_INFO_BELOW_LOGO */
}


/* Function:
 * lcd_draw_pixel() ---- draw pixel
 * Parameter:
 * x --  x coordinate where pixel will be drawn
 * y --  y coordinate where pixel will be drawn  
 * color -- Color of the pixels 
 */

static void lcd_draw_pixel(int x,int y,unsigned int color)
{
	uchar *dest,c;
	uchar row;

	dest = (uchar *)(lcd_base + y * lcd_line_length + x * (panel_info1->vl_bpix) / 8);

	 for (row=0;  row < 4;  ++row, dest += lcd_line_length)
	  {
	      uchar *d = dest;
   	      for (c=0; c<4; ++c)
		{
		   *d++ =color;
		}
	    }
}
#ifdef CONFIG_SHIKHARA 
static void shikhara_lcd_draw_pixel(int x,int y,unsigned int color)
{
	uchar *dest;
#if LCD_BPP == LCD_COLOR24
#ifdef CONFIG_PL111
        dest = (uchar *)(lcd_base + y * lcd_line_length + x * (1 << LCD_BPP) / 8);
#endif
#ifdef CONFIG_DSI
        dest = (uchar *)(lcd_base2 + y * lcd_line_length2 + x * (panel_info2->vl_bpix) / 8);
#endif
#ifdef CONFIG_HDMI
        dest = (uchar *)(lcd_base3 + y * lcd_line_length3 + x * (panel_info3->vl_bpix) / 8);
#endif
	*dest++ =  color;
	color >>= 8;
	*dest++ =  color;
	color >>= 8;
	*dest++ =  color;
#ifdef CONFIG_PL111
	dest++ ;
#endif
#endif
#ifdef CONFIG_PL111 
	 dest = (uchar *)(lcd_base + y * lcd_line_length + x * (1 << LCD_BPP) / 8);
#if LCD_BPP == LCD_COLOR16
	*dest++ = color;
	color >>= 8;
        *dest++ = color;
#elif LCD_BPP == LCD_COLOR8
	*dest++ = color; 
#endif
#endif
}
#endif


/* Function:
 * lcd_draw_rectangle() ---- Draw Ractangle	                                               
 * Parameter:
 * x -- x coordinate of cursor
 * y -- y coordinate of cursor
 * width -- width of the rectangle 
 * height -- Height of the rectange
 * color -- color with which rectangle will be drawn 
 */
void lcd_draw_rectangle(int x,int y,int width,int height, unsigned int color)
{
    int i, j;

    for (j=0; j < height; j++) {

        for (i=0; i < width; i++) {
#ifdef CONFIG_SHIKHARA 
            shikhara_lcd_draw_pixel( x+i, y+j, color);
#else
	    lcd_draw_pixel( x+i, y+j, color);	
#endif
        }
    }
}

/*
 * Function: 
 * fill_screen() ---- Fill LCD with a color
 * Function fills CLCD with chosen color as 'lcd fill' command issued.   						
 * Parameter:
 * color ---- chosen color to fill CLCD (Ex. White, Blue, Red..) 
 */

void fill_screen(unsigned int color)
{
	int i, j;
	vidinfo_t *panel_info;

#ifdef CONFIG_PL111
#if (LCD_BPP == LCD_COLOR16 || LCD_BPP == LCD_COLOR24)
        panel_info = panel_info1;

#elif LCD_BPP == LCD_COLOR8
        memset ((char *)lcd_base,
                        COLOR_MASK(color),
                        lcd_line_length*panel_info1->vl_row);
	return 0;
#endif
#endif

#ifdef CONFIG_HDMI
        panel_info = panel_info3;
#endif

#ifdef CONFIG_DSI
        panel_info = panel_info2;
#endif	
	 for (j=0; j <panel_info->vl_row ; j++)
	 {
            for (i=0; i < panel_info->vl_col ; i++)
	  {

	#ifdef CONFIG_SHIKHARA 
            shikhara_lcd_draw_pixel(i, j, color);
	#endif
	   }
	}
}


/*
 * Function: show_flyer() ---- Fill LCD with a flyer image
 * Function reads the flyerPalette from Flyer.h (Flayer Image file) and initilize 
 * the Hardware color palette and fills CLCD with flayer image as 'show flyer'                                         
 * command issued.
 */
#ifdef CONFIG_CMD_FLYER
void show_flyer()
{
	unsigned int loop, xpos, ypos, scrpos;
	unsigned int * framebuffer;
		framebuffer =(unsigned int *) lcd_base;	
		struct clcd_config *config;
                struct clcd_registers *regs;

                config = (struct  clcd_config *)(panel_info1->priv);
                regs = config->address;
			

for (loop = 0; loop < (PALETTE_WORDS * 6); loop = loop + 6)
            regs->palette[loop / 6] = (flyerPalette[loop] >> 3) |
                                 ((flyerPalette[loop + 1] >> 3) <<  5) |
                                 ((flyerPalette[loop + 2] >> 3) << 10) |
                                 ((flyerPalette[loop + 3] >> 3) << 16) |
                                 ((flyerPalette[loop + 4] >> 3) << 21) |
                                 ((flyerPalette[loop + 5] >> 3) << 26);

  for (ypos = 0; ypos < panel_info1->vl_row; ypos++) {
        for (xpos = 0; xpos < panel_info1->vl_col; xpos = xpos + 4) {
              scrpos = (ypos * (panel_info1->vl_col / 4)) + (xpos / 4);
              loop   = (((ypos * panel_info1->vl_row) / panel_info1->vl_row) * panel_info1->vl_col) + ((xpos * panel_info1->vl_col) / panel_info1->vl_col);
              *(framebuffer + scrpos) = flyerData[loop] | (flyerData[loop + 1] << 8) |
                               (flyerData[loop + 2] << 16) | (flyerData[loop + 3] << 24);
                		}
    		}
}

#endif

