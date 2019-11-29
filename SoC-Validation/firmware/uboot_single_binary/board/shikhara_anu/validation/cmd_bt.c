#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/list.h>
#include <asm/arch/bluetooth/shikhara_bt_defines.h>
#include <asm/arch/bluetooth/shikhara_bt_irq_defines.h>
#include <asm/arch/bluetooth/shikhara_bt_register_defines.h>
#include <asm/arch/shikhara_map.h>

#define BTSS_CONFIG_ADDR 	0x5d5611b8
#define SOFT_RST_REG4_ADDR 	0x5d561054

int do_bt_read_write(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

  volatile unsigned short int data,wdata = 0 ;
  volatile unsigned int temp_data;
  volatile int i;
/*  // Giving Power on signal to BT from PMU 
  temp_data= readl (PMU_CONFIG_ADDR);
  temp_data= temp_data & ~(0x00000020);
  writel (PMU_CONFIG_ADDR ,temp_data);
  for(i=0;i<20;i++)
  {
  }
*/

  //BT System controller configuration to enable clks for BT and RF. 
  writel (0x6E, BTSS_SC_REG0);

  for(i=0;i<10;i++)
  {
  }

// Default Value Check

  data = readl (BTSS_BB_INQ_LAP2);  
  data = readl(BTSS_BB_LOOPBACK);  
  data = readl(BTSS_BB_LUT_1_UPPER);
  data = readl(BTSS_BB_LUT_1_LOWER);
  data = readl(BTSS_BB_LUT_2_LOWER);
  data = readl(BTSS_BB_LUT_2_UPPER);  
  data = readl(BTSS_BB_LUT_3_LOWER);
  data = readl(BTSS_BB_LUT_3_UPPER);  
  data = readl(BTSS_BB_LUT_4_LOWER);
  data = readl(BTSS_BB_LUT_4_UPPER);  
  data = readl(BTSS_BB_LUT_5_LOWER);
  data = readl(BTSS_BB_LUT_5_UPPER);  
  data = readl(BTSS_BB_LUT_6_LOWER);
  data = readl(BTSS_BB_LUT_6_UPPER);  
  data = readl(BTSS_BB_LUT_7_LOWER);
  data = readl(BTSS_BB_LUT_7_UPPER);  
  data = readl(BTSS_BB_RADIO_DELAY_H); 
  data = readl(BTSS_BB_RADIO_DELAY  ); 
  data = readl(BTSS_BB_RF_PHD_H     ); 
  data = readl(BTSS_BB_RF_PHD_F     ); 
  data = readl(BTSS_BB_TRIG_1STID   ); 
  data = readl(BTSS_BB_TRIG_RECFHS  ); 
  data = readl(BTSS_BB_POC_REG);
  data = readl(BTSS_BB_RADIO_SEL);
  data = readl(BTSS_BB_INQ_LAP2) ; //Add
  data = readl(BTSS_BB_SLV_PARITY1) ; //Add
  data = readl(BTSS_BB_SLV_PARITY2) ; //Add

  writel (0xdbd6,BTSS_BB_RADIO_DELAY_H); 
  writel (0xe5e0, BTSS_BB_RADIO_DELAY); 
  writel (0xa0a0, BTSS_BB_RF_PHD_H); 
  writel (0xa0a0, BTSS_BB_RF_PHD_F); 
  writel (0x0135, BTSS_BB_CORR_DIS_H); 
  writel (0x0142, BTSS_BB_TRIG_1STID); 
  writel (0x0142, BTSS_BB_TRIG_RECFHS); 
  writel (0x013e, BTSS_BB_TRIG_2NDID); 


  //Radio Configuration to start BTBB 
  writel(0x00b8, BTSS_BB_POC_REG);
  writel (0x0083, BTSS_BB_RADIO_SEL);
  writel (0x0183, BTSS_BB_RADIO_SEL);
  data =  readl (BTSS_BB_INQ_LAP2);
  printf("BTSS_BB_INQ_LAP2 : 0x%08x\n", data);  //Add
  writel(0x0000, BTSS_BB_INQ_LAP2) ; //Add
  writel(0x5a82, BTSS_BB_SLV_PARITY1) ; //Add
  writel(0x7c4e, BTSS_BB_SLV_PARITY2) ; //Add
  data =  readl (BTSS_BB_INQ_LAP2);
  printf("BTSS_BB_INQ_LAP2 : 0x%08x\n", data);  //Add
  writel(0x0000, BTSS_BB_INQ_LAP2) ; //Add
  writel(0xC000, BTSS_BB_LOOPBACK) ; //Add
  writel (0xa0cd, BTSS_BB_LUT_1_LOWER);
  data = readl(BTSS_BB_LUT_1_LOWER);
  printf("BTSS_BB_LUT_1_LOWER : 0x%08x\n", data);  //Add
  writel (0x0025, BTSS_BB_LUT_1_UPPER);
  data = readl(BTSS_BB_LUT_1_UPPER);
  printf("BTSS_BB_LUT_1_UPPER : 0x%08x\n", data);  //Add
  
  writel (0x1023, BTSS_BB_LUT_2_LOWER);
  data = readl (BTSS_BB_LUT_2_LOWER);  //Add
  printf("BTSS_BB_LUT_2_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_2_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_2_UPPER);
  printf("BTSS_BB_LUT_2_UPPER : 0x%08x\n", data);  //Add
  writel(0xa0cd, BTSS_BB_LUT_3_LOWER) ; //Add
  data = readl(BTSS_BB_LUT_3_LOWER);
  printf("BTSS_BB_LUT_3_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_3_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_3_UPPER) ; //Add
  printf("BTSS_BB_LUT_3_UPPER : 0x%08x\n", data);  //Add
  
  writel(0x1023, BTSS_BB_LUT_4_LOWER) ; //Add
  data = readl(BTSS_BB_LUT_4_LOWER) ; //Add
  printf("BTSS_BB_LUT_4_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_4_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_4_UPPER) ; //Add
  printf("BTSS_BB_LUT_4_UPPER : 0x%08x\n", data);  //Add
  
  writel(0xa0cd, BTSS_BB_LUT_5_LOWER) ; //Add
  data = readl(BTSS_BB_LUT_5_LOWER) ; //Add
  printf("BTSS_BB_LUT_5_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_5_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_5_UPPER) ; //Add
  printf("BTSS_BB_LUT_5_UPPER : 0x%08x\n", data);  //Add
  
  writel(0x1023, BTSS_BB_LUT_6_LOWER) ; //Add
  data = readl(BTSS_BB_LUT_6_LOWER) ; //Add
  printf("BTSS_BB_LUT_6_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_6_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_6_UPPER) ; //Add
  printf("BTSS_BB_LUT_6_UPPER : 0x%08x\n", data);  //Add
  writel(0xa0cd, BTSS_BB_LUT_7_LOWER) ; //Add
  data = readl(BTSS_BB_LUT_7_LOWER) ; //Add
  printf("BTSS_BB_LUT_7_LOWER : 0x%08x\n", data);  //Add
  writel(0x0025, BTSS_BB_LUT_7_UPPER) ; //Add
  data = readl(BTSS_BB_LUT_7_UPPER) ; //Add
  printf("BTSS_BB_LUT_5_UPPER : 0x%08x\n", data);  //Add

/*  // Applying soft reset to BT
  writel(0x0000006D, SOFT_RST_REG4_ADDR) ; 
  for(i=0;i<10;i++)
  {
  }
  writel(0x00000005, SOFT_RST_REG4_ADDR) ; 
  for(i=0;i<10;i++)
  {
  }
*/

  data = readl(BTSS_BB_INQ_LAP2);  
  data = readl(BTSS_BB_LOOPBACK);  
  data = readl(BTSS_BB_LUT_1_UPPER);
  data = readl(BTSS_BB_LUT_1_LOWER);
  data = readl(BTSS_BB_LUT_2_LOWER);
  data = readl(BTSS_BB_LUT_2_UPPER);  
  data = readl(BTSS_BB_LUT_3_LOWER);
  data = readl(BTSS_BB_LUT_3_UPPER);  
  data = readl(BTSS_BB_LUT_4_LOWER);
  data = readl(BTSS_BB_LUT_4_UPPER);  
  data = readl(BTSS_BB_LUT_5_LOWER);
  data = readl(BTSS_BB_LUT_5_UPPER);  
  data = readl(BTSS_BB_LUT_6_LOWER);
  data = readl(BTSS_BB_LUT_6_UPPER);  
  data = readl(BTSS_BB_LUT_7_LOWER);
  data = readl(BTSS_BB_LUT_7_UPPER);  
  data = readl(BTSS_BB_RADIO_DELAY_H); 
  data = readl(BTSS_BB_RADIO_DELAY  ); 
  data = readl(BTSS_BB_RF_PHD_H     ); 
  data = readl(BTSS_BB_RF_PHD_F     ); 
  data = readl(BTSS_BB_TRIG_1STID   ); 
  data = readl(BTSS_BB_TRIG_RECFHS  ); 
  data = readl(BTSS_BB_POC_REG);
  data = readl(BTSS_BB_RADIO_SEL);
  data = readl(BTSS_BB_INQ_LAP2) ; //Add
  data = readl(BTSS_BB_SLV_PARITY1) ; //Add
  data = readl(BTSS_BB_SLV_PARITY2) ; //Add

/*
  //GPIO Configuration to terminate the test  
  writel(GPIO_AFSEL,0X00);  //Configure the Auxillary port register to set software mode
  writel(GPIO_DIR  ,0XFF);  //Configure the direction register to set gpio lines as output
  writel(GPIO_DATA ,0XAA);  //Configure the Data register this will come on GPOUT signal
  writel(GPIO_DATA ,0XA0);  //Configure the Data register this is for the bt_env transaction end
*/

	return 0;
}

// Added by Achyuth
void bt_dumpregs_err(void)
{
	 char *print = NULL;
	volatile int i;

	dbg( "BT: ============== REGISTER DUMP ==============\n");
	for(i=0;i < 0x2AC; i=i+0x04)
	{
	  sprintf(print,"BTSS_BB_BASE_ADDR + 0x%03x", i);
	  printf(strcat(print,"=0x%08x\n"), readl(BTSS_BB_BASE_ADDR+i));
	}

}
int do_bt_dump (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){
        bt_dumpregs_err();
        return 0;
        }

U_BOOT_CMD(
                bt_rdrw, 1, 1, do_bt_read_write,
                "Bluetooth Test",
                "Bluetooth Controller Validation"
          );
U_BOOT_CMD(
                bttest_regdump, 1, 1, do_bt_dump,
                "Bluetooth Register dump",
                "Bluetooth Controller Validation"
          );

