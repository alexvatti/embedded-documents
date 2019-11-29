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
*Modified  : $Date: 2017/10/06 07:06:36 $
*Revision  : $Revision: 1.1 $
*Release   : $State: Exp $
*-----------------------------------------------------------------------------
* 
*-----------------------------------------------------------------------------
* Abstract :Declaration of Mali functions 
*-----------------------------------------------------------------------------
* Overview
*----------
* 
**************************************************************/
#include <asm/arch/shikhara_map.h>
#include <common.h>
#define MALI_BASE       SHIKHARA_GPU_BASE
#define  m400 1
#define  m200 0
#ifndef CONFIG_SHIKHARA_ASIC
#define  PP_CORES 2
#else
#define PP_CORES 4
#endif

/****************************************************************************
 * SYSTEM SPECIFIC FUNCTIONS                                                  
 ****************************************************************************/
void CPU_EnableInterrupts(void);
void CPU_DisableInterrupts(void);
void CPU_InitialiseInterrupts(void);

void Mali_WaitForClk(int);
void Mali_WaitForInterrupt(void);
void MaliInstallIntHandlers(void (*callback_mali_gp)(void), 
                            void (*callback_mali_pp)(void), 
                            void (*callback_mali_mmu)(void));
void Mali_Message(char *s);
void Mali_SetupOutputFrame(void);
void Mali_JobPartDone(void);

/****************************************************************************
 * MALI REGISTER READ / WRITE
 ****************************************************************************/
void Mali_Reset(void);
void *Mali_LdMem(void *srcptr,int size,int ttb_base);
void Mali_WrReg(int unit,int core,int regnum,int value);
int  Mali_RdReg(int unit,int core,int regnum);
void Mali_WrRegAll(int unit,int regnum,int value);
void Mali_DisplayReg(int unit,int core, int regnum);

int Mali_InterruptCheck(int i_mask, int i_value);
void Mali_MaskAllInterrupts(void);

/****************************************************************************
 * MALI PERFORMANCE COUNTERS
 ****************************************************************************/
void Mali_InitPerfCounters(void);
void Mali_ReadPerfCounters(void);

/****************************************************************************
 * UTILITY FUNCTIONS FOR CHECKING TEST RESULTS
 ****************************************************************************/
int Mali_MemCmp(unsigned int *malidata_page,
                unsigned int mali_va,
                unsigned int *refdata,
                unsigned int refoffset,
                unsigned int len);

int Mali_MemCmpMasked(unsigned int *malidata_page,
                      unsigned int mali_va,
                      unsigned int *refdata,
                      unsigned int *refmask,
                      unsigned int refoffset,
                      unsigned int len);

int Mali_CompareRegs(unsigned int *reference_ptr, 
                      int mali_unit,
                     int mali_core,
                     int lowreg, 
                     int highreg);




/***********************************************
 * This Section defines the the BASE Addresses *
 ***********************************************/


/* MALI_GP_REGISTERS */
#define MALI_GP_REG_CMD				0x0020
#define MALI_GP_REG_INT_RAWSTAT			0x0024
#define MALI_GP_REG_INT_CLEAR			0x0028

/* MALI_PP_REGISTERS */
#define MALI_PP_REG_MGMT_VERSION			0x1000
#define MALI_PP_REG_MGMT_CURRENT_REND_LIST_ADDR		0x1004
#define MALI_PP_REG_MGMT_STATUS				0x1008
#define MALI_PP_REG_MGMT_CTRL				0x100C
#define MALI_PP_REG_MGMT_INT_RAWSTAT			0x1020
#define MALI_PP_REG_MGMT_INT_CLEAR			0x1024
#define MALI_PP_REG_MGMT_INT_MASK			0x1028
#define MALI_PP_REG_MGMT_INT_STATUS			0x102C
#define MALI_PP_REG_MGMT_WRITE_BOUNDARY_ENABLE		0x1040
#define MALI_PP_REG_MGMT_WRITE_BOUNDARY_LOW		0x1044
#define MALI_PP_REG_MGMT_WRITE_BOUNDARY_HIGH		0x1048
#define MALI_PP_REG_MGMT_WRITE_BOUNDARY_ADDRESS		0x104c
#define MALI_PP_REG_MGMT_BUS_ERROR_STATUS		0x1050
#define MALI_PP_REG_MGMT_WATCHDOG_DISABLE		0x1060
#define MALI_PP_REG_MGMT_WATCHDOG_TIMEOUT		0x1064
#define MALI_PP_REG_MGMT_PERF_CNT_0_ENABLE		0x1080
#define MALI_PP_REG_MGMT_PERF_CNT_0_SRC			0x1084
#define MALI_PP_REG_MGMT_PERF_CNT_0_LIMIT		0x1088
#define MALI_PP_REG_MGMT_PERF_CNT_0_VALUE		0x108C
#define MALI_PP_REG_MGMT_PERF_CNT_1_ENABLE		0x10A0
#define MALI_PP_REG_MGMT_PERF_CNT_1_SRC			0x10A4
#define MALI_PP_REG_MGMT_PERF_CNT_1_LIMIT		0x10A8
#define MALI_PP_REG_MGMT_PERF_CNT_1_VALUE		0x10AC
#define MALI_PP_REG_MGMT_DEBUG_0			0x10E0
#define MALI_PP_REG_MGMT_DEBUG_1			0x10E4
#define MALI_PP_REG_MGMT_BUILD_DATE			0x10F0

/* MALI_MMU_REGISTERS */
#define MALI_MMU_REG_PAGE_TABLE_ADDR    0x0000
#define MALI_MMU_REG_STATUS             0x0004
#define MALI_MMU_REG_CMD_REG            0x0008
#define MALI_MMU_REG_LAST_PFAULT_VADDR  0x000C
#define MALI_MMU_REG_INVALIDATE_PAGE    0x0010
#define MALI_MMU_REG_IRQ_RAW_STAT       0x0014
#define MALI_MMU_REG_IRQ_CLEAR          0x0018
#define MALI_MMU_REG_IRQ_MASK           0x001C
#define MALI_MMU_REG_IRQ_STATUS         0x0020
