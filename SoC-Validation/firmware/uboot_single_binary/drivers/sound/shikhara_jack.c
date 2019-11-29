#include <asm/arch/shikhara_max98089.h>
#include <common.h>
#include <i2c.h>
#include <asm/gpio.h>
#include <asm/io.h>

#define JACK_SENSE	SHIKHARA_GPIO_PD7
unsigned int gpio_pin = JACK_SENSE;
const char *gpio_name ="GPIO_JACKDETECT" ;

int shikhara_jack_init( void )
{
	int ret;

	ret = gpio_request(gpio_pin,gpio_name);
	if (ret){
		printf("Unable to configure the GPIO Pin %d returning\n",gpio_pin);
		return -1;
	}
	ret = gpio_direction_input(gpio_pin); 
	if (ret){
                printf("Unable to configure the GPIO Pin %d returning\n",gpio_pin);
                return -1;
        }

	return ret ;
}

 int shikhara_jack_detect( void )
{		
	if(!(gpio_get_value(gpio_pin) == 0x1)){
                printf("Unable to configure the GPIO Pin %d returning\n",gpio_pin);
                return -1;		
	}
	return 0 ;
}


