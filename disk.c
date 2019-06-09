#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <errno.h>

#include "op.h"

static int disk_fd = -1;

int disk_open(const char *path)
{
    disk_fd = open(path, O_RDWR);
    if (disk_fd < 0) {
        return -errno;
    }

    return 0;
}

int disk_read(int where, void * ptr, int size)
{

    static pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
    ssize_t pread_ret;

    pthread_mutex_lock(&read_lock);
    printf("Disk Read: 0x%jx +0x%zx\n", where, size);
    pread_ret = pread(disk_fd, ptr, size, where);
    pthread_mutex_unlock(&read_lock);
    if (size == 0) printf("[WARNING] Read operation with 0 size\n");

    return pread_ret;
}

int disk_write(int where, void * ptr, int size)
{

    static pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
    ssize_t pread_ret;

    pthread_mutex_lock(&read_lock);
    printf("Disk Write: 0x%jx +0x%zx\n", where, size);
    pread_ret = pwrite(disk_fd, ptr, size, where);
    pthread_mutex_unlock(&read_lock);
    if (size == 0) printf("[WARNING] Read operation with 0 size\n");

    return pread_ret;
}

void disk_read_super_block(super_block * block)
{
	disk_read(0, block, sizeof(super_block));
}

void disk_write_super_block(super_block * block)
{
	disk_write(0, block, sizeof(super_block));
}

void disk_read_index_table(inode_table * table)
{
	disk_read(BLOCK_SIZE, table, sizeof(inode_table));
}

void disk_write_index_table(inode_table * table)
{
	disk_write(BLOCK_SIZE, table, sizeof(inode_table));
}