#include "test/tests.h"
#include "util/kernel_boot.h"
#include "util/string.h"

#include "dev/devices.h"
#include "scheduling/scheduler.h"
#include "drivers/tty/tty_interface.h"
#include "vfs/vfs_interface.h"
#include "util/printf.h"
#include "debugger/debug.h"
#include "util/dbgprinter.h"

void intro_handler(void* ttyb, uint8_t event) {
    (void)ttyb;
    switch (event) {
        case TTY_SIGNAL_OUTB:
            printf("TTY Outb\n");
            break;
        case TTY_SIGNAL_INB: {
            printf("TTY Inb\n");
            dbg("This is a debug message\n"); 
            dbg_flush();    
            break;
        }
        case TTY_SIGNAL_FLUSH_INB:
            printf("TTY Flush Inb\n");
            break;
        case TTY_SIGNAL_FLUSH_OUTB:
            printf("TTY Flush Outb\n");
            break;
        default:
            printf("TTY Unknown Event\n");
            break;
    }
}

void _start(void) {
    boot();

    pseudo_ps();
    panic("Dummy");

    while(1);
    tty_add_subscriber("ttya", intro_handler);


    while(1);
}