
#include<common.h>
#include<shikhara_map.h>

static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}

void udelay(unsigned long us)
{
	delay(us * 200); /* approximate */
}

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



