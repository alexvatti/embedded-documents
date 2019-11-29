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
* Modified  : $Date: 2017/09/20 12:45:21 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_3_ibr03_check_0_a.h"
#include "m400_pp_3_ibr03_check_0_b.h"
#include "m400_pp_3_ibr03_check_0_c.h"
#include "m400_pp_3_ibr03_check_0_d.h"
#include "m400_pp_3_ibr03_check_0_e.h"
#include "m400_pp_3_ibr03_check_0_f.h"
#include "m400_pp_3_ibr03_check_0_g.h"
#include "m400_pp_3_ibr03_check_0_h.h"
#include "m400_pp_3_ibr03_check_0_i.h"
#include "m400_pp_3_ibr03_check_0_j.h"

int Check_m400_pp_3_ibr03_check_0_a ();
int Check_m400_pp_3_ibr03_check_0_b ();
int Check_m400_pp_3_ibr03_check_0_c ();
int Check_m400_pp_3_ibr03_check_0_d ();
int Check_m400_pp_3_ibr03_check_0_e ();
int Check_m400_pp_3_ibr03_check_0_f ();
int Check_m400_pp_3_ibr03_check_0_g ();
int Check_m400_pp_3_ibr03_check_0_h ();
int Check_m400_pp_3_ibr03_check_0_i ();
int Check_m400_pp_3_ibr03_check_0_j ();

int RunMaliTest_m400_pp_3_ibr03_part0 (void);

static int mali_step;

int RunMaliTest_m400_pp_3_ibr03 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_3_ibr03_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_3_ibr03_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_3_ibr03\n");

  /* COMMENT */                                      /* #IBR03 Mali200 */
  /* COMMENT */                                      /* #Reset with soft reset and check all default values */
  /* COMMENT */                                      /* #Unimplemented optional registers should read zero */
  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # First fill all registers */
  /* COMMENT */                                      /* #Frame registers */
  Mali_WrReg(0x1,0x0,0x0,0xffffffff);                /* writereg 10000000 ffffffff */
  Mali_WrReg(0x1,0x0,0x4,0xffffffff);                /* writereg 10000004 ffffffff */
  Mali_WrReg(0x1,0x0,0x8,0xffffffff);                /* writereg 10000008 ffffffff */
  Mali_WrReg(0x1,0x0,0xc,0xffffffff);                /* writereg 1000000c ffffffff */
  Mali_WrReg(0x1,0x0,0x10,0xffffffff);               /* writereg 10000010 ffffffff */
  Mali_WrReg(0x1,0x0,0x14,0xffffffff);               /* writereg 10000014 ffffffff */
  Mali_WrReg(0x1,0x0,0x18,0xffffffff);               /* writereg 10000018 ffffffff */
  Mali_WrReg(0x1,0x0,0x1c,0xffffffff);               /* writereg 1000001c ffffffff */
  Mali_WrReg(0x1,0x0,0x20,0xffffffff);               /* writereg 10000020 ffffffff */
  Mali_WrReg(0x1,0x0,0x24,0xffffffff);               /* writereg 10000024 ffffffff */
  Mali_WrReg(0x1,0x0,0x28,0xffffffff);               /* writereg 10000028 ffffffff */
  Mali_WrReg(0x1,0x0,0x2c,0xffffffff);               /* writereg 1000002c ffffffff */
  Mali_WrReg(0x1,0x0,0x30,0xffffffff);               /* writereg 10000030 ffffffff */
  Mali_WrReg(0x1,0x0,0x34,0xffffffff);               /* writereg 10000034 ffffffff */
  Mali_WrReg(0x1,0x0,0x38,0xffffffff);               /* writereg 10000038 ffffffff */
  Mali_WrReg(0x1,0x0,0x3c,0xffffffff);               /* writereg 1000003c ffffffff */
  Mali_WrReg(0x1,0x0,0x40,0xffffffff);               /* writereg 10000040 ffffffff */
  Mali_WrReg(0x1,0x0,0x44,0xffffffff);               /* writereg 10000044 ffffffff */
  /* COMMENT */                                      /* #Writeback registers for WB0 */
  Mali_WrReg(0x1,0x0,0x100,0xffffffff);              /* writereg 10000100 ffffffff */
  Mali_WrReg(0x1,0x0,0x104,0xffffffff);              /* writereg 10000104 ffffffff */
  Mali_WrReg(0x1,0x0,0x108,0xffffffff);              /* writereg 10000108 ffffffff */
  Mali_WrReg(0x1,0x0,0x10c,0xffffffff);              /* writereg 1000010c ffffffff */
  Mali_WrReg(0x1,0x0,0x110,0xffffffff);              /* writereg 10000110 ffffffff */
  Mali_WrReg(0x1,0x0,0x114,0xffffffff);              /* writereg 10000114 ffffffff */
  Mali_WrReg(0x1,0x0,0x118,0xffffffff);              /* writereg 10000118 ffffffff */
  Mali_WrReg(0x1,0x0,0x11c,0xffffffff);              /* writereg 1000011c ffffffff */
  Mali_WrReg(0x1,0x0,0x120,0xffffffff);              /* writereg 10000120 ffffffff */
  Mali_WrReg(0x1,0x0,0x124,0xffffffff);              /* writereg 10000124 ffffffff */
  Mali_WrReg(0x1,0x0,0x128,0xffffffff);              /* writereg 10000128 ffffffff */
  Mali_WrReg(0x1,0x0,0x12c,0xffffffff);              /* writereg 1000012c ffffffff */
  /* COMMENT */                                      /* #Writeback registers for WB1 */
  Mali_WrReg(0x1,0x0,0x200,0xffffffff);              /* writereg 10000200 ffffffff */
  Mali_WrReg(0x1,0x0,0x204,0xffffffff);              /* writereg 10000204 ffffffff */
  Mali_WrReg(0x1,0x0,0x208,0xffffffff);              /* writereg 10000208 ffffffff */
  Mali_WrReg(0x1,0x0,0x20c,0xffffffff);              /* writereg 1000020c ffffffff */
  Mali_WrReg(0x1,0x0,0x210,0xffffffff);              /* writereg 10000210 ffffffff */
  Mali_WrReg(0x1,0x0,0x214,0xffffffff);              /* writereg 10000214 ffffffff */
  Mali_WrReg(0x1,0x0,0x218,0xffffffff);              /* writereg 10000218 ffffffff */
  Mali_WrReg(0x1,0x0,0x21c,0xffffffff);              /* writereg 1000021c ffffffff */
  Mali_WrReg(0x1,0x0,0x220,0xffffffff);              /* writereg 10000220 ffffffff */
  Mali_WrReg(0x1,0x0,0x224,0xffffffff);              /* writereg 10000224 ffffffff */
  Mali_WrReg(0x1,0x0,0x228,0xffffffff);              /* writereg 10000228 ffffffff */
  Mali_WrReg(0x1,0x0,0x22c,0xffffffff);              /* writereg 1000022c ffffffff */
  /* COMMENT */                                      /* #Writeback registers for WB2 */
  Mali_WrReg(0x1,0x0,0x300,0xffffffff);              /* writereg 10000300 ffffffff */
  Mali_WrReg(0x1,0x0,0x304,0xffffffff);              /* writereg 10000304 ffffffff */
  Mali_WrReg(0x1,0x0,0x308,0xffffffff);              /* writereg 10000308 ffffffff */
  Mali_WrReg(0x1,0x0,0x30c,0xffffffff);              /* writereg 1000030c ffffffff */
  Mali_WrReg(0x1,0x0,0x310,0xffffffff);              /* writereg 10000310 ffffffff */
  Mali_WrReg(0x1,0x0,0x314,0xffffffff);              /* writereg 10000314 ffffffff */
  Mali_WrReg(0x1,0x0,0x318,0xffffffff);              /* writereg 10000318 ffffffff */
  Mali_WrReg(0x1,0x0,0x31c,0xffffffff);              /* writereg 1000031c ffffffff */
  Mali_WrReg(0x1,0x0,0x320,0xffffffff);              /* writereg 10000320 ffffffff */
  Mali_WrReg(0x1,0x0,0x324,0xffffffff);              /* writereg 10000324 ffffffff */
  Mali_WrReg(0x1,0x0,0x328,0xffffffff);              /* writereg 10000328 ffffffff */
  Mali_WrReg(0x1,0x0,0x32c,0xffffffff);              /* writereg 1000032c ffffffff */
  /* COMMENT */                                      /* #Management registers */
  Mali_WrReg(0x1,0x0,0x1020,0xffffffff);             /* writereg 10001020 ffffffff */
  Mali_WrReg(0x1,0x0,0x1028,0xffffffff);             /* writereg 10001028 ffffffff */
  Mali_WrReg(0x1,0x0,0x1040,0xffffffff);             /* writereg 10001040 ffffffff */
  Mali_WrReg(0x1,0x0,0x1044,0xffffffff);             /* writereg 10001044 ffffffff */
  Mali_WrReg(0x1,0x0,0x1048,0xffffffff);             /* writereg 10001048 ffffffff */
  Mali_WrReg(0x1,0x0,0x1060,0xffffffff);             /* writereg 10001060 ffffffff */
  Mali_WrReg(0x1,0x0,0x1064,0xffffffff);             /* writereg 10001064 ffffffff */
  Mali_WrReg(0x1,0x0,0x1068,0xffffffff);             /* writereg 10001068 ffffffff */
  Mali_WrReg(0x1,0x0,0x106c,0xffffffff);             /* writereg 1000106c ffffffff */
  Mali_WrReg(0x1,0x0,0x1080,0xffffffff);             /* writereg 10001080 ffffffff */
  Mali_WrReg(0x1,0x0,0x1084,0xffffff1f);             /* writereg 10001084 ffffff1f */
  Mali_WrReg(0x1,0x0,0x1088,0xffffffff);             /* writereg 10001088 ffffffff */
  Mali_WrReg(0x1,0x0,0x108c,0xffffffff);             /* writereg 1000108c ffffffff */
  Mali_WrReg(0x1,0x0,0x10a0,0xffffffff);             /* writereg 100010a0 ffffffff */
  Mali_WrReg(0x1,0x0,0x10a4,0xffffff1f);             /* writereg 100010a4 ffffff1f */
  Mali_WrReg(0x1,0x0,0x10a8,0xffffffff);             /* writereg 100010a8 ffffffff */
  Mali_WrReg(0x1,0x0,0x10ac,0xffffffff);             /* writereg 100010ac ffffffff */
  Mali_WrReg(0x1,0x0,0x10c0,0xffffffff);             /* writereg 100010c0 ffffffff */
  Mali_WrReg(0x1,0x0,0x10c4,0xffffffff);             /* writereg 100010c4 ffffffff */
  Mali_WrReg(0x1,0x0,0x10c8,0xffffffff);             /* writereg 100010c8 ffffffff */
  Mali_WrReg(0x1,0x0,0x10cc,0xffffffff);             /* writereg 100010cc ffffffff */
  /* COMMENT */                                      /* # Dump them */
  /* COMMENT */                                      /* #Frame registers */
  res |= Check_m400_pp_3_ibr03_check_0_a();          /* dump_reg 10000000 10000050 > ibr03_00000_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB0 */
  res |= Check_m400_pp_3_ibr03_check_0_b();          /* dump_reg 10000100 10000130 > ibr03_00001_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB1 */
  res |= Check_m400_pp_3_ibr03_check_0_c();          /* dump_reg 10000200 10000230 > ibr03_00002_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB2 */
  res |= Check_m400_pp_3_ibr03_check_0_d();          /* dump_reg 10000300 10000330 > ibr03_00003_reg.hex */
  /* COMMENT */                                      /* #Management registers */
  res |= Check_m400_pp_3_ibr03_check_0_e();          /* dump_reg 10001000 100010b0 > ibr03_00004_reg.hex */
  /* COMMENT */                                      /* #Write the soft reset pattern into CTRL_MGMT */
  Mali_WrReg(0x1,0x0,0x100c,0x00000020);             /* writereg 1000100c 00000020 */
  Mali_WaitForClk(100);                              /* wait posedge clk 100 */
  /* COMMENT */                                      /* # Dump again */
  /* COMMENT */                                      /* #Frame registers */
  res |= Check_m400_pp_3_ibr03_check_0_f();          /* dump_reg 10000000 10000050 > ibr03_00005_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB0 */
  res |= Check_m400_pp_3_ibr03_check_0_g();          /* dump_reg 10000100 10000130 > ibr03_00006_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB1 */
  res |= Check_m400_pp_3_ibr03_check_0_h();          /* dump_reg 10000200 10000230 > ibr03_00007_reg.hex */
  /* COMMENT */                                      /* #Writeback Registers for WB2 */
  res |= Check_m400_pp_3_ibr03_check_0_i();          /* dump_reg 10000300 10000330 > ibr03_00008_reg.hex */
  /* COMMENT */                                      /* #Management registers */
  res |= Check_m400_pp_3_ibr03_check_0_j();          /* dump_reg 10001000 100010b0 > ibr03_00009_reg.hex */
  Mali_MaskAllInterrupts();
  mali_step++;
  return res;
}

int Check_m400_pp_3_ibr03_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_a.data_10000000,1,0,0x0,0x50);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_b.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_c.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_d.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_e.data_10001000,1,0,0x1000,0x10b0);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_f () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_f.data_10000000,1,0,0x0,0x50);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_g () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_g.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_h () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_h.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_i () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_i.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_3_ibr03_check_0_j () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr03_check_0_j.data_10001000,1,0,0x1000,0x10b0);
  return res;
};
