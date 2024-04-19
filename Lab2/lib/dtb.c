#include "../include/dtb.h"
#include "../include/uart.h"
#include "../include/cpio.h"
#include "../include/lib.h"

#define DEBUG() uart_printf("%s, %s, %d \n", __FILE__, __FUNCTION__, __LINE__)
struct fdt_header
{
  uint32_t magic;
  uint32_t totalsize;
  uint32_t off_dt_struct;
  uint32_t off_dt_strings;
  uint32_t off_mem_rsvmap;
  uint32_t version;
  uint32_t last_camp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

static uint64_t _pad_to_4(void *num);
static void dump(void *arr, const uint32_t len);
static uint32_t rev32(uint32_t val);
static uint64_t endian_rev(void *input, int dtype_size);
char* initrd_ptr, *initramfs_end;
char* address, *size;
uint32_t get_be_int(const void *ptr) {
  const unsigned char *bytes = (unsigned char *)ptr;
  uint32_t ret = bytes[3];
  ret |= bytes[2] << 8;
  ret |= bytes[1] << 16;
  ret |= bytes[0] << 24;

  return ret;
}


 void get_initrd(int type, const char *name, const void *data, uint32_t size) {
  while(1){
  uart_puts("2454543\n");
  }
  if (type == FDT_PROP && !my_strcmp(name, "linux,initrd-start")) {
    initrd_ptr = (char *)(uintptr_t)get_be_int(data);
    // uart_puts("Address");
    // uart_printf("0x%08X", (uintptr_t)get_be_int(data));
  } else if (type == FDT_PROP && !my_strcmp(name, "linux,initrd-end"))
  {
    initramfs_end = (char*)(uintptr_t)get_be_int(data);   
    // initramfs_end = (char *)(uintptr_t)get_be_int(data);
  }
}
int fdtb_parse(void *dtb_addr, int print, dtb_callback callback)
{
  
  // DEBUG();
  struct fdt_header *header = (struct fdt_header *)dtb_addr;
  uint8_t *structure_block_end = (uint8_t *)header + rev32(header->off_dt_struct) + rev32(header->size_dt_struct);
  if (print)
    uart_printf("[Debug] magic=0x%08X, totalsize=0x%08X, off_dt_struct=0x%08X, off_dt_strings=0x%08X, version=0x%08X\r\n",
                rev32(header->magic), rev32(header->totalsize), rev32(header->off_dt_struct), rev32(header->off_dt_strings), rev32(header->version));

  // Check for magic num and version
  if (rev32(header->magic) != FDT_MAGIC || rev32(header->version) != FDT_VERSION)
  {
    uart_printf("Error, fdtb_parse(), expect magic,version=0x%08X, 0x%08X, got=0x%08X, 0x%08X. ", FDT_MAGIC, FDT_VERSION, rev32(header->magic), rev32(header->version));
    uart_printf("dtb_addr=0x%pX, totalsize=0x%08X\r\n", dtb_addr, rev32(header->totalsize));
    return -1;
  }
  // while(1){
  //   uart_puts("123\n");
  // }
  // DEBUG();
  int reserved_memory = 0;
  int depth = 0;
  uint8_t *cur = (uint8_t *)header + rev32(header->off_dt_struct);
  int finish = 0;
  struct fdt_node_prop *prop = NULL;
  char *propName = NULL;
  char *propVal = NULL;
  void* program_counter;
  //   while(1){
  //   uart_puts("45678\n");
  // }
  while (!finish)
  {
    const uint32_t token = rev32(*((uint32_t *)cur));
    uart_printf("token : %ld\n", token);
    switch (token)
    {
      case FDT_BEGIN_NODE:
        cur += 4;
        depth++;
        if (print)
          uart_printf("BEGIN_NODE depth=%d, cur=%p, nodename=%s\r\n", depth, cur, cur);
        if(!my_strcmp((char*)cur,"reserved-memory"))
        {
          reserved_memory = 1;
        }else{
          reserved_memory = 0;
        }
        cur += my_strlen((char *)(cur)) + 1; // +1 for null terminated string
        cur += _pad_to_4(cur);
        break;

      case FDT_PROP:
        cur += 4;
        prop = (struct fdt_node_prop *)cur;
        cur += sizeof(struct fdt_node_prop);
        propVal = (char *)cur;
        propName = (char *)(header) + rev32(header->off_dt_strings) + rev32(prop->nameoff);
        DEBUG();
        if (print)
          uart_printf("PROP_NODE  depth=%d, cur=%p, prop.len=%2d, .nameoff=0x%8X, propName=%s, propVal=%8X\r\n",
                      depth, cur, rev32(prop->len), rev32(prop->nameoff), propName, propVal);
        // TODO: Get linux,initrd-start start address here
        if (callback != NULL && !my_strcmp(propName, "linux,initrd-start" ))
        {
          void *addr = (void *)(uint64_t)rev32(*((uint32_t *)propVal));
          // while (1) {
          //   // uart_puts("235545\n");
          //   uart_printf("callback %p\n", callback);
          // }
          // program_counter = __builtin_return_address(0);
          // // asm volatile ("mov %0, pc" : "=r" (program_counter));
          // while (1) {
          //   uart_printf("Program counter : 0x%x\n", program_counter);
          // }
          callback(FDT_PROP, propName, (void*)propVal, rev32((*(uint64_t *)propVal)));
  
        }
        cur += rev32(prop->len);
        cur += _pad_to_4(cur);
        break;

      case FDT_END_NODE:
        cur += 4;
        if (print)
          uart_printf("END_NODE   depth=%d, cur=%p\r\n", depth, cur);
        depth--;
        break;

      case FDT_NOP:
        cur += 4; // next token (each token is 4 bytes)
        break;

      case FDT_END:
        cur += 4;
        if (print)
          uart_printf("END_____   depth=%d, cur=%p\r\n", depth, cur);
        finish = 1; // parser reaches the end, break the while
        break;

      default:
        uart_printf("Error, token=0x%08X, cur=0x%p, cur[0..64]=\r\n", token, cur);
        dump(cur, 64); // dump 64 bytes to see what's around
        cur += 4;
        break;
    }
    if (cur > structure_block_end)
    {

      uart_printf("Error, reaching the end of structure block, cur=%p, structure_block_end=%p\r\n", cur, structure_block_end);
      break;
    }
  }
  while(1){
    uart_puts("9999\n");
  }
  if (print){
    uart_printf("Parsing ends here\r\n");
  }
  return rev32(header->totalsize);
}

static uint64_t _pad_to_4(void *num)
{
  uint64_t modded = ((uint64_t)num) % 4;
  return modded == 0 ? 0 : 4 - modded;
}

static void dump(void *arr, const uint32_t len)
{
  uint8_t *ptr = (uint8_t *)arr;
  for (uint32_t i = 0; i < len; i++)
    uart_printf(" %02X", *ptr++);
  uart_printf("\r\n");
}

static uint32_t rev32(uint32_t val)
{
  return (uint32_t)endian_rev(&val, 4);
}

static uint64_t endian_rev(void *input, int dtype_size)
{

  const uint8_t *ptr = (uint8_t *)input;
  uint64_t ret = 0;

  switch (dtype_size)
  {

    case 1:
      break;

    case 2:
      ret = (ptr[0] << 8) | ptr[1];
      break;

    case 4:
      ret = (ptr[0] << 24) |
        (ptr[1] << 16) |
        (ptr[2] << 8) |
        ptr[3];
      break;

    default:
      uart_printf("[Error] Endian transformation(%d) not implemented\n", dtype_size);
      break;
  }

  return ret;
}
