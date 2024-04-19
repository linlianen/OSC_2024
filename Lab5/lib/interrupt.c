#include "../include/interrupt.h"
#include "../include/exeception.h"
#include "../include/my_malloc.h"



task_q *task_queue_init(int (*fn)(void), int priority)
{
    disable_interrupt();
    task_q *cur = (task_q *)simple_malloc(sizeof(task_q));
    cur->next = cur;
    cur->prev = cur;
    cur->fn = fn;
    cur->priority = priority;
    enable_interrupt();
    return cur;
}

void task_queue_insert(int (*fn)(void), int priority, task_q *head)
{
    disable_interrupt();
    task_q *cur = (task_q *)simple_malloc(sizeof(task_q));

    if (head->next == head)
    {
        uart_printf("task queue haven't initialize yet...");
    }
    else
    {
        cur->fn = fn;
        cur->priority = priority;
    }

    task_q *task_cur = head;
    while (task_cur->next)
    {
        if (task_cur->priority < cur->priority)
        {
            task_q *prev_t = task_cur->prev;
            cur->next = task_cur;
            cur->prev = prev_t;
            prev_t->next = cur;

            task_cur->prev = cur;
            break;
        }
    }
    enable_interrupt();
}

void task_queue_del(task_q *head, task_q *del)
{
    disable_interrupt();
    task_q *cur = head;
    while (cur->next != cur)
    {
        if (cur == del)
        {
            task_q *prev = cur->prev;

            prev->next = cur->next;

            cur->next->prev = prev;
        }
    }
    enable_interrupt();
}

void task_queue_run(task_q *head)
{
    disable_interrupt();
    task_q *run_queue = head;
    while (run_queue->next == run_queue)
    {
        run_queue->fn();
        task_queue_del(head, run_queue);
    }
    enable_interrupt();
}

