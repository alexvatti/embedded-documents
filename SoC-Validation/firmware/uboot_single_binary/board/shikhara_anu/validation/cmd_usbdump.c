#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <part.h>
#include <usb.h>
#include "../../../drivers/usb/host/xhci.h"
#include <asm/arch/shikhara_map.h>
static int do_usbregdump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	struct xhci_hccr *hccr;
	struct xhci_hcor *hcor;
        int ret,temp;

	hccr = (struct xhci_hccr *)SHIKHARA_USB3_HOST_BASE;
        hcor = (struct xhci_hcor *)((uint32_t) hccr + HC_LENGTH(xhci_readl(&hccr->cr_capbase)));

        printf("Anusoc-xhci: init hccr %x and hcor %x hc_length %d\n",
                (uint32_t)hccr, (uint32_t)hcor,
                (uint32_t)HC_LENGTH(xhci_readl(&hccr->cr_capbase)));
	
        temp = xhci_readl(&hccr->cr_capbase);
        printf("CAPLENGTH AND HCIVERSION 0x%x:\n",(unsigned int) temp);
        printf("CAPLENGTH: 0x%x\n", (unsigned int) HC_LENGTH(temp));
        printf("HCIVERSION: 0x%x\n",(unsigned int) HC_VERSION(temp));

        temp = xhci_readl(&hccr->cr_hcsparams1);
        printf("HCSPARAMS 1: 0x%x\n",(unsigned int) temp);
        printf("  Max device slots: %u\n",(unsigned int) HCS_MAX_SLOTS(temp));
        printf("  Max interrupters: %u\n",(unsigned int) HCS_MAX_INTRS(temp));
        printf("  Max ports: %u\n",(unsigned int) HCS_MAX_PORTS(temp));

        temp = xhci_readl(&hccr->cr_hcsparams2);
        printf("HCSPARAMS 2: 0x%x\n",(unsigned int) temp);
        printf("  Isoc scheduling threshold: %u\n", (unsigned int) HCS_IST(temp));
        printf("  Maximum allowed segments in event ring: %u\n",(unsigned int) HCS_ERST_MAX(temp));

        temp = xhci_readl(&hccr->cr_hcsparams3);
        printf("HCSPARAMS 3 0x%x:\n",(unsigned int) temp);
        printf("  Worst case U1 device exit latency: %u\n", (unsigned int) HCS_U1_LATENCY(temp));
        printf("  Worst case U2 device exit latency: %u\n", (unsigned int) HCS_U2_LATENCY(temp));

        temp = xhci_readl(&hccr->cr_hccparams);
        printf("HCC PARAMS 0x%x:\n", (unsigned int) temp);
        printf("  HC generates %s bit addresses\n", HCC_64BIT_ADDR(temp) ? "64" : "32");
        temp = xhci_readl(&hccr->cr_rtsoff);
        printf("RTSOFF 0x%x:\n", temp & RTSOFF_MASK);

        printf("xHCI operational registers at %p:\n", hcor);
        temp = xhci_readl(&hcor->or_usbcmd);
        printf("USBCMD 0x%x:\n", temp);
        printf("  HC is %s\n", (temp & CMD_RUN) ? "running" : "being stopped");
        printf("  HC has %sfinished hard reset\n", (temp & CMD_RESET) ? "not " : "");
        printf("  Event Interrupts %s\n", (temp & CMD_EIE) ? "enabled " : "disabled");
        printf("  Host System Error Interrupts %s\n", (temp & CMD_HSEIE) ? "enabled " : "disabled");
        printf("  HC has %sfinished light reset\n", (temp & CMD_LRESET) ? "not " : "");

        temp = xhci_readl(&hcor->or_usbsts);
        printf("USBSTS 0x%x:\n", temp);
        printf("  Event ring is %sempty\n", (temp & STS_EINT) ? "not " : "");
        printf("USBSTS 0x%x:\n", temp);
        printf("  Event ring is %sempty\n",(temp & STS_EINT) ? "not " : "");
        printf("  %sHost System Error\n", (temp & STS_FATAL) ? "WARNING: " : "No ");
	
	printf("  HC has %sfinished light reset\n", (temp & CMD_LRESET) ? "not " : "");

        temp = xhci_readl(&hcor->or_usbsts);
        printf("USBSTS 0x%x:\n", temp);
        printf("  Event ring is %sempty\n", (temp & STS_EINT) ? "not " : "");
        printf("USBSTS 0x%x:\n", temp);
        printf("  Event ring is %sempty\n",(temp & STS_EINT) ? "not " : "");
        printf("  %sHost System Error\n", (temp & STS_FATAL) ? "WARNING: " : "No ");
        printf("  HC is %s\n", (temp & STS_HALT) ? "halted" : "running");


        printf("port registers :  \n");
        unsigned int *addr;
        int i,j;
        int ports;
        char *names[4]={"status", "power","link", "reserved"};
	temp = xhci_readl(&hccr->cr_hcsparams1);
        ports = HCS_MAX_PORTS(temp);
        addr =&hccr->cr_hcsparams1;
        for (i = 0; i < ports; i++) {
                for (j = 0; j < 4; ++j) {
                        printf("%p port %s reg = 0x%x\n", addr, names[j], (unsigned int) xhci_readl(addr));
                        addr++;
                }
        }

}
U_BOOT_CMD(
        usbregdump,        3,      1,      do_usbregdump,
        "display the values of the registers...",
        "loadAddr dev:part"
);


