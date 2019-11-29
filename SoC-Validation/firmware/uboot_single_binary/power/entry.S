        @ Created by arm_to_gnu.pl from entry.S
        .syntax unified

        @ Copyright (C) 2010 ARM Limited                           
        
        @ This software is provided 'as-is', without any express or implied
        @ warranties including the implied warranties of satisfactory quality, 
        @ fitness for purpose or non infringement.  In no event will  ARM be 
        @ liable for any damages arising from the use of this software.
        
        @ Permission is granted to anyone to use, copy and modify this software for 
        @ any purpose, and to redistribute the software, subject to the following 
        @ restrictions:
        
        @ 1. The origin of this software must not be misrepresented; you must not
        @    claim that you wrote the original software. If you use this software
        @    in a product, an acknowledgment in the product documentation would be
        @    appreciated but is not required.                                       
        @ 2. Altered source versions must be plainly marked as such, and must not be
        @    misrepresented as being the original software.
        @ 3. This notice may not be removed or altered from any source distribution.
        
        .section APPF_ENTRY_POINT_CODE,"ax"     
        
        .eabi_attribute Tag_ABI_align_preserved, 1  
        
    .equ APPF_FUNCTION_INITIALIZE, 0
        
        .global appf_entry_point    
        .global appf_reset_entry_point  
        .global appf_return_to_os   
        .global appf_runtime_call_flat_mapped   
        .global appf_ttbr0  
        .global appf_ttbcr  
        .global flat_mapped     
        
        
        
        
        
        
        
        
        
        
    .equ SCTLR_I, (1<<12)
    .equ SCTLR_Z, (1<<11)
        
        
appf_entry_point:               
        @ This is the entry point from the OS
        
        @ TODO: Decide how we're calling the firmware - BL or SMC
        @  - for now we don't have SMC
        
        @ Save some stuff on the OS stack (preserve 8 byte alignment)
        push	{r4-r12, lr}   
        
        @ Preserve arguments
        mov	r5, r0  
        mov	r6, r1  
        mov	r7, r2  
        mov	r8, r3  
        
        @ Is this runtime initialization?
        cmp	r0, #APPF_FUNCTION_INITIALIZE   
        bne	1f  
        
        @ Runtime intialization (build translation tables, etc)
        bl	appf_runtime_init    
        adr	r1, appf_return_to_os_address   
        adr	r2, appf_return_to_os   
        str	r2, [r1]    
        
        @ Get new stack pointer
1:                  bl	appf_platform_get_stack_pointer  
        mov	r11, r0			  @ save SP value for later
        
        @ Restore arguments
        mov	r0, r5  
        mov	r1, r6  
        mov	r2, r7  
        mov	r3, r8  
        
        @ This function uses the following registers:
        @ r12 - OS stack pointer
        @ r11 - APPF stack pointer
        @ r10 - APPF TTBR0
        @  r9 - APPF TTBCR
        @  r5 - APPF DACR
        @  r8 - OS TTBR0
        @  r7 - OS TTBCR
        @  r6 - OS DACR
        @  r4 - OS CPSR
        
        @ Disable interrupts
        mrs	r4, CPSR    
        cpsid	if    
        
        @ Check whether we are flat-mapped
        adr	r12, flat_mapped    
        ldr	r12, [r12]  
        cmp	r12, #0     
        bne	0f	     @ Skip the translation table switch code
        
        @ Get APPF values for TTBR0, TTBCR, DACR
        adr	r10, appf_ttbr0     
        adr	r9, appf_ttbcr  
        ldr	r10, [r10]  
        ldr	r9, [r9]    
        mov	r5, #0xffffffff     
        mov	r12, #0     
        
        @ Get current TTBR0, TTBCR, DACR
        mrc	p15, 0, r8, c2, c0, 0	  @ read TTBR0
        mrc	p15, 0, r7, c2, c0, 2	  @ read TTBCR
        mrc	p15, 0, r6, c3, c0, 0	  @ read DACR
        
        @ Switch translation tables to APPF
        mcr	p15, 0, r10, c2, c0, 0	     @ write TTBR0
        mcr	p15, 0,  r9, c2, c0, 2	     @ write TTBCR
        mcr	p15, 0,  r5, c3, c0, 0	     @ write DACR
        dsb     
        mcr	p15, 0, r12, c8, c7, 0	     @ Inv both TLBs
        mcr	p15, 0, r12, c7, c5, 6	     @ Inv branch predictor array
        dsb		   @ Wait for the preceding operations to finish
        isb		   @ Now re-fetch following instructions
        
        @ Switch stacks, save OS stack pointer and TTBR0, TTBCR
0:                  mov	r12, sp     
        mov	sp, r11     
        push	{r4-r8,r12}		  @ see comment in appf_reset_entry_point below
        
        adr	r7, appf_runtime_call_flat_mapped   
        ldr	r7, [r7]    
        
        blx	r7  
        
appf_return_to_os:              
        
        @ retrieve OS stack pointer, TTBR0, TTBCR, DACR
        pop	{r4-r8,r12}     
        
        @ Switch stacks
        mov	sp, r12     
        mov	r12, #0     
        
        @ Check whether we are flat-mapped
        adr	r12, flat_mapped    
        ldr	r12, [r12]  
        cmp	r12, #0     
        bne	0f	     @ Skip the translation table switch code
        
        @ Switch translation tables back to OS
        mcr	p15, 0, r8, c2, c0, 0	  @ write TTBR0
        mcr	p15, 0, r7, c2, c0, 2	  @ write TTBCR
        mcr	p15, 0, r6, c3, c0, 0	  @ write DACR
        dsb				     @ Wait for the preceding operations to finish
        mcr	p15, 0, r12, c8, c7, 0	     @ Inv both TLBs
        mcr	p15, 0, r12, c7, c5, 6	     @ Inv branch predictor array
        dsb				     @ Wait for the preceding operations to finish
        isb				     @ Now re-fetch following instructions
        
        @ Restore interrupts
0:                  msr	CPSR_c, r4  
        
        @ Return to OS
        pop	{r4-r12, pc}    
        
        
appf_reset_entry_point:             
        @ This is the entry point from the platform warm start code
        
        @ Get stack pointer
        bl	appf_platform_get_stack_pointer  
        sub	r0, r0, #24		   @ The 6 registers PUSHed above are still valid!
        mov	sp, r0  
        
        @ Clear I cache levels to PoU (also clears BTBs)
        bl	invalidate_icache_v7_pou     
        
        @ Turn I cache and branch prediction on
        bl	read_sctlr   
        orr	r0, r0, #(SCTLR_I  |  SCTLR_Z)  
        bl	write_sctlr  
        
        @ Clear all data cache levels visible to CPU
        bl	invalidate_dcache_v7     
        
        @ Caches have been invalidated, and any other init completed.
        @ Stack pointer is valid, and I cache on
        @ So we are ready to go to the C code
        
        bl	appf_warm_reset  
        adr	r1, appf_return_to_os_address   
        ldr	r1, [r1]    
        bx	r1   
        
        
flat_mapped:                
        .word 0     
        
appf_runtime_call_flat_mapped:              
        .word 0     
        
appf_return_to_os_address:              
        .word 0     
        
appf_ttbr0:             
        .word 0     
        
appf_ttbcr:             
        .word 0     
        
        .end    
