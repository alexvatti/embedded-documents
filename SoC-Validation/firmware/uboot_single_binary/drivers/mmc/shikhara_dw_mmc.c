#include <config.h>
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/shikhara_dwmmc.h>
#include <asm/arch/shikhara_clock.h>
#include <dwmmc.h>
unsigned int get_shikhara_mmc_clk(struct dwmci_host *host, int dev_index);
struct dwmci_host  *host;
#define MAX_CLK		50000000
#define MIN_CLK		400000
#define DIV_VAL_CLK_SDMMC_50MHz MMC_CLK/50000000
#define DIV_VAL_CLK_SDMMC_100MHz MMC_CLK/100000000
#define DIV_VAL_CLK_SDMMC_200MHz MMC_CLK/200000000

static char *SHIKHARA_NAME = "SHIKHARA DWMMC";

#define SHIKHARA_CLK_GATE_3 SHIKHARA_SYS_CTRL_BASEADDR+0x010 // bits 30,29,28,27 to be modifed
/*shikhara_setup_mshc_clock() sets up the 
 * clock for sdmmc 0 and sdmmc 1.
 * The clocks are gated hence must be 
 * modified to enable the clock to the controllers*/
        
void shikhara_enable_dwmmc_clock(void)
{
#ifdef CONFIG_SHIKHARA_ASIC
        unsigned int clk_gate;
        clk_gate=readl(SHIKHARA_CLK_GATE_3);
        clk_gate=clk_gate&~(0xf8000000);//bit 30,29,28,27 set to '0' to allow clock
        writel(clk_gate,SHIKHARA_CLK_GATE_3);
#endif
}

unsigned int get_shikhara_mmc_clk(struct dwmci_host *host, int dev_index)
{
	u32 div_mmc, div_mmc_pre,sd_mmc_div;
	unsigned long sclk_mmc;
	unsigned clock = MMC_CLK;		/* Default MMC Input clock for ASIC */	
#ifdef CONFIG_SHIKHARA_ASIC
        if(dev_index == 0 || dev_index==1) {
        if(host->ioaddr==SHIKHARA_MMC0_BASE)
                sd_mmc_div=SHIKHARA_SDMMC_DIV_RATIO_0;
        }else if (dev_index==2){

       if(host->ioaddr==SHIKHARA_MMC1_BASE)
                sd_mmc_div=SHIKHARA_SDMMC_DIV_RATIO_1;
        }
#endif


#ifdef CONFIG_SHIKHARA_ASIC             /* For ASIC one of the three clock sources can be selected from the parent clock source:50Mhz,100Mhz and 200Mhz */
        if(clock<=50000000)
        {
                /* MODIFIED FOR SHIKHARA:
                 * this is assumed as the divider is 
                 * always dividing from a constant 400MHz
                 * */
                writel(DIV_VAL_CLK_SDMMC_50MHz,sd_mmc_div);
                sclk_mmc=50000000;
        }
	
	else if(clock>50000000 && clock<=100000000)
        {
                /* MODIFIED FOR SHIKHARA
                 * this is assumed as the divider 
                 * is always dividing from a constant 400MHz
                 * */
                writel(DIV_VAL_CLK_SDMMC_100MHz,sd_mmc_div);
                div_mmc_pre=dwmci_readl(host,DWMCI_UHS_REG);
                div_mmc_pre=div_mmc_pre|0x40000000;//bit 30,31 of MSHCI_UHS_REG set for speed modes
                dwmci_writel(host,div_mmc_pre,DWMCI_UHS_REG);
                sclk_mmc=100000000;
        }
        else if (clock>100000000 && clock<=200000000)
        {
                /* MODIFIED FOR SHIKHARA:
                 * this is assumed as the divider 
                 * is always dividing from a constant 400MHz
                 * */
                writel(DIV_VAL_CLK_SDMMC_200MHz,sd_mmc_div);
                div_mmc_pre=dwmci_readl(host,DWMCI_UHS_REG);
                div_mmc_pre=div_mmc_pre|0x80000000;//bit 30,31 of MSHCI_UHS_REG set for speed modes
                dwmci_writel(host,div_mmc_pre,DWMCI_UHS_REG);
                sclk_mmc=200000000;
        }
        else
        {
                printf("\nthe clock requested is not supported\n");
                return;
        }

#endif
	return sclk_mmc;


}
int shikhara_dwmci_init(u32 regbase, int index)
{
	struct dwmci_host *host = NULL;
	 host = (struct dwmci_host *)malloc(sizeof(struct dwmci_host));
	if (!host) {
		printf("dwmci_host malloc fail!\n");
		return 1;
	}

	shikhara_enable_dwmmc_clock();		/* Enable the top MMC clock which is gated */
	host->name = SHIKHARA_NAME;
	host->ioaddr = (void *)regbase;
	host->dev_index = index;
	host->mmc_clk = get_shikhara_mmc_clk;	
	host->clksel = NULL;

	debug("DW_MMC Host Base address = 0x%p \n",host->ioaddr);
	add_dwmci(host,MAX_CLK , MIN_CLK, index);
	
	return 0;
}
