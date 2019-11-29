//***************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : MIPI-DSI Bridge Controller
//Title           : Shikhara Display Configuration

//Project         : SHIKHARA

//Owner           : Kaushal 

//Date Created    : 7/10/2013

//**************************************************************************

//Description : Shikhara Display Controller Initilization and configuration
//***************************************************************************

#include <asm/arch/shikhara_mipi_dsi_bridge.h>
#include <asm/types.h>
#include <common.h>
#include <lcd.h>

u8 mipi_dsi_bridge_init(void *lcdbase2)
{

/* Disable The Shikhara Display controller(DSI Bridge) */
   writel (0X00000000, DSI_BB_PIXEL_COMP_REGISTER );

/* Configuring the address of the DDR memory from where Display controller is to fetch pixel data */
   writel (CONFIG_MIPI_DSI_FB2_ADDR, DSI_BB_DMA_BASE_ADDR_REGISTER ); 

/* Configure Horizontal resolution */
   writel (panel_info2->vl_col, DSI_BB_HORIZONTAL_RESOLUTION_REGISTER ); 

/* Configure Horizontal Front Porch */
   writel (panel_info2->vl_hfp, DSI_BB_HFP_REGISTER );  

/* Configure Horizontal Back Porch */
   writel (panel_info2->vl_hbp, DSI_BB_HBP_REGISTER );  

/* Configure Horizontal Sync width  */
   writel (panel_info2->vl_hsync_len, DSI_BB_HSYC_WIDTH_REGISTER);

/* Configure Vertical Resolution  */
   writel (panel_info2->vl_row, DSI_BB_VERTICAL_RESOLUTION_REGISTER );

/* Configure Vertical Back Porch */
   writel ( panel_info2->vl_vbp, DSI_BB_VBP_REGISTER); 

/* Configure Vertical Front Porch  */
   writel ( panel_info2->vl_vfp, DSI_BB_VFP_REGISTER);   

/* Configure Vertical Sync Width */
   writel (panel_info2->vl_vsync_len, DSI_BB_VSYNC_WIDTH_REGISTER);  

   /*Enable Bridge using dsi_bridge_enable command on u-boot prompt*/
   writel (( BITS_24_CON | RGB_COLOR_M | VIDEO_MODE | LITTLE_ENDIAN | VSEQUENCE_0 | HSEQUENCE_0), DSI_BB_PIXEL_COMP_REGISTER);

/* Set the count of number of frames for which dpishutdown signal' is to be sent as 1 */
   writel (0X00000000, DSI_BB_DPI_SHUTDOWN_CONFIG_REGISTER);

/* Enable Shikhara MIPI-DSI Display controller
   Configure pixel format for 24 bit
   RGB Color Model
   Little Endian format
   Data_En Pol- Active Low, HSYNC Pol- Active Low, VSYNC Pol- Active Low, Shutdown Pol- Active Low, Colorm Pol- Active Low
   dpicolormode- 0 (Normal Color Mode)
   dpishutdown- 0 
   Video Mode(DPI)
*/
   /*Enable Bridge using dsi_bridge_enable command on u-boot prompt*/
//   writel (( BITS_24_CON | RGB_COLOR_M | VIDEO_MODE | LITTLE_ENDIAN | VSEQUENCE_0 | HSEQUENCE_0), DSI_BB_PIXEL_COMP_REGISTER);
   printf("DSI bridge is initialized, Enable Bridge using dsi_bridge_enable command on u-boot prompt \n");
return 0;
}


void dsi_bridge_enable(void)
{
        int x;

        x = readl(DSI_BB_PIXEL_COMP_REGISTER);
        x |= DSI_BB_ENABLE;
        writel(x, DSI_BB_PIXEL_COMP_REGISTER);

}

void dsi_bridge_disable(void)
{
        int x;

        x = readl(DSI_BB_PIXEL_COMP_REGISTER);
        x &= ~(DSI_BB_ENABLE);
        writel(x, DSI_BB_PIXEL_COMP_REGISTER);

}
#if 0
void do_dsi_bridge_enable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dsi_bridge_enable();
}


void do_dsi_bridge_disable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	dsi_bridge_disable();

}
#endif

void do_dsi_disable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        dsi_bridge_disable();
        display_power_off();

}

#if 0

U_BOOT_CMD(dsi_bridge_enable,     1,      1,      do_dsi_bridge_enable,
        "To enable mipi dsi bridge ",
        " "
);

U_BOOT_CMD(dsi_bridge_disable,     1,      1,      do_dsi_bridge_disable,
        "To disable mipi dsi bridge ",
        " "
);
#endif

U_BOOT_CMD(dsi_disable,     1,      1,      do_dsi_disable,
        "To disable mipi dsi display ",
        " "
);

