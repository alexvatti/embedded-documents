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
*Modified  : $Date: 2017/10/06 07:03:54 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract : System Specific Functions - including interrupts
*----------------------------------------------------------------------------*/

#include "MaliFns.h"
//#include <stdio.h>
//#include "common.h"
//#include "IntHandler.h"
#include <asm/io.h>

void CPU_EnableInterrupts(void) {
//  enable_irq();
enable_interrupts();
};
void CPU_DisableInterrupts(void) {
//  disable_irq();
    disable_interrupts();
};
void CPU_InitialiseInterrupts(void) {
//  init_and_install_gic();
gic_init();
};

void Mali_WaitForInterrupt(void) {
  call_wfi();
//for(;;);
};

void Mali_WaitForClk(int cyc)
{ 
    int count = cyc *4;
    printf ("Mali_WaitForClk: cyc=%d, count=%d\n", cyc, count);
    while (count > 0)
        count--;
};

void Mali_InstallIntHandlers(void (*callback_mali_gp)(void), 
                            void (*callback_mali_pp)(void), 
                            void (*callback_mali_mmu)(void)) {
//#if MALI == m400
  printf("m400\n");
/*  interrupt_handler_set(159,callback_mali_gp);  // GP
  interrupt_handler_set(154,callback_mali_mmu); // GP_MMU
  interrupt_handler_set(155,callback_mali_pp);  // PP0
  interrupt_handler_set(150,callback_mali_mmu); // PP0_MMU
  interrupt_handler_set(156,callback_mali_pp);  // PP1
  interrupt_handler_set(151,callback_mali_mmu); // PP1_MMU
  interrupt_handler_set(157,callback_mali_pp);  // PP2_3
  interrupt_handler_set(152,callback_mali_mmu); // PP2_3_MMU
*/
/*int_enable(159);
  int_enable(154);
  int_enable(155);
  int_enable(150);
  int_enable(156);
  int_enable(151);
  int_enable(157);
  int_enable(152);*/


  interrupt_handler_set(65,callback_mali_gp); // GP
  interrupt_handler_set(57,callback_mali_pp); //PP0
  interrupt_handler_set(59,callback_mali_pp); //PP1
  interrupt_handler_set(61,callback_mali_pp); //PP2
  interrupt_handler_set(63,callback_mali_pp); //PP3
  interrupt_handler_set(58,callback_mali_mmu); //PP0_MMU
  interrupt_handler_set(60,callback_mali_mmu); //PP1_MMU
  interrupt_handler_set(62,callback_mali_mmu); //PP2_MMU
  interrupt_handler_set(64,callback_mali_mmu); //PP3_MMU

  int_enable(57);
  int_enable(59);
  int_enable(61);
  int_enable(63);
  int_enable(58);
  int_enable(60);
  int_enable(62);
  int_enable(64);
  int_enable(65);
/*#else
  printf("m200\n");
  interrupt_handler_set(65,callback_mali_gp);
  interrupt_handler_set(67,callback_mali_pp);
  interrupt_handler_set(68,callback_mali_mmu);

  int_enable(65);
  int_enable(67);
  int_enable(68);

#endif*/
};

/* For future use. Called just before a PP job starts */
void Mali_SetupOutputFrame (void) {
};

/* For future used. Called once a job part has recieved its required interrupts */
/* It is not currently used; but would be a suitable place to position CPU cache cleaning */
/* routines - if it was required to clean/invalidate the cpu caches before performing */
/* self checking */
void Mali_JobPartDone (void) {
};

void Mali_Message(char *s) {
  printf("%s",s);
};
