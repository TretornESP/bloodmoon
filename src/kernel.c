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

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

void read_serial_cb1(char c, int port) {
    printf("rcb1[%d] %c is %x\n", port, c, c);
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

void print_prompt() {
    serial_write_now("seriala", "    ____  __    ____  ____  ____  __  _______  ____  _   __\n", 60);
    serial_write_now("seriala", "   / __ )/ /   / __ \\/ __ \\/ __ \\/  |/  / __ \\/ __ \\/ | / /\n", 60);
    serial_write_now("seriala", "  / __  / /   / / / / / / / / / / /|_/ / / / / / / /  |/ / \n", 60);
    serial_write_now("seriala", " / /_/ / /___/ /_/ / /_/ / /_/ / /  / / /_/ / /_/ / /|  /  \n", 60);
    serial_write_now("seriala", "/_____/_____/\\____/\\____/_____/_/  /_/\\____/\\____/_/ |_/   \n", 60);
    serial_write_now("seriala", "                                                           \n", 60);
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
    device_list();

    print_prompt();
    pseudo_ps();  
    serial_write_now("seriala", "rotero@bloodmon:/$ ", 21);
    serial_ioctl("seriala", SERIAL_SUBSCRIBE_READ, read_serial_cb1);
    serial_ioctl("seriala", SERIAL_ENABLE_ECHO, 0);

    while(1) {
    }

}