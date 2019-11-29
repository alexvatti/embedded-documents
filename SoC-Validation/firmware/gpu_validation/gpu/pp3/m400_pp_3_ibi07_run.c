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
#include "m400_pp_3_ibi07_check_0_a.h"
#include "m400_pp_3_ibi07_check_0_b.h"
#include "m400_pp_3_ibi07_check_0_c.h"
#include "m400_pp_3_ibi07_check_0_d.h"
#include "m400_pp_3_ibi07_check_0_e.h"
#include "m400_pp_3_ibi07_check_0_f.h"
#include "m400_pp_3_ibi07_check_0_g.h"

int Check_m400_pp_3_ibi07_check_0_a ();
int Check_m400_pp_3_ibi07_check_0_b ();
int Check_m400_pp_3_ibi07_check_0_c ();
int Check_m400_pp_3_ibi07_check_0_d ();
int Check_m400_pp_3_ibi07_check_0_e ();
int Check_m400_pp_3_ibi07_check_0_f ();
int Check_m400_pp_3_ibi07_check_0_g ();

int RunMaliTest_m400_pp_3_ibi07_part0 (void);
int RunMaliTest_m400_pp_3_ibi07_part1 (void);

static int mali_step;

int RunMaliTest_m400_pp_3_ibi07 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_3_ibi07_part0();
    case 1  : return RunMaliTest_m400_pp_3_ibi07_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_3_ibi07_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_3_ibi07\n");

  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x1,0x0,0x1028,0x00000000);             /* writereg 10001028 00000000 #Clear mask */
  Mali_WrReg(0x1,0x0,0x1020,0x00000040);             /* writereg 10001020 00000040 #Force interrupt source */
  res |= Check_m400_pp_3_ibi07_check_0_a();          /* dump_reg 10001020 10001030 > ibi07_00000_reg0.hex #Check that it's set in rawstat */
  res |= Check_m400_pp_3_ibi07_check_0_b();          /* dump_reg 1000102C 1000103c > ibi07_00000_irq0.hex #Check that it's not set in status */
  Mali_WrReg(0x1,0x0,0x1028,0x00000040);             /* writereg 10001028 00000040 #Enable bit in mask */
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_pp_3_ibi07_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  res |= Check_m400_pp_3_ibi07_check_0_c();          /* dump_reg 10001008 10001018 > ibi07_00000_reg1.hex #Check irq line asserted in std_status */
  res |= Check_m400_pp_3_ibi07_check_0_d();          /* dump_reg 1000102C 1000103c > ibi07_00000_irq1.hex #Check status */
  Mali_WrReg(0x1,0x0,0x1024,0x00000040);             /* writereg 10001024 00000040 #Clear interrupt */
  res |= Check_m400_pp_3_ibi07_check_0_e();          /* dump_reg 10001020 10001030 > ibi07_00000_reg2.hex #Check that it's cleared in rawstat */
  res |= Check_m400_pp_3_ibi07_check_0_f();          /* dump_reg 1000102C 1000103c > ibi07_00000_irq2.hex #Check that it's cleared in status */
  res |= Check_m400_pp_3_ibi07_check_0_g();          /* dump_reg 10001008 10001018 > ibi07_00000_reg3.hex #Check irq line not asserted */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_3_ibi07_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_a.data_10031020,1,0,0x1020,0x1030);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_b.data_10031020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_c.data_10001000,1,0,0x1008,0x1018);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_d () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_d.data_10031020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_e.data_10031020,1,0,0x1020,0x1030);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_f () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_f.data_10031020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_3_ibi07_check_0_g () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_ibi07_check_0_g.data_10001000,1,0,0x1008,0x1018);
  return res;
};
