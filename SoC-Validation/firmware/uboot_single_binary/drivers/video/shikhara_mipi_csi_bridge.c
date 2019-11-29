//***************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : MIPI-CSI2 MIPI-CSI2 Bridge Controller
//Title           : MIPI-CSI2 Bridge Configuration
//Project         : SHIKHARA
//Owner           : Kaushal 
//Date Created    : 4/10/2013
//***************************************************************************
//Description : Driver does configuration for MIPI-CSI2 bridge (Camera Controller)  
//***************************************************************************

#include <asm/arch/shikhara_mipi_csi_bridge.h>
#include <asm/types.h>
#include <common.h>

/**
 * @brief: mipi_csi_bridge_init() is the function to initilize mipi-csi2 bridge  
 * @param base_address: takes the base address of mipi_csi2 bridge registers
 * @return: True if success
 **/
int mipi_csi_bridge_init(void){
	 unsigned int x,i;

	/* Address configuration of the location where the Pixel data is going to be stored Frame1 */

	for(i=0; i < NUMBER_OF_BUFFER_DESCRIPTORS; i++)
		writel( DDR_CSI_ADDR_VC1+(i*FRAME_STORAGE_ADDRESS_OFFSET) , (CSI2_BB_BASE_ADDR+(8*i))); //There are 32 Register for "Frame  Storage  Address" 8 bytes apart

	writel( 0x0, (CSI2_BB_BASE_ADDR+(8*(NUMBER_OF_BUFFER_DESCRIPTORS)))); // Assigning NULL to 32nd frame Descriptor.


	/* Vertical resolution configured for 480 */
	writel(0x1E0, CSI2_BRIDGE_VERTICAL_RES_VC1 );

	/*Horizontal resolution configured for 640 No of byte expected in a line is ((xres * 32)/8) */
	//writel((640 * 32/8), CSI2_BRIDGE_HORIZONTAL_RES_VC1 );

#ifdef CONFIG_OV5640_RGB565 
	writel(0x05000500, CSI2_BRIDGE_HORIZONTAL_RES_VC1 ); /*Both EVEN and ODD lines*/
	x = ( RGB565  | LITTLE_EN | DTYPE_INT_WITHINFRAME_DISABLED | PROGRESSIVE_V | LINE_START_END_PACKET_DIS/* LINE_START_END_PACKET_EN*/);
	writel(0x22, CSI_DSI_Direct_Config);
#endif

#ifdef CONFIG_OV5640_RGB888
	writel(0x07800780, CSI2_BRIDGE_HORIZONTAL_RES_VC1 ); /*Both EVEN and ODD lines*/
	x = ( RGB888  | LITTLE_EN | DTYPE_INT_WITHINFRAME_DISABLED | PROGRESSIVE_V | LINE_START_END_PACKET_DIS/* LINE_START_END_PACKET_EN*/);
	writel(0x24, CSI_DSI_Direct_Config);
#endif

	/* Frame Format Configuration 
	* Data Type- RGB888
	* Endianess- Little Endian
	* Datatype Interleaving- Disabled 
	* Interlaced- Progressive
	* Line Start End packets - Disabled 
	*/
	//enable_csi_bridge
	
	writel(x, CSI2_BRIDGE_FRAME_FORMAT_CONFIG_VC1);	
	writel(0x0, CSI2_BRIDGE_FRAME_LINE_INTR_MASK_VC1 ); /*Mask the interrupt from CSI BB for every 1 Frame*/
	/*Interrupt Frames Number Register for virtual channel 1 configured for interrupt after every one frame*/
	writel(NUMBER_OF_BUFFER_DESCRIPTORS, CSI2_BRIDGE_FRAME_INTNUM_VC1 );
        /* printf("mipi_csi_bridge_init() completed. \n"); */
return 0;
}

