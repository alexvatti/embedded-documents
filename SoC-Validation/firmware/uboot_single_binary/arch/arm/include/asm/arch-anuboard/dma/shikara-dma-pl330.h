
#ifndef	__SHIKARA_DMA_PL330_H_
#define	__SHIKARA_DMA_PL330_H_
#define CONFIG_CMD_DMATEST
#define SHIKARA_DMAF_AUTOSTART		(1 << 0)
#define SHIKARA_DMAF_CIRCULAR		(1 << 1)
#include <asm/arch/dma/dma.h>
#include <asm/arch/dma/pl330.h>
/*
 * PL330 can assign any channel to communicate with
 * any of the peripherals attched to the DMAC.
 * For the sake of consistency across client drivers,
 * We keep the channel names unchanged and only add
 * missing peripherals are added.
 * Order is not important since Shikara PL330 API driver
 * use these just as IDs.
 */
enum dma_ch {
	DMACH_UART2_TX = 0,
	DMACH_UART2_RX,
	DMACH_UART3_TX,
	DMACH_UART3_RX,
	DMACH_I2C0_TX,
	DMACH_I2C0_RX,
	DMACH_I2C1_TX,
	DMACH_I2C1_RX,
	DMACH_I2C2_TX,
	DMACH_I2C2_RX,
	DMACH_RESERVED,
	DMACH_UART0_TX =16,
	DMACH_UART0_RX,
	DMACH_UART1_TX,
	DMACH_UART1_RX,
	DMACH_SPI0_TX,
	DMACH_SPI0_RX,
	DMACH_SDMMC_TX= 23,
	DMACH_SDMMC_RX,
	DMACH_SPI1_TX,
	DMACH_SPI1_RX,
	DMACH_SPI2_TX,	 //GNSS
	DMACH_SPI2_RX,	//GNSSS
        DMACH_M2M_SR,
        DMACH_M2M_NSR,
	/* END Marker, also used to denote a reserved channel */
	DMACH_MAX,
};

static inline bool shikara_dma_has_circular(void)
{
	return true;
}

static inline bool shikara_dma_has_infiniteloop(void)
{
	return true;
}



#endif	/* __S3C_DMA_PL330_H_ */
