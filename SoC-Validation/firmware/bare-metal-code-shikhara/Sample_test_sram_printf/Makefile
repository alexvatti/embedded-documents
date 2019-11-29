all:
	arm-none-eabi-as   -g startup.s -o startup.o
	arm-none-eabi-gcc -c -g main.c  -o main.o -I ./include
	arm-none-eabi-gcc -c -g shikhara_uart.c  -o shikhara_uart.o -I ./include
	arm-none-eabi-gcc -c -g printf.c  -o printf.o -I ./include
	arm-none-eabi-gcc -c -g  shikhara_smc_sram.c  -o  shikhara_smc_sram.o -I ./include
	arm-none-eabi-ld -T test.ld main.o shikhara_uart.o  startup.o  printf.o shikhara_smc_sram.o -o test.elf  -L/home/engineer/Projects/shikara/gcc-arm-none-eabi-4_7-2012q4/lib/gcc/arm-none-eabi/4.7.3 -lgcc
	arm-none-eabi-objcopy -O binary test.elf test.bin
clean:
	rm test.bin test.elf startup.o main.o shikhara_uart.o printf.o shikhara_smc_sram.o 


	
