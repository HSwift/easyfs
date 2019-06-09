#ifndef _INODE_H
#define _INODE_H

int inode_get_by_path(const char *path, struct inode *cur_inode);
int get_inode_num(const char **path);
int dir_lookup(int parent, const char *name, int namelen);
int get_empty_idx();
int inode_get_idx_by_path(const char *path);

#endif