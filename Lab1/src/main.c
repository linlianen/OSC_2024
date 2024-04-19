#include "../include/lib.h"
#include "../include/mailbox.h"
#include "../include/uart.h"
#include <stdint.h>
#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024

void hello();
void help();
void reset(int tick);
void info();
void main() {
  uart_init();
  char s[100];
  int readpoint = 0;
  for (int idx = 0; idx < 100; idx++) {
    s[idx] = 0;
  }

  uart_puts("# ");
  while (1) {
    /* code */

    // uart_puts("#");
    char c = uart_getc();

    if (c == 8 || c == 127) {
      // uart_puts("get");
      if ((readpoint) > 0) {
        (readpoint)--;
        s[(readpoint)] = '\0';

        // for (int i = 0; i < readpoint; i++) {
        //   s[i] = s[i + 1];
        // }

        for (int l = readpoint; l < 100; l++) {
          s[l] = 0;
        }
        uart_puts("\n# ");
        uart_puts(s);
      }
    } else if (c == '\n' || c == '\r') {
      /* code */
      uart_send(c);
      if (readpoint != 100) {
        s[readpoint] = '\0';
        if (strncmp(s, "help", 4) == 0 && readpoint == 4) {
          help();
        } else if (strncmp(s, "hello", 5) == 0 && readpoint == 5) {
          hello();
        } else if (strncmp(s, "reboot", 6) == 0 && readpoint == 6) {
          // uart_puts("\r");
          reset(100);
          // uart_puts("Welcome~");
        } else if (strncmp(s, "info", 4) == 0 && readpoint == 4) {
          info();
        } else {
          uart_puts("No such command !! \r\n");
        }
      } else {
        uart_puts("Over the buffer length!");
      }

      readpoint = 0;
      for (int k = 0; k < 100; k++) {
        s[k] = 0;
      }
      uart_puts("# ");

    } else {

      uart_send(c);

      if ((readpoint) < 100) {
        s[readpoint] = c;
        readpoint++;
      }
    }
  }
}

void help() {
  uart_puts("\r---shell guide---\r\n");
  uart_puts("help: print this help menu\r\n");
  uart_puts("hello: print hello world\r\n");
  uart_puts("reboot: reboot the os \r\n");
  uart_puts("info: use mailbox\r\n");
}

void set(long addr, unsigned int value) {
  volatile unsigned int *point = (unsigned int *)addr;
  *point = value;
}

void reset(int tick) {
  uart_puts("\rReseting\n");            // reboot after watchdog timer expire
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
