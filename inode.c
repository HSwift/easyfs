#include "op.h"
#include <string.h>

int get_empty_idx(){
	for (int i = 0; i < INDEX_TABLE_SIZE; i++)
		if (node(i).mode == 0)
		{
			printf("[DEBUG] Allocate idx: %d\n", i);
			return i;
		}
	return -1;
}

void put_chars(const char *path,int len){
    int i = 0;
    while(i < len)putchar(path[i++]);
}

static int get_path_token_len(const char *path)
{
    int len = 0;
    while (path[len] != '/' && path[len]) len++;
    return len;
}

int dir_lookup(int parent, const char *name, int namelen){
    inode cur_inode;
    cur_inode = i_table.inode[parent];
    int ret = -1;
    int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE) + 1;
    
    printf("[DIR_LOOKUP] ");
    put_chars(name,namelen);
    printf("\n");

    for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){
            if(d_table.item[j].inode == 0)continue;
            if(strlen(d_table.item[j].name) == namelen && strncmp(d_table.item[j].name,name,namelen) == 0){
                printf("[DIR_FOUND] %s[%d]\n",d_table.item[j].name,d_table.item[j].inode);
                ret = d_table.item[j].inode;
                return ret;
            }
        }
    }
    return ret;
}

int get_inode_num(const char **path){
    int next = 0,ret;
    do {
        if (**path == '/') *path = *path + 1;
        int path_len = get_path_token_len(*path);
        ret = next;

        if (path_len == 0) {
            return ret;
        }

        next = dir_lookup(ret, *path, path_len);
        if(next == -1)
            return -ENOENT;
        if (next) *path += path_len;
    } while(next != NULL);
}

int inode_get_by_path(const char *path, inode *cur_inode){
    if(strcmp(path,"/") == 0){
        cur_inode = &node(0);
        return 0;
    }
    int inode_num = get_inode_num(&path);
    if(inode_num < 0)
        return inode_num;
    cur_inode = &node(inode_num);
    return inode_num;
}

int inode_get_idx_by_path(const char *path){
    if(strcmp(path,"/") == 0){
        return 0;
    }
    return get_inode_num(&path);
}