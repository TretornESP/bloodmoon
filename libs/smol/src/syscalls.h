#ifndef _SMOL_SYSCALLS_H
#define _SMOL_SYSCALLS_H

#include <stdint.h>

extern uint64_t _syscall_read(uint64_t fd, void* buf, uint64_t count);
extern uint64_t _syscall_write(uint64_t fd, const void* buf, uint64_t count);
extern uint64_t _syscall_open(const char* path, uint64_t flags);
extern uint64_t _syscall_close(uint64_t fd);

#endif