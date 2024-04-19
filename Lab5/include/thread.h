#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdlib.h>
#include <stdint.h>
#define MAX_THREADS 1024
#define STACK_SIZE 8192
#define THREAD_SIZE 4096
#define PID_MAX 1024
/*thread context switching*/
struct thread_context {

  unsigned long x19;
  unsigned long x20;
  unsigned long x21;
  unsigned long x22;
  unsigned long x23;
  unsigned long x24;
unsigned long x25;
  unsigned long x26;
  unsigned long x27;
  unsigned long x28;
  unsigned long fp;
  unsigned long lr;
  unsigned long sp;

};

void thread_exit();
typedef struct trapFrame 
{


  // General-Purpose Registers
  uint64_t x0;
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x14;
  uint64_t x15;
  uint64_t x16;
  uint64_t x17;
  uint64_t x18;
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t x29;
  uint64_t x30;
  uint64_t notuse;
  // Special Register
  uint64_t sp_el0;
  uint64_t notuse_1;
  uint64_t spsr_el1;
  uint64_t notuse_2;
  uint64_t elr_el1;
}tf;

/*thread state*/
typedef enum {
  THREAD_DELETED,
  THREAD_CREATED,
  THREAD_RUNNING,
  THREAD_WAITING
} thread_state;

typedef struct thread_node {
  int is_waiting;
  int is_running;
  uint64_t program_counter;
  uint64_t thread_size;
  void* thread_data;
  int is_zombie;
  uint64_t tid;
  void* kernel_stack;
  void* user_stack;
  uint64_t priority;
  struct thread_node *next;
  struct thread_node *prev;
  struct thread_context context;
  tf*  trapframe;
} thread;


#define INIT_THREAD                         \
{THREAD_RUNNING, 0x80000, 0, 0, NULL, NULL, \ 
  \ 
{0,0,0,0,0,0,0,0,0,0,0,0,0}}            




typedef struct pool{
  thread *rear;
  thread *tail;
} Pool;

typedef struct {
  thread thread_all[MAX_THREADS];
  thread *current_thread;
} thread_ctl;

thread* init_thread(void* func);
extern thread_ctl threadctl;
extern thread* get_current();
extern void set_current(thread* cur);
extern void switch_to();
thread* current_thread();
void thread_queue_push(thread *q, Pool* p);
thread*  thread_queue_remove(Pool *p);
thread* thread_remove_node(thread* t, Pool *p);
void idle();
void kernel_main();
void kill_zombie();
void foo();


void init_thread_state();
thread *create_thread(void *func);
void delay(int time);
void foo();
void schedule();
void delay(int time);
void thread_init();
#endif
