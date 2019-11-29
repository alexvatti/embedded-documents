#include <synopsys/lpddr2umctl.h>

   .text
   .global ddr_init

/* LPDDR2 memory controller initialization for SHIKHARA soc for synopsys ip */

ddr_init:

	ldr r0,=REMAP_REG   /* Disabling the On-chip ROM on first memory region */
        ldr r1,[r0]
        mov r2,#1
        bic r1,r2,LSL #25
        str r1,[r0]

/*Configure INIT0 register 
        25-16 : post_cke= n * 1024 tCK = 400ns 
	 9-0  : pre_cke= m * 1024 tCK = 200us 
*/
	ldr r0,=DDR2_INIT0_REG
        ldr r1,=DDR2_INIT0_DATA
        str r1,[r0]

        ldr r0,=DDR2_INIT1_REG
        ldr r1,=DDR2_INIT1_DATA
        str r1,[r0]
        
/* Configure INIT2 reg
	15-8  : idle_after_reset = tinit4 = 1* 32 tCK = 1us 
	3-0   : min_stable_clock= tinit2=5 tck
*/
        ldr r0,=DDR2_INIT2_REG
        ldr r1,=DDR2_INIT2_DATA
        str r1,[r0]
        
/* Configure INIT3 register 
	MR and EMRn registers are specific to DDR2 Device, refer Datasheet to configure them.
	31-16 : MR register value   = 	0x0A62		
	15-0  : EMR register value  =   0x2BA0
*/
        ldr r0,=DDR2_INIT3_REG
        ldr r1,=DDR2_INIT3_DATA
        str r1,[r0]

/*Configure INIT4 register 
	31-16 : EMR2 register value = 0x4000
	15-0  : EMR3 register value = 0x3000
*/
        ldr r0,=DDR2_INIT4_REG
        ldr r1,=DDR2_INIT4_DATA
        str r1,[r0]

/*Configure DRAMTMGn registers
These Timing registers values are configured with respect to device timings to synchronise the Controller. */

        ldr r0,=DDR2_DRAMTMG0_REG
        ldr r1,=DDR2_DRAMTMG0_DATA
        str r1,[r0]

        ldr r0,=DDR2_DRAMTMG1_REG
        ldr r1,=DDR2_DRAMTMG1_DATA
        str r1,[r0]

        ldr r0,=DDR2_DRAMTMG2_REG
        ldr r1,=DDR2_DRAMTMG2_DATA
        str r1,[r0]

        ldr r0,=DDR2_DRAMTMG3_REG
        ldr r1,=DDR2_DRAMTMG3_DATA
        str r1,[r0]

        ldr r0,=DDR2_DRAMTMG4_REG
        ldr r1,=DDR2_DRAMTMG4_DATA
        str r1,[r0]
        ldr r0,=DDR2_DRAMTMG5_REG
        ldr r1,=DDR2_DRAMTMG5_DATA
        str r1,[r0]

/* Configure the RFSHTMG Register with device values 
	27-16 : t_rfc_nom_x32 = tREFI = 7.8 us
	8-0   : t_rfc_min = 127 < x < 70000ns
*/
        ldr r0,=DDR2_RFSHTMG_REG
        ldr r1,=DDR2_RFSHTMG_DATA
        str r1,[r0]

/* Configure MSTR register 
	24-27 : No.of Ranks
	16-19 : Burst length for read and write
	10    : 2T timing mode select
	8     : seq or interleaved Burst mode
*/
        ldr r0,=DDR2_MSTR_REG
        ldr r1,=DDR2_MSTR_DATA
        str r1,[r0]

/* Configure the Status register with operating mode */

        ldr r0,=DDR2_STAT_REG
        ldr r1,=DDR2_STAT_DATA
        str r1,[r0]

/* Configure the mode register read/write control registers */

        ldr r0,=DDR2_MRCTRL0_REG
        ldr r1,=DDR2_MRCTRL0_DATA
        str r1,[r0]

        ldr r0,=DDR2_MRCTRL1_REG
        ldr r1,=DDR2_MRCTRL1_DATA
        str r1,[r0]

        mov pc,r10                   @ Go back to Start.S  

    .end

