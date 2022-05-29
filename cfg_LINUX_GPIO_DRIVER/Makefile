obj-m := linux_gpio_driver_main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	insmod linux_gpio_driver_main.ko
	chown root:gpio /dev/GPIO_DRIVER
	chmod ug=+rw /dev/GPIO_DRIVER

uninstall:
	rmmod linux_gpio_driver_main.ko
