#include "../include/gpio.h"
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

typedef struct task_Q {
  unsigned char used;
  void (*fn)(void);
  unsigned long duration;
  unsigned long register_time;
  int priority;
  struct task_Q *next;
  struct task_Q *prev;
} task_q;

void task_queue_add(void (*fn)(void), int priority, unsigned long duration);
#endif