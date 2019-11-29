//**************************************************************************************************
// Copyright (c) 2013 MosChip SemiConductor Technology Limited
// This design is confidential and proprietary of MosChip.
// All Rights Reserved
//**************************************************************************************************
//	Module          : MIPI-CSI2
//	Title           : MIPI-CSI2 Controller Driver 
//	Project         : SHIKHARA
//	Owner           : Kaushal 
//	Date Created    : 3/10/2013
//***************************************************************************************************
// Description : Initialization of MIPI-CSI2 receiver controller and CSI Bridge (Camera Controller).  
//***************************************************************************************************

#include <asm/types.h>
#include <asm/arch/shikhara_mipi_csi.h>
#include <common.h>
#include <asm/arch/shikhara_pmu_sysctrl.h>
/*
 * @brief: mipi_csi_init() --- Function to initilize MIPI-CSI2 Module.
 * @return: 0 if success else 1.
 */
u8 mipi_csi_shut_down_phy(u8); 
u8 mipi_csi_reset_phy(void);
int mipi_csi_bridge_init(void);

#define OV5640_NO_LANES 2
#define MBPS_PER_LANE  224 //112
/*
* The required sequence of MIPI CSI-2 startup as specified in the i.MX6
* reference manual is as follows:
*
* 1. Deassert presetn signal (global reset).
* It's not clear what this "global reset" signal is (maybe APB
* global reset), but in any case this step would be probably
* be carried out during driver load in csi2_probe().
*
* 2. Configure MIPI Camera Sensor to put all Tx lanes in LP-11 state.
* This must be carried out by the MIPI sensor's s_power(ON) subdev
* op.
*
* 3. D-PHY initialization.
* 4. CSI2 Controller programming (Set N_LANES, deassert PHY_SHUTDOWNZ,
* deassert PHY_RSTZ, deassert CSI2_RESETN).
* 5. Read the PHY status register (PHY_STATE) to confirm that all data and
* clock lanes of the D-PHY are in LP-11 state.
* 6. Configure the MIPI Camera Sensor to start transmitting a clock on the
* D-PHY clock lane.
* 7. CSI2 Controller programming - Read the PHY status register (PHY_STATE)
* to confirm that the D-PHY is receiving a clock on the D-PHY clock lane.
*
* All steps 3 through 7 are carried out by csi2_s_stream(ON) here. Step
* 6 is accomplished by calling the source subdev's s_stream(ON) between
* steps 5 and 7.
*/

/*
* This table is based on the table documented at
* https://community.nxp.com/docs/DOC-94312. It assumes
* a 27MHz D-PHY pll reference clock.
*/
static const struct {
	u32 max_mbps;
	u32 hsfreqrange_sel;
} hsfreq_map[] = {
	{ 90, 0x00}, {100, 0x20}, {110, 0x40}, {125, 0x02},
	{140, 0x22}, {150, 0x42}, {160, 0x04}, {180, 0x24},
	{200, 0x44}, {210, 0x06}, {240, 0x26}, {250, 0x46},
	{270, 0x08}, {300, 0x28}, {330, 0x48}, {360, 0x2a},
	{400, 0x4a}, {450, 0x0c}, {500, 0x2c}, {550, 0x0e},
	{600, 0x2e}, {650, 0x10}, {700, 0x30}, {750, 0x12},
	{800, 0x32}, {850, 0x14}, {900, 0x34}, {950, 0x54},
	{1000, 0x74},
};

static int max_mbps_to_hsfreqrange_sel(u32 max_mbps)
{
int i;
	for (i = 0; i < ARRAY_SIZE(hsfreq_map); i++)
	if (hsfreq_map[i].max_mbps > max_mbps)
	return hsfreq_map[i].hsfreqrange_sel;
	return -1;
}

static void dw_mipi_csi2_phy_write(u32 test_code, u32 test_data)
{


	/* Latest code provided by Synopsys */
	writel(0x00000001, CSI2_PHY_TEST_CTRL0);
	// TestCLR = '1'
	writel(0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestCLR = '0'
	writel(0x00000044, CSI2_PHY_TEST_CTRL1);
	// TestDIN = '44'
	writel(0x00010000, CSI2_PHY_TEST_CTRL1);
	// TestEN = '1'
	writel(0x00000002, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '1'
	writel(0x00010000, CSI2_PHY_TEST_CTRL1);
	// TestEN = '1'
	writel(0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '0'
	writel(0x00010000, CSI2_PHY_TEST_CTRL1);
	// TestEN = '1'
	writel(0x00000000, CSI2_PHY_TEST_CTRL1);
	// TestEN = '0'
	writel(test_data, CSI2_PHY_TEST_CTRL1);
	// TestDIN = '08'
	writel(0x00000002, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '1'
	writel(0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '0'
#if 0
	/* Clear PHY test interface */
	writel(PHY_TESTCLR, CSI2_PHY_TEST_CTRL0);
	writel(0x0, CSI2_PHY_TEST_CTRL1);
	writel(0x0,  CSI2_PHY_TEST_CTRL0);

	/* Raise test interface strobe signal */
	writel(PHY_TESTCLK, CSI2_PHY_TEST_CTRL0);

	/* Configure address write on falling edge and lower strobe signal */
	writel(PHY_TESTEN | test_code, CSI2_PHY_TEST_CTRL1);
	writel(0x0, CSI2_PHY_TEST_CTRL0);

	/* Configure data write on rising edge and raise strobe signal */
	writel(test_data, CSI2_PHY_TEST_CTRL1);
	writel(PHY_TESTCLK, CSI2_PHY_TEST_CTRL0);
	/* Clear strobe signal */
	writel(0x0, CSI2_PHY_TEST_CTRL0);
#endif

#if 0
	writel (0x00000004, CSI2_PHY_TEST_CTRL0);
	// TestCLR = '1'
	writel (0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestCLR = '0'
	writel (0x00000044, CSI2_PHY_TEST_CTRL1);
	// TestDIN = '44'
	writel (0x00000001, CSI2_PHY_TEST_CTRL0);
	// TestEN = '1'
	writel (0x00000003, CSI2_PHY_TEST_CTRL0);
	// TestEN = '1' and TestCLK = '1'
	writel (0x00000001, CSI2_PHY_TEST_CTRL0);
	// TestEN = '1' and TestCLK = '0'
	writel (0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestEN = '0' and TestCLK = '0'
	writel (0x00000008, CSI2_PHY_TEST_CTRL1);
	// TestDIN = '08'
	writel (0x00000002, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '1'
	writel (0x00000000, CSI2_PHY_TEST_CTRL0);
	// TestCLK = '0'

#endif
}

void Set_pixel_format(int format,int vchannel)
{
	int mipidata=0;
	
        switch(format)
        {
                case 1:
                        mipidata=(vchannel << 6) | (vchannel << 14);
                        writel((mipidata | 0x00001e1e),CSI2_DATA_IDS_1);  //YUV422
                        break;
                case 2:
                        mipidata=(vchannel << 6) | (vchannel << 14);
                        writel( (mipidata | 0x00002222),CSI2_DATA_IDS_1);  //RGB565
                        break;
                default:
                        printf("Invalid Pixel Format selected\n");
        }
}

void csi_enable(int enable)
{
	if(enable == 1) {
		mipi_csi_core_write_part(CSI2_PHY_SHUTDOWNZ, 0? 0: 1, 0, 1);
		mipi_csi_core_write_part(CSI2_DPHY_RSTZ, 1, 0, 1);
		mipi_csi_core_write_part(CSI2_RESETN, 1, 0, 1);
	}
	if(enable == 0) {
		mipi_csi_core_write_part(CSI2_PHY_SHUTDOWNZ, 1? 0: 1, 0, 1);
		mipi_csi_core_write_part(CSI2_DPHY_RSTZ, 0, 0, 1);
		mipi_csi_core_write_part(CSI2_RESETN, 0, 0, 1);
	}
	
}

int clkdatastopstatecheck()
{
int mipi_reg=0,mask=0,i=0;
	mipi_reg = mipi_csi_read(CSI2_PHY_STATE);
	printf("CSI PHY Status iin clock and data are in stop state=0x%x\n",mipi_reg);
        mask = ((1<<PHY_STOPSTATECLK) | (1<<PHY_STOPSTATEDATA_0) | (1<<PHY_STOPSTATEDATA_1)); //Mask and get the vlaues of clock and data 0 and data 1 lane
	while( (((mask & mipi_csi_read(CSI2_PHY_STATE)) != mask)) && (i < 10) ) {
                mipi_reg = mipi_csi_read(CSI2_PHY_STATE);
                printf("CSI PHY Status in clock and data lanes:%d:%x\n",i,mipi_reg);
                i++;
                udelay(1000);
       }

       if (i >= 10) {
                printf("Data and clock are not in stop state!\n");
                return -1;
      	}
        mipi_reg = mipi_csi_read(CSI2_PHY_STATE);
        printf("CSI PHY Status in clock and data in stop state function:%x\n",mipi_reg);
}

int shikhara_mipi_csi_platform_init()
{
	/* PMU Configuration */
	writel(0x0,0xd4561098);
	writel(0x00000005,0xd4561054);
	csi_power_on();
	//writel(0x0000007f,0xd45610cc); //Powering up all the peripherals comes under PMU. CSI is one of these.
	udelay(2000); /* to power up CSI sub system */
}


u8 mipi_csi_init(void)
{
u8 retval= 0;
u8 temp = 0;
u32 version = 0;
u32 csi_phy_status =0;
u32 mask=0;
u32 mbps_per_lane=MBPS_PER_LANE;
int sel;

	/* PMU Configuration */

	shikhara_mipi_csi_platform_init();

	csi_enable(0);
#if 1
       /* set only one lane (lane 0) as active (ON) */
        retval = mipi_csi_set_on_lanes(OV5640_NO_LANES); /*Actual no. of lanes are 4*/
        if(retval != 0){
                printf("Unable to set the lanes\n");
                        
                temp = mipi_csi_close();
                if(temp == 0){
                        return 1; 
                }
        }
#endif
/* MIPI-CSI Bridge initilization */
	if (mipi_csi_bridge_init()){
	 	printf("Error while configuring MIPI-CSI bridge\n ");
 		return 1;
	}
 	else{
 		/* printf(" MIPI-CSI2 Bridge Initilized Successfully\n "); */
	} 

	version = mipi_csi_read(CSI2_VERSION);
	if(!version){
		printf("Version is not valid, Unable to Initilize Driver\n");
		return 1; 
	}
	printf("MIPI-CSI2 Version is: %u\n", version);


	mipi_csi_write(CSI2_PHY_TEST_CTRL0, 1);
        mipi_csi_write(CSI2_PHY_TEST_CTRL1, 0);

	sel = max_mbps_to_hsfreqrange_sel(mbps_per_lane);
	if (sel < 0) {
		printf("Invalid mbps_per_lane=%d\n",mbps_per_lane);
	}
	dw_mipi_csi2_phy_write(0x44, sel);
//	dw_mipi_csi2_phy_write(0x44, 0x8);


	/* set only one lane (lane 0) as active (ON) */
	retval = mipi_csi_set_on_lanes(OV5640_NO_LANES); /*Actual no. of lanes are 4*/
	if(retval != 0){
		printf("Unable to set the lanes\n");
			
		temp = mipi_csi_close();
		if(temp == 0){
			return 1; 
		}
	}


	/* Set D-PHY ShutDown Input. Active Low */
	retval= mipi_csi_shut_down_phy(0);
	if(retval != 0){
		printf("Unable to bring up PHY\n");
		temp = mipi_csi_close();
		if(temp == 0){
			return 1; 
		}
	}

	/* Reset D-PHY */
	retval= mipi_csi_reset_phy();
	if(retval != 0){
		printf("Unable to reset PHY\n");
		temp = mipi_csi_close();
		if(temp == 0){
			return 1; 
		}
	}


	/* Reset MIPI-CSI receiver controller */
	retval= mipi_csi_reset_controller();	
	if(retval != 0){
		printf("Unable to reset MIPI-CSI2 Controller\n");
		temp = mipi_csi_close();
		if(temp == 0){
			return 1; 
		}
	}
//	Set_pixel_format(2,0); //2 for RGB565 1 for virtual channel
	/* Masking all the Interrupts */	
	//mipi_csi_event_disable(0xffffffff, 1);
	mipi_csi_event_enable(0xffffffff, 1);
	//mipi_csi_event_disable(0xffffffff, 2);
	mipi_csi_event_enable(0xffffffff, 2);
		
	mipi_csi_write(CSI2_PHY_TEST_CTRL0, 0);
        mipi_csi_write(CSI2_PHY_TEST_CTRL1, 0);
	
        csi_phy_status = mipi_csi_read(CSI2_PHY_STATE);
	/* printf("CSI PHY status in mipi_csi_init function = 0x%x\n", csi_phy_status); */
#if 0
	mask = ((1<<PHY_STOPSTATECLK) | (1<<PHY_STOPSTATEDATA_0) | (1<<PHY_STOPSTATEDATA_1)); //Mask and get the vlaues of clock and data 0 and data 1 lane
	//while( mask & mipi_csi_read(CSI2_PHY_STATE));
	while( (mask & mipi_csi_read(CSI2_PHY_STATE)) != mask );
#endif
	/* printf("CSI PHY status in mipi csi init= 0x%x : success\n", mipi_csi_read(CSI2_PHY_STATE)); */
	return 0;

}


/**
 * @brief: mipi_csi_get_on_lanes() --- Function to get the No. of Active Data Lanes
 * @lanes: Integer value No. of lanes
 * @return: No of Active data lanes.
 **/
 
u8 mipi_csi_get_on_lanes(void)
{
	return (mipi_csi_core_read_part(CSI2_N_LANES, 0, 2) + 1);
}

/**
 * @brief: mipi_csi_set_on_lanes() --- Function to set No. of Active Lanes
 * @lanes: Integer value No. of lanes
 * @return: 0 if success.
 **/
 
u8 mipi_csi_set_on_lanes(u8 lanes)
{
	return mipi_csi_core_write_part(CSI2_N_LANES, (lanes - 1), 0, 2);
}

/**
 * @brief: mipi_csi_reset_controller() --- Function to reset the mipi-csi2 controller
 * @return: 0 if success.
 **/
 
u8 mipi_csi_reset_controller()
{
#if 0
u8 ret = 0;
	ret = mipi_csi_core_write_part(CSI2_RESETN, 0, 0, 1);
	if(ret){
		printf("Unable to reset MIPI-CSI2 controller\n");
                return 1;
	}
	else
		return mipi_csi_core_write_part(CSI2_RESETN, 1, 0, 1);
#endif
	return mipi_csi_core_write_part(CSI2_RESETN, 1, 0, 1);
}

/*
 * @brief: mipi_csi_event_enable() --- Function to enable the events of the mipi-csi2 controller
 * @Param mask: information about mask bits with respect to events
 * @Param err_reg_no: Mask Register No.
 * @return: 0 if success.
 */

u8 mipi_csi_event_enable(u32 mask, u8 err_reg_no)
{
	switch (err_reg_no)
	{
		case 1:
			return mipi_csi_write(CSI2_MSK1, (~mask) & mipi_csi_read(CSI2_MSK1));
		case 2:
			return mipi_csi_write(CSI2_MSK2, (~mask) & mipi_csi_read(CSI2_MSK2));
			
		default:
			printf("Invalid register Number\n");
			return 1;
	}
}

/*
 * @brief: mipi_csi_event_disable() --- Function to disable the events of the mipi-csi2 controller
 * @Param mask: Information about mask bits with respect to events
 * @Param err_reg_no: Mask Register No.
 * @return: 0 if success.
 */

u8 mipi_csi_event_disable(u32 mask, u8 err_reg_no)
{
	switch (err_reg_no)
	{
		case 1:
			return mipi_csi_write(CSI2_MSK1, mask | mipi_csi_read(CSI2_MSK1));
		
		case 2:
			return mipi_csi_write(CSI2_MSK2, mask | mipi_csi_read(CSI2_MSK2));
		 
		default:
			printf("Invalid register Number\n ");
			return 1;
	}
}

/*
 * @brief: mipi_csi_event_get_source() --- Function to get the source of an event by reading the Error registers
 * @Param err_reg_no: Error Register No. 
 * @return: 32 bit value of Error register contains.
 */

u32 mipi_csi_event_get_source(u8 err_reg_no)
{
	switch (err_reg_no)
        {
                case 1:
                        return mipi_csi_read(CSI2_ERR1);
                case 2:
                        return mipi_csi_read(CSI2_ERR2);
                default:
                        printf("Invalid register Number\n");
                        return 1;
        }
}


/*
 * @brief: mipi_csi_shut_down_phy() --- Function to shutdown the D-PHY module.
 * @Param shutdown: Integeter No. 1 represents Shutdown, if 0 comes as parameter means write 1 to the register means setting Shutdown input control to active low. 
 * @return: 0 if success.
 */
u8 mipi_csi_shut_down_phy(u8 shutdown)
{
	/* printf("Setting Shutting down input control of D-PHY\n "); */
   	
	return mipi_csi_core_write_part(CSI2_PHY_SHUTDOWNZ, shutdown? 0: 1, 0, 1);
}


/*
 * @brief: mipi_csi_reset_phy() --- Function to reset the D-PHY 
 * @return: 0 if success
 */
u8 mipi_csi_reset_phy()
{
#if 0
	u8 ret = 0;
	ret = mipi_csi_core_write_part(CSI2_DPHY_RSTZ, 0, 0, 1);
        if (ret){
            	printf("Unable to reset D-PHY\n");
                return 1;
        }
        else
		 return mipi_csi_core_write_part(CSI2_DPHY_RSTZ, 1, 0, 1);
#endif
	return mipi_csi_core_write_part(CSI2_DPHY_RSTZ, 1, 0, 1);
}


u8 mipi_csi_close(void)
{
int ret = 0;  

#ifdef CONFIG_SHIKHARA_ASIC
	ret =  mipi_csi_shut_down_phy(1);
		if(ret != 0) 
			printf(" MIPI-CSI DPHY didn't shutdown\n ");
	
		return ret;
#endif
	return ret;
}



/***Register Read/Write Routines****/

u8 mipi_csi_write(u32 address, u32 data)
{
	writel(data, address);
	return 0;
}

u8 mipi_csi_core_write_part(u32 address, u32 data, u8 shift, u8 width)
{
	u32 mask = (1 << width) - 1;
	u32 temp = mipi_csi_read(address);
	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	return mipi_csi_write(address, temp);
}

u32 mipi_csi_core_read_part(u32 address, u8 shift, u8 width)
{
	return (mipi_csi_read(address) >> shift) & ((1 << width) - 1);
}

u32 mipi_csi_read(u32 address)
{
	return readl(address);
}
