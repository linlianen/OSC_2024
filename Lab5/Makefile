CC = aarch64-linux-gnu
CFLAGS = -Wall  -O0 -ffreestanding  -nostartfiles -I ./include/  
BOOTLOADER = ./bootloader
SRC = src
LIB = lib
all: kernel8.img
BUILD:=output

LINKER_FILE = linker.ld

ASM_SRCS = $(wildcard $(SRC)/*.S) 
ASM_OBJS = $(patsubst %.S, %.o, $(ASM_SRCS))

USR_SRCS = $(wildcard user_prog/*.S) 
USR_OBJS = $(patsubst %.S, %.o, $(USR_SRCS))

SRCS = $(wildcard $(SRC)/*.c) 
OBJS = $(patsubst %.c, %.o, $(SRCS))

# for c file in lib
LIB_SRC = $(wildcard $(LIB)/*.c) 
OBJ_LIB = $(patsubst %.c,%.o,$(LIB_SRC)) 


LIB_S__SRC = $(wildcard $(LIB)/*.S) 
OBJ_S__LIB = $(patsubst %.S,%.o,$(LIB_S__SRC)) 

# for bootloader in lib
BOOT_SRC = $(wildcard $(BOOTLOADER)/*.c)
BSRC_OBJ = $(patsubst %.c,%.o,$(BOOT_SRC)) 


BOOTASM_SRC = $(wildcard $(BOOTLOADER)/*.S)
BASM_OBJ = $(patsubst %.S,%.o,$(BOOTASM_SRC)) 


# BASM_OBJ = $(patsubst %.S,%.o,$(BOOTASM_SRC)) 

$(LIB)/%.o: $(LIB)/%.c
	$(CC)-gcc $(CFLAGS) -g -c $< -o $@

$(SRC)/%.o: $(SRC)/%.c
	$(CC)-gcc $(CFLAGS)  -g -c $< -o $@

$(LIB)/%.o: $(LIB)/%.S
	$(CC)-gcc  -g -c $< -o $@

%.o: %.S
	aarch64-linux-gnu-gcc -g -o $@ -c $<

$(SRC)/%.o: $(SRC)/%.S
	$(CC)-gcc -g -c $< -o $@

$(BOOTLOADER)/%.o: $(BOOTLOADER)/%.c
	$(CC)-gcc $(CFLAGS) -c $< -o $@

$(BOOTLOADER)/%.o: $(BOOTLOADER)/%.S
	$(CC)-gcc  -c $< -o $@

output/user.img : $(USR_OBJS)
	aarch64-linux-gnu-ld $(USR_OBJS) -T user_prog/linker.ld -o output/user.elf 
	aarch64-linux-gnu-objcopy -O binary output/user.elf $@
	cp $@ rootfs/ 
output/kernel8.img: $(ASM_OBJS) $(OBJS) $(OBJ_LIB) $(OBJ_S__LIB)
	aarch64-linux-gnu-ld $(ASM_OBJS) $(OBJS) $(OBJ_LIB) $(OBJ_S__LIB)-T src/linker.ld -o output/kernel8.elf 
	${CC}-objcopy -O binary output/kernel8.elf output/kernel8.img

output/bootloader.img: $(BASM_OBJ) $(BSRC_OBJ) $(OBJ_LIB)
	aarch64-linux-gnu-ld $(BASM_OBJ) $(BSRC_OBJ) $(OBJ_LIB)  -T $(BOOTLOADER)/linker.ld -o output/bootloader.elf 
	${CC}-objcopy -O binary output/bootloader.elf output/bootloader.img




image : 
	make output/kernel8.img
	make output/user.img

all-image:
	make clean 
	make output/kernel8.img
	make output/user.img
run:
	qemu-system-aarch64 -M raspi3b -kernel output/kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb  
run-gdb:
	qemu-system-aarch64 -M raspi3b -kernel output/kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb  -S -s
cpio:
	cd rootfs
	find . | cpio -o -H newc > ../initramfs.cpio
	cd ..


clean :
	rm -rf $(LIB)/*.o 
	rm -rf $(SRC)/*.o 
	rm -rf $(BOOTLOADER)/*o 
	rm -rf output/*.elf
	rm -rf output/*.img
 
