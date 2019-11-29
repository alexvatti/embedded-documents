#include <common.h>
#include <command.h>
#include<asm/io.h>
#include <asm/arch/shikhara_hdmi.h>

static void hdmi_write(u8 value, unsigned int reg)
{       
        __raw_writeb(value, HDMI_BASE_ADDR + reg);
}       
static u8 hdmi_read(unsigned int reg)
{       
        u8 value;
        
        value = __raw_readb(HDMI_BASE_ADDR + reg);
                 
        return value;
}      

static u8 detect_hdmi(void){
	return hdmi_read(HDMI_PHY_STAT0) & HDMI_PHY_HPD;
}

static void hdmi_av_composer()
{
	int reg;
        /* Set up horizontal active pixel region width */
        hdmi_write(640 >> 8, HDMI_FC_INHACTV1);
        hdmi_write(640, HDMI_FC_INHACTV0);


        /* Set up vertical blanking pixel region width */
        hdmi_write(480 >> 8, HDMI_FC_INVACTV1);
        hdmi_write(480, HDMI_FC_INVACTV0);

        /* Set up horizontal blanking pixel region width 
         hblank = left_margin + right_margin + hsync_len;*/
        hdmi_write(160 >> 8, HDMI_FC_INHBLANK1);
        hdmi_write(160, HDMI_FC_INHBLANK0);

        /* Set up vertical blanking pixel region width 
         vblank = upper_margin + lower_margin + vsync_len;*/
        hdmi_write(45, HDMI_FC_INVBLANK);

        /* Set up HSYNC active edge delay width (in pixel clks) */
        hdmi_write(16 >> 8, HDMI_FC_HSYNCINDELAY1);
        hdmi_write(16, HDMI_FC_HSYNCINDELAY0);
  /* Set up VSYNC active edge delay (in pixel clks) */
        hdmi_write(10, HDMI_FC_VSYNCINDELAY);

        /* Set up HSYNC active pulse width (in pixel clks) */
        hdmi_write(96 >> 8, HDMI_FC_HSYNCINWIDTH1);
        hdmi_write(96, HDMI_FC_HSYNCINWIDTH0);

        /* Set up VSYNC active edge delay (in pixel clks) */
        hdmi_write(2, HDMI_FC_VSYNCINWIDTH);

	/*Configuring HDMI mode. */
	reg = hdmi_read(HDMI_FC_INVIDCONF);
	reg |= 0x08;
	hdmi_write(reg, HDMI_FC_INVIDCONF);

}
static int enable_hdmi_tx(void)
{
	int reg;
	hdmi_av_composer();
	reg = hdmi_read(HDMI_PHY_CONF0);
        reg |= HDMI_PHY_CONF0_PDZ_MASK;
        hdmi_write(reg, HDMI_PHY_CONF0);
        udelay(3000);
        reg |= HDMI_PHY_CONF0_ENTMDS_MASK;
        hdmi_write(reg, HDMI_PHY_CONF0);
        udelay(3000);
        reg |= HDMI_PHY_CONF0_GEN2_TXPWRON_MASK;
        hdmi_write(reg, HDMI_PHY_CONF0);
        hdmi_write(HDMI_MC_PHYRSTZ_DEASSERT, HDMI_MC_PHYRSTZ);
	udelay(1000);
	hdmi_write(HDMI_MC_PHYRSTZ_ASSERT, HDMI_MC_PHYRSTZ);
	return TRUE;

}

int configure_audio(){

/*Configuring FC_AUD0-FC_AUD3 registers */
	hdmi_write(1<< HDMI_FC_AUDICONF0_CC_OFFSET, HDMI_FC_AUDICONF0); /*sterio(2 channels) */
	hdmi_write(0x13, HDMI_FC_AUDICONF1);/*Configuring sampling frequency(48k) and sample size(16) */
/*Configuring fixed audio data to the frame composer debug channels */
	hdmi_write(0xff, HDMI_FC_DBGAUD0CH0);
	hdmi_write(0xff, HDMI_FC_DBGAUD1CH0);
	hdmi_write(0xff, HDMI_FC_DBGAUD2CH0);
	hdmi_write(0xff, HDMI_FC_DBGAUD0CH1);
	hdmi_write(0xff, HDMI_FC_DBGAUD1CH1);
	hdmi_write(0xff, HDMI_FC_DBGAUD2CH1);
 	return 1;	
}

static int do_audio()
{
	int flag;
	char choice;
	if(!detect_hdmi()) {
		printf("No active HDMI panel detected\n");
		return -1;
			}
	if(!enable_hdmi_tx()) {
		printf("Unable to configure HDMI Transmiter controller\n");
		return -1;
			}
	if(!configure_audio()) {
		printf("Unable to configure audio parameters of HDMI transmitter controller\n");
		return -1;
			}
/* HDMI TX controller can force a color onto panel by writing RGB component 
 * values on to TMDS0, TMDS1 and TMDS2 channels respectively
 */	
	flag = 1;
/* Enable Force video and audio in frame composer */
	hdmi_write(0x11 , HDMI_FC_DBGFORCE);
	while(flag){
	printf("Enter your color choice to fill screen\n");
	printf("B for Blue\n");		/* TMDS0 channel */
	printf("G for Green\n");	/* TMDS1 channel */
	printf("R for Red\n");		/* TMDS2 channel */
	printf("W for white\n");	/* All TMDS channels */
	printf("X for Exit\n");		/* exit from here */
	choice = getc();
	printf("choice is %c\n", choice);
	hdmi_write(0, HDMI_FC_DBGTMDS0);
	hdmi_write(0, HDMI_FC_DBGTMDS1);
	hdmi_write(0, HDMI_FC_DBGTMDS2);
	
		switch(choice)
		{
			case 'B':
			case 'b': hdmi_write(0xff , HDMI_FC_DBGTMDS0);
				break;
			case 'G':
			case 'g': hdmi_write(0xff , HDMI_FC_DBGTMDS1);
				break;

			case 'R':
			case 'r': hdmi_write(0xff , HDMI_FC_DBGTMDS2);
				  break;
			case 'W':
			case 'w': hdmi_write(0xff , HDMI_FC_DBGTMDS0);
				  hdmi_write(0xff , HDMI_FC_DBGTMDS1);
				  hdmi_write(0xff , HDMI_FC_DBGTMDS2);
				break;	
			case 'x':
			case 'X': flag = 0;
				  hdmi_write(0x00 , HDMI_FC_DBGFORCE);
				break;
			default: printf("Invalid option\n");

		}//switch
		
	}//while
}//end of do_hdmi


U_BOOT_CMD( hdmi_audio_min,     1,      1,      do_audio,
        "HDMI test command to fixes continuous audio data forcebly",
        "This test uses Frame composer debug registers to force a fixed audio to listen via HDMI"
);

