#include <common.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <malloc.h>
#include <watchdog.h>
#include <asm/cache.h>
#include <asm-generic/errno.h>
#include <linux/list.h>
#include <command.h>
#include <asm/errno.h>
#define MAP_NOCACHE     (0)
#include <asm/processor.h>
#include <linux/compiler.h>
#include <linux/ctype.h>
#include <asm/unaligned.h>
#include <part.h>
#include <linux/types.h>	/* __u8 etc */

struct usb_descriptor_header {
	__u8  bLength;
	__u8  bDescriptorType;
} __attribute__ ((packed));


struct usb_ss_ep_comp_descriptor {
        u8  bLength;
        u8  bDescriptorType;    /* 0x30 */
        u8  bMaxBurst;
        u8  bmAttributes;
        u16 wBytesPerInterval;
} __attribute__ ((packed));

struct usb_endpoint_descriptor {
	u8 bLength;
	u8 bDescriptorType;	/* 0x5 */
	u8 bEndpointAddress;
	u8 bmAttributes;
	u16 wMaxPacketSize;
	u8 bInterval;
} __attribute__ ((packed));

struct usb_interface_descriptor {
	u8 bLength;
	u8 bDescriptorType;	/* 0x04 */
	u8 bInterfaceNumber;
	u8 bAlternateSetting;
	u8 bNumEndpoints;
	u8 bInterfaceClass;
	u8 bInterfaceSubClass;
	u8 bInterfaceProtocol;
	u8 iInterface;
} __attribute__ ((packed));

struct usb_configuration_descriptor {
	u8 bLength;
	u8 bDescriptorType;	/* 0x2 */
	u16 wTotalLength;
	u8 bNumInterfaces;
	u8 bConfigurationValue;
	u8 iConfiguration;
	u8 bmAttributes;
	u8 bMaxPower;
} __attribute__ ((packed));

struct usb_device_descriptor {
	u8 bLength;
	u8 bDescriptorType;	/* 0x01 */
	u16 bcdUSB;
	u8 bDeviceClass;
	u8 bDeviceSubClass;
	u8 bDeviceProtocol;
	u8 bMaxPacketSize0;
	u16 idVendor;
	u16 idProduct;
	u16 bcdDevice;
	u8 iManufacturer;
	u8 iProduct;
	u8 iSerialNumber;
	u8 bNumConfigurations;
} __attribute__ ((packed));

#if ARCH_DMA_MINALIGN > 32
#define USB_DMA_MINALIGN	ARCH_DMA_MINALIGN
#else
#define USB_DMA_MINALIGN	32
#endif

#define USB_MAX_DEVICE			32
#define USB_MAXINTERFACES		8
#define USB_MAXENDPOINTS		16
#define USB_MAXCHILDREN			8	/* This is arbitrary */
#define USB_MAX_HUB			16

#define USB_CNTL_TIMEOUT 100 /* 100ms timeout */

struct devrequest {
	unsigned char	requesttype;
	unsigned char	request;
	unsigned short	value;
	unsigned short	index;
	unsigned short	length;
} __attribute__ ((packed));

struct usb_ep_desc {
        struct usb_endpoint_descriptor          ep_desc;
        /*
         * Super Speed Device will have Super Speed Endpoint
         * Companion Descriptor  (section 9.6.7 of usb 3.0 spec)
         * Revision 1.0 June 6th 2011
         */
        struct usb_ss_ep_comp_descriptor        ss_ep_comp;
};

/* Interface */
struct usb_interface {
	struct usb_interface_descriptor desc;

	unsigned char	no_of_ep;
	unsigned char	num_altsetting;
	unsigned char	act_altsetting;
	struct usb_ep_desc ep_desc[USB_MAXENDPOINTS];
} __attribute__ ((packed));

/* Configuration information.. */
struct usb_config {
	struct usb_configuration_descriptor desc;

	unsigned char	no_of_if;	/* number of interfaces */
	struct usb_interface if_desc[USB_MAXINTERFACES];
} __attribute__ ((packed));

enum {
	/* Maximum packet size; encoded as 0,1,2,3 = 8,16,32,64 */
	PACKET_SIZE_8   = 0,
	PACKET_SIZE_16  = 1,
	PACKET_SIZE_32  = 2,
	PACKET_SIZE_64  = 3,
};

struct usb_device {
	int	devnum;			/* Device number on USB bus */
	int	speed;			/* full/low/high */
	char	mf[32];			/* manufacturer */
	char	prod[32];		/* product */
	char	serial[32];		/* serial number */

	/* Maximum packet size; one of: PACKET_SIZE_* */
	int maxpacketsize;
	/* one bit for each endpoint ([0] = IN, [1] = OUT) */
	unsigned int toggle[2];
	/* endpoint halts; one bit per endpoint # & direction;
	 * [0] = IN, [1] = OUT
	 */
	unsigned int halted[2];
	int epmaxpacketin[16];		/* INput endpoint specific maximums */
	int epmaxpacketout[16];		/* OUTput endpoint specific maximums */

	int configno;			/* selected config number */
	/* Device Descriptor */
	struct usb_device_descriptor descriptor
		__attribute__((aligned(ARCH_DMA_MINALIGN)));
	struct usb_config config; /* config descriptor */

	int have_langid;		/* whether string_langid is valid yet */
	int string_langid;		/* language ID for strings */
	int (*irq_handle)(struct usb_device *dev);
	unsigned long irq_status;
	int irq_act_len;		/* transfered bytes */
	void *privptr;
	/*
	 * Child devices -  if this is a hub device
	 * Each instance needs its own set of data structures.
	 */
	unsigned long status;
	int act_len;			/* transfered bytes */
	int maxchild;			/* Number of ports if hub */
	int portnr;
	struct usb_device *parent;
	struct usb_device *children[USB_MAXCHILDREN];
	void *controller;
};

#if defined(CONFIG_USB_UHCI) || defined(CONFIG_USB_OHCI) || \
	defined(CONFIG_USB_EHCI) || defined(CONFIG_USB_OHCI_NEW) || \
	defined(CONFIG_USB_SL811HS) || defined(CONFIG_USB_ISP116X_HCD) || \
	defined(CONFIG_USB_R8A66597_HCD) || defined(CONFIG_USB_DAVINCI) || \
	defined(CONFIG_USB_OMAP3) || defined(CONFIG_USB_DA8XX) || \
	defined(CONFIG_USB_BLACKFIN) || defined(CONFIG_USB_AM35X) || \
	defined(CONFIG_USB_XHCI) || defined(CONFIG_CMD_USB_MINTEST)

int usb3_lowlevel_init(int index, void **controller);
int usb3_lowlevel_stop(int index);
int submit3_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,int transfer_len, struct devrequest *setup);

#endif

int usb3_init(void); /* initialize the USB Controller */
int usb3_stop(void); /* stop the USB Controller */


int usb3_set_protocol(struct usb_device *dev, int ifnum, int protocol);
int usb3_set_idle(struct usb_device *dev, int ifnum, int duration,int report_id);
struct usb_device *usb3_get_dev_index(int index);
int usb3_control_msg(struct usb_device *dev, unsigned int pipe,unsigned char request, unsigned char requesttype,
			unsigned short value, unsigned short index,void *data, unsigned short size, int timeout);
int usb3_maxpacket(struct usb_device *dev, unsigned long pipe);
int usb3_get_configuration_no(struct usb_device *dev, unsigned char *buffer,int cfgno);
int usb3_get_report(struct usb_device *dev, int ifnum, unsigned char type,unsigned char id, void *buf, int size);
int usb3_get_class_descriptor(struct usb_device *dev, int ifnum,unsigned char type, unsigned char id, void *buf,
			int size);
int usb3_clear_halt(struct usb_device *dev, int pipe);
int usb3_string(struct usb_device *dev, int index, char *buf, size_t size);
int usb3_set_interface(struct usb_device *dev, int interface, int alternate);

#define create_pipe(dev,endpoint) \
		(((dev)->devnum << 8) | ((endpoint) << 15) | \
		((dev)->speed << 26) | (dev)->maxpacketsize)
#define default_pipe(dev) ((dev)->speed << 26)

#define usb_sndctrlpipe(dev, endpoint)	((2 << 30) | \
					 create_pipe(dev, endpoint))
#define usb_rcvctrlpipe(dev, endpoint)	((2 << 30) | \
					 create_pipe(dev, endpoint) | \
					 0x80)
#define usb_snddefctrl(dev)		((2 << 30) | \
					 default_pipe(dev))
#define usb_rcvdefctrl(dev)		((2 << 30) | \
					 default_pipe(dev) | \
					 0x80)

#define usb_settoggle(dev, ep, out, bit) ((dev)->toggle[out] = \
						((dev)->toggle[out] & \
						 ~(1 << ep)) | ((bit) << ep))

#define usb_endpoint_running(dev, ep, out) ((dev)->halted[out] &= ~(1 << (ep)))
#define usb_pipeout(pipe)	((((pipe) >> 7) & 1) ^ 1)
#define usb_pipein(pipe)	(((pipe) >> 7) & 1)
#define usb_pipedevice(pipe)	(((pipe) >> 8) & 0x7f)
#define usb_pipeendpoint(pipe)	(((pipe) >> 15) & 0xf)
#define usb_pipetype(pipe)	(((pipe) >> 30) & 3)
#define usb_pipecontrol(pipe)	(usb_pipetype((pipe)) == 2)

struct usb_port_status {
	unsigned short wPortStatus;
	unsigned short wPortChange;
} __attribute__ ((packed));

struct usb_hub_status {
	unsigned short wHubStatus;
	unsigned short wHubChange;
} __attribute__ ((packed));

struct usb_hub_descriptor {
	unsigned char  bLength;
	unsigned char  bDescriptorType;
	unsigned char  bNbrPorts;
	unsigned short wHubCharacteristics;
	unsigned char  bPwrOn2PwrGood;
	unsigned char  bHubContrCurrent;
	unsigned char  DeviceRemovable[(USB_MAXCHILDREN+1+7)/8];
	unsigned char  PortPowerCtrlMask[(USB_MAXCHILDREN+1+7)/8];
} __attribute__ ((packed));


struct usb_hub_device {
	struct usb_device *pusb_dev;
	struct usb_hub_descriptor desc;
};

int usb3_hub_probe(struct usb_device *dev, int ifnum);
void usb3_hub_reset(void);
int hub3_port_reset(struct usb_device *dev, int port, unsigned short *portstat);
struct usb_device *usb3_alloc_new_device(void *controller);
int usb3_new_device(struct usb_device *dev);
char *usb3_get_class_desc(unsigned char dclass);

#define USB_BUFSIZ	512

static struct usb_hub_device hub_dev[USB_MAX_HUB];
static int usb_hub_index;

#define DWC3_GCTL_PRTCAPDIR(n)  ((n) << 12)
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS	4
#define ENABLE_DEV_NOTE(x)	(1 << (x))
#define	DEV_NOTE_FWAKE		ENABLE_DEV_NOTE(1)
#define DEV_SPEED_MASK		(0xf << 10)
#define	XDEV_FS			(0x1 << 10)
#define	XDEV_LS			(0x2 << 10)
#define	XDEV_HS			(0x3 << 10)
#define	XDEV_SS			(0x4 << 10)
#define DEV_FULLSPEED(p)	(((p) & DEV_SPEED_MASK) == XDEV_FS)
#define DEV_LOWSPEED(p)		(((p) & DEV_SPEED_MASK) == XDEV_LS)
#define DEV_HIGHSPEED(p)	(((p) & DEV_SPEED_MASK) == XDEV_HS)
#define DEV_SUPERSPEED(p)	(((p) & DEV_SPEED_MASK) == XDEV_SS)
#define	SLOT_SPEED_FS		(XDEV_FS << 10)
#define	SLOT_SPEED_LS		(XDEV_LS << 10)
#define	SLOT_SPEED_HS		(XDEV_HS << 10)
#define	SLOT_SPEED_SS		(XDEV_SS << 10)
#define	ER_IRQ_CLEAR(p)		((p) & 0xfffffffe)
#define	ER_IRQ_ENABLE(p)	((ER_IRQ_CLEAR(p)) | 0x2)
#define ER_IRQ_INTERVAL_MASK	(0xffff)
#define DB_VALUE(ep, stream)	((((ep) + 1) & 0xff) | ((stream) << 16))
#define LAST_CTX(p)	((p) << 27)
#define MAX_PACKET(p)	(((p)&0xffff) << 16)
#define TRB_INTR_TARGET(p)	(((p) & 0x3ff) << 22)
#define	TRB_TO_EP_ID(p)	(((p) >> 16) & 0x1f)
#define	COMP_CODE_SHIFT		(24)
#define GET_COMP_CODE(p)	(((p) & (0xff << 24)) >> 24)
#define TRB_TYPE(p)		((p) << 10)
#define TRB_FIELD_TO_TYPE(p)	(((p) & (0xfc00)) >> 10)
#define TRB_TYPE_LINK(x)	(((x) & (0xfc00)) == TRB_TYPE(TRB_LINK))
#define TRB_TYPE_LINK_LE32(x)	(((x) & cpu_to_le32((0xfc00))) == \
				 cpu_to_le32(TRB_TYPE(TRB_LINK)))
#define NEC_FW_MINOR(p)		(((p) >> 0) & 0xff)
#define NEC_FW_MAJOR(p)		(((p) >> 8) & 0xff)

#define CTX_SIZE(_hcc) (HCC_64BYTE_CONTEXT(_hcc) ? 64 : 32)
#define XHCI_HC_LENGTH(p)	(((p) >> 00) & 0x00ff)
#define XHCI_CMD_EIE		(1 << 2)
#define XHCI_CMD_HSEIE		(1 << 3)
#define XHCI_CMD_EWE		(1 << 10)

#define XHCI_IRQS		(XHCI_CMD_EIE | XHCI_CMD_HSEIE | XHCI_CMD_EWE)
#define ANUSOC_USB3_GCTL                0xC110
#define ANUSOC_USB3_GCTL_PwrDnScale_MASK                (0x1fff << 19)
#define ANUSOC_USB3_GCTL_U2RSTECN                       (1 << 16)
#define ANUSOC_USB3_GCTL_CoreSoftReset                  (1 << 11)
#define ANUSOC_USB3_GCTL_DisScramble                    (1 << 3)
#define ANUSOC_USB3_GCTL_SsPwrClmp                      (1 << 2)
#define ANUSOC_USB3_GUSB2PHYCFG(_a)     (0xC200 + ((_a) * 0x04))
#define ANUSOC_USB3_GUSB2PHYCFGx_PHYSoftRst             (1 << 31)
#define ANUSOC_USB3_GUSB3PIPECTL(_a)    (0xC2C0 + ((_a) * 0x04))
#define ANUSOC_USB3_GUSB3PIPECTLx_PHYSoftRst            (1 << 31)
struct anusoc_drd {
        void             *regs;
};
#define CONFIG_USB_MAX_CONTROLLER_COUNT 1

#define XHCI_TRB 0xC0DEABCD
#define XHCI_CTX 0xCAFEABCD

#define DeviceRequest \
	((0x80 | (0x00 << 5) | 0x00) << 8)

#define DeviceOutRequest \
	((0 | (0x00 << 5) | 0x00) << 8)

#define upper_32_bits(n) (u32)(((n) >> 32) & 0xffffffff)
#define lower_32_bits(n) (u32)(n)

#define MAX_EP_CTX_NUM		31
#define USB_CTRL_SET_TIMEOUT	5000
#define XHCI_ALIGNMENT		64
#define MAX_HC_SLOTS            256

#define XHCI_MAX_HALT_USEC	(16*1000)

#define XHCI_MAX_RESET_USEC	(250*1000)

#define XHCI_PORT_RO ((1 << 0) | (1 << 3) | (0xf << 10) | (1 << 30))
#define XHCI_PORT_RWS ((0xf << 5) | (1 << 9) | (0x3 << 14) | (0x7 << 25))
typedef enum { false = 0, true = 1 } bool;

/* ---------------- Host -----------------------------*/
static struct usb_device usb_dev[USB_MAX_DEVICE];
static int dev_index;
static int running;
static int asynch_allowed;
static inline char *portspeed(int speed);
char usb3_started; /* flag for the started/stopped USB status */

static void usb_scan_devices(void *ctrl);

struct xhci_hccr {
	uint32_t cr_capbase;
	uint32_t cr_hcsparams1;
	uint32_t cr_hcsparams2;
	uint32_t cr_hcsparams3;
	uint32_t cr_hccparams;
	uint32_t cr_dboff;
	uint32_t cr_rtsoff;

#define HC_LENGTH(p)		XHCI_HC_LENGTH(p)
#define HC_VERSION(p)		(((p) >> 16) & 0xffff)
#define HCS_SLOTS_MASK		0xff
#define HCS_MAX_PORTS_SHIFT	24
#define HCS_MAX_PORTS_MASK	(0x7f << HCS_MAX_PORTS_SHIFT)

#define HCC_64BYTE_CONTEXT(p)	((p) & (1 << 2))
#define HCC_PPC(p)		((p) & (1 << 3))
#define HCS_INDICATOR(p)	((p) & (1 << 4))
#define	DBOFF_MASK	(~0x3)
#define	RTSOFF_MASK	(~0x1f)

};

struct xhci_hcor_portRegss {
	volatile uint32_t or_portsc;
	volatile uint32_t or_portpmsc;
	volatile uint32_t or_portli;
	volatile uint32_t reserved_3;
};

struct xhci_hcor {
	volatile uint32_t or_usbcmd;
	volatile uint32_t or_usbsts;
	volatile uint32_t or_pagesize;
	volatile uint32_t reserved_0[2];
	volatile uint32_t or_dnctrl;
	volatile uint64_t or_crcr;
	volatile uint32_t reserved_1[4];
	volatile uint64_t or_dcbaap;
	volatile uint32_t or_config;
	volatile uint32_t reserved_2[241];
	struct xhci_hcor_portRegss PortRegs[CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS];

	uint32_t reserved_4[CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS*254];
};
struct xhci_intr_reg {
	volatile __le32	irq_pending;
	volatile __le32	irq_control;
	volatile __le32	erst_size;
	volatile __le32	rsvd;
	volatile __le64	erst_base;
	volatile __le64	erst_dequeue;
};


struct xhci_run_regs {
	__le32			microframe_index;
	__le32			rsvd[7];
	struct xhci_intr_reg	ir_set[128];
};

struct xhci_doorbell_array {
	volatile __le32	doorbell[256];
};


struct xhci_protocol_caps {
	u32	revision;
	u32	name_string;
	u32	port_info;
};

struct xhci_container_ctx {
	unsigned type;
#define XHCI_CTX_TYPE_DEVICE  0x1
#define XHCI_CTX_TYPE_INPUT   0x2

	int size;
	u8 *bytes;
};

struct xhci_slot_ctx {
	__le32	dev_info;
	__le32	dev_info2;
	__le32	tt_info;
	__le32	dev_state;
	__le32	reserved[4];
};

struct xhci_ep_ctx {
	__le32	ep_info;
	__le32	ep_info2;
	__le64	deq;
	__le32	tx_info;
	__le32	reserved[3];
};


struct xhci_input_control_ctx {
	volatile __le32	drop_flags;
	volatile __le32	add_flags;
	__le32	rsvd2[6];
};

struct xhci_device_context_array {
	__le64			dev_context_ptrs[MAX_HC_SLOTS];
};

struct xhci_transfer_event {
	__le64	buffer;
	__le32	transfer_len;
	volatile __le32	flags;
};


typedef enum {
	COMP_SUCCESS = 1,
	COMP_TX_ERR, /* 4 */
	COMP_TRB_ERR, /* 5 */
	COMP_ENOMEM, /* 7 */
	COMP_BW_ERR, /* 8 */
	COMP_EBADSLT, /* 11 */
	COMP_EINVAL, /* 17 */
	COMP_CTX_STATE,/* 19 */
	COMP_DEV_ERR,/* 22 */
	COMP_MEL_ERR,/* 29 */
	COMP_2ND_BW_ERR, /* 35 */

} xhci_comp_code;

struct xhci_link_trb {
	volatile __le64 segment_ptr;
	volatile __le32 intr_target;
	volatile __le32 control;
};


struct xhci_event_cmd {
	volatile __le64 cmd_trb;
	volatile __le32 status;
	volatile __le32 flags;
};

struct xhci_generic_trb {
	volatile __le32 field[4];
};

union xhci_trb {
	struct xhci_link_trb		link;
	struct xhci_transfer_event	trans_event;
	struct xhci_event_cmd		event_cmd;
	struct xhci_generic_trb		generic;
};


typedef enum {
	TRB_SETUP=2, /* 2 */
	TRB_DATA=3, /* 3 */
	TRB_STATUS=4, /* 4 */
	TRB_LINK=6, /* 6 */
	TRB_ENABLE_SLOT=9, /* 9 */
	TRB_ADDR_DEV=11, /* 11 */
	TRB_CONFIG_EP=12, /* 12 */
	TRB_EVAL_CONTEXT=13, /* 13 */
	TRB_RESET_DEV=17, /* 17 */
	TRB_COMPLETION=33, /* 33 */
	TRB_PORT_STATUS=34, /* 34 */
} trb_type;


struct xhci_segment {
	union xhci_trb		*trbs;
	struct xhci_segment	*next;
};

struct xhci_td {
	struct list_head	td_list;
	struct list_head	cancelled_td_list;
	struct urb		*urb;
	struct xhci_segment	*start_seg;
	union xhci_trb		*first_trb;
	union xhci_trb		*last_trb;
};

struct xhci_dequeue_state {
	struct xhci_segment *new_deq_seg;
	union xhci_trb *new_deq_ptr;
	volatile int new_cycle_state;
};

struct xhci_ring {
	struct xhci_segment	*first_seg;
	union  xhci_trb		*enqueue;
	struct xhci_segment	*enq_seg;
	unsigned int		enq_updates;
	union  xhci_trb		*dequeue;
	struct xhci_segment	*deq_seg;
	unsigned int		deq_updates;
	struct list_head	td_list;
	volatile u32			cycle_state;
	volatile unsigned int		stream_id;
	bool			last_td_was_short;
};

struct xhci_erst_entry {
	__le64	seg_addr;
	__le32	seg_size;
	__le32	rsvd;
};

struct xhci_erst {
	struct xhci_erst_entry	*entries;
	unsigned int		num_entries;
	unsigned int		erst_size;
};

struct xhci_scratchpad {
	u64 *sp_array;
	void **sp_buffers;
};

struct urb_priv {
	int	length;
	int	td_cnt;
	struct	xhci_td	*td[0];
};


struct xhci_virt_ep {
	struct xhci_ring		*ring;
	struct xhci_stream_info		*stream_info;
	struct xhci_ring		*new_ring;
	unsigned int			ep_state;
#define SET_DEQ_PENDING		(1 << 0)
#define EP_HALTED		(1 << 1)	
#define EP_HALT_PENDING		(1 << 2)	
#define EP_GETTING_STREAMS	(1 << 3)
#define EP_HAS_STREAMS		(1 << 4)
#define EP_GETTING_NO_STREAMS	(1 << 5)
	struct list_head	cancelled_td_list;
	union xhci_trb		*stopped_trb;
	struct xhci_td		*stopped_td;
	unsigned int		stopped_stream;
	int			stop_cmds_pending;
	struct xhci_segment	*queued_deq_seg;
	union xhci_trb		*queued_deq_ptr;
	bool			skip;
};


struct xhci_virt_device {
	struct usb_device		*udev;
	struct xhci_container_ctx       *out_ctx;
	struct xhci_container_ctx       *in_ctx;
	int				address;
#define	XHCI_MAX_RINGS_CACHED	31
	struct xhci_virt_ep		eps[31];
	xhci_comp_code			cmd_status;
};

struct usb_linux_interface_descriptor {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned char	bInterfaceNumber;
	unsigned char	bAlternateSetting;
	unsigned char	bNumEndpoints;
	unsigned char	bInterfaceClass;
	unsigned char	bInterfaceSubClass;
	unsigned char	bInterfaceProtocol;
	unsigned char	iInterface;
} __attribute__ ((packed));

struct usb_linux_config_descriptor {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned short	wTotalLength;
	unsigned char	bNumInterfaces;
	unsigned char	bConfigurationValue;
	unsigned char	iConfiguration;
	unsigned char	bmAttributes;
	unsigned char	MaxPower;
} __attribute__ ((packed));

static inline unsigned int xhci_readl(uint32_t volatile *regs)
{
	return readl(regs);
}

static inline void xhci_writel(uint32_t volatile *regs, const unsigned int val)
{
	writel(val, regs);
}

static inline u64 xhci_readl_64(__le64 volatile *regs)
{
	__u32 *ptr = (__u32 *) regs;
	u64 val_lo = readl(ptr);
	u64 val_hi = readl(ptr + 1);
	return val_lo + (val_hi << 32);
}

static inline void xhci_writel_64(__le64 volatile *regs, const u64 val)
{
	__u32 *ptr = (__u32 *) regs;
	u32 val_lo = lower_32_bits(val);
	u32 val_hi = 0;
	writel(val_lo, ptr);
	writel(val_hi, ptr + 1);
}

int xhci3_hcd_init(int index, struct xhci_hccr **ret_hccr,struct xhci_hcor **ret_hcor);
void xhci_hcd_stop(int index);

struct xhci_ctrl {
	struct xhci_hccr *hccr;	/* R/O registers, not need for volatile */
	struct xhci_hcor *hcor;
	struct xhci_doorbell_array *dba;
	struct xhci_run_regs *run_regs;
	struct xhci_device_context_array *dcbaa		\
			__attribute__ ((aligned(ARCH_DMA_MINALIGN)));
	struct xhci_ring *event_ring;
	struct xhci_ring *cmd_ring;
	struct xhci_ring *transfer_ring;
	struct xhci_segment *seg;
	struct xhci_intr_reg *ir_set;
	struct xhci_erst erst;
	struct xhci_erst_entry entry[3];
	union xhci_trb trb;
	struct xhci_virt_device *devs[MAX_HC_SLOTS];
	int rootdev3;
	int slot_id;
	int port_id;
	uint16_t portreset;
	u8 *port_array;
	int speed;
	unsigned int handle_portStatus_Flag;
};

unsigned long trb3_addr(struct xhci_segment *seg, union xhci_trb *trb);
int xhci3_address_device(struct usb_device *usbdev);
void xhci3_poll_and_HandleEvent(struct usb_device *usbdev);
int xhci3_alloc_dev(struct usb_device *usbdev);
struct xhci_input_control_ctx *xhci3_get_input_control_ctx(struct xhci_container_ctx *ctx);
void xhci3_slot_copy(struct xhci_ctrl *ctrl,struct xhci_container_ctx *in_ctx,struct xhci_container_ctx *out_ctx);
struct xhci_slot_ctx *xhci3_get_slot_ctx(struct xhci_ctrl *ctrl,struct xhci_container_ctx *ctx);
void xhci3_endpoint_copy(struct xhci_ctrl *ctrl, struct xhci_container_ctx *in_ctx,
				struct xhci_container_ctx *out_ctx,unsigned int ep_index);
	
int usb3_init(void)
{
	int result;
	void *ctrl;
	int i = 0;
	running = 0;
	dev_index = 0;
	asynch_allowed = 1;
	usb3_hub_reset();
	printf("USB:   ");
	result = usb3_lowlevel_init(i,&ctrl);
	if (result == 0) {
		printf("scanning bus for devices... ");
		running = 1;
		usb_scan_devices(ctrl);
		usb3_started = 1;
		return 0;
	} else {
		printf("Error, couldn't init Lowlevel part\n");
		usb3_started = 0;
		return -1;
	}
}

int usb3_stop(void)
{
	int res = 0;
	int i = 0;
	if (usb3_started) {
		asynch_allowed = 1;
		usb3_started = 0;
		usb3_hub_reset();
		res = usb3_lowlevel_stop(i);
	}
	return res;
}

int usb3_control_msg(struct usb_device *dev, unsigned int pipe,
			unsigned char request, unsigned char requesttype,
			unsigned short value, unsigned short index,
			void *data, unsigned short size, int timeout)
{
	ALLOC_CACHE_ALIGN_BUFFER(struct devrequest, setup_packet, 1);

	if ((timeout == 0) && (!asynch_allowed)) {
		return -1;
	}

	setup_packet->requesttype = requesttype;
	setup_packet->request = request;
	setup_packet->value = cpu_to_le16(value);
	setup_packet->index = cpu_to_le16(index);
	setup_packet->length = cpu_to_le16(size);
	dev->status = 0x80000000L; /*not yet processed */

	if (submit3_control_msg(dev, pipe, data, size, setup_packet) < 0)
		return -1;
	if (timeout == 0)
		return (int)size;

	while (timeout--) {
		if (!((volatile unsigned long)dev->status & 0x80000000L))
			break;
		mdelay(1);
	}
	if (dev->status)
		return -1;

	return dev->act_len;

}

int usb3_maxpacket(struct usb_device *dev, unsigned long pipe)
{
	if ((pipe & 0x80) == 0)
		return dev->epmaxpacketout[((pipe>>15) & 0xf)];
	else
		return dev->epmaxpacketin[((pipe>>15) & 0xf)];
}

static void noinline
usb3_set_maxpacket_ep(struct usb_device *dev, int if_idx, int ep_idx)
{
	int b;
	struct usb_endpoint_descriptor *ep;
	u16 ep_wMaxPacketSize;

	ep = &dev->config.if_desc[if_idx].ep_desc[ep_idx].ep_desc;

	b = ep->bEndpointAddress & (0x0f);
	ep_wMaxPacketSize = get_unaligned(&ep->wMaxPacketSize);

	if ((ep->bmAttributes & 0x03) == 0) {
		dev->epmaxpacketout[b] = ep_wMaxPacketSize;
		dev->epmaxpacketin[b] = ep_wMaxPacketSize;
	} else {
		if ((ep->bEndpointAddress & 0x80) == 0) {
			if (ep_wMaxPacketSize > dev->epmaxpacketout[b]) {
				dev->epmaxpacketout[b] = ep_wMaxPacketSize;
			}
		} else {
			if (ep_wMaxPacketSize > dev->epmaxpacketin[b]) {
				dev->epmaxpacketin[b] = ep_wMaxPacketSize;
			}
		} /* if out */
	} /* if control */
}

static int usb_set_maxpacket(struct usb_device *dev)
{
	int i, ii;

	for (i = 0; i < dev->config.desc.bNumInterfaces; i++)
		for (ii = 0; ii < dev->config.if_desc[i].desc.bNumEndpoints; ii++)
			usb3_set_maxpacket_ep(dev, i, ii);

	return 0;
}

static int usb_parse_config(struct usb_device *dev,
			unsigned char *buffer, int cfgno)
{
	struct usb_descriptor_header *head;
	int index, ifno, epno, curr_if_num;
	
	u16 ep_wMaxPacketSize;
	struct usb_interface *if_desc = NULL;
	ifno = -1;
	epno = -1;
	curr_if_num = -1;

	dev->configno = cfgno;
	head = (struct usb_descriptor_header *) &buffer[0];
	if (head->bDescriptorType != (0x02)) {
		printf(" ERROR: NOT USB_CONFIG_DESC %x\n",
			head->bDescriptorType);
		return -1;
	}
	memcpy(&dev->config, buffer, buffer[0]);
	le16_to_cpus(&(dev->config.desc.wTotalLength));
	dev->config.no_of_if = 0;

	index = dev->config.desc.bLength;
	head = (struct usb_descriptor_header *) &buffer[index];
	while (index + 1 < dev->config.desc.wTotalLength) {
		switch (head->bDescriptorType) {
		case 0x04:
			if (((struct usb_interface_descriptor *) \
			     &buffer[index])->bInterfaceNumber != curr_if_num) {
				ifno = dev->config.no_of_if;
				dev->config.no_of_if++;
				memcpy(&dev->config.if_desc[ifno], &buffer[index], buffer[index]);
				dev->config.if_desc[ifno].no_of_ep = 0;
				dev->config.if_desc[ifno].num_altsetting = 1;
				curr_if_num = dev->config.if_desc[ifno].desc.bInterfaceNumber;
			} else {
				dev->config.if_desc[ifno].num_altsetting++;
			}
			break;
		case (0x05):
			epno = dev->config.if_desc[ifno].no_of_ep;
			if_desc = &dev->config.if_desc[ifno];
			if_desc->no_of_ep++;
                        memcpy(&if_desc->ep_desc[epno].ep_desc, &buffer[index], buffer[index]);

			ep_wMaxPacketSize = get_unaligned(&dev->config.if_desc[ifno].ep_desc[epno].\
							ep_desc.wMaxPacketSize);
			put_unaligned(le16_to_cpu(ep_wMaxPacketSize),&dev->config.if_desc[ifno].ep_desc[epno].\
					ep_desc.wMaxPacketSize);
			break;
		case 0x30:
                        if_desc = &dev->config.if_desc[ifno];
                        memcpy(&(if_desc->ep_desc[epno].ss_ep_comp), &buffer[index], buffer[index]);
                        break;

		default:
			if (head->bLength == 0)
				return 1;
			break;
		}
		index += head->bLength;
		head = (struct usb_descriptor_header *)&buffer[index];
	}
	return 1;
}

int usb3_clear_halt(struct usb_device *dev, int pipe)
{
	int result;
	int endp = usb_pipeendpoint(pipe)|(usb_pipein(pipe)<<7);

	result = usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),
				 (0x01), (0x02), 0,
				 endp, NULL, 0, USB_CNTL_TIMEOUT * 3);

	if (result < 0)
		return result;

	usb_endpoint_running(dev, usb_pipeendpoint(pipe), usb_pipeout(pipe));
	usb_settoggle(dev, usb_pipeendpoint(pipe), usb_pipeout(pipe), 0);
	return 0;
}
static int usb_get_descriptor(struct usb_device *dev, unsigned char type,unsigned char index, void *buf, int size)
{
	int res;
	res = usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0), (0x06), 0x80, (type << 8) + index, 0,
			buf, size, USB_CNTL_TIMEOUT);
	return res;
}

int usb3_get_configuration_no(struct usb_device *dev,
			     unsigned char *buffer, int cfgno)
{
	int result;
	unsigned int tmp;
	struct usb_configuration_descriptor *config;

	config = (struct usb_configuration_descriptor *)&buffer[0];
	result = usb_get_descriptor(dev, (0x02), cfgno, buffer, 9);
	if (result < 9) {
		if (result < 0)
			printf("unable to get descriptor, error %lX\n",
				dev->status);
		else
			printf("config descriptor too short " \
				"(expected %i, got %i)\n", 9, result);
		return -1;
	}
	tmp = le16_to_cpu(config->wTotalLength);

	if (tmp > USB_BUFSIZ) {
		return -1;
	}

	result = usb_get_descriptor(dev, (0x02), cfgno, buffer, tmp);
	return result;
}

static int usb_set_address(struct usb_device *dev)
{
	int res;

	res = usb3_control_msg(dev, usb_snddefctrl(dev), (0x05), 0, (dev->devnum), 0, NULL, 0, USB_CNTL_TIMEOUT);
	return res;
}

int usb3_set_interface(struct usb_device *dev, int interface, int alternate)
{
	struct usb_interface *if_face = NULL;
	int ret, i;

	for (i = 0; i < dev->config.desc.bNumInterfaces; i++) {
		if (dev->config.if_desc[i].desc.bInterfaceNumber == interface) {
			if_face = &dev->config.if_desc[i];
			break;
		}
	}
	if (!if_face) {
		printf("selecting invalid interface %d", interface);
		return -1;
	}
	if (if_face->num_altsetting == 1)
		return 0;

	ret = usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),
				(0x0B), (0x01),
				alternate, interface, NULL, 0,
				USB_CNTL_TIMEOUT * 5);
	if (ret < 0)
		return ret;

	return 0;
}
static int usb_set_configuration(struct usb_device *dev, int configuration)
{
	int res;
	res = usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),
				(0x09), 0,
				configuration, 0,
				NULL, 0, USB_CNTL_TIMEOUT);
	if (res == 0) {
		dev->toggle[0] = 0;
		dev->toggle[1] = 0;
		return 0;
	} else
		return -1;
}

int usb3_set_protocol(struct usb_device *dev, int ifnum, int protocol)
{
	return usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),
		0x0B, (0x01 << 5) | (0x01), protocol, ifnum, NULL, 0, USB_CNTL_TIMEOUT);
}
int usb3_set_idle(struct usb_device *dev, int ifnum, int duration, int report_id)
{
	return usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),
		0x0A, (0x01 << 5) | (0x01), (duration << 8) | report_id, ifnum, NULL, 0, USB_CNTL_TIMEOUT);
}

int usb3_get_report(struct usb_device *dev, int ifnum, unsigned char type,
		   unsigned char id, void *buf, int size)
{
	return usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0),
			0x01, 0x80 | (0x01 << 5) | (0x01),(type << 8) + id, ifnum, buf, size, USB_CNTL_TIMEOUT);
}

int usb3_get_class_descriptor(struct usb_device *dev, int ifnum,
		unsigned char type, unsigned char id, void *buf, int size)
{
	return usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		(0x06), (0x01) | 0x80,(type << 8) + id, ifnum, buf, size, USB_CNTL_TIMEOUT);
}

static int usb_get_string(struct usb_device *dev, unsigned short langid,
		   unsigned char index, void *buf, int size)
{
	int i;
	int result;

	for (i = 0; i < 3; ++i) {
		result = usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0), (0x06), 0x80,
			(0x03 << 8) + index, langid, buf, size,	USB_CNTL_TIMEOUT);

		if (result > 0)
			break;
	}

	return result;
}

int usb3_alloc_dev(struct usb_device *dev)
{
        int res;

        res = usb3_control_msg(dev, usb_sndctrlpipe(dev->parent, 0), 0XDE, 0, 0, 0, NULL, 0, USB_CNTL_TIMEOUT);

        return res;
}

static void usb_try_string_workarounds(unsigned char *buf, int *length)
{
	int newlength, oldlength = *length;

	for (newlength = 2; newlength + 1 < oldlength; newlength += 2)
		if (!isprint(buf[newlength]) || buf[newlength + 1])
			break;

	if (newlength > 2) {
		buf[0] = newlength;
		*length = newlength;
	}
}


static int usb_string_sub(struct usb_device *dev, unsigned int langid,
		unsigned int index, unsigned char *buf)
{
	int rc;
	rc = usb_get_string(dev, langid, index, buf, 255);

	if (rc < 2) {
		rc = usb_get_string(dev, langid, index, buf, 2);
		if (rc == 2)
			rc = usb_get_string(dev, langid, index, buf, buf[0]);
	}

	if (rc >= 2) {
		if (!buf[0] && !buf[1])
			usb_try_string_workarounds(buf, &rc);

		if (buf[0] < rc)
			rc = buf[0];

		rc = rc - (rc & 1); /* force a multiple of two */
	}

	if (rc < 2)
		rc = -1;

	return rc;
}

int usb3_string(struct usb_device *dev, int index, char *buf, size_t size)
{
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, mybuf, USB_BUFSIZ);
	unsigned char *tbuf;
	int err;
	unsigned int u, idx;

	if (size <= 0 || !buf || !index)
		return -1;
	buf[0] = 0;
	tbuf = &mybuf[0];

	if (!dev->have_langid) {
		err = usb_string_sub(dev, 0, 0, tbuf);
		if (err < 0) {
			return -1;
		} else if (tbuf[0] < 4) {
			return -1;
		} else {
			dev->have_langid = -1;
			dev->string_langid = tbuf[2] | (tbuf[3] << 8);
		}
	}

	err = usb_string_sub(dev, dev->string_langid, index, tbuf);
	if (err < 0)
		return err;

	size--;		/* leave room for trailing NULL char in output buffer */
	for (idx = 0, u = 2; u < err; u += 2) {
		if (idx >= size)
			break;
		if (tbuf[u+1])			/* high byte */
			buf[idx++] = '?';  /* non-ASCII character */
		else
			buf[idx++] = tbuf[u];
	}
	buf[idx] = 0;
	err = idx;
	return err;
}


struct usb_device *usb3_get_dev_index(int index)
{
	if (usb_dev[index].devnum == -1)
		return NULL;
	else
		return &usb_dev[index];
}

struct usb_device *usb3_alloc_new_device(void *ctrl)
{
	int i;
	if (dev_index == USB_MAX_DEVICE) {
		printf("ERROR, too many USB Devices, max=%d\n", USB_MAX_DEVICE);
		return NULL;
	}
	usb_dev[dev_index].devnum = dev_index + 1;
	usb_dev[dev_index].maxchild = 0;
	for (i = 0; i < USB_MAXCHILDREN; i++)
		usb_dev[dev_index].children[i] = NULL;
	usb_dev[dev_index].parent = NULL;
	usb_dev[dev_index].controller = ctrl;
	dev_index++;
	return &usb_dev[dev_index - 1];
}

char *usb3_get_class_desc(unsigned char dclass)
{
        switch (dclass) {
        case 0:
                return "See Interface";
        case 3:
                return "Human Interface";
        case 8:
                return "Mass Storage";
        case 9:
                return "Hub";
        case 0xff:
                return "Vendor specific";
        default:
                return "";
        }
}

int usb_new_dev(struct usb_device *dev){

        int addr, err;
        int tmp;
        ALLOC_CACHE_ALIGN_BUFFER(unsigned char, tmpbuf, USB_BUFSIZ);
	

        addr = dev->devnum;
        dev->devnum = 0;
        struct usb_device_descriptor *desc;
        int port = -1;
        struct usb_device *parent = dev->parent;
        

        desc = (struct usb_device_descriptor *)tmpbuf;
        dev->descriptor.bMaxPacketSize0 = 64;       /* Start off at 64 bytes  */
        dev->maxpacketsize = PACKET_SIZE_64;
        dev->epmaxpacketin[0] = 64;
        dev->epmaxpacketout[0] = 64;
	memset(desc,0,512);
        err = usb_get_descriptor(dev, (0x01), 0, desc, 64);
        if (err < 0) {
                return 1;
        }

	dev->descriptor.bMaxPacketSize0 = desc->bMaxPacketSize0;
        if (parent) {
                int j;

                for (j = 0; j < parent->maxchild; j++) {
                        if (parent->children[j] == dev) {
                                port = j;
                                break;
                        }
                }
                if (port < 0) {
                        return 1;
                }
	
	usb3_alloc_dev(dev);
	}
	dev->epmaxpacketin[0] = dev->descriptor.bMaxPacketSize0;
        dev->epmaxpacketout[0] = dev->descriptor.bMaxPacketSize0;
        switch (dev->descriptor.bMaxPacketSize0) {
        case 8:
                dev->maxpacketsize  = PACKET_SIZE_8;
                break;
        case 16:
                dev->maxpacketsize = PACKET_SIZE_16;
                break;
        case 32:
                dev->maxpacketsize = PACKET_SIZE_32;
                break;
        case 64:
                dev->maxpacketsize = PACKET_SIZE_64;
                break;
        }
	printf("\n Maxpacketsize---->%d\n",dev->maxpacketsize);
        dev->devnum = addr;
        err = usb_set_address(dev); /* set address */
        printf("\nset_address is %d ------------Done!\n",dev->devnum);

	if (err < 0) {
                printf("\n      USB device not accepting new address " \
                        "(error=%lX)\n", dev->status);
                return 1;
        }

        mdelay(10);     /* Let the SET_ADDRESS settle */

        tmp = sizeof(dev->descriptor);

        err = usb_get_descriptor(dev, (0x01), 0,
                                 tmpbuf, sizeof(dev->descriptor));
        if (err < tmp) {
                if (err < 0)
                        printf("unable to get device descriptor (error=%d)\n",
                               err);
                else
                        printf("USB device descriptor short read " \
                                "(expected %i, got %i)\n", tmp, err);
                return 1;
        }
        memcpy(&dev->descriptor, tmpbuf, sizeof(dev->descriptor));
        le16_to_cpus(&dev->descriptor.bcdUSB);
        le16_to_cpus(&dev->descriptor.idVendor);
        le16_to_cpus(&dev->descriptor.idProduct);
        le16_to_cpus(&dev->descriptor.bcdDevice);
        usb3_get_configuration_no(dev, tmpbuf, 0);
        usb_parse_config(dev, tmpbuf, 0);
        usb_set_maxpacket(dev);

        if (usb_set_configuration(dev, dev->config.desc.bConfigurationValue)) {
                return -1;
        }
        memset(dev->mf, 0, sizeof(dev->mf));
        memset(dev->prod, 0, sizeof(dev->prod));
        memset(dev->serial, 0, sizeof(dev->serial));
        if (dev->descriptor.iManufacturer)
                usb3_string(dev, dev->descriptor.iManufacturer,
                           dev->mf, sizeof(dev->mf));
        if (dev->descriptor.iProduct)
                usb3_string(dev, dev->descriptor.iProduct,
                           dev->prod, sizeof(dev->prod));
        if (dev->descriptor.iSerialNumber)
                usb3_string(dev, dev->descriptor.iSerialNumber,
                           dev->serial, sizeof(dev->serial));
        printf("Manufacturer %s\n", dev->mf);
        printf("Product      %s\n", dev->prod);
        printf("SerialNumber %s\n", dev->serial);
	 return 0;
}

int usb3_new_device(struct usb_device *dev)
{
	int addr, err;
	int tmp;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, tmpbuf, USB_BUFSIZ);

	addr = dev->devnum;
	dev->devnum = 0;

	struct usb_device_descriptor *desc;
	int port = -1;
	struct usb_device *parent = dev->parent;
        //unsigned short portstatus;

	desc = (struct usb_device_descriptor *)tmpbuf;
	dev->descriptor.bMaxPacketSize0 = 64;	    /* Start off at 64 bytes  */
	dev->maxpacketsize = PACKET_SIZE_64;
	dev->epmaxpacketin[0] = 64;
	dev->epmaxpacketout[0] = 64;

	err = usb_get_descriptor(dev, (0x01), 0, desc, 64);
	if (err < 0) {
		return 1;
	}
	dev->descriptor.bMaxPacketSize0 = desc->bMaxPacketSize0;

	if (parent) {
		int j;

		for (j = 0; j < parent->maxchild; j++) {
			if (parent->children[j] == dev) {
				port = j;
				break;
			}
		}
		if (port < 0) {
			return 1;
		}

                usb3_alloc_dev(dev);

	}

	dev->epmaxpacketin[0] = dev->descriptor.bMaxPacketSize0;
	dev->epmaxpacketout[0] = dev->descriptor.bMaxPacketSize0;
	switch (dev->descriptor.bMaxPacketSize0) {
	case 8:
		dev->maxpacketsize  = PACKET_SIZE_8;
		break;
	case 16:
		dev->maxpacketsize = PACKET_SIZE_16;
		break;
	case 32:
		dev->maxpacketsize = PACKET_SIZE_32;
		break;
	case 64:
		printf("max packet size is fixed to 64..........");
		dev->maxpacketsize = PACKET_SIZE_64;
		break;
	}
	dev->devnum = addr;

	err = usb_set_address(dev); /* set address */

	if (err < 0) {
		printf("\n      USB device not accepting new address " \
			"(error=%lX)\n", dev->status);
		return 1;
	}

	mdelay(10);	/* Let the SET_ADDRESS settle */

	tmp = sizeof(dev->descriptor);

	err = usb_get_descriptor(dev, (0x01), 0,
				 tmpbuf, sizeof(dev->descriptor));
	if (err < tmp) {
		if (err < 0)
			printf("unable to get device descriptor (error=%d)\n",
			       err);
		else
			printf("USB device descriptor short read " \
				"(expected %i, got %i)\n", tmp, err);
		return 1;
	}
	memcpy(&dev->descriptor, tmpbuf, sizeof(dev->descriptor));
	le16_to_cpus(&dev->descriptor.bcdUSB);
	le16_to_cpus(&dev->descriptor.idVendor);
	le16_to_cpus(&dev->descriptor.idProduct);
	le16_to_cpus(&dev->descriptor.bcdDevice);
	usb3_get_configuration_no(dev, tmpbuf, 0);
	usb_parse_config(dev, tmpbuf, 0);
	usb_set_maxpacket(dev);
	if (usb_set_configuration(dev, dev->config.desc.bConfigurationValue)) {
		printf("failed to set default configuration " \
			"len %d, status %lX\n", dev->act_len, dev->status);
		return -1;
	}
	memset(dev->mf, 0, sizeof(dev->mf));
	memset(dev->prod, 0, sizeof(dev->prod));
	memset(dev->serial, 0, sizeof(dev->serial));
	if (dev->descriptor.iManufacturer)
		usb3_string(dev, dev->descriptor.iManufacturer,dev->mf, sizeof(dev->mf));
	if (dev->descriptor.iProduct)
		usb3_string(dev, dev->descriptor.iProduct,dev->prod, sizeof(dev->prod));
	if (dev->descriptor.iSerialNumber)
		usb3_string(dev, dev->descriptor.iSerialNumber,dev->serial, sizeof(dev->serial));
	usb3_hub_probe(dev, 0);
	return 0;
}

static void usb_scan_devices(void *ctrl)
{
	int i;
	struct usb_device *dev;

	for (i = 0; i < USB_MAX_DEVICE; i++) {
		memset(&usb_dev[i], 0, sizeof(struct usb_device));
		usb_dev[i].devnum = -1;
	}
	dev_index = 0;
	dev = usb3_alloc_new_device(ctrl);
	if (usb3_new_device(dev))
		printf("No USB Device found\n");
	else
		printf("%d USB Device(s) found\n", dev_index);
}

static int usb_get_hub_descriptor(struct usb_device *dev, void *data, int size)
{
	return usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0),(0x06), 0x80 | ((0x01 << 5) | 0x00),
		((0x01 << 5) | 0x09) << 8, 0, data, size, USB_CNTL_TIMEOUT);
}

static int usb_clear_port_feature(struct usb_device *dev, int port, int feature)
{
	return usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),(0x01), ((0x01 << 5) | (0x03)), feature,
				port, NULL, 0, USB_CNTL_TIMEOUT);
}

static int usb_set_port_feature(struct usb_device *dev, int port, int feature)
{
	return usb3_control_msg(dev, usb_sndctrlpipe(dev, 0),(0x03), ((0x01 << 5) | (0x03)), feature,
				port, NULL, 0, USB_CNTL_TIMEOUT);
}

static int usb_get_hub_status(struct usb_device *dev, void *data)
{
	return usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0),(0x00), 0x80 | ((0x01 << 5) | 0x00), 0, 0,
			data, sizeof(struct usb_hub_status), USB_CNTL_TIMEOUT);
}

static int usb_get_port_status(struct usb_device *dev, int port, void *data)
{
	return usb3_control_msg(dev, usb_rcvctrlpipe(dev, 0),(0x00), 0x80 | ((0x01 << 5) | (0x03)), 0, port,
			data, sizeof(struct usb_hub_status), USB_CNTL_TIMEOUT);
}


static void usb_hub_power_on(struct usb_hub_device *hub)
{
	int i;
	struct usb_device *dev;
	unsigned pgood_delay = hub->desc.bPwrOn2PwrGood * 2;

	dev = hub->pusb_dev;
	for (i = 0; i < dev->maxchild; i++) {
		usb_set_port_feature(dev, i + 1, 8);
	}

	mdelay(max(pgood_delay, (unsigned)100));
}

void usb3_hub_reset(void)
{
	usb_hub_index = 0;
}

static struct usb_hub_device *usb_hub_allocate(void)
{
	if (usb_hub_index < USB_MAX_HUB)
		return &hub_dev[usb_hub_index++];

	printf("ERROR: USB_MAX_HUB (%d) reached\n", USB_MAX_HUB);
	return NULL;
}

#define MAX_TRIES 5

int hub3_port_reset(struct usb_device *dev, int port,
			unsigned short *portstat)
{
	int tries;
	ALLOC_CACHE_ALIGN_BUFFER(struct usb_port_status, portsts, 1);
	unsigned short portstatus, portchange;

	for (tries = 0; tries < MAX_TRIES; tries++) {

		usb_set_port_feature(dev, port + 1, 4);
		mdelay(200);

		if (usb_get_port_status(dev, port + 1, portsts) < 0) {
			return -1;
		}
		portstatus = le16_to_cpu(portsts->wPortStatus);
		portchange = le16_to_cpu(portsts->wPortChange);

		if ((portchange & 0x0001) ||
		    !(portstatus & 0x0001))
			return -1;

		if (portstatus & 0x0002)
			break;

		mdelay(200);
	}

	if (tries == MAX_TRIES) {
		return -1;
	}

	usb_clear_port_feature(dev, port + 1, 20);
	*portstat = portstatus;
	return 0;
}


void usb3_hub_port_connect_change(struct usb_device *dev, int port,int do_port_reset)
{
	struct usb_device *usb;
	ALLOC_CACHE_ALIGN_BUFFER(struct usb_port_status, portsts, 1);
	unsigned short portstatus;

	if (usb_get_port_status(dev, port + 1, portsts) < 0) {
		return;
	}

	portstatus = le16_to_cpu(portsts->wPortStatus);
	usb_clear_port_feature(dev, port + 1, 16);

	if (((!(portstatus & 0x0001)) &&
	     (!(portstatus & 0x0002))) || (dev->children[port])) {
		if (!(portstatus & 0x0001))
			return;
	}
	mdelay(200);
	if (do_port_reset) {
                if (hub3_port_reset(dev, port, &portstatus) < 0) {
                        printf("cannot reset XHCI port %i!?\n", port + 1);
                        return;
                }
        }

	mdelay(200);

	usb = usb3_alloc_new_device(dev->controller);

	if (portstatus & 0x0400)
		usb->speed = (0x2);
	else if (portstatus & 0x0200)
		usb->speed = 0x1;
	else
		usb->speed = 0x00;

	dev->children[port] = usb;
	usb->parent = dev;
	usb->portnr = port + 1;
	if (usb_new_dev(usb)) {
		usb_clear_port_feature(dev, port + 1, 1);
	}
}


static int usb_hub_configure(struct usb_device *dev)
{
	int i;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, USB_BUFSIZ);
	unsigned char *bitmap;
	short hubCharacteristics;
	struct usb_hub_descriptor *descriptor;
	struct usb_hub_device *hub;
	hub = usb_hub_allocate();
	if (hub == NULL)
		return -1;
	hub->pusb_dev = dev;
	if (usb_get_hub_descriptor(dev, buffer, 4) < 0) {
		return -1;
	}
	descriptor = (struct usb_hub_descriptor *)buffer;

	i = descriptor->bLength;
	if (i > USB_BUFSIZ) {
		return -1;
	}

	if (usb_get_hub_descriptor(dev, buffer, descriptor->bLength) < 0) {
		return -1;
	}
	memcpy((unsigned char *)&hub->desc, buffer, descriptor->bLength);
	put_unaligned(le16_to_cpu(get_unaligned(
			&descriptor->wHubCharacteristics)),
			&hub->desc.wHubCharacteristics);
	bitmap = (unsigned char *)&hub->desc.DeviceRemovable[0];
	memset(bitmap, 0xff, (USB_MAXCHILDREN+1+7)/8);
	bitmap = (unsigned char *)&hub->desc.PortPowerCtrlMask[0];
	memset(bitmap, 0xff, (USB_MAXCHILDREN+1+7)/8); /* PowerMask = 1B */

	for (i = 0; i < ((hub->desc.bNbrPorts + 1 + 7)/8); i++)
		hub->desc.DeviceRemovable[i] = descriptor->DeviceRemovable[i];

	for (i = 0; i < ((hub->desc.bNbrPorts + 1 + 7)/8); i++)
		hub->desc.PortPowerCtrlMask[i] = descriptor->PortPowerCtrlMask[i];

	dev->maxchild = descriptor->bNbrPorts;

	hubCharacteristics = get_unaligned(&hub->desc.wHubCharacteristics);
	switch (hubCharacteristics & 0x0003) {
	case 0x00:
		break;
	case 0x01:
		break;
	case 0x02:
	case 0x03:
		break;
	}

	switch (hubCharacteristics & 0x0018) {
	case 0x00:
		break;
	case 0x08:
		break;
	case 0x10:
	case 0x18:
		break;
	}

	if (sizeof(struct usb_hub_status) > USB_BUFSIZ) {
		return -1;
	}

	if (usb_get_hub_status(dev, buffer) < 0) {
		return -1;
	}

	usb_hub_power_on(hub);
	for (i = 0; i < dev->maxchild; i++) {
		ALLOC_CACHE_ALIGN_BUFFER(struct usb_port_status, portsts, 1);
		unsigned short portstatus, portchange;

		if (usb_get_port_status(dev, i + 1, portsts) < 0) {
			continue;
		}

		portstatus = le16_to_cpu(portsts->wPortStatus);
		portchange = le16_to_cpu(portsts->wPortChange);

		if (portchange & 0x0001) {
			usb3_hub_port_connect_change(dev, i,0);
		}
		else if ((portstatus & 0x1) == 1) {
                        usb3_hub_port_connect_change(dev, i, 1);
                }

		if (portchange & 0x0002) {
			usb_clear_port_feature(dev, i + 1, 17);

			if (!(portstatus & 0x0002) &&
			     (portstatus & 0x0001) &&
			     ((dev->children[i]))) {
				printf("already running port %i "  \
						"disabled by hub (EMI?), " \
						"re-enabling...\n", i + 1);
					usb3_hub_port_connect_change(dev, i,0);
			}
		}
		if (portstatus & 0x0004) {
			usb_clear_port_feature(dev, i+1,2);
		}

		if (portchange & 0x0008) {
			usb_clear_port_feature(dev, i+1,19);
			usb_hub_power_on(hub);
		}

		if (portchange & 0x0010) {
			usb_clear_port_feature(dev, i+1,20);
		}
	} 

	return 0;
}

int usb3_hub_probe(struct usb_device *dev, int ifnum)
{
	struct usb_interface *iface;
	struct usb_endpoint_descriptor *ep;
	int ret;

	iface = &dev->config.if_desc[ifnum];
	if (iface->desc.bInterfaceClass != 9)
		return 0;
	if ((iface->desc.bInterfaceSubClass != 0) &&
	    (iface->desc.bInterfaceSubClass != 1))
		return 0;
	if (iface->desc.bNumEndpoints != 1)
		return 0;
	ep = &iface->ep_desc[0].ep_desc;
	if (!(ep->bEndpointAddress & 0x80))
		return 0;
	if ((ep->bmAttributes & 3) != 3)
		return 0;
	ret = usb_hub_configure(dev);
	return ret;
}

int rootdev3;
static struct descriptor {
	struct usb_hub_descriptor hub;
	struct usb_device_descriptor device;
	struct usb_linux_config_descriptor config;
	struct usb_linux_interface_descriptor interface;
	struct usb_endpoint_descriptor endpoint;
	struct usb_ss_ep_comp_descriptor ep_companion;
} __attribute__ ((packed)) descriptor = {
	{
		0xc,		/* bDescLength */
		0x2a,		/* bDescriptorType: hub descriptor */
		2,		/* bNrPorts -- runtime modified */
		0,		/* wHubCharacteristics */
		10,		/* bPwrOn2PwrGood */
		0,		/* bHubCntrCurrent */
		{},		/* Device removable */
		{}		/* at most 7 ports! XXX */
	},
	{
		0x12,		/* bLength */
		1,		/* bDescriptorType: UDESC_DEVICE */
		cpu_to_le16(0x0300), /* bcdUSB: v3.0 */
		9,		/* bDeviceClass: UDCLASS_HUB */
		0,		/* bDeviceSubClass: UDSUBCLASS_HUB */
		3,		/* bDeviceProtocol: UDPROTO_SSHUBSTT */
		9,		/* bMaxPacketSize: 512 bytes  2^9 */
		0x0000,		/* idVendor */
		0x0000,		/* idProduct */
		cpu_to_le16(0x0100), /* bcdDevice */
		1,		/* iManufacturer */
		2,		/* iProduct */
		0,		/* iSerialNumber */
		1		/* bNumConfigurations: 1 */
	},
	{
		0x9,
		2,		/* bDescriptorType: UDESC_CONFIG */
		cpu_to_le16(0x19),
		1,		/* bNumInterface */
		1,		/* bConfigurationValue */
		0,		/* iConfiguration */
		0x40,		/* bmAttributes: UC_SELF_POWER */
		0		/* bMaxPower */
	},
	{
		0x9,		/* bLength */
		4,		/* bDescriptorType: UDESC_INTERFACE */
		0,		/* bInterfaceNumber */
		0,		/* bAlternateSetting */
		1,		/* bNumEndpoints */
		9,		/* bInterfaceClass: UICLASS_HUB */
		0,		/* bInterfaceSubClass: UISUBCLASS_HUB */
		0,		/* bInterfaceProtocol: UIPROTO_HSHUBSTT */
		0		/* iInterface */
	},
	{
		0x7,		/* bLength */
		5,		/* bDescriptorType: UDESC_ENDPOINT */
		0x81,		/* bEndpointAddress: IN endpoint 1 */
		3,		/* bmAttributes: UE_INTERRUPT */
		8,		/* wMaxPacketSize */
		255		/* bInterval */
	},
	{
		0x06,		/* ss_bLength */
		0x30,		/* ss_bDescriptorType: SS EP Companion */
		0x00,		/* ss_bMaxBurst: allows 1 TX between ACKs */
		/* ss_bmAttributes: 1 packet per service interval */
		0x00,
		/* ss_wBytesPerInterval: 15 bits for max 15 ports */
		0x02,
	},
};

struct xhci_ctrl xhcic_3[CONFIG_USB_MAX_CONTROLLER_COUNT];

unsigned int xhci3_getalignedlength(unsigned int length)
{

	unsigned int aligned_length = 0;
	unsigned int remain  = 0;
	unsigned int toAdd  = 0;

	remain = (length % 0x40);
	toAdd  = remain ? (0x40 - remain) : 0;
	aligned_length  = length + toAdd;

	return aligned_length;
}

static void xhci_segment_free(struct xhci_segment *seg)
{
	if (seg->trbs) {
		free(seg->trbs);
		seg->trbs = NULL;
	}

	free(seg);
}

void xhci3_ring_free(struct xhci_ring *ring)
{
	struct xhci_segment *seg;
	struct xhci_segment *first_seg;

	if (!ring)
		return;
	if (ring->first_seg) {
		first_seg = ring->first_seg;
		seg = first_seg->next;
		while (seg != first_seg) {
			struct xhci_segment *next = seg->next;
			xhci_segment_free(seg);
			seg = next;
		}
		xhci_segment_free(first_seg);
		ring->first_seg = NULL;
	}

	free(ring);
}

static void xhci_free_container_ctx(struct xhci_container_ctx *ctx)
{
	if (!ctx)
		return;
	free(ctx->bytes);
	free(ctx);
}

int xhci3_free_virt_device(struct xhci_ctrl *ctrl)
{
	int i;
	int slot_id = 0;
	struct xhci_virt_device *virt_dev;

	if (!ctrl)
		return -1;

	slot_id = ctrl->slot_id;

	if (slot_id == 0 || !ctrl->devs[slot_id]) {
		return -1;
	}

	virt_dev = ctrl->devs[slot_id];

	ctrl->dcbaa->dev_context_ptrs[slot_id] = 0;

	if (!virt_dev)
		return -1;

	for (i = 0; i < 31; ++i) {
		if (virt_dev->eps[i].ring)
			xhci3_ring_free(virt_dev->eps[i].ring);
	}

	if (virt_dev->in_ctx)
		xhci_free_container_ctx(virt_dev->in_ctx);
	if (virt_dev->out_ctx)
		xhci_free_container_ctx(virt_dev->out_ctx);

	free(ctrl->devs[slot_id]);
	ctrl->devs[slot_id] = NULL;

	return 0;
}

void xhci3_cleanup(struct xhci_ctrl *ctrl)
{
	xhci3_ring_free(ctrl->event_ring);
	xhci3_ring_free(ctrl->cmd_ring);
	xhci3_free_virt_device(ctrl);
	free(ctrl->erst.entries);
	free(ctrl->dcbaa);
	memset(ctrl, '\0', sizeof(struct xhci_ctrl));
}

void *xhci3_malloc(unsigned int size)
{
	void *ptr = memalign(XHCI_ALIGNMENT, size);
	memset(ptr, '\0', size);
	return ptr;
}

static int handshake(uint32_t volatile *ptr, uint32_t mask,
					uint32_t done, int usec)
{
	uint32_t result;

	do {
		result = xhci_readl(ptr);
		if (result == ~(uint32_t)0)
			return -ENODEV;
		result &= mask;
		if (result == done)
			return 0;
		usec--;
		udelay(1);
	} while (usec > 0);

	return -ETIMEDOUT;
}

void xhci3_quiesce(struct xhci_hcor *hcor)
{
	uint32_t halted;
	uint32_t cmd;
	uint32_t mask;

	mask = ~(XHCI_IRQS);
	halted = xhci_readl(&hcor->or_usbsts) & (1 << 0);
	if (!halted)
		mask &= ~(1 << 0);

	cmd = xhci_readl(&hcor->or_usbcmd);
	cmd &= mask;
	xhci_writel(&hcor->or_usbcmd, cmd);
}

int xhci3_halt(struct xhci_hcor *hcor)
{
	int ret;
	xhci3_quiesce(hcor);

	ret = handshake(&hcor->or_usbsts,
			(1 << 0), (1 << 0), XHCI_MAX_HALT_USEC);
	return ret;
}

static int xhci_start(struct xhci_hcor *hcor)
{
	u32 temp;
	int ret;

	printf("Starting the controller\n");
	temp = xhci_readl(&hcor->or_usbcmd);
	temp |= ((1 << 0));
	xhci_writel(&hcor->or_usbcmd, temp);
	temp = xhci_readl(&hcor->or_usbcmd);

	ret = handshake(&hcor->or_usbsts, (1 << 0), 0, XHCI_MAX_HALT_USEC);
	return ret;
}

int xhci3_reset(struct xhci_hcor *hcor)
{
	u32 command;
	u32 state;
	int ret;

	state = xhci_readl(&hcor->or_usbsts);
	if ((state & (1 << 0)) == 0) {
		return -1;
	}

	command = xhci_readl(&hcor->or_usbcmd);
	command |= (1 << 1);
	xhci_writel(&hcor->or_usbcmd, command);

	ret = handshake(&hcor->or_usbcmd, (1 << 1), 0, XHCI_MAX_RESET_USEC);
	if (ret)
		return ret;

	return handshake(&hcor->or_usbsts, (1 << 11), 0, XHCI_MAX_RESET_USEC);
}

static struct xhci_segment *xhci_segment_alloc(void)
{
	struct xhci_segment *seg;

	seg = (struct xhci_segment *)xhci3_malloc(sizeof(struct xhci_segment));
	if (!seg)
		return NULL;

	seg->trbs = (union xhci_trb *)xhci3_malloc((64*16));

	if (!seg->trbs)
		return NULL;

	seg->next = NULL;

	return seg;
}

static void xhci_link_segments(struct xhci_segment *prev,
				struct xhci_segment *next, bool link_trbs)
{
	u32 val;
	u64 val_64 = 0;

	if (!prev || !next)
		return;
	prev->next = next;
	if (link_trbs) {
		val_64 = (uintptr_t)next->trbs;
		prev->trbs[64-1].link.segment_ptr = val_64;

		val = le32_to_cpu(prev->trbs[64-1].link.control);
		val &= ~(0xfc00);
		val |= (TRB_LINK << 10);

		prev->trbs[64-1].link.control = cpu_to_le32(val);
	}
}

static void xhci_initialize_ring_info(struct xhci_ring *ring)
{
	ring->enqueue = ring->first_seg->trbs;
	ring->enq_seg = ring->first_seg;
	ring->dequeue = ring->enqueue;
	ring->deq_seg = ring->first_seg;

	ring->cycle_state = 1;
	ring->enq_updates = 0;
	ring->deq_updates = 0;
}

static struct xhci_ring *xhci_ring_alloc(unsigned int num_segs, bool link_trbs)
{
	struct xhci_ring *ring;
	struct xhci_segment *prev;

	ring = (struct xhci_ring *)malloc(sizeof(struct xhci_ring));
	if (!ring)
		return NULL;

	if (num_segs == 0)
		return ring;

	ring->first_seg = xhci_segment_alloc();
	if (!ring->first_seg)
		return NULL;
	num_segs--;

	prev = ring->first_seg;
	while (num_segs > 0) {
		struct xhci_segment *next;

		next = xhci_segment_alloc();
		if (!next)
			return NULL;
		xhci_link_segments(prev, next, link_trbs);

		prev = next;
		num_segs--;
	}
	xhci_link_segments(prev, ring->first_seg, link_trbs);
	if (link_trbs) {
		prev->trbs[64-1].link.control |=
					cpu_to_le32((0x1 << 1));
	}
	xhci_initialize_ring_info(ring);

	return ring;
}

static int enqueue_is_link_trb(struct xhci_ring *ring)
{
	struct xhci_link_trb *link = &ring->enqueue->link;

	return ((link->control & cpu_to_le32((0xfc00))) ==
			cpu_to_le32(TRB_LINK << 10));
}

static int last_trb(struct xhci_ctrl *ctrl, struct xhci_ring *ring,
			struct xhci_segment *seg, union xhci_trb *trb)
{
	if (ring == ctrl->event_ring)
		return trb == &seg->trbs[64];
	else
		return ((trb->link.control & cpu_to_le32((0xfc00))) ==
				cpu_to_le32(TRB_LINK << 10));
}

static bool last_trb_on_last_seg(struct xhci_ctrl *ctrl, struct xhci_ring *ring,
				struct xhci_segment *seg, union xhci_trb *trb)
{
	if (ring == ctrl->event_ring)
		return ((trb == &seg->trbs[64]) &&
			(seg->next == ring->first_seg));
	else
		return le32_to_cpu(trb->link.control) & (0x1 << 1);
}

static void inc_enq(struct xhci_ctrl *ctrl, struct xhci_ring *ring,
				bool consumer, bool more_trbs_coming)
{
	u32 chain;
	union xhci_trb *next;

	chain = le32_to_cpu(ring->enqueue->generic.field[3]) & (1<<4);
	next = ++(ring->enqueue);

	ring->enq_updates++;
	while (last_trb(ctrl, ring, ring->enq_seg, next)) {
		if (!consumer) {
			if (ring != ctrl->event_ring) {
			if (!chain && !more_trbs_coming)
				break;

				next->link.control &= cpu_to_le32(~(1<<4));
				next->link.control |= cpu_to_le32(chain);

				next->link.control ^= cpu_to_le32((1<<0));
			}
			if (last_trb_on_last_seg(ctrl, ring,
						ring->enq_seg, next))
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
		}
		ring->enq_seg = ring->enq_seg->next;
		ring->enqueue = ring->enq_seg->trbs;
		next = ring->enqueue;

	}
}
static struct xhci_generic_trb *queue_trb(struct xhci_ctrl *ctrl,
					struct xhci_ring *ring,
					bool consumer,
					bool more_trbs_coming,
					u32 *trb_fields)
{
	struct xhci_generic_trb *trb;
	int i;

	trb = &ring->enqueue->generic;

	for (i = 0; i < 4; i++)
		trb->field[i] = cpu_to_le32(trb_fields[i]);

	inc_enq(ctrl, ring, consumer, more_trbs_coming);

	return trb;
}

static int prepare_ring(struct xhci_ctrl *ctrl, struct xhci_ring *ep_ring,
								u32 ep_state)
{
	switch (ep_state) {
	case 0:
		return -ENOENT;
	case 4:
		return -EINVAL;
	case 2:
	case 3:
	case 1:
		break;
	default:
		return -EINVAL;
	}

	if (enqueue_is_link_trb(ep_ring)) {
		struct xhci_ring *ring = ep_ring;
		union xhci_trb *next;

		next = ring->enqueue;

		while (last_trb(ctrl, ring, ring->enq_seg, next)) {
			next->link.control &= cpu_to_le32(~(1<<4));

			next->link.control ^= cpu_to_le32((1<<0));

			if (last_trb_on_last_seg(ctrl, ring,
						ring->enq_seg, next))
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
			ring->enq_seg = ring->enq_seg->next;
			ring->enqueue = ring->enq_seg->trbs;
			next = ring->enqueue;
		}
	}

	return 0;
}

static int queue_command(struct xhci_ctrl *ctrl, u32 *trb_fields)
{
	int ret;
	u32 fields[4];

	ret = prepare_ring(ctrl, ctrl->cmd_ring, 1);
	if (ret < 0) {
		return ret;
	}

	fields[0] = trb_fields[0];
	fields[1] = trb_fields[1];
	fields[2] = trb_fields[2];
	fields[3] = trb_fields[3] | ctrl->cmd_ring->cycle_state;

	queue_trb(ctrl, ctrl->cmd_ring, false, false, fields);

	return 0;
}

void xhci3_ring_cmd_db(struct xhci_ctrl *ctrl)
{
	xhci_writel(&ctrl->dba->doorbell[0], 0x00000000);

	xhci_readl(&ctrl->dba->doorbell[0]);
}

void xhci3_ring_ep_doorbell(struct xhci_ctrl *ctrl,
				unsigned int slot_id,
				unsigned int ep_index,
				unsigned int stream_id)
{
	xhci_writel(&ctrl->dba->doorbell[slot_id], DB_VALUE(ep_index, 0));
}

unsigned long trb3_addr(struct xhci_segment *seg,
				union xhci_trb *trb)
{
	unsigned long segment_offset;

	if (!seg || !trb || trb < seg->trbs)
		return 0;

	segment_offset = trb - seg->trbs;
	if (segment_offset > 64)
		return 0;

	return (unsigned long)(seg->trbs + (segment_offset * sizeof(*trb)));
}

static u32 xhci_td_remainder(unsigned int remainder)
{
	u32 max = (1 << (21 - 17 + 1)) - 1;

	if ((remainder) >= max)
		return max << 17;
	else
		return (remainder) << 17;
}

unsigned int xhci3_get_ep_index(struct usb_endpoint_descriptor *desc,
							struct devrequest *req)
{
	unsigned int index;
	if (req != NULL) {
		index = (unsigned int)((desc->bEndpointAddress &
					(0x0f))*2);
	} else {
		index = (unsigned int)((desc->bEndpointAddress &
					(0x0f))*2) +
					(((desc->bEndpointAddress &
					(0x80)) == 0x80) ?
					1 : 0) - 1;
	}

	return index;
}

static void giveback_first_trb(struct xhci_ctrl *ctrl, int slot_id,
					unsigned int ep_index,
					unsigned int stream_id,
					int start_cycle,
					struct xhci_generic_trb *start_trb)
{
	int delay;
	if (start_cycle)
		start_trb->field[3] |= cpu_to_le32(start_cycle);
	else
		start_trb->field[3] &= cpu_to_le32(~(1<<0));

	xhci3_ring_ep_doorbell(ctrl, slot_id, ep_index, stream_id);

	delay = (ctrl->speed != (0x3)) ? 110 : 1;
	mdelay(delay);

	return;
}

struct xhci_ep_ctx *xhci3_get_ep_ctx(struct xhci_ctrl *ctrl,
				    struct xhci_container_ctx *ctx,
				    unsigned int ep_index)
{
	ep_index++;
	if (ctx->type == XHCI_CTX_TYPE_INPUT)
		ep_index++;

	return (struct xhci_ep_ctx *)
		(ctx->bytes +
		(ep_index * CTX_SIZE(readl(&ctrl->hccr->cr_hccparams))));
}

static int xhci_configure_endpoint_result(struct xhci_ctrl *ctrl,
						u32 *cmd_status)
{
	int ret = 0;

	switch (*cmd_status) {
	case COMP_ENOMEM:
		ret = -ENOMEM;
		break;
	case COMP_BW_ERR:
	case COMP_2ND_BW_ERR:
		ret = -ENOSPC;
		break;
	case COMP_TRB_ERR:
		ret = -EINVAL;
		break;
	case COMP_DEV_ERR:
		ret = -ENODEV;
		break;
	case COMP_SUCCESS:
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int xhci_evaluate_context_result(u32 *cmd_status)
{
	int ret;

	switch (*cmd_status) {
	case COMP_EINVAL:
		ret = -EINVAL;
		break;
	case COMP_EBADSLT:
	case COMP_CTX_STATE:
		ret = -EINVAL;
		break;
	case COMP_DEV_ERR:
		ret = -ENODEV;
		break;
	case COMP_MEL_ERR:
		ret = -EINVAL;
		break;
	case COMP_SUCCESS:
		ret = 0;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

int xhci3_queue_configure_endpoint(struct xhci_ctrl *ctrl, u8 *in_ctx_bytes,
								u32 slot_id)
{
	u64 val_64;
	u32 trb_fields[4];

	val_64 = (uintptr_t)in_ctx_bytes;

	trb_fields[0] = lower_32_bits(val_64);
	trb_fields[1] = upper_32_bits(val_64);
	trb_fields[2] = 0;
	trb_fields[3] = ((TRB_CONFIG_EP << 10) |
			((slot_id & 0xff) <<
			24));

	return queue_command(ctrl, trb_fields);
}

int xhci3_queue_evaluate_context(struct xhci_ctrl *ctrl, u8 *in_ctx_bytes,
								u32 slot_id)
{
	u64 val_64;
	u32 trb_fields[4];

	val_64 = (uintptr_t)in_ctx_bytes;

	trb_fields[0] = lower_32_bits(val_64);
	trb_fields[1] = upper_32_bits(val_64);
	trb_fields[2] = 0;
	trb_fields[3] = ((TRB_EVAL_CONTEXT << 10) |
			((slot_id & 0xff) <<
			24));

	return queue_command(ctrl, trb_fields);
}

static int xhci_configure_endpoint(struct usb_device *usbdev, bool ctx_change)
{
	int ret;
	struct xhci_container_ctx *in_ctx;
	u32 *cmd_status;
	struct xhci_virt_device *virt_dev;
	struct xhci_ctrl *ctrl = usbdev->controller;

	virt_dev = ctrl->devs[ctrl->slot_id];
	in_ctx = virt_dev->in_ctx;

	cmd_status = &virt_dev->cmd_status;

	if (!ctx_change)
		ret = xhci3_queue_configure_endpoint(ctrl, in_ctx->bytes,
							ctrl->slot_id);
	else
		ret = xhci3_queue_evaluate_context(ctrl, in_ctx->bytes,
							ctrl->slot_id);

	if (ret < 0)
		return -ENOMEM;

	xhci3_ring_cmd_db(ctrl);

	mdelay(5);

	xhci3_poll_and_HandleEvent(usbdev);

	if (!ctx_change)
		ret = xhci_configure_endpoint_result(ctrl, cmd_status);
	else
		ret = xhci_evaluate_context_result(cmd_status);

	return ret;
}

static int xhci_configure_ep(struct usb_device *usbdev)
{
	struct xhci_container_ctx *in_ctx;
	struct xhci_container_ctx *out_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_slot_ctx *slot_ctx;
	struct xhci_ep_ctx *ep_ctx[MAX_EP_CTX_NUM];
	int ret = 0;
	int cur_ep;
	int max_ep_flag = 0;
	int ep_index;
	unsigned int dir;
	unsigned int ep_type;
	struct xhci_ctrl *ctrl = usbdev->controller;
	int num_of_ep;
	int ep_flag = 0;
	u64 trb_64 = 0;
	int slot_id = ctrl->slot_id;
	struct xhci_virt_device *virt_dev = ctrl->devs[slot_id];
	struct usb_interface *ifdesc;

	out_ctx = virt_dev->out_ctx;
	in_ctx = virt_dev->in_ctx;

	num_of_ep = usbdev->config.if_desc[0].no_of_ep;
	ifdesc = &usbdev->config.if_desc[0];

	ctrl_ctx = xhci3_get_input_control_ctx(ctrl->devs[slot_id]->in_ctx);

	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) {
		ep_flag = xhci3_get_ep_index(&ifdesc->ep_desc[cur_ep].ep_desc,
									NULL);
		ctrl_ctx->add_flags |= cpu_to_le32(1 << (ep_flag + 1));
		if (max_ep_flag < ep_flag)
			max_ep_flag = ep_flag;
	}
	ctrl_ctx->drop_flags = 0;

	xhci3_slot_copy(ctrl, in_ctx, out_ctx);
	slot_ctx = xhci3_get_slot_ctx(ctrl, in_ctx);
	slot_ctx->dev_info &= ~(0x1f <<  27);
	slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(max_ep_flag + 1) | 0);

	xhci3_endpoint_copy(ctrl, in_ctx, out_ctx, 0);

	ep_index = 0;
	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) {
		struct usb_endpoint_descriptor *endpt_desc = NULL;

		endpt_desc = &ifdesc->ep_desc[cur_ep].ep_desc;
		trb_64 = 0;

		ep_index = xhci3_get_ep_index(endpt_desc, NULL);
		ep_ctx[ep_index] = xhci3_get_ep_ctx(ctrl, in_ctx, ep_index);

		virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, true);
		if (!virt_dev->eps[ep_index].ring)
			return -1;

		dir = (((endpt_desc->bEndpointAddress) & (0x80)) >> 7);
		ep_type = (((endpt_desc->bmAttributes) & (0x3)) | (dir << 2));
		ep_ctx[ep_index]->ep_info2 =
			cpu_to_le32(ep_type << 3);
		ep_ctx[ep_index]->ep_info2 |=
			cpu_to_le32(MAX_PACKET(endpt_desc->wMaxPacketSize));

		ep_ctx[ep_index]->ep_info2 |=
			cpu_to_le32(((0 & (0xff)) << 8) |
			((3 & 0x3) << 1));

		trb_64 = (uintptr_t)
				virt_dev->eps[ep_index].ring->first_seg->trbs;
		ep_ctx[ep_index]->deq = cpu_to_le64(trb_64 |
				virt_dev->eps[ep_index].ring->cycle_state);
	}

	ret = xhci_configure_endpoint(usbdev, false);

	return ret;
}
int xhci3_queue_ctrl_tx(struct usb_device *usbdev, struct xhci_ring *ring,
			unsigned long pipe, struct devrequest *req,
			unsigned int length, unsigned int ep_index,
			void *buffer)
{
	int ret;
	int start_cycle;
	int num_trbs;
	u32 field;
	u32 length_field;
	u64 buf_64 = 0;
	struct xhci_ring *ep_ring = ring;
	struct xhci_generic_trb *start_trb;
	struct xhci_ctrl *ctrl = usbdev->controller;
	int slot_id = ctrl->slot_id;
	u32 trb_fields[4];

	struct xhci_ep_ctx *ep_ctx = NULL;
	ep_ctx = xhci3_get_ep_ctx(ctrl, ctrl->devs[slot_id]->out_ctx, ep_index);

	num_trbs = 2;
	if (length > 0)
		num_trbs++;

	ret = prepare_ring(ctrl, ring,
				le32_to_cpu(ep_ctx->ep_info) & (0xf));

	if (ret < 0)
		return ret;

	start_trb = &ring->enqueue->generic;
	start_cycle = ring->cycle_state;

	field = 0;
	field |= (1<<6) | (TRB_SETUP << 10);
	if (start_cycle == 0)
		field |= 0x1;

	if (HC_VERSION(xhci_readl(&ctrl->hccr->cr_capbase)) == 0x100) {
		if (length > 0) {
			if (req->requesttype & 0x80)
				field |= (3 << 16);
			else
				field |= (2 << 16);
		}
	}
		trb_fields[0] = req->requesttype | req->request << 8 |
					le16_to_cpu(req->value) << 16;
		trb_fields[1] = le16_to_cpu(req->index) |
				le16_to_cpu(req->length) << 16;
		trb_fields[2] = (8 | ((0 & (0x3ff)) <<
				22));
		trb_fields[3] = field;
		queue_trb(ctrl, ring, false, true, trb_fields);

	mdelay(1);

	field = 0;
	if (usb_pipein(pipe))
		field = (1<<2) | (TRB_DATA << 10);
	else
		field = (TRB_DATA << 10);

	length_field = (length & (0x1ffff)) | xhci_td_remainder(length) |
			((0 & (0x3ff)) << 22);

	if (length > 0) {
		if (req->requesttype & 0x80)
			field |= (1<<16);
		buf_64 = (uintptr_t)buffer;

		trb_fields[0] = lower_32_bits(buf_64);
		trb_fields[1] = upper_32_bits(buf_64);
		trb_fields[2] = length_field;
		trb_fields[3] = field | ep_ring->cycle_state;
		queue_trb(ctrl, ring, false, true, trb_fields);

		mdelay(1);

	}

	field = 0;
	if (length > 0 && req->requesttype & 0x80)
		field = 0;
	else
		field = (1<<16);

	trb_fields[0] = 0;
	trb_fields[1] = 0;
	trb_fields[2] = ((0 & (0x3ff)) << 22);
	trb_fields[3] = field | (1<<5) | (TRB_STATUS << 10) | ep_ring->cycle_state;

	queue_trb(ctrl, ring, false, false, trb_fields);

	mdelay(1);

	giveback_first_trb(ctrl, slot_id, ep_index, 0, start_cycle, start_trb);

	usbdev->act_len = length;
	usbdev->status = 0;

	if (ctrl->speed != (0x3))
		mdelay(5);

	return 0;
}

int xhci3_submit_async(struct usb_device *usbdev,
			unsigned long pipe, void *buffer,
			int length, struct devrequest *req)
{
	struct xhci_ctrl *ctrl = usbdev->controller;
	unsigned int slot_id;
	unsigned int ep_index;
	int ret;

	slot_id = ctrl->slot_id;

	if (ctrl->slot_id == 0) {
		usbdev->act_len = 21;
		usbdev->status = 0;
		return 0;
	}

	if (usb_pipecontrol(pipe) && req != NULL) {
		ep_index = 0;
		ret = xhci3_queue_ctrl_tx(usbdev,
					ctrl->devs[slot_id]->eps[ep_index].ring,
					pipe, req, length, ep_index, buffer);
		if (ret)
			return -1;
	}

	return 0;
}

static void xhci_clear_port_change_bit(u16 wValue,
		u16 wIndex, uint32_t *addr, u32 port_status)
{
	char *port_change_bit;
	u32 status;

	switch (wValue) {
	case 20:
		status = (1 << 21);
		port_change_bit = "reset";
		break;
	case 16:
		status = (1 << 17);
		port_change_bit = "connect";
		break;
	case 19:
		status = (1 << 20);
		port_change_bit = "over-current";
		break;
	case 17:
		status = (1 << 18);
		port_change_bit = "enable/disable";
		break;
	case 18:
		status = (1 << 22);
		port_change_bit = "suspend/resume";
		break;
	default:
		return;
	}

	xhci_writel(addr, port_status | status);
	port_status = xhci_readl(addr);
	printf("clear port %s change, actual port %d status  = 0x%x\n", port_change_bit, wIndex, port_status);


}

u32 xhci3_port_state_to_neutral(u32 state)
{
	return (state & XHCI_PORT_RO) | (state & XHCI_PORT_RWS);
}
static unsigned int xhci_port_speed(struct usb_device *usbdev,
					unsigned int port_status)
{
	struct xhci_ctrl *ctrl = usbdev->controller;

	if (DEV_FULLSPEED(port_status)) {
		usbdev->speed = 0;
		ctrl->speed = usbdev->speed;
		return 0x1;
	}
	if (DEV_LOWSPEED(port_status)) {
		usbdev->speed = 1;
		ctrl->speed = usbdev->speed;
		return 0x2;
	}
	if (DEV_HIGHSPEED(port_status)) {
		usbdev->speed = 2;
		ctrl->speed = usbdev->speed;
		return 0x3;
	}
	if (DEV_SUPERSPEED(port_status)) {
		usbdev->speed = 3;
		ctrl->speed = usbdev->speed;
		return 0x4;
	}

	return 0;
}

int xhci3_submit_root(struct usb_device *usbdev, unsigned long pipe,void *buffer, int length, 
			struct devrequest *req)
{
	uint8_t tmpbuf[4];
	u16 typeReq;
	void *srcptr = NULL;
	int len, srclen;
	uint32_t reg;
	uint32_t *status_reg;
	struct xhci_ctrl *ctrl = usbdev->controller;
	struct xhci_hcor *hcor = ctrl->hcor;

	if (le16_to_cpu(req->index) > CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS) {
		printf("The request port(%d) is not configured\n",
			le16_to_cpu(req->index) - 1);
		return -1;
	}

	status_reg = (uint32_t *)
		     (&hcor->PortRegs[le16_to_cpu(req->index) - 1].or_portsc);
	srclen = 0;

	typeReq = req->request | req->requesttype << 8;

	switch (typeReq) {
	case DeviceRequest | (0x06):
		switch (le16_to_cpu(req->value) >> 8) {
		case (0x01):
			srcptr = &descriptor.device;
			srclen = 0x12;
			break;
		case (0x02):
			srcptr = &descriptor.config;
			srclen = 0x19;
			break;
		case 0x03:
			switch (le16_to_cpu(req->value) & 0xff) {
			case 0:	/* Language */
				srcptr = "\4\3\1\0";
				srclen = 4;
				break;
			case 1:	/* Vendor String  */
				srcptr = "\16\3u\0-\0b\0o\0o\0t\0";
				srclen = 14;
				break;
			case 2:	/* Product Name */
				srcptr = "\52\3X\0H\0C\0I\0 "
					 "\0H\0o\0s\0t\0 "
					 "\0C\0o\0n\0t\0r\0o\0l\0l\0e\0r\0";
				srclen = 42;
				break;
			default:
				goto unknown;
			}
			break;
		default:
			goto unknown;
		}
		break;
	case (0x06) | ((0x80 | ((0x01 << 5) | 0x00)) << 8):
		switch (le16_to_cpu(req->value) >> 8) {
		case ((0x01 << 5) | 0x09):
			srcptr = &descriptor.hub;
			srclen = 0x8;
			break;
		default:
			printf("unknown value %x\n", le16_to_cpu(req->value));
			goto unknown;
		}
		break;
	case 0XDE | (0x00 << 8):
		xhci3_alloc_dev(usbdev);
		break;
	case (0x05) | (0x00 << 8):
		if (usbdev->devnum == 1)
			ctrl->rootdev3 = le16_to_cpu(req->value);
		else
			xhci3_address_device(usbdev);
		break;
	case DeviceOutRequest | (0x09):
			xhci_configure_ep(usbdev);
		break;
	case (0x00) | ((0x80 | ((0x01 << 5) | 0x00)) << 8):
		tmpbuf[0] = 1;	/* USB_STATUS_SELFPOWERED */
		tmpbuf[1] = 0;
		srcptr = tmpbuf;
		srclen = 2;
		break;
	case (0x00) | ((((0x01 << 5) | (0x03)) | 0x80) << 8):
		memset(tmpbuf, 0, 4);
		reg = xhci_readl(status_reg);
		if (reg & (1 << 0)) {
			tmpbuf[0] |= 0x0001;
			tmpbuf[1] |= (xhci_port_speed(usbdev, reg) << 2);
		}
		if (reg & (1 << 1))
			tmpbuf[0] |= 0x0002;
		if (reg & (0x3 << 5))
			tmpbuf[0] |= 0x0004;
		if (reg & (1 << 3))
			tmpbuf[0] |= 0x0008;
		if (reg & (1 << 4))
			tmpbuf[0] |= 0x0010;
		if (reg & (1 << 9))
			tmpbuf[1] |= 0x0200 >> 8;
		if ((reg & (1 << 22)))
			tmpbuf[2] |= 0x0040;
		if ((reg & (1 << 19)))
			tmpbuf[2] |= 0x0020;
		if (reg & (1 << 17))
			tmpbuf[2] |= 0x0001;
		if (reg & (1 << 18))
			tmpbuf[2] |= 0x0002;
		if (reg & (1 << 20))
			tmpbuf[2] |= 0x0008;
		if (reg & (1 << 21))
			tmpbuf[2] |= 0x0010;

		srcptr = tmpbuf;
		srclen = 4;
		break;
	case (0x03) | ((0 | ((0x01 << 5) | (0x03))) << 8):
		reg = xhci_readl(status_reg);
		switch (le16_to_cpu(req->value)) {
		case 1:
			reg |= (1 << 1);
			xhci_writel(status_reg, reg);
			break;
		case 8:
			reg |= (1 << 9);
			xhci_writel(status_reg, reg);
			break;
		case 4:
			reg = xhci_readl(status_reg);
			reg |= (1 << 4);
			xhci_writel(status_reg, reg);
			xhci3_poll_and_HandleEvent(usbdev);
			break;
		default:
			printf("unknown feature %x\n", le16_to_cpu(req->value));
			goto unknown;
		}
		xhci_readl(&hcor->or_usbcmd);
		break;
	case (0x01) | ((0 | ((0x01 << 5) | (0x03))) << 8):
		reg = xhci_readl(status_reg);
		reg = xhci3_port_state_to_neutral(reg);
		switch (le16_to_cpu(req->value)) {
		case 1:
			reg &= ~(1 << 1);
			break;
		case 20:
		case 16:
		case 19:
		case 17:
			xhci_clear_port_change_bit((le16_to_cpu(req->value)), le16_to_cpu(req->index),
							status_reg, reg);
			break;
		default:
			goto unknown;
		}
		xhci_writel(status_reg, reg);
		xhci_readl(&hcor->or_usbcmd);
		break;
	default:
		goto unknown;
	}

	mdelay(5);

	len = min3(srclen, le16_to_cpu(req->length), length);
	if (srcptr != NULL && len > 0)
		memcpy(buffer, srcptr, len);

	usbdev->act_len = len;
	usbdev->status = 0;

	return 0;

unknown:
	usbdev->act_len = 0;
	usbdev->status = 0x2;

	return -1;
}

static struct xhci_container_ctx *xhci_alloc_container_ctx(struct xhci_ctrl *ctrl, int type)
{
	struct xhci_container_ctx *ctx;

	ctx = (struct xhci_container_ctx *)xhci3_malloc(sizeof(struct xhci_container_ctx));

	if (!ctx)
		return NULL;

	BUG_ON((type != XHCI_CTX_TYPE_DEVICE) && (type != XHCI_CTX_TYPE_INPUT));
	ctx->type = type;
	ctx->size = HCC_64BYTE_CONTEXT(readl(&ctrl->hccr->cr_hccparams)) ?
								2048 : 1024;
	if (type == XHCI_CTX_TYPE_INPUT)
		ctx->size += CTX_SIZE(readl(&ctrl->hccr->cr_hccparams));

	ctx->bytes = (u8 *)xhci3_malloc(ctx->size);

	return ctx;
}

struct xhci_input_control_ctx *xhci3_get_input_control_ctx(struct xhci_container_ctx *ctx)
{
	BUG_ON(ctx->type != XHCI_CTX_TYPE_INPUT);
	return (struct xhci_input_control_ctx *)ctx->bytes;
}

struct xhci_slot_ctx *xhci3_get_slot_ctx(struct xhci_ctrl *ctrl,struct xhci_container_ctx *ctx)
{
	if (ctx->type == XHCI_CTX_TYPE_DEVICE)
		return (struct xhci_slot_ctx *)ctx->bytes;

	return (struct xhci_slot_ctx *)(ctx->bytes + CTX_SIZE(readl(&ctrl->hccr->cr_hccparams)));
}

static void inc_deq(struct xhci_ctrl *ctrl, struct xhci_ring *ring, bool consumer)
{
	union xhci_trb *next = ++(ring->dequeue);

	ring->deq_updates++;
	while (last_trb(ctrl, ring, ring->deq_seg, next)) {
		if (consumer &&	last_trb_on_last_seg(ctrl, ring, ring->deq_seg, next))
			ring->cycle_state = (ring->cycle_state ? 0 : 1);

		ring->deq_seg = ring->deq_seg->next;
		ring->dequeue = ring->deq_seg->trbs;
		next = ring->dequeue;
	}

}

static void handle_port_status(struct xhci_ctrl *ctrl, union xhci_trb *event)
{
	int max_ports;

	if (((le32_to_cpu(event->generic.field[2]) & (0xff << 24)) >> 24) != COMP_SUCCESS)
		return;

	ctrl->port_id = (((le32_to_cpu(event->generic.field[0]) &  (0xff << 24)) >> 24));
	max_ports = ((readl(&ctrl->hccr->cr_hcsparams1) & HCS_MAX_PORTS_MASK) >> HCS_MAX_PORTS_SHIFT);
	printf("Maximum ports enabled ---> %d\n",max_ports);
	return;
}

static void handle_cmd_completion(struct xhci_ctrl *ctrl, struct xhci_event_cmd *event)
{
	int slot_id = ((le32_to_cpu(event->flags) & (0xff << 24)) >> 24);
	u64 cmd_addr;
	unsigned long cmd_dequeue_addr;

	cmd_addr = le64_to_cpu(event->cmd_trb);
	cmd_dequeue_addr = trb3_addr(ctrl->cmd_ring->deq_seg, ctrl->cmd_ring->dequeue);

	if (cmd_dequeue_addr == 0)
		return;

	if (cmd_addr != (u64) cmd_dequeue_addr)
		return;

	switch ((le32_to_cpu(ctrl->cmd_ring->dequeue->generic.field[3]) & (0xfc00)) >> 10) {
	case TRB_ENABLE_SLOT:
		if (((le32_to_cpu(event->status) & (0xff << 24)) >> 24) == COMP_SUCCESS)
			ctrl->slot_id = slot_id;
		else
			ctrl->slot_id = 0;
		break;

	case TRB_ADDR_DEV:
		ctrl->devs[slot_id]->cmd_status = ((le32_to_cpu(event->status) & (0xff << 24)) >> 24);
		break;

	case TRB_RESET_DEV:
		break;

	case TRB_EVAL_CONTEXT:
		ctrl->devs[slot_id]->cmd_status = GET_COMP_CODE(le32_to_cpu(event->status));
		break;

	case TRB_CONFIG_EP:
		break;
	default:
		break;
	}

	inc_deq(ctrl, ctrl->cmd_ring, false);
}

static int xhci_handle_event(struct xhci_ctrl *ctrl)
{
	union xhci_trb *event;
	int update_ptrs = 1;
	unsigned long deq;
	u64 temp_64;

	if (!ctrl->event_ring || !ctrl->event_ring->dequeue)
		return -1;

	event = ctrl->event_ring->dequeue;

	switch ((le32_to_cpu(event->event_cmd.flags) & (0xfc00)) >> 10) {
	case TRB_COMPLETION:
		handle_cmd_completion(ctrl, &event->event_cmd);
		break;

	case TRB_PORT_STATUS:
		handle_port_status(ctrl, event);
		break;
	default:
		if ((le32_to_cpu(event->event_cmd.flags) & (0xfc00)) == 0)
			update_ptrs = 0;
		break;
	}

	if (update_ptrs) {
		inc_deq(ctrl, ctrl->event_ring, true);
	}

	temp_64 = xhci_readl_64(&ctrl->ir_set->erst_dequeue);
	deq = (u32)(ctrl->event_ring->dequeue);

	if (deq == 0)

	temp_64 &= (0xf);
	temp_64 |= ((u64) deq & (u64) ~(0xf));

	temp_64 |= (1 << 3);
	xhci_writel_64(&ctrl->ir_set->erst_dequeue, temp_64);

	return 1;
}

static int xhci_check_ownership(struct xhci_ctrl *ctrl)
{
	union xhci_trb *event;

	if (!ctrl->event_ring || !ctrl->event_ring->dequeue)
		return 0;

	event = ctrl->event_ring->dequeue;
	if ((le32_to_cpu(event->event_cmd.flags) & (1<<0)) != ctrl->event_ring->cycle_state)
		return 0;

	return 1;
}

int xhci3_poll(struct xhci_ctrl *ctrl)
{
	return xhci_check_ownership(ctrl);
}

int xhci3_queue_slot_control(struct xhci_ctrl *ctrl, u32 trb_type, u32 slot_id)
{

	u32 trb_fields[4];

	trb_fields[0] = 0;
	trb_fields[1] = 0;
	trb_fields[2] = 0;
	trb_fields[3] = ((trb_type << 10) |((slot_id & 0xff) <<	24));

	return queue_command(ctrl, trb_fields);
}

int xhci3_alloc_virt_device(struct usb_device *usbdev, int slot_id)
{
	u64 byte_64 = 0;
	struct xhci_virt_device *virt_dev;
	struct xhci_ctrl *ctrl = usbdev->controller;

	if (slot_id == 0 || ctrl->devs[slot_id]) {
		return -1;
	}

	ctrl->devs[slot_id] = (struct xhci_virt_device *) malloc(sizeof(struct xhci_virt_device));

	if (!ctrl->devs[slot_id]) {
		return -1;
	}

	memset(ctrl->devs[slot_id], 0, sizeof(struct xhci_virt_device));
	virt_dev = ctrl->devs[slot_id];

	virt_dev->out_ctx = xhci_alloc_container_ctx(ctrl, XHCI_CTX_TYPE_DEVICE);
	if (!virt_dev->out_ctx) {
		return -1;
	}

	virt_dev->in_ctx = xhci_alloc_container_ctx(ctrl, XHCI_CTX_TYPE_INPUT);
	if (!virt_dev->in_ctx) {
		return -1;
	}

	virt_dev->eps[0].ring = xhci_ring_alloc(1, true);
	if (!virt_dev->eps[0].ring) {
		return -1;
	}

	byte_64 = (uintptr_t)(virt_dev->out_ctx->bytes);

	ctrl->dcbaa->dev_context_ptrs[slot_id] = byte_64;

	return 0;
}

void xhci3_endpoint_copy(struct xhci_ctrl *ctrl,struct xhci_container_ctx *in_ctx,
			struct xhci_container_ctx *out_ctx,unsigned int ep_index)
{
	struct xhci_ep_ctx *out_ep_ctx;
	struct xhci_ep_ctx *in_ep_ctx;

	out_ep_ctx = xhci3_get_ep_ctx(ctrl, out_ctx, ep_index);
	in_ep_ctx = xhci3_get_ep_ctx(ctrl, in_ctx, ep_index);

	in_ep_ctx->ep_info = out_ep_ctx->ep_info;
	in_ep_ctx->ep_info2 = out_ep_ctx->ep_info2;
	in_ep_ctx->deq = out_ep_ctx->deq;
	in_ep_ctx->tx_info = out_ep_ctx->tx_info;
}

void xhci3_slot_copy(struct xhci_ctrl *ctrl, struct xhci_container_ctx *in_ctx,
					struct xhci_container_ctx *out_ctx){

	struct xhci_slot_ctx *in_slot_ctx;
	struct xhci_slot_ctx *out_slot_ctx;

	in_slot_ctx = xhci3_get_slot_ctx(ctrl, in_ctx);
	out_slot_ctx = xhci3_get_slot_ctx(ctrl, out_ctx);

	in_slot_ctx->dev_info = out_slot_ctx->dev_info;
	in_slot_ctx->dev_info2 = out_slot_ctx->dev_info2;
	in_slot_ctx->tt_info = out_slot_ctx->tt_info;
	in_slot_ctx->dev_state = out_slot_ctx->dev_state;
}

int xhci3_queue_address_device(struct xhci_ctrl *ctrl, u8 *in_ctx_ptr, u32 slot_id)
{
	u64 val_64 = 0;
	u32 trb_fields[4];

	val_64 = (uintptr_t)in_ctx_ptr;
	trb_fields[0] = lower_32_bits(val_64);
	trb_fields[1] = upper_32_bits(val_64);
	trb_fields[2] = 0;
	trb_fields[3] = ((TRB_ADDR_DEV << 10) |	((slot_id & 0xff) << 24));

	return queue_command(ctrl, trb_fields);
}

void xhci3_copy_ep0_dequeue_into_input_ctx(struct xhci_ctrl *ctrl)
{
	struct xhci_virt_device *virt_dev;
	struct xhci_ep_ctx	*ep0_ctx;
	struct xhci_ring	*ep_ring;

	virt_dev = ctrl->devs[ctrl->slot_id];
	ep0_ctx = xhci3_get_ep_ctx(ctrl, virt_dev->in_ctx, 0);
	ep_ring = virt_dev->eps[0].ring;
	ep0_ctx->deq = cpu_to_le64(trb3_addr(ep_ring->enq_seg, ep_ring->enqueue) | ep_ring->cycle_state);
}

int xhci3_setup_addressable_virt_dev(struct usb_device *usbdev)
{
	struct xhci_virt_device *virt_dev;
	struct xhci_ep_ctx *ep0_ctx;
	struct xhci_slot_ctx *slot_ctx;
	u32 port_num = 0;
	u64 trb_64 = 0;

	struct xhci_ctrl *ctrl = usbdev->controller;

	virt_dev = ctrl->devs[ctrl->slot_id];

	if (ctrl->slot_id == 0 || !virt_dev) {
		return -EINVAL;
	}

	ep0_ctx = xhci3_get_ep_ctx(ctrl, virt_dev->in_ctx, 0);
	slot_ctx = xhci3_get_slot_ctx(ctrl, virt_dev->in_ctx);

	slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(1) | 0);

	switch (ctrl->speed) {

	case (0x3):
		slot_ctx->dev_info |= cpu_to_le32(SLOT_SPEED_SS);
		break;
	case (0x2):
		slot_ctx->dev_info |= cpu_to_le32(SLOT_SPEED_HS);
		break;
	case 0x00:
		slot_ctx->dev_info |= cpu_to_le32(SLOT_SPEED_FS);
		break;
	case 0x1:
		slot_ctx->dev_info |= cpu_to_le32(SLOT_SPEED_LS);
		break;
	default:
		BUG();
	}

	port_num = ctrl->port_id;

	slot_ctx->dev_info2 |= cpu_to_le32(((port_num & (0xff)) << 16));
	ep0_ctx->ep_info2 = cpu_to_le32(4 << 3);

	switch (ctrl->speed) {
	case (0x3):
		ep0_ctx->ep_info2 |= cpu_to_le32(((512 & (0xffff)) << 16));
		break;
	case (0x2):
	case 0x00:
		ep0_ctx->ep_info2 |= cpu_to_le32(((64 & (0xffff)) << 16));
		break;
	case 0x1:
		ep0_ctx->ep_info2 |= cpu_to_le32(((8 & (0xffff)) << 16));
		break;
	default:
		BUG();
	}
	ep0_ctx->ep_info2 |= cpu_to_le32(((0 & (0xff)) << 8) | ((3 & 0x3) << 1));

	trb_64 = (uintptr_t)virt_dev->eps[0].ring->first_seg->trbs;
	ep0_ctx->deq = cpu_to_le64(trb_64 | virt_dev->eps[0].ring->cycle_state);


	return 0;
}

void xhci3_poll_and_HandleEvent(struct usb_device *usbdev)
{
	unsigned long ts;
	struct xhci_ctrl *ctrl = usbdev->controller;
	int event_occured = 0;
	union xhci_trb *event;
	union xhci_trb	*first_trb;

	event = ctrl->event_ring->dequeue;

	ts = get_timer(0);

	do {
		if (xhci3_poll(ctrl)) {
			event_occured = 1;
			break;
		}
	} while (get_timer(ts) < USB_CTRL_SET_TIMEOUT);

	if (event_occured) {
		while (((le32_to_cpu(event->event_cmd.flags) & (0xfc00)) >>10) == TRB_PORT_STATUS) {
			if (xhci_handle_event(ctrl) < 0) {
				return;
			}
			first_trb = ctrl->event_ring->first_seg->trbs;
			printf("Address of first trb started at %p\n",first_trb);
			event = ctrl->event_ring->dequeue;
		}
		if (xhci_handle_event(ctrl) < 0) {
			return;
		}
		usbdev->act_len = 0;
		usbdev->status = 0;
	} 
	return;
}

int xhci3_address_device(struct usb_device *usbdev)
{
	int ret = 0;
	struct xhci_ctrl *ctrl = usbdev->controller;
	struct xhci_slot_ctx *slot_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_virt_device *virt_dev;
	int slot_id = ctrl->slot_id;

	virt_dev = ctrl->devs[slot_id];

	slot_ctx = xhci3_get_slot_ctx(ctrl, virt_dev->in_ctx);


	if (!slot_ctx->dev_info) {
		xhci3_setup_addressable_virt_dev(usbdev);
	} else {
		xhci3_copy_ep0_dequeue_into_input_ctx(ctrl);
	}

	ctrl_ctx = xhci3_get_input_control_ctx(virt_dev->in_ctx);
	ctrl_ctx->add_flags = cpu_to_le32((1 << 0) | (1 << 1));
	ctrl_ctx->drop_flags = 0;

	ret = xhci3_queue_address_device(ctrl, virt_dev->in_ctx->bytes,	slot_id);

	xhci3_ring_cmd_db(ctrl);
	mdelay(5);
	xhci3_poll_and_HandleEvent(usbdev);

	switch (virt_dev->cmd_status) {
	case COMP_CTX_STATE:
	case COMP_EBADSLT:
		ret = -EINVAL;
		break;
	case COMP_TX_ERR:
		ret = -EPROTO;
		break;
	case COMP_DEV_ERR:
		ret = -ENODEV;
		break;
	case COMP_SUCCESS:
		break;
	default:
		ret = 0;
		break;
	}
	if (ret < 0)
		return ret;

	slot_ctx = xhci3_get_slot_ctx(ctrl, virt_dev->out_ctx);

	virt_dev->address = (le32_to_cpu(slot_ctx->dev_state) &	(0xff)) + 1;

	ctrl_ctx->add_flags = 0;
	ctrl_ctx->drop_flags = 0;

	return 0;
}

int xhci3_alloc_dev(struct usb_device *usbdev)
{
	struct xhci_ctrl *ctrl = usbdev->controller;
	int ret;
	ret = xhci3_queue_slot_control(ctrl, TRB_ENABLE_SLOT, 0);
	if (ret)
		return -1;
	xhci3_ring_cmd_db(ctrl);
	mdelay(5);
	xhci3_poll_and_HandleEvent(usbdev);
	if (!ctrl->slot_id) 
		return -1;
	xhci3_alloc_virt_device(usbdev, ctrl->slot_id);
	return 0;
}
int
submit3_control_msg(struct usb_device *usbdev, unsigned long pipe, void *buffer,
					int length, struct devrequest *setup)
{
	struct xhci_ctrl *ctrl = usbdev->controller;
	int ret = -1;

	if (usb_pipetype(pipe) != 2) 
		return -1;
	
	if (setup->request == (0x09)) {
		if (usbdev->devnum > 1) {
			xhci3_submit_root(usbdev, pipe, buffer, length, setup);
			return xhci3_submit_async(usbdev, pipe, buffer,	length, setup);
		} else {
			usbdev->status = 0;
			usbdev->act_len = length;
			return 0;
		}
	}
	if (usb_pipedevice(pipe) == ctrl->rootdev3) {
		if (ctrl->rootdev3 == 0)
			usbdev->speed = (0x3);

		return xhci3_submit_root(usbdev, pipe, buffer, length, setup);
	}

	if ((setup->request == (0x05))){
		return xhci3_submit_root(usbdev, pipe, buffer, length, setup);
	}
	
	ret = xhci3_submit_async(usbdev, pipe, buffer, length, setup);

	return ret;
}

int xhci3_mem_init(int index, struct xhci_hccr *hccr, struct xhci_hcor *hcor)
{
	uint64_t val_64;
	uint64_t trb_64;
	uint32_t val;
	uint32_t temp;
	unsigned long deq;
	int i;
	struct xhci_segment *seg;
	struct xhci_ctrl *ctrl = &xhcic_3[index];

	ctrl->dcbaa = (struct xhci_device_context_array *)
			xhci3_malloc(sizeof(struct xhci_device_context_array));
	if (ctrl->dcbaa == NULL) 
		return -1;

	val_64 = (uintptr_t)ctrl->dcbaa;
	xhci_writel_64(&hcor->or_dcbaap, val_64);

	ctrl->cmd_ring = xhci_ring_alloc(1, true);
	if (ctrl->cmd_ring == NULL) {
		free(ctrl->dcbaa);
		return -1;
	}

	trb_64 = (uintptr_t)ctrl->cmd_ring->first_seg->trbs;
	val_64 = xhci_readl_64(&hcor->or_crcr);
	val_64 = (val_64 & (u64) (0x3f)) |
		(trb_64 & (u64) ~ (0x3f)) |
		ctrl->cmd_ring->cycle_state;
	xhci_writel_64(&hcor->or_crcr, val_64);

	val = xhci_readl(&hccr->cr_dboff);
	val &= DBOFF_MASK;
	ctrl->dba = (struct xhci_doorbell_array *)((char *)hccr + val);

	val = xhci_readl(&hccr->cr_rtsoff);
	val &= RTSOFF_MASK;
	ctrl->run_regs = (struct xhci_run_regs *)((char *)hccr + val);

	ctrl->ir_set = &ctrl->run_regs->ir_set[0];

	ctrl->event_ring = xhci_ring_alloc(3, false);
	ctrl->erst.entries = (struct xhci_erst_entry *)
		xhci3_malloc(sizeof(struct xhci_erst_entry) * 3);

	ctrl->erst.num_entries = 3;

	for (val = 0, seg = ctrl->event_ring->first_seg;
			val < 3;
			val++) {
		trb_64 = 0;
		trb_64 = (uintptr_t)seg->trbs;
		struct xhci_erst_entry *entry = &ctrl->erst.entries[val];
		xhci_writel_64(&entry->seg_addr, trb_64);
		entry->seg_size = cpu_to_le32(64);
		entry->rsvd = 0;
		seg = seg->next;
	}

	deq = (unsigned long)ctrl->event_ring->dequeue;

	temp = xhci_readl_64(&ctrl->ir_set->erst_dequeue);
	temp &= (0xf);

	temp &= ~(1 << 3);
	xhci_writel_64(&ctrl->ir_set->erst_dequeue,(((u64) deq & (u64) ~(0xf)) | temp));

	val = xhci_readl(&ctrl->ir_set->erst_size);
	val &= (0xffff << 16);
	val |= 3;
	xhci_writel(&ctrl->ir_set->erst_size, val);

	val_64 = xhci_readl_64(&ctrl->ir_set->erst_base);
	val_64 &= (0xf);
	val_64 |= ((u32)(ctrl->erst.entries) & ~(0xf));

	xhci_writel_64(&ctrl->ir_set->erst_base, val_64);
	for (i = 0; i < MAX_HC_SLOTS; ++i)
		ctrl->devs[i] = NULL;

	return 0;
}

int xhci3_hcd_init(int index, struct xhci_hccr **ret_hccr,struct xhci_hcor **ret_hcor)
{


        struct xhci_hccr *hccr;
        struct xhci_hcor *hcor;
	
        hccr = (struct xhci_hccr *)(SHIKHARA_USB3_HOST_BASE);
        hcor = (struct xhci_hcor *)((uint32_t) hccr + HC_LENGTH(xhci_readl(&hccr->cr_capbase)));
        
	*ret_hccr = hccr;
        *ret_hcor = hcor;
	
	/*


		For ASIC Setup:
				- it needs to be configured as per system controller settings.
	*/
	
        return 0;


}
int usb3_lowlevel_init(int index, void **controller)
{
	uint32_t val;
	uint32_t val2;
	uint32_t temp;
	uint32_t reg;
	struct xhci_hccr *hccr;
	struct xhci_hcor *hcor;
	struct xhci_ctrl *ctrl;
	
	if (xhci3_hcd_init(index, &hccr, (struct xhci_hcor **)&hcor) != 0)
		return -ENODEV;

	if (xhci3_halt(hcor) != 0)
		return -ENODEV;

	if (xhci3_reset(hcor) != 0)
		return -ENODEV;

	ctrl = &xhcic_3[index];
	ctrl->hccr = hccr;
	ctrl->hcor = hcor;
	val = (xhci_readl(&hccr->cr_hcsparams1) & HCS_SLOTS_MASK);
	val2 = xhci_readl(&hcor->or_config);
	val |= (val2 & ~HCS_SLOTS_MASK);
	xhci_writel(&hcor->or_config, val);

	if (xhci3_mem_init(index, hccr, hcor) < 0)
		return -ENOMEM;

	temp = xhci_readl(&hcor->or_dnctrl);
	temp &= ~(0xffff);
	temp |= DEV_NOTE_FWAKE;
	xhci_writel(&hcor->or_dnctrl, temp);

	reg = xhci_readl(&hccr->cr_hcsparams1);
	descriptor.hub.bNbrPorts = ((reg & HCS_MAX_PORTS_MASK) >> HCS_MAX_PORTS_SHIFT);
	printf("Register %x NbrPorts %d\n", reg, descriptor.hub.bNbrPorts);
	if (HCS_INDICATOR(reg))
		descriptor.hub.wHubCharacteristics |= 0x80;
	if (HCC_PPC(reg))
		descriptor.hub.wHubCharacteristics |= 0x01;
	else
		descriptor.hub.wHubCharacteristics |= 0x02;

	descriptor.hub.wHubCharacteristics |= 0x08;

	if (xhci_start(hcor)) {
		xhci3_halt(hcor);
		return -ENODEV;
	}

	temp = xhci_readl(&ctrl->ir_set->irq_control);
	temp &= ~(0xffff);
	temp |= (u32) 160;
	xhci_writel(&ctrl->ir_set->irq_control, temp);

	temp = xhci_readl(&hcor->or_usbcmd);
	temp |= (1 << 2);
	xhci_writel(&hcor->or_usbcmd, temp);

	temp = xhci_readl(&ctrl->ir_set->irq_pending);
	xhci_writel(&ctrl->ir_set->irq_pending, ER_IRQ_ENABLE(temp));

	reg = HC_VERSION(xhci_readl(&hccr->cr_capbase));
	printf("USB XHCI %x.%02x\n", reg >> 8, reg & 0xff);

	ctrl->rootdev3 = 0;
	*controller = ctrl;
	return 0;
}

int usb3_lowlevel_stop(int index)
{
	struct xhci_ctrl *ctrl = (xhcic_3 + index);

	xhci3_halt(ctrl->hcor);
	xhci3_reset(ctrl->hcor);
	xhci3_cleanup(ctrl);

	return 0;
}

static inline char *portspeed(int speed)
{
	if (speed == (0x3))
		return "5 Gb/s";
	else if (speed == (0x2))
		return "480 Mb/s";
	else if (speed == 0x1)
		return "1.5 Mb/s";
	else
		return "12 Mb/s";
}

int do_usbmintest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	int i;
	extern char usb3_started;
	if (argc < 2)
		return CMD_RET_USAGE;
	if (strncmp(argv[1], "mintest", 15) == 0){
		printf("start USB...\n");
		i = usb3_init();
		if (i >= 0) {
		}
		return 0;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	usb_get,	5,	1,	do_usbmintest,
	"USB minimum test",
	"usb_get mintest - Testing the USB devices...\n"
);

