#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_irq.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/dma.h>
#include <asm/arch/shikhara_spi.h>
#ifdef SHIKHARA_SPI_USE_DMA
#include <asm/arch/dma_code.h>
#endif

//#define DEBUG_SPI

#define CONFIG_DEFAULT_SPEED 12000000
#ifdef SHIKHARA_SPI_USE_DMA

#define MC_CODE_LEN 	64 	// we use max 64 bytes for Micro code
#ifdef CONFIG_GNSS
#define TX_RX_DMA_RIF    0x18000000       
#define RX_DMA_RIF       0x10000000
#define TX_DMA_RIF       0x8000000
#undef DEBUG_SPI
#else
/*SPI 0*/
#define TX_RX_DMA_RIF    0x300000	
#define RX_DMA_RIF	 0x200000
#define TX_DMA_RIF	 0x100000
#endif

#endif

#ifdef CONFIG_SHIKHARA_ZYNQBASED_FPGA
 #ifndef CONFIG_SHIKHARA_DDR_READY
    #define DMA_TX_PROG_BASE_ADD    (CONFIG_V7FPGA_DDR_BASE+0x4000000)
    #define DMA_RX_PROG_BASE_ADD    (CONFIG_V7FPGA_DDR_BASE+0x400C000)
    #define DMA_TEM	            (CONFIG_V7FPGA_DDR_BASE+0xC000000)
 #endif
#endif

#ifdef DEBUG_SPI
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#define SHIKHARA_SPI_BASE2            0xD4560000
#define DMA_TX_PROG_BASE_ADD    (CONFIG_ASIC_SDRAM_BASE+0x4000000)
#define DMA_RX_PROG_BASE_ADD    (CONFIG_ASIC_SDRAM_BASE+0x400C000)
#define DMA_TEM                 (CONFIG_ASIC_SDRAM_BASE+0xC000000)


struct pl022_spi_slave {
	struct spi_slave slave;
	void *regs;
	unsigned int freq;
};

static inline struct pl022_spi_slave *to_pl022_spi(struct spi_slave *slave)
{
	return container_of(slave, struct pl022_spi_slave, slave);
}

/*
 * Following three functions should be provided by the
 * board support package.
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
	__attribute__((weak, alias("__spi_cs_is_valid")));
void spi_cs_activate(struct spi_slave *slave)
	__attribute__((weak, alias("__spi_cs_activate")));
void spi_cs_deactivate(struct spi_slave *slave)
	__attribute__((weak, alias("__spi_cs_deactivate")));

int __spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void __spi_cs_activate(struct spi_slave *slave)
{


#ifdef SHIKHARA_SPI_CS_MANUAL
	if(slave->bus==0)
		writel((readl(SHIKHARA_SYS_CTRL_BASEADDR +0x214)& (~(1<<2))),SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	if(slave->bus==1)
		writel((readl(SHIKHARA_SYS_CTRL_BASEADDR +0x214)& (~(1<<5))),SHIKHARA_SYS_CTRL_BASEADDR +0x214);
#endif 

}

void __spi_cs_deactivate(struct spi_slave *slave)
{
#ifdef SHIKHARA_SPI_CS_MANUAL
	if(slave->bus==0)
		writel((readl(SHIKHARA_SYS_CTRL_BASEADDR +0x214)|(1<<2)),SHIKHARA_SYS_CTRL_BASEADDR +0x214);
	if(slave->bus==1)
		writel((readl(SHIKHARA_SYS_CTRL_BASEADDR +0x214)|(1<<5)),SHIKHARA_SYS_CTRL_BASEADDR +0x214);
#endif 

}

void spi_init()
{
	/* do nothing */

}

/*
 * ARM PL022 exists in different 'flavors'.
 * This drivers currently support the standard variant (0x00041022), that has a
 * 16bit wide and 8 locations deep TX/RX FIFO.
 */
static int pl022_is_supported(struct pl022_spi_slave *ps)
{
	/* PL022 version is 0x00041022 */
	if ((readw(ps->regs + SSP_PID0) == 0x22) &&
			(readw(ps->regs + SSP_PID1) == 0x10) &&
			((readw(ps->regs + SSP_PID2) & 0xf) == 0x04) &&
			(readw(ps->regs + SSP_PID3) == 0x00))
		return 1;

	return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct pl022_spi_slave	*ps;
	u16 scr = 1, prescaler, cr0 = 0, cpsr = 0;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ps = malloc(sizeof(*ps));

	if (!ps)
		return NULL;

	ps->slave.bus = bus;
	ps->slave.cs = cs;
	ps->freq = max_hz;
        if(ps->freq==0)
        ps->freq = CONFIG_DEFAULT_SPEED;

	switch (bus) {
		case 0:
			ps->regs = (void *)SHIKHARA_SPI_BASE0;
			/*Reset SPI 0*/
			writel(0x300000,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);
	                udelay(10);
        	        writel(0x00,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);
			/*In Shikhara we can select CS line operation:  OE signal, FSSOUT signal, Manually operate CS line */
			writel(readl(SHIKHARA_SYS_CTRL_BASEADDR+0x214)| (1<<1),SHIKHARA_SYS_CTRL_BASEADDR+0x214);
			break;
		case 1:
			ps->regs = (void *)SHIKHARA_SPI_BASE1;
			/*Reset SPI 1*/
			writel(0xC00000,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);
	                udelay(10);
        	        writel(0x00,SHIKHARA_SYS_CTRL_BASEADDR+0x04C);
			writel(readl(SHIKHARA_SYS_CTRL_BASEADDR+0x214)| (1<<4),SHIKHARA_SYS_CTRL_BASEADDR+0x214);
			break;
#ifdef CONFIG_GNSS
		case 2:
			ps->regs = (void *)SHIKHARA_SPI_BASE2;
			/*Reset SPI 2*/
			writel(0x40,SHIKHARA_SYS_CTRL_BASEADDR+0x044);
	                udelay(10);
        	        writel(0x00,SHIKHARA_SYS_CTRL_BASEADDR+0x044);
			break;
#endif

		default:
			free(ps);
			return NULL;
	}

	/* Check the PL022 version */
	if (!pl022_is_supported(ps)) {
		free(ps);
		return NULL;
	}

	/* Set requested polarity and bit mode */
#ifdef CONFIG_GNSS
	if(bus==2)
		cr0 = SSP_CR0_16BIT_MODE;
	else
#endif
		cr0 = SSP_CR0_8BIT_MODE;
	cr0 |= (mode & SPI_CPHA) ? SSP_CR0_SPH : 0;
	cr0 |= (mode & SPI_CPOL) ? SSP_CR0_SPO : 0;

	writew(cr0, ps->regs + SSP_CR0);

	/* Program the SSPClk frequency */
	prescaler = SSPCLK / ps->freq;

	if (prescaler <= 0xFF)
		cpsr = prescaler;
	else {
		for (scr = 1; scr <= SSP_SCR_MAX; scr++) {
			if (!(prescaler % scr)) {
				cpsr = prescaler / scr;
				if (cpsr <= SSP_CPSR_MAX)
					break;
			}
		}

		if (scr > SSP_SCR_MAX) {
			scr = SSP_SCR_MAX;
			cpsr = prescaler / scr;
			cpsr &= SSP_CPSR_MAX;
		}
	}

	if (cpsr & 0x1)
		cpsr++;

	writew(cpsr, ps->regs + SSP_CPSR);
	cr0 = readw(ps->regs + SSP_CR0);
	writew(cr0 | (scr - 1) << SSP_SCR_SHFT, ps->regs + SSP_CR0);
	return &ps->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	free(ps);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	/* Enable the SPI hardware */
	writew(readw(ps->regs + SSP_CR1) | SSP_CR1_MASK_SSE,
			ps->regs + SSP_CR1);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	/* Disable the SPI hardware */
	writew(0x0, ps->regs + SSP_CR1);
}



int spi_xfer_gnss(struct spi_slave *slave, unsigned int len,
		const void *dout, void *din, unsigned long flags)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);
	u32		len_tx = 0, len_rx = 0;/*len=0;*/
	u32		ret = 0;
	u16		value;
	const u8	*txp = dout;
	u8		*rxp = din;
	const u16        *txp_gnss = dout;
	u16              *rxp_gnss = din;

	dbg("SPI_XFR: SA : %p DA: %p   Len = %d\n",dout, din, (bitlen/8));	

	/*
	 * TODO: The controller can do non-multiple-of-8 bit
	 * transfers, but this driver currently doesn't support it.
	 *
	 * It's also not clear how such transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface.
	 */

	while (len_tx < len) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_TNF) {
			value = (txp_gnss != NULL) ? *txp_gnss++ : 0;
			writew(value, ps->regs + SSP_DR);
			len_tx++;	
		}

		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			*rxp_gnss++ = value;
			len_rx++;

		}
	}

	while (len_rx < len_tx) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			//if (rxp)
				*rxp_gnss++ = value;
			len_rx++;
		}
	}
	return ret;
}	







int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);
	u32		len_tx = 0, len_rx = 0, len;
	u32		ret = 0;
	u16		value;
	const u8	*txp = dout;
	u8		*rxp = din;
	const u16        *txp_gnss = dout;
	u16              *rxp_gnss = din;

	dbg("SPI_XFR: SA : %p DA: %p   Len = %d\n",dout, din, (bitlen/8));	

	if (bitlen == 0)
		/* Finish any previously submitted transfers */
		goto out;

	/*
	 * TODO: The controller can do non-multiple-of-8 bit
	 * transfers, but this driver currently doesn't support it.
	 *
	 * It's also not clear how such transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface.
	 */

	if(slave->bus!=2){
                if (bitlen % 8) {
                        ret = -1;

                        /* Errors always terminate an ongoing transfer */
                        flags |= SPI_XFER_END;
                        goto out;
                }

                len = bitlen / 8;
        }
        else
	{
                if (bitlen % 16) {
                        ret = -1;
                        /* Errors always terminate an ongoing transfer */
                        flags |= SPI_XFER_END;
                        goto out;
                }

                len = bitlen / 16;
        }

#ifdef SHIKHARA_SPI_USE_DMA
	/*For now SPI DMA will work only for GNSS slave on SPI BUS 2*/
	if(((len)> 128) && (slave->bus==2)) {
		dbg("Calling DMA \n");
		ret = spi_dma_start(slave,bitlen,dout,din);
		return ret;
	}

#endif
	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	while (len_tx < len) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_TNF) {
			if(slave->bus==2)
			{
				value = (txp_gnss != NULL) ? *txp_gnss++ : 0;
			}
			else
			{
				value = (txp != NULL) ? *txp++ : 0;
			}
			writew(value, ps->regs + SSP_DR);
			len_tx++;
		}

		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			dbg("Read Value is %X\n",value);	
			if(slave->bus==2)
			{
			if (rxp_gnss)
				*rxp_gnss++ = value;

			}
			else
			{
			if (rxp)
				*rxp++ = value;

			}
			len_rx++;
		}
	}

	while (len_rx < len_tx) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			dbg("Read Value is %X\n",value);	
			if (rxp)
				*rxp++ = value;
			len_rx++;
		}
	}
	if(len_rx < len_tx)
		dbg("len_rx < len_tx , %d, %d \n", len_rx, len_tx);

out:
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);
	return ret;
}	

#ifdef SHIKHARA_SPI_USE_DMA

int spi_dma_start(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din )
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);
	u32            ret = 0,errors, i ,st,es;
	u8             lp_cnt=0;
	volatile unsigned int *add;	
	unsigned int  *mc_tx, *mc_rx, *tem ;
	
	/*MC buffers*/
	#ifdef CONFIG_SHIKHARA_DDR_READY
	mc_tx= (unsigned int *) malloc(MC_CODE_LEN) ; 
	mc_rx= (unsigned int *) malloc(MC_CODE_LEN) ; 
	tem = (unsigned int *)malloc(bitlen/8);
	#else
	mc_tx= (unsigned int *) DMA_TX_PROG_BASE_ADD ; 
 	mc_rx= (unsigned int *) DMA_RX_PROG_BASE_ADD ;
	tem = (unsigned int *) DMA_TEM;
	#endif
	dbg("SA= %p and DA =%p and Len = %d\n",dout,din, bitlen);
	/*Reset the DMA-0 */
	writel(0x3,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);
	udelay(10); 
	writel(0x0,SHIKHARA_SYS_CTRL_BASEADDR + 0x50);

	if((u32)dout & 0x3){
		printf("Got miss aligned addr = %p\n", dout);
		return -1;
	}
	/*Copy Micro code into Memory*/
	if(slave->bus ==2){ /* GNSS SPI uses 16- bit mode*/
		memcpy(mc_tx,tx_code_daf_8,40);
		// RX Code
		memcpy(mc_rx,rx_code_saf_8,40);
		lp_cnt = bitlen/64 ; // For 16 bit TX_FIFO	
		lp_cnt = lp_cnt -1; //loop cnt ends with 0
		if(bitlen % 64){
			printf("Code supports multiples of double word aligned transfers only.\n");
			return -1 ;
		}
	}
	else{
		memcpy(mc_tx,tx_code_daf_4,40);
		// RX Code
		memcpy(mc_rx,rx_code_saf_4,40);
		lp_cnt =bitlen/32; // For 8 bit TX_FIFO
		lp_cnt = lp_cnt -1; //loop cnt ends with 0
		if(bitlen % 32){
			printf("Code supports multiples of word aligned transfers only.\n");
			return -1 ;
		}
	}

	dbg("Loop Count is %d\n",lp_cnt);   	
	if(lp_cnt > 255){
		printf("Loop Count Exceeded Maximum supported loops\n");
		return -1;
	} 
	
	/*Change Source and Destination address for TX/RX FIFO in MC*/
	if(dout)	
		*(mc_tx+2) = (unsigned int )dout;
	else 
		*(mc_tx+2) = tem;

	*(mc_tx+3) = ((*(mc_tx+3) & 0xffff)     | (((unsigned int )(ps->regs + SSP_DR) & 0xffff ) << 16));
	*(mc_tx+4) = ((*(mc_tx+4) & 0xffff0000) | (((unsigned int )(ps->regs + SSP_DR) & 0xffff0000 ) >> 16));


	*(mc_rx+2) = (unsigned int )(ps->regs + SSP_DR);
	if(din){
		*(mc_rx+3) = ((*(mc_rx+3) & 0xffff)     | (((unsigned int )din & 0xffff ) << 16));
		*(mc_rx+4) = ((*(mc_rx+4) & 0xffff0000) | (((unsigned int )din & 0xffff0000 ) >> 16));
	}else{
		*(mc_rx+3) = ((*(mc_rx+3) & 0xffff)     | (((unsigned int )tem & 0xffff ) << 16));
		*(mc_rx+4) = ((*(mc_rx+4) & 0xffff0000) | (((unsigned int )tem & 0xffff0000 ) >> 16));
	}


	/*Change the loop count value in microcode*/
	*(mc_tx+5) = ((*(mc_tx+5) & 0xffff00ff) | (lp_cnt << 8));

	*(mc_rx+5) = ((*(mc_rx+5) & 0xffff00ff) | (lp_cnt << 8));



	/*Configure DMA for TX*/
	dbg("Start configure DMA for TX\n");
	/*Enable Both TX and RX Events*/
	writel(TX_RX_DMA_RIF,NSDMAC0_INTEN) ;
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 0*/
	writel(0xa20000,NSDMAC0_DBGINST0);
	writel(mc_tx,NSDMAC0_DBGINST1 );
	writel(0x00,NSDMAC0_DBGCMD);

	/*Configure DMA for RX */

	dbg("Start configure DMA for RX\n");
	while(readl(NSDMAC0_DBGSTATUS));
	/*Allocate ch 1*/
	writel(0x1a20100,NSDMAC0_DBGINST0);
	writel(mc_rx,NSDMAC0_DBGINST1 );
	writel(0x00,NSDMAC0_DBGCMD);

	/*Start SPI*/
	spi_cs_activate(slave);
	writel(0x3,ps->regs + SSP_DMACR); /*Enable both TX and RX for DMA*/
	
	/*Poll for Interrupts*/
	dbg("Start Poll for interrupts\n");
	errors = 0;
        while (1){
                st=readl(NSDMAC0_INTSTATUS);
                es = readl(NSDMAC0_ES)& TX_RX_DMA_RIF ;
                if(es == TX_RX_DMA_RIF){ // Poll for interrupts
                        dbg(" TX and RX Interrupts Generated Successfully\n");
                        break;
                }
		else if(es == TX_DMA_RIF ){
			dbg("TX Done.. Wait for RX Done..\n");
			writel(es,NSDMAC0_INTCLR);
		}
		else if (es == RX_DMA_RIF){
			dbg("RX Done.. Break the loop\n");
			writel(es,NSDMAC0_INTCLR);
			break;
		}
                errors +=1;
                if(errors == 1000000){
                        dbg("Errors too manay.. Interrupt not generated \n");
                        break;
                }
        }
        if(errors < 1000000){
                writel(es,NSDMAC0_INTCLR); // Clear interrupts  
        }

	dbg("Interrupt generated with status value= 0x%X  and error %d\n",st,errors);
	dbg(" Manager status 0x%X\n ",readl(NSDMAC0_MNGST));
	dbg("Faltmanager reg %X\t Falt Channer reg %X\n Falt status %X Falt channel0 status 0x%X\n",readl(NSDMAC0_FSRD),readl(NSDMAC0_FSRC),readl(NSDMAC0_FTRD),readl(NSDMAC0_FTCH0));
	dbg("Channel 0 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC0), readl(NSDMAC0_CHST0));
	dbg("Fault Channel 1 status 0x%X\n",readl(NSDMAC0_FTCH1)); 
	dbg("Channel 1 PC values 0x%X Status 0x%X\n",readl(NSDMAC0_CHPC1), readl(NSDMAC0_CHST1));
	dbg("Channel0  Loop Count  status 0x%d\n",readl(NSDMAC0_LPCH0)); 
	dbg("Channel1  Loop Count  status 0x%d\n",readl(NSDMAC0_LPCH1)); 
	dbg("SPI raw intr  status reg 0x%X\n",readw(ps->regs + SSP_RIS)); 
	dbg("SPI Status reg 0x%X\n",readw(ps->regs + SSP_SR)); 

	dbg("Channel0  SAR   0x%X\n",readl(NSDMAC0_SARCH0)); 
	dbg("Channel0  DAR   0x%X\n",readl(NSDMAC0_DARCH0)); 
	dbg("Channel1  SAR   0x%X\n",readl(NSDMAC0_SARCH1)); 
	dbg("Channel1  DAR   0x%X\n",readl(NSDMAC0_DARCH1)); 
	/*Disable CS line and */
	spi_cs_deactivate(slave);
	writel(0x0,ps->regs + SSP_DMACR); /*Disable both TX and RX for DMA*/
	dbg("DMA Done\n");
	return ret;
}

#endif
