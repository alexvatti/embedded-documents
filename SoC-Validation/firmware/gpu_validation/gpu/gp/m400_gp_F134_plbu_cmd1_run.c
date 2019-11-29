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
#include "m400_gp_F134_plbu_cmd1_check_0_a.h"
#include "m400_gp_F134_plbu_cmd1_check_0_b.h"
#include "m400_gp_F134_plbu_cmd1_check_0_c.h"
#include "m400_gp_F134_plbu_cmd1_check_0_d.h"
#include "m400_gp_F134_plbu_cmd1_check_0_e.h"
#include "m400_gp_F134_plbu_cmd1_check_0_f.h"
#include "m400_gp_F134_plbu_cmd1_check_0_g.h"
#include "m400_gp_F134_plbu_cmd1_check_0_h.h"
#include "m400_gp_F134_plbu_cmd1_check_0_i.h"
#include "m400_gp_F134_plbu_cmd1_check_0_j.h"
#include "m400_gp_F134_plbu_cmd1_check_0_k.h"
#include "m400_gp_F134_plbu_cmd1_check_0_l.h"
#include "m400_gp_F134_plbu_cmd1_check_0_m.h"
#include "m400_gp_F134_plbu_cmd1_check_0_n.h"
#include "m400_gp_F134_plbu_cmd1_check_0_o.h"
#include "m400_gp_F134_plbu_cmd1_check_0_p.h"
#include "m400_gp_F134_plbu_cmd1_check_0_q.h"
#include "m400_gp_F134_plbu_cmd1_check_0_r.h"
#include "m400_gp_F134_plbu_cmd1_mem_0.h"

int Check_m400_gp_F134_plbu_cmd1_check_0_a (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_b (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_c (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_d (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_e (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_f (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_g (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_h (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_i (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_j (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_k (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_l (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_m (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_n (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_o (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_p (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_q (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);
int Check_m400_gp_F134_plbu_cmd1_check_0_r (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p);

int RunMaliTest_m400_gp_F134_plbu_cmd1_part0 (void);
int RunMaliTest_m400_gp_F134_plbu_cmd1_part1 (void);

static volatile struct t_m400_gp_F134_plbu_cmd1_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_gp_F134_plbu_cmd1 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F134_plbu_cmd1_part0();
    case 1  : return RunMaliTest_m400_gp_F134_plbu_cmd1_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F134_plbu_cmd1_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F134_plbu_cmd1\n");

  /* COMMENT */                                      /* # Mali GP dump */
  /* COMMENT */                                      /* # Test name: apb_perf_count */
  /* COMMENT */                                      /* # Dump   nr: 0 */
  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem input_mem.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x0,0x0,0x0,0x00000d00);                /* writereg  00000000  00000d00  # GP_CTRL_REG_VS_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0x4,0x00000d40);                /* writereg  00000004  00000d40  # GP_CTRL_REG_VS_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x8,0x00000f00);                /* writereg  00000008  00000f00  # GP_CTRL_REG_PLBU_CL_CURRENT_ADDR */
  Mali_WrReg(0x0,0x0,0xc,0x00001008);                /* writereg  0000000c  00001008  # GP_CTRL_REG_PLBU_CL_END_ADDR */
  Mali_WrReg(0x0,0x0,0x2c,0x000007ff);               /* writereg  0000002c  000007ff  # GP_CTRL_REG_INT_MASK */
  /* Detected Frame Start. Initialising memory*/     /* writereg 00000020 00000003 */
  malidata_0 = (struct t_m400_gp_F134_plbu_cmd1_mem_0 *)Mali_LdMem(&m400_gp_F134_plbu_cmd1_mem_0,sizeof(struct t_m400_gp_F134_plbu_cmd1_mem_0),(int)m400_gp_F134_plbu_cmd1_mem_0.ttb_first);
  
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

int RunMaliTest_m400_gp_F134_plbu_cmd1_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_mem 00000700 00000d00 > mem_dump00.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_a(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001200 00001250 > mem_dump02.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_b(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001280 000012d0 > mem_dump03.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_c(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001300 00001348 > mem_dump04.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_d(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001380 000013b8 > mem_dump05.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_e(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001400 00001470 > mem_dump06.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_f(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001480 000014f8 > mem_dump07.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_g(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001500 00001568 > mem_dump08.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_h(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001580 000015d0 > mem_dump09.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_i(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001600 00001670 > mem_dump10.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_j(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001680 000016f0 > mem_dump11.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_k(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001700 00001760 > mem_dump12.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_l(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001780 000017c8 > mem_dump13.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_m(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001800 00001868 > mem_dump14.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_n(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001880 000018e8 > mem_dump15.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_o(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001900 00001958 > mem_dump16.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_p(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001980 000019c0 > mem_dump17.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_q(malidata_0);

  /* SEE BELOW */                                    /* dump_mem 00001a00 00001eb0 > mem_dump18.hex */
  res |= Check_m400_gp_F134_plbu_cmd1_check_0_r(malidata_0);

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_gp_F134_plbu_cmd1_check_0_a (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_a *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 1536 bytes\n");

  res |= Mali_MemCmp(p->data_00000000, 0x00000700, r->data_00000700, 0, 0x180);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_b (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_b *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_b;
  int res = 0;
  Mali_Message("Performing data check of 80 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001200, r->data_00001200, 0, 0x014);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_c (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_c *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_c;
  int res = 0;
  Mali_Message("Performing data check of 80 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001280, r->data_00001280, 0, 0x014);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_d (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_d *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_d;
  int res = 0;
  Mali_Message("Performing data check of 72 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001300, r->data_00001300, 0, 0x012);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_e (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_e *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_e;
  int res = 0;
  Mali_Message("Performing data check of 56 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001380, r->data_00001380, 0, 0x00e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_f (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_f *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_f;
  int res = 0;
  Mali_Message("Performing data check of 112 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001400, r->data_00001400, 0, 0x01c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_g (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_g *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_g;
  int res = 0;
  Mali_Message("Performing data check of 120 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001480, r->data_00001480, 0, 0x01e);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_h (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_h *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_h;
  int res = 0;
  Mali_Message("Performing data check of 104 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001500, r->data_00001500, 0, 0x01a);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_i (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_i *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_i;
  int res = 0;
  Mali_Message("Performing data check of 80 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001580, r->data_00001580, 0, 0x014);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_j (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_j *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_j;
  int res = 0;
  Mali_Message("Performing data check of 112 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001600, r->data_00001600, 0, 0x01c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_k (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_k *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_k;
  int res = 0;
  Mali_Message("Performing data check of 112 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001680, r->data_00001680, 0, 0x01c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_l (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_l *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_l;
  int res = 0;
  Mali_Message("Performing data check of 96 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001700, r->data_00001700, 0, 0x018);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_m (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_m *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_m;
  int res = 0;
  Mali_Message("Performing data check of 72 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001780, r->data_00001780, 0, 0x012);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_n (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_n *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_n;
  int res = 0;
  Mali_Message("Performing data check of 104 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001800, r->data_00001800, 0, 0x01a);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_o (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_o *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_o;
  int res = 0;
  Mali_Message("Performing data check of 104 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001880, r->data_00001880, 0, 0x01a);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_p (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_p *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_p;
  int res = 0;
  Mali_Message("Performing data check of 88 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001900, r->data_00001900, 0, 0x016);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_q (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_q *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_q;
  int res = 0;
  Mali_Message("Performing data check of 64 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001980, r->data_00001980, 0, 0x010);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};

int Check_m400_gp_F134_plbu_cmd1_check_0_r (struct t_m400_gp_F134_plbu_cmd1_mem_0 *p) {
  struct t_CheckData_m400_gp_F134_plbu_cmd1_check_0_r *r = &CheckData_m400_gp_F134_plbu_cmd1_check_0_r;
  int res = 0;
  Mali_Message("Performing data check of 1200 bytes\n");

  res |= Mali_MemCmp(p->data_00001000, 0x00001a00, r->data_00001a00, 0, 0x12c);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
