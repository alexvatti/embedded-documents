# Simple boot vector file to jump to start on power up
# We also implement halt handler for all the other vectors
# in case an unexpected interrupt/exception occurs during the boot up
   
.extern     isr3_dummy
.extern     isr4_dummy
.extern     isr5_dummy
.extern     isr6_dummy
.extern     isr7_dummy
.extern     isr8_dummy
.extern     isr9_dummy
.extern     isr10_dummy
.extern     isr11_dummy
.extern     isr12_dummy
.extern     isr13_dummy
.extern     isr14_dummy
.extern     isr15_dummy

.section    .rom_vectors, text

# This directive forces this section to stay resident even if stripped out by the -zpurgetext linker option
.sectflag .rom_vectors, include  

.global _reset

_reset:         JAL _start                  ;vector 0
.align 8
mem_ex:         JAL halt_handler            ;vector 1 - Memory Exception
.align 8
ins_err:        JAL halt_handler            ;vector 2 - Instruction Error
.align 8
isr3:           JAL isr3_dummy              ;vector 3 - dummy isr
.align 8
isr4:           JAL isr4_dummy              ;vector 4 - dummy isr
.align 8
isr5:           JAL isr5_dummy                 ;vector 5 - ipc isr
.align 8
isr6:           JAL isr6_dummy              ;vector 6 - dummy isr
.align 8
isr7:           JAL isr7_dummy              ;vector 7 - dummy isr
.align 8
isr8:           JAL isr8_dummy              ;vector 8 - dummy isr
.align 8
isr9:           JAL isr9_dummy              ;vector 9 - dummy isr
.align 8
isr10:          JAL isr10_dummy             ;vector 10 - dummy isr
.align 8
isr11:          JAL isr11_dummy             ;vector 11 - dummy isr
.align 8
isr12:          JAL isr12_dummy             ;vector 12 - dummy isr
.align 8
isr13:          JAL isr13_dummy             ;vector 13 - dummy isr
.align 8
isr14:          JAL isr14_dummy             ;vector 14 - dummy isr
.align 8
isr15:          JAL isr15_dummy             ;vector 15 - dummy isr
.align 8

halt_handler:          
   flag 1
   nop
   nop
   nop

