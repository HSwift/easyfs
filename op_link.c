#include "op.h"

int op_link(const char *src, const char *dst){
    printf("[DEBUG] link(%s) to(%s)\n", src, dst);
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
    
    
    item.name_len = strlen(filename);
    strcpy(item.name,filename);
    node(cur_idx).links_count += 1; 

    if(add_directory_item(parent_idx,&item) == -1)
        return -ENOSPC;

    save_index_table();
    return 0;
}