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

#include "util/time.h"

#include "test/tests.h"

void _start(void) {
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

    time_t t = time(0);
    struct tm * tm = localtime(&t);
    char date[32];
    strftime(date, 32, "%Y-%m-%d %H:%M:%S", tm);
    printf("EPOCH: %ld TIME: %s\n", time(0), date);
    vfs_file_open("/dev/hdap2/cuak.txt", 0x69, 0x13);
    vfs_file_open("/dev/hdap1/cuak.txt", 0x69, 0x13);

    printf("KERNEL LOOPING\n");
    while(1);

}