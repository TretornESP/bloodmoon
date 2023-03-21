#include "vfs_adapters.h"
#include "vfs.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/string.h"

int vfs_file_open(const char* path, int flags, int mode) {
    printf("vfs_file_open(%s, %d, %d)\n", path, flags, mode);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        printf("native_path_buffer: %s\n", native_path_buffer);
        res = mount->fst->file_open(mount->internal_index, native_path_buffer, flags, mode);
    } else {
        printf("mount == 0\n");
    }
    free(native_path_buffer);
    return res;
}

int vfs_file_close(int fd) {
    printf("vfs_file_close(%d)\n", fd);
    char * path = get_path_from_fd(fd);

    char * native_path_buffer = malloc(strlen(path) + 1);
    struct mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        printf("native_path_buffer: %s\n", native_path_buffer);
        res = mount->fst->file_close(mount->internal_index, fd);
    } else {
        printf("mount == 0\n");
    }
    free(native_path_buffer);
    return res;
}

uint64_t vfs_file_read(int fd, void* buffer, uint64_t size) {
    printf("vfs_file_read(%d, %p, %ld)\n", fd, buffer, size);
    char * path = get_path_from_fd(fd);
    printf("Path: %s\n", path);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        printf("native_path_buffer: %s\n", native_path_buffer);
        res = mount->fst->file_read(mount->internal_index, fd, (char*)buffer, size);
    } else {
        printf("mount == 0\n");
    }
    free(native_path_buffer);
    return res;
}
uint64_t vfs_file_write(int fd, void* buffer, uint64_t size) {return 0;}
uint64_t vfs_file_seek(int fd, uint64_t offset, int whence) {return 0;}
int vfs_dir_open(const char* path) {
    printf("vfs_dir_open(%s)\n", path);

    char * native_path_buffer = malloc(strlen(path) + 1);
    struct mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        printf("native_path_buffer: %s\n", native_path_buffer);
        mount->fst->dir_open(mount->internal_index, native_path_buffer);
    } else {
        printf("mount == 0\n");
    }
    free(native_path_buffer);
    return res;

}
int vfs_dir_close(int dir) {return -1;}
int vfs_dir_read(int dir) {return -1;}
int vfs_mkdir(const char* path, int mode) {return -1;}
int vfs_rename(const char* path, const char* name) {return -1;}
int vfs_remove(const char* path) {return -1;}
int vfs_chmod(const char* path, int mode) {return -1;}
