#include <common.h>
#include <command.h>
#include <spi.h>
#include <asm/arch/shikhara_tc.h>
#include <asm/io.h>

extern int ts_calibrate(struct spi_slave *slave);

#define TOUCH_SPI_BUS	1
#define TOUCH_SPI_CS	0
#define TOUCH_SPI_HZ	3250000

int do_touch (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret=0;
	struct spi_slave *spi;
	char *cmd;
	if(argc < 2)
		goto usage;
	spi = touch_init(TOUCH_SPI_BUS, TOUCH_SPI_CS,TOUCH_SPI_HZ,SPI_MODE_0);
	if(!spi){
		printf("Problem in intialistaion of touch screen \n");
		return 0;
	}
	printf("SPI Initialised..\n");
	cmd = argv[1];
	if (strcmp(cmd, "print") == 0)
		ret = touch_read(spi);
	else if(strcmp(cmd, "calibrate") == 0)
		ret = ts_calibrate(spi);
	else 
		goto usage;

	touch_release(spi);
	printf("\n\n**********Exiting Touch Screen Test***********\n\n");	
	return ret;
usage:
	return cmd_usage(cmdtp);

}

U_BOOT_CMD(
		ts,     2,      1,      do_touch,
		" - Touch Screen commands \n",
		"calibrate - Calibrate the touchscreen controller \n"
		"     print - Prints touched positions\n"
	  );


