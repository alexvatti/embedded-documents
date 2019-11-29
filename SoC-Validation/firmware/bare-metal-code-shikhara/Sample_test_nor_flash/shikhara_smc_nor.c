#include<common.h>
#include<shikhara_map.h>
#include<shikhara_uart.h>
#include<shikhara_smc.h>
#include<errno.h>
#define NOR_DEPTH 1024
unsigned int write_data[NOR_DEPTH],read_data[NOR_DEPTH];
#define puts printf
unsigned int top_boot=1;
int smc_nor_init(void)
{
   //SMC0 clock reduceed from 133MHz to 100MHz
   writel(0x00000004, SHIKHARA_SYS_CTRL+SMC0_CLK_DIV_RATIO_OFFSET);
   writel(NOR_SET_CYCLES , SHIKHARA_SMC_BASE + SMC_MC_SET_CYCLES);
   writel(NOR_SET_OPMODE , SHIKHARA_SMC_BASE + SMC_MC_SET_OPMODE);
   writel(0x00000005,  SHIKHARA_SMC_BASE + SMC_MC_REFRESH_PERIOD_0); //Setting time interval between two bursts
   writel(NOR_DIRECT_CMD , SHIKHARA_SMC_BASE + SMC_MC_DIRECT_CMD);
   return(0);
}


/*Reading from NOR Flash ID*/
void smc_nor_read_id(void)
{

  uint16_t  manufacture_id;
  uint16_t  device_id;

  writew(CFI_CMD_READ_ID,SMC_NORFLASH_BASE_ADDR);
  manufacture_id = readw(SMC_NORFLASH_BASE_ADDR+CFI_OFFSET_MANUFACTURER_ID);

 
  device_id = readw(SMC_NORFLASH_BASE_ADDR+2);//CFI_OFFSET_DEVICE_ID);

  writew(CFI_CMD_RESET,SMC_NORFLASH_BASE_ADDR);

  printf("Manufacturer Code=0x%2x\r\n",manufacture_id);
  printf("Device ID Code=0x%4x\r\n",device_id);
  top_boot=1;
}


/*Reading from NOR Flash*/
void smc_nor_read(void)
{
  unsigned int i,j=0;
  for(i= 0; i < NOR_DEPTH ;i=i+2)
  {
	 writew(CFI_CMD_READ,((SMC_NORFLASH_BASE_ADDR+i)));
         read_data[j] = readw(((SMC_NORFLASH_BASE_ADDR+i)));
	 j++;
  }
  for(j=0;j<60000;j++);
}

/* Block Erase Setup 0x20
   Block Erase Confirm 0xDO
   Block Erase 2 Write BA 0x20 Write BA 0xD0 
*/

/*Block Erase Operation can be extended*/
//adde is offest, not absolute address
int smc_nor_blk_erase(unsigned int addr)
{
   uint16_t status;
   int res = 0;
   if (top_boot==1)  { //top boot 
   if(addr==0x00000 || (addr==0x10000)|| (addr==0x20000) || (addr==0x30000) || (addr==0x40000) || (addr==0x50000))
   {
       writew(CFI_CMD_CLEAR_STATUS,SMC_NORFLASH_BASE_ADDR+addr);
       writew(CFI_CMD_BLK_ERASE,  SMC_NORFLASH_BASE_ADDR+addr);
       writew(CFI_CMD_BLK_ERASE_CNFM, SMC_NORFLASH_BASE_ADDR+addr);

	writew(CFI_CMD_READ_STATUS,SMC_NORFLASH_BASE_ADDR+addr);

	do {
		status = readw(SMC_NORFLASH_BASE_ADDR);
		udelay(10);
	} while (!(status & CFI_STATUS_READY));

	
	if (status & CFI_STATUS_ERASE_ERROR) {
		if (status & CFI_STATUS_VPP_LOW) {
			res = -EFAULT;
		} else if (status & CFI_STATUS_LOCKED_ERROR) {
			res = -EPERM;
		} else {
			res = -EFAULT;
		}
	} else {
		puts("erase done: at address=%x\r\n",addr);
		res=0;
	}
	writew(CFI_CMD_RESET,SMC_NORFLASH_BASE_ADDR);
   }//block start offset check
   }//top boot
   return res;
}

/* Block lock Setup 0x60
   Block Unlock 0xD0
   Block Unlock 2 Write BA 0x60 Write BA 0xD0
*/ 

/*Unlock the Block */
void smc_nor_blk_unlock(unsigned int addr)
{
   if (top_boot==1)  { //top boot 
   if(addr==0x00000 || (addr==0x10000)|| (addr==0x20000) || (addr==0x30000) || (addr==0x40000) || (addr==0x50000))
   {
       writew(CFI_CMD_BLK_LCK_STP, SMC_NORFLASH_BASE_ADDR+addr);
       writew(CFI_CMD_BLK_UNLCK, SMC_NORFLASH_BASE_ADDR+addr);
       writew(CFI_CMD_RESET,SMC_NORFLASH_BASE_ADDR);	
   } //block start offset check
   }//top boot
}

/*writing to the NOR Flash */
int smc_nor_write(void)
{
  unsigned int i,j,res=0;

  for (i=0;i< NOR_DEPTH;i++)
       write_data[i]=0x1592;
  
  for(j= 0,i=0; j< NOR_DEPTH ;j=j+2)
  {
	 smc_nor_blk_unlock(j);
	 smc_nor_blk_erase(j);

	 writew(CFI_CMD_CLEAR_STATUS,SMC_NORFLASH_BASE_ADDR);
	 writew(CFI_CMD_WRITE,((SMC_NORFLASH_BASE_ADDR+j)));
	 writew(write_data[i],((SMC_NORFLASH_BASE_ADDR+j)));
	 i++;
	 writew(CFI_CMD_READ_STATUS,SMC_NORFLASH_BASE_ADDR);
	uint16_t status;
	do {
		status = readw(SMC_NORFLASH_BASE_ADDR);
		udelay(10);
	} while (!(status & CFI_STATUS_READY));

	if (status & CFI_STATUS_PROGRAM_ERROR) {
			puts("error: in write at location=%d\r\n ",i);
		if (status & CFI_STATUS_VPP_LOW) {
			puts("vpp insufficient eror at location=%d\r\n",i);
			res = -EFAULT;
		} else if (status & CFI_STATUS_LOCKED_ERROR) {
			puts("block is lock-protected\r\n");
			res = -EPERM;
		} else {
			puts("unknown fault\r\n");
			res = -EFAULT;
		}
		goto err_reset;
	}
   }
   return res;
err_reset:
	writew(CFI_CMD_RESET,SMC_NORFLASH_BASE_ADDR);
	return res;
 }
 


int smc_nor_verify(void)
{
  int pass = 1,i=0; //default it is pass
  for (i=1; i< NOR_DEPTH; i++)
  {
     if (write_data[i] != read_data[i]) {
        pass = 0;
	printf("NOR Test: FAIL at Location=%d\r\n",i);
	break;
    }else
        pass = 1; //put the break point,here and check it should be 1;
	printf("NOR Test: PASS at Location=%d\r\n",i);
  }
  return pass;
}
