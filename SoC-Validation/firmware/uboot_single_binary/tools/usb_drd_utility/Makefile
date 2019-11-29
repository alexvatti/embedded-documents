obj-m += usb_drd_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

	rm  *.ko *.o 
