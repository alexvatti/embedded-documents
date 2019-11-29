Objective:SMC -SRAM Write and Read and verify the fixed pattern
================================================================

Step 0:
=====
SMC-SRAM Memory Map in Shikhara


Name    		Size               Start Address 	End Address
NOR_MEM_0  		64MB  		   0xC4000000 		0xC7FFFFFF [ note*****:chip select 1 and interface 0] 

SMC Address:
Name    		Size               Start Address 	End Address
SMC 			4KB		   0xD45A2000		0xD45A2FFF



Step 1:
======
Memory map of Shikhara Platfrom details:on chip RAM

Name    		Size               Start Address 	End Address
Cortex A9 On-Chip RAM 	64KB 		   0xD45B0000 		0xD45BFFFF

Step 2:
======
***Need to check "arm-none-eabi-gcc" shold be installed and working fine [path has set properly]
 make all  => build
 make clean => to delete the temp files

Step 3: [Information only]
=============================
Code Compilation steps

arm-none-eabi-gcc -nostdlib -mcpu=cortex-a9 -o test.elf -T test.ld main.c startup.s
arm-none-eabi-objcopy -O binary test.elf test.bin

Step 4: to knowe symbols
======
arm-none-eabi-nm -n test.elf

Step 5:
======
smc_sram_init:
1.SMC0 clock reduceed from 133MHz to 100MHz
2. chip select is cs0(chip select), instead of cs1(chip select)

smc_sram_write: 
SMC_SRAM_BASE_ADDR:0xC4000000 , write 0x1592 into 1024 locations ( 2 bytes each)

smc_sram_read: 
SMC_SRAM_BASE_ADDR:0xC4000000 , read from 1024 locations ( 2 bytes each)

smc_sram_verify:
will copmare write and read array buffer, if same will return 1(success), if differ will return 0(fail), for 1024 locations

