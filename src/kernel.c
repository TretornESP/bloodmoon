#include "bootservices/bootservices.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/heap.h"

#include "scheduling/pit.h"
#include "scheduling/scheduler.h"

#include "fs/vfs.h"
#include "fs/vfs_adapters.h"

#include "io/interrupts.h"

#include "dev/smbios/smbios_interface.h"
#include "dev/devices.h"

#include "drivers/disk/disk.h"
#include "drivers/ahci/ahci.h"

#include "util/string.h"
#include "util/printf.h"

#include "fs/generic/fat32/generic_f32.h"
#include "fs/generic/ext2/generic_ext2.h"
#include "drivers/disk/disk_interface.h"

#include "arch/simd.h"

#include "util/time.h"
#include "util/rand.h"

#include "test/tests.h"

void _start(void) {
    init_simd();
    init_memory();
    init_paging();
    init_heap();
    init_pit(1678779503);
    init_interrupts(1);
    init_scheduler();
    init_smbios_interface();
    init_devices();
    init_drive();
    register_filesystem(fat32_registrar);
    register_filesystem(ext2_registrar);
    init_vfs();
    pseudo_ps();

    int fd = vfs_dir_open("/dev/hdap2/data/");
    vfs_dir_load(fd);
    if (fd == -1)
        panic("ERROR OPENING DIR\n");

    char name[256];
    uint32_t type;
    uint32_t name_len;

    while (vfs_dir_read(fd, name, &type, &name_len) > 0) {
        printf("DIR ENTRY: %s, %d, %d\n", name, type, name_len);
    }
    //int fd = vfs_file_open("/dev/hdap2/data/lorem-ipsum.txt", 0x69, 0x13);
    //char buf[1024];
    //vfs_file_read(fd, buf, 1024);
    //vfs_file_close(fd);

    //fd = vfs_file_creat("/dev/hdap2/data/ipsum-lorem.txt", O_RDWR);
    //if (fd == -1)
    //    panic("ERROR CREATING FILE\n");
    //vfs_file_write(fd, buf, 1024);
    //memset(buf, 0, 1024);
    //vfs_file_read(fd, buf, 1024);
    //vfs_file_close(fd);
    //vfs_dir_open("/dev/hdap2/data/");

    //printf("FILE CONTENTS: %s\n", buf);

    printf("KERNEL LOOPING\n");
    while(1);

}