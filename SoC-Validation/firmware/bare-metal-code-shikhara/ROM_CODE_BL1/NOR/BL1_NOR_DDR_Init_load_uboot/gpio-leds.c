
#include<common.h>
#include<shikhara_map.h>


/* Input port 0 , SHIKHARA_GPIO_BANK_A
   Input port 1 , SHIKHARA_GPIO_BANK_B */
int gpio_port_configure_output(int port)
{
    if(port == 0)
        writel(0xFF,SHIKHARA_GPIO_BANK_A + PL061_GPIODIR);  // port direction is output
    if (port == 1)
        writel(0xFF,SHIKHARA_GPIO_BANK_B + PL061_GPIODIR);  // port direction is output
    
    return 0;
}
/* Input port 0 , SHIKHARA_GPIO_BANK_A
   Input port 1 , SHIKHARA_GPIO_BANK_B */
int gpio_port_drive_high(int port)
{
    unsigned char value=1;
    int pin=0;

    if(port == 0) {
       	for(pin=0;pin<8;pin++)
	    writeb(value << pin, SHIKHARA_GPIO_BANK_A + (1 << (pin + 2)));
    }
    if(port == 1){
       	for(pin=0;pin<8;pin++)
	    writeb(value << pin, SHIKHARA_GPIO_BANK_B + (1 << (pin + 2)));
    }
    return 0;
}
/* Input port 0 , SHIKHARA_GPIO_BANK_A
   Input port 1 , SHIKHARA_GPIO_BANK_B */
int gpio_port_drive_low(int port)
{
    unsigned char value=0;
    int pin=0;

    if(port == 0) {
       	for(pin=0;pin<8;pin++)
	    writeb(value << pin, SHIKHARA_GPIO_BANK_A + (1 << (pin + 2)));
    }
    if(port == 1){
       	for(pin=0;pin<8;pin++)
	    writeb(value << pin, SHIKHARA_GPIO_BANK_B + (1 << (pin + 2)));
    }
    return 0;
}



