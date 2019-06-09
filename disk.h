#ifndef _DISK_H
#define _DISK_H

#include "file.h"

void disk_read_super_block(super_block * block);
void disk_write_super_block(super_block * block);
void disk_read_index_table(inode_table * table);
void disk_write_index_table(inode_table * table);
void disk_read_block(int block_offset, void * block, int n);
void disk_write_block(int block_offset, void * block, int n);
int disk_read(int where, void * ptr, int size);
int disk_write(int where, void * ptr, int size);
int disk_open(const char *path);

#endif