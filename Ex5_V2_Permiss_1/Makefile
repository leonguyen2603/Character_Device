obj-m += Ex5_v2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod Ex5_v2.ko

unload:
	sudo rmmod Ex5_v2

list:
	ls -l /dev/Ex5_character_device*

log:
	sudo dmesg -W




# make
# make load
# ls /dev/Ex5_character_device
# # /dev/Ex5_character_device0 → /dev/Ex5_character_device4

# sudo chmod 666 /dev/Ex5_character_device*

# echo "Hello 0" > /dev/Ex5_character_device0
# command cat /dev/Ex5_character_device0

# echo "World 1" > /dev/Ex5_character_device1
# command cat /dev/Ex5_character_device1
