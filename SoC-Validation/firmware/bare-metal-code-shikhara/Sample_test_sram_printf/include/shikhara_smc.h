#ifndef __SHIKHARA_SMC_H__
#define __SHIKHARA_SMC_H__ 1

#define SMC_SRAM_SET_CYCLES ((0x1 << 20) | /* set_t6 or we_time from sram_cycles */ \
                        (0x1 << 17) | /* set_t5 or t_tr from sram_cycles */    \
                        (0x1 << 14) | /* set_t4 or t_pc from sram_cycles */    \
                        (0x2 << 11) | /* set_t3 or t_wp from sram_cycles */    \
                        (0x1 << 8)  | /* set_t2 t_ceoe from sram_cycles */     \
                        (0x5 << 4)  | /* set_t1 t_wc from sram_cycles */       \
                        (0x5))        /* set_t0 t_rc from sram_cycles */

#define SMC_SRAM_SET_OPMODE ((0x1 << 13) | /* set_burst_align,set to 32 beats */    \
                        (0x1 << 12) | /* set_bls,set to default */             \
                        (0x0 << 11) | /* set_adv bit, set to default */        \
                        (0x0 << 10) | /* set_baa, we don't use baa_n */        \
                        (0x0 << 7)  | /* set_wr_bl,write brust len,set to 0 */ \
                        (0x0 << 6)  | /* set_wr_sync, set to 0 */              \
                        (0x0 << 3)  | /* set_rd_bl,read brust len,set to 0 */  \
                        (0x0 << 2)  | /* set_rd_sync, set to 0 */              \
                        (0x0)) 

#define SMC_SRAM_DIRECT_CMD ((0x0 << 23) | /* Chip Select 0 from interface 0 */            \
                        (0x2 << 21) | /* UpdateRegs operation */               \
                        (0x0 << 20) | /* No ModeReg write */                   \
                        (0x0))

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



int smc_sram_init(void);
int smc_sram_verify(void);
void smc_sram_read(void);
void smc_sram_write(void);

#endif

