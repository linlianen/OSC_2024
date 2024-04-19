CC = aarch64-linux-gnu
CFLAGS = -Wall  -O2 -ffreestanding  -nostartfiles -I ./include/ -fno-toplevel-reorder
BOOTLOADER = ./bootloader
SRC = src
LIB = lib
all: kernel8.img

LINKER_FILE = linker.ld

ASM_SRCS = $(wildcard $(SRC)/*.S) 
ASM_OBJS = $(patsubst %.S, %.o, $(ASM_SRCS))


SRCS = $(wildcard $(SRC)/*.c) 
OBJS = $(patsubst %.c, %.o, $(SRCS))

# for c file in lib
LIB_SRC = $(wildcard $(LIB)/*.c) 
OBJ_LIB = $(patsubst %.c,%.o,$(LIB_SRC))

# for bootloader in lib
BOOT_SRC = $(wildcard $(BOOTLOADER)/*.c)
BSRC_OBJ = $(patsubst %.c,%.o,$(BOOT_SRC)) 


BOOTASM_SRC = $(wildcard $(BOOTLOADER)/*.S)
BASM_OBJ = $(patsubst %.S,%.o,$(BOOTASM_SRC)) 


# BASM_OBJ = $(patsubst %.S,%.o,$(BOOTASM_SRC)) 

$(LIB)/%.o: $(LIB)/%.c
	$(CC)-gcc $(CFLAGS) -c $< -o $@ -g


$(SRC)/%.o: $(SRC)/%.c
	$(CC)-gcc $(CFLAGS) -c $< -o $@ -g


$(SRC)/%.o: $(SRC)/%.S
	$(CC)-gcc -c $< -o $@

$(BOOTLOADER)/%.o: $(BOOTLOADER)/%.c
	$(CC)-gcc $(CFLAGS) -c $< -o $@

$(BOOTLOADER)/%.o: $(BOOTLOADER)/%.S
	$(CC)-gcc  -c $< -o $@
# output/reboot.o: src/reboot.c
# 	${CC}-gcc ${CFLAGS} -c src/reboot.c -o output/reboot.o

kernel8.img: $(ASM_OBJS) $(OBJS) $(OBJ_LIB)
	aarch64-linux-gnu-ld $(ASM_OBJS) $(OBJS) $(OBJ_LIB) -T src/linker.ld -o kernel8.elf 
	${CC}-objcopy -O binary kernel8.elf kernel8.img

bootloader.img: $(BASM_OBJ) $(BSRC_OBJ) $(OBJ_LIB)
	aarch64-linux-gnu-ld $(BASM_OBJ) $(BSRC_OBJ) $(LIB)/*.o  -T $(BOOTLOADER)/linker.ld -o $(BOOTLOADER)/bootloader.elf 
	${CC}-objcopy -O binary bootloader/bootloader.elf bootloader/bootloader.img


run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb  
pi: 
	cp kernel8.img nctuos/kernel8.img
	genisoimage -o nctuos.img nctuos
	sudo dd if=nctuos.img of=/dev/sdb



clean :
	rm -rf $(LIB)/*.o $(SRC)/*.o $(BOOTLOADER)/*o kernel8.elf kernel8.img bootloader/bootloader.elf bootloader/bootloader.img
 
