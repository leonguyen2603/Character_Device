obj-m += Ex3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
load:
	sudo insmod Ex3.ko
unload:
	sudo rmmod Ex3
log:
	sudo dmesg -W
ls:
	ls /dev/mychardev_3
test:
	gcc test.c -o test
	./test
echo:
	echo "Hello World" | sudo tee /dev/mychardev_3

