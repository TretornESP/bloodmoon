#include "vfs_adapters.h"

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
