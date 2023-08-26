#ifndef _E1000_DRIVER_H
#define _E1000_DRIVER_H
#include <stdint.h>

#define IOCTL_E1000_TEST        0x1000
#define IOCTL_E1000_GET_MAC     0x1001
#define IOCTL_E1000_INJECT_CB   0x1002
#define IOCTL_E1000_INJECT_SC   0x1003
#define IOCTL_E1000_GET_STATUS  0x1004

#define E1000_DD_NAME    "INTEL E1000 DRIVER\0"

void init_e1000_dd(void);
void exit_d1000_dd(void);
#endif