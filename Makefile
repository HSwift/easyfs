COMPILER = gcc
FILESYSTEM_FILES = main.c block.c disk.c file.c inode.c op_getattr.c op_init.c op_open.c op_read.c op_readdir.c op_mkdir.c op_rmdir.c op_mknod.c op_unlink.c op_write.c op_other.c op_link.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o easyfs `pkg-config fuse --cflags --libs`
	$(COMPILER) mkfs.c -o mkfs
	@echo 'To Mount: ./easyfs -f [mount point]'

clean:
	rm easyfs
