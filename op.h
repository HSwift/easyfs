#ifndef _OP_H
#define _OP_H

#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "disk.h"
#include "file.h"
#include "inode.h"
#include "block.h"
#include "defs.h"

super_block s_block;
free_block f_block;
inode_table i_table;

int op_rmdir(const char *path);
int op_unlink(const char *path);
void op_destroy(void *private_data);
int op_access(const char *path, int mask);
void *op_init(struct fuse_conn_info *info);
int op_link(const char *src, const char *dst);
int op_chmod(const char *path, mode_t mode);
int op_mkdir(const char *path, mode_t mode);
int op_rename(const char * src,const char * dst);
int op_truncate(const char *path, off_t new_size);
int op_getattr(const char *path, struct stat *stbuf);
int op_mknod(const char *path, mode_t mode, dev_t rdev);
int op_open(const char *path, struct fuse_file_info *fi);
int op_utimens(const char *path, const struct timespec tv[2]);
int op_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int op_write(const char *path,const char *buf, size_t size, off_t offset,struct fuse_file_info *fi);
int op_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi);

#endif