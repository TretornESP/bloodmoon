#include "vfs_compat.h"
#include "../../util/string.h"

struct file_descriptor_entry open_file_table[VFS_COMPAT_MAX_OPEN_FILES] = {0};

struct file_descriptor_entry * vfs_compat_get_file_descriptor(uint32_t fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return 0;
    return &open_file_table[fd];
}

int get_fd(const char* path, int flags, int mode) {
    static int fd = 0;
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_FILES; i++) {
        if (open_file_table[fd].loaded == 0) {
            open_file_table[fd].loaded = 1;
            open_file_table[fd].flags = 0;
            open_file_table[fd].mode = 0;
            open_file_table[fd].offset = 0;
            strncpy(open_file_table[fd].name, path, strlen(path));
            return fd++;
        }
        fd++;
        if (fd >= VFS_COMPAT_MAX_OPEN_FILES) fd = 0;
    }
    return -1;
}

int release_fd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return -1;
    open_file_table[fd].loaded = 0;
    return 0;
}