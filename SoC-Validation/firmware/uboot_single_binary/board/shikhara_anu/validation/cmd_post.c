#include<common.h>
#include<command.h>

extern int nand_post(void);
extern int nor_post(void);
extern int kmi_post(void);
extern int validation_sram_test(void);
extern int ddr_test(void);
extern int uart_post(void);
extern int i2c_post(void);
extern int led_test(void);
extern int GPIO_TEST(void);
extern int spi_post(void);
extern int hdmi_post(void);
extern int i2s_codec_post(void);

#define DEBUG_POST
#ifdef DEBUG_POST
#define dbg(x...)       printf(x)
#endif

int do_post(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
/*	if(nand_post()==0)
	{
		dbg("NAND Test Pass\n");
	}
	else {
		dbg("NAND Test Fail\n");
	     } 
	if(kmi_post()==0)
	{
		dbg("KMI Test Pass\n");
	}
	else{
		dbg("KMI Test Fail\n");
	    }
*/		
	if(nor_post()==0)
	{
		dbg("NOR Test Pass\n");
		dbg("-----------------\n");
	}
	else{
		printf("NOR Test Fail\n");
	    }
	
	if(validation_sram_test()==0)
	{
		dbg("SRAM Test Pass\n");
		dbg("-----------------\n");
	}
	else {
		printf("SRAM Test Fail\n");
	     }

	 if(ddr_test()==0)
        {
                dbg("DDR Test Pass\n");
		dbg("-----------------\n");
        }
        else {
                printf("DDR Test Fail\n");
	     }
	if(i2c_post()==0)
	{
		dbg("I2C Test Pass\n");
		printf("-----------------\n");
	}
	else {
		dbg("I2C Test Fail\n");
	     }

	if(spi_post()==0)
        {
                dbg("SPI Test Pass\n");
		dbg("-----------------\n");
        }
        else {
                printf("SPI Test Fail\n");
	     }
/*       if(led_test()==0)
	{
		dbg("Seven segment LED Test Pass\n");
	}
	else {
		dbg("Seven segment LED Test Fail\n");
	     }

	if(GPIO_TEST()==0)
        {
                dbg("GPIO Loopback Test Pass\n");
        }
        else {
                dbg("GPIO Loopback Test Fail\n");
	     } */
/*
#ifdef CONFIG_HDMI
	if(hdmi_post()==0)
        {
                dbg("HDMI Test Pass\n");
        }
        else {
                dbg("HDMI Test Fail\n");
             }
#endif
*/
        if(i2s_codec_post()==0)
        {
                dbg("I2S Codec Test Pass\n");
		dbg("-----------------\n");
        }
        else {
                printf("I2S Codec Test Fail\n");
             }
	
/*	if(uart_post()==0)
        {
                dbg("UART Test Pass\n");
        }
        else {
                dbg("UART Test Fail\n");
	     }
*/
	return 0;
}

U_BOOT_CMD(
        post,        4,      0,      do_post,
        "post   - Tests various interfaces for proper functionality\n",
        "post   - Tests various interfaces for proper functionality\n"
        "    \n"
);

