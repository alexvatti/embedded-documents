#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <i2c.h>
#include <asm/arch/shikhara_i2c.h>
#include <asm/arch/shikhara_map.h>
#define DEBUG_I2C
#ifdef DEBUG_I2C
#define dbg(x...)       printf(x)
#endif


static struct i2c_regs *i2c_regs_p =
    (struct i2c_regs *)SHIKHARA_IC_BASE;

static int set_bus_num(unsigned int bus)
{
i2c_regs_p =(struct i2c_regs *)(SHIKHARA_IC_BASE+SHIKHARA_IC_SPACING*bus);
return 0;
}

int do_i2cdump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int reg,bus_num;
    bus_num = simple_strtoul(argv[1], NULL, 16);
    set_bus_num(bus_num);
    reg = readw(&i2c_regs_p->ic_con);
    dbg("IC_CON = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_tar);
    dbg("IC_TAR = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_cmd_data);
    dbg("IC_CMD_DATA = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_fs_scl_hcnt);
    dbg("IC_FS_SCL_HCNT = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_fs_scl_lcnt); 
    dbg("IC_FS_SCL_HCNT = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_ss_scl_hcnt);
    dbg("IC_SS_SCL_HCNT = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_ss_scl_lcnt);
    dbg("IC_SS_SCL_LCNT = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_intr_mask);
    dbg("IC_INTR_MASK = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_rx_tl);
    dbg("IC_RX_TL = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_tx_tl);
    dbg("IC_TX_TL = %x\n",reg); 
    reg = readw(&i2c_regs_p->ic_status);
    dbg("IC_STATUS = %x\n",reg);
}

U_BOOT_CMD(
        i2cdump,2 ,      1,      do_i2cdump,
        "I2C dump command",
        "i2cdump bus_num  - shows all i2c registers"
);

