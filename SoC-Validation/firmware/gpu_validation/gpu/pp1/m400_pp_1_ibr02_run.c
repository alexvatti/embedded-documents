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
* Modified  : $Date: 2013/05/14 11:58:44 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_1_ibr02_check_0_a.h"
#include "m400_pp_1_ibr02_check_0_b.h"
#include "m400_pp_1_ibr02_check_0_c.h"
#include "m400_pp_1_ibr02_check_0_d.h"
#include "m400_pp_1_ibr02_check_0_e.h"
#include "m400_pp_1_ibr02_check_0_f.h"
#include "m400_pp_1_ibr02_check_0_g.h"
#include "m400_pp_1_ibr02_check_0_h.h"
#include "m400_pp_1_ibr02_check_0_i.h"
#include "m400_pp_1_ibr02_check_0_j.h"
#include "m400_pp_1_ibr02_check_0_k.h"
#include "m400_pp_1_ibr02_check_0_l.h"
#include "m400_pp_1_ibr02_check_0_m.h"
#include "m400_pp_1_ibr02_check_0_n.h"
#include "m400_pp_1_ibr02_check_0_o.h"

int Check_m400_pp_1_ibr02_check_0_a ();
int Check_m400_pp_1_ibr02_check_0_b ();
int Check_m400_pp_1_ibr02_check_0_c ();
int Check_m400_pp_1_ibr02_check_0_d ();
int Check_m400_pp_1_ibr02_check_0_e ();
int Check_m400_pp_1_ibr02_check_0_f ();
int Check_m400_pp_1_ibr02_check_0_g ();
int Check_m400_pp_1_ibr02_check_0_h ();
int Check_m400_pp_1_ibr02_check_0_i ();
int Check_m400_pp_1_ibr02_check_0_j ();
int Check_m400_pp_1_ibr02_check_0_k ();
int Check_m400_pp_1_ibr02_check_0_l ();
int Check_m400_pp_1_ibr02_check_0_m ();
int Check_m400_pp_1_ibr02_check_0_n ();
int Check_m400_pp_1_ibr02_check_0_o ();

int RunMaliTest_m400_pp_1_ibr02_part0 (void);

static int mali_step;

int RunMaliTest_m400_pp_1_ibr02 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_1_ibr02_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_1_ibr02_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_1_ibr02\n");

  /* COMMENT */                                      /* #IBR02 Mali200 */
  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write all ones */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
  Mali_WrReg(0x1,0x1,0x0,0xffffffff);                /* writereg 10010000 ffffffff */
  Mali_WrReg(0x1,0x1,0x4,0xffffffff);                /* writereg 10010004 ffffffff */
  Mali_WrReg(0x1,0x1,0x8,0xffffffff);                /* writereg 10010008 ffffffff */
  Mali_WrReg(0x1,0x1,0xc,0xffffffff);                /* writereg 1001000c ffffffff */
  Mali_WrReg(0x1,0x1,0x10,0xffffffff);               /* writereg 10010010 ffffffff */
  Mali_WrReg(0x1,0x1,0x14,0xffffffff);               /* writereg 10010014 ffffffff */
  Mali_WrReg(0x1,0x1,0x18,0xffffffff);               /* writereg 10010018 ffffffff */
  Mali_WrReg(0x1,0x1,0x1c,0xffffffff);               /* writereg 1001001c ffffffff */
  Mali_WrReg(0x1,0x1,0x20,0xffffffff);               /* writereg 10010020 ffffffff */
  Mali_WrReg(0x1,0x1,0x24,0xffffffff);               /* writereg 10010024 ffffffff */
  Mali_WrReg(0x1,0x1,0x28,0xffffffff);               /* writereg 10010028 ffffffff */
  Mali_WrReg(0x1,0x1,0x2c,0xffffffff);               /* writereg 1001002c ffffffff */
  Mali_WrReg(0x1,0x1,0x30,0xffffffff);               /* writereg 10010030 ffffffff */
  Mali_WrReg(0x1,0x1,0x34,0xffffffff);               /* writereg 10010034 ffffffff */
  Mali_WrReg(0x1,0x1,0x38,0xffffffff);               /* writereg 10010038 ffffffff */
  Mali_WrReg(0x1,0x1,0x3c,0xffffffff);               /* writereg 1001003c ffffffff */
  Mali_WrReg(0x1,0x1,0x40,0xffffffff);               /* writereg 10010040 ffffffff */
  Mali_WrReg(0x1,0x1,0x44,0xffffffff);               /* writereg 10010044 ffffffff */
  Mali_WrReg(0x1,0x1,0x48,0xffffffff);               /* writereg 10010048 ffffffff */
  Mali_WrReg(0x1,0x1,0x4c,0xffffffff);               /* writereg 1001004c ffffffff */
  Mali_WrReg(0x1,0x1,0x50,0xffffffff);               /* writereg 10010050 ffffffff */
  Mali_WrReg(0x1,0x1,0x54,0xffffffff);               /* writereg 10010054 ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x1,0x100,0xffffffff);              /* writereg 10010100 ffffffff */
  Mali_WrReg(0x1,0x1,0x104,0xffffffff);              /* writereg 10010104 ffffffff */
  Mali_WrReg(0x1,0x1,0x108,0xffffffff);              /* writereg 10010108 ffffffff */
  Mali_WrReg(0x1,0x1,0x10c,0xffffff00);              /* writereg 1001010c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x110,0xffffffff);              /* writereg 10010110 ffffffff */
  Mali_WrReg(0x1,0x1,0x114,0xffffffff);              /* writereg 10010114 ffffffff */
  Mali_WrReg(0x1,0x1,0x118,0xffffffff);              /* writereg 10010118 ffffffff */
  Mali_WrReg(0x1,0x1,0x11c,0xffffffff);              /* writereg 1001011c ffffffff */
  Mali_WrReg(0x1,0x1,0x120,0xffffffff);              /* writereg 10010120 ffffffff */
  Mali_WrReg(0x1,0x1,0x124,0xffffffff);              /* writereg 10010124 ffffffff */
  Mali_WrReg(0x1,0x1,0x128,0xffffffff);              /* writereg 10010128 ffffffff */
  Mali_WrReg(0x1,0x1,0x12c,0xffffffff);              /* writereg 1001012c ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x1,0x200,0xffffffff);              /* writereg 10010200 ffffffff */
  Mali_WrReg(0x1,0x1,0x204,0xffffffff);              /* writereg 10010204 ffffffff */
  Mali_WrReg(0x1,0x1,0x208,0xffffffff);              /* writereg 10010208 ffffffff */
  Mali_WrReg(0x1,0x1,0x20c,0xffffff00);              /* writereg 1001020c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x210,0xffffffff);              /* writereg 10010210 ffffffff */
  Mali_WrReg(0x1,0x1,0x214,0xffffffff);              /* writereg 10010214 ffffffff */
  Mali_WrReg(0x1,0x1,0x218,0xffffffff);              /* writereg 10010218 ffffffff */
  Mali_WrReg(0x1,0x1,0x21c,0xffffffff);              /* writereg 1001021c ffffffff */
  Mali_WrReg(0x1,0x1,0x220,0xffffffff);              /* writereg 10010220 ffffffff */
  Mali_WrReg(0x1,0x1,0x224,0xffffffff);              /* writereg 10010224 ffffffff */
  Mali_WrReg(0x1,0x1,0x228,0xffffffff);              /* writereg 10010228 ffffffff */
  Mali_WrReg(0x1,0x1,0x22c,0xffffffff);              /* writereg 1001022c ffffffff */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x1,0x300,0xffffffff);              /* writereg 10010300 ffffffff */
  Mali_WrReg(0x1,0x1,0x304,0xffffffff);              /* writereg 10010304 ffffffff */
  Mali_WrReg(0x1,0x1,0x308,0xffffffff);              /* writereg 10010308 ffffffff */
  Mali_WrReg(0x1,0x1,0x30c,0xffffff00);              /* writereg 1001030c ffffff00 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x310,0xffffffff);              /* writereg 10010310 ffffffff */
  Mali_WrReg(0x1,0x1,0x314,0xffffffff);              /* writereg 10010314 ffffffff */
  Mali_WrReg(0x1,0x1,0x318,0xffffffff);              /* writereg 10010318 ffffffff */
  Mali_WrReg(0x1,0x1,0x31c,0xffffffff);              /* writereg 1001031c ffffffff */
  Mali_WrReg(0x1,0x1,0x320,0xffffffff);              /* writereg 10010320 ffffffff */
  Mali_WrReg(0x1,0x1,0x324,0xffffffff);              /* writereg 10010324 ffffffff */
  Mali_WrReg(0x1,0x1,0x328,0xffffffff);              /* writereg 10010328 ffffffff */
  Mali_WrReg(0x1,0x1,0x32c,0xffffffff);              /* writereg 1001032c ffffffff */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x1,0x1000,0xffffffff);             /* writereg 10011000 ffffffff */
  Mali_WrReg(0x1,0x1,0x1004,0xffffffff);             /* writereg 10011004 ffffffff */
  Mali_WrReg(0x1,0x1,0x1008,0xffffffff);             /* writereg 10011008 ffffffff */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x1,0x1010,0xffffffff);             /* writereg 10011010 ffffffff */
  Mali_WrReg(0x1,0x1,0x1014,0xffffffff);             /* writereg 10011014 ffffffff */
  Mali_WrReg(0x1,0x1,0x1018,0xffffffff);             /* writereg 10011018 ffffffff */
  Mali_WrReg(0x1,0x1,0x101c,0xffffffff);             /* writereg 1001101c ffffffff */
  Mali_WrReg(0x1,0x1,0x1020,0xffffffff);             /* writereg 10011020 ffffffff */
  /* COMMENT */                                      /* # Skip Interrupt Clear register at 0x1024 */
  Mali_WrReg(0x1,0x1,0x1028,0xffffffff);             /* writereg 10011028 ffffffff */
  Mali_WrReg(0x1,0x1,0x102c,0xffffffff);             /* writereg 1001102c ffffffff */
  Mali_WrReg(0x1,0x1,0x1030,0xffffffff);             /* writereg 10011030 ffffffff */
  Mali_WrReg(0x1,0x1,0x1034,0xffffffff);             /* writereg 10011034 ffffffff */
  Mali_WrReg(0x1,0x1,0x1038,0xffffffff);             /* writereg 10011038 ffffffff */
  Mali_WrReg(0x1,0x1,0x103c,0xffffffff);             /* writereg 1001103c ffffffff */
  Mali_WrReg(0x1,0x1,0x1040,0xffffffff);             /* writereg 10011040 ffffffff */
  Mali_WrReg(0x1,0x1,0x1044,0xffffffff);             /* writereg 10011044 ffffffff */
  Mali_WrReg(0x1,0x1,0x1048,0xffffffff);             /* writereg 10011048 ffffffff */
  Mali_WrReg(0x1,0x1,0x104c,0xffffffff);             /* writereg 1001104c ffffffff */
  Mali_WrReg(0x1,0x1,0x1050,0xffffffff);             /* writereg 10011050 ffffffff */
  Mali_WrReg(0x1,0x1,0x1054,0xffffffff);             /* writereg 10011054 ffffffff */
  Mali_WrReg(0x1,0x1,0x1058,0xffffffff);             /* writereg 10011058 ffffffff */
  Mali_WrReg(0x1,0x1,0x105c,0xffffffff);             /* writereg 1001105c ffffffff */
  Mali_WrReg(0x1,0x1,0x1060,0xffffffff);             /* writereg 10011060 ffffffff */
  Mali_WrReg(0x1,0x1,0x1064,0xffffffff);             /* writereg 10011064 ffffffff */
  Mali_WrReg(0x1,0x1,0x1068,0xffffffff);             /* writereg 10011068 ffffffff */
  Mali_WrReg(0x1,0x1,0x106c,0xffffffff);             /* writereg 1001106c ffffffff */
  Mali_WrReg(0x1,0x1,0x1070,0xffffffff);             /* writereg 10011070 ffffffff */
  Mali_WrReg(0x1,0x1,0x1074,0xffffffff);             /* writereg 10011074 ffffffff */
  Mali_WrReg(0x1,0x1,0x1078,0xffffffff);             /* writereg 10011078 ffffffff */
  Mali_WrReg(0x1,0x1,0x107c,0xffffffff);             /* writereg 1001107c ffffffff */
  Mali_WrReg(0x1,0x1,0x1080,0xffffffff);             /* writereg 10011080 ffffffff */
  Mali_WrReg(0x1,0x1,0x1084,0xffffffff);             /* writereg 10011084 ffffffff */
  Mali_WrReg(0x1,0x1,0x1088,0xffffffff);             /* writereg 10011088 ffffffff */
  Mali_WrReg(0x1,0x1,0x108c,0xffffffff);             /* writereg 1001108c ffffffff */
  Mali_WrReg(0x1,0x1,0x1090,0xffffffff);             /* writereg 10011090 ffffffff */
  Mali_WrReg(0x1,0x1,0x1094,0xffffffff);             /* writereg 10011094 ffffffff */
  Mali_WrReg(0x1,0x1,0x1098,0xffffffff);             /* writereg 10011098 ffffffff */
  Mali_WrReg(0x1,0x1,0x109c,0xffffffff);             /* writereg 1001109c ffffffff */
  Mali_WrReg(0x1,0x1,0x10a0,0xffffffff);             /* writereg 100110a0 ffffffff */
  Mali_WrReg(0x1,0x1,0x10a4,0xffffffff);             /* writereg 100110a4 ffffffff */
  Mali_WrReg(0x1,0x1,0x10a8,0xffffffff);             /* writereg 100110a8 ffffffff */
  Mali_WrReg(0x1,0x1,0x10ac,0xffffffff);             /* writereg 100110ac ffffffff */
  Mali_WrReg(0x1,0x1,0x10b0,0xffffffff);             /* writereg 100110b0 ffffffff */
  Mali_WrReg(0x1,0x1,0x10b4,0xffffffff);             /* writereg 100110b4 ffffffff */
  Mali_WrReg(0x1,0x1,0x10b8,0xffffffff);             /* writereg 100110b8 ffffffff */
  Mali_WrReg(0x1,0x1,0x10bc,0xffffffff);             /* writereg 100110bc ffffffff */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_1_ibr02_check_0_a();          /* dump_reg 10010000 10010058 > ibr02_00000_reg.hex # Frame registers */
  res |= Check_m400_pp_1_ibr02_check_0_b();          /* dump_reg 10010100 10010130 > ibr02_00001_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_1_ibr02_check_0_c();          /* dump_reg 10010200 10010230 > ibr02_00002_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_1_ibr02_check_0_d();          /* dump_reg 10010300 10010330 > ibr02_00003_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_1_ibr02_check_0_e();          /* dump_reg 10011000 100110b8 > ibr02_00004_reg.hex # Management registers */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write all zeros */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
  Mali_WrReg(0x1,0x1,0x0,0x00000000);                /* writereg 10010000 00000000 */
  Mali_WrReg(0x1,0x1,0x4,0x00000000);                /* writereg 10010004 00000000 */
  Mali_WrReg(0x1,0x1,0x8,0x00000000);                /* writereg 10010008 00000000 */
  Mali_WrReg(0x1,0x1,0xc,0x00000000);                /* writereg 1001000c 00000000 */
  Mali_WrReg(0x1,0x1,0x10,0x00000000);               /* writereg 10010010 00000000 */
  Mali_WrReg(0x1,0x1,0x14,0x00000000);               /* writereg 10010014 00000000 */
  Mali_WrReg(0x1,0x1,0x18,0x00000000);               /* writereg 10010018 00000000 */
  Mali_WrReg(0x1,0x1,0x1c,0x00000000);               /* writereg 1001001c 00000000 */
  Mali_WrReg(0x1,0x1,0x20,0x00000000);               /* writereg 10010020 00000000 */
  Mali_WrReg(0x1,0x1,0x24,0x00000000);               /* writereg 10010024 00000000 */
  Mali_WrReg(0x1,0x1,0x28,0x00000000);               /* writereg 10010028 00000000 */
  Mali_WrReg(0x1,0x1,0x2c,0x00000000);               /* writereg 1001002c 00000000 */
  Mali_WrReg(0x1,0x1,0x30,0x00000000);               /* writereg 10010030 00000000 */
  Mali_WrReg(0x1,0x1,0x34,0x00000000);               /* writereg 10010034 00000000 */
  Mali_WrReg(0x1,0x1,0x38,0x00000000);               /* writereg 10010038 00000000 */
  Mali_WrReg(0x1,0x1,0x3c,0x00000000);               /* writereg 1001003c 00000000 */
  Mali_WrReg(0x1,0x1,0x40,0x00000000);               /* writereg 10010040 00000000 */
  Mali_WrReg(0x1,0x1,0x44,0x00000000);               /* writereg 10010044 00000000 */
  Mali_WrReg(0x1,0x1,0x48,0x00000000);               /* writereg 10010048 00000000 */
  Mali_WrReg(0x1,0x1,0x4c,0x00000000);               /* writereg 1001004c 00000000 */
  Mali_WrReg(0x1,0x1,0x50,0x00000000);               /* writereg 10010050 00000000 */
  Mali_WrReg(0x1,0x1,0x54,0x00000000);               /* writereg 10010054 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x1,0x100,0x00000000);              /* writereg 10010100 00000000 */
  Mali_WrReg(0x1,0x1,0x104,0x00000000);              /* writereg 10010104 00000000 */
  Mali_WrReg(0x1,0x1,0x108,0x00000000);              /* writereg 10010108 00000000 */
  Mali_WrReg(0x1,0x1,0x10c,0x00000000);              /* writereg 1001010c 00000000 */
  Mali_WrReg(0x1,0x1,0x110,0x00000000);              /* writereg 10010110 00000000 */
  Mali_WrReg(0x1,0x1,0x114,0x00000000);              /* writereg 10010114 00000000 */
  Mali_WrReg(0x1,0x1,0x118,0x00000000);              /* writereg 10010118 00000000 */
  Mali_WrReg(0x1,0x1,0x11c,0x00000000);              /* writereg 1001011c 00000000 */
  Mali_WrReg(0x1,0x1,0x120,0x00000000);              /* writereg 10010120 00000000 */
  Mali_WrReg(0x1,0x1,0x124,0x00000000);              /* writereg 10010124 00000000 */
  Mali_WrReg(0x1,0x1,0x128,0x00000000);              /* writereg 10010128 00000000 */
  Mali_WrReg(0x1,0x1,0x12c,0x00000000);              /* writereg 1001012c 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x1,0x200,0x00000000);              /* writereg 10010200 00000000 */
  Mali_WrReg(0x1,0x1,0x204,0x00000000);              /* writereg 10010204 00000000 */
  Mali_WrReg(0x1,0x1,0x208,0x00000000);              /* writereg 10010208 00000000 */
  Mali_WrReg(0x1,0x1,0x20c,0x00000000);              /* writereg 1001020c 00000000 */
  Mali_WrReg(0x1,0x1,0x210,0x00000000);              /* writereg 10010210 00000000 */
  Mali_WrReg(0x1,0x1,0x214,0x00000000);              /* writereg 10010214 00000000 */
  Mali_WrReg(0x1,0x1,0x218,0x00000000);              /* writereg 10010218 00000000 */
  Mali_WrReg(0x1,0x1,0x21c,0x00000000);              /* writereg 1001021c 00000000 */
  Mali_WrReg(0x1,0x1,0x220,0x00000000);              /* writereg 10010220 00000000 */
  Mali_WrReg(0x1,0x1,0x224,0x00000000);              /* writereg 10010224 00000000 */
  Mali_WrReg(0x1,0x1,0x228,0x00000000);              /* writereg 10010228 00000000 */
  Mali_WrReg(0x1,0x1,0x22c,0x00000000);              /* writereg 1001022c 00000000 */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x1,0x300,0x00000000);              /* writereg 10010300 00000000 */
  Mali_WrReg(0x1,0x1,0x304,0x00000000);              /* writereg 10010304 00000000 */
  Mali_WrReg(0x1,0x1,0x308,0x00000000);              /* writereg 10010308 00000000 */
  Mali_WrReg(0x1,0x1,0x30c,0x00000000);              /* writereg 1001030c 00000000 */
  Mali_WrReg(0x1,0x1,0x310,0x00000000);              /* writereg 10010310 00000000 */
  Mali_WrReg(0x1,0x1,0x314,0x00000000);              /* writereg 10010314 00000000 */
  Mali_WrReg(0x1,0x1,0x318,0x00000000);              /* writereg 10010318 00000000 */
  Mali_WrReg(0x1,0x1,0x31c,0x00000000);              /* writereg 1001031c 00000000 */
  Mali_WrReg(0x1,0x1,0x320,0x00000000);              /* writereg 10010320 00000000 */
  Mali_WrReg(0x1,0x1,0x324,0x00000000);              /* writereg 10010324 00000000 */
  Mali_WrReg(0x1,0x1,0x328,0x00000000);              /* writereg 10010328 00000000 */
  Mali_WrReg(0x1,0x1,0x32c,0x00000000);              /* writereg 1001032c 00000000 */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x1,0x1000,0x00000000);             /* writereg 10011000 00000000 */
  Mali_WrReg(0x1,0x1,0x1004,0x00000000);             /* writereg 10011004 00000000 */
  Mali_WrReg(0x1,0x1,0x1008,0x00000000);             /* writereg 10011008 00000000 */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x1,0x1010,0x00000000);             /* writereg 10011010 00000000 */
  Mali_WrReg(0x1,0x1,0x1014,0x00000000);             /* writereg 10011014 00000000 */
  Mali_WrReg(0x1,0x1,0x1018,0x00000000);             /* writereg 10011018 00000000 */
  Mali_WrReg(0x1,0x1,0x101c,0x00000000);             /* writereg 1001101c 00000000 */
  Mali_WrReg(0x1,0x1,0x1020,0x00000000);             /* writereg 10011020 00000000 */
  Mali_WrReg(0x1,0x1,0x1024,0xffffffff);             /* writereg 10011024 ffffffff # Clear all interrupts */
  Mali_WrReg(0x1,0x1,0x1028,0x00000000);             /* writereg 10011028 00000000 */
  Mali_WrReg(0x1,0x1,0x102c,0x00000000);             /* writereg 1001102c 00000000 */
  Mali_WrReg(0x1,0x1,0x1030,0x00000000);             /* writereg 10011030 00000000 */
  Mali_WrReg(0x1,0x1,0x1034,0x00000000);             /* writereg 10011034 00000000 */
  Mali_WrReg(0x1,0x1,0x1038,0x00000000);             /* writereg 10011038 00000000 */
  Mali_WrReg(0x1,0x1,0x103c,0x00000000);             /* writereg 1001103c 00000000 */
  Mali_WrReg(0x1,0x1,0x1040,0x00000000);             /* writereg 10011040 00000000 */
  Mali_WrReg(0x1,0x1,0x1044,0x00000000);             /* writereg 10011044 00000000 */
  Mali_WrReg(0x1,0x1,0x1048,0x00000000);             /* writereg 10011048 00000000 */
  Mali_WrReg(0x1,0x1,0x104c,0x00000000);             /* writereg 1001104c 00000000 */
  Mali_WrReg(0x1,0x1,0x1050,0x00000000);             /* writereg 10011050 00000000 */
  Mali_WrReg(0x1,0x1,0x1054,0x00000000);             /* writereg 10011054 00000000 */
  Mali_WrReg(0x1,0x1,0x1058,0x00000000);             /* writereg 10011058 00000000 */
  Mali_WrReg(0x1,0x1,0x105c,0x00000000);             /* writereg 1001105c 00000000 */
  Mali_WrReg(0x1,0x1,0x1060,0x00000000);             /* writereg 10011060 00000000 */
  Mali_WrReg(0x1,0x1,0x1064,0x00000000);             /* writereg 10011064 00000000 */
  Mali_WrReg(0x1,0x1,0x1068,0x00000000);             /* writereg 10011068 00000000 */
  Mali_WrReg(0x1,0x1,0x106c,0x00000000);             /* writereg 1001106c 00000000 */
  Mali_WrReg(0x1,0x1,0x1070,0x00000000);             /* writereg 10011070 00000000 */
  Mali_WrReg(0x1,0x1,0x1074,0x00000000);             /* writereg 10011074 00000000 */
  Mali_WrReg(0x1,0x1,0x1078,0x00000000);             /* writereg 10011078 00000000 */
  Mali_WrReg(0x1,0x1,0x107c,0x00000000);             /* writereg 1001107c 00000000 */
  Mali_WrReg(0x1,0x1,0x1080,0x00000000);             /* writereg 10011080 00000000 */
  Mali_WrReg(0x1,0x1,0x1084,0x00000000);             /* writereg 10011084 00000000 */
  Mali_WrReg(0x1,0x1,0x1088,0x00000000);             /* writereg 10011088 00000000 */
  Mali_WrReg(0x1,0x1,0x108c,0x00000000);             /* writereg 1001108c 00000000 */
  Mali_WrReg(0x1,0x1,0x1090,0x00000000);             /* writereg 10011090 00000000 */
  Mali_WrReg(0x1,0x1,0x1094,0x00000000);             /* writereg 10011094 00000000 */
  Mali_WrReg(0x1,0x1,0x1098,0x00000000);             /* writereg 10011098 00000000 */
  Mali_WrReg(0x1,0x1,0x109c,0x00000000);             /* writereg 1001109c 00000000 */
  Mali_WrReg(0x1,0x1,0x10a0,0x00000000);             /* writereg 100110a0 00000000 */
  Mali_WrReg(0x1,0x1,0x10a4,0x00000000);             /* writereg 100110a4 00000000 */
  Mali_WrReg(0x1,0x1,0x10a8,0x00000000);             /* writereg 100110a8 00000000 */
  Mali_WrReg(0x1,0x1,0x10ac,0x00000000);             /* writereg 100110ac 00000000 */
  Mali_WrReg(0x1,0x1,0x10b0,0x00000000);             /* writereg 100110b0 00000000 */
  Mali_WrReg(0x1,0x1,0x10b4,0x00000000);             /* writereg 100110b4 00000000 */
  Mali_WrReg(0x1,0x1,0x10b8,0x00000000);             /* writereg 100110b8 00000000 */
  Mali_WrReg(0x1,0x1,0x10bc,0x00000000);             /* writereg 100110bc 00000000 */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_1_ibr02_check_0_f();          /* dump_reg 10010000 10010058 > ibr02_00010_reg.hex # Frame registers */
  res |= Check_m400_pp_1_ibr02_check_0_g();          /* dump_reg 10010100 10010130 > ibr02_00011_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_1_ibr02_check_0_h();          /* dump_reg 10010200 10010230 > ibr02_00012_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_1_ibr02_check_0_i();          /* dump_reg 10010300 10010330 > ibr02_00013_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_1_ibr02_check_0_j();          /* dump_reg 10011000 100110b8 > ibr02_00014_reg.hex # Management registers */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* # Write "random" values */
  /* COMMENT */                                      /* # */
  /* COMMENT */                                      /* #- Frame registers */
  Mali_WrReg(0x1,0x1,0x0,0x25564423);                /* writereg 10010000 25564423 */
  Mali_WrReg(0x1,0x1,0x4,0x34445655);                /* writereg 10010004 34445655 */
  Mali_WrReg(0x1,0x1,0x8,0x46345675);                /* writereg 10010008 46345675 */
  Mali_WrReg(0x1,0x1,0xc,0xaabb3455);                /* writereg 1001000c aabb3455 */
  Mali_WrReg(0x1,0x1,0x10,0x32456326);               /* writereg 10010010 32456326 */
  Mali_WrReg(0x1,0x1,0x14,0x34562313);               /* writereg 10010014 34562313 */
  Mali_WrReg(0x1,0x1,0x18,0x23465235);               /* writereg 10010018 23465235 */
  Mali_WrReg(0x1,0x1,0x1c,0x23435541);               /* writereg 1001001c 23435541 */
  Mali_WrReg(0x1,0x1,0x20,0x62453564);               /* writereg 10010020 62453564 */
  Mali_WrReg(0x1,0x1,0x24,0x34563256);               /* writereg 10010024 34563256 */
  Mali_WrReg(0x1,0x1,0x28,0x23466653);               /* writereg 10010028 23466653 */
  Mali_WrReg(0x1,0x1,0x2c,0x45673422);               /* writereg 1001002c 45673422 */
  Mali_WrReg(0x1,0x1,0x30,0x23456767);               /* writereg 10010030 23456767 */
  Mali_WrReg(0x1,0x1,0x34,0x23453423);               /* writereg 10010034 23453423 */
  Mali_WrReg(0x1,0x1,0x38,0x13246676);               /* writereg 10010038 13246676 */
  Mali_WrReg(0x1,0x1,0x3c,0x01123433);               /* writereg 1001003c 01123433 */
  Mali_WrReg(0x1,0x1,0x40,0x36546756);               /* writereg 10010040 36546756 */
  Mali_WrReg(0x1,0x1,0x44,0x14545656);               /* writereg 10010044 14545656 */
  Mali_WrReg(0x1,0x1,0x48,0x67835344);               /* writereg 10010048 67835344 */
  Mali_WrReg(0x1,0x1,0x4c,0x24564556);               /* writereg 1001004c 24564556 */
  Mali_WrReg(0x1,0x1,0x50,0x948b6f21);               /* writereg 10010050 948b6f21 */
  Mali_WrReg(0x1,0x1,0x54,0xbc7d2531);               /* writereg 10010054 bc7d2531 */
  /* COMMENT */                                      /* #- Writeback registers WB0 */
  Mali_WrReg(0x1,0x1,0x100,0x36457677);              /* writereg 10010100 36457677 */
  Mali_WrReg(0x1,0x1,0x104,0x65787686);              /* writereg 10010104 65787686 */
  Mali_WrReg(0x1,0x1,0x108,0x04784450);              /* writereg 10010108 04784450 */
  Mali_WrReg(0x1,0x1,0x10c,0x65787600);              /* writereg 1001010c 65787600 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x110,0x46786445);              /* writereg 10010110 46786445 */
  Mali_WrReg(0x1,0x1,0x114,0x12342345);              /* writereg 10010114 12342345 */
  Mali_WrReg(0x1,0x1,0x118,0x42563462);              /* writereg 10010118 42563462 */
  Mali_WrReg(0x1,0x1,0x11c,0x34576700);              /* writereg 1001011c 34576700 */
  Mali_WrReg(0x1,0x1,0x120,0x76834223);              /* writereg 10010120 76834223 */
  Mali_WrReg(0x1,0x1,0x124,0x34534643);              /* writereg 10010124 34534643 */
  Mali_WrReg(0x1,0x1,0x128,0x65466654);              /* writereg 10010128 65466654 */
  Mali_WrReg(0x1,0x1,0x12c,0x65468798);              /* writereg 1001012c 65468798 */
  /* COMMENT */                                      /* #- Writeback registers WB1 */
  Mali_WrReg(0x1,0x1,0x200,0x22323440);              /* writereg 10010200 22323440 */
  Mali_WrReg(0x1,0x1,0x204,0x02456456);              /* writereg 10010204 02456456 */
  Mali_WrReg(0x1,0x1,0x208,0x03436565);              /* writereg 10010208 03436565 */
  Mali_WrReg(0x1,0x1,0x20c,0x65465500);              /* writereg 1001020c 65465500 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x210,0x98436216);              /* writereg 10010210 98436216 */
  Mali_WrReg(0x1,0x1,0x214,0x65498436);              /* writereg 10010214 65498436 */
  Mali_WrReg(0x1,0x1,0x218,0x65494685);              /* writereg 10010218 65494685 */
  Mali_WrReg(0x1,0x1,0x21c,0x02342315);              /* writereg 1001021c 02342315 */
  Mali_WrReg(0x1,0x1,0x220,0x76451355);              /* writereg 10010220 76451355 */
  Mali_WrReg(0x1,0x1,0x224,0x65215723);              /* writereg 10010224 65215723 */
  Mali_WrReg(0x1,0x1,0x228,0x05673564);              /* writereg 10010228 05673564 */
  Mali_WrReg(0x1,0x1,0x22c,0x12345560);              /* writereg 1001022c 12345560 */
  /* COMMENT */                                      /* #- Writeback registers WB2 */
  Mali_WrReg(0x1,0x1,0x300,0x65498433);              /* writereg 10010300 65498433 */
  Mali_WrReg(0x1,0x1,0x304,0x34561324);              /* writereg 10010304 34561324 */
  Mali_WrReg(0x1,0x1,0x308,0x56756534);              /* writereg 10010308 56756534 */
  Mali_WrReg(0x1,0x1,0x30c,0x57623400);              /* writereg 1001030c 57623400 # Setting high/low part at same time is undefined */
  Mali_WrReg(0x1,0x1,0x310,0x76763220);              /* writereg 10010310 76763220 */
  Mali_WrReg(0x1,0x1,0x314,0x54765524);              /* writereg 10010314 54765524 */
  Mali_WrReg(0x1,0x1,0x318,0x03425603);              /* writereg 10010318 03425603 */
  Mali_WrReg(0x1,0x1,0x31c,0x23460125);              /* writereg 1001031c 23460125 */
  Mali_WrReg(0x1,0x1,0x320,0x654aa236);              /* writereg 10010320 654aa236 */
  Mali_WrReg(0x1,0x1,0x324,0x6534534b);              /* writereg 10010324 6534534b */
  Mali_WrReg(0x1,0x1,0x328,0xbb654bb6);              /* writereg 10010328 bb654bb6 */
  Mali_WrReg(0x1,0x1,0x32c,0x654c654c);              /* writereg 1001032c 654c654c */
  /* COMMENT */                                      /* #- Management registers */
  Mali_WrReg(0x1,0x1,0x1000,0x00ccc654);             /* writereg 10011000 00ccc654 */
  Mali_WrReg(0x1,0x1,0x1004,0xd65d46d0);             /* writereg 10011004 d65d46d0 */
  Mali_WrReg(0x1,0x1,0x1008,0x65446665);             /* writereg 10011008 65446665 */
  /* COMMENT */                                      /* # Skip Control Management register at 0x100c */
  Mali_WrReg(0x1,0x1,0x1010,0xd654d654);             /* writereg 10011010 d654d654 */
  Mali_WrReg(0x1,0x1,0x1014,0xa6546d5d);             /* writereg 10011014 a6546d5d */
  Mali_WrReg(0x1,0x1,0x1018,0x6a54567d);             /* writereg 10011018 6a54567d */
  Mali_WrReg(0x1,0x1,0x101c,0x6ad5d56a);             /* writereg 1001101c 6ad5d56a */
  Mali_WrReg(0x1,0x1,0x1020,0xd564b65d);             /* writereg 10011020 d564b65d */
  /* COMMENT */                                      /* # Skip Interrupt Clear register at 0x1024 */
  Mali_WrReg(0x1,0x1,0x1028,0x6d54d65b);             /* writereg 10011028 6d54d65b */
  Mali_WrReg(0x1,0x1,0x102c,0x0564d6b4);             /* writereg 1001102c 0564d6b4 */
  Mali_WrReg(0x1,0x1,0x1030,0xb654b6b4);             /* writereg 10011030 b654b6b4 */
  Mali_WrReg(0x1,0x1,0x1034,0xa65c46a4);             /* writereg 10011034 a65c46a4 */
  Mali_WrReg(0x1,0x1,0x1038,0xa65c4a65);             /* writereg 10011038 a65c4a65 */
  Mali_WrReg(0x1,0x1,0x103c,0xac654a65);             /* writereg 1001103c ac654a65 */
  Mali_WrReg(0x1,0x1,0x1040,0xa65c4a65);             /* writereg 10011040 a65c4a65 */
  Mali_WrReg(0x1,0x1,0x1044,0xdd5d5d4d);             /* writereg 10011044 dd5d5d4d */
  Mali_WrReg(0x1,0x1,0x1048,0xa65d4d6d);             /* writereg 10011048 a65d4d6d */
  Mali_WrReg(0x1,0x1,0x104c,0x6aa6a65d);             /* writereg 1001104c 6aa6a65d */
  Mali_WrReg(0x1,0x1,0x1050,0xb6b4b654);             /* writereg 10011050 b6b4b654 */
  Mali_WrReg(0x1,0x1,0x1054,0xbbb6f5f5);             /* writereg 10011054 bbb6f5f5 */
  Mali_WrReg(0x1,0x1,0x1058,0xff65f4f6);             /* writereg 10011058 ff65f4f6 */
  Mali_WrReg(0x1,0x1,0x105c,0xf6ff54ff);             /* writereg 1001105c f6ff54ff */
  Mali_WrReg(0x1,0x1,0x1060,0xff6f54f5);             /* writereg 10011060 ff6f54f5 */
  Mali_WrReg(0x1,0x1,0x1064,0xfff3f3ff);             /* writereg 10011064 fff3f3ff */
  Mali_WrReg(0x1,0x1,0x1068,0xaaafffbb);             /* writereg 10011068 aaafffbb */
  Mali_WrReg(0x1,0x1,0x106c,0xfffbbfff);             /* writereg 1001106c fffbbfff */
  Mali_WrReg(0x1,0x1,0x1070,0xfffddfff);             /* writereg 10011070 fffddfff */
  Mali_WrReg(0x1,0x1,0x1074,0xfdcdfdcd);             /* writereg 10011074 fdcdfdcd */
  Mali_WrReg(0x1,0x1,0x1078,0x06d54fd5);             /* writereg 10011078 06d54fd5 */
  Mali_WrReg(0x1,0x1,0x107c,0xddd0ccdf);             /* writereg 1001107c ddd0ccdf */
  Mali_WrReg(0x1,0x1,0x1080,0x65465466);             /* writereg 10011080 65465466 */
  Mali_WrReg(0x1,0x1,0x1084,0x43763764);             /* writereg 10011084 43763764 */
  Mali_WrReg(0x1,0x1,0x1088,0x34577772);             /* writereg 10011088 34577772 */
  Mali_WrReg(0x1,0x1,0x108c,0x25333445);             /* writereg 1001108c 25333445 */
  Mali_WrReg(0x1,0x1,0x1090,0x45766623);             /* writereg 10011090 45766623 */
  Mali_WrReg(0x1,0x1,0x1094,0x34555222);             /* writereg 10011094 34555222 */
  Mali_WrReg(0x1,0x1,0x1098,0x34567775);             /* writereg 10011098 34567775 */
  Mali_WrReg(0x1,0x1,0x109c,0x57745345);             /* writereg 1001109c 57745345 */
  Mali_WrReg(0x1,0x1,0x10a0,0x34545652);             /* writereg 100110a0 34545652 */
  Mali_WrReg(0x1,0x1,0x10a4,0x65493212);             /* writereg 100110a4 65493212 */
  Mali_WrReg(0x1,0x1,0x10a8,0x98784954);             /* writereg 100110a8 98784954 */
  Mali_WrReg(0x1,0x1,0x10ac,0x57466433);             /* writereg 100110ac 57466433 */
  Mali_WrReg(0x1,0x1,0x10b0,0x21231382);             /* writereg 100110b0 21231382 */
  Mali_WrReg(0x1,0x1,0x10b4,0x13284236);             /* writereg 100110b4 13284236 */
  Mali_WrReg(0x1,0x1,0x10b8,0x12305433);             /* writereg 100110b8 12305433 */
  Mali_WrReg(0x1,0x1,0x10bc,0x34256455);             /* writereg 100110bc 34256455 */
  /* COMMENT */                                      /* #- Dump */
  res |= Check_m400_pp_1_ibr02_check_0_k();          /* dump_reg 10010000 10010058 > ibr02_00020_reg.hex # Frame registers */
  res |= Check_m400_pp_1_ibr02_check_0_l();          /* dump_reg 10010100 10010130 > ibr02_00021_reg.hex # Writeback Registers for WB0 */
  res |= Check_m400_pp_1_ibr02_check_0_m();          /* dump_reg 10010200 10010230 > ibr02_00022_reg.hex # Writeback Registers for WB1 */
  res |= Check_m400_pp_1_ibr02_check_0_n();          /* dump_reg 10010300 10010330 > ibr02_00023_reg.hex # Writeback Registers for WB2 */
  res |= Check_m400_pp_1_ibr02_check_0_o();          /* dump_reg 10011000 100110b8 > ibr02_00024_reg.hex # Management registers */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_1_ibr02_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_a.data_10010000,1,1,0x0,0x58);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_b.data_10010100,1,1,0x100,0x130);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_c.data_10010200,1,1,0x200,0x230);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_d.data_10010300,1,1,0x300,0x330);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_e.data_10011000,1,1,0x1000,0x10b8);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_f () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_f.data_10010000,1,1,0x0,0x58);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_g () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_g.data_10010100,1,1,0x100,0x130);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_h () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_h.data_10010200,1,1,0x200,0x230);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_i () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_i.data_10010300,1,1,0x300,0x330);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_j () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_j.data_10011000,1,1,0x1000,0x10b8);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_k () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_k.data_10010000,1,1,0x0,0x58);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_l () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_l.data_10010100,1,1,0x100,0x130);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_m () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_m.data_10010200,1,1,0x200,0x230);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_n () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_n.data_10010300,1,1,0x300,0x330);
  return res;
};

int Check_m400_pp_1_ibr02_check_0_o () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_ibr02_check_0_o.data_10011000,1,1,0x1000,0x10b8);
  return res;
};
