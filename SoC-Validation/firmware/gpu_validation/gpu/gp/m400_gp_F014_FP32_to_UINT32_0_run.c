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
#include "m400_gp_F014_FP32_to_UINT32_0_check_0_a.h"
#include "m400_gp_F014_FP32_to_UINT32_0_mem_0.h"

int Check_m400_gp_F014_FP32_to_UINT32_0_check_0_a (struct t_m400_gp_F014_FP32_to_UINT32_0_mem_0 *p);

int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part0 (void);
int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part1 (void);

static volatile struct t_m400_gp_F014_FP32_to_UINT32_0_mem_0 *malidata_0;
static int mali_step;

int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0 (int init) {
  if (init) { mali_step = 0; };
  switch (mali_step) {
    case 0  : return RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part0();
    case 1  : return RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part1();
    default : return 0;
  };
};

int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part0 (void) {
  int res = 0;
  printf("RUNNING TEST: m400_gp_F014_FP32_to_UINT32_0\n");

  Mali_Reset();                                      /* reset */
  /* SEE BELOW */                                    /* load_mem const.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem vertex_in.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem instr.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  /* SEE BELOW */                                    /* load_mem command.hex */
  /* Parsed hex file. Will pagetables will be setup when frame render start detected */

  Mali_WrReg(0x0,0x0,0x0,0x00000000);                /* writereg 00 00000000 */
  Mali_WrReg(0x0,0x0,0x4,0x00000038);                /* writereg 04 00000038 */
  Mali_WrReg(0x0,0x0,0x2c,0x000003ff);               /* writereg 2c 000003ff */
  /* Detected Frame Start. Initialising memory*/     /* writereg 20 00000001 */
  malidata_0 =(struct t_m400_gp_F014_FP32_to_UINT32_0_mem_0 *) Mali_LdMem(&m400_gp_F014_FP32_to_UINT32_0_mem_0,sizeof(struct t_m400_gp_F014_FP32_to_UINT32_0_mem_0),(int)m400_gp_F014_FP32_to_UINT32_0_mem_0.ttb_first);
  
  /* Enable optional performance counting */
  Mali_InitPerfCounters();
  Mali_WrReg(0x0,0x0,0x20,0x00000001);
  mali_step++;                                       /* wait posedge irq */
  /* WAIT FOR INTERRUPT - unless an error - return with WFI code */
  if (res == 0) {
   return 255;
  }
  return res;
}

int RunMaliTest_m400_gp_F014_FP32_to_UINT32_0_part1 (void) {
  int res = 0;
  int have_interrupts = 0;
  /* Check for any interrupt */
  have_interrupts = Mali_InterruptCheck(0xFFFFFFFF,0xFFFFFFFF);
  if (!have_interrupts) {
   return 255;
  }

  Mali_ReadPerfCounters();
  Mali_JobPartDone();
  /* SEE BELOW */                                    /* dump_mem 00002000 00002100 > mem_dump.hex */
  res |= Check_m400_gp_F014_FP32_to_UINT32_0_check_0_a(malidata_0);

  Mali_MaskAllInterrupts();                          /* quit */
  mali_step++;
  return res;
}

int Check_m400_gp_F014_FP32_to_UINT32_0_check_0_a (struct t_m400_gp_F014_FP32_to_UINT32_0_mem_0 *p) {
  struct t_CheckData_m400_gp_F014_FP32_to_UINT32_0_check_0_a *r = &CheckData_m400_gp_F014_FP32_to_UINT32_0_check_0_a;
  int res = 0;
  Mali_Message("Performing data check of 256 bytes\n");

  res |= Mali_MemCmp(p->data_00002000, 0x00002000, r->data_00002000, 0, 0x040);
  /* Return -1 if a compare fail */
  if (res) { return -1; };
  Mali_Message("Check okay \n");
  return 0;
};
