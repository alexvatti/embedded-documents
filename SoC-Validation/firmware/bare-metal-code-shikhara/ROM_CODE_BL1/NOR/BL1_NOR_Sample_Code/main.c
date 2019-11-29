/*
 * main.c
*/
#include <shikhara_uart.h>
#include<common.h>
#include<gpio-leds.h>
void c_entry( )
{
	char ch;
	unsigned int i;
	uart_init();
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	printf("BL1 from NOR is loaded/copied to on chip memory for execution\r\n");
	printf("BL1 from NOR is loaded/copied to on chip memory for execution\r\n");
        gpio_port_configure_output(0);
        gpio_port_configure_output(1);
	while(1)
	{
        //drive port 0 and 1, high
        	gpio_port_drive_high(0);
        	gpio_port_drive_high(1);
		for(i=1;i<100;i++)
        	    udelay(1000);
        //drive port 0 and 1, low
        	gpio_port_drive_low(0);
        	gpio_port_drive_low(1);
        	for(i=1;i<100;i++)
        	    udelay(1000);
    	}
	
}



