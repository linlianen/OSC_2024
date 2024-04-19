#include "../include/timer.h"
#include "../include/my_malloc.h"
#include "../include/exeception.h"
#include "../include/sysreg.h"
#include "../include/uart.h"
#include "../include/lib.h"
#define CORE0_TIMER_IRQ_CTRL ((unsigned int *)0x40000040)

time_event *timeout_queue_head, *timeout_queue_tail;

void arm_core_timer_enable()
{

	// enable timer
	register unsigned int enable = 1;
	asm volatile("msr cntp_ctl_el0, %0" ::"r"(enable));
	unsigned long frq = read_sysreg(cntfrq_el0);
	write_sysreg(cntp_tval_el0, frq * 2);
	*CORE0_TIMER_IRQ_CTRL = 2;
}

void set_expiredtime(uint32_t dur)
{
	uint64_t freq;

	asm volatile("mrs %0, cntfrq_el0" : "=r"(freq) :);

	register unsigned long expire_period = freq * dur;
	asm volatile("msr cntp_tval_el0, %0" ::"r"(expire_period));
}
void arm_core_timer_disable()
{
	// disable timer
	register unsigned int enable = 0;
	asm volatile("msr cntp_ctl_el0, %0" ::"r"(enable));
	// disable timer interrupt
	*CORE0_TIMER_IRQ_CTRL = 0;
}

uint64_t get_time()
{
	uint64_t cntpct_el0;
	uint64_t cntfrq_el0;
	uint64_t time_in_sec;

	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0) :);
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0) :);

	time_in_sec = cntpct_el0 / cntfrq_el0;
	return time_in_sec;
}

void print_message(char *msg)
{
	uart_puts(msg);
}

void set_timeout(char *message, char *_time)
{
	uint64_t time = 0;

	time = myAtoi(_time);
	// int len = my_strlen(_time);
	// for (int i = 0; i < len; i++)
	// {
	// 	/* code */
	// 	time <<= 1;
	// 	time |= _time[i];
	// }

	timer_enqueue(print_message, message, time);
}

// add timer to time_queue`
void timer_enqueue(timer_callback cb, char *msg, unsigned long duration)
{
	time_event *new_event = (time_event *)simple_malloc(sizeof(time_event));

	new_event->register_time = get_time();

	new_event->duration = duration;

	new_event->callback = cb;

	// 複製訊息字串到新事件的 msg 屬性中
	for (int i = 0; i < 20; i++)
	{
		new_event->msg[i] = msg[i];
		if (msg[i] == '\0')
			break;
	}

	new_event->next = 0;
	new_event->prev = 0;

	if (timeout_queue_head == 0)
	{
		timeout_queue_head = new_event;
		timeout_queue_tail = new_event;

		arm_core_timer_enable();
		set_expiredtime(duration);
	}
	else
	{
		time_event *cur = timeout_queue_head;
		uint64_t totoaltime = cur->register_time + cur->duration;

		unsigned long timeout = new_event->register_time + new_event->duration;

		while (1)
		{
			if ((cur->register_time + cur->duration) > timeout)
				break;

			cur = cur->next;
		}

		// 如果 cur 為 0，表示新事件應該插入到隊列尾
		if (cur == 0)
		{ // cur at end
			new_event->prev = timeout_queue_tail;
			timeout_queue_tail->next = new_event;
			timeout_queue_tail = new_event;
		}
		else if (cur->prev == 0)
		{ // cur at head
			new_event->next = cur;
			(timeout_queue_head)->prev = new_event;
			timeout_queue_head = new_event;

			set_expiredtime(duration);
		}
		// 其他情況表示新事件應該插入到 cur 和 cur->prev 之間
		else
		{ // cur at middle
			new_event->next = cur;
			new_event->prev = cur->prev;
			(cur->prev)->next = new_event;
			cur->prev = new_event;
		}
	}
}

// timer_handler
void timer_handler()
{

	disable_interrupt();

	// 取得目前時間
	unsigned long current_time = get_time();

	// 將字串 "message :" 傳送至 UART 裝置
	uart_puts((char *)"\nmessage :");

	// 執行 timeout_queue_head 所指向的事件的回呼函式，將 timeout_queue_head 所指向的事件的訊息傳入
	timeout_queue_head->callback(timeout_queue_head->msg);

	// 將目前時間傳送至 UART 裝置
	uart_printf("\ncurrent time : %ds\n", current_time);

	// 將 timeout_queue_head 所指向的事件的註冊時間、執行時間以及持續時間分別傳送至 UART 裝置
	uart_printf("command executed time : %ds\n", timeout_queue_head->register_time);
	uart_printf("command duration time : %ds\n\n", timeout_queue_head->duration);

	// 取得 timeout_queue_head 的下一個事件指標
	time_event *next = timeout_queue_head->next;

	// 如果下一個事件存在
	if (next)
	{
		// 將下一個事件的前一個事件指標設為 0
		next->prev = 0;

		// 將 timeout_queue_head 設為下一個事件指標
		timeout_queue_head = next;

		// 啟用核心定時器
		arm_core_timer_enable();

		// 設定過期時間為下一個事件的註冊時間加上持續時間減去目前時間
		set_expiredtime(next->register_time + next->duration - get_time());
	}
	// 如果沒有其他事件
	else
	{
		// 將 timeout_queue_head 和 timeout_queue_tail 設為 0
		timeout_queue_head = timeout_queue_tail = 0;

		// 禁用核心定時器
		arm_core_timer_disable();
	}
	enable_interrupt();
}
