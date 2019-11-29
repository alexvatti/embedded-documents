/*Register Defination and function declarations*/
#include <asm/arch/shikhara_mipi_dsi_local.h>
#include <common.h>
#ifndef __SHIKHARA_MIPI_DSI_HAL_H__
#define __SHIKHARA_MIPI_DSI_HAL_H__


//***************************************************************************
// MIPI_DSI register base address
//***************************************************************************


#define  DSI_VERSION               0x000
#define  DSI_PWR_UP                0x004
#define  DSI_CLKMGR_CFG            0x008
#define  DSI_DPI_VCID              0x00C
#define  DSI_DPI_COLOR_CODING      0x010
#define  DSI_DPI_CFG_POL           0x014
#define  DSI_DPI_LP_CMD_TIM        0x018
#define  DSI_DBI_VCID              0x01C
#define  DSI_DBI_CFG               0x020
#define  DSI_DBI_PART_EN           0x024
#define  DSI_DBI_CMDSIZE           0x028
#define  DSI_PKTHDLR_CFG           0x02C
#define  DSI_GEN_VCID              0x030
#define  DSI_MODE_CFG              0x034
#define  DSI_VID_MODE_CFG          0x038
#define  DSI_VID_PKT_SIZE          0x03C
#define  DSI_VID_NUM_CHUNKS        0x040
#define  DSI_VID_NULL_SIZE         0x044
#define  DSI_VID_HSA_TIME          0x048
#define  DSI_VID_HBP_TIME          0x04C
#define  DSI_VID_HLINE_TIME        0x050
#define  DSI_VID_VSA_LINES         0x054
#define  DSI_VID_VBP_LINES         0x058
#define  DSI_VID_VFP_LINES         0x05C
#define  DSI_VID_VACTIVE_LINES     0x060
#define  DSI_EDPI_CMD_SIZE         0x064
#define  DSI_CMD_MODE_CFG          0x068
#define  DSI_GEN_HDR               0x06C
#define  DSI_GEN_PLD_DATA          0x070
#define  DSI_CMD_PKT_STATUS        0x074
#define  DSI_TO_CNT_CFG            0x078
#define  DSI_HS_RD_TO_CNT          0x07C
#define  DSI_LP_RD_TO_CNT          0x080
#define  DSI_HS_WR_TO_CNT          0x084
#define  DSI_LP_WR_TO_CNT     	   0x088
#define  DSI_BTA_TO_CNT            0x08C
#define  DSI_SDF_3D                0x090
#define  DSI_LPCLK_CNTRL           0x094
#define  DSI_PHY_TMR_LPCLK_CFG     0x098
#define  DSI_PHY_TMR_CFG           0x09C
#define  DSI_PHY_RSTZ              0x0A0
#define  DSI_PHY_IF_CFG            0x0A4
#define  DSI_PHY_ULPS_CTRL         0x0A8
#define  DSI_PHY_TX_TRIGGERS       0x0AC
#define  DSI_PHY_STATUS            0x0B0
#define  DSI_PHY_TEST_CTRL0        0x0B4
#define  DSI_PHY_TEST_CTRL1        0x0B8
#define  DSI_INT_ST0               0x0BC
#define  DSI_INT_ST1               0x0C0
#define  DSI_INT_MSK0              0x0C4
#define  DSI_INT_MSK1              0x0C8

//PWR_UP 0x04
#define	DSI_PWRUP_RESET			(0x0 << 0)
#define	DSI_PWRUP_POWERUP		(0x1 << 0)

//DPI_VCID 0x00c
#define	DSI_DPI_VCID_SHIFT		(0)
#define	DSI_DPI_VCID_MASK		(0x3)

//DPI_COLOR_CODING 0x010
#define	DSI_DPI_CFG_COLORCODE_SHIFT	(0)
#define	DSI_DPI_CFG_COLORCODE_MASK	(0xF)
#define	DSI_DPI_CFG_COLORCODE_EN18LOOSELY (0x1 << 8)

//DPI_CFG_POL 0x014
#define	DSI_DPI_CFG_POL_DATAEN_ACT_LOW	(0x1 << 0)
#define	DSI_DPI_CFG_POL_DATAEN_ACT_HIGH	(0x0 << 0)
#define	DSI_DPI_CFG_POL_VSYNC_ACT_LOW	(0x1 << 1)
#define	DSI_DPI_CFG_POL_VSYNC_ACT_HIGH	(0x0 << 1)
#define	DSI_DPI_CFG_POL_HSYNC_ACT_LOW	(0x1 << 2)
#define	DSI_DPI_CFG_POL_HSYNC_ACT_HIGH	(0x0 << 2)
#define	DSI_DPI_CFG_POL_SHUTD_ACT_LOW	(0x1 << 3)
#define	DSI_DPI_CFG_POL_SHUTD_ACT_HIGH	(0x0 << 3)
#define	DSI_DPI_CFG_POL_COLORMODE_ACT_LOW	(0x1 << 4)
#define	DSI_DPI_CFG_POL_COLORMODE_ACT_HIGH	(0x0 << 4)

//PCKHDL_CFG 0x02C
#define	DSI_PCKHDL_CFG_EN_EOTP_TX	(0x1 << 0)
#define	DSI_PCKHDL_CFG_EN_EOTP_RX	(0x1 << 1)
#define	DSI_PCKHDL_CFG_EN_BTA		(0x1 << 2)
#define	DSI_PCKHDL_CFG_EN_ECC_RX	(0x1 << 3)
#define	DSI_PCKHDL_CFG_EN_CRC_RX	(0x1 << 4)

//GEN_VCID 0x030
#define	DSI_GEN_VCID_RX_MASK	(0x3)
#define	DSI_GEN_VCID_RX_SHIFT	(0)
	
//MODE_CFG 0x034
#define DSI_MODE_CFG_COMMAND_MODE (1<<0)
#define DSI_MODE_CFG_VIDEO_MODE   (0<<0)

//VID_MODE_CFG 0x038
#define	DSI_VID_MODE_CFG_EN		(0x1 << 0)
#define	DSI_VID_MODE_CFG_EN_BURSTMODE	(0x3 << 1)
#define	DSI_VID_MODE_CFG_TYPE_MASK	(0x3)
#define	DSI_VID_MODE_CFG_TYPE_SHIFT	(0)
#define	DSI_VID_MODE_CFG_EN_LP_VSA	(0x1 << 8)
#define	DSI_VID_MODE_CFG_EN_LP_VBP	(0x1 << 9)
#define	DSI_VID_MODE_CFG_EN_LP_VFP	(0x1 << 10)
#define	DSI_VID_MODE_CFG_EN_LP_VACT	(0x1 << 11)
#define	DSI_VID_MODE_CFG_EN_LP_HBP	(0x1 << 12)
#define	DSI_VID_MODE_CFG_EN_LP_HFP	(0x1 << 13)
#define	DSI_VID_MODE_CFG_EN_FRAME_ACK	(0x1 << 14)
#define	DSI_VID_MODE_CFG_EN_LP_CMD	(0x1 << 15)

#define	DSI_VID_MODE_CFG_EN_LP_MODE (DSI_VID_MODE_CFG_EN_LP_VSA | \
		DSI_VID_MODE_CFG_EN_LP_VBP | \
		DSI_VID_MODE_CFG_EN_LP_VFP | \
		DSI_VID_MODE_CFG_EN_LP_HFP | \
		DSI_VID_MODE_CFG_EN_LP_HBP | \
		DSI_VID_MODE_CFG_EN_LP_VACT)

//VID_PKT_SIZE  0x03c
#define	DSI_VID_PKT_CFG_VID_PKT_SZ_MASK		(0x1FFF)
#define	DSI_VID_PKT_CFG_VID_PKT_SZ_SHIFT	(0)

//VID_NUM_CHUNKS 0x040
#define	DSI_VID_PKT_CFG_NUM_CHUNKS_MASK		(0xfff)
#define	DSI_VID_PKT_CFG_NUM_CHUNKS_SHIFT	(12)

//VID_NULL_SIZE 0x044
#define	DSI_VID_PKT_CFG_NULL_PKT_SZ_MASK	(0xfff)
#define	DSI_VID_PKT_CFG_NULL_PKT_SZ_SHIFT	(12)

#if 0
#define	MIPI_DSI_CMD_MODE_CFG_EN_LOWPOWER	(0x1FFF)
#define	MIPI_DSI_CMD_MODE_CFG_EN_CMD_MODE	(0x1 << 0)
#endif

//VID_HSA_TIME 0x048
#define	DSI_TME_LINE_CFG_HSA_TIME_MASK		(0xfff)
#define	DSI_TME_LINE_CFG_HSA_TIME_SHIFT		(0)

//VID_HBP_TIME 0X04C
#define	DSI_TME_LINE_CFG_HBP_TIME_MASK		(0xfff)
#define	DSI_TME_LINE_CFG_HBP_TIME_SHIFT		(0)

//VID_HLINE_TIME 0x050
#define	DSI_TME_LINE_CFG_HLINE_TIME_MASK	(0x7fff)
#define	DSI_TME_LINE_CFG_HLINE_TIME_SHIFT	(0)

//VID_VSA_LINES 0x054
#define	DSI_VTIMING_CFG_VSA_LINES_MASK		(0x3ff)
#define	DSI_VTIMING_CFG_VSA_LINES_SHIFT		(0)

//VID_VBP_LINES 0x058
#define	DSI_VTIMING_CFG_VBP_LINES_MASK		(0x3ff)
#define	DSI_VTIMING_CFG_VBP_LINES_SHIFT		(0)

//VID_VFP_LINES 0x05C
#define	DSI_VTIMING_CFG_VFP_LINES_MASK		(0x3ff)
#define	DSI_VTIMING_CFG_VFP_LINES_SHIFT		(0)

//VID_VACTIVE_LINES 0x060
#define	DSI_VTIMING_CFG_V_ACT_LINES_MASK	(0x3fff)
#define	DSI_VTIMING_CFG_V_ACT_LINES_SHIFT	(0)

//PHY_TMR_CFG 0X09C
#define	DSI_PHY_TMR_CFG_LP2HS_TIME_MASK		(0xff)
#define	DSI_PHY_TMR_CFG_LP2HS_TIME_SHIFT	(16)

#define	DSI_PHY_TMR_CFG_HS2LP_TIME_MASK		(0xff)
#define	DSI_PHY_TMR_CFG_HS2LP_TIME_SHIFT	(24)

//PHY_IF_CFG 0X0A4
#define	DSI_PHY_IF_CFG_N_LANES_MASK		(0x3)
#define	DSI_PHY_IF_CFG_N_LANES_SHIFT		(0)
#define	DSI_PHY_IF_CFG_WAIT_TIME_MASK		(0xff)
#define	DSI_PHY_IF_CFG_WAIT_TIME_SHIFT		(8)

//PHY_RSTZ 0x0A0
#define	DSI_PHY_RSTZ_EN_CLK			(0x1 << 2)
#define	DSI_PHY_RSTZ_DISABLE_RST		(0x1 << 1)
#define	DSI_PHY_RSTZ_DISABLE_SHUTDOWN		(0x1 << 0)
#define	DSI_PHY_RSTZ_RST			(0x0)

//PHY_STATUS 0X0B0
#define	DSI_PHY_STATUS_LOCK			(0x1 << 0)
#define	DSI_PHY_STATUS_STOPSTATE_CLK_LANE	(0x1 << 2)


//GEN_HDR 0x06c
#define	DSI_GEN_HDR_TYPE_MASK			(0xff)
#define	DSI_GEN_HDR_TYPE_SHIFT			(0)
#define	DSI_GEN_HDR_DATA_MASK			(0xffff)
#define	DSI_GEN_HDR_DATA_SHIFT			(8)

//CMD_PKT_STATUS 0x074
#define	DSI_CMD_PKT_STATUS_GEN_CMD_EMPTY	(0x1 << 0)
#define	DSI_CMD_PKT_STATUS_GEN_CMD_FULL		(0x1 << 1)
#define	DSI_CMD_PKT_STATUS_GEN_PLD_W_EMPTY	(0x1 << 2)
#define	DSI_CMD_PKT_STATUS_GEN_PLD_W_FULL	(0x1 << 3)
#define	DSI_CMD_PKT_STATUS_GEN_PLD_R_EMPTY	(0x1 << 4)
#define	DSI_CMD_PKT_STATUS_GEN_RD_CMD_BUSY	(0x1 << 6)
#if 0
#define	DSI_ERROR_MSK0_ALL_MASK			(0x1fffff)
#define	DSI_ERROR_MSK1_ALL_MASK			(0x3ffff)

#define	DSI_PHY_IF_CTRL_RESET			(0x0<<0)
#define	DSI_PHY_IF_CTRL_RESET			(0x0)

#define	DSI_PHY_IF_CTRL_TX_REQ_CLK_HS		(0x1 << 0)
#define	DSI_PHY_IF_CTRL_TX_REQ_CLK_ULPS		(0x1 << 1)
#define	DSI_PHY_IF_CTRL_TX_EXIT_CLK_ULPS	(0x1 << 2)
#define	DSI_PHY_IF_CTRL_TX_REQ_DATA_ULPS	(0x1 << 3)
#define	DSI_PHY_IF_CTRL_TX_EXIT_DATA_ULPS	(0x1 << 4)
#define	DSI_PHY_IF_CTRL_TX_TRIG_MASK		(0xF)
#define	DSI_PHY_IF_CTRL_TX_TRIG_SHIFT		(5)
#endif

#define	DSI_PHY_CLK_INIT_COMMAND		(0x44)
#define	DSI_GEN_PLD_DATA_BUF_SIZE		(0x4)

/* DSI PIXEL PLL CLOCK */

#define DSI_PIX_PLL_CTRL0 			0x174 
#define DSI_PIX_PLL_CTRL1 			0x178 

//DSI_PHY_RSTZ
#define PHY_ENABLECLK				(1<<2)
#define PHY_UNRSTZ				(1<<1)
#define PHY_UNSHUTDOWNZ				(1<<0)


#define CLK_TLPX 				0x10
#define CLK_THS_PREPARE 			0x11
#define CLK_THS_ZERO 				0x12
#define CLK_THS_TRAIL 				0x13
#define CLK_TWAKEUP 				0x14

#define DATA_TLPX(x) 		(0x20 + ((x) << 4))
#define DATA_THS_PREPARE(x) 	(0x21 + ((x) << 4))
#define DATA_THS_ZERO(x) 	(0x22 + ((x) << 4))
#define DATA_THS_TRAIL(x) 	(0x23 + ((x) << 4))
#define DATA_TTA_GO(x) 		(0x24 + ((x) << 4))
#define DATA_TTA_GET(x) 	(0x25 + ((x) << 4))
#define DATA_TWAKEUP(x) 	(0x26 + ((x) << 4))

dsih_error_t mipi_dsih_hal_dpi_video_mode_type(dsih_ctrl_t * instance, dsih_video_mode_t type);
void mipi_dsih_write_part(dsih_ctrl_t * instance, uint32_t reg_address, uint32_t data, uint8_t shift, uint8_t width);
uint32_t mipi_dsih_read_word(dsih_ctrl_t * instance, uint32_t reg_address);
uint32_t mipi_dsih_read_part(dsih_ctrl_t * instance, uint32_t reg_address, uint8_t shift, uint8_t width);
uint32_t mipi_dsih_hal_get_version(dsih_ctrl_t * instance);
void mipi_dsih_hal_power(dsih_ctrl_t * instance, int on);
void mipi_dsih_hal_dpi_video_mode_en(dsih_ctrl_t * instance, int enable);
dsih_error_t mipi_dsih_phy_bta_time(dsih_ctrl_t * instance, uint16_t no_of_byte_cycles);
/* Generic command interface */
void mipi_dsih_hal_gen_rd_vc(dsih_ctrl_t * instance, uint8_t vc);
void mipi_dsih_hal_dpi_null_packet_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_multi_packet_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_tx_escape_division(dsih_ctrl_t * instance, uint8_t tx_escape_division);
void mipi_dsih_hal_dpi_lp_during_hfp(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_lp_during_hbp(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_lp_during_vactive(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_lp_during_vfp(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_lp_during_vbp(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_lp_during_vsync(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_video_vc(dsih_ctrl_t * instance, uint8_t vc);
dsih_error_t mipi_dsih_hal_dpi_color_coding(dsih_ctrl_t * instance, dsih_color_coding_t color_coding);
void mipi_dsih_hal_dpi_18_loosely_packet_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_dpi_color_mode_pol(dsih_ctrl_t * instance, int active_low);
void mipi_dsih_hal_dpi_shut_down_pol(dsih_ctrl_t * instance, int active_low);
void mipi_dsih_hal_dpi_hsync_pol(dsih_ctrl_t * instance, int active_low);
void mipi_dsih_hal_dpi_vsync_pol(dsih_ctrl_t * instance, int active_low);
void mipi_dsih_hal_dpi_dataen_pol(dsih_ctrl_t * instance, int active_low);
void mipi_dsih_hal_gen_eotp_rx_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_gen_eotp_tx_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_bta_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_gen_ecc_rx_en(dsih_ctrl_t * instance, int enable);
void mipi_dsih_hal_gen_crc_rx_en(dsih_ctrl_t * instance, int enable);
dsih_error_t mipi_dsih_hal_dpi_video_packet_size(dsih_ctrl_t * instance, uint16_t size);
dsih_error_t mipi_dsih_hal_dpi_chunks_no(dsih_ctrl_t * instance, uint16_t no);
dsih_error_t mipi_dsih_hal_dpi_null_packet_size(dsih_ctrl_t * instance, uint16_t size);
void mipi_dsih_hal_cmd_ack_en(dsih_ctrl_t * instance, int enable);
dsih_error_t mipi_dsih_hal_dcs_wr_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_dcs_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_dcs_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_dcs_wr_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_gen_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_gen_wr_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
dsih_error_t mipi_dsih_hal_gen_rd_tx_type(dsih_ctrl_t * instance, unsigned no_of_param, int lp);
void mipi_dsih_hal_max_rd_size_type(dsih_ctrl_t * instance, int lp);
void mipi_dsih_hal_dpi_hline(dsih_ctrl_t * instance, uint16_t time);
void mipi_dsih_hal_dpi_hbp(dsih_ctrl_t * instance, uint16_t time);
void mipi_dsih_hal_dpi_hsa(dsih_ctrl_t * instance, uint16_t time);
void mipi_dsih_hal_dpi_vactive(dsih_ctrl_t * instance, uint16_t lines);
void mipi_dsih_hal_dpi_vfp(dsih_ctrl_t * instance, uint16_t lines);
void mipi_dsih_hal_dpi_vbp(dsih_ctrl_t * instance, uint16_t lines);
void mipi_dsih_hal_dpi_vsync(dsih_ctrl_t * instance, uint16_t lines);
dsih_error_t mipi_dsih_phy_hs2lp_config(dphy_t * instance, uint8_t no_of_byte_cycles);
dsih_error_t mipi_dsih_phy_lp2hs_config(dphy_t * instance, uint8_t no_of_byte_cycles);
dsih_error_t mipi_dsih_phy_clk_lp2hs_config(dphy_t * instance, uint8_t no_of_byte_cycles);
dsih_error_t mipi_dsih_phy_clk_hs2lp_config(dphy_t * instance, uint8_t no_of_byte_cycles);
uint32_t mipi_dsih_hal_int_status_0(dsih_ctrl_t * instance, uint32_t mask);
uint32_t mipi_dsih_hal_int_status_1(dsih_ctrl_t * instance, uint32_t mask);
void mipi_dsih_hal_int_mask_0(dsih_ctrl_t * instance, uint32_t mask);
void mipi_dsih_hal_int_mask_1(dsih_ctrl_t * instance, uint32_t mask);
void mipi_dsih_hal_dpi_frame_ack_en(dsih_ctrl_t * instance, int enable);
dsih_error_t mipi_dsih_hal_dpi_video_mode_type(dsih_ctrl_t * instance, dsih_video_mode_t type);
/*D-PHY related functionalites*/
void mipi_dsih_dphy_clock_en(dphy_t * instance, int en);
void mipi_dsih_dphy_reset(dphy_t * instance, int reset);
void mipi_dsih_dphy_shutdown(dphy_t * instance, int powerup);
void mipi_dsih_dphy_stop_wait_time(dphy_t * instance, uint8_t no_of_byte_cycles);
void mipi_dsih_dphy_no_of_lanes(dphy_t * instance, uint8_t no_of_lanes);
void mipi_dsih_dphy_enable_hs_clk(dphy_t * instance, int enable);
void mipi_dsih_dphy_write_word(dphy_t * instance, uint32_t reg_address, uint32_t data);
void mipi_dsih_dphy_write_part(dphy_t * instance, uint32_t reg_address, uint32_t data, uint8_t shift, uint8_t width);
uint32_t mipi_dsih_dphy_read_word(dphy_t * instance, uint32_t reg_address);
uint32_t mipi_dsih_dphy_read_part(dphy_t * instance, uint32_t reg_address, uint8_t shift, uint8_t width);
void mipi_dsih_write_word(dsih_ctrl_t * instance, uint32_t reg_address, uint32_t data);

/* dsi console write functionailties*/
void	dsi_putc	(const char c);
void	dsi_puts	(const char *s);
extern short con_col;
extern short con_row;
#endif
