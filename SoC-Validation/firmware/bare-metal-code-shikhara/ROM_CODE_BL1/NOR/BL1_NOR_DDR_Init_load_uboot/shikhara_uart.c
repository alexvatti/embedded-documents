#include<shikhara_map.h>
#include<shikhara_uart.h>
#include<common.h>

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

void uart_init(void)
{
	// Disable UART0.
   	writel(0x00000000,UART0_CR);

	// Clear pending interrupts.
	writel(0x7FF,UART0_ICR);

        //refer to baudrate_calc.c code to generate divisor and fraction value,24000000
	writel(0x0d,UART0_IBRD); //write divider value in hexa
	writel(0x01,UART0_FBRD); //write fraction value in hexa

	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	writel(UART_PL011_LCRH_FEN | UART_PL011_LCRH_WLEN_8, UART0_LCRH);

	// Enable UART0, receive & transfer enable of UART.
	writel(UART_PL011_CR_UARTEN  | UART_PL011_CR_TXE | UART_PL011_CR_RXE,UART0_CR);
}

void uart_putc(unsigned char byte)
{
	// Wait for UART to become ready to transmit.
	while ( readl(UART0_FR) & UART_PL01x_FR_TXFF ) { }
	writel(byte,UART0_DR);
}

unsigned char uart_getc()
{
    // Wait for UART to have recieved something.
    while ( readl(UART0_FR) & UART_PL01x_FR_RXFE ) { }
    return readl(UART0_DR);
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

