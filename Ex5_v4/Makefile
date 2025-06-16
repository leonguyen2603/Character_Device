obj-m += Ex5_V4.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod Ex5_V4.ko

unload:
	sudo rmmod Ex5_V4

list:
	ls -l /dev/Ex5_V4_*

log:
	sudo dmesg -W




# make
# make load
# ls /dev/Ex5_V4_
# # /dev/Ex5_V4_0 → /dev/Ex5_V4_4

# sudo chmod 666 /dev/Ex5_V4_*

# echo "Hello 0" > /dev/Ex5_V4_0
# command cat /dev/Ex5_V4_0

# echo "World 1" > /dev/Ex5_V4_1
# command cat /dev/Ex5_V4_1
