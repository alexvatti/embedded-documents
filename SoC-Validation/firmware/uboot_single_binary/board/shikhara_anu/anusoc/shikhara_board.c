/*
 * Just to satisfy init routines..
 */

#include <common.h>
#include <asm/arch/nand.h>
#include <netdev.h>
#include <sound.h>
#include <configs/anusoc.h>
#include <lcd.h>
#include <amba_clcd.h>
#include <asm/arch/shikhara_smc.h>
#include <asm/arch/shikhara_wdt.h>
#include <asm/arch/shikhara_clock.h>
#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_dwmmc.h>
#include <asm/arch/shikhara_can.h>
#include <asm/arch/shikhara_i2c.h>

void sound_parameter_init(void);

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_BL1_BUILD

#ifdef CONFIG_SHIKHARA 

#ifdef CONFIG_LCD

u32 x;
#define LCDREG(x)	        	  	(u32)((x)+ SHIKHARA_CLCD_BASE)
#define LCDTIM0              			LCDREG(0x00000000)
#define LCDTIM1             			LCDREG(0x00000004)
#define LCDTIM2              			LCDREG(0x00000008)
#define LCDTIM3         	     		LCDREG(0x0000000C)
#define LCDUPBASE               		LCDREG(0x00000010)
#define LCDLPBASE               		LCDREG(0x00000014)
#define LCDCNTL		              		LCDREG(0x00000018)
#define LCDIMSC                 		LCDREG(0x0000001C)
#define LCDRIS                  		LCDREG(0x00000020)
#define LCDMIS                  		LCDREG(0x00000024)
#define LCDICR                  		LCDREG(0x00000028)
#define LCDUPCURR               		LCDREG(0x0000002C)
#define LCDLPCURR               		LCDREG(0x00000030)
#define LCDPALETTE              		LCDREG(0x00000200)

/*
 * 'panel_info' structure for CLCD PL111, 'panel_info2' structure for MIPI-DSI Transmitter 
 * controller and DSI bridge and 'panel_info3' structure for HDMI Transmitter controller 
 * and HDMI bridge carries the respective video parameters.
 * ".endianess" field in panel_info1 structure, 0 - Little Endian format, 1 - BigEndian.
 */

vidinfo_t *panel_info1, panel_pl111[]={
/* Video parameters calculated for 640x480, @60Hz, Pixel Clock=23.75MHz (Selftest, Ref. epanorama.net, Innolux TFT panel datasheet) 
 * Planed to used When CLCD controller is cortile peripheral
 */
	{
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       45-1,
        .vl_hfp       =       60-1,
        .vl_vbp       =       14,
        .vl_vfp       =       10,
        .vl_hsync_len =       35-1,
        .vl_vsync_len =       4-1,
        .vl_bpix      =       8,
        .endianess    =       0,
        .pixclock     =       23750000
        },

/* Video parameters calculated for 640x480, @60Hz, Pixel Clock=24MHz (Ref. epanorama.net, Innolux TFT panel datasheet) */	
	{
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       52-1,
        .vl_hfp       =       50-1,
        .vl_vbp       =       31,
        .vl_vfp       =       11,
        .vl_hsync_len =       22-1,
        .vl_vsync_len =       2-1,
        .vl_bpix      =       24,     
        .endianess    =	      0,
	.pixclock     =       24000000
	},
/* Video parameters calculated for 320x240, @60Hz, Pixel Clock=6MHz (Ref. epanorama.net, Innolux TFT panel datasheet) */
	{
        .vl_col       =       320,
        .vl_row       =       240,
        .vl_hbp       =       16-1,
        .vl_hfp       =       16-1,
        .vl_vbp       =       12,
        .vl_vfp       =       15,
        .vl_hsync_len =       19-1,
        .vl_vsync_len =       3-1,
        .vl_bpix      =       24,
	.endianess    =       0,
        .pixclock     =       6000000  
	},
/* Video parameters calculated for 640x480, @60Hz, Pixel Clock=27MHz (Ref. epanorama.net, Innolux TFT panel datasheet) */


{
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       69-1,
        .vl_hfp       =       32-1,
        .vl_vbp       =       31,
        .vl_vfp       =       11,
        .vl_hsync_len =       117-1,
        .vl_vsync_len =       2-1,
        .vl_bpix      =       24,
        .endianess    =       0,
        .pixclock     =       27000000
        }


};
#if 0
/*LCD init will run only when user intaiate the lcd,so its moved to commnd base*/
struct clcd_config pl111_lcd_priv;

/*Function initilizing panel_info structure */
void clcd_parameter_init(void){
int bpp;
int env_bpp;
char *s = NULL;
unsigned long value, pcd_lo, pcd_hi;

#ifdef CONFIG_SHIKHARA_CORETILE_PERIPHERALS
        panel_info1 = &panel_pl111[0];
#else
	panel_info1 = &panel_pl111[3];
#endif
	/* Getting Bits per Pixel from the env variable */
	s = getenv("pl111_bpp_8_16_24");
	if(s){
	env_bpp = (int)simple_strtol(s, NULL, 10);
		if(env_bpp == 8 || env_bpp == 16 || env_bpp == 24){
			panel_info1->vl_bpix = env_bpp;
		}
	}

        pl111_lcd_priv.tim0 |= ((((panel_info1->vl_col/16)-1)<<2) | (panel_info1->vl_hsync_len<<8) | (panel_info1->vl_hfp<<16) | (panel_info1->vl_hbp<<24));

        pl111_lcd_priv.tim1 |= ((panel_info1->vl_row-1) | (panel_info1->vl_vsync_len<<10) | (panel_info1->vl_vfp<<16) | (panel_info1->vl_vbp<<24));

#ifdef CONFIG_SHIKHARA_ASIC
        
        /* Divider Calculation for generating Pixel clock Ex. for 640x480, 24MHz */
        value=((CLCDCLK/panel_info1->pixclock)-2);
        pcd_lo=value & 0x1F;
        pcd_hi=(value>>5) & 0x1F; 
	
	pl111_lcd_priv.tim2 |= (pcd_hi<<27) | (TIM2_IVS) | (TIM2_IHS) | ((panel_info1->vl_col-1)<<16) | pcd_lo;
#else
	pl111_lcd_priv.tim2 |=  (TIM2_BCD) | (TIM2_IVS) | (TIM2_IHS) | ((panel_info1->vl_col-1)<<16);
#endif        
	pl111_lcd_priv.tim3 |= (CLCD_LED) | (CLCD_LEE<<16);

	/* when vl_bpix == 16, CLCD PL111 is configured 6 to meet RGB565 */
		if(panel_info1->vl_bpix == 8)
			bpp = 3;
		else if(panel_info1->vl_bpix == 16)
			bpp = 6;
		else
			bpp = 5;
        
	pl111_lcd_priv.cntl |= (CNTL_LCDEN) | (bpp<<1) | (CNTL_LCDTFT) | CNTL_LCDVCOMP(0) | ((panel_info1->endianess)<<9) | (CNTL_BGR) | (CNTL_WATERMARK);
	
	panel_info1->priv=(void*)&pl111_lcd_priv;
        pl111_lcd_priv.address= (struct clcd_registers *) SHIKHARA_CLCD_BASE;
/*Debugging LCD timing registers */ 
	printf("DEBUG_LCD: clcd base address is %x\n", pl111_lcd_priv.address);
	printf("DEBUG_LCD: lcd parameter tim0 is %x\n", pl111_lcd_priv.tim0);
	printf("DEBUG_LCD: lcd parameter tim1 is %x\n", pl111_lcd_priv.tim1);
	printf("DEBUG_LCD: lcd parameter tim2 is %x\n", pl111_lcd_priv.tim2);
	printf("DEBUG_LCD: lcd parameter tim3 is %x\n", pl111_lcd_priv.tim3);

}

#ifdef CONFIG_LCD_INFO
void lcd_show_board_info(void)
{
        lcd_printf ("Board Name: Shikhara\n");
        lcd_printf ("SoC       : AnuSoC \n");
        lcd_printf ("www.moschip.com\n");
}
#endif /* CONFIG_LCD_INFO*/


#endif /* CONFIG_PL111 */
#endif

#ifdef CONFIG_DSI
vidinfo_t *panel_info2, panel_dsi[] = {

/* Video parameters calculated for 320x240, @60Hz, Pixel Clock=5.6MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook)
 * MIPI-DSI MIPI-CSI LoopBack Test
 */
	{
	.vl_col       =       320,
        .vl_row       =       240,
        .vl_hbp       =       20,
        .vl_hfp       =       20,
        .vl_vbp       =       2,
        .vl_vfp       =       2,
        .vl_hsync_len =       20,
        .vl_vsync_len =       2,
        .vl_bpix      =       24,
        .pixclock     =       5600000,
        .priv         =       NULL
        },
/* Video parameters calculated for 640x480, @60Hz, Pixel Clock=20.2MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook) */
	{
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       24,
        .vl_hfp       =       10,
        .vl_vbp       =       2,
        .vl_vfp       =       2,
        .vl_hsync_len =       18,
        .vl_vsync_len =       2,
        .vl_bpix      =       24,
        .pixclock     =       20200000,
        .priv         =       NULL
	}

};

/*
 * video parameters for MIPI-DSI/DSI Bridge as per verification code. ?  
 */
void dsi_parameter_init(){

/* Default configuration 640x480 @60Hz Pixel Clock 20.2 MHz*/
        panel_info2 = &panel_dsi[0];


}
#endif /* CONFIG_DSI */

#ifdef CONFIG_DSI_TRULY
vidinfo_t *panel_info2, panel_dsi[] = {

/* Video parameters calculated for 320x240, @60Hz, Pixel Clock=5.6MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook)
 * MIPI-DSI MIPI-CSI LoopBack Test
 */
     {
        .vl_col       =       800,	//320,
        .vl_row       =       1280,	//240,
        .vl_hbp       =       24,	//20,
        .vl_hfp       =       72,	//20,
        .vl_vbp       =       10,	//2,
        .vl_vfp       =       12,	//2,
        .vl_hsync_len =       24,	//20,
        .vl_vsync_len =       2,
        .vl_bpix      =       24,
        .pixclock     =       71900000,	//5600000,
        .priv         =       NULL
     },

/* Video parameters calculated for 640x480, @60Hz, Pixel Clock=20.2MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook) */
        {
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       24,
        .vl_hfp       =       10,
        .vl_vbp       =       2,
        .vl_vfp       =       2,
        .vl_hsync_len =       18,
        .vl_vsync_len =       2,
        .vl_bpix      =       24,
        .pixclock     =       20200000,
        .priv         =       NULL
        }

};

/*
 * video parameters for MIPI-DSI/DSI Bridge as per verification code. ?  
 */
void dsi_parameter_init(){

/* Default configuration 640x480 @60Hz Pixel Clock 20.2 MHz*/
        panel_info2 = &panel_dsi[0];

}
#endif /* CONFIG_DSI_TRULY */

#ifdef CONFIG_HDMI
vidinfo_t *panel_info3, panel_hdmi[] = {
	{
	.vl_freq      =       60,    //refresh rate
        .vl_col       =       640,
        .vl_row       =       480,
        .vl_hbp       =       48,
        .vl_hfp       =       16,
        .vl_vbp       =       33,
        .vl_vfp       =       10,
        .vl_hsync_len =       96,
        .vl_vsync_len =       2,
        .vl_bpix      =       24,
	.pixclock     =       25200000,
        .priv         =       NULL 
	},

/* Video parameters calculated for 720x480, @60Hz, Pixel Clock=27MHz (Ref. epanorama.net, CEA_861-D Specification) */
	{
	.vl_freq      =       60,    
        .vl_col       =       720,
        .vl_row       =       480,
        .vl_hbp       =       60,
        .vl_hfp       =       16,
        .vl_vbp       =       30,
        .vl_vfp       =       9,
        .vl_hsync_len =       62,
        .vl_vsync_len =       6,
        .vl_bpix      =       24,
	.pixclock     =	      27000000,
        .priv         =       NULL 
	},
	
/* Video parameters calculated for 1920x1080, @30Hz, Pixel Clock=74.25MHz (Ref. epanorama.net, CEA_861-D Specification) */
        {
        .vl_freq      =       30,
        .vl_col       =       1920,
        .vl_row       =       1080,
        .vl_hbp       =       148,
        .vl_hfp       =       88,
        .vl_vbp       =       36,
        .vl_vfp       =       4, 
        .vl_hsync_len =       44,
        .vl_vsync_len =       5, 
        .vl_bpix      =       24,
	.pixclock     =       74250000,
        .priv         =       NULL

        }

};

void hdmi_parameter_init(){
	
/* Default configuration 720x480 @60Hz Pixel Clock 27MHz*/
#ifdef HDMI_720_480
	panel_info3 = &panel_hdmi[1];
#endif
#ifdef HDMI_640_480	
	panel_info3 = &panel_hdmi[0];
#endif

#if 0
	/* Pixel Clock = (vl_col + hbp + hfp + hsync_len) * (vl_row + vbp + vfp + vsync_len) * refresh_rate  */
	/* Calculating pixel clock for panel_hdmi[1] which is 720x480 resolution*/	
	unsigned int 		htotal, vtotal;
	htotal			 = panel_info3->vl_col + panel_info3->vl_hbp + panel_info3->vl_hfp + 
				   panel_info3->vl_hsync_len;
	vtotal			 = panel_info3->vl_row + panel_info3->vl_vbp + panel_info3->vl_vfp + 
				   panel_info3->vl_vsync_len;
	panel_hdmi->pixclock	 = htotal * vtotal * panel_info3->vl_freq;

#endif
}//end of hdmi_parameter_init

#endif /* CONFIG_HDMI */
#endif /* CONFIG_SHIKHARA */

sound_info sound_params;
void sound_parameter_init()
{
	sound_params.freq      = 48000;		/* Sample frequency */
	sound_params.bps       = 16;		/* Bits per Sample */
        sound_params.rfs       = 256;		/* LR clock frame size */
        sound_params.channels  = 2;		/* No of Audio Channels */
}		
 	 
int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_ANUSOC;

        gd->bd->bi_boot_params = (PHYS_SDRAM_1+0x100);

	icache_enable();
#if 0
/*LCD init will run only when user intaiate the lcd,so its moved to commnd base*/
	#ifdef CONFIG_PL111
	clcd_parameter_init();
	#endif
#endif
 
        #if defined(CONFIG_I2C_MULTI_BUS) && defined(CONFIG_SHIKHARA_PERI_EARLY_START)
        i2c_set_bus_num(I2C_BUS0);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
        i2c_set_bus_num(I2C_BUS1);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
        i2c_set_bus_num(I2C_BUS2);
        i2c_init(IC_FAST_SPEED,CONFIG_SYS_IC_SLAVE);
        #endif
     
	#ifdef CONFIG_DSI_TRULY
	dsi_parameter_init();
	#endif

	#ifdef CONFIG_HDMI
	hdmi_parameter_init();
	#endif
       
        #ifdef CONFIG_SOUND 
        sound_parameter_init();
     	#endif
        return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;

	return 0;
}
void reset_cpu(ulong addr)
{
	writel(2,SHIKHARA_WDT_BASE+0x08);	/*Enable Reset */
	writel(2,SHIKHARA_WDT_BASE);		/*Load Timeout */
}

int board_can_init(bd_t *bis)
{
       int rc = 0;
#ifdef CONFIG_CAN_SHIKHARA
       rc = shikhara_can_initialize(0);
#endif
       return rc;
}


#endif //CONFIG_BL1_BUILD
/**
 * smc_init_nor - Initialize the NOR flash interface of the SMC.
 *
 **/

#ifdef CONFIG_INIT_NOR
int smc_init_nor(void)
{
       //SMC0 clock reduceed from 133MHz to 100MHz
	writel(0x00000004, SHIKHARA_SYS_CTRL_BASEADDR+0x14);
        writel(NOR_SET_CYCLES , SHIKHARA_SMC_BASE + SMC_MC_SET_CYCLES);
        writel(NOR_SET_OPMODE , SHIKHARA_SMC_BASE + SMC_MC_SET_OPMODE);
	writel(0x00000005, SHIKHARA_SMC_BASE + SMC_MC_REFRESH_PERIOD_0); //Setting time interval between two bursts
        writel(NOR_DIRECT_CMD , SHIKHARA_SMC_BASE + SMC_MC_DIRECT_CMD);

          return(0);
}
#endif

/**
 * smc_init_sram - Initialize the SRAM interface of the SMC.
 *
 **/

#ifdef CONFIG_INIT_SRAM
int smc_init_sram(void)
{
       //SMC0 clock reduceed from 133MHz to 100MHz
	writel(0x00000004, SHIKHARA_SYS_CTRL_BASEADDR+0x14);
        writel(SRAM_SET_CYCLES , SHIKHARA_SMC_BASE + SMC_MC_SET_CYCLES);
        writel(SRAM_SET_OPMODE , SHIKHARA_SMC_BASE + SMC_MC_SET_OPMODE);
	writel(0x00000005, SHIKHARA_SMC_BASE + SMC_MC_REFRESH_PERIOD_0); //Setting time interval between two bursts
        writel(SRAM_DIRECT_CMD , SHIKHARA_SMC_BASE + SMC_MC_DIRECT_CMD);

         return(0);
}
#endif



int board_mmc_init(bd_t *bis)
{
#ifdef CONFIG_SHIKHARA_MMC
        shikhara_dwmmc_init(0);         /* MMC 0 Initialisation  for card 0*/
        shikhara_dwmmc_init(1);         /* MMC 0 Initialisation  for card 1*/
        shikhara_dwmmc_init(2);         /* MMC 1 Initialisation */
#endif
return 0;
}

/* Data Memory Barrier */
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")
#define SYNCHRONIZE_IO dmb()


void XIo_Out32(u32 OutAddress, u32 Value)
{
        *(volatile u32 *) OutAddress = Value;
        SYNCHRONIZE_IO;
}

u32 XIo_In32(u32 InAddress)
{
        volatile u32 temp = *(volatile u32 *)InAddress;
        SYNCHRONIZE_IO;
        return temp;
}



