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
* Modified  : $Date: 2013/05/14 11:58:43 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_1_IIBF05_check_0_a.h"
#include "m400_pp_1_IIBF05_check_0_b.h"
#include "m400_pp_1_IIBF05_check_1_c.h"
#include "m400_pp_1_IIBF05_check_1_d.h"
#include "m400_pp_1_IIBF05_mem_0.h"
#include "m400_pp_1_IIBF05_mem_1.h"

int Check_m400_pp_1_IIBF05_check_0_a ();
int Check_m400_pp_1_IIBF05_check_0_b (struct t_m400_pp_1_IIBF05_mem_0 *p);
int Check_m400_pp_1_IIBF05_check_1_c ();
int Check_m400_pp_1_IIBF05_check_1_d (struct t_m400_pp_1_IIBF05_mem_1 *p);

int RunMaliTest_m400_pp_1_IIBF05_part0 (void);
int RunMaliTest_m400_pp_1_IIBF05_part1 (void);
int RunMaliTest_m400_pp_1_IIBF05_part2 (void);

static volatile struct t_m400_pp_1_IIBF05_mem_0 *malidata_0;
static volatile struct t_m400_pp_1_IIBF05_mem_1 *malidata_1;
static int mali_step;

int RunMaliTest_m400_pp_1_IIBF05 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_1_IIBF05_part0();
    case 1  : return RunMaliTest_m400_pp_1_IIBF05_part1();
    case 2  : return RunMaliTest_m400_pp_1_IIBF05_part2();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_1_IIBF05_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_1_IIBF05\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem IIBF05_00000.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x1,0x1,0x0,0x00005080);                /* writereg 10010000 00005080 */
  Mali_WrReg(0x1,0x1,0x4,0x00000040);                /* writereg 10010004 00000040 */
  Mali_WrReg(0x1,0x1,0x8,0x000058c0);                /* writereg 10010008 000058c0 */
  Mali_WrReg(0x1,0x1,0x48,0x00000075);               /* writereg 10010048 00000075 */
  Mali_WrReg(0x1,0x1,0x100,0x00000002);              /* writereg 10010100 00000002 */
  Mali_WrReg(0x1,0x1,0x104,0x00004060);              /* writereg 10010104 00004060 */
  Mali_WrReg(0x1,0x1,0x108,0x00000003);              /* writereg 10010108 00000003 */
  Mali_WrReg(0x1,0x1,0x114,0x00000010);              /* writereg 10010114 00000010 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 1001100c 00000040 */
  malidata_0 = (struct t_m400_pp_1_IIBF05_mem_0 *)Mali_LdMem(&m400_pp_1_IIBF05_mem_0,sizeof(struct t_m400_pp_1_IIBF05_mem_0),(int)m400_pp_1_IIBF05_mem_0.ttb_first);
  Mali_SetupOutputFrame();
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x1,0x1,0x100c,0x00000040);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_pp_1_IIBF05_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(1,1,0x102c);                       /* dump_reg 1001102c 1001103c */
  Mali_DisplayReg(1,1,0x1030);
  Mali_DisplayReg(1,1,0x1034);
  Mali_DisplayReg(1,1,0x1038);
  
  res |= Check_m400_pp_1_IIBF05_check_0_a();         /* dump_reg 1001102c 1001103c > IIBF05_00000_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 00004060 00005060 > IIBF05_00000_fb.hex */
  res |= Check_m400_pp_1_IIBF05_check_0_b(malidata_0);

  Mali_WrReg(0x1,0x1,0x1024,0x00000001);             /* writereg 10011024 00000001 # clear irq */
  Mali_WaitForClk(50);                               /* wait posedge clk 50 */
  /* SEE BELOW */                                    /* load_mem IIBF05_00001.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x1,0x1,0x0,0x00005080);                /* writereg 10010000 00005080 */
  Mali_WrReg(0x1,0x1,0x4,0x00000040);                /* writereg 10010004 00000040 */
  Mali_WrReg(0x1,0x1,0x8,0x000058c0);                /* writereg 10010008 000058c0 */
  Mali_WrReg(0x1,0x1,0x48,0x00000075);               /* writereg 10010048 00000075 */
  Mali_WrReg(0x1,0x1,0x100,0x00000002);              /* writereg 10010100 00000002 */
  Mali_WrReg(0x1,0x1,0x104,0x00004060);              /* writereg 10010104 00004060 */
  Mali_WrReg(0x1,0x1,0x108,0x00000003);              /* writereg 10010108 00000003 */
  Mali_WrReg(0x1,0x1,0x114,0x00000010);              /* writereg 10010114 00000010 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 1001100c 00000040 */
malidata_1 = (struct t_m400_pp_1_IIBF05_mem_1 *) Mali_LdMem(&m400_pp_1_IIBF05_mem_1,sizeof(struct t_m400_pp_1_IIBF05_mem_1),(int)m400_pp_1_IIBF05_mem_1.ttb_first);
  Mali_SetupOutputFrame();
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x1,0x1,0x100c,0x00000040);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_pp_1_IIBF05_part2 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(1,1,0x102c);                       /* dump_reg 1001102c 1001103c */
  Mali_DisplayReg(1,1,0x1030);
  Mali_DisplayReg(1,1,0x1034);
  Mali_DisplayReg(1,1,0x1038);
  
  res |= Check_m400_pp_1_IIBF05_check_1_c();         /* dump_reg 1001102c 1001103c > IIBF05_00001_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 00004060 00005060 > IIBF05_00001_fb.hex */
  res |= Check_m400_pp_1_IIBF05_check_1_d(malidata_1);

  Mali_WrReg(0x1,0x1,0x1024,0x00000001);             /* writereg 10011024 00000001 # clear irq */
  Mali_WaitForClk(50);                               /* wait posedge clk 50 */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_1_IIBF05_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_IIBF05_check_0_a.data_10011020,1,1,0x102c,0x103c);
  return res;
};

int Check_m400_pp_1_IIBF05_check_0_b (struct t_m400_pp_1_IIBF05_mem_0 *p) {
  struct t_CheckData_m400_pp_1_IIBF05_check_0_b *r = &CheckData_m400_pp_1_IIBF05_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 4096 bytes\n");

  res |= Mali_MemCmp(p->data_00004000, 0x00004060, r->data_00004060, 0, 0x3e8);
  res |= Mali_MemCmp(p->data_00005000, 0x00005000, r->data_00005000, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_pp_1_IIBF05_check_1_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_1_IIBF05_check_1_c.data_10011020,1,1,0x102c,0x103c);
  return res;
};

int Check_m400_pp_1_IIBF05_check_1_d (struct t_m400_pp_1_IIBF05_mem_1 *p) {
  struct t_CheckData_m400_pp_1_IIBF05_check_1_d *r = &CheckData_m400_pp_1_IIBF05_check_1_d;
  int res = 0;
  Mali_Message("Performing data check of 4096 bytes\n");

  res |= Mali_MemCmp(p->data_00004000, 0x00004060, r->data_00004060, 0, 0x3e8);
  res |= Mali_MemCmp(p->data_00005000, 0x00005000, r->data_00005000, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
