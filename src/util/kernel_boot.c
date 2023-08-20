#include "kernel_boot.h"

#include "../arch/simd.h"
#include "../arch/gdt.h"

#include "../debugger/debug.h"
#include "../debugger/dbgshell.h"

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

//This searches for the first terminal and initializes the debugger on it
void enable_debug(uint8_t reserved) {
    uint32_t terminals = get_device_count_by_major(DEVICE_TTY);
    if (terminals > reserved) {
        uint8_t index = 0;
        struct device* current = get_device_head();

        while (current != 0) {
            if (current->major == DEVICE_TTY) {
                if (index++ >= reserved) {
                    printf("Enabling debugger on %s\n", current->name);
                    dbg_init(current->name);
                    set_current_tty(current->name);
                    break;
                }
            }
            current = get_next_device(current);
        }
    }
}

void boot() {
    init_simd();
    init_memory();
    init_paging();
    init_heap();
    init_gdt();
    init_pit(100);
    init_interrupts(0); //One disables pit
    init_drive();
    init_serial_dd();
    init_tty_dd();
    init_smbios_interface();
    init_devices();
    enable_debug(0);
    register_filesystem(fat32_registrar);
    register_filesystem(ext2_registrar);
    register_filesystem(tty_registrar);
    init_vfs();
    init_scheduler();
    add_task(create_task((void*)init_dbgshell, "ttya"));
    go(3); //The number is the number of ticks for preemption, zero for cooperative scheduling

    panic("Kernel returned to boot() (this should never happen!)\n");
}

void __attribute__((noreturn)) halt() {
    while(1) {
        __asm__ volatile("hlt");
    }
}	