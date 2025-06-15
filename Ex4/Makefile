obj-m += Ex4.o
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
	sudo insmod Ex4.ko
unload:
	sudo rmmod Ex4
log:
	sudo dmesg -W
read_only:
	sudo chmod 444 /dev/mychardev_4
write_only:
	sudo chmod 222 /dev/mychardev_4
read_write:
	sudo chmod 666 /dev/mychardev_4
allow_all:
	sudo chmod 777 /dev/mychardev_4

###################################
# install:
# 	sudo cp Ex4.ko $(MODDIR)
# 	sudo depmod -a
# 	sudo modprobe Ex4
# uninstall:
# 	sudo modprobe -r Ex4
# 	sudo rm $(MODDIR)Ex4.ko
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
#  sudo chmod 666 /dev/mychardev_4_4.
 
#  To write to the device node, run:
#  echo "Hello, World!" > /dev/mychardev_4_4

#  To read from the device node, run:
#  command cat /dev/mychardev_4_4

#  mychardev_4_4 is the name of the character device created by the module.
