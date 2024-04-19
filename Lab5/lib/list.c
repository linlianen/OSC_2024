#include "../include/list.h"
#include "../include/exeception.h"
#include "../include/my_malloc.h"
#include "../include/uart.h"

void alloc_init(allochead *node) {
  node->next = node;
  node->prev = node;
}
void chunk_init(struct chunk_head *node) {
  disable_interrupt();
  node->size = (int)0x0;
  node->next = NULL;
  enable_interrupt();
}
void page_init(struct page_head *node) {
  node->size = (int)0x0;
  node->next = NULL;
}

void *page_remove_next(struct page_head *node) {
  list_node *next = node->next;

  void *addr = next->addr;

  node->next = next->next;

  return addr;
}
void *chunk_remove_next(struct chunk_head *node) {
  list_node *next = node->next;

  void *addr = next->addr;

  node->next = next->next;

  return addr;
}
void page_remove_node(struct page_head *node, list_node *addr) {
  list_node *cur = node->next;

  int size = node->size;
  for (; cur != NULL; cur = cur->next) {
    if (cur->next == addr) {

      list_node *next = cur->next;
      cur->next = next->next;
      node->size = size - 1;
      return;
    } else if (cur == addr) {
      list_node *next = cur->next;
      node->next = next;
      node->size = size - 1;
      return;
    }
  }
}
void chunk_remove_node(struct chunk_head *node, void *addr) {
  list_node *cur = node->next;

  int size = node->size;
  for (; cur != NULL; cur = cur->next) {
    if (cur->next == addr) {

      list_node *next = cur->next;
      cur->next = next->next;
      node->size = size - 1;
      return;
    } else if (cur == addr) {
      list_node *next = cur->next;
      node->next = next;
      node->size = size - 1;
      return;
    }
  }
}

void used_insert(allochead *node, allochead *addr) {
  disable_interrupt();

  allochead *prev = node->prev;

  addr->prev = prev;
  addr->next = node;
  prev->next = addr;
  node->prev = addr;
  enable_interrupt();
}

void page_insert_list(struct page_head *node, list_node *addr) {
  disable_interrupt();
  int size = node->size;

  list_node *cur = node->next;
  if (node->size == 0) {
    list_node *next = node->next;
    node->next = addr;
    addr->next = next;

    node->size = 1;
    enable_interrupt();
    return;
  } else {
    list_node *next = cur->next;
    cur->next = addr;
    addr->next = next;
    node->size = size + 1;
    enable_interrupt();
  }
}
void chunk_insert_list(struct chunk_head *node, list_node *addr) {
  int size = node->size;
  list_node *cur = node->next;
  if (node->size == 0) {
    addr->next = NULL;

    node->next = addr;

    node->size = size + 1;
    return;
  } else {

    addr->next = cur;
    node->size = size + 1;
    node->next = addr;
    return;
  }
}
