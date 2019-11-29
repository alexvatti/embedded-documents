#ifndef __SHIKHARA_CLOCK_H__
#define __SHIKHARA_CLOCK_H__
#include <config.h>

#ifdef CONFIG_SHIKHARA_ASIC
	#define IC_CLK 			100000000
	#define PCLK 			100000000
	#define SSPCLK 			50000000
	#define CONFIG_PL011_CLOCK	24000000
	#define CAN_CLK 		24000000	/* Input clock to CAN is 24MHz */
	#define MMC_CLK			50000000	/* Clock to MMC Host is 50MHz */
	#define DT_CLOCK                100000000         /* Timers -> 1Mhz */
	#define WDT_CLOCK	        1000000	/* WDT */
	#define I2S_CLK                 1536000
	#define CLCDCLK			400000000
	#define HCLK			400000000
	#define KMIREFCLK		8000000 	/*8Mhz */
	//#define DSI_LANE_BYTE_CLOCK 	125000000 /*D-PHY lane byte clock*/ //commented for testing
#endif
#endif
