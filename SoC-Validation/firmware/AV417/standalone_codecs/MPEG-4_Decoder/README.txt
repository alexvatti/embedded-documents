
/***************Testing MPEG4 Decoder***************/

1.  Compile the code using gmake -f Makefile command
    Convert elf to bin using below command
    elf2bin mpegdec app_mpegdec.bin

2.  Copy the compiled binary into sdcard.

3.  On board after bootup, call "mmc_start" to initialize mmc controller.

4.  Type,"fatls mmc 0:1" to check files on sdcard.

5.  fatload <interface> <dev[:part]> [addr] [filename] [bytes]

    fatload mmc 0:1 0x5d1f0000 app_jump.bin
    app_jump.bin is the application, which jumps to decoder application on DDR from SRAM.

6.  Load Decoder Application onto DDR memory 
    fatload mmc 0:1 0x41d00000 app_mpegdec.bin


7.  load input file to decoder onto DDR.
    fatload mmc 0:1 0x41b00000 encoded.m4v

8.  Now Reset the arc processor using "av417reset" command. 

9.  After reset the ARC Processor runs decoding application using the input file on 0x41b00000 DDR address.

10. After completion of decoding run the below command to copy data from DDR to mmc.
    fatwrite <interface> <dev[:part]> <addr> <filename> <bytes>
    fatwrite mmc 0:1 0x41500000 decoded.yuv 0x567000

11. Play the file using vlc player.

/**************end of MPEG4 Decoder Test***********/
