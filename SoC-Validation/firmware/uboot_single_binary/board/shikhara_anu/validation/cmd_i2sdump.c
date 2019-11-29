#include <common.h>
#include <asm/arch/shikhara_i2s.h>
#include <asm/arch/shikhara_map.h>

#define DEBUG_I2S	1
#ifdef DEBUG_I2S
 	#define dbg(x...) printf(x)
#else
 	#define dbg(x...) do { } while (0)
#endif

static inline u32 i2s_read_reg(int io_base, int reg)
{
    return readl(io_base + reg);
}


int do_i2sdump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dbg("***********Dumping I2S Registers*************\n");
	dbg("IER:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,IER));		
	dbg("IRER:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,IRER));		
	dbg("ITER:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,ITER));		
	dbg("CER:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,CER));		
	dbg("CCR:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,CCR));		
	dbg("TXFFR:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,TXFFR));		
	dbg("TER0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,TER0));		
	dbg("TCR0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,TCR0));		
	dbg("ISR0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,ISR0));		
	dbg("IMR0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,IMR0));		
	dbg("TFCR0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,TFCR0));		
	dbg("TFF0:0x%x\n",i2s_read_reg(SHIKHARA_I2S_BASE,TFF0));		

}

U_BOOT_CMD(
 	i2sdump, 1, 0, do_i2sdump,
 	"i2sdump - display I2S registers dump",
	"Display dump of I2S registers\n"
 	);
