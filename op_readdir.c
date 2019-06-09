#include "op.h"

int op_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    printf("[DEBUG] readdir(%s)\n", path);
    inode cur_inode;
	int ret = inode_get_idx_by_path(path);
    if(ret < 0)
        return ret;
    cur_inode = node(ret);

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

    int *block = cur_inode.block;
    int size = cur_inode.size;
    int block_count = (size / BLOCK_SIZE);
    for(int i = 0;i < block_count;++i){
        directory_table d_table;
        disk_read(block[i],&d_table,sizeof(d_table));
        for(int j = 0;j < DIR_LIST_SIZE;++j){
            //printf("[FILE] %s[%d]\n",d_table.item[j].name,d_table.item[j].inode);
            if(d_table.item[j].inode == 0)
                continue;
            filler(buf, d_table.item[j].name, NULL, 0);
        }
    }
	return 0;
}