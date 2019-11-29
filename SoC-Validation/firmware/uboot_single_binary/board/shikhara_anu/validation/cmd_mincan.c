#include <common.h>
#include <command.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_clock.h>

#define CAN_BASE_ADDR  	  SHIKHARA_CAN_BASE    // CAN base address

#define CAN_CONTROL           ((volatile unsigned int *)(CAN_BASE_ADDR + 0x00))
#define CAN_MODE              ((volatile unsigned int *)(CAN_BASE_ADDR + 0x00))
#define CAN_COMMAND           ((volatile unsigned int *)(CAN_BASE_ADDR + 0x01))
#define CAN_STATUS            ((volatile unsigned int *)(CAN_BASE_ADDR + 0x02))
#define CAN_INTERRUPT         ((volatile unsigned int *)(CAN_BASE_ADDR + 0x03))
#define CAN_ACCEPTANCECODE    ((volatile unsigned int *)(CAN_BASE_ADDR + 0x04))
#define CAN_INTERRUPTENABLE   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x04))
#define CAN_ACCEPTANCEMASK    ((volatile unsigned int *)(CAN_BASE_ADDR + 0x05))
#define CAN_BUSTIMING0        ((volatile unsigned int *)(CAN_BASE_ADDR + 0x06))
#define CAN_BUSTIMING1        ((volatile unsigned int *)(CAN_BASE_ADDR + 0x07))
#define CAN_SFF_TXDATA_0      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0A))
#define CAN_SFF_TXDATA_1      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0B))
#define CAN_ARBLOSTCAPTURE    ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0B))
#define CAN_SFF_TXDATA_2      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0C))
#define CAN_ERRORCODECAPTURE  ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0C))
#define CAN_SFF_TXDATA_3      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0D))
#define CAN_ERRORWARNINGLIMIT ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0D))
#define CAN_SFF_TXDATA_4      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0E))
#define CAN_RXERRORCOUNT      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0E))
#define CAN_SFF_TXDATA_5      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0F))
#define CAN_TXERRORCOUNT      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x0F))
#define CAN_SFF_TXDATA_6      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x10))
#define CAN_EFF_TXDATA_0      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x10))
#define CAN_EFF_RXDATA_0      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x10))
#define CAN_ACCEPTANCECODE0   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x10))
#define CAN_SFF_TXDATA_7      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x11))
#define CAN_EFF_TXDATA_1      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x11))
#define CAN_EFF_RXDATA_1      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x11))
#define CAN_ACCEPTANCECODE1   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x11))
#define CAN_SFF_TXDATA_8      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x12))
#define CAN_EFF_TXDATA_2      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x12))
#define CAN_EFF_RXDATA_2      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x12))
#define CAN_ACCEPTANCECODE2   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x12))
#define CAN_SFF_TXDATA_9      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x13))
#define CAN_EFF_TXDATA_3      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x13))
#define CAN_EFF_RXDATA_3      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x13))
#define CAN_ACCEPTANCECODE3   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x13))
#define CAN_SFF_RXDATA_0      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x14))
#define CAN_EFF_TXDATA_4      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x14))
#define CAN_EFF_RXDATA_4      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x14))
#define CAN_ACCEPTANCEMASK0   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x14))
#define CAN_SFF_RXDATA_1      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x15))
#define CAN_EFF_TXDATA_5      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x15))
#define CAN_EFF_RXDATA_5      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x15))
#define CAN_ACCEPTANCEMASK1   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x15))
#define CAN_SFF_RXDATA_2      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x16))
#define CAN_EFF_TXDATA_6      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x16))
#define CAN_EFF_RXDATA_6      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x16))
#define CAN_ACCEPTANCEMASK2   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x16))
#define CAN_SFF_RXDATA_3      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x17))
#define CAN_EFF_TXDATA_7      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x17))
#define CAN_EFF_RXDATA_7      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x17))
#define CAN_ACCEPTANCEMASK3   ((volatile unsigned int *)(CAN_BASE_ADDR + 0x17))
#define CAN_SFF_RXDATA_4      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x18))
#define CAN_EFF_TXDATA_8      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x18))
#define CAN_EFF_RXDATA_8      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x18))
#define CAN_SFF_RXDATA_5      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x19))
#define CAN_EFF_TXDATA_9      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x19))
#define CAN_EFF_RXDATA_9      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x19))
#define CAN_SFF_RXDATA_6      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1A))
#define CAN_EFF_TXDATA_10     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1A))
#define CAN_EFF_RXDATA_10     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1A))
#define CAN_SFF_RXDATA_7      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1B))
#define CAN_EFF_TXDATA_11     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1B))
#define CAN_EFF_RXDATA_11     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1B))
#define CAN_SFF_RXDATA_8      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1C))
#define CAN_EFF_TXDATA_12     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1C))
#define CAN_EFF_RXDATA_12     ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1C))
#define CAN_SFF_RXDATA_9      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1D))
#define CAN_RXMSGCOUNTER      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1D))
#define CAN_CLOCKDIVIDER      ((volatile unsigned int *)(CAN_BASE_ADDR + 0x1F))


#define write_data_8(register_address,register_value) *((volatile unsigned char *) (register_address)) = ((unsigned char) (register_value))
#define read_data_8(register_address) *((volatile unsigned char *) (register_address))

void can_mininit(void)
{
	volatile unsigned int i,j;
	 volatile unsigned char rdata_8= 0x1;
	 volatile unsigned char rdata;

	//writing 1 into Mode register,controller is in Reset Mode
 write_data_8(CAN_MODE,0x01);

//setting the Baudrate prescaler by writing into Bus timing0 Register
 write_data_8(CAN_BUSTIMING0,0x45);		/* http://www.esacademy.com/en/library/calculators/sja1000-timing-calculator.html 
						   This link helps to configure bit timing parameters for 125kbps */

//Setting the Segment Timings & Sample point by writing into Bus Timing1 Register
 write_data_8(CAN_BUSTIMING1,0x1c);

//written 1 into can_mode of clockdivider register to enable pelican Mode
 write_data_8(CAN_CLOCKDIVIDER,0x80);
for(i=0;i<=100;i++)
{
};

//writing 1 into Mode register,controller is in Operating Mode   
 write_data_8(CAN_MODE,0x00);

for(i=0;i<=10;i++)
{
};

//writting FF into interrupt enable Register to enable all interrupts
 write_data_8(CAN_INTERRUPTENABLE,0xFF);

//Read the RX Errorcount & TX Errorcount Registers
 rdata          = read_data_8(CAN_RXERRORCOUNT);
 rdata          = read_data_8(CAN_TXERRORCOUNT);

//writing the ID,DLC and Data Bytes into TX Data Registers
  write_data_8(CAN_EFF_TXDATA_0,0x88);
  write_data_8(CAN_EFF_TXDATA_1,0xA6);
  write_data_8(CAN_EFF_TXDATA_2,0xA0);
  write_data_8(CAN_EFF_TXDATA_3,0x78);
  write_data_8(CAN_EFF_TXDATA_4,0x9A);

for(i=0; i<=7; i=i+1)
  {
  write_data_8((CAN_BASE_ADDR+0x15+i),rdata_8++);
   }

//writing 1 into Command Register for Transmission request
write_data_8(CAN_COMMAND,0x01);

while(!(read_data_8(CAN_INTERRUPT)& 0x00000002) == 0x00000002)
{
	printf("ERROR:CAN Transmission not done\n");
}

}

int do_mincan (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if (strncmp(argv[1], "init", 4) == 0) {
		can_mininit();
	}

}

















U_BOOT_CMD(
	mincan, 2,1, do_mincan,
	"can	 - CAN bus commands\n",
	"can init - Initialise CAN controller \n"
);
