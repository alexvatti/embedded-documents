/*
 * main.c
*/
#include <shikhara_uart.h>
#include <shikhara_map.h>
#include<common.h>
void print_bootstrap_mode()
{
unsigned int boot_strap;
        boot_strap =readl(BOOT_STRAP);
	boot_strap=boot_strap&0x0f;
	printf("Boot strap mode:function, with boot_strap=%x\r\n",boot_strap);
  	switch(boot_strap)
  	{

	case 0xB:
		printf("Boot strap mode: NAND \r\n");
       	       	break;

  	case 0x9:
	 	printf("Boot strap mode:NOR \r\n");
	  	break;
 
 	case 0xA:
	 	printf("Boot strap mode:MMC/SD \r\n");
	  	break;
		
  	case 0x8:
		printf("Boot strap mode:SPI \r\n");
	  	break;

  	default:  
		printf("Boot strap mode:Invalid, check the boot strap settings/pins \r\n");
	  	break;       
   	}
  	return (0);
}
void c_entry( )
{
	char ch;
	uart_init();
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	print_bootstrap_mode();
	while ( 1 ) {
		ch=uart_getc();
                uart_putc(ch);
	}
}




