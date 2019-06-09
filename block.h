#ifndef _BLOCK_H
#define _BLOCK_H

#include "file.h"

void save_super_block();
void save_index_table();
void release_blocks(int idx);
int get_empty_block();
void add_empty_block(int offset);

#endif