
#include<common.h>
#include<shikhara_map.h>
#include<shikhara_smc.h>

#define SRAM_DEPTH 1024
unsigned int write_data[SRAM_DEPTH],read_data[SRAM_DEPTH];

int smc_sram_init(void)
{
   //SMC0 clock reduceed from 133MHz to 100MHz
   writel(0x00000004, SHIKHARA_SYS_CTRL+SMC0_CLK_DIV_RATIO_OFFSET);
   writel(SMC_SRAM_SET_CYCLES , SHIKHARA_SMC_BASE + SMC_MC_SET_CYCLES);
   writel(SMC_SRAM_SET_OPMODE , SHIKHARA_SMC_BASE + SMC_MC_SET_OPMODE);
   writel(SMC_SRAM_DIRECT_CMD , SHIKHARA_SMC_BASE + SMC_MC_DIRECT_CMD);
   return(0);
}

/*Reading from SRAM */
void smc_sram_read(void)
{
  unsigned int i;
  for(i= 0; i < SRAM_DEPTH ;i=i+1 )
  {	 
         read_data[i] = readw(SMC_SRAM_BASE_ADDR+(2*i));
  }
}


/*writing to the SRAM */
void smc_sram_write(void)
{
  unsigned int i,j;
  for (i=0;i< SRAM_DEPTH;i++)
       write_data[i]=0x1592;
  
  for(j= 0,i=0; j< SRAM_DEPTH ;j=j+1,i++)
  {
	 writew(write_data[j],SMC_SRAM_BASE_ADDR+(2*i));
  }
}

int smc_sram_verify(void)
{
  int pass = 1,i=0; //default it is pass
  for (i=0; i< SRAM_DEPTH; i++)
  {
     if (write_data[i] != read_data[i]) {
        pass = 0;
	break;
    }else
        pass = 1; //put the break point,here and check it should be 1;
  }
  return pass;
}
