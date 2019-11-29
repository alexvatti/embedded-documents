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
#include "m400_pp_3_IWCx11_check_0_a.h"
#include "m400_pp_3_IWCx11_check_0_b.h"
#include "m400_pp_3_IWCx11_mem_0.h"

int Check_m400_pp_3_IWCx11_check_0_a ();
int Check_m400_pp_3_IWCx11_check_0_b (struct t_m400_pp_3_IWCx11_mem_0 *p);

int RunMaliTest_m400_pp_3_IWCx11_part0 (void);
int RunMaliTest_m400_pp_3_IWCx11_part1 (void);

static volatile struct t_m400_pp_3_IWCx11_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_pp_3_IWCx11 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_3_IWCx11_part0();
    case 1  : return RunMaliTest_m400_pp_3_IWCx11_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_3_IWCx11_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_3_IWCx11\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem IWCx11_00000.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x1,0x0,0x0,0x00004060);                /* writereg 10000000 00004060 */
  Mali_WrReg(0x1,0x0,0x4,0x00000040);                /* writereg 10000004 00000040 */
  Mali_WrReg(0x1,0x0,0x8,0x0000a140);                /* writereg 10000008 0000a140 */
  Mali_WrReg(0x1,0x0,0x100,0x00000002);              /* writereg 10000100 00000002 */
  Mali_WrReg(0x1,0x0,0x104,0x00005080);              /* writereg 10000104 00005080 */
  Mali_WrReg(0x1,0x0,0x114,0x00000010);              /* writereg 10000114 00000010 */
  Mali_WrReg(0x1,0x0,0x200,0x00000001);              /* writereg 10000200 00000001 */
  Mali_WrReg(0x1,0x0,0x204,0x000070c0);              /* writereg 10000204 000070c0 */
  Mali_WrReg(0x1,0x0,0x208,0x0000000e);              /* writereg 10000208 0000000e */
  Mali_WrReg(0x1,0x0,0x214,0x00000010);              /* writereg 10000214 00000010 */
  Mali_WrReg(0x1,0x0,0x300,0x00000001);              /* writereg 10000300 00000001 */
  Mali_WrReg(0x1,0x0,0x304,0x00009100);              /* writereg 10000304 00009100 */
  Mali_WrReg(0x1,0x0,0x308,0x0000000d);              /* writereg 10000308 0000000d */
  Mali_WrReg(0x1,0x0,0x314,0x00000008);              /* writereg 10000314 00000008 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 1000100c 00000040 */
  malidata_0 = (struct t_m400_pp_3_IWCx11_mem_0 *)Mali_LdMem(&m400_pp_3_IWCx11_mem_0,sizeof(struct t_m400_pp_3_IWCx11_mem_0),(int)m400_pp_3_IWCx11_mem_0.ttb_first);
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

int RunMaliTest_m400_pp_3_IWCx11_part1 (void) {
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
  
  res |= Check_m400_pp_3_IWCx11_check_0_a();         /* dump_reg 1000102C 1000103c > IWCx11_00000_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 5080 0000a100 > IWCx11_00000_fb.hex */
  res |= Check_m400_pp_3_IWCx11_check_0_b(malidata_0);

  Mali_WrReg(0x1,0x0,0x1024,0x00000001);             /* writereg 10001024 00000001 #clear irq */
  Mali_WrReg(0x1,0x0,0x100c,0x00000008);             /* writereg 1000100C 00000008 # flushcache */
  Mali_WaitForClk(50);                               /* wait posedge clk 50 */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_3_IWCx11_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_3_IWCx11_check_0_a.data_10031020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_3_IWCx11_check_0_b (struct t_m400_pp_3_IWCx11_mem_0 *p) {
  struct t_CheckData_m400_pp_3_IWCx11_check_0_b *r = &CheckData_m400_pp_3_IWCx11_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 20608 bytes\n");

  res |= Mali_MemCmp(p->data_00005000, 0x00005080, r->data_00005080, 0, 0x3e0);
  res |= Mali_MemCmp(p->data_00006000, 0x00006000, r->data_00006000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00007000, 0x00007000, r->data_00007000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00008000, 0x00008000, r->data_00008000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00009000, 0x00009000, r->data_00009000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000a000, 0x0000a000, r->data_0000a000, 0, 0x040);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
