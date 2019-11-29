#include<stdio.h>

int main()
{
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

#define SMC_SRAM_DIRECT_CMD ((0x1 << 23) | /* Chip Select 2 from interface 0 */            \
                        (0x2 << 21) | /* UpdateRegs operation */               \
                        (0x0 << 20) | /* No ModeReg write */                   \
                        (0x0))

printf("values of set cycles command=0x%8x\n",SMC_SRAM_SET_CYCLES);
printf("values of set opmode command=0x%8x\n",SMC_SRAM_SET_OPMODE);
printf("values of direct command=0x%8x\n",SMC_SRAM_DIRECT_CMD);


}
