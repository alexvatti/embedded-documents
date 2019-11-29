#include <config.h>
#include <asm/arch/shikhara_map.h>

.global smc_switch
.extern _vectors_start
.arch_extension sec
/* the vector table for secure state and HYP mode */
_monitor_vectors:
        .word 0 /* reset */
        .word 0 /* undef */
        ldr pc, _secure_monitor
        .word 0
        .word 0
        .word 0
        .word 0
        .word 0
_secure_monitor:        .word smc

/*
 * secure monitor handler
 * U-boot calls this "software interrupt" in start.S
 * This is executed on a "smc" instruction, we use a "smc #0" to switch
 * to non-secure state.
 * We use only r0 and r1 here, due to constraints in the caller.
 */
        .align  5
smc:
        mrc     p15, 0, r1, c1, c1, 0           /* read Cp15 SCR */
        bic     r1, r1, #0x4e                   @ clear IRQ, FIQ, EA, nET bits
        orr     r1, r1, #0x31                   @ enable NS, AW, FW bits
        mcr     p15, 0, r1, c1, c1, 0           /* write SCR (with NS bit set)*/
        movs	pc,lr

smc_switch:
        adr     r1, _monitor_vectors
        mcr     p15, 0, r1, c12, c0, 1          /* set MVBAR register */
        mrc     p15, 0, ip, c12, c0, 0          /* save secure copy of VBAR*/
        isb
        smc #0

	mrc     p15, 0, r0, c1, c0, 0   @ Read CP15 SCTRL Register
        bic     r0, #(1<<13)              @ V = 0
        mcr     p15, 0, r0, c1, c0, 0   @ Write CP15 SCTRL Register

        ldr    r0, =_vectors_start
        mcr    p15, 0, r0, c12, c0, 0  @Set VBAR register for nonsecure prveliged processor

	mov     r0, #0x86	/* In Non-secure state, trying to access to secure peripheral results data abort */
        ldr     r1, =(SHIKHARA_SYS_CTRL_BASEADDR+0x400)    
        str     r0, [r1]
        bx	lr

