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
#include "m400_gp_F200_plbu_config_reg_reset_check_0_a.h"
#include "m400_gp_F200_plbu_config_reg_reset_check_0_b.h"
#include "m400_gp_F200_plbu_config_reg_reset_check_0_c.h"
#include "m400_gp_F200_plbu_config_reg_reset_check_0_d.h"
#include "m400_gp_F200_plbu_config_reg_reset_mem_0.h"

int Check_m400_gp_F200_plbu_config_reg_reset_check_0_a (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p);
int Check_m400_gp_F200_plbu_config_reg_reset_check_0_b (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p);
int Check_m400_gp_F200_plbu_config_reg_reset_check_0_c (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p);
int Check_m400_gp_F200_plbu_config_reg_reset_check_0_d (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p);

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part0 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part1 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part2 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part3 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part4 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part5 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part6 (void);
int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part7 (void);

static volatile struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part0();
    case 1  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part1();
    case 2  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part2();
    case 3  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part3();
    case 4  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part4();
    case 5  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part5();
    case 6  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part6();
    case 7  : return RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part7();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F200_plbu_config_reg_reset\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem input.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* COMMENT */                                      /* # Dump the value of the registers after pin reset */
  Mali_WrReg(0x0,0x0,0x8,0x00000080);                /* writereg 00000008 00000080 */
  Mali_WrReg(0x0,0x0,0xc,0x00000100);                /* writereg 0000000c 00000100 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000002 */
  malidata_0 = (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *)Mali_LdMem(&m400_gp_F200_plbu_config_reg_reset_mem_0,sizeof(struct t_m400_gp_F200_plbu_config_reg_reset_mem_0),(int)m400_gp_F200_plbu_config_reg_reset_mem_0.ttb_first);
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* SEE BELOW */                                    /* dump_mem 00010000 00010040 > conf_reg00.hex */
  res |= Check_m400_gp_F200_plbu_config_reg_reset_check_0_a(malidata_0);

  /* COMMENT */                                      /* # Write to all registers */
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 00000028 ffffffff */
  Mali_WrReg(0x0,0x0,0x8,0x00000000);                /* writereg 00000008 00000000 */
  Mali_WrReg(0x0,0x0,0xc,0x00000100);                /* writereg 0000000c 00000100 */
                                                     /* writereg 00000020 00000002 */
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part2 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* SEE BELOW */                                    /* dump_mem 00010000 00010040 > conf_reg01.hex */
  res |= Check_m400_gp_F200_plbu_config_reg_reset_check_0_b(malidata_0);

  /* COMMENT */                                      /* # Run soft reset */
  Mali_WrReg(0x0,0x0,0x2c,0x00080000);               /* writereg 0000002c 00080000 */
  Mali_WrReg(0x0,0x0,0x20,0x00000400);               /* writereg 00000020 00000400 */
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part3 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 00000028 ffffffff */
  Mali_WrReg(0x0,0x0,0x2c,0x00707bff);               /* writereg 0000002c 00707bff */
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* COMMENT */                                      /* # Dump the value of the registers */
  Mali_WrReg(0x0,0x0,0x8,0x00000080);                /* writereg 00000008 00000080 */
  Mali_WrReg(0x0,0x0,0xc,0x00000100);                /* writereg 0000000c 00000100 */
                                                     /* writereg 00000020 00000002 */
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part4 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* SEE BELOW */                                    /* dump_mem 00010000 00010040 > conf_reg02.hex */
  res |= Check_m400_gp_F200_plbu_config_reg_reset_check_0_c(malidata_0);

  /* COMMENT */                                      /* # Write to all registers */
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 00000028 ffffffff */
  Mali_WrReg(0x0,0x0,0x8,0x00000000);                /* writereg 00000008 00000000 */
  Mali_WrReg(0x0,0x0,0xc,0x00000080);                /* writereg 0000000c 00000080 */
                                                     /* writereg 00000020 00000002 */
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part5 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* COMMENT */                                      /* # Revert to defaults */
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 00000028 ffffffff */
  Mali_WrReg(0x0,0x0,0x8,0x00000100);                /* writereg 00000008 00000100 */
  Mali_WrReg(0x0,0x0,0xc,0x00000108);                /* writereg 0000000c 00000108 */
                                                     /* writereg 00000020 00000002 */
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part6 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_WrReg(0x0,0x0,0x28,0xffffffff);               /* writereg 00000028 ffffffff */
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* COMMENT */                                      /* # Dump the value of the registers */
  Mali_WrReg(0x0,0x0,0x8,0x00000080);                /* writereg 00000008 00000080 */
  Mali_WrReg(0x0,0x0,0xc,0x00000100);                /* writereg 0000000c 00000100 */
                                                     /* writereg 00000020 00000002 */
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F200_plbu_config_reg_reset_part7 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000034 */
  Mali_DisplayReg(0,0,0x28);
  Mali_DisplayReg(0,0,0x2c);
  Mali_DisplayReg(0,0,0x30);
  
  /* SEE BELOW */                                    /* dump_mem 00010000 00010040 > conf_reg03.hex */
  res |= Check_m400_gp_F200_plbu_config_reg_reset_check_0_d(malidata_0);

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_gp_F200_plbu_config_reg_reset_check_0_a (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p) {
  struct t_CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_a *r = &CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 64 bytes\n");

  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x010);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F200_plbu_config_reg_reset_check_0_b (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p) {
  struct t_CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_b *r = &CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 64 bytes\n");

  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x010);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F200_plbu_config_reg_reset_check_0_c (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p) {
  struct t_CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_c *r = &CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_c;
  int res = 0;
  Mali_Message("Performing data check of 64 bytes\n");

  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x010);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F200_plbu_config_reg_reset_check_0_d (struct t_m400_gp_F200_plbu_config_reg_reset_mem_0 *p) {
  struct t_CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_d *r = &CheckData_m400_gp_F200_plbu_config_reg_reset_check_0_d;
  int res = 0;
  Mali_Message("Performing data check of 64 bytes\n");

  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x010);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
