#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <i2c.h>
#include <asm/arch/shikhara_i2c.h>
#include <asm/types.h>
#include <asm/arch/shikhara_mipi_csi.h>
#include <asm/arch/shikhara_mipi_csi_bridge.h>
#include <asm/arch/shikhara_map.h>

int conversion(int frame_no )
{
        char *dest;

        dest=(char*)(CONFIG_HDMI_FB3_ADDR);
        char *source_addr;
        int frame_size=0xe1000,i;
        
	for(i=0;i<5;i++){
                source_addr=(char *)(DDR_CSI_ADDR_VC1 + ((i)*FRAME_STORAGE_ADDRESS_OFFSET));
                memcpy(dest,source_addr,(640*3*480));
        }
}

void do_preview_on_hdmi()
{


	OV5640_stream_on();
	printf("Going to see the preview on HDMI\n");
        while(1)
        {
                while(readl(CSI2_BRIDGE_FRAME_INTNUM_VC1) != (readl(CSI2_FRAME_COUNT_READ_VC1)));
                conversion(30);
#if 1
                if (ctrlc ())
                {
                printf("Specified frames are received\n");
                OV5640_stream_off();
                return 0;
                }
#endif
        }

#if 0 //DMA Test

        //Reset DMAC
        writel(0xC,SHIKHARA_SYS_CTRL_DMAC_CONFIG_RESET);
        mdelay(10);

        /* Change DMA Manager to NS state */
        writel(0x400,SHIKHARA_SYS_CTRL_DMAC_CONFIG_REG);
        udelay(10);
        /*Release reset for DMAC*/
        writel(0x0,SHIKHARA_SYS_CTRL_DMAC_CONFIG_RESET);

        while(1)
        {
                while(readl(CSI2_BRIDGE_FRAME_INTNUM_VC1) != (readl(CSI2_FRAME_COUNT_READ_VC1)));
                for(i=5; i< NUMBER_OF_BUFFER_DESCRIPTORS; i++)
                {
                dmatransfer_csi_hdmi (i);
                }
#if 0
                if (ctrlc ())
                {
                printf("Specified frames are received\n");
                OV5640_stream_off();
                return 0;
                }
#endif
        }
#endif

        printf("Specified frames are received\n");
        OV5640_stream_off();
        
        return;

}

U_BOOT_CMD(
        preview_on_hdmi, 2, 1, do_preview_on_hdmi,
        "streaming on HDMI from camera",
        "No parameter needed"
        );

