
#include "../include/my_malloc.h"
#include <stddef.h>
#include "../include/uart.h"

// Simple memory allocation -----------------------------------------
extern char __simple_malloc_start;
extern char __simple_malloc_end_no_paging;
static char *__simple_malloc_end = 0x00;
void *simple_malloc(size_t size)
{
  static char *ptr = &__simple_malloc_start;
  char *temp = ptr;
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