#include "comm.h"
#include "../fb/framebuffer.h"
#include "../../drivers/serial/serial.h"
#include "../../drivers/ps2/ps2.h"
#include "../../drivers/tty/tty.h"
#include "../../util/string.h"
#include "../../memory/heap.h"

void register_comm(char* (*cb)(void*, uint8_t, uint64_t)) {
    init_serial(SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE);
    int port_count = serial_count_ports();
    int *port_buffer = kmalloc(sizeof(int) * port_count);
    serial_get_ports(port_buffer);

    for (int i = 0; i < port_count; i++) {
        char* name = cb((void*)0x0, SERIAL_MAJOR, port_buffer[i]);
        int index = tty_init(name, name, TTY_MODE_SERIAL, 1024, 1024);
        struct tty * tty = get_tty(index);
        if (is_valid_tty(tty)) {
            cb((void*)tty, TTY_MAJOR, index);
        }
    }

    kfree(port_buffer);

    init_ps2();
    init_framebuffer();
    
    for (int i = 0; i < MAX_FRAMEBUFFER_COUNT; i++) {
        if (get_framebuffer(i) != 0) {
            cb((void*)0, FRAMEBUFFER_MAJOR, i);
        }
    }

    cb((void*)0, PS2_MOUSE_MAJOR, 0x0);
    cb((void*)0, PS2_KEYBOARD_MAJOR, 0x0);
}