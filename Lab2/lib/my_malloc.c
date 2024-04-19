
#include "../include/my_malloc.h"
#include <stddef.h>
#include <stdint.h>
#include "../include/uart.h"

// Simple memory allocation -----------------------------------------
extern char __simple_malloc_start;
extern char __simple_malloc_end_no_paging;
char *__simple_malloc_end = (char*)0x32000;
char* ptr;
char* temp;
void *simple_malloc(size_t size)
{
  ptr = &__simple_malloc_start;
  temp = ptr;
  ptr += size;
  if (__simple_malloc_end == 0)
  {

    __simple_malloc_end = &__simple_malloc_end_no_paging;
  }
  if (ptr > __simple_malloc_end)
  {
    return NULL;
  }

  return (void *)temp;
}
