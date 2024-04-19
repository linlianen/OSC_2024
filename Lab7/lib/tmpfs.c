#include "../include/tmpfs.h"
#include "../include/lib.h"
#include "../include/list.h"
#include "../include/mm.h"
#include "../include/uart.h"

struct vnode_operations *tmpfs_v_ops;
struct file_operations *tmpfs_f_ops;
int register_tmpfs() {

  tmpfs_v_ops =
      (struct vnode_operations *)kalloc(sizeof(struct vnode_operations));
  tmpfs_v_ops->lookup = &tmpfs_lookup;
  tmpfs_v_ops->create = &tmpfs_create;
  tmpfs_f_ops =
      (struct file_operations *)kalloc(sizeof(struct file_operations));
  tmpfs_f_ops->read = &tmpfs_read;
  tmpfs_f_ops->write = &tmpfs_write;
  tmpfs_f_ops->open = &tmpfs_open;
  return 0;
}

int tmpfs_setup_mount(struct filesystem *fs, struct mount *_mount) {
  _mount->fs = fs;
  _mount->root = tmpfs_new_node(NULL, "/", FILE_DIRECTORY);

  return 0;
}

long tmpfs_lseek(struct file *file, long offset, int whence) { return 0; }

struct vnode *tmpfs_new_node(struct tmpfs_internal *parent, const char *name,
                             enum FILE_TYPE type) {

  struct vnode *new_node = (struct vnode *)kalloc(sizeof(struct vnode));
  struct tmpfs_internal *new_internal =
      (struct tmpfs_internal *)kalloc(sizeof(struct tmpfs_internal));
  my_strcpy(new_internal->name, name);
  new_internal->parent = parent;
  new_internal->type = type;
  new_internal->child_num = 0;
  new_internal->buf->size = 0;
  if (type == FILE_REGULAR) {
    new_internal->buf->data = kalloc(MAX_FILESIZE);
  } else {
    new_internal->buf->data = 0;
  }
  new_internal->node = new_node;
  new_node->f_ops = tmpfs_f_ops;
  new_node->v_ops = tmpfs_v_ops;
  new_node->internal = (void *)new_internal;
  new_node->mount = 0;

  return new_node;
}
int tmpfs_write(struct file *file, const void *buf, size_t len) {

  struct tmpfs_internal *entry = (struct tmpfs_internal *)file->vnode->internal;

  if (((struct tmpfs_internal *)file->vnode->internal)->type != FILE_REGULAR)
    return 0;

  // copy data from buffer
  char *dest = &((char *)entry->buf->data)[file->f_pos];
  char *src = (char *)buf;
  int i = 0;
  for (; i < len && entry->buf->size + i < MAX_FILESIZE; i++) {
    dest[i] = src[i];
  }
  // update the size
  entry->buf->size += i;

  return i;
}

/*
 *
 return value :
 "-2"-- can't find
 "-1"-- to the parent level
 "0"-- in the exist level
 "1"-- find folder
 "2"-- find file
 "3"-- can't find the folder and file
 */
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target,
                 const char *componentName) {
  struct tmpfs_internal *entry = (struct tmpfs_internal *)dir_node->internal;

  if (entry->type != FILE_DIRECTORY) {
    uart_printf("This is not directory\n");
    return -2;
  } else {
    if (my_strcmp((char *)componentName, ".") == 0) {
      *target = entry->node;
      return 0;
    } else if (my_strcmp((char *)componentName, "..") == 0) {
      *target = entry->parent->node;
      return -1;

    } else {
      for (int i = 0; i < MAX_DIRECT; i++) {
        entry = ((struct tmpfs_internal *)dir_node->internal)->child[i];

        if (my_strcmp((char *)componentName, entry->name) == 0 &&
            entry->type == FILE_DIRECTORY) {
          *target = entry->node;
          return 1;
        } else if (my_strcmp(componentName, entry->name) == 0 &&
                   entry->type == FILE_REGULAR) {
          *target = entry->node;
          return 2;
        }
      }
      *target = dir_node;
      uart_printf(
          "we can't find the folder named '%s', you should mkdir that\n",
          componentName);
      return 3;
    }
  }
}
int tmpfs_read(struct file *file, void *buf, size_t len) {
  struct tmpfs_internal *internal =
      (struct tmpfs_internal *)file->vnode->internal;
  if (internal->type != FILE_REGULAR)
    return 0;

  char *dest = (char *)buf;
  char *src = &((char *)internal->buf->data)[file->f_pos];
  int i = 0;
  for (; i < len && i < internal->buf->size; i++) {
    dest[i] = src[i];
  }

  return i;
}
int tmpfs_open(struct vnode *file_node, struct file **target) { return 0; }
int tmpfs_close(struct file *file) { return 0; }
long tmpfs_getsize(struct vnode *vd) { return 0; }

int tmpfs_create(struct vnode *dir_node, struct vnode **target,
                 const char *component_name, enum FILE_TYPE type) {
  struct tmpfs_internal *parent_tmpfs =
      (struct tmpfs_internal *)dir_node->internal;
  struct vnode *new_node =
      tmpfs_new_node(parent_tmpfs, component_name, FILE_DIRECTORY);
  parent_tmpfs->child[parent_tmpfs->child_num++] =
      (struct tmpfs_internal *)new_node->internal;

  *target = new_node;
  return 0;
}
