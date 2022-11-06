#include "bootservices/bootservices.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/heap.h"

#include "scheduling/pit.h"

#include "fs/vfs.h"

#include "io/interrupts.h"

#include "dev/smbios/smbios_interface.h"
#include "dev/devices.h"

#include "drivers/disk/disk.h"
#include "drivers/ahci/ahci.h"

#include "util/string.h"
#include "util/printf.h"

#include "fs/generic/fat32/generic_f32.h"
#include "fs/generic/ext2/generic_ext2.h"

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts(1);
    init_smbios_interface();
    init_devices();
    init_drive();

    device_list();
    register_filesystem(fat32_registrar);
    register_filesystem(ext2_registrar);
    init_vfs();
    fat32_debug("/dev/hda");
    printf("VIVO\n");
    while(1);

}