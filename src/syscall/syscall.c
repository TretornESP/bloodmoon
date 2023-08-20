#include "syscall.h"
#include "../util/printf.h"
#include "../scheduling/scheduler.h"
#include "../vfs/vfs_interface.h"

uint64_t syscall(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    //printf("Syscall %d called with args: %d %d %d %d %d %d\n", syscall_id, arg1, arg2, arg3, arg4, arg5, arg6);
    (void)arg4;
    (void)arg5;
    (void)arg6;
    
    struct task * current_task = get_current_task();

    switch(syscall_id) {
        case 0: {
            if (current_task->open_files[arg1] > -1)
                vfs_file_read(current_task->open_files[arg1], (void*)arg2, arg3);
            break;
        }
        case 1: {
            if (current_task->open_files[arg1] > -1)
                //vfs_file_write(current_task->open_files[arg1], (void*)arg2, arg3);
                vfs_file_write(current_task->open_files[arg1], (void*)arg2, arg3);
            break;
        }
    }
    return 0;
}