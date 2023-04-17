#ifndef _DEVICES_H
#define _DEVICES_H
#include <stdint.h>
#include "pci/pci.h"
#include "acpi/acpi.h"

#define DEVICE_NAME_SIZE 32

#define BLOCK_SCSI 0x8
#define CHAR_SCSI 0x88
#define ABS_TO_PAGE(addr, page_size) (addr / page_size)

struct file_operations {
    //struct module *owner;
    //loff_t (*llseek) (struct file *, loff_t, int);
    uint64_t (*read) (uint64_t, uint64_t, uint64_t, uint8_t*);
    uint64_t (*write) (uint64_t, uint64_t, uint64_t, uint8_t*);
    //ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
    //ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
    //int (*iopoll)(struct kiocb *kiocb, bool spin);
    //int (*iterate) (struct file *, struct dir_context *);
    //int (*iterate_shared) (struct file *, struct dir_context *);
    //__poll_t (*poll) (struct file *, struct poll_table_struct *);
    uint64_t (*ioctl) (uint64_t, uint32_t, void*);
    //long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
    //int (*mmap) (struct file *, struct vm_area_struct *);
    //unsigned long mmap_supported_flags;
    //int (*open) (struct inode *, struct file *);
    //int (*flush) (struct file *, fl_owner_t id);
    //int (*release) (struct inode *, struct file *);
    //int (*fsync) (struct file *, loff_t, loff_t, int datasync);
    //int (*fasync) (int, struct file *, int);
    //int (*lock) (struct file *, int, struct file_lock *);
    //ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
    //unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
    //int (*check_flags)(int);
    //int (*flock) (struct file *, int, struct file_lock *);
    //ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
    //ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
    //int (*setlease)(struct file *, long, struct file_lock **, void **);
    //long (*fallocate)(struct file *file, int mode, loff_t offset, loff_t len);
    //void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
    //unsigned (*mmap_capabilities)(struct file *);
#endif
    //ssize_t (*copy_file_range)(struct file *, loff_t, struct file *, loff_t, size_t, unsigned int);
    //loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in, struct file *file_out, loff_t pos_out, loff_t len, unsigned int remap_flags);
    //int (*fadvise)(struct file *, loff_t, loff_t, int);
};

struct device_driver {
    struct file_operations *fops;
    uint8_t registered;
    char name[DEVICE_NAME_SIZE];
};

struct device {
    uint8_t bc: 1; // 0 = block, 1 = char
    uint8_t valid: 1;
    uint8_t major;
    uint8_t minor;
    char name[DEVICE_NAME_SIZE];
    uint64_t internal_id;
    void * device_control_structure;
    struct device *next;
};

void device_list();
void register_char(uint8_t, const char*, struct file_operations*);
void register_block(uint8_t, const char*, struct file_operations*);
void unregister_char(uint8_t);
void unregister_block(uint8_t);

struct device* get_device_head();
struct device* get_next_device(struct device*);
uint32_t get_device_count();
uint32_t get_device_count_by_major(uint8_t);
void init_devices();

struct device* device_search(const char*);
uint8_t device_identify(const char*, char*);
uint64_t device_ioctl (const char *, uint32_t, void*);
uint64_t device_read(const char *, uint64_t, uint64_t, uint8_t*);
uint64_t device_write(const char *, uint64_t, uint64_t, uint8_t*);

#endif