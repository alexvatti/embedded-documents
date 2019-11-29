#ifndef __SHIKHARA_DDR3_H_
#define __SHIKHARA_DDR3_H_	1

#define REMAP_REG       0xD4561098
#define MSTR_REG	0xD45A4000
#define STAT_REG        0xD45A4004
#define MRCTRL0_REG	0xD45A4010
#define MRCTRL1_REG	0xD45A4014
#define DERATEEN_REG	0xD45A4020
#define DERATEINT_REG	0xD45A4024
#define PWRCTL_REG	0xD45A4030
#define PWRTMG_REG	0xD45A4034
#define HWLPCTL_REG	0xD45A4038
#define RFSHCTL0_REG	0xD45A4050
#define RFSHCTL1_REG	0xD45A4054
#define RFSHCTL3_REG	0xD45A4060
#define RFSHTMG_REG	0xD45A4064
#define ECCCFG0_REG	0xD45A4070
#define ECCCFG1_REG	0xD45A4074
#define ECCCLR_REG	0xD45A407c
#define ECCPOISONADDR0_REG	0xD45A40b8
#define ECCPOISONADDR1_REG	0xD45A40bc
#define CRCPARCTL_REG	0xD45A40c0
#define INIT0_REG	0xD45A40d0
#define INIT1_REG	0xD45A40d4
#define INIT2_REG	0xD45A40d8
#define INIT3_REG	0xD45A40dc
#define INIT4_REG	0xD45A40e0
#define INIT5_REG	0xD45A40e4
#define DIMMCTL_REG	0xD45A40f0
#define RANKCTL_REG	0xD45A40f4
#define DRAMTMG0_REG	0xD45A4100
#define DRAMTMG1_REG	0xD45A4104
#define DRAMTMG2_REG	0xD45A4108
#define DRAMTMG3_REG	0xD45A410c
#define DRAMTMG4_REG	0xD45A4110
#define DRAMTMG5_REG	0xD45A4114
#define DRAMTMG6_REG	0xD45A4118
#define DRAMTMG7_REG	0xD45A411c
#define DRAMTMG8_REG	0xD45A4120
#define ZQCTL0_REG	0xD45A4180
#define ZQCTL1_REG	0xD45A4184
#define ZQCTL2_REG	0xD45A4188
#define DFITMG0_REG	0xD45A4190
#define DFITMG1_REG	0xD45A4194
#define DFILPCFG0_REG	0xD45A4198
#define DFIUPD0_REG	0xD45A41a0
#define DFIUPD1_REG	0xD45A41a4
#define DFIUPD2_REG	0xD45A41a8
#define DFIUPD3_REG	0xD45A41ac
#define DFIMISC_REG	0xD45A41b0
#define ADDRMAP0_REG	0xD45A4200
#define ADDRMAP1_REG	0xD45A4204
#define ADDRMAP2_REG	0xD45A4208
#define ADDRMAP3_REG	0xD45A420c
#define ADDRMAP4_REG	0xD45A4210
#define ADDRMAP5_REG	0xD45A4214
#define ADDRMAP6_REG	0xD45A4218
#define ODTCFG_REG	0xD45A4240
#define ODTMAP_REG	0xD45A4244
#define SCHED_REG	0xD45A4250
#define SCHED1_REG	0xD45A4254
#define PERFHPR1_REG	0xD45A425c
#define PERFLPR1_REG	0xD45A4264
#define PERFWR1_REG	0xD45A426c
#define PERFVPR1_REG	0xD45A4274
#define PERFVPW1_REG	0xD45A4278
#define DBG0_REG	0xD45A4300
#define DBG1_REG	0xD45A4304
#define DBGCMD_REG	0xD45A430c
#define PCCFG_REG	0xD45A4400
#define PCFGR_0_REG	0xD45A4404 
#define PCFGR_1_REG	0xD45A44b4 
#define PCFGR_2_REG	0xD45A4564 
#define PCFGW_0_REG	0xD45A4408 
#define PCFGW_1_REG	0xD45A44b8 
#define PCFGW_2_REG	0xD45A4568 
#define PCTRL_0_REG	0xD45A4490
#define PCTRL_1_REG	0xD45A4540
#define PCTRL_2_REG	0xD45A45f0
#define	PCFGQOS0_0_REG	0xD45A4494
#define	PCFGQOS0_1_REG	0xD45A4544
#define	PCFGQOS0_2_REG	0xD45A45f4
#define PCFGQOS1_0_REG	0xD45A4498
#define PCFGQOS1_1_REG	0xD45A4548
#define PCFGQOS1_2_REG	0xD45A45f8
#define PCFGWQOS0_0_REG	0xD45A449C
#define PCFGWQOS0_1_REG	0xD45A454C
#define PCFGWQOS0_2_REG	0xD45A45FC
#define PCFGWQOS1_0_REG	0xD45A44A0
#define PCFGWQOS1_1_REG	0xD45A4550
#define PCFGWQOS1_2_REG	0xD45A4600
#define SBRCTL_REG	0xD45A4f24

/*PHY Reg Offset */
#if 0
//#define PGCR_REG	0xD45A5002
#define PGCR_REG	0xD45A5008
#define DX0GCR_REG	0xD45A5070
#define DX1GCR_REG	0xD45A5080
#define DX2GCR_REG	0xD45A5090
#define DX3GCR_REG	0xD45A50a0
#define DX4GCR_REG	0xD45A50b0
#define DXCCR_REG	0xD45A500a
#define DSGCR_REG	0xD45A500b
#define DSGCR1_REG	0xD45A500b
#define DCR_REG		0xD45A500c
#define PTR0_REG	0xD45A5006
#define PTR1_REG	0xD45A5007
#define PTR2_REG	0xD45A5008
#define MR0_REG		0xD45A5010
#define MR1_REG		0xD45A5011
#define MR2_REG		0xD45A5012
#define MR3_REG		0xD45A5013
#define DTPR0_REG	0xD45A500d
#define DTPR1_REG	0xD45A500e
#define DTPR2_REG	0xD45A500f
#define PIR_REG		0xD45A5001
#endif

#define PGCR_REG        0xD45A5008
#define DX0GCR_REG      0xD45A51C0
#define DX1GCR_REG      0xD45A5200
#define DX2GCR_REG      0xD45A5240
#define DX3GCR_REG      0xD45A5280
#define DX4GCR_REG      0xD45A52C0
#define DXCCR_REG       0xD45A5028
#define DSGCR_REG       0xD45A502C
#define DSGCR1_REG      0xD45A502C
#define DCR_REG         0xD45A5030
#define PTR0_REG        0xD45A5018
#define PTR1_REG        0xD45A501C
#define PTR2_REG        0xD45A5020
#define MR0_REG         0xD45A5040
#define MR1_REG         0xD45A5044
#define MR2_REG         0xD45A5048
#define MR3_REG         0xD45A504C
#define DTPR0_REG       0xD45A5034
#define DTPR1_REG       0xD45A5038
#define DTPR2_REG       0xD45A503C
#define PIR_REG         0xD45A5004


/* uMCTL2 DATA Registes*/
#define MSTR_DATA		0x03040001
#define MRCTRL0_DATA		0x00000030
#define MRCTRL1_DATA		0x00000000
//#define STAT_DATA		0x00000000
#define DERATEEN_DATA		0x00000000
#define DERATEINT_DATA		0x00000000
#define PWRCTL_DATA		0x00000000
#define PWRTMG_DATA		0x00080003
#define HWLPCTL_DATA		0x006c0002
#define RFSHCTL0_DATA		0x00205000
#define RFSHCTL1_DATA		0x00490025
#define RFSHCTL3_DATA		0x00000000
#define RFSHTMG_DATA		0x00410050  /* modified date 12/06 :initial value :0x00040050 modified value 0x00410050*/
#define ECCCFG0_DATA		0x00000010 /* ECC disable */
//#define ECCCFG0_DATA		0x00000004 /* ECC Enable */
#define ECCCFG1_DATA		0x00000000
#define ECCCLR_DATA		0x00000000
#define ECCPOISONADDR0_DATA	0x000000a8
#define ECCPOISONADDR1_DATA	0x01000d77
#define CRCPARCTL_DATA		0x00000000
#define INIT0_DATA		0x00010083
#define INIT1_DATA		0x00690000
#define INIT2_DATA		0x00001006
#define INIT3_DATA		0x09400000
#define INIT4_DATA		0x00080000
#define INIT5_DATA		0x00100000
#define DIMMCTL_DATA		0x00000000
#define RANKCTL_DATA		0x0000033f
#define DRAMTMG0_DATA		0x090e120a
#define DRAMTMG1_DATA		0x0007080d
#define DRAMTMG2_DATA		0x00000607
#define DRAMTMG3_DATA		0x00002006
#define DRAMTMG4_DATA		0x04020304
#define DRAMTMG5_DATA		0x03030202
//#define DRAMTMG6_DATA		0x01010003
//#define DRAMTMG7_DATA		0x00000101
#define DRAMTMG8_DATA		0x00000010
#define ZQCTL0_DATA		0x20800020
#define ZQCTL1_DATA		0x00c00070
#define ZQCTL2_DATA		0x00000000
#define DFITMG0_DATA		0x02030002
#define DFITMG1_DATA		0x00030102
#define DFILPCFG0_DATA		0x07502101
#define DFIUPD0_DATA		0x00400003
#define DFIUPD1_DATA		0x00c900d3
#define DFIUPD2_DATA		0x80000002
#define DFIUPD3_DATA		0x00000000
#define DFIMISC_DATA		0x00000001
#define ADDRMAP0_DATA		0x00000015
#define ADDRMAP1_DATA		0x00070707
#define ADDRMAP2_DATA		0x00000000
#define ADDRMAP3_DATA		0x0f000000
#define ADDRMAP4_DATA		0x00000f0f
#define ADDRMAP5_DATA		0x06060606
#define ADDRMAP6_DATA		0x0f060606
#define ODTCFG_DATA		0x04000400
#define ODTMAP_DATA		0x00001110
#define SCHED_DATA		0x00000401
#define SCHED1_DATA		0x00000000
#define PERFHPR1_DATA		0x0f000001
#define PERFLPR1_DATA		0x0f000005  /* modified date 12/06 :initial value :0x0f0000ff modified value 0x0f000005*/
#define PERFWR1_DATA		0x0f00000f
#define PERFVPR1_DATA		0x000007ff
#define PERFVPW1_DATA		0x000007ff
#define DBG0_DATA		0x00000010
#define DBG1_DATA		0x00000000
#define DBGCMD_DATA		0x00000000
#define PCCFG_DATA		0x00000001
#define PCFGR_0_DATA		0x00017303
#define PCFGR_1_DATA		0x00017303
#define PCFGR_2_DATA		0x00017303
#define PCFGW_0_DATA		0x0000701c
#define PCFGW_1_DATA		0x0000701c
#define PCFGW_2_DATA		0x0000701c
#define PCTRL_0_DATA		0x00000001
#define PCTRL_1_DATA		0x00000001
#define PCTRL_2_DATA		0x00000001
#define PCFGQOS0_0_DATA		0x02100d02
#define PCFGQOS0_1_DATA		0x02100d02
#define PCFGQOS0_2_DATA		0x02100d02
#define PCFGQOS1_0_DATA		0x00020005
#define PCFGQOS1_1_DATA		0x00020005
#define PCFGQOS1_2_DATA		0x00020005
#define PCFGWQOS0_0_DATA	0x00110007
#define PCFGWQOS0_1_DATA	0x00110007
#define PCFGWQOS0_2_DATA	0x00110007
#define PCFGWQOS1_0_DATA	0x00000005
#define PCFGWQOS1_1_DATA	0x00000005
#define PCFGWQOS1_2_DATA	0x00000005
#define SBRCTL_DATA		0x0000ff10
#define DFIMISC_CLEAR_DATA            0x00000000

/* PHY Registers*/
#define PGCR_DATA	0x0b8c2e02
#define DX0GCR_DATA	0x00033981
#define DX1GCR_DATA	0x00033981
#define DX2GCR_DATA	0x00033981
#define DX3GCR_DATA	0x00033981
#define DX4GCR_DATA	0x00033981
#define DXCCR_DATA	0x00000c40
#define DSGCR_DATA	0xfa00025f
#define DSGCR1_DATA	0xFA001A5F
#define DCR_DATA	0x0000040B
#define PTR0_DATA	0x10000010
#define PTR1_DATA	0x00800080
#define PTR2_DATA	0x00080421
#define MR0_DATA	0x000000ab
#define MR1_DATA	0x000000cb
#define MR2_DATA	0x00000005
#define MR3_DATA	0x00000002
#define DTPR0_DATA	0x3cb49998
#define DTPR1_DATA	0x192a90c0
#define DTPR2_DATA	0x10039004
#define PIR_DATA	0x00040001


#define SYSTEMCTL_REG 0xD4561000
#define ddr_config ((volatile unsigned int*)0xD45A4000)   // DDR controller base address 
#define ddr_phy_config ((volatile unsigned int*)0xD45A5000)   // DDR PHY base address 
#define ddr_mem ((volatile unsigned int*)0x00000000)//unsecure boot

#endif