#include "../include/thread.h"
#include "../include/exeception.h"
#include "../include/interrupt.h"
#include "../include/mm.h"
#include "../include/uart.h"
#include "stdlib.h"
#include <stdbool.h>
#include <stdint.h>

int thread_count = 0;

Pool running;
Pool ready;
Pool zombielist;

thread *current_thread() { return get_current(); }

thread *currThread;
void init_thread_state()
{

	disable_interrupt();
	thread_count = 0;
	running.rear = NULL;
	running.tail = NULL;
	ready.rear = NULL;
	ready.tail = NULL;
	zombielist.rear = NULL;

	zombielist.tail = NULL;

	asm volatile("msr tpidr_el1, %0" ::"r"(kalloc(sizeof(thread)))); /// malloc a space for current kernel thread to prevent crash
	thread *idlethread = init_thread(idle);

	currThread = idlethread;
	set_current(currThread);
	enable_interrupt();
}

void thread_queue_push(thread *q, Pool *p)
{

	// If queue is empty, then
	// new node is front and rear both
	if (p->rear == NULL && p->tail == NULL)
	{	
		p->tail=q;
		p->rear = q;
		return;
	}
	else
	{
		q->next = p->rear;
		p->rear->prev = q;
		p->rear = q;
		return;
	}

	// Add the new node at
	// the end of queue and change rear
}

thread *thread_queue_remove(Pool *p)
{

	thread *task = p->tail;

	p->tail = task->next;

	return task;
}

thread *create_thread(void *func)
{
	disable_interrupt();
	thread *new_t = (thread *)kalloc(sizeof(thread));
	new_t->kernel_stack = kalloc(THREAD_SIZE);
	new_t->user_stack = kalloc(THREAD_SIZE);
	new_t->context.sp = (unsigned long)(new_t->kernel_stack + THREAD_SIZE);
	new_t->context.fp = new_t->context.sp;

	new_t->thread_data = func;
	new_t->is_zombie = 0;
	new_t->context.lr = (uint64_t)new_t->thread_data;
	new_t->tid = ++thread_count;
	new_t->is_waiting = 1;
	new_t->is_running = 0;
	new_t->next = NULL;
	new_t->prev = NULL;
	thread_queue_push(new_t, &ready);
	enable_interrupt();
	return new_t;
}

thread *init_thread(void *func)
{
	disable_interrupt();
	thread *init_thread = (thread *)kalloc(sizeof(thread));
	init_thread->kernel_stack = kalloc(THREAD_SIZE);
	init_thread->user_stack = kalloc(THREAD_SIZE);
	init_thread->thread_data = func;
	init_thread->context.sp =
		(unsigned long)(init_thread->kernel_stack + THREAD_SIZE);
	init_thread->context.fp = init_thread->context.sp;
	init_thread->context.lr = (uint64_t)init_thread->thread_data;

	init_thread->is_zombie = 0;
	init_thread->tid = thread_count;
	init_thread->is_running = 1;
	init_thread->is_waiting = 0;
	init_thread->next = NULL;
	init_thread->prev = NULL;
	thread_queue_push(init_thread, &running);
	enable_interrupt();
	return init_thread;
}
void schedule()
{
	// TODO: breakpoint check
	disable_interrupt();
	if (ready.tail == NULL)
	{
		return;
	}
	thread *next = ready.tail;
	thread *curr = current_thread();
	if (curr->is_running != 1)
	{
		curr = running.rear;
	}

	int thread_num = 0;
	while (curr==next && next->is_waiting != 1)
	{
		next = next->prev;
	}
	// uart_printf("now id : %d\n", currThread->tid);
	// uart_printf("next id : %d\n", next->tid);
	// delay(10000000000000);

	if (next->is_running == 1)
	{
		set_current(next);
		currThread = next;
	}

	// check zombie in for loop
	//  while(current_thread()->state!=THREAD_RUNNING){
	//
	//  }

	set_current(next);
	
	currThread = next;
	
	curr->is_waiting = 1;

	next->is_running = 1;

	curr->is_running = 0;
	next->is_waiting = 0;

	thread *pop_run = thread_remove_node(curr, &running);
	thread *pop_ready = thread_remove_node(next, &ready);
	thread_queue_push(curr, &ready);

	thread_queue_push(next, &running);
	
	switch_to(&curr->context, &next->context);
	enable_interrupt();
}

void delay(int time)
{

	while (time >= 0)
	{
		asm volatile("nop");
		time--;
	}
}

void thread_exit()
{
	currThread->is_zombie = 1;
	currThread->is_running = 0;

	currThread->is_waiting = 0;
	thread *exit = current_thread();
	uart_printf("Thread %d", exit->tid);
	thread_remove_node(exit, &ready);
	thread_remove_node(currThread, &running);
	thread_queue_push(currThread, &zombielist);
	uart_printf("Thread %d exit\n", currThread->tid);
	
	schedule();
}
void foo()
{
	for (int i = 0; i < 3; ++i)
	{
		uart_printf("Thread id: %d %d\n", currThread->tid, i);
		delay(1000000);
		// how to deal with nested schedule ??
		schedule();
	}
	thread_exit();
}

thread *thread_remove_node(thread *t, Pool *p)
{

	thread *cur = p->rear;

	for (; cur != NULL; cur = cur->next)
	{
		if (cur->tid == t->tid)
		{

			thread *now = cur;
			if (cur->prev != NULL && cur->next != NULL)
			{
				cur->prev->next = cur->next;
				cur->next->prev = cur->prev;
			}
			else if (cur->prev == NULL)
			{
				cur->next->prev = p->rear;
				cur->prev->next = cur->next;
			}
			else if (cur->next == NULL)
			{
				cur->prev->next = p->rear;
				cur->next->prev = cur->prev;
			}

			if (cur == p->rear)
			{
				thread* next = p->rear->next;
				p->rear = next;
			}

			if(cur==p->tail){
				thread* thread = p->tail->prev;
				p->tail = thread;
			}	
			return now;
		}
	}
}

void kill_zombie()
{
	// TODO : zombielist complete 03/15
	for (thread *t = zombielist.tail; t->prev != NULL; t = t->prev)
	{
		if ((t->tid == 1 || t->tid == 2 || t->tid == 3) && t->is_zombie )
		{
			thread_remove_node(t, &zombielist);
			t->is_waiting = 0;
			// thread_remove_node(t, &ready);

			kfree(t->kernel_stack);
			kfree(t->user_stack);
			kfree(t);
			return;
		}

		if ((t->tid == t->next->tid))
		{
			return;			
		}
	}
}

void idle()
{
	while (1)
	{
		kill_zombie();
		schedule();
	}
}
void kernel_main()
{

	set_current(currThread);
	for (int i = 0; i < 3; ++i)
	{
		create_thread(foo);
	}
	idle();
}
