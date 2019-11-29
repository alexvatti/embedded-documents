/**
* @file xparameters_ps.h
*
* This file contains the address definitions for the hard peripherals
* attached to the Cortex A9 core.
******************************************************************************/

#ifndef _XPARAMETERS_PSS_H_
#define _XPARAMETERS_PSS_H_

/***************************** Include Files *********************************/

#ifdef __cplusplus
	extern "C" {
#endif
/***************** Macros (Inline Functions) Definitions *********************/
/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

#define RTL_20	/* Palladium now */

/*
 * This block contains constant declarations for the peripherals
 * within the hardblock
 */

#define SHIKHARA_SCU_BASEADDR				0xD46F2000

	#define SHIKHARA_LPDDR2_CTRL_BASEADDR           0xD45A4000

/* Interrupt Controller bit assignments */

/* Shared Peripheral Interrupts (SPI) */

/* Private Peripheral Interrupts (PPI) */
#define SCU_GLOBAL_TMR_INT_ID			27
#define SCU_PRIVATE_TMR_INT_ID			29
#define SCU_WDT_INT_ID				30

/*	SCU register offsets */

#define SCU_CONTROL_OFFSET	 		0x000
#define SCU_CONFIG_OFFSET 			0x004
#define SCU_FILTER_START_OFFSET			0x040
#define SCU_FILTER_END_OFFSET 			0x044
#define SCU_NON_SECURE_ACCESS_OFFSET 		0x054

#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* protection macro */
