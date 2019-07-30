#include "op.h"

void do_write(const char *buf,size_t size, off_t offset,int idx){
    char *tmp = buf;
    int block_count = ((offset + size) / BLOCK_SIZE);
    int block_start = offset / BLOCK_SIZE;
    int block_start_off = offset % BLOCK_SIZE;
    int *blocks = node(idx).block;
    int write_size = BLOCK_SIZE;
    for(int i = block_start;i <= block_count;i++){
        if(i == block_count)
            write_size = (offset + size) % BLOCK_SIZE;
        disk_write(blocks[i] + block_start_off, tmp, write_size);
        tmp += write_size;
        block_start_off = 0;
    }
}

int op_write(const char *path,const char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi){
    printf("[DEBUG] write(%s) size(%d) offset(%d)\n",path,size,offset);
    int idx = fi->fh;
    size_t len;

    if(node(idx).uid == getuid()){
        if(MODE_TEST(node(idx).mode, S_IWUSR) == 0)
            return -EACCES;
    }else{
        if(MODE_TEST(node(idx).mode, S_IWOTH) == 0)
            return -EACCES;
    }
    if(size == 0)
        return 0;

    int new_size = size + offset;
    int old_size = node(idx).size;
    if(new_size > old_size){
        int new_block_count = new_size / BLOCK_SIZE;
        int old_block_count = old_size / BLOCK_SIZE;
        if(new_block_count > FILE_LIST_SIZE)
            return -ENOSPC;
        while(new_block_count - old_block_count){
            old_block_count++;
            int t = get_empty_block();
            if(t < 0)
                return -ENOSPC;
            node(idx).block[old_block_count] = t;
        }
    }
    do_write(buf,size,offset,idx);

    node(idx).size = new_size;

    save_index_table();
    return size;
}