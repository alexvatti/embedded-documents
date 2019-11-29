#include <common.h>
#include<asm/arch/shikhara_map.h>
#include<lcd.h>
#ifndef __SHIKHARA_HDMI_BRIDGE_H__
#define __SHIKHARA_HDMI_BRIDGE_H__

//***************************************************************************
// HDMI BLOCKBOX - Shikhara HDMI Bridge Base address and bit defination
//***************************************************************************

#define HDMI_BB_DMA_BASE_ADDR_REGISTER_1            (HDMI_BB_BASE_ADDR + 0x0)
#define HDMI_BB_U_COMP_ADDR_REGISTER_1              (HDMI_BB_BASE_ADDR + 0x4)
#define HDMI_BB_V_COMP_ADDR_REGISTER_1              (HDMI_BB_BASE_ADDR + 0x8)
#define HDMI_BB_DMA_BASE_ADDR_REGISTER_2            (HDMI_BB_BASE_ADDR + 0xC)
#define HDMI_BB_U_COMP_ADDR_REGISTER_2              (HDMI_BB_BASE_ADDR + 0x10)
#define HDMI_BB_V_COMP_ADDR_REGISTER_2              (HDMI_BB_BASE_ADDR + 0x14)
#define HDMI_BB_PIXEL_FORMAT_CNG_REGISTER           (HDMI_BB_BASE_ADDR + 0x18)
#define HDMI_BB_HORIZONTAL_RESOLUTION_REGISTER      (HDMI_BB_BASE_ADDR + 0x1C)
#define HDMI_BB_H_FRONT_P_REGISTER                  (HDMI_BB_BASE_ADDR + 0x20)
#define HDMI_BB_H_BACK_P_REGISTER                   (HDMI_BB_BASE_ADDR + 0x24)
#define HDMI_BB_HSYC_WIDTH_REGISTER                 (HDMI_BB_BASE_ADDR + 0x28)
#define HDMI_BB_VERTICAL_RESOLUTION_REGISTER        (HDMI_BB_BASE_ADDR + 0x2C)
#define HDMI_BB_V_BACK_P_REGISTER                   (HDMI_BB_BASE_ADDR + 0x30)
#define HDMI_BB_V_FRONT_P_REGISTER                  (HDMI_BB_BASE_ADDR + 0x34)
#define HDMI_BB_VSYNC_WIDTH_REGISTER                (HDMI_BB_BASE_ADDR + 0x38)
#define HDMI_BB_ERROR_STATE_REGISTER                (HDMI_BB_BASE_ADDR + 0x3C)
#define HDMI_BB_INTR_MASK_REGISTER                  (HDMI_BB_BASE_ADDR + 0x40)



// Bit definations for Pixel format configuration register Bit defination
#define HDMI_BB_ENABLE		1 << 0
#define PROGRESSIVE		0 << 1 	//Progressive scan
#define INTERLACED		1 << 1  //Interlace scan
#define DATA_EN_POL		1 << 2 	//1- active High 0- active low
#define VSYNC_POL		1 << 3	//1- active High 0- active low
#define HSYNC_POL		1 << 4  //1- active High 0- active low
#define RGB444_8Bit		0 << 5	//0000----RGB 4:4:4 (8-bit)
#define RGB444_10Bit		1 << 5  //0001----RGB 4:4:4 (10-bit)
#define RGB444_12Bit		2 << 5  //0010----RGB 4:4:4 (12-bit)
#define RGB444_16Bit		3 << 5  //0011----RGB 4:4:4 (16-bit)
#define YUV444_8Bit		4 << 5	//0100----YUV 4:4:4 (8-bit)
#define YUV444_10Bit		5 << 5  //0101---YUV 4:4:4 (10-bit)
#define YUV444_12Bit		6 << 5  //0110---YUV 4:4:4 (12-bit)
#define YUV444_16Bit		7 << 5  //0111---YUV 4:4:4 (16-bit)
#define YUV422_8Bit		8 << 5  //1000---YUV 4:2:2 (8-bit)
#define YUV422_10Bit		9 << 5 	//1001---YUV 4:2:2 (10-bit)
#define YUV422_12Bit		10 << 5 //1010---YUV 4:2:2 (12-bit)
#define HDMI_BB_RGB		0 << 9  // RGB color model
#define HDMI_BB_YUV		1 << 9  // YUV color model
#define HDMI_BB_BIGEN		1 << 10 // Big Endian
#define HDMI_BB_LITTELEN	0 << 10 // Little Endian
#define YUV420_8Bit		1 << 11 // Input Pixel Format is YUV4:2:0(8Bit)
#define YUV420_10Bit		2 << 11	// Input Pixel Format is YUV4:2:0(10Bit)

#define FORMAT_RGB      1
#define FORMAT_YUV_420  2
#define FORMAT_YUV_422  3
#define FORMAT_YUV_444  4

#define FRAME_SIZE              (panel_info3->vl_row *panel_info3->vl_col)
#define U_COMPONENT_DDR_BASE    FRAME_SIZE
#define V_COMPONENT_DDR_BASE_YUV_422    (U_COMPONENT_DDR_BASE + FRAME_SIZE/2)
#define V_COMPONENT_DDR_BASE_YUV_420    (U_COMPONENT_DDR_BASE + FRAME_SIZE/4)
#define U_COMPONENT_DDR_BASE_YUV_444	FRAME_SIZE
#define V_COMPONENT_DDR_BASE_YUV_444	(U_COMPONENT_DDR_BASE_YUV_444 + FRAME_SIZE)

enum {
/*offset and mask for pixel format */
        PIXEL_FORMAT_MASK = 0xF,
        PIXEL_FORMAT_OFFSET = 0x5,
/*offset and mask for pixel format of YUV420 */
        YUV_420_PIXEL_FORMAT_MASK = 0x3,
        YUV_420_PIXEL_FORMAT_OFFSET = 11,
/*offset and mask for HDMI bridge reset ARESETn_HDMIVDWRAPPER and reset_HDMI */
	SYS_CTRL_HDMI_BRIDGE_RESET_MASK = 0X3,
	SYS_CTRL_HDMI_BRIDGE_RESET_OFFSET = 23,
	SYS_CTRL_HDMI_BRIDGE_APB_RESET_MASK = 0x1,
	SYS_CTRL_HDMI_BRIDGE_APB_RESET_OFFSET = 0,
};


/*Function declaration */
int shikhara_hdmi_bridge_init(void *lcdbase3);
int shikhara_hdmi_bridge_reset(void);
int enable_hdmi_bridge(void);
int shikhara_hdmi_bridge_reinit(unsigned int color_choice, unsigned long lcdbase3);

#endif

