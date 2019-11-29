#include <common.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <malloc.h>
#include <usb.h>
#include <watchdog.h>
#include <asm/gpio.h>
#include <asm-generic/errno.h>
#include <linux/mtd/compat.h>
#include <linux/usb/dwc3.h>

#include "xhci.h"
#include "xhci-shikhara.h"
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_xhci.h>

#define SOFT_RESET_REG4		0x0054
#define XHCI_RESET		(1 << 19)		/* Bit 19 of System Controller */
#define XHCI_CLEAR		(1 << 19)
#define INCR16			0xe
/* Declare global data pointer */
DECLARE_GLOBAL_DATA_PTR;

/**
 * Contains pointers to register base addresses
 * for the usb controller.
 */
struct shikhara_xhci {
	struct shikhara_usb3_phy *usb3_phy;
	struct xhci_hccr *hcd;
	struct dwc3 *dwc3_reg;
	struct fdt_gpio_state vbus_gpio;
};

static struct shikhara_xhci shikhara;


void dwc3_set_mode(struct dwc3 *dwc3_reg, u32 mode)
{
	clrsetbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
			DWC3_GCTL_PRTCAPDIR(mode));
}

static void dwc3_core_soft_reset(struct dwc3 *dwc3_reg)
{
	u32 host_reset,host_clear;
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_CORESOFTRESET);

	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->g_usb3pipectl[0],
			DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->g_usb2phycfg,
			DWC3_GUSB2PHYCFG_PHYSOFTRST);

#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
	/* Assert System Controller level Global reset */
	host_reset = readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4);		/* Reading Soft reset register 4 of system controller */
	host_reset |= XHCI_RESET;
	writel(host_reset,(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));		/* ?? Since host controller reset will not reset external PHY,
											  // a system controller reset will be required for time being */
#endif
	debug("host_reset = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
	mdelay(100);

#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
	/* Clear System Controller level Global reset */
	host_clear = readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4);		/* Reading Soft reset register 4 of system controller */
	host_clear &= ~(XHCI_CLEAR);
	writel(host_clear,(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
	debug("host_clear = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
#endif
	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb3pipectl[0],
			DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb2phycfg,
			DWC3_GUSB2PHYCFG_PHYSOFTRST);

	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_CORESOFTRESET);


	setbits_le32(&dwc3_reg->g_usb2phycfg,
                        1<<3);
	debug("Addr:%x value usb2phycfg = %x \n",&dwc3_reg->g_usb2phycfg,readl(&dwc3_reg->g_usb2phycfg));
}

static int dwc3_core_init(struct dwc3 *dwc3_reg)
{
	u32 reg;
	u32 revision;
	unsigned int dwc3_hwparams1;

	revision = readl(&dwc3_reg->g_snpsid);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
		puts("this is not a DesignWare USB3 DRD Core\n");
		return -EINVAL;
	}

	dwc3_core_soft_reset(dwc3_reg);

	dwc3_hwparams1 = readl(&dwc3_reg->g_hwparams1);

	reg = readl(&dwc3_reg->g_ctl);
	reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
	reg &= ~DWC3_GCTL_DISSCRAMBLE;
	switch (DWC3_GHWPARAMS1_EN_PWROPT(dwc3_hwparams1)) {
	case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
		reg &= ~DWC3_GCTL_DSBLCLKGTNG;
		break;
	default:
		debug("No power optimization available\n");
	}

	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if ((revision & DWC3_REVISION_MASK) < 0x190a)
		reg |= DWC3_GCTL_U2RSTECN;

	writel(reg, &dwc3_reg->g_ctl);

	/* Setting the burst length as 16Dwords */

        writel(INCR16,&dwc3_reg->g_sbuscfg0);

	return 0;
}

static int shikhara_xhci_core_init(struct shikhara_xhci *shikhara)
{
	
	int ret;

	ret = dwc3_core_init(shikhara->dwc3_reg);
	if (ret) {
		debug("failed to initialize core\n");
		return -EINVAL;
	}

	/* We are hard-coding DWC3 core to Host Mode */
	dwc3_set_mode(shikhara->dwc3_reg, DWC3_GCTL_PRTCAP_HOST);

	return 0;
}

static void shikhara_xhci_core_exit(struct shikhara_xhci *shikhara)
{
	printf("Reseting System controller\n");
	unsigned int host_reset,host_clear;
#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
        /* Assert System Controller level Global reset */
        host_reset = readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4);               /* Reading Soft reset register 4 of system controller */
	debug("Before host_reset = 0x%x\n",host_reset);
        host_reset |= XHCI_RESET;
        writel(host_reset,(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));              /* ?? Since host controller reset will not reset external PHY,
                                                                                           a system controller reset will be required for time being */
	
	debug("After host_reset = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
#endif
        mdelay(100);

#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
        /* Clear System Controller level Global reset */
        host_clear = readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4);               /* Reading Soft reset register 4 of system controller */
	
	debug("Before host_clear = 0x%x\n",host_clear);
        host_clear &= ~(XHCI_CLEAR);
        writel(host_clear,(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
	debug("After host_clear = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
#endif

	return 0;
}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	struct shikhara_xhci *ctx = &shikhara;
	int ret;


	ctx->hcd = (struct xhci_hccr *)(SHIKHARA_USB3_HOST_BASE);


	ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);

	ret = shikhara_xhci_core_init(ctx);
	if (ret) {
		puts("XHCI: failed to initialize controller\n");
		return -EINVAL;
	}

	*hccr = (ctx->hcd);
	*hcor = (struct xhci_hcor *)((uint32_t) *hccr
				+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	debug("Anusoc-xhci: init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)*hccr, (uint32_t)*hcor,
		(uint32_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void xhci_hcd_stop(int index)
{
	struct shikhara_xhci *ctx = &shikhara;

	shikhara_xhci_core_exit(ctx);
}

