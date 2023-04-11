#ifndef _SERIAL_DD_H
#define _SERIAL_DD_H
#include <stdint.h>
#define DEVICE_SERIAL               0xd
#define SERIAL_FLUSH_TX             0x1
#define SERIAL_SUBSCRIBE_READ       0x2
#define SERIAL_SUBSCRIBE_WRITE      0x3
#define SERIAL_UNSUBSCRIBE_READ     0x4
#define SERIAL_UNSUBSCRIBE_WRITE    0x5
#define SERIAL_ENABLE_ECHO          0x6
#define SERIAL_DISABLE_ECHO         0x7

void init_serial_dd();

#endif