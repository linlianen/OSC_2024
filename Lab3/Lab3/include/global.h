#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define MAX_BUF_SIZE 128

extern time_event *timeout_queue_head, *timeout_queue_tail;


extern char tx_buffer[MAX_BUF_SIZE];
extern char rx_buffer[MAX_BUF_SIZE];

extern unsigned int tx_bf_r_idx;
extern unsigned int tx_bf_w_idx;
extern unsigned int rx_bf_r_idx;
extern unsigned int rx_bf_w_idx;

#endif