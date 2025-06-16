obj-m += Ex5_V3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod Ex5_V3.ko

unload:
	sudo rmmod Ex5_V3

list:
	ls -l /dev/Ex5_chardev_V3*

log:
	sudo dmesg -W




# make
# make load
# ls /dev/Ex5_chardev_V3
# # /dev/Ex5_chardev_V30 → /dev/Ex5_chardev_V34

# sudo chmod 666 /dev/Ex5_chardev_V3*

# echo "Hello 0" > /dev/Ex5_chardev_V30
# command cat /dev/Ex5_chardev_V30

# echo "World 1" > /dev/Ex5_chardev_V31
# command cat /dev/Ex5_chardev_V31
