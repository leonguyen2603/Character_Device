obj-m += Ex2_2.o
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
	sudo insmod Ex2_2.ko
unload:
	sudo rmmod Ex2_2
log:
	sudo dmesg -W

#################################
# install:
# 	sudo cp Ex2_2.ko $(MODDIR)
# 	sudo depmod -a
# 	sudo modprobe Ex2_2
# uninstall:
# 	sudo modprobe -r Ex2_2
# 	sudo rm $(MODDIR)Ex2_2.ko
# 	sudo depmod -a

################################
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

#  To create a character device node, run:
#  sudo mknod /dev/my_character_device c <major> 0
 
#  To set permissions for the device node, run:
#  sudo chmod 666 /dev/my_character_device.
 
#  To write to the device node, run:
#  echo "Hello, World!" > /dev/my_character_device

#  To read from the device node, run:
#  command cat /dev/my_character_device

#  my_character_device is the name of the character device created by the module.
