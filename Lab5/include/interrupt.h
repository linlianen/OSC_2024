#include "../include/gpio.h"
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

typedef struct task_Q
{
    unsigned char used;

    int (*fn)(void);

    int priority;

    struct task_Q *next;
    struct task_Q *prev;
} task_q;

task_q *task_queue_init(int (*fn)(void), int priority);
void task_queue_insert(int (*fn)(void), int priority, task_q *head);
void task_queue_del(task_q *head, task_q *del);
void task_queue_run(task_q *head);
#endif
