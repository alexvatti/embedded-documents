#include<asm/arch/shikhara_map.h>
/*
 * Generic RTC interface.
 */
#ifndef _SHIKHARA_RTC_H_
#define _SHIKHARA_RTC_H_

/*
 * Register definitions
 */
#define	RTC_DR		0x00	/* Data read register */
#define	RTC_MR		0x04	/* Match register */
#define	RTC_LR		0x08	/* Data load register */
#define	RTC_CR		0x0c	/* Control register */
#define	RTC_IMSC	0x10	/* Interrupt mask and set register */
#define	RTC_RIS		0x14	/* Raw interrupt status register */
#define	RTC_MIS		0x18	/* Masked interrupt status register */
#define	RTC_ICR		0x1c	/* Interrupt clear register */

#define RTC_CR_START	(1 << 0)


#endif	/* _SHIKHARA_RTC_H_ */

