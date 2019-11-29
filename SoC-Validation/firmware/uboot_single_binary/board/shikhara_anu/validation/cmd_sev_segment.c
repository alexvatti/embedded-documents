#include<common.h>
#include<command.h>
#include<asm/arch/sev_segment.h>
#include<asm/io.h>

#ifdef CONFIG_SEV_SEG

int do_ledprint(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	unsigned int led=0, n= simple_strtoul(argv[1],NULL,10);
	if(argc >2)
	led= simple_strtoul(argv[2],NULL,10);
	seg_display(n,led);
	return 0;
}
int do_ledoff(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	seg_off();
	return 0;
}

int do_ledloop(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	writel(0xf686, SHIKHARA_SYS_CTRL_BASEADDR +400);
	udelay(1000000);
	for(i=0; i< 16; i++){
       	seg_display(i,0);
       	udelay(300000);
	seg_display(i,1);
	}
	
	return 0;
}

U_BOOT_CMD(
	ledprint,3,0,do_ledprint,
	"print value in 7 segment Display",
	"Simple command to display the value in 7-seg Display"
	);
U_BOOT_CMD(
        ledoff,2,0,do_ledoff,
        "Off the  7 segment Display",
        "Simple command to off the 7-seg Display"
        );
U_BOOT_CMD(
        ledloop,2,0,do_ledloop,
        "Loop test for 7 segment Display",
        "Simple command to loop the 7-seg Display"
        );

#endif
