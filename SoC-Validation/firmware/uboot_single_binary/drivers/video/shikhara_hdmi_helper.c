#include <common.h>
#include <command.h>
#include<asm/io.h>
#include <asm/arch/shikhara_hdmi.h>
#include <asm/arch/shikhara_hdmi_bridge.h>
#include <../lib/snps-hdmi/api/api.h>
#include <asm/global_data.h>
#define DEBUG_HDMI
#ifdef DEBUG_HDMI
	#define dbg(x...) printf(x)
#else
	#define dbg(x...) do {}while(0)
#endif

void hdmi_writeb(u8 value, unsigned int reg)
{
        __raw_writeb(value, HDMI_BASE_ADDR + reg);
}
u8 hdmi_readb(unsigned int reg)
{
        u8 value;

        value = __raw_readb(HDMI_BASE_ADDR + reg);

        return value;
}


void hdmi_mask_writeb(u8 data, unsigned int reg, u8 shift, u8 mask)
{
        u8 value = hdmi_readb(reg) & ~mask;
        value |= (data << shift) & mask;
        hdmi_writeb(value, reg);
}

void hdmi_clear_overflow(void)
{       
        int count;
        u8 val;

        /* TMDS software reset */
        hdmi_writeb((u8)~HDMI_MC_SWRSTZ_TMDSSWRST_REQ, HDMI_MC_SWRSTZ);

        val = hdmi_readb(HDMI_FC_INVIDCONF);
        
        for (count = 0 ; count < 5 ; count++)
                hdmi_writeb(val, HDMI_FC_INVIDCONF);
}

int do_hdmi_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dbg("=========HDMI Register dump==========\n");
	dbg("Frame Composer Registers\n");
	dbg("HDMI_FC_INVIDCONF\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INVIDCONF, hdmi_readb(HDMI_FC_INVIDCONF));
	dbg("HDMI_FC_INHACTV1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INHACTV1, hdmi_readb(HDMI_FC_INHACTV1));
	dbg("HDMI_FC_INHACTV0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INHACTV0, hdmi_readb(HDMI_FC_INHACTV0));
	dbg("HDMI_FC_INHBLANK1\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INHBLANK1, hdmi_readb(HDMI_FC_INHBLANK1));
	dbg("HDMI_FC_INHBLANK0\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INHBLANK0, hdmi_readb(HDMI_FC_INHBLANK0));
	dbg("HDMI_FC_INVBLANK \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_INVBLANK, hdmi_readb(HDMI_FC_INVBLANK));
	dbg("HDMI_FC_VSYNCINDELAY\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_VSYNCINDELAY, hdmi_readb(HDMI_FC_VSYNCINDELAY));
	
	dbg("HDMI_FC_EXCTRLDUR \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_EXCTRLDUR, hdmi_readb(HDMI_FC_EXCTRLDUR));
	dbg("HDMI_FC_EXCTRLSPAC\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_EXCTRLSPAC, hdmi_readb(HDMI_FC_EXCTRLSPAC));	
	dbg("HDMI_FC_CH0PREAM \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_CH0PREAM, hdmi_readb(HDMI_FC_CH0PREAM));
	dbg("HDMI_FC_CH1PREAM \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_CH2PREAM, hdmi_readb(HDMI_FC_CH1PREAM));
	dbg("HDMI_FC_CH2PREAM \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_CH2PREAM, hdmi_readb(HDMI_FC_CH2PREAM));
	dbg("HDMI_FC_AVICONF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AVICONF0, hdmi_readb(HDMI_FC_AVICONF0));
	dbg("HDMI_FC_AVICONF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AVICONF1, hdmi_readb(HDMI_FC_AVICONF1));
	dbg("HDMI_FC_AVICONF2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AVICONF2, hdmi_readb(HDMI_FC_AVICONF2));
	dbg("HDMI_FC_AVICONF3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AVICONF3, hdmi_readb(HDMI_FC_AVICONF3));
	dbg("HDMI_FC_AVIVID \t\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AVIVID, hdmi_readb(HDMI_FC_AVIVID));
	dbg("HDMI_FC_AUDSCONF \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCONF, hdmi_readb(HDMI_FC_AUDSCONF));
	dbg("Phy Configuration\n");
	dbg("HDMI_PHY_CONF0 \t\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_PHY_CONF0, hdmi_readb(HDMI_PHY_CONF0));
	dbg("HDMI_PHY_TST0 \t\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_PHY_TST0, hdmi_readb(HDMI_PHY_TST0));
	dbg("HDMI_PHY_STAT0 \t\t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_PHY_STAT0, hdmi_readb(HDMI_PHY_STAT0));
	dbg("HDMI_MC_PHYRSTZ \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_MC_PHYRSTZ, hdmi_readb(HDMI_MC_PHYRSTZ));
	
	
	dbg("HDMI_VP_STUFF \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_VP_STUFF, hdmi_readb(HDMI_VP_STUFF));
	dbg("HDMI_VP_CONF \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_VP_CONF, hdmi_readb(HDMI_VP_CONF));
	dbg("HDMI_VP_PR_CD \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_VP_PR_CD, hdmi_readb(HDMI_VP_PR_CD));
	dbg("HDMI_TX_INVID0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_TX_INVID0, hdmi_readb(HDMI_TX_INVID0));	
	dbg("HDMI_TX_INSTUFFING \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_TX_INSTUFFING, hdmi_readb(HDMI_TX_INSTUFFING));
	dbg("HDCP register dump\n");
	dbg("HDMI_A_HDCPCFG0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_A_HDCPCFG0, hdmi_readb(HDMI_A_HDCPCFG0));
	dbg("HDMI_A_HDCPCFG1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_A_HDCPCFG1, hdmi_readb(HDMI_A_HDCPCFG1));
	dbg("HDMI_A_VIDPOLCFG \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_A_VIDPOLCFG, hdmi_readb(HDMI_A_VIDPOLCFG));
	dbg("HDMI_A_OESSWCFG \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_A_OESSWCFG, hdmi_readb(HDMI_A_OESSWCFG));
	dbg("HDMI Bridge register dump\n");
	dbg("HDMI_BB_PIXEL_FORMAT_CNG_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_DMA_BASE_ADDR_REGISTER_1, 
		readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER));
	dbg("HDMI_BB_DMA_BASE_ADDR_REGISTER_1 \t\tAddress:0x%x  value:0x%08x\n",  HDMI_BB_DMA_BASE_ADDR_REGISTER_1, 
		readl(HDMI_BB_DMA_BASE_ADDR_REGISTER_1));
	dbg("HDMI_BB_HORIZONTAL_RESOLUTION_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_HORIZONTAL_RESOLUTION_REGISTER, 
		readl(HDMI_BB_HORIZONTAL_RESOLUTION_REGISTER));
	dbg("HDMI_BB_H_FRONT_P_REGISTER  \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_H_FRONT_P_REGISTER, 
		readl(HDMI_BB_H_FRONT_P_REGISTER));
	dbg("HDMI_BB_HSYC_WIDTH_REGISTER, \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_HSYC_WIDTH_REGISTER, 
		readl(HDMI_BB_HSYC_WIDTH_REGISTER));
	dbg("HDMI_BB_VERTICAL_RESOLUTION_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_VERTICAL_RESOLUTION_REGISTER, 
		readl(HDMI_BB_VERTICAL_RESOLUTION_REGISTER));
	dbg("HDMI_BB_V_BACK_P_REGISTER  \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_V_BACK_P_REGISTER, 
		readl(HDMI_BB_V_BACK_P_REGISTER));
	dbg("HDMI_BB_V_FRONT_P_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_V_FRONT_P_REGISTER, 
		readl(HDMI_BB_V_FRONT_P_REGISTER));
	dbg("HDMI_BB_VSYNC_WIDTH_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_VSYNC_WIDTH_REGISTER, 
		readl(HDMI_BB_VSYNC_WIDTH_REGISTER));
	dbg("HDMI_BB_PIXEL_FORMAT_CNG_REGISTER \t\tAddress:0x%x  value:0x%08x\n", HDMI_BB_PIXEL_FORMAT_CNG_REGISTER, 
		readl(HDMI_BB_PIXEL_FORMAT_CNG_REGISTER));

	dbg("HDMI Audio Registers\n");
	dbg("HDMI_AHB_DMA_CONF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_CONF0, hdmi_readb(HDMI_AHB_DMA_CONF0));
	dbg("HDMI_AHB_DMA_START \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_START, hdmi_readb(HDMI_AHB_DMA_START));
	dbg("HDMI_AHB_DMA_STOP \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STOP, hdmi_readb(HDMI_AHB_DMA_STOP));
	dbg("HDMI_AHB_DMA_THRSLD \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_THRSLD, hdmi_readb(HDMI_AHB_DMA_THRSLD));
	dbg("HDMI_AHB_DMA_STRADDR0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STRADDR0, hdmi_readb(HDMI_AHB_DMA_STRADDR0));
	dbg("HDMI_AHB_DMA_STRADDR1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STRADDR1, hdmi_readb(HDMI_AHB_DMA_STRADDR1));
	dbg("HDMI_AHB_DMA_STRADDR2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STRADDR2, hdmi_readb(HDMI_AHB_DMA_STRADDR2));
	
	dbg("HDMI_AHB_DMA_STPADDR0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STPADDR0, hdmi_readb(HDMI_AHB_DMA_STPADDR0));
	dbg("HDMI_AHB_DMA_STPADDR1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STPADDR1, hdmi_readb(HDMI_AHB_DMA_STPADDR1));
	dbg("HDMI_AHB_DMA_STPADDR2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STPADDR2, hdmi_readb(HDMI_AHB_DMA_STPADDR2));
	dbg("HDMI_AHB_DMA_STPADDR3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STPADDR3, hdmi_readb(HDMI_AHB_DMA_STPADDR3));
	dbg("HDMI_AHB_DMA_STAT \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_STAT, hdmi_readb(HDMI_AHB_DMA_STAT));
	dbg("HDMI_AHB_DMA_INT \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_INT, hdmi_readb(HDMI_AHB_DMA_INT));	
	dbg("HDMI_AHB_DMA_MASK \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_MASK, hdmi_readb(HDMI_AHB_DMA_MASK));
 	dbg("HDMI_AHB_DMA_POL \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_POL, hdmi_readb(HDMI_AHB_DMA_POL));
	dbg("HDMI_AHB_DMA_CONF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_CONF1, hdmi_readb(HDMI_AHB_DMA_CONF1));
	dbg("HDMI_AHB_DMA_CONF2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_CONF2, hdmi_readb(HDMI_AHB_DMA_CONF2));
	dbg("HDMI_AHB_DMA_BUFFPOL \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_BUFFPOL, hdmi_readb(HDMI_AHB_DMA_BUFFPOL));
	dbg("HDMI_AHB_DMA_MASK1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AHB_DMA_MASK1, hdmi_readb(HDMI_AHB_DMA_MASK1));
	dbg("HDMI_AUD_CTS1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CTS1, hdmi_readb(HDMI_AUD_CTS1));
	dbg("HDMI_AUD_CTS2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CTS2, hdmi_readb(HDMI_AUD_CTS2));
	dbg("HDMI_AUD_CTS3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CTS3, hdmi_readb(HDMI_AUD_CTS3));
	dbg("HDMI_AUD_N1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_N1, hdmi_readb(HDMI_AUD_N1));
	dbg("HDMI_AUD_N2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_N2, hdmi_readb(HDMI_AUD_N2));	
	dbg("HDMI_AUD_N3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_N3, hdmi_readb(HDMI_AUD_N3));
	dbg("HDMI_FC_AUDICONF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDICONF0, hdmi_readb(HDMI_FC_AUDICONF0));
	dbg("HDMI_FC_AUDICONF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDICONF1, hdmi_readb(HDMI_FC_AUDICONF1));
	dbg("HDMI_FC_AUDICONF2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDICONF2, hdmi_readb(HDMI_FC_AUDICONF2));
	dbg("HDMI_FC_AUDICONF3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDICONF3, hdmi_readb(HDMI_FC_AUDICONF3));
	dbg("HDMI_FC_AUDSCONF \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCONF, hdmi_readb(HDMI_FC_AUDSCONF));
	dbg("HDMI_FC_AUDSV \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSV, hdmi_readb(HDMI_FC_AUDSV));
	dbg("HDMI_FC_AUDSU \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSU, hdmi_readb(HDMI_FC_AUDSU));	
	dbg("HDMI_FC_AUDSCHNLS0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS0, hdmi_readb(HDMI_FC_AUDSCHNLS0));
	dbg("HDMI_FC_AUDSCHNLS1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS1, hdmi_readb(HDMI_FC_AUDSCHNLS1));
	dbg("HDMI_FC_AUDSCHNLS2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS2, hdmi_readb(HDMI_FC_AUDSCHNLS2));
	dbg("HDMI_FC_AUDSCHNLS3 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS3, hdmi_readb(HDMI_FC_AUDSCHNLS3));
	dbg("HDMI_FC_AUDSCHNLS4 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS4, hdmi_readb(HDMI_FC_AUDSCHNLS4));
	dbg("HDMI_FC_AUDSCHNLS5 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS5, hdmi_readb(HDMI_FC_AUDSCHNLS5));
	dbg("HDMI_FC_AUDSCHNLS6 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS6, hdmi_readb(HDMI_FC_AUDSCHNLS6));
	dbg("HDMI_FC_AUDSCHNLS7 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS7, hdmi_readb(HDMI_FC_AUDSCHNLS7));
	dbg("HDMI_FC_AUDSCHNLS8 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_FC_AUDSCHNLS8, hdmi_readb(HDMI_FC_AUDSCHNLS8));
	dbg("HDMI_MC_SWRSTREQ2 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_MC_SWRSTREQ2, hdmi_readb(HDMI_MC_SWRSTREQ2));

	dbg("HDMI AUDIO I2S AND SPDIF REGISTERS\n");
	dbg("HDMI_AUD_CONF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CONF0, hdmi_readb(HDMI_AUD_CONF0));
	dbg("HDMI_AUD_CONF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CONF1, hdmi_readb(HDMI_AUD_CONF1));
	dbg("HDMI_AUD_INT \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_INT, hdmi_readb(HDMI_AUD_INT));
	dbg("HDMI_AUD_CONF2 \t\tAddress:0x%x  value:0x%08x \n", HDMI_BASE_ADDR+HDMI_AUD_CONF2, hdmi_readb(HDMI_AUD_CONF2));
	dbg("HDMI_AUD_INPUTCLKFS \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_INPUTCLKFS, hdmi_readb(HDMI_AUD_INPUTCLKFS));
	dbg("HDMI_AUD_SPDIF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_SPDIF0, hdmi_readb(HDMI_AUD_SPDIF0));
	dbg("HDMI_AUD_SPDIF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_SPDIF1, hdmi_readb(HDMI_AUD_SPDIF1));
	dbg("HDMI_AUD_SPDIFINT \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_SPDIFINT, hdmi_readb(HDMI_AUD_SPDIFINT));
	dbg("HDMI_AUD_CONF0_HBR \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_CONF0_HBR, hdmi_readb(HDMI_AUD_CONF0_HBR));
	dbg("HDMI_AUD_HBR_STATUS \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_HBR_STATUS, hdmi_readb(HDMI_AUD_HBR_STATUS));
	dbg("HDMI_AUD_HBR_MASK \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_AUD_HBR_MASK, hdmi_readb(HDMI_AUD_HBR_MASK));
	dbg("HDMI_GP_CONF0 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_GP_CONF0, hdmi_readb(HDMI_GP_CONF0));
	dbg("HDMI_GP_CONF1 \t\tAddress:0x%x  value:0x%08x\n", HDMI_BASE_ADDR+HDMI_GP_CONF1, hdmi_readb(HDMI_GP_CONF1));
	dbg("HDMI_GP_CONF2 \t\tAddress:0x%x  value:0x%08x \n", HDMI_BASE_ADDR+HDMI_GP_CONF2, hdmi_readb(HDMI_GP_CONF2));
	dbg("HDMI_GP_STAT \t\tAddress:0x%x  value:0x%08x \n", HDMI_BASE_ADDR+HDMI_GP_STAT, hdmi_readb(HDMI_GP_STAT));

	dbg("----synopsys driver dump----\n");	
	api_ReadConfig();
	return 0;	
}


#if defined(CONFIG_HDMI)  
void *lcd_base3;
#endif

int do_hdmi_enable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u8 reg;
#if defined(CONFIG_HDMI)
                shikhara_hdmi_init(lcd_base3);
#endif	
	
	enable_hdmi_bridge();
/*Enabling HDMI mode 	 
  From Synopsys HDMI 1.4a User guide Step:B, Initially configure the controller in DVI mode then shift the mode of HDMI Tx controller to HDMI mode if required. Initial configuration with DVI mode is recommended. 	 
  */
#ifdef CONFIG_HDMI_AUDIO
	reg = hdmi_readb(HDMI_FC_INVIDCONF); 	 
	reg |= 0x08; 	 
	hdmi_writeb(reg, HDMI_FC_INVIDCONF);
#endif
	return 0;
}


int do_hdmi_disable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u8 reg;
	
        disable_hdmi_bridge();
	
        return 0;
}


U_BOOT_CMD(
        hdmi_enable,     1,      1,      do_hdmi_enable,
        "To enable the HDMI Bridge",
        " "
	" "
);

U_BOOT_CMD(
        hdmi_disable,     1,      1,      do_hdmi_disable,
        "To enable the HDMI Bridge",
        " "
        " "
);

U_BOOT_CMD(
        hdmi_dump,     1,      1,      do_hdmi_dump,
        "To dump the HDMI register set",
        " "
        " "
);


