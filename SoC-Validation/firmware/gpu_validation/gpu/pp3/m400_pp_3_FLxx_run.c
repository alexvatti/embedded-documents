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
* Modified  : $Date: 2017/09/20 12:45:20 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_3_FLxx_check_0_a.h"
#include "m400_pp_3_FLxx_check_0_b.h"
#include "m400_pp_3_FLxx_mem_0.h"

int Check_m400_pp_3_FLxx_check_0_a ();
int Check_m400_pp_3_FLxx_check_0_b (struct t_m400_pp_3_FLxx_mem_0 *p);

int RunMaliTest_m400_pp_3_FLxx_part0 (void);
int RunMaliTest_m400_pp_3_FLxx_part1 (void);

static volatile struct t_m400_pp_3_FLxx_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_pp_3_FLxx (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_3_FLxx_part0();
    case 1  : return RunMaliTest_m400_pp_3_FLxx_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_3_FLxx_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_3_FLxx\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem FLxx_00000.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x1,0x0,0x0,0x0000c060);                /* writereg 10000000 0000c060 */
  Mali_WrReg(0x1,0x0,0x4,0x00000040);                /* writereg 10000004 00000040 */
  Mali_WrReg(0x1,0x0,0x8,0x0000d080);                /* writereg 10000008 0000d080 */
  Mali_WrReg(0x1,0x0,0xc,0x00000001);                /* writereg 1000000c 00000001 */
  Mali_WrReg(0x1,0x0,0x100,0x00000002);              /* writereg 10000100 00000002 */
  Mali_WrReg(0x1,0x0,0x104,0x00004050);              /* writereg 10000104 00004050 */
  Mali_WrReg(0x1,0x0,0x108,0x00000006);              /* writereg 10000108 00000006 */
  Mali_WrReg(0x1,0x0,0x114,0x00000040);              /* writereg 10000114 00000040 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 1000100c 00000040 */
  malidata_0 = (struct t_m400_pp_3_FLxx_mem_0 *)Mali_LdMem(&m400_pp_3_FLxx_mem_0,sizeof(struct t_m400_pp_3_FLxx_mem_0),(int)m400_pp_3_FLxx_mem_0.ttb_first);
  Mali_SetupOutputFrame();
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x1,0x0,0x100c,0x00000040);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_pp_3_FLxx_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(1,0,0x102c);                       /* dump_reg 1000102C 1000103c */
  Mali_DisplayReg(1,0,0x1030);
  Mali_DisplayReg(1,0,0x1034);
  Mali_DisplayReg(1,0,0x1038);
  
  res |= Check_m400_pp_3_FLxx_check_0_a();           /* dump_reg 1000102C 1000103c > FLxx_00000_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 4050 0000c050 > FLxx_00000_fb.hex */
  res |= Check_m400_pp_3_FLxx_check_0_b(malidata_0);

  Mali_WrReg(0x1,0x0,0x1024,0x00000001);             /* writereg 10001024 00000001 #clear irq */
  Mali_WrReg(0x1,0x0,0x100c,0x00000008);             /* writereg 1000100C 00000008 # flushcache */
  Mali_WaitForClk(50);                               /* wait posedge clk 50 */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_3_FLxx_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_FLxx_check_0_a.data_10031020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_3_FLxx_check_0_b (struct t_m400_pp_3_FLxx_mem_0 *p) {
  struct t_CheckData_m400_pp_3_FLxx_check_0_b *r = &CheckData_m400_pp_3_FLxx_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 32768 bytes\n");

  res |= Mali_MemCmp(p->data_00004000, 0x00004050, r->data_00004050, 0, 0x3ec);
  res |= Mali_MemCmp(p->data_00005000, 0x00005000, r->data_00005000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00006000, 0x00006000, r->data_00006000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00007000, 0x00007000, r->data_00007000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00008000, 0x00008000, r->data_00008000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00009000, 0x00009000, r->data_00009000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000a000, 0x0000a000, r->data_0000a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000b000, 0x0000b000, r->data_0000b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000c000, 0x0000c000, r->data_0000c000, 0, 0x014);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
