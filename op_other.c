#include "op.h"

void op_destroy(void *private_data){
    printf("[DEBUG] destroy\n");
    save_index_table();
    save_super_block();
}

int op_utimens(const char *path, const struct timespec tv[2]){
    printf("[DEBUG] utimens(%s)\n",path);
    int idx = inode_get_idx_by_path(path);
    node(idx).atime = tv[0].tv_sec;
    node(idx).mtime = tv[1].tv_sec;
    save_index_table();
    return 0;
}

int op_access(const char *path, int mask){
    printf("[DEBUG] access(%s) mask(%d)\n",path,mask);
    return 0;
}

int op_truncate(const char *path, off_t new_size){
    printf("[DEBUG] truncate(%s) mask(%d)\n",path,new_size);
    int idx = inode_get_idx_by_path(path);
    int old_size = node(idx).size;
    int new_block_count = new_size / BLOCK_SIZE;
    int old_block_count = old_size / BLOCK_SIZE;
    if(new_block_count > old_block_count){
        if(new_block_count > FILE_LIST_SIZE)
            return -ENOSPC;
        while(new_block_count - old_block_count){
            old_block_count++;
            int t = get_empty_block();
            if(t < 0)
                return -ENOSPC;
            node(idx).block[old_block_count] = t;
        }
    }else if(new_block_count < old_block_count){
        while(old_block_count - new_block_count){
            old_block_count--;
            if(node(idx).block[old_block_count] != 0)
                add_empty_block(node(idx).block[old_block_count]);
            node(idx).block[old_block_count] = 0;
        }
    }
    save_index_table();
    return 0;
}

int op_chmod(const char *path, mode_t mode){
    printf("[DEBUG] chmod(%s) mode(%d)\n",path,mode);
    int idx = inode_get_idx_by_path(path);
    if(node(idx).uid != getuid())
        return -EACCES;
    node(idx).mode = mode;
    save_index_table();
    return 0;
}

int op_chown(const char *path, int uid, int gid){
    printf("[DEBUG] chown(%s) uid(%d) gid(%d)\n",path,uid,gid);
    int idx = inode_get_idx_by_path(path);
    if(node(idx).uid != getuid())
        return -EACCES;
    node(idx).uid = uid;
    node(idx).gid = gid;
    save_index_table();
    return 0;
}

int op_rename(const char * src,const char * dst){
    printf("[DEBUG] rename(%s) to(%s)\n", src, dst);
    char parent_src[255];
    strcpy(parent_src,src);
    char *t = strrchr(parent_src,'/');
    *(t+1) = '\0';
    char *filename = strrchr(src,'/');
    filename += 1;

    int src_parent_idx = inode_get_idx_by_path(parent_src);
    printf("[DEBUG] parent_path %s[%d] name:%s\n",parent_src,src_parent_idx,filename);

    int cur_idx = inode_get_idx_by_path(src);
    if(cur_idx < 0)
        return cur_idx;

    directory_item item;
    if(get_directory_item(src_parent_idx,cur_idx,&item) == -1)
        return -ENOENT;
    

    char parent_dst[255];
    strcpy(parent_dst,dst);
    t = strrchr(parent_dst,'/');
    *(t+1) = '\0';
    filename = strrchr(dst,'/');
    filename += 1;

    int parent_idx = inode_get_idx_by_path(parent_dst);
    printf("[DEBUG] parent_path %s[%d] name:%s\n",parent_dst,parent_idx,filename);
    
    if(parent_idx < 0)
        return parent_idx;
    if(inode_get_idx_by_path(dst) >= 0)
        return -EEXIST;
    
    if(rm_directory_item(src_parent_idx,cur_idx) == -1)
        return -ENOENT;
    
    item.name_len = strlen(filename);
    strcpy(item.name,filename);

    if(add_directory_item(parent_idx,&item) == -1)
        return -ENOSPC;

    save_index_table();
    return 0;
}