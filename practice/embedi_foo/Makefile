obj-m += embedi_foo.o

PWD=$(shell pwd)
KERNEL_DIR="/home/simon/share/embedi/Embedi-linux/linux-6.9.1"

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
