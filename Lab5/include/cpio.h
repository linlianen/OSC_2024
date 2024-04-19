#ifndef __CPIO_H_
#define __CPIO_H_

#include <stdint.h>
/* #include "include/stat.h" */

#include <stddef.h>
#include <sys/stat.h>
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4

struct cpio_newc_header
{
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
  char content_buf[0];
} __attribute__((packed));

struct cpio_info
{
  /// The number of files in the CPIO archive
  unsigned int file_count;
  /// The maximum size of a file name
  unsigned int max_path_sz;
};

typedef struct __cpio_file
{
  uint8_t *pathname;
  uint32_t file_size;
  uint8_t *data_ptr;
  struct __cpio_file *next;
} cpio_file_ll;

typedef void (*cpio_parse_func)(int type, const char *name, const void *data,
                                uint32_t size);
void cpio_list_file(const char *cpio_buf);
char *findFile(char *name, char *cpio_addr);
unsigned int hex_to_int(const char* buf, int len);
void cpio_cat(char* initrd, char* name);

void align(void *size, size_t s);
void cpio_load_prog(char *filename, const char *cpio_buf);
int cpio_parse_header(
	struct cpio_newc_header *archive,
	const char **filename,
	unsigned long *_filesize, void **data,
	struct cpio_newc_header **next);
  const char *cpio_get_content(const char *path, const char *cpio_buf);

#endif
