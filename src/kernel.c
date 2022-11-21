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

#include "fs/generic/fat32exp/generic_f32_exp.h"
#include "fs/generic/ext2/generic_ext2.h"

#include "test/tests.h"

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts(1);
    init_smbios_interface();
    init_devices();
    init_drive();
    register_filesystem(get_f32_exp_driver());
    register_filesystem(ext2_registrar);
    init_vfs();
    uint8_t enabled_devices = get_enabled_f32_devices();
    printf("There are %d enabled devices\n", enabled_devices);

    for (uint8_t i = 0; i < 1; i++) {
        struct fat32_compat_device *device = get_device_at_index(i);
        printf("Device %s, partition %c at LBA %d, index: %d\n", device->name, device->partition + 'C', device->lba, i);
    }

    //fat32_debug("/dev/hdc");

    FRESULT fr;
    FATFS fs;
    FIL fil;

    f_mount(&fs, "2:", 0);
    fr = f_open(&fil, "2:/test/test.fake", FA_READ);
    if (fr != FR_OK) {
        printf("Error opening file\n");
    } else {
        printf("File opened\n");
    }

    char buf[512];
    UINT br;
    f_read(&fil, buf, 512, &br);
    printf("Read %d bytes: %s\n", br, buf);

    f_close(&fil);

    printf("KERNEL LOOPING\n");
    while(1);

}