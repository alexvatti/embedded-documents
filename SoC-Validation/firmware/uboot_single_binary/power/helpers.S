        @ Created by arm_to_gnu.pl from helpers.S
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
        
        .global dmb     
        .global wfi     
        .global wfe     
        .global sev     
        .global copy_words  
        .global appf_memcpy     
        .global appf_memset     
        
        .global dsb     
        .global read_mpidr      
        .global va_to_pa    
        .global read_sctlr  
        .global write_sctlr     
        .global read_actlr  
        .global write_actlr     
        .global read_prrr   
        .global read_nmrr   
        .global read_mvbar  
        
        
        .section APPF,"ax"  
        
        
        
dmb:                .func   
        dmb     
        bx	lr   
        .endfunc    
        
wfi:                .func   
        wfi     
        bx	lr   
        .endfunc    
        
wfe:                .func   
        wfe     
        bx	lr   
        .endfunc    
        
sev:                .func   
        sev     
        bx	lr   
        .endfunc    
        
        @ This function takes three arguments
        @ r0: Destination start address (must be word aligned)
        @ r1: Source start address (must be word aligned)
        @ r2: Number of words to copy
        @ Return value is updated destination pointer (first unwritten word)
copy_words:         .func   
        push	{r4, r5}   
0:      cmp	r2, #8  
        ble	1f  
        ldmia	r1!, {r3, r4, r5, r12}    
        stmia	r0!, {r3, r4, r5, r12}    
        sub	r2, r2, #4  
        b	0b    
        
1:      cmp	r2, #0  
        beq	3f  
2:      ldr	r3, [r1], #4    
        str	r3, [r0], #4    
        subs	r2, r2, #1     
        bne	2b  
        
3:      pop	{r4, r5}    
        bx	lr   
        .endfunc    
        
        
appf_memcpy:            .func   
        cmp	r2, #0  
        bxeq	lr     
0:                  ldrb	r3, [r1], #1   
        strb	r3, [r0], #1   
        subs	r2, #1     
        bne	0b  
        bx	lr   
        .endfunc    
        
appf_memset:            .func   
        cmp	r2, #0  
        bxeq	lr     
0:                  strb	r1, [r0], #1   
        subs	r2, #1     
        bne	0b  
        bx	lr   
        .endfunc    
        
read_mvbar:         .func   
        mrc	p15, 0, r0, c12, c0, 1  
        bx	lr   
        .endfunc    
        
        
        .section APPF_ENTRY_POINT_CODE,"ax"     
        
        
        @ Functions we need in the runtime entry point, i.e. before we switch pagetables,
        @ are placed in this area.
        
dsb:                .func   
        dsb     
        bx	lr   
        .endfunc    
        
read_mpidr:         .func   
        mrc	p15, 0, r0, c0, c0, 5   
        bx	lr   
        .endfunc    
        
va_to_pa:           .func	  @ Note: assumes conversion will be successful!
        mov	r1, r0  
        mcr	p15, 0, r0, c7, c8, 1	  @ Priv Write Current World VA-PA
        mrc	p15, 0, r0, c7, c4, 0	  @ Get PA
        bfc	r0, #0, #12		   @ We want top bits of translated addr
        bfc	r1, #12, #20		  @ plus bottom bits of input addr
        orr	r0, r0, r1  
        bx	lr   
        .endfunc    
        
read_sctlr:         .func           
        mrc     p15, 0, r0, c1, c0, 0   
        bx      lr  
        .endfunc    
        
write_sctlr:            .func   
        mcr     p15, 0, r0, c1, c0, 0   
        bx      lr  
        .endfunc    
        
read_actlr:         .func           
        mrc     p15, 0, r0, c1, c0, 1   
        bx      lr  
        .endfunc    
        
write_actlr:            .func   
        mcr     p15, 0, r0, c1, c0, 1   
        bx      lr  
        .endfunc    
        
read_prrr:          .func           
        mrc     p15, 0, r0, c10, c2, 0  
        bx      lr  
        .endfunc    
        
read_nmrr:          .func           
        mrc     p15, 0, r0, c10, c2, 1  
        bx      lr  
        .endfunc    
        
        
        .end    
