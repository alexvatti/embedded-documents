#include <common.h>
#include <lcd.h>
#include <bmp_layout.h>
#include <command.h>
#include <asm/byteorder.h>
#include<asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_hdmi_bridge.h>


#define BITPERPIX               24
#define FRAME_ADDRESS  0x1200000

#define FRAME_BUFFER_ADDRESS (CONFIG_SYS_SDRAM_BASE + FRAME_ADDRESS)



static int clamp(int a)
{
	char i;
	i=(a>255) ? 255 : (a<0) ? 0 : a;
	return i;
}

/*
convert_yuvtorbg funtion convert yuv image to rgb image.
Note : This function is defined like that only one display interface (between the three HDMI, DSI, PL111) is enable at a time.
		If more then one is enable then the behavior of the function is undefined.
*/
void convert_yuvtorbg(unsigned char *fb,unsigned char *df,int width,int hight,int bpp,int type)
{
        int i,k;
        unsigned char r,g,b;
        int hxw,count=1,dc=0;
        unsigned char *ybuf,*ubuf,*vbuf;

        hxw = width*hight;

        printf("Image width=%d and Image hight=%d\n",width,hight);
        printf("Source address=0x%x and Destination address=0x%x\n",fb,df); 

        ybuf = fb;
        ubuf = fb + hxw;

	 #ifdef CONFIG_PL111
        dc = dc + (hxw-width)*4;
	 #endif

	 #ifdef CONFIG_HDMI
	 dc = dc + (hxw-width)*3;
	 #endif

	 #ifdef CONFIG_DSI
	 dc = dc + (hxw-width)*3;
	 #endif

        if(type == 1)
        {
                vbuf = ubuf + hxw/4;
                printf("type : YUV420\n");
		printf("Start writing Image data\n");	

                i=0;
                k=0;
                while(i<hxw)
                {
                        for(count=0;count<width;count+=2,i+=2,k++)
                        {
                                r=clamp(ybuf[i]+ (1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif

                                r=clamp(ybuf[i+1]+(1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i+1]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i+1]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif
                        }
						
				#ifdef CONFIG_PL111
				dc = dc - 8*width;
				#endif

				#ifdef CONFIG_HDMI
                          	dc = dc - 6*width;
				#endif

				#ifdef CONFIG_DSI
				dc = dc - 6*width;
				#endif
				
                        k=k-width/2;
                        for(count=0;count<width;count+=2,i+=2,k++)
                        {
                                r=clamp(ybuf[i]+ (1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif

                                r=clamp(ybuf[i+1]+(1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i+1]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i+1]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif
                        }

				#ifdef CONFIG_PL111
				dc = dc - 8*width;
				#endif

				#ifdef CONFIG_HDMI
                          	dc = dc - 6*width;
				#endif

				#ifdef CONFIG_DSI
				dc = dc - 6*width;
				#endif
                }

        }
        else if(type == 2)
        {
                vbuf = ubuf + hxw/2;

                printf("type : YUV422\n");
		printf("Start writing Image data\n");	
                i=0;
                k=0;

                while(i<hxw)
                {
                        for(count=0;count<width;i+=2,k++,count+=2)
                        {
                                r=clamp(ybuf[i]+ (1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif

                                r=clamp(ybuf[i+1]+(1.402f*(ubuf[k]-128)));
                                g=clamp(ybuf[i+1]-(0.344f*(vbuf[k]-128)-0.714f*(ubuf[k]-128)));
                                b=clamp(ybuf[i+1]+(1.772f*(vbuf[k]-128)));

                                df[dc++]=r;
                                df[dc++]=g;
                                df[dc++]=b;
					#ifdef CONFIG_PL111
					//If PL111 is enable then we have to escape every 4th byte.
						dc++;
					#endif
                        }

				#ifdef CONFIG_HDMI
                          dc = dc - 6*width;
				#endif

				#ifdef CONFIG_DSI
				dc = dc - 6*width;
				#endif

				#ifdef CONFIG_PL111
				dc = dc - 8*width;
				#endif
                }
        }
        else
        {
                printf("Invalid type\n");
		return;
        }

	printf("Writing Image data done\n");
}

/*
rgbtobmp function add the bmp header on rgb image.
*/
static void rgbtobmp(unsigned char *fb,unsigned char *df,int width,int hight,int bpp)
{
	bmp_header_t *head = df;

	printf("Start adding Image header\n");
	head->signature[0] = 'B';
	head->signature[1] = 'M';
	head->file_size = width * hight * (bpp/8) + 54;
	head->reserved = 0;

	head->size = 40;
	head->width = width;
	head->height = hight;
	head->planes = 1;
	head->bit_count = bpp;
	head->compression = 0;

	head->x_pixels_per_m = 3780;
	head->y_pixels_per_m = 3780;
        if(bpp == 24)
        {
		printf("24 bpp mode.\n");
		head->data_offset = 54;
		head->image_size = 0;
                head->colors_used = 0;
		head->colors_important = 0;
        }
        else
        {
                printf("Conversion not supports\n");
                return;
        }
	printf("Adding Image header done (Image Res is %dx%d)\n",width,hight);
}

#ifdef CONFIG_HDMI //If HDMI is not enabled then yuv2bmp command is not required.
static int do_yuv2bmp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char *saddr;
	unsigned char *daddr;

        if(argc != 4)
        {
                printf("error in command\n");
                printf("use : yuv2bmp <source address> <destination address> <type(1 for YUV420 or 2 for YUV422)>\n");
                return 1;
        }

	saddr = simple_strtoul(argv[1],NULL,16);
	daddr = simple_strtoul(argv[2],NULL,16);
	if(saddr == 0)
	{
		saddr = FRAME_BUFFER_ADDRESS;
		printf("Source address given 0, Taking FRAME_BUFFER_ADDRESS 0x%x as Source address\n",FRAME_BUFFER_ADDRESS);
	}
	if(daddr == 0)
	{
		printf("Destination address should not 0\n");
		return 1;
	}

	printf("Source address = 0x%x, Destination address = 0x%x\n",saddr,daddr);
	convert_yuvtorbg(saddr,daddr+54,panel_info3->vl_col,panel_info3->vl_row,BITPERPIX,simple_strtoul(argv[3],NULL,10));
	rgbtobmp(saddr,daddr,panel_info3->vl_col,panel_info3->vl_row,BITPERPIX);
	printf("Done\n");
	return 0;
}


U_BOOT_CMD(
        yuv2bmp,   4,      1,      do_yuv2bmp,
        "yuv2bmp - convert yuv image to bmp image.",
        "For help see yuv2bmp_help"
);

static int do_yuv2bmp_help(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        printf("use : yuv2bmp <source address> <destination address> <type(1 for YUV420 or 2 for YUV422)>\n");
        printf("After using the command yuv2bmp the YUV Image file avalable on source address will be \
		convert into BMP and will be save at the given destination address\n");
        printf("If source address is given 0 from command line then by default it will take 0x%x address\
		as source address\n",FRAME_BUFFER_ADDRESS);
	return 0;
}

U_BOOT_CMD(
        yuv2bmp_help,      1,      1,      do_yuv2bmp_help,
        "yuv2bmp_help - help for yuv2bmp",
        "convert yuv image to bmp image."
);
#endif

/*
do_rgb2bmp funtion convert rgb image to bmp image.
Note : This function is defined like that only one display interface (between the three HDMI, DSI, PL111) is enable at a time.
		If more then one is enable then the behavior of the function is undefined.
*/
static int do_rgb2bmp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        unsigned char *saddr;
        unsigned char *daddr;
        int i,j,count;
        int hxw,dc=0,hight,width;

        if(argc != 3)
        {
                printf("error in command\n");
                printf("use : rgb2bmp <source address> <destination address>\n");
                return 1;
        }

        saddr = simple_strtoul(argv[1],NULL,16);
        daddr = simple_strtoul(argv[2],NULL,16);
        if(saddr == 0)
        {
                saddr = FRAME_BUFFER_ADDRESS;
		printf("Source address given 0, Taking FRAME_BUFFER_ADDRESS 0x%x as Source address\n",FRAME_BUFFER_ADDRESS);
        }
	if(daddr == 0)
	{
		printf("Destination address should not 0\n");
		return 1;
	}
	printf("Source address = 0x%x, Destination address = 0x%x\n",saddr,daddr);
#ifdef CONFIG_DSI
        width = panel_info2->vl_col;
        hight = panel_info2->vl_row;
#endif
#ifdef CONFIG_PL111
        width = panel_info1->vl_col;
        hight = panel_info1->vl_row;
#endif
#ifdef CONFIG_HDMI
	width = panel_info3->vl_col;
	hight = panel_info3->vl_row;
#endif
        hxw = width*hight;
        printf("Image width=%d and Image hight=%d\n",width,hight);

	//copy the image data from source address to destination address in revers format
	daddr += 54;
	
#ifdef CONFIG_DSI
	dc = dc + (hxw-width)*3;

	for(i=0;i<hight;i++)
	{
		memcpy(&daddr[dc],&saddr[i*width*3],width*3);
		dc = dc - 3*width;
	}
#endif
#ifdef CONFIG_PL111
	//This modification in PL111 is because 24bpp image in PL111 contain 4 byte RGBA.
	dc = dc + (hxw-width)*4;
	count = hight;
	i=0;
	while(count--)
	{
		for(j=0;j<width*4;)
		{
			daddr[dc++] = saddr[i++];
			daddr[dc++] = saddr[i++];
			daddr[dc++] = saddr[i++];
			i++;
		}
		dc = dc - 8*width;
	}
#endif
#ifdef CONFIG_HDMI
dc = dc + (hxw-width)*3;

for(i=0;i<hight;i++)
	{
		memcpy(&daddr[dc],&saddr[i*width*3],width*3);
		dc = dc - 3*width;
	}
#endif
	printf("Copy the data from source address to destination address is done\n");
	daddr = simple_strtoul(argv[2],NULL,16);
	rgbtobmp(saddr,daddr,width,hight,BITPERPIX);
	printf("Done\n");
	return 0;
}

U_BOOT_CMD(
        rgb2bmp,   3,      1,      do_rgb2bmp,
        "rgb2bmp - convert rgb image to bmp image.",
        "For help see rgb2bmp_help"
);

static int do_rgb2bmp_help(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        printf("use : rgb2bmp <source address> <destination address>\n");
        printf("After using the command rgb2bmp the rgb Image file avalable on source address will be \
                convert into BMP and will be save at the given destination address\n");
        printf("If source address is given 0 from command line then by default it will take 0x%x address\
                as source address\n",FRAME_BUFFER_ADDRESS);
        printf("It may also helpfull for screenshot.\n");
        return 0;
}

U_BOOT_CMD(
        rgb2bmp_help,   1,      1,      do_rgb2bmp_help,
        "rgb2bmp_help - help for rgb2bmp.",
        "convert rgb image to bmp image."
);

