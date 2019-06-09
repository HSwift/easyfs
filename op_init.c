#include "op.h"

void *op_init(struct fuse_conn_info *info)
{
    printf("[INFO] Using FUSE protocol %d.%d\n", info->proto_major, info->proto_minor);

    disk_read_super_block(&s_block);
	disk_read(s_block.free_blocks, &f_block, sizeof(f_block));
	disk_read_index_table(&i_table);

    printf("[DEBUG] get root at 0,mode=%d,uid=%d\n",node(0).mode,node(0).uid);

    return NULL;
}
