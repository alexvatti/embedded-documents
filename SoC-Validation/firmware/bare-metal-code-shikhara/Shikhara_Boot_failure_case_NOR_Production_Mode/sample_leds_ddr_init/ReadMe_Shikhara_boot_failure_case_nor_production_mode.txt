
Objective: Shikhara Boot failure case[NOR Production Mode(0X000))
==================================================================

step 0:
======

The memory map of UART is mentioned below taken from Tapeout Document [ UART0 is used for Console]
UART-0 	4KB 	0xD457B000 
UART-1 	4KB 	0xD457C000
UART-2 	4KB 	0xD457D000
UART-3  4KB	0xD457E000
UART-4	4KB	0xD457F000


Step 1:
=========
Memory map of Shikhara Platfrom details: ON Chip RAM details

Name    		Size               Start Address 	End Address
Cortex A9 On-Chip RAM 	64KB 		   0xD45B0000 		0xD45BFFFF

Step 2:
======
***Need to check "arm-none-eabi-gcc" shold be installed and working fine [path has set properly]

 make all  => build
 make clean => to delete the temp files

Step 3: [Information only]
==========================
Code Compilation steps

arm-none-eabi-gcc -nostdlib -mcpu=cortex-a9 -o test.elf -T test.ld main.c startup.s
arm-none-eabi-objcopy -O binary test.elf test.bin

Step 4: to know th symbols
======
arm-none-eabi-nm -n test.elf

Step 5:
======
1.Base address should point to NOR Flash Address in test.ld 
 . = 0xC0000000; /* NOR Flash Address*/

2. Data and bss section should point to On-Chip Memory Address in test.ld
. = 0xD45B0000 ; /* On-Chip Memory Address 64KB*/

3. stack is deceremental stack, so pointig to 0xD45BFFF0 in test.ld
 stack_top = 0xD45BFFF0;


4.startup.s, Please consider the change

.section "INTERRUPT_VECTOR"
 ldr pc,=(0xC0000000+0x4)
.global _Reset
_Reset:


