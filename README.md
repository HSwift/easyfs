# easyfs
操作系统课作业

## 用法
1. `make`
2. `fallocate -l 256K file.fs && ./mkfs file.fs`
3. `./easyfs -f -s file.fs path/to/mount`

## To-Do
解决大文件随机读写时的bug
