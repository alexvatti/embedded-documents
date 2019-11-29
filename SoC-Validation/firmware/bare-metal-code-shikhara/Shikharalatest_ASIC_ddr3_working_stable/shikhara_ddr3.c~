/* uMCTL2 Reg Offsets*/
#include<common.h>
#include<shikhara_ddr3.h>
#include<shikhara_map.h>
int ddr_init ()
{

   int temp, i;
   unsigned int soft_reg_rst4,ddr_reg_04;
#if 1
  /* DDR controller register*/ 
   *(volatile unsigned int *)(MSTR_REG) = (unsigned int) MSTR_DATA;
   *(volatile unsigned int *)(MRCTRL0_REG) = (unsigned int) MRCTRL0_DATA;
   *(volatile unsigned int *)(MRCTRL1_REG) = (unsigned int) MRCTRL0_DATA;
   *(volatile unsigned int *)(DERATEEN_REG) = (unsigned int) DERATEEN_DATA;
   *(volatile unsigned int *)(DERATEINT_REG) = (unsigned int) DERATEINT_DATA;
   *(volatile unsigned int *)(PWRCTL_REG) = (unsigned int) PWRCTL_DATA;
   *(volatile unsigned int *)(PWRTMG_REG) = (unsigned int) PWRTMG_DATA;
   *(volatile unsigned int *)(HWLPCTL_REG) = (unsigned int) HWLPCTL_DATA;
   *(volatile unsigned int *)(RFSHCTL0_REG) = (unsigned int) RFSHCTL0_DATA;
   *(volatile unsigned int *)(RFSHCTL1_REG) = (unsigned int) RFSHCTL1_REG;
   *(volatile unsigned int *)(RFSHCTL3_REG) = (unsigned int) RFSHCTL3_DATA;
   *(volatile unsigned int *)(RFSHTMG_REG) = (unsigned int) RFSHTMG_DATA;
// *(volatile unsigned int *)(MSTR_REG) = (unsigned int) MSTR_DATA;
// *(volatile unsigned int *)(STAT_REG) = (unsigned int)STAT_DATA;
   *(volatile unsigned int *)(ECCCFG0_REG) = (unsigned int) ECCCFG0_DATA;
   *(volatile unsigned int *)(ECCCFG1_REG) = (unsigned int) ECCCFG1_DATA;
   *(volatile unsigned int *)(ECCCLR_REG) = (unsigned int) ECCCLR_DATA;
   *(volatile unsigned int *)(ECCPOISONADDR0_REG) = (unsigned int) ECCPOISONADDR0_DATA;
   *(volatile unsigned int *)(ECCPOISONADDR1_REG) = (unsigned int) ECCPOISONADDR1_DATA;
   *(volatile unsigned int *)(CRCPARCTL_REG) = (unsigned int) CRCPARCTL_DATA;
   *(volatile unsigned int *)(INIT0_REG) = (unsigned int) INIT0_DATA;
   *(volatile unsigned int *)(INIT1_REG) = (unsigned int) INIT1_DATA;
   *(volatile unsigned int *)(INIT2_REG) = (unsigned int) INIT2_DATA;
   *(volatile unsigned int *)(INIT3_REG) = (unsigned int) INIT3_DATA;
   *(volatile unsigned int *)(INIT4_REG) = (unsigned int) INIT4_DATA;
   *(volatile unsigned int *)(INIT5_REG) = (unsigned int) INIT5_DATA;
   *(volatile unsigned int *)(DIMMCTL_REG) = (unsigned int) DIMMCTL_DATA;
   *(volatile unsigned int *)(RANKCTL_REG) = (unsigned int) RANKCTL_DATA;
   *(volatile unsigned int *)(DRAMTMG0_REG) = (unsigned int) DRAMTMG0_DATA;
   *(volatile unsigned int *)(DRAMTMG1_REG) = (unsigned int) DRAMTMG1_DATA;
   *(volatile unsigned int *)(DRAMTMG2_REG) = (unsigned int) DRAMTMG2_DATA;
   *(volatile unsigned int *)(DRAMTMG3_REG) = (unsigned int) DRAMTMG3_DATA;
   *(volatile unsigned int *)(DRAMTMG4_REG) = (unsigned int) DRAMTMG4_DATA;
   *(volatile unsigned int *)(DRAMTMG5_REG) = (unsigned int) DRAMTMG5_DATA;
   *(volatile unsigned int *)(DRAMTMG8_REG) = (unsigned int) DRAMTMG8_DATA;
   *(volatile unsigned int *)(ZQCTL0_REG) = (unsigned int) ZQCTL0_DATA;
   *(volatile unsigned int *)(ZQCTL1_REG) = (unsigned int) ZQCTL1_DATA;
   *(volatile unsigned int *)(ZQCTL2_REG) = (unsigned int) ZQCTL2_DATA;
   *(volatile unsigned int *)(DFITMG0_REG) = (unsigned int) DFITMG0_DATA;
   *(volatile unsigned int *)(DFITMG1_REG) = (unsigned int) DFITMG1_DATA;
   *(volatile unsigned int *)(DFILPCFG0_REG) = (unsigned int) DFILPCFG0_DATA;
   *(volatile unsigned int *)(DFIUPD0_REG) = (unsigned int) DFIUPD0_DATA;
   *(volatile unsigned int *)(DFIUPD1_REG) = (unsigned int) DFIUPD1_DATA;
   *(volatile unsigned int *)(DFIUPD2_REG) = (unsigned int) DFIUPD2_DATA;
   *(volatile unsigned int *)(DFIUPD3_REG) = (unsigned int) DFIUPD3_DATA;
   *(volatile unsigned int *)(DFIMISC_REG) = (unsigned int) DFIMISC_DATA;
   *(volatile unsigned int *)(ADDRMAP0_REG) = (unsigned int) ADDRMAP0_DATA;
   *(volatile unsigned int *)(ADDRMAP1_REG) = (unsigned int) ADDRMAP1_DATA;
   *(volatile unsigned int *)(ADDRMAP2_REG) = (unsigned int) ADDRMAP2_DATA;
   *(volatile unsigned int *)(ADDRMAP3_REG) = (unsigned int) ADDRMAP3_DATA;
   *(volatile unsigned int *)(ADDRMAP4_REG) = (unsigned int) ADDRMAP4_DATA;
   *(volatile unsigned int *)(ADDRMAP5_REG) = (unsigned int) ADDRMAP5_DATA;
   *(volatile unsigned int *)(ADDRMAP6_REG) = (unsigned int) ADDRMAP6_DATA;
   *(volatile unsigned int *)(ODTCFG_REG) = (unsigned int) ODTCFG_DATA;
   *(volatile unsigned int *)(ODTMAP_REG) = (unsigned int) ODTMAP_DATA;
   *(volatile unsigned int *)(SCHED_REG) = (unsigned int) SCHED_DATA;
   *(volatile unsigned int *)(SCHED1_REG) = (unsigned int) SCHED1_DATA;
   *(volatile unsigned int *)(PERFHPR1_REG) = (unsigned int) PERFHPR1_DATA;
   *(volatile unsigned int *)(PERFLPR1_REG) = (unsigned int) PERFLPR1_DATA;
   *(volatile unsigned int *)(PERFWR1_REG) = (unsigned int) PERFWR1_DATA;
   *(volatile unsigned int *)(PERFVPR1_REG) = (unsigned int) PERFVPR1_DATA;
   *(volatile unsigned int *)(PERFVPW1_REG) = (unsigned int) PERFVPW1_DATA;
   *(volatile unsigned int *)(DBG0_REG) = (unsigned int) DBG0_DATA;
   *(volatile unsigned int *)(DBG1_REG) = (unsigned int) DBG1_DATA;
   *(volatile unsigned int *)(DBGCMD_REG) = (unsigned int) DBGCMD_DATA;
   *(volatile unsigned int *)(PCCFG_REG) = (unsigned int) PCCFG_DATA;
   *(volatile unsigned int *)(PCFGR_0_REG) = (unsigned int) PCFGR_0_DATA;
   *(volatile unsigned int *)(PCFGR_1_REG) = (unsigned int) PCFGR_1_DATA;
   *(volatile unsigned int *)(PCFGR_2_REG) = (unsigned int) PCFGR_2_DATA;
   *(volatile unsigned int *)(PCFGW_0_REG) = (unsigned int)PCFGW_0_DATA;
   *(volatile unsigned int *)(PCFGW_1_REG) = (unsigned int)PCFGW_1_DATA;
   *(volatile unsigned int *)(PCFGW_2_REG) = (unsigned int)PCFGW_2_DATA;
   *(volatile unsigned int *)(PCTRL_0_REG) = (unsigned int)PCTRL_0_DATA;
   *(volatile unsigned int *)(PCTRL_1_REG) = (unsigned int)PCTRL_1_DATA;
   *(volatile unsigned int *)(PCTRL_2_REG) = (unsigned int)PCTRL_2_DATA;
   *(volatile unsigned int *)(PCFGQOS0_0_REG) = (unsigned int)PCFGQOS0_0_DATA;
   *(volatile unsigned int *)(PCFGQOS0_1_REG) = (unsigned int)PCFGQOS0_1_DATA;
   *(volatile unsigned int *)(PCFGQOS0_2_REG) = (unsigned int)PCFGQOS0_2_DATA;
   *(volatile unsigned int *)(PCFGQOS1_0_REG) = (unsigned int)PCFGQOS1_0_DATA;
   *(volatile unsigned int *)(PCFGQOS1_1_REG) = (unsigned int)PCFGQOS1_1_DATA;
   *(volatile unsigned int *)(PCFGQOS1_2_REG) = (unsigned int)PCFGQOS1_2_DATA;
   *(volatile unsigned int *)(PCFGWQOS0_0_REG) = (unsigned int)PCFGWQOS0_0_DATA;    /* Added date:12/06 */
   *(volatile unsigned int *)(PCFGWQOS0_1_REG) = (unsigned int)PCFGWQOS0_1_DATA;
// *(volatile unsigned int *)(PCFGWQOS1_0_REG) = (unsigned int)PCFGWQOS1_0_DATA;   /* removed on date : 12/06*/
// *(volatile unsigned int *)(PCFGWQOS0_1_REG) = (unsigned int)PCFGWQOS0_1_DATA;
   *(volatile unsigned int *)(PCFGWQOS0_2_REG) = (unsigned int)PCFGWQOS0_2_DATA;
   *(volatile unsigned int *)(PCFGWQOS1_0_REG) = (unsigned int)PCFGWQOS1_0_DATA;
   *(volatile unsigned int *)(PCFGWQOS1_1_REG) = (unsigned int)PCFGWQOS1_1_DATA;
   *(volatile unsigned int *)(PCFGWQOS1_2_REG) = (unsigned int)PCFGWQOS1_2_DATA;
   *(volatile unsigned int *)(SBRCTL_REG) = (unsigned int)SBRCTL_DATA;
//   *(volatile unsigned int *)(SBRCTL_REG) = (unsigned int)SBRCTL_DATA;  /* removed on date : 12/06*/
   *(volatile unsigned int *)(DFIMISC_REG) = (unsigned int)DFIMISC_CLEAR_DATA;
#endif

//PHY Registers
    soft_reg_rst4 = 0x00000005;
    writel(soft_reg_rst4, SHIKHARA_SYS_CTRL_BASEADDR+0x54);
    udelay(50000);

    ddr_reg_04 = readl(0xD45A5000);
    printf("ddr_reg_04:%x\n",ddr_reg_04); 

    while((readl(0xD45A500C) & 0x1) != 0x1){
      printf("PGCR_REG is set\n");
      udelay(50000);
    }
   *(volatile unsigned int *)(PGCR_REG) = (unsigned int)PGCR_DATA;
   *(volatile unsigned int *)(DX0GCR_REG) = (unsigned int)DX0GCR_DATA;
   *(volatile unsigned int *)(DX1GCR_REG) = (unsigned int)DX1GCR_DATA;
   *(volatile unsigned int *)(DX2GCR_REG) = (unsigned int)DX2GCR_DATA;
   *(volatile unsigned int *)(DX3GCR_REG) = (unsigned int)DX3GCR_DATA;
   *(volatile unsigned int *)(DX4GCR_REG) = (unsigned int)DX4GCR_DATA;
   *(volatile unsigned int *)(DXCCR_REG) = (unsigned int)DXCCR_DATA;
   *(volatile unsigned int *)(DSGCR_REG) = (unsigned int)DSGCR_DATA;
   *(volatile unsigned int *)(DCR_REG) = (unsigned int)DCR_DATA;
   *(volatile unsigned int *)(PTR0_REG) = (unsigned int)PTR0_DATA;
   *(volatile unsigned int *)(PTR1_REG) = (unsigned int)PTR1_DATA;
   *(volatile unsigned int *)(PTR2_REG) = (unsigned int)PTR2_DATA;
   *(volatile unsigned int *)(MR0_REG) = (unsigned int)MR0_DATA;
   *(volatile unsigned int *)(MR1_REG) = (unsigned int)MR1_DATA;
   *(volatile unsigned int *)(MR2_REG) = (unsigned int)MR2_DATA;
   *(volatile unsigned int *)(MR3_REG) = (unsigned int)MR3_DATA;
   *(volatile unsigned int *)(DTPR0_REG) = (unsigned int)DTPR0_DATA;
   *(volatile unsigned int *)(DTPR1_REG) = (unsigned int)DTPR1_DATA;
   *(volatile unsigned int *)(DTPR2_REG) = (unsigned int)DTPR2_DATA;
   *(volatile unsigned int *)(DSGCR1_REG) = (unsigned int)DSGCR1_DATA;
   *(volatile unsigned int *)(PIR_REG) = (unsigned int)PIR_DATA;
	/*added below line date: 06/12*/
   *(volatile unsigned int *)(DFIMISC_REG) = (unsigned int)DFIMISC_DATA;
   *(volatile unsigned int *)(DBG1_REG) = (unsigned int) DBG1_DATA;
   *(volatile unsigned int *)(PWRCTL_REG) = (unsigned int) 0x00000002;
   *(volatile unsigned int *)(PWRCTL_REG) = (unsigned int) PWRCTL_DATA;

    udelay(50000);

    *ddr_mem = (unsigned int)0x40414243;
    temp = *ddr_mem;
    printf(" Reading from ddr_mem0:",temp);
}




