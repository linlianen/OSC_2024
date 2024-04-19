#include "../include/uart.h"

#define physical_add 0x3F000000

// initialization
#define AUXENB ((volatile unsigned int *)(physical_add + 0x00215004))
#define AUX_MU_CNTL_REG ((volatile unsigned int *)(physical_add + 0x00215060))
#define AUX_MU_IER_REG ((volatile unsigned int *)(physical_add + 0x00215044))
#define AUX_MU_LCR_REG ((volatile unsigned int *)(physical_add + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int *)(physical_add + 0x00215050))
#define AUX_MU_BAUD ((volatile unsigned int *)(physical_add + 0x00215068))
#define AUX_MU_IIR_REG ((volatile unsigned int *)(physical_add + 0x00215048))

// Read,write data
#define AUX_MU_LSR_REG ((volatile unsigned int *)(physical_add + 0x00215054))
#define AUX_MU_IO_REG ((volatile unsigned int *)(physical_add + 0x00215040))

#define AUX_MU_MSR ((volatile unsigned int *)(physical_add + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int *)(physical_add + 0x0021505C))
#define AUX_MU_STAT ((volatile unsigned int *)(physical_add + 0x00215064))

#define GPFSEL0 ((volatile unsigned int *)(physical_add + 0x00200000))
#define GPFSEL1 ((volatile unsigned int *)(physical_add + 0x00200004))
#define GPFSEL2 ((volatile unsigned int *)(physical_add + 0x00200008))
#define GPFSEL3 ((volatile unsigned int *)(physical_add + 0x0020000C))
#define GPFSEL4 ((volatile unsigned int *)(physical_add + 0x00200010))
#define GPFSEL5 ((volatile unsigned int *)(physical_add + 0x00200014))
#define GPSET0 ((volatile unsigned int *)(physical_add + 0x0020001C))
#define GPSET1 ((volatile unsigned int *)(physical_add + 0x00200020))
#define GPCLR0 ((volatile unsigned int *)(physical_add + 0x00200028))
#define GPLEV0 ((volatile unsigned int *)(physical_add + 0x00200034))
#define GPLEV1 ((volatile unsigned int *)(physical_add + 0x00200038))
#define GPEDS0 ((volatile unsigned int *)(physical_add + 0x00200040))
#define GPEDS1 ((volatile unsigned int *)(physical_add + 0x00200044))
#define GPHEN0 ((volatile unsigned int *)(physical_add + 0x00200064))
#define GPHEN1 ((volatile unsigned int *)(physical_add + 0x00200068))
#define GPPUD ((volatile unsigned int *)(physical_add + 0x00200094))
#define GPPUDCLK0 ((volatile unsigned int *)(physical_add + 0x00200098))
#define GPPUDCLK1 ((volatile unsigned int *)(physical_add + 0x0020009C))

void uart_init() {
  register unsigned int r;

  /* initialize UART */
  *AUXENB |= 1; // enable UART1, AUX mini uart
  *AUX_MU_CNTL_REG = 0;
  *AUX_MU_LCR_REG = 3; // 8 bits
  *AUX_MU_MCR_REG = 0;
  *AUX_MU_IER_REG = 0;
  *AUX_MU_IIR_REG = 0xc6; // disable interrupts
  *AUX_MU_BAUD = 270;     // 115200 baud
  /* map UART1 to GPIO pins */
  r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
  r |= (2 << 12) | (2 << 15);    // alt5
  *GPFSEL1 = r;
  *GPPUD = 0; // enable pins 14 and 15
  r = 150;
  while (r--) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  r = 150;
  while (r--) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = 0;       // flush GPIO setup
  *AUX_MU_CNTL_REG = 3; // enable Tx, Rx
}

/*
Send a character
*/
void uart_send(unsigned int c) {
  do {
    asm volatile("nop");
    /* code */
  } while (!(*AUX_MU_LSR_REG & 0x20));

  *AUX_MU_IO_REG = c;
}

char uart_getc() {
  char r;
  do {
    asm volatile("nop");
  } while (!(*AUX_MU_LSR_REG & 0x01));

  r = (char)(*AUX_MU_IO_REG);

  return r == '\r' ? '\n' : r;
}

void uart_puts(char *s) {
  while (*s) {
    /* code */
    if (*s == '\n') {
      uart_send('\r'); /* code */
    }
    uart_send(*s++);
  }
}

void utils_uint2str_hex(unsigned int num, char *str) {
  // num=7114 digit=4
  int counter = 0;
//   uart_puts("test");
  unsigned int temp = num;
  int digit = -1;
  str[counter] = '0';
//   uart_puts("test");
  counter++;
  str[counter] = 'x';
  counter++;
  if (num == 0) {
    str[counter] = '0';
    counter++;
  } else {
    while (temp > 0) {
      temp /= 16;
      digit++;
    }
    for (int i = digit; i >= 0; i--) {
      int t = 1;
      for (int j = 0; j < i; j++) {
        t *= 16;
      }
      if (num / t >= 10) {
        *str = '0' + num / t + 39;
      } else {
        *str = '0' + num / t;
      }
      num = num % t;
      str++;
    }
  }

  *str = '\0';
}