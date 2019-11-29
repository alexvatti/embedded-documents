/*
 * main.c
*/

#include <stdbool.h>
#include<common.h>
void c_entry()
{
    uart_init();
    uart_puts("Hello, World....Shikhara Project!\r\n");
    smc_nor_init();
    uart_puts("SMC NOR Init Completed....Shikhara Project!\r\n");
    smc_nor_read_id();
    uart_puts("SMC NOR Read id Completed....Shikhara Project!\r\n");
    smc_nor_write();
    uart_puts("SMC NOR Write Completed ....Shikhara Project!\r\n");
    smc_nor_read();
    uart_puts("SMC NOR Read Completed....Shikhara Project!\r\n");
    if(smc_nor_verify()==1)
    	uart_puts("SMC NOR Verify:Success....Shikhara Project!\r\n");
    else
	uart_puts("SMC NOR Verify:Failed....Shikhara Project!\r\n");
    uart_puts("SMC NOR Test Done....!\r\n");
}