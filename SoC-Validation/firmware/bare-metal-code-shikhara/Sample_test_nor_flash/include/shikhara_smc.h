#ifndef __SHIKHARA_SMC_H__
#define __SHIKHARA_SMC_H__ 1

/* Register values for using NOR interface of SMC Controller */
#define NOR_SET_CYCLES ((0x0 << 20) | /* set_t6 or we_time from sram_cycles */ \
                        (0x1 << 17) | /* set_t5 or t_tr from sram_cycles */    \
                        (0x2 << 14) | /* set_t4 or t_pc from sram_cycles */    \
                        (0x5 << 11) | /* set_t3 or t_wp from sram_cycles */    \
                        (0x2 << 8)  | /* set_t2 t_ceoe from sram_cycles */     \
                        (0x7 << 4)  | /* set_t1 t_wc from sram_cycles */       \
                        (0x7))        /* set_t0 t_rc from sram_cycles */

#define NOR_SET_OPMODE ((0x1 << 13) | /* set_burst_align,set to 32 beats */    \
                        (0x1 << 12) | /* set_bls,set to default */             \
                        (0x0 << 11) | /* set_adv bit, set to default */        \
                        (0x0 << 10) | /* set_baa, we don't use baa_n */        \
                        (0x0 << 7)  | /* set_wr_bl,write brust len,set to 0 */ \
                        (0x0 << 6)  | /* set_wr_sync, set to 0 */              \
                        (0x0 << 3)  | /* set_rd_bl,read brust len,set to 0 */  \
                        (0x0 << 2)  | /* set_rd_sync, set to 0 */              \
                        (0x1))        /* set_mw, memory width, 16bits width*/
                                      /* 0x00002000 */

#define NOR_DIRECT_CMD ((0x1 << 23) | /* Chip 0 from interface 0 */            \
                        (0x2 << 21) | /* UpdateRegs operation */               \
                        (0x0 << 20) | /* No ModeReg write */                   \
                        (0x0))        /* Addr, not used in UpdateRegs */
                                      /* 0x01400000 */


/* Memory controller configuration register offset */
#define SMC_MC_STATUS           	0x000   /* Controller status reg, RO */
#define SMC_MC_INTERFACE_CONFIG 	0x004   /* Interface config reg, RO */
#define SMC_MC_SET_CONFIG               0x008   /* Set configuration reg, WO */
#define SMC_MC_CLR_CONFIG               0x00C   /* Clear config reg, WO */
#define SMC_MC_DIRECT_CMD               0x010   /* Direct command reg, WO */
#define SMC_MC_SET_CYCLES               0x014   /* Set cycles register, WO */
#define SMC_MC_SET_OPMODE               0x018   /* Set opmode register, WO */
#define SMC_MC_REFRESH_PERIOD_0 	0x020   /* Refresh period_0 reg, RW */
#define SMC_MC_REFRESH_PERIOD_1 	0x024   /* Refresh period_1 reg, RW */

/* CFI Command Set */
#define  CFI_CMD_BLK_LCK_STP		0x0060
#define  CFI_CMD_BLK_UNLCK		0x00D0
#define  CFI_CMD_BLK_ERASE		0x0020
#define	 CFI_CMD_BLK_ERASE_CNFM		0x00D0
#define  CFI_CMD_WRITE			0x0040
#define  CFI_CMD_READ			0x00FF
#define  CFI_CMD_READSTATUS         	0x0070
#define  CFI_CMD_RD_CFG			0x0003

#define  CFI_CMD_RD_MAN_ID		0x0000
#define  CFI_CMD_RD_DEV_ID		0x0001

enum cfi_flash_cmd {
	CFI_CMD_RESET = 0xff,
	CFI_CMD_READ_ID = 0x90,
	CFI_CMD_CFI = 0x98,
	CFI_CMD_READ_STATUS = 0x70,
	CFI_CMD_CLEAR_STATUS = 0x50,
	CFI_CMD_BLOCK_ERASE = 0x20,
	CFI_CMD_ERASE_CONFIRM = 0xD0,
	CFI_CMD_PROTECT = 0x60
};

/* protection commands */
enum flash_prot_cmd {
	CFI_PROT_LOCK = 0x01,
	CFI_PROT_UNLOCK = 0xD0,
	CFI_PROT_LOCKDOWN = 0x2F
};

/* offsets from base */
enum flash_offset {
	CFI_OFFSET_MANUFACTURER_ID = 0x00,
	CFI_OFFSET_DEVICE_ID = 0x01,
	CFI_OFFSET_INTEL_PROTECTION = 0x81,
	CFI_OFFSET_CFI_RESP = 0x10
};

/* offsets from block base */
enum flash_block_offset {
	CFI_OFFSET_BLOCK_LOCKSTATE = 0x02
};

/* status masks */
enum flash_status {
	CFI_STATUS_READY = 0x80,
	CFI_STATUS_ERASE_SUSPENDED = 0x40,
	CFI_STATUS_ERASE_ERROR = 0x20,
	CFI_STATUS_PROGRAM_ERROR = 0x10,
	CFI_STATUS_VPP_LOW = 0x08,
	CFI_STATUS_PROGRAM_SUSPENDED = 0x04,
	CFI_STATUS_LOCKED_ERROR = 0x02,
	CFI_STATUS_RESERVED = 0x01
};

int smc_nor_init(void);
int smc_nor_blk_erase(unsigned int addr);
void smc_nor_blk_unlock(unsigned int addr);
int smc_nor_verify(void);
void smc_nor_read(void);
int smc_nor_write();


#endif

