#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_spi.h>

#define SSP0             SHIKHARA_SPI_BASE0 
#define SSP1             SHIKHARA_SPI_BASE1 

// SSPCR0 Control register 0
#define SSPCR0_SCR_DFLT     0x0000      // Serial Clock Rate (divide), default set at 3
#define SSPCR0_SPH          0x0080      // SSPCLKOUT phase
#define SSPCR0_SPO          0x0040      // SSPCLKOUT polarity
#define SSPCR0_FRF_MOT      0x0000      // Frame format, Motorola
#define SSPCR0_DSS_8        0x0007      // Data packet size, 8bits

// SSPCR1 Control register 1
#define SSPCR1_MS           0x0004      // Master or Slave mode
#define SSPCR1_SSE          0x0002      // Serial port enable

// SSPSR Status register
#define SSPSR_BSY           0x0010      // Busy
#define SSPSR_RFF           0x0008      // Receive  FIFO full
#define SSPSR_RNE           0x0004      // Receive  FIFO not empty
#define SSPSR_TNF           0x0002      // Transmit FIFO not full
#define SSPSR_TFE           0x0001      // Transmit FIFO empty

#define SSPCPSR_DFLT        0x0008


int do_spislave(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int i, val;
	/*Reset both SPI's*/
	writel(0xA00000,SHIKHARA_SYS_CTRL_BASEADDR +0x4C);
	udelay(10);
	writel(0x00,SHIKHARA_SYS_CTRL_BASEADDR +0x4C);
	/*Configure CS manual for SPI 0 */
	writel(0x26,SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	//Disable serial port operation
	writel(0x0,SSP0+ SSP_CR1);
	// Set serial clock rate, Format, data size
	writel((SSPCR0_SCR_DFLT | SSPCR0_FRF_MOT | SSPCR0_DSS_8),SSP0+ SSP_CR0);
	// Clock prescale register  
	writel(SSPCPSR_DFLT,SSP0+ SSP_CPSR);

	printf("Configure SPI1.. \n");
	/*Configure SPI 1 as slave */
	writel(0x4,SSP1+ SSP_CR1);
	// Set serial clock rate, Format, data size
	writel((SSPCR0_SCR_DFLT | SSPCR0_FRF_MOT | SSPCR0_DSS_8),SSP1+ SSP_CR0);
	// Clock prescale register
	writel(SSPCPSR_DFLT,SSP1+ SSP_CPSR);
	printf("Enable Cs line.. \n");
	writel(0x22,SHIKHARA_SYS_CTRL_BASEADDR +0x214);

	for(i=0;i<10;i++){
		/**/
	    	while (readl(SSP0+SSP_SR) & SSPSR_BSY);
   		writel(i,SSP0+SSP_DR);
		while (readl(SSP1+SSP_SR) & SSPSR_BSY);
        	writel(i+10,SSP1+SSP_DR);
	/*Configure SPI 1 as slave and enable SPI operation*/
		writel((SSPCR1_MS | SSPCR1_SSE),SSP1+ SSP_CR1 );
		writel(SSPCR1_SSE,SSP0+ SSP_CR1);
    		while(readl(SSP0+ SSP_SR) & SSPSR_RFF);
    		val=readl(SSP0+SSP_DR);
		printf("SPI Master RX: 0x%X\n",val);
    		while(readl(SSP1+ SSP_SR) & SSPSR_RFF);
    		val=readl(SSP1+SSP_DR);
		printf("SPI Slave RX: 0x%X\n",val);
		writel(0x4,SSP1+ SSP_CR1 );
		writel(0x0,SSP0+ SSP_CR1 );
	}
	writel(0x26,SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	printf("SPI Slave Test: Exiting....\n ");		
	return 0;
}

U_BOOT_CMD(
		spislave, 2, 1, do_spislave,
		"SPI registers dump, give controller no",
		" "
	  );

