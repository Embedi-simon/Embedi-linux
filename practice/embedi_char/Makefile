obj-m += embedi_char.o

PWD=$(shell pwd)
KERNEL_DIR="/home/simon/share/embedi/Embedi-linux/linux-6.9.1"

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	x86_64-linux-gnu-gcc -o embedi_char_test embedi_char_test.c
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm embedi_char_test
