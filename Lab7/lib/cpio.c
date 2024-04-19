
#include "../include/uart.h"
#include "../include/lib.h"
#include "../include/cpio.h"
#include "../include/timer.h"
#include <stdint.h>
#include <sys/stat.h>
#include <stddef.h>
#include "cpio.h"

#define KSTACK_SIZE 0x2000
#define USTACK_SIZE 0x2000
/*Prototype*/
extern void *set_exception_vector_table_el1();
static size_t align_up(size_t size, int alignment);
static unsigned long parse_hex_str(char *s, unsigned int max);
/* char to Int */
static unsigned int
to_int(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return 10 + c - 'A';
	if (c >= 'a' && c <= 'f')
		return 10 + c - 'a';
	return -1;
}

/* hex to Int */
unsigned int
hex_to_int(const char *buf, int len)
{
	unsigned int out = 0;
	for (int i = 0; i < len; ++i)
	{
		out <<= 4;
		out += to_int(buf[i]);
	}
	return out;
}

/* String copy in Cpio file (size)*/
static int
cpio_strncmp(const char *s1, const char *s2, unsigned long long n)
{
	unsigned char c1 = '\0';
	unsigned char c2 = '\0';
	if (n >= 4)
	{
		unsigned long long n4 = n >> 2;
		do
		{
			c1 = (unsigned char)*s1++;
			c2 = (unsigned char)*s2++;
			if (c1 == '\0' || c1 != c2)
				return c1 - c2;
			c1 = (unsigned char)*s1++;
			c2 = (unsigned char)*s2++;
			if (c1 == '\0' || c1 != c2)
				return c1 - c2;
			c1 = (unsigned char)*s1++;
			c2 = (unsigned char)*s2++;
			if (c1 == '\0' || c1 != c2)
				return c1 - c2;
			c1 = (unsigned char)*s1++;
			c2 = (unsigned char)*s2++;
			if (c1 == '\0' || c1 != c2)
				return c1 - c2;
		} while (--n4 > 0);
		n &= 3;
	}
	while (n > 0)
	{
		c1 = (unsigned char)*s1++;
		c2 = (unsigned char)*s2++;
		if (c1 == '\0' || c1 != c2)
			return c1 - c2;
		n--;
	}
	return c1 - c2;
}



void cpio_cat(char* initrd, char* name){
	uintptr_t ptr = (uintptr_t)initrd;
	const struct cpio_newc_header *hdr;
  unsigned namesize;
  unsigned filesize;
	while (1)
	{
		hdr = (struct cpio_newc_header *)ptr;
		if (strncmp((char *)hdr->c_magic, (char *)"070701", 6))
		{
			return;
		}

		namesize = hex_to_int(hdr->c_namesize, 8);
		filesize = hex_to_int(hdr->c_filesize, 8);
		if (!my_strcmp(hdr->content_buf, "TRAILER!!!"))
		{
			break;
		}
		ptr += sizeof(struct cpio_newc_header);
		if(!(my_strcmp((char*)ptr, name))){
    break;
    }
		ptr = align_up(ptr + namesize, 4);
		ptr = align_up(ptr + filesize, 4);
	}
  if(filesize > 0){
  uart_puts("content: \n");
  uart_putsn((char*)(align_up(ptr+namesize, 4)), filesize);
  }

}
/* string copy in Cpio file */
static char *
cpio_strcpy(char *to, const char *from)
{
	char *save = to;
	while (*from != 0)
	{
		*to = *from;
		to++;
		from++;
	}
	return save;
}

/* Get String Length in cpio */
static unsigned int
cpio_strlen(const char *str)
{
	const char *s;
	for (s = str; *s; ++s)
	{
	}
	return (s - str);
}

/* Parse Cpio header */
int cpio_parse_header(
	struct cpio_newc_header *archive,
	const char **filename,
	unsigned long *_filesize, void **data,
	struct cpio_newc_header **next)

/*
 * Input:
 *  Archive : the address of archive file
 *  filename : the filename we get in Archive
 * _filesize : the filesize of archive we get
 * data : a filedata we get in archive
 * next : point to next header file
 * */
{
	unsigned long filesize;
	struct cpio_newc_header *header = archive;
	/* Ensure magic header exists. */
	if (cpio_strncmp(header->c_magic, CPIO_HEADER_MAGIC,
					 sizeof(header->c_magic)) != 0)
		return -1;

	/* Get filename and file size. */
	filesize = parse_hex_str(header->c_filesize, sizeof(header->c_filesize));
	*filename = ((char *)archive) + sizeof(struct cpio_newc_header);

	/* Ensure filename is not the trailer indicating EOF. */
	if (cpio_strncmp(*filename, CPIO_FOOTER_MAGIC, sizeof(CPIO_FOOTER_MAGIC)) == 0)
		return 1;

	/* Find offset to data. */
	unsigned long filename_length = parse_hex_str(header->c_namesize,
												  sizeof(header->c_namesize));
	*data = (void *)align_up(((unsigned long)archive) + sizeof(struct cpio_newc_header) + filename_length, CPIO_ALIGNMENT);
	*next = (struct cpio_newc_header *)align_up(((unsigned long)*data) + filesize, CPIO_ALIGNMENT);
	if (_filesize)
	{
		*_filesize = filesize;
	}
	return 0;
}

/* Parse an ASCII hex string into an integer. */
static unsigned long
parse_hex_str(char *s, unsigned int max_len)
{
	unsigned long r = 0;
	unsigned long i;

	for (i = 0; i < max_len; i++)
	{
		r *= 16;
		if (s[i] >= '0' && s[i] <= '9')
		{
			r += s[i] - '0';
		}
		else if (s[i] >= 'a' && s[i] <= 'f')
		{
			r += s[i] - 'a' + 10;
		}
		else if (s[i] >= 'A' && s[i] <= 'F')
		{
			r += s[i] - 'A' + 10;
		}
		else
		{
			return r;
		}
		continue;
	}
	return r;
}

void align(void *size, size_t s)
{
	unsigned int *x = (unsigned int *)size;
	if ((*x) & (s - 1))
	{
		(*x) += s - ((*x) & (s - 1));
	}
}

static size_t align_up(size_t size, int alignment)
{
	return (size + alignment - 1) & -alignment;
}

void *cpio_get_file(void *archive, const char *name, uint64_t *size)
{
	struct cpio_newc_header *header =
		(struct cpio_newc_header *)archive;

	while (1)
	{
		struct cpio_newc_header *next;
		void *result;
		const char *current_filename;

		int error = cpio_parse_header(header, &current_filename, size, &result, &next);

		if (error)
			return NULL;
		if (cpio_strncmp(current_filename, name, -1) == 0)
			return result;
		header = next;
	}
}

char *findFile(char *name, char *cpio_addr)
{
	char *addr = cpio_addr;
	const struct cpio_newc_header *hdr = (struct cpio_newc_header *)cpio_addr;
	while (my_strcmp((char *)(addr + sizeof(struct cpio_newc_header)), "TRAILER!!!") != 0)
	{
		if ((my_strcmp((char *)(addr + sizeof(struct cpio_newc_header)), name) == 0))
		{
			return addr;
		}
		struct cpio_newc_header *header = (struct cpio_newc_header *)addr;
		unsigned int pathname_size = hex_to_int(header->c_namesize, 8);
		unsigned int file_size = hex_to_int(header->c_filesize, 8);
		unsigned int headerPathname_size = sizeof(struct cpio_newc_header) + pathname_size;

		align(&headerPathname_size, 4);
		align(&file_size, 4);
		addr += (headerPathname_size + file_size);
	}
	return 0;
}
void cpio_list_file(const char *cpio_buf)
{
	uintptr_t ptr = (uintptr_t)cpio_buf;
	const struct cpio_newc_header *hdr;

	uart_puts("Archived files:\n");
	while (1)
	{
		hdr = (struct cpio_newc_header *)ptr;
		if (strncmp((char *)hdr->c_magic, (char *)"070701", 6))
		{
			return;
		}

		unsigned namesize = hex_to_int(hdr->c_namesize, 8);
		unsigned filesize = hex_to_int(hdr->c_filesize, 8);
		if (!my_strcmp(hdr->content_buf, "TRAILER!!!"))
		{
			break;
		}

		ptr += sizeof(struct cpio_newc_header);
		uart_puts((char *)ptr);
		uart_puts("\n");
		ptr = align_up(ptr + namesize, 4);
		ptr = align_up(ptr + filesize, 4);
	}
}

const char *cpio_get_content(const char *path, const char *cpio_buf)
{
	uintptr_t ptr = (uintptr_t)cpio_buf;
	const struct cpio_newc_header *hdr;

	while (1)
	{
		hdr = (struct cpio_newc_header *)ptr;
		if (my_strcmp(hdr->c_magic, "070701"))
		{
			return NULL;
		}
		if (!my_strcmp(hdr->content_buf, "TRAILER!!!"))
		{
			return NULL;
		}

		uint32_t namesize = hex_to_int(hdr->c_namesize, 8);
		uint32_t filesize = hex_to_int(hdr->c_filesize, 8);
		uint32_t mode = hex_to_int(hdr->c_mode, 8);

		if (!my_strcmp(hdr->content_buf, path))
		{
			if (S_ISDIR(mode))
				return "directory";

			if (filesize)
			{
				ptr += sizeof(struct cpio_newc_header);
				ptr = align_up(ptr + namesize, 4);
				return (char *)ptr;
			}
			return "";
		}
		else
		{
			ptr += sizeof(struct cpio_newc_header);
			ptr = align_up(ptr + namesize, 4);
			ptr = align_up(ptr + filesize, 4);
		}
	}
}

void cpio_load_prog(char *filename, const char *cpio_buf)
{
	char *exe_format = ".img";
	char *verify_format = strstr(filename, exe_format);
	uart_puts(verify_format);

	char *prog_addr = findFile(filename, cpio_buf);

	void *put_addr = (void *)0x200000;

	if (prog_addr)
	{
		struct cpio_newc_header *header = (struct cpio_newc_header *)prog_addr;

		unsigned int pathname_size = hex_to_int(header->c_namesize, 8);
		uint32_t file_size = hex_to_int(header->c_filesize, 8);

		uint32_t headerPathName_size = sizeof(struct cpio_newc_header) + pathname_size;

		align(&headerPathName_size, 4);
		align(&file_size, 4);

		// 使用串口打印信息，提示正在加載該文件的文件名
		uart_puts("----------------");
		uart_puts((prog_addr + sizeof(struct cpio_newc_header)));
		uart_puts("----------------\n");

		char *file_content = prog_addr + headerPathName_size;
		unsigned char *target = (unsigned char *)put_addr;
		while (file_size--)
		{
			*target = *file_content;
			target++;
			file_content++;
		} // 在此由低位往高位擺以及相反會怎樣？？

		uart_puts("moving");
		// 啟用核心計時器
		// core_timer_enable();
		arm_core_timer_enable();

		// set spsr_el1 to 0x3c0 and elr_el1 to the program’s start address.
		//  set the user program’s stack pointer to a proper position by setting sp_el0.
		//  issue eret to return to the user code.
		//  asm volatile("mov x0, 0x340  \n");
		/*
		   adr     x0, set_exception_vector_table_el1
	msr     vbar_el1, x0*/
		asm volatile("mov x0, 0x3c0  \n");
		// asm volatile("bl set_exception_vector_table_el1 \n");
		// asm volatile("adr x0, exception_vetor_table_el1\n");
		// asm volatile("msr vbar_el1, x0\n");
		asm volatile("msr spsr_el1, x0   \n");
		asm volatile("msr elr_el1, %0    \n" ::"r"(put_addr));
		asm volatile("msr sp_el0, %0    \n" ::"r"(put_addr + USTACK_SIZE));
		asm volatile("eret   \n");
		return;
	}
}
