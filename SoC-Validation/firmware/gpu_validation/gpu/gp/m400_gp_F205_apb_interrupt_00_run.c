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
#include "m400_gp_F205_apb_interrupt_00_mem_0.h"


int RunMaliTest_m400_gp_F205_apb_interrupt_00_part0 (void);
int RunMaliTest_m400_gp_F205_apb_interrupt_00_part1 (void);

static volatile struct t_m400_gp_F205_apb_interrupt_00_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_gp_F205_apb_interrupt_00 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F205_apb_interrupt_00_part0();
    case 1  : return RunMaliTest_m400_gp_F205_apb_interrupt_00_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F205_apb_interrupt_00_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F205_apb_interrupt_00\n");

  /* COMMENT */                                      /* # Mali GP dump */
  /* COMMENT */                                      /* # Test name: apb_interrupt */
  /* COMMENT */                                      /* # Dump   nr: 0 */
  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x0,0x0,0x28,0x00080000);               /* writereg 00000028 00080000  # clear reset complete bit */
  /* SEE BELOW */                                    /* load_mem apb_interrupt_00_mem.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x0,0x0,0x0,0x00000c80);                /* writereg 00000000 00000c80  # GP_CTRL_REG_VS_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0x4,0x00000cc0);                /* writereg 00000004 00000cc0  # GP_CTRL_REG_VS_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x8,0x00000e80);                /* writereg 00000008 00000e80  # GP_CTRL_REG_PLBU_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0xc,0x00000f20);                /* writereg 0000000c 00000f20  # GP_CTRL_REG_PLBU_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x28,0x00003fff);               /* writereg 00000028 00003fff  # GP_CTRL_REG_INT_CLEAR */
  Mali_WrReg(0x0,0x0,0x2c,0x00000be5);               /* writereg 0000002c 00000be5  # GP_CTRL_REG_INT_MASK */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000003 */
  malidata_0 = (struct t_m400_gp_F205_apb_interrupt_00_mem_0 *)Mali_LdMem(&m400_gp_F205_apb_interrupt_00_mem_0,sizeof(struct t_m400_gp_F205_apb_interrupt_00_mem_0),(int)m400_gp_F205_apb_interrupt_00_mem_0.ttb_first);
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000003);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F205_apb_interrupt_00_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_reg 00000000 0000011c > dump_reg.hex */
  Mali_Message("WARNING: Self-checking disabled - test was not built with reference data\n");

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}
