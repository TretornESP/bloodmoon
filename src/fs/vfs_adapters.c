#include "vfs_adapters.h"

struct dentry *vfs_f32_mount(struct file_system_type * fs_type, int a, const char * b, void * c) {
    (void)fs_type;(void)a;(void)b;(void)c;
    return (struct dentry*)0;
}
void vfs_f32_kill_sb(struct super_block * sb) {(void)sb;}