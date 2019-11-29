//***************************************************************************
// BTSS base address and BTSS register base address
//***************************************************************************
//Bluetooth Baseband controller registers

     #define BTSS_BB_BASE_ADDR  	  0xD4541000    // BTSS_BB baseband controller base address
     #ifdef SHIKHARA_DMA_CPU_MERGE1
     #define DDR_BT_INIT         	  0x096e4000    // BTSS_BB baseband controller DDR address
     #define DDR_BT_FINL         	  0x096e7000    // BTSS_BB baseband controller DDR address
     #elif SHIKHARA_DMA_CPU_MERGE2
     #define DDR_BT_INIT         	  0x096e5000    // BTSS_BB baseband controller DDR address
     #define DDR_BT_FINL         	  0x096e6000    // BTSS_BB baseband controller DDR address
     #elif SHIKHARA_DMA_CPU_MERGE3
     #define DDR_BT_INIT         	  0x096e8000    // BTSS_BB baseband controller DDR address
     #define DDR_BT_FINL         	  0x096e9000    // BTSS_BB baseband controller DDR address
     #elif SHIKHARA_DMA_CPU_MERGE4
     #define DDR_BT_INIT         	  0x096ea000    // BTSS_BB baseband controller DDR address
     #define DDR_BT_FINL         	  0x096eb000    // BTSS_BB baseband controller DDR address
     #else
     #define DDR_BT_INIT         	  0x00040000    // BTSS_BB baseband controller DDR address
     #define DDR_BT_FINL         	  0x00041000    // BTSS_BB baseband controller DDR address
     #endif
#define BTSS_BB_INSTR_REG                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x00 )))        // (0x00<<1)))  	
#define BTSS_BB_MODE_STS                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x00 )))        // (0x00<<1)))  
#define BTSS_BB_CONNECTOR                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x04 )))        // (0x02<<1)))  	
#define BTSS_BB_PLD_HEAD                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x08 )))        // (0x04<<1)))	  
#define BTSS_BB_RX_STS                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x0C )))        // (0x06<<1)))	
#define BTSS_BB_TX_STS                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x10 )))        // (0x08<<1)))	
#define BTSS_BB_REMOTE_PARITY_BITS_REGISTER3        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x14 )))        // (0X0A<<1)))
#define BTSS_BB_TXFIFO_STS                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x14 )))        // (0x0A<<1)))	
#define BTSS_BB_LOCAL_PARITY_BITS_REGISTER3         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x14 )))        // (0X0A<<1)))
#define BTSS_BB_RXFIFO1_STS                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x18 )))        // (0x0C<<1)))	
#define BTSS_BB_RXFIFO2_STS                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1C )))        // (0x0E<<1)))	
#define BTSS_BB_INQ_PARITY1                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x20 )))        // (0x10<<1)))      
#define BTSS_BB_INQ_PARITY2                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x24 )))        // (0x12<<1)))  
#define BTSS_BB_INQ_LAP1                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x28 )))        // (0x14<<1)))  	
#define BTSS_BB_INQ_LAP2                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x2C )))        // (0x16<<1)))  	
#define BTSS_BB_OB_ADDR1                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x30 )))        // (0x18<<1)))	
#define BTSS_BB_REMOTE_PARITY_BITS_REGISTER1        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x30 )))        // (0X18<<1)))
#define BTSS_BB_REMOTE_PARITY_BITS_REGISTER2        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x34 )))        // (0X1A<<1)))
#define BTSS_BB_OB_ADDR2                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x34 )))        // (0x1A<<1)))	
#define BTSS_BADD_REG                               ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x38 )))        // (0x1C<<1)))    
#define BTSS_BB_OB_ADDR3                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x38 )))        // (0x1C<<1)))	
#define BTSS_BB_SLV_PARITY1                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x3C )))        // (0x1E<<1)))    
#define BTSS_BB_SLV_PARITY2                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x40 )))        // (0x20<<1)))	
#define BTSS_BB_BD_ADDR1                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x44 )))        // (0x22<<1)))	
#define BTSS_BB_LOCAL_PARITY_BITS_REGISTER1         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x44 )))        // (0X22<<1)))
#define BTSS_BB_LOCAL_PARITY_BITS_REGISTER2         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x48 )))        // (0X24<<1)))
#define BTSS_BB_BD_ADDR2                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x48 )))        // (0x24<<1)))	
#define BTSS_BB_BD_ADDR3                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x4C )))        // (0x26<<1)))	
#define BTSS_BB_OWN_PARITY1                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x50 )))        // (0x28<<1)))    
#define BTSS_BB_OWN_PARITY2                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x54 )))        // (0x2A<<1)))    
#define BTSS_BB_NINQUIRY_NPAGE                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x58 )))        // (0x2C<<1)))  	
#define BTSS_BB_INQUIRY_PAGE_TO                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x5C )))        // (0x2E<<1)))	
#define BTSS_BB_PS_INTERVAL                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x60 )))        // (0x30<<1)))   
#define BTSS_BB_PS_WINDOW                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x64 )))        // (0x32<<1)))  
#define BTSS_BB_IS_INTERVAL                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x68 )))        // (0x34<<1)))  
#define BTSS_BB_IS_WINDOW                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x6C )))        // (0x36<<1)))  
#define BTSS_BB_TO_STS                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x70 )))        // (0x38<<1)))
#define BTSS_BB_Tb                                  ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x74 )))        // (0x3A<<1)))	
#define BTSS_BB_Db                                  ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x78 )))        // (0x3C<<1)))	
#define BTSS_BB_DACCESS_NACCESS                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x7C )))        // (0x3E<<1)))	
#define BTSS_BB_TACCESS_DELTAb                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x80 )))        // (0x40<<1)))	
#define BTSS_BB_MACCESS_NPOLL                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x84 )))        // (0x42<<1)))	
#define BTSS_BB_AR_ADDR                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x88 )))        // (0x44<<1)))    
#define BTSS_BB_RETRANSMIT_CNT                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x8C )))        // (0x46<<1)))    
#define BTSS_BB_DSNIFF                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x90 )))        // (0x48<<1)))          
#define BTSS_BB_SNIFF_ATTEMPT                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x94 )))        // (0x4A<<1)))        
#define BTSS_BB_SNIFF_TO                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x98 )))        // (0x4C<<1)))        
#define BTSS_BB_TSNIFF                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x9C )))        // (0x4E<<1)))        
#define BTSS_BB_XVAL_TOL                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xA0 )))        // (0x50<<1)))        
#define BTSS_BB_PHOFF_REG                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xA4 )))        // (0x52<<1)))        
#define BTSS_BB_T_SCO                               ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xAC )))        // (0x56<<1)))	
#define BTSS_BB_E_SCO                               ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xAC )))        // (0x56<<1)))	
#define BTSS_BB_D_SCO                               ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xB0 )))        // (0x58<<1)))	
#define BTSS_BB_ESCO_RX                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xB4 )))        // (0x5A<<1)))
#define BTSS_BB_ESCO_END_STS                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xB4 )))        // (0x5A<<1)))
#define BTSS_BB_SNIFF_ESCO_STS                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xB8 )))        // (0x5C<<1)))
#define BTSS_BB_SCO_TYPE                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xBC )))        // (0x5E<<1)))
#define BTSS_BB_ESCO_FLUSH_LEN                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xC0 )))        // (0x60<<1)))	
#define BTSS_BB_CLK_OFFSET                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xC4 )))        // (0x62<<1)))	
#define BTSS_BB_PICO_CLK1                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xC8 )))        // (0x64<<1)))	
#define BTSS_BB_PICO_CLK2                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xCC )))        // (0x66<<1)))	
#define BTSS_BB_N_CLOCK1                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xD0 )))        // (0x68<<1)))	
#define BTSS_BB_NCLOCK1                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xD0 )))        // (0x68<<1)))
#define BTSS_BB_N_CLOCK2                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xD4 )))        // (0x6A<<1)))	
#define BTSS_BB_POC_REG                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xD8 )))        // (0x6C<<1)))    
#define BTSS_BB_WIN_REG                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xDC )))        // (0x6E<<1)))    
#define BTSS_BB_LUT_0_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xE0 )))        // (0x70<<1)))    
#define BTSS_BB_LUT_0_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xE4 )))        // (0x72<<1)))   
#define BTSS_BB_LUT_1_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xE8 )))        // (0x74<<1)))    
#define BTSS_BB_LUT_1_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xEC )))        // (0x76<<1)))    
#define BTSS_BB_LUT_2_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xF0 )))        // (0x78<<1)))    
#define BTSS_BB_LUT_2_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xF4 )))        // (0x7A<<1)))    
#define BTSS_BB_LUT_3_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xF8 )))        // (0x7C<<1)))    
#define BTSS_BB_LUT_3_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0xFC )))        // (0x7E<<1)))    
#define BTSS_BB_LUT_4_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x100 )))       // (0x80<<1)))    
#define BTSS_BB_LUT_4_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x104 )))       // (0x82<<1)))    
#define BTSS_BB_LUT_5_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x108 )))       // (0x84<<1)))    
#define BTSS_BB_LUT_5_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x10C )))       // (0x86<<1)))    
#define BTSS_BB_LUT_6_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x110 )))       // (0x88<<1)))    
#define BTSS_BB_LUT_6_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x114 )))       // (0x8A<<1)))    
#define BTSS_BB_LUT_7_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x118 )))       // (0x8C<<1)))    
#define BTSS_BB_LUT_7_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x11C )))       // (0x8E<<1))) 
#define BTSS_BB_LUT_81                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x120 )))       // (0x90<<1)))  
#define BTSS_BB_LUT_82                              ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x124 )))       // (0x92<<1)))  
#define BTSS_BB_TEST_MODE                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x128 )))       // (0x94<<1))) 
#define BTSS_BB_BROADCAST_FIFO                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x128 )))       // (0x94<<1)))	 
#define BTSS_BB_RX_FIFO_LOC                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x12C )))       // (0x96<<1)))	 
#define BTSS_BB_TX_FIFO_LOC                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x12C )))       // (0x96<<1)))	 
#define BTSS_BB_TXPATH_CON_REG                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x130 )))       // (0x98<<1)))
#define BTSS_BB_RXPATH_CON_REG                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x134 )))       // (0x9A<<1)))
#define BTSS_BB_BBRX_TOCPU_FIFO                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x13C )))       // (0x9E<<1))) 
#define BTSS_BB_CPU_TOPCMTX_FIFO                    ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x13C )))       // (0x9E<<1)))
#define BTSS_BB_CPU_TOBBTX_FIFO                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x140 )))       // (0xA0<<1)))
#define BTSS_BB_PCMRX_TOCPU_FIFO                    ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x140 )))       // (0xA0<<1)))  
#define BTSS_BB_RXPATH_STS_REG                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x144 )))       // (0xA2<<1)))
#define BTSS_BB_TXPATH_STS_REG                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x148 )))       // (0xA4<<1)))
#define BTSS_BB_SCATTER_FIFO                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x148 )))       // (0xA4<<1)))	 
#define BTSS_BB_ESCO_FIFO                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x14C )))       // (0xA6<<1))) 
#define BTSS_BB_FHS_REG                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x150 )))       // (0xA8<<1)))  
#define BTSS_BB_COD_REG                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x154 )))       // (0xAA<<1)))    
#define BTSS_BB_FHS_2_REG                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x154 )))       // (0xAA<<1)))
#define BTSS_BB_ENCRYP_EN                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x158 )))       // (0xAC<<1))) 	
#define BTSS_BB_ENCRYP_KEY                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x15C )))       // (0xAE<<1)))	

#define BTSS_BB_ENCRYP_1                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x160 )))       	
#define BTSS_BB_ENCRYP_2                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x164 )))       	
#define BTSS_BB_ENCRYP_3                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x168 )))       	
#define BTSS_BB_ENCRYP_4                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x16C )))       	
#define BTSS_BB_ENCRYP_5                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x170 )))       	
#define BTSS_BB_ENCRYP_6                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x174 )))       	
#define BTSS_BB_ENCRYP_7                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x178 )))       	

#define BTSS_BB_INTR_STATUS                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x17C )))       // (0xBE<<1)))    
#define BTSS_BB_INTR_MSK                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x180 )))       // (0xC0<<1)))    
#define BTSS_BB_INTR_CHK_REG                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x180 )))       // (0xC0<<1)))
#define BTSS_BB_THRESHOLD                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x184 )))       // (0xC2<<1))) 	
#define BTSS_BB_CH_ADDR                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x188 )))       // (0xC4<<1)))    
#define BTSS_BB_RADIO_DELAY_H                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x18C )))       // (0xC6<<1)))  
#define BTSS_BB_RADIO_DELAY                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x190 )))       // (0xC8<<1)))  
#define BTSS_BB_RF_PHD_H                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x194 )))       // (0xCA <<1))  // RW              
#define BTSS_BB_RF_PHD_F                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x198 )))       // (0xCC <<1))  // RW              
#define BTSS_BB_CORR_DIS_H                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x19C )))       // (0xCE <<1))  // RW              
#define BTSS_BB_TRIG_1STID                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1A0 )))       // (0xD0 <<1))  // RW              
#define BTSS_BB_TRIG_RECFHS                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1A4 )))       // (0xD2 <<1))  // RW              
#define BTSS_BB_TRIG_2NDID                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1A8 )))       // (0xD4 <<1))  // RW   
#define BTSS_BB_RADIO_CNTR                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1AC )))       // (0xD6<<1)))	
#define BTSS_BB_RADIO_SEL                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1B0 )))       // (0xD8<<1)))	
#define BTSS_BB_RADIO_DATA                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1B4 )))       // (0xDA<<1)))  
#define BTSS_BB_RADIO_CNFG                          ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1B8 )))       // (0xDC<<1)))  
#define BTSS_BB_HBUF_INTSTS_REG                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1BC )))       // (0xDE<<1)))
#define BTSS_BB_LOOPBACK                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1C0 )))       // (0xE0<<1)))  
#define BTSS_BB_IIS_CON_REG                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1C4 )))       // (0xE2<<1)))        
#define BTSS_BB_MAP_REG0                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1C8 )))       // (0xE4<<1)))	
#define BTSS_BB_MAP_REG1                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1CC )))       // (0xE6<<1)))	
#define BTSS_BB_MAP_REG2                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1D0 )))       // (0xE8<<1)))	
#define BTSS_BB_MAP_REG3                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1D4 )))       // (0xEA<<1)))	
#define BTSS_BB_MAP_REG4                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1D8 )))       // (0xEC<<1)))	
#define BTSS_BB_FIFO_CTRL                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1DC )))       // (0xEE<<1)))  
#define BTSS_BB_TEST_CONTROL                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1DC )))       // (0xEE<<1)))        
#define BTSS_BB_ESCO_FIFO_1                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1E0 )))       // (0xF0<<1))) 
#define BTSS_BB_IIS_PRESCALAR_REG1                  ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1E4 )))       // (0xF2<<1)))        
#define BTSS_BB_IIS_PRESCALAR_REG2                  ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1E8 )))       // (0xF4<<1)))        
#define BTSS_BB_MCLK_PRESCALAR_REG1                 ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1EC )))       // (0xF6<<1)))        
#define BTSS_BB_MCLK_PRESCALAR_REG2                 ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1F0 )))       // (0xF8<<1)))        
#define BTSS_BB_MCLK_PRESCALAR_REG3                 ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1F4 )))       // (0xFA<<1)))        
#define BTSS_BB_PCD_H_STS                           ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1F8 )))       // (0xFC<<1)))  
#define BTSS_BB_PCD_STS                             ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x1F8 )))       // (0xFC<<1)))	
#define BTSS_BB_LUT_8_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x200 )))       // (0x100<<1))
#define BTSS_BB_LUT_8_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x204 )))       // (0x102<<1)) 
#define BTSS_BB_LUT_9_LOWER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x208 )))       // (0x104<<1)) 
#define BTSS_BB_LUT_9_UPPER                         ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x20C )))       // (0x106<<1)) 
#define BTSS_BB_PN0_INFO                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x210 )))       // (0x108<<1))
#define BTSS_BB_PN1_INFO                            ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x214 )))       // (0x10A<<1))
#define BTSS_BB_PN1_CLK1_REG                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x218 )))       // (0x10C<<1)  // RW      
#define BTSS_BB_PN1_CLK2_REG                        ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x21C )))       // (0x10E<<1)  // RW      
#define BTSS_BB_PN1BADDR1_REG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x220 )))       // (0x110<<1)  // RW      
#define BTSS_BB_PN1BADDR2_REG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x224 )))       // (0x112<<1)  // RW      
#define BTSS_BB_PN1BADDR3_REG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x228 )))       // (0x114<<1)  // RW      
#define BTSS_BB_PN1_PARITY1_REG                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x22C )))       // (0x116<<1)  // RW      
#define BTSS_BB_PN1_PARITY2_REG                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x230 )))       // (0x118<<1)  // RW      
#define BTSS_BB_PN1_PARITY3_REG                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x234 )))       // (0x11A<<1)  // RW      
#define BTSS_BB_PRIORITY_CNTRL                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x238 )))       // (0x11C<<1)  // RW      
#define BTSS_BB_PRIORITY_CNTRL2                     ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x23C )))       // (0x11E<<1)  // RW      
#define BTSS_BB_CORR_CNFG_REG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x258 )))       // (0x12C<<1)  // RW      
#define BTSS_BB_RADIO_CNFG_REG                      ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x25C )))       // (0x12E<<1)  // RW      
#define BTSS_BB_P_PULSE_DELAY                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x260 )))       // (0x130<<1)  // RW      
#define BTSS_BB_SPI_MOD_CNTRL_REG                   ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x264 )))       // (0x132<<1)  // RW      
#define BTSS_BB_SPI_CNTRL_REG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x268 )))       // (0x134<<1)  // RW      
#define BTSS_BB_PTA_INTER_SIG                       ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x2A0 )))       // (0x150<<1)  // RW      
#define BTSS_BB_PTA_FUT_CHAN_STATUS                 ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x2A4 )))       // (0x152<<1)  // RW      
#define BTSS_BB_PTA_CUR_PKT_STATUS                  ((volatile unsigned int *)(BTSS_BB_BASE_ADDR +(0x2A8 )))       // (0x154<<1)  // RW    

// Registers for UART in BTSS 

    #define BTSS_UART_BASE_ADDR  	  0xD4549000    // BTSS UART base address


#define  BTSS_UART_DR        	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x000))
#define  BTSS_UART_RSR       	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x004))
#define  BTSS_UART_ECR       	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x004))
#define  BTSS_UART_TFR         	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x018))
#define  BTSS_UART_ILPR      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x020))
#define  BTSS_UART_IBRD      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x024))
#define  BTSS_UART_FBRD      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x028))
#define  BTSS_UART_LCR_H     	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x02c))
#define  BTSS_UART_CR        	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x030))
#define  BTSS_UART_IFLS      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x034))
#define  BTSS_UART_IMSC      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x038))
#define  BTSS_UART_RIS       	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x03C))
#define  BTSS_UART_MIS       	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x040))
#define  BTSS_UART_ICR       	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x044))
#define  BTSS_UART_DMACR     	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0x048))
#define  BTSS_UART_PID0      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFE0))
#define  BTSS_UART_PID1      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFE4))
#define  BTSS_UART_PID2      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFE8))
#define  BTSS_UART_PID3      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFEC))
#define  BTSS_UART_CID0      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFF0))
#define  BTSS_UART_CID1      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFF4))
#define  BTSS_UART_CID2      	  ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFF8))
#define  BTSS_UART_CID3           ((volatile unsigned int *)(BTSS_UART_BASE_ADDR + 0xFFC))

// Registers for Dual Timer in BTSS 

    #define BTSS_DT_BASE_ADDR         0xD4543000    // BTSS Dual timer base address

#define BTSS_DT_TIMER1_LOAD       ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x000))   //Timer1 Load Register
#define BTSS_DT_TIMER1_VALUE      ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x004))   //Timer1 Value Register
#define BTSS_DT_TIMER1_CONTROL    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x008))   //Timer1 Control Register
#define BTSS_DT_TIMER1_INTCLR     ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x00C))   //Timer1 Interrupt Clear Register
#define BTSS_DT_TIMER1_RIS        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x010))   //Timer1 Raw Interrupt Status Register
#define BTSS_DT_TIMER1_MIS        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x014))   //Timer1 Masked interrupt Status Register
#define BTSS_DT_TIMER1_BGLOAD     ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x018))   //Timer1 Background Load Register
#define BTSS_DT_TIMER2_LOAD       ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x020))   //Timer2 Load Register
#define BTSS_DT_TIMER2_VALUE      ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x024))   //Timer2 Value Register
#define BTSS_DT_TIMER2_CONTROL    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x028))   //Timer2 Control Register
#define BTSS_DT_TIMER2_INTCLR     ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x02C))   //Timer2 Interrupt Clear Register
#define BTSS_DT_TIMER2_RIS        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x030))   //Timer2 Raw Interrupt Status Register
#define BTSS_DT_TIMER2_MIS        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x034))   //Timer2 Masked interrupt Status Register
#define BTSS_DT_TIMER2_BGLOAD     ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0x038))   //Timer2 Background Load Register
#define BTSS_DT_TIMER_ITCR        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xF00))   //Integration Test Control Register
#define BTSS_DT_TIMER_ITOP        ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xF04))   //Integration Test Output set Register
#define BTSS_DT_TIMER_PeriphID0   ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFE0))   //Peripheral ID Register 
#define BTSS_DT_TIMER_PeriphID1   ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFE4))   //Peripheral ID Register
#define BTSS_DT_TIMER_PeriphID2   ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFE8))   //Peripheral ID Register
#define BTSS_DT_TIMER_PeriphID3   ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFEC))   //Peripheral ID Register
#define BTSS_DT_TIMER_PCellID0    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFF0))   //PrimeCell ID Register
#define BTSS_DT_TIMER_PCellID1    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFF4))   //PrimeCell ID Register
#define BTSS_DT_TIMER_PCellID2    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFF8))   //PrimeCell ID Register
#define BTSS_DT_TIMER_PCellID3    ((volatile unsigned int *)(BTSS_DT_BASE_ADDR + 0xFFC))   //PrimeCell ID Register

// Registers for Watch Dog Timer in BTSS 

    #define BTSS_WDT_BASE_ADDR        0xD4544000    // BTSS Watch Dog Timer base address

#define BTSS_WDT_LOAD            ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x000))   //Load Register
#define BTSS_WDT_VALUE           ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x004))   //Value Register
#define BTSS_WDT_CONTROL         ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x008))   //Control Register
#define BTSS_WDT_INTCLR          ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x00C))   //Interrupt Clear Register
#define BTSS_WDT_RIS             ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x010))   //Raw Interrupt Status Register
#define BTSS_WDT_MIS             ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0x014))   //Masked interrupt Status Register
#define BTSS_WDT_LOCK            ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xC00))   //Lock Register
#define BTSS_WDT_ITCR            ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xF00))   //Integration Test Control Register
#define BTSS_WDT_ITOP            ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xF04))   //Integration Test Output set Register
#define BTSS_WDT_PeriphID0       ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFE0))   //Peripheral ID Register 
#define BTSS_WDT_PeriphID1       ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFE4))   //Peripheral ID Register
#define BTSS_WDT_PeriphID2       ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFE8))   //Peripheral ID Register
#define BTSS_WDT_PeriphID3       ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFEC))   //Peripheral ID Register
#define BTSS_WDT_PCellID0        ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFF0))   //PrimeCell ID Register
#define BTSS_WDT_PCellID1        ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFF4))   //PrimeCell ID Register
#define BTSS_WDT_PCellID2        ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFF8))   //PrimeCell ID Register
#define BTSS_WDT_PCellID3        ((volatile unsigned int *)(BTSS_WDT_BASE_ADDR + 0xFFC))   //PrimeCell ID Register

//Registers for GPIO in BTSS

   #define BTSS_GPIO_BASE_ADDR      0xD4548000    // BTSS GPIO base address

#define BTSS_GPIO_DATA           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x3FC))
#define BTSS_GPIO_DIR            ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x400))
#define BTSS_GPIO_IS             ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x404))
#define BTSS_GPIO_IBE            ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x408))
#define BTSS_GPIO_IEV            ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x40C))
#define BTSS_GPIO_IE             ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x410))
#define BTSS_GPIO_RIS            ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x414))
#define BTSS_GPIO_MIS            ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x418))
#define BTSS_GPIO_IC             ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x41C))
#define BTSS_GPIO_AFSEL          ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0x420))
#define BTSS_GPIO_PID0           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFE0))
#define BTSS_GPIO_PID1           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFE4))
#define BTSS_GPIO_PID2           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFE8))
#define BTSS_GPIO_PID3           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFEC))
#define BTSS_GPIO_CID0           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFF0))
#define BTSS_GPIO_CID1           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFF4))
#define BTSS_GPIO_CID2           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFF8))
#define BTSS_GPIO_CID3           ((volatile unsigned int *)(BTSS_GPIO_BASE_ADDR + 0xFFC))

//Registers for I2C in BTSS
   #define BTSS_I2C_BASE_ADDR       0xD4546000    // BTSS I2C base address

#define BTSS_I2C_CON                ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x000))
#define BTSS_I2C_TAR                ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x004))
#define BTSS_I2C_SAR                ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x008))
#define BTSS_I2C_HS_MADDR           ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x00C))
#define BTSS_I2C_DATA_CMD           ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x010))
#define BTSS_I2C_SS_SCL_HCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x014))
#define BTSS_I2C_SS_SCL_LCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x018))
#define BTSS_I2C_FS_SCL_HCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x01C))
#define BTSS_I2C_FS_SCL_LCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x020))
#define BTSS_I2C_HS_SCL_HCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x024))
#define BTSS_I2C_HS_SCL_LCNT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x028))
#define BTSS_I2C_INTR_STAT          ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x02C))
#define BTSS_I2C_INTR_MASK          ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x030))
#define BTSS_I2C_RAW_INTR_STAT      ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x034))
#define BTSS_I2C_RX_TL              ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x038))
#define BTSS_I2C_TX_TL              ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x03C))
#define BTSS_I2C_CLR_INTR           ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x040))
#define BTSS_I2C_CLR_RX_UNDER       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x044))
#define BTSS_I2C_CLR_RX_OVER        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x048))
#define BTSS_I2C_CLR_TX_OVER        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x04C))
#define BTSS_I2C_CLR_RD_REQ         ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x050))
#define BTSS_I2C_CLR_TX_ABRT        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x054))
#define BTSS_I2C_CLR_RX_DONE        ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x058))
#define BTSS_I2C_CLR_ACTIVITY       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x05C))
#define BTSS_I2C_CLR_STOP_DET       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x060))
#define BTSS_I2C_CLR_START_DET      ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x064))
#define BTSS_I2C_CLR_GEN_CALL       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x068))
#define BTSS_I2C_ENABLE             ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x06C))
#define BTSS_I2C_STATUS             ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x070))
#define BTSS_I2C_TXFLR              ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x074))
#define BTSS_I2C_RXFLR              ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x078))
#define BTSS_I2C_TX_ABRT_SOURCE     ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x080))
#define BTSS_I2C_SLV_DATA_NACK_ONLY ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x084))
#define BTSS_I2C_DMA_CR             ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x088))
#define BTSS_I2C_DMA_TDLR           ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x08C))
#define BTSS_I2C_DMA_RDLR           ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x090))
#define BTSS_I2C_SDA_SETUP          ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x094))
#define BTSS_I2C_ACK_GENERAL_CALL   ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x098))
#define BTSS_I2C_ENABLE_STATUS      ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x09C))
#define BTSS_I2C_COMP_PARAM_1       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x0F4))
#define BTSS_I2C_COMP_VERSION       ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x0F8))
#define BTSS_I2C_COMP_TYPE          ((volatile unsigned int *)(BTSS_I2C_BASE_ADDR + 0x0FC))

//Registers for SPI in BTSS
   #define BTSS_SPI_BASE_ADDR          0xD4547000    // BTSS SPI base address

#define BTSS_SSP_CR0               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x00)) 
#define BTSS_SSP_CR1               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x04)) 
#define BTSS_SSP_DR                ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x08)) 
#define BTSS_SSP_SR                ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x0C)) 
#define BTSS_SSP_CPSR              ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x10)) 
#define BTSS_SSP_IMSC              ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x14)) 
#define BTSS_SSP_RIS               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x18)) 
#define BTSS_SSP_MIS               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x1C)) 
#define BTSS_SSP_ICR               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x20)) 
#define BTSS_SSP_DMACR             ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x24)) 
#define BTSS_SSP_TCR               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x080)) 
#define BTSS_SSP_ITIP              ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x084)) 
#define BTSS_SSP_ITOP              ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x088)) 
#define BTSS_SSP_TDR               ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0x08C)) 
#define BTSS_SSP_PeriphID0         ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFE0)) 
#define BTSS_SSP_PeriphID1         ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFE4)) 
#define BTSS_SSP_PeriphID2         ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFE8)) 
#define BTSS_SSP_PeriphID3         ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFEC)) 
#define BTSS_SSP_PCellID0          ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFF0)) 
#define BTSS_SSP_PCellID1          ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFF4)) 
#define BTSS_SSP_PCellID2          ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFF8)) 
#define BTSS_SSP_PCellID3          ((volatile unsigned int *)(BTSS_SPI_BASE_ADDR + 0xFFC)) 

//Registers for System controller in BTSS

   #define BTSS_SC_BASE_ADDR           0xD4542000    // BTSS SC base address

   #define BTSS_SC_REG0               ((volatile unsigned int *)(BTSS_SC_BASE_ADDR + 0x00)) 
   #define BTSS_SC_REG1               ((volatile unsigned int *)(BTSS_SC_BASE_ADDR + 0x04))

