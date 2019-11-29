/*
 * main.c
*/
#include <shikhara_uart.h>
#include<common.h>
#include<gpio-leds.h>
#include <shikhara_map.h>
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
	unsigned int i;
	unsigned int  remap, soft_reg_rst4;

	uart_init();
	printf("Welcome to Hello, World! Shikhara Project\r\n");
	print_bootstrap_mode();
	printf("ROM Code failure case execution: Execution from the Place\r\n");
	printf("ROM Code failure case execution: Execution from the Place\r\n");
	remap = 0x0;
	writel(remap,SHIKHARA_REMAP_REG); 
    	soft_reg_rst4 = 0x1FB00005;
    	writel(soft_reg_rst4, SHIKHARA_SYS_CTRL_BASEADDR+0x54);
    	udelay(25000);
	uart_puts("Hello, World....Shikhara Project....DDR Initialization...Going to start! \r\n");
    	ddr_init();
    	uart_puts("DDR init completed...Successfully\r\n");
    	uart_puts("DDR Test Pattern...started\r\n");
    	ddr_test(0xAAAAAAAA);
    	ddr_test(0x55555555);
    	ddr_test(0xFFFFFFFF);
    	uart_puts("DDR Test Pattern...Done....Successfully\r\n");

        gpio_port_configure_output(0);
        gpio_port_configure_output(1);
	while(1)
	{
        	//drive port 0 and 1, high
        	gpio_port_drive_high(0);
        	gpio_port_drive_high(1);
		for(i=1;i<500;i++)
        	    udelay(1000);
        	//drive port 0 and 1, low
        	gpio_port_drive_low(0);
        	gpio_port_drive_low(1);
        	for(i=1;i<500;i++)
        	    udelay(1000);
    	}
	
}

int ddr_test(unsigned int data)
{
        unsigned int data_should,*address, i;
        unsigned int errors=0;
        address = (u32*)SHIKHARA_DDR_BASE_ADDRESS;
       // data= 0x12345678;
        data_should= data;

        for (i=0;i<0xFFFFF;i=i+4)
        {
              *address = data;
               address = address + 1;
        }
        address = (u32*)SHIKHARA_DDR_BASE_ADDRESS;
        for (i=0;i<0xFFFFF;i=i+4)
        {
              data=*address;
              if(data!=data_should)
                printf("ERROR: Addres 0x%p ,Should be :0x%x Is: 0x%x\r\n", address,data_should,data);
             address = address + 1;
        }
        printf("DDR data is same and is working:%x\r\n",data);
        return 0;
}



