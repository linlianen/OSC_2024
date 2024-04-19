#include "../include/syscall.h"
#include "../include/exeception.h"
#include "../include/interrupt.h"
#include "../include/lib.h"
#include "../include/cpio.h"
#include "../include/mailbox.h"
#include "../include/mm.h"
#include "../include/my_malloc.h"
#include "../include/mailbox.h"
#include "../include/thread.h"
#include "../include/timer.h"
#include "../include/uart.h"
#include <stdint.h>

extern thread *currThread;

static void child_function()
{
  uart_puts("its child function\n");
  int num = 10000;
  while (num--)
  {
    asm volatile("nop");
  }
}

void sys_fork(tf *trapFrame)
{

  disable_interrupt();
  thread *curr = current_thread();
  thread *child_thread = create_thread(curr->thread_data);
  int parent_pid = curr->tid;
  thread *parent_thread = curr;

  child_thread->thread_size = parent_thread->thread_size;

  child_thread->thread_data = kalloc(parent_thread->thread_size * sizeof(char));
  memcpy_diy(child_thread->thread_data, curr->thread_data, curr->thread_size);

  child_thread->user_stack = kalloc(4096);

  child_thread->kernel_stack = kalloc(4096);
  child_thread->context.sp = (unsigned long)(child_thread->kernel_stack + THREAD_SIZE);
  child_thread->context.fp = child_thread->context.sp;
  memcpy_diy(child_thread->user_stack, curr->user_stack, 4096);
  memcpy_diy(child_thread->kernel_stack, curr->kernel_stack, 4096);

  set_current(child_thread);
  // schedule

  // switch child thread
  if (parent_pid == currThread->tid)
  {

    store_context(get_current());

    child_thread->context = curr->context;

    child_thread->context.sp +=
      (child_thread->kernel_stack - curr->kernel_stack);
    child_thread->context.fp +=
      (child_thread->kernel_stack - curr->kernel_stack);
    enable_interrupt();

    trapFrame->x0 = child_thread->tid;
    asm volatile("str %[input0], [sp]" ::[input0] "r"(trapFrame->x0));
  }
  else
{

    trapFrame = (tf *)((char *)trapFrame + (unsigned long)curr->kernel_stack -
      (unsigned long)child_thread->kernel_stack);
    trapFrame->sp_el0 += curr->user_stack - child_thread->user_stack;
    enable_interrupt();
    trapFrame->x0 = 0;
    // asm volatile("sub sp, sp, 16*19");
    // asm volatile("str %[input0], [sp]"::[input0]"r"(trapFrame->x0));
    // asm volatile("stp %[input0], %[input1], [sp,16*0]"::
    // [input0]"r"(trapFrame->x0),[input1]"r"(trapFrame->x1):);
  }
}

/*

    case 3:
    //exec
      break;


    case 5:
    //exit
      break;

    case 7:
    //kill
      break;
*/
size_t sys_uart_read(char buf[], size_t size)
{

  char c;
  size_t idx = 0;
  disable_interrupt();
  while (idx < size)
  {
    if (buf[idx] == NULL)
    {
      buf[idx] = uart_get_c_async();
    }
    else
  {
      idx++;
    }
  }
  enable_interrupt();
  return idx;
}

size_t sys_uart_write(char buf[], size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    /* code */
    uart_put_c_async(buf[i]);
  }
  return size;
}
int sys_exec(const char *name, char *const argv[])
{ 
  char*cpio_buf;
  char *exe_format = ".img";
  char *verify_format = strstr(name, exe_format);
  void *prog_addr = findFile(name, cpio_buf);

  struct cpio_newc_header *header = (struct cpio_newc_header *)prog_addr;

  unsigned int pathname_size = hex_to_int(header->c_namesize, 8);
  uint32_t file_size = hex_to_int(header->c_filesize, 8);

  uint32_t headerPathName_size = sizeof(struct cpio_newc_header) + pathname_size;

  align(&headerPathName_size, 4);
  align(&file_size, 4);

  disable_interrupt();

  void *prog_malloc = kalloc(file_size + 4096);

  for(int i=0; i<file_size; i++){

    ((char*)prog_malloc)[i] = ((char*)prog_addr)[i];

  }

  thread *video_thread = create_thread(prog_malloc);
  
  video_thread->thread_size = file_size;
  video_thread->thread_data = prog_malloc;
  video_thread->user_stack = kalloc(4096);

  video_thread->kernel_stack = kalloc(4096);
  video_thread->context.sp = (unsigned long)(video_thread->kernel_stack + THREAD_SIZE);
  video_thread->context.fp = video_thread->context.sp;
  memcpy_diy(video_thread->user_stack, currThread->user_stack, 4096);
  memcpy_diy(video_thread->kernel_stack, currThread->kernel_stack, 4096);
  unsigned long long tmp;
  asm volatile("mrs %0, cntkctl_el1" : "=r"(tmp));
  tmp |= 1;
  asm volatile("msr cntkctl_el1, %0" : : "r"(tmp));
  
  enable_interrupt();
  return 0;

}

void sys_exit() {}

int sys_sub_mbox_call(unsigned char ch, unsigned int *mbox)
{
  uint32_t temp =
    ((((uint32_t)((uint64_t)mbox)) & ~0xF) // & ~0xF clears lower 4 bits
    | (ch & 0xF) // & 0x0000000F clears upper 28 bits
  );            // 28 bits(MSB) for value, 4 bits for the channel

  // Wait until mailbox is not full (busy)
  while (*MBOX_STATUS & MBOX_FULL)
    ;

  // Write buffer address to mobx_write
  *MBOX_WRITE = temp;

  // Wait while it's empty
  while (*MBOX_STATUS & MBOX_EMPTY)
    ;

  // Check if the value is the same as the one wrote into MBOX_WRITE
  if (*MBOX_READ == temp)
    /* is it a valid successful response? */
    return mbox[1] == MBOX_RESPONSE;
  else
    return 0;
}

void sys_kill(int pid)
{
}
