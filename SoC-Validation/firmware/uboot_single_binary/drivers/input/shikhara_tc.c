#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi.h>
#include <asm/arch/shikhara_tc.h>

struct touch_event{
	int 	model;
        struct  spi_slave        *spi;
	int 	last;
	int	rep_cnt ;
};

struct touch_event *ts;	
unsigned int vref =1 ;
int spi_ts_xfer(struct spi_slave *spi, const u8 dataout, u8 *response, int len)
{
	unsigned long flags = SPI_XFER_BEGIN;
	int ret;

	if (len == 0)	
	flags |= SPI_XFER_END;

	ret = spi_xfer(spi,8, &dataout,0,flags); 	/*Command is single byte*/
	if (ret) {
		printf("Failed to send command %x: %d\n", dataout, ret);
		return ret;
	}
	if (len) {
		ret = spi_xfer(spi,len*8,0,response, SPI_XFER_END); 	/* Recieve 2-bytes data*/
		if (ret){
		printf("Failed to read response %u: %d\n",len, ret);
			return ret;
		}
	}
	//printf("Transmitted = %X and Recieved = %X\n",dataout, *(u16 *)response);
	return ret;
}

int start_xfer(struct spi_slave *spi, const u8 command)
{
	int ret;
	u16 val, rx_buf;

        ret=spi_ts_xfer(spi,command,(u8 *)&rx_buf,sizeof(rx_buf));
         if (ret) {
                printf("Failed to send command %x: %d\n",READ_Y(vref), ret);
                return ret;
                }
	/* adjust:  on-wire is a must-ignore bit, a BE12 value, then padding;
	 * built from two 8 bit values written msb-first.
	 */
        val =be16_to_cpu(rx_buf) >> 3;	
	
	if (command== PWRDOWN)
	return val;
	
	/* Repeat it, if it  wasn't consistent enough. */
	if ((ts->rep_cnt++<2) && (abs(ts->last-val) > -1)){
       	//printf("****Repeat*** and count is %d\n",ts->rep_cnt);
		ts->last = val;
		start_xfer(spi,command);
	}
	else{
		ts->last = val;
		ts->rep_cnt =0;
	}
	return val;
}

void get_position(unsigned int* px_pos, unsigned int* py_pos,struct spi_slave *slave)
{
    unsigned int val;
	printf("....Please touch the LCD Screen.... \n");
    while(1){
	val= (readl(SHIKHARA_SYS_CTRL_BASEADDR+0x214) & 0x40);
	if(!val) {
		*px_pos = start_xfer(slave,READ_X(vref));	/* Read X Position */
		*py_pos = start_xfer(slave,READ_Y(vref));	/* Read Y Position */		
		/* Power Down mode */
		start_xfer(slave,PWRDOWN);
		udelay(200000);
		break;
		
	 }
      }
   return ;
}
struct spi_slave  *touch_init(unsigned int bus, unsigned int cs,unsigned int max_hz, unsigned int spi_mode)
{
	struct spi_slave 	*spi;
	int 			ret;
	u16 			rx_buf;
	u8			command;

	ts = (struct touch_event *)malloc(sizeof(struct touch_event));
	if (!ts) {
                printf("Failed to initialise touchscreen \n");
                return NULL;
        }
	
	spi = spi_setup_slave(bus, cs, max_hz, spi_mode);
	if (!spi) {
		printf("Failed to set up slave\n");
		return 0;
	}

	ts->spi=spi;
	ts->model = 7843;

	ret = spi_claim_bus(spi);
	if (ret) {
		printf("Failed to claim SPI bus: %d\n", ret);
		return 0;
	}
	/* 7846 has Internal reference voltage	
	 * turn on internal vREF, and let it settle */
	if(ts->model==7846) {
		ret=spi_ts_xfer(spi,REF_ON,(u8 *)&rx_buf, sizeof(rx_buf));
        	if (ret) {
                	printf("Failed to send command %x: %d\n",REF_ON, ret);
                	return 0;
        	}
		udelay(1000);
	}

	/* Take sample value in single end mode*/
	command= READ_12BIT_SER(vaux) | ADS_PD10_ALL_ON;	

        ret=spi_ts_xfer(spi,command,(u8 *)&rx_buf, sizeof(rx_buf));
        if (ret) {
                printf("Failed to send command %x: %d\n",command, ret);
                return 0;
        }
	 /* Power down the device and enable PENIRQ */
        ret=spi_ts_xfer(spi,PWRDOWN,(u8 *)&rx_buf, sizeof(rx_buf));	
        if (ret) {
                printf("Failed to send command %x: %d\n",PWRDOWN, ret);
                return 0;
        }
	return spi;
}
int touch_read( struct spi_slave *spi)
{
	unsigned int  x, y, z1, z2;
	unsigned int  Rt=0;
	
	/* PENIRQ goes LOW  when u touch, then take readings.
	 * Be in loop until, u stop the test (ctrl+c)*/
	while(!ctrlc()){
			get_position(&x,&y,spi);	

			/* Calculate Pressure using Z1 and Z2 in ADS 7846 */
			if(ts->model== 7846){
				z1 = start_xfer(spi,READ_Z1(vref));
			 	z2 = start_xfer(spi,READ_Z2(vref));
				/* Computing touch pressure resistance */	
				Rt = z2;
				Rt -= z1;
				Rt *= x;
				Rt *= 450;
				Rt /= z1;
				Rt = (Rt + 2047) >> 12;
			}
	
			/* Power down the device after taking samples and 
		 	* enable PENIRQ to detect touch*/
			start_xfer(spi,PWRDOWN);
	
			printf("X:=%4X\t Y:=%4X\t Rt=%X \n\n",x,y,Rt);
  	   }
	return 0;     
}

void touch_release(struct spi_slave *spi)
{
	spi_release_bus(spi);
	return ;
}
	
