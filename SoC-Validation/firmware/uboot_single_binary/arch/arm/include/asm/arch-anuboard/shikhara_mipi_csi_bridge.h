#include <asm/arch/shikhara_map.h>
#include <configs/anusoc.h>

#ifdef CONFIG_SHIKHARA_CORETILEBASED_FPGA
     #define DDR_CSI_ADDR_VC1   		0xE0800000
     #define DDR_CSI_ADDR_U_VC1 		0xE0C00000
     #define DDR_CSI_ADDR_V_VC1 		0xE0D00000
     #define DDR_CSI_ADDR_VC1_FRAME2    	0xE0900000
     #define DDR_CSI_ADDR_U_VC1_FRAME2  	0xE0E00000
     #define DDR_CSI_ADDR_V_VC1_FRAME2  	0xE0F00000
     #define DDR_CSI_ADDR_VC2   		0xE4000000
     #define DDR_CSI_ADDR_U_VC2 		0xE0420000
     #define DDR_CSI_ADDR_V_VC2 		0xE0620000
     #define DDR_CSI_ADDR_VC3   		0xE0200000
     #define DDR_CSI_ADDR_U_VC3 		0xE0404000
     #define DDR_CSI_ADDR_V_VC3 		0xE0604000
     #define DDR_CSI_ADDR_VC4   		0xE6000000
     #define DDR_CSI_ADDR_U_VC4 		0xE6400000
     #define DDR_CSI_ADDR_V_VC4 		0xE6600000
#endif

#ifdef CONFIG_SHIKHARA_ZYNQBASED_FPGA
     #define DDR_CSI_ADDR_VC1   		(CONFIG_V7FPGA_DDR_BASE +0x01000000)
#else

     #define DDR_CSI_ADDR_VC1   		0x11000000
     #define DDR_CSI_ADDR_U_VC1 		0x00C00000
     #define DDR_CSI_ADDR_V_VC1 		0x00D00000
     #define DDR_CSI_ADDR_VC1_FRAME2    	0x00900000
     #define DDR_CSI_ADDR_U_VC1_FRAME2  	0x00E00000
     #define DDR_CSI_ADDR_V_VC1_FRAME2  	0x00F00000
     #define DDR_CSI_ADDR_VC2   		0x04000000
     #define DDR_CSI_ADDR_U_VC2 		0x00420000
     #define DDR_CSI_ADDR_V_VC2 		0x00620000
     #define DDR_CSI_ADDR_VC3   		0x00200000
     #define DDR_CSI_ADDR_U_VC3 		0x00404000
     #define DDR_CSI_ADDR_V_VC3 		0x00604000
     #define DDR_CSI_ADDR_VC4   		0x06000000
     #define DDR_CSI_ADDR_U_VC4 		0x06400000
     #define DDR_CSI_ADDR_V_VC4 		0x06600000
#endif


/*
 * MIPI-CSI Bridge registers. 
 */
#define CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC1     	(CSI2_BB_BASE_ADDR + 0x800)
#define CSI2_BRIDGE_FRAME_INTNUM_VC1            	(CSI2_BB_BASE_ADDR + 0x808)
#define CSI2_BRIDGE_VERTICAL_RES_VC1            	(CSI2_BB_BASE_ADDR + 0x810)
#define CSI2_BRIDGE_HORIZONTAL_RES_VC1          	(CSI2_BB_BASE_ADDR + 0x818)
#define CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC1    	(CSI2_BB_BASE_ADDR + 0x820)
#define CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC1  	(CSI2_BB_BASE_ADDR + 0x828)
#define CSI2_FRAME_COUNT_READ_VC1               	(CSI2_BB_BASE_ADDR + 0x830)

#define CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC2     	(CSI2_BB_BASE_ADDR + 0x900)
#define CSI2_BRIDGE_FRAME_INTNUM_VC2            	(CSI2_BB_BASE_ADDR + 0x908)
#define CSI2_BRIDGE_VERTICAL_RES_VC2            	(CSI2_BB_BASE_ADDR + 0x910)
#define CSI2_BRIDGE_HORIZONTAL_RES_VC2          	(CSI2_BB_BASE_ADDR + 0x918)
#define CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC2    	(CSI2_BB_BASE_ADDR + 0x920)
#define CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC2  	(CSI2_BB_BASE_ADDR + 0x928)
#define CSI2_FRAME_COUNT_READ_VC2               	(CSI2_BB_BASE_ADDR + 0x930)

#define CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC3    		(CSI2_BB_BASE_ADDR + 0xa00)
#define CSI2_BRIDGE_FRAME_INTNUM_VC3           		(CSI2_BB_BASE_ADDR + 0xa08)
#define CSI2_BRIDGE_VERTICAL_RES_VC3           		(CSI2_BB_BASE_ADDR + 0xa10)
#define CSI2_BRIDGE_HORIZONTAL_RES_VC3         		(CSI2_BB_BASE_ADDR + 0xa18)
#define CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC3   		(CSI2_BB_BASE_ADDR + 0xa20)
#define CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC3 		(CSI2_BB_BASE_ADDR + 0xa28)
#define CSI2_FRAME_COUNT_READ_VC3              		(CSI2_BB_BASE_ADDR + 0xa30)

#define CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC4    		(CSI2_BB_BASE_ADDR + 0xb00)
#define CSI2_BRIDGE_FRAME_INTNUM_VC4           		(CSI2_BB_BASE_ADDR + 0xb08)
#define CSI2_BRIDGE_VERTICAL_RES_VC4           		(CSI2_BB_BASE_ADDR + 0xb10)
#define CSI2_BRIDGE_HORIZONTAL_RES_VC4         		(CSI2_BB_BASE_ADDR + 0xb18)
#define CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC4   		(CSI2_BB_BASE_ADDR + 0xb20)
#define CSI2_BRIDGE_FRAME_LINE_INTR_STATUS_VC4 		(CSI2_BB_BASE_ADDR + 0xb28)
#define CSI2_FRAME_COUNT_READ_VC4              		(CSI2_BB_BASE_ADDR + 0xb30)

#define AXI_Error_FIFO_Mask      			(CSI2_BB_BASE_ADDR + 0xc00)
#define AXI_Error_FIFO_Status    			(CSI2_BB_BASE_ADDR + 0xc08)
#define Current_AXI_Write_Addr   			(CSI2_BB_BASE_ADDR + 0xc10)
#define Packet_Header_Info       			(CSI2_BB_BASE_ADDR + 0xc18)
#define CSI_DSI_Direct_Config    			(CSI2_BB_BASE_ADDR + 0xc20)


/* Bit defination of Pixel configuration register */

#define LITTLE_EN 					0 << 6
#define BIG_EN						1 << 6
#define YUV_STORED_SEPARATELY				1 << 7
#define DTYPE_INT_WITHINFRAME_DISABLED			0 << 9
#define DTYPE_INT_WITHINFRAME_ENABLED			1 << 9
#define PROGRESSIVE_V					0 << 10 
#define INTERLACED_V					1 << 10
#define LINE_START_END_PACKET_DIS			0 << 11 
#define LINE_START_END_PACKET_EN			1 << 11

#define YUV420_8BIT					 0x18  //	YUV 420 8-bit.
#define YUV420_10BIT					 0x19  //	YUV 420 10-bit.
#define YUV420_8BIT					 0x1a  //	Legacy YUV 420 8-bit.
#define YUV420_8BIT_CHROMA_SHIFTED			 0x1c  //	YUV 420 8-bit (Chroma Shifted Pixel Sampling)
#define YUV420_10BIT_CHROMA_SHIFTED                      0x1d  //       YUV 420 10-bit (Chroma Shifted Pixel Sampling
#define YUV422_8BIT					 0x1e  //	YUV 422 8-bit
#define YUV422_10BIT					 0x1f  //	YUV 422 10-bit.


#define RGB444 						 0x20  //	RGB444.
#define RGB555						 0x21  //	RGB555.
#define RGB565						 0x22  //	RGB565.
#define	RGB666 						 0x23  //	RGB666.
#define RGB888						 0x24  //	RGB888.
#define	RAW6						 0x28  //	RAW6.
#define	RAW7						 0x29  //	RAW7.
#define RAW8						 0x2a  //	RAW8.
#define RAW10 						 0x2b  //	RAW10.
#define RAW12						 0x2c  //	RAW12.
#define RAW14 						 0x2d  //	RAW14.

#define FRAME_STORAGE_ADDRESS_OFFSET 			 0x100000
#define NUMBER_OF_BUFFER_DESCRIPTORS			 10
int mipi_csi_bridge_init(void);
