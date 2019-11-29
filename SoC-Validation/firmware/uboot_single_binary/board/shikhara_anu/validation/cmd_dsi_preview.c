#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/shikhara_mipi_csi.h>
#include <asm/arch/shikhara_mipi_csi_bridge.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_mipi_dsi_bridge.h>
#include <malloc.h>

int conversion_dsi(int frame_no )
{
        char *dest;
	int width=800;
	int height=1280;
        dest=(char*)(CONFIG_MIPI_DSI_FB2_ADDR);
        char *source_addr;
        int frame_size=0xe1000,i,j;

        for(i=0;i<5;i++){
        	dest=(char*)(CONFIG_MIPI_DSI_FB2_ADDR);
		source_addr=(char *)(DDR_CSI_ADDR_VC1 + ((i)*FRAME_STORAGE_ADDRESS_OFFSET));
		for(j=0;j<480;j++)
		{
                	memcpy(dest,source_addr,(640*3));
			dest+=2400;
			source_addr+=1920;
		}
        }
}

void do_preview_on_dsi()
{


        OV5640_stream_on();
        printf("Going to see the preview on DSI\n");
	mipi_lcd_color_pixel(800,1280,0xffffffff);
        while(1)
        {
                while(readl(CSI2_BRIDGE_FRAME_INTNUM_VC1) != (readl(CSI2_FRAME_COUNT_READ_VC1)));
                conversion_dsi(30);
#if 1
                if (ctrlc ())
                {
                printf("Specified frames are received\n");
                OV5640_stream_off();
                return 0;
                }
#endif
        }
        printf("Specified frames are received\n");
        OV5640_stream_off();

        return;

}
U_BOOT_CMD(
        preview_on_dsi, 2, 1, do_preview_on_dsi,
        "streaming on DSI from camera",
        "No parameter needed"
        );

