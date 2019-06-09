#ifndef _DEFS_H
#define _DEFS_H

#define DISK_SIZE 262144
#define BLOCK_SIZE 1024
#define EMPTY_INDEX_SIZE 16
#define INDEX_TABLE_SIZE 50
#define FILE_LIST_SIZE 14
#define DIR_LIST_SIZE 42
#define MAX_FILE_NAME 14

#define EMPTY_BLOCK_LIST_SIZE 255

#define DISK_BLOCK_OFFSET(X) (5*BLOCK_SIZE + (X) * BLOCK_SIZE)
#define IDX_TABLE_OFFSET(X) (BLOCK_SIZE + sizeof(inode) * (X))

#define MODE_TEST(A, B) (((A) & (B)) == (B))

#define node(i) (i_table.inode[i])


#endif