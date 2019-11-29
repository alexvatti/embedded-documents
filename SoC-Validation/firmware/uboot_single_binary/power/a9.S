        @ Created by arm_to_gnu.pl from a9.S
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
        
        .section APPF,"ax"  
        
        
        .global save_a9_debug   
        .global restore_a9_debug    
        
        .global save_a9_global_timer    
        .global restore_a9_global_timer     
        
        .global save_a9_other   
        .global restore_a9_other    
        
    .equ CA9_SCU_ICD, 0x1000 @@ GIC Distributor offset from SCU_BASE
    .equ CA9_SCU_ICC, 0x100 @@ GIC CPU Interface offset from SCU_BASE
    .equ CA9_SCU_TIM64, 0x200 @@ (64-bit) timer block offset from SCU_BASE
        
        @ SCU_BASE offsets:
    .equ SCU_Ctl, 0x0
    .equ SCU_Config, 0x4
    .equ SCU_PwrStatus, 0x8
    .equ SCU_InvAll, 0xC
    .equ SCU_FiltStart, 0x40
    .equ SCU_FiltEnd, 0x44
    .equ SCU_SAC, 0x50
    .equ SCU_SSAC, 0x54
        
        @ Global timer offsets
    .equ TIM64_CntLo, 0x0
    .equ TIM64_CntHi, 0x4
    .equ TIM64_Ctl, 0x8
    .equ TIM64_Status, 0xC
    .equ TIM64_CmpLo, 0x10
    .equ TIM64_CmpHi, 0x14
    .equ TIM64_AutoInc, 0x18
        
        
save_a9_global_timer:           .func   
        PUSH	{r4, r5}   
        MOVW	r12, #CA9_SCU_TIM64    
        ADD	r1,r1,r12   
        LDR	r2, [r1, #TIM64_Ctl]		  @ 64-bit timer control
        BIC	r3, r2, #0xF    
        STR	r3, [r1, #TIM64_Ctl]		  @ disable the features
        @ the registers are now frozen for the context save          
        LDR	r3, [r1, #TIM64_AutoInc]	   @ Autoincrement register
        LDR	r4, [r1, #TIM64_CmpLo]		    @ comparator - lo word
        LDR	r5, [r1, #TIM64_CmpHi]		    @ comparator - hi word
        STM	r0!, {r2-r5}    
        LDR	r2, [r1, #TIM64_CntLo]		    @ counter - lo word          
        LDR	r3, [r1, #TIM64_CntHi]		    @ counter - hi word
        STM	r0!, {r2-r3}    
        POP	{r4, r5}    
        BX	lr   
        .endfunc    
        
restore_a9_global_timer:            .func   
        PUSH	{r4, r5}   
        MOVW	r12, #CA9_SCU_TIM64    
        ADD	r1,r1,r12   
        LDM	r0!, {r2-r5}            
        STR	r3, [r1, #TIM64_AutoInc]	   @ Autoincrement register
        STR	r4, [r1, #TIM64_CmpLo]		    @ comparator - lo word
        STR	r5, [r1, #TIM64_CmpHi]		    @ comparator - hi word
        LDM	r0!, {r3-r4}    
        STR	r3, [r1, #TIM64_CntLo]		    @ counter - lo word          
        STR	r4, [r1, #TIM64_CntHi]		    @ counter - hi word
        STR	r2, [r1, #TIM64_Ctl]		  @ restore the control last 
        
        POP	{r4, r5}    
        BX	LR   
        .endfunc	   
        
        
save_a9_debug:          .func   
        @ TODO
        bx	lr	  
        .endfunc    
        
restore_a9_debug:           .func   
        @ TODO
        bx	lr	  
        .endfunc    
        
save_a9_other:          .func   
        MRC	p15,0,r1,c15,c0,0	  @ Read Power Control Register
        STR	r1, [r0], #4    
        
        MRC	p15,0,r3,c0,c0,0	   @ Read Main ID Register
        UBFX	r3, r3, #20, #4		  @ Extract major version number
        CMP	r3, #2  
        BLT	1f			   @ PLE only possible in r2p0 onwards
        MRC	p15,0,r3,c11,c0,0	  @ Read PLE IDR
        CMP	r3, #0  
        BEQ	1f			   @ No PLE present
        
        MRC	p15,0,r1,c11,c1,0	  @ Read PLE UAR
        MRC	p15,0,r2,c11,c1,1	  @ Read PLE PCR
        STM	r0!, {r1, r2}   
        
1:                  bx	lr	  
        .endfunc    
        
restore_a9_other:           .func   
        LDR	r1, [r0], #4    
        ANDS	r1, r1, #0x01		    @ We only restore the Dynamic Clock gating bit
        MCR	p15,0,r1,c15,c0,0	  @ Write Power Control Register
        
        MRC	p15,0,r3,c0,c0,0	   @ Read Main ID Register
        UBFX	r3, r3, #20, #4		  @ Extract major version number
        CMP	r3, #2  
        BLT	1f			   @ PLE only possible in r2p0 onwards
        MRC	p15,0,r3,c11,c0,0	  @ Read PLE IDR
        CMP	r3, #0  
        BEQ	1f			   @ No PLE present
        
        LDM	r0!, {r1, r2}   
        MCR	p15,0,r1,c11,c1,0	  @ Write PLE UAR
        MCR	p15,0,r2,c11,c1,1	  @ Write PLE PCR
        
1:                  bx	lr	  
        .endfunc    
        
        
        .end    
        
        
        
