#include "op.h"

int op_getattr(const char *path, struct stat *stbuf)
{
    inode cur_inode;
    int ret = 0;

    printf("[DEBUG] getattr(%s)\n", path);

    memset(stbuf, 0, sizeof(struct stat));
    ret = inode_get_idx_by_path(path);

    if (ret < 0) {
        return ret;
    }

    cur_inode = node(ret);

    stbuf->st_mode = cur_inode.mode;
    stbuf->st_nlink = cur_inode.links_count;
    stbuf->st_size = cur_inode.size;
    stbuf->st_blocks = cur_inode.block;
    stbuf->st_uid = cur_inode.uid;
    stbuf->st_gid = cur_inode.gid;
    stbuf->st_atime = cur_inode.atime;
    stbuf->st_mtime = cur_inode.mtime;
    stbuf->st_ctime = cur_inode.ctime;

    printf("[DEBUG] inode=%d,mode=%d,size=%d,uid=%d\n",ret,stbuf->st_mode,stbuf->st_size,stbuf->st_uid);

    printf("[DEBUG] getattr done\n");

    return 0;
}
