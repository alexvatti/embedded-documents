#include <common.h>
#include <asm/io.h>
#include <asm/gpio.h>


struct pl061_gpio_bank shikhara_gpio_bank[] = {
			{
		.base	= (int *) SHIKHARA_GPIO_BANK_A,
			},
			{
		.base	= (int *) SHIKHARA_GPIO_BANK_B,
			},
			{
		.base	= (int *) SHIKHARA_GPIO_BANK_C,
			},
			{
		.base	= (int *) SHIKHARA_GPIO_BANK_D,
			},
		};
		

/*Controller specific API */
struct pl061_gpio_bank *pl061_gpio_get_bank(unsigned gpio)
{	/*Returning the pin number of gpio-bank from the gpio number  */
	int bank = gpio / GPIO_PER_BANK;
	return &shikhara_gpio_bank[bank];
}

int pl061_gpio_get_pin(unsigned gpio)
{	/*Returning the pin of the bank from gpio number  */
	return gpio % GPIO_PER_BANK;
}



int pl061_gpio_direction_input(struct pl061_gpio_bank *bank , unsigned pin)
{
	int gpiodir=0;

	if (pin >= PL061_GPIO_NR)
	return -1;

	gpiodir = readb(bank->base + GPIODIR);
	gpiodir &= ~(1 << pin);
	writeb(gpiodir, bank->base + GPIODIR);
	return 0;
	
}



int pl061_gpio_direction_output(struct pl061_gpio_bank *bank , unsigned int pin, unsigned value)
{

	int gpiodir=0;

	if (pin >= PL061_GPIO_NR)
		return -1;

	writeb(!!value << pin, bank->base + (1 << (pin + 2)));
	gpiodir = readb(bank->base + GPIODIR);
	gpiodir |= 1 << pin;
	writeb(gpiodir, bank->base + GPIODIR);

	/*
	 * gpio value is set again, because pl061 doesn't allow to set value of
	 * a gpio pin before configuring it in OUT mode.
	 */
	writeb(!!value << pin, bank->base + (1 << (pin + 2)));
	return 0;	
			
}

 int pl061_gpio_get_value(struct pl061_gpio_bank *bank, unsigned int pin)
{

	return !!readb(bank->base + (1 << (pin + 2)));
}

 void pl061_gpio_set_value(struct pl061_gpio_bank *bank, unsigned int pin, unsigned int value)
{
	writeb(!!value << pin, bank->base + (1 << (pin + 2)));
}

/*
 * PL061 GPIO IRQ
 */
 void pl061_irq_disable(struct pl061_gpio_bank *bank, int gpio)
{
	unsigned int pin, gpioie;
	pin = pl061_gpio_get_pin(gpio);
	gpioie = readb(bank->base + GPIOIE);
	gpioie &= ~(1 << pin);
	writeb(gpioie, bank + GPIOIE);
	
}

 void pl061_irq_enable(struct pl061_gpio_bank *bank, int gpio)
{
	int gpioie=0;	
	unsigned int pin = pl061_gpio_get_pin(gpio); 
	gpioie = readb(bank->base + GPIOIE);
	gpioie |= 1 << pin;
	writeb(gpioie, bank->base + GPIOIE);
	
}


 int pl061_irq_type(struct pl061_gpio_bank *bank, unsigned trigger, int gpio)
{
	unsigned long flags;
	unsigned char gpiois, gpioibe, gpioiev;
	unsigned int pin = pl061_gpio_get_pin(gpio);

	if (pin < 0 || pin >= PL061_GPIO_NR)
		return -1;
	
	gpioiev = readb(bank->base + GPIOIEV);

	gpiois = readb(bank->base + GPIOIS);
	if (trigger & (IRQ_TYPE_LEVEL_HIGH | IRQ_TYPE_LEVEL_LOW)) {
		gpiois |= 1 << pin;
		if (trigger & IRQ_TYPE_LEVEL_HIGH)
			gpioiev |= 1 << pin;
		else
			gpioiev &= ~(1 << pin);
	} else
		gpiois &= ~(1 << pin);
	writeb(gpiois, bank->base + GPIOIS);

	gpioibe = readb(bank->base + GPIOIBE);
	if ((trigger & IRQ_TYPE_EDGE_BOTH) == IRQ_TYPE_EDGE_BOTH)
		gpioibe |= (1 << pin);
	else {
		gpioibe &= ~(1 << pin);
		if (trigger & IRQ_TYPE_EDGE_RISING)
			gpioiev |= 1 << pin;
		else if (trigger & IRQ_TYPE_EDGE_FALLING)
			gpioiev &= ~(1 << pin);
	}
	writeb(gpioibe, bank->base + GPIOIBE);
	writeb(gpioiev, bank->base + GPIOIEV);
	return 0;
}

/*Mode  */



/* Common GPIO API */

int gpio_request(unsigned gpio, const char *label)
{
	/*Check the gpio number and if it is <NR_IRQ then return 0 otherwise return error */
	if(gpio < PL061_GPIO_NR) 
	return 0;
	else return -1;

}

int gpio_free(unsigned gpio)
{
	/*Stop working with gpio pin make it avialable */	
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	/* Extract the Base from gpio and
	 * implement the function: pl061_direction_input( bank,  gpio)  */ 
	pl061_gpio_direction_input(pl061_gpio_get_bank(gpio),
			pl061_gpio_get_pin(gpio));

	return 0;
}

int gpio_direction_output(unsigned  gpio, int value)
{
	/* Extract the Base from gpio and 
	 * implement the function: pl061_gpio_direction_output( bank, gpio, value) */
	pl061_gpio_direction_output(pl061_gpio_get_bank(gpio), pl061_gpio_get_pin(gpio), value);
	return 0;
}

int gpio_get_value(unsigned  gpio)
{
	/* Extract the Base from gpio and 
	 * implement the function: pl061_gpio_get_value(bank, gpio) */
	return pl061_gpio_get_value(pl061_gpio_get_bank(gpio),
				       pl061_gpio_get_pin(gpio));
	//return 1;
}

int gpio_set_value(unsigned gpio, int value)
{
	/* Extract the Base from gpio and 
	 * implement the function: pl061_gpio_set_value(bank, gpio, value) */
	pl061_gpio_set_value(pl061_gpio_get_bank(gpio),
			  pl061_gpio_get_pin(gpio), value);
	return 0;
}
