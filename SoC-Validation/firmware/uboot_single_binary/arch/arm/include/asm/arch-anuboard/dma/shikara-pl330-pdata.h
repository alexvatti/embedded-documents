#ifndef __SHIKARA_PL330_PDATA_H
#define __SHIKARA_PL330_PDATA_H

#include <malloc.h>
#include <asm/arch/dma/shikara-dma-pl330.h>

/*
 * Every PL330 DMAC has max 32 peripheral interfaces,
 * of which some may be not be really used in your
 * DMAC's configuration.
 * Populate this array of 32 peri i/fs with relevant
 * channel IDs for used peri i/f and DMACH_MAX for
 * those unused.
 *
 * The platforms just need to provide this info
 * to the SHIKARA DMA API driver for PL330.
 */
struct shikara_pl330_chndata {
	enum dma_ch peri[32];
};


struct shikara_pl330_chndata shikara_pdma = {
	.peri = {
		[0] = DMACH_UART2_TX,
		[1] = DMACH_UART2_RX,
		[2] = DMACH_UART3_TX,
		[3] = DMACH_UART3_RX,
		[4] = DMACH_I2C0_TX,
		[5] = DMACH_I2C0_RX,
		[6] = DMACH_I2C1_TX,
		[7] = DMACH_I2C1_RX,
		[8] = DMACH_I2C2_TX,
		[9] = DMACH_I2C2_RX,
		[10] = DMACH_RESERVED,
		[11] = DMACH_RESERVED,
		[12] = DMACH_RESERVED,
		[13] = DMACH_RESERVED,
		[14] = DMACH_RESERVED,
		[15] = DMACH_RESERVED,
		[16] = DMACH_UART0_TX,
		[17] = DMACH_UART0_RX,
		[18] = DMACH_UART1_TX,
		[19] = DMACH_UART1_RX,
		[20] = DMACH_SPI0_TX,
		[21] = DMACH_SPI0_RX,
		[22] = DMACH_RESERVED,
		[23] = DMACH_SDMMC_TX,
		[24] = DMACH_SDMMC_RX,
		[25] = DMACH_SDMMC_TX,
		[26] = DMACH_SPI1_TX,
		[27] = DMACH_SPI1_RX,
		[28] = DMACH_MAX,
		[29] = DMACH_MAX,
		[30] = DMACH_MAX,
		[31] = DMACH_MAX,
	},
};
/*We have only two requst interfaces, but declare 32 interfaces to satisy driver*/
struct shikara_pl330_chndata shikara_mdma = {
        .peri = {
		[0] = DMACH_M2M_SR,
		[1] = DMACH_M2M_NSR,
		[2] = DMACH_MAX,
		[3] = DMACH_MAX,
		[4] = DMACH_MAX,
		[5] = DMACH_MAX,
		[6] = DMACH_MAX,
		[7] = DMACH_MAX,
		[8] = DMACH_MAX,
		[9] = DMACH_MAX,
		[10] = DMACH_MAX,
		[11] = DMACH_MAX,
		[12] = DMACH_MAX,
		[13] = DMACH_MAX,
		[14] = DMACH_MAX,
		[15] = DMACH_MAX,
		[16] = DMACH_MAX,
		[17] = DMACH_MAX,
		[18] = DMACH_MAX,
		[19] = DMACH_MAX,
		[20] = DMACH_MAX,
		[21] = DMACH_MAX,
		[22] = DMACH_MAX,
		[23] = DMACH_MAX,
		[24] = DMACH_MAX,
		[25] = DMACH_MAX,
		[26] = DMACH_MAX,
		[27] = DMACH_MAX,
		[28] = DMACH_MAX,
		[29] = DMACH_MAX,
		[30] = DMACH_MAX,
		[31] = DMACH_MAX,
	},
};
#endif 
