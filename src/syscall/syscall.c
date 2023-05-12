#include "syscall.h"
#include "../util/printf.h"

uint64_t syscall(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    printf("Syscall %d called with args: %d %d %d %d %d %d\n", syscall_id, arg1, arg2, arg3, arg4, arg5, arg6);
    return 0;
}