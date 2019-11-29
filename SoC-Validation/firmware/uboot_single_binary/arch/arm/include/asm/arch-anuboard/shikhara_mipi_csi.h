#ifndef __SHIKHARA_MIPI_CSI_H__
#define __SHIKHARA_MIPI_CSI_H__


//***************************************************************************
// MIPI_CSI2 base address and MIPI_CSI2 register base address
//***************************************************************************
#include <asm/arch/shikhara_map.h>
 
#define CSI2_VERSION           (CSI2_BASE_ADDR + 0x000)
#define CSI2_N_LANES           (CSI2_BASE_ADDR + 0x004)
#define CSI2_PHY_SHUTDOWNZ     (CSI2_BASE_ADDR + 0x008)
#define CSI2_DPHY_RSTZ         (CSI2_BASE_ADDR + 0x00C)
#define CSI2_RESETN            (CSI2_BASE_ADDR + 0x010)
#define CSI2_PHY_STATE         (CSI2_BASE_ADDR + 0x014)
#define CSI2_DATA_IDS_1        (CSI2_BASE_ADDR + 0x018)
#define CSI2_DATA_IDS_2        (CSI2_BASE_ADDR + 0x01C)
#define CSI2_ERR1              (CSI2_BASE_ADDR + 0x020)
#define CSI2_ERR2              (CSI2_BASE_ADDR + 0x024)
#define CSI2_MSK1              (CSI2_BASE_ADDR + 0x028)
#define CSI2_MSK2              (CSI2_BASE_ADDR + 0x02C)
#define CSI2_PHY_TEST_CTRL0    (CSI2_BASE_ADDR + 0x030)
#define CSI2_PHY_TEST_CTRL1    (CSI2_BASE_ADDR + 0x034)

#define PHY_TESTCLR     (1)
#define PHY_TESTCLK     (1<<1)
#define PHY_TESTEN      (1<<16)

//Register: PHY_STATE bit fields
#define PHY_STOPSTATEDATA_0 4
#define PHY_STOPSTATEDATA_1 5
#define PHY_RXCLKACTIVEHS 8
#define PHY_STOPSTATECLK  10

u8 mipi_csi_init(void);
u8 mipi_csi_close(void);
u8 mipi_csi_get_on_lanes(void);
u8 mipi_csi_set_on_lanes(u8 lanes);
u8 mipi_csi_shut_down_phy(u8 shutdown);
u8 mipi_csi_reset_phy(void);
u8 mipi_csi_reset_controller(void);
u8 mipi_csi_event_disable(u32 mask, u8 err_reg_no);
u8 mipi_csi_event_enable(u32 mask, u8 err_reg_no);
u32 mipi_csi_event_get_source(u8 err_reg_no);
/******************************************************************************
 * register access methods
******************************************************************************/
u32 mipi_csi_read(u32 address);
u8 mipi_csi_write(u32 address, u32 data);
u8 mipi_csi_core_write_part(u32 address, u32 data, u8 shift, u8 width);
u32 mipi_csi_core_read_part(u32 address, u8 shift, u8 width);
#endif


