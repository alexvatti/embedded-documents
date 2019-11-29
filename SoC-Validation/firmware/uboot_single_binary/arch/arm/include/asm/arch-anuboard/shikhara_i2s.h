
#ifndef _SHIKHARA__I2S_H_
#define __SHIKHARA_I2S_H_

#define I2S_MASTER 1	/*Since Shikhara I2S can act only as a Master device*/
/* common register for all channel */
#define IER        0x000	/* Shikhara I2S Enable/Disable Register 1: Enable 0: Disable */
#define IRER       0x004	/* Shikhara I2S Recieve Block Enable/Disable Register 1: Enable 0: Disable */
#define ITER       0x008	/* Shikhara I2S Transmitter Block Enable/Disable Register 1: Enable 0: Disable */
#define CER        0x00C	/* Shikhara I2S Clock Enable/Disable Register 1: Enable 0: Disable */
#define CCR        0x010	/* Shikhara I2S Clock Configuration Register 2:0 Gating SCLK 4:3 WSS(WORD SELECT)*/
#define RXFFR      0x014	/* Shikhara I2S Recieve Block FIFO Reset Register 1:FLUSH RX fifo's*/
#define TXFFR      0x018	/* Shikhara I2S Transmit Block FIFO Reset Regisyer 1:FLUSH TX fifo's */

/* I2STxRxRegisters for channel 0 */
#define LRBR0_LTHR0     0x020   /* Shikhara I2S Left Channel Recieve/Transmit Holding Register */ 
#define RRBR0_RTHR0     0x024	/* Shikhara I2S Right Channel Recieve/Transmit Holding Register */
#define RER0         	0x028   /* Shikhara I2S Recieve Enable Resgister0 1: Enable 0: Disable */
#define TER0        	0x02C   /* Shikhara I2S Transmit Enable Register0 1: Enable 0: Disable */
#define RCR0        	0x030   /* Shikhara I2S Recieve Configuration Register 12bits/16/24/32 bit sample size */
#define TCR0        	0x034   /* Shikhara I2S Transmit Configration Register 12bits/16/23/32 biys sample size */
#define ISR0        	0x038   /* Shikhara I2S Interrupt Status Register0 */
#define IMR0        	0x03C   /* Shikhara I2S Interrupt Mask Register0 */
#define ROR0        	0x040   /* Shikhara I2S Recieve OverRun Register0 */
#define TOR0        	0x044   /* Shikhara I2S Transmit OverRun Register0*/ 
#define RFCR0        	0x048   /* Shikhara I2S Recieve Fifo Configuration Register0 */
#define TFCR0        	0x04C   /* Shikhara I2S Transmit Fifo Configuratuin Register0 */
#define RFF0        	0x050   /* Shikhara I2S Recieve fifo flush Register0 */
#define TFF0        	0x054   /* Shikhara I2S Transmit Fifo Flush Register0*/

/* Deriver Specfic Macro's*/

#define ENABLE 		   1
#define DISABLE 	   0
#define FIFO_LENGTH 	   8
#define CON_TXFIFO_EMPTY 0x10
#define CON_TXFO_OVRN	 0x20
#define CON_RXDA_AVAIL	 0x1
#define CON_RXFO_OVRN	 0x2
#define TIMEOUT_I2S_TX     100     /* i2s transfer timeout */
#define I2S_FIFO_DEPTH_GLOBAL	16

/*CODEC INFO*/
/*
 * DAI hardware audio formats.
 *
 * Describes the physical PCM data formating and clocking. Add new formats
 * to the end.
 */
#define SND_SOC_DAIFMT_I2S              1 /* I2S mode */
#define SND_SOC_DAIFMT_RIGHT_J          2 /* Right Justified mode */
#define SND_SOC_DAIFMT_LEFT_J           3 /* Left Justified mode */
#define SND_SOC_DAIFMT_DSP_A            4 /* L data MSB after FRM LRC */
#define SND_SOC_DAIFMT_DSP_B            5 /* L data MSB during FRM LRC */
#define SND_SOC_DAIFMT_AC97             6 /* AC97 */
#define SND_SOC_DAIFMT_PDM              7 /* Pulse density modulation */

/* left and right justified also known as MSB and LSB respectively */
#define SND_SOC_DAIFMT_MSB              SND_SOC_DAIFMT_LEFT_J
#define SND_SOC_DAIFMT_LSB              SND_SOC_DAIFMT_RIGHT_J

/*
 * DAI hardware signal inversions.
 *
 * Specifies whether the DAI can also support inverted clocks for the specified
 * format.
 */
#define SND_SOC_DAIFMT_NB_NF    (1 << 8) /* normal bit clock + frame */
#define SND_SOC_DAIFMT_NB_IF    (2 << 8) /* normal BCLK + inv FRM */
#define SND_SOC_DAIFMT_IB_NF    (3 << 8) /* invert BCLK + nor FRM */
#define SND_SOC_DAIFMT_IB_IF    (4 << 8) /* invert BCLK + FRM */

/*
 * DAI hardware clock masters.
 *
 * This is wrt the codec, the inverse is true for the interface
 * i.e. if the codec is clk and FRM master then the interface is
 * clk and frame slave.
 */
#define SND_SOC_DAIFMT_CBM_CFM  (1 << 12) /* codec clk & FRM master */
#define SND_SOC_DAIFMT_CBS_CFM  (2 << 12) /* codec clk slave & FRM master */
#define SND_SOC_DAIFMT_CBM_CFS  (3 << 12) /* codec clk master & frame slave */
#define SND_SOC_DAIFMT_CBS_CFS  (4 << 12) /* codec clk & FRM slave */

#define SND_SOC_DAIFMT_FORMAT_MASK      0x000f
#define SND_SOC_DAIFMT_CLOCK_MASK       0x00f0
#define SND_SOC_DAIFMT_INV_MASK         0x0f00
#define SND_SOC_DAIFMT_MASTER_MASK      0xf000

/*
 * Master Clock Directions
 */
#define SND_SOC_CLOCK_IN                0
#define SND_SOC_CLOCK_OUT               1

/* I2S Tx Control */
#define I2S_TX_ON       1
#define I2S_TX_OFF      0


int shikhara_i2s_tx_init(void);
int shikhara_i2s_transfer_tx_data( unsigned int *data, unsigned long data_size,int16_t align);
int shikhara_i2s_record_rx_data( unsigned short *data, unsigned int data_size);
int shikhara_i2s_beep(unsigned long duration, unsigned int freq,unsigned int channel);
int shikhara_i2s_sawtooth(unsigned long duration, unsigned int freq,unsigned int channel);
#endif /* _SHIKHARA__I2S_H_ */
