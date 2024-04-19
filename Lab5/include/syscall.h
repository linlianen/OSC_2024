

#include "../include/thread.h"
// #define __NR_SYSCALLS 8
#include "../include/thread.h"
// #define SYS_GETPID_NUM    0
// #define SYS_UARTREAD_NUM  1
// #define SYS_UARTWRITE_NUM 2
// #define SYS_EXEC_NUM      3
// #define SYS_FORK_NUM      3
// #define SYS_EXIT_NUM      3
// #define SYS_MBOXCALL_NUM  4
// #define SYS_KILL_NUM     7
void sys_fork(tf* trapFrame);

size_t sys_uart_read(char buf[], size_t size);

size_t sys_uart_write(char buf[], size_t size);
int sys_exec(const char *name, char *const argv[]);
void sys_exit(); 
int sys_sub_mbox_call(unsigned char ch, unsigned int *mbox);
void sys_kill(int pid);
extern int getpid();

extern void uart_read();
extern void uart_write();
extern void exec();
extern int fork();
extern void sysexit();
extern void sys_mbox_call();
extern void kill();
extern void store_context(thread* cur);
