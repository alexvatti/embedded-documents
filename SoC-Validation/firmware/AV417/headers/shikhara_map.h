
/*#define CONFIG_SHIKHARA_FPGA*/
#define CONFIG_SHIKHARA_ASIC

#ifdef  CONFIG_SHIKHARA_ASIC
#define STACK_POINTER                 0xd41f1304
#define GLOBAL_POINTER                0xd41f1300
#define FRAME_POINTER                 0x0
#define SHIKHARA_SRAM_ADDR            0xd41f0000
#define SHIKHARA_UART0_BASE           0XD457B000
#define SHIKHARA_GPIO_BANK_A          0XD4576000
#define BG_EXT_BASE                   0x01500000
#define FG_EXT_BASE                   0x015FF200
#define BLENDED_EXT_BASE              0x01200000
#define SHIKHARA_GPIO_BANK_A_DIR      (SHIKHARA_GPIO_BANK_A+0x400)
#define SHIKHARA_GPIO_BANK_A_DATA     (SHIKHARA_GPIO_BANK_A+0x3fc)
#define SHIKHARA_CODEC_DDR_ADDR       0x01d00000
#endif

#ifdef  CONFIG_SHIKHARA_FPGA
#define STACK_POINTER                 0x5d1f1304
#define GLOBAL_POINTER                0x5d1f1300
#define FRAME_POINTER                 0x0
#define SHIKHARA_SRAM_ADDR            0x5d1f0000
#define SHIKHARA_UART0_BASE           0X5D57B000
#define SHIKHARA_GPIO_BANK_A          0x5D576000
#define BG_EXT_BASE                   0x41500000
#define FG_EXT_BASE                   0x415FF200
#define BLENDED_EXT_BASE              0x41200000
#define SHIKHARA_GPIO_BANK_A_DIR      (SHIKHARA_GPIO_BANK_A+0x400)
#define SHIKHARA_GPIO_BANK_A_DATA     (SHIKHARA_GPIO_BANK_A+0x3fc)
#define SHIKHARA_CODEC_DDR_ADDR       0x41d00000
#endif
