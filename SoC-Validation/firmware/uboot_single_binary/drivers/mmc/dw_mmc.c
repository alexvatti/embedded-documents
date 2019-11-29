#include <common.h>
#include <malloc.h>
#include <mmc.h>
#include <asm/arch/shikhara_dwmmc.h>
#include <dwmmc.h>
#include <asm-generic/errno.h>
#include <asm/cache.h>
#define PAGE_SIZE 4096
#define DEBUG_DWMMC
#ifdef DEBUG_DWMMC
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif
#define SDMMC_GET_FCNT(x)               (((x)>>17) & 0x1FFF)
#ifdef CONFIG_SHIKHARA_MMC
#define NOT_AVAILABE_IN_SHIKHARA /*need this as IDMAC is not configured in Shikhara*/
#endif

void dwmci_dumpregs_err(struct dwmci_host *host)
{
	dbg( "dwmci: ============== REGISTER DUMP ==============\n");
	dbg( "dwmci: DWMCI_CTRL:      0x%08x\n",
		dwmci_readl(host, DWMCI_CTRL));
	dbg( "dwmci: DWMCI_PWREN:     0x%08x\n",
		dwmci_readl(host, DWMCI_PWREN));
	dbg( "dwmci: DWMCI_CLKDIV:    0x%08x\n",
		dwmci_readl(host, DWMCI_CLKDIV));
	dbg( "dwmci: DWMCI_CLKSRC:    0x%08x\n",
		dwmci_readl(host, DWMCI_CLKSRC));
	dbg( "dwmci: DWMCI_CLKENA:    0x%08x\n",
		dwmci_readl(host, DWMCI_CLKENA));
	dbg( "dwmci: DWMCI_TMOUT:     0x%08x\n",
		dwmci_readl(host, DWMCI_TMOUT));
	dbg( "dwmci: DWMCI_CTYPE:     0x%08x\n",
		dwmci_readl(host, DWMCI_CTYPE));
	dbg( "dwmci: DWMCI_BLKSIZ:    0x%08x\n",
		dwmci_readl(host, DWMCI_BLKSIZ));
	dbg( "dwmci: DWMCI_BYTCNT:    0x%08x\n",
		dwmci_readl(host, DWMCI_BYTCNT));
	dbg( "dwmci: DWMCI_INTMASK:    0x%08x\n",
		dwmci_readl(host, DWMCI_INTMASK));
	dbg( "dwmci: DWMCI_CMDARG:    0x%08x\n",
		dwmci_readl(host, DWMCI_CMDARG));
	dbg( "dwmci: DWMCI_CMD:       0x%08x\n",
		dwmci_readl(host, DWMCI_CMD));
	dbg( "dwmci: DWMCI_MINTSTS:   0x%08x\n",
		dwmci_readl(host, DWMCI_MINTSTS));
	dbg( "dwmci: DWMCI_RINTSTS:   0x%08x\n",
		dwmci_readl(host, DWMCI_RINTSTS));
	dbg( "dwmci: DWMCI_STATUS:    0x%08x\n",
		dwmci_readl(host, DWMCI_STATUS));
	dbg( "dwmci: DWMCI_FIFOTH:    0x%08x\n",
		dwmci_readl(host, DWMCI_FIFOTH));
	dbg( "dwmci: DWMCI_CDETECT:   0x%08x\n",
		dwmci_readl(host, DWMCI_CDETECT));
	dbg( "dwmci: DWMCI_WRTPRT:    0x%08x\n",
		dwmci_readl(host, DWMCI_WRTPRT));
	dbg( "dwmci: DWMCI_GPIO:      0x%08x\n",
		dwmci_readl(host, DWMCI_GPIO));
	dbg( "dwmci: DWMCI_TCBCNT:    0x%08x\n",
		dwmci_readl(host, DWMCI_TCMCNT));
	dbg( "dwmci: DWMCI_TBBCNT:    0x%08x\n",
		dwmci_readl(host, DWMCI_TBBCNT));
	dbg( "dwmci: DWMCI_DEBNCE:    0x%08x\n",
		dwmci_readl(host, DWMCI_DEBNCE));
	dbg( "dwmci: DWMCI_USRID:     0x%08x\n",
		dwmci_readl(host, DWMCI_USRID));
	dbg( "dwmci: DWMCI_VERID:     0x%08x\n",
		dwmci_readl(host, DWMCI_VERID));
	dbg( "dwmci: DWMCI_HCON:      0x%08x\n",
		dwmci_readl(host, DWMCI_HCON));
	dbg( "dwmci: DWMCI_UHS_REG:   0x%08x\n",
		dwmci_readl(host, DWMCI_UHS_REG));
#ifdef NOT_AVAILABE_IN_SHIKHARA
	dbg( "dwmci: DWMCI_BMOD:      0x%08x\n",
		dwmci_readl(host, DWMCI_BMOD));
	dbg( "dwmci: DWMCI_PLDMND:   0x%08x\n",
		dwmci_readl(host, DWMCI_PLDMND));
	dbg( "dwmci: DWMCI_DBADDR:    0x%08x\n",
		dwmci_readl(host, DWMCI_DBADDR));
	dbg( "dwmci: DWMCI_IDSTS:     0x%08x\n",
		dwmci_readl(host, DWMCI_IDSTS));
	dbg( "dwmci: DWMCI_IDINTEN:   0x%08x\n",
		dwmci_readl(host, DWMCI_IDINTEN));
	dbg( "dwmci: DWMCI_DSCADDR:   0x%08x\n",
		dwmci_readl(host, DWMCI_DSCADDR));
	dbg( "dwmci: DWMCI_BUFADDR:   0x%08x\n",
		dwmci_readl(host, DWMCI_BUFADDR));
#endif
	dbg( "dwmci: ===========================================\n");
}


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

static void dwmci_set_idma_desc(u8 *idmac, unsigned int des0,
		unsigned int des1, unsigned int des2)
{
	struct dwmci_idmac *desc = (struct dwmci_idmac *)idmac;

	desc->des0 = des0;
	desc->des1 = des1;
	desc->des2 = des2;
	desc->des3 = (unsigned int)desc + sizeof(struct dwmci_idmac);
}
#ifdef NOT_AVAILABE_IN_SHIKHARA
static void dwmci_prepare_data(struct dwmci_host *host,
		struct mmc_data *data)
{
	unsigned long ctrl;
	unsigned int i = 0, flag, cnt, blk_cnt;
	struct dwmci_idmac *p;
	ulong data_start, data_end, start_addr;
	ALLOC_CACHE_ALIGN_BUFFER(struct dwmci_idmac, idmac, 65565);


	p = idmac;
	blk_cnt = data->blocks;

	dwmci_wait_reset(host, DWMCI_CTRL_FIFO_RESET);

	if (data->flags == MMC_DATA_READ)
		start_addr = (unsigned int)data->dest;
	else
		start_addr = (unsigned int)data->src;

	do {
		flag = DWMCI_IDMAC_OWN | DWMCI_IDMAC_CH ;
		flag |= (i == 0) ? DWMCI_IDMAC_FS : 0;
		if (blk_cnt <= 8) {
			flag |= DWMCI_IDMAC_LD;
			cnt = data->blocksize * blk_cnt;
		} else {
			flag &= ~DWMCI_IDMAC_LD;
			cnt = data->blocksize * 8;
		}

		dwmci_set_idma_desc((u8 *)p, flag, cnt,
				start_addr + (i * PAGE_SIZE));

		if (blk_cnt <= 8)
			break;
		blk_cnt -= 8;
		p++;
		i++;
	} while(1);

	data_start = (ulong)idmac;
	data_end = (ulong)p;
#ifndef CONFIG_BL1_BUILD
	flush_dcache_range(data_start, data_end + ARCH_DMA_MINALIGN);
#endif
	dwmci_writel(host, DWMCI_DBADDR, (unsigned int)(idmac));

	ctrl = dwmci_readl(host, DWMCI_CTRL);
	ctrl |= DWMCI_IDMAC_EN | DWMCI_DMA_EN;
	dwmci_writel(host, DWMCI_CTRL, ctrl);

	ctrl = dwmci_readl(host, DWMCI_BMOD);
	ctrl |= DWMCI_BMOD_IDMAC_FB | DWMCI_BMOD_IDMAC_EN;
	dwmci_writel(host, DWMCI_BMOD, ctrl);

	dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
	dwmci_writel(host, DWMCI_BYTCNT, data->blocksize * data->blocks);
}
#endif

static int dwmci_set_transfer_mode(struct dwmci_host *host,
		struct mmc_data *data)
{
	unsigned long mode;

	mode = DWMCI_CMD_DATA_EXP;
	if (data->flags & MMC_DATA_WRITE)
		mode |= DWMCI_CMD_RW;

	return mode;
}

static void dw_mci_set_part_bytes(struct dwmci_host *host, void *buf, int cnt)
{
        memcpy((void *)&host->part_buf, buf, cnt);
        host->part_buf_count = cnt;
}

static int dw_mci_push_part_bytes(struct dwmci_host *host, void *buf, int cnt)
{
        unsigned int shift=2;
        cnt = min(cnt, (1 << shift) - host->part_buf_count);
        memcpy((void *)&host->part_buf + host->part_buf_count, buf, cnt);
        host->part_buf_count += cnt;
        return cnt;

}

static void dw_mci_push_data32(struct dwmci_host *host, void *buf, int cnt,struct mmc_data *data)
{
        int init_cnt = cnt;

        /* try and push anything in the part_buf */
        if (host->part_buf_count) {
                int len = dw_mci_push_part_bytes(host, buf, cnt);
                buf += len;
                cnt -= len;
                if (host->part_buf_count == 4) {
                        dwmci_writel(host,DWMCI_DATA,
                                        host->part_buf32);
                        host->part_buf_count = 0;
                }
        }
        if ((unsigned long)buf & 0x3) {
                while (cnt >= 4) {
                        u32 aligned_buf[32];
                        int len = min(cnt & -4, (int)sizeof(aligned_buf));
                        int items = len >> 2;
                        int i;
                        /* memcpy from input buffer into aligned buffer */
                        memcpy(aligned_buf, buf, len);
                        buf += len;
                        cnt -= len;
                        /* push data from aligned buffer into fifo */
                        for (i = 0; i < items; ++i)
                                dwmci_writel(host, DWMCI_DATA,
                                                aligned_buf[i]);
                }
        } else
        {
                u32 *pdata = buf;
                for (; cnt >= 4; cnt -= 4)
                        dwmci_writel(host, DWMCI_DATA, *pdata++);
                buf = pdata;
        }
	/* put anything remaining in the part_buf */
        if (cnt) {
                dw_mci_set_part_bytes(host, buf, cnt);
                 /* Push data if we have reached the expected data length */
                if ((data->bytes_xfered + init_cnt) ==
                    (data->blocksize * data->blocks))
                        dwmci_writel(host, DWMCI_DATA,
                                   host->part_buf32);
        }
}

static void dw_mci_write_data_pio(struct dwmci_host *host,struct mmc_data *data)
{
        void *buf=NULL;
        unsigned int offset,bytes_xfered;
        int shift =2;
        u32 status;
        unsigned int len;
        unsigned int fifo_depth =0x80;
         unsigned int remain, fcnt;
//        do {
		 buf= (void *)data->src;
                 offset = 0;
		remain=(data->blocks)*(data->blocksize);
                do {
                 fcnt = ((fifo_depth -
                                 SDMMC_GET_FCNT(dwmci_readl(host,DWMCI_STATUS)))
                                        << shift) - host->part_buf_count;
                        len = min(remain, fcnt);
                        if (!len)
                                break;
                         dw_mci_push_data32(host, (void *)(buf + offset), len,data);
                        data->bytes_xfered += len;
                        printf("bytes xfered = %X\n",data->bytes_xfered);
                        offset += len;
                        remain -= len;

                } while (remain);
                dwmci_writel(host, DWMCI_RINTSTS, DWMCI_INTMSK_TXDR);
  //      status = dwmci_readl(host, DWMCI_RINTSTS);
    //    }while (status & DWMCI_INTMSK_TXDR);


}

static void dw_mci_pull_final_bytes(struct dwmci_host *host, void *buf, int cnt)
{
        unsigned int temp=2;
        memcpy(buf, &host->part_buf, cnt);
        host->part_buf_start = cnt;
        host->part_buf_count = (1 << temp) - cnt;
}

static void dw_mci_pull_data32(struct dwmci_host *host, void *buf, int cnt)
{
        if ((unsigned long)buf & 0x3) {
                while (cnt >= 4) {
                        /* pull data from fifo into aligned buffer */
                        u32 aligned_buf[32];
                        int len = min(cnt & -4, (int)sizeof(aligned_buf));
                        int items = len >> 2;
                        int i;
                        for (i = 0; i < items; ++i)
                                aligned_buf[i] = dwmci_readl(host,DWMCI_DATA);
                        /* memcpy from aligned buffer into output buffer */
                        memcpy(buf, aligned_buf, len);
                        buf += len;
                        cnt -= len;
                }
        } else
        {
                u32 *pdata = buf;
                for (; cnt >= 4; cnt -= 4)
                        *pdata++ = dwmci_readl(host, DWMCI_DATA);
                buf = pdata;
        }
        if (cnt) {
                host->part_buf32 = dwmci_readl(host, DWMCI_DATA);
                dw_mci_pull_final_bytes(host, buf, cnt);
        }
}


static int dw_mci_pull_part_bytes(struct dwmci_host *host, void *buf, int cnt)
{
        cnt = min(cnt, (int)host->part_buf_count);
        if (cnt) {
                memcpy(buf, (void *)&host->part_buf + host->part_buf_start,
                       cnt);
                host->part_buf_count -= cnt;
                host->part_buf_start += cnt;
                printf("copied data 0x%X\n", host->part_buf);
        }
        return cnt;
}

static void dw_mci_pull_data(struct dwmci_host *host, void *buf, int cnt)
{
        int len;

        /* get remaining partial bytes */
        len = dw_mci_pull_part_bytes(host, buf, cnt);
        if (len == cnt)
                return;
        buf += len;
        cnt -= len;

        /* get the rest of the data */
        dw_mci_pull_data32(host, buf, cnt);
}

static void dw_mci_read_data_pio(struct dwmci_host *host, char dto,struct mmc_data *data)
{
        void *buf;
        unsigned int offset,data_blk,shift=2;
        unsigned int status,len,remain,fcnt;
//         do {
                buf= (void *)data->dest;
                 data_blk=data->blocks;
                printf("no. of blocks 0x%X\n",data_blk);
                remain=data_blk*(data->blocksize);
                printf("no. of bytes to transfer 0x%X\n",remain);
                offset = 0;
                do {
                        fcnt = (SDMMC_GET_FCNT(dwmci_readl(host,DWMCI_STATUS))
                                        << shift) + host->part_buf_count;
                        printf("fcnt = 0x%X\n",fcnt);
                        len = min(remain, fcnt);
                        printf("len = 0x%X\n",len);
                        if (!len)
                                break;
                        dw_mci_pull_data(host, (void *)(buf + offset), len);
                        data->bytes_xfered += len;
                         printf("bytes_xfered = 0x%X\n",data->bytes_xfered);
                        offset += len;
                        remain -= len;
                } while (remain);

        dwmci_writel(host, DWMCI_RINTSTS,DWMCI_INTMSK_RXDR);
        status =dwmci_readl(host,DWMCI_RINTSTS);
//        } while ((status & DWMCI_INTMSK_RXDR) ||
  //               (dto && SDMMC_GET_FCNT(dwmci_readl(host,DWMCI_STATUS))));

}

static int dwmci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
		struct mmc_data *data)
{
	struct dwmci_host *host = (struct dwmci_host *)mmc->priv;
	int flags = 0, i;
	unsigned int timeout = 100000;
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

//	if (data)
//		dwmci_prepare_data(host, data);


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

	flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG);

	printf("Sending CMD%d \t\tCMD_ARG\t\t\t 0x%08X\n",cmd->cmdidx,flags);

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
		printf("Response Timeout..\n");
		return TIMEOUT;
	} else if (mask & DWMCI_INTMSK_RE) {
		printf("Response Error..\n");
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
	if(data){
	ctrl = dwmci_readl(host, DWMCI_CTRL);
        ctrl |= ~(DWMCI_IDMAC_EN) |~(DWMCI_DMA_EN);
	dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
        dwmci_writel(host, DWMCI_BYTCNT, data->blocksize * data->blocks);
	
	while(1)
        {
        unsigned int pending;
        pending = dwmci_readl(host, DWMCI_RINTSTS);
         dwmci_writel(host, DWMCI_RINTSTS,0xffffffff);
        if (pending) {
         dbg("int status 0x%X!\n",pending);
         if (pending & DWMCI_INTMSK_DTO) {
                if(data->flags|MMC_DATA_READ)
                         dw_mci_read_data_pio(host,1,data);
                break;
                }
        if (pending & DWMCI_INTMSK_RXDR) {
                 dwmci_writel(host,DWMCI_RINTSTS,DWMCI_INTMSK_RXDR);
                 dw_mci_read_data_pio(host, 0,data);
                break; // fix this for multi block transfer
                }
         if (pending & DWMCI_INTMSK_TXDR) {
                        dwmci_writel(host, DWMCI_RINTSTS, DWMCI_INTMSK_TXDR);
                                dw_mci_write_data_pio(host,data);
			break;
                }
         if (pending & DWMCI_INTMSK_CDONE) {
                        dwmci_writel(host,DWMCI_RINTSTS, DWMCI_INTMSK_CDONE);
                        printf("command done cleared\n");
                }

          }

	}	
   }

#if 0
//#ifdef NOT_AVAILABE_IN_SHIKHARA
	if (data) {
		while (1) {
			mask = dwmci_readl(host, DWMCI_RINTSTS);
			if (mask & (DWMCI_DATA_ERR | DWMCI_DATA_TOUT)) {
				printf("DATA ERROR!\n");
				return -1;
			} else if (mask & DWMCI_INTMSK_DTO)
				break;
		}

		dwmci_writel(host, DWMCI_RINTSTS, mask);

		ctrl = dwmci_readl(host, DWMCI_CTRL);
		ctrl &= ~(DWMCI_DMA_EN);
		dwmci_writel(host, DWMCI_CTRL, ctrl);
	}
#endif
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
	dwmci_writel(host, DWMCI_PWREN, 1);

	if (!dwmci_wait_reset(host, DWMCI_RESET_ALL)) {
		debug("%s[%d] Fail-reset!!\n",__func__,__LINE__);
		return -1;
	}

	dwmci_writel(host, DWMCI_RINTSTS, 0xFFFFFFFF);
	dwmci_writel(host, DWMCI_INTMASK, 0);

	dwmci_writel(host, DWMCI_TMOUT, 0xFFFFFFFF);

#ifdef NOT_AVAILABE_IN_SHIKHARA
	dwmci_writel(host, DWMCI_IDINTEN, 0);
	dwmci_writel(host, DWMCI_BMOD, 1);
#endif
	fifo_size = 0x80;
	if (host->fifoth_val)
		fifoth_val = host->fifoth_val;
	else
		fifoth_val = (0x2 << 28) | ((fifo_size/2 -1) << 16) |
			((fifo_size/2) << 0);
	dwmci_writel(host, DWMCI_FIFOTH, fifoth_val);

	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

#ifdef CONFIG_DUMP_DWMMC_REGS
	dwmci_dumpregs_err(host);
#endif
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

	mmc->priv = host;
	host->mmc = mmc;

	sprintf(mmc->name, "%s", host->name);
	mmc->send_cmd = dwmci_send_cmd;
	mmc->set_ios = dwmci_set_ios;
	mmc->init = dwmci_init;
	mmc->f_min = min_clk;
	mmc->f_max = max_clk;

	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

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
	mmc->host_caps |= MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_HC;

	err = mmc_register(mmc);

	return err;
}

