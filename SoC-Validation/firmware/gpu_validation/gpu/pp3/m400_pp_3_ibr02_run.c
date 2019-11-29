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
#include "m400_pp_3_ibr02_check_0_a.h"
#include "m400_pp_3_ibr02_check_0_b.h"
#include "m400_pp_3_ibr02_check_0_c.h"
#include "m400_pp_3_ibr02_check_0_d.h"
#include "m400_pp_3_ibr02_check_0_e.h"
#include "m400_pp_3_ibr02_check_0_f.h"
#include "m400_pp_3_ibr02_check_0_g.h"
#include "m400_pp_3_ibr02_check_0_h.h"
#include "m400_pp_3_ibr02_check_0_i.h"
#include "m400_pp_3_ibr02_check_0_j.h"
#include "m400_pp_3_ibr02_check_0_k.h"
#include "m400_pp_3_ibr02_check_0_l.h"
#include "m400_pp_3_ibr02_check_0_m.h"
#include "m400_pp_3_ibr02_check_0_n.h"
#include "m400_pp_3_ibr02_check_0_o.h"

int Check_m400_pp_3_ibr02_check_0_a ();
int Check_m400_pp_3_ibr02_check_0_b ();
int Check_m400_pp_3_ibr02_check_0_c ();
int Check_m400_pp_3_ibr02_check_0_d ();
int Check_m400_pp_3_ibr02_check_0_e ();
int Check_m400_pp_3_ibr02_check_0_f ();
int Check_m400_pp_3_ibr02_check_0_g ();
int Check_m400_pp_3_ibr02_check_0_h ();
int Check_m400_pp_3_ibr02_check_0_i ();
int Check_m400_pp_3_ibr02_check_0_j ();
int Check_m400_pp_3_ibr02_check_0_k ();
int Check_m400_pp_3_ibr02_check_0_l ();
int Check_m400_pp_3_ibr02_check_0_m ();
int Check_m400_pp_3_ibr02_check_0_n ();
int Check_m400_pp_3_ibr02_check_0_o ();

int RunMaliTest_m400_pp_3_ibr02_part0 (void);

static int mali_step;

int RunMaliTest_m400_pp_3_ibr02 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_3_ibr02_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_3_ibr02_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_3_ibr02\n");

  /* COMMENT */                                      /* #IBR02 Mali200 */
  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write all ones */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
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
  Mali_WrReg(0x1,0x0,0x48,0xffffffff);               /* writereg 10000048 ffffffff */
  Mali_WrReg(0x1,0x0,0x4c,0xffffffff);               /* writereg 1000004c ffffffff */
  Mali_WrReg(0x1,0x0,0x50,0xffffffff);               /* writereg 10000050 ffffffff */
  Mali_WrReg(0x1,0x0,0x54,0xffffffff);               /* writereg 10000054 ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x0,0x100,0xffffffff);              /* writereg 10000100 ffffffff */
  Mali_WrReg(0x1,0x0,0x104,0xffffffff);              /* writereg 10000104 ffffffff */
  Mali_WrReg(0x1,0x0,0x108,0xffffffff);              /* writereg 10000108 ffffffff */
  Mali_WrReg(0x1,0x0,0x10c,0xffffff00);              /* writereg 1000010c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x110,0xffffffff);              /* writereg 10000110 ffffffff */
  Mali_WrReg(0x1,0x0,0x114,0xffffffff);              /* writereg 10000114 ffffffff */
  Mali_WrReg(0x1,0x0,0x118,0xffffffff);              /* writereg 10000118 ffffffff */
  Mali_WrReg(0x1,0x0,0x11c,0xffffffff);              /* writereg 1000011c ffffffff */
  Mali_WrReg(0x1,0x0,0x120,0xffffffff);              /* writereg 10000120 ffffffff */
  Mali_WrReg(0x1,0x0,0x124,0xffffffff);              /* writereg 10000124 ffffffff */
  Mali_WrReg(0x1,0x0,0x128,0xffffffff);              /* writereg 10000128 ffffffff */
  Mali_WrReg(0x1,0x0,0x12c,0xffffffff);              /* writereg 1000012c ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x0,0x200,0xffffffff);              /* writereg 10000200 ffffffff */
  Mali_WrReg(0x1,0x0,0x204,0xffffffff);              /* writereg 10000204 ffffffff */
  Mali_WrReg(0x1,0x0,0x208,0xffffffff);              /* writereg 10000208 ffffffff */
  Mali_WrReg(0x1,0x0,0x20c,0xffffff00);              /* writereg 1000020c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x210,0xffffffff);              /* writereg 10000210 ffffffff */
  Mali_WrReg(0x1,0x0,0x214,0xffffffff);              /* writereg 10000214 ffffffff */
  Mali_WrReg(0x1,0x0,0x218,0xffffffff);              /* writereg 10000218 ffffffff */
  Mali_WrReg(0x1,0x0,0x21c,0xffffffff);              /* writereg 1000021c ffffffff */
  Mali_WrReg(0x1,0x0,0x220,0xffffffff);              /* writereg 10000220 ffffffff */
  Mali_WrReg(0x1,0x0,0x224,0xffffffff);              /* writereg 10000224 ffffffff */
  Mali_WrReg(0x1,0x0,0x228,0xffffffff);              /* writereg 10000228 ffffffff */
  Mali_WrReg(0x1,0x0,0x22c,0xffffffff);              /* writereg 1000022c ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x0,0x300,0xffffffff);              /* writereg 10000300 ffffffff */
  Mali_WrReg(0x1,0x0,0x304,0xffffffff);              /* writereg 10000304 ffffffff */
  Mali_WrReg(0x1,0x0,0x308,0xffffffff);              /* writereg 10000308 ffffffff */
  Mali_WrReg(0x1,0x0,0x30c,0xffffff00);              /* writereg 1000030c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x310,0xffffffff);              /* writereg 10000310 ffffffff */
  Mali_WrReg(0x1,0x0,0x314,0xffffffff);              /* writereg 10000314 ffffffff */
  Mali_WrReg(0x1,0x0,0x318,0xffffffff);              /* writereg 10000318 ffffffff */
  Mali_WrReg(0x1,0x0,0x31c,0xffffffff);              /* writereg 1000031c ffffffff */
  Mali_WrReg(0x1,0x0,0x320,0xffffffff);              /* writereg 10000320 ffffffff */
  Mali_WrReg(0x1,0x0,0x324,0xffffffff);              /* writereg 10000324 ffffffff */
  Mali_WrReg(0x1,0x0,0x328,0xffffffff);              /* writereg 10000328 ffffffff */
  Mali_WrReg(0x1,0x0,0x32c,0xffffffff);              /* writereg 1000032c ffffffff */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x0,0x1000,0xffffffff);             /* writereg 10001000 ffffffff */
  Mali_WrReg(0x1,0x0,0x1004,0xffffffff);             /* writereg 10001004 ffffffff */
  Mali_WrReg(0x1,0x0,0x1008,0xffffffff);             /* writereg 10001008 ffffffff */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x0,0x1010,0xffffffff);             /* writereg 10001010 ffffffff */
  Mali_WrReg(0x1,0x0,0x1014,0xffffffff);             /* writereg 10001014 ffffffff */
  Mali_WrReg(0x1,0x0,0x1018,0xffffffff);             /* writereg 10001018 ffffffff */
  Mali_WrReg(0x1,0x0,0x101c,0xffffffff);             /* writereg 1000101c ffffffff */
  Mali_WrReg(0x1,0x0,0x1020,0xffffffff);             /* writereg 10001020 ffffffff */
  /* COMMENT */                                      /* # Skip Interrupt Clear register at 0x1024 */
  Mali_WrReg(0x1,0x0,0x1028,0xffffffff);             /* writereg 10001028 ffffffff */
  Mali_WrReg(0x1,0x0,0x102c,0xffffffff);             /* writereg 1000102c ffffffff */
  Mali_WrReg(0x1,0x0,0x1030,0xffffffff);             /* writereg 10001030 ffffffff */
  Mali_WrReg(0x1,0x0,0x1034,0xffffffff);             /* writereg 10001034 ffffffff */
  Mali_WrReg(0x1,0x0,0x1038,0xffffffff);             /* writereg 10001038 ffffffff */
  Mali_WrReg(0x1,0x0,0x103c,0xffffffff);             /* writereg 1000103c ffffffff */
  Mali_WrReg(0x1,0x0,0x1040,0xffffffff);             /* writereg 10001040 ffffffff */
  Mali_WrReg(0x1,0x0,0x1044,0xffffffff);             /* writereg 10001044 ffffffff */
  Mali_WrReg(0x1,0x0,0x1048,0xffffffff);             /* writereg 10001048 ffffffff */
  Mali_WrReg(0x1,0x0,0x104c,0xffffffff);             /* writereg 1000104c ffffffff */
  Mali_WrReg(0x1,0x0,0x1050,0xffffffff);             /* writereg 10001050 ffffffff */
  Mali_WrReg(0x1,0x0,0x1054,0xffffffff);             /* writereg 10001054 ffffffff */
  Mali_WrReg(0x1,0x0,0x1058,0xffffffff);             /* writereg 10001058 ffffffff */
  Mali_WrReg(0x1,0x0,0x105c,0xffffffff);             /* writereg 1000105c ffffffff */
  Mali_WrReg(0x1,0x0,0x1060,0xffffffff);             /* writereg 10001060 ffffffff */
  Mali_WrReg(0x1,0x0,0x1064,0xffffffff);             /* writereg 10001064 ffffffff */
  Mali_WrReg(0x1,0x0,0x1068,0xffffffff);             /* writereg 10001068 ffffffff */
  Mali_WrReg(0x1,0x0,0x106c,0xffffffff);             /* writereg 1000106c ffffffff */
  Mali_WrReg(0x1,0x0,0x1070,0xffffffff);             /* writereg 10001070 ffffffff */
  Mali_WrReg(0x1,0x0,0x1074,0xffffffff);             /* writereg 10001074 ffffffff */
  Mali_WrReg(0x1,0x0,0x1078,0xffffffff);             /* writereg 10001078 ffffffff */
  Mali_WrReg(0x1,0x0,0x107c,0xffffffff);             /* writereg 1000107c ffffffff */
  Mali_WrReg(0x1,0x0,0x1080,0xffffffff);             /* writereg 10001080 ffffffff */
  Mali_WrReg(0x1,0x0,0x1084,0xffffffff);             /* writereg 10001084 ffffffff */
  Mali_WrReg(0x1,0x0,0x1088,0xffffffff);             /* writereg 10001088 ffffffff */
  Mali_WrReg(0x1,0x0,0x108c,0xffffffff);             /* writereg 1000108c ffffffff */
  Mali_WrReg(0x1,0x0,0x1090,0xffffffff);             /* writereg 10001090 ffffffff */
  Mali_WrReg(0x1,0x0,0x1094,0xffffffff);             /* writereg 10001094 ffffffff */
  Mali_WrReg(0x1,0x0,0x1098,0xffffffff);             /* writereg 10001098 ffffffff */
  Mali_WrReg(0x1,0x0,0x109c,0xffffffff);             /* writereg 1000109c ffffffff */
  Mali_WrReg(0x1,0x0,0x10a0,0xffffffff);             /* writereg 100010a0 ffffffff */
  Mali_WrReg(0x1,0x0,0x10a4,0xffffffff);             /* writereg 100010a4 ffffffff */
  Mali_WrReg(0x1,0x0,0x10a8,0xffffffff);             /* writereg 100010a8 ffffffff */
  Mali_WrReg(0x1,0x0,0x10ac,0xffffffff);             /* writereg 100010ac ffffffff */
  Mali_WrReg(0x1,0x0,0x10b0,0xffffffff);             /* writereg 100010b0 ffffffff */
  Mali_WrReg(0x1,0x0,0x10b4,0xffffffff);             /* writereg 100010b4 ffffffff */
  Mali_WrReg(0x1,0x0,0x10b8,0xffffffff);             /* writereg 100010b8 ffffffff */
  Mali_WrReg(0x1,0x0,0x10bc,0xffffffff);             /* writereg 100010bc ffffffff */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_3_ibr02_check_0_a();          /* dump_reg 10000000 10000058 > ibr02_00000_reg.hex # Frame registers */
  res |= Check_m400_pp_3_ibr02_check_0_b();          /* dump_reg 10000100 10000130 > ibr02_00001_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_3_ibr02_check_0_c();          /* dump_reg 10000200 10000230 > ibr02_00002_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_3_ibr02_check_0_d();          /* dump_reg 10000300 10000330 > ibr02_00003_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_3_ibr02_check_0_e();          /* dump_reg 10001000 100010b8 > ibr02_00004_reg.hex # Management registers */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write all zeros */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
  Mali_WrReg(0x1,0x0,0x0,0x00000000);                /* writereg 10000000 00000000 */
  Mali_WrReg(0x1,0x0,0x4,0x00000000);                /* writereg 10000004 00000000 */
  Mali_WrReg(0x1,0x0,0x8,0x00000000);                /* writereg 10000008 00000000 */
  Mali_WrReg(0x1,0x0,0xc,0x00000000);                /* writereg 1000000c 00000000 */
  Mali_WrReg(0x1,0x0,0x10,0x00000000);               /* writereg 10000010 00000000 */
  Mali_WrReg(0x1,0x0,0x14,0x00000000);               /* writereg 10000014 00000000 */
  Mali_WrReg(0x1,0x0,0x18,0x00000000);               /* writereg 10000018 00000000 */
  Mali_WrReg(0x1,0x0,0x1c,0x00000000);               /* writereg 1000001c 00000000 */
  Mali_WrReg(0x1,0x0,0x20,0x00000000);               /* writereg 10000020 00000000 */
  Mali_WrReg(0x1,0x0,0x24,0x00000000);               /* writereg 10000024 00000000 */
  Mali_WrReg(0x1,0x0,0x28,0x00000000);               /* writereg 10000028 00000000 */
  Mali_WrReg(0x1,0x0,0x2c,0x00000000);               /* writereg 1000002c 00000000 */
  Mali_WrReg(0x1,0x0,0x30,0x00000000);               /* writereg 10000030 00000000 */
  Mali_WrReg(0x1,0x0,0x34,0x00000000);               /* writereg 10000034 00000000 */
  Mali_WrReg(0x1,0x0,0x38,0x00000000);               /* writereg 10000038 00000000 */
  Mali_WrReg(0x1,0x0,0x3c,0x00000000);               /* writereg 1000003c 00000000 */
  Mali_WrReg(0x1,0x0,0x40,0x00000000);               /* writereg 10000040 00000000 */
  Mali_WrReg(0x1,0x0,0x44,0x00000000);               /* writereg 10000044 00000000 */
  Mali_WrReg(0x1,0x0,0x48,0x00000000);               /* writereg 10000048 00000000 */
  Mali_WrReg(0x1,0x0,0x4c,0x00000000);               /* writereg 1000004c 00000000 */
  Mali_WrReg(0x1,0x0,0x50,0x00000000);               /* writereg 10000050 00000000 */
  Mali_WrReg(0x1,0x0,0x54,0x00000000);               /* writereg 10000054 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x0,0x100,0x00000000);              /* writereg 10000100 00000000 */
  Mali_WrReg(0x1,0x0,0x104,0x00000000);              /* writereg 10000104 00000000 */
  Mali_WrReg(0x1,0x0,0x108,0x00000000);              /* writereg 10000108 00000000 */
  Mali_WrReg(0x1,0x0,0x10c,0x00000000);              /* writereg 1000010c 00000000 */
  Mali_WrReg(0x1,0x0,0x110,0x00000000);              /* writereg 10000110 00000000 */
  Mali_WrReg(0x1,0x0,0x114,0x00000000);              /* writereg 10000114 00000000 */
  Mali_WrReg(0x1,0x0,0x118,0x00000000);              /* writereg 10000118 00000000 */
  Mali_WrReg(0x1,0x0,0x11c,0x00000000);              /* writereg 1000011c 00000000 */
  Mali_WrReg(0x1,0x0,0x120,0x00000000);              /* writereg 10000120 00000000 */
  Mali_WrReg(0x1,0x0,0x124,0x00000000);              /* writereg 10000124 00000000 */
  Mali_WrReg(0x1,0x0,0x128,0x00000000);              /* writereg 10000128 00000000 */
  Mali_WrReg(0x1,0x0,0x12c,0x00000000);              /* writereg 1000012c 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x0,0x200,0x00000000);              /* writereg 10000200 00000000 */
  Mali_WrReg(0x1,0x0,0x204,0x00000000);              /* writereg 10000204 00000000 */
  Mali_WrReg(0x1,0x0,0x208,0x00000000);              /* writereg 10000208 00000000 */
  Mali_WrReg(0x1,0x0,0x20c,0x00000000);              /* writereg 1000020c 00000000 */
  Mali_WrReg(0x1,0x0,0x210,0x00000000);              /* writereg 10000210 00000000 */
  Mali_WrReg(0x1,0x0,0x214,0x00000000);              /* writereg 10000214 00000000 */
  Mali_WrReg(0x1,0x0,0x218,0x00000000);              /* writereg 10000218 00000000 */
  Mali_WrReg(0x1,0x0,0x21c,0x00000000);              /* writereg 1000021c 00000000 */
  Mali_WrReg(0x1,0x0,0x220,0x00000000);              /* writereg 10000220 00000000 */
  Mali_WrReg(0x1,0x0,0x224,0x00000000);              /* writereg 10000224 00000000 */
  Mali_WrReg(0x1,0x0,0x228,0x00000000);              /* writereg 10000228 00000000 */
  Mali_WrReg(0x1,0x0,0x22c,0x00000000);              /* writereg 1000022c 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x0,0x300,0x00000000);              /* writereg 10000300 00000000 */
  Mali_WrReg(0x1,0x0,0x304,0x00000000);              /* writereg 10000304 00000000 */
  Mali_WrReg(0x1,0x0,0x308,0x00000000);              /* writereg 10000308 00000000 */
  Mali_WrReg(0x1,0x0,0x30c,0x00000000);              /* writereg 1000030c 00000000 */
  Mali_WrReg(0x1,0x0,0x310,0x00000000);              /* writereg 10000310 00000000 */
  Mali_WrReg(0x1,0x0,0x314,0x00000000);              /* writereg 10000314 00000000 */
  Mali_WrReg(0x1,0x0,0x318,0x00000000);              /* writereg 10000318 00000000 */
  Mali_WrReg(0x1,0x0,0x31c,0x00000000);              /* writereg 1000031c 00000000 */
  Mali_WrReg(0x1,0x0,0x320,0x00000000);              /* writereg 10000320 00000000 */
  Mali_WrReg(0x1,0x0,0x324,0x00000000);              /* writereg 10000324 00000000 */
  Mali_WrReg(0x1,0x0,0x328,0x00000000);              /* writereg 10000328 00000000 */
  Mali_WrReg(0x1,0x0,0x32c,0x00000000);              /* writereg 1000032c 00000000 */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x0,0x1000,0x00000000);             /* writereg 10001000 00000000 */
  Mali_WrReg(0x1,0x0,0x1004,0x00000000);             /* writereg 10001004 00000000 */
  Mali_WrReg(0x1,0x0,0x1008,0x00000000);             /* writereg 10001008 00000000 */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x0,0x1010,0x00000000);             /* writereg 10001010 00000000 */
  Mali_WrReg(0x1,0x0,0x1014,0x00000000);             /* writereg 10001014 00000000 */
  Mali_WrReg(0x1,0x0,0x1018,0x00000000);             /* writereg 10001018 00000000 */
  Mali_WrReg(0x1,0x0,0x101c,0x00000000);             /* writereg 1000101c 00000000 */
  Mali_WrReg(0x1,0x0,0x1020,0x00000000);             /* writereg 10001020 00000000 */
  Mali_WrReg(0x1,0x0,0x1024,0xffffffff);             /* writereg 10001024 ffffffff # Clear all interrupts */
  Mali_WrReg(0x1,0x0,0x1028,0x00000000);             /* writereg 10001028 00000000 */
  Mali_WrReg(0x1,0x0,0x102c,0x00000000);             /* writereg 1000102c 00000000 */
  Mali_WrReg(0x1,0x0,0x1030,0x00000000);             /* writereg 10001030 00000000 */
  Mali_WrReg(0x1,0x0,0x1034,0x00000000);             /* writereg 10001034 00000000 */
  Mali_WrReg(0x1,0x0,0x1038,0x00000000);             /* writereg 10001038 00000000 */
  Mali_WrReg(0x1,0x0,0x103c,0x00000000);             /* writereg 1000103c 00000000 */
  Mali_WrReg(0x1,0x0,0x1040,0x00000000);             /* writereg 10001040 00000000 */
  Mali_WrReg(0x1,0x0,0x1044,0x00000000);             /* writereg 10001044 00000000 */
  Mali_WrReg(0x1,0x0,0x1048,0x00000000);             /* writereg 10001048 00000000 */
  Mali_WrReg(0x1,0x0,0x104c,0x00000000);             /* writereg 1000104c 00000000 */
  Mali_WrReg(0x1,0x0,0x1050,0x00000000);             /* writereg 10001050 00000000 */
  Mali_WrReg(0x1,0x0,0x1054,0x00000000);             /* writereg 10001054 00000000 */
  Mali_WrReg(0x1,0x0,0x1058,0x00000000);             /* writereg 10001058 00000000 */
  Mali_WrReg(0x1,0x0,0x105c,0x00000000);             /* writereg 1000105c 00000000 */
  Mali_WrReg(0x1,0x0,0x1060,0x00000000);             /* writereg 10001060 00000000 */
  Mali_WrReg(0x1,0x0,0x1064,0x00000000);             /* writereg 10001064 00000000 */
  Mali_WrReg(0x1,0x0,0x1068,0x00000000);             /* writereg 10001068 00000000 */
  Mali_WrReg(0x1,0x0,0x106c,0x00000000);             /* writereg 1000106c 00000000 */
  Mali_WrReg(0x1,0x0,0x1070,0x00000000);             /* writereg 10001070 00000000 */
  Mali_WrReg(0x1,0x0,0x1074,0x00000000);             /* writereg 10001074 00000000 */
  Mali_WrReg(0x1,0x0,0x1078,0x00000000);             /* writereg 10001078 00000000 */
  Mali_WrReg(0x1,0x0,0x107c,0x00000000);             /* writereg 1000107c 00000000 */
  Mali_WrReg(0x1,0x0,0x1080,0x00000000);             /* writereg 10001080 00000000 */
  Mali_WrReg(0x1,0x0,0x1084,0x00000000);             /* writereg 10001084 00000000 */
  Mali_WrReg(0x1,0x0,0x1088,0x00000000);             /* writereg 10001088 00000000 */
  Mali_WrReg(0x1,0x0,0x108c,0x00000000);             /* writereg 1000108c 00000000 */
  Mali_WrReg(0x1,0x0,0x1090,0x00000000);             /* writereg 10001090 00000000 */
  Mali_WrReg(0x1,0x0,0x1094,0x00000000);             /* writereg 10001094 00000000 */
  Mali_WrReg(0x1,0x0,0x1098,0x00000000);             /* writereg 10001098 00000000 */
  Mali_WrReg(0x1,0x0,0x109c,0x00000000);             /* writereg 1000109c 00000000 */
  Mali_WrReg(0x1,0x0,0x10a0,0x00000000);             /* writereg 100010a0 00000000 */
  Mali_WrReg(0x1,0x0,0x10a4,0x00000000);             /* writereg 100010a4 00000000 */
  Mali_WrReg(0x1,0x0,0x10a8,0x00000000);             /* writereg 100010a8 00000000 */
  Mali_WrReg(0x1,0x0,0x10ac,0x00000000);             /* writereg 100010ac 00000000 */
  Mali_WrReg(0x1,0x0,0x10b0,0x00000000);             /* writereg 100010b0 00000000 */
  Mali_WrReg(0x1,0x0,0x10b4,0x00000000);             /* writereg 100010b4 00000000 */
  Mali_WrReg(0x1,0x0,0x10b8,0x00000000);             /* writereg 100010b8 00000000 */
  Mali_WrReg(0x1,0x0,0x10bc,0x00000000);             /* writereg 100010bc 00000000 */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_3_ibr02_check_0_f();          /* dump_reg 10000000 10000058 > ibr02_00010_reg.hex # Frame registers */
  res |= Check_m400_pp_3_ibr02_check_0_g();          /* dump_reg 10000100 10000130 > ibr02_00011_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_3_ibr02_check_0_h();          /* dump_reg 10000200 10000230 > ibr02_00012_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_3_ibr02_check_0_i();          /* dump_reg 10000300 10000330 > ibr02_00013_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_3_ibr02_check_0_j();          /* dump_reg 10001000 100010b8 > ibr02_00014_reg.hex # Management registers */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write "random" values */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
  Mali_WrReg(0x1,0x0,0x0,0x25564423);                /* writereg 10000000 25564423 */
  Mali_WrReg(0x1,0x0,0x4,0x34445655);                /* writereg 10000004 34445655 */
  Mali_WrReg(0x1,0x0,0x8,0x46345675);                /* writereg 10000008 46345675 */
  Mali_WrReg(0x1,0x0,0xc,0xaabb3455);                /* writereg 1000000c aabb3455 */
  Mali_WrReg(0x1,0x0,0x10,0x32456326);               /* writereg 10000010 32456326 */
  Mali_WrReg(0x1,0x0,0x14,0x34562313);               /* writereg 10000014 34562313 */
  Mali_WrReg(0x1,0x0,0x18,0x23465235);               /* writereg 10000018 23465235 */
  Mali_WrReg(0x1,0x0,0x1c,0x23435541);               /* writereg 1000001c 23435541 */
  Mali_WrReg(0x1,0x0,0x20,0x62453564);               /* writereg 10000020 62453564 */
  Mali_WrReg(0x1,0x0,0x24,0x34563256);               /* writereg 10000024 34563256 */
  Mali_WrReg(0x1,0x0,0x28,0x23466653);               /* writereg 10000028 23466653 */
  Mali_WrReg(0x1,0x0,0x2c,0x45673422);               /* writereg 1000002c 45673422 */
  Mali_WrReg(0x1,0x0,0x30,0x23456767);               /* writereg 10000030 23456767 */
  Mali_WrReg(0x1,0x0,0x34,0x23453423);               /* writereg 10000034 23453423 */
  Mali_WrReg(0x1,0x0,0x38,0x13246676);               /* writereg 10000038 13246676 */
  Mali_WrReg(0x1,0x0,0x3c,0x01123433);               /* writereg 1000003c 01123433 */
  Mali_WrReg(0x1,0x0,0x40,0x36546756);               /* writereg 10000040 36546756 */
  Mali_WrReg(0x1,0x0,0x44,0x14545656);               /* writereg 10000044 14545656 */
  Mali_WrReg(0x1,0x0,0x48,0x67835344);               /* writereg 10000048 67835344 */
  Mali_WrReg(0x1,0x0,0x4c,0x24564556);               /* writereg 1000004c 24564556 */
  Mali_WrReg(0x1,0x0,0x50,0x948b6f21);               /* writereg 10000050 948b6f21 */
  Mali_WrReg(0x1,0x0,0x54,0xbc7d2531);               /* writereg 10000054 bc7d2531 */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x0,0x100,0x36457677);              /* writereg 10000100 36457677 */
  Mali_WrReg(0x1,0x0,0x104,0x65787686);              /* writereg 10000104 65787686 */
  Mali_WrReg(0x1,0x0,0x108,0x04784450);              /* writereg 10000108 04784450 */
  Mali_WrReg(0x1,0x0,0x10c,0x65787600);              /* writereg 1000010c 65787600 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x110,0x46786445);              /* writereg 10000110 46786445 */
  Mali_WrReg(0x1,0x0,0x114,0x12342345);              /* writereg 10000114 12342345 */
  Mali_WrReg(0x1,0x0,0x118,0x42563462);              /* writereg 10000118 42563462 */
  Mali_WrReg(0x1,0x0,0x11c,0x34576700);              /* writereg 1000011c 34576700 */
  Mali_WrReg(0x1,0x0,0x120,0x76834223);              /* writereg 10000120 76834223 */
  Mali_WrReg(0x1,0x0,0x124,0x34534643);              /* writereg 10000124 34534643 */
  Mali_WrReg(0x1,0x0,0x128,0x65466654);              /* writereg 10000128 65466654 */
  Mali_WrReg(0x1,0x0,0x12c,0x65468798);              /* writereg 1000012c 65468798 */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x0,0x200,0x22323440);              /* writereg 10000200 22323440 */
  Mali_WrReg(0x1,0x0,0x204,0x02456456);              /* writereg 10000204 02456456 */
  Mali_WrReg(0x1,0x0,0x208,0x03436565);              /* writereg 10000208 03436565 */
  Mali_WrReg(0x1,0x0,0x20c,0x65465500);              /* writereg 1000020c 65465500 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x210,0x98436216);              /* writereg 10000210 98436216 */
  Mali_WrReg(0x1,0x0,0x214,0x65498436);              /* writereg 10000214 65498436 */
  Mali_WrReg(0x1,0x0,0x218,0x65494685);              /* writereg 10000218 65494685 */
  Mali_WrReg(0x1,0x0,0x21c,0x02342315);              /* writereg 1000021c 02342315 */
  Mali_WrReg(0x1,0x0,0x220,0x76451355);              /* writereg 10000220 76451355 */
  Mali_WrReg(0x1,0x0,0x224,0x65215723);              /* writereg 10000224 65215723 */
  Mali_WrReg(0x1,0x0,0x228,0x05673564);              /* writereg 10000228 05673564 */
  Mali_WrReg(0x1,0x0,0x22c,0x12345560);              /* writereg 1000022c 12345560 */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x0,0x300,0x65498433);              /* writereg 10000300 65498433 */
  Mali_WrReg(0x1,0x0,0x304,0x34561324);              /* writereg 10000304 34561324 */
  Mali_WrReg(0x1,0x0,0x308,0x56756534);              /* writereg 10000308 56756534 */
  Mali_WrReg(0x1,0x0,0x30c,0x57623400);              /* writereg 1000030c 57623400 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x0,0x310,0x76763220);              /* writereg 10000310 76763220 */
  Mali_WrReg(0x1,0x0,0x314,0x54765524);              /* writereg 10000314 54765524 */
  Mali_WrReg(0x1,0x0,0x318,0x03425603);              /* writereg 10000318 03425603 */
  Mali_WrReg(0x1,0x0,0x31c,0x23460125);              /* writereg 1000031c 23460125 */
  Mali_WrReg(0x1,0x0,0x320,0x654aa236);              /* writereg 10000320 654aa236 */
  Mali_WrReg(0x1,0x0,0x324,0x6534534b);              /* writereg 10000324 6534534b */
  Mali_WrReg(0x1,0x0,0x328,0xbb654bb6);              /* writereg 10000328 bb654bb6 */
  Mali_WrReg(0x1,0x0,0x32c,0x654c654c);              /* writereg 1000032c 654c654c */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x0,0x1000,0x00ccc654);             /* writereg 10001000 00ccc654 */
  Mali_WrReg(0x1,0x0,0x1004,0xd65d46d0);             /* writereg 10001004 d65d46d0 */
  Mali_WrReg(0x1,0x0,0x1008,0x65446665);             /* writereg 10001008 65446665 */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x0,0x1010,0xd654d654);             /* writereg 10001010 d654d654 */
  Mali_WrReg(0x1,0x0,0x1014,0xa6546d5d);             /* writereg 10001014 a6546d5d */
  Mali_WrReg(0x1,0x0,0x1018,0x6a54567d);             /* writereg 10001018 6a54567d */
  Mali_WrReg(0x1,0x0,0x101c,0x6ad5d56a);             /* writereg 1000101c 6ad5d56a */
  Mali_WrReg(0x1,0x0,0x1020,0xd564b65d);             /* writereg 10001020 d564b65d */
  /* COMMENT */                                      /* # Skip Interrupt Clear register at 0x1024 */
  Mali_WrReg(0x1,0x0,0x1028,0x6d54d65b);             /* writereg 10001028 6d54d65b */
  Mali_WrReg(0x1,0x0,0x102c,0x0564d6b4);             /* writereg 1000102c 0564d6b4 */
  Mali_WrReg(0x1,0x0,0x1030,0xb654b6b4);             /* writereg 10001030 b654b6b4 */
  Mali_WrReg(0x1,0x0,0x1034,0xa65c46a4);             /* writereg 10001034 a65c46a4 */
  Mali_WrReg(0x1,0x0,0x1038,0xa65c4a65);             /* writereg 10001038 a65c4a65 */
  Mali_WrReg(0x1,0x0,0x103c,0xac654a65);             /* writereg 1000103c ac654a65 */
  Mali_WrReg(0x1,0x0,0x1040,0xa65c4a65);             /* writereg 10001040 a65c4a65 */
  Mali_WrReg(0x1,0x0,0x1044,0xdd5d5d4d);             /* writereg 10001044 dd5d5d4d */
  Mali_WrReg(0x1,0x0,0x1048,0xa65d4d6d);             /* writereg 10001048 a65d4d6d */
  Mali_WrReg(0x1,0x0,0x104c,0x6aa6a65d);             /* writereg 1000104c 6aa6a65d */
  Mali_WrReg(0x1,0x0,0x1050,0xb6b4b654);             /* writereg 10001050 b6b4b654 */
  Mali_WrReg(0x1,0x0,0x1054,0xbbb6f5f5);             /* writereg 10001054 bbb6f5f5 */
  Mali_WrReg(0x1,0x0,0x1058,0xff65f4f6);             /* writereg 10001058 ff65f4f6 */
  Mali_WrReg(0x1,0x0,0x105c,0xf6ff54ff);             /* writereg 1000105c f6ff54ff */
  Mali_WrReg(0x1,0x0,0x1060,0xff6f54f5);             /* writereg 10001060 ff6f54f5 */
  Mali_WrReg(0x1,0x0,0x1064,0xfff3f3ff);             /* writereg 10001064 fff3f3ff */
  Mali_WrReg(0x1,0x0,0x1068,0xaaafffbb);             /* writereg 10001068 aaafffbb */
  Mali_WrReg(0x1,0x0,0x106c,0xfffbbfff);             /* writereg 1000106c fffbbfff */
  Mali_WrReg(0x1,0x0,0x1070,0xfffddfff);             /* writereg 10001070 fffddfff */
  Mali_WrReg(0x1,0x0,0x1074,0xfdcdfdcd);             /* writereg 10001074 fdcdfdcd */
  Mali_WrReg(0x1,0x0,0x1078,0x06d54fd5);             /* writereg 10001078 06d54fd5 */
  Mali_WrReg(0x1,0x0,0x107c,0xddd0ccdf);             /* writereg 1000107c ddd0ccdf */
  Mali_WrReg(0x1,0x0,0x1080,0x65465466);             /* writereg 10001080 65465466 */
  Mali_WrReg(0x1,0x0,0x1084,0x43763764);             /* writereg 10001084 43763764 */
  Mali_WrReg(0x1,0x0,0x1088,0x34577772);             /* writereg 10001088 34577772 */
  Mali_WrReg(0x1,0x0,0x108c,0x25333445);             /* writereg 1000108c 25333445 */
  Mali_WrReg(0x1,0x0,0x1090,0x45766623);             /* writereg 10001090 45766623 */
  Mali_WrReg(0x1,0x0,0x1094,0x34555222);             /* writereg 10001094 34555222 */
  Mali_WrReg(0x1,0x0,0x1098,0x34567775);             /* writereg 10001098 34567775 */
  Mali_WrReg(0x1,0x0,0x109c,0x57745345);             /* writereg 1000109c 57745345 */
  Mali_WrReg(0x1,0x0,0x10a0,0x34545652);             /* writereg 100010a0 34545652 */
  Mali_WrReg(0x1,0x0,0x10a4,0x65493212);             /* writereg 100010a4 65493212 */
  Mali_WrReg(0x1,0x0,0x10a8,0x98784954);             /* writereg 100010a8 98784954 */
  Mali_WrReg(0x1,0x0,0x10ac,0x57466433);             /* writereg 100010ac 57466433 */
  Mali_WrReg(0x1,0x0,0x10b0,0x21231382);             /* writereg 100010b0 21231382 */
  Mali_WrReg(0x1,0x0,0x10b4,0x13284236);             /* writereg 100010b4 13284236 */
  Mali_WrReg(0x1,0x0,0x10b8,0x12305433);             /* writereg 100010b8 12305433 */
  Mali_WrReg(0x1,0x0,0x10bc,0x34256455);             /* writereg 100010bc 34256455 */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_3_ibr02_check_0_k();          /* dump_reg 10000000 10000058 > ibr02_00020_reg.hex # Frame registers */
  res |= Check_m400_pp_3_ibr02_check_0_l();          /* dump_reg 10000100 10000130 > ibr02_00021_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_3_ibr02_check_0_m();          /* dump_reg 10000200 10000230 > ibr02_00022_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_3_ibr02_check_0_n();          /* dump_reg 10000300 10000330 > ibr02_00023_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_3_ibr02_check_0_o();          /* dump_reg 10001000 100010b8 > ibr02_00024_reg.hex # Management registers */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_3_ibr02_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_a.data_10000000,1,0,0x0,0x58);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_b.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_c.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_d.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_e.data_10001000,1,0,0x1000,0x10b8);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_f () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_f.data_10000000,1,0,0x0,0x58);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_g () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_g.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_h () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_h.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_i () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_i.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_j () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_j.data_10001000,1,0,0x1000,0x10b8);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_k () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_k.data_10000000,1,0,0x0,0x58);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_l () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_l.data_10000100,1,0,0x100,0x130);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_m () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_m.data_10000200,1,0,0x200,0x230);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_n () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_n.data_10000300,1,0,0x300,0x330);
  return res;
};

int Check_m400_pp_3_ibr02_check_0_o () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibr02_check_0_o.data_10001000,1,0,0x1000,0x10b8);
  return res;
};
