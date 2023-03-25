#include "vfs_compat.h"
#include "../../util/string.h"
#include "../../util/printf.h"

struct file_descriptor_entry open_file_table[VFS_COMPAT_MAX_OPEN_FILES] = {0};
struct dir_t open_directory_table[VFS_COMPAT_MAX_OPEN_DIRECTORIES] = {0};

struct dir_t * vfs_compat_get_dir(uint32_t fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return 0;
    struct dir_t * entry = &open_directory_table[fd];
    return entry;
}

struct file_descriptor_entry * vfs_compat_get_file_descriptor(uint32_t fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return 0;
    struct file_descriptor_entry * entry = &open_file_table[fd];
    return entry;
}

int get_fd(const char* path, const char* mount, int flags, int mode) {
    if (strlen(path) >  VFS_FDE_NAME_MAX_LEN) return -1;
    if (strlen(mount) > VFS_FDE_NAME_MAX_LEN) return -1; 
    static int fd = 0;
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_FILES; i++) {
        if (open_file_table[fd].loaded == 0) {
            open_file_table[fd].loaded = 1;
            open_file_table[fd].flags = flags;
            open_file_table[fd].mode = mode;
            open_file_table[fd].offset = 0;
            strncpy(open_file_table[fd].name, path, strlen(path));
            strncpy(open_file_table[fd].mount, mount, strlen(path));

            return fd++;
        } else {
            fd++;
            if (fd >= VFS_COMPAT_MAX_OPEN_FILES) fd = 0;
        }
    }
    return -1;
}

int get_dirfd(const char* path const char* mount, int flags, int mode) {
    if (strlen(path) >  VFS_FDE_NAME_MAX_LEN) return -1;
    if (strlen(mount) > VFS_FDE_NAME_MAX_LEN) return -1; 
    static int fd = 0;
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DIRECTORIES; i++) {
        if (open_directory_table[fd].fd.loaded == 0) {
            open_directory_table[fd].fd.loaded = 1;
            open_directory_table[fd].fd.flags = flags;
            open_directory_table[fd].fd.mode = mode;
            open_directory_table[fd].fd.offset = 0;
            strncpy(open_directory_table[fd].fd.name, path, strlen(path));
            strncpy(open_directory_table[fd].fd.mount, mount, strlen(path));
            
            open_directory_table[fd].index = 0;
            open_directory_table[fd].number = 0;

            return fd++;
        } else {
            fd++;
            if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) fd = 0;
        }
    }
    return -1;
}

int release_fd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return -1;
    open_file_table[fd].loaded = 0;
    return 0;
}

int get_dirfd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return -1;
    open_directory_table[fd].fd.loaded = 0;
    return 0;
}