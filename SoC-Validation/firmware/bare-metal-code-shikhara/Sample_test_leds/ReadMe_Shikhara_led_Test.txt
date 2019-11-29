
Objective: Toggle the leds of GPIO-0 and GPIO-1 Port,each port having 8 leds
============================================================================

step 0:
======
Memory map of the GPIO-0 and GPIO-1
GPIO-0 4KB 0xD4576000 0xD4576FFF
GPIO-1 4KB 0xD4577000 0xD4577FFF


Step 1:
======
Memory map of Shikhara Platfrom details: On Chip RAM

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
=======
arm-none-eabi-nm -n test.elf


Step 5: Testing
===============
1. Normal Mode_0 , Please select/check the following on the board

SOC_TEST_EN	MODE_SEL_4	MODE_SEL_3	MODE_SEL_2	MODE_SEL_1	MODE_SEL_0
0		0		x		x		0		0



