/*
 * main.c
*/
#include<gpio-leds.h>

int c_entry( )
{
    unsigned int i;
    gpio_port_configure_output(0);
    gpio_port_configure_output(1);

    while(1)
    {
        //drive port 0 and 1, high
        gpio_port_drive_high(0);
        gpio_port_drive_high(1);
	for(i=1;i<=100;i++)
        	udelay(1000);
        //drive port 0 and 1, low
        gpio_port_drive_low(0);
        gpio_port_drive_low(1);
        for(i=1;i<=100;i++)
        	udelay(1000);
    }
    return 0;
}


