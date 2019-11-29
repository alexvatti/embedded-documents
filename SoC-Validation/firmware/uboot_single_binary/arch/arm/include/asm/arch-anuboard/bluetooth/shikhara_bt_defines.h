//***************************************************************************
// Copyright (c) 2012 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : bt_defines.h
//Title           :  
//Project         : SHIKHARA
//Owner           : 
//Date Created    : 28/12/2012
//***************************************************************************
//Description : 
//***************************************************************************

#ifndef __BT_DEFINES_H__
#define __BT_DEFINES_H__

// This was encryption bd address. 2 b removed from all vectors. neeta
#define    COD_REG                      0xAA    
#define    CH_ADDR                      0xC4    
#define    PHD_DELAY                    0xFF    
#define    RX_PHD_DELAY                 0xFF    
#define    CONN_TXON_DELAY              0xFF    
#define    CONN_RXON_DELAY              0xFF    
#define    TXON_DELAY                   0xFF    
#define    RXON_DELAY                   0xFF    
#define     nop                        0x00   
#define     start_inquiry              0x01   
#define     start_inq_scan             0x02   
#define     start_paging               0x03   
#define     start_page_scan            0x04   
#define     do_execute                 0x05   
#define     do_hold                    0x06   
#define     do_sniff                   0x07   
#define     start_beacon               0x08   
#define     unpark_req                 0x09   
#define     do_poll                    0x0a   
#define     do_sleep                   0x0b   
#define     do_flush                   0x0c   
#define     send_packet                0x0d   
#define     send_pkt_pri               0x12   
#define     unpark                     0x0e   
#define     soft_reset                 0x80   
#define     kill_inquiry               0x81   
#define     kill_page                  0x82   
#define     kill_hold                  0x83   
#define     kill_sniff                 0x84   
#define     kill_sleep                 0x85   
#define     kill_beacon                0x86   
#define     kill_beacon_sl             0x87   
#define     kill_broadcast             0x88   
#define     switch_piconet             0x10   
#define     BTSS_INTR                0x0   
#define     HOP_MATCH                0x1   
#define     HIGH                0x1   
#define     LOW                0x0   
#define     RST_DELAY                0x01   
#define     WATCH_DOG_CNT                0x02   
#define     DATA_DELAY                0x03   
#define     DATA_WIDTH                0x04   
#define     RST_CLK_VAL1                0x05   
#define     RST_CLK_VAL2                0x06   
//#define     inqTO1                0x10   
//#define     inqTO2               0x00   
//#define     pageTO1                0x10   
//#define     pageTO2                0x0   
#define     prTO1                0x08   
#define     prTO2                0x0   
#define     ncTO1                0xf   
#define     ncTO2                0x0   
#define     dsniff1                0x2   
#define     dsniff2                0x0   
#define     tsniff1                0xE   
#define     tsniff2                0x0   
#define     nsniff1                0x1   
#define     nsniff2                0x0   
#define     sniffTO1                0x0   
#define     sniffTO2                0x0   
#define     rand1                0x05   
#define     rand2                0x0   
//#define     hold_period1                0x1   
//#define     hold_period2                0x1   
#define     db1                0x2   
#define     db2                0x0   
#define     tb1                0x2C   
#define     tb2                0x0   
#define      nb1                0x2   
#define      nb2                0x0   
#define     deltab1                0x4   
#define     deltab2                0x0   
#define     daccess1                0x0a   
#define     daccess2                0x0   
#define     taccess1                0x2   
#define     taccess2                0x0   
#define     maccess_scheme1                0x4   
#define     maccess_scheme2                0x0   
#define     dsleep1                0x1   
#define     dsleep2                0x1   
#define     nsleep1                0x1   
#define     nsleep2                0x1   
#define     nacc_slot1                0x2   
#define     nacc_slot2                0x0   
#define     m_lap1                0x39   
#define     m_lap2                0x6b   
#define     m_lap3                0x61   
#define     m_uap                0x55   
#define     m_nap1                0x0   
#define     m_nap2                0x0   
#define     mas_par1                0x0   
#define     mas_par2                0x0   
#define     mas_par3                0x0   
#define     mas_par4                0x0   
#define     mas_par5                0x0   
#define     slv_par1                0x0   
#define     slv_par2                0x0   
#define     slv_par3                0x0   
#define     slv_par4                0x0   
#define     slv_par5                0x0   
#define     s_lap1                0x39   
#define     s_lap2                0x6b   
#define     s_lap3                0x61   
#define     s_uap                0x55   
#define     s_nap1                0x0   
#define     s_nap2                0x0   
#define     npoll1                0x4   
#define     npoll2                0x0   
#define     nbc                0x4   
#define     H_ACC                      0x01   
#define     H_PHD                      0x02   
#define     H_PLHD                     0x04   
#define     H_PLD                      0x08   
#define     H_CRC                      0x10   
// testbench related parameters.
#define     no_data_cmp_1         0x07   
#define     no_data_cmp_2         0x09   
#define     no_data_cmp_3         0x0b   
#define     no_data_cmp_4         0x07   
#define     no_data_cmp_5         0x09   
#define     no_data_cmp_6         0x0b   
#define     reset_codec_1         0x08   
#define     reset_codec_2         0x0a   
#define     reset_codec_3         0x0c   
#define     reset_codec_4         0x08   
#define     reset_codec_5         0x0a   
#define     reset_codec_6         0x0c   
#define     init_seq1                  0x21   
#define     init_seq2                  0x22   

#define BB_WRITE_REMOTE_PARITY_BITS(val) 

//   \
  //      diag_write_remote_parity_bit(val)

#define BB_WRITE_LOCAL_PARITY_BITS(val)    
//\
   //     diag_write_local_parity_bit(val)

#define      REMOTE_PARITY_BITS_REGISTER1           0X18
#define      REMOTE_PARITY_BITS_REGISTER2           0X1A
#define      REMOTE_PARITY_BITS_REGISTER3           0X0A

#define      LOCAL_PARITY_BITS_REGISTER1            0X22
#define      LOCAL_PARITY_BITS_REGISTER2            0X24
#define      LOCAL_PARITY_BITS_REGISTER3            0X0A

#define LC_PN_SEQ_MSB                       0x83848d96
#define LC_GEN_POLY_MSB                     0x615c4f6a
#define LC_GEN_POLY_LSB                     0x40000000
#define LOCAL                               0
#define SLAVE                               1
#define PN0_INFO                            0x0108
#define PN1_INFO                            0x010A


#endif // __BT_DEFINES_H__

