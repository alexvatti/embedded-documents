
/***************Testing MPEG4 Encoder***************/
Compilation:
1. Compile Application using the given Makefile
2. Convert elf to bin using "elf2bin" tool
   elf2bin example.elf example.bin

Execution:
1. Run, first arc boot loader on sram,using below commands
   fatload mmc 0:1 0x5d1f0000 arc_jump.bin

2. Before starting encoding, load the file from mmc into ram address specified below.

3. fatload <interface> <dev[:part]> [addr] [filename] [bytes]
   Using fatload mmc 0:1 0x41500000 input.yuv
   akiyo_qcif.yuv is the input file to be encoded, written into ram address 0x41500000

4. fatload mmc 0:1 0x41d00000 app_mpeg4_encoder.bin 

5. Reset arc processor using "av417reset" command.

6. After reset, the ARC Processor runs encoding application using the input file on 0x41500000 ram address.

4. After completion of encoding run the below command to copy data from ram to mmc.
   fatwrite <interface> <dev[:part]> <addr> <filename> <bytes>
   fatwrite mmc 0:1 0x41b00000 encoded.m4v 0x1dc00

5. Play the file using vlc player.

/**************end of MPEG4 Encoder Test***********/
