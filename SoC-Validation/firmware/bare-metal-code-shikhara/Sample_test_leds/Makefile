all:
	arm-none-eabi-as   -g startup.s -o startup.o
	arm-none-eabi-gcc -c -g main.c  -o main.o -I ./include
	arm-none-eabi-gcc -c -g gpio-leds.c  -o gpio-leds.o -I ./include
	arm-none-eabi-ld -T test.ld main.o gpio-leds.o  startup.o -o test.elf
	arm-none-eabi-objcopy -O binary test.elf test.bin
clean:
	rm test.bin test.elf startup.o main.o gpio-leds.o
	
