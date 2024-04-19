#ifndef __MM_H_
#define __MM_H_

#include "list.h"
#include "stdint.h"

enum blockstate { FULL, CONT, EMPTY };
#define EMPTY 0
#define CONT 1
#define FULL 2
typedef struct frame_node {
  int order;
  int used;
  int full;
  int cont;
} frameState;

typedef struct chunk_node {
  int size;
  struct chunk_node *next;
} chunk;
void *splitframe(int order, int size);
// void print_memory();
void *alloc_page(int size);

void alloc_clean();
void freepages(void *addr);

void check_and_merge(struct page_head* a,struct chunk_head *b);

int chunk_list_size(chunk* list);
void memory_reserve(uintptr_t start, uintptr_t end);
void mm_init(struct page_head* a, struct chunk_head *b);

void init_buddy();

void test_buddy();

void test_dynamic_alloc();

void *kalloc(int size);

void kfree(void* ptr);
#endif
