
#include<common.h>
#include<shikhara_map.h>
#include<shikhara_uart.h>

typedef unsigned int uint32_t;
typedef int int32_t;
 
extern int load_xmodem_code();

 void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)reg = data;
}

 uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t *)reg;
}

int strlen(const char* str)
{
	int ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

enum
{
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

void uart_init()
{
	unsigned int temp;
        unsigned int divider;
        unsigned int remainder;
        unsigned int fraction;

        temp = 16 * baudrate;
        divider = CONFIG_PL011_CLOCK / temp;
        remainder = CONFIG_PL011_CLOCK % temp;
        temp = (8 * remainder) / baudrate;
        fraction = (temp >> 1) + (temp & 1);

	// Disable UART0.
   	mmio_write(UART0_CR, 0x00000000);

	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
        //refer to baudrate_calc.c code to generate divisor and fraction value,24000000
	mmio_write(UART0_IBRD, 0x0d); //write divider value in hexa
	mmio_write(UART0_FBRD, 0x01); //write fraction value in hexa

	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

	// Mask all interrupts.
	//mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	//                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

int pl01x_putc( char c)
{
	/* Wait until there is space in the FIFO */
	if (mmio_read(UART0_FR) & UART_PL01x_FR_TXFF)
		return -EAGAIN;

	/* Send the character */
	mmio_write(UART0_DR,c);

	return 0;
}

int pl01x_getc()
{
	unsigned int data;

	/* Wait until there is data in the FIFO */
	if (mmio_read(UART0_FR) & UART_PL01x_FR_RXFE)
		return -EAGAIN;

	data = mmio_read(UART0_DR);
#if 0
	/* Check for an error flag */
	if (data & 0xFFFFFF00) {
		/* Clear the error */
		writel(0xFFFFFFFF, &regs->ecr);
		return -1;
	}
#endif

	return (int) data;
}

int pl01x_tstc()
{
	
	return !(mmio_read(UART0_FR) & UART_PL01x_FR_RXFE); //empty it will set
}


void uart_putc(unsigned char byte)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, byte);
}

unsigned char uart_getc()
{
    // Wait for UART to have recieved something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

void uart_write(const unsigned char* buffer, int size)
{
	int i;
	for ( i = 0; i < size; i++ )
		uart_putc(buffer[i]);
}

void uart_puts(const char* str)
{
	uart_write((const unsigned char*) str, strlen(str));
}

void serial_puts(const char* str)
{
	uart_write((const unsigned char*) str, strlen(str));
}


