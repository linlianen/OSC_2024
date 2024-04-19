#include <stdint.h>
#include "gpio.h"
#define physical_add 0x3F000000
#define MAX_BUF_SIZE 128

// initialization
#define AUXENB ((volatile unsigned int *)(physical_add + 0x00215004))
#define AUX_MU_CNTL_REG ((volatile unsigned int *)(physical_add + 0x00215060))
#define AUX_MU_IER_REG ((volatile unsigned int *)(physical_add + 0x00215044))
#define AUX_MU_LCR_REG ((volatile unsigned int *)(physical_add + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int *)(physical_add + 0x00215050))
#define AUX_MU_BAUD ((volatile unsigned int *)(physical_add + 0x00215068))
#define AUX_MU_IIR_REG ((volatile unsigned int *)(physical_add + 0x00215048))

// Read,write data
#define AUX_MU_IO_REG ((volatile unsigned int *)(physical_add + 0x00215040))
#define AUX_MU_IER_REG ((volatile unsigned int *)(physical_add + 0x00215044))
#define AUX_MU_IIR_REG ((volatile unsigned int *)(physical_add + 0x00215048))
#define AUX_MU_LCR_REG ((volatile unsigned int *)(physical_add + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int *)(physical_add + 0x00215050))
#define AUX_MU_LSR_REG ((volatile unsigned int *)(physical_add + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int *)(physical_add + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int *)(physical_add + 0x0021505C))
#define AUX_MU_STAT ((volatile unsigned int *)(physical_add + 0x00215064))

void uart_init();
void uart_putsn(char *s, int n) ;
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void utils_uint2str_hex(unsigned int num, char *str);
void uart_printf(const char *fmt, ...);
int uart_gets_n(int n, char *str, int echo);
uint8_t uart_read_byte();
void init_uart_async_index();
void enable_uart_tx_interrupt();
void disable_uart_tx_interrupt();
void enable_uart_rx_interrupt();
void disable_uart_rx_interrupt();
void uart_rx_interrupt_handler();
void uart_tx_interrupt_handler();
char uart_get_c_async();
void uart_put_c_async(char c);
void uart_handler_async();




