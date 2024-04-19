#include "../include/cpio.h"
#include "../include/lib.h"
#include "../include/mailbox.h"
#include "../include/mm.h"
#include "../include/timer.h"
#include "../include/thread.h"
#include "../include/uart.h"
#include "../include/dtb.h"
#include "../include/exeception.h"
#include "../include/syscall.h"
#include "../include/global.h"

#define FRAME_BINS 8 // 8 | 4
#define CACHE_BINS 8
#include <stdint.h>
#include <sys/types.h>
void hello();
void help();
void reset(int tick);
void info();
extern struct page_head alloc_mem[FRAME_BINS];
extern struct chunk_head *chunk_mem[CACHE_BINS];
extern char *initrd_ptr, *initramfs_end;
extern char __image_end;
extern void from_el1_to_el0();


static struct arglist
{
  char *arg;
  struct arglist *next;
};


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


  // arm_core_timer_enable();
  fdtb_parse(dtb_addr, 0, get_initrd);
  uart_printf("initrd start: %p, initrd end: %p\n",initrd_ptr, initramfs_end);
  uart_printf("image end: %p\n",&__image_end);
  mm_init(&alloc_mem, chunk_mem);
  init_thread_state();
  enable_interrupt();
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
          cpio_list_file(initrd_ptr);
        }else if(strncmp(arglist[0], "cat", 3) ==0)
        {
          cpio_cat(initrd_ptr, arglist[1]);
        }
        else if (strncmp(arglist[0], "dtb_parse", 9) == 0)
        { 
          uart_puts("dtb");
          fdtb_parse(dtb_addr, 1, NULL);
        }
        else if (strncmp(arglist[0], "to_el0", 6) == 0)
        {
          cpio_load_prog("user.img", initrd_ptr);
        }
        else if (strncmp(arglist[0], "uart_async", 10) == 0)
        {
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
        }else if(strncmp(arglist[0], "thread",6)==0)
        {
          kernel_main();
        }else if(strncmp(arglist[0], "buddy_alloc",11)==0)
        {
          test_buddy();
        }else if(strncmp(arglist[0], "dynamic_alloc",13)==0)
        {
          test_dynamic_alloc();
        }
        else if(strncmp(arglist[0], "fork", 4)==0){
        
          uart_printf("\nFork Test, pid %d\n", getpid());
          int cnt = 1;
          int ret = 0;
          if ((ret = fork()) == 0) { // child
            long long cur_sp;
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            uart_printf("first child pid: %d, cnt: %d, ptr: %x, sp : %x\n", getpid(), cnt, &cnt, cur_sp);
            ++cnt;

            if ((ret = fork()) != 0){
              asm volatile("mov %0, sp" : "=r"(cur_sp));
              uart_printf("first child pid: %d, cnt: %d, ptr: %x, sp : %x\n", getpid(), cnt, &cnt, cur_sp);
            }
            else{
              while (cnt < 5) {
                asm volatile("mov %0, sp" : "=r"(cur_sp));
                uart_printf("second child pid: %d, cnt: %d, ptr: %x, sp : %x\n", getpid(), cnt, &cnt, cur_sp);
                delay(1000000);
                ++cnt;
              }
            }
            sysexit();
          }
          else {
            uart_printf("parent here, pid %d, child %d\n", getpid(), ret);
          }
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
