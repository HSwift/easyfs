#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "file.h"
#include "defs.h"

super_block s_block;
free_block f_block;
inode_table i_table;

char *file_path;

void disk_read(int offset, void * ptr, int n)
{
	FILE * fp = fopen(file_path, "r+b");
	fseek(fp, offset, SEEK_SET);
	fread(ptr, n, 1, fp);
	fclose(fp);
}

void disk_write(int offset, void * ptr, int n)
{
	FILE * fp = fopen(file_path, "r+b");
	fseek(fp, offset, SEEK_SET);
	fwrite(ptr, n, 1, fp);
	fclose(fp);
}

void disk_read_super_block(super_block * block)
{
	disk_read(0, block, sizeof(super_block));
}

void disk_write_super_block(super_block * block)
{
	disk_write(0, block, sizeof(super_block));
}

void disk_read_index_table(inode_table * table)
{
	disk_read(BLOCK_SIZE, table, sizeof(inode_table));
}

void disk_write_index_table(inode_table * table)
{
	disk_write(BLOCK_SIZE, table, sizeof(inode_table));
}

void disk_read_block(int block_offset, void * block, int n)
{
	disk_read(DISK_BLOCK_OFFSET(block_offset), block, n);
}

void disk_write_block(int block_offset, void * block, int n)
{
	disk_write(DISK_BLOCK_OFFSET(block_offset), block, n);
}

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
	printf("[DEBUG] 获取Block: %d\n", r);
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
	printf("[DEBUG] 释放Block: %d\n", offset);
	save_super_block();
}

void create_file(int idx,unsigned short mode,int size){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    node(idx).mode = mode;
    node(idx).uid = getuid();
    node(idx).size = size;
    node(idx).ctime = tv.tv_sec;
    node(idx).atime = tv.tv_sec;
    node(idx).mtime = tv.tv_sec;
    node(idx).gid = getgid();
    node(idx).links_count = 1;
    node(idx).block[0] = get_empty_block();
    save_index_table();
}

int init_disk(){
    s_block.free_blocks_count = 0;
    s_block.free_blocks = -1;
    f_block.next = -1;
    s_block.magic = 0x0C2A0E;
    
    int p = -1;
    int max_block = (DISK_SIZE - BLOCK_SIZE - sizeof(inode_table)) / BLOCK_SIZE;
    int group_size = EMPTY_BLOCK_LIST_SIZE;
    int max_group = max_block / group_size;
    int n_remain_block = max_block % group_size;
    printf("[INFO] block数:%d\n",max_block);
    printf("[INFO] group数:%d\n",max_group);
    printf("[INFO] remain block数:%d\n",n_remain_block);
    s_block.blocks_count = max_block;
    s_block.inodes_count = INDEX_TABLE_SIZE;
    s_block.first_data_block = BLOCK_SIZE*2;
    for (int i = 0; i < max_group; i++)
    {
        f_block.list[0] = DISK_BLOCK_OFFSET(i * group_size);
        for (int j = 1; j < EMPTY_BLOCK_LIST_SIZE; j++)
            f_block.list[j] = DISK_BLOCK_OFFSET(i * group_size + j);
        f_block.next = p;
        p = DISK_BLOCK_OFFSET(i * group_size);
        disk_write_block(i * group_size, &f_block, sizeof(f_block));
    }
    if (n_remain_block > 0)
    {
        f_block.list[0] = DISK_BLOCK_OFFSET(max_group * group_size);
        for (int i = 1; i < n_remain_block; i++)
            f_block.list[i] = DISK_BLOCK_OFFSET(max_group * group_size + i);
        f_block.next = p;
        disk_write_block(max_group * group_size, &f_block, sizeof(f_block));
        s_block.free_blocks_count = n_remain_block;
        s_block.free_blocks = DISK_BLOCK_OFFSET(max_group * group_size);
    }
    else
    {
        s_block.free_blocks_count = EMPTY_BLOCK_LIST_SIZE;
        s_block.free_blocks = DISK_BLOCK_OFFSET((max_group - 1) * group_size);
    }
    save_super_block();
    create_file(0,S_IFDIR|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,1024);
}

int main(int argc,char **argv){
    long int size;

    if(argc == 1){
        printf("Usage: %s [disk file]\n",argv[0]);
        return -1;
    }

    file_path = argv[1];
    FILE *fp = fopen(file_path, "rb");
    if(fp == NULL){
        printf("[ERR] 文件不存在或权限不足\n");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    printf("[INFO] 文件大小:%d\n",size);
    fclose(fp);

    if(size < 262144){
        printf("[ERR] 文件至少大于256K\n");
        return -1;
    }

    printf("\n");

    init_disk();

    return 0;
}
