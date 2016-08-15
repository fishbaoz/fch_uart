KERN_DIR=/home/baozheng/x86/linux-3.19.8

all:
	make -C $(KERN_DIR) M=$(shell pwd) modules

clean:
	make -C $(KERN_DIR) M=$(shell pwd) modules clean
	rm -rf modules.order

obj-m += 8250_fch.o
