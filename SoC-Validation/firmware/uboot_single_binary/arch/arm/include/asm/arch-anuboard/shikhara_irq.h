#ifndef __SHIKHARA_IRQ_H__
#define __SHIKHARA_IRQ_H__


#ifdef CONFIG_SHIKHARA_ASIC
	#define SHIKHARA_MAX_IRQ   110
	#define IPC_AV417           56
	#define USB3_HOST_IRQ  	    52
	#define USB3_DRD_IRQ0	    53// not updated to latest plz check
	#define USB3_DRD_IRQ1 	    54// not updated to latest plz check
	#define MMC0_IRQ_NUM        70
	#define MMC1_IRQ_NUM        71 
	#define WDT_IRQ_NUM         72
	#define DT0_IRQ_NUM         73
	#define DT1_IRQ_NUM         74
	#define SMC_IRQ_NUM         75
	#define UART0_IRQ_NUM       76
	#define UART1_IRQ_NUM       77
	#define UART2_IRQ_NUM       78
	#define UART3_IRQ_NUM       79
	#define GPIO0_IRQ_NUM       88
	#define GPIO1_IRQ_NUM       89
	#define GPIO2_IRQ_NUM       90
	#define GPIO3_IRQ_NUM       91
	#define I2S_IRQ_NUM         92
	#define RTC_IRQ_NUM         93
	#define SPI0_IRQ_NUM        94
	#define SPI1_IRQ_NUM        95
	#define KMI_IRQ_NUM         96
	#define CAN_IRQ_NUM         97
	#define I2C0_IRQ_NUM        98
	#define I2C1_IRQ_NUM        99
	#define I2C2_IRQ_NUM        100
        #define GNSS_INT            34
#endif
#endif


