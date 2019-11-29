/*****Prime cell pl050 Keyboard Mouse controller *****/

This file describes list of files and its functionalities, which are necessary files for (PL050) Keyboard and Mouse Interface controller.

shikhara_kmi_keyboard.c : its a keyboard driver provides interfaces, sothat characters can be read by o/p devices(Ex: LCD)

-------------------------------------------------------------------------------------

shikhara_kmi_pc_keyb.h  : Configures defines kmi controller registers base address and its offsets.

-------------------------------------------------------------------------------------

shikhara_kmi.c		 : Provides low level implementation reading and writing to kmi registers.

-------------------------------------------------------------------------------------

shikhara_kmi_pc_keyb.c	:	Initializes and provides functionalities for KMI(PL-50) controller
