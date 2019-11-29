//***************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//***************************************************************************
//Module          : MIPI-DSI
//Title           : MIPI-DSI Controller Driver 
//Project         : SHIKHARA
//Owner           : Kaushal 
//Date Created    : 3/10/2013
//***************************************************************************
//Description : Driver does initialization of MIPI-DSI controller  
//***************************************************************************


#include <asm/arch/shikhara_map.h>
#include <asm/arch/shikhara_mipi_dsi_local.h>
#include <asm/arch/shikhara_mipi_dsi_hal.h>
#include <asm/arch/shikhara_pmu_sysctrl.h>
#include <common.h>
#include <malloc.h>
#include <lcd.h>
#include <asm/arch/shikhara_clock.h>

#ifdef CONFIG_DSI_CONSOLE
#include <video_font.h>		/* Get font data, width and height */
#include <video_font_data.h>

int dsi_is_enabled = 0;
void *dsi_console_address;
short con_col;
short con_row;

#define CON_ROWS           (panel_info2->vl_row / VIDEO_FONT_HEIGHT)
#define CON_COLS		(panel_info2->vl_col / VIDEO_FONT_WIDTH)
#define CON_ROW_SIZE		(VIDEO_FONT_HEIGHT * lcd_line_length2)
#define CON_ROW_FIRST	(dsi_console_address)
#define CON_ROW_SECOND	(dsi_console_address + CON_ROW_SIZE)
#define CON_ROW_LAST	(dsi_console_address + CON_SIZE \
					- CON_ROW_SIZE)
#define CON_SIZE		(CON_ROW_SIZE * CON_ROWS)
#define CON_SCROLL_SIZE	(CON_SIZE - CON_ROW_SIZE)

#endif

#define NUMBER_OF_CHUNKS                (0x8)
#define NULL_PKT_SIZE                   (0x8)


#undef MIPI_DSI_1000MBPS_LANE
#define MIPI_DSI_500MBPS_LANE
#undef  MIPI_DSI_450MBPS_LANE

#define DSI_PIXEL_CLOCK 71980 //80000 KHZ, 80 MHZ Time period=12.5 ns

#ifdef MIPI_DSI_1000MBPS_LANE
//Speed:1000Mbps/lane: Reg: PHY_TMR_CFG
#define PHY_DATA_LP2HS_TIME		0x6E //decimal:110 (bit pos:23..16)	
#define PHY_DATA_HS2LP_TIME		0x26 //decimal:38 (bit pos:32..24)

//Speed:1000Mbps/lane Reg: PHY_TMR_LPCLK_CFG
#define PHY_CLKLP2HS_TIME      0x85 //(bit pos:9..0)
#define PHY_CLKHS2LP_TIME      0x34 //(bit pos:25..16)

//Speed:1000Mbps/lane 
#define DSI_PHY_TEST_CTRL1_VALUE 0x74

//Lane Byte clcok @ Speed:1000Mbps/lane
#define DSI_LANE_BYTE_CLOCK    125000000 //125MHZ working
#endif

#ifdef MIPI_DSI_500MBPS_LANE
//Speed:500Mbps/lane  Reg:PHY_TMR_CFG
#define PHY_DATA_LP2HS_TIME		0x3E	 //decimal:62
#define PHY_DATA_HS2LP_TIME		0x19	 //devimal:25

//Speed:500Mbps/lane Reg:PHY_TMR_LPCLK_CFG
#define PHY_CLKLP2HS_TIME     0x4c //decimal:76 (bit pos:9..0)
#define PHY_CLKHS2LP_TIME     0x22 //decimal:34 (bit pos:25..16)

//Speed:500Mbps/lane 
#define DSI_PHY_TEST_CTRL1_VALUE 0x2c

#define DSI_LANE_BYTE_CLOCK       62500000 //62.5MHZ working(fix)
#endif

#ifdef MIPI_DSI_450MBPS_LANE
//Speed:450Mbps/lane  Reg:PHY_TMR_CFG
#define PHY_DATA_LP2HS_TIME             0x3A     //decimal:58
#define PHY_DATA_HS2LP_TIME             0x18     //devimal:24

//Speed:450Mbps/lane Reg:PHY_TMR_LPCLK_CFG
#define PHY_CLKLP2HS_TIME     0x46 //decimal:70 (bit pos:9..0)
#define PHY_CLKHS2LP_TIME     0x22 //decimal:34 (bit pos:25..16)

//Speed:450Mbps/lane 
#define DSI_PHY_TEST_CTRL1_VALUE 0x0c

#define DSI_LANE_BYTE_CLOCK       56250000 //56.25MHZ working(fix)
#endif


dphy_t phy[] ={

	/*DPHY Dummy initilization set--1*/
	{
	.address = DSI_BASE_ADDR, 
	.reference_freq = 27000, /* [KHz] */
	.status = NOT_INITIALIZED,
	.core_read_function = io_bus_read32,
	.core_write_function = io_bus_write32
	}

};


/** DPI Video parameter Structure. ?*/
dsih_dpi_video_t video[]={
/*
 * Video parameters calculated for 640x480, @60Hz, Pixel Clock=20.2MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook) 
 */
		{
                .no_of_lanes                    = 1,
                .virtual_channel                = 0,
                .video_mode                     = VIDEO_BURST_WITH_SYNC_PULSES,
                .max_hs_to_lp_cycles            = 50,
                .max_lp_to_hs_cycles            = 153,
                .max_clk_hs_to_lp_cycles        = 66,   
                .max_clk_lp_to_hs_cycles        = 181,
                .non_continuous_clock           = 0,
                .receive_ack_packets            = 0,
		.byte_clock                     = DSI_LANE_BYTE_CLOCK/1000,    //KHz
		.pixel_clock                    = 20200,    //KHz (As per the MIPI-DSI data sheet for 640x480 pixelclock should be 20200khz)
                .color_coding                   = COLOR_CODE_24BIT,
                .is_18_loosely                  = 0,
                .data_en_polarity               = 0,
                .h_polarity                     = 0,
                .h_active_pixels                = 640,
                .h_sync_pixels                  = 18,
                .h_back_porch_pixels            = 24,
                .h_total_pixels	           	= 692,
                .v_polarity                     = 0,    
                .v_active_lines                 = 480,
                .v_sync_lines                   = 2,
                .v_back_porch_lines             = 2,
                .v_total_lines                  = 486
		},
/* Video parameters calculated for 320x240, @60Hz, Pixel Clock=5.6MHz (Ref. epanorama.net, Synopsys MIPI-DSI IP databook) 
 * Planed to use with DSI-CSI LoopBack
 */
/*HX8260-A_DS_v01_141127.pdf  Page number 77 , Video input timing for 800X1280  taken Typical values*/
              {
                .no_of_lanes                    = 4, //number of lanes
                .virtual_channel                = 0, //virtual channel
                .video_mode                     = VIDEO_BURST_WITH_SYNC_PULSES,
                .max_hs_to_lp_cycles            = 50,
                .max_lp_to_hs_cycles            = 153,
                .max_clk_hs_to_lp_cycles        = 66,
                .max_clk_lp_to_hs_cycles        = 181,
                .non_continuous_clock           = 0,
                .receive_ack_packets            = 1,
                .byte_clock                     = DSI_LANE_BYTE_CLOCK/1000,    //KHz
                .pixel_clock                    = DSI_PIXEL_CLOCK,		//71980   //KHz (As per HX8260-A_DS_v01_141127.pdf  Page number 77 , Video input timing for 800X1280 )
                .color_coding                   = COLOR_CODE_24BIT,
                .is_18_loosely                  = 0,
                .data_en_polarity               = 0, 				/*Active High*/
                .h_polarity                     = 1, 				/*Active High*/
                .h_active_pixels                = 800, 				//800 Horizontal address
                .h_sync_pixels                  = 24, 				//24 Horizontal synchronization
                .h_back_porch_pixels            = 24, 				//24 Horizontal back porch
                .h_total_pixels                 = 920, 				//920 Horizontal total
                .v_polarity                     = 1, 				/*Active High*/
                .v_active_lines                 = 1280, 			//1280 Vertical address
                .v_sync_lines                   = 2,				//2 Vertical synchronization
                .v_back_porch_lines             = 10, 				//10 Vertical back porch
                .v_total_lines                  = 1304, 			//1304 Vertical total
                }
	
};

static dsih_error_t dphy_initilization(dphy_t *, dsih_dpi_video_t * );
u8 mipi_csi_init(void);
u8 mipi_dsi_bridge_init(void *);
/*Pointer to DSI Host controller context structure */
static dsih_ctrl_t *instance = 0;


static void shikhara_mipi_dsi_platform_init()
{
	int rdata = 0;

        //dsi_bridge_disable();
        
        //display_power_off();
	//udelay(10000);
        display_power_on();
	udelay(10000);
	printf("Shikhara MIPI DSI Platform PLL Configuration: System Controller\n");
#if 1
/* Configured as per Moschip ASIC Team */
/* SYS CTRL */
//Alex: Step 1: To pull the Reset/Soft Reset 
#if 0
	udelay(1000);
	writel(0x00000000,SHIKHARA_SYS_CTRL_BASEADDR + 0x98);

/* LPDDR, GIC Init */

	udelay(1000);
	writel(0x00000005,SHIKHARA_SYS_CTRL_BASEADDR + 0x54);
	udelay(1000);
#endif
/* DSI PLL Configuration */
//Alex: Step 2: PLL Configuration of MIPI using SYS CTRL
#if 1 //71.9MHZ
	writel(0x00000043,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);  /* dsipixelpll_cntrl_1 */
	writel(0x02C203C4,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL0);  /* 71.4 MHz Typical is 71.9MHz*/
	writel(0x00000004,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);  /* dsipixelpll_cntrl_1 */
	udelay(100);
	writel(0x00000004,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1); /* dsipixelpll_cntrl_1 */
	udelay(1000);

	rdata = readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);	// Reading LOCK STATUS Register

	while ((rdata & 0X00000604) != 0X00000604)	// Waiting for LOCK = 1'b1 (< 200us)
	{
		rdata = readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);	// Reading LOCK STATUS Register
	}
#endif

#if 0 //80MHZ
	writel(0x00000043,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);  /* dsipixelpll_cntrl_1 */
	writel(0X03CF89E2,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL0);  /* 80 MHz ,Typical is 71.9MHz*/
	//writel(0x004000DD,SHIKHARA_SYS_CTRL_BASEADDR + 0x174); /*5.6 MHz*/
	writel(0x00000003,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);  /* dsipixelpll_cntrl_1 */
	udelay(100);
	writel(0x00000003,SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1); /* dsipixelpll_cntrl_1 */
	udelay(1000);

	rdata = readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);	// Reading LOCK STATUS Register

	while ((rdata & 0X00000603) != 0X00000603)	// Waiting for LOCK = 1'b1 (< 200us)
	{
		rdata = readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1);	// Reading LOCK STATUS Register
	}
#endif

	printf("DSI_PIX_PLL_CTRL1 : DSI PLL LOCKS are proper\n");
	printf("DSI_PIX_PLL_CTRL0: 0x%x\n", readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL0));
	printf("DSI_PIX_PLL_CTRL1: 0x%x\n", readl(SHIKHARA_SYS_CTRL_BASEADDR + DSI_PIX_PLL_CTRL1));

	printf("dsicfgpll_cntrl_0: 0x%x\n", readl(SHIKHARA_SYS_CTRL_BASEADDR + 0x164));
	printf("dsicfgpll_cntrl_1: 0x%x\n", readl(SHIKHARA_SYS_CTRL_BASEADDR + 0x168));
	
	
#endif

//Alex: Step 2: PMU COnfiguration
/*	#define PMU_CONFIG_ADDR	0xd45610cc
	writel(0X007F,PMU_CONFIG_ADDR);*/
        /*display_power_off();
	udelay(10000);
        display_power_on();
	udelay(10000);*/
	printf("PMU Power is proper\n");
	printf("Shikhara MIPI DSI Platform PLL Configuration done Success: System Controller\n");
}

void shikhara_mipi_dsi_init(void *lcdbase2){

	int i;
#ifdef CONFIG_DSI_CONSOLE
	struct stdio_dev dsidev;
#endif

	dsih_error_t err = OK;
	u8 retval = NULL;
	u32 dsi_dphy_status=0;
	u32 reg_val;
	int rdata = 0;

	shikhara_mipi_dsi_platform_init();

/* DSI-CSI Loopback */	
#ifdef CONFIG_DSICSI_LOOPBACK
        if (mipi_csi_init()){
               	printf("LoopBack: Error while MIPI-CSI and CSI Bridge interface\n");
       	}
        else{
                printf("LoopBack: MIPI-CSI and CSI Bridge initilized successfully\n");
        }
#endif

	if (instance == 0){
		instance = (dsih_ctrl_t*) (malloc(sizeof(dsih_ctrl_t)));
	}

	/*Initilization of MIPI-DSI Host controller*/	
	instance->address = DSI_BASE_ADDR;
	instance->phy_instance = &phy[0];
	instance->max_lanes = 4; /* Max. No. of lanes  */
	instance->max_bta_cycles = 4095;
	instance->status = NOT_INITIALIZED;
	instance->color_mode_polarity = 0;
	instance->shut_down_polarity = 0;
	instance->core_read_function = io_bus_read32;
	instance->core_write_function = io_bus_write32;



	/*Power down/Reset MIPI-DSI Host core*/
	/*step 1: keeping core and d-phy in reset state */
	mipi_dsih_hal_power(instance, 0);

	/*step 2: D-PHY Module initilization*/
	err = dphy_initilization((instance->phy_instance), &video[1]);
	if( err == OK )
	{
		printf("DPHY Module initilized:Success\n");			
	}

	/*Initilization of DPI interface*/
	if (mipi_dsih_dpi_video(instance, &video[1]))
	{
		printf("Error while configuring DSI interface\n");
	}

	/*Masks the interrupt generation triggered by the INT_ST0 and INT_ST1 register, same as default value*/
	mipi_dsih_hal_int_mask_0(instance, 0x001FFFFF);
	mipi_dsih_hal_int_mask_1(instance, 0x001FFFFF);

#ifdef CONFIG_SHIKHARA_ASIC	
	err = mipi_dsih_phy_bta_time(instance, instance->max_bta_cycles);
	if (err)
	{
		printf("ERR_DSI_OVERFLOW");
	}
	/* by default, all commands are sent in LP */
	//mipi_dsih_hal_dcs_wr_tx_type(instance, 0, 1);
	//mipi_dsih_hal_dcs_wr_tx_type(instance, 1, 1);
	//mipi_dsih_hal_dcs_wr_tx_type(instance, 3, 1); /* long packet*/
	//mipi_dsih_hal_dcs_rd_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 1, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 2, 1);
	mipi_dsih_hal_gen_wr_tx_type(instance, 3, 1); /* long packet*/
	mipi_dsih_hal_gen_rd_tx_type(instance, 0, 1);
	mipi_dsih_hal_gen_rd_tx_type(instance, 1, 1);
	mipi_dsih_hal_gen_rd_tx_type(instance, 2, 1);
	//mipi_dsih_hal_cmd_ack_en(instance,1);
#else /*FPGA is using Loopback DSI and CSI*/
        mipi_dsih_hal_dcs_wr_tx_type(instance, 0, 0);
	mipi_dsih_hal_dcs_wr_tx_type(instance, 1, 0);
	mipi_dsih_hal_dcs_wr_tx_type(instance, 3, 0); /* long packet*/
	mipi_dsih_hal_dcs_rd_tx_type(instance, 0, 0);
	mipi_dsih_hal_gen_wr_tx_type(instance, 0, 0);
	mipi_dsih_hal_gen_wr_tx_type(instance, 1, 0);
	mipi_dsih_hal_gen_wr_tx_type(instance, 2, 0);
	mipi_dsih_hal_gen_wr_tx_type(instance, 3, 0); /* long packet*/
	mipi_dsih_hal_gen_rd_tx_type(instance, 0, 0);
	mipi_dsih_hal_gen_rd_tx_type(instance, 1, 0);
	mipi_dsih_hal_gen_rd_tx_type(instance, 2, 0);
#endif

	/* by default, RX_VC = 0, NO EOTp, EOTn, BTA, ECC rx and CRC rx */
	//Changed as per dsi_dpi_burst_test_36confg.c suggested by ASIC/FPGA Team
	mipi_dsih_hal_gen_rd_vc(instance, 0);//This field indicates the Generic interface read-back virt chan id.
    	writel(0xf000f0,DSI_BASE_ADDR + DSI_DPI_LP_CMD_TIM);	
      	

	mipi_dsih_hal_power(instance, 1);
       //It is MUST to read once after DSI controller init
	dsi_dphy_status = mipi_dsih_read_word(instance, DSI_PHY_STATUS);
	printf("DSI DPHY status is expected(0x1539) =%x\n", dsi_dphy_status);

	instance->status = INITIALIZED;

/* 
 *commands needs to send to the Panel for Panel initilization
 */
 

/*
 * Shikhara Display Controller Initilization
 */
	
	if (mipi_dsi_bridge_init(lcdbase2)){
		printf("Error while configuring DSI interface\n");
	}
	else{
		printf("Shikhara Display controller initilized successfully\n");
	}

	udelay(1000);
	mipi_lcd_init(); //panel initialization
#ifdef CONFIG_DSI_CONSOLE
		/* Device initialization */
	dsi_is_enabled = 1;
        dsi_console_address = lcdbase2;
	con_col = 0;
        con_row = 0;
	
	memset (&dsidev, 0, sizeof (dsidev));

	strcpy (dsidev.name, "dsi");
	dsidev.ext   = 0;			/* No extensions */
	dsidev.flags = DEV_FLAGS_OUTPUT;	/* Output only */
	dsidev.putc  = dsi_putc;		/* 'putc' function */
	dsidev.puts  = dsi_puts;		/* 'puts' function */

	stdio_register (&dsidev);


#endif
}


/*
* dsi phy reg write function
*/
static void dsi_phy_tst_set(u32 reg, u32 val)
{
	u32 reg_write = 0x10000 + reg;
	/*
	* latch reg first
	*/
	writel(reg_write, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL1);
	writel(0x02, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	writel(0x00, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	/*
	* then latch value
	*/
	writel(val, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL1);
	writel(0x02, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	writel(0x00, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	udelay(1000);
}

/*DPHY configuration*/
static dsih_error_t dphy_initilization(dphy_t *phy, dsih_dpi_video_t * video_params)
{

	dsih_error_t err_code;
	int readval=0;
	if ( phy == 0 ) 
	{
		printf("Instance not found\n");
	}

	printf("Shikhara MIPI DSI DPHY Initialization...Start\n");
	/* Reset D-Phy Module */	
	mipi_dsih_dphy_reset(phy, 0);

	/*  Set lane value and phy stop wait time. */

	/* Configure minimum wait period for HS transmission request after a stop state*/
	mipi_dsih_dphy_stop_wait_time(phy, 0x28);

	/* Configuring No. of active lanes in D-PHY */
	mipi_dsih_dphy_no_of_lanes(phy, video_params->no_of_lanes);


	/*  Set phy clk division.  */
	/* dividing by 6 is aimed for max PHY frequency, 1GHz ?? not sure, do check*/
	//mipi_dsih_hal_tx_escape_division(instance, 0x70);
	mipi_dsih_write_word(instance, DSI_CLKMGR_CFG, 0x1070); /*changed by Moschip ASIC team*/


	/* Set lp and hs switching params. */
	readval=readl(DSI_BASE_ADDR + DSI_PHY_TMR_CFG);
	writel((((PHY_DATA_HS2LP_TIME << 24 )|(PHY_DATA_LP2HS_TIME <<16))|readval),DSI_BASE_ADDR + DSI_PHY_TMR_CFG);
	writel(((PHY_CLKHS2LP_TIME << 16)| (PHY_CLKLP2HS_TIME)),DSI_BASE_ADDR + DSI_PHY_TMR_LPCLK_CFG);


	/* Reset to clean up phy tst params. */
	writel(0, DSI_BASE_ADDR + DSI_PHY_RSTZ);
	writel(0, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	writel(1, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);
	writel(0, DSI_BASE_ADDR + DSI_PHY_TEST_CTRL0);

	dsi_phy_tst_set(0x44,DSI_PHY_TEST_CTRL1_VALUE);

	writel(PHY_ENABLECLK, DSI_BASE_ADDR + DSI_PHY_RSTZ);
	udelay(10);
	writel(PHY_ENABLECLK | PHY_UNSHUTDOWNZ, DSI_BASE_ADDR + DSI_PHY_RSTZ);
	udelay(10);
	writel(PHY_ENABLECLK | PHY_UNRSTZ | PHY_UNSHUTDOWNZ, DSI_BASE_ADDR + DSI_PHY_RSTZ);
	udelay(1000);

	/* Enable HS clock */
        mipi_dsih_dphy_enable_hs_clk(phy, 1);
	//need to write the logic to write into phy_txrequestclkhs
	mipi_dsih_dphy_txrequestclkhs(phy,1);	
	phy->status = INITIALIZED;
	printf("Shikhara MIPI DSI DPHY Initialization...Success\n");
	return OK;
}


/*DPI configuration*/
dsih_error_t mipi_dsih_dpi_video(dsih_ctrl_t * instance, dsih_dpi_video_t * video_params)
{
	dsih_error_t err_code = OK;

	if (video_params->no_of_lanes > instance->max_lanes)
	{
		return ERR_DSI_OUT_OF_BOUND;
	}
	printf("Shikhara MIPI DSI DPI VIDEO Initialization...Start\n");
	/*set up ACKs and error reporting */
	mipi_dsih_hal_dpi_lp_cmd_en(instance,1); //added by Alex/FW Team for testing
	mipi_dsih_hal_dpi_frame_ack_en(instance, video_params->receive_ack_packets);
	mipi_dsih_hal_dpi_video_vc(instance, video_params->virtual_channel);
	/*24 bit color coding */
	mipi_dsih_hal_dpi_color_coding(instance, video_params->color_coding);

	/*polarities of signals*/	
	mipi_dsih_hal_dpi_hsync_pol(instance, (video_params->h_polarity));
	mipi_dsih_hal_dpi_vsync_pol(instance, (video_params->v_polarity));
	mipi_dsih_hal_dpi_dataen_pol(instance,(video_params->data_en_polarity));
	/*Color mode pin polarity*/
	mipi_dsih_hal_dpi_color_mode_pol(instance, (instance->color_mode_polarity));
	/*Shut Down pin polarity*/
	mipi_dsih_hal_dpi_shut_down_pol(instance, (instance->shut_down_polarity));


	mipi_dsih_hal_gen_eotp_rx_en(instance, 1);
	mipi_dsih_hal_gen_eotp_tx_en(instance, 1);
	mipi_dsih_hal_bta_en(instance, 1); ///changed by alex/fw team for testing
	mipi_dsih_hal_gen_ecc_rx_en(instance, 1);
	mipi_dsih_hal_gen_crc_rx_en(instance, 1);
	mipi_dsih_hal_dpi_video_mode_type(instance, video_params->video_mode);

	/* BURST by default, returns to LP during ALL empty periods - energy saving */
	mipi_dsih_hal_dpi_lp_during_hfp(instance, 1);
	//mipi_dsih_hal_dpi_lp_during_hbp(instance, 1); //not sure, yet to confirm it
	mipi_dsih_hal_dpi_lp_during_vactive(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vfp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vbp(instance, 1);
	mipi_dsih_hal_dpi_lp_during_vsync(instance, 1);     

	/*DSP Video mode enable*/
        mipi_dsih_hal_dpi_video_mode_en(instance, 1);

	if (video_params->video_mode == VIDEO_BURST_WITH_SYNC_PULSES)
	{ 
		err_code =  mipi_dsih_hal_dpi_video_packet_size(instance, video_params->h_active_pixels);
		err_code = mipi_dsih_hal_dpi_chunks_no(instance, 0);
		err_code = mipi_dsih_hal_dpi_null_packet_size(instance, 0);
		mipi_dsih_hal_dpi_null_packet_en(instance, 0);
		mipi_dsih_hal_dpi_multi_packet_en(instance, 0);
	
	}
	else if (video_params->video_mode == VIDEO_NON_BURST_WITH_SYNC_EVENTS)
    	{
		printf("In VIDEO_NON_BURST_WITH_SYNC_EVENTS mode \n");
                err_code =  mipi_dsih_hal_dpi_video_packet_size(instance, 8); //VID_PKT_SIZE = (12.5/8)*4 lanes*4 pixel=25
                err_code = mipi_dsih_hal_dpi_chunks_no(instance, 101); //num_chunks = HACT/vid_pkt_size = 800/4 = 200
                err_code = mipi_dsih_hal_dpi_null_packet_size(instance, 13); //Null_packet_overhead = 12
									    //vid_null_size (VID_NULL_SIZE) = Chunk_overhead - Null_packet_overhead = 7
                mipi_dsih_hal_dpi_null_packet_en(instance, 0);
                mipi_dsih_hal_dpi_multi_packet_en(instance, 0);

    	}

	printf("BYTE LANE Clock:%d, PIXEL CLOCK:%d\n",video_params->byte_clock,video_params->pixel_clock);
#if 0
	mipi_dsih_hal_dpi_hline(instance, (video_params->h_total_pixels * (video_params->byte_clock / video_params->pixel_clock)));
	mipi_dsih_hal_dpi_hbp(instance, (video_params->h_back_porch_pixels * (video_params->byte_clock / video_params->pixel_clock)));
	mipi_dsih_hal_dpi_hsa(instance, (video_params->h_sync_pixels * (video_params->byte_clock / video_params->pixel_clock)));
#endif

#ifdef MIPI_DSI_1000MBPS_LANE
	mipi_dsih_hal_dpi_hline(instance,1599); //1.5625 * 800= 1437.5
	mipi_dsih_hal_dpi_hbp(instance,41); //1.5625 * 24 = 37.5
	mipi_dsih_hal_dpi_hsa(instance,41); //1.5625 * 24 = 37.5
#endif

#ifdef MIPI_DSI_500MBPS_LANE
        mipi_dsih_hal_dpi_hline(instance,800);  //0.869 * 920
        mipi_dsih_hal_dpi_hbp(instance,21);    //0.869 * 24
        mipi_dsih_hal_dpi_hsa(instance,21);   //0.869 * 24 
#endif

#ifdef MIPI_DSI_450MBPS_LANE
        mipi_dsih_hal_dpi_hline(instance,718); //0.78 *920 = 718
        mipi_dsih_hal_dpi_hbp(instance,19); //0.78 * 24 = 19
        mipi_dsih_hal_dpi_hsa(instance,19); //0.78 * 24 = 19
#endif

	mipi_dsih_hal_dpi_vactive(instance, video_params->v_active_lines);
	mipi_dsih_hal_dpi_vbp(instance, video_params->v_back_porch_lines);
	mipi_dsih_hal_dpi_vsync(instance, video_params->v_sync_lines);
	mipi_dsih_hal_dpi_vfp(instance, video_params->v_total_lines - (video_params->v_back_porch_lines + video_params->v_sync_lines + video_params->v_active_lines));

	//newly added for testing , yet to confirm it
	#define hstx_to_cnt 1000
	#define lprx_to_cnt 1000
	writel(hstx_to_cnt << 16 | lprx_to_cnt ,DSI_BASE_ADDR + DSI_TO_CNT_CFG);

	if(err_code != OK)
		printf("failed in mipi_dsih_dpi_video() \n");

	printf("Shikhara MIPI DSI DPI VIDEO Initialization...Success\n");
        return err_code;
}

#ifdef CONFIG_DSI_CONSOLE

static void con_scrollup (void)
{
	/* Copy up rows ignoring the first one */
	memcpy (CON_ROW_FIRST, CON_ROW_SECOND, CON_SCROLL_SIZE);

	/* Clear the last one */
	memset (CON_ROW_LAST, COLOR_MASK(lcd_color_bg), CON_ROW_SIZE);
}


static inline void con_newline (void)
{
	++con_row;
	con_col = 0;

	/* Check if we need to scroll the terminal */
	if (con_row >= CON_ROWS) {
		/* Scroll everything up */
		con_scrollup () ;
		--con_row;
	}
}


static inline void dsi_putc_xy (ushort x, ushort y, uchar c){
	shikhara_lcd_drawchars_24_bit(x, y, &c, 1);
}

static inline void con_back (void){
	if (--con_col < 0) {
		con_col = CON_COLS-1 ;
		if (--con_row < 0) {
			con_row = 0;
		}
	}

	dsi_putc_xy (con_col * VIDEO_FONT_WIDTH, con_row * VIDEO_FONT_HEIGHT, ' ');
}


void dsi_puts(const char *s){
	if (!dsi_is_enabled) {
		serial_puts (s);
		return;
	}

	while (*s) {
		dsi_putc (*s++);
	}
}

void dsi_putc(const char c){
	if (!dsi_is_enabled) {
		serial_putc (c);
		return;
	}

	switch (c) {
	case '\r':	con_col = 0;
			return;

	case '\n':	con_newline();
			return;

	case '\t':	/* Tab (8 chars alignment) */
			con_col +=  8;
			con_col &= ~7;

			if (con_col >= CON_COLS) {
				con_newline();
			}
			return;

	case '\b':	con_back();
			return;

	default:	dsi_putc_xy (con_col * VIDEO_FONT_WIDTH,
				     con_row * VIDEO_FONT_HEIGHT,
				     c);
			if (++con_col >= CON_COLS) {
				con_newline();
			}
			return;
	}
}

#endif

#if 0
void do_shikhara_mipi_dsi_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
#endif
void do_dsi_enable(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int *lcdbase2=CONFIG_MIPI_DSI_FB2_ADDR;
	shikhara_mipi_dsi_init(lcdbase2);
}

#if 0
U_BOOT_CMD(shikhara_mipi_dsi_init,     1,      1,      do_shikhara_mipi_dsi_init,
        "To initialize MIPI DSI Host",
        " "
        " "
);
#endif
U_BOOT_CMD(dsi_enable,     1,      1,      do_dsi_enable,
        "To initialize MIPI DSI Host & Truly lcd Panel",
        " "
        " "
);
