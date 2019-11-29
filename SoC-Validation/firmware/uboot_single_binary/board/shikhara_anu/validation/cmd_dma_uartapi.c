
#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/list.h>
#include <asm/arch/dma/shikara-dma-pl330.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/serial_pl01x.h>
#include <asm/arch/shikhara_map.h>

static int dmatest_func(void);

#define CONFIG_PL01x_PORTS     {(void *)SHIKHARA_UART0_BASE,(void *)SHIKHARA_UART1_BASE,(void *)SHIKHARA_UART2_BASE,(void *)SHIKHARA_UART3_BASE}

static volatile unsigned char *const port[] = CONFIG_PL01x_PORTS;

static unsigned int baudrate = CONFIG_BAUDRATE;

#define NUM_PORTS (sizeof(port)/sizeof(port[0]))

unsigned int xfer_size = 16;
unsigned short burst = 1;	/* 1 byte at a time*/
bool count=0;
typedef struct {
 	unsigned tx_id;  /* For Channel index */
	unsigned rx_id;
	enum shikara_dma_buffresult res;
	int size;
	struct shikara_dma_client cl;
	unsigned int *data;
} uart_dma;
uart_dma *uart1_dma;
static u32 *srcbuf;
void shikara_dma_cb(enum dma_ch *chan, void *buf_id,
			int size, enum shikara_dma_buffresult res)
{
	#ifdef CONFIG_PL330_INTERRUPT_MODE
	unsigned int temp;
	unsigned char *addr;
        /*Check the result */
        if (res != SHIKARA_RES_OK || size != xfer_size) {
 	       printf("SHIKARA DMA : xfer is not successful  Res-%u Xfer_size-%d!\n", res, size);
               return -1;
           }

                addr = (unsigned char*)srcbuf;
                /* Check Transaction is successful or not */
           while(readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_RXFE);
           while(!(readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_RXFE)){
                    temp =readl(port[1] + UART_PL01x_DR);
                    if(temp != *addr)
                    {
                            printf("Read Value from UART is 0x%X, But it should be 0x%X\n",temp,*addr);
                            return 1;
                    }
                    addr++;
           }
	#else
	uart1_dma->res = res;
	uart1_dma->size = size;
	printf("SHIKARA DMA TEST : callback to cmd_dma res= %d \n",res);
	count= 1;
	#endif
	
}


static int shikhara_serial_init (void)
{
        unsigned int temp;
        unsigned int divider;
        unsigned int remainder;
        unsigned int fraction;

        /* First, disable everything */
        writel(0x0, port[1] + UART_PL011_CR);

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

        writel(divider, port[1] + UART_PL011_IBRD);
        writel(fraction, port[1] + UART_PL011_FBRD);

        /* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
        writel((UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN),
                port[1] + UART_PL011_LCRH);

        /* Finally, enable the UART */
        writel((UART_PL011_CR_UARTEN | UART_PL011_CR_TXE | UART_PL011_CR_RXE),
                port[1] + UART_PL011_CR);
        return 0;
}



static void init_buf(u32 buf[], int clr, unsigned int bytes)
{
	unsigned int i;

	for (i = 0; i < bytes/4 ; i++)
		buf[i] = clr ? 0 : 0x12345678;
}

static int dmatest_func()
{
	int ret = 0, temp,error=0; 
	unsigned char *addr;
        srcbuf = (unsigned int *)malloc(16);
	init_buf(srcbuf,0,16);
	ret = shikara_dma_devconfig(uart1_dma->tx_id, SHIKARA_DMASRC_MEM,(unsigned long) port[1] + UART_PL01x_DR);
		if(ret)
			{
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
			}


		/*Generate UART DMAC request interface */
    		writel(0x3, port[1] + UART_PL01x_DMAC);

		ret = shikara_dma_enqueue_ring(uart1_dma->tx_id, (void *)uart1_dma,
				(u32)srcbuf, xfer_size,0);
		if(ret){
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
			}

		if(!count)
		printf("SHIKARA DMA : Interrupt time out \n");
		count=0;

	//	shikara_dma_getposition(uart1_dma->tx_id, srcbuf, (unsigned int *)port[1] + UART_PL01x_DR);
		#ifndef CONFIG_PL330_INTERRUPT_MODE
		/*Check the result */			
		if (uart1_dma->res != SHIKARA_RES_OK
				|| uart1_dma->size != xfer_size) {
			printf("SHIKARA DMA : xfer is not successful  Res-%u Xfer_size-%d!\n", uart1_dma->res, uart1_dma->size);
			 return -1;
		}
		
		addr = (unsigned char*)srcbuf;
		/* Check Transaction is successful or not */
		while(readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_RXFE);
    		while(!(readl(port[1] + UART_PL01x_FR) & UART_PL01x_FR_RXFE)){
       			temp =readl(port[1] + UART_PL01x_DR);
			if(temp != *addr)
			{
	        		printf("Read Value from UART is 0x%X, But it should be 0x%X\n",temp,*addr);
				error++;
			}
			addr++;
		}
		shikara_dma_free(uart1_dma->tx_id, &uart1_dma->cl); //moon
		#endif	
  return error;
  
}
int do_dma_api_uart(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;

	uart1_dma = (uart_dma *)malloc(sizeof(uart_dma));
	uart1_dma->cl.name ="uart 1";
	uart1_dma->tx_id= DMACH_UART1_TX ;
	uart1_dma->rx_id= DMACH_UART1_RX;	
	
	/*Initialise UART*/
	shikhara_serial_init();

	/*Initialise DMAC*/
	if(dma_pl330_init(1)){
             printf("DMAC not initialised, exiting from DMA Test\n ");
	     return 0;
         }	

	ret = shikara_dma_request(uart1_dma->tx_id,&uart1_dma->cl,NULL);
	if (ret) {
		printf("Shikhara DMA: Error in acq %d\n", ret);
		goto thrd_dma_acq_err;
	}
	shikara_dma_set_buffdone_fn(uart1_dma->tx_id, shikara_dma_cb);
	ret = shikara_dma_config(uart1_dma->tx_id, burst);
	if (ret) {
		printf("Shikhara DMA Test: Error in config(%d)\n", ret);
		goto thrd_dma_cfg_err;
	}
	ret= dmatest_func();
	if(!ret)
	goto thread_exit;	
thrd_dma_cfg_err:
	shikara_dma_free(uart1_dma->tx_id, &uart1_dma->cl);
thrd_dma_acq_err:
	printf("SHIKHARA DMA API test with UART1 loopback is Un successful\n");
	return 0;
thread_exit : 
			
	printf("SHIKARA DMA API test with UART1 Loopback is successful\n" );;
	return 0;
}

U_BOOT_CMD(
	uartdma_api, 1, 1, do_dma_api_uart,
	"DMAC 0 Test with API using UART1 Loopback",
	"Connect UART 1 with Loopback"
	);

