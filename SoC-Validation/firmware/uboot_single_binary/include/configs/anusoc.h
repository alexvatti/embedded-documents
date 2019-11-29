/*
 * (C) Copyright 2013 Moschip Semiconductor
 *
 * Shikhara configuration settings
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_clock.h>

#ifndef __CONFIG_ANUSOC_H
#define __CONFIG_ANUSOC_H

	/* High Level Configuration Options  SOC*/
	#define CONFIG_SHIKHARA_ASIC

        #define CONFIG_ARMV7 /* CPU */
        #define CONFIG_SHIKHARA /* Board */
        #undef CONFIG_ENABLE_MMU /*Purposely disbaled data cache*/

        /* Keep L2 Cache Disabled */
        #define CONFIG_SYS_L2CACHE_OFF
        #define CONFIG_SYS_CACHELINE_SIZE       32

        /* DCACHE Flag */
        #define CONFIG_SYS_DCACHE_OFF           // If defined means DCACHE are disabled 
        #define CONFIG_SYS_LONGHELP

        /*Interrupt support flags */
        #define CONFIG_USE_IRQ
        #define CONFIG_STACKSIZE_IRQ            0x80000 //512KB is set for irq stack
        #define CONFIG_STACKSIZE_FIQ            4096

	#define CONFIG_NR_DRAM_BANKS            1
	#define PHYS_SDRAM_1                    CONFIG_ASIC_SDRAM_BASE
	#define CONFIG_SYS_SDRAM_BASE		CONFIG_ASIC_SDRAM_BASE
	#define PHYS_SDRAM_1_SIZE		CONFIG_ASIC_DDR_SIZE //0x3FFFFFFF
        #define CONFIG_SYS_MEMTEST_START 	PHYS_SDRAM_1
        #define CONFIG_SYS_MEMTEST_END 		(CONFIG_SYS_MEMTEST_START + \
                                            		PHYS_SDRAM_1_SIZE - (16 * 1024 * 1024))
	#define CONFIG_SYS_INIT_RAM_ADDR        CONFIG_ASIC_INIT_RAM_ADDR


        #define CONFIG_TC
        #define CONFIG_GIC_SHIKHARA
        #define CONFIG_SHIKHARA_DISPLAYS
        #define CONFIG_SHIKHARA_VALIDATION
        #define CONFIG_SHIKHARA_DDR_READY               1
        #undef CONFIG_SHIKHARA_PERI_EARLY_START 
        #undef CONFIG_RELOCATION_OFF

	/* MIPI-CSI CONFIGURATION */
        #define CONFIG_CSI              1

	/* MIPI-DSI CONFIGURATION */
	#define CONFIG_DSI_TRULY

	/* TO Enable DISPLAYS */
	#define CONFIG_SHIKHARA_DISPLAYS

        /* LCD CONFIGURATION INFORMATION*/
	#ifdef CONFIG_SHIKHARA_DISPLAYS
        	#define CONFIG_LCD                      1
        	//#define CONFIG_PL111                  1
        	/* LCD Fonts */
        	#define LCD_BPP                         LCD_COLOR24
        	/* Environmental Variable*/
        	#define CONFIG_PL111_BPP                24
        	#define CONFIG_LCD_INFO                 1                 
        	#define CONFIG_SYS_WHITE_ON_BLACK       1
        	#define CONFIG_LCD_INFO_BELOW_LOGO      1
        	/*To Support bmp display command*/
        	#define CONFIG_LCD_LOGO                 1                 
		#ifdef CONFIG_PL111                   
        		#define CONFIG_CMD_FLYER                1
		#endif
        	#define CONFIG_CMD_LCD                  1
		#ifdef CONFIG_LCD
        		#define CONFIG_CMD_BMP                  1
		#endif

        	#undef CONFIG_VIDEO_BMP_GZIP
        	#undef LCD_TEST_PATTERN
       	 	#undef CONFIG_SPLASH_SCREEN
        	#define CONFIG_CONSOLE_MUX              1
        	#define CONFIG_SYS_CONSOLE_IS_IN_ENV    1
        	#undef CONFIG_SILENT_CONSOLE
        	#undef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE     
        	#undef CONFIG_SYS_CONSOLE_INFO_QUIET  

		/* HDMI CONFIGURATION*/
        	#define CONFIG_HDMI
		#ifdef CONFIG_HDMI
        		/* HDMIDVIMIN COMMAND ENABLE/DISABLE */
        		#define CONFIG_HDMI_DVI_MIN             1
        		#undef INSERT_AUTO_PCUV
        		#define CONFIG_HDMI_AUDIO        
        		#undef HDCP_CONFIG      
        		/* HDMI console support */
        		#undef CONFIG_HDMI_CONSOLE
			#ifdef CONFIG_HDMI_AUDIO
        			#define CONFIG_HDMI_AUDIO_MIN   1
			#endif
		#endif

	        /*MIPI-DSI CONFIGURATION */
	        #undef CONFIG_DSI               
		#ifdef CONFIG_DSI
        		/* DSI console support */
        		#undef CONFIG_DSI_CONSOLE       
		#endif
        	/* MIPI DSICSI LOOPBACK */
        	#undef CONFIG_DSICSI_LOOPBACK   

        	/* Touch Screen Controller*/
		#ifdef CONFIG_PL111
        		#define CONFIG_TC
        	#endif
	#endif/*CONFIG_SHIKHARA_DISPLAYS*/


        /*Enable this flag to test GPU */
        #undef CONFIG_SHIKHARA_GPU
	#ifdef CONFIG_SHIKHARA_GPU
        	#define CONFIG_SHIKHARA_GPU_CORE
        	#define CONFIG_MALIGPU_GP
        	#define CONFIG_MALIGPU_PP0
        	#define CONFIG_MALIGPU_PP1
        	#define CONFIG_MALIGPU_PP2
        	#define CONFIG_MALIGPU_PP3
        	#define CONFIG_MALIGPU_MP1
        	#define CONFIG_MALIGPU_MP2
        	#define CONFIG_MALIGPU_MP3
	#endif/*endif for CONFIG_SHIKHARA_GPU check*/

	#undef CONFIG_GPU_SCENE
	#ifdef CONFIG_GPU_SCENE
        	#define CONFIG_SHIKHARA_GPU_CORE
	#endif


	/*  ARC AV417 configuration */
        #define CONFIG_CMD_AV417 
        #define CONFIG_CMD_IPC


        /*  GNSS configuration */
	#define CONFIG_GNSS
	#ifdef CONFIG_GNSS
        	#undef CONFIG_SHIKHARA_DDR_READY
        	#undef CONFIG_SYS_L2CACHE_OFF
	#endif

	#define CONFIG_PL011_SERIAL
	/* default boot is according to the bootmode switch settings */
        #define CONFIG_CONS_INDEX               0
        #define CONFIG_BOOTCOMMAND              "run modeboot"
        #define CONFIG_BAUDRATE                 115200
        #define CONFIG_SYS_BAUDRATE_TABLE       { 9600, 38400, 115200 }
        #define CONFIG_BOOTDELAY                3 /* -1 to Disable autoboot */
        #define CONFIG_PSS_SERIAL
        #define CONFIG_SYS_PROMPT               "shikhara-uboot>"


        /* NOR CONFIGURATION*/
        #undef CONFIG_SYS_NO_FLASH                              /* Define if u don't want flash*/
	#ifndef CONFIG_SYS_NO_FLASH
        	#define CONFIG_INIT_NOR                 1
        	#define CONFIG_CMD_NOR
        	#undef CONFIG_BOOT_XIP 
        	#define CONFIG_SYS_MAX_FLASH_BANKS      1
        	/* max number of sectors/blocks on one chip */
        	#define CONFIG_SYS_MAX_FLASH_SECT       516 
        	#define CONFIG_FLASH_SHOW_PROGRESS      10
        	#define CONFIG_FLASH_CFI_DRIVER         1
        	#define CONFIG_SYS_FLASH_CFI
        	#define CONFIG_SYS_FLASH_CFI_WIDTH      FLASH_CFI_16BIT
        	#undef CONFIG_SYS_FLASH_EMPTY_INFO
        	#define CONFIG_SYS_FLASH_PROTECTION             /* don't use hardware protection */
        	#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE       /* use buffered writes (20x faster) */
		#ifdef CONFIG_MTD
        		#define CONFIG_FLASH_CFI_MTD     1
		#endif
	#endif


        /*MMC configuration*/
	#define CONFIG_SHIKHARA_MMC
	#ifdef CONFIG_SHIKHARA_MMC
        	#define CONFIG_GENERIC_MMC
        	#define USE_MMC0
        	#define MMC_MAX_CHANNEL                 3
        	#define CONFIG_MMC
        	#define SHIKHARA_MMC_RAM_OFFSET         0x300000
        	//#define CONFIG_MMC_TRACE
       		#define CONFIG_CMD_MMC                  /*needed for mmc read/write commands from cmdline*/
        	#define CONFIG_ENV_IS_IN_MMC            /* assuming environment is NOT in mmc*/
        	#undef CONFIG_ENV_IS_IN_MMC
        	#define CONFIG_CMD_MMC_SPI              /*only needed if mmc is accesed through SPI and NOT a host controller*/
        	#undef CONFIG_CMD_MMC_SPI
	#endif


       /*FAT&DOS Support*/
        #define CONFIG_DOS_PARTITION            1
        #define CONFIG_CMD_FAT
        #define CONFIG_FAT_WRITE


        /*USB Configuration host*/
        #define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS      3
        #define CONFIG_USB                              1
	#ifdef CONFIG_USB
        	#define CONFIG_CMD_USB
        	#define CONFIG_USB_XHCI
        	#define CONFIG_USB_XHCI_SHIKHARA
        	#define CONFIG_USB_STORAGE
	#endif

        /*USB Configuration device*/
        #define CONFIG_USB_DEVICE                       1
	#ifdef CONFIG_USB_DEVICE
        	#define CONFIG_SHIKHARA_USBD3
        	#define SHIKHARA_USB_DEVICE_STORAGE_START_OFFSET       0xAA00000// 170 MB from DDR base
	#endif


	/* I2C CONFIGURATION */
        #define CONFIG_CMD_EEPROM               1 
        #define CONFIG_CMD_I2C                  1
        #define CONFIG_HARD_I2C                 1
        #define CONFIG_SHIKHARA_I2C
        #define CONFIG_I2C_MULTI_BUS

	 /*Sound Configuration*/
	#define CONFIG_SGTL5000_CODEC   1       
        #define CONFIG_CMD_SOUND
        #define CONFIG_SOUND
        #define CONFIG_I2S


        /* CAN Configurations */
        #define CONFIG_SHIKHARA_CAN
        #define CONFIG_CAN                      1
        #define CONFIG_CMD_CAN                  1
        #define CONFIG_CAN_SHIKHARA             1


        /*  KMI CONFIGURATION */
        #define CONFIG_KMI_MOS  
	#ifdef CONFIG_KMI_MOS
        	#define CONFIG_PS2KBD           1
        	#undef KMI_DRIVER_INTERRUPT_MODE
	#endif
        
	/*RTC CONFIGURATION*/
        #define CONFIG_RTC_SHIKHARA     1

	/*Configuration for GPIO */
        #define CONFIG_SHIKHARA_GPIO    1



	#define CONFIG_RELOCATION_OFF   			//1 with this flag relocation is off
    

	#undef CONFIG_YAFFS2 

	 /* Physical Memory map*/
	#if defined (CONFIG_SHIKHARA_ASIC)
		#define CONFIG_NVM
		#define CONFIG_NVM_CMD
	#endif

	#if defined(CONFIG_DSI_TRULY) || defined(CONFIG_SHIKHARA_GPU) || defined(CONFIG_GNSS) || defined(CONFIG_RELOCATION_OFF)
		#define CONFIG_RELOCATION_OFF   			//1 with this flag relocation is off
        	#define CONFIG_SYS_TEXT_BASE             0x3D000000
	#endif

	//#define CONFIG_LPDDR2_INIT              1
	//#define CONFIG_L2_CACHE  		1
	//#define CONFIG_CORE_LOWPOWER_MODE     1


	#define CONFIG_SYS_INIT_STACK_SIZE	0x100000         //stack size is set to 1MB
	#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
						CONFIG_SYS_INIT_STACK_SIZE - \
						GENERATED_GBL_DATA_SIZE)

	#define CONFIG_BL1_SP_ADDR    	       	(CONFIG_SYS_SDRAM_BASE+0x300000)            
	#define CONFIG_BL0_SP_ADDR             	(CONFIG_V7ONCHIP_SRAM_BASE+0xFA00)          
	#define CONFIG_SYS_MALLOC_LEN 	       	 0x400000
	#define CONFIG_BL_MALLOC_LEN           	 0x4000   //malloc size is set to 1 MB
	#define CONFIG_BL0_MALLOC_START         (CONFIG_V7ONCHIP_SRAM_BASE+0xA600)
	#define CONFIG_BL1_MALLOC_START        	(CONFIG_SYS_SDRAM_BASE+0x200000)
	#undef  CONFIG_BL1_IMAGE	
	
	#define CONFIG_SYS_MAXARGS 		16
	#define CONFIG_SYS_CBSIZE 	        2048
	#define CONFIG_SYS_PBSIZE              	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
	#define CONFIG_SYS_HZ			(DT_CLOCK/1000)
	#define CONFIG_SYS_LOAD_ADDR		0 /* default? */
	
	/* SRAM CONFIGURATION*/
	#define CONFIG_INIT_SRAM 1
	#define CONFIG_PL330	1
	#undef CONFIG_PL330_INTERRUPT_MODE

	/* NAND CONFIGURATION INFORMATION*/
	#define CONFIG_MTD                
	#define CONFIG_SHIKHARA_NAND         		1
	#define CONFIG_ANUSOC_NAND_BUSWIDTH_16
	#define CONFIG_SYS_NAND_ONFI_DETECTION
	#ifdef CONFIG_MTD
		#define CONFIG_SYS_MAX_NAND_DEVICE   	1
		#define CONFIG_MTD_DEVICE            	1
		#define CONFIG_MTD_PARTITIONS        	1
		#undef CONFIG_MTD_CONCAT         
		#define CONFIG_CMD_NAND	            	1
		#undef CONFIG_SYS_NAND_SELECT_DEVICE
	#endif


	/* NOR CONFIGURATION*/
	#ifdef CONFIG_SHIKHARA_ASIC
		#define SHIKHARA_SPI_USE_DMA
		#define SHIKHARA_SPI_CS_MANUAL	// define to operate CS line Manually
		#define CONFIG_SPI_PL022
		#define CONFIG_SPI_FLASH_STMICRO
		#define CONFIG_ENV_SIZE 		(256<<10)
		#define CONFIG_ENV_SECT_SIZE            CONFIG_ENV_SIZE
		#define CONFIG_ENV_OFFSET               0x80000 /*2nd sector on 256 KB sector size*/
	#endif

	#define CONFIG_CMD_SPI
	#define CONFIG_SPI_FLASH
	#define CONFIG_CMD_SF
	#define CONFIG_ENV_IS_IN_SPI_FLASH
	#define CONFIG_CMD_SAVEENV      /* Command to save ENV to Flash */
	#undef CONFIG_ENV_IS_NOWHERE
	#define CONFIG_ENV_OVERWRITE

	/* CSI-HDMI */
        #define HDMI_640_480 
        #undef HDMI_720_480 
        #define CONFIG_OV5640_RGB888
        #undef CONFIG_OV5640_RGB565

		
		
	#if defined(CONFIG_RELOCATE_VECTORS) && defined(CONFIG_RELOCATION_OFF)
		#define CONFIG_SHIKHARA_GPU
		#undef CONFIG_SHIKHARA_GPU
	#endif


	#undef CONFIG_SHIKHARA_POST
	#undef CONFIG_UART1
	#undef CONFIG_EXTRA_ENV_SETTINGS
	
	/* Default environment */
	#define CONFIG_IPADDR   		10.10.70.102
	#define CONFIG_SERVERIP 		10.10.70.101
	/*MEMORY DISPLAY COMMAND CONFIGURATION */
	#undef CONFIG_CMD_MEMORY           	
	/*Implements functiosn like my_memcpy for fast memcpy*/
	#define CONFIG_SHIKHARA_HELPER
	#define CONFIG_USE_ARCH_MEMSET
	#define CONFIG_USE_ARCH_MEMCPY
	
	#include <config_cmd_default.h>
	#define CONFIG_CMD_DATE         /* RTC? */
	#define CONFIG_CMD_PING         /* Might be useful for debugging */
	#define CONFIG_REGINFO          /* Again, debugging */
	#undef CONFIG_CMD_SETGETDCR     /* README says 4xx only */
	#define CONFIG_TIMESTAMP        /* print image timestamp on bootm, etc */
	#define CONFIG_PANIC_HANG /* For development/debugging */
	#define CONFIG_AUTO_COMPLETE
	#define CONFIG_CMDLINE_EDITING



	/* Validation configs */
	#ifdef CONFIG_SHIKHARA_VALIDATION
		#define CONFIG_KMI_MIN_TESTS
	       	#undef DMA_PERI
	       	#define CONFIG_UART_LOOPBACK_TEST
	       	#define CONFIG_USB_HOST_DUMPTEST 	1
	       	#define CONFIG_USB_DEV_DUMPTEST 	1
	       	#define CONFIG_USB_INTR_TEST    	1	
	       	#define CONFIG_CMD_DMATEST
	       	#define CONFIG_SPI_MINITEST
	       	#define CONFIG_SPI_SLAVE
	       	#define CONFIG_NORFLASH_TEST
	       	#define CONFIG_I2C_MINTEST  		1
	       	#define CONFIG_I2C_DUMP  		1
	       	#define CONFIG_I2C_MINTEST 		1
	      	#define CONFIG_SRAM_TEST 		1
	       	#define CONFIG_CAN_TEST  		1
	       	#define CONFIG_DT_TEST  		1
	       	#define CONFIG_WDT_TEST  		1
	       	#define CONFIG_GPU_TEST 		1
	       /*#define CONFIG_GPU_SCENE 		1*/
	       	#define CONFIG_PMU_TEST 		1
	       	#define CONFIG_USB_MINTEST 		1
	       	#define CONFIG_I2S_DUMP    		1	
	       	#define CONFIG_CMD_DWMMC_TEST 		1
	       	#define CONFIG_CMD_CAN_TEST 		1	
	       	#define CONFIG_NAND_TEST
	       	#define CONFIG_SPI_DUMP
	       	#define CONFIG_SPI_SLAVETEST	
	      	#define CONFIG_GIC_DUMP	
	       	#define CONFIG_TZASC
		#define CONFIG_SHIKHARA_POST	
	     // 	#define CONFIG_GNSS
		//#define CONFIG_GPU_SCENE
	#endif   /*Validation Configs*/

#endif /* __CONFIG_ANUBOARD_H */
