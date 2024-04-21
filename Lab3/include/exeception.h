#include "../include/uart.h"
#include "../include/lib.h"
#include "../include/irq.h"
#include "../include/gpio.h"
#include "../include/timer.h"



void enable_interrupt();
void disable_interrupt();
void default_handler();
void lower_sync_handler();
void uart_print();
void exec_task();
void irq_uart_handler();






