all:
	arm-none-eabi-as   -g startup.s -o startup.o
	arm-none-eabi-gcc -c -g main.c  -o main.o -I ./include
	arm-none-eabi-gcc -c -g shikhara_uart.c  -o shikhara_uart.o -I ./include
	arm-none-eabi-gcc -c -g printf.c  -o printf.o -I ./include
	arm-none-eabi-gcc -c -g shikhara_ddr3.c -o  shikhara_ddr3.o -I ./include
	arm-none-eabi-gcc -c -g  delay.c  -o  delay.o -I ./include
	arm-none-eabi-gcc -c -g gpio-leds.c -o gpio-leds.o -I ./include
	arm-none-eabi-ld -T test.ld main.o shikhara_uart.o  startup.o  printf.o shikhara_ddr3.o delay.o gpio-leds.o -o test.elf  -L /home/engineer/Projects/shikara/gcc-arm-none-eabi-4_7-2012q4/lib/gcc/arm-none-eabi/4.7.3 -lgcc
	arm-none-eabi-objcopy -O binary test.elf test.bin
clean:
	rm -rf *.bin *.elf *.o  


	
