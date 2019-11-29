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
* Modified  : $Date: 2013/05/14 11:58:06 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_mp_1_F023_GP_PP0_check_0_a.h"
#include "m400_mp_1_F023_GP_PP0_check_0_b.h"
#include "m400_mp_1_F023_GP_PP0_check_0_c.h"
#include "m400_mp_1_F023_GP_PP0_check_0_d.h"
#include "m400_mp_1_F023_GP_PP0_mem_0.h"

int Check_m400_mp_1_F023_GP_PP0_check_0_a (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p);
int Check_m400_mp_1_F023_GP_PP0_check_0_b (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p);
int Check_m400_mp_1_F023_GP_PP0_check_0_c ();
int Check_m400_mp_1_F023_GP_PP0_check_0_d (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p);

int RunMaliTest_m400_mp_1_F023_GP_PP0_part0 (void);
int RunMaliTest_m400_mp_1_F023_GP_PP0_part1 (void);

static volatile struct t_m400_mp_1_F023_GP_PP0_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_mp_1_F023_GP_PP0 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_mp_1_F023_GP_PP0_part0();
    case 1  : return RunMaliTest_m400_mp_1_F023_GP_PP0_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_mp_1_F023_GP_PP0_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_mp_1_F023_GP_PP0\n");

  /* COMMENT */                                      /* # Mali GP dump */
  /* COMMENT */                                      /* # Test name: plbu_scissor02_elements */
  /* COMMENT */                                      /* # Dump   nr: 12 */
  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x0,0x0,0x28,0x00080000);               /* writereg 00000028 00080000  # clear reset complete bit */
  /* SEE BELOW */                                    /* load_mem plbu_scissor02_elements_12_mem.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem GTxx01_00000.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x3,0x0,0x1c,0x00000003);               /* writereg 0100001c 00000003  # enable L2 cache */
  /* COMMENT */                                      /* # GP */
  Mali_WrReg(0x0,0x0,0x8,0x00001300);                /* writereg 00000008 00001300  # GP_CTRL_REG_PLBU_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0xc,0x00001400);                /* writereg 0000000c 00001400  # GP_CTRL_REG_PLBU_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x28,0x00003fff);               /* writereg 00000028 00003fff  # GP_CTRL_REG_INT_CLEAR */
  Mali_WrReg(0x0,0x0,0x2c,0x000007fe);               /* writereg 0000002c 000007fe  # GP_CTRL_REG_INT_MASK */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000002 */
  malidata_0 = (struct t_m400_mp_1_F023_GP_PP0_mem_0 *)Mali_LdMem(&m400_mp_1_F023_GP_PP0_mem_0,sizeof(struct t_m400_mp_1_F023_GP_PP0_mem_0),(int)m400_mp_1_F023_GP_PP0_mem_0.ttb_first);
  
  /* Enable optional performance counting */
/*  Mali_InitPerfCounters(); */
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  /* COMMENT */                                      /* # PP0 */
  Mali_WrReg(0x1,0x0,0x1028,0x00000fff);             /* writereg 10001028 00000fff # render_job */
  Mali_WrReg(0x1,0x0,0x1024,0x00000fff);             /* writereg 10001024 00000fff # render_job */
  Mali_WrReg(0x1,0x0,0x0,0x100020c0);                /* writereg 10000000 100020c0 # render_job */
  Mali_WrReg(0x1,0x0,0x4,0x10000040);                /* writereg 10000004 10000040 # render_job */
  Mali_WrReg(0x1,0x0,0x8,0x100022c0);                /* writereg 10000008 100022c0 # render_job */
  Mali_WrReg(0x1,0x0,0x100,0x00000002);              /* writereg 10000100 00000002 # render_job */
  Mali_WrReg(0x1,0x0,0x104,0x100000c0);              /* writereg 10000104 100000c0 # render_job */
  Mali_WrReg(0x1,0x0,0x114,0x00000010);              /* writereg 10000114 00000010 # render_job */
  Mali_SetupOutputFrame();                           /* writereg 1000100c 00000040 # render_job */
  Mali_WrReg(0x1,0x0,0x100c,0x00000040);
  mali_step++;                                       /* wait irq 00010001 00010001 */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_mp_1_F023_GP_PP0_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  have_interrupts = Mali_InterruptCheck(0x00010001,0x00010001);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  Mali_DisplayReg(0,0,0x24);                         /* dump_reg 00000024 00000028 */
  
  /* COMMENT */                                      /* # GP */
  /* SEE BELOW */                                    /* dump_mem 00001500 00002100 > plbu_scissor02_elements_12_dump_tile_lists.hex */
  res |= Check_m400_mp_1_F023_GP_PP0_check_0_a(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003380 00049880 > plbu_scissor02_elements_12_dump_plbu_heap.hex */
  res |= Check_m400_mp_1_F023_GP_PP0_check_0_b(malidata_0);

  /* COMMENT */                                      /* # PP0 */
  Mali_DisplayReg(1,0,0x102c);                       /* dump_reg 1000102C 1000103c */
  Mali_DisplayReg(1,0,0x1030);
  Mali_DisplayReg(1,0,0x1034);
  Mali_DisplayReg(1,0,0x1038);
  
  res |= Check_m400_mp_1_F023_GP_PP0_check_0_c();    /* dump_reg 1000102C 1000103c > GTxx01_00000_irq.hex */
  /* SEE BELOW */                                    /* dump_mem 100000c0 100020c0 > GTxx01_00000_fb.hex */
  res |= Check_m400_mp_1_F023_GP_PP0_check_0_d(malidata_0);

  Mali_WrReg(0x1,0x0,0x1024,0x00000001);             /* writereg 10001024 00000001 #clear irq */
  Mali_WrReg(0x1,0x0,0x100c,0x00000008);             /* writereg 1000100C 00000008 # flushcache */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_mp_1_F023_GP_PP0_check_0_a (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p) {
  struct t_CheckData_m400_mp_1_F023_GP_PP0_check_0_a *r = &CheckData_m400_mp_1_F023_GP_PP0_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 3072 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001500, r->data_00001500, 0, 0x2c0);
  res |= Mali_MemCmp(p->data_00002000, 0x00002000, r->data_00002000, 0, 0x040);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_mp_1_F023_GP_PP0_check_0_b (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p) {
  struct t_CheckData_m400_mp_1_F023_GP_PP0_check_0_b *r = &CheckData_m400_mp_1_F023_GP_PP0_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 288000 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003380, r->data_00003380, 0, 0x320);
  res |= Mali_MemCmp(p->data_00004000, 0x00004000, r->data_00004000, 0, 0x400);
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
  res |= Mali_MemCmp(p->data_00010000, 0x00010000, r->data_00010000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00011000, 0x00011000, r->data_00011000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00012000, 0x00012000, r->data_00012000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00013000, 0x00013000, r->data_00013000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00014000, 0x00014000, r->data_00014000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00015000, 0x00015000, r->data_00015000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00016000, 0x00016000, r->data_00016000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00017000, 0x00017000, r->data_00017000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00018000, 0x00018000, r->data_00018000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00019000, 0x00019000, r->data_00019000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001a000, 0x0001a000, r->data_0001a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001b000, 0x0001b000, r->data_0001b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001c000, 0x0001c000, r->data_0001c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001d000, 0x0001d000, r->data_0001d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001e000, 0x0001e000, r->data_0001e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0001f000, 0x0001f000, r->data_0001f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00020000, 0x00020000, r->data_00020000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00021000, 0x00021000, r->data_00021000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00022000, 0x00022000, r->data_00022000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00023000, 0x00023000, r->data_00023000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00024000, 0x00024000, r->data_00024000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00025000, 0x00025000, r->data_00025000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00026000, 0x00026000, r->data_00026000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00027000, 0x00027000, r->data_00027000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00028000, 0x00028000, r->data_00028000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00029000, 0x00029000, r->data_00029000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002a000, 0x0002a000, r->data_0002a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002b000, 0x0002b000, r->data_0002b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002c000, 0x0002c000, r->data_0002c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002d000, 0x0002d000, r->data_0002d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002e000, 0x0002e000, r->data_0002e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0002f000, 0x0002f000, r->data_0002f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00030000, 0x00030000, r->data_00030000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00031000, 0x00031000, r->data_00031000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00032000, 0x00032000, r->data_00032000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00033000, 0x00033000, r->data_00033000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00034000, 0x00034000, r->data_00034000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00035000, 0x00035000, r->data_00035000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00036000, 0x00036000, r->data_00036000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00037000, 0x00037000, r->data_00037000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00038000, 0x00038000, r->data_00038000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00039000, 0x00039000, r->data_00039000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003a000, 0x0003a000, r->data_0003a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003b000, 0x0003b000, r->data_0003b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003c000, 0x0003c000, r->data_0003c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003d000, 0x0003d000, r->data_0003d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003e000, 0x0003e000, r->data_0003e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_0003f000, 0x0003f000, r->data_0003f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00040000, 0x00040000, r->data_00040000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00041000, 0x00041000, r->data_00041000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00042000, 0x00042000, r->data_00042000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00043000, 0x00043000, r->data_00043000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00044000, 0x00044000, r->data_00044000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00045000, 0x00045000, r->data_00045000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00046000, 0x00046000, r->data_00046000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00047000, 0x00047000, r->data_00047000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00048000, 0x00048000, r->data_00048000, 0, 0x400);
  res |= Mali_MemCmp(p->data_00049000, 0x00049000, r->data_00049000, 0, 0x220);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_mp_1_F023_GP_PP0_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_1_F023_GP_PP0_check_0_c.data_10001020,1,0,0x1030,0x103c);
  return res;
};

int Check_m400_mp_1_F023_GP_PP0_check_0_d (struct t_m400_mp_1_F023_GP_PP0_mem_0 *p) {
  struct t_CheckData_m400_mp_1_F023_GP_PP0_check_0_d *r = &CheckData_m400_mp_1_F023_GP_PP0_check_0_d;
  int res = 0;
  Mali_Message("Performing data check of 8192 bytes\n");

  res |= Mali_MemCmp(p->data_10000000, 0x100000c0, r->data_100000c0, 0, 0x3d0);
  res |= Mali_MemCmp(p->data_10001000, 0x10001000, r->data_10001000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10002000, 0x10002000, r->data_10002000, 0, 0x030);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
