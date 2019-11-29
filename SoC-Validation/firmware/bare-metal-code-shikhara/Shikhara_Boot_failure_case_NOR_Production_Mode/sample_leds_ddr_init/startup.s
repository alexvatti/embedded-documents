.section "INTERRUPT_VECTOR"
 ldr pc,=(0xC0000000+0x4)
.global _Reset
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
  mrs     r0,cpsr
  bic     r0,r0,#0x1f
  orr     r0,r0,#0xd3
  msr     cpsr,r0

init_bss:
  /* Initialize .bss */
  ldr   r0, =sbss
  ldr   r1, =ebss
  ldr   r2, =bss_size

  /* Handle bss_size == 0*/
  cmp   r2, #0
  beq   cpu_init_cp15
  mov   r4, #0
zero:
  strb  r4, [r0], #1
  subs  r2, r2, #1
  bne   zero

cpu_init_cp15:
        /*
         * Invalidate L1 I/D
         */
        mov     r0, #0                  @ set up for MCR
        mcr     p15, 0, r0, c8, c7, 0   @ invalidate TLBs
        mcr     p15, 0, r0, c7, c5, 0   @ invalidate icache
        mcr     p15, 0, r0, c7, c5, 6   @ invalidate BP array
        mcr     p15, 0, r0, c7, c10, 4  @ DSB
        mcr     p15, 0, r0, c7, c5, 4   @ ISB

        /*
         * disable MMU stuff and caches
         */
        mrc     p15, 0, r0, c1, c0, 0
        bic     r0, r0, #0x00002000     @ clear bits 13 (--V-)
        bic     r0, r0, #0x00000007     @ clear bits 2:0 (-CAM)
        orr     r0, r0, #0x00000002     @ set bit 1 (--A-) Align
        orr     r0, r0, #0x00000800     @ set bit 11 (Z---) BTB

        orr     r0, r0, #0x00001000     @ set bit 12 (I) I-cache

        mcr     p15, 0, r0, c1, c0, 0

@==================================================================
@ SMP initialization
@==================================================================
        mrc     p15, 0, r0, c0, c0, 5      @ Read CPU ID register
        ands    r0, r0, #0x03              @ Mask off, leaving the CPU ID field
        beq     primary_cpu_init
        bne     secondary_cpus_init

secondary_cpus_init:
         wfi

primary_cpu_init:
init_stack:
  ldr     sp, =(stack_top)
  mov     r0, #0
  bl	  c_entry
