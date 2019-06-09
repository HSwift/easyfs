#include "op.h"

int op_unlink(const char *path){
    printf("[DEBUG] unlink(%s)\n", path);
    char parent_path[255];
    strcpy(parent_path,path);
    char *t = strrchr(parent_path,'/');
    *(t+1) = '\0';
    char *filename = strrchr(path,'/');
    filename += 1;

    int parent_idx = inode_get_idx_by_path(parent_path);
    printf("[DEBUG] parent_path %s[%d] name:%s\n",parent_path,parent_idx,filename);

    int cur_idx = inode_get_idx_by_path(path);

    if(cur_idx < 0)
        return cur_idx;
    if(!S_ISREG(node(cur_idx).mode))
        return -EISDIR;
    if(rm_directory_item(parent_idx,cur_idx) < 0)
        return -ENOENT;

    node(cur_idx).links_count -= 1;
    if(node(cur_idx).links_count == 0){
        release_blocks(cur_idx);
        node(cur_idx).mode = 0;
    }
    
    save_index_table();

    return 0;
}