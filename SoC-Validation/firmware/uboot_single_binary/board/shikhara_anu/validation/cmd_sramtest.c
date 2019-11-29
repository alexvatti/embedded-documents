#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/string.h>
#include <asm/arch/shikhara_smc.h>

#define SRAM_BASE	CONFIG_SMC_SRAM_BASE	
extern int smc_init_sram(void);

/* SRAM base address */

int SRAM_OFFSET=0x100;

int sram_init(void)
{
#if defined(CONFIG_CMD_SRAM)
	puts("SMC initialization with RAM:\n");
	smc_init_sram();
	return(0);
	
#endif
}

int do_sram_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

       sram_init();
       return(0);
}




U_BOOT_CMD(
        sram_init, 1,  1, do_sram_init,
        "INIT SRAM: INIT for SRAM\n",
        "sram_init  -SRAM init "
);

int validation_sram_test(void)
{
	uint32_t *buf_wr;
	uint32_t *buf_rd;
	unsigned int data , *address, i;
	unsigned int errors=0;	
	buf_rd = (uint32_t *)malloc(1024);	
	if(!buf_rd)
		printf("Validation_sram_test(): malloc() fails\n");
	
	buf_wr = (uint32_t *)(SRAM_BASE+SRAM_OFFSET);
	address = buf_wr;
	data= 0x1234567;

	for (i=1024;i>0;i=i-4)
   	{
              data = ((data<<1) | (data>>31))^ (data & 0x10300401) ;
       	      *address = data;
               address = address + 1;
    	}
	
	// clean destination memory
    	address = buf_rd;
   	for (i=1024;i>0;i=i-4)
    	{
              *address = 0x00000000;
              address = address + 1;
    	}
	memcpy(buf_rd,buf_wr, 1024);
	udelay(10000);

	data= 0x1234567;
	address= buf_rd;
	for (i=1024;i>0;i=i-4)
    	{
              data = ((data<<1) | (data>>31))^ (data & 0x10300401) ;
              if ((*address) != (data))
              {
	           	printf("ERROR: Addres 0x%p ,Should be :0x%x Is: 0x%x \n", address,data,*address);
	            	errors += 1;
	            	if(errors > 0)
	                    goto fail;
       	}
	      address = address + 1;
   	}
	printf("SRAM data is same and SMC Controller is working\n");
	return 0;
fail:
	return -1;
}

int do_sram_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

       validation_sram_test();
       return(0);
}



U_BOOT_CMD(
        sram_test, 1,  1, do_sram_test,
        "TESTING SRAM: Simple write/read test for SRAM\n",
	"sram_test  - write/read into SRAM "
);
