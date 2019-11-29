#ifndef __SHIKHARA_SPI_H__
#define __SHIKHARA_SPI_H__

#define SSP_CR0         0x000
#define SSP_CR1         0x004
#define SSP_DR          0x008
#define SSP_SR          0x00C
#define SSP_CPSR        0x010
#define SSP_IMSC        0x014
#define SSP_RIS         0x018
#define SSP_MIS         0x01C
#define SSP_ICR         0x020
#define SSP_DMACR       0x024
#define SSP_ITCR        0x080
#define SSP_ITIP        0x084
#define SSP_ITOP        0x088
#define SSP_TDR         0x08C

#define SSP_PID0        0xFE0
#define SSP_PID1        0xFE4
#define SSP_PID2        0xFE8
#define SSP_PID3        0xFEC

#define SSP_CID0        0xFF0
#define SSP_CID1        0xFF4
#define SSP_CID2        0xFF8
#define SSP_CID3        0xFFC

/* SSP Control Register 0  - SSP_CR0 */
#define SSP_CR0_SPO             (0x1 << 6)
#define SSP_CR0_SPH             (0x1 << 7)
#define SSP_CR0_8BIT_MODE       (0x07)
#define SSP_CR0_16BIT_MODE      (0x0F)
#define SSP_SCR_MAX             (0xFF)
#define SSP_SCR_SHFT            8

/* SSP Control Register 0  - SSP_CR1 */
#define SSP_CR1_MASK_SSE        (0x1 << 1)

#define SSP_CPSR_MAX            (0xFE)

/* SSP Status Register - SSP_SR */
#define SSP_SR_MASK_TFE         (0x1 << 0) /* Transmit FIFO empty */
#define SSP_SR_MASK_TNF         (0x1 << 1) /* Transmit FIFO not full */
#define SSP_SR_MASK_RNE         (0x1 << 2) /* Receive FIFO not empty */
#define SSP_SR_MASK_RFF         (0x1 << 3) /* Receive FIFO full */
#define SSP_SR_MASK_BSY         (0x1 << 4) /* Busy Flag */


/*SPI 0*/
#define RX_DMA_IF   21
#define TX_DMA_IF   20
#define TX_RX_DMA_RIF    0x300000       
#define RX_DMA_RIF       0x200000
#define TX_DMA_RIF       0x100000

#endif
