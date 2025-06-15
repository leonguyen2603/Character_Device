obj-m += Ex2_1.o
		#the object file (.o) has same name as source file (.c)

KDIR = /lib/modules/$(shell uname -r)/build M=$(PWD) 
		#$(shell uname -r) returns the curent kernel version
		#M$(PWD) -> curent working directory

MODDIR = /lib/modules/$(shell uname -r)/kernel/drivers/

all:
	make -C $(KDIR) modules
clean: 
	make -C $(KDIR) clean
load:
	sudo insmod Ex2_1.ko
unload:
	sudo rmmod Ex2_1
log:
	sudo dmesg -W

###################################
# install:
# 	sudo cp Ex2_1.ko $(MODDIR)
# 	sudo depmod -a
# 	sudo modprobe Ex2_1
# uninstall:
# 	sudo modprobe -r Ex2_1
# 	sudo rm $(MODDIR)Ex2_1.ko
# 	sudo depmod -a

##################################
#  To compile the module, run:
#  make

#  To clean the build files, run:
#  make clean

#  To install the module, run:
#  make install

#  To uninstall the module, run:
#  make uninstall

#  To load the module, run:
#  make load

#  To unload the module, run:
#  make unload

#  To view kernel messages, run:
#  make log
 
#  To set permissions for the device node, run:
#  sudo chmod 666 /dev/mychardev.
 
#  To write to the device node, run:
#  echo "Hello, World!" > /dev/mychardev

#  To read from the device node, run:
#  command cat /dev/mychardev

#  mychardev is the name of the character device created by the module.
