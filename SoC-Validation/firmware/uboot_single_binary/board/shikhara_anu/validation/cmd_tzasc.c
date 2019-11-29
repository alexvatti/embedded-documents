/*
 * The CPU is in secure mode and TZASC configure with 2 Regions (region- 0 and 1), Region 1 as Secure.
 * To Test TZASC, we try to access Region-1 Address from Both Secure and Non Secure Modes of CPU. 
 */

#include<common.h>
#include<command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>

#define TZASC_BASE              TZASC0_BASE

/*Register Definitions*/
#define TZASC_CONFIGURE                         TZASC_BASE+0x000
#define TZASC_ACTION                            TZASC_BASE+0x004
#define TZASC_LOCKDOWN_RANGE                    TZASC_BASE+0x008
#define TZASC_LOCKDOWN_SELECT                   TZASC_BASE+0x00C
#define TZASC_INTR_STATUS                       TZASC_BASE+0x010
#define TZASC_INTR_CLEAR                        TZASC_BASE+0x014
#define TZASC_FAIL_ADD_LOW                      TZASC_BASE+0x020
#define TZASC_FAIL_ADD_HIGH                     TZASC_BASE+0x024
#define TZASC_FAIL_CNTRL                        TZASC_BASE+0x028

#define TZASC_SECURITY_INVERSION_EN            TZASC_BASE+0x034
#define TZASC_SPECULATION_CONTROL               TZASC_BASE+0x030

#define TZASC_REGION_SETUP_LOW_0                TZASC_BASE+0x100
#define TZASC_REGION_SETUP_HIGH_0               TZASC_BASE+0x104
#define TZASC_REGION_ATTRIBUTES_0               TZASC_BASE+0x108

#define TZASC_REGION_SETUP_LOW_1                TZASC_BASE+0x110
#define TZASC_REGION_SETUP_HIGH_1               TZASC_BASE+0x114
#define TZASC_REGION_ATTRIBUTES_1               TZASC_BASE+0x118

extern void smc_switch();

/*TZASC is connected only for DDR (LPDDR in asic)*/

int do_tzasc(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int i,ret=0,cnt=10;
	unsigned int fail,val;
	unsigned int *add =  (unsigned int *)CONFIG_SYS_SDRAM_BASE;
	printf("Processor is in Secure world... \n");
	printf("Configure TZASC...\n");
	/*Configure TZASC */
	writel(0x3, TZASC_ACTION);                // Sets tzasc_int HIGH and issues a DECERR response.
	writel(0x0,TZASC_LOCKDOWN_RANGE);        // Disable the lockdown_regions
	writel(0x0, TZASC_LOCKDOWN_SELECT);       // No effect on Speculation_control, Security_inversion_en
						// and lockdown_range Register
	writel(0x3, TZASC_SPECULATION_CONTROL);   // Write access speculation and Read access speculation is disabled.
	writel(0x0, TZASC_SECURITY_INVERSION_EN); // security inversion is not permitted.

	/*Region 1 configureations*/
	writel(0x0, TZASC_REGION_SETUP_HIGH_1);   // Region 1 start at address[63:32] 0x0
	writel(CONFIG_SYS_SDRAM_BASE, TZASC_REGION_SETUP_LOW_1);  // Region 1 start at address[31:0] at DDR2 Starting address
	writel(0x30000000,TZASC_REGION_ATTRIBUTES_1);   // Secure Read Write access (RW) for 64MB for Region 1  

	printf("Performing writes/reads to secure memory region.. 0x%X\n",add);
	val = 0xaa55aa55;
	for(i=0;i<5;i++){
		add = add+1;
		writel(val,add);           /*We are in Secure state, We can not get DECERR unless Security Inversion enabled*/
		while(cnt){
			if(readl(TZASC_INTR_STATUS)){
				printf("Interrupted due to access violations\n");
				writel(0x0, TZASC_INTR_CLEAR);
				fail = readl(TZASC_FAIL_ADD_LOW);
				printf("Access Violated at address %X\n",fail);
				return  -1 ;
			}
			cnt = cnt-1;
		}
		ret = readl(add);
		if(ret!=val)
			printf("Read value and write values are not same\n");
	}

	printf("Switch to Non Secure World.. processor will go to abort mode..  \n");
	smc_switch();
	return 0;

}

int tzasc_ns( void ){

	unsigned int i, ret, fail,cnt=10,val;
	unsigned int *add =  (unsigned int *)CONFIG_SYS_SDRAM_BASE;
	/* We have to get DECERROR and  CPU switches to ABORT Mode */	
	printf("In Nonsecure world.. Trying to access Secure memory\n");	
	val = 0xabcdef;
	for(i=0;i<5;i++){
		add =add+1;
		writel(val,add);              

		while(cnt--){
			if(readl(TZASC_INTR_STATUS)){                                  /* We can't access these registers in Non-Secure mode
											  lets assume that before coming here cpu  will go to abort mode*/
				printf("Interrupted due to access violations\n");
				writel(0x0, TZASC_INTR_CLEAR);
				fail = readl(TZASC_FAIL_ADD_LOW);
				printf("Access Violated at address %X\n",fail);
				goto ret;
			}
		}

		ret = readl(add);
                if(ret!=val)
                        printf("Read value and write values are not same\n");
	}

ret:
	return 0;
}

U_BOOT_CMD(tzasc,2,0,do_tzasc,
		"Test TZASC ",
		"Simple Test Case"
	  );

