#include <sys/time.h>
#include "op.h"

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


int add_directory_item(int directory_idx,directory_item *item){
	inode cur_inode = node(directory_idx);
	int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
	for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){ 
			if(d_table.item[j].inode == 0){
				memcpy(&d_table.item[j],item,sizeof(directory_item));
				disk_write(block[i],&d_table,sizeof(d_table));
				return 0;
			}
        }
    }
	return -1;
}

int get_directory_item(int directory_idx,int idx,directory_item *item){
	inode cur_inode = node(directory_idx);
	int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
	for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){ 
			if(d_table.item[j].inode == idx){
				memcpy(item,&d_table.item[j],sizeof(directory_item));
				return 0;
			}
        }
    }
	return -1;
}

int rm_directory_item(int directory_idx,int idx){
	inode cur_inode = node(directory_idx);
	int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
	for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){ 
			if(d_table.item[j].inode == idx){
				d_table.item[j].inode = 0;
				disk_write(block[i],&d_table,sizeof(d_table));
				return 0;
			}
        }
    }
	return -1;
}

int is_empty_directory(int directory_idx){
	inode cur_inode = node(directory_idx);
	int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
	for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){ 
			if(d_table.item[j].inode != 0){
				return 0;
			}
        }
    }
	return 1;
}