
firmware Release Oct,26,2017  

Contains

|-- AV417
|-- bare-metal-code-shikhara
|-- DSI_Images_800x1280
|-- gpu_validation
|-- Hdmi_audio_video_files
|-- Hdmi_Image_720x480
|-- Hdmi_Images_640x480
|-- NavikaUI
|-- ReadMe.txt
|-- toolchain
|-- uboot_single_binary
`-- USBTOOL.zip

Tool Chain:
==========
toolchain directory contain the following (gcc vertsion 4.7.2) 

gcc-arm-none-eabi-4_7-2012q4-20121208-linux.tar.bz2 
 
If you do not have the tool chain, extract the file "gcc-arm-none-eabi-4_7-2012q4-20121208-linux.tar.bz2", by using the following command

1.tar -jxvf gcc-arm-none-eabi-4_7-2012q4-20121208-linux.tar.bz2 , after extraction, we will see the following

2.ls -l gcc-arm-none-eabi-4_7-2012q4/bin/
total 19556
-rwxr-xr-x 1 engineer engineer  608844 Dec  7  2012 arm-none-eabi-addr2line
-rwxr-xr-x 2 engineer engineer  633120 Dec  7  2012 arm-none-eabi-ar
-rwxr-xr-x 2 engineer engineer 1065788 Dec  7  2012 arm-none-eabi-as
-rwxr-xr-x 2 engineer engineer  463292 Dec  7  2012 arm-none-eabi-c++
-rwxr-xr-x 1 engineer engineer  608220 Dec  7  2012 arm-none-eabi-c++filt
-rwxr-xr-x 1 engineer engineer  463292 Dec  7  2012 arm-none-eabi-cpp
-rwxr-xr-x 1 engineer engineer   21300 Dec  7  2012 arm-none-eabi-elfedit
-rwxr-xr-x 2 engineer engineer  463292 Dec  7  2012 arm-none-eabi-g++
-rwxr-xr-x 2 engineer engineer  459228 Dec  7  2012 arm-none-eabi-gcc
-rwxr-xr-x 2 engineer engineer  459228 Dec  7  2012 arm-none-eabi-gcc-4.7.3
-rwxr-xr-x 1 engineer engineer   18996 Dec  7  2012 arm-none-eabi-gcc-ar
-rwxr-xr-x 1 engineer engineer   18932 Dec  7  2012 arm-none-eabi-gcc-nm
-rwxr-xr-x 1 engineer engineer   18932 Dec  7  2012 arm-none-eabi-gcc-ranlib
-rwxr-xr-x 1 engineer engineer  179940 Dec  7  2012 arm-none-eabi-gcov
-rwxr-xr-x 1 engineer engineer 3364364 Dec  7  2012 arm-none-eabi-gdb
-rwxr-xr-x 1 engineer engineer 3364364 Dec  7  2012 arm-none-eabi-gdbtui
-rwxr-xr-x 1 engineer engineer  667532 Dec  7  2012 arm-none-eabi-gprof
-rwxr-xr-x 4 engineer engineer  896704 Dec  7  2012 arm-none-eabi-ld
-rwxr-xr-x 4 engineer engineer  896704 Dec  7  2012 arm-none-eabi-ld.bfd
-rwxr-xr-x 2 engineer engineer  617964 Dec  7  2012 arm-none-eabi-nm
-rwxr-xr-x 2 engineer engineer  765988 Dec  7  2012 arm-none-eabi-objcopy
-rwxr-xr-x 2 engineer engineer  922252 Dec  7  2012 arm-none-eabi-objdump
-rwxr-xr-x 2 engineer engineer  633120 Dec  7  2012 arm-none-eabi-ranlib
-rwxr-xr-x 1 engineer engineer  369744 Dec  7  2012 arm-none-eabi-readelf
-rwxr-xr-x 1 engineer engineer  610016 Dec  7  2012 arm-none-eabi-size
-rwxr-xr-x 1 engineer engineer  609484 Dec  7  2012 arm-none-eabi-strings
-rwxr-xr-x 2 engineer engineer  765988 Dec  7  2012 arm-none-eabi-strip

3. Set the ~/.bashrc file (in ubuntu) by
 PATH=$PATH:Realtive Path/gcc-arm-none-eabi-4_7-2012q4/bin/


u-boot Source Code:
==================

uboot_single_binary contains the u-boot source , which have all the functionality and test cases for validation of Shikhara SOC Board.

1.Configuration file in u-boot: include/configs/anusoc.h
2.Default Configuation of u-boot:
	1. Relocation is off 		(#define CONFIG_RELOCATION_OFF )                       
	2. Loading and Execution Address:  0x3D000000           (#define CONFIG_SYS_TEXT_BASE  0x3D000000)
	3. HDMI is configured for 640x480, to see the camera preview.
	4. GPU & GPU_SCENE are disabled by default
        5. GPU & GPU_SCENE are need to enabled at a time one

	*Enable this flag to test GPU */
        #undef CONFIG_SHIKHARA_GPU
        #ifdef CONFIG_SHIKHARA_GPU
                #define CONFIG_SHIKHARA_GPU_CORE
                #define CONFIG_MALIGPU_GP
                #define CONFIG_MALIGPU_PP0
                #define CONFIG_MALIGPU_PP1
                #define CONFIG_MALIGPU_PP2
                #define CONFIG_MALIGPU_PP3
                #define CONFIG_MALIGPU_MP1
                #define CONFIG_MALIGPU_MP2
                #define CONFIG_MALIGPU_MP3
        #endif/*endif for CONFIG_SHIKHARA_GPU check*/

        #undef CONFIG_GPU_SCENE
        #ifdef CONFIG_GPU_SCENE
                #define CONFIG_SHIKHARA_GPU_CORE
        #endif

	6. AV417 Alpha blending, need to configure HDMI to 720x480 Resolution.
	   #define HDMI_640_480 
           #undef HDMI_720_480 

	7.Compilation Fix (GNSS )
	enter into uboot_single_binary directory, open the Makefile, modify library path(need to find libgcc)

	PLATFORM_LIBGCC := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -L $(PWD)/lib/gnss-fw -lgps  -L /home/engineer/Documents/Toolchains/gcc-arm-none-eabi-4_7-2012q4/arm-none-eabi/lib -lgcc -lc -lm
	

Bare Metal Code:
===============
These are the sample programs to validate/test the initial bring of SOC/Board, these are copied to Shikhara SOC Board with the help of JTAG.

Contain the following directories
|-- ROM_CODE_BL1
|-- Sample_test_bootstrap
|-- Sample_test_leds
|-- Sample_test_nor_flash
|-- Sample_test_sram_printf
|-- Sample_test_uart
|-- Sample_test_uart_printf
|-- Shikhara_Boot_failure_case_NOR_Production_Mode
`-- Shikharalatest_ASIC_ddr3_working_stable


AV417:
=====

Contain the following directories

|-- app_alphablending
|-- app_ipc
|-- app_jump
|-- AV417_ASIC_BINARIES
|-- AV417_BINARIES
|-- AV417_STATIC_LIBRARIES
|-- AV417_VIDEO_FILES
|-- boot
|-- CVS
|-- drivers
|-- headers
|-- linker
`-- standalone_codecs

AV417_ASIC_BINARIES 
   1. contain the pre-built tested binaries, copy to sd card for testing
   2. Alpha blending is compiled and configured and tested for HDMI 720X480 Resolution, we need to select in u-boot level (720x480)
   3. The  DDR address 0x0120_0000 , 0x0150_0000,0x01D0_0000 are fixed and used by AV417, These are hard coded during the AV417 Application compilation.



GPU :
===

gpu_validation directory contains the following 
|-- clean.sh
`-- gpu
    |-- gp
    |-- mp1
    |-- mp2
    |-- mp3
    |-- pp0
    |-- pp1
    |-- pp2
    |-- pp3
    `-- proxycon

HDMI
====

Hdmi_audio_video_file contains the following for the sake of testing
.
|-- Shape_of_You.wav
|-- sia_common_common.wav
`-- video_raw_176x144_24bpp_149frames

Hdmi_Image_720x480  contians the 720x480 Resolution BMP files for testing
.
|-- 1.bmp
|-- 2.bmp
|-- 3.bmp
|-- 5.bmp
|-- 5..bmp
|-- a.yuv
|-- b.yuv
|-- c.yuv
`-- Hdmi_bmpslideShow_1500_delay.img


Hdmi_Images_640x480 contians the 720x480 Resolution BMP files for testing
.
|-- 1.bmp
|-- 2.bmp
|-- 3.bmp
|-- 4.bmp
|-- 5.bmp
`-- Hdmi_bmpslideShow_1500_delay.img


DSI:
===

DSI_Images_800x1280 contians the 800x1280 Resolution BMP files for testing
.
|-- red_flowers_macro_rose_800x1280.bmp
|-- red_street_ferrari_ferrari_800x1280.bmp
`-- wine_glasses_white_barrel_bottle_wine_box_800x1280.bmp


USB TOOL:
=======

contains the following
USBTOOL.zip

Extract USBTOOL.zip, follow the USB DRD Test Procedure from the Validation Report


