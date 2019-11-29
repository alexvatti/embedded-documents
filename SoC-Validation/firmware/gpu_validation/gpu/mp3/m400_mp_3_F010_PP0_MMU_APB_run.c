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
* Modified  : $Date: 2013/05/14 11:58:21 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_mp_3_F010_PP0_MMU_APB_check_0_a.h"
#include "m400_mp_3_F010_PP0_MMU_APB_check_0_b.h"
#include "m400_mp_3_F010_PP0_MMU_APB_check_0_c.h"

int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_a ();
int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_b ();
int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_c ();

int RunMaliTest_m400_mp_3_F010_PP0_MMU_APB_part0 (void);

static int mali_step;

int RunMaliTest_m400_mp_3_F010_PP0_MMU_APB (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_mp_3_F010_PP0_MMU_APB_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_mp_3_F010_PP0_MMU_APB_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_mp_3_F010_PP0_MMU_APB\n");

  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # Write to all registers, but bit 1 interrupt clear. */
  /* COMMENT */                                      /* # Dump registers. */
  Mali_WrReg(0x2,0x1,0x0,0xffffffff);                /* writereg 12000000 ffffffff */
  Mali_WrReg(0x2,0x1,0x4,0xffffffff);                /* writereg 12000004 ffffffff */
  Mali_WrReg(0x2,0x1,0x8,0xffffffff);                /* writereg 12000008 ffffffff */
  Mali_WrReg(0x2,0x1,0xc,0xffffffff);                /* writereg 1200000c ffffffff */
  Mali_WrReg(0x2,0x1,0x10,0xffffffff);               /* writereg 12000010 ffffffff */
  Mali_WrReg(0x2,0x1,0x14,0xffffffff);               /* writereg 12000014 ffffffff */
  Mali_WrReg(0x2,0x1,0x18,0xfffffffe);               /* writereg 12000018 fffffffe */
  Mali_WrReg(0x2,0x1,0x1c,0xffffffff);               /* writereg 1200001c ffffffff */
  Mali_WrReg(0x2,0x1,0x20,0xffffffff);               /* writereg 12000020 ffffffff */
  /* SEE BELOW */                                    /* dump_reg 12000000 12000024 > regdump0.hex */
  res |= Check_m400_mp_3_F010_PP0_MMU_APB_check_0_a();

  /* COMMENT */                                      /* # Clear interrupts and dump. */
  Mali_WrReg(0x2,0x1,0x18,0x00000001);               /* writereg 12000018 00000001 */
  /* SEE BELOW */                                    /* dump_reg 12000000 12000024 > regdump1.hex */
  res |= Check_m400_mp_3_F010_PP0_MMU_APB_check_0_b();

  /* COMMENT */                                      /* # Reset and dump registers. */
  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* dump_reg 12000000 12000024 > regdump2.hex */
  res |= Check_m400_mp_3_F010_PP0_MMU_APB_check_0_c();

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_3_F010_PP0_MMU_APB_check_0_a.data_12000000,2,1,0x0,0x24);
  return res;
};

int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_3_F010_PP0_MMU_APB_check_0_b.data_12000000,2,1,0x0,0x24);
  return res;
};

int Check_m400_mp_3_F010_PP0_MMU_APB_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_3_F010_PP0_MMU_APB_check_0_c.data_12000000,2,1,0x0,0x24);
  return res;
};
