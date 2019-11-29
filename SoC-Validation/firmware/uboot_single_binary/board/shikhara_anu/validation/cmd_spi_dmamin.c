/* Simple DMA to Peripheral test, Used to transfer data to SPI0, connected in loopback mode and recieve same data back.
 */


#include <asm/io.h>
#include <linux/string.h>
#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/dma.h>
/*Register Def*/
#define SSP_BASE             SHIKHARA_SPI_BASE0  // SSP Prime Cell

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
#define SSPCR0_DSS_16       0x000F      // Data packet size, 16bits

// SSPCR1 Control register 1
#define SSPCR1_SSE          0x0002      // Serial port enable
#define SSPCR1_LBM          0x0001      // Loop Back Mode

// SSPCPSR Clock prescale register
#define SSPCPSR_DFLT        0x0008      // Clock prescale (use with SCR), default set at 8

// SSPDMACR DMA control register
#define SSPDMACR_TXDMAE     0x0002      // Enable Transmit FIFO DMA
#define SSPDMACR_RXDMAE     0x0001      // Enable Receive  FIFO DMA

#define TX_DMA_IF   20
#define RX_DMA_IF   21
#define TX_RX_DMA_RIF    0x300000
#define RX_DMA_RIF       0x200000
#define TX_DMA_RIF       0x100000

#define TX_BUF		 0x4c000000
#define RX_BUF		 0x4d000000

#define DEBUG_DMAC
#ifdef DEBUG_DMAC
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

static unsigned int dma_code_tx[] = {
	0x871501BC,
	0x00BC01CC,
	0x4C000000,
	0x200802BC,
	0xA0355D57,
	0xA0322020,
	0x13A02B07,
	0xA034063B,
	0xFFFFFF00,
	0xFFFFFFFF,
};

static unsigned int dma_code_rx[] = {
	0x473201BC,
	0x00BC01C5,
	0x5D572008,
	0x000002BC,
	0xA8354D00,
	0xA8321F20,
	0x13A82B07,
	0xA834063B,
	0xFFFFFF00,
	0xFFFFFFFF,
};

#define TX_RX_RIF	0x300000 

int do_spi_dmamin(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int i,errors=0, st=0, temp, es;
	volatile unsigned int *mc_tx, *mc_rx;
	unsigned char *add = (volatile unsigned int *) 0x4c000000;
	unsigned char *add2;
	/*Fill the destination buffer (Rx buffer) with zeros*/
	for (i=0;i<66;i++)
	{
		*add = 'A'+i;
		add = add + 1;
	}
	memset(0x4D000000, 0x0, 100);   
	mc_tx= (volatile unsigned int *)0x40400000;

	mc_rx= (volatile unsigned int *)0x40500000;

	/*Copy the microcode for both channels.*/
	memcpy(mc_tx, dma_code_tx,  40);
	memcpy(mc_rx, dma_code_rx,  40);
	/*Reset the DMA-0 for both CH 0 and CH1*/
	writel(0x3,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	udelay(10); 
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	/*Reset SPI*/
	writel(0x300000,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);
	udelay(10);
	writel(0x00,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);

	/*Configure SPI */
	//	 writel(0x26,SHIKHARA_SYS_CTRL_BASEADDR +0x214);

	/*Disable serial port operation */
	writel(0x0,SSPCR1);
	/* Set 16 bit mode */
	writel(SSPCR0_DSS_16,SSPCR0);
	/* Clock prescale register set to /8 */
	writel(SSPCPSR_DFLT,SSPCPSR);
	/* Disable FIFO DMA */
	writel(0x0,SSPDMACR) ;
	/* Enable Loop Back Mode and SPI Operation*/	
	writel((SSPCR1_LBM | SSPCR1_SSE),SSPCR1);

	/* Temp code to test mc code*/
	/*Configure DMA for TX for channel 0*/
	printf("Start configure DMA for TX\n");
	writel(TX_RX_RIF,NSDMAC0_INTEN) ;
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 0*/
	writel(0xa20000,NSDMAC0_DBGINST0);
	writel(mc_tx, NSDMAC0_DBGINST1 );
	writel(0x00,NSDMAC0_DBGCMD);
	/*Configure DMA for RX  for channel 1*/

	printf("Start configure DMA for RX\n");
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 1*/
	writel(0x1a20100,NSDMAC0_DBGINST0);
	writel(mc_rx,NSDMAC0_DBGINST1);
	writel(0x00,NSDMAC0_DBGCMD);

	printf("Start DMA request from UART\n");
	writel((SSPDMACR_TXDMAE | SSPDMACR_RXDMAE), SSPDMACR );

	dbg("Wait for DMAC to complete\n"); 

	errors = 0;


 	while (1){
                st=readl(NSDMAC0_INTSTATUS);
                es = readl(NSDMAC0_ES)& TX_RX_DMA_RIF ;
                if(es == TX_RX_DMA_RIF){ // Poll for interrupts
                        printf(" TX and RX Interrupts Generated Successfully\n");
                        break;
                }
                else if(es == TX_DMA_RIF ){
                        dbg("TX Done.. Wait for RX Done..\n");
                        writel(es,NSDMAC0_INTCLR);
                }
                else if (es == RX_DMA_RIF){
                        dbg("RX Done.. Break the loop\n");
                        writel(es,NSDMAC0_INTCLR);
                        break;
                }
                errors +=1;
                if(errors == 1000000){
                        printf("Errors too manay.. Interrupt not generated \n");
                        break;
                }
        }

	if(errors < 1000000){
		writel(es,NSDMAC0_INTCLR); // Clear interrupts	
	}
	else
	{
		printf("Interrupt status = 0x%X  Event Status = 0x%X and error %d\n",st,es,errors);
		printf("Faltmanager reg %X\t Falt Channer reg %X\n Falt status %X Falt channel0 status 0x%X\n",readl(NSDMAC0_FSRD),readl(NSDMAC0_FSRC),readl(NSDMAC0_FTRD),readl(NSDMAC0_FTCH0));
		printf("Channel 0 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC0), readl(NSDMAC0_CHST0));
		printf("Fault Channel 1 status 0x%X\n",readl(NSDMAC0_FTCH1));
		printf("Channel 1 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC1), readl(NSDMAC0_CHST1));
		printf("Channel0  Loop Count  status 0x%X\n",readl(NSDMAC0_LPCH0));
		printf("Channel1  Loop Count  status 0x%X\n",readl(NSDMAC0_LPCH1));
		printf("Channel0  SAR   0x%X\n",readl(NSDMAC0_SARCH0));
		printf("Channel0  DAR   0x%X\n",readl(NSDMAC0_DARCH0));
		printf("Channel1  SAR   0x%X\n",readl(NSDMAC0_SARCH1));
		printf("Channel1  DAR   0x%X\n",readl(NSDMAC0_DARCH1));
	}
	dbg("Interrupt status = 0x%X  Event Status = 0x%X and error %d\n",st,es,errors);
	dbg(" Manager status 0x%X\n ",readl(NSDMAC0_MNGST));
	dbg("Faltmanager reg %X\t Falt Channer reg %X\n Falt status %X Falt channel0 status 0x%X\n",readl(NSDMAC0_FSRD),readl(NSDMAC0_FSRC),readl(NSDMAC0_FTRD),readl(NSDMAC0_FTCH0));
	dbg("Channel 0 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC0), readl(NSDMAC0_CHST0));
	dbg("Fault Channel 1 status 0x%X\n",readl(NSDMAC0_FTCH1)); 
	dbg("Channel 1 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC1), readl(NSDMAC0_CHST1));
	dbg("Channel0  Loop Count  status 0x%X\n",readl(NSDMAC0_LPCH0)); 
	dbg("Channel1  Loop Count  status 0x%X\n",readl(NSDMAC0_LPCH1)); 

	dbg("Channel0  SAR   0x%X\n",readl(NSDMAC0_SARCH0));
	dbg("Channel0  DAR   0x%X\n",readl(NSDMAC0_DARCH0));
	dbg("Channel1  SAR   0x%X\n",readl(NSDMAC0_SARCH1));
	dbg("Channel1  DAR   0x%X\n",readl(NSDMAC0_DARCH1));

	add = TX_BUF;
	add2 = RX_BUF;
        for(i=0;i<66;i++)
        {
                if(*add != *add2)
                {
                        printf("Read Value from uart is 0x%X, But it should be 0x%X\n",*add,*add2);
			printf("FAILED: Non-Secure DMA SPI0 LOOP BACK test\n");
                        return 0;
                }
                add++;
                add2++;
        }

	printf("PASSED: Non-Secure DMA SPI0 LOOP BACK test\n");

	return 0;
}

U_BOOT_CMD(
		spi_loopdma, 2,  1, do_spi_dmamin,
		" Loopback testing of SPI 0 with DMA for both Rx and Tx",
		""
	  );


