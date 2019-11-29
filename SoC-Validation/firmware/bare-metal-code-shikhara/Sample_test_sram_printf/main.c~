/*
 * main.c
*/
#include<shikhara_smc.h>
#include <shikhara_uart.h>
#include<common.h>
void c_entry( )
{
    uart_init();
    uart_puts("Hello, World....Shikhara Project!\r\n");
    smc_sram_init();
    uart_puts("SMC SRAM Init Completed....Shikhara Project!\r\n");
    smc_sram_write();
    uart_puts("SMC SRAM Write Completed ....Shikhara Project!\r\n");
    smc_sram_read();
    uart_puts("SMC SRAM Read Completed....Shikhara Project!\r\n");
    if(smc_sram_verify()==1)
    	uart_puts("SMC SRAM Verify:Success....Shikhara Project!\r\n");
     else
	uart_puts("SMC SRAM Verify: Failed....Shikhara Project!\r\n");
    uart_puts("SMC SRAM Test Done....!\r\n");
}



