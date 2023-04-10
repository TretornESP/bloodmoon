#include "comm.h"
#include "../../drivers/serial/serial.h"
#include "../../memory/heap.h"
void register_comm(void (*cb)(uint8_t, uint64_t)) {
    init_serial(SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE);
    int port_count = serial_count_ports();
    int *port_buffer = malloc(sizeof(int) * port_count);
    serial_get_ports(port_buffer);

    for (int i = 0; i < port_count; i++) {
        cb(SERIAL_MAJOR, port_buffer[i]);
    }

    free(port_buffer);
}