/*----------------------------------------------------------------------------
*
*The confidential and proprietary information contained in this file may
*only be used by a person authorised under and to the extent permitted
*by a subsisting licensing agreement from ARM Limited.
*
*        (C) COPYRIGHT 2008-2009 ARM Limited.
*             ALL RIGHTS RESERVED
*             
*This entire notice must be reproduced on all copies of this file
*and copies of this file may only be made by a person if such person is
*permitted to do so under the terms of a subsisting license agreement
*from ARM Limited.
*Modified  : $Date: 2017/10/06 07:03:56 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract :Toplevel file implementing Main()
*-----------------------------------------------------------------------------
* Overview
*----------
* 
**************************************************************/
//#include <stdio.h>

#include "MaliFns.h"
#include<asm/arch/gpu_test.h>

extern int RunMaliTest_m400_gp_F000_read_write_reg(int init);
extern int RunMaliTest_m400_gp_F004_UINT32_0_to_FP32(int init);
extern int RunMaliTest_m400_gp_F005_INT8_0_to_FP32(int init);
extern int RunMaliTest_m400_gp_F012_FP32_to_UINT8_0(int init);
extern int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0(int init);
extern int RunMaliTest_m400_gp_F065_standard_mult(int init);
extern int RunMaliTest_m400_gp_F094_RSQ(int init);
extern int RunMaliTest_m400_gp_F096_EX2(int init);
extern int RunMaliTest_m400_gp_F115_call(int init);
extern int RunMaliTest_m400_gp_F123_vs_cmd0(int init);
extern int RunMaliTest_m400_gp_F124_vs_cmd1(int init);
extern int RunMaliTest_m400_gp_F134_plbu_cmd1(int init);
extern int RunMaliTest_m400_gp_F200_plbu_config_reg_reset(int init);
extern int RunMaliTest_m400_gp_F205_apb_interrupt_00 (int init);
extern int RunMaliTest_m400_gp_F207_plbu_cull00_elements_ccw_50(int init);
extern int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00(int init);
extern int RunMaliTest_m400_pp_0_FAxx(int init);
extern int RunMaliTest_m400_pp_0_FBxx(int init);
extern int RunMaliTest_m400_pp_0_FLSxx(int init);
extern int RunMaliTest_m400_pp_0_FLxx(int init);
extern int RunMaliTest_m400_pp_0_FNxx(int init);
extern int RunMaliTest_m400_pp_0_FTxx(int init);
extern int RunMaliTest_m400_pp_0_FVxx(int init);
extern int RunMaliTest_m400_pp_0_GCxx04(int init);
extern int RunMaliTest_m400_pp_0_GCxx07(int init);
extern int RunMaliTest_m400_pp_0_GFxx03(int init);
extern int RunMaliTest_m400_pp_0_GFxx29(int init);
extern int RunMaliTest_m400_pp_0_GFxx39(int init);
extern int RunMaliTest_m400_pp_0_GFxx42(int init);
extern int RunMaliTest_m400_pp_0_GFxx49(int init);
extern int RunMaliTest_m400_pp_0_GFxx56(int init);
extern int RunMaliTest_m400_pp_0_GLxx05(int init);
extern int RunMaliTest_m400_pp_0_GLxx06(int init);
extern int RunMaliTest_m400_pp_0_GLxx08(int init);
extern int RunMaliTest_m400_pp_0_GLxx09(int init);
extern int RunMaliTest_m400_pp_0_ibi01(int init);
extern int RunMaliTest_m400_pp_0_ibi02(int init);
extern int RunMaliTest_m400_pp_0_ibi03(int init);
extern int RunMaliTest_m400_pp_0_ibi04(int init);
extern int RunMaliTest_m400_pp_0_ibi05(int init);
extern int RunMaliTest_m400_pp_0_ibi06(int init);
extern int RunMaliTest_m400_pp_0_ibi07(int init);
extern int RunMaliTest_m400_pp_0_ibi08(int init);
extern int RunMaliTest_m400_pp_0_ibi09(int init);
extern int RunMaliTest_m400_pp_0_ibr01(int init);
extern int RunMaliTest_m400_pp_0_ibr02(int init);
extern int RunMaliTest_m400_pp_0_ibr03(int init);
extern int RunMaliTest_m400_pp_0_IIBF02(int init);
extern int RunMaliTest_m400_pp_0_IIBF05(int init);
extern int RunMaliTest_m400_pp_0_IWCx11(int init);
extern int RunMaliTest_m400_pp_0_IWGCxx(int init);
extern int RunMaliTest_m400_pp_1_FAxx(int init);
extern int RunMaliTest_m400_pp_1_FBxx(int init);
extern int RunMaliTest_m400_pp_1_FLSxx(int init);
extern int RunMaliTest_m400_pp_1_FLxx(int init);
extern int RunMaliTest_m400_pp_1_FNxx(int init);
extern int RunMaliTest_m400_pp_1_FTxx(int init);
extern int RunMaliTest_m400_pp_1_FVxx(int init);
extern int RunMaliTest_m400_pp_1_GCxx04(int init);
extern int RunMaliTest_m400_pp_1_GCxx07(int init);
extern int RunMaliTest_m400_pp_1_GFxx03(int init);
extern int RunMaliTest_m400_pp_1_GFxx29(int init);
extern int RunMaliTest_m400_pp_1_GFxx39(int init);
extern int RunMaliTest_m400_pp_1_GFxx42(int init);
extern int RunMaliTest_m400_pp_1_GFxx49(int init);
extern int RunMaliTest_m400_pp_1_GFxx56(int init);
extern int RunMaliTest_m400_pp_1_GLxx05(int init);
extern int RunMaliTest_m400_pp_1_GLxx06(int init);
extern int RunMaliTest_m400_pp_1_GLxx08(int init);
extern int RunMaliTest_m400_pp_1_GLxx09(int init);
extern int RunMaliTest_m400_pp_1_ibi01(int init);
extern int RunMaliTest_m400_pp_1_ibi02(int init);
extern int RunMaliTest_m400_pp_1_ibi03(int init);
extern int RunMaliTest_m400_pp_1_ibi04(int init);
extern int RunMaliTest_m400_pp_1_ibi05(int init);
extern int RunMaliTest_m400_pp_1_ibi06(int init);
extern int RunMaliTest_m400_pp_1_ibi07(int init);
extern int RunMaliTest_m400_pp_1_ibi08(int init);
extern int RunMaliTest_m400_pp_1_ibi09(int init);
extern int RunMaliTest_m400_pp_1_ibr01(int init);
extern int RunMaliTest_m400_pp_1_ibr02(int init);
extern int RunMaliTest_m400_pp_1_ibr03(int init);
extern int RunMaliTest_m400_pp_1_IIBF02(int init);
extern int RunMaliTest_m400_pp_1_IIBF05(int init);
extern int RunMaliTest_m400_pp_1_IWCx11(int init);
extern int RunMaliTest_m400_pp_1_IWGCxx(int init);
extern int RunMaliTest_m400_pp_2_FAxx(int init);
extern int RunMaliTest_m400_pp_2_FBxx(int init);
extern int RunMaliTest_m400_pp_2_FLSxx(int init);
extern int RunMaliTest_m400_pp_2_FLxx(int init);
extern int RunMaliTest_m400_pp_2_FNxx(int init);
extern int RunMaliTest_m400_pp_2_FTxx(int init);
extern int RunMaliTest_m400_pp_2_FVxx(int init);
extern int RunMaliTest_m400_pp_2_GCxx04(int init);
extern int RunMaliTest_m400_pp_2_GCxx07(int init);
extern int RunMaliTest_m400_pp_2_GFxx03(int init);
extern int RunMaliTest_m400_pp_2_GFxx29(int init);
extern int RunMaliTest_m400_pp_2_GFxx39(int init);
extern int RunMaliTest_m400_pp_2_GFxx42(int init);
extern int RunMaliTest_m400_pp_2_GFxx49(int init);
extern int RunMaliTest_m400_pp_2_GFxx56(int init);
extern int RunMaliTest_m400_pp_2_GLxx05(int init);
extern int RunMaliTest_m400_pp_2_GLxx06(int init);
extern int RunMaliTest_m400_pp_2_GLxx08(int init);
extern int RunMaliTest_m400_pp_2_GLxx09(int init);
extern int RunMaliTest_m400_pp_2_ibi01(int init);
extern int RunMaliTest_m400_pp_2_ibi02(int init);
extern int RunMaliTest_m400_pp_2_ibi03(int init);
extern int RunMaliTest_m400_pp_2_ibi04(int init);
extern int RunMaliTest_m400_pp_2_ibi05(int init);
extern int RunMaliTest_m400_pp_2_ibi06(int init);
extern int RunMaliTest_m400_pp_2_ibi07(int init);
extern int RunMaliTest_m400_pp_2_ibi08(int init);
extern int RunMaliTest_m400_pp_2_ibi09(int init);
extern int RunMaliTest_m400_pp_2_ibr01(int init);
extern int RunMaliTest_m400_pp_2_ibr02(int init);
extern int RunMaliTest_m400_pp_2_ibr03(int init);
extern int RunMaliTest_m400_pp_2_IIBF02(int init);
extern int RunMaliTest_m400_pp_2_IIBF05(int init);
extern int RunMaliTest_m400_pp_2_IWCx11(int init);
extern int RunMaliTest_m400_pp_2_IWGCxx(int init);
extern int RunMaliTest_m400_pp_3_FAxx(int init);
extern int RunMaliTest_m400_pp_3_FBxx(int init);
extern int RunMaliTest_m400_pp_3_FLSxx(int init);
extern int RunMaliTest_m400_pp_3_FLxx(int init);
extern int RunMaliTest_m400_pp_3_FNxx(int init);
extern int RunMaliTest_m400_pp_3_FTxx(int init);
extern int RunMaliTest_m400_pp_3_FVxx(int init);
extern int RunMaliTest_m400_pp_3_GCxx04(int init);
extern int RunMaliTest_m400_pp_3_GCxx07(int init);
extern int RunMaliTest_m400_pp_3_GFxx03(int init);
extern int RunMaliTest_m400_pp_3_GFxx29(int init);
extern int RunMaliTest_m400_pp_3_GFxx39(int init);
extern int RunMaliTest_m400_pp_3_GFxx42(int init);
extern int RunMaliTest_m400_pp_3_GFxx49(int init);
extern int RunMaliTest_m400_pp_3_GFxx56(int init);
extern int RunMaliTest_m400_pp_3_GLxx05(int init);
extern int RunMaliTest_m400_pp_3_GLxx06(int init);
extern int RunMaliTest_m400_pp_3_GLxx08(int init);
extern int RunMaliTest_m400_pp_3_GLxx09(int init);
extern int RunMaliTest_m400_pp_3_ibi01(int init);
extern int RunMaliTest_m400_pp_3_ibi02(int init);
extern int RunMaliTest_m400_pp_3_ibi03(int init);
extern int RunMaliTest_m400_pp_3_ibi04(int init);
extern int RunMaliTest_m400_pp_3_ibi05(int init);
extern int RunMaliTest_m400_pp_3_ibi06(int init);
extern int RunMaliTest_m400_pp_3_ibi07(int init);
extern int RunMaliTest_m400_pp_3_ibi08(int init);
extern int RunMaliTest_m400_pp_3_ibi09(int init);
extern int RunMaliTest_m400_pp_3_ibr01(int init);
extern int RunMaliTest_m400_pp_3_ibr02(int init);
extern int RunMaliTest_m400_pp_3_ibr03(int init);
extern int RunMaliTest_m400_pp_3_IIBF02(int init);
extern int RunMaliTest_m400_pp_3_IIBF05(int init);
extern int RunMaliTest_m400_pp_3_IWCx11(int init);
extern int RunMaliTest_m400_pp_3_IWGCxx(int init);
extern int RunMaliTest_m400_mp_1_F000_MMU_APB(int init);
extern int RunMaliTest_m400_mp_1_F010_PP0_MMU_APB (int init);
extern int RunMaliTest_m400_mp_1_F013_MMU_GP_IRQ (int init);
extern int RunMaliTest_m400_mp_1_F014_MMU_PP0_IRQ (int init);
extern int RunMaliTest_m400_mp_1_F023_GP_PP0 (int init);
extern int RunMaliTest_m400_mp_2_F000_MMU_APB (int init);
extern int RunMaliTest_m400_mp_2_F010_PP0_MMU_APB (int init);
extern int RunMaliTest_m400_mp_2_F011_PP1_MMU_APB (int init);
extern int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ (int init);
extern int RunMaliTest_m400_mp_2_F014_MMU_PP0_IRQ (int init);
extern int RunMaliTest_m400_mp_2_F015_MMU_PP1_IRQ (int init);
extern int RunMaliTest_m400_mp_2_F023_GP_PP0 (int init);
extern int RunMaliTest_m400_mp_2_F024_GP_PP0_PP1 (int init);
extern int RunMaliTest_m400_mp_3_F000_MMU_APB (int init);
extern int RunMaliTest_m400_mp_3_F010_PP0_MMU_APB (int init);
extern int RunMaliTest_m400_mp_3_F011_PP1_MMU_APB (int init);
extern int RunMaliTest_m400_mp_3_F013_MMU_GP_IRQ (int init);
extern int RunMaliTest_m400_mp_3_F014_MMU_PP0_IRQ (int init);
extern int RunMaliTest_m400_mp_3_F015_MMU_PP1_IRQ (int init);
extern int RunMaliTest_m400_mp_3_F023_GP_PP0 (int init);
extern int RunMaliTest_m400_mp_3_F024_GP_PP0_PP1 (int init);
extern int RunMaliTest_m400_mp_3_F028_GP_PP0_PP1_PP2 (int init);
extern int RunMaliTest_m400_mp_3_F030_PP2_MMU_APB (int init);
extern int RunMaliTest_m400_mp_3_F032_MMU_PP2_IRQ (int init);
extern int RunMaliTest_m400_mp_4_F000_MMU_APB (int init);
extern int RunMaliTest_m400_mp_4_F010_PP0_MMU_APB (int init);
extern int RunMaliTest_m400_mp_4_F011_PP1_MMU_APB (int init);
extern int RunMaliTest_m400_mp_4_F013_MMU_GP_IRQ (int init);
extern int RunMaliTest_m400_mp_4_F014_MMU_PP0_IRQ (int init);
extern int RunMaliTest_m400_mp_4_F015_MMU_PP1_IRQ (int init);
extern int RunMaliTest_m400_mp_4_F023_GP_PP0 (int init);
extern int RunMaliTest_m400_mp_4_F024_GP_PP0_PP1 (int init);
extern int RunMaliTest_m400_mp_4_F028_GP_PP0_PP1_PP2 (int init);
extern int RunMaliTest_m400_mp_4_F030_PP2_MMU_APB (int init);
extern int RunMaliTest_m400_mp_4_F032_MMU_PP2_IRQ (int init);
extern int RunMaliTest_proxycon_etc_fsaa4_1080p_Average_f75_mc4 (int init);

int (*TestList_GP[])(int)={
#ifdef CONFIG_MALIGPU_PROXYCON
RunMaliTest_proxycon_etc_fsaa4_1080p_Average_f75_mc4,
#endif
#ifdef CONFIG_MALIGPU_GP
        RunMaliTest_m400_gp_F000_read_write_reg,
        RunMaliTest_m400_gp_F004_UINT32_0_to_FP32,
        RunMaliTest_m400_gp_F005_INT8_0_to_FP32,
        RunMaliTest_m400_gp_F012_FP32_to_UINT8_0,
        RunMaliTest_m400_gp_F014_FP32_to_UINT32_0,
        RunMaliTest_m400_gp_F065_standard_mult,
        RunMaliTest_m400_gp_F094_RSQ,
        RunMaliTest_m400_gp_F096_EX2,
        RunMaliTest_m400_gp_F115_call,
        RunMaliTest_m400_gp_F123_vs_cmd0,
        RunMaliTest_m400_gp_F124_vs_cmd1 ,
	RunMaliTest_m400_gp_F134_plbu_cmd1, 
	RunMaliTest_m400_gp_F200_plbu_config_reg_reset,
	RunMaliTest_m400_gp_F205_apb_interrupt_00,
	RunMaliTest_m400_gp_F207_plbu_cull00_elements_ccw_50,
	RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00,
#endif
#ifdef CONFIG_MALIGPU_PP0
	RunMaliTest_m400_pp_0_FAxx,	
	RunMaliTest_m400_pp_0_FBxx,
	RunMaliTest_m400_pp_0_FLSxx,
	RunMaliTest_m400_pp_0_FLxx,
	RunMaliTest_m400_pp_0_FNxx,
	RunMaliTest_m400_pp_0_FTxx,
	RunMaliTest_m400_pp_0_FVxx,
	RunMaliTest_m400_pp_0_GCxx04,
	RunMaliTest_m400_pp_0_GCxx07,
	RunMaliTest_m400_pp_0_GFxx03,
	RunMaliTest_m400_pp_0_GFxx29,
	RunMaliTest_m400_pp_0_GFxx39,
	RunMaliTest_m400_pp_0_GFxx42,
	RunMaliTest_m400_pp_0_GFxx49,
	RunMaliTest_m400_pp_0_GFxx56,
	RunMaliTest_m400_pp_0_GLxx05,
	RunMaliTest_m400_pp_0_GLxx06,
	RunMaliTest_m400_pp_0_GLxx08,
	RunMaliTest_m400_pp_0_GLxx09,
	RunMaliTest_m400_pp_0_ibi01,
	RunMaliTest_m400_pp_0_ibi02,
	RunMaliTest_m400_pp_0_ibi03,
	RunMaliTest_m400_pp_0_ibi04,
	RunMaliTest_m400_pp_0_ibi05,
	RunMaliTest_m400_pp_0_ibi07,
	RunMaliTest_m400_pp_0_ibi08,
	RunMaliTest_m400_pp_0_ibi09,
	RunMaliTest_m400_pp_0_ibr01,
	RunMaliTest_m400_pp_0_ibr02,
	RunMaliTest_m400_pp_0_ibr03,
	RunMaliTest_m400_pp_0_IIBF02,
	RunMaliTest_m400_pp_0_IIBF05,
	RunMaliTest_m400_pp_0_IWCx11,
	RunMaliTest_m400_pp_0_IWGCxx,
#endif
#ifdef CONFIG_MALIGPU_PP1
	RunMaliTest_m400_pp_1_FAxx,
	RunMaliTest_m400_pp_1_FBxx,
	RunMaliTest_m400_pp_1_FLSxx,
	RunMaliTest_m400_pp_1_FLxx,
	RunMaliTest_m400_pp_1_FNxx,
	RunMaliTest_m400_pp_1_FTxx,
	RunMaliTest_m400_pp_1_FVxx,
	RunMaliTest_m400_pp_1_GCxx04,
	RunMaliTest_m400_pp_1_GCxx07,
	RunMaliTest_m400_pp_1_GFxx03,
	RunMaliTest_m400_pp_1_GFxx29,
	RunMaliTest_m400_pp_1_GFxx39,
	RunMaliTest_m400_pp_1_GFxx42,
	RunMaliTest_m400_pp_1_GFxx49,
	RunMaliTest_m400_pp_1_GFxx56,
	RunMaliTest_m400_pp_1_GLxx05,
	RunMaliTest_m400_pp_1_GLxx06,
	RunMaliTest_m400_pp_1_GLxx08,
	RunMaliTest_m400_pp_1_GLxx09,
	RunMaliTest_m400_pp_1_ibi01,
	RunMaliTest_m400_pp_1_ibi02,
	RunMaliTest_m400_pp_1_ibi03,
	RunMaliTest_m400_pp_1_ibi04,
	RunMaliTest_m400_pp_1_ibi05,
	RunMaliTest_m400_pp_1_ibi06,
	RunMaliTest_m400_pp_1_ibi07,
	RunMaliTest_m400_pp_1_ibi08,
	RunMaliTest_m400_pp_1_ibi09,
	RunMaliTest_m400_pp_1_ibr01,
	RunMaliTest_m400_pp_1_ibr02,
	RunMaliTest_m400_pp_1_ibr03,
	RunMaliTest_m400_pp_1_IIBF02,
	RunMaliTest_m400_pp_1_IIBF05,
	RunMaliTest_m400_pp_1_IWCx11,
	RunMaliTest_m400_pp_1_IWGCxx,
#endif
#ifdef CONFIG_MALIGPU_PP2
	RunMaliTest_m400_pp_2_FAxx,
        RunMaliTest_m400_pp_2_FBxx,
        RunMaliTest_m400_pp_2_FLSxx,
        RunMaliTest_m400_pp_2_FLxx,
        RunMaliTest_m400_pp_2_FNxx,
        RunMaliTest_m400_pp_2_FTxx,
        RunMaliTest_m400_pp_2_FVxx,
        RunMaliTest_m400_pp_2_GCxx04,
        RunMaliTest_m400_pp_2_GCxx07,
        RunMaliTest_m400_pp_2_GFxx03,
        RunMaliTest_m400_pp_2_GFxx29,
        RunMaliTest_m400_pp_2_GFxx39,
        RunMaliTest_m400_pp_2_GFxx42,
        RunMaliTest_m400_pp_2_GFxx49,
        RunMaliTest_m400_pp_2_GFxx56,
        RunMaliTest_m400_pp_2_GLxx05,
        RunMaliTest_m400_pp_2_GLxx06,
        RunMaliTest_m400_pp_2_GLxx08,
        RunMaliTest_m400_pp_2_GLxx09,
        RunMaliTest_m400_pp_2_ibi01,
        RunMaliTest_m400_pp_2_ibi02,
        RunMaliTest_m400_pp_2_ibi03,
        RunMaliTest_m400_pp_2_ibi04,
        RunMaliTest_m400_pp_2_ibi05,
        RunMaliTest_m400_pp_2_ibi06,
        RunMaliTest_m400_pp_2_ibi07,
        RunMaliTest_m400_pp_2_ibi08,
        RunMaliTest_m400_pp_2_ibi09,
        RunMaliTest_m400_pp_2_ibr01,
        RunMaliTest_m400_pp_2_ibr02,
        RunMaliTest_m400_pp_2_ibr03,
        RunMaliTest_m400_pp_2_IIBF02,
        RunMaliTest_m400_pp_2_IIBF05,
        RunMaliTest_m400_pp_2_IWCx11,
        RunMaliTest_m400_pp_2_IWGCxx,
#endif
#ifdef CONFIG_MALIGPU_PP3
        RunMaliTest_m400_pp_3_FAxx,
        RunMaliTest_m400_pp_3_FBxx,
        RunMaliTest_m400_pp_3_FLSxx,
        RunMaliTest_m400_pp_3_FLxx,
        RunMaliTest_m400_pp_3_FNxx,
        RunMaliTest_m400_pp_3_FTxx,
        RunMaliTest_m400_pp_3_FVxx,
        RunMaliTest_m400_pp_3_GCxx04,
        RunMaliTest_m400_pp_3_GCxx07,
        RunMaliTest_m400_pp_3_GFxx03,
        RunMaliTest_m400_pp_3_GFxx29,
        RunMaliTest_m400_pp_3_GFxx39,
        RunMaliTest_m400_pp_3_GFxx42,
        RunMaliTest_m400_pp_3_GFxx49,
        RunMaliTest_m400_pp_3_GFxx56,
        RunMaliTest_m400_pp_3_GLxx05,
        RunMaliTest_m400_pp_3_GLxx06,
        RunMaliTest_m400_pp_3_GLxx08,
        RunMaliTest_m400_pp_3_GLxx09,
        RunMaliTest_m400_pp_3_ibi01,
        RunMaliTest_m400_pp_3_ibi02,
        RunMaliTest_m400_pp_3_ibi03,
        RunMaliTest_m400_pp_3_ibi04,
        RunMaliTest_m400_pp_3_ibi05,
        RunMaliTest_m400_pp_3_ibi06,
        RunMaliTest_m400_pp_3_ibi07,
        RunMaliTest_m400_pp_3_ibi08,
        RunMaliTest_m400_pp_3_ibi09,
        RunMaliTest_m400_pp_3_ibr01,
        RunMaliTest_m400_pp_3_ibr02,
        RunMaliTest_m400_pp_3_ibr03,
        RunMaliTest_m400_pp_3_IIBF02,
        RunMaliTest_m400_pp_3_IIBF05,
        RunMaliTest_m400_pp_3_IWCx11,
        RunMaliTest_m400_pp_3_IWGCxx,
#endif
#ifdef CONFIG_MALIGPU_MP1 
	RunMaliTest_m400_mp_1_F000_MMU_APB,
	RunMaliTest_m400_mp_1_F010_PP0_MMU_APB,
	RunMaliTest_m400_mp_1_F013_MMU_GP_IRQ,
	RunMaliTest_m400_mp_1_F014_MMU_PP0_IRQ,
	RunMaliTest_m400_mp_1_F023_GP_PP0,
#endif
#ifdef CONFIG_MALIGPU_MP2
	RunMaliTest_m400_mp_2_F000_MMU_APB,
	RunMaliTest_m400_mp_2_F010_PP0_MMU_APB,
	RunMaliTest_m400_mp_2_F011_PP1_MMU_APB,
	RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ,
	RunMaliTest_m400_mp_2_F014_MMU_PP0_IRQ,
	RunMaliTest_m400_mp_2_F015_MMU_PP1_IRQ,
	RunMaliTest_m400_mp_2_F023_GP_PP0,
	RunMaliTest_m400_mp_2_F024_GP_PP0_PP1,
#endif
#ifdef CONFIG_MALIGPU_MP3 
	RunMaliTest_m400_mp_3_F000_MMU_APB,
	RunMaliTest_m400_mp_3_F010_PP0_MMU_APB,  
	RunMaliTest_m400_mp_3_F011_PP1_MMU_APB,  
	RunMaliTest_m400_mp_3_F013_MMU_GP_IRQ,  
	RunMaliTest_m400_mp_3_F014_MMU_PP0_IRQ,  
	RunMaliTest_m400_mp_3_F015_MMU_PP1_IRQ,
	RunMaliTest_m400_mp_3_F023_GP_PP0,  
	RunMaliTest_m400_mp_3_F024_GP_PP0_PP1,  
	RunMaliTest_m400_mp_3_F028_GP_PP0_PP1_PP2,  
	RunMaliTest_m400_mp_3_F030_PP2_MMU_APB,  
	RunMaliTest_m400_mp_3_F032_MMU_PP2_IRQ, 
#endif
#ifdef CONFIG_MALIGPU_MP4
	RunMaliTest_m400_mp_4_F000_MMU_APB,
	RunMaliTest_m400_mp_4_F010_PP0_MMU_APB,
	RunMaliTest_m400_mp_4_F011_PP1_MMU_APB,
	RunMaliTest_m400_mp_4_F013_MMU_GP_IRQ,
	RunMaliTest_m400_mp_4_F014_MMU_PP0_IRQ,
	RunMaliTest_m400_mp_4_F015_MMU_PP1_IRQ,
	RunMaliTest_m400_mp_4_F023_GP_PP0,
	RunMaliTest_m400_mp_4_F024_GP_PP0_PP1,
	RunMaliTest_m400_mp_4_F028_GP_PP0_PP1_PP2,
	RunMaliTest_m400_mp_4_F030_PP2_MMU_APB,
	RunMaliTest_m400_mp_4_F032_MMU_PP2_IRQ,
#endif
};

typedef int (*MaliFunctionPtr)(int init);

/* Interrupt call backs */
void MaliIntCallBack (int IntNum, int IntSrc);

static MaliFunctionPtr mali_currentfunction_ptr;

//static int Mali_FinishedFlag = 0;
//static int Mali_ReturnCode   = 0;

int Mali_FinishedFlag = 0; //MOSCHIP
int Mali_ReturnCode   = 0; //MOSCHIP
//TestList_GP TestName;
/********************************************************************************
 * Main routine for Mali Test.
 *
 * 1. Setup all interrupts
 * 2. Run first part of test. Remainder of test parts operate in interrupt handler.
 * 3. Spin until FinishedFlag is set.
 ********************************************************************************/
int gpu_main (int n) {

  /* 1. Disable CPU interrupts; and initalise any interrupt controller */
  /* 2. Install mali interrupt handlers */
  /* 3. Install interrupt handlers for mali interrupts */
  CPU_DisableInterrupts();
//  CPU_InitialiseInterrupts();
  Mali_InstallIntHandlers(&MaliIntCallBack,
                          &MaliIntCallBack,
                          &MaliIntCallBack
                          );

  /* 4. Run the first part of this test. */
  /*    Note this can trigger an interrupt; hence we run this whilst interrupts */
  /*    are disabled. If interrupt masking cannot be done - then create another */
  /*    system to mali interrupts until after this routine has returned.        */
  /* 5. Enable interrupts */
    mali_currentfunction_ptr = TestList_GP[n];
  Mali_ReturnCode = (*mali_currentfunction_ptr)(1);
  if (Mali_ReturnCode != 255) {
    Mali_FinishedFlag = 1;
  };
  CPU_EnableInterrupts();

  /* Do whatever you like - mali now runs from within interrupt handler
     For reference we have an infinite loop with a wait for interrupt instruction
     inside - waiting until all Mali jobs are finished.
     ............
   */
  while (!Mali_FinishedFlag) {
    Mali_WaitForInterrupt();
  };

  if (Mali_ReturnCode != 0) {
    printf ("TEST FAILED\n");
  } else {
    printf ("TEST PASSED\n");
  };
};

/********************************************************************************
 * This is the mali interrupt handler for expected interrupts
 *
 * 1. acknowledge the interrupt 
 * 2. call the next part in this test 
 * 3. set finished flag if not returning with wait for interrupt return code
 ********************************************************************************/
void MaliIntCallBack (int IntNum, int IntSrc) {
  /* Only run next step if we have not finished */
  /* This ensures that if the callback is executed when no mali interrupts */
  /* are actually pending; the test still works */

  if (Mali_FinishedFlag == 0) {
    Mali_ReturnCode = (*mali_currentfunction_ptr)(0);
  };
  if (Mali_ReturnCode != 255) {
    Mali_FinishedFlag = 1;
  };
};
