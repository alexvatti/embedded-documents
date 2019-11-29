
#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_smc.h>
#include <asm/arch/shikhara_map.h>

#define NAND_MEM_ADDR_0                 0x5C     // 8-bits wide
#define NAND_CMD_CYC                    0x5      // 3-bits wide
#define NAND_CMD_SEL                    0x0      // 1-bit wide
#define NAND_DEPTH                     	16 
#define CLEAR_CS                        0x1

#define ONFI_RESET		0xFF
#define ONFI_CMD_READ_END	0x30
#define ONFI_CMD_READ		0x00
#define ONFI_CMD_WRITE_END	0x10
#define ONFI_CMD_WRITE		0x80
#define ONFI_CMD_ERASE		0x60
#define ONFI_CMD_ERASE_END	0xD0
#define ONFI_CMD_READID		0x90

#define DEBUG_NAND
#ifdef DEBUG_NAND
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

unsigned int md_id, dv_id, dv_co,dv_info;
volatile unsigned int cmd_addr= 0x00000000, data_addr=0x00000000; 
short volatile int NAND_START_CMD =0x00 ;    
short volatile int NAND_END_CMD_VALID= 0x0;    
short volatile int NAND_END_CMD = 0x00 ;     
short volatile int NAND_DATA_CLR_CS  = 0x0  ;    
short volatile int NAND_ECC_LST      = 0x0  ;     
unsigned int error;
unsigned int pass,temp;
volatile unsigned int write[16], read[16];
int nand_post(void)
{
          /* Reset the Chip */
  cmd_addr = (  (NAND_MEM_ADDR_0 << 24)|
                (NAND_END_CMD_VALID << 20) |
                (NAND_CMD_SEL << 19) |
                (NAND_END_CMD << 11) |
                (ONFI_RESET << 3));

    writel(0x0,cmd_addr);
    dbg("Wait for interrupt\n");
/* Wait for interrupt */
   while(!(readl(SHIKHARA_SMC_BASE+SMC_MC_STATUS))){
        udelay(500);
        if(error++ > 40)
        break;
        }

    printf("Reading ID \n");
    writel( (readl(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)|(1<<4)),(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)); /* Clear interrupt*/
/* Read Id command */
    NAND_START_CMD     = ONFI_CMD_READID ;
    cmd_addr = ((NAND_MEM_ADDR_0 << 24)| (0x3 << 21)| (NAND_END_CMD_VALID << 20) | (NAND_CMD_SEL << 19) | (NAND_END_CMD << 11) | (NAND_START_CMD << 3));
    data_addr = ((NAND_MEM_ADDR_0 << 24) |
                  (!(CLEAR_CS)) << 21) |
                  (NAND_END_CMD_VALID << 20) |
                  (0x1<< 19) |
                  (NAND_END_CMD << 11)
                 ;
    dbg("Start sending\n");
    writel(0x0,cmd_addr);
    udelay(100);

    md_id = readl(data_addr);
    dv_id = readl(data_addr);
    dv_co = readl(data_addr);
    dv_info = readl(data_addr);
    printf("NAND Flash Device Detected\n\t Manufacturer ID: 0x%X\n\t Device ID: 0x%X\n\t Device Configuration: 0x%X\n",md_id, dv_id, dv_co);
    printf("\t Device Information: 0x%X\n",dv_info);

     data_addr = ((NAND_MEM_ADDR_0 << 24) |
                  ((CLEAR_CS)) << 21 |
                  (NAND_END_CMD_VALID << 20) |
                  (0x1<< 19) |
                  (NAND_END_CMD << 11));

     writel(0x00,data_addr); /* Disable Chip Select */

     return(0);
}

int do_nand_mintest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  unsigned int i;
    nand_post();

  for (i=0;i< sizeof(write);i++)
  {
       temp =(temp+i)& 0xFFFFFFFF;
       write[i]=temp;
  }

    data_addr = ((NAND_MEM_ADDR_0 << 24) |
                  (!(CLEAR_CS)) << 21 |
                  (NAND_END_CMD_VALID << 20) |
                  (0x1<< 19) |
                  (NAND_END_CMD << 11));

     writel(0x00,data_addr); /* assert Chip Select */


 /* Erase command */ 
    NAND_END_CMD_VALID = 0x1  ; 
    NAND_END_CMD       = ONFI_CMD_ERASE_END ; 
    NAND_START_CMD     = ONFI_CMD_ERASE ;  
    cmd_addr = ((NAND_MEM_ADDR_0 << 24)| (0x3 << 21)| (NAND_END_CMD_VALID << 20) | (NAND_CMD_SEL << 19) | (NAND_END_CMD << 11) | (NAND_START_CMD << 3)); 
    printf("StartErase Command\n");
    writel(0x00, cmd_addr);
    printf("Wait for interrupt\n");
/* Wait for interrupt */
   while(!(readl(SHIKHARA_SMC_BASE+SMC_MC_STATUS))){
	udelay(500);
	if(error++ > 40)
	break;
	}
    writel( (readl(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)|(1<<4)),(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)); /* Clear interrupt*/

   printf("Start writing\n");
/* Start Writing */  
    NAND_END_CMD_VALID = 0x0  ; 
    NAND_END_CMD       = ONFI_CMD_WRITE_END; 
    NAND_START_CMD     = ONFI_CMD_WRITE ;  
    cmd_addr = ((NAND_MEM_ADDR_0 << 24)| (NAND_CMD_CYC << 21)| (NAND_END_CMD_VALID << 20) | (NAND_CMD_SEL << 19) | (NAND_END_CMD << 11) | (NAND_START_CMD << 3)); 

    data_addr = ((NAND_MEM_ADDR_0 << 24) |
		  (!(CLEAR_CS) << 21) |
		  (NAND_END_CMD_VALID << 20) |
		  (0x1<< 19) |
		  (NAND_END_CMD << 11) //|
                   // (ECC Last << 10)); /* We are not using ECC*/
         	 );
    // Formatting AXI Address with start command, address cycles
    for(i= 0; i < NAND_DEPTH ; i=i+1 )
  
    {
        writel(0x00+(i*4),cmd_addr);
  	/* Wait for interrupt */
   	while(!(readl(SHIKHARA_SMC_BASE+SMC_MC_STATUS))){
		udelay(500);
		if(error++ > 40)
		break;
	}
	writel((readl(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)|(1<<4)),(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG));
	/*Write data */
	printf("Write Data %X\n",write[i]);
	writel(write[i],data_addr);
	udelay(10);
    }
    printf("after data write\n");
    data_addr |= CLEAR_CS << 21;
    writel(0x00,data_addr); /* Disable Chip Select */

    printf("Start Read Back the data\n");
/* Read back the data */

    NAND_END_CMD_VALID = 0x1  ; 
    NAND_END_CMD       = ONFI_CMD_READ_END ; 
    NAND_START_CMD     = ONFI_CMD_READ ;  
    cmd_addr = ((NAND_MEM_ADDR_0 << 24) | (NAND_CMD_CYC << 21)| (NAND_END_CMD_VALID << 20) | (NAND_CMD_SEL << 19) | (NAND_END_CMD << 11) | (NAND_START_CMD << 3)); 

    data_addr = ((NAND_MEM_ADDR_0 << 24) |
                  (!(CLEAR_CS) << 21) |
                  (NAND_END_CMD_VALID << 20) |
                  (0x1<< 19) |
                  (NAND_END_CMD << 11)//|
                    // (ECC Last << 10)); /* We are not using ECC*/
	 	);

    for(i= 0; i < NAND_DEPTH ; i=i+1 )
    {
	 writel(0x00+(i*4),cmd_addr);
	printf("Start reading\n");
	/* Wait for interrupt */
   	while(!(readl(SHIKHARA_SMC_BASE+SMC_MC_STATUS))){
		if(error++ >  40)
		break;
	}
	writel( (readl(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG)|(1<<4)),(SHIKHARA_SMC_BASE+SMC_MC_CLR_CONFIG));
	
       read[i] = readl(data_addr);
	printf("Read value is %X\n",read[i]);
       udelay(10);
    }
    printf("Stop All transactions\n"); 
    data_addr |= CLEAR_CS << 21;
    writel(0x0,data_addr); /* Disable Chip Select */
    error = 0 ;
    pass= 0;
/* Compare Result*/
  for (i=0; i<NAND_DEPTH; i++)
    {
       if (write[i] != read[i])
       {
       	   pass =0; //apERR_MEM_START;
           printf("ERROR: Value 0x%x ,Should be :0x%x  \n",read[i], write[i]);
           error += 1;
           if(error >= 5)
           break;
     }
  }
  printf("Test Completed\n");
  return 0;
}


U_BOOT_CMD(
        nand_mintest, 2,  1, do_nand_mintest,
        "Min Test for  NAND validation\n",
        ""
);
/*

int do_nandsmptest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i=0;
	//Reset
	writel(0x0, 0x5c0007f8);
	udelay(1000);
//	read
	writel(0x0, 0x5cb18000);
	writel(0x0, 0x5cb18004);
	for(i=0;i<4;i++)
	dbg("Read Value is %X\n",readl(0x5c2e0400));


//	Erase
	writel(0x00, 0x5c768300);
	udelay(1000000);
//	Read again
	writel(0x0, 0x5cb18000);
        writel(0x0, 0x5cb18004);
        for(i=0;i<4;i++)
        dbg("Read Value is %X\n",readl(0x5c2e0400));

//	Write
	writel(0x00, 0x5ca08400);
	writel(0x00, 0x5ca08404);
	writel(0xa5a5,0x5c288000);
	writel(0x00, 0x5c388000);
	dbg("Writes complete\n");
	
//	Read again
        writel(0x0, 0x5cb18000);
        writel(0x0, 0x5cb18004);
        for(i=0;i<4;i++)
        dbg("Read Value is %X\n",readl(0x5c2e0400));
	
}
U_BOOT_CMD(
        nand_smpltest, 2,  1, do_nandsmptest,
        "TESTING NAND\n",
        " - writes the STR into NAND read back same "
);
*/
