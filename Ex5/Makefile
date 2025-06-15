obj-m += multi_chardev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod multi_chardev.ko

unload:
	sudo rmmod multi_chardev

list:
	ls -l /dev/mychardev*

log:
	sudo dmesg -W




# make
# make load
# ls /dev/mychardev*
# # /dev/mychardev0 → /dev/mychardev4

# sudo chmod 666 /dev/mychardev*

# echo "Hello 0" > /dev/mychardev0
# command cat /dev/mychardev0

# echo "World 1" > /dev/mychardev1
# command cat /dev/mychardev1
