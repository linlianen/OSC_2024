#ifndef __LIST_H_
#define __LIST_H_
#include <stddef.h>
#include <stdint.h>

typedef struct listnode {
  void *addr;
  struct listnode *next ;
} list_node;

struct page_head {
  int size;
  list_node *next;
};

struct chunk_head {
  int size;
  list_node *next;
};

typedef struct alloc_head {
  struct alloc_head *next, *prev;
} allochead;

void chunk_init(struct chunk_head *node);
void page_init(struct page_head *node);

void used_insert(allochead *node, allochead *addr);
void alloc_init(allochead *node);

void *page_remove_next(struct page_head *node);
void *chunk_remove_next(struct chunk_head *node);

void page_remove_node(struct page_head *node, list_node *addr);
void chunk_remove_node(struct chunk_head *node, void *addr);

void page_insert_list(struct page_head *node, list_node *addr);
void chunk_insert_list(struct chunk_head *node, list_node *addr);
#endif
