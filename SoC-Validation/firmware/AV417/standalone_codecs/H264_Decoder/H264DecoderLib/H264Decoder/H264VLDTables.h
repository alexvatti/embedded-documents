/* ------------------------------------------------------------------------ */
/* FILE:			CH264Decoder.h											*/
/* DESCRIPTION:		BT H.264 VLD Tables header file.						*/
/* VERSION:			v2.00													*/
/* DATE:			29 June 2005											*/
/* MODIFIED BY:	ARC International (UK) Limited								*/
/* MODIFIED DATE:	31 March 2006											*/
/*																			*/
/*				 (c) British Telecommunications plc							*/
/*						All rights reserved 2005							*/
/*							COPYRIGHT NOTICE								*/
/*																			*/
/*	British Telecommunications plc owns the sole copyright to this			*/
/*	software. You may not (1) make a copy of this software except for the	*/
/*	purposes of maintaining a single archive copy (2) derive works			*/
/*	herefrom (3) distribute this work to others. This notice is provided	*/
/*  for information and other rights may also apply.						*/
/*																			*/
/* ------------------------------------------------------------------------ */

#ifndef	CH264_VLD_TABLES_H
#define	CH264_VLD_TABLES_H

#define END_OF_TABLE 255
#define CTX_UNUSED 65535	// 0xffff

#define H264_UNF_TABLE 127

#if !defined(CAVLC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)

/* ********************************************************** */

extern const UNSIGNED_SEPTET coeff_token_vld_table0[240][2];

extern const int coeff_token_value_table0[][2];

extern const UNSIGNED_SEPTET coeff_token_vld_table1[256][2];

extern const int coeff_token_value_table1[][2];

extern const UNSIGNED_SEPTET coeff_token_vld_table2[272][2];

extern const int coeff_token_value_table2[][2];

extern const UNSIGNED_SEPTET coeff_token_vld_chroma[48][2];

extern const int coeff_token_value_chroma[][2];

/* ********************************************************** */

extern const int level_table_threshold[6];

/* ********************************************************** */

extern const UNSIGNED_SEPTET total_zeros_1_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_2_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_3_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_4_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_5_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_6_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_7_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_8_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_9_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_10_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_11_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_12_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_13_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_14_coeff[][2];

extern const UNSIGNED_SEPTET total_zeros_15_coeff[][2];

extern const void * total_zeros_coeff_table_ptrs[16];

extern const int total_zeros_coeff_table_code_length[16];

extern const int total_zeros_dc_table[3][8][2];

/* ********************************************************** */

extern const int run_before_table[6][8][2];

/* ********************************************************** */

#endif // !defined(CAVLC_HW_PRESENT) && !defined(CAVLC_MODEL_ON)

/* CABAC tables												  */

// In following table: 1st dimension is context id (0-398) 
//					   2nd is value of cabac_init_idc (0, 1, 2, or 3 (which is an artificial value if the picture is intra or si)
//					   3rd is m and n

#ifdef __CABAC__

extern const int m_m_n_tab[][4][2];

extern const int I_MB_Binarisation[96][7];	// Invalid values are 0, valid values the value of the MB + 1
											// indexed by value, length-1

extern const int P_MB_Binarisation[4][3];	// Invalid values are 0, valid values the value of the MB + 1
											// indexed by value, length-1

extern const int B_MB_Binarisation[122][7];	// Invalid values are 0, valid values the value of the MB + 1
											// indexed by value, length-1

extern const int P_MB_Subtype_Binarisation[4][3];

extern const int B_MB_Subtype_Binarisation[60][6];

#endif


/* ********************************************************** */

#endif

/*
--------------------------------------------------------------------------------
End of file.
--------------------------------------------------------------------------------
*/
