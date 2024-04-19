#include "../include/cpio.h"
#include "../include/lib.h"
#include "../include/mailbox.h"
#include "../include/uart.h"
#include "../include/dtb.h"
#include "../include/global.h"
#include "../include/my_malloc.h"

#define FRAME_BINS 8 // 8 | 4
#define CACHE_BINS 8
#include <stdint.h>
#include <sys/types.h>
void hello();
void help();
void reset(int tick);
void info();
extern char *initrd_ptr, *initramfs_end;
extern char __image_end;
#define initrdptr 0x20000000
extern void from_el1_to_el0();
extern char* ptr, *temp;
extern char __simple_malloc_end;
static struct arglist
{
  char *arg;
  struct arglist *next;
};



void main(void *dtb_addr)
{

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
  uart_init();
  // arm_core_timer_enable();
  //fdtb_parse(dtb_addr, 1, get_initrd);
  // while(1){
  //   uart_puts("12345");
  // }
  uart_printf("\r\nWelcome to Lab1!!\r\n");
  // uart_printf("image end: %p\n",&__image_end);

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
        }else if(strncmp(arglist[0], "malloc",6) ==0){
               char* malloc =  simple_malloc(20); 
        uart_printf("start address : %X\n",temp);  
        uart_printf("end address : %X\n",ptr);  
        }
        else if (strncmp(arglist[0], "info", 4) == 0 && readpoint == 4)
        {
        info();
        }
        else if (strncmp(arglist[0], "ls", 2) == 0)
        {
          // uart_printf("0x%08X", (uintptr_t)initrd_ptr);
          cpio_list_file((char*)initrdptr);
        }else if(strncmp(arglist[0], "cat", 3) ==0)
        {
          cpio_cat((char*)initrdptr, arglist[1]);
        }
        else if (strncmp(arglist[0], "dtb_parse", 9) == 0)
        { 
          uart_puts("dtb");
          fdtb_parse(dtb_addr, 1, NULL);
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
  uint64_t iss;
  asm volatile("mov %0, x8" : "=r"(iss));
  uart_printf("iss: %x\n\n", iss);
  uart_printf("spsr_el1 = 0x%08lX, elr_el1 = 0x%08lX, esr_el1 = 0x%08lX, cause = %lu\r\n", spsr_el1, elr_el1, esr_el1, cause);
}

void c_irq_handler_timer(uint64_t ticks, uint64_t freq)
{	
  uart_printf("ticks=%ld, freq=%ld, time elapsed=%ldms\r\n", ticks, freq, (ticks * 1000) / freq);
}
