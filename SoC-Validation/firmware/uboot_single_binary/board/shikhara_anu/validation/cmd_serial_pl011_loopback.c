#include <asm/io.h>
#include <linux/string.h>
#include <common.h>
#include <command.h>
#include <asm/arch/serial_pl01x.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_irq.h>
#include <watchdog.h>
#include <asm/arch/shikhara_map.h>

#define HASH_ASCII 0x23

#define DEBUG_UART
#ifdef DEBUG_UART
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#define CONFIG_PL01x_PORTS     {(void *)SHIKHARA_UART0_BASE,(void *)SHIKHARA_UART1_BASE,(void *)SHIKHARA_UART2_BASE,(void *)SHIKHARA_UART3_BASE}

static volatile unsigned char *const port[] = CONFIG_PL01x_PORTS;

static unsigned int baudrate = CONFIG_BAUDRATE;

#define NUM_PORTS (sizeof(port)/sizeof(port[0]))

char serial_rx;
char loopback_connector_flag=0;

int shikhara_serial_init (void)
{
        unsigned int temp;
        unsigned int divider;
        unsigned int remainder;
        unsigned int fraction;
	int i;
	
	for(i=1;i<=3;i++)
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

        /* Finally, enable the UART */
        writel((UART_PL011_CR_UARTEN | UART_PL011_CR_TXE | UART_PL011_CR_RXE),
                port[i] + UART_PL011_CR);
        }

        return 0;
}

void shikhara_pl01x_putc (int portnum, char c)
{
        /* Wait until there is space in the FIFO */
        while (readl(port[portnum] + UART_PL01x_FR) & UART_PL01x_FR_TXFF)
 	WATCHDOG_RESET();

        /* Send the character */
        writel(c, port[portnum] + UART_PL01x_DR);
}

int shikhara_pl01x_getc (int portnum)
{
        unsigned int data;

        /* Wait until there is data in the FIFO */
        while (readl(port[portnum] + UART_PL01x_FR) & UART_PL01x_FR_RXFE)
	WATCHDOG_RESET();

        data = readl(port[portnum] + UART_PL01x_DR);

        /* Check for an error flag */
        if (data & 0xFFFFFF00) {
                /* Clear the error */
                writel(0xFFFFFFFF, port[portnum] + UART_PL01x_ECR);
                return -1;
        }

        return (int) data;
}

int uart_post(void)
{
     	 int d;
   	 shikhara_serial_init();
         loopback_connector_flag =1;
  	 for(d=1;d<=3;d++)
{
           shikhara_pl01x_putc(d,HASH_ASCII);
           serial_rx=shikhara_pl01x_getc(d);
           if(serial_rx != HASH_ASCII)
           loopback_connector_flag=0;
}
           if(loopback_connector_flag){
           dbg("the loopback test is pass for all UART's\n");
	   return 0;
	 }
	   else{
	   dbg("loopback test is fail\n");
	   return -1;
	 }

	return 0;
}
   


int do_loopback_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
   if(simple_strtoul(argv[1],NULL,10) < 4)
     {	    
          shikhara_serial_init();//initializing uart0,uart1,uart2,uart3
     	  shikhara_pl01x_putc(simple_strtoul(argv[1],NULL,10),HASH_ASCII);
     	  serial_rx=shikhara_pl01x_getc(simple_strtoul(argv[1],NULL,10));
     	  if(serial_rx == HASH_ASCII)
          loopback_connector_flag = 1;

           if(loopback_connector_flag)
           dbg("The loopback test is pass for %d UART\n",simple_strtoul(argv[1],NULL,10));
	   else
	   dbg("The loopback test is fail for %d UART\n",simple_strtoul(argv[1],NULL,10));
      }
      else
      {
	uart_post();	
       }
     return(0);
} 

U_BOOT_CMD(
        uartloopback_test, 2,  1, do_loopback_test,
        "Type uartloopback_test 1 for Loopback testing of uart1\n"
        "Type uartloopback_test 2 for Loopback testing of uart2\n"
        "Type uartloopback_test 3 for Loopback testing of uart3\n",
	    "compare tx data and rx data of the uart if the result is same produces PASS"
);


