#ifndef __DTB_H_
#define __DTB_H_
#include <stdint.h>
#include <stddef.h>
#define FDT_MAGIC 0xD00DFEED
#define FDT_VERSION 0x00000011
#define FDT_NULL 0x00000000
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

#define FDT_CPIO_INITRAMFS_PROPNAME "linux,initrd-start"


struct fdt_node_prop{
	uint32_t len;
	uint32_t nameoff;
};

typedef void (*dtb_callback)(int type, const char *name, const void *data, uint32_t size);
int fdtb_parse(void *dtb, int print, dtb_callback callback);

#endif