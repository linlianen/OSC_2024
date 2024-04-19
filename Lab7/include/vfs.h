#ifndef _VFS_H_
#define _VFS_H_
#include "../include/tmpfs.h"
#define O_CREAT 00000100
#include <stddef.h>
struct vnode{
  struct mount *mount;            //屬於哪個mount
  struct vnode_operations* v_ops; // (create, lookup)
  struct file_operations* f_ops;  //(write, read)
  void* internal;                 //節點內容(各檔案系統實作)
};


struct  dentry_list{
  char* dir_list;
  struct dentry_list* next;
};

struct file{
  struct vnode* vnode;        //屬於哪個節點

  size_t f_pos;               //檔案位置
  struct file_operations* f_ops; //(create, write)
  int flags;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write) (struct file* file, const void* buf, size_t len);
  int (*read) (struct file* file, void* buf, size_t len);
  int (*open) (struct vnode* file_open, struct file** target);
  int (*close) (struct file *file);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name, enum FILE_TYPE type);
};




void rootfs_init();
int register_fs(struct filesystem* fs);

struct file* vfs_open(const char* pathname, int flags);
// 1. release the file descriptor
int vfs_close(struct file* file);
// 1. write len byte from buf to the opened file.
// 2. return written size or error code if an error occurs.
int vfs_write(struct file* file, const void* buf, size_t len);
// 1. read min(len, readable file data size) byte to buf from the opened file.
// 2. return read size or error code if an error occurs.
int vfs_read(struct file* file, void* buf, size_t len);

int vfs_traverse(struct vnode* dir, struct vnode**target, int level, int folder_num);
int vfs_mkdir(const char* pathname);
#endif
