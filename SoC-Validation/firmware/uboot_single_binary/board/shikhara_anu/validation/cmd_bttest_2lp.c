#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/list.h>
#include <asm/arch/bluetooth/shikhara_bt_defines.h>
#include <asm/arch/bluetooth/shikhara_bt_irq_defines.h>
#include <asm/arch/bluetooth/shikhara_bt_register_defines.h>
#include <asm/arch/shikhara_map.h>

#define BTSS_CONFIG_ADDR 0x5d5611b8

unsigned int volatile * const BT_TX_STS_INTR_DONE = (unsigned int *) SEMA_ADDR;
unsigned int volatile * const BT_RX_STS_INTR_DONE = (unsigned int *) SEMA_ADDR+0x1;
unsigned int volatile * const BT_MODE_STS_INTR_DONE = (unsigned int *) SEMA_ADDR+0x2;
unsigned int volatile * const BT_TIMEOUT_INTR_DONE = (unsigned int *) SEMA_ADDR+0x3;
unsigned int volatile * const BT_HLC_INTR_DONE = (unsigned int *) SEMA_ADDR+0x4;
unsigned int volatile * const BT_RADIO_INTR_DONE = (unsigned int *) SEMA_ADDR+0x5;
unsigned int volatile * const BT_BB_TIMER_INTR_DONE = (unsigned int *) SEMA_ADDR+0x6;
unsigned int volatile * const BT_SNIFF_INTR_DONE = (unsigned int *) SEMA_ADDR+0x7;
unsigned int volatile * const BT_START_ESCO_INTR_DONE = (unsigned int *) SEMA_ADDR+0x8;
unsigned int volatile * const BT_END_ESCO_INTR_DONE  = (unsigned int *) SEMA_ADDR+0x9;
unsigned int volatile * const BT_ACCESS_ERROR_INTR_DONE = (unsigned int *) SEMA_ADDR+0xA;
unsigned int volatile * const BT_POWER_ON_INTR_DONE = (unsigned int *) SEMA_ADDR+0xB;
unsigned int volatile * const BT_TX_FIFO_EMPTY_INTR_DONE = (unsigned int *) SEMA_ADDR+0xC;
unsigned int volatile * const BT_HOST_PCMTX_INTR_DONE = (unsigned int *) SEMA_ADDR+0xF;
unsigned int volatile * const BT_HOST_PCMRX_INTR_DONE = (unsigned int *) SEMA_ADDR+0x10;

//#define BB_REG_BASE_ADDR 0xD4541000

void WRITE16 (volatile unsigned int* reg_addr, unsigned int data, unsigned int mask)
{
	unsigned int reg_data;
	volatile unsigned int* bb_reg_addr;
	int i;
//	// -------------------------------------------------
//	// Modified by Vishnu
//	// -------------------------------------------------
//	reg_addr = reg_addr << 1;
//	// -------------------------------------------------
//	
//        bb_reg_addr = (vBB_write_registerolatile unsigned int *)(BB_REG_BASE_ADDR+(reg_addr));
        bb_reg_addr = reg_addr;
  //      printf("Addr: %x\n", (unsigned int)bb_reg_addr);
	reg_data = *bb_reg_addr;
        //printf("Read Data: %x\n", reg_data);
	reg_data = reg_data & (~mask); 	// Clearing mask bits
	data = reg_data | data; 		// writing data to masked loc	
        //printf("Write Data: %x\n", data);
	*bb_reg_addr = data;
        reg_data = *bb_reg_addr;
        //printf("Read Data: %x\n", reg_data);

	//printf("\n");
	// -------------------------------------------------
	// Modified by Vishnu
	// -------------------------------------------------
        for(i =0;i<5;i++);	
	// -------------------------------------------------
}
// --------------------------------------------------------------------------------------

// Read baseband register 
// Mask the read data and provided data and compare
// Need to log error if there is a mismatch

void READ16 (volatile unsigned int* reg_addr, unsigned short data, unsigned short mask)
{
	unsigned int reg_data;
	volatile unsigned int* bb_reg_addr;
//	// -------------------------------------------------
//	// Modified by Vishnu
//	// -------------------------------------------------
//	reg_addr = reg_addr << 1;
//	// -------------------------------------------------
//        bb_reg_addr = (volatile unsigned int *)(BB_REG_BASE_ADDR+(reg_addr));
        bb_reg_addr = reg_addr;
	
	reg_data = *bb_reg_addr;

	if ((reg_data & mask) != (data & mask))
	{
	}
}	

void SPIN16 (volatile unsigned int* reg_addr, unsigned int data, unsigned int mask) 
{
	// unsigned short reg_data;
	
	unsigned int reg_data;
	volatile unsigned int* bb_reg_addr;
//	// -------------------------------------------------
//	// Modified by Vishnu
//	// -------------------------------------------------
//        reg_addr = reg_addr << 1;
//	// -------------------------------------------------
//        bb_reg_addr = (volatile unsigned int *)(BB_REG_BASE_ADDR+(reg_addr));
        bb_reg_addr = reg_addr;
	reg_data = *bb_reg_addr;

	while ((reg_data & mask) != (data & mask))
	{
	//	printf("BTSS_BB_TXFIFO_STS = 0x%x\n",readl(BTSS_BB_TXFIFO_STS));
		printf("BTSS_BB_INTR_STATUS = 0x%x\n",readl(BTSS_BB_INTR_STATUS));
	//	printf("BTSS_BB_TX_STS = 0x%x\n",readl(BTSS_BB_TX_STS));
	//	printf("reg_data = 0x%x\n",reg_data);
        //      printf("BTSS_BB_RXFIFO1_STS = 0x%x\n",readl(BTSS_BB_RXFIFO1_STS));
        //      printf("BTSS_BB_RXFIFO2_STS = 0x%x\n",readl(BTSS_BB_RXFIFO2_STS));

		reg_data = *bb_reg_addr;
	}
}
void ENABLE_ENCRYPTION  (unsigned int am_addr)
{
	WRITE16(BTSS_BB_CONNECTOR,am_addr*32,0xffff);
	WRITE16(BTSS_BB_ENCRYP_KEY, 0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_1,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_2,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_3,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_4,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_5,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_6,0xFFFF,0xffff);
	WRITE16(BTSS_BB_ENCRYP_7,0xFFFF,0xffff);

// enable encryption
	WRITE16(BTSS_BB_ENCRYP_EN,0x03,0x0003);
}
void diag_generate_parity_bit(unsigned int lap, unsigned char *parity_bits)
{
    unsigned int barker_seq;
    unsigned int info_bits;
    unsigned int hold_mod1;
    unsigned int hold_mod2;
    unsigned int mod2;
    unsigned int mod1;
    unsigned int temp;
    unsigned char i;

    /* Barker sequence generation. */
    if(lap & 0x00800000)
        barker_seq = (lap | 0x13000000);
    else
        barker_seq = lap | 0x2c000000;
    /* Info bit generation. */
    info_bits = ((barker_seq << 2) ^ LC_PN_SEQ_MSB) & 0xfffffffc;

    mod2 = info_bits;
    mod1 = 0x00;

    for(i=0;i<=29;i++)
    {
        if((mod2 & 0x80000000) == 0x80000000)
        {
            if((mod1 & 0x80000000) == 0x80000000)
                mod2 = (mod2 << 1) | 0x01;
            else
                mod2 = (mod2 << 1);
                mod1 = mod1 << 1;
                mod2 = mod2 ^ LC_GEN_POLY_MSB;
                mod1 = mod1 ^ LC_GEN_POLY_LSB;
        }
        else
        {
            if((mod1 & 0x80000000) == 0x80000000)
                mod2 = (mod2 << 1) | 0x01;
            else
                mod2 = (mod2 << 1);
             mod1 = mod1 << 1;
        }
    }

    hold_mod2 = ((mod2 & 0x3fffffff) << 2);
    hold_mod1 = mod1 >> 30;
    temp = hold_mod2 | hold_mod1;

    parity_bits[0]=  (unsigned char)(temp & 0x000000ff);
    parity_bits[1] = (unsigned char)((temp & 0x0000ff00)>>8);
    parity_bits[2] = (unsigned char)((temp & 0x00ff0000)>>16);
    parity_bits[3] = (unsigned char)((temp & 0xff000000)>>24);
    temp = (mod2) >> 30;
    parity_bits[4] = (unsigned char)(temp & 0x00000003);
    return;
}


void generate_and_write_parity_bit(unsigned int lap, unsigned char role)
{
    unsigned char  parity_bits[5];
    unsigned short temp_reg ;
    
    diag_generate_parity_bit(lap, &parity_bits[0]);

    if (role == LOCAL)
    {
        /* Program the remote device parity bits */
        writel((*((unsigned int *) &parity_bits[0])),BTSS_BB_LOCAL_PARITY_BITS_REGISTER1);
        writel(0x00014d27,BTSS_BB_LOCAL_PARITY_BITS_REGISTER2);  //Modified by Moschip
		
                temp_reg = parity_bits[4];
		BB_WRITE_LOCAL_PARITY_BITS(temp_reg); //bt_defines.h

    }
    else
    {
        /* Program the remote device parity bits */
        writel((*((unsigned int *) &parity_bits[0])),BTSS_BB_REMOTE_PARITY_BITS_REGISTER1);
        writel((*((unsigned int *) &parity_bits[2])),BTSS_BB_REMOTE_PARITY_BITS_REGISTER2);

        temp_reg = parity_bits[4];
        BB_WRITE_REMOTE_PARITY_BITS(temp_reg);
    }
}



void DIRECT_CONNECTION_M16 (volatile unsigned int no_bytes,volatile unsigned int type)
{
	unsigned int  m_lap;
        WRITE16 (BTSS_BB_THRESHOLD,0x40,0xff);
	WRITE16 (BTSS_BB_BD_ADDR1 , m_lap1+m_lap2*256,0xffff );  // m_lap1 + (m_lap2<<8) //bt_defines.h
	WRITE16 (BTSS_BB_BD_ADDR2 , m_lap3+m_uap*256,0xffff  );
	WRITE16 (BTSS_BB_BD_ADDR3 , m_nap1+m_nap2*256,0xff );
	//configure master's parity bit here
    
    // Need to write Local Parity
    // To generate local/own parity
	m_lap = m_lap1 + (m_lap2 << 8) + (m_lap3 << 16);
	generate_and_write_parity_bit(m_lap, LOCAL);
    
//no of data bytes=10
	WRITE16 (BTSS_BB_LUT_1_LOWER,no_bytes+type*256,0xffff);
	WRITE16 (BTSS_BB_LUT_1_UPPER,0x0100,0xff20);
	WRITE16 (BTSS_BB_SCO_TYPE,0x8030,0x80ff);
	WRITE16 (BTSS_BB_PN0_INFO,0x0003,0x0fff); // ADDED 
}


void WAIT_CLK (unsigned short  clk_count)
{
        unsigned short i;
        for (i=0; i<40;i++);
}


void RADIO_CONF16 (void)
{
//24 - 00b8, 18 - 0088, 16 - 0078, 12- 0058
	WRITE16(BTSS_BB_POC_REG,0x00b8,0xffff);
	WRITE16(BTSS_BB_RADIO_SEL,0x03,0xffff);
	WAIT_CLK(12);
	WRITE16 (BTSS_BB_RADIO_SEL,0x0103,0xffff);
	WRITE16 (BTSS_BB_OWN_PARITY1,0x5a82,0xffff);
	WRITE16 (BTSS_BB_OWN_PARITY2,0x7c4e,0xffff);
	WRITE16 (BTSS_BB_INQ_LAP2,0x0000,0xc00);
	WRITE16 (BTSS_BB_SLV_PARITY1,0x5a82,0xffff);
	WRITE16 (BTSS_BB_SLV_PARITY2,0x7c4e,0xffff);
	WRITE16 (BTSS_BB_INQ_LAP2,0x0000,0x3000);
	WRITE16 (BTSS_BB_LOOPBACK,0x8000,0xC000);
}

void WAIT_SIG (unsigned int signal_name,unsigned char level) 
{
  if (level == HIGH) 
  {
      SPIN16 (BTSS_BB_INTR_MSK,0x0001,0x0001);
  }
  else
  {
      SPIN16 (BTSS_BB_INTR_MSK,0x0000,0x0001);
  }      
}

void EXECUTE16 (unsigned int connecter,unsigned int sco_type)
{
	WRITE16 (BTSS_BB_CONNECTOR , connecter,0xffff );
	WRITE16 (BTSS_BB_SCO_TYPE , sco_type,0xffff);
	WRITE16 (BTSS_BB_INSTR_REG , do_execute,0xff );
}

//macro for enabling sniff mode

void SNIFF16 ( unsigned int am_address,volatile unsigned int dsniffx, volatile unsigned int dsniffy)
{
	WRITE16 (BTSS_BB_CONNECTOR,am_address,0xffff);
	WRITE16 (BTSS_BB_DSNIFF,dsniffx + 256*dsniffy,0xffff);
	WRITE16 (BTSS_BB_TSNIFF,tsniff1 + 256*tsniff2,0xffff);
	WRITE16 (BTSS_BB_SNIFF_ATTEMPT,nsniff1 + 256*nsniff2,0xffff);
	WRITE16 (BTSS_BB_SNIFF_TO,sniffTO1 + 256*sniffTO2,0xffff);
	WRITE16 (BTSS_BB_INSTR_REG,do_sniff,0xff);
}

//macro for enabling hold mode

void HOLD16 ( unsigned int am_address,volatile unsigned int hold_period1,volatile unsigned int hold_period2)
{
	WRITE16 (BTSS_BB_XVAL_TOL, 0x0,0xffff);
	WRITE16 (BTSS_BB_CONNECTOR,am_address,0xffff);
	WRITE16 (BTSS_BB_DSNIFF,0x0002,0xffff);
	WRITE16 (BTSS_BB_TSNIFF,0x0010,0xffff);
	WRITE16 (BTSS_BB_INSTR_REG,do_hold,0xff);
}

void INQUIRY16 (volatile unsigned int inqTO1, volatile unsigned int inqTO2, volatile unsigned int lap1, 
			volatile unsigned int lap2,volatile unsigned int lap3, volatile unsigned int catc_res, 
			volatile unsigned int inqpar1, volatile unsigned int inqpar2, volatile unsigned int inqpar3) 
{

	WRITE16 (BTSS_BB_INQ_LAP1 , lap1 + 256*lap2,0xffff );
	WRITE16 (BTSS_BB_INQ_LAP1+2 , lap3 + 256*catc_res,0x03ff);
	WRITE16 (BTSS_BB_INQ_PARITY1 , inqpar1,0xffff );
	WRITE16 (BTSS_BB_INQ_PARITY2 , inqpar2,0xffff );
	WRITE16 (BTSS_BB_INQ_LAP2 , inqpar3*0x4000,0xc000 );
	WRITE16 (BTSS_BB_NINQUIRY_NPAGE , 0x0040,0xffff);
	WRITE16 (BTSS_BB_INQUIRY_PAGE_TO , inqTO1 + 256*inqTO2,0xffff);
	WRITE16 (BTSS_BB_INSTR_REG , start_inquiry,0xffff);
}

void PAGE16 (volatile unsigned int lap1, volatile unsigned int lap2, volatile unsigned int lap3, 
		volatile unsigned int uap, volatile unsigned int nap1, volatile unsigned int nap2, 
		volatile unsigned int clk_off1, volatile unsigned int clk_off2, volatile unsigned int npage1, 
		volatile unsigned int npage2, volatile unsigned int fhs_data, volatile unsigned int pageTO2, 
		volatile unsigned int pageTO1, volatile unsigned int ownpar1, volatile unsigned int ownpar2, 
		volatile unsigned int ownpar3, volatile unsigned int slvpar1, volatile unsigned int slvpar2, 
		volatile unsigned int slvpar3) 
{
	volatile unsigned int s_lap;
    
	WRITE16 (BTSS_BB_OB_ADDR1 , lap1 + 256*lap2,0xffff );
	WRITE16 (BTSS_BB_OB_ADDR2  , lap3 + 256*uap,0xffff );
	WRITE16 (BTSS_BB_OB_ADDR3  , nap1 + 256*nap2,0xffff);
	WRITE16 (BTSS_BB_CLK_OFFSET , clk_off1 + 256*clk_off2,0xffff);
	WRITE16 (BTSS_BB_INQUIRY_PAGE_TO , pageTO1 + 256*pageTO2,0xffff);
	WRITE16 (BTSS_BB_NINQUIRY_NPAGE , npage1 + 256*npage2,0xffff);
	WRITE16 (BTSS_BB_LUT_1_LOWER , 0x3001,0xf001 );
	WRITE16 (BTSS_BB_FHS_REG , 256*fhs_data,0xffff ) ;

	// Page has Slave BD_ADDR so need to generate Slave Parity
	// Generate and Write Slv Parity
	s_lap = lap1 + (lap2 << 8) + (lap3 << 16);
	generate_and_write_parity_bit(s_lap, SLAVE);

	WRITE16 (BTSS_BB_INSTR_REG , start_paging,0xff);
}
void CONFIG_ADDR16 (volatile unsigned int lap1, unsigned int lap2, unsigned int lap3, unsigned int uap, 
			volatile unsigned int nap1, unsigned int nap2)
{
    volatile unsigned int lap;
    
	WRITE16 (BTSS_BB_BD_ADDR1 , lap1 + 256*lap2,0xffff );
	WRITE16 (BTSS_BB_BD_ADDR2 , lap3 + 256*uap,0xffff );
	WRITE16 (BTSS_BB_BD_ADDR3 , nap1 + 256*nap2,0xffff);
	
	lap = lap1 + (lap2 << 8) + (lap3 << 16);
	generate_and_write_parity_bit(lap, LOCAL);
}


int do_btirq(int int_id)
{
   volatile unsigned int rdata = 0;
   volatile unsigned int temp_data = 0;
   volatile unsigned int check; 
	//printf("do_btirq %d\n",int_id);
       
   if(int_id == BT_ID)
   {
      	rdata  = readl(BTSS_BB_INTR_STATUS);
   	check  = rdata;

	//printf("BTSS_BB_INTR_STATUS ==== %d\n",rdata);

	//printf("do_btirq %d\n",int_id);
	if((check & 0x00010001) == 0x00010001)		//checking for the receive status interrupt bit number 0
	{
              *BT_RX_STS_INTR_DONE = 1;
               readl(BTSS_BB_RX_STS);
   			//intr_done is common to testcase.c and this file,used to indicate interrupt has occured
         }
         if((check & 0x00020002) == 0x00020002) 		//checking for the transmit status interrupt bit number 1
	{
              *BT_TX_STS_INTR_DONE  = 1;   								
          	readl(BTSS_BB_TX_STS );
	//	printf("*BT_TX_STS_INTR_DONE ======= %d\n",*BT_TX_STS_INTR_DONE);
         }
         if((check & 0x00040004) == 0x00040004)                     //checking for the mode status interrupt bit number 2
	{
              *BT_MODE_STS_INTR_DONE = 1;   								
              readl (BTSS_BB_MODE_STS);
         }
         if((check & 0x00080008) == 0x00080008)                     //checking for the timeout interrupt bit number 3
	{
              *BT_TIMEOUT_INTR_DONE = 1;   								
              readl (BTSS_BB_TO_STS);
         }
         if((check & 0x00100010) == 0x00100010)                     //checking for the fifo empty interrupt bit number 4
	{
              *BT_TX_FIFO_EMPTY_INTR_DONE = 1;   								
              writel(0x00100010,BTSS_BB_INTR_STATUS );
         }
          if((check & 0x00200020) == 0x00200020) 		//checking for the Hardware connection interrupt bit number 5
	{  
               *BT_HLC_INTR_DONE = 1;   			  
                readl (BTSS_BB_INTR_STATUS);
         }
         if((check & 0x00800080) == 0x00800080)                      //checking for the Radio interrupt bit number 7
	{
	       *BT_RADIO_INTR_DONE = 1;   								
                readl (BTSS_BB_RADIO_CNTR);
         }
         if((check & 0x01000100) == 0x01000100)                      //checking for BB Timer Interrupt bit number 8
	{ 
		*BT_BB_TIMER_INTR_DONE = 1;   								
                readl (BTSS_BB_INTR_STATUS);
         }
         if((check & 0x02000200) == 0x02000200)                      //checking for BB sniff Interrupt bit number 9
	{
		*BT_SNIFF_INTR_DONE = 1;   								
               #ifndef SNIFF_CLR
                writel(0x02000200,BTSS_BB_INTR_STATUS );
               #endif
         }
         if((check & 0x04000400) == 0x04000400)                      //checking for BB start ESCO Interrupt bit number 10
	{
		*BT_START_ESCO_INTR_DONE = 1;   								
                readl (BTSS_BB_INTR_STATUS);
         }
         if((check & 0x08000800) == 0x08000800)                      //checking for BB end ESCO Interrupt bit number 11
	{
		*BT_END_ESCO_INTR_DONE = 1;   								
                readl (BTSS_BB_INTR_STATUS);
         }
         if((check & 0x10001000) == 0x10001000)                      //checking for BB Access Error Interrupt bit number 12
	{
		*BT_ACCESS_ERROR_INTR_DONE = 1;   								
      	        readl(BTSS_BB_INTR_STATUS);
         }
         if((check & 0x40004000) == 0x40004000)                      //checking for BB Power ON Interrupt bit number 14
        {
        	*BT_POWER_ON_INTR_DONE = 1;   								
                 readl (BTSS_BB_POC_REG);
         }
        if((check & 0x80008000) == 0x80008000)                      //checking for BB Power ON Interrupt bit number 14
                {
                       temp_data = readl(BTSS_BB_HBUF_INTSTS_REG);
                       if((temp_data & 0x00080008) == 0x00080008)
                       {
                	*BT_HOST_PCMTX_INTR_DONE = 1;   								
                       }
                       if((temp_data & 0x00040004) == 0x00040004)
                       {
                	*BT_HOST_PCMRX_INTR_DONE = 1;   								
                       }
                 }
        	 	writel(BT_ID,/*GIC_ICCEOIR*/0xf8f00110); 				  
			//writing to the GIC End of Register to indicate the completion of interrupt
           }
   return 0;
}


int do_bttest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	volatile unsigned int i;
	volatile unsigned int temp_data;
	volatile unsigned short int temp_fifo;
	volatile unsigned short int rdata;
  
	*BT_TX_STS_INTR_DONE = 0;
	*BT_RX_STS_INTR_DONE = 0;
	*BT_MODE_STS_INTR_DONE = 0;
	*BT_TIMEOUT_INTR_DONE = 0;
	*BT_HLC_INTR_DONE = 0;
	*BT_RADIO_INTR_DONE = 0;
	*BT_BB_TIMER_INTR_DONE = 0;
	*BT_SNIFF_INTR_DONE = 0;
	*BT_START_ESCO_INTR_DONE = 0;
	*BT_END_ESCO_INTR_DONE  = 0;
	*BT_ACCESS_ERROR_INTR_DONE = 0;
	*BT_POWER_ON_INTR_DONE = 0;
	*BT_TX_FIFO_EMPTY_INTR_DONE = 0;


//	writel(0x0000006d,0x5d561054);
//	for(i=0;i<20;i++);
//	writel(0x00000005,0x5d561054);
///* Giving Power on signal to BT from PMU  Not required for FPGA 
#if 0 
	temp_data= readl (PMU_CONFIG_ADDR);
	temp_data= temp_data & ~(0x00000020);
	writel (PMU_CONFIG_ADDR ,temp_data);
	for( i=0;i<10;i++)
	{
	}
#endif
// Giving Power on signal to BT from PMU  Not required for FPGA*/
	printf("enable the interrupt\n");
           //enable the interrupt.
           //   int_enable(84);
           //     interrupt_handler_set(84,do_btirq);
	printf("enable the interrupt done\n");

// Configure BT system controller to provide clock enables to the BB controller
	//printf("BTSS_SC_REG0 = 0x%x, BTSS_CONFIG_ADDR = 0x%x\n",BTSS_SC_REG0,BTSS_CONFIG_ADDR);
	writel (0x6E,BTSS_SC_REG0);
//	writel (0x7E,BTSS_SC_REG0);
//	printf("BTSS_SC_REG0 = 0x%x\n",BTSS_SC_REG0);
	writel (0x00000001,BTSS_CONFIG_ADDR );
// Configuring POC register to come out of sleep mode and make the BB controller active
//	printf("BTSS_BB_INTR_STATUS = 0x%x, BTSS_BB_TXFIFO_STS =0x%x\n",BTSS_BB_INTR_STATUS,BTSS_BB_TXFIFO_STS);
	writel(0xFFFCFFFC,BTSS_BB_INTR_STATUS);
	writel(0x03000300,BTSS_BB_TXFIFO_STS);


// Basic Radio configuration common to all the testcases
	RADIO_CONF16();
//	printf("0x5d5410d8 value = 0x%x\n",readl(0x5d5410d8));
// End of basic configuration 

//Write 10 bytes in fifo
	printf("Writing 0x1234 into tx FIFO\n");  // Added by ACHYUTH
	for (i=0;i<5;i=i+1) {
		//temp_fifo = readw(DDR_BT_FINL+i*2);
		//writel(temp_fifo,BTSS_BB_TX_FIFO_LOC );
		writel(0x1234,BTSS_BB_TX_FIFO_LOC );
/*		printf("BTSS_BB_TXFIFO_STS = 0x%x\n",readl(BTSS_BB_TXFIFO_STS));
		printf("BTSS_BB_RXFIFO1_STS = 0x%x\n",readl(BTSS_BB_RXFIFO1_STS));
		printf("BTSS_BB_RXFIFO2_STS = 0x%x\n",readl(BTSS_BB_RXFIFO2_STS));
		printf("i=%d\n",i);
*/
	}
	

  
//DIRECT_CONNECTION no_bytes type
	DIRECT_CONNECTION_M16(0x0a , 0x30);
// switch off the whitener.
	WRITE16 (BTSS_BB_SCO_TYPE , 0x80 , 0x0080);

//	printf("BT_TX_STS_INTR_DONE\n");
//wait for acl packet transmit interrupt

/*	while(!(*BT_TX_STS_INTR_DONE))
	{
	}*/
	SPIN16(BTSS_BB_INTR_STATUS,0x0002,0x0002);
	printf("BTSS_BB_INTR_STATUS 0x%x\n", readl(BTSS_BB_INTR_STATUS));
          	readl(BTSS_BB_TX_STS );
	*BT_TX_STS_INTR_DONE = 0;
	printf("Interrupt asserted BT_TX_STS_INTR_DONE\n"); 
	SPIN16(BTSS_BB_INTR_STATUS,0x0001,0x0001);
	printf("BTSS_BB_INTR_STATUS 0x%x\n", readl(BTSS_BB_INTR_STATUS));
          	readl(BTSS_BB_RX_STS );
// wait for acl packet received interrupt
// check whether it is i_pcdrx
/*	while(!(*BT_RX_STS_INTR_DONE))
	{
	}*/
	*BT_RX_STS_INTR_DONE = 0;
	printf("Interrupt asserted BT_RX_STS_INTR_DONE\n");

  // check the received length.
	readl (BTSS_BB_PLD_HEAD);
  
  //read 10 bytes in fifo with am_addr=1
	for (i=0;i<5;i=i+1) {
		temp_data = readl(BTSS_BB_BROADCAST_FIFO);
		writel(temp_data,DDR_BT_INIT+i*4);
	} 
	for (i=0;i<5;i++) {
		temp_data = readl(DDR_BT_INIT+i*4);
		printf("Reading first loop FIFO DATA = 0x%08x\n",temp_data);
	}
   
#if 0
///* Giving Power on signal to BT from PMU  Not required for FPGA 
  //GPIO Configuration to terminate the test  
	writel(GPIO_AFSEL,0X00);  //Configure the Auxillary port register to set software mode
	writel(GPIO_DIR  ,0XFF);  //Configure the direction register to set gpio lines as output
	writel(GPIO_DATA ,0XE0);  //Configure the Data register this is for the radio data comparision
	writel(GPIO_DATA ,0XA0);  //Configure the Data register this is for the bt_env transaction end
// Giving Power on signal to BT from PMU  Not required for FPGA*/
#endif
//	return 0;
// second loopback added by Achyuth
// code for softreset
/*
	writel(0x0000006d,0x5d561054);
	for(i=0;i<20;i++);
	writel(0x00000005,0x5d561054);
	
	printf("SOFT reset done\n");
*/

	printf("Writing 0x5678 into tx FIFO\n");  // Added by ACHYUTH
	for (i=0;i<5;i=i+1) {
		//temp_fifo = readw(DDR_BT_FINL+i*2);
		//writel(temp_fifo,BTSS_BB_TX_FIFO_LOC );
		writel(0x5678,BTSS_BB_TX_FIFO_LOC );
/*		printf("BTSS_BB_TXFIFO_STS = 0x%x\n",readl(BTSS_BB_TXFIFO_STS));
		printf("BTSS_BB_RXFIFO1_STS = 0x%x\n",readl(BTSS_BB_RXFIFO1_STS));
		printf("BTSS_BB_RXFIFO2_STS = 0x%x\n",readl(BTSS_BB_RXFIFO2_STS));
		printf("i=%d\n",i);
*/
	}
	

  
//DIRECT_CONNECTION no_bytes type
	DIRECT_CONNECTION_M16(0x0a , 0x30);
// switch off the whitener.
	WRITE16 (BTSS_BB_SCO_TYPE , 0x80 , 0x0080);

//	printf("BT_TX_STS_INTR_DONE\n");
//wait for acl packet transmit interrupt

/*	while(!(*BT_TX_STS_INTR_DONE))
	{
	}*/
	SPIN16(BTSS_BB_INTR_STATUS,0x0002,0x0002);
	printf("BTSS_BB_INTR_STATUS 0x%x\n", readl(BTSS_BB_INTR_STATUS));
          	readl(BTSS_BB_TX_STS );
	*BT_TX_STS_INTR_DONE = 0;
	printf("Interrupt asserted BT_TX_STS_INTR_DONE\n"); 
	SPIN16(BTSS_BB_INTR_STATUS,0x0001,0x0001);
	printf("BTSS_BB_INTR_STATUS 0x%x\n", readl(BTSS_BB_INTR_STATUS));
          	readl(BTSS_BB_RX_STS );
// wait for acl packet received interrupt
// check whether it is i_pcdrx
/*	while(!(*BT_RX_STS_INTR_DONE))
	{
	}*/
	*BT_RX_STS_INTR_DONE = 0;
	printf("Interrupt asserted BT_RX_STS_INTR_DONE\n");

  // check the received length.
	readl (BTSS_BB_PLD_HEAD);
  
  //read 10 bytes in fifo with am_addr=1
	for (i=0;i<5;i=i+1) {
		temp_data = readl(BTSS_BB_BROADCAST_FIFO);
		writel(temp_data,DDR_BT_INIT+i*4);
	} 
	for (i=0;i<5;i++) {
		temp_data = readl(DDR_BT_INIT+i*4);
		printf("Reading second loop FIFO DATA = 0x%08x\n",temp_data);
	}
   
	return 0;
}

U_BOOT_CMD(
                bttest, 1, 1, do_bttest,
                "Bluetooth Test",
                "Bluetooth Controller Validation"
          );

