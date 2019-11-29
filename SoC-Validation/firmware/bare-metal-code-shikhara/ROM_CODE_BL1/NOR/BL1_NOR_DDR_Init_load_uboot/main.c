/*
 * main.c
*/
#include<common.h>
#include<shikhara_smc.h>
#include <shikhara_uart.h>
#include <shikhara_map.h>

#define SYSTEMCTL_REG 0xD4561000
#define DDR_BASE_ADDRESS 0x00000000

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
#if 0
void blink_led(void)
{
	*((volatile unsigned int *)0xD4576400) = 0xff; //output
        *((volatile unsigned int *)0xD45763fc) = 0xff; //output

	*((volatile unsigned int *)0xD4577400) = 0xff; //output
        *((volatile unsigned int *)0xD45773fc) = 0x00; //output
	
}
#endif

void uboot_copy(int nor_addr, int ddr_addr,int length)
{
	int i = 0;
	for(i=0;i<(length/4);i++) { //
//		printf("nor addr:0x%x value:0x%x\n\r", nor_addr, *((unsigned int *)nor_addr));	
		*((unsigned int *)ddr_addr) = *((unsigned int *)nor_addr);	
//		printf("ddr addr:0x%x ddr_value:0x%x \n\r", ddr_addr, *((unsigned int *)ddr_addr));	
		nor_addr = nor_addr+4;
		ddr_addr = ddr_addr+4;	
	}
}
#if 0
void led_off(void)
{
	*((volatile unsigned int *)0xD4576400) = 0xff; //output
        *((volatile unsigned int *)0xD45763fc) = 0x00; //output	
	
	*((volatile unsigned int *)0xD4577400) = 0xff; //output
        *((volatile unsigned int *)0xD45773fc) = 0xff; //output
}
#endif
void c_entry( )
{
	unsigned int sys_ctl, remap, soft_reg_rst4, ddr_reg,ddr_reg_08,ddr_reg_04, nor_addr = 0xc0040000, ddr_addr = 0x3D000000;
	int a = 0;

	remap = 0x0;
    	gpio_port_configure_output(0);
        gpio_port_configure_output(1);

	gpio_port_drive_high(0);
        gpio_port_drive_high(1);

	writel(remap,0xD4561098);
	soft_reg_rst4 = 0x1FB00005;
	writel(soft_reg_rst4, SYSTEMCTL_REG+0x54);
	/*    soft_reg_rst4 = 0x00000005;
	      writel(soft_reg_rst4, SYSTEMCTL_REG+0x54);
	      udelay(250000);*/
	uart_init();
	uart_puts("------ : Welcome to Shikhara Project... \r\n");
	print_bootstrap_mode();
	uart_puts("DDR init Going to start\r\n");
	ddr_init();
	uart_puts("DDR init completed\r\n");

	//uboot_copy(nor_addr, ddr_addr, 0x1cbf0);
	uboot_copy(nor_addr, ddr_addr, 0xD0000);
	uart_puts("U-BOOT is copied to DDR  completed\r\n");

	gpio_port_drive_low(0);
        gpio_port_drive_low(1);

	__asm__ ("mov r0,#1");
	__asm__ __volatile__ ("mov r1,#2" );
	__asm__ __volatile__ ("mov r2,#3" );
	__asm__ __volatile__ ("mov r3,#4" );
	__asm__ __volatile__ ("ldr pc,=(0x3D000000)" );
	
//	relocate_code();	
}
