#include <common.h>
#include <command.h>

static const char pszMe[] = "usbdev_regdump: ";

int do_usbdev_regdump(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{

        int *ptr;

        ptr = SHIKHARA_USB3_HOST_BASE;

        printf("Shikara registers starts at %p\n",ptr);
        printf("Device Register Map:\n");
        printf("DCFG: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC700,readl(SHIKHARA_USB3_HOST_BASE+0xC700));
        printf("DCTL: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC704,readl(SHIKHARA_USB3_HOST_BASE+0xC704));
        printf("DEVTEN: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC708,readl(SHIKHARA_USB3_HOST_BASE+0xC708));
        printf("DSTS: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC70C,readl(SHIKHARA_USB3_HOST_BASE+0xC70C));
        printf("DGCMDPAR: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC710,readl(SHIKHARA_USB3_HOST_BASE+0xC710));
        printf("DGCMD: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC714,readl(SHIKHARA_USB3_HOST_BASE+0xC714));
        printf("DALEPENA: Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC718,readl(SHIKHARA_USB3_HOST_BASE+0xC720));
        printf("DEPCMDPAR2(0): Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC71C,readl(SHIKHARA_USB3_HOST_BASE+0xC800));
        printf("DEPCMDPAR1(0): Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC720,readl(SHIKHARA_USB3_HOST_BASE+0xC804));
        printf("DEPCMDPAR0(0): Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC800,readl(SHIKHARA_USB3_HOST_BASE+0xC808));
        printf("DEPCMD(0) : Address at %p and value %X\n",SHIKHARA_USB3_HOST_BASE+0xC804,readl(SHIKHARA_USB3_HOST_BASE+0xC80C));

	printf("Global Registers:\n");

	printf ("GSBUSCFG0      %X\n ", readl(0X5d30C100)); 
	printf ("GSBUSCFG1      %X\n ", readl(0x5d30C104)); 
	printf ("GTXTHRCFG     %X \n ", readl(0x5d30C108)); 
	printf ("GRXTHRCFG       %X\n", readl(0x5d30C10C)); 
	printf ("GCTL PwrDnScale %X\n", readl(0x5d30C110)); 
	printf ("GPMSTS          %X\n", readl(0x5d30C114)); 
	printf ("GSTS CBELT      %X\n", readl(0x5d30C118)); 
	printf ("GUCTL1          %X\n", readl(0x5d30C11C)); 
	printf ("GSNPSID         %X\n", readl(0x5d30C120)); 
	printf ("GGPIO           %X\n", readl(0x5d30C124)); 
	printf ("GUID            %X\n", readl(0x5d30C128)); 
	printf ("GUCTL           %X\n", readl(0x5d30C12C)); 
	printf ("GBUSERRADDR     %X\n", readl(0x5d30C130)); 
	printf ("GBUSERRADDR     %X\n", readl(0x5d30C134)); 
	printf ("GPRTBIMAP       %X\n", readl(0x5d30C138)); 
	printf ("GPRTBIMAP       %X\n", readl(0x5d30C13C)); 
	printf ("GHWPARAMS0      %X\n", readl(0x5d30C140)); 
	printf ("GHWPARAMS1      %X\n", readl(0x5d30C144)); 
	printf ("GHWPARAMS2      %X\n", readl(0x5d30C148)); 
	printf ("GHWPARAMS3      %X\n", readl(0x5d30C14C)); 
	printf ("GHWPARAMS4      %X\n", readl(0x5d30C150)); 
	printf ("GHWPARAMS5      %X\n", readl(0x5d30C154)); 
	printf ("GHWPARAMS6      %X\n", readl(0x5d30C158)); 
	printf (	"GHWPARAMS7     %X \n", readl(0x5d30C15C)); 
	printf ("GDBGFIFOSPACE   %X\n", readl(0x5d30C160)); 
	printf ("GDBGLTSSM       %X\n", readl(0x5d30C164)); 
	printf ("GDBGLNMCC       %X\n", readl(0x5d30C168)); 
	printf ("GDBGBMU BMU_BCU %X\n", readl(0x5d30C16C)); 
	printf ("GDBGLSPMUX Rsvd %X\n", readl(0x5d30C170)); 
	printf ("GDBGLSP LSP     %X\n", readl(0x5d30C174)); 
	printf ("GDBGEPINFO0     %X\n", readl(0x5d30C178)); 
	printf ("GDBGEPINFO1     %X\n", readl(0x5d30C17C)); 

        return 0;
}

U_BOOT_CMD(
           usbdev_regdump,3,0,do_usbdev_regdump,
           "USBD3 Controller-initialisation\n",
           "usbbdev_regdump  - initialize USB device and ready to receive for Windows server (specific)\n"
);

