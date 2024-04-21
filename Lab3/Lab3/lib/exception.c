
#include "../include/exeception.h"
#include "../include/global.h"
#include "../include/interrupt.h"

int used_concurrent=0;
void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }
void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void default_handler() {
  disable_interrupt();
  unsigned long spsr = read_sysreg(spsr_el1);
  unsigned long elr = read_sysreg(elr_el1);
  unsigned long esr = read_sysreg(esr_el1);
  uart_puts("-----default handler-----\n");
  uart_printf("spsr_el1: %x\n", spsr);
  uart_printf("elr_el1: %x\n", elr);
  uart_printf("esr_el1: %x\n\n", esr);
  enable_interrupt();
}

void lower_sync_handler(uint64_t spsr_el1, uint64_t elr_el1, uint64_t esr_el1,
                        uint64_t cause) {
  uart_puts("-----lower_sync_handler-----\n");

  uart_puts("spsr el1 :");
  uart_2hex(spsr_el1);
  uart_puts("\n");
  uart_puts("elr_el1:");
  uart_2hex(elr_el1);
  uart_puts("\n");
  uart_puts("esr_el1:");
  uart_2hex(esr_el1);
  uart_puts("\n");
  // uart_printf("spsr_el1: %x\n", spsr_el1);
  // uart_printf("elr_el1: %x\n", elr_el1);
  // uart_printf("esr_el1: %x\n\n", esr_el1);
}

void uart_print() {
  disable_interrupt();
  uart_puts("hello");
  enable_interrupt();
}

void irq_uart_handler() {

  //;
  disable_interrupt();
  // uart_puts("irq\n");
  if (*IRQ_PENDING_1 & IRQ_PENDING_1_AUX_INT) {

    // uart_puts("in while");
    if (*AUX_MU_IIR_REG & 0x2) {
      *DISABLE_IRQS_1 = (1 << 29);
      if(used_concurrent==0){
        disable_uart_tx_interrupt();
        uart_tx_interrupt_handler();
      }else{
        
      disable_uart_tx_interrupt();
      task_queue_add(uart_tx_interrupt_handler, uart_priority);
      }

      *ENABLE_IRQS_1 = (1 << 29);
    } else if (*AUX_MU_IIR_REG & (0x4)) {
      // uart_puts("rx\n");
      *DISABLE_IRQS_1 = (1 << 29);
      // uart_rx_interrupt_handler();
      if(used_concurrent==0){
        disable_uart_rx_interrupt();
        uart_rx_interrupt_handler();
      }else{
        task_queue_add(uart_rx_interrupt_handler, uart_priority);
      }

      *ENABLE_IRQS_1 = (1 << 29);
    }
  } else if (*CORE0_INT_SRC & 0x2) {
    *DISABLE_IRQS_1 = (1 << 29);
    if(used_concurrent==0){
      timer_handler();
    }else{

      task_queue_add(timer_handler, timer_priority);
    }
    *ENABLE_IRQS_1 = (1 << 29);
  } else {
    uart_printf("uart interrupt error\n");
  }
  if(used_concurrent){
  exec_task();
  }
  enable_interrupt();
}
