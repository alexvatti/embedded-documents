/*
 * Moschip Semicondutor Technology Limited 
 * Simple U-Boot driver for the PrimeCell PL010/PL011 UARTs */




#include  "serial_pl01x.h"
#undef  CONFIG_SHIKHARA_FPGA
#define   CONFIG_BAUDRATE 115200

#ifdef    CONFIG_SHIKHARA_FPGA
#define   CONFIG_PL011_CLOCK            24000000
#define   SHIKHARA_UART0_BASE           0X5D57b000
#else
#define   CONFIG_PL011_CLOCK            24000000
#define   SHIKHARA_UART0_BASE           0XD457B000
#endif

int serial_init(void) __attribute__ ((section(".boot")));
void pl01x_putc (char c);
int pl01x_getc ();
static int pl01x_tstc ();
unsigned int baudrate = CONFIG_BAUDRATE;

#define readl(a)		(*(volatile unsigned int *)(a))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))

int serial_init (void)
{ 
    
	unsigned int temp;
	unsigned int divider;
	unsigned int remainder;
	unsigned int fraction;

	/* First, disable everything */
	writel(0x0, SHIKHARA_UART0_BASE + UART_PL011_CR);

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

	writel(divider, SHIKHARA_UART0_BASE + UART_PL011_IBRD);
	writel(fraction, SHIKHARA_UART0_BASE + UART_PL011_FBRD);

	/* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
	writel((UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN),
		SHIKHARA_UART0_BASE + UART_PL011_LCRH);

	/* Finally, enable the UART */
	writel((UART_PL011_CR_UARTEN | UART_PL011_CR_TXE | UART_PL011_CR_RXE),
		SHIKHARA_UART0_BASE + UART_PL011_CR);

         return 0;
}



void serial_putc (const char c)
{
	if (c == '\n')
		pl01x_putc ('\r');

	pl01x_putc (c);
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

void print(const char *s)
{
	serial_puts(s);
}
int serial_getc (void)
{
	return pl01x_getc ();
}

int serial_tstc (void)
{
	return pl01x_tstc ();
}

void serial_setbrg (void)
{
	baudrate = 115200;
	serial_init();
}

void pl01x_putc (char c)
{
	/* Wait until there is space in the FIFO */
	while (readl(SHIKHARA_UART0_BASE + UART_PL01x_FR) & UART_PL01x_FR_TXFF);
		//WATCHDOG_RESET();

	/* Send the character */
	writel(c, SHIKHARA_UART0_BASE + UART_PL01x_DR);
}

int pl01x_getc ()
{
	unsigned int data;

	/* Wait until there is data in the FIFO */
	while (readl(SHIKHARA_UART0_BASE + UART_PL01x_FR) & UART_PL01x_FR_RXFE);
		//WATCHDOG_RESET();

	data = readl(SHIKHARA_UART0_BASE + UART_PL01x_DR);

	/* Check for an error flag */
	if (data & 0xFFFFFF00) {
		/* Clear the error */
		writel(0xFFFFFFFF, SHIKHARA_UART0_BASE + UART_PL01x_ECR);
		return -1;
	}

	return (int) data;
}

static int pl01x_tstc ()
{
	return !(readl(SHIKHARA_UART0_BASE + UART_PL01x_FR) & UART_PL01x_FR_RXFE);
}
