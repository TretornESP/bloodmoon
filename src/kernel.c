#include "test/tests.h"
#include "util/kernel_boot.h"

#include "dev/devices.h"
#include "scheduling/scheduler.h"
#include "drivers/tty/tty_interface.h"
#include "vfs/vfs_interface.h"
#include "util/printf.h"

void intro_handler(void* ttyb, uint8_t event) {
    (void)(ttyb);
    switch (event) {
        case TTY_SIGNAL_OUTB:
            printf("TTY Outb\n");
            break;
        case TTY_SIGNAL_INB: {
            char text[32];
            int fd = vfs_file_open("ttyap0/", 0, 0);
            if (fd < 0) {
                printf("Failed to open file\n");
            } else {
                vfs_file_read(fd, text, 32);
                vfs_file_close(fd);
            }
            printf("TTY Inb: %s\n", text);
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

    device_list();
    pseudo_ps();

    tty_add_subscriber("ttya", intro_handler);
    
    halt();

}