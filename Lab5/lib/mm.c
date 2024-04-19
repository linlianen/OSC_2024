#define __GNU_SOURCE

#include "../include/mm.h"
#include "../include/cpio.h"
#include "../include/dtb.h"
#include "../include/exeception.h"
#include "../include/list.h"
#include "../include/uart.h"
#include <stddef.h>
#include <stdint.h>
#define FRAME_BASE ((uintptr_t)0x0)
#define FRAME_END ((uintptr_t)0x3c000000)
#define ERROR 0xffffffff
// frame array number
#define FRAME_ARRAY_SIZE ((FRAME_END - FRAME_BASE) / PAGE_SIZE)
// frame size
#define PAGE_SIZE 0x1000
#define CHUNK_SIZE 0x10
#define FRAME_BINS 8 // 8 | 4

#define MAX_ORDER (FRAME_BINS - 1)
#define FRAME_SIZE_MAX ORDER2SIZE(MAX_ORDER)
#define ORDER2SIZE(ord) (PAGE_SIZE * (1 << (ord)))
/* we dont need free tag */
#define FRAME_FREE 0x8
#define FRAME_INUSE 0x4
#define FRAME_MEM_CACHE 0x2

#define IS_INUSE(flag) ((flag).flags & FRAME_INUSE)
#define IS_MEM_CACHE(flag) ((flag).flags & FRAME_MEM_CACHE)

/* cache start from 32 bytes to 2048 bytes */
#define CACHE_BINS 8
#define CACHE_MAX_ORDER (CACHE_BINS - 1)
static int size_to_chunk_order(unsigned int size);
frameState frames[FRAME_ARRAY_SIZE];
struct page_head alloc_mem[FRAME_BINS];
struct chunk_head chunk_mem[CACHE_BINS];
extern void *_dtb_ptr;
extern char *initrd_ptr, *initramfs_end, *address, *size;
extern char _skernel, __image_end, __simple_malloc_end_no_paging, _ekernel;
/* __simple_malloc_end is determined at run time*/
static unsigned align_up_exp(unsigned n) {
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;

  return n;
}

struct freed {
  struct freed *next;
};

static uint32_t align_up(uint32_t size, int alignment) {
  return (size * alignment - 1) & -alignment;
}

static int addr2idx(void *addr) {
  return (((uintptr_t)addr & -PAGE_SIZE) - FRAME_BASE) / PAGE_SIZE;
}

static int page_to_frame_order(unsigned count) {
  count = align_up_exp(count);
  return __builtin_ctz(count);
}

static int size_to_cache_order(unsigned size) {
  size = align_up_exp(size);
  size /= CHUNK_SIZE;
  return __builtin_ctz(size);
}

void mm_init(struct page_head *a, struct chunk_head *b) {
  init_buddy();
  // spin block
  memory_reserve((uintptr_t)0x00, (uintptr_t)0x1000);
  // //physical memory
  memory_reserve((uintptr_t)&_skernel, (uintptr_t)&_ekernel);
  uart_printf("phyical memeory reserved start: %p, end: %p\n",
              (uintptr_t)&_skernel, (uintptr_t)&_ekernel);
  // // initramfs
  memory_reserve((uintptr_t)initrd_ptr, (uintptr_t)initramfs_end);

  uart_printf("initrd memeory reserved start: %p, end: %p\n",
              (uintptr_t)initrd_ptr, (uintptr_t)initramfs_end);

  // dtb memory
  memory_reserve((uintptr_t)(address), (uintptr_t)(address) + (uintptr_t)size);
  // uart_printf("dtb memeory reserved start: %p, end:
  // %p\n",(uintptr_t)(address),(uintptr_t)(address)+(uintptr_t)size);
  memory_reserve((uintptr_t)(&__image_end),
                 (uintptr_t)&__simple_malloc_end_no_paging);
  uart_printf("simple memeory reserved start: %p, end: %p\n",
              (uintptr_t)(&__image_end),
              (uintptr_t)&__simple_malloc_end_no_paging);

  check_and_merge(a, b);
  // print_memory();
}
void init_buddy() {

  disable_interrupt();
  for (int i = 0; i < FRAME_BINS; i++) {
    /* code */
    page_init(&alloc_mem[i]);
  }
  // fold
  for (size_t k; k < CACHE_BINS; k++) {
    chunk_init(&chunk_mem[k]);
  }

  for (int j = 0; j < FRAME_ARRAY_SIZE; j++) {
    /* code */
    frames[j].order = 0;
    frames[j].used = 0;
    frames[j].cont = 0;
    frames[j].full = 0;
  }
  enable_interrupt();
}

void memory_reserve(uintptr_t start, uintptr_t end) {
  // start = start & ~(PAGE_SIZE - 1);
  // end = align_up(end, PAGE_SIZE);
  //
  // for (uintptr_t i = start; i < end; i++)
  // {
  //   /* code */
  //   int idx = addr2idx((void *)i);
  //   frames[idx].state = FULL;
  //   frames[idx].order = -1;
  // }
  int start_idx = (((uintptr_t)start) - FRAME_BASE) / PAGE_SIZE;
  int end_idx = (((uintptr_t)end) - FRAME_BASE) / PAGE_SIZE;

  frames[0].full = 1;
  frames[0].used = 1;
  int size = end_idx - start_idx;
  int order = page_to_frame_order(size);
  if (start_idx < end_idx && start_idx < FRAME_ARRAY_SIZE &&
      end_idx < FRAME_ARRAY_SIZE) {
    if (start_idx == end_idx) {
      frames[start_idx].full = 1;
      frames[start_idx].used = 1;
      frames[start_idx].order = order;
    } else {
      for (int i = start_idx; i < end_idx; i++) {
        frames[i].full = 1;
        frames[i].used = 1;
        frames[i].order = order;
      }
    }
  }
}

// void alloc_clean(){
//   allochead* head = alloc_used;
//   int i = 0;
//   while(head != alloc_used->prev){
//     void* addr = head->next;
//     if(addr == (uintptr_t)0x3C009000 || addr == (uintptr_t)0x3C007000)
//     {
//       head = head->next;
//
//     }else if(head->next==(uintptr_t)0x00){
//       return;
//     }
//     else{
//       i++;
//       freepages(addr);
//       head = head->next;
//     }
//   }
//
//
//
// }

int chunk_list_size(chunk *list) {
  int size = 0;
  try:
    while (list != NULL) {
      size++;

      list = list->next;
    }
  catch : return size;
}
void *kalloc(int size) {

  int alloc_size = size < CHUNK_SIZE ? CHUNK_SIZE : size;
  void *ptr;
  if (alloc_size < PAGE_SIZE) {
    // split the page frame
    int page_idx = 0;
    int order = size_to_chunk_order(alloc_size);
    if (chunk_mem[order].size < 1) {
      int block_cnt = (int)PAGE_SIZE / alloc_size;
      // allochead* alloc_head = alloc_used;
      void *mem = alloc_page(1);
      // while((uintptr_t)mem > FRAME_END || mem == (uintptr_t)0x00){
      //   // used_insert(alloc_used, mem);
      //   page_idx = addr2idx(mem);
      //   frames[page_idx].state = EMPTY;
      //   mem = alloc_page(1);
      //
      // }
      // alloc_clean();
      int idx = ((uintptr_t)mem - FRAME_BASE) / PAGE_SIZE;
      frames[idx].used = 0;
      frames[idx].cont = 1;
      frames[idx].full = 0;
      frames[idx].order = order;
      for (int i = 0; i < block_cnt; i++) {
        void *ptr_list = (void *)((uintptr_t)mem + i * size);

        /*
        TODO: insert the list

        */
        // uart_printf("adddress :%p\n", ptr_list);

        chunk_insert_list(&chunk_mem[order], ptr_list);
      }
    }

    ptr = chunk_mem[order].next;
    while (ptr == NULL) {
      chunk_mem[order].next = chunk_mem[order].next->next;
      chunk_mem[order].size -= 1;
      ptr = chunk_mem[order].next;
    }
    int idx_addr = addr2idx(ptr);
    frames[idx_addr].used = 1;
    frames[idx_addr].cont = 1;
    frames[idx_addr].order = order;

    chunk_remove_node(&chunk_mem[order], ptr);
    // ptr = remove_tail(&chunk_mem[order]);
    uart_printf("chunk size : %d, address :%p\n", alloc_size, ptr);
    // remove_node(&chunk_mem[order], ptr);
  } else {
    int page_cnt = size / PAGE_SIZE;

    ptr = alloc_page(page_cnt);
  }

  return ptr;
}

static int size_to_chunk_order(unsigned int size) {
  size = align_up_exp(size);
  size /= CHUNK_SIZE;

  return __builtin_ctz(size);
}

void kfree(void *ptr) {
  int idx = addr2idx(ptr);
  chunk *ptr_chunk = (chunk *)ptr;
  if (idx > FRAME_ARRAY_SIZE) {
    idx = addr2idx((void *)FRAME_END);
    uart_printf("idx: %d\n", idx);
    return;
  }
  if (frames[idx].used == 1 && frames[idx].full == 0) {
    frames[idx].used = 0;
    frames[idx].cont = 0;
    int order = frames[idx].order;
    chunk_remove_node(&chunk_mem[order], ptr);
    uart_printf("insert address %p in order %d in chunk\n", ptr, order);

  } else {
    freepages(ptr);
  }
}

void check_and_merge(struct page_head *a, struct chunk_head *b) {

  disable_interrupt();
  for (int i = 0; i <= MAX_ORDER; i++) {

    int frame_idx = 0;
    int order = i;
    void *page_addr;
    int next_page_idx = 0;
    while (1) {
      /* code */
      // if(i>1){
      // uart_printf("123\n");
      // }
      int frame_count = 0;

      // uart_printf("page head size : %d\n", alloc_mem[0].size);
      next_page_idx = frame_idx + (1 << i);
      if (frames[frame_idx].used == 0 && frames[frame_idx].order == i &&
          frames[next_page_idx].order == frames[frame_idx].order) {

        frame_count = frame_idx;
        order = i + 1; // insert to free block
        next_page_idx = frame_idx + (1 << order);
        while (frame_count < next_page_idx && frames[frame_count].used == 0 &&
               frame_count < FRAME_ARRAY_SIZE) {
          // uart_printf("frame_count : %d, frame size :%d\n", frame_count,
          // FRAME_ARRAY_SIZE);
          frames[frame_count].order += 1;
          frames[frame_count].used = 0;
          //   if(frame_count <60){
          // uart_printf("index: %d, order: %d\n", frame_count,
          // frames[frame_count].order);
          //   }
          frame_count++;
        }
        // if(i > 1)
        // {
        //   uart_printf("order: %d", frames[frame_idx].order);
        // }

      } else if (frames[frame_idx].used == 0 && frames[frame_idx].order == i &&
                 frames[next_page_idx].order != frames[frame_idx].order) {
        // uart_printf("b page head size : %d\n", alloc_mem[0].size);
        frames[frame_idx].used = 0;
        frame_idx = next_page_idx;
      } else if (frames[frame_idx].used == 1) {
        int order = frames[frame_idx].order;

        // uart_printf("c page head size : %d\n", alloc_mem[0].size);
        next_page_idx = frame_idx + 1;
      }
      // if(frame_idx< 300){
      //   uart_printf("final index : %d, frames order: %d,",frame_idx,
      //   frames[frame_idx].order); uart_printf("frames state: %d\n",
      //   frames[frame_idx].state);
      // }
      //
      frame_idx = next_page_idx;
      if (frame_idx >= FRAME_SIZE_MAX) {
        break;
      }
    }
  }
  enable_interrupt();

  int frame_list_idx = 0;
  void *page_list_addr;
  int frame_list_order = 0;
  while (frame_list_idx < FRAME_SIZE_MAX) {
    if (frames[frame_list_idx].used == 0) {
      frame_list_order = frames[frame_list_idx].order;

      page_list_addr = (void *)(FRAME_BASE + PAGE_SIZE * (frame_list_idx - 1));
      if ((uintptr_t)page_list_addr <= FRAME_END) {
        page_insert_list(&alloc_mem[frame_list_order], page_list_addr);
      }
      frame_list_idx = frame_list_idx + (1 << frame_list_order);
    } else {

      frame_list_order = frames[frame_list_idx].order;
      frame_list_idx = frame_list_idx + 1;
    }
  }
  // for (int idx=0 ; idx<30; idx++) {
  //   if(frames[idx].state == EMPTY){
  //     uart_printf("idx: %d, order: %d\n",idx,frames[idx]);
  //   }
  // }
  uart_printf("merge finish\n");
}

void *splitframe(int order, int size) {

  void *split_addr;

  split_addr = alloc_mem[order].next;
  while (split_addr == (void *)0x00) {
    alloc_mem[order].next = alloc_mem[order].next->next;
    split_addr = alloc_mem[order].next;
    alloc_mem[order].size -= 1;
  }
  page_remove_node(&alloc_mem[order], split_addr);
  int page_order = order;
  int split_idx = addr2idx(split_addr);
  int split_idx_2 = 0;
  page_order -= 1;
  frames[split_idx].order = page_order;
  frames[split_idx].used = 0;
  frames[split_idx].full = 0;
  frames[split_idx].cont = 0;

  split_idx_2 = split_idx ^ (1 << page_order);
  void *split_addr_2 = (void *)(FRAME_BASE + PAGE_SIZE * (split_idx_2));
  frames[split_idx_2].order = page_order;
  frames[split_idx_2].used = 0;
  frames[split_idx_2].full = 0;
  page_insert_list(&alloc_mem[page_order], split_addr);
  page_insert_list(&alloc_mem[page_order], split_addr_2);

  // uart_puts("\r\n--------SPLIT FRAME--------\n");
  // uart_printf("Split Order %d to Order %d in %p\n", order, (order - 1),
  //             split_addr);
  // uart_printf("Split Order %d to Order %d in %p\n", order, (order - 1),
  //             split_addr_2);
  while (page_order > size) {

    split_addr = alloc_mem[page_order].next;
    while (split_addr == (void *)0x00) {
      alloc_mem[page_order].next = alloc_mem[page_order].next->next;
      alloc_mem[page_order].size -= 1;
      split_addr = alloc_mem[page_order].next;
    }
    split_idx = addr2idx(split_addr);
    page_order -= 1;
    frames[split_idx].order -= 1;
    frames[split_idx].used = 0;
    frames[split_idx].full = 0;
    split_idx_2 = split_idx ^ (1 << page_order);
    split_addr_2 = (void *)(FRAME_BASE + PAGE_SIZE * (split_idx_2));
    frames[split_idx_2].order -= 1;
    frames[split_idx_2].used = 0;
    frames[split_idx_2].full = 0;
    page_insert_list(&alloc_mem[page_order], split_addr);
    page_insert_list(&alloc_mem[page_order], split_addr_2);

    // uart_printf("Split Order %d to Order %d in %p\n", page_order + 1,
    //             (page_order), split_addr);
    // uart_printf("Split Order %d to Order %d in %p\n", page_order + 1,
    //             (page_order), split_addr_2);
  }
  /*
  TODO: print target addr and order
  */
  page_remove_node(&alloc_mem[page_order], split_addr);
  frames[split_idx].order = page_order;
  frames[split_idx].used = 1;
  frames[split_idx].full = 1;
  // uart_printf("-------SPLIT FRAME END------\n");
  return split_addr;
}

void *alloc_page(int size) {

  int frame_order = page_to_frame_order(size);
  void *target_addr;
  int page_idx = 0;
  if (alloc_mem[frame_order].size > 0) {
    target_addr = alloc_mem[frame_order].next;
    while (target_addr == (void *)0x0) {

      alloc_mem[frame_order].next = alloc_mem[frame_order].next->next;
      alloc_mem[frame_order].size -= 1;
      target_addr = alloc_mem[frame_order].next;
    }
    if (target_addr != (void *)0x00) {
      page_idx = (int)((uintptr_t)target_addr - FRAME_BASE) / (int)PAGE_SIZE;
    }

    frames[page_idx].used = 1;
    frames[page_idx].full = 1;
    frames[page_idx].cont = 0;
    page_remove_node(&alloc_mem[frame_order], target_addr);

  } else {
    for (int i = frame_order + 1; i <= MAX_ORDER; i++) {
      if (alloc_mem[i].size > 0) {
        // splitframes
        target_addr = splitframe(i, frame_order);
        break;
      }
    }
  }

  uart_printf("Allocate page address %p in Order %d\n", target_addr,
            frame_order);
  int target_idx =
      (((uintptr_t)target_addr & ~PAGE_SIZE) - FRAME_BASE) / PAGE_SIZE;
  frames[target_idx].order = frame_order;
  frames[target_idx].used = 1;
  return target_addr;
}

/*
TODO: freepages function
*/

/*Reverse process in split page*/
void freepages(void *addr) {

  int page_idx = ((uintptr_t)addr - FRAME_BASE) / PAGE_SIZE;

  uint32_t order = frames[page_idx].order;

  void *buddy_addr;
  int buddy_idx = page_idx ^ (1 << order);
  frames[page_idx].used = 0;
  frames[page_idx].full = 0;
  frames[page_idx].cont = 0;
  int free_idx = 0;

  while (order <= MAX_ORDER && frames[buddy_idx].used == 0 &&
         order == frames[buddy_idx].order) {
    buddy_addr = (void *)(FRAME_BASE + PAGE_SIZE * buddy_idx);
    page_remove_node(&alloc_mem[order], buddy_addr);

    order += 1;

    page_idx = page_idx < buddy_idx ? page_idx : buddy_idx;

    buddy_idx = page_idx ^ (1 << order);
  }
  if (frames[buddy_idx].used == 0) {
    free_idx = page_idx < buddy_idx ? page_idx : buddy_idx;
  } else {
    free_idx = page_idx;
  }
  frames[free_idx].order = order;
  void *victim_addr = (void *)(FRAME_BASE + free_idx * PAGE_SIZE);
  page_insert_list(&alloc_mem[order], victim_addr);

  uart_printf("Free address %p in Order %d\n", victim_addr, order);
}

void test_buddy() {
  int test_size = 5;
  void *a[test_size];
  uart_puts("\r\n----------ALLOC PAGE----------\n");
  for (int i = 0; i < test_size; i++) {
    a[i] = alloc_page(test_size);
  }
  uart_puts("---------END ALLOCATE PAGE---------\n");

  uart_puts("\r\n-----FREE--------\n");
  for (int idx; idx < test_size; idx++) {
    freepages(a[idx]);
  }
  uart_puts("\r\n------END------\n");
}

void test_dynamic_alloc() {
  uart_puts("Allocate a1\n");
  int *a1 = kalloc(sizeof(int));
  uart_printf("address %p in a1\n", a1);
  int *a2 = kalloc(sizeof(int));
  uart_printf("address %p in a2\n", a2);

  kfree((void *)a1);
  kfree((void *)a2);
}
