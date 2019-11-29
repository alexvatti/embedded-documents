#ifndef __SHIKHARA_MAP_H_
#define __SHIKHARA_MAP_H_
#include <config.h>

#ifdef CONFIG_SHIKHARA_ASIC
	#define CONFIG_ASIC_DDR_SIZE          (1024 * 1024 * 1024)
	#define CONFIG_ASIC_SDRAM_BASE        0x0
	#define CONFIG_ASIC_INIT_RAM_ADDR     0x0

	#define SHIKHARA_USB3_HOST_BASE       0XD4300000
	#define SHIKHARA_USB3_DRD_BASE        0XD4400000
	#define IPC_BASE_ADDR                 0XD45A0000
	#define SHIKHARA_CAN_BASE             0XD4587000
	#define SHIKHARA_CLCD_BASE            0XD4554000
	#define SHIKHARA_MMC0_BASE            0XD4555000
	#define SHIKHARA_MMC1_BASE            0XD4556000
	#define SHIKHARA_DMAC0_NSBASE         0XD4565000
	#define SHIKHARA_DMAC2_SBASE          0XD4568000
	#define SHIKHARA_DMAC2_NSBASE         0XD4569000
	#define SHIKHARA_DT0_BASE             0XD456E000
	#define SHIKHARA_DT1_BASE             0XD456F000
	#define SHIKHARA_WDT_BASE             0XD4571000
	#define SHIKHARA_SPI_BASE0            0XD4572000
	#define SHIKHARA_SPI_BASE1            0XD4573000
	#define SHIKHARA_KMI_BASE             0XD4575000
	#define SHIKHARA_GPIO_BANK_A 	      0XD4576000
	#define SHIKHARA_GPIO_BANK_B          0XD4577000
	#define SHIKHARA_GPIO_BANK_C          0XD4578000
	#define SHIKHARA_GPIO_BANK_D          0XD4579000
	#define SHIKHARA_UART0_BASE           0XD457B000
	#define SHIKHARA_UART1_BASE           0XD457C000
	#define SHIKHARA_UART2_BASE           0XD457D000
	#define SHIKHARA_UART3_BASE           0XD457E000
	#define SHIKHARA_UART4_BASE           0XD457F000
	#define SHIKHARA_I2S_BASE             0XD4581000
	#define SHIKHARA_IC_BASE              0xD4582000
	#define SHIKHARA_RTC_BASE             0xD45A3000
	/*GIC Distributor and Cpu Interface Base address */
	#define CONFIG_MPCORE_PRIVATE_BASE    0xD46F2000
	#define SHIKHARA_SMC_BASE	      0xD45A2000 	
	#define DSI_BB_BASE_ADDR 	      0xD45A7000 
	#define DSI_BASE_ADDR 		      0xD4562000
	#define DSI_PHY_BASE_ADDRESS	      (DSI_BASE_ADDR + 0x098)
	#define CSI2_BB_BASE_ADDR 	      0xD45A8000
	#define CSI2_BASE_ADDR 		      0xD4563000   
	#define HDMI_BB_BASE_ADDR   	      0xD45A6000   
	#define HDMI_BASE_ADDR 		      0xD4588000 
	#define CONFIG_HDMI_FB3_ADDR          0x01200000 
	#define CONFIG_MIPI_DSI_FB2_ADDR      0x01800000	
	#define CONFIG_CLCD_FB1_ADDR          0x01E00000	
	
	/* Flashes base address*/
        #define CONFIG_NORFLASH0              0xC0000000
        #define CONFIG_NORFLASH1              0xC4000000
        #define CONFIG_NORFLASH2              0xC8000000
        #define CONFIG_NORFLASH3              0xCC000000
	#define CONFIG_SYS_FLASH_BASE         CONFIG_NORFLASH0	/* We use generic driver for NOR */
        #define CONFIG_SMC_SRAM_BASE          CONFIG_NORFLASH1
	#define SHIKHARA_NANDCHIP_BASE        0xD0000000
	#define SHIKHARA_SYS_CTRL_BASEADDR    0xD4561000
	#define SHIKHARA_GPU_BASE	      0xD4590000
	#define TZASC0_BASE		      0xD456B000
	#define TZASC1_BASE                   0xD456C000
	#define TZASC2_BASE                   0xD456D000
	#define SHIKHARA_GPU_BASE             0xD4590000
	#define NVM_CTRL_BASE		      0xD45A9000
	#define CONFIG_ROM_ADDR               0XD4700004
        #define CONFIG_V7ONCHIP_SRAM_BASE     0XD45B0000 
#endif //CONFIG_SHIKHARA_ASIC




#define BOOT_STRAP                    (SHIKHARA_SYS_CTRL_BASEADDR+0xc4)
#define CONFIG_ONCHIP_SRAM_SIZE        0x10000  //64KB size


// Private Peripherals of mpcore memory map.
#define CONFIG_ARM_SCU                (CONFIG_MPCORE_PRIVATE_BASE +0x0)
#define SHIKHARA_GIC_INTERFACE        (CONFIG_MPCORE_PRIVATE_BASE +0x0100)
#define CONFIG_ARM_GTIMER             (CONFIG_MPCORE_PRIVATE_BASE +0x0200)
#define CONFIG_ARM_PRIVATE_TIMER      (CONFIG_MPCORE_PRIVATE_BASE +0x0600)
#define SHIKHARA_GIC_DISTRIBUTOR      (CONFIG_MPCORE_PRIVATE_BASE +0x1000)
#endif
