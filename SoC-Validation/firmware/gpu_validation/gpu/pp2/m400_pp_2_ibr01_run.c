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
* Modified  : $Date: 2017/09/20 12:39:35 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_2_ibr01_check_0_a.h"
#include "m400_pp_2_ibr01_check_0_b.h"
#include "m400_pp_2_ibr01_check_0_c.h"
#include "m400_pp_2_ibr01_check_0_d.h"
#include "m400_pp_2_ibr01_check_0_e.h"

int Check_m400_pp_2_ibr01_check_0_a ();
int Check_m400_pp_2_ibr01_check_0_b ();
int Check_m400_pp_2_ibr01_check_0_c ();
int Check_m400_pp_2_ibr01_check_0_d ();
int Check_m400_pp_2_ibr01_check_0_e ();

int RunMaliTest_m400_pp_2_ibr01_part0 (void);

static int mali_step;

int RunMaliTest_m400_pp_2_ibr01 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_2_ibr01_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_2_ibr01_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_2_ibr01\n");

  /* COMMENT */                                      /* #IBR01 Mali200 */
  /* COMMENT */                                      /* #Reset with pin and check all default values */
  /* COMMENT */                                      /* #Unimplemented optional registers should read zero */
  Mali_Reset();                                      /* reset */
  res |= Check_m400_pp_2_ibr01_check_0_a();          /* dump_reg 10000000 10000050 > ibr01_00000_reg.hex # Frame registers */
  res |= Check_m400_pp_2_ibr01_check_0_b();          /* dump_reg 10000100 10000130 > ibr01_00001_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_2_ibr01_check_0_c();          /* dump_reg 10000200 10000230 > ibr01_00002_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_2_ibr01_check_0_d();          /* dump_reg 10000300 10000330 > ibr01_00003_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_2_ibr01_check_0_e();          /* dump_reg 10001000 100010b0 > ibr01_00004_reg.hex # Management registers */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_2_ibr01_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_2_ibr01_check_0_a.data_10000000,1,0,0x0,0x50);
  return res;
};

int Check_m400_pp_2_ibr01_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_2_ibr01_check_0_b.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_2_ibr01_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_2_ibr01_check_0_c.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_2_ibr01_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_2_ibr01_check_0_d.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_2_ibr01_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_2_ibr01_check_0_e.data_10001000,1,0,0x1000,0x10b0);
  return res;
};
