#include "../include/cpio.h"
#include "../include/lib.h"
#include "../include/mailbox.h"
#include "../include/timer.h"
#include "../include/uart.h"
#include "../include/dtb.h"
#include "../include/interrupt.h"
#include "../include/exeception.h"
#include "../include/global.h"

#include <stdint.h>
#include <sys/types.h>
#define initrdptr 0x20000000
void hello();
void help();
void reset(int tick);
void info();
char *initrd_ptr;
extern int used_concurrent;
extern void from_el1_to_el0();
static uint32_t get_be_int(const void *ptr)
{
	const unsigned char *bytes = (unsigned char *)ptr;
	uint32_t ret = bytes[3];
	ret |= bytes[2] << 8;
	ret |= bytes[1] << 16;
	ret |= bytes[0] << 24;

	return ret;
}

static struct arglist
{
	char *arg;
	struct arglist *next;
};

void get_initrd(int type, const char *name, const void *data, uint32_t size)
{
	if (type == FDT_PROP && !my_strcmp(name, "linux,initrd-start"))
	{
		initrd_ptr = (char *)(uintptr_t)get_be_int(data);
		// uart_puts("Address");
		// uart_printf("0x%08X", (uintptr_t)get_be_int(data));
	}
}

void main(void *dtb_addr)
{
	uart_init();

	struct arglist *arg_link;
	char s[100];
	char *arglist[3];
	int readpoint = 0;
	for (int idx = 0; idx < 100; idx++)
	{
		s[idx] = 0;
	}
	uint32_t _dtb_ptr;
	char *arg;
	enable_interrupt();
  // enable_uart_tx_interrupt();	
  // enable_uart_rx_interrupt();
	arm_core_timer_enable();
	// fdtb_parse(dtb_addr, 0, get_initrd);
	uart_puts("# ");
	while (1)
	{
		/* code */

		// uart_puts("#");
		char c = uart_getc();

		if (c == 8 || c == 127)
		{
			// uart_puts("get");
			if ((readpoint) > 0)
			{
				(readpoint)--;
				s[(readpoint)] = '\0';

				// for (int i = 0; i < readpoint; i++) {
				//   s[i] = s[i + 1];
				// }

				for (int l = readpoint; l < 100; l++)
				{
					s[l] = 0;
				}
				uart_puts("\n# ");
				uart_puts(s);
			}
		}
		else if (c == '\n' || c == '\r')
		{
			/* code */
			uart_send(c);

			arg = my_strtok(s, "-");
			int i = 0;
			while (arg != NULL)
			{
				arglist[i] = arg;

				arg = my_strtok(NULL, "-");
				i++;
			}

			if (readpoint != 100)
			{
				s[readpoint] = '\0';

				if (strncmp(arglist[0], "help", 4) == 0 && readpoint == 4)
				{
					help();
				}
				else if (strncmp(arglist[0], "hello", 5) == 0 && readpoint == 5)
				{
					hello();
				}
				else if (strncmp(arglist[0], "reboot", 6) == 0 && readpoint == 6)
				{
					// uart_puts("\r");
					reset(100);
					// uart_puts("Welcome~");
				}
				else if (strncmp(arglist[0], "info", 4) == 0 && readpoint == 4)
				{
					info();
				}
				else if (strncmp(arglist[0], "ls", 2) == 0)
				{
					// uart_printf("0x%08X", (uintptr_t)initrd_ptr);
					cpio_list_file((char*)initrdptr);
				}
				else if (strncmp(arglist[0], "dtb-parse", 9) == 0)
				{
					fdtb_parse(dtb_addr, 1, NULL);
				}
				else if (strncmp(arglist[0], "to_el0", 6) == 0)
				{
					cpio_load_prog("user.img", (char*)initrdptr);
				}
				else if (strncmp(arglist[0], "uart_async", 10) == 0)
				{
					// uart_puts("1234");

					uart_handler_async();
				}
				else if (strncmp(arglist[0], "set_timer", 10) == 0)
				{

					if (arglist[1] != NULL)
					{
						set_timeout(arglist[1], arglist[2]);
					}
					// arm_core_timer_enable();
					// get_time();
				}else if(strncmp(arglist[0], "start_task_queue", 16)==0)
        {
          enable_uart_rx_interrupt();
          used_concurrent = 1;
        }
				else
				{
					uart_puts("No such command !! \r\n");
				}
			}
			else
			{
				uart_puts("Over the buffer length!");
			}

			readpoint = 0;
			for (int k = 0; k < 100; k++)
			{
				s[k] = 0;
			}

			for (int a_i = 0; a_i < 3; a_i++)
			{
				/* code */
				arglist[a_i] = 0;
			}

			uart_puts("# ");
		}
		else
		{

			uart_send(c);

			if ((readpoint) < 100)
			{
				s[readpoint] = c;
				readpoint++;
			}
		}
	}
}

void unhandled_exception(uint64_t spsr_el1, uint64_t elr_el1, uint64_t esr_el1, uint64_t cause)
{
	uart_printf("spsr_el1 = 0x%08lX, elr_el1 = 0x%08lX, esr_el1 = 0x%08lX, cause = %lu\r\n", spsr_el1, elr_el1, esr_el1, cause);
}

void c_irq_handler_timer(uint64_t ticks, uint64_t freq)
{
	uart_printf("ticks=%ld, freq=%ld, time elapsed=%ldms\r\n", ticks, freq, (ticks * 1000) / freq);
}
