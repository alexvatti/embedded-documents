/* CONFIDENTIAL AND PROPRIETARY INFORMATION                        */
/* Copyright 2007 ARC International (Unpublished)                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* This document, material and/or software contains confidential   */
/* and proprietary information of ARC International and is         */
/* protected by copyright, trade secret and other state, federal,  */
/* and international laws, and may be embodied in patents issued   */
/* or pending.  Its receipt or possession does not convey any      */
/* rights to use, reproduce, disclose its contents, or to          */
/* manufacture, or sell anything it may describe.  Reverse         */
/* engineering is prohibited, and reproduction, disclosure or use  */
/* without specific written authorization of ARC International is  */
/* strictly forbidden.  ARC and the ARC logotype are trademarks of */
/* ARC International.                                              */

/* May contain code (c) British Telecommunications plc 2005        */


#ifndef	ARC_EE_ENCODER_TABLES_H
#define	ARC_EE_ENCODER_TABLES_H

#ifdef __cplusplus
extern "C" 
{
#endif 
#if 0
#ifndef ABS
#define ABS(x) (((x) < 0) ? (-(x)) : (x))
#endif
#endif


extern const unsigned char h264_encoder_intra_cbp_table[48];

extern const unsigned char h264_encoder_inter_cbp_table[48];


extern const unsigned int h264_total_coeff_trailing_ones_table0[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_table1[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_table2[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_table_DC_chroma[5][4];

extern const unsigned int h264_total_coeff_trailing_ones_lengths_table0[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_lengths_table1[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_lengths_table2[17][4];
extern const unsigned int h264_total_coeff_trailing_ones_length_table_DC_chroma[5][4];

extern const unsigned int h264_vlc0_table[31][2];

extern const unsigned int h264_total_zeros_1_coeff[16][2];
extern const unsigned int h264_total_zeros_2_coeff[15][2];
extern const unsigned int h264_total_zeros_3_coeff[14][2];
extern const unsigned int h264_total_zeros_4_coeff[13][2];
extern const unsigned int h264_total_zeros_5_coeff[12][2];
extern const unsigned int h264_total_zeros_6_coeff[11][2];
extern const unsigned int h264_total_zeros_7_coeff[10][2];
extern const unsigned int h264_total_zeros_8_coeff[9][2];
extern const unsigned int h264_total_zeros_9_coeff[8][2];
extern const unsigned int h264_total_zeros_10_coeff[7][2];
extern const unsigned int h264_total_zeros_11_coeff[6][2];
extern const unsigned int h264_total_zeros_12_coeff[5][2];
extern const unsigned int h264_total_zeros_13_coeff[4][2];
extern const unsigned int h264_total_zeros_14_coeff[3][2];
extern const unsigned int h264_total_zeros_15_coeff[2][2];

extern const unsigned int h264_total_zeros_dc_1_coeff[4][2];
extern const unsigned int h264_total_zeros_dc_2_coeff[3][2];
extern const unsigned int h264_total_zeros_dc_3_coeff[2][2];

extern const unsigned int vld_run_before_1_zeros_left[2][2];
extern const unsigned int vld_run_before_2_zeros_left[3][2];
extern const unsigned int vld_run_before_3_zeros_left[4][2];
extern const unsigned int vld_run_before_4_zeros_left[5][2];
extern const unsigned int vld_run_before_5_zeros_left[6][2];
extern const unsigned int vld_run_before_6_zeros_left[7][2];
extern const unsigned int vld_run_before_7_zeros_left[15][2];

//***********************
// MPEG4/H263 VLC tables
//***********************
#define ADD_BIT_OFFSET 30
#define STATUS_REG_OFFSET 0x19
#define BUFF_REG_OFFSET 0x0E
#define CURR_ADDR_OFFSET 0x14
#define RESIDUE_ONLY_MASK 0x10 
#define CN_EP_START 10
#define MAX_RUN    64
#define MAX_LEVEL  64

typedef struct RLTable {
    int n;                         ///< number of entries of table_vlc minus 1 
    int last;                      ///< number of values for last = 0 
    const unsigned int (*table_vlc)[2];
    const char *table_run;
    const char *table_level;
    unsigned char *index_run[2];         ///< encoding only 
    char *max_level[2];          ///< encoding & decoding 
    char *max_run[2];            ///< encoding & decoding 
} RLTable;

extern const unsigned char DCtab_chrom[13][2];
extern const unsigned char DCtab_lum[13][2];
extern unsigned char uni_DCtab_lum_len[512];
extern unsigned char uni_DCtab_chrom_len[512];
extern unsigned short uni_DCtab_lum_bits[512];
extern unsigned short uni_DCtab_chrom_bits[512];

extern RLTable rl_intra;
extern RLTable rl_inter;
extern RLTable rl_intra_aic;

extern unsigned int uni_mpeg4_intra_rl_bits[64*64*2*2];
extern unsigned char  uni_mpeg4_intra_rl_len [64*64*2*2];
extern unsigned int  uni_mpeg4_inter_rl_bits[64*64*2*2];
extern unsigned char  uni_mpeg4_inter_rl_len [64*64*2*2];
extern unsigned char  uni_h263_intra_aic_rl_len [64*64*2*2];
extern unsigned char  uni_h263_inter_rl_len [64*64*2*2];

extern const unsigned char intra_MCBPC_code[9];
extern const unsigned char intra_MCBPC_bits[9];
extern const unsigned char inter_MCBPC_code[28];
extern const unsigned char inter_MCBPC_bits[28];
extern const unsigned char cbpy_tab[16][2];
extern const unsigned char ff_zigzag_direct[64];
extern const unsigned char mvtab[33][2];
extern const int modified_quant[2][32];

void put_run_length(RLTable *rl, int last,int run,int level);
void Mp4TableInit();

inline int get_rl_index(const RLTable *rl, int last, int run, int level)
{
    int index;
    index = rl->index_run[last][run];
    if (index >= rl->n)
        return rl->n;
    if (level > rl->max_level[last][run])
        return rl->n;
    return index + level - 1;
}


#define UNI_MPEG4_ENC_INDEX(last,run,level) ((last)*128*64 + (run)*128 + (level))

#ifdef __cplusplus
}
#endif

#endif

/* ------------------------------------------------------------------------ */
/* END OF FILE																*/
/* ------------------------------------------------------------------------ */
