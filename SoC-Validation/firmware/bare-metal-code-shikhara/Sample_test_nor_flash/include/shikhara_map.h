
#ifndef __SHIKHARA_MAP_H_
#define __SHIKHARA_MAP_H_	1

//shikha memory map
#define SHIKHARA_SYS_CTRL_BASEADDR    0xD4561000
#define CONFIG_V7ONCHIP_SRAM_BASE     0XD45B0000

//UART
#define SHIKHARA_UART0_BASE 	0xD457B000 
#define SHIKHARA_UART1_BASE 	0xD457C000
#define SHIKHARA_UART2_BASE 	0xD457D000
#define SHIKHARA_UART3_BASE 	0xD457E000
#define SHIKHARA_UART4_BASE 	0xD457F000

//GPIO
/*ARM PrimeCell General Purpose Input/Output (PL061)*/
#define SHIKHARA_GPIO_BANK_A 	0xD4576000
#define SHIKHARA_GPIO_BANK_B 	0xD4577000
#define SHIKHARA_GPIO_BANK_C 	0xD4578000
#define SHIKHARA_GPIO_BANK_D 	0xD4579000

/*The GPIODIR register is the data direction register. Bits set to HIGH in the GPIODIR
configure corresponding pin to be an output. Clearing a bit configures the pin to be
input. All bits are cleared by a reset. Therefore, the GPIO pins are input by default*/


/*The GPIODATA register is the data register. In software control mode, values written in
the GPIODATA register are transferred onto the GPOUT pins if the respective pins
have been configured as outputs through the GPIODIR register*/

/*PL-061 Register Description*/
#define PL061_GPIODATA        0x3FC /* PrimeCell GPIO Data Register */
#define PL061_GPIODIR         0x400 /* PrimeCell GPIO Direction Register */


//UART
#define UART0_BASE		SHIKHARA_UART0_BASE
#define UART_CLOCK 		24000000 //24MHZ
#define CONFIG_PL011_CLOCK 	24000000
#define CONFIG_BAUDRATE		115200
#define EAGAIN 11

//SMC
#define SMC_NORFLASH_BASE_ADDR		0xC0000000 //smc nor base address
#define SHIKHARA_SMC_BASE		0xD45A2000 //smc base address

#define CONFIG_NORFLASH0              0xC0000000
#define CONFIG_NORFLASH1              0xC4000000
#define SHIKHARA_SYS_CTRL	      0xD4561000
#define SMC0_CLK_DIV_RATIO_OFFSET	0X14	     
#endif
