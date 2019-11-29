# dltool/Makefile
#
# Makefile for SMDK24XX dltool
#
# (c) 2004,2006 Ben Dooks, Ben Dooks <ben-smdk2440@fluff.org>
#

.PHONY: clean

dltool: dltool.c
        $(CC) -O2 -Wall -o smdk-usbdl dltool.c -L/usr/lib/ -lusb -I/usr/include/libusb-1.0/

clean:
	-rm smdk-usbdl

