/*----------------------------------------------------------------------------
*
*The confidential and proprietary information contained in this file may
*only be used by a person authorised under and to the extent permitted
*by a subsisting licensing agreement from ARM Limited.
*
*        (C) COPYRIGHT 2008-2009 ARM Limited.
*             ALL RIGHTS RESERVED
*             
*This entire notice must be reproduced on all copies of this file
*and copies of this file may only be made by a person if such person is
*permitted to do so under the terms of a subsisting license agreement
*from ARM Limited.
*Modified  : $Date: 2017/10/06 07:03:57 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract :Implementaion of all functions that are used to access MALI
*-----------------------------------------------------------------------------
* Overview
*----------
* 
**************************************************************/

#include <MaliFns.h>
#include <asm/arch/shikhara_map.h>
//#include <stdio.h>
#include <common.h>
#define MALI m400
int *Mali_Address(int unit,int core);

void Mali_Reset(void) 
{
  /* Reset Mali PP */
  /*  Mali_WrRegAll(1,MALI_PP_REG_MGMT_INT_CLEAR,0x1F); */
#if MALI == m400
#endif
Mali_WrRegAll(1,MALI_PP_REG_MGMT_CTRL,0x80);
  /* Reset Mali GP */
/*    Mali_WrRegAll(0,MALI_GP_REG_INT_CLEAR,0x1FFF); */
  Mali_WrRegAll(0,MALI_GP_REG_CMD,0x400); 

#if MALI == m400
  /* Reset L2 */
  Mali_WrRegAll(3,0x10,1);  /* L2 Flushed */
// #if FORCEL2
  Mali_WrRegAll(3,0x1C,3);  /* L2 enabled */
// #endif
#endif

#if MALI == m400
  /* Reset MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_CMD_REG,2); /* stall MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_CMD_REG,6); /* soft-reset MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_CMD_REG,3); /* out of stall mode MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_IRQ_CLEAR,0x0);  
#else
  /* Reset MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_CMD_REG,6); /* soft-reset MMU */
  Mali_WrRegAll(2,MALI_MMU_REG_IRQ_CLEAR,0x0);  
#endif


};

void *Mali_LdMem(void *srcptr,int size,int ttb_base) {
#if MALI == m400
  /* Setup MMU to point to data structures for this image */
  Mali_WrRegAll(2,0x8,2); /* stall MMU  */
  Mali_WrRegAll(2,0x0,ttb_base);
  Mali_WrRegAll(2,0x8,4); /* clear MMU  */
  Mali_WrRegAll(2,0x1C,3); /* enable interrupts */
  Mali_WrRegAll(2,0x8,0); /* enable mmu paging */
  Mali_WrRegAll(2,0x8,3); /* un-stall MMU  */
#else
  /* Setup MMU to point to data structures for this image */
  Mali_WrRegAll(2,0x0,ttb_base);
  Mali_WrRegAll(2,0x8,4); /* clear MMU  */
  Mali_WrRegAll(2,0x1C,3); /* enable interrupts */
  Mali_WrRegAll(2,0x8,0); /* enable mmu paging */
#endif

  return srcptr;
};

/* This function checks if we have the required interrupts.
 * It polls the Mali interrupt registers for this.
 * If we do not have the required interrupts, then it remembers which ones we 
 * have seen, and then masks them.
 */
int Mali_InterruptCheck(int i_mask, int i_value)
{

  int have_any_int = 0;  /* Set to 1 is any of the interrupts are triggered */
  int ret_int      = 0;  /* Holds the return value to the calling routine */

  static int interrupt_state = 0; /* updated to hold the value of the interrupt triggred */

  int mask_gp        = 0;          /* Indicates if GP INT needs to be masked */
  int mask_gp_mmu    = 0;          /* Indicates if GP MMU INT needs to be masked */
  int mask_pp[4]     = {0,0,0,0};  /* Indicates if PP INT needs to be masked, one for each core */
  int mask_pp_mmu[4] = {0,0,0,0};  /* Indicates if PP MMU INT needs to be masked, one for each core */
 
  int core;
  int rd_int        = 0;           /* Holds the status of Interrupt register read for each core */

  printf ("Interrupt Received\n");
  

  if (((i_mask >> 16) & 1) > 0) {
    rd_int             = Mali_RdReg(0,0,0x68) & 0x1;           /* read GP INT bit */    
    if (rd_int !=0){
        interrupt_state |= 0x1 << 16;
        mask_gp = 1;
        have_any_int = 1;
    }
  }
  
  if (((i_mask >> 24) & 1) > 0) {
    rd_int             = Mali_RdReg(2,0,0x20) & 0x3;           /* read GP MMU INT bit */
    if (rd_int !=0){
        interrupt_state |= 0x1 << 24;
        mask_gp_mmu = 1;
        have_any_int = 1;
    }
  }
  
  for (core=0; core < PP_CORES; core++) {
    if (((i_mask >> core) & 1) > 0) {
      rd_int     = Mali_RdReg(1,core,0x1008) & (0x1 << 6) ; /* read PPx bit */
      if (rd_int !=0) {
          interrupt_state |= 0x1 << core;
          mask_pp[core] = 1;
          have_any_int = 1;
      }
    }
    if (((i_mask >> 24) & 1) > 0) {
      rd_int = Mali_RdReg(2,core+1,0x20) & 0x3;           /* read PPX MMU INT bit */
      if (rd_int !=0){
        interrupt_state |= 0x1 << 24;
        mask_pp_mmu[core] = 1;
        have_any_int = 1;
      }
    }
  }


  /* Update return status of interrupt if conditions matched else mask the triggered interrupts 
   * and return false */
  if ((have_any_int && i_mask == 0xFFFFFFFF) || interrupt_state == i_mask ){
      ret_int = 1;
      interrupt_state = 0;
  }
  else
  {
      if (mask_gp) {
           printf ("Masking GP INT\n");
          Mali_WrReg(0x0,0x0,0x2c,0x00000000); 
      }

      if (mask_gp_mmu) {
          printf ("Masking GP MMU INT\n");
          Mali_WrReg(0x2,0x0,0x001c,0x00000000);
      }

      for (core=0; core < PP_CORES; core++) {
          if (mask_pp[core]) {
              printf ("Masking PP%d INT\n", core);
              Mali_WrReg(0x1,core,0x1028,0x00000000);   
          }
          if (mask_pp_mmu[core]) {
              printf ("Masking PP%d MMU INT\n", core);
              Mali_WrReg(0x2,core+1,0x001c,0x00000000);
          }
      }
  }
  printf ("Done Processing Interrupt\n");
  return ret_int;

};

void Mali_MaskAllInterrupts (void) {
  int core;
  Mali_WrReg(0x0,0x0,0x2c,0x00000000); 
  Mali_WrReg(0x2,0x0,0x001c,0x00000000);
  for (core=0; core < PP_CORES; core++) {
    Mali_WrReg(0x1,core,0x1028,0x00000000);   
    Mali_WrReg(0x2,core+1,0x001c,0x00000000);
  };
};


/* This will write to the register is in all cores */
/* i.e. if MMU - all MMUs */
/*      if PP  - all PPs  */
void Mali_WrRegAll(int unit,int regnum,int value)
{
  int core;

  /* write register to first core */
  Mali_WrReg(unit,0,regnum,value);

  /* write register to all other cores */
  /* 3 mmus ; 2 pps */
#if MALI == m400
  /* For PP (unit == 1) write to any other cores */
  if (unit == 1) {
    for (core=1; core < PP_CORES; core++) {
      Mali_WrReg(unit,core,regnum,value);
    };
  };
  /* For MMU (unit == 2) we have core 0 is mmu, so write to all PP MMUs */
  if (unit == 2) {
    for (core=1; core < (PP_CORES+1); core++) {
      Mali_WrReg(unit,core,regnum,value);
    };
  };
#endif
};

int Mali_RdReg(int unit,int core, int regnum)
{
  int *reg_ptr = Mali_Address(unit,core);
  reg_ptr += (regnum/4);
  
  return *reg_ptr;
};


void Mali_WrReg(int unit,int core,int regnum,int value)
{
  int *reg_ptr = Mali_Address(unit,core);
  reg_ptr += (regnum/4);
  
  *reg_ptr = value;
};

#define  M400_PP_CORE_OFFSET  0x2000 
#define  M400_MMU_CORE_OFFSET 0x1000 

int *Mali_Address(int unit,int core)
{
  int *reg_ptr = (int *)0;

#if MALI == m400
  /* Write the appropriate register */
  if (unit == 0) { /* GP */
    reg_ptr = (int *)MALI_BASE;
  };
  if (unit == 1) { /* PP */
    reg_ptr = (int *)(MALI_BASE + 0x8000 + (M400_PP_CORE_OFFSET * core));
  };
  if (unit == 2) { /* MMU first one is for GP */
    reg_ptr = (int *)(MALI_BASE + 0x3000 + (M400_MMU_CORE_OFFSET * core));
  };
  if (unit == 3) { /* L2 */
    reg_ptr = (int *)(MALI_BASE + 0x1000);
  };
#else
  /* Write the appropriate register */
  if (unit == 0) { /* GP */
    reg_ptr = (int *)(MALI_BASE+0x2000);
  };
  if (unit == 1) { /* PP */
    reg_ptr = (int *)(MALI_BASE);
  };
  if (unit == 2) { /* MMU */
    reg_ptr = (int *)(MALI_BASE+0x3000);
  };
#endif
  return reg_ptr;
};

