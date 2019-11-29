#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_irq.h>
static int cnt;
void temp_hndler(){
	printf("Got Edge level interrupt\n");
	cnt++;
}
int do_intr_gnss_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("Enable Interrupt\n");
        int_enable(61);
	printf("Set Handler\n");

	interrupt_handler_set(61,temp_hndler);
	printf("Poll for interupts\n");
	while(1){
		printf("In While loop count is %d \n",cnt);
		if(cnt ==100){
			int_disable(61);
			cnt = 0;
			break;
		}
	}

}


U_BOOT_CMD(
                gnss_intr_min, 1, 1, do_intr_gnss_test,
                " Simple GNSS testcase to test GNSS IP",
                " Prints NMEA messages fro GNSS IP"
          );


