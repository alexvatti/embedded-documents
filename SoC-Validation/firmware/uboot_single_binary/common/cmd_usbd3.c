#include <common.h>
#include <command.h>
#include "../drivers/usb/gadget/regs-usb-device.h"

#include "../drivers/usb/gadget/usbd3-ss.h"

#define PMU_CONFIG_ADDR 0xD45610CC
#define READY_PMU      0xD45610D0
#define TZONE1         0xD456C108

static const char pszMe[] = "fut: ";

int do_fut(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{

#ifdef CONFIG_SHIKHARA_ASIC
       unsigned int temp_data= readl (PMU_CONFIG_ADDR);
       debug("config pmu:0x%X\n",temp_data);
       temp_data= temp_data & ~(0x00000002);
        //writel (PMU_CONFIG_ADDR ,temp_data);
        writel (temp_data,PMU_CONFIG_ADDR);
       debug("config pmu:0x%X\n",temp_data);
       temp_data= readl (PMU_CONFIG_ADDR);
        debug("config pmu:0x%X\n",temp_data);

       while( readl(READY_PMU) & 0x0C);

       writel(0xF0000000,TZONE1);
#endif




	if (argv[0][0] == 'u') {
		DNW = 0;
	}
	else {
		DNW = 1;
		shikhara_got_header = 0;
	}

	switch (argc) {
	case 1 :
		shikhara_usbd_dn_addr = SHIKHARA_USB_DEVICE_STORAGE_START_OFFSET;	/* Default Address */
		break;
	case 2 :
		shikhara_usbd_dn_addr = simple_strtoul(argv[1], NULL, 16);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	shikhara_receive_done = 0;

	shikhara_usbctl_init();
	shikhara_usbc_activate();

	printf("Download address 0x%08x\n", shikhara_usbd_dn_addr);
	printf("Now, Waiting for USB Host to transmit data\n");

	while (1) {
		if (ANUSOC_USBD_DETECT_IRQ()) {		// Polling for the Interrupt bit in Eventcount register
			shikhara_udc_int_hndlr();
			ANUSOC_USBD_CLEAR_IRQ();
		}

		if (shikhara_receive_done)
			break;

		if (ctrlc ()) {
			break;
		}
	}

	/* when operation is done, usbd must be stopped */
	shikhara_usb_stop();

	return 0;
}

U_BOOT_CMD(
	   fut,3,0,do_fut,
	   "USBD3 Controller-initialisation\n",
	   "fut addr  - initialize USB device and ready to receive for Windows server (specific)\n"
);


