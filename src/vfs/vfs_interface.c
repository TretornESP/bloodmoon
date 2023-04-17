#include "vfs_interface.h"
#include "vfs.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/string.h"

int vfs_file_open(const char* path, int flags, int mode) {
    printf("vfs_file_open(%s, %d, %d)\n", path, flags, mode);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        res = mount->fst->file_open(mount->internal_index, native_path_buffer, flags, mode);
    }
    free(native_path_buffer);
    return res;
}

int vfs_file_close(int fd) {
    printf("vfs_file_close(%d)\n", fd);
    char * path = get_full_path_from_fd(fd);
    if (path == 0) {
        return -1;
    }

    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        res = mount->fst->file_close(mount->internal_index, fd);
    }
    free(native_path_buffer);
    free(path);
    return res;
}

uint64_t vfs_file_read(int fd, void* buffer, uint64_t size) {
    printf("vfs_file_read(%d, %p, %ld)\n", fd, buffer, size);
    char * path = get_full_path_from_fd(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->file_read(mount->internal_index, fd, (char*)buffer, size);
    }
    free(native_path_buffer);
    free(path);
    return res;
}

uint64_t vfs_file_write(int fd, void* buffer, uint64_t size) {
    printf("vfs_file_write(%d, %p, %ld)\n", fd, buffer, size);
    char * path = get_full_path_from_fd(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->file_write(mount->internal_index, fd, (char*)buffer, size);
    }
    free(native_path_buffer);
    free(path);
    return res;
}

int vfs_file_creat(const char* path, int mode) {
    printf("vfs_file_creat(%s, %d)\n", path, mode);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        res = mount->fst->file_creat(mount->internal_index, native_path_buffer, mode);
    }
    free(native_path_buffer);
    return res;
}

uint64_t vfs_file_seek(int fd, uint64_t offset, int whence) {
    printf("vfs_file_seek(%d, %ld, %d)\n", fd, offset, whence);
    char * path = get_full_path_from_fd(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    uint64_t res = 0;
    if (mount != 0) {
        res = mount->fst->file_seek(mount->internal_index, fd, offset, whence);
    }
    free(native_path_buffer);
    free(path);
    return res;
}

int vfs_dir_open(const char* path) {
    printf("vfs_dir_open(%s)\n", path);

    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        res = mount->fst->dir_open(mount->internal_index, native_path_buffer);
    }
    free(native_path_buffer);
    return res;

}

int vfs_dir_close(int fd) {
    printf("vfs_dir_close(%d)\n", fd);

    char * path = get_full_path_from_dir(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->dir_close(mount->internal_index, fd);
    }

    free(native_path_buffer);
    free(path);
    return res;
}

void vfs_file_flush(int fd) {
    printf("vfs_file_flush(%d)\n", fd);

    char * path = get_full_path_from_fd(fd);
    if (path == 0) {
        return;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    if (mount != 0) {
        mount->fst->file_flush(mount->internal_index, fd);
    }

    free(native_path_buffer);
    free(path);
}

int vfs_dir_load(int fd) {
    printf("vfs_dir_load(%d)\n", fd);

    char * path = get_full_path_from_dir(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->dir_load(mount->internal_index, fd);
    }

    free(native_path_buffer);
    free(path);
    return res;
}

void vfs_dir_list(char* name) {
    int fd = vfs_dir_open(name);
    if (fd < 0) {
        printf("Error opening directory %s\n", name);
        return;
    }

    int res = vfs_dir_load(fd);
    if (res < 0) {
        printf("Error loading directory %s\n", name);
        return;
    }

    printf("Directory %s contents:\n", name);
    char name_buffer[256];
    uint32_t type;
    uint32_t name_len;
    while (vfs_dir_read(fd, name_buffer, &type, &name_len) > 0) {
        printf("DIR ENTRY: %s, %d, %d\n", name_buffer, type, name_len);
    }

    vfs_dir_close(fd);
    return;
}

int vfs_dir_read(int fd, char* name, uint32_t * name_len, uint32_t * type) {
    //printf("vfs_dir_read(%d)\n", fd);

    char * path = get_full_path_from_dir(fd);
    if (path == 0) {
        return -1;
    }
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->dir_read(mount->internal_index, fd, name, name_len, type);
    }

    free(native_path_buffer);
    free(path);
    return res;
}

int vfs_mkdir(const char* path, int mode) {
    printf("vfs_dir_creat(%s)\n", path);

    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    int res = -1;
    if (mount != 0) {
        res = mount->fst->dir_creat(mount->internal_index, native_path_buffer, mode);
    }
    free(native_path_buffer);
    return res;
}

int vfs_rename(const char* path, const char* name) {return -1;}

int vfs_remove(const char* path, uint8_t force) {
    printf("vfs_remove(%s)\n", path);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);

    int res = -1;
    if (mount != 0) {
        res = mount->fst->prepare_remove(mount->internal_index, native_path_buffer);
        if (res == 0) return -2;
        if (is_safe_for_removing(native_path_buffer, force) == 0) return -1;
        res = mount->fst->remove(mount->internal_index, native_path_buffer);
    }
    free(native_path_buffer);

    return res;
}

int vfs_chmod(const char* path, int mode) {return -1;}

void vfs_debug_by_path(const char* path) {
    printf("vfs_debug_by_path(%s)\n", path);
    char * native_path_buffer = malloc(strlen(path) + 1);
    struct vfs_mount* mount = get_mount_from_path(path, native_path_buffer);
    if (mount != 0) {
        mount->fst->debug();
    }
    free(native_path_buffer);
}