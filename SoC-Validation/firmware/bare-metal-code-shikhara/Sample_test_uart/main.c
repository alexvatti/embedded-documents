/*
 * main.c
*/
#include <shikhara_uart.h>
void c_entry( )
{
	char ch;
	uart_init();
	uart_puts("Welcome to Hello World!\r\n");
	uart_puts("Welcome to Hello World!\r\n");
	while ( 1 ) {
		ch=uart_getc();
                uart_putc(ch);
	}
}




