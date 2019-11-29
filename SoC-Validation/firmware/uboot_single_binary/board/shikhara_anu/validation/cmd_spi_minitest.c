/*
 * Simple test case for the SSP PL-022 (Serial Peripheral Interface) validation.
 * It reads the flash ID connected at chipselect 0 on SPI Bus 0.
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>

//#define DEBUG_SPI
#ifdef DEBUG_SPI
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif


#define SSP_BASE             SHIKHARA_SPI_BASE0  // SSP Prime Cell
/* SPI Reset Register*/
#define SOFT_RST_REG2_ADDR  (SHIKHARA_SYS_CTRL_BASEADDR + 0x04C)

#define SSPCR0              (SSP_BASE + 0x00)
#define SSPCR1              (SSP_BASE + 0x04)
#define SSPDR               (SSP_BASE + 0x08)
#define SSPSR               (SSP_BASE + 0x0C)
#define SSPCPSR             (SSP_BASE + 0x10)
#define SSPIMSC             (SSP_BASE + 0x14)
#define SSPRIS              (SSP_BASE + 0x18)
#define SSPMIS              (SSP_BASE + 0x1C)
#define SSPICR              (SSP_BASE + 0x20)
#define SSPDMACR            (SSP_BASE + 0x24)

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

// SSPCPSR Clock prescale register
#define SSPCPSR_DFLT        0x0008      // Clock prescale (use with SCR), default set at 8
#define SSPMAXTIME          1000 
#define READ_FLASH_ID               0x9f

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

// Wait for SSP Read/Write FIFO to clear
// returns FALSE if SSP time out
int apSSP_Wait()
{
	int waittimer;

	waittimer = SSPMAXTIME;

	while ((readl(SSPSR) & SSPSR_BSY) && waittimer) {
		udelay(100);
		waittimer--;
	}

	if (waittimer)
		return TRUE;
	else
		return FALSE;
}

int apSSP_flashID(){

	unsigned int val,i;
	unsigned char *temp;

	temp= &val;
	/*Enable CS line 3rd bit low */ 
	writel(0x22,SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	while (readl(SSPSR) & SSPSR_BSY);
	writel(READ_FLASH_ID,SSPDR);
	while(readl(SSPSR) & SSPSR_RFF);
	val=readl(SSPDR);
	for(i=0; i<=3;i++){ 
		/*Write dummy to get ID*/ 
		while (!(readl(SSPSR) &SSPSR_TFE));
		writel(0x00,SSPDR); 
		/*Read Data*/
		while(readl(SSPSR) & SSPSR_RFF);
		//printf("Status: %X\n", *SSPSR);
		*temp++ = readl(SSPDR);
	}
	/*disable CS line 3rd bit high */ 
	writel(0x26,SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	printf("Data Flash ID is 0x%X\n",SWAP_UINT32(val));
	if(SWAP_UINT32(val)==0x202018)
	{
		printf("spi mintest succesfully completed\n");
		return 0;
	}else
	{
		printf("spi mintest failed\n");
		return -1;
	}

}

int spimintest_init()
{
	writel(0x26,SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	//Disable serial port operation
	writel(0x0,SSPCR1);
	// Set serial clock rate, Format, data size
	writel( (SSPCR0_SCR_DFLT | SSPCR0_FRF_MOT | SSPCR0_DSS_8),SSPCR0);
	// Clock prescale register set to /8, with SCR gives 0.78MHz SCLK
	writel(SSPCPSR_DFLT,SSPCPSR);
	// Mask all FIFO/IRQ interrupts appart from the Tx FIFO empty interrupt
	//*SSPIMSC = 0x8;

	// Disable FIFO DMA
	writel(0x0,SSPDMACR) ;
	// Enable serial port operation
	writel(SSPCR1_SSE,SSPCR1);
	return 0;

}

int spi_mintest()
{
        int ret;
        spimintest_init();
        /* Read Data Flash ID */
        ret=apSSP_flashID();
        return ret;

}

int spi_post()
{
      return spi_mintest();
}

int do_spi_flashid(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	spimintest_init();  
	/* Read Data Flash ID */
	apSSP_flashID();
	return 0;
}

U_BOOT_CMD(
		spi_flashid, 1, 1, do_spi_flashid,
		" Simple SPI testcase to read Flash id",
		" Reads flash id connected on CS-0 SPI Bus-0"
	  );
