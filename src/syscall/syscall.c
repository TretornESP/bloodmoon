#include "syscall.h"
#include "../util/printf.h"
#include "../scheduling/scheduler.h"
#include "../vfs/vfs_interface.h"

uint64_t syscall(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    //printf("Syscall %d called with args: %d %d %d %d %d %d\n", syscall_id, arg1, arg2, arg3, arg4, arg5, arg6);
    
    switch(syscall_id) {
        case 0: {
            int fd;
            printf("Syscall 0 called\n");
            if (arg1 == 0 || arg1 == 1 || arg1 == 2) {
                fd = vfs_file_open(get_current_tty(), 0, 0);
            } else {
                printf("Invalid file descriptor\n");
                return 0;
            }

            if (fd < 0) {
                printf("Could not open file\n");
                return 0;
            }
            vfs_file_read(fd, (void*)arg2, arg3);
            vfs_file_close(fd);
            break;
        }
        case 1: {
            printf("Syscall 0 called\n");
            int fd;
            if (arg1 == 0 || arg1 == 1 || arg1 == 2) {
                fd = vfs_file_open(get_current_tty(), 0, 0);
            } else {
                printf("Invalid file descriptor\n");
                return 0;
            }

            if (fd < 0) {
                printf("Could not open file\n");
                return 0;
            }
            vfs_file_write(fd, (void*)arg2, arg3);
            vfs_file_close(fd);
            break;
        }
    }
    return 0;
}