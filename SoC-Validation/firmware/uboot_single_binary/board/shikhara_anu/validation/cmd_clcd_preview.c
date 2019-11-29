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
#define BMP_FB_ADDRESS  0x12f00000
#define OV5640_WIDTH    640
#define OV5640_HEIGHT   480


void do_preview_on_lcd()
{
        int i=20;
        OV5640_stream_on();
       while(1)
        {
                while(readl(CSI2_BRIDGE_FRAME_INTNUM_VC1) != (readl(CSI2_FRAME_COUNT_READ_VC1)));
                preview_on_clcd();
#if 1
                if (ctrlc ())
                {
                printf("Specified frames are received\n");
                OV5640_stream_off();
                return 0;
                }
#endif
        }
}


int preview_on_clcd()
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
        }
        return 0;
}
U_BOOT_CMD(preview_on_clcd, 2, 1, do_preview_on_lcd,
        "streaming on CLCD from camera",
        "No parameter needed"
        );

