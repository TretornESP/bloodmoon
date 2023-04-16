#include "test/tests.h"
#include "util/kernel_boot.h"

#include "dev/devices.h"
#include "scheduling/scheduler.h"

#include "drivers/tty/tty_interface.h"

void _start(void) {
    boot();

    device_list();
    pseudo_ps();

    tty_write_now("ttya", "Hello, world!", 14);

    while(1) {
        
    }

}