// SEMA_ADDR to pass message between handler and the C code 
#ifdef CONFIG_SHIKHARA_ASIC
      #ifdef SRAM
      #define SEMA_ADDR 0xD45BF000
      #else 
      #define SEMA_ADDR 0x0000F000
      #endif
      //BT                        
      #define BT_I2C_ID	      0X00000065        
      #define BT_SSP_ID	      0X00000066        
      #define BT_TIMER_ID     0X00000067        
      #define BT_WDT_ID	      0X00000068        
      #define BT_GPIO_ID      0X00000069        
      #define BT_UART_ID      0X0000006A        
      #define BT_ID           0X0000006B        
#endif 

//BT signals
extern unsigned int volatile * const BT_HOST_PCMTX_INTR_DONE;
extern unsigned int volatile * const BT_HOST_PCMRX_INTR_DONE;
extern unsigned int volatile * const BT_TX_STS_INTR_DONE ;
extern unsigned int volatile * const BT_RX_STS_INTR_DONE ;
extern unsigned int volatile * const BT_MODE_STS_INTR_DONE ;
extern unsigned int volatile * const BT_TIMEOUT_INTR_DONE ;
extern unsigned int volatile * const BT_HLC_INTR_DONE ;
extern unsigned int volatile * const BT_RADIO_INTR_DONE ;
extern unsigned int volatile * const BT_BB_TIMER_INTR_DONE ;
extern unsigned int volatile * const BT_SNIFF_INTR_DONE ;
extern unsigned int volatile * const BT_START_ESCO_INTR_DONE ;
extern unsigned int volatile * const BT_END_ESCO_INTR_DONE ;
extern unsigned int volatile * const BT_ACCESS_ERROR_INTR_DONE ;
extern unsigned int volatile * const BT_POWER_ON_INTR_DONE ;
extern unsigned int volatile * const BT_TX_FIFO_EMPTY_INTR_DONE ;

