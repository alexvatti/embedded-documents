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
#include "m400_mp_2_F013_MMU_GP_IRQ_check_0_a.h"
#include "m400_mp_2_F013_MMU_GP_IRQ_check_0_b.h"

int Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_a ();
int Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_b ();

int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part0 (void);
int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part1 (void);
int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part2 (void);

static int mali_step;

int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part0();
    case 1  : return RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part1();
    case 2  : return RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part2();
    default : return 0;
  };
};

int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_mp_2_F013_MMU_GP_IRQ\n");

  Mali_Reset();                                      /* reset */
  /* COMMENT */                                      /* # Set up interrupt mask */
  Mali_WrReg(0x2,0x0,0x1c,0x00000001);               /* writereg 0200001c 00000001 */
  Mali_WrReg(0x2,0x0,0x14,0x00000001);               /* writereg 02000014 00000001 */
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_reg 02000014 02000024 > regdump0.hex */
  res |= Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_a();

  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x2,0x0,0x1c,0x00000002);               /* writereg 0200001c 00000002 */
  Mali_WrReg(0x2,0x0,0x14,0x00000002);               /* writereg 02000014 00000002 */
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_mp_2_F013_MMU_GP_IRQ_part2 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_reg 02000014 02000024 > regdump1.hex */
  res |= Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_b();

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_2_F013_MMU_GP_IRQ_check_0_a.data_02000010,2,0,0x14,0x24);
  return res;
};

int Check_m400_mp_2_F013_MMU_GP_IRQ_check_0_b () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_2_F013_MMU_GP_IRQ_check_0_b.data_02000010,2,0,0x14,0x24);
  return res;
};
