#include <asm/io.h>
#include <common.h>
#include <sound.h>
#include <asm/arch/shikhara_i2s.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_jack.h>

/*

   Algorithm of I2S PLAYBACK CODE FLOW in U-BOOT
   --------- -- --  -----    ---- ---- -- -----

   ---------------
   IDLE
   ---------------
   |	
   |
   --------------------
   Enable Shikhara I2S
   IER[0] = 1  
   --------------------
   |
   ---------------------------
   Set the Transmit Resolution 
   in TFCR0 Register
   ---------------------------
   |
   ---------------------------------
   Set the Tranmit Fifo Configuration 
   to 1 such that when one byte is
   transmitted interrupt is obtained
   ---------------------------------
   |
   -----------------------------------
   Enable Transmitter Block ITER[0]=1
   -----------------------------------
   |
   -----------------------------------
   Enable Clock Generation CER[0]=1
   -----------------------------------
   |
   -----------------------------------
   Fill Tx Fifo's by writing data into
   LTHR and RTHR register
   -----------------------------------

 */

static inline void i2s_write_reg(int io_base, int reg, u32 val)
{
	writel(val, io_base + reg);
}

static inline u32 i2s_read_reg(int io_base, int reg)
{
	return readl(io_base + reg);
}

static void shikhara_i2s_txb_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, ITER , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , ITER , DISABLE ) ;
	}

}

static void shikhara_i2s_rxb_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, IRER , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , IRER , DISABLE ) ;
	}

}


/*

   Enables/Disables the SHIKHARA I2S MODULE

ON :- Enable/Disable Shikhara I2S.

 */

static void shikhara_i2s_pwr_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, IER , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , IER , DISABLE ) ;	
	}

}

/*

   Enables/Disables the SHIKHARA I2S Transmitter Module

ON :- Enable/Disable Shikhara I2S.

 */

static void shikhara_i2s_tx_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, TER0 , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , TER0 , DISABLE ) ;
	}

}
/*

   Enables/Disables the SHIKHARA I2S Receiver Channels

ON :- Enable/Disable Shikhara I2S.

 */

static void shikhara_i2s_rx_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, RER0 , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , RER0 , DISABLE ) ;
	}

}

/*
   Enables/Disables the SHIKHARA I2S CLOCK Module 

ON :- Enable/Disable Shikhara I2S.

 */
static void shikhara_i2s_clk_ctrl(int ON)
{
	if(ON)
	{
		i2s_write_reg( SHIKHARA_I2S_BASE, CER , ENABLE) ;
	}
	else
	{
		i2s_write_reg(SHIKHARA_I2S_BASE , CER , DISABLE ) ;
	}

}

static int set_ws_len(int ws_len)
{

	switch( ws_len){
		case 12:
			i2s_write_reg(SHIKHARA_I2S_BASE, TCR0, 0x1);
			break;
		case 16:
			i2s_write_reg(SHIKHARA_I2S_BASE, TCR0, 0x2);
			break;
		case 20:
			i2s_write_reg(SHIKHARA_I2S_BASE, TCR0, 0x3);
			break;
		case 24:
			i2s_write_reg(SHIKHARA_I2S_BASE, TCR0, 0x4);
			break;
		case 32:
			i2s_write_reg(SHIKHARA_I2S_BASE, TCR0, 0x5);
			break;

		default:
			printf("Sample Resolution Not Supported \n");
			return -1;
	}
	return 0;
}

static int set_rx_ws_len(int ws_len)
{

	switch( ws_len){
		case 12:
			i2s_write_reg(SHIKHARA_I2S_BASE, RCR0, 0x1);
			break;
		case 16:
			i2s_write_reg(SHIKHARA_I2S_BASE, RCR0, 0x2);
			break;
		case 20:
			i2s_write_reg(SHIKHARA_I2S_BASE, RCR0, 0x3);
			break;
		case 24:
			i2s_write_reg(SHIKHARA_I2S_BASE, RCR0, 0x4);
			break;
		case 32:
			i2s_write_reg(SHIKHARA_I2S_BASE, RCR0, 0x5);
			break;

		default:
			printf("Sample Resolution Not Supported \n");
			return -1;
	}
	return 0;
}


int shikhara_i2s_tx_init(void)
{
	int ret ; 
	int ws_len;

#ifdef CONFIG_SHIKHARA_JACK	
	/* JACK initialisation */
	ret = shikhara_jack_init();	
	if (ret){
		printf("JACK Initialisation failed\n");
		return -1;
	}
#endif	
	ws_len = sound_params.bps;

	/* Disable Tx channel 0 */
	shikhara_i2s_tx_ctrl(DISABLE);
	ret = set_ws_len(ws_len);
	if( ret != 0){
		printf(" ERROR setting resolution \n");
		return -1 ;
	}

	/* Disable Rx channel 0 */
	shikhara_i2s_rx_ctrl(DISABLE);
	ret = set_rx_ws_len(ws_len);

	if( ret != 0){
		printf(" ERROR setting resolution \n");
		return -1 ;
	}

	i2s_write_reg(SHIKHARA_I2S_BASE , TFCR0, 0x04);  	//Setting the threshold level in FIFO for two samples(L&R), means as soon as \
	75% of FIFO is empty it will trigger interrupt
		/* Rx FIFO Threshold level */
		i2s_write_reg(SHIKHARA_I2S_BASE, RFCR0, 0x00);

	/* Unmasking Interrupt sources*/
	i2s_write_reg(SHIKHARA_I2S_BASE, IMR0, 0x00); 

	shikhara_i2s_tx_ctrl(ENABLE);

	shikhara_i2s_rx_ctrl(ENABLE);

	/* Clock  Configuration Register No Gating*/
	i2s_write_reg(SHIKHARA_I2S_BASE , CCR, 0x00);  

	return 0 ;
}



int shikhara_i2s_transfer_tx_data(unsigned int *data, unsigned long data_size,int16_t Align)
{
	int i;
	int start;
	int ret;
	unsigned short left_data , right_data ;	

#ifdef CONFIG_SHIKHARA_JACK 
	ret = shikhara_jack_detect();
	if(ret){
		printf("Please insert Headphone into Jack Connector\n");
		return -1;
	}
#endif

	/* fill the tx buffer before starting the tx transmit and clock */ 
	for(i=0;i<FIFO_LENGTH;i++){
		if(Align == MONO){
			left_data = (*data) & 0xffff;
			right_data = left_data;
			i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
			i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
			left_data =( *data >> 16 ) & 0xffff; 
			right_data = left_data;
			i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
			i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
		}
		else{
			left_data = (*data) & 0xffff;
			right_data = ( *data >> 16 ) & 0xffff;
			i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
			i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
		}
		data++;
		data_size = data_size - 4;

	}
	printf("PLAYBACK STARTED\n");
	printf("\n"); 
	printf("\n");
	printf("\n");
	printf("\n");
	shikhara_i2s_pwr_ctrl(ENABLE);		/* Enable the I2S block */	
	shikhara_i2s_txb_ctrl(ENABLE);		/* Enable the Transmitter block */
	shikhara_i2s_clk_ctrl(ENABLE);		/* Enable the clocks */

	/* As of Know we will be supporting the ONLY 16-bit resolution data transmission for I2S*/


	while (data_size > 0) {
		start = get_timer(0);
		if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {		// Repeating the samples inorder to play mono as stereo
			if(Align == MONO){
				left_data = (*data) & 0xffff;
				right_data = left_data;
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
				left_data =( *data >> 16 ) & 0xffff;
				right_data = left_data;
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
				data++;
				data_size=data_size-4;
			} else{	 							// Filling two samples(L/R) since threshold level of FIFO is configured accordingly
				left_data  = (*data) & 0xffff;
				right_data = (*data >> 16 ) & 0xffff;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
				data++;
				left_data  = (*data) & 0xffff;
				right_data = (*data >> 16 ) & 0xffff;
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
				data++;
				data_size = data_size -8;

			}
			if (ctrlc ()){
				printf("Playback Interrupted\n");
				return 0;
			}
			if(CON_TXFO_OVRN & (readl(SHIKHARA_I2S_BASE +ISR0))){
				printf("TxFIFO Overrun Error\n");
			}
		} else {
			if (get_timer(start) > TIMEOUT_I2S_TX) {
				shikhara_i2s_txb_ctrl(DISABLE);
				if(I2S_MASTER)
					shikhara_i2s_clk_ctrl(DISABLE);
				shikhara_i2s_tx_ctrl(DISABLE);
				printf("ERROR: I2S Transfer Timeout\n");
				return -1;
			}
		}
	}

	printf("PLAYBACK COMPLETED\n");

	shikhara_i2s_txb_ctrl(DISABLE);
	if(I2S_MASTER)
		shikhara_i2s_clk_ctrl(DISABLE);
	shikhara_i2s_pwr_ctrl(DISABLE);	

	return 0;
}

int shikhara_i2s_record_rx_data(unsigned short *data, unsigned int size)
{
	int start;

	shikhara_i2s_pwr_ctrl(ENABLE); /* Enable the I2S block */ 
	shikhara_i2s_rxb_ctrl(ENABLE); /* Enable the Receiver block */ 
	shikhara_i2s_clk_ctrl(ENABLE); /* Enable the clocks */	

	while(size > 0){
		start = get_timer(0);
		/* Rx FIFO DATA */
		if(CON_RXDA_AVAIL & readl((SHIKHARA_I2S_BASE +ISR0))){
			*data = readl(SHIKHARA_I2S_BASE + LRBR0_LTHR0);
			data++;
			*data = readl(SHIKHARA_I2S_BASE + RRBR0_RTHR0); 
			data++;
			size -= 4;
		}else {
			if (get_timer(start) > TIMEOUT_I2S_TX) {
				shikhara_i2s_rxb_ctrl(DISABLE);
				if(I2S_MASTER)
					shikhara_i2s_clk_ctrl(DISABLE);
				shikhara_i2s_rx_ctrl(DISABLE);
				printf("ERROR: I2S Receive Timeout\n");
				return -1;
			}
		}	
	}

	shikhara_i2s_rxb_ctrl(DISABLE);
	if(I2S_MASTER)
		shikhara_i2s_clk_ctrl(DISABLE);
	shikhara_i2s_rx_ctrl(DISABLE);

	return 0;
}
int shikhara_i2s_beep(unsigned long duration, unsigned int freq,unsigned int channel)
{
	long i,j,start;
	unsigned short left_data,right_data ;
	unsigned long data;

	shikhara_i2s_pwr_ctrl(ENABLE);
	shikhara_i2s_txb_ctrl(ENABLE);
	shikhara_i2s_clk_ctrl(ENABLE);

	/*
	 * Generate sqarewave. Start with high level, duty cycle 1:1.
	 */
	for(j = 0; j < ((duration * freq) / 1000); j++) {
		for(i = 0; i < (44100/(freq*2)); i ++) {
			data = 0x7FFF;
			start = get_timer(0);
			if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {
				left_data  = (data) & 0xffff ;
				right_data = (data >> 16 ) & 0xffff ;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
			} else {
				if (get_timer(start) > TIMEOUT_I2S_TX) {
					shikhara_i2s_txb_ctrl(DISABLE);
					if(I2S_MASTER)	 
						shikhara_i2s_clk_ctrl(DISABLE);
					shikhara_i2s_pwr_ctrl(DISABLE);
					printf("ERROR: I2S Transfer Timeout\n");
					return -1;
				}
			}
		}
		for(i = 0; i < (44100/(freq*2)); i ++) {
			data = 0x8000;
			start = get_timer(0);
			if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {
				left_data  = (data) & 0xffff ;
				right_data = (data >> 16 ) & 0xffff ;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);

			} else {
				if (get_timer(start) > TIMEOUT_I2S_TX) {
					shikhara_i2s_txb_ctrl(DISABLE);
					if(I2S_MASTER)
						shikhara_i2s_clk_ctrl(DISABLE);
					shikhara_i2s_pwr_ctrl(DISABLE);
					printf("ERROR: I2S Transfer Timeout\n");
					return -1;
				}
			}
		}
	}
	shikhara_i2s_txb_ctrl(DISABLE);
	if(I2S_MASTER)
		shikhara_i2s_clk_ctrl(DISABLE);
	shikhara_i2s_pwr_ctrl(DISABLE);
	return 0 ;
}


int shikhara_i2s_sawtooth(unsigned long duration, unsigned int freq,unsigned int channel)
{
	long i,j,start;
	unsigned short left_data, right_data ;
	unsigned long data;

	shikhara_i2s_pwr_ctrl(ENABLE);
	shikhara_i2s_txb_ctrl(ENABLE);
	shikhara_i2s_clk_ctrl(ENABLE);


	for(j = 0; j < ((duration * freq) / 1000); j++) {
		for(i = 0; i <= 0x7FFF; i += (0x7FFF/(44100/(freq*4)))) {
			data = (i & 0xFFFF);
			start = get_timer(0);
			if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {
				left_data  = (data) & 0xffff ;
				right_data = (data >> 16 ) & 0xffff ;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
			} else {
				if (get_timer(start) > TIMEOUT_I2S_TX) {
					shikhara_i2s_txb_ctrl(DISABLE);
					if(I2S_MASTER)
						shikhara_i2s_clk_ctrl(DISABLE);
					shikhara_i2s_pwr_ctrl(DISABLE);
					printf("ERROR: I2S Transfer Timeout\n");
					return -1;
				}
			}

		}
		for(i = 0x7FFF; i >= -0x7FFF; i -= (0xFFFF/(44100/(freq*2)))) {
			data = (i & 0xFFFF);
			/* data format: right data left data) */
			start = get_timer(0);
			if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {
				left_data  = (data) & 0xffff ;
				right_data = (data >> 16 ) & 0xffff ;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
			} else {
				if (get_timer(start) > TIMEOUT_I2S_TX) {
					shikhara_i2s_txb_ctrl(DISABLE);
					if(I2S_MASTER)	 
						shikhara_i2s_clk_ctrl(DISABLE);
					shikhara_i2s_pwr_ctrl(DISABLE);
					printf("ERROR: I2S Transfer Timeout\n");
					return -1;
				}
			}

		}
		for(i = -0x7FFF; i <= 0; i += (0x7FFF/(44100/(freq*4)))) {
			data = (i & 0xFFFF);
			start = get_timer(0);
			if ((CON_TXFIFO_EMPTY & (readl(SHIKHARA_I2S_BASE +ISR0)))) {
				left_data  = (data) & 0xffff ;
				right_data = (data >> 16 ) & 0xffff ;	
				i2s_write_reg(SHIKHARA_I2S_BASE,LRBR0_LTHR0,left_data);
				i2s_write_reg(SHIKHARA_I2S_BASE,RRBR0_RTHR0,right_data);
			} else {
				if (get_timer(start) > TIMEOUT_I2S_TX) {
					shikhara_i2s_txb_ctrl(DISABLE);
					if(I2S_MASTER)	 
						shikhara_i2s_clk_ctrl(DISABLE);
					shikhara_i2s_pwr_ctrl(DISABLE);
					printf("ERROR: I2S Transfer Timeout\n");
					return -1;
				}
			}

		}
	}
	shikhara_i2s_txb_ctrl(DISABLE);
	if(I2S_MASTER)
		shikhara_i2s_clk_ctrl(DISABLE);
	shikhara_i2s_pwr_ctrl(DISABLE);
	return 0 ;
}
