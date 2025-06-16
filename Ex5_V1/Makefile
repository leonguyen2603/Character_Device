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
	ls -l /dev/Ex5_chardev*

log:
	sudo dmesg -W




# make
# make load
# ls /dev/Ex5_chardev*
# # /dev/Ex5_chardev0 → /dev/Ex5_chardev4

# sudo chmod 666 /dev/Ex5_chardev*

# echo "Hello 0" > /dev/Ex5_chardev0
# command cat /dev/Ex5_chardev0

# echo "World 1" > /dev/Ex5_chardev1
# command cat /dev/Ex5_chardev1
