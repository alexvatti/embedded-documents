#include <common.h>
#include <command.h>
#include<asm/io.h>
#include <asm/arch/shikhara_mipi_dsi_bridge.h>
#include <asm/arch/shikhara_mipi_dsi_local.h>
#include <asm/arch/shikhara_mipi_dsi_hal.h>

#include <asm/arch/shikhara_mipi_csi.h>
#include <asm/arch/shikhara_mipi_csi_bridge.h>

#define DEBUG_CSI_DSI
#ifdef DEBUG_CSI_DSI
	#define dbg(x...) printf(x)
#else
	#define dbg(x...) do {}while(0)
#endif




void do_csi_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dbg("=========CSI Register dump==========\n");
	//FOR CSI2_CONTROLLER
	printf("CSI2_VERSION: Address at %x and value %X\n",CSI2_VERSION,readl(CSI2_VERSION));
	printf("CSI2_N_LANES: Address at %x and value %X\n",CSI2_N_LANES,readl(CSI2_N_LANES));
	printf("CSI2_PHY_SHUTDOWNZ: Address at %x and value %X\n",CSI2_PHY_SHUTDOWNZ,readl(CSI2_PHY_SHUTDOWNZ));
	printf("CSI2_DPHY_RSTZ: Address at %x and value %X\n",CSI2_DPHY_RSTZ,readl(CSI2_DPHY_RSTZ));
	printf("CSI2_RESETN: Address at %x and value %X\n",CSI2_RESETN,readl(CSI2_RESETN));
	printf("CSI2_PHY_STATE: Address at %x and value %X\n",CSI2_PHY_STATE,readl(CSI2_PHY_STATE));
	printf("CSI2_DATA_IDS_1: Address at %x and value %X\n",CSI2_DATA_IDS_1,readl(CSI2_DATA_IDS_1));
	printf("CSI2_DATA_IDS_2: Address at %x and value %X\n",CSI2_DATA_IDS_2,readl(CSI2_DATA_IDS_2));
	printf("CSI2_ERR1: Address at %x and value %X\n",CSI2_ERR1,readl(CSI2_ERR1));
	printf("CSI2_ERR2: Address at %x and value %X\n",CSI2_ERR2,readl(CSI2_ERR2));
	printf("CSI2_MSK1: Address at %x and value %X\n",CSI2_MSK1,readl(CSI2_MSK1));
	printf("CSI2_MSK2: Address at %x and value %X\n",CSI2_MSK2,readl(CSI2_MSK2));
	printf("CSI2_PHY_TEST_CTRL0: Address at %x and value %X\n",CSI2_PHY_TEST_CTRL0,readl(CSI2_PHY_TEST_CTRL0));
	printf("CSI2_PHY_TEST_CTRL1: Address at %x and value %X\n",CSI2_PHY_TEST_CTRL1,readl(CSI2_PHY_TEST_CTRL1));

		
}

void do_csi_bridge_dump(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	dbg("=========CSI BRIDGE Register dump==========\n");

	// CSI_BB common Registers:
	printf("CSI Dest FB: Address at %x and value %X\n",CSI2_BB_BASE_ADDR,readl(CSI2_BB_BASE_ADDR));
	printf("CSI frame staus register  at %x and value %X\n",(CSI2_BB_BASE_ADDR+4),readl(CSI2_BB_BASE_ADDR+4));
	printf("AXI_Error_FIFO_Mask: Address at %x and value %X\n",AXI_Error_FIFO_Mask,readl(AXI_Error_FIFO_Mask)); 
	printf("AXI_Error_FIFO_Status: Address at %x and value %X\n",AXI_Error_FIFO_Status,readl(AXI_Error_FIFO_Status));
	printf("Current_AXI_Write_Addr: Address at %x and value %X\n",Current_AXI_Write_Addr,readl(Current_AXI_Write_Addr));
	printf("Packet_Header_Info: Address at %x and value %X\n",Packet_Header_Info,readl(Packet_Header_Info));
	printf("CSI_DSI_Direct_Config: Address at %x and value %X\n",CSI_DSI_Direct_Config,readl(CSI_DSI_Direct_Config));

	// For Virtual Channel_1
	printf("FRAME_FORMAT_CON_VC1: Address at %x and value %X\n", CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC1,readl(CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC1));
	printf("FRAME_INT_VC1: Address at %x and value %X\n", CSI2_BRIDGE_FRAME_INTNUM_VC1,readl(CSI2_BRIDGE_FRAME_INTNUM_VC1));
	printf("HORZ_RES_VC1: Address at %x and value %X\n",CSI2_BRIDGE_HORIZONTAL_RES_VC1,readl(CSI2_BRIDGE_HORIZONTAL_RES_VC1));
	printf("VERTICAL_RES_VC1: Address at %x and value %X\n",CSI2_BRIDGE_VERTICAL_RES_VC1,readl(CSI2_BRIDGE_VERTICAL_RES_VC1));
	printf("FRAM_LINE_INT_MASK_VC1: Address at %x and value %X\n",CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC1,readl(CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC1));
	printf("FRAM_LINE_INT_STAT_VC1: Address at %x and value %X\n",CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC1,readl(CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC1));
	printf("FRAME_COUNT_READ_VC1: Address at %x and value %X\n",CSI2_FRAME_COUNT_READ_VC1,readl(CSI2_FRAME_COUNT_READ_VC1));


		
}
#if 0
#ifdef CONFIG_DSI_TRULY
void do_dsi_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int val;
	dbg("=========DSI Register dump==========\n");
	//FOR DSI_CONTROLLER
	printf("DSI_VERSION: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_VERSION),readl(DSI_BASE_ADDR+DSI_VERSION));
	printf("DSI_PWR_UP: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PWR_UP),readl(DSI_BASE_ADDR+DSI_PWR_UP));
	printf("DSI_CLKMGR_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_CLKMGR_CFG),readl(DSI_BASE_ADDR+DSI_CLKMGR_CFG));
	printf("DSI_DPI_VCID: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DPI_VCID),readl(DSI_BASE_ADDR+DSI_DPI_VCID));
	printf("DSI_DPI_COLOR_CODING: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DPI_COLOR_CODING),readl(DSI_BASE_ADDR+DSI_DPI_COLOR_CODING));
	printf("DSI_DPI_CFG_POL: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DPI_CFG_POL),readl(DSI_BASE_ADDR+DSI_DPI_CFG_POL));
        val=readl(DSI_BASE_ADDR+DSI_DPI_CFG_POL);
	
	if(DSI_DPI_CFG_POL_DATAEN_ACT_LOW & val)
		printf("Data Enable Polarity ACTIVE LOW and value is 1\n");
	else
		printf("Data Enable Polarity ACTIVE HIGH and value is 0\n");
        
	if(DSI_DPI_CFG_POL_VSYNC_ACT_LOW & val)
        	printf("VSYNC Polarity ACTIVE LOW and value is 1\n");
        else
        	printf("VSYNC Polarity ACTIVE HIGH and value is 0\n");
        
	if(DSI_DPI_CFG_POL_HSYNC_ACT_LOW & val)
        	printf("HSYNC Polarity ACTIVE LOW and value is 1\n");
        else
        	printf("HSYNC Polarity ACTIVE HIGH and value is 0\n");

	printf("DSI_DPI_LP_CMD_TIM: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DPI_LP_CMD_TIM),readl(DSI_BASE_ADDR+DSI_DPI_LP_CMD_TIM));
	//printf("DSI_DBI_VCID: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DBI_VCID),readl(DSI_BASE_ADDR+DSI_DBI_VCID));
	//printf("DSI_DBI_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DBI_CFG),readl(DSI_BASE_ADDR+DSI_DBI_CFG));
	//printf("DSI_DBI_PART_EN: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DBI_PART_EN),readl(DSI_BASE_ADDR+DSI_DBI_PART_EN));
	//printf("DSI_DBI_CMDSIZE: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_DBI_CMDSIZE),readl(DSI_BASE_ADDR+DSI_DBI_CMDSIZE));
	printf("DSI_PKTHDLR_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PKTHDLR_CFG),readl(DSI_BASE_ADDR+DSI_PKTHDLR_CFG));
	printf("DSI_GEN_VCID: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_GEN_VCID),readl(DSI_BASE_ADDR+DSI_GEN_VCID));
	printf("DSI_MODE_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_MODE_CFG),readl(DSI_BASE_ADDR+DSI_MODE_CFG));
	printf("DSI_VID_MODE_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_VID_MODE_CFG),readl(DSI_BASE_ADDR+DSI_VID_MODE_CFG));
	printf("DSI_VID_PKT_SIZE: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_PKT_SIZE),readl(DSI_BASE_ADDR+DSI_VID_PKT_SIZE),readl(DSI_BASE_ADDR+DSI_VID_PKT_SIZE));
	printf("DSI_VID_NUM_CHUNKS: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_VID_NUM_CHUNKS),readl(DSI_BASE_ADDR+DSI_VID_NUM_CHUNKS));
	printf("DSI_VID_NULL_SIZE: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_VID_NULL_SIZE),readl(DSI_BASE_ADDR+DSI_VID_NULL_SIZE));
	printf("DSI_VID_HSA_TIME: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_HSA_TIME),readl(DSI_BASE_ADDR+DSI_VID_HSA_TIME),readl(DSI_BASE_ADDR+DSI_VID_HSA_TIME));
	printf("DSI_VID_HBP_TIME: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_HBP_TIME),readl(DSI_BASE_ADDR+DSI_VID_HBP_TIME),readl(DSI_BASE_ADDR+DSI_VID_HBP_TIME));
	printf("DSI_VID_HLINE_TIME: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_HLINE_TIME),readl(DSI_BASE_ADDR+DSI_VID_HLINE_TIME),readl(DSI_BASE_ADDR+DSI_VID_HLINE_TIME));
	printf("DSI_VID_VSA_LINES: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_VSA_LINES),readl(DSI_BASE_ADDR+DSI_VID_VSA_LINES),readl(DSI_BASE_ADDR+DSI_VID_VSA_LINES));
	printf("DSI_VID_VBP_LINES: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_VBP_LINES),readl(DSI_BASE_ADDR+DSI_VID_VBP_LINES),readl(DSI_BASE_ADDR+DSI_VID_VBP_LINES));
	printf("DSI_VID_VFP_LINES: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_VFP_LINES),readl(DSI_BASE_ADDR+DSI_VID_VFP_LINES),readl(DSI_BASE_ADDR+DSI_VID_VFP_LINES));
	printf("DSI_VID_VACTIVE_LINES: Address at %x and value hex:%X decimal:%d\n", (DSI_BASE_ADDR+DSI_VID_VACTIVE_LINES),readl(DSI_BASE_ADDR+DSI_VID_VACTIVE_LINES),readl(DSI_BASE_ADDR+DSI_VID_VACTIVE_LINES        ));
	//printf("DSI_EDPI_CMD_SIZE: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_EDPI_CMD_SIZE),readl(DSI_BASE_ADDR+DSI_EDPI_CMD_SIZE));
	printf("DSI_CMD_MODE_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_CMD_MODE_CFG),readl(DSI_BASE_ADDR+DSI_CMD_MODE_CFG));
	//printf("DSI_GEN_HDR: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_GEN_HDR),readl(DSI_BASE_ADDR+DSI_GEN_HDR));
	//printf("DSI_GEN_PLD_DATA: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_GEN_PLD_DATA),readl(DSI_BASE_ADDR+DSI_GEN_PLD_DATA));
	//printf("DSI_CMD_PKT_STATUS: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_CMD_PKT_STATUS),readl(DSI_BASE_ADDR+DSI_CMD_PKT_STATUS));
	//printf("DSI_TO_CNT_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_TO_CNT_CFG),readl(DSI_BASE_ADDR+DSI_TO_CNT_CFG));
	//printf("DSI_HS_RD_TO_CNT: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_HS_RD_TO_CNT),readl(DSI_BASE_ADDR+DSI_HS_RD_TO_CNT));
	//printf("DSI_LP_RD_TO_CNT : Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_LP_RD_TO_CNT ),readl(DSI_BASE_ADDR+DSI_LP_RD_TO_CNT ));
	//printf("DSI_HS_WR_TO_CNT: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_HS_WR_TO_CNT),readl(DSI_BASE_ADDR+DSI_HS_WR_TO_CNT));
	//printf("DSI_LP_WR_TO_CNT: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_LP_WR_TO_CNT),readl(DSI_BASE_ADDR+DSI_LP_WR_TO_CNT));
	//printf("DSI_BTA_TO_CNT: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_BTA_TO_CNT),readl(DSI_BASE_ADDR+DSI_BTA_TO_CNT));
	//printf("DSI_SDF_3D: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_SDF_3D),readl(DSI_BASE_ADDR+DSI_SDF_3D));
	printf("DSI_LPCLK_CNTRL: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_LPCLK_CNTRL),readl(DSI_BASE_ADDR+DSI_LPCLK_CNTRL));
	printf("DSI_PHY_TMR_LPCLK_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_TMR_LPCLK_CFG),readl(DSI_BASE_ADDR+DSI_PHY_TMR_LPCLK_CFG));
	printf("DSI_PHY_TMR_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_TMR_CFG),readl(DSI_BASE_ADDR+DSI_PHY_TMR_CFG));
	printf("DSI_PHY_RSTZ: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_RSTZ),readl(DSI_BASE_ADDR+DSI_PHY_RSTZ));
	printf("DSI_PHY_IF_CFG: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_IF_CFG),readl(DSI_BASE_ADDR+DSI_PHY_IF_CFG));
	printf("DSI_PHY_ULPS_CTRL: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_ULPS_CTRL),readl(DSI_BASE_ADDR+DSI_PHY_ULPS_CTRL));
	printf("DSI_PHY_TX_TRIGGERS: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_TX_TRIGGERS),readl(DSI_BASE_ADDR+DSI_PHY_TX_TRIGGERS));
	printf("DSI_PHY_STATUS: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_STATUS),readl(DSI_BASE_ADDR+DSI_PHY_STATUS));
	printf("DSI_PHY_TEST_CTRL0: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_TEST_CTRL0),readl(DSI_BASE_ADDR+DSI_PHY_TEST_CTRL0));
	printf("DSI_PHY_TEST_CTRL1: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_PHY_TEST_CTRL1),readl(DSI_BASE_ADDR+DSI_PHY_TEST_CTRL1));
	printf("DSI_INT_ST0: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_INT_ST0),readl(DSI_BASE_ADDR+DSI_INT_ST0));
	printf("DSI_INT_ST1: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_INT_ST1),readl(DSI_BASE_ADDR+DSI_INT_ST1));
	printf("DSI_INT_MSK0: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_INT_MSK0),readl(DSI_BASE_ADDR+DSI_INT_MSK0));
	printf("DSI_INT_MSK1: Address at %x and value %X\n", (DSI_BASE_ADDR+DSI_INT_MSK1),readl(DSI_BASE_ADDR+DSI_INT_MSK1));

		
}

void do_dsi_bridge_dump(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	dbg("=========DSI BRIDGE  Register dump==========\n");
	//FOR DSI_BB
	printf("DSI_BB_DMA_BASE_ADDR_REGISTER: Address at %x and value %X\n",DSI_BB_DMA_BASE_ADDR_REGISTER,readl(DSI_BB_DMA_BASE_ADDR_REGISTER));
	printf("DSI_BB_U_COMP_ADDR_REGISTER: Address at %x and value %X\n",DSI_BB_U_COMP_ADDR_REGISTER,readl(DSI_BB_U_COMP_ADDR_REGISTER));
	printf("DSI_BB_V_COMP_ADDR_REGISTER: Address at %x and value %X\n",DSI_BB_V_COMP_ADDR_REGISTER,readl(DSI_BB_V_COMP_ADDR_REGISTER));
	printf("DSI_BB_PIXEL_COMP_REGISTER: Address at %x and value %X\n",DSI_BB_PIXEL_COMP_REGISTER,readl(DSI_BB_PIXEL_COMP_REGISTER));
	printf("DSI_BB_HORIZONTAL_RESOLUTION_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_HORIZONTAL_RESOLUTION_REGISTER,readl(DSI_BB_HORIZONTAL_RESOLUTION_REGISTER),readl(DSI_BB_HORIZONTAL_RESOLUTION_REGISTER));
	printf("DSI_BB_HFP_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_HFP_REGISTER,readl(DSI_BB_HFP_REGISTER),readl(DSI_BB_HFP_REGISTER));
	printf("DSI_BB_HBP_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_HBP_REGISTER,readl(DSI_BB_HBP_REGISTER),readl(DSI_BB_HBP_REGISTER));
	printf("DSI_BB_HSYC_WIDTH_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_HSYC_WIDTH_REGISTER,readl(DSI_BB_HSYC_WIDTH_REGISTER));
	printf("DSI_BB_VERTICAL_RESOLUTION_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_VERTICAL_RESOLUTION_REGISTER,readl(DSI_BB_VERTICAL_RESOLUTION_REGISTER),readl(DSI_BB_VERTICAL_RESOLUTION_REGISTER));
	printf("DSI_BB_VBP_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_VBP_REGISTER,readl(DSI_BB_VBP_REGISTER),readl(DSI_BB_VBP_REGISTER));
	printf("DSI_BB_VFP_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_VFP_REGISTER,readl(DSI_BB_VFP_REGISTER),readl(DSI_BB_VFP_REGISTER));
	printf("DSI_BB_VSYNC_WIDTH_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_VSYNC_WIDTH_REGISTER,readl(DSI_BB_VSYNC_WIDTH_REGISTER),readl(DSI_BB_VSYNC_WIDTH_REGISTER));
	printf("DSI_BB_ERROR_STATE_REGISTER: Address at %x and value hex:%X decimal:%d\n",DSI_BB_ERROR_STATE_REGISTER,readl(DSI_BB_ERROR_STATE_REGISTER),readl(DSI_BB_ERROR_STATE_REGISTER));
	printf("DSI_BB_INTERRUPT_MASK_REGISTER: Address at %x and value %X\n",DSI_BB_INTERRUPT_MASK_REGISTER,readl(DSI_BB_INTERRUPT_MASK_REGISTER));
	printf("DSI_BB_DPI_SHUTDOWN_CONFIG_REGISTER: Address at %x and value %X\n",DSI_BB_DPI_SHUTDOWN_CONFIG_REGISTER,readl(DSI_BB_DPI_SHUTDOWN_CONFIG_REGISTER));

		
}
#if 0
void do_dsi_bridge_enable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dsi_bridge_enable();
}

U_BOOT_CMD(
        dsi_bridge_enable,     1,      1,      do_dsi_bridge_enable,
        "To enable the DSI Bridge",
        " "
        " "
);
#endif
U_BOOT_CMD(
        dsi_dump,     1,      1,      do_dsi_dump,
        "To dump the dsi register set",
        " "
        " "
);

U_BOOT_CMD(
        dsi_bridge_dump,     1,      1,      do_dsi_bridge_dump,
        "To dump the csi register set",
        " "
        " "
);

#endif /*#ifdef CONFIG_DSI*/
#endif

U_BOOT_CMD(
        csi_dump,     1,      1,      do_csi_dump,
        "To dump the csi register set",
        " "
        " "
);



U_BOOT_CMD(
        csi_bridge_dump,     1,      1,      do_csi_bridge_dump,
        "To dump the csi register set",
        " "
        " "
);



int rgb565torgb888_conversion(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int frame_no=0;
	unsigned int source_addr=0;
	//frame_no = atoi(argv[1]);
	source_addr=(DDR_CSI_ADDR_VC1 + ((frame_no)*FRAME_STORAGE_ADDRESS_OFFSET));

	char *source=(char*)(source_addr),*dest=(char*)(CONFIG_HDMI_FB3_ADDR);

	char r,g,b,temp1,temp2,temp3;
	int width=640,height=480;
	int i,j;

	for( i = 0;  i<height; i++){
	for( j = 0;  j<width; j++){
#ifdef CONFIG_OV5640_RGB888	
		temp1 = *source; 
		source++;
        	temp2 = *source;
		source++; 
		temp3 = *source;
		source++;
                 *(dest++) = temp1;
                 *(dest++) = temp2;
                 *(dest++) = temp3;

#endif

#ifdef CONFIG_OV5640_RGB565
                temp1 = *source; 
                source++;
                temp2 = *source;
                source++; 

                b = (temp1 & 0xF1);
                r = ((temp2 & 0x1f) << 3);
                g = ((temp1 << 5) | ((temp2 & 0xE0) >>3));
                 
                 *(dest++) = b;
		 *(dest++) = g;
                 *(dest++) = r;
#endif
#ifdef HDMI_720_480 
	for( j = 640;  j<720; j++){
		dest++;
		dest++;
		dest++;
	}
#endif
   }
   }//i loop
   return 0;	
}
#if 1
int rgb565torgb888()
{
        int frame_no=5;
        unsigned int source_addr=0;
        //frame_no = atoi(argv[1]);
        source_addr=(DDR_CSI_ADDR_VC1 + ((frame_no)*FRAME_STORAGE_ADDRESS_OFFSET));

        char *source=(char*)(source_addr),*dest=(char*)(CONFIG_HDMI_FB3_ADDR);

        int frame_size=0xe1000;

        char r,g,b,temp1,temp2,temp3;
        int width=640,height=480;
        int i,j;

        for( i = 0;  i<height; i++){
        for( j = 0;  j<width; j++){

                temp1 = *source;
                source++;
                temp2 = *source;
                source++;
                temp3 = *source;
                source++;
#if 0

                b = (temp1 & 0xF1);
                r = ((temp2 & 0x1f) << 3);
                g= ((temp1 << 5) | ((temp2 & 0xE0) >>3));
#endif


                 *(dest++) = temp1; //b;
                 *(dest++) = temp2; //g;
                 *(dest++) = temp3; //r;
                        //frame_size = frame_size -2;
        }
 #if 0
	    	 for( j = 640;  j<720; j++){
        	        dest++;
                	dest++;
                	dest++;
        	}
#endif
   }//i loop


}



# define CUSTOM_CONSOLE_COLOR_BLACK    0x00000000
# define CUSTOM_CONSOLE_COLOR_WHITE    0xffffff00/* Must remain last / highest   */
# define CUSTOM_CONSOLE_COLOR_RED      0xff000000
# define CUSTOM_CONSOLE_COLOR_GREEN    0x00ff0000
# define CUSTOM_CONSOLE_COLOR_YELLOW   0xffff0000
# define CUSTOM_CONSOLE_COLOR_BLUE     0x0000ff00
# define CUSTOM_CONSOLE_COLOR_MAGENTA  0xff00ff00
# define CUSTOM_CONSOLE_COLOR_CYAN     0x00FFff00

int color_conversion(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int frame_no=0;
	unsigned int source_addr=0;
	//frame_no = atoi(argv[1]);
	source_addr=(DDR_CSI_ADDR_VC1 + ((frame_no)*FRAME_STORAGE_ADDRESS_OFFSET)); 
	char *source=(char*)(source_addr),*dest=(char*)(CONFIG_HDMI_FB3_ADDR);
	int frame_size=0x96000;
	unsigned int color;
	char r,g,b,temp1,temp2;
	int width=640,height=480;
	int i,j;

	for( i = 0;  i<height; i++){
	for( j = 0;  j<width; j++){

		if((j>=0) && (j < (width/4))) {
		color =CUSTOM_CONSOLE_COLOR_RED;
        	*dest =  0x00; // ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest = 0x00; // ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest = 0xff; // ((color>>8) & 0xff);
        	dest++;
		}


		if((j>=(width/4))&& (j < (width*2/4))) {
		color =CUSTOM_CONSOLE_COLOR_GREEN;
        	*dest = 0x00; // ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest = 0xff; // ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest = 0x00; // ((color>>8) & 0xff);
        	dest++;

		
		}


		if(j>=(width*2/4) & j < (width*3/4)) {
		color =CUSTOM_CONSOLE_COLOR_BLUE;
        	*dest = 0xff; // ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest = 0x00; // ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest = 0x00;  //((color>>8) & 0xff);
        	dest++;

		
		}
		if(j>=(width*3/4) & j < (width)) {
		color =CUSTOM_CONSOLE_COLOR_YELLOW;
        	*dest = 0x00; // ((color>>24) & 0xff);
       		//color >>= 8;
        	dest++;

        	*dest = 0x00; // ((color>>16) & 0xff);
        	//color >>= 8;
        	dest++;

        	*dest = 0x00; // ((color>>8) & 0xff);
        	dest++;

	
		}
		

	}
	for( j = 640;  j<720; j++){
	dest++;
	dest++;
	dest++;
	}

   }
 


}
#endif
U_BOOT_CMD(color_pattern, 5, 1, color_conversion,
        "Color conversion from RGB565 to RGB888",
        "Provide frame size and source and destination address"
        );

U_BOOT_CMD(RGB565toRGB88, 5, 1, rgb565torgb888_conversion,
        "Color conversion from RGB565 to RGB888",
        "Provide frame number to convert"
        );
