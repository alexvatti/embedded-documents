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
* Modified  : $Date: 2013/05/14 11:57:51 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_gp_F000_read_write_reg_check_0_a.h"
#include "m400_gp_F000_read_write_reg_check_0_b.h"
#include "m400_gp_F000_read_write_reg_check_0_c.h"
#include "m400_gp_F000_read_write_reg_check_0_d.h"
#include "m400_gp_F000_read_write_reg_check_0_e.h"
#include "m400_gp_F000_read_write_reg_check_0_f.h"

int Check_m400_gp_F000_read_write_reg_check_0_a ();
int Check_m400_gp_F000_read_write_reg_check_0_b ();
int Check_m400_gp_F000_read_write_reg_check_0_c ();
int Check_m400_gp_F000_read_write_reg_check_0_d ();
int Check_m400_gp_F000_read_write_reg_check_0_e ();
int Check_m400_gp_F000_read_write_reg_check_0_f ();

int RunMaliTest_m400_gp_F000_read_write_reg_part0 (void);

static int mali_step;

int RunMaliTest_m400_gp_F000_read_write_reg (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F000_read_write_reg_part0();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F000_read_write_reg_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F000_read_write_reg\n");

  /* COMMENT */                                      /* # Test initial reset values */
  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_reset1.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_a();

  /* COMMENT */                                      /* # Write inverted reset values */
  Mali_WrReg(0x0,0x0,0x0,0xffffffff);                /* writereg 00 ffffffff */
  Mali_WrReg(0x0,0x0,0x4,0xffffffff);                /* writereg 04 ffffffff */
  Mali_WrReg(0x0,0x0,0x8,0xffffffff);                /* writereg 08 ffffffff */
  Mali_WrReg(0x0,0x0,0xc,0xffffffff);                /* writereg 0c ffffffff */
  Mali_WrReg(0x0,0x0,0x10,0xffffffff);               /* writereg 10 ffffffff */
  Mali_WrReg(0x0,0x0,0x14,0xffffffff);               /* writereg 14 ffffffff */
  Mali_WrReg(0x0,0x0,0x18,0xffffffff);               /* writereg 18 ffffffff */
  Mali_WrReg(0x0,0x0,0x1c,0xffffffff);               /* writereg 1c ffffffff */
  /* COMMENT */                                      /* # Skip command register at 0x20 */
  Mali_WrReg(0x0,0x0,0x24,0xffffffff);               /* writereg 24 ffffffff */
  /* COMMENT */                                      /* # Skip interrupt clear at 0x28 */
  Mali_WrReg(0x0,0x0,0x2c,0xffffffff);               /* writereg 2c ffffffff */
  Mali_WrReg(0x0,0x0,0x30,0xffffffff);               /* writereg 30 ffffffff */
  Mali_WrReg(0x0,0x0,0x34,0xffffffff);               /* writereg 34 ffffffff */
  Mali_WrReg(0x0,0x0,0x38,0x000000ff);               /* writereg 38 000000ff */
  Mali_WrReg(0x0,0x0,0x3c,0xffffffff);               /* writereg 3c ffffffff */
  Mali_WrReg(0x0,0x0,0x40,0xffffffff);               /* writereg 40 ffffffff */
  Mali_WrReg(0x0,0x0,0x44,0xffffffff);               /* writereg 44 ffffffff */
  Mali_WrReg(0x0,0x0,0x48,0xffffffff);               /* writereg 48 ffffffff */
  Mali_WrReg(0x0,0x0,0x4c,0xffffffff);               /* writereg 4c ffffffff */
  Mali_WrReg(0x0,0x0,0x50,0xffffffff);               /* writereg 50 ffffffff */
  Mali_WrReg(0x0,0x0,0x54,0xffffffff);               /* writereg 54 ffffffff */
  Mali_WrReg(0x0,0x0,0x58,0xffffffff);               /* writereg 58 ffffffff */
  Mali_WrReg(0x0,0x0,0x5c,0xffffffff);               /* writereg 5c ffffffff */
  Mali_WrReg(0x0,0x0,0x60,0xffffffff);               /* writereg 60 ffffffff */
  Mali_WrReg(0x0,0x0,0x64,0xffffffff);               /* writereg 64 ffffffff */
  Mali_WrReg(0x0,0x0,0x68,0xffffffff);               /* writereg 68 ffffffff */
  Mali_WrReg(0x0,0x0,0x6c,0xf5f8fffb);               /* writereg 6c f5f8fffb */
  Mali_WrReg(0x0,0x0,0x70,0xffffffff);               /* writereg 70 ffffffff */
  Mali_WrReg(0x0,0x0,0x74,0xffffffff);               /* writereg 74 ffffffff */
  Mali_WrReg(0x0,0x0,0x78,0xffffffff);               /* writereg 78 ffffffff */
  Mali_WrReg(0x0,0x0,0x7c,0xffffffff);               /* writereg 7c ffffffff */
  Mali_WrReg(0x0,0x0,0x80,0xffffffff);               /* writereg 80 ffffffff */
  Mali_WrReg(0x0,0x0,0x84,0xffffffff);               /* writereg 84 ffffffff */
  Mali_WrReg(0x0,0x0,0x88,0xffffffff);               /* writereg 88 ffffffff */
  Mali_WrReg(0x0,0x0,0x8c,0xffffffff);               /* writereg 8c ffffffff */
  Mali_WrReg(0x0,0x0,0x90,0xffffffff);               /* writereg 90 ffffffff */
  Mali_WrReg(0x0,0x0,0x94,0xffffffff);               /* writereg 94 ffffffff */
  Mali_WrReg(0x0,0x0,0x98,0xffffffff);               /* writereg 98 ffffffff */
  Mali_WrReg(0x0,0x0,0xac,0xffffffff);               /* writereg ac ffffffff */
  Mali_WrReg(0x0,0x0,0xa0,0xffffffff);               /* writereg a0 ffffffff */
  Mali_WrReg(0x0,0x0,0xa4,0xfff0bdbf);               /* writereg a4 fff0bdbf */
  Mali_WrReg(0x0,0x0,0xa8,0xffffffff);               /* writereg a8 ffffffff */
  Mali_WrReg(0x0,0x0,0xac,0xffffffff);               /* writereg ac ffffffff */
  /* COMMENT */                                      /* # "Start" the cores to update shadow registers */
                                                     /* writereg 20 00000013 */
  /* Enable optional performance counting */
/*  Mali_InitPerfCounters(); */
  Mali_WrReg(0x0,0x0,0x20,0x00000013);
  Mali_WaitForClk(1);                                /* wait posedge clk 1 */
  /* COMMENT */                                      /* # dump */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_neg.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_b();

  /* COMMENT */                                      /* # Do a soft reset, just to test that no bits are sticking */
  Mali_WrReg(0x0,0x0,0x20,0x00000020);               /* writereg 20 00000020 */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_reset2.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_c();

  /* COMMENT */                                      /* # Write all ones, and test */
  Mali_WrReg(0x0,0x0,0x0,0xffffffff);                /* writereg 00 ffffffff */
  Mali_WrReg(0x0,0x0,0x4,0xffffffff);                /* writereg 04 ffffffff */
  Mali_WrReg(0x0,0x0,0x8,0xffffffff);                /* writereg 08 ffffffff */
  Mali_WrReg(0x0,0x0,0xc,0xffffffff);                /* writereg 0c ffffffff */
  Mali_WrReg(0x0,0x0,0x10,0xffffffff);               /* writereg 10 ffffffff */
  Mali_WrReg(0x0,0x0,0x14,0xffffffff);               /* writereg 14 ffffffff */
  Mali_WrReg(0x0,0x0,0x18,0xffffffff);               /* writereg 18 ffffffff */
  Mali_WrReg(0x0,0x0,0x1c,0xffffffff);               /* writereg 1c ffffffff */
  /* COMMENT */                                      /* # Skip command register at 0x20 */
  Mali_WrReg(0x0,0x0,0x24,0xffffffff);               /* writereg 24 ffffffff */
  /* COMMENT */                                      /* # Skip interrupt clear at 0x28 */
  Mali_WrReg(0x0,0x0,0x2c,0xffffffff);               /* writereg 2c ffffffff */
  Mali_WrReg(0x0,0x0,0x30,0xffffffff);               /* writereg 30 ffffffff */
  Mali_WrReg(0x0,0x0,0x34,0xffffffff);               /* writereg 34 ffffffff */
  Mali_WrReg(0x0,0x0,0x38,0xffffffff);               /* writereg 38 ffffffff */
  Mali_WrReg(0x0,0x0,0x3c,0xffffffff);               /* writereg 3c ffffffff */
  Mali_WrReg(0x0,0x0,0x40,0xffffffff);               /* writereg 40 ffffffff */
  Mali_WrReg(0x0,0x0,0x44,0xffffffff);               /* writereg 44 ffffffff */
  Mali_WrReg(0x0,0x0,0x48,0xffffffff);               /* writereg 48 ffffffff */
  Mali_WrReg(0x0,0x0,0x4c,0xffffffff);               /* writereg 4c ffffffff */
  Mali_WrReg(0x0,0x0,0x50,0xffffffff);               /* writereg 50 ffffffff */
  Mali_WrReg(0x0,0x0,0x54,0xffffffff);               /* writereg 54 ffffffff */
  Mali_WrReg(0x0,0x0,0x58,0xffffffff);               /* writereg 58 ffffffff */
  Mali_WrReg(0x0,0x0,0x5c,0xffffffff);               /* writereg 5c ffffffff */
  Mali_WrReg(0x0,0x0,0x60,0xffffffff);               /* writereg 60 ffffffff */
  Mali_WrReg(0x0,0x0,0x64,0xffffffff);               /* writereg 64 ffffffff */
  Mali_WrReg(0x0,0x0,0x68,0xffffffff);               /* writereg 68 ffffffff */
  Mali_WrReg(0x0,0x0,0x6c,0xffffffff);               /* writereg 6c ffffffff */
  Mali_WrReg(0x0,0x0,0x70,0xffffffff);               /* writereg 70 ffffffff */
  Mali_WrReg(0x0,0x0,0x74,0xffffffff);               /* writereg 74 ffffffff */
  Mali_WrReg(0x0,0x0,0x78,0xffffffff);               /* writereg 78 ffffffff */
  Mali_WrReg(0x0,0x0,0x7c,0xffffffff);               /* writereg 7c ffffffff */
  Mali_WrReg(0x0,0x0,0x80,0xffffffff);               /* writereg 80 ffffffff */
  Mali_WrReg(0x0,0x0,0x84,0xffffffff);               /* writereg 84 ffffffff */
  Mali_WrReg(0x0,0x0,0x88,0xffffffff);               /* writereg 88 ffffffff */
  Mali_WrReg(0x0,0x0,0x8c,0xffffffff);               /* writereg 8c ffffffff */
  Mali_WrReg(0x0,0x0,0x90,0xffffffff);               /* writereg 90 ffffffff */
  Mali_WrReg(0x0,0x0,0x94,0xffffffff);               /* writereg 94 ffffffff */
  Mali_WrReg(0x0,0x0,0x98,0xffffffff);               /* writereg 98 ffffffff */
  Mali_WrReg(0x0,0x0,0xac,0xffffffff);               /* writereg ac ffffffff */
  Mali_WrReg(0x0,0x0,0xa0,0xffffffff);               /* writereg a0 ffffffff */
  Mali_WrReg(0x0,0x0,0xa4,0xffffffff);               /* writereg a4 ffffffff */
  Mali_WrReg(0x0,0x0,0xa8,0xffffffff);               /* writereg a8 ffffffff */
  Mali_WrReg(0x0,0x0,0xac,0xffffffff);               /* writereg ac ffffffff */
  /* COMMENT */                                      /* # "Start" the cores to update shadow registers */
  Mali_WrReg(0x0,0x0,0x20,0x00000013);               /* writereg 20 00000013 */
  Mali_WaitForClk(1);                                /* wait posedge clk 1 */
  /* COMMENT */                                      /* # dump */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_ones.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_d();

  /* COMMENT */                                      /* # Write all zeros, and test */
  Mali_WrReg(0x0,0x0,0x0,0x00000000);                /* writereg 00 00000000 */
  Mali_WrReg(0x0,0x0,0x4,0x00000000);                /* writereg 04 00000000 */
  Mali_WrReg(0x0,0x0,0x8,0x00000000);                /* writereg 08 00000000 */
  Mali_WrReg(0x0,0x0,0xc,0x00000000);                /* writereg 0c 00000000 */
  Mali_WrReg(0x0,0x0,0x10,0x00000000);               /* writereg 10 00000000 */
  Mali_WrReg(0x0,0x0,0x14,0x00000000);               /* writereg 14 00000000 */
  Mali_WrReg(0x0,0x0,0x18,0x00000000);               /* writereg 18 00000000 */
  Mali_WrReg(0x0,0x0,0x1c,0x00000000);               /* writereg 1c 00000000 */
  /* COMMENT */                                      /* # Skip command register at 0x20 */
  Mali_WrReg(0x0,0x0,0x24,0x00000000);               /* writereg 24 00000000 */
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 28 ffffffff # Clear interrupts */
  Mali_WrReg(0x0,0x0,0x2c,0x00000000);               /* writereg 2c 00000000 */
  Mali_WrReg(0x0,0x0,0x30,0x00000000);               /* writereg 30 00000000 */
  Mali_WrReg(0x0,0x0,0x34,0x00000000);               /* writereg 34 00000000 */
  Mali_WrReg(0x0,0x0,0x38,0x00000000);               /* writereg 38 00000000 */
  Mali_WrReg(0x0,0x0,0x3c,0x00000000);               /* writereg 3c 00000000 */
  Mali_WrReg(0x0,0x0,0x40,0x00000000);               /* writereg 40 00000000 */
  Mali_WrReg(0x0,0x0,0x44,0x00000000);               /* writereg 44 00000000 */
  Mali_WrReg(0x0,0x0,0x48,0x00000000);               /* writereg 48 00000000 */
  Mali_WrReg(0x0,0x0,0x4c,0x00000000);               /* writereg 4c 00000000 */
  Mali_WrReg(0x0,0x0,0x50,0x00000000);               /* writereg 50 00000000 */
  Mali_WrReg(0x0,0x0,0x54,0x00000000);               /* writereg 54 00000000 */
  Mali_WrReg(0x0,0x0,0x58,0x00000000);               /* writereg 58 00000000 */
  Mali_WrReg(0x0,0x0,0x5c,0x00000000);               /* writereg 5c 00000000 */
  Mali_WrReg(0x0,0x0,0x60,0x00000000);               /* writereg 60 00000000 */
  Mali_WrReg(0x0,0x0,0x64,0x00000000);               /* writereg 64 00000000 */
  Mali_WrReg(0x0,0x0,0x68,0x00000000);               /* writereg 68 00000000 */
  Mali_WrReg(0x0,0x0,0x6c,0x00000000);               /* writereg 6c 00000000 */
  Mali_WrReg(0x0,0x0,0x70,0x00000000);               /* writereg 70 00000000 */
  Mali_WrReg(0x0,0x0,0x74,0x00000000);               /* writereg 74 00000000 */
  Mali_WrReg(0x0,0x0,0x78,0x00000000);               /* writereg 78 00000000 */
  Mali_WrReg(0x0,0x0,0x7c,0x00000000);               /* writereg 7c 00000000 */
  Mali_WrReg(0x0,0x0,0x80,0x00000000);               /* writereg 80 00000000 */
  Mali_WrReg(0x0,0x0,0x84,0x00000000);               /* writereg 84 00000000 */
  Mali_WrReg(0x0,0x0,0x88,0x00000000);               /* writereg 88 00000000 */
  Mali_WrReg(0x0,0x0,0x8c,0x00000000);               /* writereg 8c 00000000 */
  Mali_WrReg(0x0,0x0,0x90,0x00000000);               /* writereg 90 00000000 */
  Mali_WrReg(0x0,0x0,0x94,0x00000000);               /* writereg 94 00000000 */
  Mali_WrReg(0x0,0x0,0x98,0x00000000);               /* writereg 98 00000000 */
  Mali_WrReg(0x0,0x0,0x9c,0x00000000);               /* writereg 9c 00000000 */
  Mali_WrReg(0x0,0x0,0xa0,0x00000000);               /* writereg a0 00000000 */
  Mali_WrReg(0x0,0x0,0xa4,0x00000000);               /* writereg a4 00000000 */
  Mali_WrReg(0x0,0x0,0xa8,0x00000000);               /* writereg a8 00000000 */
  Mali_WrReg(0x0,0x0,0xac,0x00000000);               /* writereg ac 00000000 */
  /* COMMENT */                                      /* # "Start" the cores to update shadow registers */
  Mali_WrReg(0x0,0x0,0x20,0x00000013);               /* writereg 20 00000013 */
  Mali_WaitForClk(1);                                /* wait posedge clk 1 */
  /* COMMENT */                                      /* # dump (We'll get a hang and some completed interrupts) */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_zeros.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_e();

  /* COMMENT */                                      /* # Write "random" value to check address decoding */
  Mali_WrReg(0x0,0x0,0x0,0x147fac6b);                /* writereg 00 147fac6b */
  Mali_WrReg(0x0,0x0,0x4,0x147fac6b);                /* writereg 04 147fac6b # Keep start and stop equal to avoid running random code */
  Mali_WrReg(0x0,0x0,0x8,0x358b45ce);                /* writereg 08 358b45ce */
  Mali_WrReg(0x0,0x0,0xc,0x358b45ce);                /* writereg 0c 358b45ce # Keep start and stop equal to avoid running random code */
  Mali_WrReg(0x0,0x0,0x10,0x54bc97d8);               /* writereg 10 54bc97d8 */
  Mali_WrReg(0x0,0x0,0x14,0x95fb2c24);               /* writereg 14 95fb2c24 */
  Mali_WrReg(0x0,0x0,0x18,0x554fbc78);               /* writereg 18 554fbc78 */
  Mali_WrReg(0x0,0x0,0x1c,0x82bfce52);               /* writereg 1c 82bfce52 */
  /* COMMENT */                                      /* # Skip command register at 0x20 */
  Mali_WrReg(0x0,0x0,0x24,0x45678677);               /* writereg 24 45678677 */
  /* COMMENT */                                      /* # Skip interrupt clear at 0x28 */
  Mali_WrReg(0x0,0x0,0x2c,0x66654468);               /* writereg 2c 66654468 */
  Mali_WrReg(0x0,0x0,0x30,0x98432187);               /* writereg 30 98432187 */
  Mali_WrReg(0x0,0x0,0x34,0x754bcabf);               /* writereg 34 754bcabf */
  Mali_WrReg(0x0,0x0,0x38,0x3578baed);               /* writereg 38 3578baed */
  Mali_WrReg(0x0,0x0,0x3c,0xaaddeeff);               /* writereg 3c aaddeeff */
  Mali_WrReg(0x0,0x0,0x40,0xaeefb822);               /* writereg 40 aeefb822 */
  Mali_WrReg(0x0,0x0,0x44,0x11565338);               /* writereg 44 11565338 */
  Mali_WrReg(0x0,0x0,0x48,0x35496837);               /* writereg 48 35496837 */
  Mali_WrReg(0x0,0x0,0x4c,0x85482a31);               /* writereg 4c 85482a31 */
  Mali_WrReg(0x0,0x0,0x50,0x486678ba);               /* writereg 50 486678ba */
  Mali_WrReg(0x0,0x0,0x54,0x852edf85);               /* writereg 54 852edf85 */
  Mali_WrReg(0x0,0x0,0x58,0x75abc451);               /* writereg 58 75abc451 */
  Mali_WrReg(0x0,0x0,0x5c,0x66543335);               /* writereg 5c 66543335 */
  Mali_WrReg(0x0,0x0,0x60,0x150b6530);               /* writereg 60 150b6530 */
  Mali_WrReg(0x0,0x0,0x64,0x98763548);               /* writereg 64 98763548 */
  Mali_WrReg(0x0,0x0,0x68,0x3987639b);               /* writereg 68 3987639b */
  Mali_WrReg(0x0,0x0,0x6c,0x11123454);               /* writereg 6c 11123454 */
  Mali_WrReg(0x0,0x0,0x70,0x56367678);               /* writereg 70 56367678 */
  Mali_WrReg(0x0,0x0,0x74,0x45872457);               /* writereg 74 45872457 */
  Mali_WrReg(0x0,0x0,0x78,0x78778223);               /* writereg 78 78778223 */
  Mali_WrReg(0x0,0x0,0x7c,0x45657376);               /* writereg 7c 45657376 */
  Mali_WrReg(0x0,0x0,0x80,0x34676645);               /* writereg 80 34676645 */
  Mali_WrReg(0x0,0x0,0x84,0x23566846);               /* writereg 84 23566846 */
  Mali_WrReg(0x0,0x0,0x88,0x34666565);               /* writereg 88 34666565 */
  Mali_WrReg(0x0,0x0,0x8c,0x24556454);               /* writereg 8c 24556454 */
  Mali_WrReg(0x0,0x0,0x90,0x24566544);               /* writereg 90 24566544 */
  Mali_WrReg(0x0,0x0,0x94,0x23466554);               /* writereg 94 23466554 */
  Mali_WrReg(0x0,0x0,0x98,0x23672456);               /* writereg 98 23672456 */
  Mali_WrReg(0x0,0x0,0x9c,0x23465554);               /* writereg 9c 23465554 */
  Mali_WrReg(0x0,0x0,0xa0,0x23645543);               /* writereg a0 23645543 */
  Mali_WrReg(0x0,0x0,0xa4,0x23465573);               /* writereg a4 23465573 */
  Mali_WrReg(0x0,0x0,0xa8,0x23645342);               /* writereg a8 23645342 */
  Mali_WrReg(0x0,0x0,0xac,0x23452345);               /* writereg ac 23452345 */
  /* COMMENT */                                      /* # "Start" the cores to update shadow registers */
  Mali_WrReg(0x0,0x0,0x20,0x00000013);               /* writereg 20 00000013 */
  Mali_WaitForClk(1);                                /* wait posedge clk 1 */
  /* COMMENT */                                      /* # dump */
  /* SEE BELOW */                                    /* dump_reg 0 000000b0 > dump_rnd.hex */
  res |= Check_m400_gp_F000_read_write_reg_check_0_f();

  Mali_MaskAllInterrupts();
  mali_step++;
  return res;
}

int Check_m400_gp_F000_read_write_reg_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_a.data_00000000,0,0,0x0,0xb0);
  return res;
};

int Check_m400_gp_F000_read_write_reg_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_b.data_00000000,0,0,0x0,0xb0);
  return res;
};

int Check_m400_gp_F000_read_write_reg_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_c.data_00000000,0,0,0x0,0xb0);
  return res;
};

int Check_m400_gp_F000_read_write_reg_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_d.data_00000000,0,0,0x0,0xb0);
  return res;
};

int Check_m400_gp_F000_read_write_reg_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_e.data_00000000,0,0,0x0,0xb0);
  return res;
};

int Check_m400_gp_F000_read_write_reg_check_0_f () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_gp_F000_read_write_reg_check_0_f.data_00000000,0,0,0x0,0xb0);
  return res;
};
