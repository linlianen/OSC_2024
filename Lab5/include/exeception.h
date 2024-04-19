#include "../include/gpio.h"
#include "../include/irq.h"
#include "../include/lib.h"
#include "../include/timer.h"
#include "../include/uart.h"
#include "../include/syscall.h"
#include "../include/thread.h"
#include <stdint.h>

void enable_interrupt();
void disable_interrupt();
void default_handler();

void lower_sync_handler(tf *trap_frame, uint64_t elr_el1,
                        uint64_t esr_el1);
void uart_print();
void irq_uart_handler();
void get_pid(tf *trap_frame);
