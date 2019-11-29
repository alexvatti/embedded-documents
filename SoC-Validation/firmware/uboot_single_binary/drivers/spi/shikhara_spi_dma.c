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
#include <asm/arch/dma/shikara-dma-pl330.h>
#include <asm/arch/dma/pl330.h>
//#include <asm/arch/dma/dma.h>

#ifdef SHIKHARA_SPI_USE_DMA
#include <asm/arch/dma_code.h>
#endif

//#define DEBUG

#ifdef SHIKHARA_SPI_USE_DMA

#define MC_CODE_LEN 	64 	// we use max 64 bytes for Micro code
#ifdef CONFIG_GNSS
#define TX_DMA_IF   27
#define RX_DMA_IF   28
#define TX_RX_DMA_RIF    0x18000000       
#define RX_DMA_RIF       0x10000000
#define TX_DMA_RIF       0x8000000
#undef DEBUG
#else
/*SPI 0*/
#define TX_DMA_IF   20
#define RX_DMA_IF   21
#define TX_RX_DMA_RIF    0x300000	
#define RX_DMA_RIF	 0x200000
#define TX_DMA_RIF	 0x100000
#endif

#endif


#ifdef DEBUG
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
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
		ret = spi_dma_start_api(slave,bitlen,dout,din);
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

typedef struct {
 	unsigned tx_id;  /* For Channel index */
	unsigned rx_id;
	enum shikara_dma_buffresult res;
	int size;
	struct shikara_dma_client cl;
	unsigned int *data;
} spi_dma;
spi_dma *spi1_dma;

void shikara_dma_cb_spi(enum dma_ch *chan, void *buf_id,
                        int size, enum shikara_dma_buffresult res)
{
        spi1_dma->res = res;
        spi1_dma->size = size;
        printf("SHIKARA DMA TEST SPI : callback to cmd_dma res= %d \n",res);
        //count= 1;

}

int spi_dma_start_api(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din )
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);
	unsigned short burst = 1;
	unsigned int  *tem ;
	int ret = 0; 
	unsigned int xfer_size = 16;


	spi1_dma->cl.name ="spi 2";
	spi1_dma->tx_id= DMACH_SPI2_TX; //GNSS
	spi1_dma->rx_id= DMACH_SPI2_RX; //GNSS

	if(dma_pl330_init(1)){
             printf("DMAC not initialised, exiting from DMA Test\n ");
	     return 0;
         }	

	if((u32)dout & 0x3){
		printf("Got miss aligned addr = %p\n", dout);
		return -1;
	}

	//DMAC channel request for SPI Tx
	ret = shikara_dma_request(spi1_dma->tx_id,&spi1_dma->cl,NULL);
	if (ret) {
		printf("Shikhara DMA: Error in acq %d\n", ret);
		return -1;
	}

	//DMAC channel request for SPI Rx
	ret = shikara_dma_request(spi1_dma->rx_id,&spi1_dma->cl,NULL);
	if (ret) {
		printf("Shikhara DMA: Error in acq %d\n", ret);
		return -1;
	}

	
	ret = shikara_dma_set_buffdone_fn(spi1_dma->tx_id, shikara_dma_cb_spi); //shikara_dma_cb_spi -call back fn name.
	ret = shikara_dma_set_buffdone_fn(spi1_dma->rx_id, shikara_dma_cb_spi); //shikara_dma_cb_spi -call back fn name.

	ret = shikara_dma_config(spi1_dma->tx_id, burst); 	/* 1 byte at a time*/
	if (ret) {
		printf("Shikhara DMA Test: Error in config(%d)\n", ret);
		shikara_dma_free(spi1_dma->tx_id, &spi1_dma->cl);
		return -1;
	}

	
	ret = shikara_dma_config(spi1_dma->rx_id, burst); 	/* 1 byte at a time*/
	if (ret) {
		printf("Shikhara DMA Test: Error in config(%d)\n", ret);
		shikara_dma_free(spi1_dma->rx_id, &spi1_dma->cl);
		return -1;
	}

	//*************configure dmac****************//
	
	#ifdef CONFIG_SHIKHARA_DDR_READY
	tem = (unsigned int *)malloc(bitlen/8);
	#else
	tem = (unsigned int *) DMA_TEM;
	#endif
	dbg("SA= %p and DA =%p and Len = %d\n",dout,din, bitlen);

	if(dout)
	{
		ret = shikara_dma_devconfig(spi1_dma->tx_id, SHIKARA_DMASRC_MEM, dout);
		if(ret)
		{
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
		}
		ret = shikara_dma_enqueue_ring(spi1_dma->tx_id, (void *)spi1_dma,
			ps->regs + SSP_DR, xfer_size,0);//xfer_size=slave->bus*8
	}
	else
	{
		ret = shikara_dma_devconfig(spi1_dma->tx_id, SHIKARA_DMASRC_MEM, tem);
		if(ret)
		{
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
		}
		ret = shikara_dma_enqueue_ring(spi1_dma->tx_id, (void *)spi1_dma,
			ps->regs + SSP_DR, xfer_size,0);
	}

	if(din)
	{
		ret = shikara_dma_devconfig(spi1_dma->rx_id, SHIKARA_DMASRC_HW, ps->regs + SSP_DR);
		if(ret)
		{
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
		}
		ret = shikara_dma_enqueue_ring(spi1_dma->rx_id, (void *)spi1_dma,
			din, xfer_size,0);
	}
	else
	{
		ret = shikara_dma_devconfig(spi1_dma->rx_id, SHIKARA_DMASRC_HW, ps->regs + SSP_DR);
		if(ret)
		{
			printf("SHIKARA DMA: unable to configure the DMAC channel\n");
			return -1;
		}
		ret = shikara_dma_enqueue_ring(spi1_dma->rx_id, (void *)spi1_dma,
			tem, xfer_size,0);
	}

		
	writel(0x3,ps->regs + SSP_DMACR); /*Enable both TX and RX for DMA*/

	if(ret){
		printf("SHIKARA DMA: unable to configure the DMAC channel\n");
		return -1;
	}

	printf("SHIKARA DMA API test with spi1 is successful\n" );
	return 0;


}
#endif
