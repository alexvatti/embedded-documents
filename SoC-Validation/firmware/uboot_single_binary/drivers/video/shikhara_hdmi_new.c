// All Rights Reserved
//***************************************************************************
//Module          : HDMI Transmitter controller 
//Title           : HDMI Transmitter Controller Driver 
//Project         : SHIKHARA
//Owner           : Kranthi
//Date Created    : 23/10/2013
//***************************************************************************
//Description : Driver does HDMI Transmitter controller and Phy Configuration  
//***************************************************************************
#include <asm/types.h>
#include <common.h>
#include <asm/arch/shikhara_hdmi.h>
#include <asm/arch/shikhara_hdmi_bridge.h>
#include <lcd.h>
#include "../lib/snps-hdmi/api/api.h"
#define Error -1

#ifdef CONFIG_HDMI_CONSOLE
#include <video_font.h>         /* Get font data, width and height */
#include <video_font_data.h>

int hdmi_is_enabled = 0;
void *hdmi_console_address;
short con_hdmi_col;
short con_hdmi_row;

#define CON_HDMI_ROWS           (panel_info3->vl_row / VIDEO_FONT_HEIGHT)
#define CON_HDMI_COLS                (panel_info3->vl_col / VIDEO_FONT_WIDTH)
#define CON_HDMI_ROW_SIZE            (VIDEO_FONT_HEIGHT * lcd_line_length3)
#define CON_HDMI_ROW_FIRST   (hdmi_console_address)
#define CON_HDMI_ROW_SECOND  (hdmi_console_address + CON_HDMI_ROW_SIZE)
#define CON_HDMI_ROW_LAST    (hdmi_console_address + CON_HDMI_SIZE \
                                        - CON_HDMI_ROW_SIZE)
#define CON_HDMI_SIZE                (CON_HDMI_ROW_SIZE * CON_HDMI_ROWS)
#define CON_HDMI_SCROLL_SIZE (CON_HDMI_SIZE - CON_HDMI_ROW_SIZE)
#endif
int shikhara_hdmi_init(void *lcdbase3);

/*
According to Synopsys HDMI Transmitter Controller User guide, Below steps are to be followed for initialise and configure 
HDMI 1.4b Tx Controller.

Step A: Wait for Hot Plug Detect(HPD)
Step B: Configure VGA (640x480)
Step C: Read Sinks EDID
Step D: Configure Video Mode
Step E: Configure Audio Mode
Step F: Configure Infoframes
Step G: Configure HDCP

*/

/*
CTS and N values are used by source and sink to get the sampling freq.
the values are extracted from below formula 
		128*fs = ftmds * N/CTS
*/
/*Macros for pll_clocks */
#define P_DIVIDOR	39	//(P=P[5:0]+1)
#define M_DIVIDOR	41	//(M=M[13:6]+1)
#define N_DIVIDOR	0	//(N=N[18:14]+1)
#ifdef CONFIG_HDMI_AUDIO
audioParams_t *pAudio, hdmi_audio_params ;
#endif
hdcpParams_t *pHdcp, hdcp_params;
productParams_t *pProduct, product_params;
videoParams_t *pVideo, hdmi_video_params = {
	.mHdmi = 0,
	.mEncodingOut = RGB,
	.mEncodingIn = RGB,
	.mColorResolution = 8,
	.mPixelRepetitionFactor = 0,
	.mRgbQuantizationRange = 0,
	.mPixelPackingDefaultPhase = 0,
	.mColorimetry = 0,
	.mScanInfo = 0,
	.mActiveFormatAspectRatio = 8,
	.mNonUniformScaling = 0,
	.mExtColorimetry = ~0,
	.mItContent = 0,
	.mEndTopBar = ~0,
	.mStartBottomBar = ~0,
	.mEndLeftBar = ~0,
	.mStartRightBar = ~0,
	.mCscFilter = 0,
	.mHdmiVideoFormat = 0,
	.m3dStructure = 0,
	.m3dExtData = 0,
	.mHdmiVic = 0
	
};
dtd_t *dtd, hdmi_dtd;
static u16 hdmi_base_addr = 0x8000;


int shikhara_hdmi_reinit(unsigned int color_choice, unsigned long lcdbase3)
{

	switch(color_choice)
	{
		case FORMAT_RGB:
			videoParams_SetEncodingIn(pVideo, RGB);
			videoParams_SetCscScale(pVideo, FALSE);
			break;
	/* There is no support for YUV420 in HDMI Tx, HDMI Bridge is converting YUV420 to YUV 422 format (see HDMI Bridge Doc) */
		case FORMAT_YUV_420:
		case FORMAT_YUV_422:
			videoParams_SetEncodingIn(pVideo, YCC422);
			videoParams_SetCscScale(pVideo, TRUE);
			break;

		case FORMAT_YUV_444:
			videoParams_SetEncodingIn(pVideo, YCC444);
			videoParams_SetCscScale(pVideo, TRUE);
			break;

	}
	shikhara_hdmi_init((void *)lcdbase3);
	return 0;

}

int shikhara_hdmi_uninit(void)
{
	videoParams_Reset(pVideo);
	shikhara_hdmi_init((void *)lcd_base3);
	lcd_clear();
	return 0;	
}
static void pixel_pll_config(void)
{

	writel(0x00000040,SHIKHARA_SYS_CTRL_BASEADDR+0x170 );	
#ifdef HDMI_640_480
	writel(0x0000030B,SHIKHARA_SYS_CTRL_BASEADDR+0x16c );  /* 25Mhz*/
#endif
#ifdef HDMI_720_480
	writel(0x0000034B,SHIKHARA_SYS_CTRL_BASEADDR+0x16c );
#endif
	writel(0x00000001,SHIKHARA_SYS_CTRL_BASEADDR+0x170 );
	udelay(1000);  

 //enabling HDMI clocks and resets.
/*	u32 reg32 = __raw_readl(PMU_CONFIG_ADDR);                //PMU_CONFIG_ADDR + SysControl Address
        reg32 = reg32 | 0x03E ;
        __raw_writel(reg32, PMU_CONFIG_ADDR);*/
 /*
  Pixel value need to be configured
  fpixel = fref * (1/N) * (M) * (1/P)
  fout = 25.2MHz and M=42, P = 40 and N = 1
  */






}

static int detect_hdmi(void)
{
        return (__raw_readl(HDMI_BASE_ADDR+HDMI_PHY_STAT0) & HDMI_PHY_HPD);//checking the HPD bit

}

static int video_params_init(void)
{
	dtd = &hdmi_dtd;
	pVideo = &hdmi_video_params;
#ifdef HDMI_720_480
	dtd_Fill(dtd, 2, 59940);
#endif
#ifdef HDMI_640_480
	dtd_Fill(dtd, 1, 59941);
#endif
	videoParams_SetDtd(pVideo, dtd);
	videoParams_SetColorimetry(pVideo, ITU601);
	videoParams_SetActiveFormatAspectRatio(pVideo, 0xA);	
#ifdef CONFIG_HDMI_AUDIO
	videoParams_SetHdmi(pVideo, TRUE);
#endif
	return TRUE;
}

#ifdef CONFIG_HDMI_AUDIO
static int audio_params_init(void)
{

	pAudio = &hdmi_audio_params;
	audioParams_Reset(pAudio);
	audioParams_SetInterfaceType(pAudio, DMA);
	audioParams_SetCodingType(pAudio, PCM);
	audioParams_SetChannelAllocation(pAudio, 0);
	audioParams_SetPacketType(pAudio, AUDIO_SAMPLE);
	audioParams_SetLevelShiftValue(pAudio, 0);
	audioParams_SetDownMixInhibitFlag(pAudio, 0);
	audioParams_SetClockFsFactor(pAudio, 128);
	audioParams_SetSamplingFrequency(pAudio, 48000);
#ifdef INSERT_AUTO_PCUV
	audioParams_SetDmaThreshold(pAudio, 0x10);
#else
	audioParams_SetDmaThreshold(pAudio, 126);
#endif
#ifdef INSERT_AUTO_PCUV
	/*IEC Header information*/
	audioParams_SetSampleSize(pAudio, 16);
	audioParams_SetIecCategoryCode(pAudio, 0);
	audioParams_SetOriginalSamplingFrequency(pAudio, 48000);
	audioParams_SetIecCgmsA(pAudio, 0);
	audioParams_SetIecCopyright(pAudio, 0);
	audioParams_SetIecSourceNumber(pAudio, 2);
#endif	
	audioParams_SetDmaBeatIncrement(pAudio, DMA_4_BEAT_INCREMENT);
	return TRUE;
}
#endif

int shikhara_hdmi_init(void *lcdbase3){

#ifdef CONFIG_HDMI_CONSOLE
        struct stdio_dev hdmidev;
#endif
	//writel(0x0000007E, SHIKHARA_SYS_CTRL_BASEADDR+0xCC);   /* Enable HDMI power */
        hdmi_power_off();
        udelay(2000);
        hdmi_power_on();
        udelay(2000);
	if(!detect_hdmi())
		{  
			printf("No active HDMI panel found\n");
			return Error;
		}
	printf("HDMI Panel Detected\n");	
#ifdef CONFIG_SHIKHARA_ASIC 
 	pixel_pll_config();
#endif	
	shikhara_hdmi_bridge_reset();
	if (shikhara_hdmi_bridge_init(lcdbase3)){
                printf("Error while configuring HDMI interface\n");
		return FALSE;
	        }
        else{
                printf("Shikhara Display controller initilized successfully\n");
                }
	
	video_params_init();
#ifdef CONFIG_HDMI_AUDIO
	audio_params_init();
#endif 

//	shikhara_hdmi_set_encoding(pVideo, RGB, FALSE);
//api_Initialize(u16 address, u8 dataEnablePolarity, u16 sfrClock, u8 force)
	if(!api_Initialize(hdmi_base_addr, TRUE, 27000, FALSE))
		printf("api_initialize failure\n");
	else 
		printf("HDMI initialized \n");
//api_Configure(pVideo, pAudio, pProduct, pHdcp)
#ifdef CONFIG_HDMI_AUDIO
	if(!api_Configure(pVideo, pAudio, FALSE, FALSE))
#else
	if(!api_Configure(pVideo, 0, 0, 0))
#endif
		printf("api_configure failure\n");
	else
		printf("HDMI configured Successfully\n");

#ifdef CONFIG_HDMI_CONSOLE
                /* Device initialization */
        hdmi_is_enabled = 1;
        hdmi_console_address = lcdbase3;
        con_hdmi_col = 0;
        con_hdmi_row = 0;

        memset (&hdmidev, 0, sizeof (hdmidev));

        strcpy (hdmidev.name, "hdmi");
        hdmidev.ext   = 0;                       /* No extensions */
        hdmidev.flags = DEV_FLAGS_OUTPUT;        /* Output only */
        hdmidev.putc  = hdmi_putc;                /* 'putc' function */
        hdmidev.puts  = hdmi_puts;                /* 'puts' function */

        stdio_register (&hdmidev);
#endif	

	return TRUE;
}//end of shikhara_hdmi_init

#ifdef CONFIG_HDMI_CONSOLE

static void con_hdmi_scrollup (void)
{
        /* Copy up rows ignoring the first one */
        memcpy (CON_HDMI_ROW_FIRST, CON_HDMI_ROW_SECOND, CON_HDMI_SCROLL_SIZE);

        /* Clear the last one */
        memset (CON_HDMI_ROW_LAST, COLOR_MASK(lcd_color_bg), CON_HDMI_ROW_SIZE);
}

static inline void con_hdmi_newline (void)
{
        ++con_hdmi_row;
        con_hdmi_col = 0;

        /* Check if we need to scroll the terminal */
        if (con_hdmi_row >= CON_HDMI_ROWS) {
                /* Scroll everything up */
                con_hdmi_scrollup () ;
                --con_hdmi_row;
        }
}



static inline void hdmi_putc_xy (ushort x, ushort y, uchar c){
	shikhara_lcd_drawchars_24_bit(x, y, &c, 1);
}

static inline void con_hdmi_back (void){
        if (--con_hdmi_col < 0) {
                con_hdmi_col = CON_HDMI_COLS-1 ;
                if (--con_hdmi_row < 0) {
                        con_hdmi_row = 0;
                }
        }

        hdmi_putc_xy (con_hdmi_col * VIDEO_FONT_WIDTH, con_hdmi_row * VIDEO_FONT_HEIGHT, ' ');
}


void hdmi_puts(const char *s){
        if (!hdmi_is_enabled) {
                serial_puts (s);
                return;
        }

        while (*s) {
                hdmi_putc (*s++);
        }
}

void hdmi_putc(const char c){
        if (!hdmi_is_enabled) {
                serial_putc (c);
                return;
        }

        switch (c) {
        case '\r':      con_hdmi_col = 0;
                        return;

        case '\n':      con_hdmi_newline();
                        return;

        case '\t':      /* Tab (8 chars alignment) */
                        con_hdmi_col +=  8;
                        con_hdmi_col &= ~7;

                        if (con_hdmi_col >= CON_HDMI_COLS) {
                                con_hdmi_newline();
                        }
                        return;

        case '\b':      con_hdmi_back();
                        return;

        default:        hdmi_putc_xy (con_hdmi_col * VIDEO_FONT_WIDTH,
                                     con_hdmi_row * VIDEO_FONT_HEIGHT,
                                     c);
                        if (++con_hdmi_col >= CON_HDMI_COLS) {
                                con_hdmi_newline();
                        }
                        return;
        }
}

#endif
