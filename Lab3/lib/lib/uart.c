#include "../include/uart.h"
#include "../include/exeception.h"
#include "../include/global.h"
#include "../include/gpio.h"
#include "../include/irq.h"
#include "../include/my_printf.h"

char tx_buffer[MAX_BUF_SIZE];
char rx_buffer[MAX_BUF_SIZE];

unsigned int tx_bf_r_idx;
unsigned int tx_bf_w_idx;
unsigned int rx_bf_r_idx;
unsigned int rx_bf_w_idx;

void init_uart_async_index() {
  tx_bf_r_idx = 0;
  tx_bf_w_idx = 0;
  rx_bf_r_idx = 0;
  rx_bf_w_idx = 0;
}

void enable_uart_tx_interrupt() { *AUX_MU_IER_REG |= 2; }

void disable_uart_tx_interrupt() { *AUX_MU_IER_REG &= ~(2); }

void enable_uart_rx_interrupt() { *AUX_MU_IER_REG |= 1; }

void disable_uart_rx_interrupt() { *AUX_MU_IER_REG &= ~(1); }
void uart_2hex(unsigned int d) {
  unsigned int n;
  int c;
  for (c = 28; c >= 0; c -= 4) {
    n = (d >> c) & 0xF;
    n += n > 9 ? 0x37 : 0x30;
    uart_send(n);
  }
}
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
  init_uart_async_index();
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

void uart_printf(const char *fmt, ...) {
  int len = 0;
  char buf[128];
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  len = vsnprintf_(buf, sizeof(buf), fmt, args);
  if (len > 0) {
    uart_puts(buf);
  }
  __builtin_va_end(args);
}

void l2s_r(char **p, long int i) {

  char d = (i % 10) + '0';
  if (i >= 10)
    l2s_r(p, i / 10);
  *((*p)++) = d;
}

void uart_puti(unsigned int i) {
  static char buf[24];
  char *p = buf;
  if (i < 0) {
    *p++ = '-';
    i = 0 - i;
  }
  l2s_r(&p, i);
  *p = '\0';
  uart_puts(buf);
  return;
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

int uart_gets_n(int n, char *str, int echo) {

  char temp;
  int index = 0;
  while (index < n) {

    // check for maximum length
    if (n == 0) {
      str[0] = '\0';
      return 1;
    } else {
      uart_puts("123");
      temp = uart_getc();
      if (temp == '\b' && index > 0) {
        index--;
        str[index] = '\0';
        return -1;
      } else if (temp != '\b' && index > 0) {
        if (echo) {
          uart_send(temp);
        }
        str[index] = temp;
        index++;
      } else if (temp == '\n') {
        str[index] = '\0';
        return index + 1;
      }
    }
  }
}

uint8_t uart_read_byte() {
  uint8_t r;
  // wait until something is in the buffer
  while (!(*AUX_MU_IO_REG))
    ;

  // read it and return
  r = (char)(*AUX_MU_IO_REG);

  // convert carriage return to newline
  r = (r == '\r' ? '\n' : r);
  return r;
}

// read from computer to kernel
void uart_rx_interrupt_handler() {
  // 若buffer滿了，即暫停interrupt handler，就不能收資料。
  if ((rx_bf_w_idx + 1) % MAX_BUF_SIZE == rx_bf_r_idx) {
    disable_uart_rx_interrupt();
    return;
  }

  disable_interrupt();
  rx_buffer[rx_bf_w_idx] = uart_getc();
  rx_bf_w_idx++;
  rx_bf_w_idx %= MAX_BUF_SIZE;
  enable_interrupt();
}

// write from raspi to kernel
void uart_tx_interrupt_handler() {
  if (tx_bf_r_idx == tx_bf_w_idx) {
    disable_uart_tx_interrupt();
    return;
  }

  *(AUX_MU_IO_REG) = (unsigned int)tx_buffer[tx_bf_r_idx];
  tx_bf_r_idx++;
  tx_bf_r_idx %= MAX_BUF_SIZE;

  enable_uart_tx_interrupt();
}

// read from read buffer
char uart_get_c_async() {
  enable_uart_rx_interrupt();

  // 開啟後，即把interrupt
  // handler打開，handler會開始探訪，這邊意思是我們還沒有值進來
  if (rx_bf_r_idx == rx_bf_w_idx) {
    asm volatile("nop");
    return 0;
  }

  char c = rx_buffer[rx_bf_r_idx];
  rx_bf_r_idx++;
  rx_bf_r_idx %= MAX_BUF_SIZE;

  return c;
}

// write in write buffer
void uart_put_c_async(char c) {
  while ((tx_bf_w_idx + 1) % MAX_BUF_SIZE == tx_bf_r_idx) {
    /* code */
    enable_uart_tx_interrupt();
  }
  disable_interrupt();
  tx_buffer[tx_bf_w_idx] = c;

  tx_bf_w_idx++;
  tx_bf_w_idx %= MAX_BUF_SIZE;
  enable_interrupt();
  enable_uart_tx_interrupt();
}

void uart_handler_async() {
  char c;
  *ENABLE_IRQS_1 = (1 << 29);
  int clock = 1500;
  while (clock--) {
    asm volatile("nop");
  }
  char buffer[128];
  size_t index = 0;
  while (1) {

    c = uart_get_c_async();

    if (c == 27)
      break;
    uart_put_c_async(c);
  }
  *DISABLE_IRQS_1 = (1 << 29);
  uart_puts("\n");
}
