#include <command.h>
#include <common.h>
#include <malloc.h>
#include <flash.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
extern int smc_init_nor(void);

#define DEBUG_NOR
#ifdef DEBUG_NOR
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

 u_char *buf;
 u_char *verify_buf;
 u_long addr_start=0, addr_end;
 unsigned int size=0,i;
int smc_nor_init(void)
{
#if defined(CONFIG_CMD_NOR)
    	puts("SMC initialization with NOR:\n");
	smc_init_nor();
#endif
}

int do_smc_nor_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

       smc_nor_init();
       return(0);
}




U_BOOT_CMD(
        nor_init, 1,  1, do_smc_nor_init,
        "INIT SMC NOR: INIT for SMC NOR\n",
        "nor_init  -SMC NOR init "
);



extern unsigned long flash_init(void);
void initbuff(u_char buf[], int val, int size)
{	
	for(i=0; i<size; i++)
		buf[i]= val;
}

int nor_post(void)
{

	flash_info_t *info;
	info = &flash_info[0];
	flash_init();

        for (i=0; i<info->sector_count; ++i) 
        {
           if (flash_real_protect(info, i, 0))  //unprotecting sectors
                putc ('.');
	}

        if(addr_start ==0 && size == 0)
         {
	   addr_start=(u_long)CONFIG_SYS_FLASH_BASE + 0x2000000;
	   size      = 0x40000;
         }
        
	addr_end = addr_start + size - 1;
	//flash_info_t *info_first = addr2info (addr);
	//flash_info_t *info_last  = addr2info (end );

	buf = malloc(size);
	if (!buf) {
		printf("Not enough malloc space available!\n");
		return -1;
	}
	initbuff(buf, 1, size);

	verify_buf = malloc(size);
	if (!verify_buf) {
		printf("Not enough malloc space available!\n");
		return -1;
	}
	initbuff(verify_buf, 0, size);

	
	dbg("Erasing 0x%08lx - 0x%08lx", addr_start, addr_end);
	if (flash_sect_erase(addr_start, addr_end) > 0) {
		printf("Error: could not erase flash\n");
		return 1;
	}

	printf("Copying to flash...\n");
	if (flash_write(buf, addr_start, size) > 0) {
		dbg("Error: could not copy to flash\n");
		return 1;
	}
	for(i=0;i<10; i++)
	{
		udelay(10000);
		dbg("**********");
	}
	printf("Copying from flash...\n");
	memcpy(verify_buf, addr_start, size);
	printf("Comparing Data ...\n");
/*Compare two buffers*/
	if (memcmp(buf, verify_buf, size))
	{
	printf("\nRead/Write test failed at 0x%x\n", (u32)addr_start);
	return -1;
	}
	else 
	printf("Test passed... Total no of bytes: %d\n ",size);


        for (i=0; i<info->sector_count; ++i)
        {
           if (flash_real_protect(info, i, 1)) //protecting sectors
                putc ('.');
        }

	return 0;
}


int do_nortest(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{

        if(argc <3) {
		addr_start = 0XC1000000;
		size = 0x40000;
	}

        addr_start = simple_strtoul(argv[1], NULL, 16);
        size = simple_strtoul(argv[2], NULL, 16);
        nor_post();
        return(0);
 usage:
        return cmd_usage(cmdtp);

}



U_BOOT_CMD( nortest, 3,1, do_nortest,
	" Simple NOR Read and write test\n",
	" <addr> <len> - Writes len of bytes to addr in NOR Flash, ex/default/preffered: nortest 0xC1000000 0X40000"
);


