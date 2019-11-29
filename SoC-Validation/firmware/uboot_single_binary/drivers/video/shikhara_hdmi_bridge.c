//***************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : Shikhara HDMI Bridge 
//Title           : Shikhara HDMI Bridge
//Project         : SHIKHARA
//Owner           : Kaushal Billore
//Date Created    : 22/10/2013

//**************************************************************************
//Description : Shikhara HDMI Bridge Initilization and Configuration
//***************************************************************************

#include <asm/types.h>
#include <asm/arch/shikhara_hdmi_bridge.h>
#include <common.h>
#include <lcd.h>
#include <asm/io.h>

int shikhara_hdmi_bridge_reset(void)
{
	unsigned int reg;
/* APB reset for both HDMI Bridge and HDMI Tx Controller in System contrller */
	reg = readl(SHIKHARA_SYS_CTRL_BASEADDR + 0x44);
	reg |=	(SYS_CTRL_HDMI_BRIDGE_APB_RESET_MASK << SYS_CTRL_HDMI_BRIDGE_APB_RESET_OFFSET);
	writel(reg, SHIKHARA_SYS_CTRL_BASEADDR + 0x44);
	reg &=  ~(SYS_CTRL_HDMI_BRIDGE_APB_RESET_MASK << SYS_CTRL_HDMI_BRIDGE_APB_RESET_OFFSET);
	writel(reg, SHIKHARA_SYS_CTRL_BASEADDR + 0x44);	
/* HDMI Bridge reset(bit-24) and Pixelclock(PLL) reset for both HDMI Bridge and HDMI Tx controller(bit-23) */	
	reg = readl(SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	reg |= ( SYS_CTRL_HDMI_BRIDGE_RESET_MASK << SYS_CTRL_HDMI_BRIDGE_RESET_OFFSET);
	writel(reg, SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	reg &= ~(SYS_CTRL_HDMI_BRIDGE_RESET_MASK << SYS_CTRL_HDMI_BRIDGE_RESET_OFFSET);
	writel(reg, SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	return 0;

}
/*Low-level HDMI bridge function to configure pixel format and respective addresses */
int shikhara_hdmi_bridge_reinit(unsigned int color_choice, unsigned long lcdbase3)
{
	unsigned int reg;

	switch(color_choice)
	{
		case FORMAT_RGB:
			reg = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
                        reg &= ~((PIXEL_FORMAT_MASK<< PIXEL_FORMAT_OFFSET) | (YUV_420_PIXEL_FORMAT_MASK << YUV_420_PIXEL_FORMAT_OFFSET) | HDMI_BB_YUV);
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			writel(lcdbase3, HDMI_BB_DMA_BASE_ADDR_REGISTER_1);
			break; 

		case FORMAT_YUV_420:
			reg = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			reg &= ~((PIXEL_FORMAT_MASK<< PIXEL_FORMAT_OFFSET) | (YUV_420_PIXEL_FORMAT_MASK << YUV_420_PIXEL_FORMAT_OFFSET));
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			reg |= YUV420_8Bit | HDMI_BB_YUV;
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			writel(lcdbase3, HDMI_BB_DMA_BASE_ADDR_REGISTER_1);
			writel(lcdbase3 + U_COMPONENT_DDR_BASE, HDMI_BB_U_COMP_ADDR_REGISTER_1);
			writel(lcdbase3 + V_COMPONENT_DDR_BASE_YUV_420, HDMI_BB_V_COMP_ADDR_REGISTER_1);
			break; 
			
		case FORMAT_YUV_422:
			reg = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
                        reg &= ~((PIXEL_FORMAT_MASK<< PIXEL_FORMAT_OFFSET) | (YUV_420_PIXEL_FORMAT_MASK << YUV_420_PIXEL_FORMAT_OFFSET));
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			reg |= YUV422_8Bit | HDMI_BB_YUV;
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			writel(lcdbase3, HDMI_BB_DMA_BASE_ADDR_REGISTER_1);
			writel(lcdbase3 + U_COMPONENT_DDR_BASE, HDMI_BB_U_COMP_ADDR_REGISTER_1);
			writel(lcdbase3 + V_COMPONENT_DDR_BASE_YUV_422, HDMI_BB_V_COMP_ADDR_REGISTER_1);
			break;
 
		case FORMAT_YUV_444:
			reg = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
                        reg &= ~((PIXEL_FORMAT_MASK << PIXEL_FORMAT_OFFSET) | (YUV_420_PIXEL_FORMAT_MASK << YUV_420_PIXEL_FORMAT_OFFSET));
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			reg |= YUV444_8Bit | HDMI_BB_YUV;
			writel(reg, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
			writel(lcdbase3, HDMI_BB_DMA_BASE_ADDR_REGISTER_1);
			writel(lcdbase3 + U_COMPONENT_DDR_BASE_YUV_444, HDMI_BB_U_COMP_ADDR_REGISTER_1);
			writel(lcdbase3 + V_COMPONENT_DDR_BASE_YUV_444, HDMI_BB_V_COMP_ADDR_REGISTER_1);
			break; 
	}
		return 0;
}


/*
 * Low level HDMI Bridge Initilization function
 */
int shikhara_hdmi_bridge_init(void *lcdbase3){
unsigned int x;
#if 0
 x = readl(SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
 x |= (1<<24);
 writel(x, SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
 x &= ~(1<<24);
 writel(x, SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
#endif
/* HDMI Bridge is disable for now. */ 
	 writel (0X00000000, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);

/* lcdbase3 (DDR_HDMI_VIDEO_ADDR) is configured as a framebuffer3 for HDMI Bridge */
	 writel ((int)lcdbase3 , HDMI_BB_DMA_BASE_ADDR_REGISTER_1); 
	 writel ((int)lcdbase3 , HDMI_BB_DMA_BASE_ADDR_REGISTER_2); 

/* Configure Horizontal resolution as 720 */
      	 writel (panel_info3->vl_col, HDMI_BB_HORIZONTAL_RESOLUTION_REGISTER);    

/* Configure Horizontal Front Porch 16 */ 
	 writel (panel_info3->vl_hfp, HDMI_BB_H_FRONT_P_REGISTER);                

/* Configure Horizontal Back Porch 60 */
	 writel (panel_info3->vl_hbp, HDMI_BB_H_BACK_P_REGISTER);                

/* Configure Horizontal Sync width as 62 */ 
	 writel (panel_info3->vl_hsync_len, HDMI_BB_HSYC_WIDTH_REGISTER);               

/* Configure Vertical Resolution as 480*/	
         writel (panel_info3->vl_row, HDMI_BB_VERTICAL_RESOLUTION_REGISTER);      

/* Configure Vertical Back Porch as 30 */
      	 writel (panel_info3->vl_vbp, HDMI_BB_V_BACK_P_REGISTER);                 

/* Configure Vertical Front Porch as 9*/	
	 writel (panel_info3->vl_vfp, HDMI_BB_V_FRONT_P_REGISTER );                

/* Configure Vertical Sync Width as 6 */ 
	writel (panel_info3->vl_vsync_len, HDMI_BB_VSYNC_WIDTH_REGISTER); 

/* Pixel Format Register Configuration:-
 * HDMI display controller- Enabled
 * Interlaced- Progressive
 * Data_En Pol- Active Low, HSYNC Pol- Active Hight, VSYNC Pol- Active High
 * Pixel Format- RGB444 (8Bit)
 * Color Model- RGB
 * Endianess- Little Endian
 */
	writel ((VSYNC_POL | HSYNC_POL | DATA_EN_POL | PROGRESSIVE | RGB444_8Bit | HDMI_BB_RGB | HDMI_BB_LITTELEN), HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);

return 0;
}

int enable_hdmi_bridge(void){
	int x;
	
	x = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
	x |= 1;
 	writel(x, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
 	
	return 0;
}

void disable_hdmi_bridge(void)
{
	int x;
	x = readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);
	x &= ~(DATA_EN_POL | HDMI_BB_ENABLE);
	writel(x, HDMI_BB_PIXEL_FORMAT_CNG_REGISTER);

        hdmi_power_off();  

}

