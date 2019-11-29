/*
 * main.c
*/
#include<shikhara_smc.h>
#include <shikhara_uart.h>
#include <shikhara_map.h>
#include<common.h>

void c_entry( )
{
    unsigned int sys_ctl, remap, soft_reg_rst4, ddr_reg,ddr_reg_08,ddr_reg_04;
    remap = 0x0;

    writel(remap,SHIKHARA_REMAP_REG); 
    soft_reg_rst4 = 0x1FB00005;
    writel(soft_reg_rst4, SHIKHARA_SYS_CTRL_BASEADDR+0x54);
    udelay(25000);
/*  soft_reg_rst4 = 0x00000005;
    writel(soft_reg_rst4, SHIKHARA_SYS_CTRL_BASEADDR+0x54);
    udelay(250000);*/
    uart_init();

    uart_puts("Hello, World....Shikhara Project....DDR Initialization...Going to start! \r\n");
    ddr_init();
    uart_puts("DDR init completed...Successfully\r\n");

    uart_puts("DDR Test Pattern...started\r\n");
    ddr_test(0xAAAAAAAA);
    ddr_test(0x55555555);
    ddr_test(0xFFFFFFFF);
    uart_puts("DDR Test Pattern...Done....Successfully\r\n");
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

