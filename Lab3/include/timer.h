#ifndef __TIMER_H_
#define __TIMER_H_

#include <stdint.h>

void arm_core_timer_enable();
void arm_core_timer_disable();
uint64_t get_time();

typedef void (*timer_callback)(char *);
void timer_enqueue(timer_callback cb, char *msg, unsigned long duration);
void set_expiredtime(uint32_t dur);
void add_timer(timer_callback cb, char *msg, unsigned long duration);
void timer_handler();
void print_message(char *msg);
void set_timeout(char *message, char *_time);
typedef struct timeout_event
{
    unsigned long register_time;
    unsigned long duration;
    timer_callback callback;
    char *msg;
    struct timeout_event *prev, *next;
} time_event;

#endif
