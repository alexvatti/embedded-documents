.section "INTERRUPT_VECTOR"
.global _Reset
.global relocate_code
.global _Start
_Start:
 ldr pc,=(0xC0000004)
_Reset:
  B Reset_Handler /* Reset */
  B . /* Undefined */
  B . /* SWI */
  B . /* Prefetch Abort */
  B . /* Data Abort */
  B . /* reserved */
  B . /* IRQ */
  B . /* FIQ */
 
Reset_Handler:
 /* set the cpu to SVC32 mode  */
  bl      cpu_init_cp15
  mrs     r0,cpsr
  bic     r0,r0,#0x1f
  orr     r0,r0,#0xd3
  msr     cpsr,r0

  ldr     sp, =(stack_top)
  mov     r0, #0
  bl	  c_entry
  ldr 	pc,=(0x3D000000)

relocate_code:
	mov r0,#1
	mov r1,#2
	mov r2,#3
	ldr pc,=(0x3D000000)
	 

cpu_init_cp15:
        mrc     p15, 0, r0, c0, c0, 5      @ Read CPU ID register
        ands    r0, r0, #0x03              @ Mask off, leaving the CPU ID field
        beq     primary_cpu_init
        bne     secondary_cpus_init

secondary_cpus_init:
        wfi

primary_cpu_init:
  mrs     r0,cpsr
  bic     r0,r0,#0x1f
  orr     r0,r0,#0xd3
  msr     cpsr,r0

  ldr     sp, =(stack_top)
  mov     r0, #0
  bl	  c_entry
  ldr 	pc,=(0x3D000000)
  mov       ip,lr
  mov       lr,ip
  mov       pc, lr 


