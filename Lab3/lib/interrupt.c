#include "../include/interrupt.h"

#include "../include/exeception.h"
#include "../include/my_malloc.h"
/*something wrong*/
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

void task_queue_add(void (*fn)(void), int priority, unsigned long duration) {
  disable_interrupt();
  task_q *new_task = (task_q *)simple_malloc(sizeof(task_q));
  new_task->fn = fn;
  new_task->priority = priority;
  new_task->register_time = get_time();
  new_task->duration = duration;
  if (task_queue_head == 0) {
    task_queue_head = new_task;
    task_queue_tail = new_task;
    set_expiredtime(new_task->duration + new_task->register_time - get_time());
  } else {
    task_q *cur = task_queue_head;
    unsigned long timeout = new_task->register_time + new_task->duration;

    while (1) {
      if ((cur->register_time + cur->duration) > timeout) {
        break;
      } else {
        task_q *cur = task_queue_head;
        // uint64_t totoaltime = cur->register_time + cur->duration;

        unsigned long timeout = new_task->register_time + new_task->duration;

        while (1) {
          if ((cur->register_time + cur->duration) > timeout)
            break;

          cur = cur->next;
        }

        // 如果 cur 為 0，表示新事件應該插入到隊列尾
        if (cur == 0) { // cur at end
          new_task->prev = task_queue_tail;
          task_queue_tail->next = new_task;
          task_queue_tail = new_task;

        } else if (cur->prev == 0) { // cur at head
          new_task->next = cur;
          (task_queue_head)->prev = new_task;
          task_queue_head = new_task;
          set_expiredtime(new_task->register_time + new_task->duration -
                          get_time());
        }
        // 其他情況表示新事件應該插入到 cur 和 cur->prev 之間
        else { // cur at middle
          new_task->next = cur;
          new_task->prev = cur->prev;
          (cur->prev)->next = new_task;
          cur->prev = new_task;
        }
      }
    }
  }
  enable_interrupt();
  // task_q * cur = (task_q*);
}

// for preemptive or not
void task_queue_run()
