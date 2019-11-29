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
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p.h"
#include "m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0.h"

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);
int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p);

int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part0 (void);
int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part1 (void);

static volatile struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part0();
    case 1  : return RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F208_plbu_set_rsw_vdb_address_00\n");

  /* COMMENT */                                      /* # Mali GP dump */
  /* COMMENT */                                      /* # Test name: plbu_set_rsw_vdb_address */
  /* COMMENT */                                      /* # Dump   nr: 0 */
  Mali_Reset();                                      /* reset */
  Mali_WrReg(0x0,0x0,0x28,0x00080000);               /* writereg 00000028 00080000  # clear reset complete bit */
  /* SEE BELOW */                                    /* load_mem input_mem.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x0,0x0,0x8,0x00000a80);                /* writereg 00000008 00000a80  # GP_CTRL_REG_PLBU_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0xc,0x00000b28);                /* writereg 0000000c 00000b28  # GP_CTRL_REG_PLBU_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x28,0x00003fff);               /* writereg 00000028 00003fff  # GP_CTRL_REG_INT_CLEAR */
  Mali_WrReg(0x0,0x0,0x2c,0x000007fe);               /* writereg 0000002c 000007fe  # GP_CTRL_REG_INT_MASK */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000002 */
  malidata_0 = (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *)Mali_LdMem(&m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0,sizeof(struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0),(int)m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0.ttb_first);
  
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

int RunMaliTest_m400_gp_F208_plbu_set_rsw_vdb_address_00_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_mem 00000c80 00001130 > mem_dump00.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001400 00002778 > mem_dump01.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002780 00002c70 > mem_dump02.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002c80 00002ce0 > mem_dump03.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002d00 00002d78 > mem_dump04.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002d80 00002e60 > mem_dump05.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002e80 00002ef8 > mem_dump06.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00002f00 00003060 > mem_dump07.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003080 000030f0 > mem_dump08.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003100 00003138 > mem_dump09.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003180 000031b8 > mem_dump10.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003200 00003248 > mem_dump11.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003280 000032a8 > mem_dump12.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003300 00003328 > mem_dump13.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003380 00003398 > mem_dump14.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00003400 00003410 > mem_dump15.hex */
  res |= Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p(malidata_0);

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 1200 bytes\n");

  res |= Mali_MemCmp(p->data_00000000, 0x00000c80, r->data_00000c80, 0, 0x0e0);
  res |= Mali_MemCmp(p->data_00001000, 0x00001000, r->data_00001000, 0, 0x04c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 4984 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001400, r->data_00001400, 0, 0x300);
  res |= Mali_MemCmp(p->data_00002000, 0x00002000, r->data_00002000, 0, 0x1de);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_c;
  int res = 0;
  Mali_Message("Performing data check of 1264 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002780, r->data_00002780, 0, 0x13c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_d;
  int res = 0;
  Mali_Message("Performing data check of 96 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002c80, r->data_00002c80, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_e;
  int res = 0;
  Mali_Message("Performing data check of 120 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002d00, r->data_00002d00, 0, 0x01e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_f;
  int res = 0;
  Mali_Message("Performing data check of 224 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002d80, r->data_00002d80, 0, 0x038);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_g;
  int res = 0;
  Mali_Message("Performing data check of 120 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002e80, r->data_00002e80, 0, 0x01e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_h;
  int res = 0;
  Mali_Message("Performing data check of 352 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002f00, r->data_00002f00, 0, 0x040);
  res |= Mali_MemCmp(p->data_00003000, 0x00003000, r->data_00003000, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_i;
  int res = 0;
  Mali_Message("Performing data check of 112 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003080, r->data_00003080, 0, 0x01c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_j;
  int res = 0;
  Mali_Message("Performing data check of 56 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003100, r->data_00003100, 0, 0x00e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_k;
  int res = 0;
  Mali_Message("Performing data check of 56 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003180, r->data_00003180, 0, 0x00e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_l;
  int res = 0;
  Mali_Message("Performing data check of 72 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003200, r->data_00003200, 0, 0x012);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_m;
  int res = 0;
  Mali_Message("Performing data check of 40 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003280, r->data_00003280, 0, 0x00a);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_n;
  int res = 0;
  Mali_Message("Performing data check of 40 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003300, r->data_00003300, 0, 0x00a);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o;
  int res = 0;
  Mali_Message("Performing data check of 32 bytes\n");

  res |= Mali_MemCmpMasked(p->data_00003000, 0x00003380, r->data_00003380, Mask_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_o_0  , 0x0, 0x008);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p (struct t_m400_gp_F208_plbu_set_rsw_vdb_address_00_mem_0 *p) {
  struct t_CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p *r = &CheckData_m400_gp_F208_plbu_set_rsw_vdb_address_00_check_0_p;
  int res = 0;
  Mali_Message("Performing data check of 16 bytes\n");

  res |= Mali_MemCmp(p->data_00003000, 0x00003400, r->data_00003400, 0, 0x004);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
