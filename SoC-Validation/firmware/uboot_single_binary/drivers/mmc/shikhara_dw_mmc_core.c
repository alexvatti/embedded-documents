#include <common.h>
#include <malloc.h>
#include <mmc.h>
#include <dwmmc.h>
#include <asm/arch/shikhara_dwmmc.h>
#include <asm-generic/errno.h>
#include <asm/cache.h>
#define PAGE_SIZE 4096
//#define DEBUG_DWMMC
#ifdef DEBUG_DWMMC
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
#define SDMMC_GET_FCNT(x)               (((x)>>17) & 0x1FFF)
#define FIFO_DEPTH 	0x80
static int dwmci_wait_reset(struct dwmci_host *host, u32 value)
{
	unsigned long timeout = 1000;
	u32 ctrl;

	dwmci_writel(host, DWMCI_CTRL, value);

	while (timeout--) {
		ctrl = dwmci_readl(host, DWMCI_CTRL);
		if (!(ctrl & DWMCI_RESET_ALL))
			return 1;
		if (timeout == 0)
			break;
	}
	return 0;
}

static int dwmci_set_transfer_mode(struct dwmci_host *host,
		struct mmc_data *data)
{
	unsigned long mode;

	mode = DWMCI_CMD_DATA_EXP;
	if (data->flags & MMC_DATA_WRITE)
		mode |= DWMCI_CMD_RW;

	return mode;
}

static int dwmci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
		struct mmc_data *data)
{
	struct dwmci_host *host = (struct dwmci_host *)mmc->priv;
	int flags = 0, i;
	unsigned int timeout = 100000;
	int data_size;
	unsigned int *data_buf;
	u32 retry = 10000;
	u32 mask, ctrl;

	while (dwmci_readl(host, DWMCI_STATUS) & DWMCI_BUSY) {
		if (timeout == 0) {
			printf("Timeout on data busy\n");
			return TIMEOUT;
		}
		timeout--;
	}

	dwmci_writel(host, DWMCI_RINTSTS, DWMCI_INTMSK_ALL);
	if (data)
	{
		if(data->flags == MMC_DATA_READ)
                {	if((unsigned int)data->dest & 0x3){
			printf("unaligned destination address\n");
			return -1;
			}
		}
		else
		{
			if((unsigned int)data->src & 0x3){
			printf("unaligned source address\n");
			return -1;
			}
		}	
		
	}

	dwmci_writel(host, DWMCI_CMDARG, cmd->cmdarg);

	if (data) {
		flags = dwmci_set_transfer_mode(host, data);
	}

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
		return -1;

	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		flags |= DWMCI_CMD_ABORT_STOP;
	else
		flags |= DWMCI_CMD_PRV_DAT_WAIT;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		flags |= DWMCI_CMD_RESP_EXP;
		if (cmd->resp_type & MMC_RSP_136)
			flags |= DWMCI_CMD_RESP_LENGTH;
	}

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= DWMCI_CMD_CHECK_CRC;
	if(host->dev_index==0 || host->dev_index==2) {
        //printf("host index=%d\n", host->dev_index);
        flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG);
        } else {
        //printf("host index=%d\n", host->dev_index);

        flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG| 1<<16);
        }


	if(data){
		dbg("BLK_SZ=%d, BYTECNT=%d\n",data->blocksize,(data->blocksize * data->blocks));
		dwmci_wait_reset(host, DWMCI_CTRL_FIFO_RESET);
		dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
		dwmci_writel(host, DWMCI_BYTCNT, data->blocksize * data->blocks);
	}	
	debug("Sending CMD%d\n",cmd->cmdidx);

	dwmci_writel(host, DWMCI_CMD, flags);

	for (i = 0; i < retry; i++) {
		mask = dwmci_readl(host, DWMCI_RINTSTS);
		if (mask & DWMCI_INTMSK_CDONE) {
			if (!data)
				dwmci_writel(host, DWMCI_RINTSTS, mask);
			break;
		}
	}

	if (i == retry)
		return TIMEOUT;

	if (mask & DWMCI_INTMSK_RTO) {
		debug("Response Timeout..\n");
		return TIMEOUT;
	} else if (mask & DWMCI_INTMSK_RE) {
		debug("Response Error..\n");
		return -1;
	}


	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP3);
			cmd->response[1] = dwmci_readl(host, DWMCI_RESP2);
			cmd->response[2] = dwmci_readl(host, DWMCI_RESP1);
			cmd->response[3] = dwmci_readl(host, DWMCI_RESP0);
		} else {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP0);
		}
	}

	if (data) {
		unsigned int chk,pending=0;
		int avail=0;


		data_size = data->blocksize * data->blocks;
		timeout = 1000;
		if (data->flags == MMC_DATA_READ)
			data_buf = (unsigned int)data->dest;
		else
			data_buf = (unsigned int)data->src;
		
		pending = dwmci_readl(host,DWMCI_RINTSTS);
		dbg("intsts before TF = 0x%X \n",pending);
		
//		if (pending &(DWMCI_INTMSK_TXDR |DWMCI_INTMSK_RXDR))
			//clearing thresholds as we are not basing any checks on them
//			dwmci_writel(host,DWMCI_RINTSTS,(DWMCI_INTMSK_TXDR |DWMCI_INTMSK_RXDR));	
		if (pending &(DWMCI_DATA_ERR|DWMCI_DATA_TOUT))
		{
                       printf("DATA/TOUT error 0x%x\n",dwmci_readl(host,DWMCI_RINTSTS));
			dwmci_writel(host,DWMCI_RINTSTS,pending);
                       return -1;
                }
		
		if(data->flags == MMC_DATA_READ)
		{
			while(data_size > 0)
			{
			chk=dwmci_readl(host,DWMCI_RINTSTS);
				if(chk & DWMCI_INTMSK_RXDR)
				dwmci_writel(host,DWMCI_RINTSTS,DWMCI_INTMSK_RXDR);
			if(chk & (DWMCI_INTMSK_DTO|DWMCI_INTMSK_RXDR)){
                                        avail=SDMMC_GET_FCNT(dwmci_readl(host,DWMCI_STATUS));
				for(;avail>0;avail--){
					dbg("av=0x%X\t",avail);
					*data_buf=dwmci_readl(host,DWMCI_DATA);
					data_buf++;
					data_size-=4;
					}
			}
				if(dwmci_readl(host,DWMCI_RINTSTS)&(DWMCI_DATA_ERR|DWMCI_DATA_TOUT))
				{
					pending = dwmci_readl(host,DWMCI_RINTSTS);
                                	printf("RX:DATA/TOUT error 0x%x\n",pending);
		                        dwmci_writel(host,DWMCI_RINTSTS,pending);
                                	return -1;
                                }	
				dbg("data RX :intsts = 0x%X  : DATA SIZE =%d\n",dwmci_readl(host,DWMCI_RINTSTS),data_size);
			}
			do
			{
				if(dwmci_readl(host,DWMCI_RINTSTS)&DWMCI_INTMSK_DTO){
					dbg("DTO done\n");
					dbg("RX done\n");
					break;
				}
				udelay(200);
			} while(--timeout);
			if(timeout==0){
				printf("DTO failed\n");
			dbg("DTO timeout %d\n",timeout);
			pending = dwmci_readl(host,DWMCI_RINTSTS);
                       	printf("DTO 0x%x\n",pending);
		        dwmci_writel(host,DWMCI_RINTSTS,pending);
			return -1;
			}


		}
		else {
			while(data_size>0)
			{
			dbg("data_sz=%d\n",data_size);
			if(dwmci_readl(host,DWMCI_RINTSTS) & DWMCI_INTMSK_TXDR){
				dwmci_writel(host,DWMCI_RINTSTS,DWMCI_INTMSK_TXDR);

                                        avail=FIFO_DEPTH - SDMMC_GET_FCNT(dwmci_readl(host,DWMCI_STATUS));
				for(;avail>0;avail--){
					dbg("av=0x%X\t",avail);
					dwmci_writel(host,DWMCI_DATA, *data_buf);
					data_buf++;
					data_size-=4;
				}
			


			}
				if(dwmci_readl(host,DWMCI_RINTSTS)&(DWMCI_DATA_ERR|DWMCI_DATA_TOUT))
                                {
					pending = dwmci_readl(host,DWMCI_RINTSTS);
                                	printf("TX:DATA/TOUT error 0x%x\n",pending);
		                        dwmci_writel(host,DWMCI_RINTSTS,pending);
				dbg("TCMCNT=0x%X,TBBCNT=0x%X\n",dwmci_readl(host,DWMCI_TCMCNT),dwmci_readl(host,DWMCI_TBBCNT));	
                                        return -1;
                                }
				dbg("data TX :intsts = 0x%X \n",dwmci_readl(host,DWMCI_RINTSTS));
			}
			do
			{
				dbg("TCMCNT=0x%X,TBBCNT=0x%X\n",dwmci_readl(host,DWMCI_TCMCNT),dwmci_readl(host,DWMCI_TBBCNT));	

				if(dwmci_readl(host,DWMCI_RINTSTS)&DWMCI_INTMSK_DTO){
					dbg("DTO done\n");
					dbg("TX done\n");
					break;
				}
				udelay(200);
			}while(--timeout);
			if(timeout==0){
				printf("DTO failed\n");
			dbg("DTO timeout %d\n",timeout);
			pending = dwmci_readl(host,DWMCI_RINTSTS);
                        printf("DTO error 0x%x\n",pending);
		        dwmci_writel(host,DWMCI_RINTSTS,pending);
			return -1;
			}



		}

		dwmci_writel(host, DWMCI_RINTSTS, mask);

	}

	udelay(100);

	return 0;
}

static int dwmci_setup_bus(struct dwmci_host *host, u32 freq)
{
	u32 div, status;
	int timeout = 10000;
	unsigned long sclk;

	if (freq == host->clock)
		return 0;

	/*
	 * If host->mmc_clk didn't define,
	 * then assume that host->bus_hz is source clock value.
	 * host->bus_hz should be set from user.
	 */
	if (host->mmc_clk){
		sclk = host->mmc_clk(host->dev_index);
	}
	else if (host->bus_hz){
		sclk = host->bus_hz;
	}
	else {
		printf("Didn't get source clock value..\n");
		return -EINVAL;
	}

	for (div = 1; div < 255; div++) {
		if ((sclk / (2 * div)) <= freq) {
			break;
		}
	}


	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

	dwmci_writel(host, DWMCI_CLKDIV, div);
	dwmci_writel(host, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout == 0) {
			printf("TIMEOUT error!!\n");
			return -ETIMEDOUT;
		}
	} while ((status & DWMCI_CMD_START) && timeout--);

	dwmci_writel(host, DWMCI_CLKENA, DWMCI_CLKEN_ENABLE |
			DWMCI_CLKEN_LOW_PWR);

	dwmci_writel(host, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout == 0) {
			printf("TIMEOUT error!!\n");
			return -ETIMEDOUT;
		}
	} while ((status & DWMCI_CMD_START) && timeout--);

	host->clock = freq;

	return 0;
}

static void dwmci_set_ios(struct mmc *mmc)
{
	struct dwmci_host *host = (struct dwmci_host *)mmc->priv;
	u32 ctype;

	printf("Buswidth = %d, clock: %d\n",mmc->bus_width, mmc->clock);

	dwmci_setup_bus(host, mmc->clock);
	switch (mmc->bus_width) {
		case 8:
			ctype = DWMCI_CTYPE_8BIT;
			break;
		case 4:
			ctype = DWMCI_CTYPE_4BIT;
			break;
		default:
			ctype = DWMCI_CTYPE_1BIT;
			break;
	}

	dwmci_writel(host, DWMCI_CTYPE, ctype);

	if (host->clksel){
		host->clksel(host);
	}
}

static int dwmci_init(struct mmc *mmc)
{
	struct dwmci_host *host = (struct dwmci_host *)mmc->priv;
        u32 fifo_size, fifoth_val;

        printf("Dwmci_init Host base address = 0x%p \n",host->ioaddr);
        dwmci_writel(host, DWMCI_PWREN, 0x0000FFFF);

        if (!dwmci_wait_reset(host, DWMCI_RESET_ALL)) {
                debug("%s[%d] Fail-reset!!\n",__func__,__LINE__);
                return -1;
        }

        dwmci_writel(host, DWMCI_RINTSTS, 0xFFFFFFFF);
        dwmci_writel(host, DWMCI_INTMASK, 0xFFFFFFFF);
//      dwmci_writel(host, DWMCI_INTMASK, 0);

        dwmci_writel(host, DWMCI_TMOUT, 0xFFFFFFFF);

        fifo_size = 0x80;
        if (host->fifoth_val)
                fifoth_val = host->fifoth_val;
        else
                fifoth_val = (0x2 << 28) | ((fifo_size/2 -1) << 16) |
                        ((fifo_size/2) << 0);
        dwmci_writel(host, DWMCI_FIFOTH, fifoth_val);

        //dwmci_writel(host, DWMCI_CLKENA, 0);
        dwmci_writel(host, DWMCI_CLKENA, 0x0000FFFF);
        dwmci_writel(host, DWMCI_CLKSRC, 0);

        return 0;

}

int add_dwmci(struct dwmci_host *host, u32 max_clk, u32 min_clk, int index)
{
	struct mmc *mmc;
	int err = 0;

	mmc = malloc(sizeof(struct mmc));

	if (!mmc) {
		printf("mmc malloc fail!\n");
		return -1;
	}
	memset(mmc,0,sizeof(struct mmc));

	mmc->priv = host;
	host->mmc = mmc;

	sprintf(mmc->name, "%s", host->name);
	mmc->send_cmd = dwmci_send_cmd;
	mmc->set_ios = dwmci_set_ios;
	mmc->init = dwmci_init;
	mmc->f_min = min_clk;
	mmc->f_max = max_clk;
	//shikhara FPGA does not support 1.8V as of now
	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 ; 

	if (host->caps)
		mmc->host_caps = host->caps;
	else
		mmc->host_caps = 0;

	if (host->buswidth == 8) {
		mmc->host_caps |= MMC_MODE_8BIT;
		mmc->host_caps &= ~MMC_MODE_4BIT;
	} else {
		mmc->host_caps |= MMC_MODE_4BIT;
		mmc->host_caps &= ~MMC_MODE_8BIT;
	}
	mmc->host_caps |= MMC_MODE_HS | MMC_MODE_HC; // shikhara fpga host is limited to 50MHz hence cannot do 52MHz MMC 
	err = mmc_register(mmc);

	return err;
}

