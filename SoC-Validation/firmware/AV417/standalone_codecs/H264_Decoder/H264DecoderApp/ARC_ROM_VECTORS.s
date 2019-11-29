# Simple boot vector file to jump to start on power up
# We also implement halt handler for all the other vectors
# in case an unexpected interrupt/exception occurs during the boot up
   
.section    .rom_vectors, text

# This directive forces this section to stay resident even if stripped out by the -zpurgetext linker option
.sectflag .rom_vectors, include  

.global _reset
_reset:         JAL _start                  ;vector 0
halt_handler:          
   flag 1
   nop
   nop
   nop

