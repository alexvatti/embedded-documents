
Objective: BOOT STRAP Modes Testing 
====================================

step 0:
======

The memory map of UART is mentioned below taken from Tapeout Document [ UART0 is used for Console]
UART-0 	4KB 	0xD457B000 
UART-1 	4KB 	0xD457C000
UART-2 	4KB 	0xD457D000
UART-3  4KB	0xD457E000
UART-4	4KB	0xD457F000

#define SHIKHARA_SYS_CTRL_BASEADDR    0xD4561000
#define BOOT_STRAP                    (SHIKHARA_SYS_CTRL_BASEADDR+0xc4)

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
=======
Test procedure:
shikhara_soc_19042017.pdf, page number 11, BOOT STRAP OPTIONS

          CLD_17/C_LCD_B1       CLD_16/C_LCD_B0        CLD_15/C_LCD_G7   CLD_14/C_LCD_G6
  
NAND (B)    1          			0             	1         	1

NOR (9)     1          			0             	0         	1


SD (A)      1          			0             	1         	0


SPI(8)      1          			0             	0         	0


