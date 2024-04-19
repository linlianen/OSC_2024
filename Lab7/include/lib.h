#ifndef _LIB_H_
#define _LIB_H_
#include <stddef.h>

#define read_sysreg(r) ({                      \
    \ 
    unsigned long __val;                       \
    \ 
    asm volatile("mrs %0, " #r : "=r"(__val)); \
    __val;                                     \
})

#define write_sysreg(r, __val) ({                 \
    asm volatile("msr " #r ", %0 " ::"r"(__val)); \
})

int strncmp(const char *a, const char *b, int len);
void memcpy_diy(char *dst, const char *src, size_t n);
size_t my_strlen(const char *s);
void *my_memset(void *dest, int val, size_t len);
void my_strcpy(char* dst, const char* src);
int my_strcmp(const char *a, const char *b);
char *strstr(const char *s, const char *find);
char *my_strtok_r(char *s, const char *delim, char **last);
char *my_strtok(char *s, const char *delim);
int myAtoi(char* str);
#endif
