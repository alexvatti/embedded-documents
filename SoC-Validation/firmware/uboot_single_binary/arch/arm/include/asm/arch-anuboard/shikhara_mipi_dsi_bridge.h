//***************************************************************************
// Copyright (c) 2012 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : MIPI-DSI Bridge Controller
//Title              : Shikhara Display Configuration
//Project          : SHIKHARA
//Owner           : Kaushal 
//Date Created    : 7/10/2013
//***************************************************************************
//Description : Shikhara Display Controller Register Description and 
// APIs.
//***************************************************************************
#include<asm/arch/shikhara_map.h>
#ifndef __SHIKHARA_MIPI_DSI_BRIDGE_H__
#define __SHIKHARA_MIPI_DSI_BRIDGE_H__


#define DSI_BB_DMA_BASE_ADDR_REGISTER            (DSI_BB_BASE_ADDR + 0x0)
#define DSI_BB_U_COMP_ADDR_REGISTER              (DSI_BB_BASE_ADDR + 0x4)
#define DSI_BB_V_COMP_ADDR_REGISTER              (DSI_BB_BASE_ADDR + 0x8)
#define DSI_BB_PIXEL_COMP_REGISTER               (DSI_BB_BASE_ADDR + 0xC)
#define DSI_BB_HORIZONTAL_RESOLUTION_REGISTER    (DSI_BB_BASE_ADDR + 0x10)
#define DSI_BB_HFP_REGISTER                      (DSI_BB_BASE_ADDR + 0x14)
#define DSI_BB_HBP_REGISTER                      (DSI_BB_BASE_ADDR + 0x18)
#define DSI_BB_HSYC_WIDTH_REGISTER               (DSI_BB_BASE_ADDR + 0x1C)
#define DSI_BB_VERTICAL_RESOLUTION_REGISTER      (DSI_BB_BASE_ADDR + 0x20)
#define DSI_BB_VBP_REGISTER                      (DSI_BB_BASE_ADDR + 0x24)
#define DSI_BB_VFP_REGISTER                      (DSI_BB_BASE_ADDR + 0x28)
#define DSI_BB_VSYNC_WIDTH_REGISTER              (DSI_BB_BASE_ADDR + 0x2C)
#define DSI_BB_ERROR_STATE_REGISTER              (DSI_BB_BASE_ADDR + 0x30)
#define DSI_BB_INTERRUPT_MASK_REGISTER           (DSI_BB_BASE_ADDR + 0x34)
#define DSI_BB_DPI_SHUTDOWN_CONFIG_REGISTER      (DSI_BB_BASE_ADDR + 0x38)



/* Bit definations of MIPI-DSI Bridge registers*/

#define DSI_BB_ENABLE		1 << 0
#define DATA_EN			1 << 2		// 1-Active low, 0-Active High
#define VSYNC_POL		1 << 3		// 1-Active low, 0-Active High  
#define HYNCE_POL		1 << 4		// 1-Active low, 0-Active High
#define BITS_16_CON1		0 << 5		// 0000: 16-bit configuration 1 
#define BITS_16_CON2		1 << 5		// 0001: 16-bit configuration 2 
#define BITS_16_CON3		2 << 5		// 0001: 16-bit configuration 3
#define BITS_18_CON1		3 << 5		// 0011: 18-bit configuration 1
#define BITS_18_CON2              4 << 5		// 0100: 18-bit configuration 2
#define BITS_24_CON		5 << 5		// 0101: 24-bit configuration
#define BITS_20_YCBCR_422		6 << 5		// 0110: 20-bit YCbCr 4:2:2 loosely packed 
#define BIT_20_YCBCR_422		7 << 5		// 0111: 20-bit YCbCr 4:2:2 
#define BITS_20_YCBCR_422 	8 << 5		// 1000: 16-bit YCbCr 4:2:2 
#define BITS_30_CON		9 << 5		// 1001: 30-bit 
#define BITS_36_CON		10 << 5		// 1010: 36-bit 
#define BITS_12_YCBCR_420		11 << 5		// 1011-1111: 12-bit YCbCr 4:2:0
#define RGB_COLOR_M		0 <<  9		// RGB color model
#define YUV_COLOR_M		1 << 9		// YUV color model
#define VIDEO_MODE		0 << 10		// Video mode for MIPI-DSI's DPI interface
#define COMMMAND_MODE_AUTO	1 << 10		// Command mode Auto for MIPI-DSI DBI interface
#define COMMAND_MODE_MANUAL	2 << 10 	// Command mode Manual for MIPI-DSI DBI interface (ByPass MIPI-DSI Bridge)
#define LITTLE_ENDIAN		0 << 12		// Little endian 
#define BIG_ENDIAN		1 << 12 	// BIG endian
#define SHUTDOWN_POL		1 << 13		// Shutdown signal polarity to active low
#define COLOR_M_POL		1 << 14		// Color mode polarity to active low
#define SHUTDOWN_PANEL		1 << 15		// 1- Shutdown the display
#define COLOR_MODE		1 << 16 	// 1- COLOR MODE change from Normal to Reduced color mode
#define CSI_TO_DSI_DIRECT	1 << 17		// 1- Y or RGB  FIFO, U – FIFIO, V- FIFO will be filled by data from CSI bridge. AXI read transactions will not performed by the DSI bridge. 		// 0- Y or RGB  FIFO, U – FIFIO, V- FIFO will be filled by AXI read transactions
#define VSEQUENCE_0		0 << 18		// 00 – VSA->VBP->VACT->VFP
#define VSEQUENCE_1             1 << 18		// 01 – VBP->VACT->VFP->VSA
#define VSEQUENCE_2             2 << 18		// 10 – VACT->VFP->VSA->VBP
#define VSEQUENCE_3             3 << 18		// 11 – VFP->VSA->VBP->VACT

#define HSEQUENCE_0             0 << 20         // 00 – HSA->HBP->HACT->HFP
#define HSEQUENCE_1             1 << 20         // 01 – HBP->HACT->HFP->HSA
#define HSEQUENCE_2             2 << 21         // 10 – HACT->HFP->HSA->HBP
#define HSEQUENCE_3             3 << 22         // 11 – HFP->HSA->HBP->HACT
#endif
