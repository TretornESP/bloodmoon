#include "vfs_compat.h"
#include "../../util/string.h"
#include "../../util/printf.h"
#include "../../memory/heap.h"

struct file_descriptor_entry open_file_table[VFS_COMPAT_MAX_OPEN_FILES] = {0};
dir_t open_directory_table[VFS_COMPAT_MAX_OPEN_DIRECTORIES] = {0};
struct device_descriptor_entry open_device_table[VFS_COMPAT_MAX_OPEN_DEVICES] = {0};

dir_t * vfs_compat_get_dir(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return 0;
    dir_t * entry = &open_directory_table[fd];
    return entry;
}

struct device_descriptor_entry * vfs_compat_get_device(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DEVICES) return 0;
    struct device_descriptor_entry * entry = &open_device_table[fd];
    return entry;
}

struct file_descriptor_entry * vfs_compat_get_file_descriptor(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return 0;
    struct file_descriptor_entry * entry = &open_file_table[fd];
    return entry;
}

int get_devfd(const char* path, const char* mount, int flags, int mode) {
    if (strlen(path) >  VFS_FDE_NAME_MAX_LEN) return -1;
    if (strlen(mount) > VFS_FDE_NAME_MAX_LEN) return -1;
    static int fd = 0;
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DEVICES; i++) {
        if (open_device_table[fd].loaded == 0) {
            open_device_table[fd].loaded = 1;
            open_device_table[fd].flags = flags;
            open_device_table[fd].mode = mode;
            open_device_table[fd].offset = 0;
            strncpy(open_device_table[fd].name, path, strlen(path));
            strncpy(open_device_table[fd].mount, mount, strlen(mount));

            return fd++;
        } else {
            fd++;
            if (fd >= VFS_COMPAT_MAX_OPEN_DEVICES) fd = 0;
        }
    }
    return -1;
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
            strncpy(open_file_table[fd].mount, mount, strlen(mount));

            return fd++;
        } else {
            fd++;
            if (fd >= VFS_COMPAT_MAX_OPEN_FILES) fd = 0;
        }
    }
    return -1;
}

int get_dirfd(const char* path, const char* mount, int flags, int mode) {
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
            strncpy(open_directory_table[fd].fd.mount, mount, strlen(mount));
            
            open_directory_table[fd].index = 0;
            open_directory_table[fd].number = 0;
            open_directory_table[fd].dentries = (struct dentry*) malloc(sizeof(struct dentry));
            memset(open_directory_table[fd].dentries, 0, sizeof(struct dentry));

            return fd++;
        } else {
            fd++;
            if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) fd = 0;
        }
    }
    return -1;
}

int is_open(const char* path) {
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_FILES; i++) {
        if (open_file_table[i].loaded == 1) {
            if (strcmp(open_file_table[i].name, path) == 0) {
                return i;
            }
        }
    }
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DEVICES; i++) {
        if (open_device_table[i].loaded == 1) {
            if (strcmp(open_device_table[i].name, path) == 0) {
                return i;
            }
        }
    }
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DIRECTORIES; i++) {
        if (open_directory_table[i].fd.loaded == 1) {
            if (strcmp(open_directory_table[i].fd.name, path) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int force_release(const char * path) {
    int changes = 0;
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_FILES; i++) {
        if (open_file_table[i].loaded == 1) {
            if (strcmp(open_file_table[i].name, path) == 0) {
                open_file_table[i].loaded = 0;
                changes++;
            }
        }
    }
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DEVICES; i++) {
        if (open_device_table[i].loaded == 1) {
            if (strcmp(open_device_table[i].name, path) == 0) {
                open_device_table[i].loaded = 0;
                changes++;
            }
        }
    }
    for (int i = 0; i < VFS_COMPAT_MAX_OPEN_DIRECTORIES; i++) {
        if (open_directory_table[i].fd.loaded == 1) {
            if (strcmp(open_directory_table[i].fd.name, path) == 0) {
                open_directory_table[i].fd.loaded = 0;
                changes++;
            }
        }
    }
    return changes;
}

int release_fd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_FILES) return -1;
    open_file_table[fd].loaded = 0;
    return 0;
}

int release_devfd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DEVICES) return -1;
    open_device_table[fd].loaded = 0;
    return 0;
}

int read_dirfd(int fd, char * name, uint32_t * name_len, uint32_t * type) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return -1;
    if (open_directory_table[fd].fd.loaded == 0) return -1;
    if (open_directory_table[fd].index >= open_directory_table[fd].number) return 0;

    uint32_t index = open_directory_table[fd].index;
    struct dentry * dentry = open_directory_table[fd].dentries;
    if (dentry == 0 || dentry->next == 0) return 0;
    dentry = dentry->next; // Skip the first entry as it is empty
    for (uint32_t i = 0; i < index; i++) {
        //printf("dentry: %s, nl: %d, t: %d\n", dentry->name, dentry->name_len, dentry->type);
        dentry = dentry->next;
    }
    strncpy(name, dentry->name, dentry->name_len);
    *name_len = dentry->name_len;
    *type = dentry->type;
    open_directory_table[fd].index++;
    return 1;
}

int release_dirfd(int fd) {
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return -1;
    open_directory_table[fd].fd.loaded = 0;
    struct dentry * dentry_head = open_directory_table[fd].dentries;
    struct dentry * dentry = dentry_head;
    while (dentry->next != 0) {
        dentry = dentry->next;
        free(dentry);
    }
    return 0;
}

uint8_t add_file_to_dirfd(int fd, const char* name, uint32_t inode, uint32_t type, uint32_t name_len) {
    printf("add_file_to_dirfd: %d, %s, %d, %d, %d\n", fd, name, inode, type, name_len);
    if (fd >= VFS_COMPAT_MAX_OPEN_DIRECTORIES) return 0;
    if (strlen(name) > VFS_FDE_NAME_MAX_LEN) return 0;
    if (open_directory_table[fd].fd.loaded == 0) return 0;
    struct dentry * dentry_head = open_directory_table[fd].dentries;
    struct dentry * dentry = dentry_head;
    while (dentry->next != 0) {
        dentry = dentry->next;
    }
    dentry->next = (struct dentry*) malloc(sizeof(struct dentry));
    dentry = dentry->next;
    dentry->next = 0;
    dentry->inode = inode;
    dentry->type = type;
    dentry->name_len = name_len;
    strncpy(dentry->name, name, strlen(name));
    open_directory_table[fd].number++;
    
    return 1;
}