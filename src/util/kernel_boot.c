#include "kernel_boot.h"

#include "../arch/simd.h"

#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../memory/heap.h"

#include "../scheduling/pit.h"
#include "../scheduling/scheduler.h"

#include "../io/interrupts.h"

#include "../dev/smbios/smbios_interface.h"
#include "../dev/devices.h"

#include "../drivers/disk/disk.h"
#include "../drivers/serial/serial_dd.h"
#include "../drivers/serial/serial_interface.h"
#include "../drivers/tty/tty_dd.h"
#include "../drivers/tty/tty_interface.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/disk/disk_interface.h"


#include "../vfs/vfs.h"
#include "../vfs/vfs_interface.h"
#include "../vfs/generic/fat32/generic_f32.h"
#include "../vfs/generic/ext2/generic_ext2.h"
#include "../vfs/generic/tty/generic_tty.h"

#include "string.h"
#include "printf.h"
#include "time.h"
#include "rand.h"

void print_prompt() {
    int fd = vfs_file_open("ttyap0/", 0, 0);
    if (fd < 0) return;

    vfs_file_write(fd, "    ____  __    ____  ____  ____  __  _______  ____  _   __\n", 60);
    vfs_file_write(fd, "   / __ )/ /   / __ \\/ __ \\/ __ \\/  |/  / __ \\/ __ \\/ | / /\n", 60);
    vfs_file_write(fd, "  / __  / /   / / / / / / / / / / /|_/ / / / / / / /  |/ / \n", 60);
    vfs_file_write(fd, " / /_/ / /___/ /_/ / /_/ / /_/ / /  / / /_/ / /_/ / /|  /  \n", 60);
    vfs_file_write(fd, "/_____/_____/\\____/\\____/_____/_/  /_/\\____/\\____/_/ |_/   \n", 60);
    vfs_file_write(fd, "                                                           \n", 60);
    vfs_file_write(fd, "rotero@bloodmon:/$ ", 19);
    vfs_file_flush(fd);

    vfs_file_close(fd);
}

void boot() {
    init_simd();
    init_memory();
    init_paging();
    init_heap();
    init_pit(1678779503);
    init_interrupts(1);
    init_scheduler();
    init_drive();
    init_serial_dd();
    init_tty_dd();
    init_devices();
    init_smbios_interface();
    register_filesystem(fat32_registrar);
    register_filesystem(ext2_registrar);
    register_filesystem(tty_registrar);
    init_vfs();

    print_prompt();
}

void __attribute__((noreturn)) halt() {
    while(1) {
        __asm__ volatile("hlt");
    }
}	