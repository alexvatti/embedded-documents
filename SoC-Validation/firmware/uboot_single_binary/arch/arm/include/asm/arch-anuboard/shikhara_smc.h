

#ifndef __ASM_ARCH_SMC_H__
#define __ASM_ARCH_SMC_H__

/* Register values for using NOR interface of SMC Controller */
#define NOR_SET_CYCLES ((0x0 << 20) | /* set_t6 or we_time from sram_cycles */ \
			(0x1 << 17) | /* set_t5 or t_tr from sram_cycles */    \
			(0x2 << 14) | /* set_t4 or t_pc from sram_cycles */    \
			(0x5 << 11) | /* set_t3 or t_wp from sram_cycles */    \
			(0x2 << 8)  | /* set_t2 t_ceoe from sram_cycles */     \
			(0x7 << 4)  | /* set_t1 t_wc from sram_cycles */       \
			(0x7))	      /* set_t0 t_rc from sram_cycles */

#define NOR_SET_OPMODE ((0x1 << 13) | /* set_burst_align,set to 32 beats */    \
			(0x1 << 12) | /* set_bls,set to default */	       \
			(0x0 << 11) | /* set_adv bit, set to default */	       \
			(0x0 << 10) | /* set_baa, we don't use baa_n */	       \
			(0x0 << 7)  | /* set_wr_bl,write brust len,set to 0 */ \
			(0x0 << 6)  | /* set_wr_sync, set to 0 */	       \
			(0x0 << 3)  | /* set_rd_bl,read brust len,set to 0 */  \
			(0x0 << 2)  | /* set_rd_sync, set to 0 */	       \
			(0x0))	      /* set_mw, memory width, 16bits width*/
				      /* 0x00002000 */
#define NOR_DIRECT_CMD ((0x1 << 23) | /* Chip 0 from interface 0 */	       \
			(0x2 << 21) | /* UpdateRegs operation */	       \
			(0x0 << 20) | /* No ModeReg write */		       \
			(0x0))	      /* Addr, not used in UpdateRegs */
				      /* 0x01400000 */

/* Register values for using SRAM interface of SMC Controller */
#define SRAM_SET_CYCLES (0x00125155)
#define SRAM_SET_OPMODE (0x00003000)
#define SRAM_DIRECT_CMD (0x00400000)	/* Chip 1 */



/* Memory controller configuration register offset */
#define SMC_MC_STATUS		0x000	/* Controller status reg, RO */
#define SMC_MC_INTERFACE_CONFIG	0x004	/* Interface config reg, RO */
#define SMC_MC_SET_CONFIG		0x008	/* Set configuration reg, WO */
#define SMC_MC_CLR_CONFIG		0x00C	/* Clear config reg, WO */
#define SMC_MC_DIRECT_CMD		0x010	/* Direct command reg, WO */
#define SMC_MC_SET_CYCLES		0x014	/* Set cycles register, WO */
#define SMC_MC_SET_OPMODE		0x018	/* Set opmode register, WO */
#define SMC_MC_REFRESH_PERIOD_0	0x020	/* Refresh period_0 reg, RW */
#define SMC_MC_REFRESH_PERIOD_1	0x024	/* Refresh period_1 reg, RW */

/* Chip select configuration register offset */
#define SMC_CS_IF0_CHIP_0_OFFSET	0x100	/* Interface 0 chip 0 config */
#define SMC_CS_IF0_CHIP_1_OFFSET	0x120	/* Interface 0 chip 1 config */
#define SMC_CS_IF0_CHIP_2_OFFSET	0x140	/* Interface 0 chip 2 config */
#define SMC_CS_IF0_CHIP_3_OFFSET	0x160	/* Interface 0 chip 3 config */
#define SMC_CS_IF1_CHIP_0_OFFSET	0x180	/* Interface 1 chip 0 config */
#define SMC_CS_IF1_CHIP_1_OFFSET	0x1A0	/* Interface 1 chip 1 config */
#define SMC_CS_IF1_CHIP_2_OFFSET	0x1C0	/* Interface 1 chip 2 config */
#define SMC_CS_IF1_CHIP_3_OFFSET	0x1E0	/* Interface 1 chip 3 config */

/* User configuration register offset */
#define SMC_UC_STATUS_OFFSET	0x200	/* User status reg, RO */
#define SMC_UC_CONFIG_OFFSET	0x204	/* User config reg, WO */

/* Integration test register offset */
#define SMC_IT_OFFSET		0xE00

/* ID configuration register offset */
#define SMC_ID_PERIP_0_OFFSET	0xFE0
#define SMC_ID_PERIP_1_OFFSET	0xFE4
#define SMC_ID_PERIP_2_OFFSET	0xFE8
#define SMC_ID_PERIP_3_OFFSET	0xFEC
#define SMC_ID_PCELL_0_OFFSET	0xFF0
#define SMC_ID_PCELL_1_OFFSET	0xFF4
#define SMC_ID_PCELL_2_OFFSET	0xFF8
#define SMC_ID_PCELL_3_OFFSET	0xFFC

/* Add ECC reg, nand_cycles, sram_cycles and opmode_x_n registers
   There are multiple interfaces also we need to take care.*/

/* Register definitions */
/* ECC register offset */
#define SMC_ECC_IF0_OFFSET		0x300   /* Interface 0 ECC register */
#define SMC_ECC_IF1_OFFSET		0x400   /* Interface 1 ECC register */
#define SMC_ECC_STATUS_OFFSET(addr)	(0x000 + addr) /* ECC status register */
#define SMC_ECC_MEMCFG_OFFSET(addr)	(0x004 + addr) /* ECC mem config reg */
#define SMC_ECC_MEMCMD1_OFFSET(addr)(0x008 + addr) /*ECC mem cmd1 reg*/
#define SMC_ECC_MEMCMD2_OFFSET(addr)(0x00C + addr) /*ECC mem cmd2 reg*/
#define SMC_ECC_ADDR0_OFFSET(addr)	(0x010 + addr) /* ECC address0 reg */
#define SMC_ECC_ADDR1_OFFSET(addr)	(0x014 + addr) /* ECC address1 reg */
#define SMC_ECC_VALUE0_OFFSET(addr)	(0x018 + addr) /* ECC value 0 reg */
#define SMC_ECC_VALUE1_OFFSET(addr)	(0x01C + addr) /* ECC value 1 reg */
#define SMC_ECC_VALUE2_OFFSET(addr)	(0x020 + addr) /* ECC value 2 reg */
#define SMC_ECC_VALUE3_OFFSET(addr)	(0x024 + addr) /* ECC value 3 reg */
#define SMC_ECC_VALUE4_OFFSET(addr)	(0x028 + addr) /* ECC value 4 reg */

/* nand_cycles, sram_cycles and opmode_x_n registers offsets*/
#define SMC_FLASH_CYCLES(addr)	(0x000 + addr) /* NAND and SRAM cycle */
#define SMC_OPMODE(addr)		(0x004 + addr) /* Chip opmode reg */

extern int smc_init_nor(void);
extern int smc_init_sram(void);

#endif /* __ASM_ARCH_SMC_H__ */




