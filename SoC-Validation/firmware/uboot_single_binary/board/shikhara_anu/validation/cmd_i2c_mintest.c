#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <i2c.h>
#include <asm/arch/shikhara_i2c.h>

//#define DEBUG_I2C
#ifdef DEBUG_I2C
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

/*
 * get_alen: small parser helper function to get address length
 * returns the address length
 */
static uint get_alen(char *arg)
{
	int     j;
	int     alen;

	alen = 1;
	for (j = 0; j < 8; j++) {
		if (arg[j] == '.') {
			alen = arg[j+1] - '0';
			break;
		} else if (arg[j] == '\0')
			break;
	}
	return alen;
}

static unsigned int i2ctest_read(unsigned char chip,unsigned int reg,unsigned int reg_len, 
		unsigned short *data,unsigned int count)
{
	int ret;
	dbg("%s: Read Addr  yours: 0x%02X\n",
			__func__, reg);
	ret = i2c_read(chip , reg,reg_len, (unsigned char *)data, count);
	if (ret != 0) {
		dbg("Error while reading register %#04x\n", reg);
		return -1;
	}

	return 0;
}

static unsigned int i2ctest_write(unsigned char chip,unsigned int reg,unsigned int reg_len, 
		unsigned short data,unsigned int count)
{
	dbg("%s: Write Addr  yours: 0x%02X, Data :  0x%02X\n",
			__func__, reg, data);
	return i2c_write(chip,reg,reg_len,(unsigned char *)&data,count);
}

int i2c_mintest(char chip,ulong addr,uint alen,uchar byte,int count)
{
	unsigned short tmp_data;
	dbg(" ------------- doing write operation -------------\n");
	i2ctest_write(chip,addr,alen,byte,count);
	unsigned char data[alen];
	dbg("-------------- doing read operation --------------\n");
	i2ctest_read(chip,addr,alen,&data,count);
	if(alen==2){
		tmp_data=data[1]<<8|data[0];
	}
	else
	{
		tmp_data=data[0];
	}
	dbg("value of byte written %x data read data[0]%x data[1]%x\n",byte,data[0],data[1]);
	if(tmp_data==byte)
	{
		printf("i2c test is successfully completed---\n");
	}
	else
	{
		printf("i2c test failed---\n");
		return -1;
	}
	return 0;
}

int i2c_post()
{ 
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
		i2c_set_bus_num(I2C_BUS0);
        return i2c_mintest(0x50,0x0,0x1,0xff,0x1);
}

int do_i2c_mintest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char   chip;
	ulong   addr;
	uint    alen;
	uchar   byte;
	int     count;
	/*
	 * Chip is always specified.
	 */
	chip = simple_strtoul(argv[1], NULL, 16);
	/*
	 * Address is always specified.
	 */
	addr = simple_strtoul(argv[2], NULL, 16);
	alen = get_alen(argv[2]);
	if (alen > 3)
		return CMD_RET_USAGE;
	/*
	 * Value to write is always specified.
	 */
	byte = simple_strtoul(argv[3], NULL, 16);
	/*
	 * Optional count
	 */
	if (argc == 5)
		count = simple_strtoul(argv[4], NULL, 16);
	else
		count = 1;
	i2c_mintest(chip,addr,alen,byte,count);
}        

int do_i2c_eeprom_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

       char txbuf[256];
       char rxbuf[256];
       int i=0,j=0,ret=0;
       char   chip=0x50;
       ulong   addr=0;
       uint    reg_len=2;
       unsigned int count=64;
       char localbuf[16];

       for(i=0;i<256;i++)
               txbuf[i]=0x55;

       printf("This is EEPROM Interfaced on I2C, Address=0x%x\n",chip);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
       i2c_set_bus_num(I2C_BUS0);
       
       printf("Going to write to device location=0x%x of data=0x%x, length=%d\n",addr,txbuf[0],count*4);
       ret= i2c_write(chip,addr,reg_len,(unsigned char *)&txbuf,count);
       addr=0x40;
       ret= i2c_write(chip,addr,reg_len,(unsigned char *)&txbuf[0x40],count);
       addr=0x80;
       ret= i2c_write(chip,addr,reg_len,(unsigned char *)&txbuf[0x80],count);
       addr=0xc0;
       ret= i2c_write(chip,addr,reg_len,(unsigned char *)&txbuf[0xc0],count);

       addr=0;
       printf("Going to Read from device location=0x%x\n",addr);
       addr=0;
       for(i=0;i<16;i++) {
               ret = i2c_read(chip , addr,reg_len, (unsigned char *)&localbuf, 15);
               ret=memcmp(localbuf,txbuf,15);
               if(ret != 0) printf("Write and Read fails at addr=0x%x\n",addr);
               addr= addr + 0x10;
       }
       printf("I2C EEPROM Write and Read Success of data=0x%x\n",txbuf[0]);
}

U_BOOT_CMD(
		i2c_mintest,5 ,      1,      do_i2c_mintest,
		"I2C test command",
		"i2c_mintest chip addr value count"
	  );

U_BOOT_CMD(
       i2c_eeprom_test,1 ,      1,      do_i2c_eeprom_test,
       "I2C EEPROM test command to write , read and veirfy",
       "I2C EEPROM Writem Read verification"
);
