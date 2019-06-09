#ifndef _FILE_H
#define _FILE_H
#include "defs.h"

typedef struct super_block {
    unsigned int magic;
    unsigned int inodes_count;
    unsigned int blocks_count;
    unsigned int first_data_block;
    unsigned int free_blocks_count;
    unsigned int free_blocks;
} super_block;

typedef struct inode {
    unsigned short mode;
    unsigned short uid;
    unsigned int size;
    unsigned int atime;
    unsigned int ctime;
    unsigned int mtime;
    unsigned short gid;
    unsigned short links_count;
    unsigned int block[FILE_LIST_SIZE];
} inode;

typedef struct inode_table {
	inode inode[INDEX_TABLE_SIZE];
} inode_table;

typedef struct free_block {
    int list[EMPTY_BLOCK_LIST_SIZE];
    int next;
} free_block;

typedef struct directory_item {
    unsigned int inode;
    unsigned short name_len;
    unsigned short file_type;
    char name[MAX_FILE_NAME];
} directory_item;

typedef struct directory_table {
    directory_item item[DIR_LIST_SIZE];
} directory_table;

void create_file(int idx,unsigned short mode,int size);
int add_directory_item(int directory_idx,directory_item *item);
int rm_directory_item(int directory_idx,int idx);
int is_empty_directory(int directory_idx);

#endif