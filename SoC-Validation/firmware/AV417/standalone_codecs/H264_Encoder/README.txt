
/***************Testing H264 Encoder***************/

Compilation:
1. Compile Application using the given Makefile(gmake -f Makefile.win32.h264)
2. Convert elf to bin using "elf2bin" tool
   elf2bin h264dec h264dec.bin

Execution:
1. Run, first arc boot loader on sram,using below commands
   fatload mmc 0:1 0x5d1f0000 arc_jump.bin

2. Before starting encoding, load the file from mmc into ram address specified below.

3. fatload <interface> <dev[:part]> [addr] [filename] [bytes]
   Using fatload mmc 0:1 0x41500000 input.yuv
   input.yuv is the input file to be encoded, written into ram address 0x41500000

4. fatload mmc 0:1 0x41d00000 h264_encoder_app.bin 

5. Reset arc processor using "av417reset" command.

6. After reset, the ARC Processor runs encoding application using the input file on 0x41500000 ram address.

4. After completion of encoding run the below command to copy data from ram to mmc.
   fatwrite <interface> <dev[:part]> <addr> <filename> <bytes>
   fatwrite mmc 0:1 0x41b00000 encoded.h264 0x13000

5. Play the file using vlc player.

Clean:
1.Inorder to clean the code during compilation use the following commands.
gmake -f Makefile.win32.h264 clean
gmake -f Makefile.win32.h264 libclean
gmake -f Makefile.win32.h264 depclean
gmake -f Makefile.win32.h264 libdepclean

/**************end of H264 Encoder Test***********/
