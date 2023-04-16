#ifndef _COMM_H
#define _COMM_H
#define SERIAL_RX_BUFFER_SIZE 4096
#define SERIAL_TX_BUFFER_SIZE 4096
#define SERIAL_MAJOR 0x8d
#define TTY_MAJOR 0xe
#include <stdint.h>

void register_comm(char* (*cb)(void*, uint8_t, uint64_t));

#endif