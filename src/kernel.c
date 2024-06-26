#include "test/tests.h"
#include "util/kernel_boot.h"
#include "util/string.h"
#include "devices/devices.h"
#include "sched/scheduler.h"
#include "drivers/tty/tty_interface.h"
#include "vfs/vfs_interface.h"
#include "util/printf.h"
#include "debugger/debug.h"
#include "debugger/dbgshell.h"
#include "util/dbgprinter.h"
#include "memory/paging.h"
#include "memory/memory.h"
#include "util/timeout.h"
#include "arch/ssp.h"

void _start(void) {
    __asm__ volatile("cli");
    boot();

    panic("Kernel panic: _start() reached");
}