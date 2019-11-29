/*Shikhara GPIO Pin Information */
#include<linux/compiler.h>
#include<common.h>
#include<asm/arch/shikhara_map.h>
#ifndef __SHIKHARA_GPIO_H__
#define __SHIKHARA_GPIO_H__

#define GPIO_INPUT	0x00
#define GPIO_OUTPUT	0X01 

/*Total No. of available gpio's */ 
#define PL061_GPIO_NR 32
#define DAT_MASK(x)		(0x1 << (x))
#define GPIO_PER_BANK 8

/*PL-061 Register Description*/

#define GPIODATA        0x3fc
#define GPIODIR		0x400 /* PrimeCell GPIO Direction Register */
#define GPIOIS		0x404 /* PrimeCell GPIO Interrupt Sense Register */
#define GPIOIBE		0x408 /* PrimeCell GPIO Interrupt Both Edges Register */
#define GPIOIEV		0x40C /* PrimeCell GPIO interrupt event Register */
#define GPIOIE		0x410 /* PrimeCell GPIO interrupt mask Register */
#define GPIORIS 	0x414 /* PrimeCell GPIO Raw Interrupt Status */
#define GPIOMIS 	0x418 /* PrimeCell GPIO Masked Interrupt Status */
#define GPIOIC 		0x41C /* PrimeCell GPIO Interrupt Clear Register */
#define GPIOAFSEL 	0x420 /* PrimeCell GPIO Mode Control Select Register */

#define IRQ_TYPE_EDGE_RISING	0x00000001
#define	IRQ_TYPE_EDGE_FALLING	0x00000002
#define	IRQ_TYPE_EDGE_BOTH	(IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define	IRQ_TYPE_LEVEL_HIGH	0x00000004
#define	IRQ_TYPE_LEVEL_LOW	0x00000008


enum gpio_pin {
	SHIKHARA_GPIO_PA0 = 0,	/*Pin 0 BANK-A*/
	SHIKHARA_GPIO_PA1,
	SHIKHARA_GPIO_PA2,
	SHIKHARA_GPIO_PA3,
	SHIKHARA_GPIO_PA4,
	SHIKHARA_GPIO_PA5,
	SHIKHARA_GPIO_PA6,
	SHIKHARA_GPIO_PA7,
	SHIKHARA_GPIO_PB0,	/*Pin 7 BANK-B*/
	SHIKHARA_GPIO_PB1,
	SHIKHARA_GPIO_PB2,
	SHIKHARA_GPIO_PB3,
	SHIKHARA_GPIO_PB4,
	SHIKHARA_GPIO_PB5,
	SHIKHARA_GPIO_PB6,
	SHIKHARA_GPIO_PB7,
	SHIKHARA_GPIO_PC0,	/*Pin 15 BANK-C*/
	SHIKHARA_GPIO_PC1,
	SHIKHARA_GPIO_PC2,
	SHIKHARA_GPIO_PC3,
	SHIKHARA_GPIO_PC4,
	SHIKHARA_GPIO_PC5,
	SHIKHARA_GPIO_PC6,
	SHIKHARA_GPIO_PC7,
	SHIKHARA_GPIO_PD0,	/*PIN 23 BANK-D*/
	SHIKHARA_GPIO_PD1,
	SHIKHARA_GPIO_PD2,
	SHIKHARA_GPIO_PD3,
	SHIKHARA_GPIO_PD4,
	SHIKHARA_GPIO_PD5,
	SHIKHARA_GPIO_PD6,
	SHIKHARA_GPIO_PD7,
	};


struct pl061_gpio_bank {

	void __iomem		*base;
	unsigned int		irq_base;
};

/*Function declaration */
void pl061_irq_disable(struct pl061_gpio_bank *bank, int gpio);
void pl061_irq_enable(struct pl061_gpio_bank *bank, int gpio);
int pl061_irq_type(struct pl061_gpio_bank *bank, unsigned trigger, int gpio);
struct pl061_gpio_bank *pl061_gpio_get_bank(unsigned gpio);
int s5p_gpio_get_pin(unsigned gpio);
int pl061_gpio_get_value(struct pl061_gpio_bank *bank, unsigned int pin);
void pl061_gpio_set_value(struct pl061_gpio_bank *bank, unsigned int pin, unsigned int value);
int pl061_gpio_direction_input(struct pl061_gpio_bank *bank , unsigned pin);
int pl061_gpio_direction_output(struct pl061_gpio_bank *bank , unsigned int pin, unsigned value);
#endif



