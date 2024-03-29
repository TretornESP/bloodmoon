#include "test/tests.h"
#include "util/kernel_boot.h"
#include "util/string.h"

#include "dev/devices.h"
#include "scheduling/scheduler.h"
#include "drivers/tty/tty_interface.h"
#include "vfs/vfs_interface.h"
#include "util/printf.h"
#include "debugger/debug.h"
#include "debugger/dbgshell.h"
#include "util/dbgprinter.h"
#include "memory/paging.h"
#include "memory/memory.h"
#include "util/timeout.h"

void _start(void) {
    boot();

    panic("Kernel panic: _start() reached");
}