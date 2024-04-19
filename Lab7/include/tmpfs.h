#ifndef _TMPFS_H_
#define _TMPFS_H_

#include "../include/vfs.h"
#include <stddef.h>
#define TMPFS_COMP_LEN  16
#define MAX_DIRECT      16
#define MAX_FILESIZE    0x1000

struct tmpfs_buf
{
  unsigned int size;
  void* data;
};

enum FILE_TYPE 
{
  FILE_DIRECTORY, 
  FILE_REGULAR,
};

struct tmpfs_internal{
  char name[TMPFS_COMP_LEN];
  enum FILE_TYPE type;
  struct tmpfs_internal* parent;
  struct tmpfs_internal *child[MAX_DIRECT];
  struct vnode *node;
  int child_num;
  struct tmpfs_buf* buf;
};

struct vnode* tmpfs_new_node(struct tmpfs_internal *parent, const char *name, enum FILE_TYPE type);
int tmpfs_setup_mount(struct filesystem*fs, struct mount* _mount);
int register_tmpfs();
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* componentName); 
int tmpfs_create(struct vnode* dir_node,struct vnode** target, const char* componentName, enum FILE_TYPE type); 
int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_read(struct file* file,void* buf, size_t len);
int tmpfs_open(struct vnode *file_node, struct file **target);


#endif
