#ifndef __SHIKHARA_XHCI_H__
#define __SHIKHARA_XHCI_H__


#ifdef CONFIG_SHIKHARA_ASIC
#define ANUSOC_XHCI_BASE 	0xD4300000
#endif

#define ANUSOC_PHY_BASE		0x12100000	// ?? SAMSUNG PHY base address as ANUSOC not yet confirmed
#define ANUSOC_POWER_BASE	0x10040000	// ?? SAMSUNG POWER base address as ANUSOC not yet confirmed

#define USB_DEVICE_PHY_CONTROL_OFFSET	0x0704	// ?? Offset of PHY in Power Control Register
#define USB_DEVICE_PHY_CONTROL			(ANUSOC_POWER_BASE+USB_DEVICE_PHY_CONTROL_OFFSET)
#endif

