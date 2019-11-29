#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#define SEVENSEG_CONTROL        SHIKHARA_SYS_CTRL_BASEADDR+0x400

#define DEBUG_SEV
#ifdef DEBUG_SEV
#define dbg(x...)       printf(x)
#endif

const   unsigned char   HEXA_DATA[] = {
//'0'   '1'   '2'   '3'   '4'   '5'   '6'   '7'   '8'   '9'   'a'   'b'   'c'   'd'   'e'   'f'
 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47,0xFF };

int led_test(void)
{
	int i=0,j=0;
        dbg("\nThis test prints Digits on Seven Segment LED's\n\n");

        for(i=0; i<16;i++){
//           seg_display(i);
             writel(HEXA_DATA[i], SEVENSEG_CONTROL);
             udelay(100000);
        }
        for(j=0; j<16;j++){
             writel((HEXA_DATA[j] << 8), SEVENSEG_CONTROL);
             udelay(100000);
        }

        return (0);
}

int do_ledtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

       led_test();
       return(0);
}

U_BOOT_CMD(
        ledtest,2,0,do_ledtest,
        "test to print digits on 7 segment Display",
        "Simple command to test the 7-seg Display"
        );

