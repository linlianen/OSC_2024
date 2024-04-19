
#include "../include/exeception.h"
#include "../include/global.h"
#include "../include/syscall.h"
#include "../include/thread.h"
#include <stddef.h>
#include <stdint.h>
void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }
void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void default_handler()
{
	disable_interrupt();
	unsigned long spsr = read_sysreg(spsr_el1);
	unsigned long elr = read_sysreg(elr_el1);
	unsigned long esr = read_sysreg(esr_el1);
	uart_puts("-----default handler-----\n");
	uart_printf("spsr_el1: %x\n", spsr);
	uart_printf("elr_el1: %x\n", elr);
	uart_printf("esr_el1: %x\n\n", esr);
	uint64_t iss;
	asm volatile("mov %0, x8" : "=r"(iss));
	uart_printf("iss: %x\n\n", iss);
	enable_interrupt();
}

void get_pid(tf *trap_frame)
{
	uint64_t task_id = current_thread()->tid;
	uart_printf("task id: %d\n", task_id);
	trap_frame->x0 = task_id;
}
void lower_sync_handler(tf *trap_frame, uint64_t elr_el1, uint64_t esr_el1)
{

	int e_cause = (esr_el1 >> 26) & 0b111111;
	int iss = esr_el1 & 0x1FFFFFF;
	if (e_cause == 0b010101)
	{
		uint64_t syscall_num = trap_frame->x8;
		switch (syscall_num)
		{
			case 0:
				get_pid(trap_frame);
				break;
			case 1:
				//uart read
				sys_uart_read((char*)trap_frame->x0, (size_t)trap_frame->x1);    
				break;
			case 2:
				//uart write
				sys_uart_write((char*)trap_frame->x0, (size_t)trap_frame->x1);    
				break;
			case 3:
				//exec
				break;

			case 4:
				sys_fork(trap_frame);

				break;
			case 5:
				//exit
				break;
			case 6:
				//int mbox_call

				sys_sub_mbox_call((unsigned char)trap_frame->x0, (unsigned int *)trap_frame->x1);
				break;
			case 7:
				//kill
				break;
		}
	}
	else
	{
	}
	enable_interrupt();
}

void uart_print()
{
	disable_interrupt();
	uart_puts("hello");
	enable_interrupt();
}

void irq_uart_handler()
{

	disable_interrupt();
	// uart_puts("irq\n");
	if (*IRQ_PENDING_1 & IRQ_PENDING_1_AUX_INT)
	{
		// uart_puts("in while");
		if (*AUX_MU_IIR_REG & 0x2)
		{
			*DISABLE_IRQS_1 = (1 << 29);
			disable_uart_tx_interrupt();
			uart_tx_interrupt_handler();
			*ENABLE_IRQS_1 = (1 << 29);
		}
		else if (*AUX_MU_IIR_REG & (0x4))
		{
			// uart_puts("rx\n");
			*DISABLE_IRQS_1 = (1 << 29);
			disable_uart_rx_interrupt();
			uart_rx_interrupt_handler();
			*ENABLE_IRQS_1 = (1 << 29);
		}
	}
	else if (*CORE0_INT_SRC & 0x2)
	{
		*DISABLE_IRQS_1 = (1 << 29);

		timer_handler();
		arm_core_timer_disable();
		*ENABLE_IRQS_1 = (1 << 29);
	}
	else
	{
		uart_printf("uart interrupt error\n");
	}
	enable_interrupt();
}
