/* Basic Synopsys MSHC Controller test for reading SD Card Manufacturer ID and 
   printing it on to the console */
#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_clock.h>
#define DEBUG_DWMMC
#ifdef DEBUG_DWMMC
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
/***************************************************************************
 SDMMC base address and SDMMC register base address
**************************************************************************/

#define SDMMC_BASE_ADDR         SHIKHARA_MMC0_BASE // SDMMC Base Address
    
#define DDR_SDMMC_TX_ADDR	SHIKHARA_MMC_RAM_START	// DDR Address

#define OCR_BUSY                0x80000000

#define SDMMC_GET_FCNT(x)               (((x)>>17) & 0x1FFF)
#define FIFO_DEPTH      0x80


#define SDMMC_CTRL              ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x000))
#define SDMMC_PWREN             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x004))
#define SDMMC_CLKDIV            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x008))
#define SDMMC_CLKSRC            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x00C))
#define SDMMC_CLKENA            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x010))
#define SDMMC_TMOUT             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x014))
#define SDMMC_CTYPE             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x018))
#define SDMMC_BLKSIZ            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x01C))
#define SDMMC_BYTCNT            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x020))
#define SDMMC_INTMASK           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x024))
#define SDMMC_CMDARG            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x028))
#define SDMMC_CMD               ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x02C))
#define SDMMC_RESP0             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x030))
#define SDMMC_RESP1             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x034))
#define SDMMC_RESP2             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x038))
#define SDMMC_RESP3             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x03C))
#define SDMMC_MINTSTS           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x040))
#define SDMMC_RINTSTS           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x044))
#define SDMMC_STATUS            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x048))
#define SDMMC_FIFOTH            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x04C))
#define SDMMC_CDETECT           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x050))
#define SDMMC_WRTPRT            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x054))
#define SDMMC_GPIO              ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x058))
#define SDMMC_TCBCNT            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x05C))
#define SDMMC_TBBCNT            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x060))
#define SDMMC_DEBNCE            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x064))
#define SDMMC_USRID             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x068))
#define SDMMC_VERID             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x06C))
#define SDMMC_HCON              ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x070))
#define SDMMC_UHS_REG           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x074))
#define SDMMC_RST_n             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x078))
#define SDMMC_BMOD              ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x080))
#define SDMMC_PLDMND            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x084))
#define SDMMC_DBADDR            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x088))
#define SDMMC_IDSTS             ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x08C))
#define SDMMC_IDINTEN           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x090))
#define SDMMC_DSCADDR           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x094))
#define SDMMC_BUFADDR           ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x098))
#define SDMMC_CLKSEL            ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x09C))
#define SDMMC_CARDTHRCTL        ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x100))
#define SDMMC_BACK_END_POWER    ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x104))
#define SDMMC_UHS_REG_EXT       ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x108))
#define SDMMC_EMMC_DDR_REG      ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x10C))
#define SDMMC_FIFO              ((volatile unsigned int *)(SDMMC_BASE_ADDR + 0x200))

#define write_data(register_address,register_value) *((volatile unsigned int *) (register_address)) = ((unsigned int) (register_value))
#define read_data(register_address) *((volatile unsigned int *) (register_address))
#define UNUSABLE_CARD 		17 /* Unusable Card */
#define SD_VERSION		0x20000
#define SD_VER2			(SD_VERSION | 0x20)

typedef volatile unsigned int uint32;
uint32 sdmmc_read =0;
static int dwmmc_setup_clock(u32 freq)
{
	unsigned long sclk;
	u32 div;

	sclk = MMC_CLK; 
	for (div = 1; div < 255; div++) {
		if ((sclk / (2 * div)) <= freq) {
			break;
		}
	}

	write_data(SDMMC_CLKSRC ,0x00000000);
	write_data(SDMMC_CLKENA ,0x0);
	
	write_data(SDMMC_CLKDIV,div);

	return 0;
}


int do_dwmmcinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  uint32 version,rsp_r7,ocr,high_capacity,cid[4],csd[4],read_bl_len,i=0;
  volatile unsigned long long capacity;
  volatile unsigned long long csize,cmult;

  /* Clock Initialization */
dbg("DWMMCinfo start\n");
  dwmmc_setup_clock(400000);
dbg("DWMMCinfo clock set\n"); 
    /* Controller Initialization Sequence */
  write_data(SDMMC_PWREN  ,0X0000ffff);         // Power Enable register configuration to enable the card power
  write_data(SDMMC_INTMASK,0Xffffffff);         // Mask all the interrupts except Data Transfer Done interrupt
  write_data(SDMMC_CTRL, 0x7);			// As per Samsung specific code
  write_data(SDMMC_RINTSTS,0Xffffffff);         // Clear all the interrupt status if any bit set already
  write_data(SDMMC_TMOUT  ,0Xffffff40);         // Configure the Timeout register with 40 clock cycles

  write_data(SDMMC_CLKENA,0x0);			// As per Samsung specific code
  write_data(SDMMC_CLKSRC,0x0);			// As per Samsung specific code

  
  write_data(SDMMC_FIFOTH ,0X003f0040);         // FIFO Transmit and Receive WaterMark level is set as 256(100) and 25(FF)
  write_data(SDMMC_CTRL   ,0X00000010);         // Interrupt Enable bit([4]) is set in control register
  write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	          			        // and enable the command to card
  write_data(SDMMC_CLKENA ,0X0000ffff);         // Clock Enable bit is set to drive clock source to the card
  write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	          			        // and enable the command to card
  write_data(SDMMC_CTYPE  ,0X00000000);         // Configure CardType register for single bit data transfer enabling

dbg("DWMMCinfo configured card type to 1 bit\n");
  /***************** CMD 0 *********************/
  dbg("DWMMCinfo CMD 0 start\n");

  // For to identify the card type, make the card to enter into idle state (Send CMD0)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD0 to the card
  write_data(SDMMC_CMD   ,0X80000000);                              // Command register is configured to send CMD0 to the card
					        	  	     // i) Start Cmd bit is set([31])
                                                                     // ii) Command index is set as 0([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
  	{
		dbg("CMD0 response error \n");
		return -1;
  	}

  write_data(SDMMC_RINTSTS,0x00000004);      
 dbg("DWMMCinfo CMD 0 end\n");

  /***************** CMD 8 **********************/
dbg("DWMMCinfo CMD 8 start\n");
  // Check the Connected Card is SDSC or Not (Send CMD-8 to card, if response comes it is SDSC else non-SDSC card)
  write_data(SDMMC_CMDARG,0X000001aa);                              // Command Argument register is configured to send CMD8 to the card
  write_data(SDMMC_CMD   ,0X80000148);                              // Command register is configured to send CMD8 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
							             // iii) Response Expect bit is set([6])
							             // iv) Command index is set as 8([5:0])
  udelay(2000);

  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD8 response error \n");
		return -1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);      

  rsp_r7 = read_data(SDMMC_RESP0);

  if((rsp_r7 & 0xff) != 0xaa)
	return UNUSABLE_CARD;
  else
	version = SD_VER2;

  // The card is identified as SDSC.
 dbg("DWMMCinfo CMD 8 end\n");
 /***************** CMD 55 *********************/
 dbg("DWMMCinfo CMD 55 start\n");  

  // Check the Connected Card is SDHC or Not (Send ACMD-41 to card, if response comes it is SDHC else non-SDHC card)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD55(Application Command) to the card
  write_data(SDMMC_CMD   ,0X80000077);                              // Command register is configured to send CMD55 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 55([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD55 response error \n");
		return -1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);      
dbg("DWMMCinfo CMD 55  end\n");
  /*********************** CMD 41 **************/
  dbg("DWMMCinfo CMD 41 Start\n");

  write_data(SDMMC_CMDARG ,0X40300000);                             // Command Argument register is configured to send CMD41 to the card
  write_data(SDMMC_CMD    ,0X80000069);                             // Command register is configured to send CMD41 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 41([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD41 response error \n");
		return -1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);
 dbg("DWMMCinfo CMD 41  end\n");
  /***************** CMD 55 *********************/
 dbg("DWMMCinfo 2:CMD 55  start\n");
  // Check the Connected Card is SDHC or Not (Send ACMD-41 to card, if response comes it is SDHC else non-SDHC card)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD55(Application Command) to the card
  write_data(SDMMC_CMD   ,0X80000077);                              // Command register is configured to send CMD55 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 55([5:0])
  
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD55 response error \n");
		return 1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);      
  dbg("DWMMCinfo 2:CMD 55  end\n");
  /**************************** CMD 41 ************/
 dbg("DWMMCinfo 2:CMD 41  start\n");
  write_data(SDMMC_CMDARG ,0x40300000);					// As per Samsung code
  write_data(SDMMC_CMD    ,0X80000169);                             // Command register is configured to send CMD41 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 41([5:0])

  udelay(2000);
 while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD41 response error \n");
		return 1;
	}
  write_data(SDMMC_RINTSTS,0x00000004);     

  ocr = read_data(SDMMC_RESP0);
  high_capacity = ((ocr & 0x40000000) == 0x40000000);
 dbg("DWMMCinfo 2:CMD 41  end\n");

  /**************************** CMD 2 ***********/
dbg("DWMMCinfo CMD 2  start\n");
  
  // The Connected Card is identified as MMC (Send CMD-2 to initialize the card)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD2 to the card
  write_data(SDMMC_CMD   ,0X800001c2);                              // Command register is configured to send CMD2 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
								     // iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
							             // iv) Response Expect bit is set([6])
							             // v) Command index is set as 2([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD2 response error \n");
		return -1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);

  cid[0] = read_data(SDMMC_RESP3);
  cid[1] = read_data(SDMMC_RESP2);
  cid[2] = read_data(SDMMC_RESP1);
  cid[3] = read_data(SDMMC_RESP0);
 dbg("DWMMCinfo CMD 2  end\n");

  
  /************************** CMD 3 **************/
 dbg("DWMMCinfo CMD 3 start\n");
  
 // The Connected Card is identified as MMC (Send CMD-3 to initialize the card)
  write_data(SDMMC_CMDARG,0X00020000);                              // Command Argument register is configured to send CMD3 to the card
  write_data(SDMMC_CMD   ,0X80000143);                              // Command register is configured to send CMD3 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
							             // iii) Response Expect bit is set([6])
							             // iv) Command index is set as 3([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD3 response error \n");
		return -1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);
  dbg("DWMMCinfo CMD 3 end\n");

  
  /******************************* CMD9 **************/
dbg("DWMMCinfo CMD 9 start\n");

  write_data(SDMMC_CMDARG,0x12340000);				// Command Argument register is configured to send CMD3 to the card
  write_data(SDMMC_CMD,0x800001C9);				// Command register is configured to send CMD2 to the card
                                                                     // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
                                                                     // iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
                                                                     // iv) Response Expect bit is set([6])
                                                                     // v) Command index is set as 9([5:0])
  udelay(2000);

  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD3 response error \n");
		return 1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);      

  csd[0] = read_data(SDMMC_RESP3);
  csd[1] = read_data(SDMMC_RESP2);
  csd[2] = read_data(SDMMC_RESP1);
  csd[3] = read_data(SDMMC_RESP0);
 

  read_bl_len = 1 << ((csd[1] >> 16) & 0xf);

  csize = (csd[1] & 0x3f) << 16
			| (csd[2] & 0xffff0000) >> 16;
  cmult = 8;										 

  capacity = (csize + 1) << (cmult + 2);
  capacity *= read_bl_len;
 dbg("DWMMCinfo CMD 9 end\n");

  /***************************** CMD7 *********************/
dbg("DWMMCinfo CMD 7 start\n");
  // The Connected Card is identified as SDSC (Send CMD-7 to select the card0)
  write_data(SDMMC_CMDARG,0X00010000);                              // Command Argument register is configured to send CMD7 to the card
  write_data(SDMMC_CMD   ,0X80000047);                              // Command register is configured to send CMD7 to the card
 							             // i) Start Cmd bit is set([31])
							             // ii) Response Expect bit is set([6])
							             // iii) Command index is set as 7([5:0])

  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD7 response error \n");
		return -1;
	}
 
  write_data(SDMMC_RINTSTS,0x00000004);     

        printf("Manufacturer ID: %x\n", cid[0] >> 24);
	printf("OEM: %x\n", (cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", cid[0] & 0xff,
			(cid[1] >> 24), (cid[1] >> 16) & 0xff,
			(cid[1] >> 8) & 0xff, cid[1] & 0xff);

	printf("Rd Block Len: %d\n", read_bl_len);

	printf("%s version %d.%d\n",  "SD",
			(version >> 4) & 0xf, version & 0xf);

	printf("High Capacity: %s\n", high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	print_size(capacity, "\n");
	return 0;
}

int do_dwmmc_1_blk_tx(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  uint32 version,rsp_r7,ocr,high_capacity,cid[4],csd[4],read_bl_len,i=0;
  volatile unsigned long long capacity;
  volatile unsigned long long csize,cmult;

  /* Clock Initialization */
dbg("DWMMCinfo start\n");
  dwmmc_setup_clock(400000);
dbg("DWMMCinfo clock set\n"); 
    /* Controller Initialization Sequence */
  write_data(SDMMC_PWREN  ,0X0000ffff);         // Power Enable register configuration to enable the card power
  write_data(SDMMC_INTMASK,0Xffffffff);         // Mask all the interrupts except Data Transfer Done interrupt
  write_data(SDMMC_CTRL, 0x7);			// As per Samsung specific code
  write_data(SDMMC_RINTSTS,0Xffffffff);         // Clear all the interrupt status if any bit set already
  write_data(SDMMC_TMOUT  ,0Xffffff40);         // Configure the Timeout register with 40 clock cycles

  write_data(SDMMC_CLKENA,0x0);			// As per Samsung specific code
  write_data(SDMMC_CLKSRC,0x0);			// As per Samsung specific code

  
  write_data(SDMMC_FIFOTH ,0X003f0040);         // FIFO Transmit and Receive WaterMark level is set as 256(100) and 25(FF)
  write_data(SDMMC_CTRL   ,0X00000010);         // Interrupt Enable bit([4]) is set in control register
  write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	          			        // and enable the command to card
  write_data(SDMMC_CLKENA ,0X0000ffff);         // Clock Enable bit is set to drive clock source to the card
  write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	          			        // and enable the command to card
  write_data(SDMMC_CTYPE  ,0X00000000);         // Configure CardType register for single bit data transfer enabling

dbg("DWMMCinfo configured card type to 1 bit\n");
  /***************** CMD 0 *********************/
  dbg("DWMMCinfo CMD 0 start\n");

  // For to identify the card type, make the card to enter into idle state (Send CMD0)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD0 to the card
  write_data(SDMMC_CMD   ,0X80000000);                              // Command register is configured to send CMD0 to the card
					        	  	     // i) Start Cmd bit is set([31])
                                                                     // ii) Command index is set as 0([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
  	{
		dbg("CMD0 response error \n");
		return -1;
  	}

  write_data(SDMMC_RINTSTS,0x00000004);      
 dbg("DWMMCinfo CMD 0 end\n");

  /***************** CMD 8 **********************/
dbg("DWMMCinfo CMD 8 start\n");
  // Check the Connected Card is SDSC or Not (Send CMD-8 to card, if response comes it is SDSC else non-SDSC card)
  write_data(SDMMC_CMDARG,0X000001aa);                              // Command Argument register is configured to send CMD8 to the card
  write_data(SDMMC_CMD   ,0X80000148);                              // Command register is configured to send CMD8 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
							             // iii) Response Expect bit is set([6])
							             // iv) Command index is set as 8([5:0])
  udelay(2000);

  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD8 response error \n");
		return -1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);      

  rsp_r7 = read_data(SDMMC_RESP0);

  if((rsp_r7 & 0xff) != 0xaa){
	dbg("UNUSABLE_CARD\n");
	return UNUSABLE_CARD;
	}
  else
	version = SD_VER2;

  // The card is identified as SDSC.
 dbg("DWMMCinfo CMD 8 end\n");
  /***************** CMD 55 *********************/
int timeout=1000;
        do{
 dbg("DWMMCinfo 2:CMD 55  start\n");
  // Check the Connected Card is SDHC or Not (Send ACMD-41 to card, if response comes it is SDHC else non-SDHC card)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD55(Application Command) to the card
  write_data(SDMMC_CMD   ,0XA0002177);                              // Command register is configured to send CMD55 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 55([5:0])
  
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD55 response error \n");
		return 1;
	}

  write_data(SDMMC_RINTSTS,0x00000004);      
  dbg("DWMMCinfo 2:CMD 55  end\n");
  /**************************** CMD 41 ************/
 dbg("DWMMCinfo 2:CMD 41  start\n");
  write_data(SDMMC_CMDARG ,0x40300000);					// As per Samsung code
  write_data(SDMMC_CMD    ,0XA0002069);                             // Command register is configured to send CMD41 to the card
     						             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
     						             // iii) Response Expect bit is set([6])
     						             // iv) Command index is set as 41([5:0])

  udelay(2000);
 while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD41 response error \n");
		return 1;
	}
  write_data(SDMMC_RINTSTS,0x00000004);     

  ocr = read_data(SDMMC_RESP0);
}while(!(ocr & OCR_BUSY) && timeout--);
  high_capacity = ((ocr & 0x40000000) == 0x40000000);
 dbg("DWMMCinfo 2:CMD 41  end\n");

  /**************************** CMD 2 ***********/
dbg("DWMMCinfo CMD 2  start\n");
  
  // The Connected Card is identified as MMC (Send CMD-2 to initialize the card)
  write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD2 to the card
  write_data(SDMMC_CMD   ,0XA00001c2);                              // Command register is configured to send CMD2 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
								     // iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
							             // iv) Response Expect bit is set([6])
							             // v) Command index is set as 2([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD2 response error \n");
		return -1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);

  cid[0] = read_data(SDMMC_RESP3);
  cid[1] = read_data(SDMMC_RESP2);
  cid[2] = read_data(SDMMC_RESP1);
  cid[3] = read_data(SDMMC_RESP0);
 dbg("DWMMCinfo CMD 2  end\n");

  
  /************************** CMD 3 **************/
 dbg("DWMMCinfo CMD 3 start\n");
  
 // The Connected Card is identified as MMC (Send CMD-3 to initialize the card)
  write_data(SDMMC_CMDARG,0X00020000);                              // Command Argument register is configured to send CMD3 to the card
  write_data(SDMMC_CMD   ,0X80000143);                              // Command register is configured to send CMD3 to the card
							             // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
							             // iii) Response Expect bit is set([6])
							             // iv) Command index is set as 3([5:0])
  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD3 response error \n");
		return -1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);
  dbg("DWMMCinfo CMD 3 end\n");

  
  /******************************* CMD9 **************/
dbg("DWMMCinfo CMD 9 start\n");

  write_data(SDMMC_CMDARG,0xAAAA0000);				// Command Argument register is configured to send CMD3 to the card
  write_data(SDMMC_CMD,0xA000021C9);				// Command register is configured to send CMD2 to the card
                                                                     // i) Start Cmd bit is set([31])
                                                                     // ii) Check Response CRC bit is set([8])
                                                                     // iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
                                                                     // iv) Response Expect bit is set([6])
                                                                     // v) Command index is set as 9([5:0])
  udelay(2000);

  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD3 response error \n");
		return 1;
	}										 

  write_data(SDMMC_RINTSTS,0x00000004);      

  csd[0] = read_data(SDMMC_RESP3);
  csd[1] = read_data(SDMMC_RESP2);
  csd[2] = read_data(SDMMC_RESP1);
  csd[3] = read_data(SDMMC_RESP0);
 

  read_bl_len = 1 << ((csd[1] >> 16) & 0xf);

  csize = (csd[1] & 0x3f) << 16
			| (csd[2] & 0xffff0000) >> 16;
  cmult = 8;										 

  capacity = (csize + 1) << (cmult + 2);
  capacity *= read_bl_len;
 dbg("DWMMCinfo CMD 9 end\n");

  /***************************** CMD7 *********************/
dbg("DWMMCinfo CMD 7 start\n");
  // The Connected Card is identified as SDSC (Send CMD-7 to select the card0)
  write_data(SDMMC_CMDARG,0XAAAA0000);                              // Command Argument register is configured to send CMD7 to the card
  write_data(SDMMC_CMD   ,0XA0002147);                              // Command register is configured to send CMD7 to the card
 							             // i) Start Cmd bit is set([31])
							             // ii) Response Expect bit is set([6])
							             // iii) Command index is set as 7([5:0])

  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
 	{  
		dbg("CMD7 response error \n");
		return -1;
	}
 
  write_data(SDMMC_RINTSTS,0x00000004);     


  udelay(2000);
 write_data(SDMMC_CTYPE  ,0X00000000);                    	     dbg(" Single bit data transfer is set in the Card Type register\n");
							
  sdmmc_read                    = read_data(SDMMC_UHS_REG);         // Read UHS REG for knowing DDR data transfer and Voltage supplied is 3.3/1.8V
  write_data(SDMMC_UHS_REG,0X00000000);                             // Write UHS REG for NON_DDR data transfer and Voltage supplied as 3.3V
  write_data(SDMMC_BLKSIZ ,0X00000200);                             // Block Size register is configured as 512 bytes data transfer
  write_data(SDMMC_BYTCNT,0X00000200);                              // Byte Count register is configured as 512 bytes to be counted when data transfer is in progress

  // The Connected Card is identified as MMC (Send CMD-16 to inform the card the data transfer is 512 bytes)
     write_data(SDMMC_CMDARG,0X00000200);                              dbg(" Command Argument register is configured to send CMD16 to the card\n");
       write_data(SDMMC_CMD   ,0XA0002150);                              // Command register is configured to send CMD16 to the card
                                                                            // i) Start Cmd bit is set([31])
                                                                              // iv) Response Expect bit is set([6])
                                                                              // v) Command index is set as 16([5:0])

  udelay(2000);
  while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
        {
                dbg("CMD16 response error \n");
                return 1;
        }

  write_data(SDMMC_RINTSTS,0x00000004);


 write_data(SDMMC_CTRL,0x00000012);                                dbg(" Interrupt Enable bit is set([4]) in the Control register\n");
                                                                     // FIFO Reset bit is set([1]) in the Control register

  // Send Single Block Write Data Transfer Command (Send CMD-24 to inform the card to write 512 bytes in the Memory)
  write_data(SDMMC_CMDARG,0X00000000);                              dbg("Command Argument register is configured to send CMD24 to the card\n");
  write_data(SDMMC_CMD   ,0XA0002758);                              // Command register is configured to send CMD24 to the card
                                                                     // i) Start Cmd bit is set([31])
                                                                     // ii) Write Command bit is set([10])
                                                                     // iii) Data Expected bit is set([9])
                                                                     // iv) Check Response CRC bit is set([8])
                                                                     // v) Response Expect bit is set([6])
                                                                     // vi) Command index is set as 24([5:0])

  udelay(2000);
while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
        {
                dbg("CMD24 response error \n");
                return 1;
        }

write_data(SDMMC_RINTSTS,0x00000004);

  udelay(2000);
while(!(read_data(SDMMC_RINTSTS) & 0x00000010) == 0x00000010)
        {
                dbg("FIFO reset error \n");
                return 1;
        }

write_data(SDMMC_RINTSTS,0x00000010);
dbg("tx 1 started\n");

timeout=1000;
        unsigned int data_sz=512,*ram_tx_addr = (unsigned int *)malloc(512*sizeof(char));
	printf("writing from address 0x%X\n",ram_tx_addr);
        while(data_sz>0)
        {
                if(FIFO_DEPTH - SDMMC_GET_FCNT(read_data(SDMMC_STATUS)) ){
                write_data(SDMMC_FIFO, *ram_tx_addr);
                ram_tx_addr++;
                data_sz-=4;
                }
                        //dbg("data TF :intsts = 0x%X \n",read_data(SDMMC_RINTSTS));
        }
        do
        {
                if(read_data(SDMMC_RINTSTS)&0x8){
                        dbg("DTO done\n");
                        break;
                }
        udelay(200);
        }while(--timeout);
        if(timeout==0)
                dbg("DTO failed\n");
                dbg("DTO timeout %d\n",timeout);

/*
for(i=0; i<=255; i=i+4)
    {
     sdmmc_read = read_data (DDR_SDMMC_TX_ADDR + i);
     write_data((SDMMC_FIFO),sdmmc_read);

    }

  udelay(2000);
while(!(read_data(SDMMC_RINTSTS) & 0x00000010) == 0x00000010)
        {
                dbg("FIFO reset error \n");
                return 1;
        }

write_data(SDMMC_RINTSTS,0x00000010);

dbg("tx 2 started\n");
for(i=256; i<=511; i=i+4)
    {
     sdmmc_read = read_data (DDR_SDMMC_TX_ADDR + i);
     write_data((SDMMC_FIFO),sdmmc_read);
     
    }

  udelay(2000);
while(!(read_data(SDMMC_RINTSTS) & 0x00000008) == 0x00000008)
        {
                dbg("FIFO xfer over error \n");
                return 1;
        }

write_data(SDMMC_RINTSTS,0x00000008);

*/
  udelay(2000);


dbg("tx done\n");
return 0;
}
int do_dwmmc_1_blk_rx(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint32 version,rsp_r7,ocr,high_capacity,cid[4],csd[4],read_bl_len,i=0;
	volatile unsigned long long capacity;
	volatile unsigned long long csize,cmult;

	/* Clock Initialization */
	dbg("DWMMCinfo start\n");
	dwmmc_setup_clock(400000);
	dbg("DWMMCinfo clock set\n"); 
	/* Controller Initialization Sequence */
	write_data(SDMMC_PWREN  ,0X0000ffff);         // Power Enable register configuration to enable the card power
	write_data(SDMMC_INTMASK,0Xffffffff);         // Mask all the interrupts except Data Transfer Done interrupt
	write_data(SDMMC_CTRL, 0x7);			// As per Samsung specific code
	write_data(SDMMC_RINTSTS,0Xffffffff);         // Clear all the interrupt status if any bit set already
	write_data(SDMMC_TMOUT  ,0xFFFFFFFF);         // Configure the Timeout register with 40 clock cycles

	write_data(SDMMC_CLKENA,0x0);			// As per Samsung specific code
	write_data(SDMMC_CLKSRC,0x0);			// As per Samsung specific code


	write_data(SDMMC_FIFOTH ,0X003f0040);         // FIFO Transmit and Receive WaterMark level is set as 256(100) and 25(FF)
	write_data(SDMMC_CTRL   ,0X00000010);         // Interrupt Enable bit([4]) is set in control register
	write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	// and enable the command to card
	write_data(SDMMC_CLKENA ,0X0000ffff);         // Clock Enable bit is set to drive clock source to the card
	write_data(SDMMC_CMD    ,0X80202000);         // Command register is configured to update the clock registers 
	// and enable the command to card
	write_data(SDMMC_CTYPE  ,0X00000000);         // Configure CardType register for single bit data transfer enabling
	write_data(SDMMC_CLKSEL,0x03000000); 

	dbg("DWMMCinfo configured card type to 1 bit\n");
	/***************** CMD 0 *********************/
	dbg("DWMMCinfo CMD 0 start\n");

	// For to identify the card type, make the card to enter into idle state (Send CMD0)
	write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD0 to the card
	write_data(SDMMC_CMD   ,0XA0000000);                              // Command register is configured to send CMD0 to the card
	// i) Start Cmd bit is set([31])
	// ii) Command index is set as 0([5:0])
	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{
		dbg("CMD0 response error \n");
		return -1;
	}

	dbg("CMD0:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));
	write_data(SDMMC_RINTSTS,0x00000004);      
	dbg("DWMMCinfo CMD 0 end\n");

	/***************** CMD 8 **********************/
	dbg("DWMMCinfo CMD 8 start\n");
	// Check the Connected Card is SDSC or Not (Send CMD-8 to card, if response comes it is SDSC else non-SDSC card)
	write_data(SDMMC_CMDARG,0X000001aa);                              // Command Argument register is configured to send CMD8 to the card
	write_data(SDMMC_CMD   ,0XA0000148);                              // Command register is configured to send CMD8 to the card
	// i) Start Cmd bit is set([31])
	// ii) Check Response CRC bit is set([8])
	// iii) Response Expect bit is set([6])
	// iv) Command index is set as 8([5:0])
	udelay(2000);

	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{  
		dbg("CMD8 response error \n");
		return -1;
	}
	dbg("CMD8:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);      

	rsp_r7 = read_data(SDMMC_RESP0);

	if((rsp_r7 & 0xff) != 0xaa)
		return UNUSABLE_CARD;
	else
		version = SD_VER2;

	// The card is identified as SDSC.
	dbg("DWMMCinfo CMD 8 end\n");
	/***************** CMD 55 *********************/
	int timeout=1000;
	do{
		dbg("DWMMCinfo CMD 55 start\n");  

		// Check the Connected Card is SDHC or Not (Send ACMD-41 to card, if response comes it is SDHC else non-SDHC card)
		write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD55(Application Command) to the card
		write_data(SDMMC_CMD   ,0XA0002177);                              // Command register is configured to send CMD55 to the card
		// i) Start Cmd bit is set([31])
		// ii) Check Response CRC bit is set([8])
		// iii) Response Expect bit is set([6])
		// iv) Command index is set as 55([5:0])
		udelay(2000);
		while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
		{  
			dbg("CMD55 response error \n");
			return -1;
		}
		dbg("CMD55:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));
		dbg("CMD55:response = 0x%X \n",read_data(SDMMC_RESP0));

		write_data(SDMMC_RINTSTS,0x00000004);      
		dbg("DWMMCinfo CMD 55  end\n");
		/*********************** CMD 41 **************/
		dbg("DWMMCinfo CMD 41 Start\n");

		write_data(SDMMC_CMDARG ,0X40300000);                             // Command Argument register is configured to send CMD41 to the card
		write_data(SDMMC_CMD    ,0XA0002069);                             // Command register is configured to send CMD41 to the card
		// i) Start Cmd bit is set([31])
		// ii) Check Response CRC bit is set([8])
		// iii) Response Expect bit is set([6])
		// iv) Command index is set as 41([5:0])

		udelay(2000);
		while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
		{  
			dbg("CMD41 response error \n");
			return -1;
		}
		dbg("CMD41:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

		write_data(SDMMC_RINTSTS,0x00000004);
		dbg("DWMMCinfo CMD 41  end\n");
		ocr = read_data(SDMMC_RESP0);

	}while(!(ocr & OCR_BUSY) && timeout--);


	/**************************** CMD 2 ***********/
	dbg("DWMMCinfo CMD 2  start\n");

	// The Connected Card is identified as MMC (Send CMD-2 to initialize the card)
	write_data(SDMMC_CMDARG,0X00000000);                              // Command Argument register is configured to send CMD2 to the card
	write_data(SDMMC_CMD   ,0XA00001c2);                              // Command register is configured to send CMD2 to the card
	// i) Start Cmd bit is set([31])
	// ii) Check Response CRC bit is set([8])
	// iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
	// iv) Response Expect bit is set([6])
	// v) Command index is set as 2([5:0])
	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{  
		dbg("CMD2 response error \n");
		return -1;
	}										 
	dbg("CMD2:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);

	cid[0] = read_data(SDMMC_RESP3);
	cid[1] = read_data(SDMMC_RESP2);
	cid[2] = read_data(SDMMC_RESP1);
	cid[3] = read_data(SDMMC_RESP0);
	dbg("DWMMCinfo CMD 2  end\n");


	/************************** CMD 3 **************/
	dbg("DWMMCinfo CMD 3 start\n");

	// The Connected Card is identified as MMC (Send CMD-3 to initialize the card)
	write_data(SDMMC_CMDARG,0X00020000);                              // Command Argument register is configured to send CMD3 to the card
	write_data(SDMMC_CMD   ,0XA0000143);                              // Command register is configured to send CMD3 to the card
	// i) Start Cmd bit is set([31])
	// ii) Check Response CRC bit is set([8])
	// iii) Response Expect bit is set([6])
	// iv) Command index is set as 3([5:0])
	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{  
		dbg("CMD3 response error \n");
		return -1;
	}										 
	dbg("CMD3:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);
	dbg("DWMMCinfo CMD 3 end\n");


	/******************************* CMD9 **************/
	dbg("DWMMCinfo CMD 9 start\n");

	write_data(SDMMC_CMDARG,0xAAAA0000);				// Command Argument register is configured to send CMD3 to the card
	write_data(SDMMC_CMD,0xA000021C9);				// Command register is configured to send CMD2 to the card
	// i) Start Cmd bit is set([31])
	// ii) Check Response CRC bit is set([8])
	// iii) Response Length bit is set 1([7]). Controller expects long response from card(136 bit)
	// iv) Response Expect bit is set([6])
	// v) Command index is set as 9([5:0])
	udelay(2000);

	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{  
		dbg("CMD9 response error \n");
		return 1;
	}										 
	dbg("CMD9:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);      

	csd[0] = read_data(SDMMC_RESP3);
	csd[1] = read_data(SDMMC_RESP2);
	csd[2] = read_data(SDMMC_RESP1);
	csd[3] = read_data(SDMMC_RESP0);


	read_bl_len = 1 << ((csd[1] >> 16) & 0xf);

	csize = (csd[1] & 0x3f) << 16
		| (csd[2] & 0xffff0000) >> 16;
	cmult = 8;										 

	capacity = (csize + 1) << (cmult + 2);
	capacity *= read_bl_len;
	dbg("DWMMCinfo CMD 9 end\n");

	/***************************** CMD7 *********************/
	dbg("DWMMCinfo CMD 7 start\n");
	// The Connected Card is identified as SDSC (Send CMD-7 to select the card0)
	write_data(SDMMC_CMDARG,0XAAAA0000);                              // Command Argument register is configured to send CMD7 to the card
	write_data(SDMMC_CMD   ,0XA0002147);                              // Command register is configured to send CMD7 to the card
	// i) Start Cmd bit is set([31])
	// ii) Response Expect bit is set([6])
	// iii) Command index is set as 7([5:0])

	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{  
		dbg("CMD7 response error \n");
		return -1;
	}
	dbg("CMD7:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);     


	udelay(2000);
	write_data(SDMMC_CTYPE  ,0X00000000);                    	     dbg(" Single bit data transfer is set in the Card Type register\n");

	sdmmc_read                    = read_data(SDMMC_UHS_REG);         // Read UHS REG for knowing DDR data transfer and Voltage supplied is 3.3/1.8V
	write_data(SDMMC_UHS_REG,0X00000000);                             // Write UHS REG for NON_DDR data transfer and Voltage supplied as 3.3V
	write_data(SDMMC_BLKSIZ ,0X00000200);                             // Block Size register is configured as 512 bytes data transfer
	write_data(SDMMC_BYTCNT,0X00000200);                              // Byte Count register is configured as 512 bytes to be counted when data transfer is in progress

	// The Connected Card is identified as MMC (Send CMD-16 to inform the card the data transfer is 512 bytes)
	write_data(SDMMC_CMDARG,0X00000200);                              dbg(" Command Argument register is configured to send CMD16 to the card\n");
	write_data(SDMMC_CMD   ,0XA0002150);                              // Command register is configured to send CMD16 to the card
	// i) Start Cmd bit is set([31])
	// iv) Response Expect bit is set([6])
	// v) Command index is set as 16([5:0])

	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{
		dbg("CMD16 response error \n");
		return 1;
	}
	dbg("CMD16:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);


	write_data(SDMMC_CTRL,0x00000012);                                dbg(" Interrupt Enable bit is set([4]) in the Control register\n");
	// FIFO Reset bit is set([1]) in the Control register

	// Send Single Block Write Data Transfer Command (Send CMD-24 to inform the card to write 512 bytes in the Memory)
	write_data(SDMMC_CMDARG,0X00000000);                              dbg("Command Argument register is configured to send CMD17 to the card\n");
	write_data(SDMMC_CMD   ,0XA0002351);                              // Command register is configured to send CMD24 to the card
	// i) Start Cmd bit is set([31])
	// ii) Write Command bit is set([10])
	// iii) Data Expected bit is set([9])
	// iv) Check Response CRC bit is set([8])
	// v) Response Expect bit is set([6])
	// vi) Command index is set as 24([5:0])

	udelay(2000);
	while(!(read_data(SDMMC_RINTSTS) & 0x00000004) == 0x00000004)
	{
		dbg("CMD17 response error \n");
		return 1;
	}
	dbg("CMD17:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));

	write_data(SDMMC_RINTSTS,0x00000004);

	udelay(2000);
	dbg("CMD17i after delay:intsts = 0x%X \n",read_data(SDMMC_RINTSTS));
	while(!(read_data(SDMMC_RINTSTS) & 0x00000010) == 0x00000010)
	{
		dbg("FIFO reset error \n");
		return 1;
	}

	write_data(SDMMC_RINTSTS,0x00000010);
	printf("RX started\n");
	timeout=1000;
	unsigned int data_sz=512,*ram_rx_addr = (unsigned int *)malloc(512*sizeof(char));
	printf("reading from address 0x%X\n",ram_rx_addr);
	while(data_sz>0)
	{
		if(SDMMC_GET_FCNT(read_data(SDMMC_STATUS)) ){
		*ram_rx_addr=read_data(SDMMC_FIFO);
		ram_rx_addr++;
		data_sz-=4;
		}
			dbg("data TF :intsts = 0x%X \n",read_data(SDMMC_RINTSTS));
	}
	do
	{
		if(read_data(SDMMC_RINTSTS)&0x8){
			dbg("DTO done\n");
			break;
		}
	udelay(200);
	}while(--timeout);
	if(timeout==0)
		printf("DTO failed\n");
		dbg("DTO timeout %d\n",timeout);
			
	
	printf("RX done\n");
return 0;
}

void dwmci_dumpregs_err(void)
{
	dbg( "dwmci: ============== REGISTER DUMP ==============\n");
	dbg( "dwmci: SDMMC_CTRL:      0x%08x\n",
		read_data(SDMMC_CTRL));
	dbg( "dwmci: SDMMC_PWREN:     0x%08x\n",
		read_data(SDMMC_PWREN));
	dbg( "dwmci: SDMMC_CLKDIV:    0x%08x\n",
		read_data(SDMMC_CLKDIV));
	dbg( "dwmci: SDMMC_CLKSRC:    0x%08x\n",
		read_data(SDMMC_CLKSRC));
	dbg( "dwmci: SDMMC_CLKENA:    0x%08x\n",
		read_data(SDMMC_CLKENA));
	dbg( "dwmci: SDMMC_TMOUT:     0x%08x\n",
		read_data( SDMMC_TMOUT));
	dbg( "dwmci: SDMMC_CTYPE:     0x%08x\n",
		read_data( SDMMC_CTYPE));
	dbg( "dwmci: SDMMC_BLKSIZ:    0x%08x\n",
		read_data( SDMMC_BLKSIZ));
	dbg( "dwmci: SDMMC_BYTCNT:    0x%08x\n",
		read_data( SDMMC_BYTCNT));
	dbg( "dwmci: SDMMC_INTMASK:    0x%08x\n",
		read_data( SDMMC_INTMASK));
	dbg( "dwmci: SDMMC_CMDARG:    0x%08x\n",
		read_data( SDMMC_CMDARG));
	dbg( "dwmci: SDMMC_CMD:       0x%08x\n",
		read_data( SDMMC_CMD));
	dbg( "dwmci: SDMMC_MINTSTS:   0x%08x\n",
		read_data( SDMMC_MINTSTS));
	dbg( "dwmci: SDMMC_RINTSTS:   0x%08x\n",
		read_data( SDMMC_RINTSTS));
	dbg( "dwmci: SDMMC_STATUS:    0x%08x\n",
		read_data( SDMMC_STATUS));
	dbg( "dwmci: SDMMC_FIFOTH:    0x%08x\n",
		read_data( SDMMC_FIFOTH));
	dbg( "dwmci: SDMMC_CDETECT:   0x%08x\n",
		read_data( SDMMC_CDETECT));
	dbg( "dwmci: SDMMC_WRTPRT:    0x%08x\n",
		read_data( SDMMC_WRTPRT));
	dbg( "dwmci: SDMMC_GPIO:      0x%08x\n",
		read_data( SDMMC_GPIO));
	dbg( "dwmci: SDMMC_TCBCNT:    0x%08x\n",
		read_data( SDMMC_TCBCNT));
	dbg( "dwmci: SDMMC_TBBCNT:    0x%08x\n",
		read_data( SDMMC_TBBCNT));
	dbg( "dwmci: SDMMC_DEBNCE:    0x%08x\n",
		read_data( SDMMC_DEBNCE));
	dbg( "dwmci: SDMMC_USRID:     0x%08x\n",
		read_data( SDMMC_USRID));
	dbg( "dwmci: SDMMC_VERID:     0x%08x\n",
		read_data( SDMMC_VERID));
	dbg( "dwmci: SDMMC_HCON:      0x%08x\n",
		read_data( SDMMC_HCON));
	dbg( "dwmci: SDMMC_UHS_REG:   0x%08x\n",
		read_data( SDMMC_UHS_REG));
}

int do_dwmmc_dump (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){
	dwmci_dumpregs_err();
	return 0;
	}


U_BOOT_CMD(
        dwmmc_dump, 1, 0, do_dwmmc_dump,
        "display Desgingware reg dump",
        "- display register dump"
);


U_BOOT_CMD(
        dwmmc_write_blk, 1, 0, do_dwmmc_1_blk_tx,
        "display Desgingware MMC info",
        "- write one block to card from RAM using fifo"
);

U_BOOT_CMD(
                dwmmc_read_blk, 1, 0, do_dwmmc_1_blk_rx,
                "display Desgingware MMC info",
                "- read one block from card to RAM using fifo"
          );



U_BOOT_CMD(
	dwmmcinfo, 1, 0, do_dwmmcinfo,
	"display Desgingware MMC info",
	"- dislay info of the current MMC device"
);

 



