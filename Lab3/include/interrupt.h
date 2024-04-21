#include "../include/gpio.h"
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define timer_priority 5
#define uart_priority 20

typedef struct task_Q {
  unsigned char used;
  void (*fn)(void);
  unsigned long duration;
  unsigned long register_time;
  int priority;
  struct task_Q *next;
  struct task_Q *prev;
} task_q;

void exec_task();
void task_queue_add(void (*fn)(void), int priority);
#endif