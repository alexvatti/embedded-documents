/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __SHIKHARA_H__
#define __SHIKHARA_H__

#include <can.h>

/* PeliCAN mode address map */

/* reset and operating mode */
#define CAN_MOD          0       /* Mode register                   */
#define CAN_CMR          1       /* Command register                */
#define CAN_SR           2       /* Status register                 */
#define CAN_IR           3       /* Interrupt register              */
#define CAN_IER          4       /* Interrupt enable register       */
#define CAN_BTR0         6       /* Bus timing register 0           */
#define CAN_BTR1         7       /* Bus timing register 1           */
#define CAN_OCR          8       /* Output control register         */
#define CAN_ALC         11       /* Arbitration lost capture        */
#define CAN_ECC         12       /* Error code capture register     */
#define CAN_RXERR       14       /* Receive error counter           */
#define CAN_TXERR       15       /* Transmit error counter          */
#define CAN_CDR         31       /* Clock divider register          */

/* reset mode */
#define CAN_ACR0        16       /* Acceptance code register 0      */
#define CAN_ACR1        17       /* Acceptance code register 1      */
#define CAN_ACR2        18       /* Acceptance code register 2      */
#define CAN_ACR3        19       /* Acceptance code register 3      */
#define CAN_AMR0        20       /* Acceptance mask register 0      */
#define CAN_AMR1        21       /* Acceptance mask register 1      */
#define CAN_AMR2        22       /* Acceptance mask register 2      */
#define CAN_AMR3        23       /* Acceptance mask register 3      */

/* operating mode */
#define CAN_FIR         16       /* Frame information register      */
#define CAN_ID1         17       /* Identifier 1                    */
#define CAN_ID2         18       /* Identifier 2                    */
#define CAN_ID3         19       /* Identifier 3 (EFF only)         */
#define CAN_ID4         20       /* Identifier 4 (EFF only)         */

#define CAN_DATA_SFF(x) (19 + (x)) /* Data registers in case of standard
                                    * frame format; 0 <= x <= 7 */
#define CAN_DATA_EFF(x) (21 + (x)) /* Data registers in case of extended
                                    * frame format; 0 <= x <= 7 */
/* Mode register */
#define CAN_MOD_RM           (1<<0) /* Reset Mode                          */
#define CAN_MOD_LOM          (1<<1) /* Listen Only Mode                    */
#define CAN_MOD_STM          (1<<2) /* Self Test Mode                      */
#define CAN_MOD_AFM          (1<<3) /* Acceptance Filter Mode              */
#define CAN_MOD_SM           (1<<4) /* Sleep Mode                          */

/* Command register */
#define CAN_CMR_TR           (1<<0) /* Transmission request                */
#define CAN_CMR_AT           (1<<1) /* Abort Transmission                  */
#define CAN_CMR_RRB          (1<<2) /* Release Receive Buffer              */
#define CAN_CMR_CDO          (1<<3) /* Clear Data Overrun                  */
#define CAN_CMR_SRR          (1<<4) /* Self reception request              */

/* Status register */
#define CAN_SR_RBS           (1<<0) /* Receive Buffer Status               */
#define CAN_SR_DOS           (1<<1) /* Data Overrun Status                 */
#define CAN_SR_TBS           (1<<2) /* Transmit Buffer Status              */
#define CAN_SR_TCS           (1<<3) /* Transmission Complete Status        */
#define CAN_SR_RS            (1<<4) /* Receive Status                      */
#define CAN_SR_TS            (1<<5) /* Transmit Status                     */
#define CAN_SR_ES            (1<<6) /* Error Status                        */
#define CAN_SR_BS            (1<<7) /* Bus Status                          */

/* Interrupt register */
#define CAN_IR_RI            (1<<0) /* Receive Interrupt                   */
#define CAN_IR_TI            (1<<1) /* Transmit Interrupt                  */
#define CAN_IR_EI            (1<<2) /* Error Warning Interrupt             */
#define CAN_IR_DOI           (1<<3) /* Data Overrun Interrupt              */
#define CAN_IR_WUI           (1<<4) /* Wake-Up Interrupt                   */
#define CAN_IR_EPI           (1<<5) /* Error Passive Interrupt             */
#define CAN_IR_ALI           (1<<6) /* Arbitration Lost Interrupt          */
#define CAN_IR_BEI           (1<<7) /* Bus Error Interrupt                 */

/* Interrupt enable register */
#define CAN_IER_RIE          (1<<0) /* Receive Interrupt Enable            */
#define CAN_IER_TIE          (1<<1) /* Transmit Interrupt Enable           */
#define CAN_IER_EIE          (1<<2) /* Error Warning Interrupt Enable      */
#define CAN_IER_DOIE         (1<<3) /* Data Overrun Interrupt Enable       */
#define CAN_IER_WUIE         (1<<4) /* Wake-Up Interrupt Enable            */
#define CAN_IER_EPIE         (1<<5) /* Error Passive Interrupt Enable      */
#define CAN_IER_ALIE         (1<<6) /* Arbitration Lost Interrupt Enable   */
#define CAN_IER_BEIE         (1<<7) /* Bus Error Interrupt Enable          */

/* Output control register */
#define CAN_OCR_MODE_BIPHASE 0
#define CAN_OCR_MODE_TEST    1
#define CAN_OCR_MODE_NORMAL  2
#define CAN_OCR_MODE_CLOCK   3
#define CAN_OCR_TX0_INVERT   (1<<2)
#define CAN_OCR_TX0_PULLDOWN (1<<3)
#define CAN_OCR_TX0_PULLUP   (2<<3)
#define CAN_OCR_TX0_PUSHPULL (3<<3)
#define CAN_OCR_TX1_INVERT   (1<<5)
#define CAN_OCR_TX1_PULLDOWN (1<<6)
#define CAN_OCR_TX1_PULLUP   (2<<6)
#define CAN_OCR_TX1_PUSHPULL (3<<6)

/* Error code capture register */

/*
 * The segmentation field gives information about the location of
 * errors on the bus
 */
#define CAN_ECC_SEG_MASK     31     /* Segmentation field mask             */
#define CAN_ECC_DIR          (1<<5) /* Transfer direction                  */
#define CAN_ECC_ERR_BIT      (0<<6)
#define CAN_ECC_ERR_FORM     (1<<6)
#define CAN_ECC_ERR_STUFF    (2<<6)
#define CAN_ECC_ERR_MASK     (3<<6) /* Error code mask                     */

/* Frame information register */
#define CAN_FIR_DLC_MASK     15     /* Data length code mask               */
#define CAN_FIR_RTR          (1<<6) /* Remote transmission request         */
#define CAN_FIR_EFF          (1<<7) /* Extended frame format               */

/* Clock divider register */
#define CAN_CDR_CLK_OFF      (1<<3) /* Clock off (CLKOUT pin)              */
#define CAN_CDR_CBP          (1<<6) /* CAN input comparator bypass         */
#define CAN_CDR_CAN_MODE     (1<<7) /* CAN mode: 1 = PeliCAN               */

#define SHIKHARA_SIZE	     0x80

//void shikhara_register (struct can_dev *dev, unsigned long base);
int shikhara_can_initialize(int num);
int shikhara_init(struct can_dev *dev, unsigned int baud);
int shikhara_xmit(struct can_dev *dev, struct can_msg *msg);
int shikhara_recv(struct can_dev *dev, struct can_msg *msg);
int shikhara_status(struct can_dev *dev, int level);
#endif  /* __SHIKHARA_H__ */

