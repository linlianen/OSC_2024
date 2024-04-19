#include "../include/vfs.h"
#include "../include/lib.h"
#include "../include/mm.h"
#include "../include/thread.h"
#include "../include/tmpfs.h"
struct vnode *current_dir;
struct mount *rootfs;

extern thread *currThread;
char folderArray[16][100];

void rootfs_init() {
  struct filesystem *tmpfs =
      (struct filesystem *)kalloc(sizeof(struct filesystem));
  tmpfs->name = (char *)kalloc(16);
  my_strcpy(tmpfs->name, "tmpfs");
  
  tmpfs->setup_mount = &tmpfs_setup_mount;
  register_fs(tmpfs);

  rootfs = (struct mount *)kalloc(sizeof(struct mount));
  tmpfs->setup_mount(tmpfs, rootfs);
  current_dir = rootfs->root;
}

int register_fs(struct filesystem *fs) {
  if (my_strcmp(fs->name, "tmpfs") == 0) {
    return register_tmpfs();
  }
  return -1;
}

int split_folder(char *pathname) {
  int path_i = 0;
  int name_i = 0;
  int folder_i = 0;

  for (int k = 0; k < 16; k++) {
    for (int l = 0; l < 100; l++) {
      folderArray[k][l] = 0;
    }
  }

  while (pathname[path_i] != '\0') {

    folderArray[folder_i][name_i] = pathname[path_i];

    name_i++;
    if (pathname[path_i] == '/') {
      folder_i++;
      name_i = 0;
    }

    path_i++;
  }
  int k = 0;
  return folder_i;
}
struct file *vfs_open(const char *pathname, int flags) {
  int folder_num = split_folder(pathname);

  struct vnode *dir = current_dir;
  struct vnode *target = 0;
  struct tmpfs_internal *dir_file;
  struct tmpfs_internal *file;
  struct file *fd;
  int file_level = 0;
  if (my_strcmp(folderArray[0], "/") == 0) {
    dir = rootfs->root;
  }

  if (folder_num == 1) {
    if (flags == O_CREAT) {
      // create

      int ret_file =
          dir->v_ops->create(dir, &target, folderArray[1], FILE_REGULAR);
      fd = (struct file *)kalloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
      return fd;
    } else {
      // open
      file_level = vfs_traverse(dir, &target, 0, folder_num);
      fd = (struct file *)kalloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_pos = 0;
      fd->f_ops = target->f_ops;
      return fd;
    }

  } else {
    file_level = vfs_traverse(dir, &target, 0, folder_num);
    // uart_printf("folder level :%d\n", file_level);
    // uart_printf("folder number : %d\n", folder_num);
    dir = target;
    if (file_level < folder_num && flags == O_CREAT) {
      int ret_file =
          dir->v_ops->create(dir, &target, folderArray[++file_level], FILE_REGULAR);

      fd = (struct file *)kalloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
      return fd;
    } else {

      fd = (struct file *)kalloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_pos = 0;
      fd->f_ops = target->f_ops;
      return fd;
    }
  }
  // 1. 回傳最接近檔案的目錄
  //  --> 若沒有找到但有O_CREATE則create(tmpfs)
  //  -->若有找到則open(tmpfs)
  // 2. 創建文件描述
  // 3. 回傳檔案描述符
}
int vfs_read(struct file *file, void *buf, size_t len) {
    //read len byte from opened file to buf
    //return reading size
    return file->f_ops->read(file, buf, len);
}

int vfs_write(struct file *file, const void *buf, size_t len) {
    //write len byte from buf to the opened file
    //return the writing size (i)
    return file->f_ops->write(file, buf, len);
}
int vfs_mkdir(const char *pathname) {
  struct vnode *dir = current_dir;
  struct vnode *target = 0;
  struct tmpfs_internal *parent_tmpfs;
  void *a = currThread->kernel_stack;
  int folder_level = 0;
  char *dir_name = (char *)kalloc(16);
  int folder_num = split_folder(pathname);
  if (my_strcmp(folderArray[0], "/") == 0) {
    dir = rootfs->root;
  }

  if (folder_num > 1) {
    folder_level = vfs_traverse(dir, &target, 0, folder_num);

  } else {
    target = rootfs->root;
  }

  dir = target;

  if (folder_level == folder_num - 1 || folder_num == 1) {
    // create folder
    int ret_create = dir->v_ops->create(dir, &target, folderArray[folder_num],
                                        FILE_DIRECTORY);

    uart_printf("success mkdir '%s'\n", folderArray[folder_num]);
  }
}

int vfs_traverse(struct vnode *dir, struct vnode **target, int level,
                 int folder_num) {

  char *component_name;
  int ret_level = 0;
  if (folder_num == 1 && my_strcmp(folderArray[0], "/") == 0) {

    *target = rootfs->root;
    return 0;
  } else if (folder_num > 1 && level < folder_num) {
    while (level <= folder_num) {

      if (my_strcmp(folderArray[level], "/") == 0) {
        dir = rootfs->root;
        level++;
        continue;
      } else {
        component_name = my_strtok(folderArray[level], "/");
      }
      ret_level = dir->v_ops->lookup(dir, target, component_name);

      if (ret_level == 0) {
        continue;
      } else if (ret_level == 1) {
        level = level + 1;
        dir = *target;
      } else if (ret_level == 2) {
        break;
      } else if (ret_level == 3) {
        level--;
        break;
      } else if (ret_level == -2) {
        break;
      } else if (ret_level == -1) {
        level--;
        dir = *target;
      }
    }

    return level;
  }
}
