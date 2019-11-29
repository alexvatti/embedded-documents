#include <common.h>
#include <malloc.h>
#include <command.h>
#include <asm/errno.h>
#include <asm/io.h>
#include "../host/regs-usb3-shikhara-drd-phy.h"
#include "./regs-usb-device.h"
#include "usbd3-ss.h"
#include <asm/arch/shikhara_xhci.h>
#include <asm/arch/shikhara-usbgadget.h>
#include <asm/cache.h>

#define DWC3_GCTL_PRTCAPDIR(n)	((n) << 12)
#define DWC3_GCTL_PRTCAP_OTG	3
#define DWC3_DCTL_HIRD_THRES_MASK	(0x1f << 24)
#define DWC3_DCTL_L1_HIBER_EN	(1 << 18)
#define DWC3_DCTL_HIRD_THRES(n)	((n) << 24)

//#define USBD3_DBG_VER
#ifdef USBD3_DBG_VER
#define DBG_USBD3v(fmt, args...) printf( fmt, ##args)
#else
#define DBG_USBD3v(fmt, args...) do { } while (0)
#endif

//#define USBD3_DBG
#ifdef USBD3_DBG
#define DBG_USBD3(fmt, args...)	printf( fmt, ##args)
#else
#define DBG_USBD3(fmt, args...) do { } while (0)
#endif
#define Assert(value) value ? : printf("[%s:%d] Assert(%d) \n", __func__, __LINE__, value)

#define SHIKHARA_USB3_GCTL_PwrDnScale_MASK                (0x1fff << 19)
#define SHIKHARA_USB3_GCTL_DisScramble                    (1 << 3)

#define SOFT_RESET_REG4 0x0054
#define DRD_RESET (1 << 17) 		/* Bit 17 of System Controller Software Reset Register 4  */
#define DRD_CLEAR (1 << 17)

#define INCR16		(0xe)		/* Enabling 4/8/16 beat burst for AHB/AXI Master */

void drd_flush_cache(uint32_t addr, u32 len)
{
	BUG_ON((void *)addr == NULL || len == 0);

	flush_dcache_range(addr & ~(CONFIG_SYS_CACHELINE_SIZE - 1),
				ALIGN(addr + len, CONFIG_SYS_CACHELINE_SIZE));
}

void drd_inval_cache(uint32_t addr, u32 len)
{
	BUG_ON((void *)addr == NULL || len == 0);

	invalidate_dcache_range(addr & ~(CONFIG_SYS_CACHELINE_SIZE - 1),
				ALIGN(addr + len, CONFIG_SYS_CACHELINE_SIZE));
}
const u8 string_desc0[] =
{
	4, STRING_DESCRIPTOR, LANGID_US_L, LANGID_US_H,
};

const u8 dnw_string_desc1[] = 
{
	(0x14+2), STRING_DESCRIPTOR,
	'S', 0x0, 'y', 0x0, 's', 0x0, 't', 0x0, 'e', 0x0,
	'm', 0x0, ' ', 0x0, 'M', 0x0, 'C', 0x0, 'U', 0x0,
};

const u8 dnw_string_desc2[] =
{
	(0x2a+2), STRING_DESCRIPTOR,
	'S', 0x0, 'E', 0x0, 'C', 0x0, ' ', 0x0, 'S', 0x0,
	'3', 0x0, 'C', 0x0, '6', 0x0, '4', 0x0, '0', 0x0,
	'0', 0x0, 'X', 0x0, ' ', 0x0, 'T', 0x0, 'e', 0x0,
	's', 0x0, 't', 0x0, ' ', 0x0, 'B', 0x0, '/', 0x0,
	'D', 0x0
};

/* setting the device qualifier descriptor and a string descriptor */
const u8 qualifier_desc[] =
{
	0x0a,	/*  0 desc size */
	0x06,	/*  1 desc type (DEVICE_QUALIFIER)*/
	0x00,	/*  2 USB release */
	0x02,	/*  3 => 2.00*/
	0xFF,	/*  4 class */
	0x00,	/*  5 subclass */
	0x00,	/*  6 protocol */
	64,	/*  7 max pack size */
	0x01,	/*  8 number of other-speed configuration */
	0x00,	/*  9 reserved */
};

const u8 config_full[] =
{
	0x09,	/*  0 desc size */
	0x07,	/*  1 desc type (other speed)*/
	0x20,	/*  2 Total length of data returned */
	0x00,	/*  3 */
	0x01,	/*  4 Number of interfaces supported by this speed configuration */
	0x01,	/*  5 value to use to select configuration */
	0x00,	/*  6 index of string desc */
		/*  7 same as configuration desc */
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,
	0x19,	/*  8 same as configuration desc */

};

const u8 config_full_total[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x83 ,0x02 ,0x40 ,0x00 ,0x00,
  0x07 ,0x05 ,0x04 ,0x02 ,0x40 ,0x00 ,0x00
};

const u8 config_high[] =
{
	0x09,	/*  0 desc size */
	0x07,	/*  1 desc type (other speed)*/
	0x20,	/*  2 Total length of data returned */
	0x00,	/*  3 */
	0x01,	/*  4 Number of interfaces supported by this speed configuration */
	0x01,	/*  5 value to use to select configuration */
	0x00,	/*  6 index of string desc */
		/*  7 same as configuration desc */
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,
	0x19,	/*  8 same as configuration desc */

};

const u8 config_high_total[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x81 ,0x02 ,0x00 ,0x02 ,0x00,
  0x07 ,0x05 ,0x02 ,0x02 ,0x00 ,0x02 ,0x00
};

const u8 set_sel[6];

#define CONFIG_DESC_TOTAL_SIZE	\
	(CONFIG_DESC_SIZE+INTERFACE_DESC_SIZE+ENDPOINT_DESC_SIZE*2)
#define CONFIG_SS_DESC_TOTAL_SIZE	\
	(CONFIG_DESC_SIZE+INTERFACE_DESC_SIZE+ENDPOINT_DESC_SIZE*2+ENDPOINT_COMP_DESC_SIZE*2)
#define BOS_DESC_TOTAL_SIZE	\
	(BOS_DESC_SIZE+USB20_EXT_CAP_DESC_SIZE+SUPER_CAP_DESC_SIZE+CONTAIN_CAP_DESC_SIZE)
#define TEST_PKT_SIZE 53

#define USB_CAP_20_EXT  0x2
#define USB_CAP_SS      0x3
#define USB_CAP_CID     0x4

u8 test_pkt [TEST_PKT_SIZE] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/*JKJKJKJK x 9*/
	0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,	/*JJKKJJKK x 8*/
	0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,	/*JJJJKKKK x 8*/
	0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	/*JJJJJJJKKKKKKK x8 - '1'*/
	0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,		/*'1' + JJJJJJJK x 8*/
	0xFC,0x7E,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0x7E	/*{JKKKKKKK x 10},JK*/
};

USBDEV3 oUsbDev3;
USB_GET_STATUS oStatusGet;
USB_INTERFACE_GET oInterfaceGet;
u16 g_usConfig;
u16 g_usUploadPktLength=0;
u8 g_bTransferEp0 = 0;
u8 g_bSuspendResumeDBG_USBD3;
u8 g_bUsingUsbDownAddr = 0;
u32 g_uUsbDownAddr = 0;
u32 *g_uUsbDevCtrlInBufferPtr=NULL;
u32 *g_uUsbDevCtrlOutBufferPtr=NULL;
u32 *g_uUsbDevBulkOutTmpBufferPtr=NULL;
u32 *g_uUsbDevBulkOutBufferPtr=NULL;
u8 bCalled_SetEndPoint = 0;
volatile usbdev3_trb_ptr_t g_pBulkOutTrb0;
volatile usbdev3_trb_ptr_t g_pBulkOutTrbArray_Base;
volatile usbdev3_trb_ptr_t g_pBulkInTrbArray_Base;
u8 *g_ucTempDownBuf;
u32 g_uCntOfDescOutComplete = 0;
u32 g_uCntOfBulkOutTrb, g_uCntOfBulkInTrb;
u8 g_bIsBulkOutXferDone = 0;
u8 g_bIsBulkInXferDone = 0;
unsigned int shikhara_usbd_dn_addr = 0;
unsigned int shikhara_usbd_dn_cnt = 0;
int is_fastboot = 0;
int DNW;
int shikhara_got_header = 0;
int shikhara_receive_done = 0;
extern ulong virt_to_phy_shikhara5210(ulong addr);
static u32 shikhara_usb_malloc(u32 uSize, u32 uAlign);
static void shikhara_usb_free(u32 uAddr);
static void shikhara_usb_fill_trb(usbdev3_trb_ptr_t pTrb, u32 uBufAddr, u32 uLen, u32 uTrbCtrl, u32 uHwo);
static int shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uTrbAddr, u32 uStrmidSofn, u32 *uTri);

static void shikhara_usb_phy_on(void)
{
	#ifdef CONFIG_SHIKHARA_ASIC
	writel(0x1, USB_DEVICE_PHY_CONTROL);		/* Power configuration register for giving POWER to USB PHY from System Controller, not needed for FPGA in case of ASIC 
							   can be implemented */
	#endif
}

static void shikhara_usb_init_phy(void)
{
#ifdef CONFIG_SHIKHARA_ASIC				/* USB PHY related configuration as per the type of PHY implemented */
#if 0
	u32 eClkFreq = EXTREF_24MHz,reg_phytest;
	usbdev3_phy_clkpwr.data = 0;

	switch(eClkFreq){
		case DIFF_100MHz:
			usbdev3_phy_clkpwr.b.fsel = 0x27;
			usbdev3_phy_clkpwr.b.refclksel = 2; 
			usbdev3_phy_clkpwr.b.mpll_multiplier = 0x19;
			usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
			usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x00;
			break;
		case EXTREF_50MHz:
			usbdev3_phy_clkpwr.b.fsel = 0x7;
			usbdev3_phy_clkpwr.b.refclksel = 3; 
			usbdev3_phy_clkpwr.b.mpll_multiplier = 0x32;
			usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
			usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x00;
			break;
		case EXTREF_20MHz:
			usbdev3_phy_clkpwr.b.fsel = 0x4;
			usbdev3_phy_clkpwr.b.refclksel = 3; 
			usbdev3_phy_clkpwr.b.mpll_multiplier = 0x7d;
			usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
			usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x00;
			break;
		case EXTREF_19_2MHz:
			usbdev3_phy_clkpwr.b.fsel = 0x3;
			usbdev3_phy_clkpwr.b.refclksel = 3; 
			usbdev3_phy_clkpwr.b.mpll_multiplier = 0x02;
			usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
			usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x88;
			break;
		case EXTREF_24MHz:
		default:
			usbdev3_phy_clkpwr.b.fsel = 0x5;
			usbdev3_phy_clkpwr.b.refclksel = 3; 
			usbdev3_phy_clkpwr.b.mpll_multiplier = 0x68;
			usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
			usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x88;
			break;
	}

#ifdef CONFIG_CPU_SHIKHARA5250_EVT1
	usbdev3_phy_clkpwr.b.commononn = 1;		
#else
	usbdev3_phy_clkpwr.b.commononn = 1;
#endif
	usbdev3_phy_clkpwr.b.portreset = 1;		
	usbdev3_phy_clkpwr.b.retenablen = 1;		
	usbdev3_phy_clkpwr.b.ref_clkdiv2 = 0;
	usbdev3_phy_clkpwr.b.ref_ssp_en = 1;
	usbdev3_phy_clkpwr.b.ssc_en = 1;
	usbdev3_phy_clkpwr.b.ssc_range = 0;
	usbdev3_phy_clkpwr.b.ssc_ref_clk_sel = 0x0;

	writel(usbdev3_phy_clkpwr.data, SHIKHARA_PHY_CLKPWR);
	usbdev3_phy_clkpwr.b.portreset = 0;
	udelay(10);
	writel(usbdev3_phy_clkpwr.data, SHIKHARA_PHY_CLKPWR);
#endif
#endif
}

static void shikhara_usb_wait_cable_insert(void)
{
	u32 tmp;
	int ucFirst = 1;

	do {
		tmp = readl(rGEVNTCOUNT0);
		if ((tmp || (readl(rDSTS) & 0x7) == 0x0)) {
			printf("USB cable Connected![0x%x]\n", readl(rDSTS) & 0x7);
			break;
		} else if(ucFirst) {
			printf("Insert a USB cable into the connector!\n");
			ucFirst = 0;
		}
		udelay(10);
	} while (1);
}

/* Allocating 32byte aligned memory */
static u32 shikhara_usb_malloc(u32 uSize, u32 uAlign)
{
	u32 uAddr, uTemp1, uTemp2, uTemp3;
	u32 uAddr_aligned;
	u32 uAllocHwMem_AlignConstraint = 32;

	uTemp2 = uAlign;
	uTemp3 = uAllocHwMem_AlignConstraint;
	while(uTemp3)
	{
		uTemp1 = uTemp2%uTemp3;
		uTemp2 = uTemp3;
		uTemp3 = uTemp1;
	}
		
	uAlign = uAlign*uAllocHwMem_AlignConstraint/uTemp2;
	uAddr = (u32)malloc(uSize + uAlign);
	uTemp2 = uAddr % uAlign;

	uAddr_aligned = uAddr + (uAlign - uTemp2);
	*(u32 *)(uAddr_aligned-4) = uAddr;

	memset((void *)uAddr, 0, uSize);

	DBG_USBD3("shikhara_usb_malloc:Addr=0x%08x[0x%08x], Size=%d, Align=%d\n\n", uAddr, uAddr_aligned, uSize, uAlign);

	return uAddr_aligned;
}

static void shikhara_usb_free(u32 uAddr)
{
	u32 uFree_addr;
	if (uAddr != 0)
	{
		uFree_addr = *(u32 *)(uAddr - 4);
		free((u8 *)uFree_addr);
	}
	DBG_USBD3("\n\nshikhara_usb_free:0x%08x[0x%8x]\n\n", uAddr, uFree_addr);
}

/* Dumping the setup packet */
static void shikhara_usb_print_ep0_pkt(u8 *pt, u8 count)
{
	int i;
	DBG_USBD3("[DBG:");
	for(i=0;i<count;i++)
		DBG_USBD3("%x,", pt[i]);
	DBG_USBD3("]\n");
}

/* Preparing the device,configuration,endpoint,string descriptors */
static void shikhara_usb_set_descriptor_tlb(void)
{
	oUsbDev3.m_oDesc.oDescDevice.bLength=DEVICE_DESC_SIZE;
	oUsbDev3.m_oDesc.oDescDevice.bDescriptorType=DEVICE_DESCRIPTOR;
	oUsbDev3.m_oDesc.oDescDevice.bDeviceClass=0xFF;
	oUsbDev3.m_oDesc.oDescDevice.bDeviceSubClass=0x0;
	oUsbDev3.m_oDesc.oDescDevice.bDeviceProtocol=0x0;
	if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER) {
		oUsbDev3.m_oDesc.oDescDevice.bMaxPacketSize0=SUPER_SPEED_CONTROL_PKT_EXP_SZ;
	} else {
		oUsbDev3.m_oDesc.oDescDevice.bMaxPacketSize0=oUsbDev3.m_uControlEPMaxPktSize;
	}
	oUsbDev3.m_oDesc.oDescDevice.idVendorL=0xE8;
	oUsbDev3.m_oDesc.oDescDevice.idVendorH=0x04;
	oUsbDev3.m_oDesc.oDescDevice.idProductL=0x34;
	oUsbDev3.m_oDesc.oDescDevice.idProductH=0x12;
	oUsbDev3.m_oDesc.oDescDevice.bcdDeviceL=0x00;
	oUsbDev3.m_oDesc.oDescDevice.bcdDeviceH=0x01;
	oUsbDev3.m_oDesc.oDescDevice.iManufacturer=0x1; // index of string descriptor
	oUsbDev3.m_oDesc.oDescDevice.iProduct=0x2;	// index of string descriptor
	oUsbDev3.m_oDesc.oDescDevice.iSerialNumber=0x0;
	oUsbDev3.m_oDesc.oDescDevice.bNumConfigurations=0x1;

	if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER) {
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBL=0x00;
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBH=0x03; 	// Ver 3.0
	} else if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_HIGH) {
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBL=0x00;
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBH=0x02; 	// Ver 2.0
	} else if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_FULL) {
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBL=0x10;
		oUsbDev3.m_oDesc.oDescDevice.bcdUSBH=0x01; 	// Ver 2.0
	}

	oUsbDev3.m_oDesc.oDescConfig.bLength=CONFIG_DESC_SIZE; // 0x9 bytes
	oUsbDev3.m_oDesc.oDescConfig.bDescriptorType=CONFIGURATION_DESCRIPTOR;
	if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER)
		oUsbDev3.m_oDesc.oDescConfig.wTotalLengthL=CONFIG_SS_DESC_TOTAL_SIZE;
	else
		oUsbDev3.m_oDesc.oDescConfig.wTotalLengthL=CONFIG_DESC_TOTAL_SIZE;
	oUsbDev3.m_oDesc.oDescConfig.wTotalLengthH=0;
	oUsbDev3.m_oDesc.oDescConfig.bNumInterfaces=1;
	oUsbDev3.m_oDesc.oDescConfig.bConfigurationValue=1;
	oUsbDev3.m_oDesc.oDescConfig.iConfiguration=0;
	oUsbDev3.m_oDesc.oDescConfig.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED; // bus powered only.
	oUsbDev3.m_oDesc.oDescConfig.maxPower=25; // draws 50mA current from the USB bus.

	oUsbDev3.m_oDesc.oDescInterface.bLength=INTERFACE_DESC_SIZE; // 9
	oUsbDev3.m_oDesc.oDescInterface.bDescriptorType=INTERFACE_DESCRIPTOR;
	oUsbDev3.m_oDesc.oDescInterface.bInterfaceNumber=0x0;
	oUsbDev3.m_oDesc.oDescInterface.bAlternateSetting=0x0; // ?
	oUsbDev3.m_oDesc.oDescInterface.bNumEndpoints = 2;	// # of endpoints except EP0
	oUsbDev3.m_oDesc.oDescInterface.bInterfaceClass=0xff; // 0x0 ?
	oUsbDev3.m_oDesc.oDescInterface.bInterfaceSubClass=0x0;
	oUsbDev3.m_oDesc.oDescInterface.bInterfaceProtocol=0x0;
	oUsbDev3.m_oDesc.oDescInterface.iInterface=0x0;

	if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER) {
		oUsbDev3.m_oSSDesc.oDescEp0.bLength=ENDPOINT_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescEp0.bDescriptorType=ENDPOINT_DESCRIPTOR;
		oUsbDev3.m_oSSDesc.oDescEp0.bEndpointAddress=BULK_IN_EP|EP_ADDR_IN;
		oUsbDev3.m_oSSDesc.oDescEp0.bmAttributes=EP_ATTR_BULK;
		oUsbDev3.m_oSSDesc.oDescEp0.wMaxPacketSizeL=(u8)oUsbDev3.m_uBulkEPMaxPktSize; /* 64*/
		oUsbDev3.m_oSSDesc.oDescEp0.wMaxPacketSizeH=(u8)(oUsbDev3.m_uBulkEPMaxPktSize>>8);
		oUsbDev3.m_oSSDesc.oDescEp0.bInterval=0x0; /* not used */

		oUsbDev3.m_oSSDesc.oDescEp0Comp.bLength=6;
		oUsbDev3.m_oSSDesc.oDescEp0Comp.bDescriptorType=0x30;
		oUsbDev3.m_oSSDesc.oDescEp0Comp.bMaxBurst=15;
		oUsbDev3.m_oSSDesc.oDescEp0Comp.bmAttributes=0;
		oUsbDev3.m_oSSDesc.oDescEp0Comp.wBytesPerInterval=0;

		/* Standard endpoint1 descriptor */
		oUsbDev3.m_oSSDesc.oDescEp1.bLength=ENDPOINT_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescEp1.bDescriptorType=ENDPOINT_DESCRIPTOR;
		oUsbDev3.m_oSSDesc.oDescEp1.bEndpointAddress=BULK_OUT_EP|EP_ADDR_OUT;
		oUsbDev3.m_oSSDesc.oDescEp1.bmAttributes=EP_ATTR_BULK;
		oUsbDev3.m_oSSDesc.oDescEp1.wMaxPacketSizeL=(u8)oUsbDev3.m_uBulkEPMaxPktSize; /* 64*/
		oUsbDev3.m_oSSDesc.oDescEp1.wMaxPacketSizeH=(u8)(oUsbDev3.m_uBulkEPMaxPktSize>>8);
		oUsbDev3.m_oSSDesc.oDescEp1.bInterval=0x0; /* not used */

		oUsbDev3.m_oSSDesc.oDescEp1Comp.bLength=6;
		oUsbDev3.m_oSSDesc.oDescEp1Comp.bDescriptorType=0x30;
		oUsbDev3.m_oSSDesc.oDescEp1Comp.bMaxBurst=15;
		oUsbDev3.m_oSSDesc.oDescEp1Comp.bmAttributes=0;
		oUsbDev3.m_oSSDesc.oDescEp1Comp.wBytesPerInterval=0;

		// Standard BOS(Binary Object Store) descriptor
		oUsbDev3.m_oSSDesc.oDescBos.bLength = BOS_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescBos.bDescriptorType = 0x0F;
		oUsbDev3.m_oSSDesc.oDescBos.wTotalLength = BOS_DESC_TOTAL_SIZE;
		oUsbDev3.m_oSSDesc.oDescBos.bNumDeviceCaps = 3;

		oUsbDev3.m_oSSDesc.oDescUsb20Ext.bLength = USB20_EXT_CAP_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescUsb20Ext.bDescriptorType = 0x10;
		oUsbDev3.m_oSSDesc.oDescUsb20Ext.bDevCapabilityType = USB_CAP_20_EXT;
		oUsbDev3.m_oSSDesc.oDescUsb20Ext.bmAttributes = 0x2;

		oUsbDev3.m_oSSDesc.oDescSuperCap.bLength = SUPER_CAP_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescSuperCap.bDescriptorType = 0x10;
		oUsbDev3.m_oSSDesc.oDescSuperCap.bDevCapabilityType = USB_CAP_SS;
		oUsbDev3.m_oSSDesc.oDescSuperCap.bmAttributes = 0x0;
		oUsbDev3.m_oSSDesc.oDescSuperCap.wSpeedsSupported = 0xC;
		oUsbDev3.m_oSSDesc.oDescSuperCap.bFunctionalitySupport = 2;
		/* TODO: set correct value */
		oUsbDev3.m_oSSDesc.oDescSuperCap.bU1DevExitLat = 0x4;
		oUsbDev3.m_oSSDesc.oDescSuperCap.wU2DevExitLat = 0x4;

		oUsbDev3.m_oSSDesc.oDescContainCap.bLength = CONTAIN_CAP_DESC_SIZE;
		oUsbDev3.m_oSSDesc.oDescContainCap.bDescriptorType = 0x10;
		oUsbDev3.m_oSSDesc.oDescContainCap.bDevCapabilityType = USB_CAP_CID;
		oUsbDev3.m_oSSDesc.oDescContainCap.bReserved = 0x0;
		memset(oUsbDev3.m_oSSDesc.oDescContainCap.containerID, 0x0, 16);
	}
	else {
		/* Standard endpoint0 descriptor */
		oUsbDev3.m_oDesc.oDescEp0.bLength=ENDPOINT_DESC_SIZE;
		oUsbDev3.m_oDesc.oDescEp0.bDescriptorType=ENDPOINT_DESCRIPTOR;
		oUsbDev3.m_oDesc.oDescEp0.bEndpointAddress=BULK_IN_EP|EP_ADDR_IN;
		oUsbDev3.m_oDesc.oDescEp0.bmAttributes=EP_ATTR_BULK;
		oUsbDev3.m_oDesc.oDescEp0.wMaxPacketSizeL=(u8)oUsbDev3.m_uBulkEPMaxPktSize; /* 64*/
		oUsbDev3.m_oDesc.oDescEp0.wMaxPacketSizeH=(u8)(oUsbDev3.m_uBulkEPMaxPktSize>>8);
		oUsbDev3.m_oDesc.oDescEp0.bInterval=0x0; /* not used */

		/* Standard endpoint1 descriptor */
		oUsbDev3.m_oDesc.oDescEp1.bLength=ENDPOINT_DESC_SIZE;
		oUsbDev3.m_oDesc.oDescEp1.bDescriptorType=ENDPOINT_DESCRIPTOR;
		oUsbDev3.m_oDesc.oDescEp1.bEndpointAddress=BULK_OUT_EP|EP_ADDR_OUT;
		oUsbDev3.m_oDesc.oDescEp1.bmAttributes=EP_ATTR_BULK;
		oUsbDev3.m_oDesc.oDescEp1.wMaxPacketSizeL=(u8)oUsbDev3.m_uBulkEPMaxPktSize; /* 64*/
		oUsbDev3.m_oDesc.oDescEp1.wMaxPacketSizeH=(u8)(oUsbDev3.m_uBulkEPMaxPktSize>>8);
		oUsbDev3.m_oDesc.oDescEp1.bInterval=0x0; /* not used */
	}
}

static void shikhara_usb_softreset_phy(u8 ucSet)
{
	usbdev3_gusb2phycfg_t usbdev3_gusb2phycfg;
	usbdev3_gusb3pipectl_t usbdev3_gusb3pipectl;

	usbdev3_gusb2phycfg.data = readl(rGUSB2PHYCFG);
	usbdev3_gusb2phycfg.b.phy_soft_reset = ucSet;
	writel(usbdev3_gusb2phycfg.data, rGUSB2PHYCFG);

	usbdev3_gusb3pipectl.data = readl(rGUSB3PIPECTL);
	usbdev3_gusb3pipectl.b.phy_soft_reset = ucSet;
	writel(usbdev3_gusb3pipectl.data, rGUSB3PIPECTL);
}

static void shikhara_usb_softreset_core(void)
{
	DBG_USBD3("Resetting the Device Core\n");
	usbdev3_dctl_t usbdev3_dctl;

	usbdev3_dctl.data = readl(rDCTL);
	usbdev3_dctl.b.core_soft_reset = 1;
	usbdev3_dctl.b.run_stop = 0;
	writel(usbdev3_dctl.data, rDCTL);

	do
	{
		udelay(10);
		usbdev3_dctl.data = readl(rDCTL);
	} while (usbdev3_dctl.b.core_soft_reset == 1);
	
	udelay(10);
	DBG_USBD3("Resetting the Device Core Completed\n");
}

static void shikhara_usb_enable_eventinterrupt(void)
{
	usbdev3_gevntsiz_t usbdev3_gevntsiz;

	usbdev3_gevntsiz.data = readl(rGEVNTSIZ0);
	usbdev3_gevntsiz.b.event_int_mask = 0;
	writel(usbdev3_gevntsiz.data, rGEVNTSIZ0);
}

static void shikhara_usb_disable_eventinterrupt(void)
{
	usbdev3_gevntsiz_t usbdev3_gevntsiz;

	usbdev3_gevntsiz.data = readl(rGEVNTSIZ0);
	usbdev3_gevntsiz.b.event_int_mask = 1;
	writel(usbdev3_gevntsiz.data, rGEVNTSIZ0);
}

static void shikhara_usb_flush_eventbuffer(void)
{
	u32 uEventCount;

	uEventCount = readl(rGEVNTCOUNT0) & 0xffff;

	writel(uEventCount, rGEVNTCOUNT0);
}

static int shikhara_usb_wait_ep_cmd_complete(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uSec)
{
	usbdev3_depcmd_t usbdev3_depcmd;
	u32 uEpCmdAddr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMD(ucEpNum) : rDOEPCMD(ucEpNum);

	do {
		usbdev3_depcmd.data = readl(uEpCmdAddr);
		DBG_USBD3v("D%cEPCMD(%d)=0x%08x\n",(eEpDir==USBDEV3_EP_DIR_IN)? 'I' : 'O', ucEpNum,readl(uEpCmdAddr));		

		if (!(usbdev3_depcmd.b.cmd_active))
		{
			DBG_USBD3("Complete: D%cEPCMD(%d)=0x%08x\n",
				(eEpDir==USBDEV3_EP_DIR_IN)? 'I' : 'O', ucEpNum, usbdev3_depcmd.data);
			return 1;
		}
	
		udelay(1);
		uSec--;
	} while (uSec > 0);

	DBG_USBD3("TimeOver: D%cEPCMD(%d)=0x%08x\n",
		(eEpDir==USBDEV3_EP_DIR_IN)? 'I' : 'O', ucEpNum, usbdev3_depcmd.data);
	return 0;
}

/* Issue Set Endpoint Configuration command */
static int shikhara_usb_set_ep_cfg(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uPar0, u32 uPar1)
{
	DBG_USBD3v("DEPCFG : exynos_usb_set_ep_cfg %s\n",(eEpDir==USBDEV3_EP_DIR_IN)? "IN" : "OUT");
	usbdev3_depcmd_t usbdev3_depcmd;
	u32 uEpCmdAddr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMD(ucEpNum) : rDOEPCMD(ucEpNum);
	u32 uEpPar0Addr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMDPAR0(ucEpNum) : rDOEPCMDPAR0(ucEpNum);
	u32 uEpPar1Addr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMDPAR1(ucEpNum) : rDOEPCMDPAR1(ucEpNum);


	writel(uPar1, uEpPar1Addr);
	writel(uPar0, uEpPar0Addr);
	DBG_USBD3v("DIEPCMDPAR0 = 0x%x\n",readl(rDIEPCMDPAR0(ucEpNum)));
	DBG_USBD3v("DIEPCMDPAR1 = 0x%x\n",readl(rDIEPCMDPAR1(ucEpNum)));


	usbdev3_depcmd.data = 0;
	usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_SET_EP_CFG;
	usbdev3_depcmd.b.cmd_active = 1;
	writel(usbdev3_depcmd.data, uEpCmdAddr);
	if (!shikhara_usb_wait_ep_cmd_complete(eEpDir, ucEpNum, 500/*CMDCOMPLETEWAIT_UNIT*/))
	{
		return 0;
	}

	return 1;
}

/* Issue Set Transfer Resource configuration command */
static int shikhara_usb_set_ep_xfer_rsrc_cfg(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uPar0)
{
	DBG_USBD3v("DEPXFERCFG : exynos_usb_set_ep_xfer_rsrc_cfg\n",(eEpDir==USBDEV3_EP_DIR_IN)? "IN" : "OUT");
	usbdev3_depcmd_t usbdev3_depcmd;
	u32 uEpCmdAddr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMD(ucEpNum) : rDOEPCMD(ucEpNum);
	u32 uEpPar0Addr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMDPAR0(ucEpNum) : rDOEPCMDPAR0(ucEpNum);

	writel(uPar0, uEpPar0Addr);

	usbdev3_depcmd.data = 0;
	usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_SET_EP_XFER_RSRC_CFG;
	usbdev3_depcmd.b.cmd_active = 1;
	writel(usbdev3_depcmd.data, uEpCmdAddr);

	if (!shikhara_usb_wait_ep_cmd_complete(eEpDir, ucEpNum, CMDCOMPLETEWAIT_UNIT))
	{
		return 0;
	}

	return 1;
}

static void shikhara_usb_enable_ep(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum)
{
	u32 uEpEnaIdx;

	uEpEnaIdx = ucEpNum*2;

	if (eEpDir == USBDEV3_EP_DIR_IN)
	{
		uEpEnaIdx++;
	}

	writel(readl(rDALEPENA) | (1<<uEpEnaIdx), rDALEPENA);
	DBG_USBD3v("DALEPENA = 0x%x\n",readl(rDALEPENA));
}

static int shikhara_usb_activate_ep0(void)
{
	usbdev3_depcmd_t usbdev3_depcmd;
	usbdev3_depcmdpar0_set_ep_cfg_t usbdev3_depcmdpar0_set_ep_cfg;
	usbdev3_depcmdpar1_set_ep_cfg_t usbdev3_depcmdpar1_set_ep_cfg;
	usbdev3_depcmd.data = 0;
	usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_START_NEW_CFG;
	usbdev3_depcmd.b.cmd_active = 1;
	DBG_USBD3v("DEPSTARTCFG\n");
	
	writel(usbdev3_depcmd.data, rDOEPCMD(0));	// Issuing Start New Configuration command to EP0

	if (!shikhara_usb_wait_ep_cmd_complete(USBDEV3_EP_DIR_OUT, 0, CMDCOMPLETEWAIT_UNIT))
	{
		return 0;
	}

	usbdev3_depcmdpar0_set_ep_cfg.data = 0;
	usbdev3_depcmdpar0_set_ep_cfg.b.ep_type = USBDEV3_EP_CTRL;
	usbdev3_depcmdpar0_set_ep_cfg.b.mps = oUsbDev3.m_uControlEPMaxPktSize;		// Filling the PAR0 for Set Endpt configuration command

	usbdev3_depcmdpar1_set_ep_cfg.data = 0;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_cmpl_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_nrdy_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_dir = USBDEV3_EP_DIR_OUT;			// Filling the PAR1 for Set Endpt configuration command
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_num = 0;

	/* Issuing SET ENDPOINT CONFIGURATION command to EP0 OUT */
	if (!shikhara_usb_set_ep_cfg(USBDEV3_EP_DIR_OUT, 0, usbdev3_depcmdpar0_set_ep_cfg.data, usbdev3_depcmdpar1_set_ep_cfg.data))
	{
		return 0;
	}

	/* Issuing SET ENDPOINT TRANSFER RESOURCE CONFIGURATION command to EP0 OUT */
	if (!shikhara_usb_set_ep_xfer_rsrc_cfg(USBDEV3_EP_DIR_OUT, 0, 1))
	{
		return 0;
	}

	usbdev3_depcmdpar0_set_ep_cfg.data = 0;
	usbdev3_depcmdpar0_set_ep_cfg.b.ep_type = USBDEV3_EP_CTRL;
	usbdev3_depcmdpar0_set_ep_cfg.b.mps = oUsbDev3.m_uControlEPMaxPktSize;
	usbdev3_depcmdpar1_set_ep_cfg.data = 0;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_cmpl_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_nrdy_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_dir = USBDEV3_EP_DIR_IN;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_num = 0;

	/* Issuing SET ENDPOINT CONFIGURATION command to EP0 IN */
	if (!shikhara_usb_set_ep_cfg(USBDEV3_EP_DIR_IN, 0, usbdev3_depcmdpar0_set_ep_cfg.data, usbdev3_depcmdpar1_set_ep_cfg.data))
	{
		return 0;
	}
	/* Issuing SET ENDPOINT TRANSFER RESOURCE CONFIGURATION command to EP0 IN */
	if (!shikhara_usb_set_ep_xfer_rsrc_cfg(USBDEV3_EP_DIR_IN, 0, 1))
	{
		return 0;
	}

	return 1;
}

static int shikhara_usb_activate_ep(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum)
{
	usbdev3_depcmd_t usbdev3_depcmd;
	usbdev3_depcmdpar0_set_ep_cfg_t usbdev3_depcmdpar0_set_ep_cfg;
	usbdev3_depcmdpar1_set_ep_cfg_t usbdev3_depcmdpar1_set_ep_cfg;

	if (oUsbDev3.m_bEPs_Enabled == 0)
	{
		oUsbDev3.m_bEPs_Enabled = 1;
		usbdev3_depcmd.data = 0;
		usbdev3_depcmd.b.param = 2;
		usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_START_NEW_CFG;
		usbdev3_depcmd.b.cmd_active = 1;
		writel(usbdev3_depcmd.data, rDOEPCMD(0));
		if (!shikhara_usb_wait_ep_cmd_complete(USBDEV3_EP_DIR_OUT, 0, CMDCOMPLETEWAIT_UNIT))
		{
			return 0;
		}
	}

	usbdev3_depcmdpar0_set_ep_cfg.data = 0;
	usbdev3_depcmdpar0_set_ep_cfg.b.ep_type = USBDEV3_EP_BULK;
	usbdev3_depcmdpar0_set_ep_cfg.b.mps = oUsbDev3.m_uBulkEPMaxPktSize;

	if (eEpDir == USBDEV3_EP_DIR_IN)
		usbdev3_depcmdpar0_set_ep_cfg.b.fifo_num = ucEpNum;

	usbdev3_depcmdpar0_set_ep_cfg.b.brst_siz = 0;
	usbdev3_depcmdpar1_set_ep_cfg.data = 0;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_cmpl_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_dir = (u32)eEpDir;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_num = ucEpNum;
	
	/* Issuing SET ENDPOINT CONFIGURATION command to BULK EP */
	if (!shikhara_usb_set_ep_cfg(eEpDir, ucEpNum, usbdev3_depcmdpar0_set_ep_cfg.data, usbdev3_depcmdpar1_set_ep_cfg.data))
		return 0;

	/* Issuing SET ENDPOINT TRANSFER RESOURCE CONFIGURATION command to BULK EP */
	if (!shikhara_usb_set_ep_xfer_rsrc_cfg(eEpDir, ucEpNum, 1))
		return 0;


	return 1;
}

static void shikhara_usb_runstop_device(u8 ucSet)
{
	u32 drd_reset,drd_clear;
	usbdev3_dctl_t usbdev3_dctl;

	usbdev3_dctl.data = readl(rDCTL);
	usbdev3_dctl.b.run_stop = ucSet;	
	usbdev3_dctl.b.keep_connect = 0;	
	writel(usbdev3_dctl.data, rDCTL);
	DBG_USBD3v("DCTL = 0x%x\n",readl(rDCTL));

	if(!ucSet){
#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
	/* Assert System Controller level DRD Global reset */
	drd_reset =  readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4); 	/* Reading Soft reset register 4 of system controller */
	drd_reset |= DRD_RESET;
	writel(drd_reset, (SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4)); 	/* This will reset DRD controller */
	DBG_USBD3v("drd_reset = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
#endif

	mdelay(1000);
#if defined(CONFIG_SHIKHARA_FPGA) || defined(CONFIG_SHIKHARA_ASIC)
	/* Clear System Controller level DRD Global reset */
	drd_clear = readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4); 
	drd_clear &= ~(DRD_CLEAR);
	writel(drd_clear, (SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));	/* This will clear the reset bit of DRD controller on system level */
	DBG_USBD3v("drd_clear = 0x%x\n",readl(SHIKHARA_SYS_CTRL_BASEADDR + SOFT_RESET_REG4));
#endif
	}
}

/* Issuing START CONFIGURATION command to BULK EP */
static int shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uTrbAddr, u32 uStrmidSofn, u32 *uTri)
{
	DBG_USBD3v("DEPSTRTXFER: exynos_usb_start_ep_xfer\n");
	usbdev3_depcmd_t usbdev3_depcmd;
	u32 uEpCmdAddr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMD(ucEpNum) : rDOEPCMD(ucEpNum);
	u32 uEpPar0Addr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMDPAR0(ucEpNum) : rDOEPCMDPAR0(ucEpNum);
	u32 uEpPar1Addr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMDPAR1(ucEpNum) : rDOEPCMDPAR1(ucEpNum);


	writel(virt_to_phys((void*)uTrbAddr), uEpPar1Addr);
	writel(0, uEpPar0Addr);
	DBG_USBD3v("DIEPCMDPAR0 = 0x%x\n",readl(rDIEPCMDPAR0(ucEpNum)));
        DBG_USBD3v("DIEPCMDPAR1 = 0x%x\n",readl(rDIEPCMDPAR1(ucEpNum)));

	usbdev3_depcmd.data = 0;
	usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_START_XFER;
	usbdev3_depcmd.b.cmd_active = 1;
	usbdev3_depcmd.b.param = uStrmidSofn;
	writel(usbdev3_depcmd.data, uEpCmdAddr);

	if (!shikhara_usb_wait_ep_cmd_complete(eEpDir, ucEpNum, CMDCOMPLETEWAIT_UNIT))
		return 0;

	usbdev3_depcmd.data = readl(uEpCmdAddr);
	*uTri = usbdev3_depcmd.b.param & 0x7f;
	DBG_USBD3v("Transfer Resource Index = %d\n",*uTri);
	return 1;
}

static void shikhara_usb_fill_trb(usbdev3_trb_ptr_t pTrb, u32 uBufAddr, u32 uLen, u32 uTrbCtrl, u32 uHwo)
{
	pTrb->buf_ptr_l = virt_to_phys((void *)uBufAddr);
	pTrb->buf_ptr_h = 0;
	pTrb->status.data = 0;
	pTrb->status.b.buf_siz = uLen;
	pTrb->control.data = uTrbCtrl & 0xfffffffeU;

	if (uHwo)
		pTrb->control.b.hwo = 1;

	drd_flush_cache(pTrb,sizeof(usbdev3_trb_t));		// Flush the TRB structure so as it gets written on system memory
	DBG_USBD3v("Setup TRB Content\n");
        DBG_USBD3v("\tTRB Buffer Pointer Low: 0x%x\n",pTrb->buf_ptr_l);
        DBG_USBD3v("\tTRB Buffer Size: 0x%x\n",pTrb->status.b.buf_siz);
        DBG_USBD3v("\tTRB Control data: 0x%x\n",pTrb->control.data);
}

static int shikhara_usb_start_ep0_setup_rx(void)
{
	DBG_USBD3v("Configure EP0 for setup packets\n");
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;
	usbdev3_trb_ctrl.data = 0;
	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_SETUP;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 1;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	/* Fill TRB structure pointing to setup packet buffer */
	shikhara_usb_fill_trb(oUsbDev3.m_oSetupTrbPtr, (u32)oUsbDev3.m_oDeviceRequest,
		oUsbDev3.m_uControlEPMaxPktSize, usbdev3_trb_ctrl.data, 1);

	/* Issue START TRANSFER command */	
	if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_OUT, 0, (u32)oUsbDev3.m_oSetupTrbPtr, 0, &oUsbDev3.m_uTriOut[0]))
		return 0;

	return 1;
}

/* Initialising the Control EP0 for control transfer */
static int shikhara_usb_init_core(USBDEV3_SPEED_e eSpeed)
{
	u32 uData,reg_dctl;
	

	usbdev3_gusb2phycfg_t usbdev3_gusb2phycfg;
	usbdev3_gusb3pipectl_t usbdev3_gusb3pipectl;
	usbdev3_gevntsiz_t usbdev3_gevntsiz;
	usbdev3_dcfg_t usbdev3_dcfg;
	usbdev3_devten_t usbdev3_devten;

	uData = readl(rGSNPSID);
	printf("IP version is %1x.%03x\n", (uData&0xf000)>>12, (uData&0x0fff));
	writel(0, rGEVNTADR_HI0);
	oUsbDev3.m_CurrentEventPosition = 0;
	writel(virt_to_phys((void *)oUsbDev3.m_pEventBuffer), rGEVNTADR_LO0);
	drd_inval_cache((uint32_t)oUsbDev3.m_pEventBuffer,512);
	usbdev3_gevntsiz.data = 0;
	usbdev3_gevntsiz.b.event_siz = 4*USBDEV3_EVENT_BUFFER_COUNT;
	usbdev3_gevntsiz.b.event_int_mask = 0;
	writel(usbdev3_gevntsiz.data, rGEVNTSIZ0);		// Filling the Event registers with start address of event buffer, size of buffer
	writel(0, rGEVNTCOUNT0);
	DBG_USBD3("GEVNTADR_LO0 = 0x%x\n",readl(rGEVNTADR_LO0));
        DBG_USBD3("GEVNTSIZ0 = 0x%x\n",readl(rGEVNTSIZ0));

	usbdev3_dcfg.data = 0;
	usbdev3_dcfg.b.dev_speed = (u32) eSpeed;
	usbdev3_dcfg.b.dev_addr = 0;
	usbdev3_dcfg.b.intr_num = 0;
	usbdev3_dcfg.b.lpm_cap = 1;
	writel(usbdev3_dcfg.data, rDCFG);
	DBG_USBD3("DCFG = 0x%x\n",readl(rDCFG));		// Keeping the device in superspeed 

	shikhara_usb_disable_eventinterrupt();
	shikhara_usb_flush_eventbuffer();
	shikhara_usb_enable_eventinterrupt();

	usbdev3_devten.data = 0;
	usbdev3_devten.b.disconn_evt_en = 1;
	usbdev3_devten.b.usb_reset_en = 1;
	usbdev3_devten.b.conn_done_en = 1;
	usbdev3_devten.b.usb_lnk_sts_chng_en = 0;
	usbdev3_devten.b.wake_up_en = 1;
	usbdev3_devten.b.errtic_err_en = 1;
	usbdev3_devten.b.cmd_cmplt_en = 1;
	usbdev3_devten.b.evnt_overflow_en = 1;
	writel(usbdev3_devten.data, rDEVTEN);
	DBG_USBD3("DEVTEN = 0x%x\n",readl(rDEVTEN));		// Enabling all the events interested to receive

	reg_dctl = readl(rDCTL);
	reg_dctl &= ~(DWC3_DCTL_HIRD_THRES_MASK | DWC3_DCTL_L1_HIBER_EN);
	reg_dctl  |= DWC3_DCTL_HIRD_THRES(28);
	writel(reg_dctl,rDCTL);
	DBG_USBD3("DCTL = 0x%x\n",readl(rDCTL));

	usbdev3_gusb2phycfg.data = readl(rGUSB2PHYCFG);
	usbdev3_gusb2phycfg.b.suspend_usb2_phy = 0;
	writel(usbdev3_gusb2phycfg.data, rGUSB2PHYCFG);// . to activate EP0
	usbdev3_gusb3pipectl.data = 0x00240002;	// clear suspend bit
	writel(usbdev3_gusb3pipectl.data, rGUSB3PIPECTL);
	
	if(!shikhara_usb_activate_ep0()){
		DBG_USBD3("Activate ep0 Fail\n");
	}

	/* Issue START TRANSFER for the EP0 */	
	if (!shikhara_usb_start_ep0_setup_rx())
	{
		DBG_USBD3("Start Ep0 Setup Rx Fail\n");
		return -1; 
	}

	/* Enable EPO IN and OUT */		
	shikhara_usb_enable_ep(USBDEV3_EP_DIR_OUT, 0);
	shikhara_usb_enable_ep(USBDEV3_EP_DIR_IN, 0);
	
	return 0;
}

static void shikhara_usb_handle_disconnect_int(void)
{
	oUsbDev3.m_eUsbDev3State = USBDEV3_STATE_DEFAULT;
}

static int shikhara_usb_end_ep_xfer(USBDEV3_EP_DIR_e eEpDir, u8 ucEpNum, u32 uTri)
{
	usbdev3_depcmd_t usbdev3_depcmd;
	u32 uEpCmdAddr = (eEpDir==USBDEV3_EP_DIR_IN)? rDIEPCMD(ucEpNum) : rDOEPCMD(ucEpNum);

	usbdev3_depcmd.data = 0;
	usbdev3_depcmd.b.cmd_type = DEPCMD_CMD_END_XFER;
	usbdev3_depcmd.b.cmd_active = 1;
	usbdev3_depcmd.b.param = uTri;
	writel(usbdev3_depcmd.data, uEpCmdAddr);

	if (!shikhara_usb_wait_ep_cmd_complete(eEpDir, ucEpNum, (10*CMDCOMPLETEWAIT_UNIT)))	
		return 0;

	return 1;
}

/* Reset Event Handler */
static void shikhara_usb_handle_reset_int(void)
{
	u32 uEpNum;
	usbdev3_dcfg_t usbdev3_dcfg;
	usbdev3_dsts_t usbdev3_dsts;
	usbdev3_dgcmd_t usbdev3_dgcmd;
	int reg_dctl;
	oUsbDev3.m_eUsbDev3State = USBDEV3_STATE_DEFAULT;

	reg_dctl = readl(rDCTL);
        reg_dctl &= ~(0xf << 1);
        writel(reg_dctl,rDCTL);

	for (uEpNum = 1; uEpNum < TOTL_EP_COUNT; uEpNum++)
	{
		if (oUsbDev3.m_uTriIn[uEpNum])
		{
			shikhara_usb_end_ep_xfer(USBDEV3_EP_DIR_IN, uEpNum, oUsbDev3.m_uTriIn[uEpNum]);
			oUsbDev3.m_uTriIn[uEpNum] = 0;
		}
	}

	for (uEpNum = 1; uEpNum < TOTL_EP_COUNT; uEpNum++)
	{
		if (oUsbDev3.m_uTriOut[uEpNum])
		{
			shikhara_usb_end_ep_xfer(USBDEV3_EP_DIR_OUT, uEpNum, oUsbDev3.m_uTriOut[uEpNum]);
			oUsbDev3.m_uTriOut[uEpNum] = 0;
		}
	}

	usbdev3_dgcmd.data= 0;
	usbdev3_dgcmd.b.cmd_type = DGCMD_CMD_ALL_FIFO_FLUSH;
	usbdev3_dgcmd.b.cmd_active = 1;
	writel(usbdev3_dgcmd.data, rDGCMD);		// Flushing FIFO

	do
	{
		udelay(1);
		usbdev3_dgcmd.data = readl(rDGCMD);
	}while(usbdev3_dgcmd.b.cmd_active);

	do
	{
		udelay(1);
		usbdev3_dsts.data = readl(rDSTS);
	}while(!(usbdev3_dsts.b.rx_fifo_empty));

	usbdev3_dcfg.data = readl(rDCFG);
	usbdev3_dcfg.b.dev_addr = 0;
	writel(usbdev3_dcfg.data, rDCFG);

}

static void shikhara_usb_set_maxpktsizes(USBDEV3_SPEED_e eSpeed)
{
	oUsbDev3.m_eSpeed = eSpeed;

	if (eSpeed == USBDEV3_SPEED_SUPER)
	{
		oUsbDev3.m_uControlEPMaxPktSize = SUPER_SPEED_CONTROL_PKT_SIZE;
		oUsbDev3.m_uBulkEPMaxPktSize = SUPER_SPEED_BULK_PKT_SIZE;
	}
	else if (eSpeed == USBDEV3_SPEED_HIGH)
	{
		oUsbDev3.m_uControlEPMaxPktSize = HIGH_SPEED_CONTROL_PKT_SIZE;
		oUsbDev3.m_uBulkEPMaxPktSize = HIGH_SPEED_BULK_PKT_SIZE;
	}
	else
	{
		oUsbDev3.m_uControlEPMaxPktSize = FULL_SPEED_CONTROL_PKT_SIZE;
		oUsbDev3.m_uBulkEPMaxPktSize = FULL_SPEED_BULK_PKT_SIZE;
	}
}

static void shikhara_usb_get_connected_speed(USBDEV3_SPEED_e *eSpeed)
{
	usbdev3_dsts_t usbdev3_dsts;
	usbdev3_dsts.data = readl(rDSTS);
	*eSpeed = (USBDEV3_SPEED_e)usbdev3_dsts.b.connect_speed;
}

/* Connect Done event handler */
static void shikhara_usb_handle_connect_done_int(void)
{
	USBDEV3_SPEED_e eSpeed;
	usbdev3_depcmdpar0_set_ep_cfg_t usbdev3_depcmdpar0_set_ep_cfg;
	usbdev3_depcmdpar1_set_ep_cfg_t usbdev3_depcmdpar1_set_ep_cfg;
	int reg_gctl;
	oUsbDev3.m_uEp0State = EP0_STATE_INIT;

	shikhara_usb_get_connected_speed(&eSpeed);
	
	reg_gctl = readl(rGCTL);
        reg_gctl |= ((0 & 3) << 6);
        writel(reg_gctl,rGCTL);

	switch(eSpeed) {
		case USBDEV3_SPEED_SUPER :
			shikhara_usb_set_maxpktsizes(USBDEV3_SPEED_SUPER);
			DBG_USBD3("(Enumerated Speed : Super)\n");
			break;

		case USBDEV3_SPEED_HIGH :
			shikhara_usb_set_maxpktsizes(USBDEV3_SPEED_HIGH);
			DBG_USBD3("(Enumerated Speed : High)\n");
			break;

		case USBDEV3_SPEED_FULL :
			shikhara_usb_set_maxpktsizes(USBDEV3_SPEED_FULL);
			DBG_USBD3("(Enumerated Speed : Full)\n");
			break;

		default :
			return;
	}

	usbdev3_depcmdpar0_set_ep_cfg.data = 0;
	usbdev3_depcmdpar0_set_ep_cfg.b.ep_type = USBDEV3_EP_CTRL;
	usbdev3_depcmdpar0_set_ep_cfg.b.mps = oUsbDev3.m_uControlEPMaxPktSize;
	usbdev3_depcmdpar0_set_ep_cfg.b.ign_dsnum = 1;

	usbdev3_depcmdpar1_set_ep_cfg.data = 0;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_cmpl_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.xfer_nrdy_en = 1;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_num = 0;
	usbdev3_depcmdpar1_set_ep_cfg.b.ep_dir = USBDEV3_EP_DIR_OUT;
		
	if (!shikhara_usb_set_ep_cfg(USBDEV3_EP_DIR_OUT, 0, usbdev3_depcmdpar0_set_ep_cfg.data, usbdev3_depcmdpar1_set_ep_cfg.data))
		return ;


	usbdev3_depcmdpar1_set_ep_cfg.b.ep_dir = USBDEV3_EP_DIR_IN;

	if (!shikhara_usb_set_ep_cfg(USBDEV3_EP_DIR_IN, 0, usbdev3_depcmdpar0_set_ep_cfg.data, usbdev3_depcmdpar1_set_ep_cfg.data))
		return ;

		shikhara_usb_set_descriptor_tlb();
}

/* Device event handler */
static void shikhara_usb_handle_dev_event(usbdev3_devt_t uDevEvent)
{
	switch (uDevEvent.b.evt_type)
	{
		case DEVT_DISCONN:
			DBG_USBD3("Disconnect\n");
			shikhara_usb_handle_disconnect_int();
			break;

		case DEVT_USBRESET:
			DBG_USBD3("USB Reset\n");
			shikhara_usb_handle_reset_int();
			break;

		case DEVT_CONNDONE:
			DBG_USBD3("Connect Done\n");
			shikhara_usb_handle_connect_done_int();
			break;

		case DEVT_WKUP:
			DBG_USBD3("Wakeup\n");
			break;

		case DEVT_ULST_CHNG:
			
			DBG_USBD3("Link Status Change\n");
			break;

		case DEVT_EOPF:
			DBG_USBD3("End of Periodic Frame\n");
			break;

		case DEVT_SOF:
			DBG_USBD3("Start of Frame\n");
			break;

		case DEVT_ERRATICERR:
			DBG_USBD3("Erratic Error\n");
			break;

		case DEVT_CMD_CMPL:
			DBG_USBD3("Command Complete\n");
			break;

		case DEVT_OVERFLOW:
			DBG_USBD3("Overflow\n");
			break;

		default:
			DBG_USBD3("Unknown event!\n");
	}

}

static void shikhara_usb_handle_ep0_in_xfer_complete(void)
{
	switch (oUsbDev3.m_uEp0State) {
		case EP0_STATE_IN_DATA_PHASE:
			oUsbDev3.m_uEp0State = EP0_STATE_OUT_WAIT_NRDY;
			break;

		case EP0_STATE_IN_STATUS_PHASE:
			oUsbDev3.m_uEp0State = EP0_STATE_INIT;

			if (!shikhara_usb_start_ep0_setup_rx())
				return;
			break;

		default :
			DBG_USBD3("\nError : [EP0-InXferComplete]Not Supported @%d\n", oUsbDev3.m_uEp0State);
			break;
	}
}

static void shikhara_usb_handle_ep_in_xfer_complete(void)
{
	g_uCntOfDescOutComplete = 0;

	g_bIsBulkInXferDone = 1;

	shikhara_usb_free((u32)g_pBulkInTrbArray_Base);

	oUsbDev3.m_pUpPt += oUsbDev3.m_uUploadSize;
	DBG_USBD3("DMA IN : Transfer Complete\n");

}

/* Preparing TRB for DATA IN phase of control transfer */
static int shikhara_usb_start_ep0_in_xfer(u32 pBufAddr, u32 uLen)
{
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;

	usbdev3_trb_ctrl.data = 0;

	if (oUsbDev3.m_uEp0State == EP0_STATE_IN_STATUS_PHASE) {
		if (oUsbDev3.m_bEp0ThreeStage)
			usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_STATUS_3;
		else
			usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_STATUS_2;
	} else {
		usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_CTLDATA_1ST;
	}

	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 1;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	if(uLen != 0)	
		drd_flush_cache((u32)pBufAddr,uLen);		// Flushing the descriptor structure

	shikhara_usb_fill_trb(oUsbDev3.m_oInTrbPtr, pBufAddr, uLen, usbdev3_trb_ctrl.data, 1);
	
	
	if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_IN, 0, (u32)oUsbDev3.m_oInTrbPtr, 0, &oUsbDev3.m_uTriIn[0]))
	{
		return 0;
	}

	return 1;
}

static int shikhara_usb_start_ep0_out_xfer(u32 pBufAddr, u32 uLen)
{
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;

	usbdev3_trb_ctrl.data = 0;

	if (oUsbDev3.m_uEp0State == EP0_STATE_OUT_STATUS_PHASE) {
		if (oUsbDev3.m_bEp0ThreeStage)
			usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_STATUS_3;
		else
			usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_STATUS_2;
	} else {
		usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_CTLDATA_1ST;
	}

	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 1;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	shikhara_usb_fill_trb(oUsbDev3.m_oOutTrbPtr, pBufAddr, uLen, usbdev3_trb_ctrl.data, 1);


	if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_OUT, 0, (u32)oUsbDev3.m_oOutTrbPtr, 0, &oUsbDev3.m_uTriOut[0]))
		return 0;


	return 1;
}

static void shikhara_usb_setup_in_status_phase(void)
{

	DBG_USBD3("Setup EP0 IN ZLP\n");

	oUsbDev3.m_uEp0State = EP0_STATE_IN_STATUS_PHASE;

	shikhara_usb_start_ep0_in_xfer(oUsbDev3.m_uStatusBufAddr, 0);
}

static void shikhara_usb_setup_out_status_phase(void)
{
	DBG_USBD3("Setup EP0 OUT ZLP\n");

	oUsbDev3.m_uEp0State = EP0_STATE_OUT_STATUS_PHASE;

	shikhara_usb_start_ep0_out_xfer((u32)oUsbDev3.m_oDeviceRequest, 0);
}

static void shikhara_usb_handle_ep0_in_xfer_not_ready(void)
{
	switch (oUsbDev3.m_uEp0State) {
		case EP0_STATE_IN_WAIT_NRDY:
			shikhara_usb_setup_in_status_phase();
			break;
		default :
			DBG_USBD3("\nError : [EP0-InXferNotReady]Not Supported @%d\n", oUsbDev3.m_uEp0State);
			break;
	}
}

static void shikhara_usb_handle_ep_in_event(usbdev3_depevt_t uEpInEvent)
{
	u32 uEpNum = uEpInEvent.b.ep_num/2;

	DBG_USBD3("[EP%d] IN State = 0x%x Type = 0x%x[%x]\n", uEpNum, oUsbDev3.m_uEp0State, uEpInEvent.b.evt_type, uEpInEvent.data);
	switch (uEpInEvent.b.evt_type)
	{
		case DEPEVT_EVT_XFER_CMPL:
			DBG_USBD3("[EP%d] IN xfer complete @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			if (uEpNum == 0)
				shikhara_usb_handle_ep0_in_xfer_complete();
			else if (uEpNum == BULK_IN_EP) {
					shikhara_usb_handle_ep_in_xfer_complete();
			}
			else
				Assert(0);
			break;

		case DEPEVT_EVT_XFER_IN_PROG:
			DBG_USBD3("[EP%d] IN xfer in progress @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_XFER_NRDY:
			DBG_USBD3("[EP%d] IN xfer not ready @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			if (uEpNum == 0)
				shikhara_usb_handle_ep0_in_xfer_not_ready();
			break;

		case DEPEVT_EVT_FIFOXRUN:
			DBG_USBD3("[EP%d] IN FIFO Underrun Error @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_STRM_EVT:
			DBG_USBD3("[EP%d] IN Stream Event @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_EPCMD_CMPL:
			DBG_USBD3("[EP%d] IN Command Complete @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		default:
			DBG_USBD3("Unknown event!\n");
	}
}

/* Prepare first Bulk TRB during control transfer stage */
static int shikhara_usb_prepare_1st_bulk_out_trb(void)
{
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;

	g_bIsBulkOutXferDone = 0;
	g_bIsBulkInXferDone = 0;

	g_pBulkOutTrb0 = (usbdev3_trb_ptr_t)shikhara_usb_malloc(sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);
	if (g_pBulkOutTrb0 == NULL)
	{
		DBG_USBD3v("TRB0 Malloc failed \n");
		Assert(0);
	}
	g_ucTempDownBuf = (u8 *)shikhara_usb_malloc(4096 + 1, USBDEV3_MDWIDTH/8);

	if (g_ucTempDownBuf == NULL)
	{
		DBG_USBD3v("TEMP BUF0 Malloc failed \n");
		Assert(0);
	}

	usbdev3_trb_ctrl.data = 0;
	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_NORMAL;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 1;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	shikhara_usb_fill_trb(g_pBulkOutTrb0, (u32)g_ucTempDownBuf, oUsbDev3.m_uBulkEPMaxPktSize, usbdev3_trb_ctrl.data, 1);


	if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_OUT, BULK_OUT_EP, (u32)g_pBulkOutTrb0, 0, &oUsbDev3.m_uTriOut[BULK_OUT_EP]))
	{
		return 0;
	}
	return 1;
}

/* Read the setup packet received from Host and perform necessary action */
void shikhara_usb_handle_setup(void)
{
	const u8 *string_desc1, *string_desc2;
	usbdev3_dcfg_t usbdev3_dcfg;
	u32 uRemoteWakeUp = 0;

	drd_inval_cache((uint32_t)oUsbDev3.m_oDeviceRequest,sizeof(DEVICE_REQUEST));		// Invalidate data buffer before reading

	shikhara_usb_print_ep0_pkt((u8 *)oUsbDev3.m_oDeviceRequest, 8);

	switch(oUsbDev3.m_oDeviceRequest->bmRequestType & 0x60) {
		case STANDARD_TYPE:
			break;

		case CLASS_TYPE:
			DBG_USBD3("Class Type Request is not supported yet\n");
			return;

		case VENDOR_TYPE:
			DBG_USBD3("Vendor Type Request is not supported yet\n");
			return;

		default:
			DBG_USBD3("0x%02x Type Request is not supported yet\n", oUsbDev3.m_oDeviceRequest->bmRequestType & 0x60);
			return;
	}

	if (oUsbDev3.m_oDeviceRequest->bmRequestType & 0x80)
		oUsbDev3.m_uEp0State = EP0_STATE_IN_DATA_PHASE;
	else
		oUsbDev3.m_uEp0State = EP0_STATE_OUT_DATA_PHASE;

	oUsbDev3.m_uDeviceRequestLength = (u32)((oUsbDev3.m_oDeviceRequest->wLength_H << 8) | oUsbDev3.m_oDeviceRequest->wLength_L);

	oUsbDev3.m_bEp0ThreeStage = 1;

	if (oUsbDev3.m_uDeviceRequestLength == 0)
	{
		oUsbDev3.m_uEp0State = EP0_STATE_IN_WAIT_NRDY;
		oUsbDev3.m_bEp0ThreeStage = 0;
	}

	switch(oUsbDev3.m_oDeviceRequest->bRequest) {
		case STANDARD_SET_ADDRESS:
			usbdev3_dcfg.data = readl(rDCFG);
			usbdev3_dcfg.b.dev_addr = oUsbDev3.m_oDeviceRequest->wValue_L;
			writel(usbdev3_dcfg.data, rDCFG);

			DBG_USBD3("\n MCU >> Set Address : %d \n", usbdev3_dcfg.b.dev_addr);

			oUsbDev3.m_eUsbDev3State = USBDEV3_STATE_ADDRESSED;
			break;

		case STANDARD_SET_DESCRIPTOR:
			DBG_USBD3("\n MCU >> Set Descriptor \n");
			break;

		case STANDARD_SET_CONFIGURATION:
			DBG_USBD3("\n MCU >> Set Configuration \n");
			if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER){
				debug("Super speed enumeration success\n");
				printf("Enumeration success\n");
			}
			g_usConfig = oUsbDev3.m_oDeviceRequest->wValue_L; 
			oUsbDev3.m_eUsbDev3State = USBDEV3_STATE_CONFIGURED;

			shikhara_usb_activate_ep(USBDEV3_EP_DIR_IN, BULK_IN_EP);
			shikhara_usb_activate_ep(USBDEV3_EP_DIR_OUT, BULK_OUT_EP);

			shikhara_usb_enable_ep(USBDEV3_EP_DIR_IN, BULK_IN_EP);
			shikhara_usb_enable_ep(USBDEV3_EP_DIR_OUT, BULK_OUT_EP);
			
			shikhara_usb_prepare_1st_bulk_out_trb();
			break;

		case STANDARD_GET_CONFIGURATION:
			shikhara_usb_start_ep0_in_xfer((u32)&g_usConfig, 1);
			break;

		case STANDARD_GET_DESCRIPTOR:
			switch (oUsbDev3.m_oDeviceRequest->wValue_H)
			{
				case DEVICE_DESCRIPTOR:
					oUsbDev3.m_uDeviceRequestLength = (u32)((oUsbDev3.m_oDeviceRequest->wLength_H << 8) |
						oUsbDev3.m_oDeviceRequest->wLength_L);
					DBG_USBD3("\n MCU >> Get Device Descriptor = 0x%x \n",oUsbDev3.m_uDeviceRequestLength);

					if (oUsbDev3.m_uDeviceRequestLength<=DEVICE_DESC_SIZE)
					{
						shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescDevice))+0, oUsbDev3.m_uDeviceRequestLength);
					}
					else
					{
						shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescDevice))+0, DEVICE_DESC_SIZE);
					}
					break;

				case CONFIGURATION_DESCRIPTOR:
					oUsbDev3.m_uDeviceRequestLength = (u32)((oUsbDev3.m_oDeviceRequest->wLength_H << 8) |
						oUsbDev3.m_oDeviceRequest->wLength_L);
					DBG_USBD3("\n MCU >> Get Configuration Descriptor = 0x%x \n",oUsbDev3.m_uDeviceRequestLength);

					if (oUsbDev3.m_uDeviceRequestLength > CONFIG_DESC_SIZE){
						if(oUsbDev3.m_uDeviceRequestLength<=oUsbDev3.m_oDesc.oDescConfig.wTotalLengthL)
						{
							shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescConfig))+0, oUsbDev3.m_uDeviceRequestLength);
						}
						else
						{
							shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescConfig))+0, oUsbDev3.m_oDesc.oDescConfig.wTotalLengthL);
						}
					}
					else		// for win2k
					{
						if(oUsbDev3.m_uDeviceRequestLength<=CONFIG_DESC_SIZE)
						{
							shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescConfig))+0, oUsbDev3.m_uDeviceRequestLength);
						}
						else
						{
							shikhara_usb_start_ep0_in_xfer(((u32)&(oUsbDev3.m_oDesc.oDescConfig))+0, CONFIG_DESC_SIZE);
						}
					}
					break;

				case STRING_DESCRIPTOR :
					DBG_USBD3("\n MCU >> Get String Descriptor \n");
						string_desc1 = dnw_string_desc1;
						string_desc2 = dnw_string_desc2;

					switch(oUsbDev3.m_oDeviceRequest->wValue_L)
					{
						case 0:
							shikhara_usb_start_ep0_in_xfer((u32)string_desc0, STRING_DESC0_SIZE);
							break;
						case 1:
							shikhara_usb_start_ep0_in_xfer((u32)string_desc1, STRING_DESC1_SIZE);
							break;
						case 2:
							shikhara_usb_start_ep0_in_xfer((u32)string_desc2, STRING_DESC2_SIZE);
							break;
						case 3:
							shikhara_usb_start_ep0_in_xfer((u32)string_desc2, STRING_DESC2_SIZE);
							break;
						default:
								break;
					}
					break;

				case ENDPOINT_DESCRIPTOR:
					DBG_USBD3("\n MCU >> Get Endpoint Descriptor \n");
					switch(oUsbDev3.m_oDeviceRequest->wValue_L&0xf)
					{
						case 0:
							if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER)
								shikhara_usb_start_ep0_in_xfer((u32)&(oUsbDev3.m_oSSDesc.oDescEp0), ENDPOINT_DESC_SIZE+ENDPOINT_COMP_DESC_SIZE);
							else
								shikhara_usb_start_ep0_in_xfer((u32)&(oUsbDev3.m_oDesc.oDescEp0), ENDPOINT_DESC_SIZE);
							break;
						case 1:
							if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_SUPER)
								shikhara_usb_start_ep0_in_xfer((u32)&(oUsbDev3.m_oSSDesc.oDescEp1), ENDPOINT_DESC_SIZE+ENDPOINT_COMP_DESC_SIZE);
							else
								shikhara_usb_start_ep0_in_xfer((u32)&(oUsbDev3.m_oDesc.oDescEp1), ENDPOINT_DESC_SIZE);
							break;
						default:
							break;
					}
					break;

				case DEVICE_QUALIFIER:
					oUsbDev3.m_uDeviceRequestLength = (u32)((oUsbDev3.m_oDeviceRequest->wLength_H << 8) |
						oUsbDev3.m_oDeviceRequest->wLength_L);
					DBG_USBD3("\n MCU >> Get Device Qualifier Descriptor = 0x%x \n",oUsbDev3.m_uDeviceRequestLength);

					if(oUsbDev3.m_uDeviceRequestLength<=10)
					{
						shikhara_usb_start_ep0_in_xfer((u32)qualifier_desc, oUsbDev3.m_uDeviceRequestLength);
					}
					else
					{
						shikhara_usb_start_ep0_in_xfer((u32)qualifier_desc, 10);
					}
					break;

			   	case OTHER_SPEED_CONFIGURATION :
					DBG_USBD3(("\n MCU >> Get OTHER_SPEED_CONFIGURATION \n"));
					oUsbDev3.m_uDeviceRequestLength = (u32)((oUsbDev3.m_oDeviceRequest->wLength_H << 8) |
						oUsbDev3.m_oDeviceRequest->wLength_L);

					if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_HIGH)
					{
						if (oUsbDev3.m_uDeviceRequestLength ==9)
						{
							shikhara_usb_start_ep0_in_xfer((u32)config_high, 9);
						}
					    	else if(oUsbDev3.m_uDeviceRequestLength ==32)
						{
							shikhara_usb_start_ep0_in_xfer((u32)config_high_total, 32);
						}
					}
					else if (oUsbDev3.m_eSpeed == USBDEV3_SPEED_FULL)
					{
						if (oUsbDev3.m_uDeviceRequestLength ==9)
						{
							shikhara_usb_start_ep0_in_xfer((u32)config_full, 9);
						}
						else if(oUsbDev3.m_uDeviceRequestLength ==32)
						{
							shikhara_usb_start_ep0_in_xfer((u32)config_full_total, 32);
						}
					}
					else	
					{
						DBG_USBD3("\n %s(line %d)\n", __FILE__, __LINE__);
						DBG_USBD3("Error : Not implemented yet\n");
					}

					break;
				case BOS :
					if (oUsbDev3.m_uDeviceRequestLength == BOS_DESC_SIZE)
						shikhara_usb_start_ep0_in_xfer((u32)&oUsbDev3.m_oSSDesc.oDescBos, BOS_DESC_SIZE);
					else
						shikhara_usb_start_ep0_in_xfer((u32)&oUsbDev3.m_oSSDesc.oDescBos, BOS_DESC_TOTAL_SIZE);
					break;
			}
			break;

		case STANDARD_CLEAR_FEATURE:
			DBG_USBD3("\n MCU >> Clear Feature \n");
			switch (oUsbDev3.m_oDeviceRequest->bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (oUsbDev3.m_oDeviceRequest->wValue_L == 1)
						uRemoteWakeUp = 0;
					break;

				case ENDPOINT_RECIPIENT:
					if (oUsbDev3.m_oDeviceRequest->wValue_L == 0)
					{
						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 0;

						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_IN_EP)
							oStatusGet.EndpointIn= 0;

						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_OUT_EP) 
							oStatusGet.EndpointOut= 0;
					}
					break;

				default:
					break;
			}
			break;

		case STANDARD_SET_FEATURE:
			DBG_USBD3("\n MCU >> Set Feature \n");
			switch (oUsbDev3.m_oDeviceRequest->bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (oUsbDev3.m_oDeviceRequest->wValue_L == 1)
						uRemoteWakeUp = 1;
						break;

				case ENDPOINT_RECIPIENT:
					if (oUsbDev3.m_oDeviceRequest->wValue_L == 0)
					{
						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 1;

						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_IN_EP)
							oStatusGet.EndpointIn= 1;

						if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_OUT_EP)
							oStatusGet.EndpointOut= 1;
					}
					break;

				default:
					break;
			}

			switch (oUsbDev3.m_oDeviceRequest->wValue_L) {

				case EP_STALL:
					break;


				case TEST_MODE:
					if ((0 != oUsbDev3.m_oDeviceRequest->wIndex_L ) ||(0 != oUsbDev3.m_oDeviceRequest->bmRequestType))
					{
						DBG_USBD3("\n %s(line %d)\n", __FILE__, __LINE__);
						DBG_USBD3("Error : Wrong Request Parameter\n");
						break;
					}

					switch(oUsbDev3.m_oDeviceRequest->wIndex_H)
					{
						usbdev3_dctl_t usbdev3_dctl;

						case TEST_J:
							//Set Test J
							shikhara_usb_start_ep0_in_xfer((u32)NULL, 0);
							DBG_USBD3 ("Test_J\n");
							usbdev3_dctl.data = readl(rDCTL);
							usbdev3_dctl.b.test_ctrl = (u32)DCTL_TEST_J_MODE;
							writel(usbdev3_dctl.data, rDCTL);
							break;

						case TEST_K:
							//Set Test K
							shikhara_usb_start_ep0_in_xfer((u32)NULL, 0);
							DBG_USBD3 ("Test_K\n");
							usbdev3_dctl.data = readl(rDCTL);
							usbdev3_dctl.b.test_ctrl = (u32)DCTL_TEST_K_MODE;
							writel(usbdev3_dctl.data, rDCTL);
							break;

						case TEST_SE0_NAK:
							//Set Test SE0_NAK
							shikhara_usb_start_ep0_in_xfer((u32)NULL, 0);
							DBG_USBD3 ("Test_SE0_NAK\n");
							usbdev3_dctl.data = readl(rDCTL);
							usbdev3_dctl.b.test_ctrl = (u32)DCTL_TEST_SE0_NAK_MODE;
							writel(usbdev3_dctl.data, rDCTL);
							break;

						case TEST_PACKET:
							shikhara_usb_start_ep0_in_xfer((u32)NULL, 0);
							DBG_USBD3 ("Test_Packet\n");
							usbdev3_dctl.data = readl(rDCTL);
							usbdev3_dctl.b.test_ctrl = (u32)DCTL_TEST_PACKET_MODE;
							writel(usbdev3_dctl.data, rDCTL);
							break;

						case TEST_FORCE_ENABLE:
							shikhara_usb_start_ep0_in_xfer((u32)NULL, 0);
							DBG_USBD3 ("Test_Force_Enable\n");
							usbdev3_dctl.data = readl(rDCTL);
							usbdev3_dctl.b.test_ctrl = (u32)DCTL_TEST_FORCE_ENABLE;
							writel(usbdev3_dctl.data, rDCTL);
							break;
					}

					break;

				default:
					break;
			}
			break;

		case STANDARD_GET_STATUS:
			switch(oUsbDev3.m_oDeviceRequest->bmRequestType)
			{
				case  (0x80):	//device
					oStatusGet.Device=((u8)uRemoteWakeUp<<1)|0x1;		// SelfPowered
					shikhara_usb_start_ep0_in_xfer((u32)&oStatusGet.Device, 1);
					break;

				case  (0x81):	//interface
					oStatusGet.Interface=0;
					shikhara_usb_start_ep0_in_xfer((u32)&oStatusGet.Interface, 1);
					break;

				case  (0x82):	//endpoint
					if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == CONTROL_EP)
						shikhara_usb_start_ep0_in_xfer((u32)&oStatusGet.EndpointCtrl, 1);

					if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_IN_EP)
						shikhara_usb_start_ep0_in_xfer((u32)&oStatusGet.EndpointIn, 1);

					if ((oUsbDev3.m_oDeviceRequest->wIndex_L & 0x7f) == BULK_OUT_EP)
						shikhara_usb_start_ep0_in_xfer((u32)&oStatusGet.EndpointOut, 1);
					break;

				default:
					break;
			}
			break;

		case STANDARD_GET_INTERFACE:
			shikhara_usb_start_ep0_in_xfer((u32)&oInterfaceGet.AlternateSetting, 1);
			break;

		case STANDARD_SET_INTERFACE:
			oInterfaceGet.AlternateSetting= oUsbDev3.m_oDeviceRequest->wValue_L;
			break;

		case STANDARD_SYNCH_FRAME:
			break;

		case STANDARD_SET_SEL:
			oUsbDev3.m_bReq_Set_sel= 1;
			shikhara_usb_start_ep0_out_xfer((u32)&set_sel, oUsbDev3.m_uControlEPMaxPktSize);
			DBG_USBD3("Standard Req : SET SEL\n");
			break;

		case STANDARD_ISOCH_DELY:
			DBG_USBD3("Standard Req : ISOCH Delay\n");
			break;

		default:
			DBG_USBD3("\n %s(line %d)\n", __FILE__, __LINE__);
			DBG_USBD3("Error : This Request(%d) is not implemented yet\n", oUsbDev3.m_oDeviceRequest->bRequest);
			break;
	}
}

static void shikhara_usb_handle_ep0_out_xfer_complete(void)
{
	switch (oUsbDev3.m_uEp0State) {
		case EP0_STATE_INIT:
			shikhara_usb_handle_setup();
			break;

		case EP0_STATE_OUT_DATA_PHASE:
			oUsbDev3.m_uEp0State = EP0_STATE_IN_WAIT_NRDY;
			break;

		case EP0_STATE_OUT_STATUS_PHASE:
			oUsbDev3.m_uEp0State = EP0_STATE_INIT;
			if (!shikhara_usb_start_ep0_setup_rx())
			{
				return;
			}
			break;

		default :
			DBG_USBD3("\nError : [EP0-OutXferComplete]Not Supported @%d\n", oUsbDev3.m_uEp0State);
			break;
	}

}

/* Bulk Transfer handling */
static void shikhara_usb_handle_ep_out_xfer_complete(void)
{
	u16	usRxCnt;
	u16 usCheck;
	u32 usCapTrbBufSiz;
	u32 uLastBufSize;
	u32 i=0;
	usbdev3_trb_ptr_t pBulkOutTrb;
	usbdev3_trb_ptr_t pBulkInTrb;
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;

	if (g_uCntOfDescOutComplete == 0)
	{
		// Check whether TRB was finished successfully or not
		if ((g_pBulkOutTrb0->control.b.hwo != 0)||(g_pBulkOutTrb0->status.b.trb_sts != 0))
		{
			DBG_USBD3v("status failed.....\n");
			Assert(0);
		}

		g_uCntOfDescOutComplete++;

		DBG_USBD3v("oUsbDev3.m_uBulkEPMaxPktSize -->%d\n  g_pBulkOutTrb0->status.b.buf_siz --> %d\n",oUsbDev3.m_uBulkEPMaxPktSize,g_pBulkOutTrb0->status.b.buf_siz);
		usRxCnt = oUsbDev3.m_uBulkEPMaxPktSize - g_pBulkOutTrb0->status.b.buf_siz;
		DBG_USBD3v("first time usRxCnt --> %d\n",usRxCnt);


		shikhara_usb_free((u32)g_pBulkOutTrb0);

		if (usRxCnt == 10)		//Upload Request
		{
			DBG_USBD3v("upload req.....\n");
			usCheck = *((u8 *)(g_ucTempDownBuf+8)) + (*((u8 *)(g_ucTempDownBuf+9))<<8);

			if (usCheck == 0x1)
			{
				oUsbDev3.m_uUploadAddr =
					*((u8 *)(g_ucTempDownBuf+0))+
					(*((u8 *)(g_ucTempDownBuf+1))<<8)+
					(*((u8 *)(g_ucTempDownBuf+2))<<16)+
					(*((u8 *)(g_ucTempDownBuf+3))<<24);

				oUsbDev3.m_uUploadSize =
					*((u8 *)(g_ucTempDownBuf+4))+
					(*((u8 *)(g_ucTempDownBuf+5))<<8)+
					(*((u8 *)(g_ucTempDownBuf+6))<<16)+
					(*((u8 *)(g_ucTempDownBuf+7))<<24);

				shikhara_usb_free((u32)g_ucTempDownBuf);

				oUsbDev3.m_pUpPt=(u8 *)oUsbDev3.m_uUploadAddr;

				DBG_USBD3("UploadAddress : 0x%x, UploadSize: %d\n", oUsbDev3.m_uUploadAddr, oUsbDev3.m_uUploadSize);

 				if  (oUsbDev3.m_uUploadSize>0)
				{
					DBG_USBD3("Dma Start for IN PKT \n");

					usCapTrbBufSiz = TRB_BUF_SIZ_LIMIT/oUsbDev3.m_uBulkEPMaxPktSize*oUsbDev3.m_uBulkEPMaxPktSize;

					g_uCntOfBulkInTrb = oUsbDev3.m_uUploadSize/usCapTrbBufSiz;

					if ((oUsbDev3.m_uUploadSize%usCapTrbBufSiz) != 0)
					{
						g_uCntOfBulkInTrb++;
					}

					g_pBulkInTrbArray_Base = (usbdev3_trb_ptr_t)shikhara_usb_malloc(g_uCntOfBulkInTrb*sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);

					if (g_pBulkInTrbArray_Base == NULL)
					{
						DBG_USBD3v("g_pBulkInTrbArray_Base - malloc failed \n");
						Assert(0);
					}

					pBulkInTrb = g_pBulkInTrbArray_Base;
					usbdev3_trb_ctrl.data = 0;
					usbdev3_trb_ctrl.b.lst = 0;
					usbdev3_trb_ctrl.b.chn = 1;
					usbdev3_trb_ctrl.b.csp = 0;
					usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_NORMAL;
					usbdev3_trb_ctrl.b.isp_imi = 1;
					usbdev3_trb_ctrl.b.ioc = 0;
					usbdev3_trb_ctrl.b.strmid_sofn = 0;
					for(i=0;i<(g_uCntOfBulkInTrb-1);i++)
					{
						shikhara_usb_fill_trb(pBulkInTrb, (u32)(oUsbDev3.m_pUpPt+usCapTrbBufSiz*i), usCapTrbBufSiz, usbdev3_trb_ctrl.data, 1);
						pBulkInTrb++;
					}

					usbdev3_trb_ctrl.b.lst = 1;
					usbdev3_trb_ctrl.b.chn = 0;
					usbdev3_trb_ctrl.b.ioc = 1;
					uLastBufSize = oUsbDev3.m_uUploadSize-usCapTrbBufSiz*i;
					uLastBufSize = ((uLastBufSize+oUsbDev3.m_uBulkEPMaxPktSize-1)/oUsbDev3.m_uBulkEPMaxPktSize)*oUsbDev3.m_uBulkEPMaxPktSize;
					shikhara_usb_fill_trb(pBulkInTrb, (u32)(oUsbDev3.m_pUpPt+usCapTrbBufSiz*i), uLastBufSize, usbdev3_trb_ctrl.data, 1);
					if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_IN, BULK_IN_EP, (u32)g_pBulkInTrbArray_Base, 0, &oUsbDev3.m_uTriIn[BULK_IN_EP])){
					DBG_USBD3v("usb start has failed.....\n");
					return;
					}
				}
			}
			else
			{
				DBG_USBD3v("check sum Failed.......");
				Assert(0);
			}
		}
		else
		{
			
			DBG_USBD3v("download req.....\n");
			drd_inval_cache(g_ucTempDownBuf,4096);
			oUsbDev3.m_uDownloadAddress =
					*((u8 *)(g_ucTempDownBuf+0))+
					(*((u8 *)(g_ucTempDownBuf+1))<<8)+
					(*((u8 *)(g_ucTempDownBuf+2))<<16)+
					(*((u8 *)(g_ucTempDownBuf+3))<<24);
			DBG_USBD3v("oUsbDev3.m_uDownloadAddress --> %X\n",oUsbDev3.m_uDownloadAddress);
			oUsbDev3.m_uDownloadFileSize =
				*((u8 *)(g_ucTempDownBuf+4))+
				(*((u8 *)(g_ucTempDownBuf+5))<<8)+
				(*((u8 *)(g_ucTempDownBuf+6))<<16)+
				(*((u8 *)(g_ucTempDownBuf+7))<<24);
			DBG_USBD3v("oUsbDev3.m_uDownloadFileSize --> %X\n",oUsbDev3.m_uDownloadFileSize);
			drd_inval_cache(oUsbDev3.m_uDownloadAddress,oUsbDev3.m_uDownloadFileSize);
			if (shikhara_usbd_dn_addr)
			{
				oUsbDev3.m_uDownloadAddress = shikhara_usbd_dn_addr;
				DBG_USBD3v("2nd oUsbDev3.m_uDownloadAddress --> %X\n",oUsbDev3.m_uDownloadAddress);
			}

			oUsbDev3.m_pDownPt=(u8 *)oUsbDev3.m_uDownloadAddress;

			printf("downloadAddress : 0x%x, downloadFileSize: %d\n", oUsbDev3.m_uDownloadAddress, oUsbDev3.m_uDownloadFileSize);

			memcpy((void *)oUsbDev3.m_pDownPt, (void *)(g_ucTempDownBuf+8), usRxCnt-8);

			shikhara_usb_free((u32)g_ucTempDownBuf);
			DBG_USBD3v("usRxCnt --> %d\n",usRxCnt);

			oUsbDev3.m_pDownPt += usRxCnt-8;

			if (oUsbDev3.m_uDownloadFileSize>usRxCnt)
			{

				usCapTrbBufSiz = TRB_BUF_SIZ_LIMIT/oUsbDev3.m_uBulkEPMaxPktSize*oUsbDev3.m_uBulkEPMaxPktSize;
				DBG_USBD3v("usCapTrbBufSiz --> %d\n",usCapTrbBufSiz);

				g_uCntOfBulkOutTrb = (oUsbDev3.m_uDownloadFileSize-usRxCnt)/usCapTrbBufSiz;
				DBG_USBD3v("g_uCntOfBulkOutTrb --> %d\n",g_uCntOfBulkOutTrb);

				if ((oUsbDev3.m_uDownloadFileSize-usRxCnt)%usCapTrbBufSiz != 0)
				{
					g_uCntOfBulkOutTrb++;
					DBG_USBD3v("g_uCntOfBulkOutTrb --> %d\n",g_uCntOfBulkOutTrb);
				}

				g_pBulkOutTrbArray_Base = (usbdev3_trb_ptr_t)shikhara_usb_malloc(g_uCntOfBulkOutTrb*sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);

				if (g_pBulkOutTrbArray_Base == NULL)
				{
					DBG_USBD3v("g_pBulkOutTrbArray_Base - malloc failed....");
					Assert(0);
				}

				pBulkOutTrb = (usbdev3_trb_ptr_t)virt_to_phys((void *)g_pBulkOutTrbArray_Base);

				usbdev3_trb_ctrl.data = 0;
				usbdev3_trb_ctrl.b.lst = 0;
				usbdev3_trb_ctrl.b.chn = 1;
				usbdev3_trb_ctrl.b.csp = 0;
				usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_NORMAL;
				usbdev3_trb_ctrl.b.isp_imi = 1;
				usbdev3_trb_ctrl.b.ioc = 0;
				usbdev3_trb_ctrl.b.strmid_sofn = 0;

				for(i=0;i<(g_uCntOfBulkOutTrb-1);i++)
				{
					shikhara_usb_fill_trb(pBulkOutTrb, (u32)(oUsbDev3.m_pDownPt+usCapTrbBufSiz*i), usCapTrbBufSiz, usbdev3_trb_ctrl.data, 1);
					pBulkOutTrb++;
				}
				DBG_USBD3v("Trb %x processed:\n",pBulkOutTrb);


				usbdev3_trb_ctrl.b.lst = 1;
				usbdev3_trb_ctrl.b.chn = 0;
				usbdev3_trb_ctrl.b.ioc = 1;
				uLastBufSize = (oUsbDev3.m_uDownloadFileSize-usRxCnt)-usCapTrbBufSiz*i;
				uLastBufSize = ((uLastBufSize+oUsbDev3.m_uBulkEPMaxPktSize-1)/oUsbDev3.m_uBulkEPMaxPktSize)*oUsbDev3.m_uBulkEPMaxPktSize;
				shikhara_usb_fill_trb(pBulkOutTrb, (u32)(oUsbDev3.m_pDownPt+usCapTrbBufSiz*i), uLastBufSize, usbdev3_trb_ctrl.data, 1);
				
				if (!shikhara_usb_start_ep_xfer(USBDEV3_EP_DIR_OUT, BULK_OUT_EP, (u32)g_pBulkOutTrbArray_Base, 0, &oUsbDev3.m_uTriOut[BULK_OUT_EP])){	
				DBG_USBD3v("Last packet:usb start has failed.....\n");
				return;
				}
			}
			else
			{
				g_uCntOfDescOutComplete = 0;

				shikhara_receive_done = 1;

				DBG_USBD3("DMA OUT : Transfer Complete\n");
			}
		}
	}
	else
	{
		DBG_USBD3v("Final else");
		g_uCntOfDescOutComplete = 0;

		shikhara_receive_done = 1;

		shikhara_usb_free((u32)g_pBulkOutTrbArray_Base);

		oUsbDev3.m_pDownPt += (oUsbDev3.m_uDownloadFileSize - 8);

		printf("Download Done!! Download Address: 0x%x, Download Filesize:0x%x\n",
				oUsbDev3.m_uDownloadAddress, (oUsbDev3.m_uDownloadFileSize-10));
	}
}

static void shikhara_usb_handle_ep0_out_xfer_not_ready(void)
{
	switch (oUsbDev3.m_uEp0State) {
		case EP0_STATE_OUT_WAIT_NRDY:
			shikhara_usb_setup_out_status_phase();
			break;
		default :
			DBG_USBD3("\nError : [EP0-OutXferNotReady]Not Supported @%d\n", oUsbDev3.m_uEp0State);
			break;
	}
}

static void shikhara_usb_handle_ep_out_event(usbdev3_depevt_t uEpOutEvent)
{
	u32 uEpNum = uEpOutEvent.b.ep_num/2;	

	DBG_USBD3("[EP%d] Out State = 0x%x Type = 0x%x[0x%x]\n", uEpNum, oUsbDev3.m_uEp0State, uEpOutEvent.b.evt_type, uEpOutEvent.data);
	switch (uEpOutEvent.b.evt_type)
	{
		case DEPEVT_EVT_XFER_CMPL:
			DBG_USBD3("[EP%d] OUT xfer complete @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			if (uEpNum == 0)
				shikhara_usb_handle_ep0_out_xfer_complete();
			else if (uEpNum == BULK_OUT_EP)
					shikhara_usb_handle_ep_out_xfer_complete();
			else
				Assert(0);
			break;

		case DEPEVT_EVT_XFER_IN_PROG:
			DBG_USBD3("[EP%d] OUT xfer in progress @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_XFER_NRDY:
			DBG_USBD3("[EP%d] OUT xfer not ready @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			if (uEpNum == 0)
				shikhara_usb_handle_ep0_out_xfer_not_ready();
			else
				;
			break;

		case DEPEVT_EVT_FIFOXRUN:
			DBG_USBD3("[EP%d] OUT FIFO Overrun Error @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_STRM_EVT:
			DBG_USBD3("[EP%d] OUT Stream Event @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		case DEPEVT_EVT_EPCMD_CMPL:
			DBG_USBD3("[EP%d] OUT Command Complete @%d\n", uEpNum, oUsbDev3.m_uEp0State);
			break;

		default:
			DBG_USBD3("Unknown event!\n");
	}

}

/* Event Handler */
static void shikhara_usb_handle_event(void)
{
	u16 uEventCount, uLoop=0;
	u32 uEventBufferCopied;

	drd_inval_cache((uint32_t)oUsbDev3.m_pEventBuffer,512);		// Invalidating the buffer before reading

	uEventCount = readl(rGEVNTCOUNT0);			// Reading Event count to know no of events

	uEventCount = uEventCount/4;

	if (uEventCount == 0)
		return;

	else
		DBG_USBD3("## Event Count is %d ##\n", uEventCount);


	while((uEventCount-- > 0) && (uLoop < USBDEV3_EVENT_BUFFER_COUNT))
	{
		if (oUsbDev3.m_CurrentEventPosition == USBDEV3_EVENT_BUFFER_COUNT)
		{
			oUsbDev3.m_CurrentEventPosition = 0;
		}
	
		uEventBufferCopied = *(oUsbDev3.m_pEventBuffer + oUsbDev3.m_CurrentEventPosition);		// Copying the content of event buffer to know type of event

		uLoop++;
		oUsbDev3.m_CurrentEventPosition++;

		writel(4, rGEVNTCOUNT0);		// Clearing the event
		if (uEventBufferCopied == 0)
			DBG_USBD3("## Null Event!##\n");
		else
		{
			DBG_USBD3("\nLoop%d: Content of %dth Event Buffer is 0x%08x\n", uLoop, oUsbDev3.m_CurrentEventPosition, uEventBufferCopied);

			if (uEventBufferCopied & 0x1)		// Device event
			{
				usbdev3_devt_t usbdev3_devt;
				usbdev3_devt.data = uEventBufferCopied;

				if (usbdev3_devt.b.dev_specific != 0)
					DBG_USBD3("Other Core Event\n");

				DBG_USBD3v(" Device event \n");
				shikhara_usb_handle_dev_event(usbdev3_devt);
			}
			else
			{					// Endpoint event
				usbdev3_depevt_t usbdev3_depevt;
				u32 uEpNum;

				usbdev3_depevt.data = uEventBufferCopied;

				uEpNum = usbdev3_depevt.b.ep_num;
				DBG_USBD3v(" EpNum --> %d\n",uEpNum);
				if (uEpNum & 1){
					DBG_USBD3v("IN event came....\n");
					shikhara_usb_handle_ep_in_event(usbdev3_depevt);
				}
				else{
					DBG_USBD3v("OUT event came....\n");
					shikhara_usb_handle_ep_out_event(usbdev3_depevt);
				}
			}
		}
	}
}

/* Start the DRD controller */
int shikhara_usbc_activate (void)
{
	shikhara_usb_runstop_device(1);
	shikhara_usb_wait_cable_insert();
	return 0;
}

/* Stop the DRD controller */
int shikhara_usb_stop( void )
{
	shikhara_usb_runstop_device(0);

	return 0;
}

int shikhara_udc_int_hndlr(void)
{
	shikhara_usb_handle_event();
	return 0;
}

/* Initalise the DRD controller */
int shikhara_usbctl_init(void)
{
	usbdev3_gusb2phycfg_t usbdev3_gusb2phycfg;
	usbdev3_gusb3pipectl_t usbdev3_gusb3pipectl;
	usbdev3_gctl_t usbdev3_gctl;
	USBDEV3_SPEED_e eSpeed = USBDEV3_SPEED_SUPER;
	u32 reg_val,reg_gctl;

	oUsbDev3.m_eSpeed = eSpeed;
	oUsbDev3.m_eUsbDev3State = USBDEV3_STATE_DEFAULT;
	oUsbDev3.m_uEp0State = EP0_STATE_UNCONNECTED;
	oUsbDev3.m_uEp0SubState = 0;
	oUsbDev3.m_bEPs_Enabled = 0;
	oUsbDev3.m_bReq_Set_sel = 0;
	switch(eSpeed)
	{
		case USBDEV3_SPEED_SUPER:
			oUsbDev3.m_uControlEPMaxPktSize = SUPER_SPEED_CONTROL_PKT_SIZE;
			break;

		case USBDEV3_SPEED_FULL:
			oUsbDev3.m_uControlEPMaxPktSize = FULL_SPEED_CONTROL_PKT_SIZE;
			break;

		default :
			oUsbDev3.m_uControlEPMaxPktSize = HIGH_SPEED_CONTROL_PKT_SIZE;
			break;
	}

	/* Allocating all the TRB's, event buffer and data buffer in system memory */
	oUsbDev3.m_pEventBuffer = (u32 *)shikhara_usb_malloc(4*USBDEV3_EVENT_BUFFER_COUNT, USBDEV3_MDWIDTH/8);
	oUsbDev3.m_uStatusBufAddr = shikhara_usb_malloc(CTRL_BUF_SIZE, USBDEV3_MDWIDTH/8);
	oUsbDev3.m_oSetupTrbPtr = (usbdev3_trb_ptr_t)shikhara_usb_malloc(sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);
	oUsbDev3.m_oOutTrbPtr = (usbdev3_trb_ptr_t)shikhara_usb_malloc(sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);
	oUsbDev3.m_oInTrbPtr = (usbdev3_trb_ptr_t)shikhara_usb_malloc(sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);
	oUsbDev3.m_oDeviceRequest = (DEVICE_REQUEST_PTR)shikhara_usb_malloc(sizeof(DEVICE_REQUEST),USBDEV3_MDWIDTH/8);

	shikhara_usb_softreset_core();		// Softreset Device Controller
	
	writel(INCR16,rGSBUSCFG0);		// Keeping AXI burst as 16

	/* Core Softreset */
	usbdev3_gctl.data = readl(rGCTL);
	usbdev3_gctl.b.core_soft_reset = 1;
	writel(usbdev3_gctl.data, rGCTL);
	
	shikhara_usb_softreset_phy(1);
	shikhara_usb_phy_on();
	shikhara_usb_init_phy();
	shikhara_usb_softreset_phy(0);
	usbdev3_gctl.data = readl(rGCTL);
	usbdev3_gctl.b.core_soft_reset = 0;	
	writel(usbdev3_gctl.data, rGCTL);	// Core Reset release

	unsigned int temp = 0;
	temp = readl(rGUSB2PHYCFG);
	DBG_USBD3("1.Default GUSB2PHYCFG = 0x%x\n",readl(rGUSB2PHYCFG));
	temp = temp | 1<<3 ;
	writel(temp,rGUSB2PHYCFG);
	DBG_USBD3("2.Default GUSB2PHYCFG = 0x%x\n",readl(rGUSB2PHYCFG));


	reg_gctl = readl(rGCTL);
	DBG_USBD3("Default GCTL = 0x%x\n",reg_gctl);
	reg_gctl &= ~(SHIKHARA_USB3_GCTL_PwrDnScale_MASK |
		SHIKHARA_USB3_GCTL_DisScramble);
	writel(reg_gctl,rGCTL);			// Disable Powerdown 
	
	DBG_USBD3("Default GUSB2PHYCFG = 0x%x\n",readl(rGUSB2PHYCFG));
	usbdev3_gusb2phycfg.data = readl(rGUSB2PHYCFG);
	usbdev3_gusb2phycfg.b.suspend_usb2_phy = 0;
	usbdev3_gusb2phycfg.b.enable_sleep_n = 0;
	writel(usbdev3_gusb2phycfg.data, rGUSB2PHYCFG);		// Disable USB2.0 PHY suspend

	DBG_USBD3("Default GUSB3PIPECTL = 0x%x\n",readl(rGUSB3PIPECTL));
	usbdev3_gusb3pipectl.data = readl(rGUSB3PIPECTL);
	usbdev3_gusb3pipectl.b.suspend_usb3_ss_phy = 0;
	writel(usbdev3_gusb3pipectl.data, rGUSB3PIPECTL);	// Disable USB3.0 PIPE suspend

	reg_val = readl(rGCTL);
	reg_val &= ~(DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG));
	reg_val |= DWC3_GCTL_PRTCAPDIR(2);
	writel(reg_val, rGCTL);			// Keeping the DRD in Device mode

	if (shikhara_usb_init_core(eSpeed))
	{
		DBG_USBD3("Shikhara USB3 Core Init Fail\n");
		return 0;
	}

	return 0;
}
