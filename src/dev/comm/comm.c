#include "comm.h"
#include "../../drivers/serial/serial.h"
#include "../../drivers/tty/tty.h"
#include "../../util/string.h"
#include "../../memory/heap.h"

void register_comm(char* (*cb)(void*, uint8_t, uint64_t)) {
    init_serial(SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE);
    int port_count = serial_count_ports();
    int *port_buffer = malloc(sizeof(int) * port_count);
    serial_get_ports(port_buffer);

    for (int i = 0; i < port_count; i++) {
        char* name = cb((void*)0x0, SERIAL_MAJOR, port_buffer[i]);
        int index = tty_init(name, TTY_MODE_SERIAL, 1024, 1024);
        struct tty * tty = get_tty(index);
        if (is_valid_tty(tty)) {
            cb((void*)tty, TTY_MAJOR, index);
        }
    }

    free(port_buffer);
}