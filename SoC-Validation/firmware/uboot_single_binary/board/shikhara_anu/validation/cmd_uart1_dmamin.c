/* Simple DMA to Peripheral test, Used to transfer data to UART 1 TX FIFO
   Uart-1 should be connected in Loopback mode. Transfers 16 bytes of data to TX FIFO and read back.
 */

#include <asm/io.h>
#include "../../../include/malloc.h"
#include <linux/string.h>
#include <common.h>
#include <command.h>
#include <asm/arch/serial_pl01x.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_irq.h>
#include <watchdog.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/dma.h>

#define XFER_SIZE       16*4
#define MCODE_SIZE      40

#define DEBUG_DMAC
#define ONLY_RX_DMA
#ifdef DEBUG_DMAC
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#ifndef ONLY_RX_DMA
static unsigned int dma_code_tx[] = {
/*
;SA burst size 32 bits , SA burst length 1
; DA burst size 8 bits, DA burst length 4

DMAMOV CCR SP7 DP7 SB1 SS32 DAF DB4 DS8
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 18 is Peripheral number
DMAFLUSHP P18
; Specify no of counts, each count transfers 4-bytes
DMALP 18
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;               peripheral signals a DMA request for that DMA channel. 18 is Peripheral number
DMAWFP 18, burst
DMALDB
; 18 is Peripheral number
DMASTPB P18
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 18 is event number
DMASEV E18
DMAEND
*/
0x070501BC,
0x00BC01CC,
0x40000000,
0x000002BC,
0x90355F00,
0x90321120,
0x13902B07,
0x9034063B,
0xFFFFFF00,
0xFFFFFFFF,
};
#endif

static unsigned int dma_code_rx [] = {
/*
;A burst size 8 bits , SA burst length 4
; DA burst size 32 bits, DA burst length 1

DMAMOV CCR SP7 DP7 SAF SB4 SS8 DB1 DS32
DMAMOV   SAR, 0x40000000  ;0x40000000 is Source Address, This need to change.
DMAMOV   DAR, 0x5F000000  ;0x5F000000 is Destination Address, This need to change.
; Flush Peripheral clears the state in the DMAC. 19 is Peripheral number
DMAFLUSHP P19
; Specify no of counts, each count transfers 4-bytes
DMALP 20
; Wait For Peripheral instructs the DMAC to halt execution of the thread until the specified
;               peripheral signals a DMA request for that DMA channel. 19 is Peripheral number
DMAWFP 19, burst
DMALDB
; 19 is Peripheral number
DMASTPB P19
DMAWMB
DMALPENDB
;Send Event instructs the DMAC to modify an event-interrupt resource. 19 is event number
DMASEV E19
DMAEND
*/
0x473001BC,
0x00BC01C1,
0x40000000,
0x000002BC,
0x98355F00,
0x98321320,
0x13982B07,
0x9834063B,
0xFFFFFF00,
0xFFFFFFFF,
};

#define DMA_CODE_L	12

#define CONFIG_PL01x_PORTS     {(void *)SHIKHARA_UART0_BASE,(void *)SHIKHARA_UART1_BASE,(void *)SHIKHARA_UART2_BASE,(void *)SHIKHARA_UART3_BASE}

static volatile unsigned char *const port[] = CONFIG_PL01x_PORTS;

static unsigned int baudrate = CONFIG_BAUDRATE;

int dual_dma_uart1(void);

#define NUM_PORTS (sizeof(port)/sizeof(port[0]))


static int shikhara_serial_init (void)
{
	unsigned int temp;
	unsigned int divider;
	unsigned int remainder;
	unsigned int fraction;
	int i;
	/* Reset UART 1*/
	writel(0xC,SHIKHARA_SYS_CTRL_BASEADDR+ 0x4c);	
	udelay(10);
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR+ 0x4c);	
	for(i=0;i<=3;i++)
	{
		/* First, disable everything */
		writel(0x0, port[i] + UART_PL011_CR);

		/*
		 * Set baud rate
		 *
		 * IBRD = UART_CLK / (16 * BAUD_RATE)
		 * FBRD = RND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) / (16 * BAUD_RATE))
		 */
		temp = 16 * baudrate;
		divider = CONFIG_PL011_CLOCK / temp;
		remainder = CONFIG_PL011_CLOCK % temp;
		temp = (8 * remainder) / baudrate;
		fraction = (temp >> 1) + (temp & 1);

		writel(divider, port[i] + UART_PL011_IBRD);
		writel(fraction, port[i] + UART_PL011_FBRD);

		/* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
		writel((UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN),
				port[i] + UART_PL011_LCRH);
		//	writel(0x02,0x5d57c034);
		/* Finally, enable the UART */
		writel((UART_PL011_CR_UARTEN | UART_PL011_CR_TXE | UART_PL011_CR_RXE),
				port[i] + UART_PL011_CR);
	}

	return 0;
}

int dual_dma_uart1()
{
	unsigned int i,errors=0, st=0,lp_cnt;
	volatile unsigned int *add;
	u32  *mc_rx;
	#ifndef ONLY_RX_DMA
	u32 *mc_tx;
	#endif
        unsigned char *txbuf, *rxbuf;

        txbuf = (unsigned char*)malloc(XFER_SIZE);
        rxbuf = (unsigned char*)malloc(XFER_SIZE);
        printf("TX_BUFF = 0x%x, RX_BUFF = 0x%x", (u32)txbuf, (u32)rxbuf);

        /*Fill 16 bytes of data with some known values as fixed location in DDR*/
        add = (u32*)txbuf;
	for(i=0;i<4; i++,add++)//;
		*add = 0x1234 ;
 
	/*Fill the destination buffer (Rx buffer) with zeros*/
	memset(rxbuf, 0x0, XFER_SIZE);   

#ifndef ONLY_RX_DMA
	mc_tx= malloc(MCODE_SIZE);
#endif

	mc_rx= (u32*)malloc(MCODE_SIZE);

	/*Copy the microcode for both channels.*/
#ifndef ONLY_RX_DMA
	memcpy(mc_tx, dma_code_tx,  MCODE_SIZE);
#endif
	memcpy((unsigned int*)mc_rx, dma_code_rx,  MCODE_SIZE);

	/*Reset the DMA-0 for both CH 0 and CH1*/
	writel(0x3,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	udelay(10); 
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);

	/* Temp code to test mc code*/
	lp_cnt = 4; /*No. of words to DMA*/
#ifndef ONLY_RX_DMA
	*(mc_tx+2) = txbuf;
	*(mc_tx+3) = ((*(mc_tx+3) & 0xffff)     | (((unsigned int) (port[1] + UART_PL01x_DR)& 0xffff ) << 16));
	*(mc_tx+4) = ((*(mc_tx+4) & 0xffff0000) | (((unsigned int)(port[1] + UART_PL01x_DR) & 0xffff0000 ) >> 16));
#endif
	*(mc_rx+2) = (unsigned int )(port[1] + UART_PL01x_DR);
	*(mc_rx+3) = ((*(mc_rx+3) & 0xffff)     | (((int)rxbuf & 0xffff ) << 16));
	*(mc_rx+4) = ((*(mc_rx+4) & 0xffff0000) | (((int)rxbuf & 0xffff0000 ) >> 16));
#ifndef ONLY_RX_DMA
	/*Change the lp count value in microcode*/
	*(mc_tx+5) = ((*(mc_tx+5) & 0xffff00ff) | (lp_cnt << 8));
#endif

	*(mc_rx+5) = ((*(mc_rx+5) & 0xffff00ff) | (lp_cnt << 8));

#ifndef ONLY_RX_DMA
	/*Configure DMA for TX for channel 0*/
	printf("Start configure DMA for TX\n");
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 0*/
	writel(0xa20000,NSDMAC0_DBGINST0);
	writel( mc_tx, NSDMAC0_DBGINST1 );
	//   writel(TX_REQNO,NSDMAC0_INTEN) ;
	writel(0x2,NSDMAC0_INTEN) ;
	writel(0x00,NSDMAC0_DBGCMD);
#endif
	/*Configure DMA for RX  for channel 1*/

	printf("Start configure DMA for RX\n");
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 1*/
	writel(0x1a20100,NSDMAC0_DBGINST0);
	writel((u32)mc_rx,NSDMAC0_DBGINST1 );
	writel((u32)0x4,NSDMAC0_INTEN) ;
	writel(0x00,NSDMAC0_DBGCMD);

               
        while (readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_TXFF)
        WATCHDOG_RESET();
	printf("Start DMA request from UART\n");
	/*Generate UART DMAC request interface */    
#ifndef ONLY_RX_DMA
	writel(0x3, port[1] + UART_PL01x_DMAC);
#else
	writel(0x1, port[1] + UART_PL01x_DMAC);
#endif

#ifdef ONLY_RX_DMA   
	{
		int i;
	        while (readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_TXFF)
	        WATCHDOG_RESET();
		for (i=0; i < 32 ; i++)
		{
			writel(('A' +i), port[1] + UART_PL01x_DR);
		}
	}
#endif
	dbg("Wait for DMAC to complete\n"); 
	errors = 0;
	while (1)
	{ 
		st=readl(NSDMAC0_INTSTATUS)|| readl(NSDMAC0_ES);
		if(st) // Poll for interrupts
			break;
		errors +=1;
		if(errors == 10000)
			break;
	}
	dbg("Interrupt generated with status value= 0x%X  and error %d\n",st,errors);
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

	for(i=0;i<8;i++)
	{
		#ifndef ONLY_RX_DMA
		if(*txbuf != *rxbuf)
                {
                	printf("Read Value from UART is 0x%X, But it should be 0x%X\n",*rxbuf,*txbuf);
			printf("FAILED: Non-Secure DMA  UART LOOP BACK test with Tx and Rx\n");
			return 0;
                }
		txbuf++;
		#else
                if(('A'+ i) != *rxbuf)
                {
                        printf("Read Value from UART is 0x%X, But it should be 0x%X\n",*rxbuf,('A'+ i));
                        printf("FAILED: Non-Secure DMA  UART LOOP BACK test with only Rx\n");
			return 0;
                }
		#endif
		
                rxbuf++;
	}

	#ifndef ONLY_RX_DMA
	printf("PASSED: Non-Secure DMA UART LOOP BACK test with Tx and Rx\n");
	#else
	printf("PASSED: Non-Secure DMA UART LOOP BACK test with only Rx\n");
	#endif
	return 0;
}

int do_loopdualdma(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{ 
	shikhara_serial_init();//initializing uart0,uart1,uart2,uart3
	dual_dma_uart1();

	return(0);
} 

U_BOOT_CMD(
		dualuartdma, 2,  1, do_loopdualdma,
		"Type dualuartdma  for Loopback testing of uart1 with DMA for both Rx and Tx",
		""
	  );


