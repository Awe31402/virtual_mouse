PROJECT = vms
obj-m := $(PROJECT).o
PWD := $(shell pwd)
KDIR = /lib/modules/$(shell uname -r)/build

all:
	@echo "Compiling module $(PROJECT)"
	@make -C $(KDIR) M=$(PWD) modules
usr:
	gcc vms_usr.c -o vms_usr
	gcc vms_event.c -o vms_event

install_mod:
	@echo "Installing module......"
	sudo insmod vms.ko
	#sudo gpm -m /dev/input/event13 -t evdev

uninstall_mod:
	@echo "Uninstalling module....."
	sudo rmmod $(PROJECT).ko
clean:
	rm -rf *.ko  *.mod.c   *.o *.order *.symvers .*.cmd .*.swp .tmp_versions 2> /dev/null
