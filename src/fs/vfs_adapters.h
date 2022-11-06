#ifndef _VFS_ADAPTERS_H
#define _VFS_ADAPTERS_H
#include <stdint.h>

char register_partition(const char*, uint32_t);
uint8_t unregister_partition(char);
uint8_t detect(const char*, uint32_t);
int file_open(const char*, int, int);
int file_close(int);
uint64_t file_read(int, void*, uint64_t);
uint64_t file_write(int, void*, uint64_t);
uint64_t file_seek(int, uint64_t, int);
int file_sync(int);
int dir_open(const char*);
int dir_close(int);
int dir_read(int);
int mkdir(const char*, int);
int rename(const char*, const char*);
int remove(const char*);
int chmod(const char*, int);

#endif