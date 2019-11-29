#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/string.h>
#include <asm/arch/shikhara_smc.h>

int ddr_test(void)
{
	uint32_t *buf_wr;
	uint32_t *buf_rd;
	unsigned int data , *address, i;
	unsigned int errors=0;	
	buf_rd = (uint32_t *)malloc(1024);	
	buf_wr = (uint32_t *)malloc(1024);
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
            	if(errors >= 10)
                    goto ret;
       	      }
	      address = address + 1;
   	}
	printf("DDR data is same and is working\n");
ret:
	return 0;
}

int do_ddr_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

      	ddr_test();
	return (0);
}



U_BOOT_CMD(
        ddrtest, 1,  1, do_ddr_test,
        "TESTING DDR: Simple write/read test for DDR\n",
	"ddr_test  - write/read into DDR "
);
