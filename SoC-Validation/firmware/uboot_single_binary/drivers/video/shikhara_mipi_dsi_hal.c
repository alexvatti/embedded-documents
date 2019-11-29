/*mipi_dsi_hal.c -function definitions (Host and Phy related) */
#include <asm/arch/shikhara_map.h>
#include <common.h>
#include <asm/arch/shikhara_mipi_dsi_hal.h>
#include <asm/io.h>
#include<video/mipi_display.h>

#include <common.h>
#include <lcd.h>
#include <bmp_layout.h>

//-------------------------------------------------------------------------------------------------------------------------------
void mipi_dsih_write_word(dsih_ctrl_t * instance, uint32_t reg_address, uint32_t data)
{

	instance->core_write_function(instance->address, reg_address, data);
}

void mipi_dsih_write_part(dsih_ctrl_t * instance, uint32_t reg_address, uint32_t data, uint8_t shift, uint8_t width)
{
	uint32_t mask = (1 << width) - 1;
	uint32_t temp = mipi_dsih_read_word(instance, reg_address);

	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	mipi_dsih_write_word(instance, reg_address, temp);
}

uint32_t mipi_dsih_read_word(dsih_ctrl_t * instance, uint32_t reg_address)
{
	return instance->core_read_function(instance->address, reg_address);
}

uint32_t mipi_dsih_read_part(dsih_ctrl_t * instance, uint32_t reg_address, uint8_t shift, uint8_t width)
{
	return (mipi_dsih_read_word(instance, reg_address) >> shift) & ((1 << width) - 1);
}

void mipi_dsih_dphy_write_part(dphy_t * instance, uint32_t reg_address, uint32_t data, uint8_t shift, uint8_t width)
{
        uint32_t mask = 0;
        uint32_t temp = 0;
        if (instance->core_read_function != 0)
        {
                mask = (1 << width) - 1;
                temp = mipi_dsih_dphy_read_word(instance, reg_address);
                temp &= ~(mask << shift);
                temp |= (data & mask) << shift;
                mipi_dsih_dphy_write_word(instance, reg_address, temp);
        }
}
uint32_t mipi_dsih_dphy_read_word(dphy_t * instance, uint32_t reg_address)
{
        if (instance->core_read_function == 0)
        {
                return ERR_DSI_INVALID_IO;
        }
        return instance->core_read_function(instance->address, reg_address);
}
void mipi_dsih_dphy_write_word(dphy_t * instance, uint32_t reg_address, uint32_t data)
{
        if (instance->core_write_function != 0)
        {
                instance->core_write_function(instance->address, reg_address, data);
        }
}
uint32_t mipi_dsih_dphy_read_part(dphy_t * instance, uint32_t reg_address, uint8_t shift, uint8_t width)
{
        return (mipi_dsih_dphy_read_word(instance, reg_address) >> shift) & ((1 << width) - 1);
}


void io_bus_write32(uint32_t address, uint32_t reg_address, uint32_t data){
	writel(data, address+reg_address);

}

uint32_t  io_bus_read32(uint32_t address, uint32_t reg_address){
	uint32_t temp;
	temp= readl(address+reg_address);
	return temp;
}

//--------------------------------------------------------------------------------------------------------------------------------
uint32_t mipi_dsih_hal_get_version(dsih_ctrl_t * instance)
{
	return mipi_dsih_read_word(instance, DSI_VERSION);
}

void mipi_dsih_hal_power(dsih_ctrl_t * instance, int on)
{
	mipi_dsih_write_part(instance, DSI_PWR_UP, on, 0, 1);
}

void mipi_dsih_hal_tx_escape_division(dsih_ctrl_t * instance, uint8_t tx_escape_division)
{
	mipi_dsih_write_part(instance, DSI_CLKMGR_CFG, tx_escape_division, 0, 8);
}


void mipi_dsih_hal_dpi_lp_during_hfp(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 13, 1);
}


void mipi_dsih_hal_dpi_lp_during_hbp(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 12, 1);
}

void mipi_dsih_hal_dpi_lp_during_vactive(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 11, 1);
}

void mipi_dsih_hal_dpi_lp_during_vfp(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 10, 1);
}

void mipi_dsih_hal_dpi_lp_during_vbp(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 9, 1);
}

void mipi_dsih_hal_dpi_lp_during_vsync(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 8, 1);
}

void mipi_dsih_hal_dpi_video_vc(dsih_ctrl_t * instance, uint8_t vc)
{
	mipi_dsih_write_part(instance, DSI_DPI_VCID, (uint32_t)(vc), 0, 2);
}

dsih_error_t mipi_dsih_hal_dpi_color_coding(dsih_ctrl_t * instance, dsih_color_coding_t color_coding)
{
	dsih_error_t err = OK;
	if (color_coding > COLOR_CODE_MAX)
	{
		if (instance->log_error != 0)
		{
			instance->log_error("invalid colour configuration");
		}
		err = ERR_DSI_COLOR_CODING;
	}
	else
	{
		mipi_dsih_write_part(instance,DSI_DPI_COLOR_CODING, color_coding, 0, 4);
	}
	return err;
}


void mipi_dsih_hal_dpi_18_loosely_packet_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_DPI_COLOR_CODING, enable, 8, 1);
}

void mipi_dsih_hal_dpi_color_mode_pol(dsih_ctrl_t * instance, int active_low)
{
	mipi_dsih_write_part(instance, DSI_DPI_CFG_POL, active_low, 4, 1);
}

void mipi_dsih_hal_dpi_shut_down_pol(dsih_ctrl_t * instance, int active_low)
{
	mipi_dsih_write_part(instance, DSI_DPI_CFG_POL, active_low, 3, 1);
}

void mipi_dsih_hal_dpi_hsync_pol(dsih_ctrl_t * instance, int active_low)
{
	mipi_dsih_write_part(instance, DSI_DPI_CFG_POL, active_low, 2, 1);
}

void mipi_dsih_hal_dpi_vsync_pol(dsih_ctrl_t * instance, int active_low)
{
	mipi_dsih_write_part(instance, DSI_DPI_CFG_POL, active_low, 1, 1);
}

void mipi_dsih_hal_dpi_dataen_pol(dsih_ctrl_t * instance, int active_low)
{
	mipi_dsih_write_part(instance, DSI_DPI_CFG_POL, active_low, 0, 1);
}

void mipi_dsih_hal_gen_eotp_rx_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_PKTHDLR_CFG, enable, 1, 1);
}

void mipi_dsih_hal_gen_eotp_tx_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_PKTHDLR_CFG, enable, 0, 1);
}

void mipi_dsih_hal_bta_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_PKTHDLR_CFG, enable, 2, 1);
}

void mipi_dsih_hal_gen_ecc_rx_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_PKTHDLR_CFG, enable, 3, 1);
}

void mipi_dsih_hal_gen_crc_rx_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_PKTHDLR_CFG, enable, 4, 1);
}

dsih_error_t mipi_dsih_hal_dpi_video_packet_size(dsih_ctrl_t * instance, uint16_t size)
{
	if (size < (1 << 14))/* 14-bit field */
	{
		mipi_dsih_write_part(instance, DSI_VID_PKT_SIZE, size, 0, 14);
		return OK;
	}else
		return ERR_DSI_OUT_OF_BOUND;
	
}


dsih_error_t mipi_dsih_hal_dpi_chunks_no(dsih_ctrl_t * instance, uint16_t no)
{
	if (no < (1 << 13))
	{
		mipi_dsih_write_part(instance, DSI_VID_NUM_CHUNKS, no, 0, 13);
		return OK;
	}else
		return ERR_DSI_OUT_OF_BOUND;
	
}

dsih_error_t mipi_dsih_hal_dpi_null_packet_size(dsih_ctrl_t * instance, uint16_t size)
{
	if (size < (1 << 13)) /* 13-bit field */
	{
		mipi_dsih_write_part(instance, DSI_VID_NULL_SIZE, size, 0, 13);
		return OK;
	}
	else
	{
		return (ERR_DSI_OUT_OF_BOUND);
	}
}

void mipi_dsih_hal_cmd_ack_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, enable, 1, 1);
}

dsih_error_t mipi_dsih_hal_dcs_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp)
{
	dsih_error_t err = OK;
	switch (no_of_param)
	{
	case 0:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 18, 1);
		break;
	default:
		if (instance->log_error != 0)
		{
			instance->log_error("undefined DCS Read packet type");
		}
		err = ERR_DSI_OUT_OF_BOUND;
		break;
	}
	return err;
}

dsih_error_t mipi_dsih_hal_dcs_wr_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp)
{
	switch (no_of_param)
	{
	case 0:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 16, 1);
		break;
	case 1:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 17, 1);
		break;
	default:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 19, 1);
		break;
	}
	return OK;
}

dsih_error_t mipi_dsih_hal_gen_wr_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp)
{
	switch (no_of_param)
	{
	case 0:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 8, 1);
		break;
	case 1:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 9, 1);
		break;
	case 2:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 10, 1);
		break;
	default:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 14, 1);
		break;
	}
	return OK;
}


dsih_error_t mipi_dsih_hal_gen_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp)
{
	dsih_error_t err = OK;
	switch (no_of_param)
	{
	case 0:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 11, 1);
		break;
	case 1:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 12, 1);
		break;
	case 2:
		mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 13, 1);
		break;
	default:
		if (instance->log_error != 0)
		{
			instance->log_error("undefined Generic Read packet type");
		}
		err = ERR_DSI_OUT_OF_BOUND;
		break;
	}
	return err;
}



void mipi_dsih_hal_max_rd_size_tx_type(dsih_ctrl_t * instance, int lp)
{
	mipi_dsih_write_part(instance, DSI_CMD_MODE_CFG, lp, 24, 1);
}

void mipi_dsih_hal_dpi_hline(dsih_ctrl_t * instance, uint16_t time)
{
	mipi_dsih_write_part(instance, DSI_VID_HLINE_TIME, time, 0, 15);
}

void mipi_dsih_hal_dpi_hbp(dsih_ctrl_t * instance, uint16_t time)
{
	mipi_dsih_write_part(instance, DSI_VID_HBP_TIME, time, 0, 12);
}

void mipi_dsih_hal_dpi_hsa(dsih_ctrl_t * instance, uint16_t time)
{
	mipi_dsih_write_part(instance, DSI_VID_HSA_TIME, time, 0, 12);
}

void mipi_dsih_hal_dpi_vactive(dsih_ctrl_t * instance, uint16_t lines)
{
	mipi_dsih_write_part(instance, DSI_VID_VACTIVE_LINES, lines, 0, 14);
}

void mipi_dsih_hal_dpi_vfp(dsih_ctrl_t * instance, uint16_t lines)
{
	mipi_dsih_write_part(instance, DSI_VID_VFP_LINES, lines, 0, 10);
}

void mipi_dsih_hal_dpi_vbp(dsih_ctrl_t * instance, uint16_t lines)
{
	mipi_dsih_write_part(instance, DSI_VID_VBP_LINES, lines, 0, 10);
}

void mipi_dsih_hal_dpi_vsync(dsih_ctrl_t * instance, uint16_t lines)
{
	mipi_dsih_write_part(instance, DSI_VID_VSA_LINES, lines, 0, 10);
}

dsih_error_t mipi_dsih_phy_lp2hs_config(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_write_part(instance, DSI_PHY_TMR_CFG, no_of_byte_cycles, 16, 8);
	return OK;
}

dsih_error_t mipi_dsih_phy_clk_hs2lp_config(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_write_part(instance, DSI_PHY_TMR_LPCLK_CFG, no_of_byte_cycles, 16, 10);
	return OK;
}

uint32_t mipi_dsih_hal_int_status_0(dsih_ctrl_t * instance, uint32_t mask)
{
	return (mipi_dsih_read_word(instance, DSI_INT_ST0) & mask);
}

uint32_t mipi_dsih_hal_int_status_1(dsih_ctrl_t * instance, uint32_t mask)
{
	return (mipi_dsih_read_word(instance, DSI_INT_ST1) & mask);
}

void mipi_dsih_hal_int_mask_0(dsih_ctrl_t * instance, uint32_t mask)
{
	mipi_dsih_write_word(instance, DSI_INT_MSK0, mask);
}

void mipi_dsih_hal_int_mask_1(dsih_ctrl_t * instance, uint32_t mask)
{
	mipi_dsih_write_word(instance, DSI_INT_MSK1, mask);
}

void mipi_dsih_hal_dpi_video_mode_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_MODE_CFG, enable? 0: 1, 0, 1);
}

dsih_error_t mipi_dsih_phy_bta_time(dsih_ctrl_t * instance, uint16_t no_of_byte_cycles)
{
	if (no_of_byte_cycles < 0x8000) /* 15-bit field */
	{
		mipi_dsih_write_part(instance, DSI_PHY_TMR_CFG, no_of_byte_cycles, 0, 15);
	}
	else
	{
		return ERR_DSI_OVERFLOW;
	}
	return OK;
}
void mipi_dsih_hal_gen_rd_vc(dsih_ctrl_t * instance, uint8_t vc)
{
	mipi_dsih_write_part(instance, DSI_GEN_VCID, vc, 0, 2);
}

void mipi_dsih_hal_dpi_lp_cmd_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 15, 1);
}

void mipi_dsih_hal_dpi_frame_ack_en(dsih_ctrl_t * instance, int enable)
{
	mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, enable, 14, 1);
}

dsih_error_t mipi_dsih_phy_hs2lp_config(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_write_part(instance, DSI_PHY_TMR_CFG, no_of_byte_cycles, 24, 8);
	return OK;
}

dsih_error_t mipi_dsih_phy_clk_lp2hs_config(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_write_part(instance, DSI_PHY_TMR_LPCLK_CFG, no_of_byte_cycles, 0, 10);
	return OK;
}


dsih_error_t mipi_dsih_hal_dpi_video_mode_type(dsih_ctrl_t * instance, dsih_video_mode_t type)
{
	if (type < 3)
	{
		mipi_dsih_write_part(instance, DSI_VID_MODE_CFG, type, 0, 2);
		return OK;
	}
	else
	{
		if (instance->log_error != 0)
		{
			instance->log_error("undefined type");
		}
		return ERR_DSI_OUT_OF_BOUND;
	}
}
void mipi_dsih_hal_dpi_null_packet_en(dsih_ctrl_t * instance, int enable)
{
}
void mipi_dsih_hal_dpi_multi_packet_en(dsih_ctrl_t * instance, int enable)
{
}

/*D-PHY related functionalities*/
void mipi_dsih_dphy_reset(dphy_t * instance, int reset)
{
	mipi_dsih_dphy_write_part(instance, DSI_PHY_RSTZ, reset, 1, 1);
}

void mipi_dsih_dphy_stop_wait_time(dphy_t * instance, uint8_t no_of_byte_cycles)
{
	mipi_dsih_dphy_write_part(instance, DSI_PHY_IF_CFG, no_of_byte_cycles, 8, 8);
}

void mipi_dsih_dphy_no_of_lanes(dphy_t * instance, uint8_t no_of_lanes)
{
	mipi_dsih_dphy_write_part(instance, DSI_PHY_IF_CFG, no_of_lanes - 1, 0, 2);
}
void mipi_dsih_dphy_clock_en(dphy_t * instance, int en)
{
	mipi_dsih_dphy_write_part(instance, DSI_PHY_RSTZ, en, 2, 1);
}
void mipi_dsih_dphy_shutdown(dphy_t * instance, int powerup)
{
	mipi_dsih_dphy_write_part(instance, DSI_PHY_RSTZ, powerup, 0, 1);

}
void mipi_dsih_dphy_enable_hs_clk(dphy_t * instance, int enable)
{
	mipi_dsih_dphy_write_part(instance, DSI_LPCLK_CNTRL, enable, 0, 1);
}
void mipi_dsih_dphy_txrequestclkhs(dphy_t * instance, int enable)
{
	mipi_dsih_dphy_write_part(instance, DSI_LPCLK_CNTRL, enable, 1, 1);
}

void gen_write_cmd(unsigned long gen_hdr)
{
    while (readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_CMD_FULL) ; //wait, if it is full
    writel(gen_hdr,(DSI_BASE_ADDR + DSI_GEN_HDR));

    while ((!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_CMD_EMPTY)) &&
           (!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_PLD_W_EMPTY))) ;
}

int gen_read_data(unsigned int cmd)
{ 
    gen_write_cmd(cmd);
    while (!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_RD_CMD_BUSY)) ; // wait for busy
    while (readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_RD_CMD_BUSY) ; // wait for idle
   // while ((readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_PLD_R_EMPTY)) ; // if it empty
    return readl(DSI_BASE_ADDR + DSI_GEN_PLD_DATA); 
}
unsigned int val=0;
void gen_write_data(unsigned long gen_pld_data)
{
    while (readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) &DSI_CMD_PKT_STATUS_GEN_PLD_W_FULL) ;
    writel(gen_pld_data,DSI_BASE_ADDR + DSI_GEN_PLD_DATA);
}

void TC358768_DCS_write_1A_1P(unsigned char address, unsigned char para)
{
    val=(MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM | address << 8 | para << 16);

    while (readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_CMD_FULL) ; //wait, if it is full
    writel(val,(DSI_BASE_ADDR + DSI_GEN_HDR));

    while ((!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_CMD_EMPTY)) &&
           (!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_PLD_W_EMPTY))) ;
}
void TC358768_DCS_write_1A_0P(unsigned char address)
{
    val=(MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM | address << 8);

    while (readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS) & DSI_CMD_PKT_STATUS_GEN_CMD_FULL) ; //wait, if it is full
    writel(val,(DSI_BASE_ADDR + DSI_GEN_HDR));

    while ((!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_CMD_EMPTY)) &&
           (!(readl(DSI_BASE_ADDR + DSI_CMD_PKT_STATUS)& DSI_CMD_PKT_STATUS_GEN_PLD_W_EMPTY))) ;

}

void mipi_lcd_init()
{
	int i,val = 0;
	static int count =0;

        writel(0xF0000000, TZASC2_BASE+0x108);
        writel(0xF0000000, TZASC1_BASE+0x108);
        writel(0xF0000000, TZASC0_BASE+0x108);

	dsi_bridge_disable();
	udelay(1000);
	dsi_bridge_enable();
	
	//writel(DSI_MODE_CFG_COMMAND_MODE,DSI_BASE_ADDR + DSI_MODE_CFG); //setting in the command mode
	//writel(DSI_PCKHDL_CFG_EN_CRC_RX | DSI_PCKHDL_CFG_EN_ECC_RX | DSI_PCKHDL_CFG_EN_BTA ,DSI_BASE_ADDR + DSI_PKTHDLR_CFG); //BTA,RX CRC and ECC Enable
	//writel(0x000fffff,DSI_BASE_ADDR + DSI_CMD_MODE_CFG); //command Write and read in low power mode for Generic and DCS

	while (!(readl(DSI_BASE_ADDR + DSI_PHY_STATUS) & DSI_PHY_STATUS_LOCK)) ;
        printf("PHY is locked\n");
        while (!(readl(DSI_BASE_ADDR + DSI_PHY_STATUS) & DSI_PHY_STATUS_STOPSTATE_CLK_LANE)) ;
        printf("Clock lane is in stop state\n");
	
	val = MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE | (0x04 <<8);
        gen_write_cmd(val);
#if 0	
	/* {0x23, 0, 0, 0, 0, 2 , {0xB0,0x04}}, */
	val=0xB0 | 0x04 << 8;
	gen_write_cmd((val << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM);
       	val = gen_read_data((0XB0 << 8) | MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM);
        printf("Data came back from 0xB0 Address is %d\n", val);

	/* {0x05, 0, 0, 0, 0, 1 , {0x00}}, */
	gen_write_cmd((0x00 << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM);

	/* {0x05, 0, 0, 0, 0, 1 , {0x00}}, */
	gen_write_cmd((0x00 << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM);
	/* {0x29, 0, 0, 0, 0, 7 , {0xB3,0x14,0x00,0x00,
					0x00,0x00,0x00}}, */
	gen_write_data(0xB3 | (0x000014 << 8));
	gen_write_data(0x000000);
	gen_write_cmd((7 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);

       	val = gen_read_data((0XB3 << 8) | MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM);
        printf("Data came back from 0xB3 Address is %d\n", val);

	/* {0x29, 0, 0, 0, 0, 3 , {0xB6,0x3A,0xC3}}, */
	gen_write_data(0xB6 | (0xC33A << 8));
	gen_write_cmd((3 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	val = gen_read_data((0XB6 << 8) | MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM);
        printf("Data came back from 0xB6 Address is %d\n", val);

	/* {0x23, 0, 0, 0, 0, 2 , {0xC0,0x00}}, */
	val=0xC0|0x00<<8;
	gen_write_cmd((val << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM);

	/* {0x29,0,0,0,0,35 ,{0xC1,0x84,0x60,0x10,
			      0xEB,0xFF,0x6F,0xCE,
			      0xFF,0xFF,0x17,0x12,
			      0x58,0x73,0xAE,0x31,
			      0x20,0xC6,0xFF,0xFF,
			      0x1F,0xF3,0xFF,0x5F,
			      0x10,0x10,0x10,0x10,
			      0x00,0x62,0x01,0x22,
			      0x22,0x00,0x01}}, */
	gen_write_data(0xC1 | (0x106084 << 8));
	gen_write_data(0xCE6FFFEB);	
	gen_write_data(0x1217FFFF);
	gen_write_data(0x31AE7358);
	gen_write_data(0xFFFFC620);
	gen_write_data(0x5FFFF31F);
	gen_write_data(0x10101010);
	gen_write_data(0x22016200);
	gen_write_data(0x010022);
	gen_write_cmd((0X23 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);

	/* {0x29, 0, 0, 0, 0, 8 , {0xC2,0x31,0xF7,0x80,
				   0x06,0x08,0x80,0x00}}, */
	gen_write_data(0xC2 | (0x80F731 << 8));
	gen_write_data(0X00800806);
	gen_write_cmd((8 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
		
	/* {0x29, 0, 0, 0, 0, 4 , {0xC3,0x01,0x00,0x00}},*/
	gen_write_data(0xC3 | (0x000001 << 8));
	gen_write_cmd((8 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29,0,0,0,0,23,{0xC4,0x70,0x00,0x00,
			     0x00,0x00,0x00,0x00,
			     0x00,0x00,0x0C,0x06,
			     0x00,0x00,0x00,0x00,
			     0x00,0x00,0x00,0x00,
			     0x00,0x0C,0x06}}, */
	
	gen_write_data(0xC4 | (0x000070 << 8));
	gen_write_data(0X00000000);
	gen_write_data(0X060C0000);
	gen_write_data(0X00000000);
	gen_write_data(0X00000000);
	gen_write_data(0X060C00);
	gen_write_cmd((0X17 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29,0,0,0,0,41 ,{0xC6,0xC8,0x08,0x67,
			      0x08,0x67,0x00,0x00,
			      0x00,0x00,0x00,0x00,
			      0x00,0x00,0x00,0x00,
			      0x00,0x00,0x16,0x18,
			      0x08,0xC8,0x08,0x67,
			      0x08,0x67,0x00,0x00,
			      0x00,0x00,0x00,0x00,
			      0x00,0x00,0x00,0x00,
			      0x00,0x00,0x16,0x18,
					     0x08}}, */
	
	gen_write_data(0xC6 | (0x6708C8 << 8));
	gen_write_data(0X00006708);
	gen_write_data(0X00000000);
	gen_write_data(0X00000000);
	gen_write_data(0X18160000);
	gen_write_data(0X6708C808);
	gen_write_data(0X00006708);
	gen_write_data(0X00000000);
	gen_write_data(0X00000000);
	gen_write_data(0X18160000);
        gen_write_data(0X00000008);
	gen_write_cmd((0X29 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	
	/* {0x29,0,0,0,0,31 ,{0xC7,0x00,0x0D,0x19,
			      0x23,0x30,0x3C,0x47,
			      0x57,0x3C,0x44,0x50,
			      0x60,0x68,0x70,0x75,
			      0x00,0x0D,0x19,0x23,
			      0x30,0x3C,0x47,0x57,
			      0x3C,0x44,0x50,0x60,
			      0x68,0x70,0x75}}, */
	
	gen_write_data(0xC7 | (0x190D00 << 8));
	gen_write_data(0X473C3023);
	gen_write_data(0X50443C57);
	gen_write_data(0X75706860);
	gen_write_data(0X23190D00);
	gen_write_data(0X57473C30);
	gen_write_data(0X6050443C);
	gen_write_data(0X757068);
	gen_write_cmd((0X1F << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29,0,0,0,0,20 ,{0xC8,0x00,0x00,0x00,
			      0x00,0x00,0xFC,0x00,
			      0x00,0x00,0x00,0x00,
			      0xFC,0x00,0x00,0x00,
			      0x00,0x00,0xFC,0x00}}, */
	gen_write_data(0xC8 | (0x000000 << 8));
	gen_write_data(0X00FC0000);
	gen_write_data(0X00000000);
	gen_write_data(0X000000FC);
	gen_write_data(0X00FC0000);
	gen_write_cmd((0X14 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29, 0, 0, 0, 0, 9 , {0xCB,0x31,0xFC,0x3F,
			           0x8C,0x00,0x00,0x00,
				   0x00}}, */
	gen_write_data(0xCB | (0x3FFC31 << 8));
	gen_write_data(0X0000008C);
	gen_write_data(0X00);
	gen_write_cmd((0X9 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x23, 0, 0, 0, 0, 2 , {0xCC,0x0B}}, */
	val=0XCC | 0x0B << 8;
	gen_write_cmd((val << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM);
	
	/* {0x29, 0, 0, 0, 0, 11 , {0xD0,0x11,0x81,0xBB,
				    0x19,0x99,0x4C,0x19,
				    0x19,0x0C,0x00}}, */
	
	gen_write_data(0xD0 | (0xBB8111 << 8));
	gen_write_data(0X194C9919);
	gen_write_data(0X000C19);
	gen_write_cmd((0X0B << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29,0,0,0,0,26 ,{0xD3,0x1B,0x33,0xBB,
			      0xBB,0xB3,0x33,0x33,
			      0x33,0x01,0x01,0x00,
			      0xA0,0xD8,0xA0,0x0D,
			      0x5E,0x5E,0x44,0x3B,
			      0x22,0x72,0x07,0x3D,
			      0xBF,0x33}}, */
	gen_write_data(0xD3 | (0xBB331B << 8));
	gen_write_data(0X3333B3BB);
	gen_write_data(0X00010133);
	gen_write_data(0X0DA0D8A0);
	gen_write_data(0X3B445E5E);
	gen_write_data(0X3D077222);
	gen_write_data(0X33BF);
	gen_write_cmd((0X0B << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29, 0, 0, 0, 0, 8 , {0xD5,0x06,0x00,0x00,
				   0x01,0x3A,0x01,0x3A}}, */
	gen_write_data(0xD5 | (0x000006 << 8));
	gen_write_data(0X3A013A01);
	gen_write_cmd((0X08 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x29, 0, 0, 0, 0, 8 , {0xD5,0x06,0x00,0x00,
				   0x01,0x3A,0x01,0x3A}}, */
	gen_write_data(0xD5 | (0x000006 << 8));
	gen_write_data(0X3A013A01);
	gen_write_cmd((0X08 << 8) | MIPI_DSI_GENERIC_LONG_WRITE);
	
	/* {0x05, 0, 0, 0, 0, 1 , {0x29}}, */
	udelay(200000);
	gen_write_cmd((0x29 << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM);
	/* {0x05, 0, 0, 0, 200, 1, {0x11}}, */
        udelay(200000); 
	gen_write_cmd((0x11 << 8) | MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM);
        udelay(200000);
#endif

		TC358768_DCS_write_1A_1P(0xB0,0x00);//PAGE 0
		TC358768_DCS_write_1A_1P(0xB1,0x67);
#if 0 //BIST Enable
		TC358768_DCS_write_1A_1P(0xB2,0x4A); //Added for testing BIST
		//TC358768_DCS_write_1A_1P(0xB2,0x49); //Added for testing BIST
#endif
		TC358768_DCS_write_1A_1P(0xB3,0x48);
		//TC358768_DCS_write_1A_1P(0xB3,0x4B);
		TC358768_DCS_write_1A_1P(0xBB,0xE8); //lane 0
		//TC358768_DCS_write_1A_1P(0xBB,0xEE); //NUMBER OF LANES 4
		TC358768_DCS_write_1A_1P(0xC4,0x09);
		TC358768_DCS_write_1A_1P(0xC6,0x09);
		TC358768_DCS_write_1A_1P(0xCB,0x3F);
		TC358768_DCS_write_1A_1P(0xCC,0x2D);
		TC358768_DCS_write_1A_1P(0xCD,0x20);
		TC358768_DCS_write_1A_1P(0xCE,0x1D);
		TC358768_DCS_write_1A_1P(0xCF,0x0A);
		TC358768_DCS_write_1A_1P(0xD0,0x00);
		TC358768_DCS_write_1A_1P(0xD1,0x02);
		TC358768_DCS_write_1A_1P(0xD2,0x0D);
		TC358768_DCS_write_1A_1P(0xD3,0x10);
		TC358768_DCS_write_1A_1P(0xD4,0x10);
		TC358768_DCS_write_1A_1P(0xD5,0x0E);
		TC358768_DCS_write_1A_1P(0xD6,0x3F);
		TC358768_DCS_write_1A_1P(0xD7,0x2D);
		TC358768_DCS_write_1A_1P(0xD8,0x20);
		TC358768_DCS_write_1A_1P(0xD9,0x1D);
		TC358768_DCS_write_1A_1P(0xDA,0x0A);
		TC358768_DCS_write_1A_1P(0xDB,0x00);
		TC358768_DCS_write_1A_1P(0xDC,0x01);
		TC358768_DCS_write_1A_1P(0xDD,0x0E);
		TC358768_DCS_write_1A_1P(0xDE,0x10);
		TC358768_DCS_write_1A_1P(0xDF,0x10);
		TC358768_DCS_write_1A_1P(0xE0,0x0E);

		TC358768_DCS_write_1A_1P(0xB0,0x02);//PAGE 2
		TC358768_DCS_write_1A_1P(0xB1,0x00);
		TC358768_DCS_write_1A_1P(0xB2,0x00);
		TC358768_DCS_write_1A_1P(0xB3,0x10);
		TC358768_DCS_write_1A_1P(0xB4,0x0E);
		TC358768_DCS_write_1A_1P(0xB5,0x0C);
		TC358768_DCS_write_1A_1P(0xB6,0x0C);
		TC358768_DCS_write_1A_1P(0xB7,0x0A);
		TC358768_DCS_write_1A_1P(0xB8,0x0A);
		TC358768_DCS_write_1A_1P(0xB9,0x08);
		TC358768_DCS_write_1A_1P(0xBA,0x00);
		TC358768_DCS_write_1A_1P(0xBB,0x00);
		TC358768_DCS_write_1A_1P(0xBC,0x00);
		TC358768_DCS_write_1A_1P(0xBD,0x00);
		TC358768_DCS_write_1A_1P(0xBE,0x00);
  		TC358768_DCS_write_1A_1P(0xBF,0x00);

		TC358768_DCS_write_1A_1P(0xC0,0x08);
		TC358768_DCS_write_1A_1P(0xC1,0x06);
		TC358768_DCS_write_1A_1P(0xC2,0x06);
		TC358768_DCS_write_1A_1P(0xC3,0x02);
		TC358768_DCS_write_1A_1P(0xC4,0x04);
		TC358768_DCS_write_1A_1P(0xC5,0x00);
		TC358768_DCS_write_1A_1P(0xC6,0x00);
		TC358768_DCS_write_1A_1P(0xC7,0x00);
		TC358768_DCS_write_1A_1P(0xC8,0x00);
		TC358768_DCS_write_1A_1P(0xC9,0x0F);
		TC358768_DCS_write_1A_1P(0xCA,0x0D);
		TC358768_DCS_write_1A_1P(0xCB,0x0B);
		TC358768_DCS_write_1A_1P(0xCC,0x0B);
		TC358768_DCS_write_1A_1P(0xCD,0x09);
		TC358768_DCS_write_1A_1P(0xCE,0x09);
		TC358768_DCS_write_1A_1P(0xCF,0x07);

		TC358768_DCS_write_1A_1P(0xD0,0x00);
		TC358768_DCS_write_1A_1P(0xD1,0x00);
		TC358768_DCS_write_1A_1P(0xD2,0x00);
		TC358768_DCS_write_1A_1P(0xD3,0x00);
		TC358768_DCS_write_1A_1P(0xD4,0x00);
		TC358768_DCS_write_1A_1P(0xD5,0x00);
		TC358768_DCS_write_1A_1P(0xD6,0x07);
		TC358768_DCS_write_1A_1P(0xD7,0x05);
		TC358768_DCS_write_1A_1P(0xD8,0x05);
		TC358768_DCS_write_1A_1P(0xD9,0x01);
		TC358768_DCS_write_1A_1P(0xDA,0x03);
		TC358768_DCS_write_1A_1P(0xDB,0x00);
		TC358768_DCS_write_1A_1P(0xDC,0x00);


		TC358768_DCS_write_1A_1P(0xB0,0x03); //PAGE 3
		TC358768_DCS_write_1A_1P(0xC4,0x07);
		TC358768_DCS_write_1A_1P(0xC6,0x05);
		TC358768_DCS_write_1A_1P(0xC7,0x01);
		TC358768_DCS_write_1A_1P(0xD0,0x00);
		TC358768_DCS_write_1A_1P(0xD1,0x04);
		TC358768_DCS_write_1A_1P(0xD2,0x04);
		TC358768_DCS_write_1A_1P(0xD3,0x01);
		TC358768_DCS_write_1A_1P(0xD4,0x02);
		TC358768_DCS_write_1A_1P(0xD5,0x03);
		TC358768_DCS_write_1A_1P(0xD6,0x04);

		//TC358768_DCS_write_1A_1P(0xB0,0x06);//PAGE 6
		//TC358768_DCS_write_1A_1P(0xB8,0x85);//BTA Enable : BTA->TX procedure function enable.
		TC358768_DCS_write_1A_1P(MIPI_DCS_SET_PIXEL_FORMAT,0X70); //RGB24 bit format

		TC358768_DCS_write_1A_0P(0x11);
		udelay(200000);
		TC358768_DCS_write_1A_0P(0x32);
		TC358768_DCS_write_1A_0P(0x29);
		udelay(200000);
		printf("TRULY LCD Panel initialization is done properly\n");
		writel(0x1000,DSI_CLKMGR_CFG + DSI_BASE_ADDR);
}

#if 0
void do_mipi_lcd_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mipi_lcd_init();
}

U_BOOT_CMD(mipi_lcd_init,     1,      1,      do_mipi_lcd_init,
        "To initialize Truly Display",
        " "
);
#endif
void mipi_lcd_color_pixel(unsigned int width,unsigned int height,unsigned long color)
{
    char *dest = (char*)CONFIG_MIPI_DSI_FB2_ADDR;
    int i=0,j=0;

        for( i = 0;  i<height; i++){
        for( j = 0;  j<width; j++){

                *dest =  ((color>>24) & 0xff);
                //color >>= 8;
                dest++;

                *dest =  ((color>>16) & 0xff);
                //color >>= 8;
                dest++;

                *dest =  ((color>>8) & 0xff);
                dest++;

                //*dest = (color & 0x00);
                //dest++;
        }
        //dest = (readl(LCDUPBASE) + (width*4*i));
   }
}

void mipi_lcd_color_bar(unsigned int width,unsigned int height,unsigned long color)
{
    char *dest = (char*)CONFIG_MIPI_DSI_FB2_ADDR;
    int i=0,j=0;

        for( i = 0;  i<height; i++){
	color=0xff000000; //BGR , So Blue
        for( j = 0;  j<width/3; j++){
                *dest =  ((color>>24) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>16) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>8) & 0xff);
                dest++;
                //*dest = (color & 0x00);
                //dest++;
        }
	color=0x00ff0000; //BGR, so Green
        for( ;  j<(2*width)/3; j++){
                *dest =  ((color>>24) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>16) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>8) & 0xff);
                dest++;
                //*dest = (color & 0x00);
                //dest++;
        }
	color=0x0000ff00; //BGR, So Red
        for( ;  j<width; j++){
                *dest =  ((color>>24) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>16) & 0xff);
                //color >>= 8;
                dest++;
                *dest =  ((color>>8) & 0xff);
                dest++;
                //*dest = (color & 0x00);
                //dest++;
        }

        //dest = (readl(LCDUPBASE) + (width*4*i));
   }
}



void do_dsi_color_pattern(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	printf("Going to display Blue colour on MIPI DSI Panel.....\n");
	mipi_lcd_color_pixel(800,1280,0xff000000); //BGR , SO Blue Colour (BGR Colour Pattern)

	for(i=1;i<=100;i++)
	udelay(1000);

	printf("Going to display Green colour on MIPI DSI Panel.....\n");
	mipi_lcd_color_pixel(800,1280,0x00ff0000); //Green Colour (BGR Colour Pattern)

	for(i=1;i<=100;i++)
	udelay(1000);

	printf("Going to display Red colour on MIPI DSI Panel.....\n");
	mipi_lcd_color_pixel(800,1280,0x0000ff00); //RED Colour (BGR Colour Pattern)

	for(i=1;i<=100;i++)
	udelay(1000);

	printf("Going to display Green,Blue,Red colour bar pattern  on MIPI DSI Panel.....\n");
	mipi_lcd_color_bar(800,1280,0x00000000);

}

U_BOOT_CMD(dsi_color_pattern,     1,      1,      do_dsi_color_pattern,
        "To print dsi color pattern on Truly Panel",
        " "
);


/* Function:
 * lcd_display_bitmap() ---- Show BMP Image 
 * Display the BMP file located at address bmp_image(Only uncompressed).
 * Parameter:
 * bmp_image -- RAM address where BMP Image stored 
 * x -- x Coordinate 
 * y -- y Coordinate
 * return: 0 (Success), 1 (Error)  
 */     

int dsi_display_bitmap_custom(ulong bmp_image, int x, int y)
{       
        ushort *cmap_base = NULL;
        ushort i, j;
        bmp_image_t *bmp=(bmp_image_t *)bmp_image;
        uchar *bmap, *bmap2,*bmap3 ;
        unsigned long width, height,width2, height2,width3, height3, byte_width;
        int nbytes;
	unsigned colors, bmp_bpix,bpix;
	unsigned long pwidth = panel_info2[0].vl_col;
	unsigned long pheight = panel_info2[0].vl_row;

	printf("Display-Panel: w=%d x h=%d  \n",(int)pwidth, (int)pheight);

	if (!((bmp->header.signature[0]=='B') &&
                (bmp->header.signature[1]=='M'))) {
                printf ("Error: no valid bmp image at %lx\n", bmp_image);
                return 1;
        }

         width = width2 = width3 = le32_to_cpu (bmp->header.width);
         height = height2 = height3 = le32_to_cpu (bmp->header.height);
         bmp_bpix = le16_to_cpu(bmp->header.bit_count);
         colors = bmp_bpix;

	 bpix = panel_info2[0].vl_bpix;

	if ((bpix != 1) && (bpix != 8) && (bpix != 16) && (bpix != 24))  {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix, bmp_bpix);
                return 1;
        }

        /* We support displaying 8bpp BMPs on 16bpp LCDs */
        if (bpix != bmp_bpix && (bmp_bpix != 8 || bmp_bpix != 16 || bmp_bpix != 24)) {
                printf ("Error: %d bit/pixel mode, but BMP has %d bit/pixel\n",
                        bpix,
                        le16_to_cpu(bmp->header.bit_count));
                return 1;
        }

	printf("Display-bmp: w=%d x h=%d  with %d colors\n",(int)width, (int)height, (int)colors);

	//lcd_line_length = (panel_info2[0].vl_col * NBITS (panel_info2[0].vl_bpix)) / 8;
	
	 if(bpix == 8){
                nbytes = 1;
        } else if(bpix == 16){
                nbytes = 2;
        } else if(bpix == 24){
                nbytes = 3;
        } else{
                printf("Error: %d bit/pixel mode", bpix);
                return 1;
        }

        if ((x + width)>pwidth)
                width = pwidth - x;
        if ((y + height)>panel_info2[0].vl_row)
                height = panel_info2[0].vl_row - y;

	printf("Display-bmp: w=%d x h=%d  with %d colors\n",(int)width, (int)height, (int)colors);
        /* CLCD Framebuffer calculation*/
 	char *fb = CONFIG_MIPI_DSI_FB2_ADDR;

	bmap = bmap2 = bmap3 = (uchar *)bmp + le32_to_cpu (bmp->header.data_offset);

	for (i = 0; i < height; ++i) {
             for (j = 0; j < width; j++) {


/* BMP files has pixels value stored (default) as BGR, Little Endian format
 * and CLCD controller is also programmed to read pixel data in little endian format 
 * (shikhara_board.c - clcd_parameter_init()).
 */
                *(fb++) = bmap[0]; //B
                *(fb++) = bmap[1]; //G
                *(fb++) = bmap[2]; //R
                //fb++;  // For 24-bit it takes 4 bytes, 1 extra byte is empty.

                bmap += 3;
           }
	   //unused space
	   for(j=width;j<pwidth;j++) {

                *(fb++) = 0x00;
                *(fb++) = 0x00;
                *(fb++) = 0x00;
                //fb++;  // For 24-bit it takes 4 bytes, 1 extra byte is empty.
	   }	
	}
}

