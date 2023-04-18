#include "test/tests.h"
#include "util/kernel_boot.h"
#include "util/string.h"

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
            
            int fd2 = vfs_file_open("ttybp0/", 0, 0);
            if (fd2 < 0) {
                printf("Failed to open out file\n");
            } else {
                if (!strcmp(text, "say")) {
                    vfs_file_write(fd2, "echo hello from bloodmoon\n", strlen("echo hello from bloodmoon\n"));
                } else {
                    vfs_file_write(fd2, "unknown command\n", strlen("unknown command\n"));
                }
                vfs_file_flush(fd2);
                vfs_file_close(fd2);
            }
            
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

//This example program creates a telnet server on the port 1235
//You may connect to it using telnet localhost 1235
//You will be able to type in commands on STDIN and see the output through the telnet connection

//Actual commands: 
//say: will echo hello from bloodmoon

void _start(void) {
    boot();

    device_list();
    pseudo_ps();
    tty_add_subscriber("ttya", intro_handler);
    
    halt();

}