#include "bootservices/bootservices.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/heap.h"
#include "scheduling/pit.h"
#include "io/interrupts.h"
#include "dev/smbios_interface.h"
#include "dev/devices.h"
#include "drivers/disk.h"
#include "util/printf.h"
#include "util/string.h"
#include "fs/fat32.h"

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts(1);
    init_smbios_interface();
    init_devices();
    init_drive();

    fat32_debug();
    printf("VIVO\n");
    while(1);

}