#include <sys/types.h>
#include "op.h"

void do_read(char *buf,size_t size, off_t offset,inode *cur_inode){
    int block_count = ((offset + size) / BLOCK_SIZE);
    int block_start = offset / BLOCK_SIZE;
    int block_start_off = offset % BLOCK_SIZE;
    int *blocks = cur_inode->block;
    int read_size = BLOCK_SIZE;
    for(int i = block_start;i <= block_count;i++){
        if(i == block_count)
            read_size = (offset + size) % BLOCK_SIZE;
        disk_read(blocks[i] + block_start_off, buf, read_size);
        buf += read_size;
        block_start_off = 0;
    }
}

int op_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi)
{
    printf("[DEBUG] read(%s) size(%d) offset(%d)\n",path,size,offset);

    inode cur_inode;
    size_t len;
    int idx = fi->fh;

    cur_inode = node(idx);

    if(node(idx).uid == getuid()){
        if(MODE_TEST(node(idx).mode, S_IRUSR) == 0)
            return -EACCES;
    }else{
        if(MODE_TEST(node(idx).mode, S_IROTH) == 0)
            return -EACCES;
    }


    if(size == 0)
        return 0;

	len = cur_inode.size;
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		do_read(buf, size, offset, &cur_inode);
	} else
		size = 0;

	return size;
}
