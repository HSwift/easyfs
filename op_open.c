#include "op.h"

int op_open(const char *path, struct fuse_file_info *fi)
{   
    printf("[DEBUG] open(%s) flag(%d)\n", path, fi->flags);

    fi->fh = inode_get_idx_by_path(path);
    printf("[DEBUG] %s is inode %d\n", path, fi->fh);

    return 0;
}