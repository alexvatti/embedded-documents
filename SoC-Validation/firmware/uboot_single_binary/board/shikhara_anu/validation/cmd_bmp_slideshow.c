/*
 * (C) Copyright 2002
 * Detlev Zundel, DENX Software Engineering, dzu@denx.de.
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
 * BMP handling routines
 */

#include <common.h>
#include <lcd.h>
#include <bmp_layout.h>
#include <command.h>
#include <asm/byteorder.h>
#include<asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_hdmi_bridge.h>

#define DEBUG_SLIDESHOW
#ifdef DEBUG_SLIDESHOW
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
extern void *hdmi_console_address;

#define START_SLIDE_SHOW_FB 	0x6400000 // 100 MB from DDR base
#define START_SLIDE_SHOW_BMP 	0x8C00000 // 140 MB from DDR base

#define SIZEOF_ONE_BMP 0x600000 // 6 MB image size for 1920X 1080, 24 bit
#define BMP_SLIDESHOW_1 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_BMP + (0*SIZEOF_ONE_BMP))
#define BMP_SLIDESHOW_2 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_BMP + (1*SIZEOF_ONE_BMP))
#define BMP_SLIDESHOW_3 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_BMP + (2*SIZEOF_ONE_BMP))
#define BMP_SLIDESHOW_4 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_BMP + (3*SIZEOF_ONE_BMP))
#define BMP_SLIDESHOW_5 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_BMP + (4*SIZEOF_ONE_BMP))

#define FB_SLIDESHOW_1 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_FB + (0*SIZEOF_ONE_BMP))
#define FB_SLIDESHOW_2 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_FB + (1*SIZEOF_ONE_BMP))
#define FB_SLIDESHOW_3 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_FB + (2*SIZEOF_ONE_BMP))
#define FB_SLIDESHOW_4 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_FB + (3*SIZEOF_ONE_BMP))
#define FB_SLIDESHOW_5 (CONFIG_SYS_SDRAM_BASE + START_SLIDE_SHOW_FB + (4*SIZEOF_ONE_BMP))

/* Function:
 * lcd_display_bitmap_slideshow() ---- Show BMP Image 
 * Display the BMP file located at address bmp_image(Only uncompressed).
 * Parameter:
 * bmp_image -- RAM address where BMP Image stored 
 * x -- x Coordinate 
 * y -- y Coordinate
 * return: 0 (Success), 1 (Error)  
 *Note : This function is defined like that only one display interface (between the three HDMI, DSI, PL111) is enable at a time.
 *	      If more then one is enable then the behavior of the function is undefined.
 */

int lcd_display_bitmap_slideshow(ulong bmp_image, uchar * fb_base_current)
{

	ushort i;
	int x=0;
	int y =0;
	bmp_image_t *bmp=(bmp_image_t *)bmp_image;
	uchar *bmap;
	ushort padded_line;
	unsigned long width, height ;
	int nbytes;
	int lcd_count=0,bmap_count=0;


#ifdef CONFIG_PL111
	unsigned long pwidth1 = panel_info1->vl_col;
#endif
#ifdef CONFIG_DSI
	unsigned long pwidth2 = panel_info2->vl_col;
#endif
#ifdef CONFIG_HDMI
	unsigned long pwidth3 = panel_info3->vl_col;
#endif


	unsigned  bpix, bmp_bpix;


	if (!((bmp->header.signature[0]=='B') &&
		(bmp->header.signature[1]=='M'))) {
		printf ("Error: no valid bmp image at %lx\n", bmp_image);
		return 1;
	}

	width = le32_to_cpu (bmp->header.width);
	height = le32_to_cpu (bmp->header.height);
	bmp_bpix = le16_to_cpu(bmp->header.bit_count);


#ifdef CONFIG_PL111
	bpix = panel_info1->vl_bpix;
#endif
#ifdef CONFIG_DSI
	bpix = panel_info2->vl_bpix;
#endif
#ifdef CONFIG_HDMI
	bpix = panel_info3->vl_bpix;
#endif

	
	/* We support displaying 24bpp BMPs only */
	if (bpix != bmp_bpix && ( bpix != 24)) {
		printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
			bpix,
			le16_to_cpu(bmp->header.bit_count));
		return 1;
	}

	 if(bpix == 24){
		nbytes = 3;
	} else{ 
		printf("Error: %d bit/pixel mode", bpix);
		return 1;
	}
	/* padded line being calculated at byte level.(ex. 8bpp-1byte, 16bpp-2bpp, 24bpp-3byte)*/
	padded_line = ((width * nbytes) & 0x3) ? (((width * nbytes) & ~0x3)+4) : (width * nbytes);


#ifdef CONFIG_PL111
	if ((x + width)>pwidth1)
                width = pwidth1 - x;
	if ((y + height)>panel_info1->vl_row)
                height = panel_info1->vl_row - y;

	/*LCD Framebuffer calculation*/
	uchar *fb1   = (uchar *) (fb_base_current+(y + height - 1) * lcd_line_length + x * bpix / 8);
#endif
#ifdef CONFIG_DSI
	if ((x + width)>pwidth2)
                width = pwidth2 - x;
	if ((y + height)>panel_info2->vl_row)
                height = panel_info2->vl_row - y;

	/* MIPI-DSI Framebuffer calculation*/
	uchar *fb2   = (uchar *) (fb_base_current+(y + height - 1) * lcd_line_length2 + x * bpix / 8);
#endif
#ifdef CONFIG_HDMI
	if ((x + width)>pwidth3)
                width = pwidth3 - x;
	if ((y + height)>panel_info3->vl_row)
                height = panel_info3->vl_row - y;

	/*HDMI Framebuffer calculation*/
	uchar *fb3   = (uchar *) (fb_base_current+(y + height - 1) * lcd_line_length3 + x * bpix / 8);

#endif

// Setting up bmap where Pixel Data starts in BMP image file.
	bmap = (uchar *)bmp + le32_to_cpu (bmp->header.data_offset);

	switch (bmp_bpix) {
      case 24:
                for (i = 0; i < height; ++i) {
#ifdef CONFIG_PL111
		for(lcd_count=0;lcd_count<lcd_line_length;lcd_count++)
		{
			fb1[lcd_count++] = bmap[bmap_count++];
			fb1[lcd_count++] = bmap[bmap_count++];
			fb1[lcd_count++] = bmap[bmap_count++];
			lcd_count++; //
		}
		fb1 -= (width * 4 + lcd_line_length);
#endif

#ifdef CONFIG_HDMI
/*
Assumeing my SHikhara HDMI controller Bridge is confiured as Little Endian, same as BGR of BMP file.
*/
          	memcpy(fb3, bmap, lcd_line_length3);
	   	fb3+=lcd_line_length3;
		bmap +=lcd_line_length3;
		bmap += (padded_line - (width * 3));
              fb3 -= (width * 3 + lcd_line_length3);
#endif

#ifdef CONFIG_DSI
/*
Assumeing my SHikhara DSI controller Bridge is confiured as Little Endian, same as BGR of BMP file.
*/
          	memcpy(fb2, bmap, lcd_line_length2);
	   	fb2+=lcd_line_length2;
		bmap +=lcd_line_length2;
		bmap += (padded_line - (width * 3));
              fb2 -= (width * 3 + lcd_line_length2);
#endif



        }
	break;

	default:
		printf("Error: %d bit/pixel mode", bmp_bpix);
		break;
	};

	return (0);
}

/*
*Change the HDMI/DVI frame buffer on the fly.
*Note : This function is defined like that only one display interface (between the three HDMI, DSI, PL111) is enable at a time.
*	If more then one is enable then the behavior of the function is undefined.
*/
int  flip_to_new_fb (unsigned int lcdbase)
{

#ifdef CONFIG_PL111
#ifdef CONFIG_SHIKHARA
	shikhara_pl111_lcd_init(lcdbase);
#else
	lcd_ctrl_init(lcdbase);
#endif //CONFIG_SHIKHARA
#endif //CONFIG_PL111

#ifdef CONFIG_HDMI
/* lcdbase (DDR_HDMI_VIDEO_ADDR) is configured as a framebuffer3 for HDMI Bridge */
	 writel (lcdbase , HDMI_BB_DMA_BASE_ADDR_REGISTER_1);  
#ifdef CONFIG_HDMI_CONSOLE
	hdmi_console_address = lcdbase;
#endif
#endif
	return 0;
}

/*
 * Subroutine:  do_bmp_slideshow
 *
 * Description: runs the slideshow with 5 images
 *
 * Inputs:	None
 *
 * Return:      None
 *
 *Note : This function is defined like that only one display interface (between the three HDMI, DSI, PL111) is enable at a time.
 *	      If more then one is enable then the behavior of the function is undefined.
 */
static int do_bmp_slideshow(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int count =20;
	unsigned int intial_count =20;
	unsigned int delay =500000;

	
        static int onetime;

	if(!(argc == 1 || argc == 2 || argc == 3))
	{
		printf("Error in command\n");
		printf("Use : bmpslideshow <time delay b/n two slide(in ms)> <no of loop>\n");
		return 0;
	}

	if(argc == 2 || argc == 3)
	{
		delay = simple_strtoul(argv[1], NULL, 10);
		delay *= 1000;
	}
	if(argc == 3)
	{
		count = simple_strtoul(argv[2], NULL, 10);
		intial_count = count;
	}
	printf("Delay value is %d ms\n",delay); 
	if(count == (unsigned int)-1)
		printf("Loop will run infinite\n");
	else
		printf("loop will run %d times\n",count);

	 printf("inside bmp_slideshow\n");
         if(!onetime)
	 {

		lcd_display_bitmap_slideshow((ulong)BMP_SLIDESHOW_1, (uchar *) FB_SLIDESHOW_1);
		dbg("Converted first file from BMP to BGR\n");
		lcd_display_bitmap_slideshow((ulong)BMP_SLIDESHOW_2, (uchar *) FB_SLIDESHOW_2);
		dbg("Converted second file from BMP to BGR\n");
		lcd_display_bitmap_slideshow((ulong)BMP_SLIDESHOW_3, (uchar *) FB_SLIDESHOW_3);
		dbg("Converted third file from BMP to BGR\n");
		lcd_display_bitmap_slideshow((ulong)BMP_SLIDESHOW_4, (uchar *) FB_SLIDESHOW_4);
		dbg("Converted fourth file from BMP to BGR\n");
		lcd_display_bitmap_slideshow((ulong)BMP_SLIDESHOW_5, (uchar *) FB_SLIDESHOW_5);
		dbg("Converted fifth file from BMP to BGR\n");
		onetime++;
        }
	printf("Now starting the slide show..\n");
	while((count == (unsigned int)-1) || count--)
	{
			dbg("Showing first BMP file in slideshow, ..\n");   
			dbg("Count is %d of %x, BMP_SLIDESHOW_1 = %x , FB_SLIDESHOW_1= %x\n", count, intial_count, BMP_SLIDESHOW_1,FB_SLIDESHOW_1 );   
			flip_to_new_fb((unsigned int)FB_SLIDESHOW_1); 
		       udelay(delay);


			dbg("Showing second BMP file in slideshow, ..\n");   
			dbg("Count is %d of %x, BMP_SLIDESHOW_2 = %x , FB_SLIDESHOW_2= %x\n", count, intial_count, BMP_SLIDESHOW_2,FB_SLIDESHOW_2 );   
			flip_to_new_fb((unsigned int)FB_SLIDESHOW_2); 
		       udelay(delay);

			dbg("Showing third BMP file in slideshow, ..\n");   
			dbg("Count is %d of %x, BMP_SLIDESHOW_3 = %x , FB_SLIDESHOW_3= %x\n", count, intial_count, BMP_SLIDESHOW_3,FB_SLIDESHOW_3 );   
			flip_to_new_fb((unsigned int)FB_SLIDESHOW_3); 
		       udelay(delay);
			   
			dbg("Showing fourth BMP file in slideshow, ..\n");   
			dbg("Count is %d of %x, BMP_SLIDESHOW_4 = %x , FB_SLIDESHOW_4= %x\n", count, intial_count, BMP_SLIDESHOW_4,FB_SLIDESHOW_4 );   
			flip_to_new_fb((unsigned int)FB_SLIDESHOW_4); 
		       udelay(delay);

			dbg("Showing fifth BMP file in slideshow, ..\n");   
			dbg("Count is %d of %x, BMP_SLIDESHOW_5 = %x , FB_SLIDESHOW_5= %x\n", count, intial_count, BMP_SLIDESHOW_5,FB_SLIDESHOW_5 );   
			flip_to_new_fb((unsigned int)FB_SLIDESHOW_5); 
		       udelay(delay);
			if (ctrlc ())
			{
                        	printf("Playback Interrupted\n");
                        	return 0;
                	}
	
	}
		#ifdef CONFIG_HDMI
		flip_to_new_fb((unsigned int)lcd_base3);
		#endif
		#ifdef CONFIG_PL111
		flip_to_new_fb((unsigned int)lcd_base);
		#endif
		#ifdef CONFIG_DSI
		flip_to_new_fb((unsigned int)lcd_base2);
		#endif
	       dbg("Done with the slideshow, try again ..\n");

		return 0;
}
static int do_bmp_slideshow_help(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
       printf("First use MMC commands to copy 5 BMP files in below addresses. Then run bmpslideshow command\n");
	printf("Use : bmpslideshow <time delay b/n two slide(in ms)> <no of loop>\n");
	printf("Use : If no of loop is -1 it will run as infinite loop(use ctrl+c for break. default time delay and number of loop is 500ms and 20\n");

	printf("BMP_SLIDESHOW_1 = %x , FB_SLIDESHOW_1= %x\n",  BMP_SLIDESHOW_1,FB_SLIDESHOW_1);   
       printf("BMP_SLIDESHOW_2 = %x , FB_SLIDESHOW_2= %x\n",  BMP_SLIDESHOW_2,FB_SLIDESHOW_2);   
   	printf("BMP_SLIDESHOW_3 = %x , FB_SLIDESHOW_3= %x\n",  BMP_SLIDESHOW_3,FB_SLIDESHOW_3);   
	printf("BMP_SLIDESHOW_4 = %x , FB_SLIDESHOW_4= %x\n",  BMP_SLIDESHOW_4,FB_SLIDESHOW_4);   
	printf("BMP_SLIDESHOW_5 = %x , FB_SLIDESHOW_5= %x\n",  BMP_SLIDESHOW_5,FB_SLIDESHOW_5);   

}

U_BOOT_CMD(
	bmpslideshow,	3,	1,	do_bmp_slideshow,
	"runs the slideshow with 5 images",
	"bmpslideshow - show five images in rotation"	
);
U_BOOT_CMD(
	bmpslideshow_help,	1,	1,	do_bmp_slideshow_help,
	"runs the slideshow with 5 images",
	"bmpslideshow - show five images in rotation"	
);



