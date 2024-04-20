#include "../include/command.h"
#include "../include/mailbox.h"
#include "../include/uart.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024

void help() {
	uart_puts("\r---shell guide---\r\n");
	uart_puts("help: print this help menu\r\n");
	uart_puts("hello: print hello world\r\n");
	uart_puts("reboot: reboot the os \r\n");
	uart_puts("info: use mailbox\r\n");
	uart_puts("ls: list filename in initramfs.cpio\r\n");
	uart_puts("dtb-parse: traverse device node.\r\n");
}

void set(long addr, unsigned int value) {
	volatile unsigned int *point = (unsigned int *)addr;
	*point = value;
}

void reset(int tick) {
	uart_puts("\rReseting\n");        // reboot after watchdog timer expire
	set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
	while (1)
		;
}

void info() {
	uint32_t board_vision = 0;
	uint32_t size = 0;
	uint32_t *base_addr = 0;
	char board_str[256] = {0};
	char mem_size[256] = {0};
	char base_addr_str[256] = {0};
	// uart_puts("avd");
	mbox_board_rev(&board_vision);
	// uart_puts("avs");

	mbox_arm_mem_info(&base_addr, &size);
	// uart_puts("avl");

	utils_uint2str_hex(board_vision, board_str);
	utils_uint2str_hex(size, mem_size);
	utils_uint2str_hex(*base_addr, base_addr_str);
	uart_puts("\rBoard vision : ");
	uart_puts(board_str);
	uart_puts("\n");
	uart_puts("Mem size :");

	uart_puts(mem_size);
	uart_puts("\n");
	uart_puts("Board address :");

	uart_puts(base_addr_str);
	uart_puts("\n");
}

void cancel_reset() {
	set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}

void hello() { uart_puts("\rHello World\r\n"); }
