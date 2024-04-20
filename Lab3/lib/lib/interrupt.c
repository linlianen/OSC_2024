#include "../include/interrupt.h"

#include "../include/exeception.h"
#include "../include/my_malloc.h"

/*
typedef struct task_Q {
  unsigned char used;
  int (*fn)(void);
  int priority;
  struct task_Q *next;
  struct task_Q *prev;
} task_q;
*/
task_q *task_queue_head, *task_queue_tail;

void task_queue_add(void (*fn)(void), int priority) {

  task_q *new_task = (task_q *)simple_malloc(sizeof(task_q));
  new_task->fn = fn;
  new_task->priority = priority;

  if (task_queue_head == 0) {
    task_queue_head = new_task;
    task_queue_tail = new_task;

  } else {
    task_q *task_cur = task_queue_head;

    while (task_cur) {
      if (task_cur->priority > new_task->priority) {
        break;
      }

      task_cur = task_cur->next;
    }

    if (task_cur == 0) {
      new_task->prev = task_queue_tail;
      task_queue_tail->next = new_task;
      task_queue_tail = new_task;
    } else if (task_cur->prev == 0) {
      task_cur->prev = new_task;
      new_task->next = task_cur;
      task_queue_head = new_task;
    } else {
      task_q *next = task_cur->next;
      task_cur->next = new_task;
      new_task->prev = task_cur;
      new_task->next = next;
      next->prev = new_task;
    }
  }

  // task_q * cur = (task_q*);
}

void exec_task() {
  while (1) {
    uart_puts("1234\n");
    task_queue_head->fn();
    disable_interrupt();
    task_queue_head = task_queue_head->next;
    if (task_queue_head) {
      task_queue_head->prev = 0;
    } else {
      task_queue_head = task_queue_tail = 0;
      enable_interrupt();
      return;
    }
    enable_interrupt();
  }
}
// for preemptive or not
// void task_queue_run()