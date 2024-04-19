#include "../include/command.h"
#include "../include/lib.h"
#include "../include/my_sscanf.h"
#include "../include/uart.h"
#include <stdint.h>
#define ADDR_IMAGE_START 0x80000

extern uint64_t __start__;
static void load_kernel_uart(void *arg_to_main);
void boot_main(void *dtb_addr) {

  uart_init();

  uart_puts("\r\n\r\n");
  uart_puts("Welcome Lab2 >> Bootloader");
  uart_printf("After relocation: __start__=0x%p, main=0x%p, dtb_addr=%p\r\n",
              &__start__, boot_main, dtb_addr);

  int buffer_counter = 0;
  char input_char;
  char buffer[128];
  for (int i = 0; i < 128; i++) {
    buffer[i] = 0;
  }
  uart_puts("# ");
  while (1) {
    /* code */

    // uart_puts("#");
    input_char = uart_getc();

    if (input_char == 8 || input_char == 127) {
      // uart_puts("get");
      if ((buffer_counter) > 0) {
        (buffer_counter)--;
        buffer[(buffer_counter)] = '\0';

        // for (int i = 0; i < readpoint; i++) {
        //   s[i] = s[i + 1];
        // }

        for (int l = buffer_counter; l < 100; l++) {
          buffer[l] = 0;
        }
        uart_puts("\n# ");
        uart_puts(buffer);
      }
    } else if (input_char == '\n' || input_char == '\r') {
      /* code */
      uart_send(input_char);
      if (buffer_counter != 100) {
        buffer[buffer_counter] = '\0';
        if (strncmp(buffer, "help", 4) == 0 && buffer_counter == 4) {
          help();
        } else if (strncmp(buffer, "hello", 5) == 0 && buffer_counter == 5) {
          hello();
        } else if (strncmp(buffer, "reboot", 6) == 0 && buffer_counter == 6) {
          // uart_puts("\r");
          reset(100);
          // uart_puts("Welcome~");
        } else if (strncmp(buffer, "info", 4) == 0 && buffer_counter == 4) {
          info();
        } else if (strncmp(buffer, "reload", 6) == 0) {
          load_kernel_uart(dtb_addr);
        } else {
          uart_puts("No such command !! \r\n");
        }
      } else {
        uart_puts("Over the buffer length!");
      }

      buffer_counter = 0;
      for (int k = 0; k < 100; k++) {
        buffer[k] = 0;
      }
      uart_puts("# ");

    } else {

      uart_send(input_char);

      if ((buffer_counter) < 100) {
        buffer[buffer_counter] = input_char;
        buffer_counter++;
      }
    }
  }
}

static void load_kernel_uart(void *arg_to_main) {
  char input_s[5];
  uint64_t bytes_to_print = 0;
  uint8_t *addr_kernel = (uint8_t *)ADDR_IMAGE_START;
  uint64_t image_size = 0;

  for (int i = 0; i < 5; i++) {
    input_s[i] = 0;
  }
  // Read size
  unsigned int kernel_size = 0;
  for (int i = 0; i < 4; i++) {
    kernel_size <<= 8;
    kernel_size |= uart_getc();
  }

  uart_printf("size : %d\n", kernel_size);
  // uart_printf("Receiving %s bytes...\n", (void *)input_s);
  // sscanf_(input_s, "%lu", image_size);
  // uart_printf("size: %lu", image_size);
  if (kernel_size == 0) {
    uart_printf("Error, image size cannot be 0\r\n");
    return;
  }
  // Read binary from uart
  else {

    for (int i = 0; i < kernel_size; i++) {
      // uart_puts("Transfer...\n");
      addr_kernel[i] = uart_getc();
    }
    bytes_to_print = (kernel_size > 20) ? (uint64_t)20 : (uint64_t)kernel_size;
    uart_printf("%d of bytes received from uart. \n", kernel_size);
    uart_printf("First %d bytes received (HEX): \n", bytes_to_print);
    for (uint64_t i = 0; i < bytes_to_print; i++)
      uart_printf("%02X ", addr_kernel[i]);
    uart_printf("\r\n");

    // Jump to new kernel
    volatile void (*jump_to_new_kernel)(void *) =
        (volatile void (*)(void *))(addr_kernel);
    jump_to_new_kernel(arg_to_main);

    while (1)
      ;
  }
}