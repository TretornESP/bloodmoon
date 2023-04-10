#ifndef _COMM_H
#define _COMM_H
#define SERIAL_RX_BUFFER_SIZE 4096
#define SERIAL_TX_BUFFER_SIZE 4096
#define SERIAL_MAJOR 0x8d
#include <stdint.h>

void register_comm(void (*cb)(uint8_t, uint64_t));

#endif