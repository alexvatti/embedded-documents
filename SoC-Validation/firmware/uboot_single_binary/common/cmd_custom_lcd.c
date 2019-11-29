#include <common.h>
#include <asm/arch/nand.h>
#include <netdev.h>
#include <sound.h>
#include <configs/anusoc.h>
#include <lcd.h>
#include <amba_clcd.h>
#include <asm/arch/shikhara_smc.h>
#include <asm/arch/shikhara_wdt.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_dwmmc.h>
#include <asm/arch/shikhara_can.h>
#include <asm/arch/shikhara_i2c.h>

#define LCDREG(x)	        	  	(u32)((x)+ SHIKHARA_CLCD_BASE)
#define LCDTIM0              			LCDREG(0x00000000)
#define LCDTIM1             			LCDREG(0x00000004)
#define LCDTIM2              			LCDREG(0x00000008)
#define LCDTIM3         	     		LCDREG(0x0000000C)
#define LCDUPBASE               		LCDREG(0x00000010)
#define LCDLPBASE               		LCDREG(0x00000014)
#define LCDCNTL		              		LCDREG(0x00000018)
#define LCDIMSC                 		LCDREG(0x0000001C)
#define LCDRIS                  		LCDREG(0x00000020)
#define LCDMIS                  		LCDREG(0x00000024)
#define LCDICR                  		LCDREG(0x00000028)
#define LCDUPCURR               		LCDREG(0x0000002C)
#define LCDLPCURR               		LCDREG(0x00000030)
#define LCDPALETTE              		LCDREG(0x00000200)

# define CUSTOM_CONSOLE_COLOR_BLACK    0x00000000
# define CUSTOM_CONSOLE_COLOR_WHITE    0xffffff00/* Must remain last / highest   */
# define CUSTOM_CONSOLE_COLOR_RED      0xff000000
# define CUSTOM_CONSOLE_COLOR_GREEN    0x00ff0000
# define CUSTOM_CONSOLE_COLOR_YELLOW   0xffff0000
# define CUSTOM_CONSOLE_COLOR_BLUE     0x0000ff00
# define CUSTOM_CONSOLE_COLOR_MAGENTA  0xff00ff00
# define CUSTOM_CONSOLE_COLOR_CYAN     0x00FFff00

vidinfo_t custom_panel_info1[] = {
/* Video parameters calculated for 800x480, @60Hz, Pixel Clock=26.4MHz (Ref. epanorama.net, Innolux TFT panel datasheet) */
#if 0
{
        .vl_col       =       800,
        .vl_row       =       480,
        .vl_hbp       =       46,
        .vl_hfp       =       16,
        .vl_vbp       =       23,
        .vl_vfp       =       7,
        .vl_hsync_len =       1,
        .vl_vsync_len =       1,
        .vl_bpix      =       24,
        .endianess    =       0,
        .pixclock     =       26400000
        }
#endif
{
        .vl_col       =       800,
        .vl_row       =       480,
        .vl_hbp       =       46-1,
        .vl_hfp       =       210, //354,
        .vl_vbp       =       23-1,
        .vl_vfp       =       22, //147,
        .vl_hsync_len =       40-1,
        .vl_vsync_len =       20-1,
        .vl_bpix      =       24,
        .endianess    =       0,
        .pixclock     =       33300000
}

};

int clcd_poweron()
{
        int bpp=5; //for 24 bit
	unsigned long cntl=0;
	cntl |= (CNTL_LCDPWR)| (CNTL_LCDEN) | (bpp<<1) | (CNTL_LCDTFT) | CNTL_LCDVCOMP(0) | ((custom_panel_info1[0].endianess)<<9) | (CNTL_WATERMARK);
	writel(cntl,LCDCNTL);
	printf("DEBUG_LCD: lcd parameter cntl is %x\r\n", readl(LCDCNTL));
}

int clcd_power_off()
{
    //    int bpp=5; //for 24 bit
        unsigned int cntl=0;
        //cntl &= ~((CNTL_LCDPWR)| (CNTL_LCDEN) | (bpp<<1) | (CNTL_LCDTFT) | CNTL_LCDVCOMP(0) | ((custom_panel_info1[0].endianess)<<9) | (CNTL_WATERMARK));
        cntl &= ~(CNTL_LCDPWR);
        writel(cntl,LCDCNTL);
        display_power_off();
        printf("Lcd Powered off \r\n");
}


int clcd_init()
{
	struct clcd_config pl111_lcd_priv;
	int bpp=5; //for 24 bit
	unsigned long value, pcd_lo, pcd_hi;


	pl111_lcd_priv.tim0 = pl111_lcd_priv.tim1 = pl111_lcd_priv.tim2 =  pl111_lcd_priv.tim3= pl111_lcd_priv.cntl=0;

	pl111_lcd_priv.tim0 |= ((((custom_panel_info1[0].vl_col/16)-1)<<2) | (custom_panel_info1[0].vl_hsync_len<<8) | (custom_panel_info1[0].vl_hfp<<16) | (custom_panel_info1[0].vl_hbp<<24));

        pl111_lcd_priv.tim1 |= ((custom_panel_info1[0].vl_row-1) | (custom_panel_info1[0].vl_vsync_len<<10) | (custom_panel_info1[0].vl_vfp<<16) | (custom_panel_info1[0].vl_vbp<<24));

	value=((CLCDCLK/(custom_panel_info1[0].pixclock))-2);
        pcd_lo=value & 0x1F;
        pcd_hi=(value>>5) & 0x1F;

        pl111_lcd_priv.tim2 |= (pcd_hi<<27) |((custom_panel_info1->vl_col-1)<<16) | pcd_lo;

	pl111_lcd_priv.tim3 |= (CLCD_LED) | (CLCD_LEE<<16);

	writel(CONFIG_CLCD_FB1_ADDR,LCDUPBASE);

	pl111_lcd_priv.cntl |= (CNTL_LCDEN) | (bpp<<1) | (CNTL_LCDTFT) | CNTL_LCDVCOMP(0) | ((custom_panel_info1[0].endianess)<<9) | (CNTL_WATERMARK);
	
	writel(pl111_lcd_priv.tim0,LCDTIM0);
	writel(pl111_lcd_priv.tim1,LCDTIM1);
	writel(pl111_lcd_priv.tim2,LCDTIM2);
	writel(pl111_lcd_priv.tim3,LCDTIM3);
	writel(pl111_lcd_priv.cntl,LCDCNTL);

    

    printf("DEBUG_LCD: clcd base address is %x\r\n", SHIKHARA_CLCD_BASE);
    printf("DEBUG_LCD: lcd parameter tim0 is %x\r\n", readl(LCDTIM0));
    printf("DEBUG_LCD: lcd parameter tim1 is %x\r\n", readl(LCDTIM1));
    printf("DEBUG_LCD: lcd parameter tim2 is %x\r\n", readl(LCDTIM2));
    printf("DEBUG_LCD: lcd parameter tim3 is %x\r\n", readl(LCDTIM3));
    printf("DEBUG_LCD: lcd parameter cntl is %x\r\n", readl(LCDCNTL));
    printf("DEBUG_LCD: lcd parameter UPBASE is %x\r\n", readl(LCDUPBASE));

}
int do_lcd_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	printf("LCD Resolutin: width=%d height=%d\n",custom_panel_info1[0].vl_col,custom_panel_info1[0].vl_row); 

        printf("Address: %x\t Value of tim0 REG: 0x%x\n",LCDTIM0,readl(LCDTIM0));
        printf("Address: %x\t Value of tim1 REG: 0x%x\n",LCDTIM1,readl(LCDTIM1));
        printf("Address: %x\t Value of tim2 REG: 0x%x\n",LCDTIM2,readl(LCDTIM2));
        printf("Address: %x\t Value of tim3 REG: 0x%x\n",LCDTIM3,readl(LCDTIM3));
        printf("Address: %x\t Value of ubas REG: 0x%x\n",LCDUPBASE,readl(LCDUPBASE));
        printf("Address: %x\t Value of lbas REG: 0x%x\n",LCDLPBASE,readl(LCDLPBASE));
        printf("Address: %x\t Value of cntl REG: 0x%x\n",LCDCNTL,readl(LCDCNTL));
        printf("Address: %x\t Value of stat REG: 0x%x\n",LCDRIS,readl(LCDRIS));
        printf("Address: %x\t Value of intc REG: 0x%x\n",LCDICR,readl(LCDICR));
        printf("Address: %x\t Value of ucur REG: 0x%x\n",LCDUPCURR,readl(LCDUPCURR));
        printf("Address: %x\t Value of lcur REG: 0x%x\n",LCDLPCURR,readl(LCDLPCURR));
        printf("Address: %x\t Value of palette REG: 0x%x\n",LCDPALETTE,readl(LCDPALETTE));

}

U_BOOT_CMD(
        lcd_dump,     1,      1,      do_lcd_dump,
        "To dump the LCD(PL-111) register set",
        " "
        " "
);


void lcd_color_pixel(unsigned int width,unsigned int height,unsigned long color)
{
    char *dest = readl(LCDUPBASE);
    int i=0,j=0;
	for( i = 0;  i<height; i++){
	for( j = 0;  j<width; j++){

        	*dest =  ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest =  ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest =  ((color>>8) & 0xff);
        	dest++;

		*dest = (color & 0x00);
		dest++;
	}
	//dest = (readl(LCDUPBASE) + (width*4*i));
   }
}

void lcd_color_bar(unsigned int width,unsigned int height)
{
    char *dest = readl(LCDUPBASE);
    unsigned long color;

    int i=0,j=0;
	for( i = 0;  i<height; i++){
	for( j = 0;  j<width; j++){
		if(j>=0 & j < (width/4)) {
		color =CUSTOM_CONSOLE_COLOR_RED;
        	*dest =  ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest =  ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest =  ((color>>8) & 0xff);
        	dest++;

		*dest = (color & 0x00);
		dest++;
		}
		if(j>=(width/4) & j < (width*2/4)) {
		color =CUSTOM_CONSOLE_COLOR_GREEN;
        	*dest =  ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest =  ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest =  ((color>>8) & 0xff);
        	dest++;

		*dest = (color & 0x00);
		dest++;
		}
		if(j>=(width*2/4) & j < (width*3/4)) {
		color =CUSTOM_CONSOLE_COLOR_BLUE;
        	*dest =  ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest =  ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest =  ((color>>8) & 0xff);
        	dest++;

		*dest = (color & 0x00);
		dest++;
		}
		if(j>=(width*3/4) & j < (width)) {
		color =CUSTOM_CONSOLE_COLOR_YELLOW;
        	*dest =  ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest =  ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest =  ((color>>8) & 0xff);
        	dest++;

		*dest = (color & 0x00);
		dest++;
		}
	}
   }
}
int do_lcd_enable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int i;

    printf("Going for PL111(CLCD) Panel configuration...\n");
    for(i=1;i<100;i++)
    	udelay(25000);
    display_power_on();
    clcd_init();
    udelay(25000);
    clcd_poweron();
}

U_BOOT_CMD(
        lcd_enable,     1,      1,      do_lcd_enable,
        "To init LCD(PL-111) Display Panel",
        " "
);

int do_lcd_color_pattern(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned int i;
    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_GREEN); //Green
    printf("Going to Display/Show Green Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_BLUE); //blue
    printf("Going to Display/Show Blue Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_YELLOW);
    printf("Going to Display/Show Yellow Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_MAGENTA);
    printf("Going to Display/Show Magenta Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_CYAN);
    printf("Going to Display/Show Cyan Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_WHITE);
    printf("Going to Display/Show White Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_pixel(800,480,CUSTOM_CONSOLE_COLOR_BLACK);
    printf("Going to Display/Show Block Color\n");

    for(i=1;i<100;i++)
    	udelay(25000);
    lcd_color_bar(800,480);
    printf("Going to Display/Show Color: Red,Green,Blue,Yellow\n");

    for(i=1;i<100;i++)
    	udelay(25000);
	
    printf("lcd color bar test has done\n");

}

int lcd_display_bitmap_custom(ulong bmp_image, int x, int y)
{       
        ushort *cmap_base = NULL;
        ushort i, j;
        bmp_image_t *bmp=(bmp_image_t *)bmp_image;
        uchar *bmap, *bmap2,*bmap3 ;
        ushort padded_line,padded_line2,padded_line3;
        unsigned long width, height,width2, height2,width3, height3, byte_width;
        int nbytes;
	unsigned colors, bpix,bpix2 ,bpix3, bmp_bpix;
	unsigned long pwidth = custom_panel_info1[0].vl_col;
	unsigned long pheight = custom_panel_info1[0].vl_row;

	printf("Display-Panel: w=%d x h=%d  \n",(int)pwidth, (int)pheight);

	if (!((bmp->header.signature[0]=='B') &&
                (bmp->header.signature[1]=='M'))) {
                printf ("Error: no valid bmp image at %lx\n", bmp_image);
                return 1;
        }

         width = width2 = width3 = le32_to_cpu (bmp->header.width);
         height = height2 = height3 = le32_to_cpu (bmp->header.height);
         bmp_bpix = le16_to_cpu(bmp->header.bit_count);
         colors = 1 << bmp_bpix;

	 bpix = custom_panel_info1[0].vl_bpix;

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

	printf("Display-bmp: w=%d x h=%d  with %d colors\n",(int)width, (int)height, (int)colors);

	 lcd_line_length = (custom_panel_info1[0].vl_col * NBITS (custom_panel_info1[0].vl_bpix)) / 8;
	
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

        if ((x + width)>pwidth)
                width = pwidth - x;
        if ((y + height)>custom_panel_info1[0].vl_row)
                height = custom_panel_info1[0].vl_row - y;

	printf("Display-bmp: w=%d x h=%d  with %d colors\n",(int)width, (int)height, (int)colors);
        /* CLCD Framebuffer calculation*/
 	char *fb = readl(LCDUPBASE);

	bmap = bmap2 = bmap3 = (uchar *)bmp + le32_to_cpu (bmp->header.data_offset);

	for (i = 0; i < height; ++i) {
             for (j = 0; j < width; j++) {


/* BMP files has pixels value stored (default) as BGR, Little Endian format
 * and CLCD controller is also programmed to read pixel data in little endian format 
 * (shikhara_board.c - clcd_parameter_init()).
 */
                *(fb++) = bmap[2];//R
                *(fb++) = bmap[1];//G
                *(fb++) = bmap[0];//B
                fb++;  // For 24-bit it takes 4 bytes, 1 extra byte is empty.

                bmap += 3;
           }
	   //unused space
	   for(j=width;j<pwidth;j++) {

                *(fb++) = 0x00;
                *(fb++) = 0x00;
                *(fb++) = 0x00;
                fb++;  // For 24-bit it takes 4 bytes, 1 extra byte is empty.
	   }	
	}
}

int do_lcd_disable(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
   clcd_power_off();
}

U_BOOT_CMD(
        lcd_color_pattern,     3,      1,      do_lcd_color_pattern,
        "To print color pattern on LCD Panel ",
        " "
);

U_BOOT_CMD(
        lcd_disable,     1,      1,      do_lcd_disable,
        "To power of the lcd ",
        " "
);

