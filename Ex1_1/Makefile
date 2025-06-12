obj-m += Ex1_1.o
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
	sudo insmod Ex1_1.ko
unload:
	sudo rmmod Ex1_1
log:
	sudo dmesg -W
		
###############################
# install:
# 	sudo cp Ex1_1.ko $(MODDIR)
# 	sudo depmod -a
# 	sudo modprobe Ex1_1
# uninstall:
# 	sudo modprobe -r Ex1_1
# 	sudo rm $(MODDIR)Ex1_1.ko
# 	sudo depmod -a