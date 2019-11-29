

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/shikhara_i2c.h>
#include <i2c.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_map.h>

//define DEBUG_I2C
#ifdef DEBUG_I2C
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

static unsigned int current_bus;

static struct i2c_regs *i2c_regs_p =
    (struct i2c_regs *)SHIKHARA_IC_BASE;

/*
 * set_speed - Set the i2c speed mode (standard, high, fast)
 * @i2c_spd:	required i2c speed mode
 *
 * Set the i2c speed mode (standard, high, fast)
 */
static void set_speed(int i2c_spd)
{
	unsigned int cntl;
	unsigned int enbl;

	/* to set speed cltr must be disabled */
	enbl = readw(&i2c_regs_p->ic_enable);
	enbl &= ~IC_ENABLE_0B;
	writew(enbl, &i2c_regs_p->ic_enable);

	cntl = (readw(&i2c_regs_p->ic_con) & (~IC_CON_SPD_MSK));

	switch (i2c_spd) {
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_HS;
		writew(HS_SCL_HIGHCOUNT, &i2c_regs_p->ic_hs_scl_hcnt);
        	writew(HS_SCL_LOWCOUNT, &i2c_regs_p->ic_hs_scl_lcnt);
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
                writew(SS_SCL_HIGHCOUNT, &i2c_regs_p->ic_ss_scl_hcnt);
	        writew(SS_SCL_LOWCOUNT, &i2c_regs_p->ic_ss_scl_lcnt);
         	break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
                writew(FS_SCL_HIGHCOUNT, &i2c_regs_p->ic_fs_scl_hcnt);
 		writew(FS_SCL_LOWCOUNT, &i2c_regs_p->ic_fs_scl_lcnt);
                break;
	}


	writew(cntl, &i2c_regs_p->ic_con);

	/* Enable back i2c now speed set */
	enbl |= IC_ENABLE_0B;
	writew(enbl, &i2c_regs_p->ic_enable);
}

/*
 * i2c_set_bus_speed - Set the i2c speed
 * @speed:	required i2c speed
 *
 * Set the i2c speed.
 */
int i2c_set_bus_speed(unsigned int speed)
{
	if (speed >= IC_MAX_SPEED)
		set_speed(IC_SPEED_MODE_MAX);
	else if (speed >= IC_FAST_SPEED)
		set_speed(IC_SPEED_MODE_FAST);
	else
		set_speed(IC_SPEED_MODE_STANDARD);

	return 0;
}

/*
 * i2c_get_bus_speed - Gets the i2c speed
 *
 * Gets the i2c speed.
 */
unsigned int i2c_get_bus_speed(void)
{
	u32 cntl;

	cntl = (readw(&i2c_regs_p->ic_con) & IC_CON_SPD_MSK);

	if (cntl == IC_CON_SPD_HS)
		return IC_MAX_SPEED;
	else if (cntl == IC_CON_SPD_FS)
		return IC_FAST_SPEED;
	else if (cntl == IC_CON_SPD_SS)
		return IC_STANDARD_SPEED;

	return 0;
}

/*
 * i2c_init - Init function
 * @speed:	required i2c speed
 * @slaveadd:	slave address for the device
 *
 * Initialization function.
 */
void i2c_init(int speed, int slaveadd)
{
	dbg("coming inside i2c_init--\n");
        unsigned int enbl;
	/* Disable i2c */
	enbl = readw(&i2c_regs_p->ic_enable);
	enbl &= ~IC_ENABLE_0B;
	writew(enbl, &i2c_regs_p->ic_enable);

	writew((IC_CON_SD | IC_CON_SPD_FS | IC_CON_MM), &i2c_regs_p->ic_con); 
	writew(IC_RX_TL, &i2c_regs_p->ic_rx_tl);
	writew(IC_TX_TL, &i2c_regs_p->ic_tx_tl);
	i2c_set_bus_speed(IC_FAST_SPEED);
	writew(IC_STOP_DET, &i2c_regs_p->ic_intr_mask);
	writew(slaveadd, &i2c_regs_p->ic_sar);

	/* Enable i2c */
	enbl = readw(&i2c_regs_p->ic_enable);
	enbl |= IC_ENABLE_0B;
	writew(enbl, &i2c_regs_p->ic_enable);


}

/*
 * i2c_setaddress - Sets the target slave address
 * @i2c_addr:	target i2c address
 *
 * Sets the target slave address.
 */
static void i2c_setaddress(unsigned int i2c_addr)
{
        unsigned int addr;
        unsigned int enbl;

        enbl = readw(&i2c_regs_p->ic_enable);
        enbl &= ~IC_ENABLE_0B;
        writew(enbl, &i2c_regs_p->ic_enable);

	writew(i2c_addr, &i2c_regs_p->ic_tar);
        addr = readw(&i2c_regs_p->ic_tar);

        enbl = readw(&i2c_regs_p->ic_enable);
        enbl |= IC_ENABLE_0B;
        writew(enbl, &i2c_regs_p->ic_enable);
       
}




/*
 * i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int i2c_wait_for_bb(void)
{
	unsigned long start_time_bb = get_timer(0);

	while ((readw(&i2c_regs_p->ic_status) & IC_STATUS_MA) ||
	       !(readw(&i2c_regs_p->ic_status) & IC_STATUS_TFE)) {

		/* Evaluate timeout */
		if (get_timer(start_time_bb) > (unsigned long)(IC_BYTE_TO_BB))
			return 1;
	}

	return 0;
}

/* check parameters for i2c_read and i2c_write */
static int check_params(uint addr, int alen, uchar *buffer, int len)
{
	if (buffer == NULL) {
		printf("Buffer is invalid\n");
		return 1;
	}
	return 0;
}

static int i2c_xfer_init(uchar chip, uint addr,int alen)
{
        dbg("alen=%x\n",alen);	

        if (i2c_wait_for_bb()) {
          printf(" i2c bus access failed \n");
	  return 1;
        }

	i2c_setaddress(chip);

        while(alen--)
        {
	  writew((addr>>(alen*8))&0xff, &i2c_regs_p->ic_cmd_data);
        }
	return 0;
}

static int i2c_xfer_finish(void)
{
	ulong start_stop_det = get_timer(0);

	while (1) {
		if ((readw(&i2c_regs_p->ic_raw_intr_stat) & IC_STOP_DET)) {
			readw(&i2c_regs_p->ic_clr_stop_det);
			break;
		} else if (get_timer(start_stop_det) > IC_STOPDET_TO) {
			break;
		}
	}

	if (i2c_wait_for_bb()) {
		printf("Timed out waiting for bus\n");
		return 1;
	}


	/* Wait for read/write operation to complete on actual memory */
	udelay(10000);

	return 0;
}

/*
 * i2c_read - Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
        int i,num_read;
        unsigned long start_time_rx;
        unsigned short status;
        unsigned int loop_counter;
        dbg("len= %x alen=%x\n",len,alen);
        num_read = len;
        if (check_params(addr, alen, buffer, len))
        { 
                printf(" wrong i2c params \n");
                return 1;
        }
        if (i2c_xfer_init(chip, addr, alen))
        {
                printf(" i2c transfer failed \n");
                return 1;
        }
                for(i= num_read-1;i>=0;i--)
                {
               	  writew(IC_CMD, &i2c_regs_p->ic_cmd_data);
                }
                udelay(1000);
                dbg("\nREADING\n  ");
                status = readw(&i2c_regs_p->ic_status);
                dbg("I2C status = 0x%x\n",status);

                for(i=len-1;i>=0;i--)
                {
                  loop_counter = 20;
                  udelay(1000);

                  while(!(readw(&i2c_regs_p->ic_status) & IC_STATUS_RFNE))
                  {
                     loop_counter--;
                     if(loop_counter==0)
                     break;
                  }
 
                  if(loop_counter==0)
                  {
                     //printf("read failed \n");
                     return 1;
                  }

                  buffer[i] = (uchar)readw(&i2c_regs_p->ic_cmd_data);
                }
        return i2c_xfer_finish();
}

/*
 * i2c_write - Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int i,al;
        unsigned char buf;
	unsigned long start_time_tx;
        dbg("len=%x\n",len);
	if (check_params(addr, alen, buffer, len))
        {
           printf(" wrong i2c params \n");
	   return 1;
        }
	if (i2c_xfer_init(chip, addr, alen))
        {
           printf(" i2c transfer failed \n");
	   return 1;
        }
        while(len)
        { 
           while(!(readw(&i2c_regs_p->ic_status) & IC_STATUS_TFNF));
           al=len-1;     
	   writew(buffer[al],&i2c_regs_p->ic_cmd_data);
           len--;
        }

	return i2c_xfer_finish();
}

/*
 * i2c_probe - Probe the i2c chip
 */
int i2c_probe(uchar chip)
{
	u32 tmp;
	int ret;

	/*
	 * Try to read the first location of the chip.
	 */
	ret = i2c_read(chip, 0, 1, (uchar *)&tmp, 1);
	if (ret)
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	return ret;
}

#ifdef CONFIG_I2C_MULTI_BUS
int i2c_set_bus_num(unsigned int bus)
{
current_bus = bus;
i2c_regs_p =(struct i2c_regs *)(SHIKHARA_IC_BASE+SHIKHARA_IC_SPACING*current_bus);
return 0;
}

unsigned int i2c_get_bus_num(void)
{
	return current_bus;
}
#endif
