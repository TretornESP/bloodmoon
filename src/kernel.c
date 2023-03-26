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

    //Create folder erasable
    vfs_mkdir("/dev/hdap2/data/erasable", 0);
    //Create folder noterasable
    vfs_mkdir("/dev/hdap2/data/noterasable", 0);
    //Create file lorem-ipsum.txt in erasable
    vfs_file_creat("/dev/hdap2/data/erasable/lorem-ipsum.txt", 0);
    //Create file ipsum-lorem.txt in noterasable
    vfs_file_creat("/dev/hdap2/data/noterasable/ipsum-lorem.txt", 0);
    //Open file erasable/lorem-ipsum.txt
    int fd = vfs_file_open("/dev/hdap2/data/erasable/lorem-ipsum.txt", 0, 0);
    //Try to delete erasable/lorem-ipsum.txt
    vfs_remove("/dev/hdap2/data/erasable/lorem-ipsum.txt", 0);
    //Close file erasable/lorem-ipsum.txt
    vfs_file_close(fd);
    //Try to delete erasable/lorem-ipsum.txt
    vfs_remove("/dev/hdap2/data/erasable/lorem-ipsum.txt", 0);
    //Try to delete erasable
    vfs_remove("/dev/hdap2/data/erasable", 0);
    //Try to delete noterasable
    vfs_remove("/dev/hdap2/data/noterasable", 0);
    //Print directory contents of /data

    vfs_dir_list("/dev/hdap2/data");
    vfs_dir_list("/dev/hdap2/data/erasable");
    vfs_dir_list("/dev/hdap2/data/noterasable");

    printf("KERNEL LOOPING\n");
    while(1);

}