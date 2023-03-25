#ifndef _VFS_ADAPTERS_H
#define _VFS_ADAPTERS_H
#include <stdint.h>

int vfs_file_open(const char*, int, int);
int vfs_file_close(int);
uint64_t vfs_file_read(int, void*, uint64_t);
uint64_t vfs_file_write(int, void*, uint64_t);
uint64_t vfs_file_seek(int, uint64_t, int);
int vfs_file_creat(const char*, int);

int vfs_dir_open(const char*);
int vfs_dir_close(int);
int vfs_dir_read(int);
int vfs_mkdir(const char*, int);

int vfs_rename(const char*, const char*);
int vfs_remove(const char*);
int vfs_chmod(const char*, int);
void vfs_debug_by_path(const char* path);
#endif