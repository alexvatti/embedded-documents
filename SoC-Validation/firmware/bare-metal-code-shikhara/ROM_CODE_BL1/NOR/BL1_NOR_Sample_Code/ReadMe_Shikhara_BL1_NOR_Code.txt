
Objective: NOR Booting , Using the help of ROM Code(0x9)
=======================================================

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
1. Text code should point to  On-Chip Memory Address in test.ld
 . = 0xD45B0000 ; /* On-Chip Memory Address 64KB*/

2.stack pointer in test.ld
stack_top = 0xD45BFFF0;

3.Copy/flash test.bin to NOR Falsh Base address(0xc0000000), using u-boot commands via JTAG/DS-5

4.Then power down, put the Boot Strap mode 0x9, Remove the JTAG/DS-5,  and Power On

5. See the boot messages and leds are blinking



