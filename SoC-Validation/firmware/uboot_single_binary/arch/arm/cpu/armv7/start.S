/*
 * Startup Code for SHIKHARA/ARM Cortex-A9 CPU-core
 *
 * Authors:	shabbir.kolimi@moschip.com
 * 		balaji.vishwanadhula@moschip.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <asm/system.h>
#include <asm/arch/shikhara_map.h>
#include <synopsys/lpddr2umctl.h>

.globl _start
.globl _vectors_start
.globl ddr_init
.globl pl341_ddr_init
.globl copy_vectors

_start:

#ifdef CONFIG_SHIKHARA_ASIC
#ifdef CONFIG_BOOT_XIP
        ldr pc,=(CONFIG_NORFLASH0+0x4)
#endif

#ifdef CONFIG_BL0_BUILD
  	ldr pc, =CONFIG_ROM_ADDR
#endif
#endif

_vectors_start:

	b	reset

#ifndef CONFIG_BL1_BUILD
#ifdef CONFIG_USE_IRQ
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq
_pad:			.word 0x12345678 /* now 16*4=64 */

.global _end_vect
_end_vect:

	.balignl 16,0xdeadbeef
#endif
#endif
/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************/
.globl _TEXT_BASE
_TEXT_BASE:
        .word   CONFIG_SYS_TEXT_BASE
	
/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start_ofs
_bss_start_ofs:
	.word __bss_start - _start

.global	_image_copy_end_ofs
_image_copy_end_ofs:
	.word 	__image_copy_end - _start

.globl _bss_end_ofs
_bss_end_ofs:
	.word __bss_end__ - _start

.globl _end_ofs
_end_ofs:
	.word _end - _start

/* IRQ stack memory (calculated at run-time) */
#ifdef CONFIG_USE_IRQ
.globl IRQ_STACK_START
IRQ_STACK_START:
        .word   0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
        .word 0x0badc0de
#endif

.globl IRQ_STACK_START_IN
IRQ_STACK_START_IN:
	.word	0x0badc0de

/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f              
	orr	r0, r0, #0xd3          @ Setting mode bits to SVC and disabling I and F bits by setting 1's
	msr	cpsr,r0
      #ifdef  CONFIG_BL1_BUILD 
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x3F
        str     r1,[r3]
      #endif

	bl	cpu_init_cp15

/* Set stackpointer in DDR2 RAM to call board_init_f */
after_copy:
	
	#ifdef CONFIG_SHIKHARA_ZYNQBASED_FPGA

        @ unlock SLCR
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 8)
        ldr r2, =0xDF0D
        str r2, [r1]

        @ remap DDR to zero
        @ FILTERSTART
        ldr r1, =(XPSS_SCU_BASEADDR + 0x40)
        ldr r2, =0
        str r2, [r1]

        @ Device config APB
        @ unlock the PCAP
        ldr r1, =(XPSS_DEV_CFG_APB_BASEADDR + 0x34)
        ldr r2, =0x757BDF0D
        str r2, [r1]
        ldr r1, =(XPSS_DEV_CFG_APB_BASEADDR + 0x28)
        ldr r2, =0xFFFFFFFF
        str r2, [r1]

        @ OCM_CFG
        @ Mask out the ROM
        @ map ram into upper addresses
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR +0x910)
        ldr r2, =0x1F
        str r2, [r1]

        @ FPGA_RST_CTRL
        @ clear resets on AXI fabric ports
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 0x240)
        ldr r2, =0x0
        str r2, [r1]

	@ TZ_DDR_RAM
        @ Set DDR trust zone non-secure
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 0x430)
        ldr r2, =0xFFFFFFFF
        str r2, [r1]

/* Changes made by Moschip */
#ifdef CONFIG_ZYNQ_TRUSTZONE
        #set APB access to Non-secure
        ldr r1, =(XPSS_TZ_CTRL_BASEADDR + 0x18)
        ldr r2, =0x7FFF
        str r2, [r1]

        #set SCU NS access control register
        ldr r1,=(XPSS_SYS_CTRL_BASEADDR + 0x54)
        ldr r2,=0x0fff
        str r2, [r1]
#endif

        @ set urgent bits with register
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 0x61C)
        ldr r2, =0
        str r2, [r1]

        @urgent write, ports S2/S3
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 0x600)
        ldr r2, =0xC
        str r2, [r1]

        @ relock SLCR
        ldr r1, =(XPSS_SYS_CTRL_BASEADDR + 0x4)
        ldr r2, =0x767B
        str r2, [r1]
        #endif

	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR)
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
	ldr	r0,=0x00000000
	#ifndef CONFIG_BL1_BUILD
	bl	board_init_f
        #endif

/*------------------------------------------------------------------------------*/

/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */

#ifndef CONFIG_BL1_BUILD
	.globl	relocate_code
relocate_code:
	mov	r4, r0	/* save addr_sp */
	mov	r5, r1	/* save addr of gd */
	mov	r6, r2	/* save addr of destination */

	/* Set up the stack						    */
stack_setup:
	mov	sp, r4

	adr	r0, _start
	cmp	r0, r6
	moveq	r9, #0		/* no relocation. relocation offset(r9) = 0 */
	beq	clear_bss		/* skip relocation */
	mov	r1, r6			/* r1 <- scratch for copy_loop */
	ldr	r3, _image_copy_end_ofs
	add	r2, r0, r3		/* r2 <- source end address	    */

copy_loop:
	ldmia	r0!, {r9-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r9-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end address [r2]    */
	blo	copy_loop

#ifndef CONFIG_SPL_BUILD
	/*
	 * fix .rel.dyn relocations
	 */
	ldr	r0, _TEXT_BASE		/* r0 <- Text base */
	sub	r9, r6, r0		/* r9 <- relocation offset */
	ldr	r10, _dynsym_start_ofs	/* r10 <- sym table ofs */
	add	r10, r10, r0		/* r10 <- sym table in FLASH */
	ldr	r2, _rel_dyn_start_ofs	/* r2 <- rel dyn start ofs */
	add	r2, r2, r0		/* r2 <- rel dyn start in FLASH */
	ldr	r3, _rel_dyn_end_ofs	/* r3 <- rel dyn end ofs */
	add	r3, r3, r0		/* r3 <- rel dyn end in FLASH */
fixloop:
	ldr	r0, [r2]		/* r0 <- location to fix up, IN FLASH! */
	add	r0, r0, r9		/* r0 <- location to fix up in RAM */
	ldr	r1, [r2, #4]
	and	r7, r1, #0xff
	cmp	r7, #23			/* relative fixup? */
	beq	fixrel
	cmp	r7, #2			/* absolute fixup? */
	beq	fixabs
	/* ignore unknown type of fixup */
	b	fixnext
fixabs:
	/* absolute fix: set location to (offset) symbol value */
	mov	r1, r1, LSR #4		/* r1 <- symbol index in .dynsym */
	add	r1, r10, r1		/* r1 <- address of symbol in table */
	ldr	r1, [r1, #4]		/* r1 <- symbol value */
	add	r1, r1, r9		/* r1 <- relocated sym addr */
	b	fixnext
fixrel:
	/* relative fix: increase location by offset */
	ldr	r1, [r0]
	add	r1, r1, r9
fixnext:
	str	r1, [r0]
	add	r2, r2, #8		/* each rel.dyn entry is 8 bytes */
	cmp	r2, r3
	blo	fixloop
	b	clear_bss
_rel_dyn_start_ofs:
	.word __rel_dyn_start - _start
_rel_dyn_end_ofs:
	.word __rel_dyn_end - _start
_dynsym_start_ofs:
	.word __dynsym_start - _start

clear_bss:
	ldr	r0, _bss_start_ofs
	ldr	r1, _bss_end_ofs
	mov	r4, r6			/* reloc addr */
	add	r0, r0, r4
	add	r1, r1, r4
	mov	r2, #0x00000000		/* clear			    */

clbss_l:str	r2, [r0]		/* clear loop...		    */
	add	r0, r0, #4
	cmp	r0, r1
	bne	clbss_l

#ifdef CONFIG_ENABLE_MMU               /* Configure page table, enable MMU and D-cache */
        bl      dcache_enable
#endif

#ifdef CONFIG_L2_CACHE
	bl      l2_cache_init
#endif

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */
jump_2_ram:
/*
 * If I-cache is enabled invalidate it
 */
#ifndef CONFIG_SYS_ICACHE_OFF
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB
#endif
	ldr	r0, _board_init_r_ofs
	adr	r1, _start
	add	lr, r0, r1
	add	lr, lr, r9
	/* setup parameters for board_init_r */
	mov	r0, r5		/* gd_t */
	mov	r1, r6		/* dest_addr */
	/* jump to it ... */
	mov	pc, lr

_board_init_r_ofs:
	.word board_init_r - _start

#endif	/* #ifndef CONFIG_BL1_BUILD */
#endif
/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
.globl cpu_init_cp15
cpu_init_cp15:
	/*
	 * Invalidate L1 I/D
	 */

        #ifdef  CONFIG_BL1_BUILD
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x06
        str     r1,[r3]
       #endif                      

	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
#ifdef CONFIG_SYS_ICACHE_OFF
	bic	r0, r0, #0x00001000	@ clear bit 12 (I) I-cache
#else
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
#endif
	mcr	p15, 0, r0, c1, c0, 0

@==================================================================
@ SMP initialization
@==================================================================
        mrc     p15, 0, r0, c0, c0, 5      @ Read CPU ID register
        ands    r0, r0, #0x03              @ Mask off, leaving the CPU ID field
        beq     primary_cpu_init
        bne     secondary_cpus_init

secondary_cpus_init:

                    #ifdef  CONFIG_BL1_BUILD
                    ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
                    mov     r1,#0x4F
                    str     r1,[r3]
                   #endif

			wfi

primary_cpu_init:

        #ifdef  CONFIG_BL1_BUILD
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x5B
        str     r1,[r3]
        #endif


	mov       ip,lr
	bl        low_level_init
	mov       lr,ip
	mov	  pc, lr			@ back to my caller


#ifndef CONFIG_BL1_BUILD
#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_USE_IRQ
/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE		@ carve out a frame on current
						@ user stack
	stmia	sp, {r0 - r12}			@ Save user registers (now in
						@ svc mode) r0-r12
	ldr	r2, IRQ_STACK_START_IN		@ set base 2 words into abort
						@ stack
	ldmia	r2, {r2 - r3}			@ get values for "aborted" pc
						@ and cpsr (into parm regs)
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp				@ save current stack into r0
						@ (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC			@ !! R8 NEEDS to be saved !!
						@ a reserved stack spot would
						@ be good.
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into
						@ cpsr
	.endm


	.macro get_bad_stack
	ldr	r13, IRQ_STACK_START_IN		@ setup our mode stack (enter
						@ in banked mode)

	str	lr, [r13]			@ save caller lr in position 0
						@ of saved stack
	mrs	lr, spsr			@ get the spsr
	str	lr, [r13, #4]			@ save spsr in position 1 of
						@ saved stack

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	msr	spsr, r13			@ switch modes, make sure
						@ moves will execute
	mov	lr, pc				@ capture return pc
	movs	pc, lr				@ jump to next instruction &
						@ switch modes.
	.endm

	.macro get_bad_stack_swi
	sub	r13, r13, #4			@ space on current stack for
						@ scratch reg.
	str	r0, [r13]			@ save R0's value.
	ldr	r0, IRQ_STACK_START_IN		@ get data regions start
						@ spots for abort stack
	str	lr, [r0]			@ save caller lr in position 0
						@ of saved stack
	mrs	r0, spsr			@ get the spsr
	str	lr, [r0, #4]			@ save spsr in position 1 of
						@ saved stack
	ldr	r0, [r13]			@ restore r0
	add	r13, r13, #4			@ pop stack entry
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack_swi
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs	
	bl	do_irq
	irq_restore_user_regs	

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq
#endif /* CONFIG_USE_IRQ */
#endif /* CONFIG_SPL_BUILD */
#endif /* CONFIG_BL1_BUILD */



.globl low_level_init

low_level_init:

       #ifdef  CONFIG_BL1_BUILD
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x66
        str     r1,[r3]
      #endif


        adr r0,_start       @Current base addr of code
        ldr r1,_TEXT_BASE   @Original base addr in ram
        cmp r0,r1           @r0 == r1 then skip dram init and u-boot.bin loading
           
	beq after_copy

	#ifndef CONFIG_BL0_BUILD
         
        #ifdef CONFIG_SHIKHARA_ASIC  
        #ifdef CONFIG_LPDDR2_INIT
                 bl  ddr_init  	      	@ initialising the lpddr2 by bl1
        #endif
        #endif


        #endif

#ifdef CONFIG_BL1_BUILD
copy_data:

        #ifdef  CONFIG_BL1_BUILD
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x6D
        str     r1,[r3]
        #endif

        ldr  r1,=data_start
        ldr  r0,=text_end
        ldr  r2,=data_end

copy_loop_1:
         ldmia   r0!, {r9-r10}           /* copy from source address [r0]    */
         stmia   r1!, {r9-r10}           /* copy to   target address [r1]    */
         cmp     r1, r2                  /* until source end address [r2]    */
         blo     copy_loop_1
#endif


clear_bsse:

        #ifdef  CONFIG_BL1_BUILD
        ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
        mov     r1,#0x7D
        str     r1,[r3]
        #endif

        ldr     r0, =__bss_start
        ldr     r1, =__bss_end__
        mov     r2, #0x00000000         /* clear                            */

clbsse_l:
         str     r2, [r0]                /* clear loop...                    */
         add     r0, r0, #4
         cmp     r0, r1
         bne     clbsse_l

	
         #ifndef CONFIG_BL0_BUILD		
         
          #ifdef  CONFIG_BL1_BUILD
          ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
          mov     r1,#0x7F
          str     r1,[r3]
          #endif

          ldr sp,=CONFIG_BL1_SP_ADDR		@SP is set to DDR
	 #else

          #ifdef  CONFIG_BL1_BUILD
          ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
          mov     r1,#0x07
          str     r1,[r3]
          #endif

	  ldr sp,=CONFIG_BL0_SP_ADDR             @SP is set to SRAM
         #endif
	
	 
        #ifdef CONFIG_SHIKHARA_ASIC  
	ldr    r0, =REMAP_REG               @ REMAP Address
        ldr    r1, [r0]
        bic    r1, r1, #0x1
        str    r1, [r0]

check_remap_disable:    @ Checking for RAMAP bit is disbled or not
     ldr    r2, [r0]
     ands   r2, r2, #0x1
     bne    check_remap_disable
       #endif
	  #ifndef CONFIG_BOOT_XIP
	  #ifndef CONFIG_BL0_BUILD
               bl load_uboot       		@load  u-boot based on the boot strap
	  #endif
	  #ifndef CONFIG_BL0_BUILD
	

               #ifdef  CONFIG_BL1_BUILD
              ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
              mov     r1,#0x77
              str     r1,[r3]
              #endif

               ldr pc,=CONFIG_SYS_TEXT_BASE      @ Jump to u-boot
	  #else

               #ifdef  CONFIG_BL1_BUILD
               ldr     r3,=(SHIKHARA_SYS_CTRL_BASEADDR+0x400)
               mov     r1,#0x6F
               str     r1,[r3]
               #endif

   	       #ifdef CONFIG_BL1_IMAGE	
               ldr pc,=(CONFIG_V7ONCHIP_SRAM_BASE+0x40) @0x40 is the image_hdr size of bl1
  	       #endif
               ldr pc,=CONFIG_V7ONCHIP_SRAM_BASE
	  #endif
	  #endif
	   b after_copy

#ifdef CONFIG_L2_CACHE

.global l2_cache_init

l2_cache_init:
             
        ldr r0,L2CC_PL310

        @ Set aux cntrl
        @ Way size = 64KB

        ldr     r1, =0x00060000
        str     r1, [r0,#0x104]

        @ Set tag RAM latency
        @ 8 cycles RAM write access latency
        @ 8 cycles RAM read access latency
        @ 8 cycles RAM setup latency

        ldr     r1, =0x00000777
        str     r1, [r0,#0x108]

        @ Set Data RAM latency
        @ 8 cycles RAM write access latency
        @ 8 cycles RAM read access latency
        @ 8 cycles RAM setup latency

        ldr     r1, =0x00000777
        str     r1, [r0,#0x10C]

        @Cache maintenance - invalidate by way (0xff) - base offset 0x77C
        ldr     r1, =0xFF
        str     r1, [r0,#0x77C]

poll_invalidate:
        ldr     r1, [r0,#0x77C]
        tst     r1, #1
        bne     poll_invalidate

        @ Enable Event Counter Control Register. Reset counter 0 and 1 values

        ldr     r1, =0x007
        str     r1, [r0,#0x200]

 @ Counter 1. Count Drhit event

        LDR     r1, =0x008
        STR     r1, [r0,#0x204]

        @ Counter 0. Count Dwhit event
        LDR     r1, =0x010
        STR     r1, [r0,#0x208]

        @ Ensure L2 remains disabled for the time being
        LDR     r1, =0x0
        STR     r1, [r0,#0x100]

        mov     pc, lr

L2CC_PL310:
             .word   0xD46F4000
#endif

copy_vectors:

	mrc     p15, 0, r0, c1, c0, 0   @ Read CP15 SCTRL Register
        bic     r0, #(1<<13)              @ V = 0
        mcr     p15, 0, r0, c1, c0, 0   @ Write CP15 SCTRL Register

        ldr    r0, =_vectors_start
        mcr    p15, 0, r0, c12, c0, 0  @Set VBAR

        mov    pc,lr

.end


