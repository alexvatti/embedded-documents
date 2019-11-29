

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_gic.h>
#include "../../../include/usb.h"
void temp_intr_hndler(void);
int cnt;
void temp_intr_hndler(){
        printf("\nGot USB interrupt\n");
        cnt++;
	usb_lowlevel_stop(0);
	return;
}
int do_intr_usb_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	void *ctrl;
        printf("Enable Interrupt\n");
        int_enable(67);
        printf("Set Handler\n");

        interrupt_handler_set(67,(void*)temp_intr_hndler);
	usb_lowlevel_init(0, 0, &ctrl);
        printf("Poll for interupts\n");
        while(1){
                printf("In While loop count is %d \n",cnt);
		if(ctrlc()){
                        int_disable(67);
			return 0;
			}
                if(cnt){
                        int_disable(67);
                        cnt = 0;
                        break;
                }
        }
	return 0;
}


U_BOOT_CMD(
                usb_intr, 1, 1, do_intr_usb_test,
                " Simple USB testcase to register and raise interrupt from GIC",
                " Prints debug message if interrupt is raised"
          );

