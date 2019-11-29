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
* Modified  : $Date: 2013/05/14 11:58:21 $
* Revision  : $Revision: 1.1 $
* Release   : $State: Exp $
*-----------------------------------------------------------------------------*/
#include "MaliFns.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_c.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_e.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f.h"
#include "m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0.h"

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p);
int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p);
int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_c ();
int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p);
int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_e ();
int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p);

int RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part0 (void);
int RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part1 (void);

static volatile struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part0();
    case 1  : return RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_mp_3_F017_MMU_GP_PP0_PP1\n");

  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x0,0x0,0x28,0x00080000);               /* writereg 00000028 00080000  # clear reset complete bit */
  /* SEE BELOW */                                    /* load_mem plbu_scissor02_elements_12_mem.hex 10000000 */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem GTxx01_00000.hex                   20000000 */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem GTxx09_00000.hex                   30000000 */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem page_table.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x3,0x0,0x1c,0x00000003);               /* writereg 0100001c 00000003  # enable L2 cache */
  /* COMMENT */                                      /* # GP */
  Mali_WrReg(0x2,0x0,0x0,0xf0000000);                /* writereg 02000000 f0000000  # set mmu page table directory address */
  Mali_WrReg(0x2,0x0,0x8,0x00000000);                /* writereg 02000008 00000000  # enable mmu */
  Mali_WrReg(0x0,0x0,0x8,0x00001300);                /* writereg 00000008 00001300  # GP_CTRL_REG_PLBU_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0xc,0x00001400);                /* writereg 0000000c 00001400  # GP_CTRL_REG_PLBU_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x28,0x00003fff);               /* writereg 00000028 00003fff  # GP_CTRL_REG_INT_CLEAR */
  Mali_WrReg(0x0,0x0,0x2c,0x000007fe);               /* writereg 0000002c 000007fe  # GP_CTRL_REG_INT_MASK */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000002 */
  malidata_0 = (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *)Mali_LdMem(&m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0,sizeof(struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0),(int)m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0.ttb_first);
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000002);
  /* COMMENT */                                      /* # PP0 */
  Mali_WrReg(0x2,0x1,0x0,0xf0002000);                /* writereg 12000000 f0002000 # set mmu page table directory address */
  Mali_WrReg(0x2,0x1,0x8,0x00000000);                /* writereg 12000008 00000000 # enable mmu */
  Mali_WrReg(0x1,0x0,0x0,0x00006480);                /* writereg 10000000 00006480 */
  Mali_WrReg(0x1,0x0,0x4,0x00000440);                /* writereg 10000004 00000440 */
  Mali_WrReg(0x1,0x0,0x8,0x000084c0);                /* writereg 10000008 000084c0 */
  Mali_WrReg(0x1,0x0,0x100,0x00000002);              /* writereg 10000100 00000002 */
  Mali_WrReg(0x1,0x0,0x104,0x00004460);              /* writereg 10000104 00004460 */
  Mali_WrReg(0x1,0x0,0x114,0x00000010);              /* writereg 10000114 00000010 */
  Mali_SetupOutputFrame();                           /* writereg 1000100c 00000040 */
  Mali_WrReg(0x1,0x0,0x100c,0x00000040);
  /* COMMENT */                                      /* # PP1 */
  Mali_WrReg(0x2,0x2,0x0,0xf0004000);                /* writereg 12010000 f0004000 */
  Mali_WrReg(0x2,0x2,0x8,0x00000000);                /* writereg 12010008 00000000 */
  Mali_WrReg(0x1,0x1,0x0,0x00006080);                /* writereg 10010000 00006080 */
  Mali_WrReg(0x1,0x1,0x4,0x00000040);                /* writereg 10010004 00000040 */
  Mali_WrReg(0x1,0x1,0x8,0x000080c0);                /* writereg 10010008 000080c0 */
  Mali_WrReg(0x1,0x1,0x100,0x00000002);              /* writereg 10010100 00000002 */
  Mali_WrReg(0x1,0x1,0x104,0x00004060);              /* writereg 10010104 00004060 */
  Mali_WrReg(0x1,0x1,0x114,0x00000010);              /* writereg 10010114 00000010 */
  Mali_WrReg(0x1,0x1,0x100c,0x00000040);             /* writereg 1001100c 00000040 */
  mali_step++;                                       /* wait irq 00010003 00010003 */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_mp_3_F017_MMU_GP_PP0_PP1_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  have_interrupts = Mali_InterruptCheck(0x00010003,0x00010003);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* COMMENT */                                      /* # GP */
  /* SEE BELOW */                                    /* dump_mem 10001500 10002100 > plbu_scissor02_elements_12_dump_tile_lists.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 10003380 10049880 > plbu_scissor02_elements_12_dump_plbu_heap.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b(malidata_0);

  /* COMMENT */                                      /* # PP0 */
  Mali_DisplayReg(1,0,0x102c);                       /* dump_reg 1000102C 1000103c */
  Mali_DisplayReg(1,0,0x1030);
  Mali_DisplayReg(1,0,0x1034);
  Mali_DisplayReg(1,0,0x1038);
  
  /* SEE BELOW */                                    /* dump_reg 1000102C 1000103c > GTxx01_00000_irq.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_c();

  /* SEE BELOW */                                    /* dump_mem 20004460 20006460 > GTxx01_00000_fb.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d(malidata_0);

  Mali_WrReg(0x1,0x0,0x1024,0x00000001);             /* writereg 10001024 00000001 #clear irq */
  Mali_WrReg(0x1,0x0,0x100c,0x00000008);             /* writereg 1000100C 00000008 # flushcache */
  /* COMMENT */                                      /* # PP1 */
  Mali_DisplayReg(1,1,0x102c);                       /* dump_reg 1001102c 1001103c */
  Mali_DisplayReg(1,1,0x1030);
  Mali_DisplayReg(1,1,0x1034);
  Mali_DisplayReg(1,1,0x1038);
  
  /* SEE BELOW */                                    /* dump_reg 1001102c 1001103c > GTxx09_00000_irq.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_e();

  /* SEE BELOW */                                    /* dump_mem 30004060 30006060 > GTxx09_00000_fb.hex */
  res |= Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f(malidata_0);

  Mali_WrReg(0x1,0x1,0x1024,0x00000001);             /* writereg 10011024 00000001 #clear irq */
  Mali_WrReg(0x1,0x1,0x100c,0x00000008);             /* writereg 1001100c 00000008 # flushcache */
  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p) {
  struct t_CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a *r = &CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 3072 bytes\n");

  res |= Mali_MemCmp(p->data_10001000, 0x10001500, r->data_10001500, 0, 0x2c0);
  res |= Mali_MemCmp(p->data_10002000, 0x10002000, r->data_10002000, 0, 0x040);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p) {
  struct t_CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b *r = &CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 288000 bytes\n");

  res |= Mali_MemCmp(p->data_10003000, 0x10003380, r->data_10003380, 0, 0x320);
  res |= Mali_MemCmp(p->data_10004000, 0x10004000, r->data_10004000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10005000, 0x10005000, r->data_10005000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10006000, 0x10006000, r->data_10006000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10007000, 0x10007000, r->data_10007000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10008000, 0x10008000, r->data_10008000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10009000, 0x10009000, r->data_10009000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000a000, 0x1000a000, r->data_1000a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000b000, 0x1000b000, r->data_1000b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000c000, 0x1000c000, r->data_1000c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000d000, 0x1000d000, r->data_1000d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000e000, 0x1000e000, r->data_1000e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1000f000, 0x1000f000, r->data_1000f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10010000, 0x10010000, r->data_10010000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10011000, 0x10011000, r->data_10011000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10012000, 0x10012000, r->data_10012000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10013000, 0x10013000, r->data_10013000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10014000, 0x10014000, r->data_10014000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10015000, 0x10015000, r->data_10015000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10016000, 0x10016000, r->data_10016000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10017000, 0x10017000, r->data_10017000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10018000, 0x10018000, r->data_10018000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10019000, 0x10019000, r->data_10019000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001a000, 0x1001a000, r->data_1001a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001b000, 0x1001b000, r->data_1001b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001c000, 0x1001c000, r->data_1001c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001d000, 0x1001d000, r->data_1001d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001e000, 0x1001e000, r->data_1001e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1001f000, 0x1001f000, r->data_1001f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10020000, 0x10020000, r->data_10020000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10021000, 0x10021000, r->data_10021000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10022000, 0x10022000, r->data_10022000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10023000, 0x10023000, r->data_10023000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10024000, 0x10024000, r->data_10024000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10025000, 0x10025000, r->data_10025000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10026000, 0x10026000, r->data_10026000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10027000, 0x10027000, r->data_10027000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10028000, 0x10028000, r->data_10028000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10029000, 0x10029000, r->data_10029000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002a000, 0x1002a000, r->data_1002a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002b000, 0x1002b000, r->data_1002b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002c000, 0x1002c000, r->data_1002c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002d000, 0x1002d000, r->data_1002d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002e000, 0x1002e000, r->data_1002e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1002f000, 0x1002f000, r->data_1002f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10030000, 0x10030000, r->data_10030000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10031000, 0x10031000, r->data_10031000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10032000, 0x10032000, r->data_10032000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10033000, 0x10033000, r->data_10033000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10034000, 0x10034000, r->data_10034000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10035000, 0x10035000, r->data_10035000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10036000, 0x10036000, r->data_10036000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10037000, 0x10037000, r->data_10037000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10038000, 0x10038000, r->data_10038000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10039000, 0x10039000, r->data_10039000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003a000, 0x1003a000, r->data_1003a000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003b000, 0x1003b000, r->data_1003b000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003c000, 0x1003c000, r->data_1003c000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003d000, 0x1003d000, r->data_1003d000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003e000, 0x1003e000, r->data_1003e000, 0, 0x400);
  res |= Mali_MemCmp(p->data_1003f000, 0x1003f000, r->data_1003f000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10040000, 0x10040000, r->data_10040000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10041000, 0x10041000, r->data_10041000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10042000, 0x10042000, r->data_10042000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10043000, 0x10043000, r->data_10043000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10044000, 0x10044000, r->data_10044000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10045000, 0x10045000, r->data_10045000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10046000, 0x10046000, r->data_10046000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10047000, 0x10047000, r->data_10047000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10048000, 0x10048000, r->data_10048000, 0, 0x400);
  res |= Mali_MemCmp(p->data_10049000, 0x10049000, r->data_10049000, 0, 0x220);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_c () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_c.data_10001020,1,0,0x102c,0x103c);
  return res;
};

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p) {
  struct t_CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d *r = &CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_d;
  int res = 0;
  Mali_Message("Performing data check of 8192 bytes\n");

  res |= Mali_MemCmp(p->data_20004000, 0x20004460, r->data_20004460, 0, 0x2e8);
  res |= Mali_MemCmp(p->data_20005000, 0x20005000, r->data_20005000, 0, 0x400);
  res |= Mali_MemCmp(p->data_20006000, 0x20006000, r->data_20006000, 0, 0x118);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_e () {
  int res = 0;
  res |= Mali_CompareRegs(CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_e.data_10011020,1,1,0x102c,0x103c);
  return res;
};

int Check_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f (struct t_m400_mp_3_F017_MMU_GP_PP0_PP1_mem_0 *p) {
  struct t_CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f *r = &CheckData_m400_mp_3_F017_MMU_GP_PP0_PP1_check_0_f;
  int res = 0;
  Mali_Message("Performing data check of 8192 bytes\n");

  res |= Mali_MemCmp(p->data_30004000, 0x30004060, r->data_30004060, 0, 0x3e8);
  res |= Mali_MemCmp(p->data_30005000, 0x30005000, r->data_30005000, 0, 0x400);
  res |= Mali_MemCmp(p->data_30006000, 0x30006000, r->data_30006000, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};