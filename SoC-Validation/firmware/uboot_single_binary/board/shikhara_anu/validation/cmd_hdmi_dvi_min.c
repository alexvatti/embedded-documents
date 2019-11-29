#include <common.h>
#include <command.h>
#include<asm/io.h>
#include <asm/arch/shikhara_hdmi.h>


static u8 detect_hdmi(void){
	return hdmi_readb(HDMI_PHY_STAT0) & HDMI_PHY_HPD;
}

int hdmi_post()
{
	if(detect_hdmi() == 0)
	{
		printf("HDMI Panel not detected, Please connect HDMI cable\n");
		return -1;
	}
	else
	{
		printf("HDMI Panel detected\n");
		return 0;
	}

}

static int enable_hdmi_tx(void)
{
	int reg;
	reg = hdmi_readb(HDMI_PHY_CONF0);
        reg |= HDMI_PHY_CONF0_PDZ_MASK;
        hdmi_writeb(reg, HDMI_PHY_CONF0);
        udelay(3000);
        reg |= HDMI_PHY_CONF0_ENTMDS_MASK;
        hdmi_writeb(reg, HDMI_PHY_CONF0);
        udelay(3000);
        reg |= HDMI_PHY_CONF0_GEN2_TXPWRON_MASK;
        hdmi_writeb(reg, HDMI_PHY_CONF0);
	hdmi_writeb(HDMI_MC_PHYRSTZ_ASSERT, HDMI_MC_PHYRSTZ);
	udelay(1000);
        hdmi_writeb(HDMI_MC_PHYRSTZ_DEASSERT, HDMI_MC_PHYRSTZ);
	return TRUE;

}

static int do_hdmi()
{
	int flag;
	char choice;
	if(!detect_hdmi()) {
		printf("No active HDMI panel detected\n");
		return -1;
			}
#if 0
	if(!enable_hdmi_tx()) {
		printf("Unable to activate HDMI Transmiter controller\n");
		return -1;
			}
#endif
/* HDMI TX controller can force a color onto panel by writing RGB component 
 * values on to TMDS0, TMDS1 and TMDS2 channels respectively
 */	
	flag = 1;
/* Enable Force video in frame composer */
	hdmi_writeb(0x01 , HDMI_FC_DBGFORCE);
	while(flag){
	printf("Enter your color choice to fill screen\n");
	printf("B for Blue\n");		/* TMDS0 channel */
	printf("G for Green\n");	/* TMDS1 channel */
	printf("R for Red\n");		/* TMDS2 channel */
	printf("W for white\n");	/* All TMDS channels */
	printf("X for Exit\n");		/* exit from here */
	choice = getc();
	printf("choice is %c\n", choice);
	hdmi_writeb(0, HDMI_FC_DBGTMDS0);
	hdmi_writeb(0, HDMI_FC_DBGTMDS1);
	hdmi_writeb(0, HDMI_FC_DBGTMDS2);
	
		switch(choice)
		{
			case 'B':
			case 'b': hdmi_writeb(0xff , HDMI_FC_DBGTMDS0);
				break;
			case 'G':
			case 'g': hdmi_writeb(0xff , HDMI_FC_DBGTMDS1);
				break;

			case 'R':
			case 'r': hdmi_writeb(0xff , HDMI_FC_DBGTMDS2);
				  break;
			case 'W':
			case 'w': hdmi_writeb(0xff , HDMI_FC_DBGTMDS0);
				  hdmi_writeb(0xff , HDMI_FC_DBGTMDS1);
				  hdmi_writeb(0xff , HDMI_FC_DBGTMDS2);
				break;	
			case 'x':
			case 'X': flag = 0;
				  hdmi_writeb(0x00 , HDMI_FC_DBGFORCE);
				break;
			default: printf("Invalid option\n");

		}//switch
		
	}//while
}//end of do_hdmi


U_BOOT_CMD(
        hdmi_dvi_min,     1,      1,      do_hdmi,
        "HDMI test command like fill with a color",
        "Used to test HDMI TMDS channels"
        "This test uses Frame composer debug registers to force a fixed color on panel\n"
);

