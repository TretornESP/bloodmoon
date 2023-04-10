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
#include "drivers/serial/serial_dd.h"
#include "drivers/serial/serial_interface.h"

#include "util/string.h"
#include "util/printf.h"

#include "vfs/generic/fat32/generic_f32.h"
#include "vfs/generic/ext2/generic_ext2.h"
#include "drivers/disk/disk_interface.h"

#include "drivers/keyboard/keyboard.h"

#include "arch/simd.h"

#include "util/time.h"
#include "util/rand.h"

#include "test/tests.h"

void read_serial_cb1(char c, int port) {
    printf("rcb1[%d] %c\n", port, c);
}

void write_serial_cb1(char c, int port) {
    printf("wcb1[%d] %c\n", port, c);
}

void read_serial_cb2(char c, int port) {
    printf("rcb2[%d] %c\n", port, c);
}

void write_serial_cb2(char c, int port) {
    printf("wcb2[%d] %c\n", port, c);
}

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
    init_serial_dd();
    register_filesystem(fat32_registrar);
    register_filesystem(ext2_registrar);
    init_vfs();
    pseudo_ps();  
    device_list();

    printf("KERNEL LOOPING\n");

    serial_write_now("seriala", "Hello from COM1\n", 17);
    halt_until_enter();
    char buf[256];
    
    int res = serial_read("seriala", (uint8_t*)buf, 16);
    for (int i = 0; i < res; i++) {
        printf("%c", buf[i]);
    }
    while(1) {
    }

}