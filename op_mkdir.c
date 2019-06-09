#include "op.h"
#include <string.h>

int op_mkdir(const char *path, mode_t mode){
    printf("[DEBUG] mkdir(%s) mode(%d)\n", path, mode);

    char parent_path[255];
    strcpy(parent_path,path);
    char *t = strrchr(parent_path,'/');
    *(t+1) = '\0';
    char *filename = strrchr(path,'/');
    filename += 1;

    int parent_idx = inode_get_idx_by_path(parent_path);
    printf("[DEBUG] parent_path %s[%d] name:%s\n",parent_path,parent_idx,filename);
    if(parent_idx < 0)
        return parent_idx;

    if(inode_get_idx_by_path(path) >= 0)
        return -EEXIST;

    int new_inode = get_empty_idx();
    if(new_inode == -1)
        return -ENOSPC;
    create_file(new_inode,S_IFDIR|mode,1024);
    directory_item new_item;
    new_item.file_type = S_IFDIR;
    new_item.inode = new_inode;
    new_item.name_len = strlen(filename);
    strcpy(new_item.name,filename);

    if(add_directory_item(parent_idx,&new_item) == -1)
        return -ENOSPC;

    save_super_block();
    save_index_table();
    return 0;
}