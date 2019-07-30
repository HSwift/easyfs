/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using high-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello.c `pkg-config fuse3 --cflags --libs` -o hello
 *
 * ## Source code ##
 * \include hello.c
 */


#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>
#include <stddef.h>

#include "op.h"

/*
 * Command line options
 *
 * We can't set default values for the char* fields here because
 * fuse_opt_parse would attempt to free() them when the user specifies
 * different values on the command line.
 */
static struct options {
	const char *disk;
} options;

static int opt_proc(void *data, const char *arg, int key,
                        struct fuse_args *outargs)
{
    (void) data;
    (void) outargs;

    switch (key) {
    case FUSE_OPT_KEY_OPT:
        return 1;
    case FUSE_OPT_KEY_NONOPT:
        if (!options.disk) {
            options.disk = strdup(arg);
            return 0;
        }
        return 1;
    default:
        fprintf(stderr, "internal error\n");
        abort();
    }
}

static struct fuse_operations oper = {
	.init       = op_init,
	.getattr	= op_getattr,
	.readdir	= op_readdir,
	.open		= op_open,
	.read		= op_read,
    .mkdir      = op_mkdir,
    .rmdir      = op_rmdir,
    .mknod      = op_mknod,
    .unlink     = op_unlink,
    .write      = op_write,
    .destroy    = op_destroy,
    .utimens    = op_utimens,
    .access     = op_access,
    .truncate   = op_truncate,
    .chmod      = op_chmod,
    .rename     = op_rename,
    .link       = op_link,
    .chown      = op_chown,
};

int main(int argc, char *argv[])
{
    int res;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // Default options
    options.disk = NULL;

    if (fuse_opt_parse(&args, &options, NULL, opt_proc) == -1) {
        return EXIT_FAILURE;
    }

    if (!options.disk) {
        fprintf(stderr, "Usage: %s <disk> <mountpoint>\n", argv[0]);
        exit(1);
    }

    if (disk_open(options.disk) < 0) {
        fprintf(stderr, "disk_open: %s: %s\n", options.disk,
                strerror(errno));
        return EXIT_FAILURE;
    }

    res = fuse_main(args.argc, args.argv, &oper, NULL);

    fuse_opt_free_args(&args);

    return res;
}   
