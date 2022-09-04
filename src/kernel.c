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


void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts(1);
    init_smbios_interface();
    init_devices();
    init_drive();

    uint8_t * buffer = (uint8_t*)malloc(0x1000);
    device_read("/dev/sda", 1, 0, buffer);

    for (int i = 0; i < 512; i++) {
        printf("%c", buffer[i]);
    }
    
    while(1);
}