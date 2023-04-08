#include "bootservices/bootservices.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/heap.h"

#include "scheduling/pit.h"
#include "scheduling/scheduler.h"

#include "vfs/vfs.h"
#include "vfs/vfs_adapters.h"

#include "io/interrupts.h"

#include "dev/smbios/smbios_interface.h"
#include "dev/devices.h"

#include "drivers/disk/disk.h"
#include "drivers/ahci/ahci.h"
#include "drivers/serial/serial.h"

#include "util/string.h"
#include "util/printf.h"

#include "vfs/generic/fat32/generic_f32.h"
#include "vfs/generic/ext2/generic_ext2.h"
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
    init_serial(4096, 4096);
    pseudo_ps();

    printf("KERNEL LOOPING\n");

    while(1) {
    }

}