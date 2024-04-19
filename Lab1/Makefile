CC = aarch64-linux-gnu
CFLAGS = -Wall -g -O2 -ffreestanding  -nostartfiles
all: kernel8.img

output/a.o: src/start.S
	${CC}-gcc ${CFLAGS} -c src/start.S -o output/start.o

output/main.o: src/main.c
	${CC}-gcc ${CFLAGS} -c src/main.c -o output/main.o

output/lib.o: src/lib.c
	${CC}-gcc ${CFLAGS} -c src/lib.c -o output/lib.o

output/UART.o: src/uart.c
	${CC}-gcc ${CFLAGS} -c src/uart.c -o output/uart.o

output/mailbox.o: src/mailbox.c
	${CC}-gcc ${CFLAGS} -c src/mailbox.c -o output/mailbox.o

# output/reboot.o: src/reboot.c
# 	${CC}-gcc ${CFLAGS} -c src/reboot.c -o output/reboot.o

kernel8.img:  output/main.o output/UART.o output/a.o output/lib.o output/mailbox.o
	${CC}-ld -Iinclude -T src/linker.ld -o kernel8.elf output/start.o output/main.o output/uart.o output/lib.o output/mailbox.o
	${CC}-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -f kernel8.elf
	rm -f output/*

run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio

pi: 
	cp kernel8.img nctuos/kernel8.img
	genisoimage -o nctuos.img nctuos
	sudo dd if=nctuos.img of=/dev/sdb


