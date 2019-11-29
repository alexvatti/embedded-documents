/*
 * main.c
*/
#include <shikhara_uart.h>
#include<common.h>

void c_entry( )
{
	char ch;
	uart_init();
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	while ( 1 ) {
		ch=uart_getc();
                uart_putc(ch);
	}
}




