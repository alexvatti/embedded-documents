/*
 *	Keyboard(KMI) Controller Registers on normal PCs.
 */
#include<asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_clock.h>
#ifndef __SHIKHARA_KMI_H__
#define __SHIKHARA_KMI_H__


#define KMI_CR                  ((volatile unsigned char *)(SHIKHARA_KMI_BASE + 0x000))   /* Controller command register (W) */
#define KMI_STAT                ((volatile unsigned char *)(SHIKHARA_KMI_BASE + 0x004))   /* Status register (R) */       
#define KMI_DATA                ((volatile unsigned char *)(SHIKHARA_KMI_BASE + 0x008))   /* Keyboard data register (R/W) */
#define KMI_CLKDIV              ((volatile unsigned char *)(SHIKHARA_KMI_BASE + 0x00C))   /*clock divisor offset value*/
#define KMI_IIR                 ((volatile unsigned char *)(SHIKHARA_KMI_BASE + 0x010))   /* interrupt identification register*/

/**********CONTROL BITS***********/
#define KMI_CR_FCL		0X01
#define KMI_CR_FDL		0X02
#define KMI_CR_EN		0x04	/* enabling the kmi from control register*/ 
#define KMI_CR_TXINT		0X08	/* TRANSMIT interrupt ENABLED*/
#define KMI_CR_RXINT		0X10	/* receiver interrupt ENABLED*/
#define KMI_CR_TYPE		0x20
#define KMI8MHZ			8000000
#define KMI_CLKDIV_DIVISOR 	( KMIREFCLK/KMI8MHZ - 1)    // 24Mhz->8MHz = div by 3


#define kbd_send(c,b)       (HW_REG((b), KMI_DATA) = (c))
#define HW_REG(base,offset) (*(volatile unsigned int *)((base) + (offset)))

/* KMI Status Register Bits */
#define KMI_STAT_TXEMPTY	0x40	/* Tx register is empty, ready to Xmit*/
#define KMI_STAT_TXBUSY		0x20	/* KMI currently sending data */
#define KMI_STAT_RXFULL		0X10	/* Receive register is full, ready to read */
#define KMI_STAT_RXBUSY		0X08	/* Receive register is receiving data */
#define KMI_STAT_RX_PARITY	0X04	/* parity chck for last received byte */
#define KMI_STAT_CLK       	0x02
#define KMI_STAT_DAT       	0x01

/* KMI Intr Bits */
#define KMI_IIR_TXINT	0X02	/*TX interrupt asserted*/
#define KMI_IIR_RXINT	0X01	/*Rx interrupt assetted*/

/*Macro's defined for TestApp of KMI  */
// KMI Misc. defines
#define KMI_DATA_RESET     0xFF    // Keyboard reset
#define KMI_DATA_ACK	   0xFA	   // Keyboard acknowledge
#define KMI_DATA_RTR       0xAA    // Keyboard response to reset
#define KMI_DATA_KEYUP     0xF0    // Key up control code
#define KMI_DATA_SHIFTL      18    // Shift key left
#define KMI_DATA_SHIFTR      89    // Shift key right
#define KMI_RESET_TIME     1000    // 1 second after reset
#define KMI_IDLEMAX        5000    // Maximum idle time
#define apERR_KMI_START   -1
#define apERR_NONE	   0
#define TRUE		   1
#define FALSE		   0
#define gic_kmi		  99
// Mouse Misc. defines
#define MOUSE_DATA_RESET    0xFF    // Mouse reset
#define MOUSE_DATA_ACK      0xFA    // Mouse acknowledge
#define MOUSE_DATA_ENABLE   0xF4    // Mouse enable
#define MOUSE_DATA_RTR      0xAA    // Mouse response to reset
#define MOUSE_RESET_TIME    2000    // 2 seconds after reset
#define MOUSE_ENABLE_TIME    100    // 100mS after enable
#define MOUSE_IDLEMAX       50000    // Maximum idle time
#define MOUSE_LEFT          0x01    // Left   button
#define MOUSE_RIGHT         0x02    // Right  button
#define MOUSE_MIDDLE        0x04    // Middle button
#define MOUSE_EVENT         0x08    // Mouse event occured
#define MOUSE_XSIGN         0x10    // X value sign bit
#define MOUSE_YSIGN         0x20    // Y value sign bit

/*  Function: apKBD_TEST(void)
   *   Purpose: Main entry for KMI test
   *
   * Arguments: None
   *   Returns:
   *        OK: apERR_NONE
   *     Error: apERR_KMI_START;
   */

int apKBD_TEST(void);
int apMOUSE_TEST(void);


/* Generic routines from shikhara_kmi_pc_keyb.c */
void kbd_write_output(unsigned char cmd);

unsigned char kbd_read_input(void);

unsigned char kbd_read_status(void);

int kmi_init(void);


void kmi_check1(void);
void kmi_check2(void);

int kbd_wait_for_input(void);
void kbd_send_data(unsigned char data);
#endif
