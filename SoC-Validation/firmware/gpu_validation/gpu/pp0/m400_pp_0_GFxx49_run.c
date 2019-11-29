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
* Modified  : $Date: 2013/05/14 11:58:32 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_pp_0_GFxx49_check_0_a.h"
#include "m400_pp_0_GFxx49_check_0_b.h"
#include "m400_pp_0_GFxx49_mem_0.h"

int Check_m400_pp_0_GFxx49_check_0_a ();
int Check_m400_pp_0_GFxx49_check_0_b (struct t_m400_pp_0_GFxx49_mem_0 *p);

int RunMaliTest_m400_pp_0_GFxx49_part0 (void);
int RunMaliTest_m400_pp_0_GFxx49_part1 (void);

static volatile struct t_m400_pp_0_GFxx49_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_pp_0_GFxx49 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_pp_0_GFxx49_part0();
    case 1  : return RunMaliTest_m400_pp_0_GFxx49_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_pp_0_GFxx49_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_pp_0_GFxx49\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem GFxx49_00000.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x1,0x0,0x0,0x00004060);                /* writereg 10000000 00004060 */
  Mali_WrReg(0x1,0x0,0x4,0x00000040);                /* writereg 10000004 00000040 */
  Mali_WrReg(0x1,0x0,0x8,0x00010400);                /* writereg 10000008 00010400 */
  Mali_WrReg(0x1,0x0,0xc,0x00000002);                /* writereg 1000000c 00000002 */
  Mali_WrReg(0x1,0x0,0x10,0x00ffffff);               /* writereg 10000010 00ffffff */
  Mali_WrReg(0x1,0x0,0x18,0xff000000);               /* writereg 10000018 ff000000 */
  Mali_WrReg(0x1,0x0,0x1c,0xff000000);               /* writereg 1000001c ff000000 */
  Mali_WrReg(0x1,0x0,0x20,0xff000000);               /* writereg 10000020 ff000000 */
  Mali_WrReg(0x1,0x0,0x24,0xff000000);               /* writereg 10000024 ff000000 */
  Mali_WrReg(0x1,0x0,0x28,0x0000002b);               /* writereg 10000028 0000002b */
  Mali_WrReg(0x1,0x0,0x2c,0x0000001f);               /* writereg 1000002c 0000001f */
  Mali_WrReg(0x1,0x0,0x48,0x00000075);               /* writereg 10000048 00000075 */
  Mali_WrReg(0x1,0x0,0x100,0x00000002);              /* writereg 10000100 00000002 */
  Mali_WrReg(0x1,0x0,0x104,0x00004380);              /* writereg 10000104 00004380 */
  Mali_WrReg(0x1,0x0,0x108,0x00000003);              /* writereg 10000108 00000003 */
  Mali_WrReg(0x1,0x0,0x114,0x00000016);              /* writereg 10000114 00000016 */
  Mali_WrReg(0x1,0x0,0x118,0x00000002);              /* writereg 10000118 00000002 */
  Mali_WrReg(0x1,0x0,0x11c,0x0000000f);              /* writereg 1000011c 0000000f */
  Mali_WrReg(0x1,0x0,0x120,0x00001800);              /* writereg 10000120 00001800 */
  Mali_WrReg(0x1,0x0,0x200,0x00000001);              /* writereg 10000200 00000001 */
  Mali_WrReg(0x1,0x0,0x204,0x0000a3c0);              /* writereg 10000204 0000a3c0 */
  Mali_WrReg(0x1,0x0,0x208,0x0000000f);              /* writereg 10000208 0000000f */
  Mali_WrReg(0x1,0x0,0x214,0x00000016);              /* writereg 10000214 00000016 */
  Mali_WrReg(0x1,0x0,0x218,0x00000002);              /* writereg 10000218 00000002 */
  Mali_WrReg(0x1,0x0,0x21c,0x0000000f);              /* writereg 1000021c 0000000f */
  Mali_WrReg(0x1,0x0,0x220,0x00001800);              /* writereg 10000220 00001800 */
  /* Detected Frame Start. Initialising memory*/     /* writereg 1000100c 00000040 */
  malidata_0 = (struct t_m400_pp_0_GFxx49_mem_0 *)Mali_LdMem(&m400_pp_0_GFxx49_mem_0,sizeof(struct t_m400_pp_0_GFxx49_mem_0),(int)m400_pp_0_GFxx49_mem_0.ttb_first);
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

int RunMaliTest_m400_pp_0_GFxx49_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(1,0,0x102c);                       /* dump_reg 1000102c 1000103c */
  Mali_DisplayReg(1,0,0x1030);
  Mali_DisplayReg(1,0,0x1034);
  Mali_DisplayReg(1,0,0x1038);
  
  res |= Check_m400_pp_0_GFxx49_check_0_a();         /* dump_reg 1000102c 1000103c > GFxx49_00000_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 00004380 00010380 > GFxx49_00000_fb.hex */
  res |= Check_m400_pp_0_GFxx49_check_0_b(malidata_0);

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_pp_0_GFxx49_check_0_a () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_pp_0_GFxx49_check_0_a.data_10001020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_pp_0_GFxx49_check_0_b (struct t_m400_pp_0_GFxx49_mem_0 *p) {
  struct t_CheckData_m400_pp_0_GFxx49_check_0_b *r = &CheckData_m400_pp_0_GFxx49_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 49152 bytes\n");

  res |= Mali_MemCmp(p->data_00004000, 0x00004380, r->data_00004380, 0, 0x320);
  res |= Mali_MemCmp(p->data_00005000, 0x00005000, r->data_00005000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00006000, 0x00006000, r->data_00006000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00007000, 0x00007000, r->data_00007000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00008000, 0x00008000, r->data_00008000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00009000, 0x00009000, r->data_00009000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000a000, 0x0000a000, r->data_0000a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000b000, 0x0000b000, r->data_0000b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000c000, 0x0000c000, r->data_0000c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000d000, 0x0000d000, r->data_0000d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000e000, 0x0000e000, r->data_0000e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0000f000, 0x0000f000, r->data_0000f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x0e0);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};