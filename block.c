#include <string.h>

#include "disk.h"
#include "op.h"

void save_super_block()
{
	disk_write_super_block(&s_block);
}

void save_index_table()
{
	disk_write_index_table(&i_table);
}

int get_empty_block()
{
	if (s_block.free_blocks_count == 0)
		return -1;
	s_block.free_blocks_count--;
	int r = f_block.list[s_block.free_blocks_count];
	if (s_block.free_blocks_count == 0)
	{
		s_block.free_blocks = f_block.next;
		if (s_block.free_blocks == -1)
			s_block.free_blocks_count = 0;
		else
		{
			s_block.free_blocks_count = EMPTY_BLOCK_LIST_SIZE;
			disk_read(f_block.next, &f_block, sizeof(f_block));
		}
	}
	save_super_block();
	printf("[DEBUG] get Block: %d\n", r);
	return r;
}

void add_empty_block(int offset)
{
	if (offset == -1) return;
	if (s_block.free_blocks_count == EMPTY_BLOCK_LIST_SIZE)
	{
		f_block.list[0] = offset;
		f_block.next = s_block.free_blocks;
		disk_write(offset, &f_block, sizeof(f_block));
		s_block.free_blocks_count = 1;
		s_block.free_blocks = offset;
	}
	else
	{
		f_block.list[s_block.free_blocks_count++] = offset;
		disk_write(s_block.free_blocks, &f_block, sizeof(f_block));
	}
	printf("[DEBUG] add Block: %d\n", offset);
	save_super_block();
}

void release_blocks(int idx){
	inode cur_inode = node(idx);
	int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
	for(int i = 0;i <= block_count;++i){
		if(block[i] == 0)continue;
		add_empty_block(block[i]);
		block[i] = 0;
	}
}