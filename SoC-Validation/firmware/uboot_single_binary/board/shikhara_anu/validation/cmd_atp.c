#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/shikhara_mipi_csi.h>
#include <asm/arch/shikhara_mipi_csi_bridge.h>
#include <asm/arch/shikhara_map.h>
#include <malloc.h>
#include <spi.h>
#include <asm/arch/shikhara_tc.h>
#define BMP_FB_ADDRESS 	0x12f00000
#define OV5640_WIDTH 	640
#define OV5640_HEIGHT 	480
#define TOUCH_SPI_BUS   1
#define TOUCH_SPI_CS    0
#define TOUCH_SPI_HZ    3250000

//extern int start_xfer(struct spi_slave *, const u8 );
typedef struct bmp_header {
        /* Header */
        char signature[2];
        __u32   file_size;
        __u32   reserved;
        __u32   data_offset;
        /* InfoHeader */
        __u32   size;
        __u32   width;
        __u32   height;
        __u16   planes;
        __u16   bit_count;
        __u32   compression;
        __u32   image_size;
        __u32   x_pixels_per_m;
        __u32   y_pixels_per_m;
        __u32   colors_used;
        __u32   colors_important;
        /* ColorTable */

} __attribute__ ((packed)) bmp_header_t;



int preview_on_clcd_atp(struct spi_slave *spi)
{
 	int i,j,k,l, width, height,Pix_size=3;
        char *dest;

        dest=(char*)(CONFIG_CLCD_FB1_ADDR);
        char *source_addr;
        width = OV5640_WIDTH;
        height = OV5640_HEIGHT;
        char b,g,r;

        for(i=0; i<5; i++)
        {
                source_addr=(char *)(DDR_CSI_ADDR_VC1 + ((i)*FRAME_STORAGE_ADDRESS_OFFSET));
                dest=(char*)(CONFIG_CLCD_FB1_ADDR);
                for(j=0;j<height;j++)
                {

                        for(k=0;k<width;k++)
                        {
                        	b=*source_addr; source_addr++;
                         	g=*source_addr; source_addr++;
                         	r=*source_addr; source_addr++;

                         	*dest=r;
                          	dest++;
                         	*dest=g;
                          	dest++;
                         	*dest=b;
                          	dest++;
                          	dest++;
                        }
                        for(l=640;l<800;l++) { //lcd excess width
                                *dest=0x00;
                                dest++;
                                *dest=0x00;
                                dest++;
                                *dest=0x00;
                                dest++;
                                *dest=0x00;
                                dest++;
                        }
                }
		if(my_touch_read(spi) == 1)
		return 1;
        }
	return 0;
}

static int do_fat_fswrite(unsigned long addr,unsigned long count)
{
        long size;
        block_dev_desc_t *dev_desc = NULL;
        int dev = 0;
        int part = 1;
        char *ep;

        size = file_fat_write("camera_snap.bmp", (void *)addr, count);
        if (size == -1) {
                printf("\n** Unable to write ..\n");
                return 1;
        }

        printf("camera snap size:%ld bytes written to sd\n", size);

        return 0;
}

void do_capture_frame_save()
{
        char *bmp_data,*bmp_Addr;
        bmp_header_t *head = (char *)BMP_FB_ADDRESS;
        bmp_Addr=(char *)BMP_FB_ADDRESS;
        int width=OV5640_WIDTH;
        int hight=OV5640_HEIGHT;
        int bpp=24;

        printf("Adding BMP Image header\n");
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
                printf("Pixel format is 24 bpp mode.\n");
                head->data_offset = 54;
                head->image_size = 0;
                head->colors_used = 0;
                head->colors_important = 0;
        	bmp_data=(char *)(BMP_FB_ADDRESS + head->data_offset);
        	memcpy(bmp_data,(uchar *)(DDR_CSI_ADDR_VC1+(3*FRAME_STORAGE_ADDRESS_OFFSET)),(head->file_size-54));
        	do_fat_fswrite(bmp_Addr,head->file_size);
        }
        else
        {
                printf("Pixel format & conversion do not supports\n");
                printf("Failed to save the captured data\n");
                return;
        }
        printf("Camera snap is successfully stored in SD(With Res is %dx%d)\n",width,hight);
}

int my_touch_read( struct spi_slave *spi)
{
        unsigned int  x, y, z1, z2;
        unsigned int  Rt=0,val=0;
	unsigned int vref =1 ;

       // get_position(&x,&y,spi); //non blocking
    
        	
        	val= (readl(SHIKHARA_SYS_CTRL_BASEADDR+0x214) & 0x40);
        	if(!val) {
               x = start_xfer(spi,READ_X(vref));       /* Read X Position */
               y = start_xfer(spi,READ_Y(vref));       /* Read Y Position */
                /* Power Down mode */
                start_xfer(spi,PWRDOWN);
                udelay(20000);
		start_xfer(spi,PWRDOWN);
		printf("X:=%4X\t Y:=%4X\t Rt=%X \n\n",x,y,Rt);
                //break;
		return 1;
                }
    return 0;
}

int do_atp_function()
{
	struct spi_slave *spi;
	//step 1: spi touch initialization
	spi = touch_init(TOUCH_SPI_BUS, TOUCH_SPI_CS,TOUCH_SPI_HZ,SPI_MODE_0);
        if(!spi){
                printf("Problem in intialistaion of touch screen \n");
                return 0;
        }
        printf("SPI Initialised..\n");

	//step 2: print message to screen , to touch the panel for preview
	printf("Please touch the screeen to start preview\n");

        while(1) {

		if(my_touch_read(spi) == 1)
		{
		  //step 3: stream on: preview
		  OV5640_stream_on();
		  break;	 
		}
	}
        printf("Please wait to see the preview on CLCD...... touch the screeen to save frame\n");
	while(1){
		//step 4: preview on clcd
		
		if(preview_on_clcd_atp(spi)==1)
		//if(my_touch_read(spi) == 1)
		{
			 //step 5: save to sd
                  	 while(readl(CSI2_BRIDGE_FRAME_INTNUM_VC1) != (readl(CSI2_FRAME_COUNT_READ_VC1)));       
			 do_capture_frame_save();
			 printf("Save to save frame\n");
			 break; 
		}
	}
        OV5640_stream_off();
	spi_release_bus(spi);
	printf("Camera preview is closed\n");
        return 0;
}




U_BOOT_CMD(capture_frame, 2, 1, do_capture_frame_save,
        "Capture the camera frame & save to SD Card",
        "No parameter needed"
        );

U_BOOT_CMD(ATP_Test, 2, 1, do_atp_function,
        "ATP test for streaming on CLCD from camera and saving the capture image on SD",
        "No parameter needed"
        );

