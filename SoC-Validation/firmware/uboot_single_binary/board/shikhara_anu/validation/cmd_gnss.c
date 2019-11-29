//#define TRACK_DATA_DEBUG // this is to verify tracking data, any variation may indicate problems in tracking part (actual values are know only to Navika)
//#define DUMP_20INT_DEBUG // this is to verify acquistion data, any variation may indicate problems in acqusition part (actual values are know only to Navika)
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/serial_pl01x.h>
#include <spi.h>
#include <asm/arch/start_gps.h>
#include <watchdog.h>

typedef unsigned short int      T_U16;

#define CONFIG_PL01x_PORTS     (void *)SHIKHARA_UART1_BASE
static volatile unsigned char *const port = CONFIG_PL01x_PORTS;
static unsigned int baudrate = CONFIG_BAUDRATE;
#define GNSS_SPI_CLK 25000000
#define GNSS_SPI_BUS  2
#define GNSS_SPI_CS  0
#define GNSS_SPI_MODE 0

//#define dbg printf
//#define PROFILE_GNSS
unsigned long long Previous_ticks;
unsigned long long Current_ticks;
int write_data;
int ignore_read;

/*
T_U16 *Glonass_dsp_rx_buffer_mine = CONFIG_V7FPGA_DDR_BASE+0x100; 
T_U16 *Dsp_tx_buffer_mine = CONFIG_V7FPGA_DDR_BASE+0x40000;
T_U16 *Glonass_dsp_tx_buffer_mine = CONFIG_V7FPGA_DDR_BASE+0x30000;
T_U16 *Dsp_rx_buffer_mine = CONFIG_V7FPGA_DDR_BASE+0x50E00;
*/

int tenmsec_interrupt_count;
struct spi_slave *gnss_slave;

void Init_GlobalData(void)
{
	global_i = 0;
	GloDataReadIndex=0;
	GpsDataReadIndex=0;
	tenmsec_interrupt_count=0;
	glonass=0;
	gps=0;
	glo_acq=0;
	gps_acq=0;
	Nmea_Data_Size=0;
	Nmea_Buf_Read_Index=0;
	pattern=0;
	twohundredmsec_count=0;
	pattern_gps=0;
		write_data = 0;
/*	memset(Glonass_dsp_rx_buffer_mine, 0x0, sizeof(Glonass_dsp_rx_buffer));
	memset(Dsp_tx_buffer_mine,0x0, sizeof(Dsp_tx_buffer));
	memset(Glonass_dsp_tx_buffer_mine,0x0,sizeof(Glonass_dsp_tx_buffer));	
	memset(Dsp_rx_buffer_mine,0x0,sizeof(Dsp_rx_buffer));*/
}

int gps_uart_init(void)
{
	unsigned int temp;
	unsigned int divider;
	unsigned int remainder;
	unsigned int fraction;

	/* First, disable everything */
	writel(0x0, port + UART_PL011_CR);

	/*  * Set baud rate
	 *
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = RND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */

	temp = 16 * baudrate;
	divider = CONFIG_PL011_CLOCK / temp;
	remainder = CONFIG_PL011_CLOCK % temp;
	temp = (8 * remainder) / baudrate;
	fraction = (temp >> 1) + (temp & 1);

	writel(divider, port + UART_PL011_IBRD);
	writel(fraction, port + UART_PL011_FBRD);

	/* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
	writel((UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN),
			port + UART_PL011_LCRH);

	/* Finally, enable the UART */
	writel((UART_PL011_CR_UARTEN | UART_PL011_CR_TXE | UART_PL011_CR_RXE),
			port + UART_PL011_CR);

	return 0;

}

int gps_putc (unsigned char c)
{
	if (!(readl(port + UART_PL01x_FR) & UART_PL01x_FR_TXFF))
	{
		/* Send the character */
		writel(c, port + UART_PL01x_DR);
		return 1;
	}
	else
		return -1;

}

void gps_uart_write(void)
{
	int chk=0;
	if(Nmea_Data_Size){
		chk=gps_putc(Nmea_Buf[Nmea_Buf_Read_Index]);
		if(chk>0)
		{
		Nmea_Buf_Read_Index++;
		Nmea_Data_Size--;
		}
	}
}

void VerifyChecksum(void)
{
	int i;
	if(gps || glonass || gps_acq || glo_acq)
	{

		dbg("glo%d,gps %d: glo_acq %d, gps %d \r\n",glonass,gps,glo_acq,gps_acq);
	}
	if(pattern || pattern_gps)
	{
	   dbg("pat-glo=%d, pat-gps=%d\r\n",pattern,pattern_gps);
	}
	glonass=0;
	gps=0;
	glo_acq=0;
	gps_acq=0;
	pattern=0;
	pattern_gps=0;
	GloDataReadIndex=0;
	for(i=0;i<20;i++)
	{
		if((GloDataRead[i]&0xE400) != 0xE400)
			dbg("%d 0x%x ",i,GloDataRead[i]);
		if((GpsDataRead[i]&0xD800) != 0xD800)
			dbg("%d 0x%x ",i,GpsDataRead[i]);
	}

}

/*
NOTE: In this interrupt handler, We have to transfer 1024 words of GNSS data (512- Glonass, 512- GPS) over GNSS SPI.
We have to receive 1451 words for Glonass Rx buffer, 1451 words for GPS Rx buffer over GNSS SPI. 
*/

static void tenms_interrupt ( void *arg)
{
	tenmsec_interrupt_count = tenmsec_interrupt_count + 1 - ignore_read;
	int i,j, csum;	
	T_U16 index_to_read_buffer =0;   
#ifdef PROFILE_GNSS
	unsigned long int time;

		Previous_ticks=Current_ticks;
		Current_ticks=get_ticks();
		if(Previous_ticks)
		{
			time=((Current_ticks-Previous_ticks)*20)/1000000;
			if(time>12)
			{
				printf("*****interrupt missed \r\n");
			}
		}
#endif

	/* SPI DMA supports 64 bit multiple transactions only*/
        if(spi_xfer_gnss(gnss_slave, (512), Dsp_tx_buffer, Glonass_dsp_rx_buffer , SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("1: Error during SPI transaction - GPS data tx failed\n");

        if(spi_xfer_gnss(gnss_slave, (512), Glonass_dsp_tx_buffer, (&Glonass_dsp_rx_buffer[512]) , SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("3: Error during SPI transaction\n");

       if(spi_xfer_gnss(gnss_slave, 424, NULL, (&Glonass_dsp_rx_buffer[1024]) , SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("2: Error during SPI transaction\n");

        if(spi_xfer_gnss(gnss_slave, 3, NULL, (&Glonass_dsp_rx_buffer[1448]), SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("2: Error during SPI transaction\n");

        /*Now read next 2k data*/
        index_to_read_buffer =0;
        if(spi_xfer_gnss(gnss_slave,1020 , NULL, Dsp_rx_buffer , SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("4: Error during SPI transaction\n");
        if(spi_xfer_gnss(gnss_slave,428 , NULL,(&Dsp_rx_buffer[1020]) , SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("4: Error during SPI transaction\n");

        if(spi_xfer_gnss(gnss_slave, 3, NULL, (&Dsp_rx_buffer[1448] ), SPI_XFER_BEGIN | SPI_XFER_END)!=0)
                dbg("4: Error during SPI transaction\n");

if(ignore_read) {
	ignore_read = 0;

} else {
	
#ifdef DUMP_20INT_DEBUG	
	memcpy(&dumpdata[tenmsec_interrupt_count*2902],Glonass_dsp_rx_buffer,2902);
	memcpy(&dumpdata[(tenmsec_interrupt_count*2902)+1451],Dsp_rx_buffer,2902);
	tenmsec_interrupt_count = tenmsec_interrupt_count + 1 ;
#endif
#ifdef TRACK_DATA_DEBUG
		track_data[0] = Dsp_rx_buffer[1];
		track_data[1] = Dsp_rx_buffer[2];
		write_data = 1;
#endif
	TenMillisecondsTask();
}
}
int do_gnss_test_buff (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])	
{
	printf("The address of Glonass_dsp_rx_buffer = 0x%X\t size=%d\n", Glonass_dsp_rx_buffer,sizeof(Glonass_dsp_rx_buffer));
	printf("The address of Dsp_rx_buffer = 0x%X\t sizr=%d\n", Dsp_rx_buffer,sizeof(Dsp_rx_buffer));
	printf("Dsp_tx_buffer = 0x%X\t Glonass_dsp_tx_buffer 0x%X\n",Dsp_tx_buffer,Glonass_dsp_tx_buffer);
	


	return 0;
	
}

#define PMU_CONFIG_ADDR 0xD45610CC
#define READY_PMU      0xD45610D0
#define TZONE1         0xD456C108
#define TZONE2         0xD456D108
#define PWR_DWN_COMMS   (1 << 5)

int do_gnss_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i,j,k,temp_data;

	temp_data= readl (PMU_CONFIG_ADDR);
       temp_data= temp_data & ~(0x00000040);
        writel (temp_data,PMU_CONFIG_ADDR);
        printf("pmu_addr after gnss power up:0x%x\n\r", temp_data);

       temp_data= readl (PMU_CONFIG_ADDR);

        temp_data= readl (PMU_CONFIG_ADDR);
        temp_data= temp_data & ~(PWR_DWN_COMMS);
        writel(temp_data, PMU_CONFIG_ADDR);
        printf("pmu_addr after bt power up:0x%x\n\r", temp_data);


        mdelay(100);
       writel(0xF0000000,TZONE1);
       writel(0xF0000000,TZONE2);




	dbg("start gps \n");
	if(gps_uart_init())
		dbg("GPS uart init failed\n");
	Init_GlobalData();

	gnss_slave = spi_setup_slave(GNSS_SPI_BUS, GNSS_SPI_CS, GNSS_SPI_CLK,GNSS_SPI_MODE); //struct spi_slave *spi_setup_slave(unsigned int bus, 
												//unsigned int cs, unsigned int max_hz, unsigned int mode)
	if (!gnss_slave) {
		dbg("Invalid device %d:%d\n", 0, 0);
		return 1;
	}	

	interrupt_handler_set(GNSS_INT,tenms_interrupt);
	GpsGlonassApplicationFunction();
	puts("Library Initialised..\n");
	spi_claim_bus(gnss_slave);
	ignore_read = 1;
	//int_enable(GNSS_INT);	
	writel((1<<5),SHIKHARA_SYS_CTRL_BASEADDR+0x44);
	//writel(0x20,0x5d561044);
	mdelay(1000);
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR+0x44);
	//writel(0x0,0x5d561044);
	mdelay(100);	
	printf("Interrupt enabled..\n");
	int_enable(GNSS_INT);	

	while(1){
#ifdef DUMP_20INT_DEBUG 
		if(tenmsec_interrupt_count>=20){
			int_disable(GNSS_INT);
			printf("data dump\n");
			for (i=0;i<20;i++){
				printf("\nint number %d\n",i);
				for (j=0;j<2902;j++){
					printf("0x%X ",dumpdata[j+(2902*i)]);
				}
			}
			printf("\nend of dump\n");
			dbg("glo%d,gps %d: glo_acq %d, gps %d \r\n",glonass,gps,glo_acq,gps_acq);
			return 0;
		}	
#endif
		if(tenmsec_interrupt_count>=20)
		{
			tenmsec_interrupt_count=0;
			VerifyChecksum();
#if 1  
			GpsGlonassTwoFiftymsecTask();
			twohundredmsec_count+=1;
			if(Nmea_Data_Size==0)
			{
				GetNmeaMsg(Nmea_Buf,&Nmea_Data_Size);
			}
			else
			{
				//	dbg("Sending previous data is not finished yet \r\n");
			}
#endif
			/*250ms task shall be called here */
		}
#ifdef TRACK_DATA_DEBUG
		if(write_data == 1)
{

		write_data=0;
		printf("0x%X ",track_data[0]);
		printf("0x%X ",track_data[1]);

		for (i=0;i<1;i++)
		{
			for (k=0;k<2;k++)
			{
			//	while((readl(port + UART_PL01x_FR) & UART_PL01x_FR_TXFF))
			//	{
					/* Send the character */
			//		writel(track_data[k+(i*10)], port + UART_PL01x_DR);
			//	}
							printf("0x%X ",track_data[k]);
			}
		}

}
#endif
#if 1 
		if(twohundredmsec_count >= 5)
		{
			twohundredmsec_count=0;
			GpsReadData(&gps_data_buf);
			if(gps_data_buf.Pos_Avail)
			{
				printf("position available \n");
				printf("Lat=%ld, Lon=%ld, Alt=%ld \r\n",gps_data_buf.Lat,gps_data_buf.Lon,gps_data_buf.Alt);
			}
		}
#endif
#if 1

		if(Nmea_Data_Size)
		{
			//gps_uart_write();
			//dbg("%s",Nmea_Buf);
			if(!(readl(port + UART_PL01x_FR) & UART_PL01x_FR_TXFF))
			{
				/* Send the character */
				writel((Nmea_Buf[Nmea_Buf_Read_Index]), port + UART_PL01x_DR);
				Nmea_Buf_Read_Index++;
				Nmea_Data_Size--;
			}

		}
		else
		{
			Nmea_Buf_Read_Index=0;
		}
#endif
		if (ctrlc ()){
			printf("GPS Interrupted\n");
			int_disable(GNSS_INT);
			return 0;
		}
	}

}
U_BOOT_CMD(
		gnss_test, 1, 1, do_gnss_test,
		" Simple GNSS testcase to test GNSS IP",
		" Prints NMEA messages fro GNSS IP"
	  );
U_BOOT_CMD(
		gnss_test_buff, 1, 1, do_gnss_test_buff,
		" Prints the buffer Phy address",
		" "
	  );

