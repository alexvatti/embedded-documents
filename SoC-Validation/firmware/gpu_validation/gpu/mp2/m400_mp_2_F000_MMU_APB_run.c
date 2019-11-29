/*----------------------------------------------------------------------------
*
* The confidential and proprietary information contained in this file may
* only be used by a person authorised under and to the extent permitted
* by a subsisting licensing agreement from ARM Limited.
*
*        (C) COPYRIGHT 2008-2009 ARM Limited.
*             ALL RIGHTS RESERVED
*             
* This entire notice must be reproduced on all copies of this file
* and copies of this file may only be made by a person if such person is
* permitted to do so under the terms of a subsisting license agreement
* from ARM Limited.
*
* Modified  : $Date: 2013/05/14 11:58:11 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_mp_2_F000_MMU_APB_check_0_a.h"
#include "m400_mp_2_F000_MMU_APB_check_0_b.h"
#include "m400_mp_2_F000_MMU_APB_check_0_c.h"

int Check_m400_mp_2_F000_MMU_APB_check_0_a ();
int Check_m400_mp_2_F000_MMU_APB_check_0_b ();
int Check_m400_mp_2_F000_MMU_APB_check_0_c ();

int RunMaliTest_m400_mp_2_F000_MMU_APB_part0 (void);

static int mali_step;

int RunMaliTest_m400_mp_2_F000_MMU_APB (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_mp_2_F000_MMU_APB_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_mp_2_F000_MMU_APB_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_mp_2_F000_MMU_APB\n");

  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # Write to all registers, but bit 1 interrupt clear. */
  /* COMMENT */                                      /* # Dump registers. */
  Mali_WrReg(0x2,0x0,0x0,0xffffffff);                /* writereg 02000000 ffffffff */
  Mali_WrReg(0x2,0x0,0x4,0xffffffff);                /* writereg 02000004 ffffffff */
  Mali_WrReg(0x2,0x0,0x8,0xffffffff);                /* writereg 02000008 ffffffff */
  Mali_WrReg(0x2,0x0,0xc,0xffffffff);                /* writereg 0200000c ffffffff */
  Mali_WrReg(0x2,0x0,0x10,0xffffffff);               /* writereg 02000010 ffffffff */
  Mali_WrReg(0x2,0x0,0x14,0xffffffff);               /* writereg 02000014 ffffffff */
  Mali_WrReg(0x2,0x0,0x18,0xfffffffe);               /* writereg 02000018 fffffffe */
  Mali_WrReg(0x2,0x0,0x1c,0xffffffff);               /* writereg 0200001c ffffffff */
  Mali_WrReg(0x2,0x0,0x20,0xffffffff);               /* writereg 02000020 ffffffff */
  res |= Check_m400_mp_2_F000_MMU_APB_check_0_a();   /* dump_reg 02000000 02000024 > regdump0.hex */
  /* COMMENT */                                      /* # Clear interrupts and dump. */
  Mali_WrReg(0x2,0x0,0x18,0x00000001);               /* writereg 02000018 00000001 */
  res |= Check_m400_mp_2_F000_MMU_APB_check_0_b();   /* dump_reg 02000000 02000024 > regdump1.hex */
  /* COMMENT */                                      /* # Reset and dump registers. */
  Mali_Reset();                                      /* reset */
  res |= Check_m400_mp_2_F000_MMU_APB_check_0_c();   /* dump_reg 02000000 02000024 > regdump2.hex */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_mp_2_F000_MMU_APB_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_2_F000_MMU_APB_check_0_a.data_02000000,2,0,0x0,0x24);
  return res;
};

int Check_m400_mp_2_F000_MMU_APB_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_2_F000_MMU_APB_check_0_b.data_02000000,2,0,0x0,0x24);
  return res;
};

int Check_m400_mp_2_F000_MMU_APB_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_2_F000_MMU_APB_check_0_c.data_02000000,2,0,0x0,0x24);
  return res;
};
