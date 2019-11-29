all:
	arm-none-eabi-as   -g startup.s -o startup.o
	arm-none-eabi-gcc -c -g main.c  -o main.o -I ./include
	arm-none-eabi-gcc -c -g shikhara_uart.c  -o shikhara_uart.o -I ./include
	arm-none-eabi-ld -Ttext=0x0 test.ld main.o shikhara_uart.o  startup.o -o test.elf
	arm-none-eabi-objcopy -O binary test.elf test.bin
clean:
	rm test.bin test.elf startup.o main.o shikhara_uart.o 
	
	
